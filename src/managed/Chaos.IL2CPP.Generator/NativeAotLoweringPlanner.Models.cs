using Chaos.IL2CPP.Contracts;

namespace Chaos.IL2CPP.Generator;

public sealed partial class NativeAotLoweringPlanner
{
    private readonly record struct InvocationTarget(
        string TargetSymbol,
        IReadOnlyList<AotCoreIrAbiSlotArtifact> ParameterAbis,
        AotCoreIrAbiSlotArtifact ReturnAbi,
        IReadOnlySet<int> RawArgumentIndices,
        string? OpenDefinitionSubjectId = null,
        SharedGenericBodyId? SharedGenericBodyId = null,
        InstantiationStubId? InstantiationStubId = null,
        RuntimeGenericContextArtifact? RuntimeGenericContext = null,
        string? InlineCppExpression = null,
        int ExternalRuntimeTableIndex = -1,
        string? DirectNativeSymbol = null);

    private sealed record ExternalRuntimeHelperDefinition(
        string SubjectId,
        string TargetSymbol,
        string Source,
        IReadOnlyList<AotCoreIrAbiSlotArtifact> ParameterAbis,
        AotCoreIrAbiSlotArtifact ReturnAbi,
        IReadOnlySet<int> RawArgumentIndices,
        IReadOnlySet<string>? ReferencedStaticFieldSubjectIds = null,
        string? DirectNativeSymbol = null,
        string? DirectNativeHeader = null);

    private sealed record ExternalRuntimeDispatchEntry(
        string SubjectId,
        int Index);

    // ── Bridge/Import thunk definition ────────────────────────────────────────
    // A bridge thunk is a C++ wrapper function that adapts a managed calling
    // convention to a native target (InternalCall, P/Invoke, interpreter bridge).
    // It handles GC transition, argument marshalling, and exception boundaries
    // for calls crossing the managed/native boundary.
    private sealed record BridgeImportThunkDefinition(
        string SubjectId,
        string ThunkSymbol,
        string TargetSymbol,
        IReadOnlyList<AotCoreIrAbiSlotArtifact> ParameterAbis,
        AotCoreIrAbiSlotArtifact ReturnAbi,
        bool RequiresGcTransition,
        bool HasMarshalling,
        bool IsInternalCall,
        bool IsPInvokeImport,
        string? ModuleName = null,
        string? EntryPointName = null,
        int ImportCallingConvention = 0);

    private sealed record EnumerableJoinSupportVariant(
        string EnumerableTypeSubjectId,
        AotCoreIrMethodArtifact GetEnumeratorMethod,
        AotCoreIrMethodArtifact MoveNextMethod,
        AotCoreIrMethodArtifact GetCurrentMethod);

    private sealed record AssemblyReflectionTypeEntry(
        string AssemblyName,
        string TypeSubjectId,
        string TypeDisplayName);

    private sealed record AssemblyReflectionSupportModel(
        IReadOnlyList<AssemblyReflectionTypeEntry> TypeEntries)
    {
        public static readonly AssemblyReflectionSupportModel Empty = new([]);
    }

    private sealed record ReflectionMemberTypeEntry(
        string TypeSubjectId,
        string TypeName,
        string? GenericDefinitionTypeSubjectId,
        IReadOnlyList<string> GenericArgumentTypeSubjectIds,
        int GenericParameterCount,
        int MetadataToken);

    internal sealed record ReflectionMemberFieldEntry(
        string DeclaringTypeSubjectId,
        string FieldName,
        int MetadataToken,
        long? ConstantValue = null);

    private sealed record ReflectionMemberMethodEntry(
        string MethodSubjectId,
        string DeclaringTypeSubjectId,
        string MethodName,
        IReadOnlyList<string> ParameterNames,
        bool IsConstructor,
        int MetadataToken);

    private sealed record ReflectionMemberSupportModel(
        IReadOnlyList<ReflectionMemberTypeEntry> TypeEntries,
        IReadOnlyList<ReflectionMemberFieldEntry> FieldEntries,
        IReadOnlyList<ReflectionMemberMethodEntry> MethodEntries)
    {
        public static readonly ReflectionMemberSupportModel Empty = new([], [], []);
    }

