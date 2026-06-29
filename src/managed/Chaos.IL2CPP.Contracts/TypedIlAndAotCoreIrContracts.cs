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

    /// <summary>
    /// Whether the type is a COM import interface (marked with ComImportAttribute / TypeAttributes.Import).
    /// When true, method calls on this type must be dispatched through the COM vtable
    /// instead of the managed vtable.
    /// </summary>
    public bool IsComImport { get; init; }

    /// <summary>
    /// 16-byte COM interface GUID from <see cref="System.Runtime.InteropServices.GuidAttribute"/>,
    /// formatted as a standard 36-character string (e.g., "ABCDEF01-2345-6789-ABCD-EF0123456789").
    /// Null if the type has no GuidAttribute. Used by CCW codegen to emit GUID constants
    /// and RegisterCcwInterface calls for multi-interface QI support.
    /// </summary>
    public string? ComInterfaceGuid { get; init; }

    /// <summary>
    /// Kind of COM interface type: 0=Unknown (IUnknown-based), 1=Dispatch (IDispatch-based),
    /// 2=Dual (both). Derived from <see cref="System.Runtime.InteropServices.ComInterfaceTypeAttribute"/>.
    /// Defaults to 0 (IUnknown) when no attribute is present.
    /// Used by IDispatch codegen to select vtable layout (3+N vs 7+N slots).
    /// </summary>
    public int ComInterfaceTypeKind { get; init; }

    public string? BaseTypeSubjectId { get; init; }

    public IReadOnlyList<string>? ImplementedInterfaceSubjectIds { get; init; }

    public string? DeclaringTypeSubjectId { get; init; }

    public AotCoreIrTypeShapeKind DeclaringTypeShape { get; init; }

    /// <summary>
    /// Field type subject ID (e.g. "System.Int32", "System.Boolean").
    /// Populated only when <see cref="Kind"/> is <see cref="AotCoreIrReferenceKind.Field"/>.
    /// Used by the planner to emit correct C++ field types instead of uniform CHAOS_IL2CPP_INTPTR.
    /// </summary>
    public string? FieldTypeSubjectId { get; init; }
}

/// <summary>
/// Describes a single field within a struct marshalling descriptor tree.
/// Maps to mach the C++ <c>StructFieldDescriptorV1</c> layout.
/// </summary>
public sealed record StructFieldDescriptorArtifact
{
    /// <summary>
    /// Field kind string matching <c>marshal_abi::StructFieldKind</c> names:
    /// "Blittable", "BoolField", "StringField", "NestedStruct", "ByValArray",
    /// "LPArray", "DecimalField", "DateTimeField", "ObjectField", "GuidField".
    /// </summary>
    public required string Kind { get; init; }

    /// <summary>
    /// Byte offset within the native blob (and managed blob, for V1).
    /// </summary>
    public required int Offset { get; init; }

    /// <summary>
    /// Field size in bytes (native representation size).
    /// </summary>
    public required int Size { get; init; }

    /// <summary>
    /// For ByValArray: element count. 0 for non-array fields.
    /// </summary>
    public int ArrayCount { get; init; }

    /// <summary>
    /// For array fields: the native element type ("U1", "I4", "R8", "Struct", etc.).
    /// Maps to <c>marshal_abi::NativeElementType</c>.
    /// </summary>
    public string? ElementType { get; init; }

    /// <summary>
    /// For NestedStruct: the SubjectId of the nested type, used to look up
    /// its own StructMarshallingDescriptorArtifact for recursive emission.
    /// </summary>
    public string? NestedTypeSubjectId { get; init; }

    /// <summary>
    /// The managed field name. Populated from <c>ManagedFieldModel.Name</c>
    /// during <c>AotCoreIrLowering.BuildStructMarshallingDescriptor</c>.
    /// Used by <c>Marshal.OffsetOf&lt;T&gt;(string)</c> codegen shapes to emit
    /// parallel field-name arrays for field-name-to-offset resolution.
    /// </summary>
    public string? Name { get; init; }
}

