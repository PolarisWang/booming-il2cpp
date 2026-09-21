using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading;
using Chaos.IL2CPP.Contracts;

namespace Chaos.IL2CPP.Generator;

public sealed partial class NativeAotLoweringPlanner
{

    /// <summary>Linear sequence of instructions with optional terminator.</summary>
    internal sealed record IRBlock(
        IReadOnlyList<AotCoreIrInstructionArtifact> BodyInstructions,
        AotCoreIrInstructionArtifact? Terminator
    ) : StructuredIRNode;

    /// <summary>Sequential composition of IR nodes.</summary>
    internal sealed record IRSequence(IReadOnlyList<StructuredIRNode> Nodes) : StructuredIRNode;

    /// <summary>If-then-else 鈥?condition expressed via eval-stack + branch opcode.</summary>
    internal sealed record IRIfThenElse(
        IReadOnlyList<AotCoreIrInstructionArtifact> ConditionInstructions,
        AotCoreIrInstructionArtifact BranchTerminator,
        StructuredIRNode ThenBody,
        StructuredIRNode? ElseBody,
        StructuredIRNode? PostMergeBody = null,
        int PreConditionDepth = 0,
        // Whether the arms LEAVE A VALUE on the eval stack for the merge point to
        // consume (`b = c ? x : y;`), as opposed to a plain statement `if (c) {}`.
        //
        // This must not be inferred from PostMergeBody's shape: both forms produce an
        // ordinary instruction sequence there.  It is a property of the IL — the arms'
        // net stack effect — and is computed when the node is built.
        //
        // Without it the emitter restored the depth to PreConditionDepth at the merge,
        // discarding the value the arms had pushed; the following `stloc` then popped a
        // stale slot (observed as a managed handle being returned instead of the
        // computed result).
        bool MergeCarriesValue = false
    ) : StructuredIRNode;

    /// <summary>Header-controlled while loop.</summary>
    internal sealed record IRWhileLoop(
        IReadOnlyList<AotCoreIrInstructionArtifact> ConditionInstructions,
        AotCoreIrInstructionArtifact? ConditionTerminator,
        StructuredIRNode Body,
        int ExitOffset
    ) : StructuredIRNode;

    /// <summary>Latch-controlled do-while loop.</summary>
    internal sealed record IRDoWhileLoop(
        StructuredIRNode Body,
        IReadOnlyList<AotCoreIrInstructionArtifact> LatchInstructions,
        AotCoreIrInstructionArtifact? LatchTerminator,
        int HeaderOffset,
        int ExitOffset
    ) : StructuredIRNode;

    /// <summary>Switch dispatch.</summary>
    internal sealed record IRSwitch(
        IReadOnlyList<AotCoreIrInstructionArtifact> SwitchInstructions,
        IReadOnlyDictionary<int, StructuredIRNode> CaseBodies,
        StructuredIRNode? DefaultBody,
        int ExitOffset,
        IReadOnlySet<int> FallthroughCaseValues
    ) : StructuredIRNode;

    // ── pc-dispatch state machine for irreducible CFGs ─────────────

    /// <summary>
    /// Represents a single case in a pc-dispatch state machine. Each case
    /// corresponds to one basic block in the irreducible CFG, identified by
    /// its program counter value.
    /// </summary>
    internal sealed record PcDispatchCase(
        int PcValue,
        IReadOnlyList<AotCoreIrInstructionArtifact> Instructions,
        AotCoreIrInstructionArtifact? Terminator,
        int NextPcValue,
        int FallthroughPcValue = -1, // -1 = fall-through path not applicable (for ret/throw/br/exit)
        int ExitTargetOffset = -1);  // leave/br target outside this CFG; -1 = none