    private sealed record StaticFieldDataEntry(
        string FieldSubjectId,
        string MemberType,
        IReadOnlyList<byte> Bytes);

    private sealed record StaticFieldDataSupportModel(
        IReadOnlyDictionary<string, StaticFieldDataEntry> EntriesBySubjectId)
    {
        public static readonly StaticFieldDataSupportModel Empty =
            new(new Dictionary<string, StaticFieldDataEntry>(StringComparer.Ordinal));
    }

    private sealed record CatchOnlyExceptionMethodShape(
        AotCoreIrExceptionRegionArtifact ExceptionRegion,
        IReadOnlyList<AotCoreIrInstructionArtifact> PrefixInstructions,
        IReadOnlyList<AotCoreIrInstructionArtifact> TryInstructions,
        IReadOnlyList<AotCoreIrInstructionArtifact> HandlerInstructions,
        IReadOnlyList<AotCoreIrInstructionArtifact> TailInstructions);

    private sealed record FilterOnlyExceptionMethodShape(
        AotCoreIrExceptionRegionArtifact FilterRegion,
        IReadOnlyList<AotCoreIrInstructionArtifact> PrefixInstructions,
        IReadOnlyList<AotCoreIrInstructionArtifact> TryInstructions,
        IReadOnlyList<AotCoreIrInstructionArtifact> FilterInstructions,
        IReadOnlyList<AotCoreIrInstructionArtifact> HandlerInstructions,
        IReadOnlyList<AotCoreIrInstructionArtifact> TailInstructions);

    private sealed record CatchAndFinallyExceptionMethodShape(
        AotCoreIrExceptionRegionArtifact CatchRegion,
        IReadOnlyList<AotCoreIrInstructionArtifact> PrefixInstructions,
        IReadOnlyList<AotCoreIrInstructionArtifact> PreInnerFinallyInstructions,
        IReadOnlyList<AotCoreIrInstructionArtifact> InnerTryInstructions,
        FinallyHandlerShape? InnerFinallyHandler,
        IReadOnlyList<AotCoreIrInstructionArtifact> PostInnerTryInstructions,
        IReadOnlyList<AotCoreIrInstructionArtifact> HandlerInstructions,
        IReadOnlyList<FinallyHandlerShape> OuterFinallyHandlers,
        IReadOnlyList<AotCoreIrInstructionArtifact> TailInstructions);

    private sealed record FinallyOnlyExceptionMethodShape(
        IReadOnlyList<AotCoreIrInstructionArtifact> PrefixInstructions,
        IReadOnlyList<AotCoreIrInstructionArtifact> TryInstructions,
        IReadOnlyList<FinallyHandlerShape> FinallyHandlers,
        IReadOnlyList<AotCoreIrInstructionArtifact> TailInstructions);

    private sealed record FinallyHandlerShape(
        AotCoreIrExceptionRegionArtifact ExceptionRegion,
        IReadOnlyList<AotCoreIrInstructionArtifact> Instructions);

    private sealed record FinallyHandlerGuardShape(
        IReadOnlyList<AotCoreIrInstructionArtifact> ConditionInstructions,
        bool BranchWhenNonZeroToEnd);

    private sealed record FinallyHandlerEmissionPlan(
        FinallyHandlerGuardShape? Guard,
        IReadOnlyList<AotCoreIrInstructionArtifact> BodyInstructions,
        bool GuardTargetsEndFinally = true);

    private sealed record FilterAndFinallyExceptionMethodShape(
        AotCoreIrExceptionRegionArtifact FilterRegion,
        IReadOnlyList<AotCoreIrInstructionArtifact> PrefixInstructions,
        IReadOnlyList<AotCoreIrInstructionArtifact> TryInstructions,
        IReadOnlyList<AotCoreIrInstructionArtifact> FilterInstructions,
        IReadOnlyList<AotCoreIrInstructionArtifact> HandlerInstructions,
        IReadOnlyList<FinallyHandlerShape> FinallyHandlers,
        IReadOnlyList<AotCoreIrInstructionArtifact> TailInstructions);