/// <summary>
/// Complete marshalling descriptor for a struct type, mapping its managed
/// representation to native layout. Carried from AotCoreIrLowering to
/// ObjectModelEmission so that codegen can emit <c>StructMarshallingDescriptorV1</c>
/// C++ globals.
/// </summary>
public sealed record StructMarshallingDescriptorArtifact
{
    /// <summary>
    /// SubjectId of the struct type this descriptor describes.
    /// </summary>
    public required string TypeSubjectId { get; init; }

    /// <summary>
    /// Total struct size in bytes (native layout).
    /// </summary>
    public required int TotalSize { get; init; }

    /// <summary>
    /// Field descriptors, in field declaration order.
    /// </summary>
    public required IReadOnlyList<StructFieldDescriptorArtifact> Fields { get; init; }
}

public sealed record AotCoreIrArtifact
{
    public string FormatVersion { get; init; } = "v0";

    public string ArtifactKind { get; init; } = "aotCoreIr";

    public required IReadOnlyList<AotCoreIrMethodArtifact> Methods { get; init; }

    /// <summary>
    /// Marshalling descriptors for complex (non-blittable) value types
    /// used in P/Invoke signatures. Emitted as <c>StructMarshallingDescriptorV1</c>
    /// C++ globals from ObjectModelEmission.
    /// </summary>
    public IReadOnlyList<StructMarshallingDescriptorArtifact>? StructMarshallingDescriptors { get; init; }
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

    /// <summary>
    /// Fully-qualified type subject IDs for each managed parameter, in declaration order.
    /// Populated during semantic world construction from the original PE metadata.
    /// Format: "AssemblyName/Namespace.TypeName" (e.g. "System.Data.Common/System.Data.CommandBehavior").
    /// Used by AOT IR lowering (ResolveAbiSlot) as the authoritative TypeSubjectId when
    /// the managed type cannot be resolved in the local assembly scope.
    /// Null when not available (legacy/manifest-only methods).
    /// </summary>
    public IReadOnlyList<string>? ManagedParameterTypeSubjectIds { get; init; }

    public required int LocalCount { get; init; }

    public required int ExceptionRegionCount { get; init; }

    public required IReadOnlyList<AotCoreIrExceptionRegionArtifact> ExceptionRegions { get; init; }

    public required IReadOnlyList<AotCoreIrInstructionArtifact> Instructions { get; init; }

    /// Whether this method is a P/Invoke (DllImport) external method.
    public bool IsPInvoke { get; init; }

    /// Whether this method is marked with <see cref="System.Runtime.InteropServices.UnmanagedCallersOnlyAttribute"/>.
    /// When true, the method is callable directly from native code through its <see cref="NativeSymbol"/>.
    public bool IsUnmanagedCallersOnly { get; init; }

    /// For P/Invoke methods: the native DLL module name (e.g. "kernel32").
    public string? ImportModuleName { get; init; }

    /// For P/Invoke methods: the native entry-point name (e.g. "Sleep").
    public string? ImportEntryPointName { get; init; }

    /// <summary>
    /// For P/Invoke methods: raw <see cref="System.Reflection.MethodImportAttributes"/>
    /// calling-convention bits (mask 0x0700). 0 = default (WinApi).
    /// </summary>
    public int ImportCallingConvention { get; init; }

    /// <summary>
    /// For P/Invoke methods: raw <see cref="System.Reflection.MethodImportAttributes"/>
    /// CharSet bits (mask 0x0006). 0 = default (Ansi).
    /// </summary>
    public int ImportCharSet { get; init; }

    /// <summary>
    /// For P/Invoke methods: when true, the native call is preceded by
    /// <c>SetLastError(0)</c> and followed by <c>GetLastError()</c> stored
    /// to thread-local storage so <c>Marshal.GetLastPInvokeError()</c> can
    /// retrieve the per-call error code.
    /// </summary>
    public bool ImportSetLastError { get; init; }

    /// <summary>
    /// For P/Invoke methods: when true, the module name is "__Internal",
    /// meaning the native function is linked statically at compile time
    /// (no LoadLibrary/GetProcAddress needed). The codegen emits a direct
    /// <c>extern "C"</c> function declaration instead.
    /// </summary>
    public bool IsInternalLink { get; init; }

