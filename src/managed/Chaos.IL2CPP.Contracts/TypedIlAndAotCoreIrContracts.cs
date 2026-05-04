namespace Chaos.IL2CPP.Contracts;

public sealed record TypedIlIrArtifact
{
    public string FormatVersion { get; init; } = "v0";

    public string ArtifactKind { get; init; } = "typedIlIr";

    public required IReadOnlyList<TypedIlMethodArtifact> Methods { get; init; }
}

/// <summary>
/// Canonical reference kinds carried by <see cref="AotCoreIrArtifact"/>.
/// </summary>
public enum AotCoreIrReferenceKind : byte
{
    /// <summary>
    /// Reference points to a managed type subject.
    /// </summary>
    Type = 1,

    /// <summary>
    /// Reference points to a managed field subject.
    /// </summary>
    Field = 2,

    /// <summary>
    /// Reference points to a managed method subject.
    /// </summary>
    Method = 3,
}

/// <summary>
/// Minimal type-shape information carried by <see cref="AotCoreIrReferenceArtifact"/>.
/// </summary>
public enum AotCoreIrTypeShapeKind : byte
{
    /// <summary>
    /// Type is a reference type.
    /// </summary>
    ReferenceType = 1,

    /// <summary>
    /// Type is a value type.
    /// </summary>
    ValueType = 2,

    /// <summary>
    /// Type is an interface.
    /// </summary>
    InterfaceType = 3,
}

/// <summary>
/// Minimal object/runtime services required by the current AOT core IR slice.
/// </summary>
public enum AotCoreIrRuntimeServiceKind : byte
{
    /// <summary>
    /// Allocates an object instance and wires constructor dispatch.
    /// </summary>
    NewObject = 1,

    /// <summary>
    /// Loads an instance field value.
    /// </summary>
    LoadInstanceField = 2,

    /// <summary>
    /// Stores an instance field value.
    /// </summary>
    StoreInstanceField = 3,

    /// <summary>
    /// Loads a static field value.
    /// </summary>
    LoadStaticField = 4,

    /// <summary>
    /// Stores a static field value.
    /// </summary>
    StoreStaticField = 5,

    /// <summary>
    /// Allocates a managed array.
    /// </summary>
    NewArray = 6,

    /// <summary>
    /// Performs a checked reference cast.
    /// </summary>
    CastClass = 7,

    /// <summary>
    /// Performs an instance-of compatible cast probe.
    /// </summary>
    IsInst = 8,

    /// <summary>
    /// Boxes a value type into a managed object.
    /// </summary>
    Box = 9,

    /// <summary>
    /// Produces a managed pointer to boxed storage.
    /// </summary>
    Unbox = 10,

    /// <summary>
    /// Extracts a value from boxed storage.
    /// </summary>
    UnboxAny = 11,

    /// <summary>
    /// Loads a managed array element.
    /// </summary>
    LoadArrayElement = 12,

    /// <summary>
    /// Stores a managed array element.
    /// </summary>
    StoreArrayElement = 13,

    /// <summary>
    /// Initializes value-type storage through a managed pointer.
    /// </summary>
    InitObject = 14,
}

/// <summary>
/// Encodes managed EH region shape after lowering into AOT Core IR.
/// </summary>
public enum AotCoreIrExceptionRegionKind : byte
{
    /// <summary>
    /// Typed catch handler.
    /// </summary>
    Catch = 1,

    /// <summary>
    /// Finally handler.
    /// </summary>
    Finally = 2,

    /// <summary>
    /// Fault handler.
    /// </summary>
    Fault = 3,

    /// <summary>
    /// Filtered catch handler.
    /// </summary>
    Filter = 4,
}

/// <summary>
/// Encodes the lowered ABI carrier used by Native AOT direct-call signatures.
/// </summary>
public enum AotCoreIrAbiCarrierKind : byte
{
    /// <summary>
    /// Method returns no value.
    /// </summary>
    Void = 0,

    /// <summary>
    /// 32-bit integer carrier.
    /// </summary>
    Int32 = 1,

    /// <summary>
    /// Native pointer-sized integer carrier.
    /// </summary>
    NativeInt = 2,

    /// <summary>
    /// Value type travels by value through the native ABI.
    /// </summary>
    ValueTypeByValue = 3,

    /// <summary>
    /// Signed 8-bit integer carrier.
    /// </summary>
    Int8 = 4,

    /// <summary>
    /// Unsigned 8-bit integer carrier.
    /// </summary>
    UInt8 = 5,

    /// <summary>
    /// Signed 16-bit integer carrier.
    /// </summary>
    Int16 = 6,

    /// <summary>
    /// Unsigned 16-bit integer carrier.
    /// </summary>
    UInt16 = 7,

    /// <summary>
    /// 32-bit floating-point carrier.
    /// </summary>
    Float32 = 8,

    /// <summary>
    /// 64-bit floating-point carrier.
    /// </summary>
    Float64 = 9,

    /// <summary>
    /// Signed 64-bit integer carrier.
    /// </summary>
    Int64 = 10,

    /// <summary>
    /// Unsigned 64-bit integer carrier.
    /// </summary>
    UInt64 = 11,

    /// <summary>
    /// By-reference parameter (ref/out). Carried as tagged pointer on eval stack,
    /// formalized for ABI manifest validation across DLL boundaries.
    /// </summary>
    ByRef = 12,

    /// <summary>
    /// Multi-return value marker — indicates an out parameter that is part of the
    /// logical return value set. Codegen emits as hidden pointer or struct return.
    /// </summary>
    MultiReturn = 13,

    /// <summary>
    /// By-reference to value type (ref struct). Carries type identity of the
    /// underlying value type for ABI compatibility checks.
    /// </summary>
    ByRefToValueType = 14,
}

