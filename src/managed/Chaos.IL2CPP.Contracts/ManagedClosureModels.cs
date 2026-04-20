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

    public bool IsPreserved { get; init; }

    public bool IsUnmanagedCallersOnly { get; init; }

    public required int MetadataToken { get; init; }

    public required IReadOnlyList<ManagedParameterModel> Parameters { get; init; }

    public ManagedImportModel? Import { get; init; }

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

public sealed record GenericContextArtifact
{
    public required GenericContextKind ContextKind { get; init; }

    public required string DefinitionSubjectId { get; init; }

    public IReadOnlyList<string>? TypeArguments { get; init; }

    public IReadOnlyList<string>? MethodArguments { get; init; }
}

