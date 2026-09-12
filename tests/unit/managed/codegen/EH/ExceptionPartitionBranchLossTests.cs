using Chaos.IL2CPP.Generator.Tests.Infra;
using Xunit;

namespace Chaos.IL2CPP.Generator.Tests.Emission;

/// <summary>
/// A try/catch partition whose CFG is irreducible must still execute its branches.
///
/// WHY THIS TEST EXISTS
/// --------------------
/// <c>BuildExceptionPartitionTree</c> (<c>StructuredIR.Emit.cs</c>) recovers structure for
/// each partition. When a partition's CFG is irreducible *even after interval analysis* it
/// calls <c>EmitExceptionPartitionFallback</c>, whose doc comment claims it preserves
/// correctness "at the cost of flat linear emission".
///
/// That claim is FALSE. The fallback returns ONE <c>IRBlock</c> with no terminator
/// structure, and the linear emitter renders every branch opcode as a comment
/// (<c>// beq (structured EH branch)</c>, <c>// br (handled via structured EH branches)</c>)
/// — see <c>ExceptionEmission.EmitInstruction.cs</c>. With the branch gone, the emitted C++
/// falls straight through EVERY arm of the conditional. That is not "flat but correct";
/// it is a different program.
///
/// This was found while measuring <c>YieldOne::MoveNext</c> for A4 (async iterators), whose
/// catch-only partition produced 10 such comments; its yield path's <c>SetResult(true)</c>
/// ended up after an unconditional <c>return</c>, so the iterator always reported "no more
/// items". The defect is NOT iterator-specific — any try/catch with a branch crossing a
/// partition boundary can hit it. This test pins the general case with no async involved.
///
/// THE ORACLE, AND WHY IT IS SHAPED THIS WAY
/// -----------------------------------------
/// Asserting merely that the source contains a branch construct would be weak — an <c>if</c>
/// inside a comment also contains one. So the assertions are (a) the decisive
/// branch-shaped C++ control flow actually appears (structured <c>if</c> or pc-dispatch),
/// and (b) both arms' distinct payloads survive, so flattening cannot silently drop one.
/// </summary>
public sealed class ExceptionPartitionBranchLossTests
{
    private readonly PlannerFixture _fixture = new();

    /// <summary>
    /// Counterexample (决策2=A). The try partition branches forward to an offset that
    /// leaves the try range and re-enters it, which is what makes the partition CFG
    /// irreducible and routes it into the partition fallback.
    ///
    /// OPCODE SPELLING IS LOAD-BEARING HERE. The planner and the EH linear emitter only
    /// recognize the CANONICAL LONG forms — <c>br</c>/<c>brfalse</c>/<c>leave</c> and
    /// <c>ldarg</c>/<c>ldc.i4</c>/<c>stloc</c>/<c>ldloc</c> with a separate operand.
    /// Compact CIL spellings (<c>br.s</c>, <c>leave.s</c>, <c>ldarg.0</c>, <c>ldc.i4.1</c>,
    /// <c>stloc.0</c>) never reach this layer: the Loader's instruction decoder
    /// (<c>LoaderStage.InstructionDecoding.cs</c>) canonicalizes them, and
    /// <c>AotCoreIrLowering.cs:160</c> strips the <c>.s</c> suffix. A fixture written in
    /// compact spellings goes red for a spelling production never emits, which is
    /// exactly the kind of non-load-bearing red this test must avoid.
    ///
    /// CIL byte layout (offsets must match the planner's partition math):
    /// <code>
    ///   0x00 ldarg    0     1     (arg index 0)
    ///   0x01 brfalse  0x0B  2     -> target outside [tryStart, tryEnd) = [0, 0x0B)
    ///   0x03 ldc.i4   1     1     then-arm payload
    ///   0x04 stloc    0     1
    ///   0x05 br       0x09  2     -> jumps BACK into the try range (crossing/retargeting)
    ///   0x07 ldc.i4   2     1     else-arm payload
    ///   0x08 stloc    0     1
    ///   0x09 leave    0x0E  2     exits the try region (target = tail)
    ///   0x0B (try ends)
    ///   0x0B stloc    1     1     handler: store exception
    ///   0x0C leave    0x0E  2
    ///   0x0E ldloc    0     1     tail
    ///   0x0F ret
    /// </code>
    /// </summary>
    [Fact]
    public void IrreducibleTryPartition_StillExecutesItsBranch()
    {
        var method = ModelFactory.CreateMethod(
            "TestModule.TestClass::TryWithBranchingBody",
            returnType: "System.Int32",
            returnAbi: ModelFactory.Int32Abi,
            instructions: new[]
            {
                ModelFactory.Instruction("ldarg", ilOffset: 0x00, intOperand: 0),
                ModelFactory.Instruction("brfalse", ilOffset: 0x01, intOperand: 0x0B),
                ModelFactory.Instruction("ldc.i4", ilOffset: 0x03, intOperand: 1),
                ModelFactory.Instruction("stloc", ilOffset: 0x04, intOperand: 0),
                ModelFactory.Instruction("br", ilOffset: 0x05, intOperand: 0x09),
                ModelFactory.Instruction("ldc.i4", ilOffset: 0x07, intOperand: 2),
                ModelFactory.Instruction("stloc", ilOffset: 0x08, intOperand: 0),
                ModelFactory.Instruction("leave", ilOffset: 0x09, intOperand: 0x0E),
                ModelFactory.Instruction("stloc", ilOffset: 0x0B, intOperand: 1),
                ModelFactory.Instruction("leave", ilOffset: 0x0C, intOperand: 0x0E),
                ModelFactory.Instruction("ldloc", ilOffset: 0x0E, intOperand: 0),
                ModelFactory.Instruction("ret", ilOffset: 0x0F),
            },
            exceptionRegions: new[]
            {
                ModelFactory.CatchRegion(
                    tryOffset: 0x00, tryLength: 0x0B,
                    handlerOffset: 0x0B, handlerLength: 0x03),
            });

        var source = _fixture.RunPlannerSingleMethod(method);

        // Everything before CHAOS_EH_CATCH_BEGIN is the TRY partition — the part under test.
        // Scoping matters: the catch boilerplate always contains an `if (!chaos_eh_match_type...)`
        // line, so an unscoped `source.Contains("if (")` would be satisfied by the handler
        // even when the try partition emitted no branch at all.
        int catchBegin = source.IndexOf("CHAOS_EH_CATCH_BEGIN", StringComparison.Ordinal);
        Assert.True(catchBegin > 0, "expected a catch handler in the emitted body:\n" + source);
        var tryPartition = source[..catchBegin];

        // (1) The fatal shape: a branch reduced to a comment. Both spellings are wrong
        //     program text — control falls straight through, so BOTH arms run and the
        //     else payload always wins.
        Assert.DoesNotContain("(structured EH branch)", tryPartition);

        // (2) The decisive assertion: the branch DECISION must survive, inside the try
        //     partition. A branch is a two-way choice, so the emitted C++ must contain a
        //     condition test selecting between two DIFFERENT successors.
        //
        //     This is the assertion that a "labeled blocks present" check would miss: a
        //     pc-dispatch whose entry case simply falls out (`chaos_pc = -1;`) still
        //     contains `chaos_pc` and both payload literals, yet reaches NEITHER arm.
        //     Requiring two distinct successor assignments inside one `if`/`else` is what
        //     makes this load-bearing.
        bool hasStructuredIf = tryPartition.Contains("if (") && !tryPartition.Contains("chaos_pc");
        bool hasPcBranch = tryPartition.Contains("if (") && tryPartition.Contains("else")
            && CountDistinctPcAssignmentsInConditional(tryPartition) >= 2;
        Assert.True(hasStructuredIf || hasPcBranch,
            "The try partition's branch must emit a real two-way DECISION (a condition test "
            + "selecting between two different successors), not just labeled blocks or "
            + "branch-comments. Emitted try partition was:\n" + tryPartition);

        // (3) Both arms must stay reachable, so both payloads must appear.
        Assert.Contains("static_cast<CHAOS_IL2CPP_INTPTR>(1)", tryPartition);
        Assert.Contains("static_cast<CHAOS_IL2CPP_INTPTR>(2)", tryPartition);
    }