    /// <summary>
    /// pc-dispatch state machine node. Generated for irreducible CFGs that
    /// cannot be made reducible by interval analysis + node splitting.
    /// Emitted as: int32_t chaos_pc = N; while (chaos_pc >= 0) { switch(chaos_pc) { case ... } }
    /// </summary>
    /// <param name="ExitTargetOffsets">
    /// IL offsets this dispatch jumps to that are NOT blocks in its own CFG — i.e. real
    /// continuations in another partition (typically the EH tail). A <c>leave</c> out of an
    /// EH region lands here, and the target is NOT the tail's first block in general.
    /// The enclosing region must forward the taken target so the tail resumes at the
    /// right block; dropping it makes every post-region continuation fall into the tail's
    /// entry and silently execute the wrong arm.
    /// </param>
    /// <param name="OffsetToPc">
    /// IL offset → pc value for this dispatch's own blocks. The emitter uses it to turn
    /// an incoming <c>chaos_continuation</c> offset (set by an upstream region's exit) into
    /// the pc this dispatch must start at.
    /// </param>
    /// <param name="ResumeOffsets">
    /// IL offsets at which control from OUTSIDE this dispatch may enter it (a TAIL partition
    /// entered by a <c>leave</c> out of the try/handler). When non-empty the emitter must open
    /// with a runtime selection on the handoff slot rather than the constant
    /// <paramref name="PcVariableInit"/>, because the entry block is not known at emission time.
    /// Empty/absent means "always enter at <paramref name="PcVariableInit"/>" (the common case).
    /// </param>
    internal sealed record IRPcDispatch(
        IReadOnlyList<PcDispatchCase> Cases,
        int PcVariableInit,
        IReadOnlyList<int>? ExitTargetOffsets = null,
        IReadOnlyDictionary<int, int>? OffsetToPc = null,
        IReadOnlyList<int>? ResumeOffsets = null,
        int FallOutExitTargetOffset = -1
    ) : StructuredIRNode;

    // 鈹€鈹€ Leaf control-flow nodes 鈹€鈹€

    internal sealed record IRBreak : StructuredIRNode;
    internal sealed record IRContinue : StructuredIRNode;
    internal sealed record IRReturn : StructuredIRNode;
    internal sealed record IRThrow : StructuredIRNode;

    // 鈹€鈹€ Exception regions (first-class IR nodes) 鈹€鈹€

    internal enum IRExceptionKind { TryCatch, TryFinally, TryFilter }

    internal sealed record IRExceptionRegion(
        IRExceptionKind Kind,
        StructuredIRNode TryBody,
        StructuredIRNode HandlerBody,
        string? CatchTypeSubjectId = null,
        IReadOnlyList<AotCoreIrInstructionArtifact>? FilterInstructions = null,
        IReadOnlyList<int>? RegionExitTargetOffsets = null
    ) : StructuredIRNode;

    /// <summary>
    /// A single <c>try</c> with several <c>catch</c> clauses — the C# shape
    /// <c>try { … } catch (A) { … } catch (B) { … }</c>.
    ///
    /// This must NOT be modelled as N sequential <see cref="IRExceptionRegion"/>s
    /// sharing one try body: that emits N separate <c>CHAOS_EH_TRY</c> blocks and
    /// therefore runs the try body N times, with only the last catch reachable.
    /// C# semantics are one execution of the try body with the clauses tried in
    /// order, so the emission is one try block containing a match chain.
    ///
    /// <paramref name="Clauses"/> is ordered as written in source: the first entry
    /// is the first <c>catch</c>.  A null <c>CatchTypeSubjectId</c> on a clause
    /// means a bare <c>catch { }</c> (matches anything), which the C# compiler only
    /// ever places last.
    /// </summary>
    internal sealed record IRMultiCatchRegion(
        StructuredIRNode TryBody,
        IReadOnlyList<IRMultiCatchClause> Clauses,
        IReadOnlyList<int>? RegionExitTargetOffsets = null
    ) : StructuredIRNode;

    internal sealed record IRMultiCatchClause(
        string? CatchTypeSubjectId,
        StructuredIRNode HandlerBody);

    // Async IR nodes (F6)
    internal enum AsyncAwaiterKind { TaskAwaiter, TaskAwaiterOfT, ValueTaskAwaiter, YieldAwaitable, ConfiguredTaskAwaiter, CustomAwaiter }
    internal sealed record IRAwait(string Expr, AsyncAwaiterKind Kind, StructuredIRNode Cont, bool Sync = false) : StructuredIRNode;
}
