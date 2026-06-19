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
        int PreConditionDepth = 0
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
        int FallthroughPcValue = -1); // -1 = fall-through path not applicable (for ret/throw/br/exit)

    /// <summary>
    /// pc-dispatch state machine node. Generated for irreducible CFGs that
    /// cannot be made reducible by interval analysis + node splitting.
    /// Emitted as: int32_t chaos_pc = N; while (chaos_pc >= 0) { switch(chaos_pc) { case ... } }
    /// </summary>
    internal sealed record IRPcDispatch(
        IReadOnlyList<PcDispatchCase> Cases,
        int PcVariableInit
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
        IReadOnlyList<AotCoreIrInstructionArtifact>? FilterInstructions = null
    ) : StructuredIRNode;
    // Async IR nodes (F6)
    internal enum AsyncAwaiterKind { TaskAwaiter, TaskAwaiterOfT, ValueTaskAwaiter, YieldAwaitable, ConfiguredTaskAwaiter, CustomAwaiter }
    internal sealed record IRAwait(string Expr, AsyncAwaiterKind Kind, StructuredIRNode Cont, bool Sync = false) : StructuredIRNode;
}
