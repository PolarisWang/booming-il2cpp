namespace Chaos.IL2CPP.CodeGen;

internal enum RuntimeSkeletonExecutionBackendKind
{
    ManagedInvoke,
    MetadataHelper,
    ImportedBridge,
    Kernel,
    Intrinsic,
}

internal sealed record RuntimeSkeletonSemanticFamilyDescriptor(
    string FamilyId,
    string SubjectIdPrefix,
    RuntimeSkeletonExecutionBackendKind PreferredBackend,
    bool SupportsStaticForwarder = false,
    bool SupportsInstanceMethods = false,
    bool SupportsConstructors = false,
    bool SupportsInstanceFieldGetters = false,
    bool SupportsImportedBridge = false);

internal static class RuntimeSkeletonSemanticRegistry
{
    public const string FloatingScalarFamilyId = "floating-scalar";
    public const string MetadataBackedValueTypeFamilyId = "metadata-backed-value-type";
    public const string NumberFormattingFamilyId = "number-formatting";
    public const string DecimalClusterFamilyId = "decimal-cluster";
    public const string PrimitiveScalarFamilyId = "primitive-scalar";
    public const string WideNumericFamilyId = "wide-numeric";
    public const string CalendarStructFamilyId = "calendar-struct";
    public const string IdentityStructFamilyId = "identity-struct";
    public const string VectorManagedInvokeFamilyId = "vector-managed-invoke";
    public const string CompanionManagedInvokeFamilyId = "owner-attached-companion";
    public const string DateTimeSemanticEngineFamilyId = "datetime-semantic-engine";
    public const string GlobalizationDateTimeSupportFamilyId = "globalization-datetime-support";