    /// <summary>
    /// For P/Invoke methods: when true, the method is annotated with
    /// <see cref="System.Runtime.InteropServices.SuppressGCTransitionAttribute"/>.
    /// The generated wrapper skips the GC_TRANSITION_TO_PREEMPTIVE /
    /// GC_TRANSITION_TO_COOPERATIVE pair around the native call.
    /// </summary>
    public bool IsSuppressGCTransition { get; init; }

    /// For P/Invoke methods: indices of parameters whose managed type is
    /// System.String and therefore need UTF-8 marshalling.
    public IReadOnlyList<int>? StringParameterIndices { get; init; }

    /// For P/Invoke methods: indices of parameters whose managed type derives
    /// from <see cref="System.Runtime.InteropServices.SafeHandle"/>.
    /// These are marshalled by extracting the inner handle value (IntPtr)
    /// via <c>MarshalSafeHandleGetHandle</c> before the native call.
    public IReadOnlyList<int>? SafeHandleParameterIndices { get; init; }

    /// For P/Invoke methods: indices of parameters whose managed type is a
    /// blittable value type (struct composed of blittable primitive fields).
    /// These are marshalled via a stack copy + pointer pass, no conversion.
    public IReadOnlyList<int>? BlittableStructParameterIndices { get; init; }

    /// For P/Invoke methods: true when the return type is a blittable value type.
    /// The native function returns the struct by value; the stub returns it directly.
    public bool HasBlittableStructReturn { get; init; }

    /// For P/Invoke methods: indices of parameters whose managed type is a
    /// value type containing string fields (besides blittable primitives).
    /// Each string field is converted to/from UTF-8 CoTaskMem around the call.
    public IReadOnlyList<int>? SimpleNonBlittableStructParameterIndices { get; init; }

    /// For each entry in <see cref="SimpleNonBlittableStructParameterIndices"/>,
    /// the SubjectIds of the string fields in that struct, grouped per parameter.
    /// Indexed by the same order as <see cref="SimpleNonBlittableStructParameterIndices"/>.
    public IReadOnlyList<IReadOnlyList<string>>? SimpleNonBlittableStructStringFieldSubjectIds { get; init; }

    /// For P/Invoke methods: indices of parameters whose managed type is a
    /// complex non-blittable value type with arbitrary field types. These are
    /// marshalled via a deep copy with per-field conversion.
    public IReadOnlyList<int>? ComplexStructParameterIndices { get; init; }

    /// <summary>
    /// For P/Invoke methods: the SubjectId of each complex struct parameter type.
    /// Parallel to <see cref="ComplexStructParameterIndices"/> (same order and count).
    /// </summary>
    public IReadOnlyList<string>? ComplexStructParameterTypeSubjectIds { get; init; }

    /// <summary>
    /// For P/Invoke methods: the assembly name that declares this method.
    /// Used by the DllImportResolver lookup to find the per-assembly resolver callback.
    /// </summary>
    public string? DeclaringAssemblyName { get; init; }
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

    public InstructionOpCode? OpCode { get; init; }

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

    /// <summary>
    /// When non-null, this callvirt was preceded by a <c>constrained.</c> IL prefix.
    /// The value is the type SubjectId that should be used to resolve the call target.
    /// </summary>
    public string? ConstrainedTypeSubjectId { get; init; }

    /// <summary>
    /// For <see cref="HybridDispatchKind.ComVtable"/> dispatch: the vtable slot
    /// index to call. Slot 0-2 are IUnknown (QueryInterface/AddRef/Release);
    /// slot 3+ are interface-specific methods in declaration order.
    /// </summary>
    public int? ComVtableSlot { get; init; }

    /// <summary>
    /// For ComVtable dispatch: whether the method has [PreserveSig].
    /// If false (COM default), the HRESULT return is checked and
    /// COMException is thrown on failure; void-return methods suppress
    /// the eval-stack push.
    /// </summary>
    public bool IsPreserveSig { get; init; }
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

    /// <summary>
    /// When non-null, this callvirt was preceded by a <c>constrained.</c> IL prefix.
    /// The value is the type SubjectId that should be used to resolve the call target.
    /// </summary>
    public string? ConstrainedTypeSubjectId { get; init; }
}