    /// <summary>
    /// Scoping counterexample. A try/catch WITHOUT a branching partition must keep emitting
    /// normally — the fix must not route every EH method through a heavier path.
    /// </summary>
    [Fact]
    public void StraightLineTryCatch_KeepsEmittingNormally()
    {
        var method = ModelFactory.CreateMethod(
            "TestModule.TestClass::TryStraightLine",
            returnType: "System.Void",
            instructions: new[]
            {
                ModelFactory.Instruction("nop", ilOffset: 0x00),
                ModelFactory.Instruction("leave", ilOffset: 0x01, intOperand: 0x06),
                ModelFactory.Instruction("stloc", ilOffset: 0x04, intOperand: 0),
                ModelFactory.Instruction("leave", ilOffset: 0x05, intOperand: 0x06),
                ModelFactory.Instruction("ret", ilOffset: 0x06),
            },
            exceptionRegions: new[]
            {
                ModelFactory.CatchRegion(
                    tryOffset: 0x00, tryLength: 0x03,
                    handlerOffset: 0x04, handlerLength: 0x03),
            });

        var source = _fixture.RunPlannerSingleMethod(method);

        Assert.Contains("CHAOS_EH_TRY", source);
        Assert.Contains("CHAOS_EH_CATCH_BEGIN", source);
    }

    /// <summary>
    /// Counts how many DISTINCT pc values a conditional block assigns — i.e. how many
    /// different successors a single branch decision can select. Returns 0 when there is
    /// no conditional at all (the flattened / branch-comment shape).
    /// </summary>
    private static int CountDistinctPcAssignmentsInConditional(string tryPartition)
    {
        var values = new System.Collections.Generic.HashSet<string>(StringComparer.Ordinal);
        foreach (System.Text.RegularExpressions.Match m in
                 System.Text.RegularExpressions.Regex.Matches(
                     tryPartition, @"if\s*\([^)]*\)\s*\{(?<then>[^}]*)\}\s*else\s*\{(?<else>[^}]*)\}"))
        {
            foreach (var group in new[] { m.Groups["then"], m.Groups["else"] })
                foreach (System.Text.RegularExpressions.Match a in
                         System.Text.RegularExpressions.Regex.Matches(group.Value, @"chaos_pc\s*=\s*(-?\d+)"))
                    values.Add(a.Groups[1].Value);
        }
        return values.Count;
    }
}