    private static readonly RuntimeSkeletonSemanticFamilyDescriptor[] Descriptors =
    [
        new(
            FloatingScalarFamilyId,
            "/System.Half::",
            RuntimeSkeletonExecutionBackendKind.ManagedInvoke,
            SupportsStaticForwarder: true,
            SupportsInstanceMethods: true,
            SupportsConstructors: true),
        new(
            FloatingScalarFamilyId,
            "/System.Single::",
            RuntimeSkeletonExecutionBackendKind.ManagedInvoke,
            SupportsInstanceMethods: true),
        new(
            FloatingScalarFamilyId,
            "/System.Double::",
            RuntimeSkeletonExecutionBackendKind.ManagedInvoke,
            SupportsInstanceMethods: true),
        new(
            FloatingScalarFamilyId,
            "/System.Runtime.InteropServices.NFloat::",
            RuntimeSkeletonExecutionBackendKind.ManagedInvoke,
            SupportsStaticForwarder: true,
            SupportsInstanceMethods: true,
            SupportsConstructors: true,
            SupportsInstanceFieldGetters: true),
        new(
            NumberFormattingFamilyId,
            "/System.Number::",
            RuntimeSkeletonExecutionBackendKind.ManagedInvoke),
        new(
            DecimalClusterFamilyId,
            "/System.Decimal",
            RuntimeSkeletonExecutionBackendKind.ManagedInvoke,
            SupportsStaticForwarder: true,
            SupportsInstanceMethods: true,
            SupportsConstructors: true,
            SupportsInstanceFieldGetters: true),
        new(
            PrimitiveScalarFamilyId,
            "/System.Char::",
            RuntimeSkeletonExecutionBackendKind.ManagedInvoke,
            SupportsStaticForwarder: true,
            SupportsInstanceMethods: true,
            SupportsConstructors: true),
        new(
            PrimitiveScalarFamilyId,
            "/System.CharEnumerator::",
            RuntimeSkeletonExecutionBackendKind.ManagedInvoke,
            SupportsInstanceMethods: true,
            SupportsConstructors: true),
        new(
            PrimitiveScalarFamilyId,
            "/System.Boolean::",
            RuntimeSkeletonExecutionBackendKind.ManagedInvoke,
            SupportsInstanceMethods: true),
        new(
            PrimitiveScalarFamilyId,
            "/System.Byte::",
            RuntimeSkeletonExecutionBackendKind.ManagedInvoke,
            SupportsStaticForwarder: true,
            SupportsInstanceMethods: true),
        new(
            PrimitiveScalarFamilyId,
            "/System.SByte::",
            RuntimeSkeletonExecutionBackendKind.ManagedInvoke,
            SupportsInstanceMethods: true),
        new(
            PrimitiveScalarFamilyId,
            "/System.Int16::",
            RuntimeSkeletonExecutionBackendKind.ManagedInvoke,
            SupportsInstanceMethods: true),
        new(
            PrimitiveScalarFamilyId,
            "/System.UInt16::",
            RuntimeSkeletonExecutionBackendKind.ManagedInvoke,
            SupportsStaticForwarder: true,
            SupportsInstanceMethods: true),
        new(
            PrimitiveScalarFamilyId,
            "/System.Int32::",
            RuntimeSkeletonExecutionBackendKind.ManagedInvoke,
            SupportsStaticForwarder: true,
            SupportsInstanceMethods: true),
        new(
            PrimitiveScalarFamilyId,
            "/System.UInt32::",
            RuntimeSkeletonExecutionBackendKind.ManagedInvoke,
            SupportsStaticForwarder: true,
            SupportsInstanceMethods: true),
        new(
            PrimitiveScalarFamilyId,
            "/System.Int64::",
            RuntimeSkeletonExecutionBackendKind.ManagedInvoke,
            SupportsStaticForwarder: true,
            SupportsInstanceMethods: true),
        new(
            PrimitiveScalarFamilyId,
            "/System.UInt64::",
            RuntimeSkeletonExecutionBackendKind.ManagedInvoke,
            SupportsStaticForwarder: true,
            SupportsInstanceMethods: true),
        new(
            PrimitiveScalarFamilyId,
            "/System.IntPtr::",
            RuntimeSkeletonExecutionBackendKind.ManagedInvoke,
            SupportsStaticForwarder: true,
            SupportsInstanceMethods: true,
            SupportsConstructors: true),
        new(
            PrimitiveScalarFamilyId,
            "/System.UIntPtr::",
            RuntimeSkeletonExecutionBackendKind.ManagedInvoke,
            SupportsStaticForwarder: true,
            SupportsInstanceMethods: true,
            SupportsConstructors: true),
        new(
            WideNumericFamilyId,
            "/System.Int128::",
            RuntimeSkeletonExecutionBackendKind.ManagedInvoke,
            SupportsStaticForwarder: true,
            SupportsInstanceMethods: true,
            SupportsConstructors: true,
            SupportsInstanceFieldGetters: true),
        new(
            WideNumericFamilyId,
            "/System.UInt128::",
            RuntimeSkeletonExecutionBackendKind.ManagedInvoke,
            SupportsStaticForwarder: true,
            SupportsInstanceMethods: true,
            SupportsConstructors: true,
            SupportsInstanceFieldGetters: true),
        new(
            CalendarStructFamilyId,
            "/System.DateTime::",
            RuntimeSkeletonExecutionBackendKind.ManagedInvoke,
            SupportsInstanceMethods: true,
            SupportsConstructors: true,
            SupportsInstanceFieldGetters: true),
        new(
            CalendarStructFamilyId,
            "/System.DateTimeOffset::",
            RuntimeSkeletonExecutionBackendKind.ManagedInvoke,
            SupportsInstanceMethods: true,
            SupportsConstructors: true,
            SupportsInstanceFieldGetters: true),
        new(
            CalendarStructFamilyId,
            "/System.DateOnly::",
            RuntimeSkeletonExecutionBackendKind.ManagedInvoke,
            SupportsInstanceMethods: true,
            SupportsConstructors: true,
            SupportsInstanceFieldGetters: true),
        new(
            CalendarStructFamilyId,
            "/System.TimeOnly::",
            RuntimeSkeletonExecutionBackendKind.ManagedInvoke,
            SupportsInstanceMethods: true,
            SupportsConstructors: true,
            SupportsInstanceFieldGetters: true),
        new(
            CalendarStructFamilyId,
            "/System.TimeSpan::",
            RuntimeSkeletonExecutionBackendKind.ManagedInvoke,
            SupportsStaticForwarder: true,
            SupportsInstanceMethods: true,
            SupportsConstructors: true,
            SupportsInstanceFieldGetters: true),
        new(
            IdentityStructFamilyId,
            "/System.Guid::",
            RuntimeSkeletonExecutionBackendKind.ManagedInvoke,
            SupportsInstanceMethods: true,
            SupportsConstructors: true),
        new(
            IdentityStructFamilyId,
            "/System.Index::",
            RuntimeSkeletonExecutionBackendKind.ManagedInvoke,
            SupportsInstanceMethods: true,
            SupportsConstructors: true,
            SupportsInstanceFieldGetters: true),
        new(
            IdentityStructFamilyId,
            "/System.Range::",
            RuntimeSkeletonExecutionBackendKind.ManagedInvoke,
            SupportsInstanceMethods: true,
            SupportsConstructors: true,
            SupportsInstanceFieldGetters: true),
        new(
            IdentityStructFamilyId,
            "/System.Text.Rune::",
            RuntimeSkeletonExecutionBackendKind.ManagedInvoke,
            SupportsInstanceMethods: true,
            SupportsConstructors: true,
            SupportsInstanceFieldGetters: true),
        new(
            VectorManagedInvokeFamilyId,
            "/System.Numerics.Vector2::",
            RuntimeSkeletonExecutionBackendKind.ManagedInvoke,
            SupportsInstanceMethods: true),
        new(
            VectorManagedInvokeFamilyId,
            "/System.Numerics.Vector3::",
            RuntimeSkeletonExecutionBackendKind.ManagedInvoke,
            SupportsInstanceMethods: true),
        new(
            VectorManagedInvokeFamilyId,
            "/System.Numerics.Vector4::",
            RuntimeSkeletonExecutionBackendKind.ManagedInvoke,
            SupportsInstanceMethods: true),
        new(
            CompanionManagedInvokeFamilyId,
            "/System.DateTime+LeapSecondCache::",
            RuntimeSkeletonExecutionBackendKind.ManagedInvoke,
            SupportsConstructors: true),
        new(
            CompanionManagedInvokeFamilyId,
            "/System.DateOnly+<>c::",
            RuntimeSkeletonExecutionBackendKind.ManagedInvoke,
            SupportsInstanceMethods: true,
            SupportsConstructors: true),
        new(
            CompanionManagedInvokeFamilyId,
            "/System.TimeOnly+<>c::",
            RuntimeSkeletonExecutionBackendKind.ManagedInvoke,
            SupportsInstanceMethods: true,
            SupportsConstructors: true),
        new(
            CompanionManagedInvokeFamilyId,
            "/System.Guid+GuidResult::",
            RuntimeSkeletonExecutionBackendKind.ManagedInvoke,
            SupportsInstanceMethods: true,
            SupportsConstructors: true),
        new(
            DateTimeSemanticEngineFamilyId,
            "/System.DateTimeFormat::",
            RuntimeSkeletonExecutionBackendKind.ManagedInvoke),
        new(
            DateTimeSemanticEngineFamilyId,
            "/System.DateTimeParse::",
            RuntimeSkeletonExecutionBackendKind.ManagedInvoke),
        new(
            DateTimeSemanticEngineFamilyId,
            "/System.DateTimeRawInfo::",
            RuntimeSkeletonExecutionBackendKind.ManagedInvoke,
            SupportsInstanceMethods: true),
        new(
            DateTimeSemanticEngineFamilyId,
            "/System.DateTimeResult::",
            RuntimeSkeletonExecutionBackendKind.ManagedInvoke,
            SupportsInstanceMethods: true),
        new(
            GlobalizationDateTimeSupportFamilyId,
            "/System.Globalization.CultureData::",
            RuntimeSkeletonExecutionBackendKind.ManagedInvoke,
            SupportsInstanceMethods: true,
            SupportsConstructors: true,
            SupportsInstanceFieldGetters: true),
        new(
            GlobalizationDateTimeSupportFamilyId,
            "/System.Globalization.DateTimeFormatInfo::",
            RuntimeSkeletonExecutionBackendKind.ManagedInvoke,
            SupportsInstanceMethods: true,
            SupportsConstructors: true,
            SupportsInstanceFieldGetters: true),
        new(
            MetadataBackedValueTypeFamilyId,
            "/System.Enum::",
            RuntimeSkeletonExecutionBackendKind.ManagedInvoke,
            SupportsInstanceMethods: true,
            SupportsConstructors: true,
            SupportsImportedBridge: true),
    ];

    public static RuntimeSkeletonSemanticFamilyDescriptor? TryResolveBySubjectId(string subjectId)
    {
        return Descriptors.FirstOrDefault(
            descriptor => subjectId.Contains(descriptor.SubjectIdPrefix, StringComparison.Ordinal));
    }

    public static RuntimeSkeletonSemanticFamilyDescriptor GetRequiredBySubjectId(string subjectId)
    {
        return TryResolveBySubjectId(subjectId) ??
               throw new InvalidOperationException($"missing runtime skeleton semantic descriptor for '{subjectId}'");
    }
}
