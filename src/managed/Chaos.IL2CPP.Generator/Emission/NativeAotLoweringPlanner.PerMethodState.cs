// Per-method state container for NativeAotLoweringPlanner.
// All instance fields that are reset per-method go here.
// Accessed via ThreadLocal<PerMethodState> to enable parallel method emission.
using System.Collections.Generic;

namespace Chaos.IL2CPP.Generator;

public sealed partial class NativeAotLoweringPlanner
{
    private sealed class PerMethodState
    {
        public int LinearScratchCounter;
        public int NextInlineId;
        public int DispatchLabelSeq;
        public string? PendingEnumBoxSubjectId;
        public string? PendingBoxSubjectId;
        public bool PendingBoxHasProvider;
        public IReadOnlyList<Contracts.AotCoreIrInstructionArtifact>? LookaheadInstructionList;
        public int LookaheadInstructionIndex;
        public IReadOnlyList<Contracts.AotCoreIrInstructionArtifact>? LinearInstructionList;
        public int LinearInstructionIndex;
        public string? LastCheckedArrayExpr;
        public string? LastCheckedIndexExpr;
        public HashSet<int>? LoopArrayAccessSkipOffsets;
        public Dictionary<int, string>? HoistedIVs;
        public Dictionary<int, (string VarName, SlotType SlotType)>? HoistedInvariantLocals;
        public Dictionary<int, string>? HoistedArrayBaseSlots;
        public Dictionary<string, int>? SlotVarToLocalSlot;
        public Dictionary<int, string>? AccumulatorSlots;
        public HashSet<int>? StructLocalSlots;
        /// <summary>Local slots that hold a GC-heap pointer to a boxed async state machine
        /// (the <c>&gt;d__</c> instance).  <c>ldloca</c> on these slots must emit the slot's
        /// VALUE (the box pointer, which is already the durable address of the struct),
        /// not <c>&amp;chaos_locals[N]</c> (the address of the stack slot).  Passing the stack
        /// slot address across a thread resumption is invalid (the slot dies when the calling
        /// frame returns).  Scoped to async MoveNext + its async entry, empty otherwise.</summary>
        public HashSet<int>? AsyncBoxPointerLocalSlots;
        public Dictionary<int, SlotType>? FloatLocalSlots;
        public HashSet<int>? Int64LocalSlots;
        public StructuredSlotEmissionContext? ActiveStructuredSlotContext;
        public List<string>? PendingBranchSkipLabels;
        public Stack<SlotType>? StructuredSlotTypes;
        public int StructuredIrDepth;
        public List<(string VarName, string Expression)>? PreTryFoldInitializers;
        public string? CurrentMethodNativeSymbol;
        public Contracts.AotCoreIrMethodArtifact? CurrentMethodArtifact;
    }
}
