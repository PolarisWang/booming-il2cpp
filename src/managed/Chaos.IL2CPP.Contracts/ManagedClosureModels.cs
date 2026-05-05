namespace Chaos.IL2CPP.Contracts;

public sealed record AssemblyIdentityModel
{
    public required string Name { get; init; }

    public required Guid ModuleVersionId { get; init; }
}

public sealed record LoadedAssemblyModel
{
    public required string InputAssemblyPath { get; init; }

    public required AssemblyIdentityModel Assembly { get; init; }

    public required string EntryPointSubjectId { get; init; }

    public GenericInstantiationDemandGraphModel? GenericInstantiationDemandGraph { get; init; }

    public required IReadOnlyList<ManagedTypeModel> Types { get; init; }

    public required IReadOnlyList<ManagedFieldModel> Fields { get; init; }

    public required IReadOnlyList<ManagedPropertyModel> Properties { get; init; }

    public required IReadOnlyList<ManagedMethodModel> Methods { get; init; }
}

public sealed record ManagedTypeModel
{
    public required string AssemblyName { get; init; }

    public required string? NamespaceName { get; init; }

    public required string Name { get; init; }

    public required string SubjectId { get; init; }

    public required string DefinitionSubjectId { get; init; }

    public required string DisplayName { get; init; }

    public bool IsInterface { get; init; }

    public bool IsValueType { get; init; }

    public bool IsSealed { get; init; }

    /// <summary>
    /// Whether the type is marked with <see cref="System.Runtime.InteropServices.ComImportAttribute"/>.
    /// Maps to <see cref="System.Reflection.TypeAttributes.Import"/> in the ECMA 335 metadata.
    /// </summary>
    public bool IsComImport { get; init; }

    public string? BaseTypeSubjectId { get; init; }

    public IReadOnlyList<string>? ImplementedInterfaceSubjectIds { get; init; }

    public bool IsPreserved { get; init; }

    public required int MetadataToken { get; init; }
}

public sealed record ManagedFieldModel
{
    public required string AssemblyName { get; init; }

    public required string DeclaringTypeSubjectId { get; init; }

    public required string Name { get; init; }

    public required string FieldType { get; init; }

    public required string SubjectId { get; init; }

    public required string DefinitionSubjectId { get; init; }

    public bool IsStatic { get; init; }

    public bool IsThreadStatic { get; init; }

    public bool IsPreserved { get; init; }

    public required int MetadataToken { get; init; }
}

public sealed record ManagedPropertyModel
{
    public required string AssemblyName { get; init; }

    public required string DeclaringTypeSubjectId { get; init; }

    public required string Name { get; init; }

    public required string PropertyType { get; init; }

    public IReadOnlyList<string>? IndexParameterTypes { get; init; }

    public required string SubjectId { get; init; }

    public required string DefinitionSubjectId { get; init; }

    public bool IsPreserved { get; init; }

    public required int MetadataToken { get; init; }
}

public sealed record ManagedMethodModel
{
    public required string AssemblyName { get; init; }

    public required string DeclaringTypeSubjectId { get; init; }

    public required string DeclaringTypeDisplayName { get; init; }

    public required string Name { get; init; }

    public int GenericParameterCount { get; init; }

    public required string ReturnType { get; init; }

    public required string SubjectId { get; init; }

    public required string DefinitionSubjectId { get; init; }

    public required string Signature { get; init; }

    public required bool IsStatic { get; init; }

    public required bool IsVirtual { get; init; }

    public bool IsFinal { get; init; }

    public bool IsPreserved { get; init; }

    public bool IsUnmanagedCallersOnly { get; init; }

    public required int MetadataToken { get; init; }

    public required IReadOnlyList<ManagedParameterModel> Parameters { get; init; }

    public ManagedImportModel? Import { get; init; }

    public RuntimeGenericContextArtifact? RuntimeGenericContext { get; init; }

    public required ManagedMethodBodyModel Body { get; init; }
}

public sealed record ManagedImportModel
{
    public required string ModuleName { get; init; }

    public required string EntryPointName { get; init; }
}

public sealed record ManagedParameterModel
{
    public required string Name { get; init; }

    public required string Type { get; init; }
}

public sealed record ManagedMethodBodyModel
{
    public required IReadOnlyList<ManagedBlockModel> Blocks { get; init; }

    public required IReadOnlyList<ManagedExceptionRegionModel> ExceptionRegions { get; init; }
}

public sealed record ManagedBlockModel
{
    public required string BlockId { get; init; }

    public required IReadOnlyList<ManagedInstructionModel> Instructions { get; init; }
}

public sealed record ManagedExceptionRegionModel
{
    public required string HandlingKind { get; init; }

    public required int TryOffset { get; init; }

    public required int TryLength { get; init; }

    public required int HandlerOffset { get; init; }