/// <summary>
/// Minimal ABI slot information consumed by Native AOT lowering.
/// </summary>
public sealed record AotCoreIrAbiSlotArtifact
{
    public required AotCoreIrAbiCarrierKind CarrierKindCode { get; init; }

    public string? TypeSubjectId { get; init; }

    public AotCoreIrTypeShapeKind TypeShape { get; init; }
}

public sealed record AotCoreIrReferenceArtifact
{
    public required AotCoreIrReferenceKind Kind { get; init; }

    public required string AssemblyName { get; init; }

    public required string SubjectId { get; init; }

    public string? OpenDefinitionSubjectId { get; init; }

    public SharedGenericBodyId? SharedGenericBodyId { get; init; }

    public InstantiationStubId? InstantiationStubId { get; init; }

    public RuntimeGenericContextArtifact? RuntimeGenericContext { get; init; }

    public GenericDiagnosticArtifact? GenericDiagnostic { get; init; }

    public AotCoreIrTypeShapeKind TypeShape { get; init; }

    public string? ArrayElementSubjectId { get; init; }

    public AotCoreIrTypeShapeKind ArrayElementTypeShape { get; init; }

    public string? ArrayElementBaseTypeSubjectId { get; init; }

    public IReadOnlyList<string>? ArrayElementImplementedInterfaceSubjectIds { get; init; }

    public bool IsSealed { get; init; }

    public string? BaseTypeSubjectId { get; init; }

    public IReadOnlyList<string>? ImplementedInterfaceSubjectIds { get; init; }

    public string? DeclaringTypeSubjectId { get; init; }

    public AotCoreIrTypeShapeKind DeclaringTypeShape { get; init; }
}

public sealed record AotCoreIrArtifact
{
    public string FormatVersion { get; init; } = "v0";

    public string ArtifactKind { get; init; } = "aotCoreIr";

    public required IReadOnlyList<AotCoreIrMethodArtifact> Methods { get; init; }
}

public sealed record AotCoreIrMethodArtifact
{
    public required string MethodId { get; init; }

    public required string SubjectId { get; init; }

    public required string Signature { get; init; }

    public required ManagedMethodIdentityArtifact Identity { get; init; }

    public string? OpenDefinitionSubjectId { get; init; }

    public SharedGenericBodyId? SharedGenericBodyId { get; init; }

    public InstantiationStubId? InstantiationStubId { get; init; }

    public RuntimeGenericContextArtifact? RuntimeGenericContext { get; init; }

    public GenericDiagnosticArtifact? GenericDiagnostic { get; init; }

    public required string NativeSymbol { get; init; }

    public required bool IsStatic { get; init; }

    public required string ReturnType { get; init; }

    public required AotCoreIrAbiSlotArtifact ReturnAbi { get; init; }

    public required int ParameterCount { get; init; }

    public required IReadOnlyList<AotCoreIrAbiSlotArtifact> ParameterAbis { get; init; }

    public required int LocalCount { get; init; }

    public required int ExceptionRegionCount { get; init; }

    public required IReadOnlyList<AotCoreIrExceptionRegionArtifact> ExceptionRegions { get; init; }

    public required IReadOnlyList<AotCoreIrInstructionArtifact> Instructions { get; init; }
}

public sealed record AotCoreIrExceptionRegionArtifact
{
    public required AotCoreIrExceptionRegionKind HandlingKindCode { get; init; }

    public required int TryOffset { get; init; }

    public required int TryLength { get; init; }

    public required int HandlerOffset { get; init; }

    public required int HandlerLength { get; init; }

    public int? FilterOffset { get; init; }

    public string? CatchTypeSubjectId { get; init; }
}

public sealed record AotCoreIrInstructionArtifact
{
    public required string Op { get; init; }

    public object? Operand { get; init; }

    public required int IlOffset { get; init; }

    public string? ResultType { get; init; }

    public string? Callee { get; init; }

    public ManagedCallSiteSignature? CallSiteSignature { get; init; }

    public ManagedInstructionReference? Reference { get; init; }

    public AotCoreIrReferenceArtifact? TargetReference { get; init; }

    public AotCoreIrRuntimeServiceKind? RuntimeServiceKind { get; init; }

    public string? TargetSymbol { get; init; }

    public int? TargetParameterCount { get; init; }

    public string? TargetReturnType { get; init; }

    public HybridDispatchKind? DispatchKindCode { get; init; }
}

public sealed record TypedIlMethodArtifact
{
    public required string MethodId { get; init; }

    public required string SubjectId { get; init; }

    public required string Signature { get; init; }

    public required ManagedMethodIdentityArtifact Identity { get; init; }

    public required string MethodRole { get; init; }

    public required string BodyAvailability { get; init; }

    public required BodyAvailabilityCode BodyAvailabilityCode { get; init; }

    public required IReadOnlyList<string> Capabilities { get; init; }

    public required IReadOnlyList<TypedIlParameterArtifact> Parameters { get; init; }

    public required IReadOnlyList<TypedIlBlockArtifact> Blocks { get; init; }
}

public sealed record TypedIlParameterArtifact
{
    public required string Name { get; init; }

    public required string Type { get; init; }
}

public sealed record TypedIlBlockArtifact
{
    public required string BlockId { get; init; }

    public required IReadOnlyList<TypedIlInstructionArtifact> Instructions { get; init; }
}

public sealed record TypedIlInstructionArtifact
{
    public required string Op { get; init; }

    public object? Operand { get; init; }

    public string? ResultType { get; init; }

    public string? Callee { get; init; }

    public ManagedCallSiteSignature? CallSiteSignature { get; init; }

    public ManagedInstructionReference? Reference { get; init; }

    public HybridDispatchKind? DispatchKindCode { get; init; }
}

