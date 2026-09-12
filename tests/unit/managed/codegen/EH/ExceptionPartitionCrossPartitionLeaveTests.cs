using Chaos.IL2CPP.Generator.Tests.Infra;
using Xunit;

namespace Chaos.IL2CPP.Generator.Tests.Emission;

/// <summary>
/// A try/catch whose TRY-side <c>leave</c> targets a block INSIDE the tail, not the tail's
/// first block, must still resume at that block.
///
/// WHY THIS TEST EXISTS
/// --------------------
/// <c>BuildExceptionPartitionTree</c> (<c>StructuredIR.Emit.cs</c>) emits the try/handler as an
/// <c>IRExceptionRegion</c> and then the tail as a PLAIN SEQUENTIAL SIBLING node. There is no
/// channel by which a <c>leave</c> inside the try can specify WHERE in the tail to resume:
/// control leaving the region (or falling out of the handler) always resumes at the tail's
/// FIRST block.
///
/// The sibling test <c>ExceptionPartitionBranchLossTests</c> does not catch this, because its
/// fixture's <c>leave</c> operands (0x0E and 0x0E) both target the tail's ENTRY — precisely
/// the case where "resume at the tail's first block" happens to be right.
///
/// This defect is what makes <c>&lt;YieldOne&gt;d__0::MoveNext</c> (async iterator, A4) return
/// "no more elements": its try ends with <c>leave 169</c> targeting the "yielded" arm at
/// IL_00A9, while the tail's FIRST block is the mutually-exclusive "exhausted" arm at IL_0080
/// (which calls <c>SetResult(false)</c> and returns). Both arms end in <c>ret</c>, so falling
/// into the wrong one silently produces a different result — not a crash.
///
/// THE ORACLE
/// ----------
/// Both arms carry a DISTINCT payload literal. The assertion is that the arm the try's
/// <c>leave</c> selects (payload 2) is reachable — i.e. the emitted C++ must contain real
/// control flow that can reach payload 2 AFTER the region. Under the current defect the
/// region's exit falls straight into the tail's first block (payload 1), and payload 2 is
/// emitted after an unconditional <c>return</c> — dead code.
///
/// CIL byte layout:
/// <code>
///   0x00 ldarg    0     1     arg 0
///   0x01 brfalse  0x08  2     -> tail entry (payload 1 arm)
///   0x03 ldc.i4   2     1     try's chosen payload
///   0x04 stloc    0     1
///   0x05 leave    0x0C  2     -> tail's SECOND block (payload 2 arm); NOT the tail entry
///   0x07 (try ends at 0x07)
///   0x07 stloc    1     1     handler: store exception
///   0x08 leave    0x0A  2     -> tail entry
///   0x0A (handler ends at 0x0A)
///   0x0A ldc.i4   1     1     tail block A (FIRST) — exhausted arm, payload 1
///   0x0B stloc    0     1
///   0x0C (tail block B — the try's leave target, payload 2)
///   0x0C ldc.i4   3     1     distinct third payload so block B is identifiable
///   0x0D stloc    0     1
///   0x0E ldloc    0     1     tail exit
///   0x0F ret
/// </code>
/// (Handler is [0x07,0x0A); tail is >= 0x0A.)
/// </summary>
public sealed class ExceptionPartitionCrossPartitionLeaveTests
{
    private readonly PlannerFixture _fixture = new();

    [Fact]
    public void TryLeaveTargetingNonEntryTailBlock_ResumesAtThatBlock()
    {
        var method = ModelFactory.CreateMethod(
            "TestModule.TestClass::TryLeaveIntoMidTail",
            returnType: "System.Int32",
            returnAbi: ModelFactory.Int32Abi,
            instructions: new[]
            {
                ModelFactory.Instruction("ldarg", ilOffset: 0x00, intOperand: 0),
                ModelFactory.Instruction("brfalse", ilOffset: 0x01, intOperand: 0x0A),
                ModelFactory.Instruction("ldc.i4", ilOffset: 0x03, intOperand: 2),
                ModelFactory.Instruction("stloc", ilOffset: 0x04, intOperand: 0),
                ModelFactory.Instruction("leave", ilOffset: 0x05, intOperand: 0x0C),
                ModelFactory.Instruction("stloc", ilOffset: 0x07, intOperand: 1),
                ModelFactory.Instruction("leave", ilOffset: 0x08, intOperand: 0x0A),
                ModelFactory.Instruction("ldc.i4", ilOffset: 0x0A, intOperand: 1),
                ModelFactory.Instruction("stloc", ilOffset: 0x0B, intOperand: 0),
                ModelFactory.Instruction("ldc.i4", ilOffset: 0x0C, intOperand: 3),
                ModelFactory.Instruction("stloc", ilOffset: 0x0D, intOperand: 0),
                ModelFactory.Instruction("ldloc", ilOffset: 0x0E, intOperand: 0),
                ModelFactory.Instruction("ret", ilOffset: 0x0F),
            },
            exceptionRegions: new[]
            {
                ModelFactory.CatchRegion(
                    tryOffset: 0x00, tryLength: 0x07,
                    handlerOffset: 0x07, handlerLength: 0x03),
            });

        var source = _fixture.RunPlannerSingleMethod(method);

        // All three payload literals must be present (none silently dropped).
        Assert.Contains("static_cast<CHAOS_IL2CPP_INTPTR>(1)", source);
        Assert.Contains("static_cast<CHAOS_IL2CPP_INTPTR>(2)", source);

        // THE LOAD-BEARING ASSERTION: the tail block the try's `leave` selects (payload 3)
        // must be reachable. Under the fix the tail is emitted as a pc-dispatch state machine
        // that reads `chaos_continuation` to select its entry block, so payload 3 lives
        // inside a case of that dispatch and is reachable when chaos_continuation == 0x0C.
        //
        // Before the fix the tail was flat sequential: region exit fell into the tail's first
        // block (payload 1), which ended in `return;`, making everything after it dead code.
        // We assert the presence of the handoff mechanism rather than text order.
        Assert.Contains("static_cast<CHAOS_IL2CPP_INTPTR>(3)", source);
        Assert.Contains("static_cast<CHAOS_IL2CPP_INTPTR>(1)", source);
        Assert.Contains("static_cast<CHAOS_IL2CPP_INTPTR>(2)", source);

        // The fix must produce TWO pc-dispatch sections (try + tail).
        int d1 = source.IndexOf("// pc-dispatch state machine", StringComparison.Ordinal);
        int d2 = d1 < 0 ? -1 : source.IndexOf("// pc-dispatch state machine", d1 + 1, StringComparison.Ordinal);
        Assert.True(d2 > d1,
            "The tail must be emitted as a pc-dispatch (second state machine) so the "
            + "entry block can be selected at runtime from the handoff slot.\n" + source);

        // The tail dispatch must read chaos_continuation to choose its entry.
        Assert.Contains("chaos_continuation ==", source);

        // Payload 3 (the try's leave target at 0x0C) must appear inside the TAIL dispatch
        // (after the second pc-dispatch), not inside the try dispatch or the handler.
        int payload3Idx = source.IndexOf("static_cast<CHAOS_IL2CPP_INTPTR>(3)", d2, StringComparison.Ordinal);
        Assert.True(payload3Idx > d2,
            "Payload 3 must live in the TAIL dispatch, reachable from the handoff slot.\n"
            + source);
    }
}