    public required int HandlerLength { get; init; }

    public int? FilterOffset { get; init; }

    public string? CatchTypeSubjectId { get; init; }
}

public sealed record ManagedInstructionModel
{
    public required string Op { get; init; }

    public object? Operand { get; init; }

    public int? IlOffset { get; init; }

    public string? ResultType { get; init; }

    public string? Callee { get; init; }

    public ManagedCallSiteSignature? CallSiteSignature { get; init; }

    public ManagedInstructionReference? Reference { get; init; }
}

public sealed record ManagedInstructionReference
{
    public required string AssemblyName { get; init; }

    public required string SubjectKind { get; init; }

    public required string SubjectId { get; init; }
}

/// <summary>
/// Canonical call-site kinds carried by managed / typed-il / AOT core instruction artifacts.
/// </summary>
public enum ManagedCallSiteKind : byte
{
    /// <summary>
    /// Call site describes an indirect call through a function pointer.
    /// </summary>
    FunctionPointer = 1,
}

/// <summary>
/// Stable call-site signature metadata needed by indirect-call instructions such as <c>calli</c>.
/// </summary>
public sealed record ManagedCallSiteSignature
{
    public required ManagedCallSiteKind KindCode { get; init; }

    public required string ReturnType { get; init; }

    public required IReadOnlyList<string> ParameterTypes { get; init; }
}

public sealed record CanonicalSubjectsModel
{
    public required IReadOnlyList<CanonicalSubjectModel> Subjects { get; init; }
}

public sealed record CanonicalSubjectModel
{
    public required string SubjectKind { get; init; }

    public required string SubjectId { get; init; }

    public required string CanonicalSubjectId { get; init; }
}

public sealed record SemanticShapesModel
{
    public required IReadOnlyList<TypeShapeModel> Types { get; init; }

    public required IReadOnlyList<FieldShapeModel> Fields { get; init; }

    public required IReadOnlyList<PropertyShapeModel> Properties { get; init; }

    public required IReadOnlyList<MethodShapeModel> Methods { get; init; }
}

public sealed record TypeShapeModel
{
    public required string SubjectId { get; init; }

    public required string Kind { get; init; }
}

public sealed record FieldShapeModel
{
    public required string SubjectId { get; init; }

    public required string Kind { get; init; }
}

public sealed record PropertyShapeModel
{
    public required string SubjectId { get; init; }

    public required string Kind { get; init; }
}

public sealed record MethodShapeModel
{
    public required string SubjectId { get; init; }

    public required string MethodRole { get; init; }

    public required string BodyAvailability { get; init; }

    public required BodyAvailabilityCode BodyAvailabilityCode { get; init; }
}

public enum BodyAvailabilityCode : byte
{
    None = 0,
    NativeGenerated = 1,
    InterpreterReady = 2,
    BridgeDispatch = 3,
    MetadataOnly = 4,
    ExternalRuntime = 5,
    Unsupported = 6,
}

public enum HybridDispatchKind : byte
{
    None = 0,
    Direct = 1,
    Virtual = 2,
    Bridge = 3,
    ExternalRuntime = 4,
    Unsupported = 5,

    /// <summary>
    /// Method call on a ComImport interface. Dispatched through the COM vtable
    /// rather than the managed vtable. The <see cref="AotCoreIrInstructionArtifact.ComVtableSlot"/>
    /// field carries the vtable index.
    /// </summary>
    ComVtable = 6,
}

/// <summary>
/// Encodes whether a generic context is carried by a closed type, a closed method, or both.
/// </summary>
public enum GenericContextKind : byte
{
    /// <summary>
    /// Closed context comes from the declaring type instantiation.
    /// </summary>
    TypeInstantiation = 1,

    /// <summary>
    /// Closed context comes from the method instantiation.
    /// </summary>
    MethodInstantiation = 2,

    /// <summary>
    /// Closed context carries both declaring-type and method instantiation arguments.
    /// </summary>
    TypeAndMethodInstantiation = 3,
}

public enum GenericSupportKind : byte
{
    Legal = 1,

    Shared = 2,

    Specialized = 3,

    Forbidden = 4,
}

public enum GenericSpecializationKind : byte
{
    SharedBody = 1,

    SpecializedBody = 2,
}

public enum GenericDemandFamilyKind : byte
{
    ClosedGenericType = 1,

    ClosedMethodOnGenericType = 2,

    ClosedGenericMethod = 3,

    ClosedGenericMethodOnGenericType = 4,
}

public sealed record GenericInstantiationKey
{
    public required GenericContextKind ContextKind { get; init; }

    public required string DefinitionSubjectId { get; init; }

    public IReadOnlyList<string>? TypeArguments { get; init; }

    public IReadOnlyList<string>? MethodArguments { get; init; }
}

public sealed record SharedGenericBodyId
{
    public required string Value { get; init; }
}