    private sealed record MultipleCatchExceptionMethodShape(
        IReadOnlyList<AotCoreIrExceptionRegionArtifact> CatchRegions,
        IReadOnlyList<AotCoreIrInstructionArtifact> PrefixInstructions,
        IReadOnlyList<AotCoreIrInstructionArtifact> TryInstructions,
        IReadOnlyList<IReadOnlyList<AotCoreIrInstructionArtifact>> HandlerInstructionsList,
        IReadOnlyList<AotCoreIrInstructionArtifact> TailInstructions);

    // ── Generic exception shape (fallback for non-standard EH patterns) ──

    private sealed record GenericExceptionRegionEntry(
        AotCoreIrExceptionRegionArtifact Region,
        IReadOnlyList<AotCoreIrInstructionArtifact> TryInstructions,
        IReadOnlyList<AotCoreIrInstructionArtifact> HandlerInstructions,
        IReadOnlyList<AotCoreIrInstructionArtifact>? FilterInstructions = null);

    private sealed record GenericExceptionMethodShape(
        IReadOnlyList<AotCoreIrInstructionArtifact> PrefixInstructions,
        IReadOnlyList<AotCoreIrInstructionArtifact> TailInstructions,
        IReadOnlyList<GenericExceptionRegionEntry> Regions,
        int HandlerPushes);

    private sealed record CustomAttributeSupportModel(
        IReadOnlyDictionary<string, string> QueryAttributeTypeByCallee,
        IReadOnlyDictionary<string, string> SyntheticGetterFieldByMethodSubjectId,
        IReadOnlyList<CustomAttributeMaterializationPlan> Materializations,
        IReadOnlySet<string> AdditionalReferenceTypeSubjectIds,
        IReadOnlySet<string> AdditionalInstanceFieldSubjectIds,
        bool RequiresStringSupport,
        bool UsesMemberInfoIsDefined)
    {
        public static readonly CustomAttributeSupportModel Empty = new(
            new Dictionary<string, string>(StringComparer.Ordinal),
            new Dictionary<string, string>(StringComparer.Ordinal),
            [],
            new HashSet<string>(StringComparer.Ordinal),
            new HashSet<string>(StringComparer.Ordinal),
            false,
            false);
    }

    private enum CustomAttributeTargetKind : byte
    {
        Type = 1,
        Method = 2,
        Field = 3,
        Property = 4,
        Param = 5,
    }

    private enum CustomAttributeLiteralKind : byte
    {
        Null = 0,
        Boolean = 1,
        Byte = 2,
        SByte = 3,
        Int16 = 4,
        Int32 = 5,
        Int64 = 6,
        UInt16 = 7,
        UInt32 = 8,
        UInt64 = 9,
        Single = 10,
        Double = 11,
        Char = 12,
        String = 13,
        Type = 14,
        Enum = 15,
    }

    private sealed record CustomAttributeLiteralValue(
        CustomAttributeLiteralKind Kind,
        object? Value);

    private sealed record CustomAttributeFieldAssignment(
        string FieldSubjectId,
        CustomAttributeLiteralValue Value);

    private sealed record CustomAttributeMaterializationPlan(
        CustomAttributeTargetKind TargetKind,
        uint TargetMetadataToken,
        string TargetSubjectId,
        string AttributeTypeSubjectId,
        IReadOnlyList<CustomAttributeFieldAssignment> Assignments);

    private readonly record struct MetadataTypeIdentity(
        string AssemblyName,
        string NamespaceName,
        string TypeName)
    {
        public string SubjectId => ManagedNaming.CreateTypeSubjectId(AssemblyName, NamespaceName, TypeName);

        public string DisplayName => ManagedNaming.CreateTypeDisplayName(AssemblyName, NamespaceName, TypeName);
    }
}
