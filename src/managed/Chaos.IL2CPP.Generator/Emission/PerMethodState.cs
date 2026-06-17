using Chaos.IL2CPP.Contracts;
namespace Chaos.IL2CPP.Generator;

public sealed partial class NativeAotLoweringPlanner
{
    /// <summary>
    /// Per-method mutable state used during IL→C++ code emission.
    /// Each thread gets its own instance via ThreadLocal to enable
    /// parallel method body emission without shared state corruption.
    /// All fields are reset at the start of each EmitManagedMethod call.
    /// </summary>
    private sealed class PerMethodState
    {
        public int LinearScratchCounter;
        public int NextInlineId;
        public int DispatchLabelSeq;

        public string? PendingEnumBoxSubjectId;
        public string? PendingBoxSubjectId;
        public bool PendingBoxHasProvider;

        public IReadOnlyList<AotCoreIrInstructionArtifact>? LookaheadInstructionList;
        public int LookaheadInstructionIndex;
        public IReadOnlyList<AotCoreIrInstructionArtifact>? LinearInstructionList;
        public int LinearInstructionIndex;

        public string? LastCheckedArrayExpr;
        public string? LastCheckedIndexExpr;

        public HashSet<int>? LoopArrayAccessSkipOffsets;
        public Dictionary<int, string>? HoistedIVs;
        public Dictionary<int, (string VarName, SlotType SlotType)>? HoistedInvariantLocals;
        public Dictionary<int, string>? HoistedArrayBaseSlots;
        public Dictionary<string, int>? SlotVarToLocalSlot;
        public Dictionary<int, string>? AccumulatorSlots;

        public StructuredSlotEmissionContext? ActiveStructuredSlotContext;
        public readonly Stack<SlotType> StructuredSlotTypes = new();

        public HashSet<int>? StructLocalSlots;
        public Dictionary<int, SlotType>? FloatLocalSlots;
        public HashSet<int>? Int64LocalSlots;

        public List<(string VarName, string Expression)>? PreTryFoldInitializers;
        public string? CurrentMethodNativeSymbol;
        public AotCoreIrMethodArtifact? CurrentMethodArtifact;

        public void Reset()
        {
            LinearScratchCounter = 0;
            NextInlineId = 0;
            DispatchLabelSeq = 0;
            PendingEnumBoxSubjectId = null;
            PendingBoxSubjectId = null;
            PendingBoxHasProvider = false;
            LookaheadInstructionList = null;
            LookaheadInstructionIndex = 0;
            LinearInstructionList = null;
            LinearInstructionIndex = 0;
            LastCheckedArrayExpr = null;
            LastCheckedIndexExpr = null;
            LoopArrayAccessSkipOffsets = null;
            HoistedIVs = null;
            HoistedInvariantLocals = null;
            HoistedArrayBaseSlots = null;
            SlotVarToLocalSlot = null;
            AccumulatorSlots = null;
            ActiveStructuredSlotContext = null;
            StructuredSlotTypes.Clear();
            StructLocalSlots = null;
            FloatLocalSlots = null;
            Int64LocalSlots = null;
            PreTryFoldInitializers = null;
            CurrentMethodNativeSymbol = null;
            CurrentMethodArtifact = null;
        }
    }
}