public sealed record InstantiationStubId
{
    public required string Value { get; init; }
}

public sealed record RuntimeGenericContextArtifact
{
    public required GenericInstantiationKey InstantiationKey { get; init; }

    public required SharedGenericBodyId SharedGenericBodyId { get; init; }

    public required InstantiationStubId InstantiationStubId { get; init; }

    public required GenericSupportKind SupportKindCode { get; init; }

    public required GenericSpecializationKind SpecializationKindCode { get; init; }

    public string? StatusReasonCode { get; init; }
}

public sealed record GenericDiagnosticArtifact
{
    public required string SubjectId { get; init; }

    public required string DefinitionSubjectId { get; init; }

    public required string DisplaySubjectId { get; init; }

    public required GenericInstantiationKey InstantiationKey { get; init; }
}

public sealed record GenericInstantiationDemandModel
{
    public required string RequestingAssemblyName { get; init; }

    public required string OwningAssemblyName { get; init; }

    public required string SubjectKind { get; init; }

    public required string SubjectId { get; init; }

    public required string DefinitionSubjectId { get; init; }

    public required string DemandSourceKind { get; init; }

    public required GenericInstantiationKey InstantiationKey { get; init; }

    public required GenericSupportKind SupportKindCode { get; init; }

    public required GenericSpecializationKind SpecializationKindCode { get; init; }

    public required GenericDemandFamilyKind FamilyKindCode { get; init; }

    public bool IsCrossAssembly { get; init; }
}

public sealed record GenericInstantiationDemandGraphModel
{
    public required IReadOnlyList<GenericInstantiationDemandModel> Demands { get; init; }
}

public sealed record GenericCapabilityMatrixArtifact
{
    public string FormatVersion { get; init; } = "v0";

    public string ArtifactKind { get; init; } = "genericCapabilityMatrix";

    public required string OwnerSubjectId { get; init; }

    public required string EntrySubjectId { get; init; }

    public required IReadOnlyList<string> HotUpdateModes { get; init; }

    public required GenericCapabilityMatrixGateStatus Gates { get; init; }

    public required IReadOnlyList<GenericCapabilityFamilyBudgetArtifact> FamilyBudgets { get; init; }

    public required IReadOnlyList<GenericCapabilityBoundaryCaseArtifact> BoundaryCases { get; init; }

    public required IReadOnlyList<GenericCapabilityMatrixEntryArtifact> Entries { get; init; }
}

public sealed record GenericCapabilityMatrixGateStatus
{
    public required string Status { get; init; }

    public required int UnsupportedLeakCount { get; init; }

    public required int MissingAuthorityCount { get; init; }

    public required int NonCanonicalHotUpdateNameCount { get; init; }

    public required IReadOnlyList<GenericCapabilityMatrixLeakArtifact> Leaks { get; init; }
}

public sealed record GenericCapabilityFamilyBudgetArtifact
{
    public required string FamilyId { get; init; }

    public required string DisplayName { get; init; }

    public required int BudgetLimit { get; init; }

    public required int ObservedCount { get; init; }
}

public sealed record GenericCapabilityBoundaryCaseArtifact
{
    public required string BoundaryKind { get; init; }

    public required string SourceMethodSubjectId { get; init; }

    public required int IlOffset { get; init; }

    public required string TargetSubjectId { get; init; }

    public required string EvidenceKind { get; init; }

    public required string Status { get; init; }
}

public sealed record GenericCapabilityMatrixEntryArtifact
{
    public required string SubjectKind { get; init; }

    public required string SubjectId { get; init; }

    public required string DefinitionSubjectId { get; init; }

    public required string DemandSourceKind { get; init; }

    public required string RequestingAssemblyName { get; init; }

    public required string OwningAssemblyName { get; init; }

    public required GenericDemandFamilyKind FamilyKindCode { get; init; }

    public required GenericContextKind ContextKindCode { get; init; }

    public required GenericSupportKind SupportKindCode { get; init; }

    public required GenericSpecializationKind SpecializationKindCode { get; init; }

    public required bool IsCrossAssembly { get; init; }

    public required string StatusReasonCode { get; init; }

    public required string AuthoritySource { get; init; }

    public required string OpenDefinitionSubjectId { get; init; }

    public required string SharedGenericBodyId { get; init; }

    public required string InstantiationStubId { get; init; }

    public required bool HasOpenDefinitionAuthority { get; init; }

    public required bool HasSharedBodyAuthority { get; init; }

    public required bool HasInstantiationStubAuthority { get; init; }

    public required bool HasRuntimeGenericContextAuthority { get; init; }
}

public sealed record GenericCapabilityMatrixLeakArtifact
{
    public required string SubjectId { get; init; }

    public required string LeakKind { get; init; }

    public required string Reason { get; init; }
}

