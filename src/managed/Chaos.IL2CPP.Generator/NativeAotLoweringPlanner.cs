using System;
using System.Collections.Generic;
using System.Collections.Immutable;
using System.Diagnostics;
using System.Globalization;
using System.Linq;
using System.Reflection;
using System.Reflection.Metadata;
using System.Reflection.Metadata.Ecma335;
using System.Reflection.PortableExecutable;
using System.Runtime.ExceptionServices;
using System.Text;
using System.Text.Json;
using System.Threading.Tasks;
using Chaos.IL2CPP.Contracts;
using Scriban.Runtime;

namespace Chaos.IL2CPP.Generator;

public sealed partial class NativeAotLoweringPlanner
{
    private IReadOnlyDictionary<string, AotCoreIrMethodArtifact> _methodsBySubjectId =
        new Dictionary<string, AotCoreIrMethodArtifact>(StringComparer.Ordinal);

    private Dictionary<string, string> _attributeStorageFieldIndex =
        new Dictionary<string, string>(StringComparer.Ordinal);

    private IReadOnlyDictionary<string, string?> _referenceTypeBaseSubjectIds =
        new Dictionary<string, string?>(StringComparer.Ordinal);

    private IReadOnlyDictionary<string, HashSet<string>> _referenceTypeImplementedInterfaceSubjectIds =
        new Dictionary<string, HashSet<string>>(StringComparer.Ordinal);

    private IReadOnlySet<string> _valueTypeSubjectIds =
        new HashSet<string>(StringComparer.Ordinal);

    private IReadOnlySet<string> _enumTypeSubjectIds =
        new HashSet<string>(StringComparer.Ordinal);

    // Types verified to have zero GC-reference fields (safe for stack allocation).
    // Populated during EmitObjectModelDeclarations, consumed by EmitLinearNewObject.
    private HashSet<string>? _typesSafeForStackAllocation;

    // Enum field value→name map, built once from FieldEntries or PE metadata scan.
    // Used by both AOT Bake planning and BoxToString switch emission.
    private Dictionary<string, Dictionary<long, string>> _enumValueToNameMap =
        new(StringComparer.Ordinal);

    private CustomAttributeSupportModel _customAttributeSupport = CustomAttributeSupportModel.Empty;
    private AssemblyReflectionSupportModel _assemblyReflectionSupport = AssemblyReflectionSupportModel.Empty;
    private ReflectionMemberSupportModel _reflectionMemberSupport = ReflectionMemberSupportModel.Empty;
    private StaticFieldDataSupportModel _staticFieldDataSupport = StaticFieldDataSupportModel.Empty;
    private IReadOnlySet<string> _hotUpdateAssemblyNames =
        new HashSet<string>(StringComparer.Ordinal);

    private IReadOnlyList<string> _cachedClosureAssemblyPaths = Array.Empty<string>();
    private IReadOnlyDictionary<string, string> _closureAssemblyPathByName =
        new Dictionary<string, string>(StringComparer.Ordinal);
    // ── Phase 3: AOT Devirtualization ──
    private readonly struct DevirtualizationHint
    {
        public static readonly DevirtualizationHint None = new(false, null!, null!, null);
        public bool CanDevirtualize { get; }
        public string ImplementationMethodSubjectId { get; }
        public string DeclaringTypeSubjectId { get; }
        public string? GuardTypeSubjectId { get; }  // non-null = conditional guard check

        public DevirtualizationHint(
            bool canDevirtualize,
            string implementationMethodSubjectId,
            string declaringTypeSubjectId,
            string? guardTypeSubjectId = null)
        {
            CanDevirtualize = canDevirtualize;
            ImplementationMethodSubjectId = implementationMethodSubjectId;
            DeclaringTypeSubjectId = declaringTypeSubjectId;
            GuardTypeSubjectId = guardTypeSubjectId;
        }
    }

    private Dictionary<string, DevirtualizationHint> _devirtualizationHints =
        new Dictionary<string, DevirtualizationHint>(StringComparer.Ordinal);

    // Types that are instantiated in the compiled closure (via newobj or constrained prefix).
    // Used for reachability-based devirtualization: if a virtual method has multiple
    // implementations but only one is on an instantiated type, the call can be devirtualized.
    private HashSet<string> _instantiatedTypeSubjectIds =
        new HashSet<string>(StringComparer.Ordinal);

    // A2.4: Pre-computed fold map for System.Enum::ToString() calls where the
    // `this` argument is a ldsfld of a literal enum field. Maps call-site IlOffset
    // to the enum field name string literal.
    private Dictionary<int, string> _enumToStringFoldMap =
        new Dictionary<int, string>();

    // ── Enum AOT bake (A2.5) ───────────────────────────────────────────
    /// For calls where the enum type is known at codegen time (e.g. ldtoken
    /// <DayOfWeek> + call Enum::Parse/GetName/Format), this pre-computes the
    /// result so the emitter can substitute a compile-time constant.
    private sealed record EnumAotBakeEntry(
        string EnumTypeId,
        string Callee,      // full callee subject id
        string? ConstantStr, // pre-evaluated string result (for GetName/Format/ToString)
        long? ConstantInt,   // pre-evaluated numeric result (for Parse/IsDefined — boxed)
        int ArgCount,        // how many eval-stack args to consume
        int[]? SkipIlOffsets // IlOffsets of dead instructions to skip (GetTypeFromHandle, box)
    );

    private Dictionary<(string MethodId, int IlOffset), EnumAotBakeEntry> _enumAotBakeMap =
        new Dictionary<(string, int), EnumAotBakeEntry>();

    /// IlOffset skip set for enum AOT-bake DCE (dead GetTypeFromHandle + box instructions).
    private Dictionary<string, HashSet<int>> _enumAotBakeSkipIlOffsets =
        new Dictionary<string, HashSet<int>>();

    /// Counter for AOT-baked enum cache array indexing.
    /// Incremented during emission to assign sequential indices.
    private int _enumAotBakeCacheCount;

    /// Total size of the _g_bake_cache_[] array, set before the emission phase.
    private int _enumAotBakeCacheArraySize;

    // ── TypeInfo* direct API (A2.6) ──────────────────────────────────────
    /// Pre-computed fold map for typeof(T).IsAssignableFrom(typeof(U)) type
    /// hierarchy calls where both arguments are typeof() constants known at
    /// AOT time.  Maps call-site IlOffset → fold entry with pre-resolved
    /// TypeInfo* expressions that bypass GetTypeFromHandle entirely.
    private readonly record struct TypeHierarchyPtrFoldEntry(
        string PtrFunctionName,
        string TypeExpr1,
        string? TypeExpr2,
        int[] SkipIlOffsets);  // IlOffsets of ltoken + GetTypeFromHandle to DCE

    // NOTE: These map to CHAOS_IL2CPP_FORCEINLINE functions in hierarchy_fast_api.h
    // (included in the generated code TU), enabling cross-TU inlining into SEH-protected
    // methods. The *PtrFast variants also use a single merged parent walk (subclass +
    // interface check combined) to eliminate redundant traversals.
    private static readonly Dictionary<string, string> TypeHierarchyPtrOptimizationMap = new(StringComparer.Ordinal)
    {
        { "IsAssignableFrom", "ChaosReflectionIsAssignableFromPtr" },
        { "IsSubclassOf",     "ChaosReflectionIsSubclassOfPtr" },
        { "IsAssignableTo",   "ChaosReflectionIsAssignableToPtr" },
        { "IsInstanceOfType", "ChaosReflectionIsInstanceOfTypePtr" },
    };

    private Dictionary<(string MethodNativeSymbol, int IlOffset), TypeHierarchyPtrFoldEntry> _typeHierarchyPtrFoldMap = new();
    private Dictionary<string, HashSet<int>> _typeHierarchyPtrSkipIlOffsets = new();

    // ── typeof(T) compile-time fold (A2.7) ─────────────────────────────────
    /// For any <c>ldtoken &lt;const_type&gt; + call GetTypeFromHandle</c> where the
    /// type is AOT-known, fold both instructions into a direct TypeInfo* pointer
    /// expression, bypassing the runtime metadata dispatch entirely.
    /// Affects <c>typeof(byte)</c>, <c>typeof(DayOfWeek)</c>, etc. used as
    /// arguments to Enum.Format/Parse/TryParse.
    private readonly record struct TypeOfFoldEntry(
        string TypeInfoExpr,  // e.g. "reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_mt_X.AsTypeInfoHot())"
        int[] SkipIlOffsets); // [ltoken_offset]

    private Dictionary<(string MethodNativeSymbol, int IlOffset), TypeOfFoldEntry> _typeOfFoldMap = new();
    private Dictionary<string, HashSet<int>> _typeOfSkipIlOffsets = new();

    // Pre-try TypeInfo* fold initializers: emitted BEFORE CHAOS_EH_TRY so the
    // *Ptr call avoids SEH frame setup/teardown overhead.
    private List<(string VarName, string Expression)>? _preTryFoldInitializers;

    private CodegenMode _codegenMode = CodegenMode.Aot;
    private HashSet<string>? _subjectMethodSubjectIds;

    // Verification dispatch manifest (populated by BuildDispatchEntryCode)
    private string? _manifestJson;

    // ── Hot-update static field helpers ───────────────────────────────────

    /// Compute FNV-1a 24-bit hash (matches ComputeAssemblyHash in static_var_store.h).
    private static uint ComputeFNVHash(ReadOnlySpan<char> text)
    {
        uint hash = 2166136261u;
        foreach (char c in text)
        {
            hash ^= (uint)c;
            hash *= 16777619u;
        }
        uint result = hash & 0x00FFFFFFu;
        return result != 0 ? result : 1u;
    }

    /// Returns true if the field belongs to a hot-update assembly.
    private bool IsHotUpdateField(string subjectId)
    {
        if (_hotUpdateAssemblyNames.Count == 0) return false;
        var slashIdx = subjectId.IndexOf('/');
        if (slashIdx <= 0) return false;
        var assemblyName = subjectId.Substring(0, slashIdx);
        return _hotUpdateAssemblyNames.Contains(assemblyName);
    }

    /// Compute (assembly_hash, field_hash) for use with static_var_store_read/write_field.
    private (uint AssemblyHash, uint FieldHash) GetHotUpdateFieldHashes(string subjectId)
    {
        var assemblyName = GetAssemblyNameFromSubjectId(subjectId);
        return (ComputeFNVHash(assemblyName), ComputeFNVHash(subjectId));
    }

	private static readonly System.Collections.Concurrent.ConcurrentDictionary<string, string> _cppStringLiteralCache =
		new System.Collections.Concurrent.ConcurrentDictionary<string, string>(StringComparer.Ordinal);

	private static readonly System.Collections.Concurrent.ConcurrentDictionary<string, string> _sanitizedSubjectIdCache =
		new System.Collections.Concurrent.ConcurrentDictionary<string, string>(StringComparer.Ordinal);

	private static readonly System.Collections.Concurrent.ConcurrentDictionary<string, string> _pseudoMetadataHandleCache =
		new System.Collections.Concurrent.ConcurrentDictionary<string, string>(StringComparer.Ordinal);

	// Tracks allocated pseudo-metadata handles to detect FNV-1a hash collisions.
	// With ~10000 type entries in 25-bit hash space, collision probability is ~78%.
	// When a collision is detected, the hash is incremented until a free slot is found.
	private static readonly System.Collections.Concurrent.ConcurrentDictionary<uint, string> _usedPseudoMetadataHandles =
		new System.Collections.Concurrent.ConcurrentDictionary<uint, string>();


    private IReadOnlyList<IGrouping<string, AotCoreIrMethodArtifact>> _methodsGroupedByDeclaringType =
        Array.Empty<IGrouping<string, AotCoreIrMethodArtifact>>();

    private IReadOnlyList<AotCoreIrMethodArtifact> _genericStaticMethodCandidates =
        Array.Empty<AotCoreIrMethodArtifact>();

    // ── Pre-built O(1) indexes for invocation planning ──
    private Dictionary<string, AotCoreIrMethodArtifact> _asyncMoveNextMethods =
        new(StringComparer.Ordinal);
    private HashSet<string> _allDeclaringTypeSubjectIds =
        new(StringComparer.Ordinal);
    private Dictionary<string, List<AotCoreIrMethodArtifact>> _methodsByDeclaringType =
        new(StringComparer.Ordinal);

    private IReadOnlyDictionary<string, int>? _vtableSlotMap;
    private IReadOnlyDictionary<string, int>? _vtableLengths;
    private IReadOnlySet<string>? _vtableTypes;
    private IReadOnlySet<string>? _interfaceTypeSubjectIds;
    private IReadOnlySet<string>? _sealedTypeSubjectIds;
    private HashSet<string> _typesWithInstanceMethods = new(StringComparer.Ordinal);

    // ── COM interface vtable data for CCW registration ──
    // Collected from AotCoreIrReferenceArtifact.ComInterfaceGuid during Create().
    // Key: type subject ID, Value: COM interface vtable info with GUID and method count.
    private Dictionary<string, ComInterfaceVtableInfo> _comInterfaceVtableData = new(StringComparer.Ordinal);

    internal sealed record ComInterfaceMethodSlot(uint Token, string NativeSymbol, string MethodName, int ParamCount);
    internal sealed record ComInterfaceVtableInfo(
        string Guid,
        ulong StableId,
        ComInterfaceMethodSlot[] Methods,
        bool IsDispatch = false);

    // ── VTable descriptor data for BootstrapRuntime registration ──
    private sealed record VTableSlotEntry(uint MethodToken, string NativeSymbol);
    private sealed record VTableDescriptorData(
        ulong StableId,
        string TypeTokenLiteral,
        string BaseTokenLiteral,
        VTableSlotEntry[] Slots,
        string VTableArraySymbol,
        int VTableLength,
        byte TypeShape,
        string? IfaceMapSymbol,
        int IfaceCount,
        string SanitizedId);
    private List<VTableDescriptorData>? _vtableDescriptors;

    // ── A4-Dual+V2 Header kind / vtable variant decision engine ──
    /// <summary>
    /// Determines the ObjectHeader kind for a given type. PureType = no sync (value types,
    /// boxed primitives, sealed types with no finalizer). ThinLockable = default for reference
    /// types (unified 16B header; virtual dispatch via type_info->vtable_array).
    /// </summary>
    private enum HeaderKind { PureType, ThinLockable }

    /// <summary>
    /// VTable variant: V0 (no vtable — interfaces, pure value types in V0 emission),
    /// V1 (indirect — vtable_array points to external VTable_symbol[]),
    /// V2 (inline — TypeInfoV2 with inline_slots[6] for ≤6 virtual methods).
    /// </summary>
    private enum VTableVariant { V0, V1, V2 }

    private HeaderKind GetHeaderKind(string typeSubjectId)
    {
        // String: ThinLockable (supports sync via lock())
        if (typeSubjectId.Contains("/System.String"))
            return HeaderKind.ThinLockable;

        // Value types → PureType (no sync needed)
        if (_valueTypeSubjectIds.Contains(typeSubjectId))
            return HeaderKind.PureType;

        // Interface types → PureType
        if (_interfaceTypeSubjectIds?.Contains(typeSubjectId) == true)
            return HeaderKind.PureType;

        // Sealed types with no virtual methods → PureType
        if (_sealedTypeSubjectIds?.Contains(typeSubjectId) == true &&
            !_typesWithInstanceMethods.Contains(typeSubjectId))
        {
            return HeaderKind.PureType;
        }

        // Everything else → ThinLockable (default, supports sync + vtable dispatch via type_info)
        return HeaderKind.ThinLockable;
    }

    private VTableVariant GetVTableVariant(string typeSubjectId, int vtableLength)
    {
        if (vtableLength == 0)
            return VTableVariant.V0;
        if (vtableLength <= 6)
            return VTableVariant.V2;
        return VTableVariant.V1;
    }

    // Phase 0: ModuleRegistry Tier 0 type data (populated after EmitObjectModelDeclarations)
    private HashSet<string>? _allEmittedTypeSubjectIds;
    private int _moduleTypeCount;
    private readonly List<uint> _moduleTypeFlags = new();
    private readonly List<string> _moduleTypeNames = new();
    private readonly List<string> _moduleTypeNamespaces = new();
    private readonly List<uint> _moduleTypeParentTokens = new();
    private readonly List<string?> _moduleTypeInfoSymbols = new();
    private readonly List<uint> _moduleNestedTypeChildren = new();
    private readonly List<uint> _moduleNestedTypeOffsets = new();
    private readonly List<uint> _moduleGenericParamConstraintData = new();
    private readonly List<uint> _moduleGenericParamConstraintOffsets = new();
    private readonly List<string> _moduleTypeSubjectIds = new();
    private bool _hasCustomAttributeBlob;

    /// <summary>
    /// Static field declarations (subjectId → fieldTypeSubjectId) captured during
    /// EmitObjectModelDeclarations for extern declarations in the shared header.
    /// </summary>
    private Dictionary<string, string?>? _staticFieldDeclarations;

    /// <summary>
    /// Value type subject IDs captured during EmitObjectModelDeclarations for
    /// chaos_valuetype_* forward declarations in the shared header.
    /// </summary>
    private HashSet<string>? _emittedValueTypeSubjectIds;

    /// <summary>
    /// Full C++ struct body code for value types that have fields or _backing
    /// members, captured during EmitObjectModelDeclarations. Used by
    /// BuildTypeDeclarationsCode to emit struct definitions in the shared header
    /// instead of bare typedefs, avoiding C2556/C2371 when the page file's object
    /// model section also defines these types.
    /// Null when no value type has a non-trivial struct body.
    /// </summary>
    private string? _valueTypeStructCode;

    /// <summary>
    /// Subject IDs of value types that have struct definitions in _valueTypeStructCode.
    /// Types NOT in this set should use typedef CHAOS_IL2CPP_INT32 in the shared header
    /// (they are enum-like types with no fields).
    /// </summary>
    private HashSet<string>? _valueTypeStructSubjectIds;

    /// <summary>
    /// Per-type field lists captured during EmitObjectModelDeclarations. Used by
    /// BuildTypeDeclarationsCode to emit complete reference type struct definitions
    /// in the shared header, avoiding C2027 when non-page-0 TUs allocate types.
    /// </summary>
    private Dictionary<string, List<string>>? _fieldsByDeclaringType;

    /// <summary>
    /// Field type map captured during EmitObjectModelDeclarations. Maps each field
    /// subject ID to its type subject ID, used for C++ type emission in struct defs.
    /// </summary>
    private Dictionary<string, string?>? _fieldTypeMap;

    /// <summary>
    /// Boxed type subject IDs captured during EmitObjectModelDeclarations. These are
    /// types for which chaos_boxed_type_* struct definitions are needed in the shared
    /// header (value types that appear as boxed heap allocations in managed code).
    /// </summary>
    private HashSet<string>? _boxedTypeSubjectIds;

    /// <summary>
    /// Complete reference type struct definitions captured during EmitObjectModelDeclarations
    /// via the StringBuilder checkpoint technique. These go to the shared header instead of
    /// page 0's object model to avoid C2027 on non-page-0 TUs.
    /// </summary>
    private string? _referenceTypeStructCode;

    /// <summary>
    /// Complete boxed type struct definitions captured during EmitObjectModelDeclarations
    /// via the StringBuilder checkpoint technique. These go to the shared header instead of
    /// page 0's object model to avoid C2027 on non-page-0 TUs.
    /// </summary>
    private string? _boxedTypeStructCode;

    /// <summary>
    /// External runtime helper definitions captured during Create, used by
    /// BuildTypeDeclarationsCode to emit extern declarations in the shared header.
    /// </summary>
    private IReadOnlyList<ExternalRuntimeHelperDefinition>? _externalRuntimeHelpers;

    /// <summary>
    /// Method declarations captured during BuildingPlanToTemplateModel, emitted into the
    /// shared header so page-split TUs can call methods across pages with correct signatures.
    /// Only set when TU paging is active (multiple page files exist).
    /// </summary>
    private IReadOnlyList<string>? _methodDeclarations;

    private readonly RuntimeHelperShapeRegistry _shapeRegistry = RuntimeHelperShapeRegistry.BuildDefault();

    /// <summary>
    /// Assembly name for the current AOT module. Used for cross-module call detection.
    /// </summary>
    private string _assemblyName = string.Empty;

    /// <summary>
    /// Maps method SubjectId → method_table index for cross-module calls.
    /// </summary>
    private readonly Dictionary<string, uint> _methodTableIndices = new(StringComparer.Ordinal);

    /// <summary>
    /// Next available method table index (sequential allocation).
    /// </summary>
    private uint _nextMethodTableIndex;

    /// <summary>
    /// Collected method table entries for initialization code generation.
    /// </summary>
    private readonly List<(uint Index, string NativeSymbol)> _methodTableEntries = new();
    private readonly List<(string SubjectId, string NativeSymbol)> _reversePInvokeEntries = new();

    /// <summary>
    /// Maps method native symbol → index in reachableMethods (used for ABI manifest origin).
    /// </summary>
    private Dictionary<string, int> _methodNativeSymbolToManifestIndex = new();

    /// <summary>
    /// Maps method native symbol → slot index in s_hotpatch_entries (Hotpatch).
    /// Populated by <see cref="BuildDispatchSlotMap"/> before method body emission.
    /// </summary>
    private Dictionary<string, int>? _nativeSymbolToDispatchSlot;

    /// <summary>
    /// Entry function code (RunNativeAot) to emit only in the first translation unit page.
    /// Populated for both generic-managed-entry and full-assembly-entry plans.
    /// </summary>
    private string? _entryFunctionCode;

    /// <summary>
    /// Native symbol of the method currently being emitted. Set/reset in
    /// <see cref="EmitManagedMethod"/> to detect self-calls that would cause
    /// infinite recursion (the codegen may collapse an unloverable IL body to
    /// a single "call self; ret" sequence).
    /// </summary>
    private string? _currentMethodNativeSymbol;

    /// <summary>Current method artifact, used by inlining budget checks.</summary>
    private AotCoreIrMethodArtifact? _currentMethodArtifact;

    /// <summary>
    /// Module-local symbol table: subjectId → nativeSymbol for all methods in the
    /// current codegen output.  Enables <see cref="EmitLinearCall"/> to detect
    /// same-module callees and emit direct C++ calls instead of going through the
    /// extern runtime dispatch table.
    /// Populated from <see cref="_methodsBySubjectId"/> during Create.
    /// </summary>
    private Dictionary<string, string> _moduleSymbolTable =
        new(StringComparer.Ordinal);

    /// <summary>
    /// Reverse of <see cref="_moduleSymbolTable"/>: nativeSymbol → subjectId.
    /// Used by inlining to resolve the callee SubjectId from InvocationTarget.TargetSymbol
    /// when the instruction-level metadata (Callee, TargetReference) is null due to
    /// lowering-time devirtualization consuming the original callee reference.
    /// </summary>
    private Dictionary<string, string> _nativeSymbolToSubjectId =
        new(StringComparer.Ordinal);

    // ══════════════════════════════════════════════════════════════════════════════════════════
    // Structured recovery metrics (instance-level, collected per codegen run)
    // ══════════════════════════════════════════════════════════════════════════════════════════

    internal int StructuredMethodCount;
    internal int StructuredExceptionBodyCount;
    internal int TotalMethodCount;

    /// <summary>Number of methods reachable from the entry point via AOT call graph.</summary>
    internal int AotReachableMethodCount;

    /// <summary>Number of methods not reachable from the entry point.</summary>
    internal int AotUnreachableMethodCount;

    /// <summary>Number of hotpatch dispatch entries emitted.</summary>
    internal int HotpatchEntryCount;

    /// <summary>
    /// Total methods eligible for hotpatch dispatch: those with IL bodies or P/Invoke,
    /// deduplicated by NativeSymbol.
    /// </summary>
    internal int HotpatchEligibleMethodCount;

    /// <summary>
    /// Number of methods that required pc-dispatch state machine emission
    /// due to irreducible CFG (after interval analysis + node splitting).
    /// </summary>
    internal int PcDispatchCount;
    internal int CodegenFailureCount;
    internal Dictionary<string, int> CodegenFailureByType = new();
    internal Dictionary<string, int> CodegenFailureByChunk = new();

    /// <summary>
    /// Maps unresolvable cross-assembly subjectId → index in kChaosExternalRuntimeFnTable.
    /// Populated by <see cref="PrebuildExternalRuntimeDispatchTable"/> before method body emission.
    /// </summary>
    private readonly Dictionary<string, int> _externalRuntimeSubjects = new(StringComparer.Ordinal);

    /// <summary>
    /// Bridge/import thunks: C++ wrapper functions for calls crossing the managed/native
    /// boundary. Populated by <see cref="CollectBridgeImportThunks"/> and emitted after
    /// all method bodies in Create.
    /// </summary>
    private Dictionary<string, BridgeImportThunkDefinition>? _bridgeImportThunks;

    public NativeAotTemplateModel Create(
        NativeAotLoweringPlanArtifact loweringPlan,
        AotCoreIrArtifact aotCoreIr,
        AotCoreIrMethodArtifact entryMethod,
        ManagedClosureManifestArtifact closureManifest,
        MetadataRegistrationArtifact metadataRegistration,
        SupplementalMetadataTemplateArtifact supplementalMetadataTemplate,
        bool fullAssemblyMode = false,
        CodegenMode mode = CodegenMode.Aot,
        HashSet<string>? subjectMethods = null)
    {
        ArgumentNullException.ThrowIfNull(loweringPlan);
        ArgumentNullException.ThrowIfNull(aotCoreIr);
        ArgumentNullException.ThrowIfNull(closureManifest);
        ArgumentNullException.ThrowIfNull(metadataRegistration);
        ArgumentNullException.ThrowIfNull(supplementalMetadataTemplate);

        if (!fullAssemblyMode)
            ArgumentNullException.ThrowIfNull(entryMethod);

        // Clear per-run collision tracking for pseudo-metadata handles.
        // Each codegen invocation (chunk) starts with a clean hash space.
        _usedPseudoMetadataHandles.Clear();

        _codegenMode = mode;
        _subjectMethodSubjectIds = subjectMethods;

        // Skip entry ABI validation for full-closure assembly translation
        if (!closureManifest.FullAssemblyClosure)
            ValidateEntryMethod(entryMethod);
        if (!fullAssemblyMode &&
            !string.Equals(entryMethod.NativeSymbol, loweringPlan.EntrySymbol, StringComparison.Ordinal))
        {
            throw new InvalidOperationException(
                $"native-aot entry symbol '{loweringPlan.EntrySymbol}' does not match aot-core-ir symbol '{entryMethod.NativeSymbol}'");
        }

        _methodsBySubjectId = aotCoreIr.Methods.ToDictionary(method => method.SubjectId, StringComparer.Ordinal);
        // Build index of types with at least one instance method (O(m) once, then O(1) per type lookup).
        foreach (var method in aotCoreIr.Methods)
        {
            if (!method.IsStatic && method.Identity.DeclaringTypeSubjectId is { } dt)
                _typesWithInstanceMethods.Add(dt);

            // Scan for instantiated types: newobj, box, constrained. prefixes
            foreach (var instr in method.Instructions)
            {
                if (instr.Op == "newobj" || instr.Op == "box")
                {
                    var callee = instr.Callee ?? instr.TargetReference?.SubjectId;
                    if (callee is { Length: > 0 })
                    {
                        var colonIdx = callee.IndexOf("::", StringComparison.Ordinal);
                        if (colonIdx > 0)
                            _instantiatedTypeSubjectIds.Add(callee.Substring(0, colonIdx));
                    }
                }
                if (instr.Op == "callvirt" && instr.ConstrainedTypeSubjectId is { Length: > 0 } ct)
                    _instantiatedTypeSubjectIds.Add(ct);
            }
        }
        _assemblyName = loweringPlan.AssemblyName;

        // Build module-local symbol table: all methods in this codegen output
        // that belong to the current assembly get a subjectId → nativeSymbol
        // entry so EmitLinearCall can detect same-module callees and emit
        // direct C++ calls instead of routing through the extern table.
        _moduleSymbolTable = _methodsBySubjectId
            .Where(kvp => IsSameModuleMethod(kvp.Key))
            .ToDictionary(kvp => kvp.Key, kvp => kvp.Value.NativeSymbol, StringComparer.Ordinal);

        // Build reverse symbol table for inlining SubjectId resolution.
        _nativeSymbolToSubjectId = _moduleSymbolTable
            .ToDictionary(kvp => kvp.Value, kvp => kvp.Key, StringComparer.Ordinal);

        _attributeStorageFieldIndex = BuildAttributeStorageFieldIndex(_methodsBySubjectId);
        _referenceTypeBaseSubjectIds = CollectReferenceTypeBaseSubjectIds(aotCoreIr);
        _referenceTypeImplementedInterfaceSubjectIds = CollectReferenceTypeImplementedInterfaceSubjectIds(aotCoreIr);
        _valueTypeSubjectIds = CollectValueTypeSubjectIds(aotCoreIr);
        _sealedTypeSubjectIds = CollectSealedTypeSubjectIds(aotCoreIr);
        _interfaceTypeSubjectIds = CollectInterfaceTypeSubjectIds(aotCoreIr);
        _comInterfaceVtableData = CollectComInterfaceVtableData(aotCoreIr, _methodsBySubjectId, metadataRegistration);

        // Build generic sharing canonical map: group generic methods by open
        // definition and determine which reference-type instantiations can share
        // a single canonical body.
        _genericSharingCanonicalMap = BuildGenericSharingCanonicalMap(
            _methodsBySubjectId, _valueTypeSubjectIds);

        // Apply global sharing directive from the lowering plan (if present).
        // Global entries take priority: they ensure consistent canonical selection
        // across compilation units that process different subsets of the same codebase.
        if (loweringPlan.GlobalSharingCanonicalMap is { Count: > 0 } globalMap)
        {
            foreach (var (nonCanonicalSubjectId, canonicalSubjectId) in globalMap)
            {
                if (_methodsBySubjectId.TryGetValue(canonicalSubjectId, out var canonicalMethod) &&
                    _methodsBySubjectId.TryGetValue(nonCanonicalSubjectId, out _))
                {
                    _genericSharingCanonicalMap[nonCanonicalSubjectId] = canonicalMethod.NativeSymbol;
                }
            }
        }

        // Build the set of canonical NativeSymbols that need the hidden
        // chaos_generic_context parameter for runtime type resolution.
        _sharedContextSymbols = BuildSharedContextSymbols();

        var methodsForLowering = fullAssemblyMode
            ? CollectAllMethods(aotCoreIr)
            : CollectReachableMethods(aotCoreIr, entryMethod);

        // Ensure consistent ordering by numeric subject suffix where present,
        // so that RunNativeAot(slot) → Subject_{slot} is correct for all
        // dispatch paths (native + hotpatch). Without this sort, slot numbers
        // are assigned in call-graph DFS order (fullAssemblyMode=false) or
        // alphabetical order (fullAssemblyMode=true), both of which produce
        // Subject_10 before Subject_1.
        methodsForLowering = methodsForLowering
            .OrderBy(m => ExtractNumericSortKey(m.SubjectId))
            .ThenBy(m => m.SubjectId, StringComparer.Ordinal)
            .ToList();

        // Pre-compute stubNeedsContext: for each method, determine if its
        // instantiation stub needs the chaos_generic_context parameter.  Uses
        // union semantics: if ANY method sharing a stub needs context, the stub
        // gets context.  This avoids C2733 (extern "C" cannot be overloaded).
        _stubNeedsContext.Clear();
        foreach (var m in methodsForLowering)
        {
            var stub = TryGetInstantiationStubSymbol(m);
            if (string.IsNullOrEmpty(stub)) continue;
            // Stub needs context if: (1) its native symbol is a shared canonical body,
            // or (2) the method is a non-canonical shared instantiation that forwards
            // to the canonical body via the generic sharing map.
            if (_sharedContextSymbols.Contains(m.NativeSymbol) ||
                _genericSharingCanonicalMap.ContainsKey(m.SubjectId))
                _stubNeedsContext[stub] = true;
            else if (!_stubNeedsContext.ContainsKey(stub))
                _stubNeedsContext[stub] = false;
        }

        // Also add stub symbols that need context into _sharedContextSymbols so
        // call site emission (EmitLinearResolvedInvocation etc.) correctly passes
        // chaos_generic_context when calling stub definitions.
        foreach (var kvp in _stubNeedsContext)
        {
            if (kvp.Value)
                _sharedContextSymbols.Add(kvp.Key);
        }

        // Augment lowering set with methods of types that implement COM interfaces.
        // These concrete implementations are referenced by vtable arrays for CCW dispatch
        // but may not be discovered by static call-graph reachability (interface dispatch).
        if (!fullAssemblyMode && _referenceTypeImplementedInterfaceSubjectIds?.Count > 0)
        {
            var existingIds = new HashSet<string>(methodsForLowering.Select(m => m.SubjectId), StringComparer.Ordinal);
            var extraMethods = new List<AotCoreIrMethodArtifact>();

            foreach (var kvp in _referenceTypeImplementedInterfaceSubjectIds)
            {
                var typeId = kvp.Key;
                foreach (var method in _methodsBySubjectId.Values)
                {
                    if (string.Equals(method.Identity.DeclaringTypeSubjectId, typeId, StringComparison.Ordinal)
                        && !existingIds.Contains(method.SubjectId))
                    {
                        extraMethods.Add(method);
                        existingIds.Add(method.SubjectId);
                    }
                }
            }

            if (extraMethods.Count > 0)
            {
                extraMethods.Sort((a, b) => string.Compare(a.SubjectId, b.SubjectId, StringComparison.Ordinal));
                methodsForLowering = methodsForLowering.Concat(extraMethods).ToList();
            }
        }
        _methodNativeSymbolToManifestIndex = methodsForLowering
            .Select((method, idx) => (method.NativeSymbol, idx))
            .DistinctBy(t => t.NativeSymbol)
            .ToDictionary(t => t.NativeSymbol, t => t.idx);
        _nativeSymbolToDispatchSlot = BuildDispatchSlotMap(methodsForLowering, metadataRegistration);
        var stringLiterals = CollectStringLiterals(methodsForLowering);

        // Compute hotpatch coverage statistics — eligible methods with IL bodies or P/Invoke,
        // deduplicated by NativeSymbol (matching GetHotpatchableMethods() logic).
        var hotpatchEligibleSymbols = new HashSet<string>(StringComparer.Ordinal);
        HotpatchEligibleMethodCount = _methodsBySubjectId.Values
            .Where(m => m.Instructions.Count > 0 || m.IsPInvoke)
            .Count(m => hotpatchEligibleSymbols.Add(m.NativeSymbol));

        // Compute AOT call-graph reachability from the entry point method.
        // Only methods reachable via call/callvirt/newobj/ldftn instructions
        // from the entry point need full C++ function bodies.
        var aotReachableSubjectIds = ComputeAotReachableSubjectIds(
            entryMethod?.SubjectId, methodsForLowering);

        // In full-assembly mode (no entry point), all methods are AOT-reachable
        // since any method may be invoked via the RunNativeAot dispatch table
        // by the runtime harness (entry.exe loop over kAotMethodCount).
        // Skip abstract/interface methods with 0 instructions — they have no
        // IL body to translate and get unreachable stubs instead.
        if (fullAssemblyMode)
        {
            foreach (var m in methodsForLowering)
            {
                if (m.Instructions.Count > 0)
                    aotReachableSubjectIds.Add(m.SubjectId);
            }
        }

        // Force-include subject methods: these are callable from the C++ dispatch
        // table (kMethodTable[]) even though the managed call graph from the entry
        // point may not reach them (each Subject_N is an independent test method).
        if (_subjectMethodSubjectIds is { Count: > 0 })
        {
            int forcedCount = 0;
            foreach (var m in methodsForLowering)
            {
                if (_subjectMethodSubjectIds.Contains(m.SubjectId) && m.Instructions.Count > 0)
                {
                    if (aotReachableSubjectIds.Add(m.SubjectId))
                        forcedCount++;
                }
            }
            if (forcedCount > 0)
            {
                Console.WriteLine($"[subject-methods] force-included {forcedCount} dispatch-visible method(s) into AOT reachable set");
            }
        }

        // Methods of types that implement COM interfaces are referenced by vtable
        // entries and need real bodies even if not statically reachable via call graph.
        if (_referenceTypeImplementedInterfaceSubjectIds?.Count > 0)
        {
            var comImplTypes = new HashSet<string>(_referenceTypeImplementedInterfaceSubjectIds.Keys, StringComparer.Ordinal);
            foreach (var sid in methodsForLowering.Select(m => m.SubjectId))
            {
                if (!aotReachableSubjectIds.Contains(sid))
                {
                    // Find the declaring type for this method
                    var slashIdx = sid.LastIndexOf('/');
                    var colonIdx = sid.IndexOf("::", StringComparison.Ordinal);
                    if (slashIdx > 0 && colonIdx > slashIdx)
                    {
                        var declTypeId = sid.Substring(0, colonIdx);
                        if (comImplTypes.Contains(declTypeId))
                        {
                            aotReachableSubjectIds.Add(sid);
                        }
                    }
                }
            }
        }

        AotReachableMethodCount = aotReachableSubjectIds.Count;
        // Only count unreachable among the methods we're actually emitting full bodies for.
        var loweringSubjectIds = new HashSet<string>(methodsForLowering.Select(m => m.SubjectId), StringComparer.Ordinal);
        AotUnreachableMethodCount = loweringSubjectIds.Count(sid => !aotReachableSubjectIds.Contains(sid));
        _stringIdMapping = BuildStringIdMapping(stringLiterals);
        _cachedClosureAssemblyPaths = EnumerateClosureAssemblyPaths(closureManifest).ToArray();
        _closureAssemblyPathByName = BuildClosureAssemblyPathByNameCore(_cachedClosureAssemblyPaths);
        _hotUpdateAssemblyNames = closureManifest.HotUpdateAssemblyNames
            ?? new HashSet<string>(StringComparer.Ordinal);
        _methodsGroupedByDeclaringType = _methodsBySubjectId.Values
            .Where(m => !string.IsNullOrWhiteSpace(m.Identity.DeclaringTypeSubjectId))
            .OrderBy(m => m.Identity.DeclaringTypeSubjectId, StringComparer.Ordinal)
            .ThenBy(m => m.SubjectId, StringComparer.Ordinal)
            .GroupBy(m => m.Identity.DeclaringTypeSubjectId, StringComparer.Ordinal)
            .ToArray();
        _genericStaticMethodCandidates = _methodsBySubjectId.Values
            .Where(c => c.IsStatic && c.ParameterCount == 1 && c.ReturnAbi.CarrierKindCode == AotCoreIrAbiCarrierKind.NativeInt && c.ParameterAbis.Count == 1 && c.ParameterAbis[0].CarrierKindCode == AotCoreIrAbiCarrierKind.NativeInt && c.SubjectId.Contains("!!0", StringComparison.Ordinal))
            .OrderBy(c => c.SubjectId, StringComparer.Ordinal)
            .ToArray();

        // ── Build pre-computed O(1) indexes for invocation planning ──
        _asyncMoveNextMethods = BuildAsyncMoveNextIndex(_methodsBySubjectId);
        _allDeclaringTypeSubjectIds = BuildAllDeclaringTypeSubjectIds(
            _methodsBySubjectId, _referenceTypeBaseSubjectIds, _referenceTypeImplementedInterfaceSubjectIds);
        _methodsByDeclaringType = BuildMethodsByDeclaringTypeIndex(_methodsBySubjectId);
        CustomAttributeSupportModel? customAttributeSupport = null;
        AssemblyReflectionSupportModel? assemblyReflectionSupport = null;
        ReflectionMemberSupportModel? reflectionMemberSupport = null;
        StaticFieldDataSupportModel? staticFieldDataSupport = null;
        var _sw = Stopwatch.StartNew();
        long _tPhase0 = 0, _tPhase1 = 0, _tPhase2 = 0, _tPhase3 = 0, _tPhase4 = 0, _tPhase5 = 0;
        long _tEnumCollect = 0, _tEnumMap = 0, _tStaticInit = 0, _tExtHelpers = 0, _tExtDispatch = 0, _tBridgeThunks = 0;

        _tPhase0 = _sw.ElapsedMilliseconds;
        // NOTE: Sequential model building to avoid heap corruption (0xC000037D)
        // observed with Parallel.Invoke on large method sets (51718 methods).
        // TODO: Investigate root cause and re-enable parallel once identified.
        customAttributeSupport = BuildCustomAttributeSupportModel(
            methodsForLowering,
            supplementalMetadataTemplate);
        assemblyReflectionSupport = BuildAssemblyReflectionSupportModel(
            methodsForLowering,
            supplementalMetadataTemplate);
        reflectionMemberSupport = BuildReflectionMemberSupportModel(
            methodsForLowering,
            supplementalMetadataTemplate);
        staticFieldDataSupport = BuildStaticFieldDataSupportModel(
            methodsForLowering,
            metadataRegistration);


        _customAttributeSupport = customAttributeSupport!;
        _assemblyReflectionSupport = assemblyReflectionSupport!;
        _reflectionMemberSupport = reflectionMemberSupport!;
        _staticFieldDataSupport = staticFieldDataSupport!;
        _tPhase1 = _sw.ElapsedMilliseconds;
        _enumTypeSubjectIds = CollectEnumTypeSubjectIds(_reflectionMemberSupport, _cachedClosureAssemblyPaths);
        _tEnumCollect = _sw.ElapsedMilliseconds;
        _enumValueToNameMap = BuildEnumValueToNameMap();
        _tEnumMap = _sw.ElapsedMilliseconds;
        _staticInitializationSupport = BuildStaticInitializationSupportModel(
            methodsForLowering,
            closureManifest);
        _tStaticInit = _sw.ElapsedMilliseconds;
        var externalRuntimeHelpers = CollectExternalRuntimeHelpers(methodsForLowering, _staticInitializationSupport);
        _tExtHelpers = _sw.ElapsedMilliseconds;
        _externalRuntimeHelpers = externalRuntimeHelpers;
        CollectExternalRuntimeDispatchEntries(methodsForLowering);
        _tExtDispatch = _sw.ElapsedMilliseconds;
        CollectBridgeImportThunks(methodsForLowering);
        _tBridgeThunks = _sw.ElapsedMilliseconds;
        _tPhase2 = _sw.ElapsedMilliseconds;
        var objectModelBuilder = new StringBuilder(65536);
        EmitRuntimePrelude(objectModelBuilder, externalRuntimeHelpers, _staticFieldDataSupport);
        EmitObjectModelDeclarations(objectModelBuilder, methodsForLowering, externalRuntimeHelpers, metadataRegistration);
        // Phase 0: Collect ModuleRegistry Tier 0 type data from PE metadata
        CollectModuleTypeData(closureManifest.InputAssemblyPath);
        // Phase 1 string-id table via Scriban
        var stringIdCode = BuildStringIdTable(stringLiterals);
        if (!string.IsNullOrEmpty(stringIdCode))
        {
            objectModelBuilder.AppendLine();
            objectModelBuilder.AppendLine(stringIdCode);
            objectModelBuilder.AppendLine();
        }
        if (externalRuntimeHelpers.Any(helper => IsSpanRuntimeHelperSubjectId(helper.SubjectId)))
        {
            EmitSpanRuntimePrelude(objectModelBuilder, _staticFieldDataSupport);
        }
        EmitDelegateRuntimeSupportDefinitions(objectModelBuilder, methodsForLowering, externalRuntimeHelpers);
        EmitExternalRuntimeHelperDefinitions(objectModelBuilder, externalRuntimeHelpers);
        EmitStaticInitializationDefinitions(objectModelBuilder);
        EmitGenericRegistration(objectModelBuilder, supplementalMetadataTemplate, metadataRegistration, out var genericRegistrationHelperCode, out var aotRegistrationCode);

        // A2.4: Pre-scan for ldsfld→Enum::ToString() constant-folding patterns
        BuildEnumToStringFoldTable(methodsForLowering);
        // A2.5: Pre-scan for Enum::Parse/GetName/Format/IsDefined with constant args
        BuildEnumAotBakeTable(methodsForLowering);

        // Emit file-scope _g_bake_cache_[] array for AOT-baked enum results.
        // MSVC C2712 forbids function-local static variables with dynamic
        // initializers inside __try/__except.  The array sits at namespace
        // scope so method bodies inside CHAOS_EH_TRY can reference it without
        // triggering the compiler error.
        _enumAotBakeCacheArraySize = _enumAotBakeMap.Values.Count(e => e.ConstantInt != null);
        _enumAotBakeCacheCount = 0;  // reset counter for sequential indexing
        if (_enumAotBakeCacheArraySize > 0)
        {
            objectModelBuilder.AppendLine();
            objectModelBuilder.AppendLine("// File-scope cache for AOT-baked enum box values");
            objectModelBuilder.AppendLine($"// (avoids MSVC C2712 from function-local static inside __try/__except)");
            objectModelBuilder.AppendLine($"static CHAOS_IL2CPP_INTPTR _g_bake_cache_[{_enumAotBakeCacheArraySize}] = {{}};");
            objectModelBuilder.AppendLine();
        }
        // A2.6: Pre-scan for typeof(T).IsAssignableFrom(typeof(U)) → *Ptr direct API
        BuildTypeHierarchyPtrFoldTable(methodsForLowering);
        // A2.7: Pre-scan for typeof(const_type) → direct TypeInfo* pointer
        BuildTypeOfFoldTable(methodsForLowering);
        _tPhase3 = _sw.ElapsedMilliseconds;

        var methodDeclarations = BuildMethodDeclarations(methodsForLowering, _sharedContextSymbols, _stubNeedsContext);
        _methodDeclarations = methodDeclarations;
        // ── Emit method bodies ──────────────────────────────────────────
        // Deduplicate by NativeSymbol to prevent C2084 (duplicate function body)
        // and C2733 (duplicate extern "C" symbol) when two managed methods map
        // to the same extern "C" symbol (e.g. generic instantiations that collapse
        // to the same ABI signature in subject-mode codegen).  Also skips entries
        // with empty NativeSymbol (no usable body to emit).
        IReadOnlyList<AotCoreIrMethodArtifact> emitMethods = methodsForLowering;
        {
            var seenNs = new HashSet<string>(StringComparer.Ordinal);
            var filtered = new List<AotCoreIrMethodArtifact>(methodsForLowering.Count);
            foreach (var m in methodsForLowering)
                if (!string.IsNullOrWhiteSpace(m.NativeSymbol) && seenNs.Add(m.NativeSymbol))
                    filtered.Add(m);
            if (filtered.Count < methodsForLowering.Count)
            {
                emitMethods = filtered;
                int subjectDropped = 0;
                var droppedSet = new HashSet<string>(methodsForLowering.Select(m => m.NativeSymbol!))
                    .Except(filtered.Select(m => m.NativeSymbol!));
                foreach (var m in methodsForLowering)
                    if (IsSubjectMethod(m.SubjectId) && !filtered.Contains(m))
                        subjectDropped++;
                if (subjectDropped > 0)
                    Console.Error.WriteLine($"[NS-DEDUP] Dropped {subjectDropped} subject method(s) of {methodsForLowering.Count - filtered.Count} total deduped");
            }
        }
        var allMethods = new List<NativeAotMethodTemplateModel>(emitMethods.Count);
        for (int i = 0; i < emitMethods.Count; i++)
            allMethods.Add(EmitOneMethod(emitMethods[i], aotReachableSubjectIds));
        List<NativeAotMethodTemplateModel> methods = allMethods;
        _tPhase4 = _sw.ElapsedMilliseconds;

        // Capture pc-dispatch count from the static counter.
        // Incremented during BuildMethodSourceSafe → EmitViaStructuredIR → EmitPcDispatch.
        PcDispatchCount = (int)Interlocked.Read(ref s_pcDispatchCount);


        var entryBridgeArguments = fullAssemblyMode ? "" : BuildEntryBridgeArguments(entryMethod);

        var abiManifestCode = BuildAbiManifest(methodsForLowering);
        var nameIndexCode = BuildHotpatchTable(methodsForLowering, metadataRegistration);
        var externalRuntimeTableCode = BuildExternalRuntimeDispatchTable(
            helperSymbolBySubjectId: externalRuntimeHelpers?
                .Where(h => !string.IsNullOrEmpty(h.TargetSymbol))
                .ToDictionary(h => h.SubjectId, h => h.TargetSymbol, StringComparer.Ordinal));
        var moduleRegistrationCode = BuildModuleRegistration();
        var moduleRegSb = new StringBuilder(moduleRegistrationCode, 65536);
        if (!string.IsNullOrEmpty(nameIndexCode))
        {
            moduleRegSb.Append(Environment.NewLine);
            moduleRegSb.Append(nameIndexCode);
        }
        if (!string.IsNullOrEmpty(externalRuntimeTableCode))
        {
            moduleRegSb.Append(Environment.NewLine);
            moduleRegSb.Append(externalRuntimeTableCode);
        }
        if (!string.IsNullOrEmpty(aotRegistrationCode))
        {
            moduleRegSb.Append(Environment.NewLine);
            moduleRegSb.Append(aotRegistrationCode);
        }
        if (!string.IsNullOrEmpty(abiManifestCode))
        {
            moduleRegSb.Insert(0, abiManifestCode + Environment.NewLine);
        }
        if (_methodTableEntries.Count > 0)
        {
            moduleRegSb.Append(BuildMethodTableInitialization());
        }

        // Step 1: Emit pure-data dispatch tables.
        // Produces kMethodTable[], kDefaultArgThunks[], kSubjectSlotMap[] as data-only arrays.
        // Dispatch routing (hotpatch check, interpreter fallback) lives in
        // <chaos/hotpatch_dispatch.h> (runtime library), not in generated code.
        var dispatchEntryCode = BuildDispatchEntryCode(methodsForLowering);
        if (!string.IsNullOrEmpty(dispatchEntryCode))
        {
            moduleRegSb.Append(Environment.NewLine);
            moduleRegSb.Append(dispatchEntryCode);
        }

        // Step 1.5: Emit GC slot map section for precise stack root scanning.
        // Placed BEFORE CodeRegistrationV0 so the slot_map_section_begin/end
        // symbols are defined before they're referenced.
        var gcSlotMapCode = BuildGcSlotMapSection(methodsForLowering);
        if (!string.IsNullOrEmpty(gcSlotMapCode))
        {
            moduleRegSb.Append(Environment.NewLine);
            moduleRegSb.Append(gcSlotMapCode);
        }

        // Step 1.75: Emit bridge/import thunks in the registration-only section so they
        // appear in only the first translation unit page, avoiding duplicate symbol errors.
        // These C++ wrapper functions handle GC transition and calling convention adaptation
        // for calls crossing the managed/native boundary.
        if (_bridgeImportThunks is { Count: > 0 })
        {
            moduleRegSb.Append(Environment.NewLine);
            moduleRegSb.Append("// ── Bridge/import thunks ──");
            moduleRegSb.Append(Environment.NewLine);
            if (_bridgeImportThunks.Values.Any(t => t.ExternalRuntimeTableIndex >= 0))
            {
                // Forward-declare the external runtime dispatch table. The actual definition
                // is emitted in BuildExternalRuntimeDispatchTable (above in Step 1).
                moduleRegSb.Append("extern \"C\" void* kChaosExternalRuntimeFnTable[];");
                moduleRegSb.Append(Environment.NewLine);
            }
            foreach (var thunk in _bridgeImportThunks.Values
                .OrderBy(t => t.ThunkSymbol, StringComparer.Ordinal))
            {
                EmitBridgeImportThunk(moduleRegSb, thunk);
            }
        }

        // Step 2: Emit CodeRegistrationV0, MetadataRegistrationV0, CodegenRegistrationOptionsV0
        // as extern "C" symbols for RegisterCodegen + BootstrapRuntime path.
        var codeRegistrationCode = EmitCodeRegistrationStructs(methodsForLowering, metadataRegistration,
            hasGcSlotMapSection: !string.IsNullOrEmpty(gcSlotMapCode) && !gcSlotMapCode.Contains("(empty)"));
        if (!string.IsNullOrEmpty(codeRegistrationCode))
        {
            moduleRegSb.Append(Environment.NewLine);
            moduleRegSb.Append(codeRegistrationCode);
        }

        // Step 3: Emit ReflectionQueryImageDescriptor for module.image,
        // enabling ResolveSubjectId to find call_target via reflection query model.
        // Build extern "C" kAotMethodCount at file scope for Python-generated runtime-entry.cpp link-time visibility.
        var methodCount = methodsForLowering.Count;

        var reflectionQueryCode = EmitReflectionQueryImage(methodsForLowering, metadataRegistration);
        if (!string.IsNullOrEmpty(reflectionQueryCode))
        {
            moduleRegSb.Append(Environment.NewLine);
            moduleRegSb.Append(reflectionQueryCode);
        }
        else if (methodCount > 0)
        {
            // When there are no reflection queries, emit a zero-initialized
            // kReflImage definition so the ModuleDescriptor's .image field
            // (which always references kReflImage) has a linkable symbol.
            // See also: reflImageForwardDecl below.
            moduleRegSb.Append(Environment.NewLine);
            moduleRegSb.Append("const ::chaos::il2cpp::runtime_core::ReflectionQueryImageDescriptor kReflImage = {};");
        }

        // Build extern "C" kAotMethodCount at file scope for Python-generated runtime-entry.cpp link-time visibility.
        var globalDeclarations = methodCount > 0
            ? $"// extern \"C\" definition for link-time visibility from runtime-entry.cpp\nextern \"C\" const int kAotMethodCount = {methodCount};\n"
            : string.Empty;

        // Always define ChaosJitRegisterAll so runtime-entry.cpp can call it unconditionally.
        // In JIT mode (guarded by CHAOS_IL2CPP_JIT_MODE), it registers all methods for
        // JIT dispatch via precode stubs.  In AOT mode it's always a no-op.
        // The #ifdef guard is critical: the same generated C++ code is compiled into
        // both entry.exe (AOT, no JIT_MODE) and entry-jit.exe (JIT, with JIT_MODE).
        // Without the guard, the AOT build would trigger JIT compilation through
        // RegisterJitEntryMethods → JitStubDispatchImpl → Compile() in chaos_jit.lib.
        if (_codegenMode.HasFlag(CodegenMode.Jit) && methodCount > 0)
        {
            globalDeclarations += "\n#ifdef CHAOS_IL2CPP_JIT_MODE\n";
            globalDeclarations += "\n" + BuildJitMethodRegistration(methodsForLowering, metadataRegistration);
            globalDeclarations += $@"
extern ""C"" const HotpatchModuleV0* chaos_il2cpp_aot_hotpatch_module;
extern ""C"" void ChaosJitRegisterAll() {{
    // Register hotpatch module so GetDispatchEntry can resolve tokens → slots
    chaos::il2cpp::runtime_core::RegisterHotpatchModule(chaos_il2cpp_aot_hotpatch_module);
    RegisterJitEntryMethods(kChaosJitEntries, kChaosJitEntryCount);
}}
#endif
";
        }
        else
        {
            globalDeclarations += @"
extern ""C"" void ChaosJitRegisterAll() {}
";
        }

        // Generate entry function (RunNativeAot) for all plan kinds that have
        // a callable workload entry point. The benchmark host main() calls
        // RunNativeAot(entryIndex) which delegates to InvokeWorkload.
        // We add a forward declaration at file scope because the entry symbol is
        // declared inside the codegen namespace (Scriban template) while the entry
        // function needs to be at file scope (outside the namespace) for
        // native_aot_main.cpp link-time visibility.
        // For generic-managed-entry plans, the lowering plan explicitly specifies
        // NativeEntryFunctionName and EntrySymbol. For full-assembly-entry plans,
        // we search the method list for InvokeWorkload as a well-known entry point.
        string? invokeWorkloadSymbol = null;
        if (loweringPlan.PlanKind == "full-assembly-entry" && methodCount > 0)
        {
            foreach (var method in methodsForLowering)
            {
                if (method.SubjectId.Contains("::InvokeWorkload:", StringComparison.Ordinal))
                {
                    invokeWorkloadSymbol = method.NativeSymbol;
                    break;
                }
            }
        }

        string? entryFunctionCode = null;
        if (loweringPlan.PlanKind == "generic-managed-entry"
            && !string.IsNullOrEmpty(loweringPlan.NativeEntryFunctionName)
            && !string.IsNullOrEmpty(loweringPlan.EntrySymbol)
            && methodCount > 0)
        {
            entryFunctionCode = $@"
// Forward declaration for entry symbol (defined in codegen namespace above).
extern ""C"" CHAOS_IL2CPP_INT32 {loweringPlan.EntrySymbol}(CHAOS_IL2CPP_INT32);
extern ""C"" CHAOS_IL2CPP_INT32 {loweringPlan.NativeEntryFunctionName}(CHAOS_IL2CPP_INT32 entryIndex) {{
    return {loweringPlan.EntrySymbol}(entryIndex);
}}
";
        }
        else if (invokeWorkloadSymbol is not null && methodCount > 0)
        {
            // Fallback for full-assembly-entry plans: generate RunNativeAot that
            // forwards to the InvokeWorkload method (found by SubjectId pattern).
            entryFunctionCode = $@"
// Forward declaration for InvokeWorkload (defined in codegen namespace below).
extern ""C"" CHAOS_IL2CPP_INT32 {invokeWorkloadSymbol}(CHAOS_IL2CPP_INT32);
extern ""C"" CHAOS_IL2CPP_INT32 RunNativeAot(CHAOS_IL2CPP_INT32 entryIndex) {{
    return {invokeWorkloadSymbol}(entryIndex);
}}
";
        }

        if (entryFunctionCode is not null)
        {
            // Store the entry function separately from globalDeclarations so that
            // NativeAotEmitter can emit it only in the first page (page-0001),
            // avoiding duplicate symbol errors across translation units.
            _entryFunctionCode = entryFunctionCode;
        }

        // Add kReflImage forward declaration BEFORE the ModuleDescriptor that references it.
        // The actual definition is emitted in Step 3 (EmitReflectionQueryImage) later in
        // moduleRegistrationCode, but the ModuleDescriptor (from BuildModuleRegistration)
        // needs the symbol visible at its point of definition.
        var reflImageForwardDecl = methodCount > 0
            ? "\n// Forward declaration for module.image (emitted below in Step 3)\nextern const ::chaos::il2cpp::runtime_core::ReflectionQueryImageDescriptor kReflImage;\n"
            : string.Empty;

        // Inject kReflImage forward declaration and kAotMethodCount namespace-extern into
        // moduleRegistrationCode BEFORE the ModuleDescriptor from BuildModuleRegistration.
        // This ensures symbols are declared before use within the codegen namespace.
        // The actual kReflImage definition is emitted in Step 3 below.
        var namespacePreamble = new StringBuilder();
        if (methodCount > 0)
        {
            namespacePreamble.AppendLine();
            namespacePreamble.AppendLine("// Forward declaration for module.image (defined in Step 3 below)");
            namespacePreamble.AppendLine("extern const ::chaos::il2cpp::runtime_core::ReflectionQueryImageDescriptor kReflImage;");
            namespacePreamble.AppendLine();
            namespacePreamble.AppendLine("// Namespace-scoped extern declaration for kAotMethodCount.");
            namespacePreamble.AppendLine("// (Definition at file scope via globalDeclarations for runtime-entry.cpp link-time visibility.)");
            namespacePreamble.AppendLine("extern \"C\" const int kAotMethodCount;");
        }
        moduleRegistrationCode = namespacePreamble.ToString() + moduleRegSb.ToString();

        // Phase 1 diagnostics: log StructuredIR coverage summary
        LogPhase1Summary();

        // Hotpatch dispatch coverage diagnostics
        LogHotpatchCoverage();

        // Compute enum metadata header (may be empty if no enum types found)
        var enumMetaHeader = GenerateEnumMetadataHeader();

        // If enum types exist, append an IIFE that registers the generated enum
        // metadata tables and dispatch table at static init time.
        // The header may be non-empty (e.g., "No enum types found" comment) without
        // defining ChaosRegisterEnumGeneratedMetadata, so check for the function name.
        if (!string.IsNullOrEmpty(enumMetaHeader) && enumMetaHeader.Contains("ChaosRegisterEnumGeneratedMetadata"))
        {
            moduleRegistrationCode += @"
    // Register generated enum metadata (tables + dispatch + type descriptors).
    // ChaosRegisterEnumGeneratedMetadata is defined in enum_metadata.generated.h.
    static const CHAOS_IL2CPP_UINT32 s_enum_registered = []() noexcept {
        ChaosRegisterEnumGeneratedMetadata();
        return 1u;
    }();
";
        }

        // Build A1 typed dispatch table header + A2 dispatch wiring source.
        // These are emitted as separate files (chaos_generated_module.h/.cpp) for
        // typed dispatch via ChaosRuntimeHost. Empty when methodsForLowering is empty.
        var moduleHeader = BuildGeneratedModuleHeader(methodsForLowering);
        var moduleSource = BuildGeneratedModuleSource(methodsForLowering);

        // Build include list — most are unconditional; feature-specific headers
        // (com_ccw.h, enum_stubs.h, enum_metadata.generated.h) are included only
        // when the generated code actually references those features.
        var includes_ = new List<string>
        {
            "<chaos/common.h>",
            "<chaos/type_info.h>",
            "\"runtime_core.h\"",
            // Unified exception-handling macros (CHAOS_EH_TRY / CHAOS_EH_CATCH_BEGIN / etc.)
            // Must appear after runtime_core.h which provides the EH backend type definitions.
            "<chaos/eh.h>",
            "\"codegen_bridge.h\"",
            "\"module_registry.h\"",
            "\"abi_manifest.h\"",
            "\"hotpatch_table.h\"",
            "\"runtime_vtable.h\"",
            "\"runtime_instantiation.h\"",
            "\"reflection_query_model.h\"",
            "\"load_store_chaos_bridge.h\"",
            // Interpreter dispatch for hotpatch-kept-native & flat-goto fallback
            "\"interpreter_entry.h\"",
            // GC write barrier (SATB pre-write barrier for reference-type field stores).
            "<gc/gc_bgc_inline.h>",
            // GC card table (post-write dirty card marking for generational GC).
            "<gc/gc_card_table.h>",
            // Common generated runtime prelude (shared header, ~200 lines
            // of helper functions previously emitted inline in every file).
            "<ChaosGeneratedRuntimePrelude.h>",
        };
        // com_ccw.h — only needed when COM interface vtable data is present.
        if (_comInterfaceVtableData is { Count: > 0 })
            includes_.Add("\"com_ccw.h\"");
        // Enum runtime stubs — only needed when there are enum types in the closure.
        if (_enumTypeSubjectIds is { Count: > 0 })
            includes_.Add("\"enum_stubs.h\"");
                                        // Async stubs (Task.Yield / YieldAwaitable) - always included;
                                        // the header is tiny (~15 lines) and the stubs are only
                                        // referenced when async yield methods are present.
                                        includes_.Add("\"async_stubs.h\"");
        // Exception stubs (ChaosInvokeAction) — needed by verification dispatch.
        // Always included; the header is tiny and the inline function is zero-cost.
        includes_.Add("\"exception_stubs.h\"");
        // Enum metadata header — only included when there are enum types in the closure.
        // Saves ~500 KB of C++ parsing per translation unit when no enums are present.
        if (!string.IsNullOrEmpty(enumMetaHeader))
            includes_.Add("\"enum_metadata.generated.h\"");
        // Hierarchy fast API — only needed when TypeInfo* ptr fold is active
        // (typeof(T).IsAssignableFrom(typeof(U)) → *PtrFast inlined calls).
        if (_typeHierarchyPtrFoldMap is { Count: > 0 })
            includes_.Add("\"reflection_api.h\"");
        // Native bridge headers (e.g., "convert.h") from external runtime
        // helpers that map to direct native function calls.
        includes_.AddRange(
            externalRuntimeHelpers
                .Select(h => h.DirectNativeHeader)
                .Where(h => !string.IsNullOrEmpty(h))
                .Distinct(StringComparer.Ordinal)
                .Select(h => h!));

        _tPhase5 = _sw.ElapsedMilliseconds;
        var gcMemBeforeToString = GC.GetTotalMemory(false);
        var wbBeforeToString = System.Diagnostics.Process.GetCurrentProcess().WorkingSet64;
        Console.Error.WriteLine(
            $"[T] TIMING:assemble={_tPhase0}ms|" +
            $"parallel_models={_tPhase1 - _tPhase0}ms|" +
            $"enum_collect={_tEnumCollect - _tPhase1}ms|" +
            $"enum_map={_tEnumMap - _tEnumCollect}ms|" +
            $"static_init={_tStaticInit - _tEnumMap}ms|" +
            $"ext_helpers={_tExtHelpers - _tStaticInit}ms|" +
            $"ext_dispatch={_tExtDispatch - _tExtHelpers}ms|" +
            $"bridge_thunks={_tBridgeThunks - _tExtDispatch}ms|" +
            $"post_parse_enum_helpers={_tPhase2 - _tPhase1}ms|" +
            $"object_model_emit={_tPhase3 - _tPhase2}ms|" +
            $"method_bodies={_tPhase4 - _tPhase3}ms|" +
            $"registration_dispatch={_tPhase5 - _tPhase4}ms|" +
            $"total={_tPhase5}ms|" +
            $"gc_before_tostring={gcMemBeforeToString / (1024 * 1024)}MB|" +
            $"ws_before_tostring={wbBeforeToString / (1024 * 1024)}MB");

        // ── Codegen failure summary ──
        if (CodegenFailureCount > 0)
        {
            Console.Error.WriteLine($"[CODGEN-FAIL] total={CodegenFailureCount} methods fell back to stubs");
            Console.Error.WriteLine("[CODGEN-FAIL] by exception type:");
            foreach (var kv in CodegenFailureByType.OrderByDescending(kv => kv.Value))
                Console.Error.WriteLine($"  {kv.Value,5}x {kv.Key}");
            Console.Error.WriteLine("[CODGEN-FAIL] by chunk (SubjectId prefix):");
            foreach (var kv in CodegenFailureByChunk.OrderByDescending(kv => kv.Value))
                Console.Error.WriteLine($"  {kv.Value,5}x {kv.Key}");
        }

        // Store the ObjectModelCode as either a string (for Scriban path) or as
        // a StringBuilder reference (for the direct builder path, avoiding a 3+ GB
        // ToString() allocation that causes OutOfMemoryException on Large Object Heap).
        const int ObjectModelCodeLargeThreshold = 200_000;
        bool isObjectModelLarge = objectModelBuilder.Length > ObjectModelCodeLargeThreshold;

        // For small ObjectModelCode, keep as string (Scriban compatibility).
        // For large (>200K chars), pass the StringBuilder reference to avoid a
        // 3+ GB string allocation that would OOM on the Large Object Heap.
        StringBuilder? objectModelCodeBuilder;
        string objectModelCode;
        if (isObjectModelLarge)
        {
            objectModelCodeBuilder = objectModelBuilder;
            objectModelCode = "";
            // NOTE: Intentionally NOT setting objectModelBuilder.Length = 0 here.
            // objectModelCodeBuilder and objectModelBuilder reference the SAME
            // StringBuilder instance. Clearing Length would destroy the content
            // that the emitter (NativeAotEmitter) reads via GetChunks(), causing
            // the entire object model section to vanish from generated output.
        }
        else
        {
            // TrimEnd is safe here — the string is under the threshold.
            objectModelCode = objectModelBuilder.ToString().TrimEnd();
            objectModelCodeBuilder = null;
            objectModelBuilder.Length = 0;
        }
        // NOTE: .TrimEnd() is intentionally omitted despite potential trailing
        // whitespace — it would allocate a second 3+ GB copy, doubling peak
        // memory at the worst possible moment.  Trailing whitespace in generated
        // C++ is harmless for the compiler.

        return new NativeAotTemplateModel
        {
            Includes = includes_,
            ObjectModelCode = objectModelCode,
            ObjectModelCodeBuilder = objectModelCodeBuilder,
            TypeDeclarationsCode = BuildTypeDeclarationsCode(SanitizeCppIdentifier(loweringPlan.AssemblyName)),
            GenericRegistrationCode = genericRegistrationHelperCode,
            MethodDeclarations = methodDeclarations,
            Methods = methods,
            EntrySubjectId = loweringPlan.EntrySubjectId,
            EntrySymbol = loweringPlan.EntrySymbol,
            EntryNativeSymbol = entryMethod.NativeSymbol,
            NativeEntryFunctionName = loweringPlan.NativeEntryFunctionName,
            EntryBridgeArguments = entryBridgeArguments,
            ShapeDispatchHeaderContent = _shapeRegistry.GenerateCppShapeHeader(),
            EnumMetadataHeaderContent = enumMetaHeader,
            ModuleRegistrationCode = moduleRegistrationCode,
            WorkloadAbi = loweringPlan.WorkloadAbi,
            GlobalDeclarations = globalDeclarations,
            EntryFunctionCode = _entryFunctionCode ?? "",
            ManifestJson = _manifestJson ?? "",
            CodegenNamespace = SanitizeCppIdentifier(loweringPlan.AssemblyName),
            GeneratedModuleHeaderContent = moduleHeader,
            GeneratedModuleSourceContent = moduleSource,
        };
    }

    private static string SanitizeCppIdentifier(string name)
    {
        // Replace non-alphanumeric characters (except underscore) with underscores
        // to produce a valid C++ identifier from an assembly name.
        var sb = new System.Text.StringBuilder(name.Length);
        foreach (char c in name)
        {
            sb.Append(char.IsLetterOrDigit(c) ? c : '_');
        }
        return sb.ToString();
    }

    /// <summary>
    /// Builds C++ declarations for the shared header (native-aot.generated.header.h).
    /// When TU paging is active, each translation unit page needs visibility to
    /// all type symbols, static variables, and runtime helpers defined on other
    /// pages without ODR violations. This method emits:
    ///   - struct chaos_type_&lt;id&gt;; forward declarations
    ///   - struct chaos_boxed_type_&lt;id&gt;; forward declarations
    ///   - struct chaos_valuetype_&lt;id&gt;; forward declarations
    ///   - extern MethodTable chaos_mt_&lt;id&gt;; (not TypeInfoV0 — MethodTable is the
    ///     actual definition type; &lt;chaos/type_info.h&gt; brings it into scope)
    ///   - extern "C" kChaosExternalRuntimeFnTable[]; (if bridge thunks use it)
    ///   - extern "C" HotpatchEntryV0 s_hotpatch_entries[]; (if dispatch slots exist)
    ///   - extern CHAOS_IL2CPP_INTPTR chaos_static_&lt;id&gt;; for each static field
    ///   - extern CHAOS_IL2CPP_INTPTR chaos_string_materialize(...); (if string IDs)
    ///   - extern bool chaos_is_array_store_compatible(...);
    ///   - extern CHAOS_IL2CPP_INTPTR chaos_default_interpolated_string_handler_*(...);
    ///     (if interpolated string helpers are reachable)
    ///   - extern declarations for all chaos_external_runtime_* helpers
    /// </summary>
    /// <param name="codegenNamespace">C++ namespace for the codegen (e.g. "CombinedSubjects").
    /// Declarations are wrapped in `namespace chaos::il2cpp::codegen::{codegenNamespace}` to
    /// match the definition namespace on page 0, avoiding linker unresolved externals.</param>
    private string BuildTypeDeclarationsCode(string codegenNamespace)
    {
        if (_allEmittedTypeSubjectIds is not { Count: > 0 })
            return string.Empty;

        var sb = new System.Text.StringBuilder();
        sb.AppendLine("// Auto-generated type declarations (TU paging)");
        sb.AppendLine("#pragma once");
        sb.AppendLine();
        sb.AppendLine("#include <chaos/native_types.h>");
        sb.AppendLine("#include <chaos/type_info.h>  // MethodTable, TypeInfoV0 (complete type)");
        sb.AppendLine("#include \"generated_code_compat.h\"  // PureTypeHeader for delegate type definitions");
        sb.AppendLine("#include \"ChaosGeneratedRuntimePrelude.h\"  // chaos_managed_array for array-compat checks");
        sb.AppendLine();

        // chaos_valuetype_* definitions — these are opaque 32-bit value types in the
        // managed ABI surface. Must come BEFORE boxed type struct definitions (below)
        // because boxed types contain "chaos_valuetype_X value{};" and require complete
        // value type structs.
        // 1. Types with fields or _backing: emit struct definitions (from
        //    _valueTypeStructCode) so sizeof and ABI are correct.
        // 2. Types without fields (pure enums): emit typedef CHAOS_IL2CPP_INT32
        //    for correct ABI (int32_t register passing, not empty-struct sizeof=1).
        // Structs are NOT re-emitted in page file object model, avoiding C2556/C2371.
        bool hasAnyForwardDeclarations = false;
        var vtCode = _valueTypeStructCode;
        if (vtCode is { Length: > 0 })
        {
            sb.Append(vtCode);
            hasAnyForwardDeclarations = true;
        }
        // Emit typedef for remaining value types (enum-like, no struct definition).
        if (_emittedValueTypeSubjectIds is { Count: > 0 })
        {
            HashSet<string>? structSubjectIds = _valueTypeStructSubjectIds;
            foreach (var typeId in _emittedValueTypeSubjectIds.OrderBy(id => id, StringComparer.Ordinal))
            {
                if (structSubjectIds?.Contains(typeId) == true)
                    continue; // already has struct definition above
                sb.Append("typedef CHAOS_IL2CPP_INT32 ");
                sb.Append(GetNativeValueTypeSymbol(typeId));
                sb.AppendLine(";");
            }
            hasAnyForwardDeclarations = true;
        }
        if (hasAnyForwardDeclarations)
            sb.AppendLine();

        // ── Reference type struct definitions (complete) ──
        // Emitted in the shared header so that non-page-0 TUs have complete types for
        // CHAOS_IL2CPP_NEW_GC (sizeof), field access via reinterpret_cast, etc.
        // Page 0 skips these struct definitions to avoid C2011 redefinition.
        // Including these BEFORE the forward declaration loop below — C++ allows
        // forward declarations after definitions, so the redundant forward decls
        // in the loop are harmless.
        if (_referenceTypeStructCode is { Length: > 0 })
        {
            sb.Append(_referenceTypeStructCode);
        }

        // ── Struct forward declarations ──
        // Page files use reinterpret_cast<chaos_type_<id>*>(ptr),
        // reinterpret_cast<chaos_boxed_type_<id>*>(ptr), and
        // CHAOS_IL2CPP_NEW_GC(chaos_boxed_type_<id>, ...).
        // For types in _boxedTypeSubjectIds we emit the complete struct definition
        // in the shared header so that CHAOS_IL2CPP_NEW_GC compiles in page files.
        // The object model section on page 0 skips these types to avoid C2011 redefinition.
        bool hasAnyStructDeclarations = false;
        foreach (var typeId in _allEmittedTypeSubjectIds.OrderBy(id => id, StringComparer.Ordinal))
        {
            // Concrete delegate types (e.g. System.Action, System.Func<,>) need full
            // flat struct definitions in the shared header so that page files can access
            // delegate members (chaos_delegate_invocation_count, etc.) via reinterpret_cast.
            // The root Delegate/MulticastDelegate types keep forward declarations — their
            // full inherited definitions are only on page 0 (object model section).
            bool isConcreteDelegate = IsDelegateTypeSubjectId(typeId, _referenceTypeBaseSubjectIds)
                && !string.Equals(typeId, DelegateTypeSubjectId, StringComparison.Ordinal)
                && !string.Equals(typeId, MulticastDelegateTypeSubjectId, StringComparison.Ordinal);

            if (isConcreteDelegate)
            {
                sb.Append("struct ");
                sb.Append(GetNativeTypeSymbol(typeId));
                sb.AppendLine(" {");
                sb.AppendLine("    PureTypeHeader header{};");
                sb.AppendLine("    CHAOS_IL2CPP_INTPTR chaos_delegate_target = 0;");
                sb.AppendLine("    CHAOS_IL2CPP_INTPTR chaos_delegate_method_ptr = 0;");
                sb.AppendLine("    CHAOS_IL2CPP_INTPTR chaos_delegate_invocation_list = 0;");
                sb.AppendLine("    CHAOS_IL2CPP_INTPTR chaos_delegate_invocation_count = 0;");
                sb.AppendLine("    CHAOS_IL2CPP_UINT32 chaos_delegate_method_token = 0;");
                sb.AppendLine("    CHAOS_IL2CPP_UINT32 _pad = 0;");
                sb.AppendLine("};");
            }
            else
            {
                sb.Append("struct ");
                sb.Append(GetNativeTypeSymbol(typeId));
                sb.AppendLine(";");
            }

            // Boxed type: emit complete struct for types that appear as heap allocations
            // (CHAOS_IL2CPP_NEW_GC) in codegen output, so page files can compile.
            if (_boxedTypeSubjectIds?.Contains(typeId) == true)
            {
                sb.Append("struct ");
                sb.Append(GetNativeBoxTypeSymbol(typeId));
                sb.AppendLine(" {");
                sb.AppendLine("    PureTypeHeader header{};");
                if (IsStructuredValueTypeSubjectId(typeId))
                {
                    sb.Append("    ");
                    sb.Append(GetNativeValueTypeSymbol(typeId));
                    sb.AppendLine(" value{};");
                }
                else
                {
                    sb.AppendLine("    CHAOS_IL2CPP_INTPTR value = 0;");
                }
                sb.AppendLine("};");
            }
            else
            {
                sb.Append("struct ");
                sb.Append(GetNativeBoxTypeSymbol(typeId));
                sb.AppendLine(";");
            }

            hasAnyStructDeclarations = true;
        }
        if (hasAnyStructDeclarations)
            sb.AppendLine();

        // ── Hotpatch dispatch table (global scope) ──
        // Must be at GLOBAL scope because HotpatchEntryV0 is a C typedef defined in
        // codegen_bridge.h (global scope).  Page files include codegen_bridge.h before
        // the shared header, so ::HotpatchEntryV0 is a complete type when the header
        // is read.  If we forward-declared it inside the codegen namespace it would
        // create an unrelated incomplete type, and sizeof would fail.
        if (_nativeSymbolToDispatchSlot is { Count: > 0 })
        {
            sb.AppendLine("struct HotpatchEntryV0;");
            sb.AppendLine("extern \"C\" HotpatchEntryV0 s_hotpatch_entries[];");
            sb.AppendLine();
        }

        // ── Codegen namespace ──
        // MethodTable definitions, static field variables, type ID constants, and
        // runtime helper functions (chaos_string_materialize, chaos_is_array_store_compatible,
        // chaos_default_interpolated_string_handler_*, chaos_external_runtime_*,
        // ChaosReflectionSetExceptionMetadata_2params) are all defined inside
        // `namespace chaos::il2cpp::codegen::{codegenNamespace}` on page 0.
        // Their extern declarations MUST be in the same namespace to avoid LNK2001
        // unresolved external symbols (the linker treats global-scope and namespace-scoped
        // symbols as different entities).  HotpatchEntryV0 is excluded from this namespace
        // block because it is a C typedef from codegen_bridge.h.
        sb.Append("namespace chaos::il2cpp::codegen::");
        sb.AppendLine(codegenNamespace);
        sb.AppendLine("{");

        foreach (var typeId in _allEmittedTypeSubjectIds.OrderBy(id => id, StringComparer.Ordinal))
        {
            var symbol = GetNativeMethodTableSymbol(typeId);
            sb.Append("extern MethodTable ");
            sb.Append(symbol);
            sb.AppendLine(";");
        }
        sb.AppendLine();

        // ── VTable extern declarations (inside codegen namespace) ──
        // Types with vtables defined in the object model (page 0) need extern
        // declarations so that type-info-defs.generated.cpp (paged build) can
        // reference them without C2065 undeclared identifier.
        // Use _vtableLengths cross-referenced with _allEmittedTypeSubjectIds
        // rather than _vtableTypes, since the latter may be null/empty in some
        // code paths while _vtableLengths is the authoritative source.
        if (_vtableLengths is { Count: > 0 } && _allEmittedTypeSubjectIds is { Count: > 0 })
        {
            foreach (var typeId in _allEmittedTypeSubjectIds.OrderBy(id => id, StringComparer.Ordinal))
            {
                if (_vtableLengths.TryGetValue(typeId, out int vtLen) && vtLen > 0)
                {
                    var symbol = GetNativeVTableSymbol(typeId);
                    sb.Append("extern const void* ");
                    sb.Append(symbol);
                    sb.AppendLine("[];");
                }
            }
            sb.AppendLine();
        }

        // ── Static field extern declarations (inside codegen namespace) ──
        // Declared as TU-scoped variables in the object model (page 0) with actual types
        // determined by MapFieldTypeToCppType. Page files reference them by name and need
        // extern declarations with matching types to compile without C2371 redefinition.
        if (_staticFieldDeclarations is { Count: > 0 })
        {
            foreach (var kvp in _staticFieldDeclarations.OrderBy(k => k.Key, StringComparer.Ordinal))
            {
                var cppType = MapFieldTypeToCppType(kvp.Value);
                sb.Append("extern ");
                sb.Append(cppType);
                sb.Append(' ');
                sb.Append(GetNativeStaticFieldSymbol(kvp.Key));
                sb.AppendLine(";");
            }
            sb.AppendLine();
        }

        // ── Interface type ID constants (inline constexpr, inside namespace) ──
        // Interface map arrays in page files reference chaos_type_id_* constants.
        // Emit as `inline constexpr` in the shared header so all TUs get their own
        // compile-time constant with the same value.
        var allInterfaceTypeIds = new HashSet<string>(StringComparer.Ordinal);
        if (_interfaceTypeSubjectIds is { Count: > 0 })
        {
            allInterfaceTypeIds.UnionWith(_interfaceTypeSubjectIds);
        }
        if (_referenceTypeImplementedInterfaceSubjectIds is { Count: > 0 })
        {
            foreach (var ifaceSet in _referenceTypeImplementedInterfaceSubjectIds.Values)
            {
                if (ifaceSet is { Count: > 0 })
                    allInterfaceTypeIds.UnionWith(ifaceSet);
            }
        }
        if (allInterfaceTypeIds.Count > 0)
        {
            foreach (string ifaceId in allInterfaceTypeIds.OrderBy(id => id, StringComparer.Ordinal))
            {
                ulong ifaceStableId = ComputeStableTypeId(ifaceId);
                sb.Append("inline constexpr CHAOS_IL2CPP_INTPTR ");
                sb.Append(GetNativeTypeIdSymbol(ifaceId));
                sb.Append(" = static_cast<CHAOS_IL2CPP_INTPTR>(");
                sb.Append(ifaceStableId.ToString());
                sb.AppendLine("ULL);");
            }
            sb.AppendLine();
        }

        // Non-interface type ID extern declarations
        // These are `inline constexpr` in page 0's object model but need
        // extern declarations in the shared header so page files can reference
        // them without triggering "unused inline variable" or ODR issues.
        if (_allEmittedTypeSubjectIds is { Count: > 0 })
        {
            foreach (var typeId in _allEmittedTypeSubjectIds.OrderBy(id => id, StringComparer.Ordinal))
            {
                if (allInterfaceTypeIds.Count > 0 && allInterfaceTypeIds.Contains(typeId))
                    continue; // already declared as inline constexpr above
                sb.Append("extern const uint64_t ");
                sb.Append(GetNativeTypeIdSymbol(typeId));
                sb.AppendLine(";");
            }
            sb.AppendLine();
        }

        // ── Span runtime helper declarations (inside codegen namespace) ──
        // chaos_initialize_array_from_field_data_int32 and
        // chaos_create_field_data_span_int32 are defined in the object model
        // (SpanRuntimePrelude at BuildObjectModel line 864) when
        // RuntimeHelpers.InitializeArray / CreateSpan are reachable.
        // Page files that call these functions need extern declarations
        // to compile without C3861.
        if (_externalRuntimeHelpers?.Any(helper => IsSpanRuntimeHelperSubjectId(helper.SubjectId)) == true)
        {
            sb.AppendLine("extern void chaos_initialize_array_from_field_data_int32(CHAOS_IL2CPP_INTPTR chaos_array_value, CHAOS_IL2CPP_INTPTR chaos_field_handle);");
            sb.AppendLine("extern CHAOS_IL2CPP_INTPTR chaos_create_field_data_span_int32(CHAOS_IL2CPP_INTPTR chaos_field_handle);");
            sb.AppendLine();
        }

        // ── Runtime helper declarations (inside codegen namespace) ──
        // These are DEFINED inside the codegen namespace on page 0 (in the object
        // model section), so their extern declarations MUST also be inside the
        // namespace to avoid LNK2019 from namespace-scoped vs global-scope mismatch.
        // Unused declarations are harmless — the linker only resolves referenced symbols.

        // chaos_string_materialize: conditionally emitted when string IDs exist
        if (_stringIdMapping is { Count: > 0 })
        {
            sb.AppendLine("CHAOS_IL2CPP_INTPTR chaos_string_materialize(CHAOS_IL2CPP_INTPTR chaos_value) noexcept;");
            sb.AppendLine();
        }

        // chaos_is_array_store_compatible: always emitted in object model
        sb.AppendLine("bool chaos_is_array_store_compatible(const chaos_managed_array* chaos_array, CHAOS_IL2CPP_INTPTR chaos_value) noexcept;");
        sb.AppendLine();

        // ── DefaultInterpolatedStringHandler declarations (inside codegen namespace) ──
        // These are defined inside the codegen namespace on page 0 (via
        // EmitDelegateRuntimeSupportDefinitions).  Page files call them via unqualified
        // lookup within the same namespace, so extern declarations must also be inside
        // the namespace to avoid LNK2019 (namespace-scoped vs global-scope mismatch).
        // Unused declarations are harmless — the linker only resolves referenced symbols.
        sb.AppendLine("void chaos_default_interpolated_string_handler_reset(CHAOS_IL2CPP_INTPTR chaos_handler_ref, CHAOS_IL2CPP_INT32 chaos_literal_length, CHAOS_IL2CPP_INT32 chaos_trailing_count);");
        sb.AppendLine("void chaos_default_interpolated_string_handler_append_string(CHAOS_IL2CPP_INTPTR chaos_handler_ref, CHAOS_IL2CPP_INTPTR chaos_string_value);");
        sb.AppendLine("void chaos_default_interpolated_string_handler_append_int32(CHAOS_IL2CPP_INTPTR chaos_handler_ref, CHAOS_IL2CPP_INT32 chaos_value);");
        sb.AppendLine("CHAOS_IL2CPP_INTPTR chaos_default_interpolated_string_handler_to_string_and_clear(CHAOS_IL2CPP_INTPTR chaos_handler_ref);");
        sb.AppendLine();

        sb.AppendLine("} // namespace chaos::il2cpp::codegen::" + codegenNamespace);
        sb.AppendLine();

        // ── External runtime dispatch table (global scope) ──
        // Bridge thunks in page files call through kChaosExternalRuntimeFnTable[idx].
        // The array is defined in the module registration section.
        // This is `extern "C"` and must be at global scope.
        if (_bridgeImportThunks is { Count: > 0 } &&
            _bridgeImportThunks.Values.Any(t => t.ExternalRuntimeTableIndex >= 0))
        {
            sb.AppendLine("extern \"C\" void* kChaosExternalRuntimeFnTable[];");
            sb.AppendLine();
        }

        // ── Runtime helper function declarations (global scope) ──
        // These functions are DEFINED in the native runtime library (not inside the
        // codegen namespace), so their extern declarations MUST be at global scope
        // to avoid LNK2001 unresolved external symbols.

        // ── chaos_external_runtime_* declarations (inside codegen namespace) ──
        // These helpers are DEFINED on page 0 inside the codegen namespace, so their
        // extern declarations MUST also be inside the namespace to match.
        if (_externalRuntimeHelpers is { Count: > 0 })
        {
            sb.Append("namespace chaos::il2cpp::codegen::");
            sb.AppendLine(codegenNamespace);
            sb.AppendLine("{");
            foreach (var helper in _externalRuntimeHelpers)
            {
                // Extract the first line of the source (the function signature)
                // and convert it to a declaration by appending ";".
                var source = helper.Source;
                if (string.IsNullOrEmpty(source))
                    continue;
                int newlineIdx = source.IndexOf('\n');
                string signatureLine = newlineIdx >= 0
                    ? source.Substring(0, newlineIdx).Trim()
                    : source.Trim();
                if (string.IsNullOrEmpty(signatureLine))
                    continue;
                // Remove `static ` prefix if present (should be gone after template fix,
                // but handle gracefully for any remaining static helpers)
                if (signatureLine.StartsWith("static ", StringComparison.Ordinal))
                    signatureLine = signatureLine.Substring(7);
                sb.Append("extern ");
                sb.Append(signatureLine);
                sb.AppendLine(";");
            }
            sb.AppendLine();
            sb.AppendLine("} // namespace chaos::il2cpp::codegen::" + codegenNamespace);
            sb.AppendLine();
        }

        // ── ChaosReflectionSetExceptionMetadata_2params (global scope) ──
        // Called from ArgumentOutOfRangeException..ctor(string,string) in page
        // files.  Declared in exception_api.cpp in the runtime — at global scope.
        sb.AppendLine("void ChaosReflectionSetExceptionMetadata_2params(CHAOS_IL2CPP_INTPTR chaos_exception, CHAOS_IL2CPP_INTPTR chaos_message, CHAOS_IL2CPP_INTPTR chaos_param_name);");
        sb.AppendLine();

        return sb.ToString();
    }

    private static string BuildEntryBridgeArguments(AotCoreIrMethodArtifact entryMethod)
    {
        ArgumentNullException.ThrowIfNull(entryMethod);

        if (entryMethod.ParameterCount == 0)
        {
            return "";
        }

        if (entryMethod.ParameterCount != 1 || entryMethod.ParameterAbis.Count != 1)
        {
            throw new NotSupportedException(
                $"native-aot entry bridge supports only zero-parameter or single-int32 entry methods, but '{entryMethod.SubjectId}' has {entryMethod.ParameterCount} parameters.");
        }

        var carrierKind = entryMethod.ParameterAbis[0].CarrierKindCode;
        if (carrierKind != AotCoreIrAbiCarrierKind.Int32)
        {
            throw new NotSupportedException(
                $"native-aot entry bridge supports only System.Int32 entry parameters, but '{entryMethod.SubjectId}' uses carrier '{carrierKind}'.");
        }

        return "chaos_entry_index";
    }

    /// <summary>
    /// Phase 0: Collect ModuleRegistry Tier 0 type data from the assembly PE metadata.
    /// Populates _moduleTypeFlags, _moduleTypeNames, _moduleTypeNamespaces,
    /// _moduleTypeParentTokens, _moduleTypeInfoSymbols indexed by TokenToIndex(token).
    /// On failure (e.g., missing assembly), Tier 0 arrays remain empty — the runtime
    /// falls back to Tier 2 metadata (ReflectionQueryImageDescriptor).
    /// </summary>
    private void CollectModuleTypeData(string assemblyPath)
    {
        if (string.IsNullOrEmpty(assemblyPath) || !File.Exists(assemblyPath))
            return;

        try
        {
            using var stream = File.OpenRead(assemblyPath);
            using var peReader = new PEReader(stream);
            if (!peReader.HasMetadata)
                return;

            var metadataReader = peReader.GetMetadataReader();

            // Verify the metadata assembly name matches our lowering plan
            var assemblyDef = metadataReader.GetAssemblyDefinition();
            var metadataAssemblyName = metadataReader.GetString(assemblyDef.Name);
            if (!string.Equals(metadataAssemblyName, _assemblyName, StringComparison.Ordinal))
                return;

            // Enumerate all TypeDef entries (row-indexed by ECMA TypeDef table)
            var nestedTypeMap = new Dictionary<uint, List<uint>>();
            var typeTokenToConstraints = new Dictionary<uint, List<uint>>();
            foreach (var handle in metadataReader.TypeDefinitions)
            {
                var typeDef = metadataReader.GetTypeDefinition(handle);
                var name = metadataReader.GetString(typeDef.Name);
                var ns = metadataReader.GetString(typeDef.Namespace);
                var attributes = typeDef.Attributes;
                var parentHandle = typeDef.BaseType;

                // Compute type flags
                uint flags = ComputeTypeFlags(metadataReader, typeDef, parentHandle);

                // Compute subjectId for cross-referencing with emitted TypeInfo set
                string subjectId = ComputeTypeDefSubjectId(metadataReader, handle, _assemblyName);

                // Parent token (same-assembly TypeDef only; cross-assembly → 0)
                uint parentToken = 0;
                if (!parentHandle.IsNil && parentHandle.Kind == HandleKind.TypeDefinition)
                    parentToken = (uint)MetadataTokens.GetToken(parentHandle);

                // Check if this type has a TypeInfo emitted (reachable types only)
                string? typeInfoSymbol = _allEmittedTypeSubjectIds?.Contains(subjectId) == true
                    ? GetNativeTypeInfoSymbol(subjectId)
                    : null;

                // Nested type relationship: record parent → child token
                var declaringHandle = typeDef.GetDeclaringType();
                if (!declaringHandle.IsNil)
                {
                    uint declaringToken = (uint)MetadataTokens.GetToken(declaringHandle);
                    uint childToken = (uint)MetadataTokens.GetToken(handle);
                    if (!nestedTypeMap.TryGetValue(declaringToken, out var childList))
                    {
                        childList = new List<uint>();
                        nestedTypeMap[declaringToken] = childList;
                    }
                    childList.Add(childToken);
                }

                // Generic param constraint extraction: build per-type flat array
                // of (param_index << 29 | constraint_token) entries.
                uint typeToken = (uint)MetadataTokens.GetToken(handle);
                var gpHandles = typeDef.GetGenericParameters();
                if (gpHandles.Count > 0)
                {
                    var constraintEntries = new List<uint>();
                    foreach (var gpHandle in gpHandles)
                    {
                        var gp = metadataReader.GetGenericParameter(gpHandle);
                        int paramIdx = gp.Index;
                        foreach (var gpcHandle in gp.GetConstraints())
                        {
                            var gpc = metadataReader.GetGenericParameterConstraint(gpcHandle);
                            uint ct = (uint)MetadataTokens.GetToken(gpc.Type);
                            constraintEntries.Add(((uint)paramIdx << 29) | (ct & 0x1FFFFFFFu));
                        }
                    }
                    if (constraintEntries.Count > 0)
                        typeTokenToConstraints[typeToken] = constraintEntries;
                }

                _moduleTypeFlags.Add(flags);
                _moduleTypeNames.Add(name);
                _moduleTypeNamespaces.Add(ns);
                _moduleTypeParentTokens.Add(parentToken);
                _moduleTypeInfoSymbols.Add(typeInfoSymbol);
                _moduleTypeSubjectIds.Add(subjectId);
                _moduleTypeCount++;
            }

            // Build prefix-sum nested type arrays from the collected parent→children map.
            // Type index i corresponds to token 0x02000000 | (i + 1).
            for (int i = 0; i < _moduleTypeCount; i++)
            {
                uint typeToken = 0x02000000u | (uint)(i + 1);
                _moduleNestedTypeOffsets.Add((uint)_moduleNestedTypeChildren.Count);
                if (nestedTypeMap.TryGetValue(typeToken, out var children))
                {
                    _moduleNestedTypeChildren.AddRange(children);
                }
            }
            _moduleNestedTypeOffsets.Add((uint)_moduleNestedTypeChildren.Count);

            // Build prefix-sum generic param constraint arrays (same token→index mapping).
            for (int i = 0; i < _moduleTypeCount; i++)
            {
                uint typeToken = 0x02000000u | (uint)(i + 1);
                _moduleGenericParamConstraintOffsets.Add((uint)_moduleGenericParamConstraintData.Count);
                if (typeTokenToConstraints.TryGetValue(typeToken, out var entries))
                {
                    _moduleGenericParamConstraintData.AddRange(entries);
                }
            }
            _moduleGenericParamConstraintOffsets.Add((uint)_moduleGenericParamConstraintData.Count);
        }
        catch (Exception ex) when (ex is IOException or InvalidOperationException or BadImageFormatException)
        {
            // Graceful fallback: Tier 0 arrays remain empty (code emits nullptr)
            System.Console.Error.WriteLine(
                $"[warning] CollectModuleTypeData: failed to read assembly metadata from '{assemblyPath}': {ex.GetType().Name}: {ex.Message}");
            _moduleTypeCount = 0;
            _moduleTypeFlags.Clear();
            _moduleTypeNames.Clear();
            _moduleTypeNamespaces.Clear();
            _moduleTypeParentTokens.Clear();
            _moduleTypeInfoSymbols.Clear();
            _moduleNestedTypeChildren.Clear();
            _moduleNestedTypeOffsets.Clear();
            _moduleGenericParamConstraintData.Clear();
            _moduleGenericParamConstraintOffsets.Clear();
        }
    }

    /// <summary>
    /// Resolve a TypeDef/TypeRef parent handle to a "Namespace.Name" string.
    /// Returns null for nil handles or unsupported handle kinds (TypeSpec).
    /// </summary>
    private static string? ResolveBaseTypeName(MetadataReader reader, EntityHandle parentHandle)
    {
        if (parentHandle.IsNil) return null;

        string? parentNs;
        string? parentName;

        switch (parentHandle.Kind)
        {
            case HandleKind.TypeDefinition:
                var parentDef = reader.GetTypeDefinition((TypeDefinitionHandle)parentHandle);
                parentNs = reader.GetString(parentDef.Namespace);
                parentName = reader.GetString(parentDef.Name);
                break;
            case HandleKind.TypeReference:
                var parentRef = reader.GetTypeReference((TypeReferenceHandle)parentHandle);
                parentNs = reader.GetString(parentRef.Namespace);
                parentName = reader.GetString(parentRef.Name);
                break;
            default:
                return null;
        }

        return string.IsNullOrEmpty(parentNs) ? parentName : $"{parentNs}.{parentName}";
    }

    /// <summary>
    /// Compute the type-flags bitmask for a TypeDef entry.
    /// Flags correspond to the kFlag* constants in module_registry.h.
    /// </summary>
    private static uint ComputeTypeFlags(MetadataReader reader, TypeDefinition typeDef, EntityHandle parentHandle)
    {
        uint flags = 0;
        var attributes = typeDef.Attributes;

        // Access flags from metadata attributes
        if ((attributes & TypeAttributes.Public) != 0 || (attributes & TypeAttributes.NestedPublic) != 0)
            flags |= 1u << 10;  // kFlagIsPublic
        if ((attributes & (TypeAttributes.NestedAssembly | TypeAttributes.NestedFamANDAssem
                         | TypeAttributes.NestedFamily | TypeAttributes.NestedFamORAssem
                         | TypeAttributes.NestedPrivate | TypeAttributes.NestedPublic)) != 0)
            flags |= 1u << 9;   // kFlagIsNested
        if ((attributes & TypeAttributes.Abstract) != 0)
            flags |= 1u << 2;   // kFlagIsAbstract
        if ((attributes & TypeAttributes.Sealed) != 0)
            flags |= 1u << 3;   // kFlagIsSealed
        if ((attributes & TypeAttributes.Interface) != 0)
            flags |= 1u << 4;   // kFlagIsInterface

        // ValueType / Enum via base type
        if (!parentHandle.IsNil)
        {
            var parentFullName = ResolveBaseTypeName(reader, parentHandle);
            if (parentFullName != null)
            {
                // System.Enum extends System.ValueType but is NOT a value type in reflection
                // System.ValueType itself extends System.Object — no special handling needed
                if (string.Equals(parentFullName, "System.Enum", StringComparison.Ordinal))
                    flags |= 1u << 1;  // kFlagIsEnum
                else if (string.Equals(parentFullName, "System.ValueType", StringComparison.Ordinal))
                    flags |= 1u << 0;  // kFlagIsValueType
            }
        }

        // Generic type definition: has generic parameters
        if (typeDef.GetGenericParameters().Count > 0)
            flags |= (1u << 6) | (1u << 7);  // kFlagIsGenericType | kFlagIsGenericTypeDef

        return flags;
    }

    /// <summary>
    /// Compute the SubjectId for a TypeDef from its metadata row.
    /// Format: "AssemblyName/Namespace.TypeName" or "AssemblyName/DeclaringType+NestedName".
    /// </summary>
    private static string ComputeTypeDefSubjectId(MetadataReader reader, TypeDefinitionHandle handle, string assemblyName)
    {
        var typeDef = reader.GetTypeDefinition(handle);
        var name = reader.GetString(typeDef.Name);
        var ns = reader.GetString(typeDef.Namespace);

        // Nested types use the declaring type's full name as prefix
        var declaringHandle = typeDef.GetDeclaringType();
        if (!declaringHandle.IsNil)
        {
            var declaringSubjectId = ComputeTypeDefSubjectId(reader, declaringHandle, assemblyName);
            return $"{declaringSubjectId}+{name}";
        }

        string fullName = string.IsNullOrEmpty(ns) ? name : $"{ns}.{name}";
        return $"{assemblyName}/{fullName}";
    }

    /// <summary>
    /// Build a NativeSymbol → dispatch table slot index mapping by replicating
    /// the same sorting logic used in <see cref="BuildHotpatchTable"/>.
    ///
    /// Only methods with metadata tokens are included (the only ones
    /// that appear in s_hotpatch_entries).
    /// </summary>
    // ── Pre-computed O(1) index builders for invocation planning ──

    private static Dictionary<string, AotCoreIrMethodArtifact> BuildAsyncMoveNextIndex(
        IReadOnlyDictionary<string, AotCoreIrMethodArtifact> methodsBySubjectId)
    {
        var index = new Dictionary<string, AotCoreIrMethodArtifact>(StringComparer.Ordinal);
        foreach (var method in methodsBySubjectId.Values)
        {
            if (!method.IsStatic &&
                string.Equals(GetMethodName(method.SubjectId), "MoveNext", StringComparison.Ordinal))
            {
                index[method.Identity.DeclaringTypeSubjectId] = method;
            }
        }
        return index;
    }

    private static HashSet<string> BuildAllDeclaringTypeSubjectIds(
        IReadOnlyDictionary<string, AotCoreIrMethodArtifact> methodsBySubjectId,
        IReadOnlyDictionary<string, string?> referenceTypeBaseSubjectIds,
        IReadOnlyDictionary<string, HashSet<string>> referenceTypeImplementedInterfaceSubjectIds)
    {
        var capacity = referenceTypeBaseSubjectIds.Count
            + referenceTypeImplementedInterfaceSubjectIds.Count
            + methodsBySubjectId.Count;
        var index = new HashSet<string>(capacity, StringComparer.Ordinal);
        foreach (var key in referenceTypeBaseSubjectIds.Keys)
            index.Add(key);
        foreach (var key in referenceTypeImplementedInterfaceSubjectIds.Keys)
            index.Add(key);
        foreach (var method in methodsBySubjectId.Values)
        {
            var declaringType = method.Identity.DeclaringTypeSubjectId;
            if (!string.IsNullOrEmpty(declaringType))
                index.Add(declaringType);
        }
        return index;
    }

    private static Dictionary<string, List<AotCoreIrMethodArtifact>> BuildMethodsByDeclaringTypeIndex(
        IReadOnlyDictionary<string, AotCoreIrMethodArtifact> methodsBySubjectId)
    {
        var index = new Dictionary<string, List<AotCoreIrMethodArtifact>>(StringComparer.Ordinal);
        foreach (var method in methodsBySubjectId.Values)
        {
            var declaringType = method.Identity.DeclaringTypeSubjectId;
            if (string.IsNullOrEmpty(declaringType))
                continue;
            if (!index.TryGetValue(declaringType, out var list))
            {
                list = new List<AotCoreIrMethodArtifact>();
                index[declaringType] = list;
            }
            list.Add(method);
        }
        return index;
    }

    /// <summary>
    /// Build symbol → dispatch slot map for ALL methods with IL bodies,
    /// not just reachable ones. This ensures hotpatch can target any method
    /// in the module, regardless of reachability from the AOT entry point.
    ///
    /// Methods without ECMA metadata tokens receive synthetic tokens
    /// (0x80000000 | syntheticIndex) so they still occupy dispatch table
    /// slots reachable by name-based lookup.
    /// </summary>
    /// <summary>
    /// Returns all methods that should get hotpatch dispatch slots:
    /// all non-abstract methods with IL bodies, deduplicated by NativeSymbol
    /// (shared generics share the same native symbol), sorted for deterministic
    /// slot assignment.
    /// </summary>
    private IReadOnlyList<AotCoreIrMethodArtifact> GetHotpatchableMethods()
    {
        var seenSymbols = new HashSet<string>(StringComparer.Ordinal);
        return _methodsBySubjectId.Values
            .Where(m => m.Instructions.Count > 0 || m.IsPInvoke) // has IL body or P/Invoke with wrapper — excludes abstract/interface stubs
            .OrderBy(m => ExtractNumericSortKey(m.SubjectId))
            .ThenBy(m => m.SubjectId, StringComparer.Ordinal)
            .Where(m => seenSymbols.Add(m.NativeSymbol)) // deduplicate by NativeSymbol
            .ToList();
    }

    /// <summary>
    /// Build symbol → dispatch slot map for ALL methods with IL bodies,
    /// not just reachable ones. This ensures hotpatch can target any method
    /// in the module, regardless of reachability from the AOT entry point.
    ///
    /// Methods without ECMA metadata tokens receive synthetic tokens
    /// (0x80000000 | syntheticIndex) so they still occupy dispatch table
    /// slots reachable by name-based lookup.
    ///
    /// The return value maps NativeSymbol → slot index, matching the
    /// s_hotpatch_entries[] array order from BuildHotpatchTable.
    /// </summary>
    private Dictionary<string, int> BuildDispatchSlotMap(
        IReadOnlyList<AotCoreIrMethodArtifact> reachableMethods,
        MetadataRegistrationArtifact metadataRegistration)
    {
        var tokenLookup = new MetadataTokenLookup(metadataRegistration.Registrations);

        // Use ALL hotpatchable methods, not just reachableMethods, so
        // unreachable methods also get dispatch slots for hotpatch.
        var allMethods = GetHotpatchableMethods();
        int syntheticTokenCounter = 1;

        var entries = new List<(string TypeName, string TypeNamespace, string NativeSymbol, uint Token)>();
        foreach (var method in allMethods)
        {
            string typeSubjectId;
            try
            {
                typeSubjectId = GetMethodDeclaringTypeSubjectId(method.SubjectId);
            }
            catch
            {
                continue;
            }

            var typeName = GetTypeDisplayName(typeSubjectId);
            var typeNamespace = GetTypeNamespace(typeSubjectId);
            uint token = tokenLookup.TryGetMethodToken(method.SubjectId);
            if (token == 0)
            {
                // Assign synthetic token: high bit set to avoid collision with ECMA tokens.
                token = 0x80000000u | (uint)(syntheticTokenCounter++);
            }

            entries.Add((typeName, typeNamespace, method.NativeSymbol, token));
        }

        // Assign slots in entries iteration order, matching the s_hotpatch_entries[]
        // array order from BuildHotpatchTable.
        var result = new Dictionary<string, int>(entries.Count, StringComparer.Ordinal);
        for (int slot = 0; slot < entries.Count; slot++)
        {
            result[entries[slot].NativeSymbol] = slot;
        }

        return result;
    }

    /// <summary>
    /// Returns true if the given subjectId belongs to the current AOT module,
    /// based on the "AssemblyName/..." prefix convention.
    /// </summary>
    private bool IsSameModuleMethod(string subjectId)
    {
        int slashIndex = subjectId.IndexOf('/');
        if (slashIndex < 0)
            return false;
        string subjectAssembly = subjectId.Substring(0, slashIndex);
        return string.Equals(subjectAssembly, _assemblyName, StringComparison.Ordinal);
    }

    /// <summary>
    /// Attempts to resolve a call target as a module-local direct symbol.
    /// Returns true when the callee subjectId is in <see cref="_moduleSymbolTable"/>,
    /// meaning it belongs to the current codegen output and can be called as a
    /// direct C++ function (skipping the extern runtime dispatch table).
    ///
    /// This is a defense-in-depth check: even if <see cref="ResolveDirectInvocationTarget"/>
    /// returns an InvocationTarget with ExternalRuntimeTableIndex set (due to a future
    /// code path), this check ensures same-module calls always use direct symbols.
    /// </summary>
    private bool TryResolveModuleLocalCall(
        AotCoreIrInstructionArtifact instruction,
        InvocationTarget invocationTarget,
        out string nativeSymbol)
    {
        string? callee = instruction.Callee ?? instruction.TargetReference?.SubjectId;
        if (callee != null && _moduleSymbolTable.TryGetValue(callee, out nativeSymbol))
        {
            // Only use the local symbol when the invocation target doesn't already
            // have a DirectNativeSymbol (which is already optimized) and isn't
            // going through the hotpatch path (handled earlier).
            if (invocationTarget.DirectNativeSymbol == null)
                return true;
        }
        nativeSymbol = null!;
        return false;
    }

    /// <summary>
    /// Determines whether a call target should use method_table dispatch (cross-module call).
    /// If so, allocates or retrieves the method table index.
    /// </summary>
    private bool TryGetMethodTableIndex(string? callee, string nativeSymbol, out uint index)
    {
        index = 0;
        if (string.IsNullOrEmpty(callee))
            return false;

        // Extract assembly name from callee SubjectId (format: "AssemblyName/Type::Method").
        int slashIndex = callee.IndexOf('/');
        if (slashIndex < 0)
            return false;

        string calleeAssembly = callee.Substring(0, slashIndex);
        if (string.Equals(calleeAssembly, _assemblyName, StringComparison.Ordinal))
            return false;

        // Cross-module call: allocate or retrieve method table index.
        if (!_methodTableIndices.TryGetValue(callee, out index))
        {
            index = _nextMethodTableIndex++;
            _methodTableIndices[callee] = index;
            _methodTableEntries.Add((index, nativeSymbol));
        }

        return true;
    }

    /// <summary>
    /// Builds C++ code that initializes method table entries for cross-module calls.
    /// Called once during template model creation, after all methods are emitted.
    /// </summary>
    private string BuildMethodTableInitialization()
    {
        var entries = new ScriptObject[_methodTableEntries.Count];
        for (int i = 0; i < _methodTableEntries.Count; i++)
        {
            var (entryIndex, nativeSymbol) = _methodTableEntries[i];
            bool hasManifestIndex = _methodNativeSymbolToManifestIndex.TryGetValue(nativeSymbol, out int manifestIdx);
            var entryModel = new ScriptObject
            {
                ["index"] = entryIndex,
                ["native_symbol"] = nativeSymbol,
                ["has_manifest_index"] = hasManifestIndex,
            };
            if (hasManifestIndex)
            {
                entryModel["manifest_index"] = manifestIdx;
            }
            entries[i] = entryModel;
        }

        var model = new ScriptObject
        {
            ["entries"] = entries,
        };

        var result = ScribanTemplateRenderer.RenderTemplate(
            NativeAotTemplateCatalog.GetMethodTableInitializationTemplate(), model);
        return "\n" + result;
    }

    /// <summary>
    /// Computes the set of method subjectIds reachable via AOT call graph
    /// traversal from the given entry point. BFS through call/callvirt/newobj/
    /// ldftn/ldvirtftn instructions, resolving callees through _methodsBySubjectId.
    /// </summary>
    private static HashSet<string> ComputeAotReachableSubjectIds(
        string? entrySubjectId,
        IReadOnlyList<AotCoreIrMethodArtifact> methods)
    {
        var reachable = new HashSet<string>(StringComparer.Ordinal);
        if (string.IsNullOrEmpty(entrySubjectId))
            return reachable;

        var bySubjectId = methods
            .GroupBy(m => m.SubjectId, StringComparer.Ordinal)
            .ToDictionary(g => g.Key, g => g.ToList(), StringComparer.Ordinal);

        var queue = new Queue<string>();
        queue.Enqueue(entrySubjectId);
        reachable.Add(entrySubjectId);

        while (queue.Count > 0)
        {
            var current = queue.Dequeue();
            if (!bySubjectId.TryGetValue(current, out var methodVariants))
                continue;

            foreach (var method in methodVariants)
            {
                if (method.Instructions == null)
                    continue;

                foreach (var instr in method.Instructions)
                {
                    string op = instr.Op;
                    if (op != "call" && op != "callvirt" && op != "newobj" &&
                        op != "ldftn" && op != "ldvirtftn")
                        continue;

                    string? callee = instr.Callee ?? instr.TargetReference?.SubjectId;
                    if (string.IsNullOrEmpty(callee))
                        continue;

                    // Add both the exact callee and any open-definition variant
                    if (reachable.Add(callee))
                        queue.Enqueue(callee);

                    // Also follow to the resolved instantiation if available
                    if (instr.TargetReference?.SubjectId is { } targetRef &&
                        targetRef != callee &&
                        reachable.Add(targetRef))
                        queue.Enqueue(targetRef);
                }
            }
        }

        return reachable;
    }

    /// <summary>
    /// Builds a minimal C++ function body stub for AOT-unreachable methods.
    /// Unreachable methods still need a dispatchable entry point (for the
    /// interpreter dispatch table) but do not require a full native body.
    /// Returns a default value to avoid crashing when the fact loop runs
    /// every dispatch-table entry including unreachable interface stubs.
    /// </summary>
    private static string BuildAotUnreachableMethodStub(AotCoreIrMethodArtifact method)
    {
        var returnAbi = method.ReturnAbi;
        var returnType = MapAbiSlotReturnType(returnAbi);
        var paramAbis = GetMethodAbiParameterSlots(method);
        var paramList = FormatAbiSlotParameterSignature(paramAbis);
        var symbol = method.NativeSymbol;

        // Phase A+B: detect subject methods (Subject_N / CustomEntrySubject_N) that
        // would silently produce empty stubs — WARNING at codegen time, FAIL at runtime.
        bool isSubjectMethod = method.SubjectId is not null &&
            (method.SubjectId.Contains("::Subject_") || method.SubjectId.Contains("::CustomEntrySubject_"));

        if (isSubjectMethod)
        {
            Console.Error.WriteLine($"[WARNING] Subject method '{method.SubjectId}' is AOT-unreachable — generated body will be empty. Add to --subject-methods or fix reachability.");
        }

        var builder = new StringBuilder();
        builder.AppendLine($"// AOT-unreachable stub: {method.SubjectId}");
        builder.AppendLine($"extern \"C\" {returnType} {symbol}({paramList})");
        builder.AppendLine("{");
        if (isSubjectMethod)
        {
            builder.AppendLine("    CHAOS_IL2CPP_FAIL(\"AOT-unreachable subject method called — missing function body\");");
        }
        else if (!string.IsNullOrEmpty(returnType) && returnType != "void")
        {
            builder.AppendLine($"    return {{}};");
        }
        builder.AppendLine("}");

        // Also emit the generic instantiation stub definition if this method
        // has an InstantiationStubId. The codegen may reference the stub symbol
        // from other method bodies even when this method is not AOT-reachable,
        // so the stub must exist as a valid C++ symbol.
        if (method.InstantiationStubId is not null)
        {
            var stubSymbol = ManagedNaming.CreateInstantiationStubSymbol(method.InstantiationStubId);
            builder.AppendLine();
            builder.AppendLine($"// AOT-unreachable generic instantiation stub: {method.SubjectId}");
            builder.AppendLine($"extern \"C\" {returnType} {stubSymbol}({paramList})");
            builder.AppendLine("{");
            if (isSubjectMethod)
            {
                builder.AppendLine("    CHAOS_IL2CPP_FAIL(\"AOT-unreachable subject method called — missing function body\");");
            }
            else if (!string.IsNullOrEmpty(returnType) && returnType != "void")
            {
                builder.AppendLine($"    return {{}};");
            }
            builder.AppendLine("}");
        }

        return builder.ToString();
    }

    private string BuildMethodSourceSafe(AotCoreIrMethodArtifact method)
    {
        try
        {
            return BuildMethodSource(method);
        }
        catch (Exception ex)
        {
            var msg = $"[codegen] WARNING: codegen failed for {method.SubjectId}, emitting stub. Root cause: {ex.GetType().Name}: {ex.Message}";
            Console.Error.WriteLine(msg);
            CodegenFailureCount++;
            var exType = ex.GetType().Name;
            lock (CodegenFailureByType)
            {
                CodegenFailureByType.TryGetValue(exType, out var ct);
                CodegenFailureByType[exType] = ct + 1;
            }
            var chunk = method.SubjectId?.Split('/').FirstOrDefault() ?? "unknown";
            lock (CodegenFailureByChunk)
            {
                CodegenFailureByChunk.TryGetValue(chunk, out var cc);
                CodegenFailureByChunk[chunk] = cc + 1;
            }
            return BuildAotUnreachableMethodStub(method);
        }
    }

    /// <summary>
    /// Emit one method's body, collecting reverse-P/Invoke entries as a side effect.
    /// Extracted as a separate method to give the JIT a clear stack-cleanup boundary
    /// (avoiding stack accumulation observed with Select().ToList() lambda closure).
    /// </summary>
    private NativeAotMethodTemplateModel EmitOneMethod(
        AotCoreIrMethodArtifact method,
        HashSet<string> aotReachableSubjectIds)
    {
        if (method.IsUnmanagedCallersOnly)
            _reversePInvokeEntries.Add((method.SubjectId, method.NativeSymbol));

        return new NativeAotMethodTemplateModel
        {
            SubjectId = method.SubjectId,
            NativeSymbol = method.NativeSymbol,
            MethodSource = aotReachableSubjectIds.Contains(method.SubjectId)
                ? BuildMethodSourceSafe(method)
                : BuildAotUnreachableMethodStub(method),
        };
    }

    private string BuildMethodSource(AotCoreIrMethodArtifact method)
    {
        var builder = new StringBuilder(4096);
        if (!string.IsNullOrWhiteSpace(method.OpenDefinitionSubjectId) ||
            method.SharedGenericBodyId is not null ||
            method.InstantiationStubId is not null ||
            method.RuntimeGenericContext is not null)
        {
            builder.AppendLine(FormatGenericExecutionAuthorityComment(
                method.OpenDefinitionSubjectId,
                method.SharedGenericBodyId,
                method.InstantiationStubId,
                method.RuntimeGenericContext));
        }

        EmitManagedMethod(builder, method);
        EmitGenericInstantiationStub(builder, method);
        return builder.ToString().TrimEnd();
    }

    private static void EmitExternalRuntimeHelperDefinitions(
        StringBuilder builder,
        IReadOnlyList<ExternalRuntimeHelperDefinition> externalRuntimeHelpers)
    {
        foreach (var helper in externalRuntimeHelpers)
        {
            builder.AppendLine(helper.Source);
            builder.AppendLine();
        }
    }

    private void EmitDelegateRuntimeSupportDefinitions(
        StringBuilder builder,
        IReadOnlyList<AotCoreIrMethodArtifact> reachableMethods,
        IReadOnlyList<ExternalRuntimeHelperDefinition> externalRuntimeHelpers)
    {
        bool hasCombineOrRemove = externalRuntimeHelpers.Any(helper =>
            helper.TargetSymbol.Contains("Delegate__Combine", StringComparison.Ordinal) ||
            helper.TargetSymbol.Contains("Delegate__Remove", StringComparison.Ordinal));
        if (!hasCombineOrRemove)
        {
            return;
        }

        var delegateTypeSubjectIds = CollectReachableDelegateTypeSubjectIds(reachableMethods)
            .Where(subjectId =>
                !string.Equals(subjectId, DelegateTypeSubjectId, StringComparison.Ordinal) &&
                !string.Equals(subjectId, MulticastDelegateTypeSubjectId, StringComparison.Ordinal));

        // Build delegate entry models for the Scriban switch block
        var delegateEntryModels = delegateTypeSubjectIds
            .Select(subjectId =>
            {
                var model = new ScriptObject
                {
                    ["type_id_symbol"] = GetNativeTypeIdSymbol(subjectId),
                    ["native_type_symbol"] = GetNativeTypeSymbol(subjectId),
                    ["type_info_symbol"] = GetNativeTypeInfoSymbol(subjectId),
                    ["has_vtable"] = _vtableTypes?.Contains(subjectId) == true,
                };
                if (_vtableTypes?.Contains(subjectId) == true)
                {
                    model["vtable_symbol"] = GetNativeVTableSymbol(subjectId);
                }
                return model;
            })
            .ToArray();

        var model = new ScriptObject
        {
            ["delegate_entries"] = delegateEntryModels,
        };

        var result = ScribanTemplateRenderer.RenderTemplate(
            NativeAotTemplateCatalog.GetDelegateRuntimeSupportTemplate(), model);
        builder.AppendLine(result);

        // ── Emit delegate thunk registrations (for Marshal.GetFunctionPointerForDelegate) ──
        // Build per-delegate-type thunk entries for the DelegateThunks template.
        var delegateThunkModels = delegateTypeSubjectIds
            .Select(subjectId =>
            {
                // Resolve param_count from the delegate's Invoke method signature when available.
                int paramCount = 0;
                if (_methodsByDeclaringType.TryGetValue(subjectId, out var methods))
                {
                    var invokeMethod = methods.FirstOrDefault(m =>
                        string.Equals(GetMethodName(m.SubjectId), "Invoke", StringComparison.Ordinal));
                    if (invokeMethod != null)
                    {
                        paramCount = invokeMethod.ParameterAbis.Count;
                    }
                }
                if (paramCount == 0)
                {
                    // Fallback: extract from Func/Action generic type name.
                    paramCount = ExtractDelegateArityFromSubjectId(subjectId);
                }
                if (paramCount > 8) paramCount = 8;

                var thunkModel = new ScriptObject
                {
                    ["type_id"] = subjectId,
                    ["has_custom_thunk"] = false,
                    ["thunk_symbol"] = "NativeDfnThunkArity" + paramCount,
                    ["native_type_symbol"] = GetNativeTypeSymbol(subjectId),
                    ["param_count"] = paramCount,
                };
                return thunkModel;
            })
            .ToArray();

        if (delegateThunkModels.Length > 0)
        {
            // Forward declarations are emitted by DelegateRuntimeSupport.cpp.scriban
            // (void* parameter variant — intentionally not typed RuntimeState*/ThreadState*
            // because those types are defined in header_layouts.cpp not a header).

            var thunkResult = ScribanTemplateRenderer.RenderTemplate(
                NativeAotTemplateCatalog.GetDelegateThunksTemplate(),
                new ScriptObject { ["delegate_thunks"] = delegateThunkModels });
            builder.AppendLine(thunkResult);
        }
    }

    private static int ExtractDelegateArityFromSubjectId(string subjectId)
    {
        // SubjectId format: "Assembly/TypeName`N[[...]]"
        var slash = subjectId.IndexOf('/');
        if (slash < 0) return 0;
        var typeName = subjectId.Substring(slash + 1);

        var backtick = typeName.IndexOf('`');
        if (backtick < 0) return 0;

        var arityStr = string.Empty;
        for (int i = backtick + 1; i < typeName.Length; i++)
        {
            if (char.IsDigit(typeName[i]))
                arityStr += typeName[i];
            else
                break;
        }
        if (string.IsNullOrEmpty(arityStr) || !int.TryParse(arityStr, out int arity))
            return 0;

        var baseName = typeName.Substring(0, backtick);
        var lastDot = baseName.LastIndexOf('.');
        var shortName = lastDot >= 0 ? baseName.Substring(lastDot + 1) : baseName;

        return shortName switch
        {
            "Func" when arity >= 1 => arity - 1,
            "Action" => arity,
            _ => 0,
        };
    }

    private IReadOnlyList<string> CollectReachableDelegateTypeSubjectIds(
        IReadOnlyList<AotCoreIrMethodArtifact> reachableMethods)
    {
        var subjectIds = new HashSet<string>(StringComparer.Ordinal);

        foreach (var method in reachableMethods)
        {
            foreach (var instruction in method.Instructions)
            {
                if (instruction.TargetReference is { Kind: AotCoreIrReferenceKind.Type } targetReference &&
                    IsDelegateTypeSubjectId(targetReference.SubjectId, _referenceTypeBaseSubjectIds))
                {
                    subjectIds.Add(targetReference.SubjectId);
                }

                if (!string.IsNullOrEmpty(instruction.Callee) &&
                    instruction.Callee.Contains("::Invoke(", StringComparison.Ordinal))
                {
                    var declaringTypeSubjectId = GetMethodDeclaringTypeSubjectId(instruction.Callee);
                    if (IsDelegateTypeSubjectId(declaringTypeSubjectId, _referenceTypeBaseSubjectIds))
                    {
                        subjectIds.Add(declaringTypeSubjectId);
                    }
                }

                if (!string.IsNullOrEmpty(instruction.Callee) &&
                    TryReadSingleGenericTypeArgument(
                        instruction.Callee,
                        MarshalGetFunctionPointerForDelegateMethodPrefix,
                        out var marshalDelegateTypeSubjectId) &&
                    TryResolveReferenceTypeSubjectId(marshalDelegateTypeSubjectId, out marshalDelegateTypeSubjectId) &&
                    IsDelegateTypeSubjectId(marshalDelegateTypeSubjectId, _referenceTypeBaseSubjectIds))
                {
                    subjectIds.Add(marshalDelegateTypeSubjectId);
                }

                if (!string.IsNullOrEmpty(instruction.Callee) &&
                    TryReadSingleGenericTypeArgument(
                        instruction.Callee,
                        MarshalGetDelegateForFunctionPointerMethodPrefix,
                        out marshalDelegateTypeSubjectId) &&
                    TryResolveReferenceTypeSubjectId(marshalDelegateTypeSubjectId, out marshalDelegateTypeSubjectId) &&
                    IsDelegateTypeSubjectId(marshalDelegateTypeSubjectId, _referenceTypeBaseSubjectIds))
                {
                    subjectIds.Add(marshalDelegateTypeSubjectId);
                }
            }
        }

        if (subjectIds.Count > 0)
        {
            subjectIds.Add(DelegateTypeSubjectId);
            subjectIds.Add(MulticastDelegateTypeSubjectId);
        }

        return subjectIds
            .OrderBy(subjectId => subjectId, StringComparer.Ordinal)
            .ToArray();
    }

    private static IReadOnlyDictionary<string, string> BuildClosureAssemblyPathByNameCore(
        IReadOnlyList<string> closureAssemblyPaths)
    {
        var pathsByAssemblyName = new Dictionary<string, string>(closureAssemblyPaths.Count, StringComparer.Ordinal);
        foreach (var assemblyPath in closureAssemblyPaths)
        {
            using var stream = File.OpenRead(assemblyPath);
            using var peReader = new PEReader(stream);
            if (!peReader.HasMetadata)
            {
                continue;
            }

            var metadataReader = peReader.GetMetadataReader();
            var assemblyName = metadataReader.GetString(metadataReader.GetAssemblyDefinition().Name);
            pathsByAssemblyName[assemblyName] = assemblyPath;
        }

        return pathsByAssemblyName;
    }

    private static bool TryParseStaticFieldDataSize(
        string memberType,
        out int size)
    {
        const string marker = "__StaticArrayInitTypeSize=";
        size = 0;
        var markerIndex = memberType.IndexOf(marker, StringComparison.Ordinal);
        if (markerIndex < 0)
        {
            return false;
        }

        return int.TryParse(
            memberType[(markerIndex + marker.Length)..],
            NumberStyles.None,
            CultureInfo.InvariantCulture,
            out size) &&
               size >= 0;
    }

    private static bool TryLoadStaticFieldDataBytes(
        string assemblyPath,
        string fieldSubjectId,
        int size,
        out IReadOnlyList<byte> bytes)
    {
        bytes = Array.Empty<byte>();
        using var stream = File.OpenRead(assemblyPath);
        using var peReader = new PEReader(stream);
        if (!peReader.HasMetadata)
        {
            return false;
        }

        var metadataReader = peReader.GetMetadataReader();
        var assemblyName = metadataReader.GetString(metadataReader.GetAssemblyDefinition().Name);
        if (!TryResolveFieldDefinitionHandle(
                metadataReader,
                assemblyName,
                fieldSubjectId,
                out var fieldHandle))
        {
            return false;
        }

        var fieldDefinition = metadataReader.GetFieldDefinition(fieldHandle);
        var relativeVirtualAddress = fieldDefinition.GetRelativeVirtualAddress();
        if (relativeVirtualAddress <= 0)
        {
            return false;
        }

        var sectionData = peReader.GetSectionData(relativeVirtualAddress);
        if (sectionData.Length < size)
        {
            return false;
        }

        bytes = sectionData.GetReader(0, size).ReadBytes(size);
        return true;
    }

    private static bool TryResolveFieldDefinitionHandle(
        MetadataReader metadataReader,
        string assemblyName,
        string fieldSubjectId,
        out FieldDefinitionHandle fieldHandle)
    {
        fieldHandle = default;
        var declaringTypeSubjectId = GetDeclaringTypeSubjectId(fieldSubjectId);
        var fieldName = GetFieldName(fieldSubjectId);

        foreach (var candidateTypeHandle in metadataReader.TypeDefinitions)
        {
            if (!TryResolveTypeDefinitionIdentity(
                    metadataReader,
                    assemblyName,
                    candidateTypeHandle,
                    out var typeIdentity) ||
                !string.Equals(typeIdentity.SubjectId, declaringTypeSubjectId, StringComparison.Ordinal))
            {
                continue;
            }

            var typeDefinition = metadataReader.GetTypeDefinition(candidateTypeHandle);
            foreach (var candidateFieldHandle in typeDefinition.GetFields())
            {
                var candidateFieldDefinition = metadataReader.GetFieldDefinition(candidateFieldHandle);
                if (string.Equals(
                        metadataReader.GetString(candidateFieldDefinition.Name),
                        fieldName,
                        StringComparison.Ordinal))
                {
                    fieldHandle = candidateFieldHandle;
                    return true;
                }
            }

            return false;
        }

        return false;
    }

    private static bool TryResolveTypeDefinitionHandleForReflectionMemberEntry(
        SupplementalMetadataTypeTemplateEntry typeEntry,
        IReadOnlyList<SupplementalMetadataTypeTemplateEntry> assemblyTypeEntries,
        out TypeDefinitionHandle typeDefinitionHandle)
    {
        if (TryCreateMetadataEntityHandle(typeEntry.MetadataToken, out var handle) &&
            handle.Kind == HandleKind.TypeDefinition)
        {
            typeDefinitionHandle = (TypeDefinitionHandle)handle;
            return true;
        }

        if (string.IsNullOrEmpty(typeEntry.DefinitionSubjectId) ||
            string.Equals(typeEntry.SubjectId, typeEntry.DefinitionSubjectId, StringComparison.Ordinal))
        {
            typeDefinitionHandle = default;
            return false;
        }

        var definitionEntry = assemblyTypeEntries.FirstOrDefault(candidate =>
            string.Equals(candidate.SubjectId, typeEntry.DefinitionSubjectId, StringComparison.Ordinal));
        if (definitionEntry is null)
        {
            typeDefinitionHandle = default;
            return false;
        }

        if (TryCreateMetadataEntityHandle(definitionEntry.MetadataToken, out handle) &&
            handle.Kind == HandleKind.TypeDefinition)
        {
            typeDefinitionHandle = (TypeDefinitionHandle)handle;
            return true;
        }

        typeDefinitionHandle = default;
        return false;
    }

    private static bool TryResolveMethodDefinitionForReflectionMemberEntry(
        MetadataReader metadataReader,
        SupplementalMetadataMethodTemplateEntry methodEntry,
        IReadOnlyList<SupplementalMetadataTypeTemplateEntry> assemblyTypeEntries,
        out MethodDefinitionHandle methodDefinitionHandle)
    {
        if (TryCreateMetadataEntityHandle(methodEntry.MetadataToken, out var handle) &&
            handle.Kind == HandleKind.MethodDefinition)
        {
            methodDefinitionHandle = (MethodDefinitionHandle)handle;
            return true;
        }

        if (string.IsNullOrEmpty(methodEntry.DefinitionSubjectId))
        {
            methodDefinitionHandle = default;
            return false;
        }

        var definitionDeclaringTypeSubjectId = GetMethodDeclaringTypeSubjectId(methodEntry.DefinitionSubjectId);
        var definitionTypeEntry = assemblyTypeEntries.FirstOrDefault(candidate =>
            string.Equals(candidate.SubjectId, definitionDeclaringTypeSubjectId, StringComparison.Ordinal));
        if (definitionTypeEntry is null ||
            !TryResolveTypeDefinitionHandleForReflectionMemberEntry(
                definitionTypeEntry,
                assemblyTypeEntries,
                out var typeDefinitionHandle))
        {
            methodDefinitionHandle = default;
            return false;
        }

        var metadataMethodName = GetMetadataMethodNameForReflectionMemberDefinition(methodEntry.DefinitionSubjectId);
        var typeDefinition = metadataReader.GetTypeDefinition(typeDefinitionHandle);
        foreach (var candidateHandle in typeDefinition.GetMethods())
        {
            var candidateDefinition = metadataReader.GetMethodDefinition(candidateHandle);
            if (!string.Equals(
                    metadataReader.GetString(candidateDefinition.Name),
                    metadataMethodName,
                    StringComparison.Ordinal))
            {
                continue;
            }

            if (GetMethodParameterCount(metadataReader, candidateDefinition) != methodEntry.ParameterCount)
            {
                continue;
            }

            methodDefinitionHandle = candidateHandle;
            return true;
        }

        methodDefinitionHandle = default;
        return false;
    }

    private static bool TryCreateMetadataEntityHandle(int metadataToken, out EntityHandle handle)
    {
        if (metadataToken == 0)
        {
            handle = default;
            return false;
        }

        try
        {
            handle = MetadataTokens.EntityHandle(metadataToken);
            return !handle.IsNil;
        }
        catch (ArgumentException)
        {
            handle = default;
            return false;
        }
    }

    private static string GetMetadataMethodNameForReflectionMemberDefinition(string methodSubjectId)
    {
        var methodName = GetMethodName(methodSubjectId);
        var genericArgumentIndex = methodName.IndexOf('<');
        if (genericArgumentIndex >= 0)
        {
            methodName = methodName[..genericArgumentIndex];
        }

        return ManagedNaming.StripGenericArity(methodName);
    }

    private static int GetMethodParameterCount(MetadataReader metadataReader, MethodDefinition methodDefinition)
    {
        return methodDefinition.GetParameters()
            .Select(parameterHandle => metadataReader.GetParameter(parameterHandle))
            .Count(parameter => parameter.SequenceNumber > 0);
    }

    private void CollectCustomAttributeMaterializations(
        MetadataReader metadataReader,
        string currentAssemblyName,
        string targetSubjectId,
        CustomAttributeTargetKind targetKind,
        uint entityMetadataToken,
        CustomAttributeHandleCollection attributeHandles,
        IReadOnlySet<string> queriedDisplayNames,
        IReadOnlySet<string> memberInfoIsDefinedAttributeTypeSubjectIds,
        IDictionary<string, string> displayNameToSubjectId,
        ICollection<CustomAttributeMaterializationPlan> materializations,
        ISet<string> materializationKeys)
    {
        foreach (var attributeHandle in attributeHandles)
        {
            if (!TryGetAttributeTypeIdentity(metadataReader, currentAssemblyName, attributeHandle, out var attributeTypeIdentity))
            {
                continue;
            }

            var isExplicitQuery = queriedDisplayNames.Contains(attributeTypeIdentity.DisplayName);
            var isRequestedByIsDefined = memberInfoIsDefinedAttributeTypeSubjectIds.Contains(attributeTypeIdentity.SubjectId);
            if (!isExplicitQuery && !isRequestedByIsDefined)
            {
                continue;
            }

            if (isExplicitQuery)
            {
                RegisterCustomAttributeTypeSubjectId(
                    attributeTypeIdentity.DisplayName,
                    attributeTypeIdentity.SubjectId,
                    displayNameToSubjectId);
            }

            var key = $"{(byte)targetKind}:{targetSubjectId}:{attributeTypeIdentity.SubjectId}";
            if (!materializationKeys.Add(key))
            {
                continue;
            }

            materializations.Add(CreateCustomAttributeMaterializationPlan(
                metadataReader,
                targetSubjectId,
                targetKind,
                entityMetadataToken,
                attributeHandle,
                attributeTypeIdentity.SubjectId));
        }
    }

    private void CollectSyntheticMethodCustomAttributeMaterializations(
        MetadataReader metadataReader,
        string assemblyName,
        string targetSubjectId,
        uint methodToken,
        MethodDefinition methodDefinition,
        IReadOnlySet<string> queriedDisplayNames,
        IReadOnlySet<string> memberInfoIsDefinedAttributeTypeSubjectIds,
        IDictionary<string, string> displayNameToSubjectId,
        ICollection<CustomAttributeMaterializationPlan> materializations,
        ISet<string> materializationKeys)
    {
        if ((memberInfoIsDefinedAttributeTypeSubjectIds.Contains(DllImportAttributeTypeSubjectId) ||
             queriedDisplayNames.Contains(DllImportAttributeDisplayName)) &&
            methodDefinition.Attributes.HasFlag(MethodAttributes.PinvokeImpl))
        {
            if (queriedDisplayNames.Contains(DllImportAttributeDisplayName))
            {
                RegisterCustomAttributeTypeSubjectId(
                    DllImportAttributeDisplayName,
                    DllImportAttributeTypeSubjectId,
                    displayNameToSubjectId);
            }

            var key = $"{(byte)CustomAttributeTargetKind.Method}:{targetSubjectId}:{DllImportAttributeTypeSubjectId}";
            if (materializationKeys.Add(key))
            {
                materializations.Add(CreateDllImportAttributeMaterializationPlan(metadataReader, targetSubjectId, methodDefinition, methodToken));
            }
        }

        if (!memberInfoIsDefinedAttributeTypeSubjectIds.Contains(UnmanagedCallersOnlyAttributeTypeSubjectId) &&
            !queriedDisplayNames.Contains(UnmanagedCallersOnlyAttributeDisplayName))
        {
            return;
        }

        foreach (var attributeHandle in methodDefinition.GetCustomAttributes())
        {
            if (!TryGetAttributeTypeIdentity(
                    metadataReader,
                    assemblyName,
                    attributeHandle,
                    out var attributeTypeIdentity) ||
                !string.Equals(
                    attributeTypeIdentity.SubjectId,
                    UnmanagedCallersOnlyAttributeTypeSubjectId,
                    StringComparison.Ordinal))
            {
                continue;
            }

            if (queriedDisplayNames.Contains(UnmanagedCallersOnlyAttributeDisplayName))
            {
                RegisterCustomAttributeTypeSubjectId(
                    UnmanagedCallersOnlyAttributeDisplayName,
                    UnmanagedCallersOnlyAttributeTypeSubjectId,
                    displayNameToSubjectId);
            }

            var key = $"{(byte)CustomAttributeTargetKind.Method}:{targetSubjectId}:{UnmanagedCallersOnlyAttributeTypeSubjectId}";
            if (!materializationKeys.Add(key))
            {
                continue;
            }

            materializations.Add(CreateCustomAttributeMaterializationPlan(
                metadataReader,
                targetSubjectId,
                CustomAttributeTargetKind.Method,
                methodToken,
                attributeHandle,
                UnmanagedCallersOnlyAttributeTypeSubjectId));
        }
    }

    private void CollectClosureWideSyntheticMethodCustomAttributeMaterializations(
        MetadataReader metadataReader,
        string assemblyName,
        IReadOnlySet<string> queriedDisplayNames,
        IReadOnlySet<string> memberInfoIsDefinedAttributeTypeSubjectIds,
        IDictionary<string, string> displayNameToSubjectId,
        ICollection<CustomAttributeMaterializationPlan> materializations,
        ISet<string> materializationKeys)
    {
        if (!memberInfoIsDefinedAttributeTypeSubjectIds.Contains(DllImportAttributeTypeSubjectId) &&
            !memberInfoIsDefinedAttributeTypeSubjectIds.Contains(UnmanagedCallersOnlyAttributeTypeSubjectId) &&
            !queriedDisplayNames.Contains(DllImportAttributeDisplayName) &&
            !queriedDisplayNames.Contains(UnmanagedCallersOnlyAttributeDisplayName))
        {
            return;
        }

        foreach (var typeHandle in metadataReader.TypeDefinitions)
        {
            if (!TryResolveTypeDefinitionIdentity(
                    metadataReader,
                    assemblyName,
                    typeHandle,
                    out var declaringTypeIdentity))
            {
                continue;
            }

            var typeDefinition = metadataReader.GetTypeDefinition(typeHandle);
            foreach (var methodHandle in typeDefinition.GetMethods())
            {
                if (!TryCreateMetadataMethodSubjectId(
                        metadataReader,
                        assemblyName,
                        declaringTypeIdentity.SubjectId,
                        methodHandle,
                        out var targetSubjectId) ||
                    string.IsNullOrEmpty(targetSubjectId))
                {
                    continue;
                }

                CollectSyntheticMethodCustomAttributeMaterializations(
                    metadataReader,
                    assemblyName,
                    targetSubjectId!,
                    (uint)MetadataTokens.GetToken(methodHandle),
                    metadataReader.GetMethodDefinition(methodHandle),
                    queriedDisplayNames,
                    memberInfoIsDefinedAttributeTypeSubjectIds,
                    displayNameToSubjectId,
                    materializations,
                    materializationKeys);
            }
        }
    }

    private static bool TryCreateMetadataMethodSubjectId(
        MetadataReader metadataReader,
        string assemblyName,
        string declaringTypeSubjectId,
        MethodDefinitionHandle methodHandle,
        out string? methodSubjectId)
    {
        methodSubjectId = null;
        var methodDefinition = metadataReader.GetMethodDefinition(methodHandle);
        var signature = methodDefinition.DecodeSignature(
            new MetadataMethodSignatureTypeNameProvider(metadataReader, assemblyName),
            genericContext: null);
        var methodName = metadataReader.GetString(methodDefinition.Name);
        if (string.IsNullOrEmpty(methodName) ||
            string.IsNullOrEmpty(signature.ReturnType))
        {
            return false;
        }

        methodSubjectId = ManagedNaming.CreateMethodSubjectId(
            declaringTypeSubjectId,
            methodName,
            signature.ReturnType,
            signature.ParameterTypes.ToArray(),
            methodDefinition.GetGenericParameters().Count);
        return true;
    }

    private CustomAttributeMaterializationPlan CreateDllImportAttributeMaterializationPlan(
        MetadataReader metadataReader,
        string targetSubjectId,
        MethodDefinition methodDefinition,
        uint methodToken)
    {
        var import = methodDefinition.GetImport();
        var moduleReference = metadataReader.GetModuleReference(import.Module);
        var moduleName = metadataReader.GetString(moduleReference.Name);
        var entryPointName = import.Name.IsNil
            ? metadataReader.GetString(methodDefinition.Name)
            : metadataReader.GetString(import.Name);
        var assignments = new List<CustomAttributeFieldAssignment>
        {
            new(
                ResolveAttributeStorageField(DllImportAttributeTypeSubjectId, "Value"),
                new CustomAttributeLiteralValue(CustomAttributeLiteralKind.String, moduleName)),
            new(
                ResolveAttributeStorageField(DllImportAttributeTypeSubjectId, "EntryPoint"),
                new CustomAttributeLiteralValue(CustomAttributeLiteralKind.String, entryPointName)),
        };

        if (import.Attributes.HasFlag(MethodImportAttributes.ExactSpelling))
        {
            assignments.Add(new CustomAttributeFieldAssignment(
                ResolveAttributeStorageField(DllImportAttributeTypeSubjectId, "ExactSpelling"),
                new CustomAttributeLiteralValue(CustomAttributeLiteralKind.Boolean, true)));
        }

        if (import.Attributes.HasFlag(MethodImportAttributes.SetLastError))
        {
            assignments.Add(new CustomAttributeFieldAssignment(
                ResolveAttributeStorageField(DllImportAttributeTypeSubjectId, "SetLastError"),
                new CustomAttributeLiteralValue(CustomAttributeLiteralKind.Boolean, true)));
        }

        // P1.3: CallingConvention mapping (MethodImportAttributes → System.Runtime.InteropServices.CallingConvention).
        var ccBits = import.Attributes & MethodImportAttributes.CallingConventionMask;
        if (ccBits != 0) // 0 = WinApi (default, skip)
        {
            int ccValue = ccBits switch
            {
                MethodImportAttributes.CallingConventionWinApi => 1,   // CallingConvention.WinApi
                MethodImportAttributes.CallingConventionCDecl => 2,    // CallingConvention.CDecl
                MethodImportAttributes.CallingConventionStdCall => 3,  // CallingConvention.StdCall
                MethodImportAttributes.CallingConventionThisCall => 4, // CallingConvention.ThisCall
                MethodImportAttributes.CallingConventionFastCall => 5, // CallingConvention.FastCall
                _ => 1, // fallback: WinApi
            };
            assignments.Add(new CustomAttributeFieldAssignment(
                ResolveAttributeStorageField(DllImportAttributeTypeSubjectId, "CallingConvention"),
                new CustomAttributeLiteralValue(CustomAttributeLiteralKind.Int32, ccValue)));
        }

        // P1.3: CharSet mapping (MethodImportAttributes → System.Runtime.InteropServices.CharSet).
        var csBits = import.Attributes & MethodImportAttributes.CharSetMask;
        // 0x0000 = default (Ansi, skip); explicit Ansi=0x0002, Unicode=0x0004, Auto=0x0006
        if (csBits != 0)
        {
            int csValue = csBits switch
            {
                MethodImportAttributes.CharSetAnsi => 2,    // CharSet.Ansi
                (MethodImportAttributes)0x0004 => 3,        // CharSet.Unicode
                MethodImportAttributes.CharSetAuto => 4,    // CharSet.Auto
                _ => 2, // fallback: Ansi
            };
            assignments.Add(new CustomAttributeFieldAssignment(
                ResolveAttributeStorageField(DllImportAttributeTypeSubjectId, "CharSet"),
                new CustomAttributeLiteralValue(CustomAttributeLiteralKind.Int32, csValue)));
        }

        return new CustomAttributeMaterializationPlan(
            CustomAttributeTargetKind.Method,
            methodToken,
            targetSubjectId,
            DllImportAttributeTypeSubjectId,
            assignments);
    }

    private CustomAttributeMaterializationPlan CreateCustomAttributeMaterializationPlan(
        MetadataReader metadataReader,
        string targetSubjectId,
        CustomAttributeTargetKind targetKind,
        uint entityMetadataToken,
        CustomAttributeHandle attributeHandle,
        string attributeTypeSubjectId)
    {
        var attribute = metadataReader.GetCustomAttribute(attributeHandle);
        var decodedValue = attribute.DecodeValue(NativeAotCustomAttributeTypeProvider.Instance);
        var constructorParameterNames = GetAttributeConstructorParameterNames(metadataReader, attribute.Constructor);
        var assignments = new List<CustomAttributeFieldAssignment>();

        for (var index = 0; index < decodedValue.FixedArguments.Length; index++)
        {
            var memberName = ResolveFixedArgumentMemberName(
                attributeTypeSubjectId,
                constructorParameterNames,
                index,
                decodedValue.FixedArguments.Length);
            assignments.Add(new CustomAttributeFieldAssignment(
                ResolveAttributeStorageField(attributeTypeSubjectId, memberName),
                CreateCustomAttributeLiteralValue(decodedValue.FixedArguments[index].Value)));
        }

        foreach (var namedArgument in decodedValue.NamedArguments)
        {
            if (string.IsNullOrEmpty(namedArgument.Name))
            {
                throw new NotSupportedException(
                    $"native-aot custom-attribute materialization found an unnamed argument on '{attributeTypeSubjectId}'.");
            }

            assignments.Add(new CustomAttributeFieldAssignment(
                ResolveAttributeStorageField(attributeTypeSubjectId, namedArgument.Name),
                CreateCustomAttributeLiteralValue(namedArgument.Value)));
        }

        return new CustomAttributeMaterializationPlan(
            targetKind,
            entityMetadataToken,
            targetSubjectId,
            attributeTypeSubjectId,
            assignments);
    }

    private static IReadOnlySet<string> CollectMemberInfoIsDefinedAttributeTypeSubjectIds(
        IReadOnlyList<AotCoreIrMethodArtifact> reachableMethods)
    {
        var attributeTypeSubjectIds = new HashSet<string>(StringComparer.Ordinal);
        foreach (var method in reachableMethods)
        {
            for (var index = 0; index < method.Instructions.Count; index++)
            {
                var instruction = method.Instructions[index];
                if (!MatchesMethodSubject(
                        instruction.Callee ?? string.Empty,
                        "System.Private.CoreLib/System.Reflection.MemberInfo",
                        "IsDefined",
                        "System.Type",
                        "System.Boolean"))
                {
                    continue;
                }

                if (!TryResolveMemberInfoIsDefinedAttributeTypeSubjectId(method.Instructions, index, out var attributeTypeSubjectId) ||
                    string.IsNullOrEmpty(attributeTypeSubjectId))
                {
                    continue;
                }

                attributeTypeSubjectIds.Add(attributeTypeSubjectId!);
            }
        }

        return attributeTypeSubjectIds;
    }

    private static bool TryResolveMemberInfoIsDefinedAttributeTypeSubjectId(
        IReadOnlyList<AotCoreIrInstructionArtifact> instructions,
        int callIndex,
        out string? attributeTypeSubjectId)
    {
        attributeTypeSubjectId = null;
        if (callIndex < 3 ||
            !string.Equals(instructions[callIndex - 1].Op, "ldc.i4", StringComparison.Ordinal) ||
            !MatchesMethodSubject(
                instructions[callIndex - 2].Callee ?? string.Empty,
                "System.Private.CoreLib/System.Type",
                "GetTypeFromHandle",
                "System.RuntimeTypeHandle"))
        {
            return false;
        }

        var loadTokenInstruction = instructions[callIndex - 3];
        if (!string.Equals(loadTokenInstruction.Op, "ldtoken", StringComparison.Ordinal))
        {
            return false;
        }

        if (loadTokenInstruction.TargetReference?.Kind == AotCoreIrReferenceKind.Type &&
            !string.IsNullOrEmpty(loadTokenInstruction.TargetReference.SubjectId))
        {
            attributeTypeSubjectId = loadTokenInstruction.TargetReference.SubjectId;
            return true;
        }

        if (loadTokenInstruction.Operand is string directSubjectId &&
            !string.IsNullOrEmpty(directSubjectId))
        {
            attributeTypeSubjectId = directSubjectId;
            return true;
        }

        if (loadTokenInstruction.Operand is JsonElement { ValueKind: JsonValueKind.String } element)
        {
            var jsonSubjectId = element.GetString();
            if (!string.IsNullOrEmpty(jsonSubjectId))
            {
                attributeTypeSubjectId = jsonSubjectId;
                return true;
            }
        }

        return false;
    }

    private static IReadOnlyList<string> GetAttributeConstructorParameterNames(
        MetadataReader metadataReader,
        EntityHandle constructorHandle)
    {
        if (constructorHandle.Kind != HandleKind.MethodDefinition)
        {
            return [];
        }

        var methodDefinition = metadataReader.GetMethodDefinition((MethodDefinitionHandle)constructorHandle);
        return methodDefinition.GetParameters()
            .Select(handle => metadataReader.GetString(metadataReader.GetParameter(handle).Name))
            .Where(name => !string.IsNullOrEmpty(name))
            .ToArray();
    }

    private static void RegisterCustomAttributeTypeSubjectId(
        string displayName,
        string subjectId,
        IDictionary<string, string> displayNameToSubjectId)
    {
        if (displayNameToSubjectId.TryGetValue(displayName, out var existingSubjectId) &&
            !string.Equals(existingSubjectId, subjectId, StringComparison.Ordinal))
        {
            return;
        }

        displayNameToSubjectId[displayName] = subjectId;
    }

    private string ResolveFixedArgumentMemberName(
        string attributeTypeSubjectId,
        IReadOnlyList<string> constructorParameterNames,
        int argumentIndex,
        int totalArgumentCount)
    {
        if (argumentIndex < constructorParameterNames.Count &&
            !string.IsNullOrEmpty(constructorParameterNames[argumentIndex]))
        {
            var parameterName = constructorParameterNames[argumentIndex];
            return char.ToUpperInvariant(parameterName[0]) + parameterName[1..];
        }

        if (totalArgumentCount == 1)
        {
            return "Value";
        }

        throw new NotSupportedException(
            $"native-aot custom-attribute materialization could not bind fixed argument #{argumentIndex} for '{attributeTypeSubjectId}'.");
    }

    private string ResolveAttributeStorageField(
        string attributeTypeSubjectId,
        string memberName)
    {
        var key = $"{attributeTypeSubjectId}:{memberName}";
        if (_attributeStorageFieldIndex.TryGetValue(key, out var cached))
        {
            return cached;
        }

        return ManagedNaming.CreateFieldSubjectId(attributeTypeSubjectId, memberName);
    }

    private static Dictionary<string, string> BuildAttributeStorageFieldIndex(
        IReadOnlyDictionary<string, AotCoreIrMethodArtifact> methodsBySubjectId)
    {
        var index = new Dictionary<string, string>(methodsBySubjectId.Count, StringComparer.Ordinal);
        foreach (var method in methodsBySubjectId.Values)
        {
            if (method.IsStatic || method.ParameterCount != 0)
            {
                continue;
            }

            // Check if this method is a property getter (auto-property pattern)
            var getterMemberName = ExtractGetterMemberName(method);
            if (getterMemberName is null)
            {
                continue;
            }

            if (!TryGetAutoGetterStorageFieldSubjectId(method, out var fieldSubjectId) ||
                string.IsNullOrEmpty(fieldSubjectId))
            {
                continue;
            }

            var declaringType = method.Identity.DeclaringTypeSubjectId;
            var key = $"{declaringType}:{getterMemberName}";
            index.TryAdd(key, fieldSubjectId!);
        }

        return index;
    }

    private static string? ExtractGetterMemberName(AotCoreIrMethodArtifact method)
    {
        var subjectId = method.SubjectId;
        var getterPrefix = "::get_";
        var getterSuffix = ":";
        var startIndex = subjectId.IndexOf(getterPrefix, StringComparison.Ordinal);
        if (startIndex < 0)
        {
            return null;
        }

        startIndex += getterPrefix.Length;
        var endIndex = subjectId.IndexOf(getterSuffix, startIndex, StringComparison.Ordinal);
        if (endIndex < 0)
        {
            return null;
        }

        return subjectId[startIndex..endIndex];
    }

    private static bool TryGetAutoGetterStorageFieldSubjectId(
        AotCoreIrMethodArtifact method,
        out string? fieldSubjectId)
    {
        fieldSubjectId = null;
        if (method.IsStatic || method.ParameterCount != 0)
        {
            return false;
        }

        var fieldLoads = method.Instructions
            .Where(instruction =>
                string.Equals(instruction.Op, "ldfld", StringComparison.Ordinal) &&
                instruction.TargetReference?.Kind == AotCoreIrReferenceKind.Field &&
                !string.IsNullOrWhiteSpace(instruction.TargetReference.SubjectId))
            .Select(instruction => instruction.TargetReference!.SubjectId)
            .Distinct(StringComparer.Ordinal)
            .ToArray();
        if (fieldLoads.Length != 1)
        {
            return false;
        }

        fieldSubjectId = fieldLoads[0];
        return true;
    }

    private static CustomAttributeLiteralValue CreateCustomAttributeLiteralValue(object? value)
    {
        return value switch
        {
            null => new CustomAttributeLiteralValue(CustomAttributeLiteralKind.Null, null),
            bool booleanValue => new CustomAttributeLiteralValue(CustomAttributeLiteralKind.Boolean, booleanValue),
            byte byteValue => new CustomAttributeLiteralValue(CustomAttributeLiteralKind.Byte, byteValue),
            sbyte byteValue => new CustomAttributeLiteralValue(CustomAttributeLiteralKind.SByte, byteValue),
            short shortValue => new CustomAttributeLiteralValue(CustomAttributeLiteralKind.Int16, shortValue),
            int intValue => new CustomAttributeLiteralValue(CustomAttributeLiteralKind.Int32, intValue),
            long longValue => new CustomAttributeLiteralValue(CustomAttributeLiteralKind.Int64, longValue),
            ushort shortValue => new CustomAttributeLiteralValue(CustomAttributeLiteralKind.UInt16, shortValue),
            uint intValue => new CustomAttributeLiteralValue(CustomAttributeLiteralKind.UInt32, intValue),
            ulong longValue => new CustomAttributeLiteralValue(CustomAttributeLiteralKind.UInt64, longValue),
            float floatValue => new CustomAttributeLiteralValue(CustomAttributeLiteralKind.Single, floatValue),
            double doubleValue => new CustomAttributeLiteralValue(CustomAttributeLiteralKind.Double, doubleValue),
            char charValue => new CustomAttributeLiteralValue(CustomAttributeLiteralKind.Char, charValue),
            string stringValue => new CustomAttributeLiteralValue(CustomAttributeLiteralKind.String, stringValue),
            _ => throw new NotSupportedException(
                $"native-aot custom-attribute materialization does not support literal value '{value.GetType().FullName}'."),
        };
    }

    private static IEnumerable<string> EnumerateClosureAssemblyPaths(
        ManagedClosureManifestArtifact closureManifest)
    {
        if (closureManifest.ResolvedAssemblies is { Count: > 0 })
        {
            foreach (var resolvedAssembly in closureManifest.ResolvedAssemblies)
            {
                if (!string.IsNullOrEmpty(resolvedAssembly.Path))
                {
                    yield return Path.GetFullPath(resolvedAssembly.Path);
                }
            }

            yield break;
        }

        yield return Path.GetFullPath(closureManifest.InputAssemblyPath);

        if (closureManifest.AdditionalAssemblyPaths is null)
        {
            yield break;
        }

        foreach (var assemblyPath in closureManifest.AdditionalAssemblyPaths)
        {
            yield return Path.GetFullPath(assemblyPath);
        }
    }

    private static bool TryParseCustomAttributeQueryCallee(
        string? callee,
        out string? attributeDisplayName)
    {
        attributeDisplayName = null;
        const string declaringTypeSubjectId =
            "System.Private.CoreLib/System.Reflection.CustomAttributeExtensions";
        const string openGenericMethodPrefix =
            "System.Private.CoreLib/System.Reflection.CustomAttributeExtensions::GetCustomAttribute<";

        if (string.IsNullOrEmpty(callee) ||
            !string.Equals(GetMethodDeclaringTypeSubjectId(callee), declaringTypeSubjectId, StringComparison.Ordinal) ||
            !GetMethodName(callee).StartsWith("GetCustomAttribute<", StringComparison.Ordinal) ||
            !GetMethodParameterTypes(callee).SequenceEqual(["System.Reflection.MemberInfo"]) ||
            !TryReadSingleGenericTypeArgument(callee, openGenericMethodPrefix, out var attributeTypeName))
        {
            return false;
        }

        attributeDisplayName = GetTypeDisplayName(attributeTypeName);
        return !string.IsNullOrEmpty(attributeDisplayName);
    }

    private static bool TryParseAttributeGetterMethodSubjectId(
        string? subjectId,
        out string? attributeTypeSubjectId,
        out string? memberName)
    {
        attributeTypeSubjectId = null;
        memberName = null;
        if (string.IsNullOrEmpty(subjectId) ||
            !subjectId.EndsWith("()", StringComparison.Ordinal))
        {
            return false;
        }

        var separatorIndex = subjectId.IndexOf("::get_", StringComparison.Ordinal);
        if (separatorIndex <= 0)
        {
            return false;
        }

        attributeTypeSubjectId = subjectId[..separatorIndex];
        memberName = subjectId.Substring(separatorIndex + "::get_".Length, subjectId.Length - separatorIndex - "::get_".Length - 2);
        return !string.IsNullOrEmpty(attributeTypeSubjectId) &&
               !string.IsNullOrEmpty(memberName);
    }

    private static bool TryGetAttributeTypeIdentity(
        MetadataReader metadataReader,
        string currentAssemblyName,
        CustomAttributeHandle attributeHandle,
        out MetadataTypeIdentity attributeTypeIdentity)
    {
        var attribute = metadataReader.GetCustomAttribute(attributeHandle);
        return TryResolveTypeIdentity(metadataReader, currentAssemblyName, attribute.Constructor, out attributeTypeIdentity);
    }

    private static bool TryResolveTypeIdentity(
        MetadataReader metadataReader,
        string currentAssemblyName,
        EntityHandle constructorOrTypeHandle,
        out MetadataTypeIdentity typeIdentity)
    {
        switch (constructorOrTypeHandle.Kind)
        {
            case HandleKind.MethodDefinition:
                var methodDefinition = metadataReader.GetMethodDefinition((MethodDefinitionHandle)constructorOrTypeHandle);
                return TryResolveTypeDefinitionIdentity(
                    metadataReader,
                    currentAssemblyName,
                    methodDefinition.GetDeclaringType(),
                    out typeIdentity);
            case HandleKind.MemberReference:
                var memberReference = metadataReader.GetMemberReference((MemberReferenceHandle)constructorOrTypeHandle);
                return TryResolveTypeIdentity(metadataReader, currentAssemblyName, memberReference.Parent, out typeIdentity);
            case HandleKind.TypeDefinition:
                return TryResolveTypeDefinitionIdentity(
                    metadataReader,
                    currentAssemblyName,
                    (TypeDefinitionHandle)constructorOrTypeHandle,
                    out typeIdentity);
            case HandleKind.TypeReference:
                return TryResolveTypeReferenceIdentity(
                    metadataReader,
                    currentAssemblyName,
                    (TypeReferenceHandle)constructorOrTypeHandle,
                    out typeIdentity);
            default:
                typeIdentity = default;
                return false;
        }
    }

    private static bool TryResolveTypeDefinitionIdentity(
        MetadataReader metadataReader,
        string currentAssemblyName,
        TypeDefinitionHandle handle,
        out MetadataTypeIdentity typeIdentity)
    {
        var typeDefinition = metadataReader.GetTypeDefinition(handle);
        var typeName = metadataReader.GetString(typeDefinition.Name);
        var namespaceName = metadataReader.GetString(typeDefinition.Namespace);
        var declaringTypeHandle = typeDefinition.GetDeclaringType();
        if (!declaringTypeHandle.IsNil &&
            TryResolveTypeDefinitionIdentity(metadataReader, currentAssemblyName, declaringTypeHandle, out var declaringTypeIdentity))
        {
            typeIdentity = declaringTypeIdentity with
            {
                TypeName = $"{declaringTypeIdentity.TypeName}+{typeName}",
            };
            return true;
        }

        typeIdentity = new MetadataTypeIdentity(currentAssemblyName, namespaceName, typeName);
        return true;
    }

    private static bool TryResolveTypeReferenceIdentity(
        MetadataReader metadataReader,
        string currentAssemblyName,
        TypeReferenceHandle handle,
        out MetadataTypeIdentity typeIdentity)
    {
        var typeReference = metadataReader.GetTypeReference(handle);
        var typeName = metadataReader.GetString(typeReference.Name);
        var namespaceName = metadataReader.GetString(typeReference.Namespace);
        switch (typeReference.ResolutionScope.Kind)
        {
            case HandleKind.AssemblyReference:
                var assemblyReference = metadataReader.GetAssemblyReference((AssemblyReferenceHandle)typeReference.ResolutionScope);
                typeIdentity = new MetadataTypeIdentity(
                    metadataReader.GetString(assemblyReference.Name),
                    namespaceName,
                    typeName);
                return true;

            case HandleKind.TypeReference:
                if (TryResolveTypeReferenceIdentity(
                        metadataReader,
                        currentAssemblyName,
                        (TypeReferenceHandle)typeReference.ResolutionScope,
                        out var declaringTypeIdentity))
                {
                    typeIdentity = declaringTypeIdentity with
                    {
                        TypeName = $"{declaringTypeIdentity.TypeName}+{typeName}",
                    };
                    return true;
                }

                break;

            case HandleKind.ModuleDefinition:
            case HandleKind.AssemblyDefinition:
                typeIdentity = new MetadataTypeIdentity(currentAssemblyName, namespaceName, typeName);
                return true;
        }

        typeIdentity = default;
        return false;
    }
}

internal sealed class NativeAotCustomAttributeTypeProvider : ICustomAttributeTypeProvider<string>
{
    public static readonly NativeAotCustomAttributeTypeProvider Instance = new();

    private NativeAotCustomAttributeTypeProvider()
    {
    }

    public string GetPrimitiveType(PrimitiveTypeCode typeCode)
    {
        return typeCode switch
        {
            PrimitiveTypeCode.Boolean => "bool",
            PrimitiveTypeCode.Byte => "byte",
            PrimitiveTypeCode.Char => "char",
            PrimitiveTypeCode.Double => "double",
            PrimitiveTypeCode.Int16 => "short",
            PrimitiveTypeCode.Int32 => "int",
            PrimitiveTypeCode.Int64 => "long",
            PrimitiveTypeCode.Object => "object",
            PrimitiveTypeCode.SByte => "sbyte",
            PrimitiveTypeCode.Single => "float",
            PrimitiveTypeCode.String => "string",
            PrimitiveTypeCode.UInt16 => "ushort",
            PrimitiveTypeCode.UInt32 => "uint",
            PrimitiveTypeCode.UInt64 => "ulong",
            _ => typeCode.ToString(),
        };
    }

    public string GetSystemType()
    {
        return "System.Type";
    }

    public string GetSZArrayType(string elementType)
    {
        return $"{elementType}[]";
    }

    public string GetTypeFromDefinition(MetadataReader reader, TypeDefinitionHandle handle, byte rawTypeKind)
    {
        return NativeAotCustomAttributeTypeNameResolver.GetTypeName(reader, handle);
    }

    public string GetTypeFromReference(MetadataReader reader, TypeReferenceHandle handle, byte rawTypeKind)
    {
        return NativeAotCustomAttributeTypeNameResolver.GetTypeName(reader, handle);
    }

    public string GetTypeFromSerializedName(string name)
    {
        return name;
    }

    public PrimitiveTypeCode GetUnderlyingEnumType(string type)
    {
        return PrimitiveTypeCode.Int32;
    }

    public bool IsSystemType(string type)
    {
        return string.Equals(type, "System.Type", StringComparison.Ordinal);
    }
}

internal static class NativeAotCustomAttributeTypeNameResolver
{
    public static string GetTypeName(MetadataReader metadataReader, TypeDefinitionHandle handle)
    {
        var typeDefinition = metadataReader.GetTypeDefinition(handle);
        var typeName = metadataReader.GetString(typeDefinition.Name);
        var namespaceName = metadataReader.GetString(typeDefinition.Namespace);
        var declaringTypeHandle = typeDefinition.GetDeclaringType();
        if (!declaringTypeHandle.IsNil)
        {
            return $"{GetTypeName(metadataReader, declaringTypeHandle)}+{typeName}";
        }

        return string.IsNullOrEmpty(namespaceName)
            ? typeName
            : $"{namespaceName}.{typeName}";
    }

    public static string GetTypeName(MetadataReader metadataReader, TypeReferenceHandle handle)
    {
        var typeReference = metadataReader.GetTypeReference(handle);
        var typeName = metadataReader.GetString(typeReference.Name);
        var namespaceName = metadataReader.GetString(typeReference.Namespace);
        if (typeReference.ResolutionScope.Kind == HandleKind.TypeReference)
        {
            return $"{GetTypeName(metadataReader, (TypeReferenceHandle)typeReference.ResolutionScope)}+{typeName}";
        }

        return string.IsNullOrEmpty(namespaceName)
            ? typeName
            : $"{namespaceName}.{typeName}";
    }
}

// ── Step 1-3: Dispatch, CodeRegistration structs, ReflectionQueryImage ───────────
public sealed partial class NativeAotLoweringPlanner
{
    // ── Shared hotpatch dispatch condition emission ───────────────────────
    // All dispatch decision points must use this helper so the condition
    // (flags check + kHotpatchKeepNative exclusion + acquire fence) is
    // consistent across all call sites.
    //
    // Emits:  if (HotpatchIsActive(entry) && !(entry.flags & kHotpatchKeepNative))
    //
    // With kHotpatchMakeActive and kHotpatchKeepNative:
    //   - kHotpatchActive (bit 0):  method has been patched, interpreter IR is ready
    //   - kHotpatchKeepNative (bit 1): method uses external runtime dispatch;
    //     interpreter cannot execute it, keep on native path even when patched
    private static void EmitHotpatchDispatchCondition(StringBuilder sb, string entryExpr, string indent = "    ")
    {
        sb.Append(indent).Append("if (chaos::il2cpp::runtime_core::HotpatchIsActive(")
          .Append(entryExpr).Append(")")
          .Append(" && !chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(")
          .Append(entryExpr).Append("))");
    }

    // ── Shared hotpatch entry reference emission ─────────────────────────
    private static void EmitHotpatchEntryRef(StringBuilder sb, string entryExpr, string indentation)
    {
        sb.Append(indentation).Append("auto& ").Append(entryExpr)
          .Append(" = s_hotpatch_entries[").Append(entryExpr).Append("];");
    }

    private string BuildDispatchEntryCode(IReadOnlyList<AotCoreIrMethodArtifact> methods)
    {
        if (methods.Count == 0) return string.Empty;

        ulong defaultStringId = _stringIdMapping is { Count: > 0 }
            ? _stringIdMapping.First().Value
            : 0UL;

        var methodEntries = new List<ScriptObject>(methods.Count);
        var subjectEntries = new List<ScriptObject>();

        // ── Phase 1 diagnostic: SubjectId match rate ──
        if (_subjectMethodSubjectIds is { Count: > 0 })
        {
            var subjectIdsInMethods = new HashSet<string>(methods.Select(m => m.SubjectId!),
                StringComparer.Ordinal);
            int matched = 0, missed = 0;
            foreach (var sid in _subjectMethodSubjectIds)
            {
                if (subjectIdsInMethods.Contains(sid))
                    matched++;
                else
                    missed++;
            }
            Console.Error.WriteLine($"[SUBJECT-MATCH] {matched} matched, {missed} missed " +
                $"(out of {_subjectMethodSubjectIds.Count} subject-methods, " +
                $"{methods.Count} methods in dispatch)");
            if (missed > 0)
            {
                // Log first 10 missed SubjectIds as samples
                int sampleCount = 0;
                foreach (var sid in _subjectMethodSubjectIds)
                {
                    if (!subjectIdsInMethods.Contains(sid))
                    {
                        if (sampleCount < 10)
                            Console.Error.WriteLine($"  [SUBJECT-MISS] {sid}");
                        sampleCount++;
                        if (sampleCount == 10)
                        {
                            Console.Error.WriteLine($"  [SUBJECT-MISS] ... and {missed - 10} more");
                            break;
                        }
                    }
                }
            }
        }
        for (int i = 0; i < methods.Count; i++)
        {
            var method = methods[i];
            var ac = method.ParameterCount;
            var isInstance = !method.IsStatic;

            var paramList = new List<ScriptObject>(ac + (isInstance ? 1 : 0));
            if (isInstance)
            {
                paramList.Add(new ScriptObject
                {
                    ["is_string"] = false,
                    ["is_this"] = true,
                });
            }
            for (int j = 0; j < ac; j++)
            {
                var abi = j < method.ParameterAbis.Count ? method.ParameterAbis[j] : null;
                var isString = abi != null && IsStringParameterSlot(abi);
                var isByRef = abi != null && (abi.CarrierKindCode == AotCoreIrAbiCarrierKind.ByRef
                    || abi.CarrierKindCode == AotCoreIrAbiCarrierKind.ByRefToValueType
                    || abi.CarrierKindCode == AotCoreIrAbiCarrierKind.MultiReturn);
                paramList.Add(new ScriptObject
                {
                    ["is_string"] = isString,
                    ["is_byref"] = isByRef,
                    ["is_this"] = false,
                });
            }

            var entry = new ScriptObject
            {
                ["index"] = i,
                ["native_symbol"] = method.NativeSymbol,
                ["param_count"] = ac + (isInstance ? 1 : 0),
                ["params"] = paramList,
                ["is_instance"] = isInstance,
            };

            methodEntries.Add(entry);

            if (IsSubjectMethod(method.SubjectId))
            {
            int subjectIdx = ExtractSubjectIndex(method.SubjectId);
                if (subjectIdx < 0)
                    subjectIdx = subjectEntries.Count; // sequential index for CombinedSubjects

                subjectEntries.Add(new ScriptObject
                {
                    ["subject_index"] = subjectIdx,
                    ["contract_index"] = subjectIdx,
                    ["method_index"] = i,
                });
            }
        }

        // Deduplicate subject entries by subject_index — each unique subject
        // produces both a wrapper and the actual method body (2 entries) but
        // the slot map should expose only one entry per subject (the wrapper).
        // Keeping both causes the fact runner to iterate 2× expected subjects
        // and the wrappers lack EH catch blocks, so noexcept violations in the
        // actual method propagate to the fact runner's outer catch as abort().
        var seenSubjectIndices = new HashSet<int>();
        var deduped = new List<ScriptObject>(subjectEntries.Count);
        foreach (var se in subjectEntries)
        {
            if (seenSubjectIndices.Add((int)se["subject_index"]))
                deduped.Add(se);
        }
        if (deduped.Count < subjectEntries.Count)
        {
            Console.Error.WriteLine($"[SUBJECT-DEDUP] Removed {subjectEntries.Count - deduped.Count} duplicate subject entries (wrapper+actual → one per subject)");
            subjectEntries = deduped;
        }

        // Filter subject entries to only reference method indices that have
        // corresponding dispatch entries in s_hotpatch_entries[].
        // GetHotpatchableMethods() (used by BuildHotpatchTable) returns fewer
        // entries than methodsForLowering — it excludes methods without IL bodies
        // and deduplicates by NativeSymbol. Raw method indices from the
        // methodsForLowering loop may exceed s_hotpatch_entries[] bounds, causing
        // STATUS_ACCESS_VIOLATION when the hotupdate loop dispatches those subjects.
        int actualEntryCount = HotpatchEntryCount;
        if (actualEntryCount > 0 && subjectEntries.Count > 0)
        {
            var filtered = new List<ScriptObject>(subjectEntries.Count);
            foreach (var se in subjectEntries)
            {
                if ((int)se["method_index"] < actualEntryCount)
                    filtered.Add(se);
            }
            if (filtered.Count < subjectEntries.Count)
            {
                Console.Error.WriteLine($"[SUBJECT-FILTER] Removed {subjectEntries.Count - filtered.Count} subject(s) with method_index >= {actualEntryCount}");
                subjectEntries = filtered;
            }
        }

        Console.Error.WriteLine($"[DISPATCH-DIAG] total methods: {methods.Count}, subject entries: {subjectEntries.Count}");
        if (subjectEntries.Count > 0)
        {
            Console.Error.WriteLine($"[DISPATCH-DIAG] first subject entry: method_index={subjectEntries[0]["method_index"]}, subject_index={subjectEntries[0]["subject_index"]}");
        }
        else if (methods.Count > 0)
        {
            // Show assembly distribution
            var assemblyCounts = methods
                .Select(m => m.SubjectId?.Split('/').FirstOrDefault() ?? "?")
                .GroupBy(a => a)
                .OrderByDescending(g => g.Count())
                .ToList();
            Console.Error.WriteLine($"[DISPATCH-DIAG] method SubjectId assembly distribution:");
            foreach (var g in assemblyCounts)
                Console.Error.WriteLine($"  {g.Key}: {g.Count()}");
            Console.Error.WriteLine($"[DISPATCH-DIAG] sample SubjectIds (first 5):");
            for (int si = 0; si < Math.Min(5, methods.Count); si++)
            {
                bool isSubj = IsSubjectMethod(methods[si].SubjectId);
                Console.Error.WriteLine($"  [{si}] subjectId={methods[si].SubjectId} isSubject={isSubj}");
            }
            // Check if ANY of the SubjectIds from the metadata match
            if (_subjectMethodSubjectIds is { Count: > 0 })
            {
                var matchedIds = methods
                    .Select(m => m.SubjectId)
                    .Where(id => _subjectMethodSubjectIds.Contains(id))
                    .ToList();
                Console.Error.WriteLine($"[DISPATCH-DIAG] SubjectId matches from metadata: {matchedIds.Count}");
                if (matchedIds.Count > 0)
                {
                    Console.Error.WriteLine($"[DISPATCH-DIAG] First match: {matchedIds[0]}");
                }
                else
                {
                    // Show one metadata SubjectId to compare
                    var sampleMetaId = _subjectMethodSubjectIds.First();
                    Console.Error.WriteLine($"[DISPATCH-DIAG] No matches. Sample metadata SubjectId: {sampleMetaId}");
                    Console.Error.WriteLine($"[DISPATCH-DIAG] Sample AOT Core IR SubjectId: {methods[0].SubjectId}");
                }
            }
        }

        var model = new ScriptObject
        {
            ["is_jit_mode"] = _codegenMode.HasFlag(CodegenMode.Jit),
            ["methods"] = methodEntries,
            ["methods_count"] = methods.Count,
            ["default_string_id"] = (long)defaultStringId,
            ["subject_entries"] = subjectEntries,
        };

        var template = NativeAotTemplateCatalog.GetDispatchEntryCodeTemplate();
        var cppCode = ScribanTemplateRenderer.RenderTemplate(template, model);

        // Build and store methods-manifest.json for Python verification dispatch generator.
        _manifestJson = BuildMethodsManifestJson(methods);

        return cppCode;
    }

    private string BuildMethodsManifestJson(IReadOnlyList<AotCoreIrMethodArtifact> methods)
    {
        ulong defaultStringId = _stringIdMapping is { Count: > 0 }
            ? _stringIdMapping.First().Value
            : 0UL;

        var manifestMethods = new List<Dictionary<string, object>>(methods.Count);
        for (int i = 0; i < methods.Count; i++)
        {
            var method = methods[i];
            var ac = method.ParameterCount;
            var isInstance = !method.IsStatic;

            var paramsList = new List<Dictionary<string, object>>(ac + (isInstance ? 1 : 0));
            if (isInstance)
            {
                paramsList.Add(new Dictionary<string, object>
                {
                    ["isString"] = false,
                    ["isThis"] = true,
                });
            }
            for (int j = 0; j < ac; j++)
            {
                var abi = j < method.ParameterAbis.Count ? method.ParameterAbis[j] : null;
                var isString = abi != null && IsStringParameterSlot(abi);
                paramsList.Add(new Dictionary<string, object>
                {
                    ["isString"] = isString,
                    ["isThis"] = false,
                });
            }

            int subjectIdx = -1;
            if (IsSubjectMethod(method.SubjectId))
                subjectIdx = ExtractSubjectIndex(method.SubjectId);
            var manifestMethod = new Dictionary<string, object>
            {
                ["index"] = i,
                ["nativeSymbol"] = method.NativeSymbol,
                ["subjectId"] = method.SubjectId,
                ["paramCount"] = ac + (isInstance ? 1 : 0),
                ["params"] = paramsList,
                ["isInstance"] = isInstance,
            };
            if (subjectIdx >= 0)
                manifestMethod["subjectIndex"] = subjectIdx;

            manifestMethods.Add(manifestMethod);
        }

        var manifest = new Dictionary<string, object>
        {
            ["schemaVersion"] = 2,
            ["assemblyName"] = _assemblyName,
            ["methodCount"] = methods.Count,
            ["defaultStringId"] = (long)defaultStringId,
            ["methods"] = manifestMethods,
        };

        return System.Text.Json.JsonSerializer.Serialize(manifest, new System.Text.Json.JsonSerializerOptions
        {
            WriteIndented = true,
            PropertyNamingPolicy = System.Text.Json.JsonNamingPolicy.CamelCase,
        });
    }

    /// <summary>
    /// Check whether a method is a subject method.
    /// When _subjectMethodSubjectIds is set (from --subject-methods), uses set-based
    /// lookup first (most specific); falls back to naming conventions for chunks
    /// where the constructed SubjectIds don't match AOT IR naming (e.g., when
    /// AutoTestGenerator's generatedMethodId differs from actual C# method names).
    /// </summary>
    private bool IsSubjectMethod(string subjectId)
    {
        // 0. Always exclude Benchmark_ wrappers — they call instance methods on
        //    null `this` without NullReferenceException handling, causing native
        //    crashes (STATUS_ACCESS_VIOLATION or STATUS_STACK_BUFFER_OVERRUN) in
        //    AOT dispatch.  The corresponding [Fact] variant wraps the same call
        //    in Assert.Throws<NullReferenceException> and is the correct
        //    correctness-verification entry point.
        if (subjectId.Contains("::Benchmark_", StringComparison.Ordinal))
            return false;

        // 1. When --subject-methods is provided, ONLY exact matches are valid.
        //    The CombinedSubjects prefix fallback below would also capture SDK
        //    infrastructure methods (Assert.AreEqual, ResultToLong, etc.) from
        //    the CombinedSubjects assembly, inflating kSubjectEntryCount to
        //    4000-5000 instead of the actual 200-300 Fact wrappers.
        if (_subjectMethodSubjectIds != null)
            return _subjectMethodSubjectIds.Contains(subjectId);

        // 2. CombinedSubjects methods: any method from the subject-assembly DLL
        //    (assembly name "CombinedSubjects") is a subject method by definition.
        //    Exclude constructors (.ctor, .cctor) and closures (<...>) — these are
        //    infrastructure methods generated by the compiler, not subject wrappers.
        if (subjectId.StartsWith("CombinedSubjects/", StringComparison.Ordinal))
        {
            if (subjectId.Contains(".ctor") || subjectId.Contains('<'))
                return false;
            return true;
        }

        // 3. Match ::Subject_N pattern (numbered subject wrappers).
        const string subjectPrefix = "::Subject_";
        int idx = subjectId.IndexOf(subjectPrefix, StringComparison.Ordinal);
        if (idx < 0) return false;
        int start = idx + subjectPrefix.Length;
        if (start >= subjectId.Length) return false;
        int end = start;
        while (end < subjectId.Length && char.IsAsciiDigit(subjectId[end]))
            end++;
        return end > start;
    }

    /// <summary>
    /// Extract subject index from a SubjectId string.
    /// Subject methods have SubjectIds ending like "::CustomEntrySubject_N:..." or "::Subject_N:...".
    /// Returns -1 if the SubjectId is not a subject method.
    /// </summary>
    private static int ExtractSubjectIndex(string subjectId)
    {
        const string customPrefix = "::CustomEntrySubject_";
        const string subjectPrefix = "::Subject_";

        // Try CustomEntrySubject_N first.
        int idx = subjectId.IndexOf(customPrefix, StringComparison.Ordinal);
        int prefixLen = customPrefix.Length;
        if (idx < 0)
        {
            idx = subjectId.IndexOf(subjectPrefix, StringComparison.Ordinal);
            prefixLen = subjectPrefix.Length;
        }
        if (idx < 0) return -1;

        int start = idx + prefixLen;
        if (start >= subjectId.Length) return -1;
        int end = start;
        while (end < subjectId.Length && char.IsAsciiDigit(subjectId[end]))
            end++;
        if (end == start) return -1;
        return int.Parse(subjectId.Substring(start, end - start), CultureInfo.InvariantCulture);
    }

    // ── Step 2: CodeRegistrationV0 + MetadataRegistrationV0 + CodegenRegistrationOptionsV0 ──
    // Emitted as extern "C" symbols for RegisterCodegen + BootstrapRuntime path.
    // References the generic registration arrays emitted by EmitGenericRegistration.

    private string EmitCodeRegistrationStructs(
    IReadOnlyList<AotCoreIrMethodArtifact> methods,
    MetadataRegistrationArtifact metadataRegistration,
    bool hasGcSlotMapSection = false)
{
    _ = metadataRegistration; // unused — kept to avoid changing callers
    if (methods.Count == 0) return string.Empty;

    var model = new ScriptObject
    {
        ["methods"] = methods
            .Select(m => new ScriptObject { ["native_symbol"] = m.NativeSymbol })
            .ToArray(),
        ["methods_count"] = methods.Count,
        ["reverse_pinvoke_count"] = _reversePInvokeEntries.Count,
        ["reverse_pinvoke_entries"] = _reversePInvokeEntries.Count > 0
            ? _reversePInvokeEntries
                .Select(e => new ScriptObject { ["native_symbol"] = e.NativeSymbol })
                .ToArray()
            : Array.Empty<ScriptObject>(),
        ["assembly_name"] = EscapeCppStringLiteral(_assemblyName),
        ["has_gc_slot_map_section"] = hasGcSlotMapSection,
    };

    // ── VTable descriptors for BootstrapRuntime TypeVTable registration ──
    // Always set (even when empty) to avoid Scriban "function not found" error.
    if (_vtableDescriptors is { Count: > 0 })
    {
        model["vtable_descriptors"] = _vtableDescriptors
            .Select(d => new ScriptObject
            {
                ["stable_id"] = "CHAOS_IL2CPP_UINT64_C(0x" + d.StableId.ToString("X16") + ")",
                ["type_token_literal"] = d.TypeTokenLiteral,
                ["base_token_literal"] = d.BaseTokenLiteral,
                ["slot_count"] = d.Slots.Length,
                ["slots_symbol"] = "kSlots_" + d.SanitizedId,
                ["vtable_array_symbol"] = d.VTableArraySymbol,
                ["vtable_length"] = d.VTableLength,
                ["type_shape"] = d.TypeShape,
                ["iface_map_symbol"] = d.IfaceMapSymbol ?? "nullptr",
                ["iface_count"] = d.IfaceCount,
            })
            .ToArray();
        model["vtable_descriptor_count"] = _vtableDescriptors.Count;
    }
    else
    {
        model["vtable_descriptors"] = Array.Empty<ScriptObject>();
        model["vtable_descriptor_count"] = 0;
    }

    return ScribanTemplateRenderer.RenderTemplate(
        NativeAotTemplateCatalog.GetCodeRegistrationTemplate(), model);
}

// ── Step 3: ReflectionQueryImageDescriptor ──────────────────────────────────
    // Emits ReflectionQueryMethodDescriptor[] and ReflectionQueryTypeDescriptor[]
    // arrays, and a ReflectionQueryImageDescriptor that module.image points to.
    // This enables ResolveSubjectId to find call_target via reflection query model.
    private string EmitReflectionQueryImage(
        IReadOnlyList<AotCoreIrMethodArtifact> methods,
        MetadataRegistrationArtifact metadataRegistration)
    {
        if (methods.Count == 0 || _moduleTypeSubjectIds.Count == 0) return string.Empty;

        var tokenLookup = new MetadataTokenLookup(metadataRegistration.Registrations);

        // Build type -> methods map grouped by declaring type.
        // Preserve the AotCoreIrMethodArtifact to extract ReturnType/ParameterCount.
        var typeMethodMap = new Dictionary<string, List<(AotCoreIrMethodArtifact Artifact, string TypeNs, string TypeName)>>(StringComparer.Ordinal);
        foreach (var method in methods)
        {
            string declaringType;
            try { declaringType = GetMethodDeclaringTypeSubjectId(method.SubjectId); }
            catch { continue; }

            if (!typeMethodMap.TryGetValue(declaringType, out var list))
            {
                list = new List<(AotCoreIrMethodArtifact, string, string)>();
                typeMethodMap[declaringType] = list;
            }

            list.Add((method, GetTypeNamespace(declaringType), GetTypeDisplayName(declaringType)));
        }

        if (typeMethodMap.Count == 0) return string.Empty;

        var typeIndexToMethods = new List<(string TypeSubjectId, int Count, string SafeName, string Ns, string Name)>();
        // Pre-collect field data for all types in the type method map by assembly.
        // This ensures field descriptor arrays can be emitted in the reflection query image.
        var typeFieldMap = new Dictionary<string, List<(string SubjectId, string Name, string Type, long Value, uint Token, uint Flags)>>(StringComparer.Ordinal);
        var typeMethodAttrMap = new Dictionary<string, Dictionary<uint, uint>>(StringComparer.Ordinal); // typeSubjectId → {methodToken → flags}
        var typeEventMap = new Dictionary<string, List<(string SubjectId, string Name, string Type)>>(StringComparer.Ordinal);
        var assemblyTypeQueue = new Dictionary<string, HashSet<string>>(StringComparer.Ordinal);
        foreach (var typeSubjectId in typeMethodMap.Keys)
        {
            var slashIdx = typeSubjectId.IndexOf('/');
            var assemblyName = slashIdx >= 0 ? typeSubjectId.Substring(0, slashIdx) : _assemblyName;
            if (!assemblyTypeQueue.TryGetValue(assemblyName, out var typeSet))
            {
                typeSet = new HashSet<string>(StringComparer.Ordinal);
                assemblyTypeQueue[assemblyName] = typeSet;
            }
            typeSet.Add(typeSubjectId);
        }

        foreach (var (assemblyName, typeIds) in assemblyTypeQueue)
        {
            if (!_closureAssemblyPathByName.TryGetValue(assemblyName, out var assemblyPath))
                continue;

            try
            {
                using var stream = new FileStream(assemblyPath, FileMode.Open, FileAccess.Read, FileShare.Read);
                using var peReader = new PEReader(stream);
                if (!peReader.HasMetadata) continue;
                var metadataReader = peReader.GetMetadataReader();

                foreach (var typeHandle in metadataReader.TypeDefinitions)
                {
                    var typeDef = metadataReader.GetTypeDefinition(typeHandle);
                    var typeNs = metadataReader.GetString(typeDef.Namespace);
                    var typeName = metadataReader.GetString(typeDef.Name);
                    var nsTypeName = string.IsNullOrEmpty(typeNs) ? typeName : typeNs + "." + typeName;
                    var candidateId = assemblyName + "/" + nsTypeName;

                    if (!typeIds.Contains(candidateId))
                        continue;

                    var fields = new List<(string SubjectId, string Name, string Type, long Value, uint Token, uint Flags)>();
                    foreach (var fieldHandle in typeDef.GetFields())
                    {
                        var fieldDef = metadataReader.GetFieldDefinition(fieldHandle);
                        var fieldName = metadataReader.GetString(fieldDef.Name);
                        uint fieldToken = (uint)MetadataTokens.GetToken(fieldHandle);

                        long fieldValue = 0;
                        var constHandle = fieldDef.GetDefaultValue();
                        if (!constHandle.IsNil)
                        {
                            try
                            {
                                var constant = metadataReader.GetConstant(constHandle);
                                var blobReader = metadataReader.GetBlobReader(constant.Value);
                                switch ((PrimitiveTypeCode)constant.TypeCode)
                                {
                                    case PrimitiveTypeCode.Boolean: fieldValue = blobReader.ReadBoolean() ? 1L : 0L; break;
                                    case PrimitiveTypeCode.Byte:    fieldValue = blobReader.ReadByte(); break;
                                    case PrimitiveTypeCode.SByte:   fieldValue = blobReader.ReadSByte(); break;
                                    case PrimitiveTypeCode.Int16:   fieldValue = blobReader.ReadInt16(); break;
                                    case PrimitiveTypeCode.UInt16:  fieldValue = blobReader.ReadUInt16(); break;
                                    case PrimitiveTypeCode.Char:    fieldValue = blobReader.ReadChar(); break;
                                    case PrimitiveTypeCode.Int32:   fieldValue = blobReader.ReadInt32(); break;
                                    case PrimitiveTypeCode.UInt32:  fieldValue = blobReader.ReadUInt32(); break;
                                    case PrimitiveTypeCode.Int64:   fieldValue = blobReader.ReadInt64(); break;
                                    case PrimitiveTypeCode.UInt64:  fieldValue = (long)blobReader.ReadUInt64(); break;
                                }
                            }
                            catch { }
                        }

                        // Compute field flags from PE FieldAttributes
                        var fa = fieldDef.Attributes;
                        uint fieldFlags = 0;
                        if ((fa & System.Reflection.FieldAttributes.FieldAccessMask) == System.Reflection.FieldAttributes.Public)
                            fieldFlags |= 1u << 0; // kFieldFlagIsPublic
                        if (fa.HasFlag(System.Reflection.FieldAttributes.Static))
                            fieldFlags |= 1u << 1; // kFieldFlagIsStatic
                        if (fa.HasFlag(System.Reflection.FieldAttributes.InitOnly))
                            fieldFlags |= 1u << 2; // kFieldFlagIsInitOnly
                        if (fa.HasFlag(System.Reflection.FieldAttributes.Literal))
                            fieldFlags |= 1u << 3; // kFieldFlagIsLiteral

                        var fieldSubjectId = candidateId + "::" + fieldName;
                        fields.Add((fieldSubjectId, fieldName, "System.Int32", fieldValue, fieldToken, fieldFlags));
                    }

                    typeFieldMap[candidateId] = fields;

                    // Collect method attributes for flag emission
                    var methodTokenFlags = new Dictionary<uint, uint>();
                    foreach (var methodHandle in typeDef.GetMethods())
                    {
                        var methodDef = metadataReader.GetMethodDefinition(methodHandle);
                        uint mToken = (uint)MetadataTokens.GetToken(methodHandle);
                        var ma = methodDef.Attributes;
                        uint methodFlags = 0;
                        if ((ma & System.Reflection.MethodAttributes.MemberAccessMask) == System.Reflection.MethodAttributes.Public)
                            methodFlags |= 1u << 0; // kMethodFlagIsPublic
                        if (ma.HasFlag(System.Reflection.MethodAttributes.Static))
                            methodFlags |= 1u << 1; // kMethodFlagIsStatic
                        if (ma.HasFlag(System.Reflection.MethodAttributes.Virtual))
                            methodFlags |= 1u << 2; // kMethodFlagIsVirtual
                        methodTokenFlags[mToken] = methodFlags;
                    }
                    typeMethodAttrMap[candidateId] = methodTokenFlags;

                    // Collect event data for this type
                    var events = new List<(string SubjectId, string Name, string Type)>();
                    foreach (var eventHandle in typeDef.GetEvents())
                    {
                        var eventDef = metadataReader.GetEventDefinition(eventHandle);
                        var eventName = metadataReader.GetString(eventDef.Name);
                        var eventSubjectId = candidateId + "::" + eventName;

                        // Resolve event handler type from the event's Type property
                        string eventType = "System.EventHandler";
                        try
                        {
                            var eventTypeHandle = eventDef.Type;
                            if (!eventTypeHandle.IsNil &&
                                TryResolveTypeIdentity(metadataReader, assemblyName, eventTypeHandle, out var typeIdentity))
                            {
                                eventType = typeIdentity.DisplayName;
                            }
                        }
                        catch { }

                        events.Add((eventSubjectId, eventName, eventType));
                    }
                    typeEventMap[candidateId] = events;
                }
            }
            catch { }
        }

        var typeGroups = new List<object>();
        foreach (var kvp in typeMethodMap)
        {
            var typeSubjectId = kvp.Key;
            var methodsInType = kvp.Value;
            var safeName = SanitizeCppIdentifier(typeSubjectId.Replace('/', '_').Replace(':', '_'));
            typeIndexToMethods.Add((typeSubjectId, methodsInType.Count, safeName, methodsInType[0].TypeNs, methodsInType[0].TypeName));

            uint typeToken = tokenLookup.TryGetTypeToken(typeSubjectId);

            var typeMethodAttrs = typeMethodAttrMap.TryGetValue(typeSubjectId, out var mta)
                ? mta : null;

            var methodEntries = methodsInType.Select(m =>
            {
                uint mToken = tokenLookup.TryGetMethodToken(m.Artifact.SubjectId);
                string returnType = m.Artifact.ReturnType ?? "System.Void";
                string methodName = GetMethodName(m.Artifact.SubjectId);
                uint methodFlags = (mToken > 0 && typeMethodAttrs != null && typeMethodAttrs.TryGetValue(mToken, out var mf)) ? mf : 0u;
                return new
                {
                    metadata_token_hex = mToken > 0 ? "0x" + mToken.ToString("X8") : "0u",
                    subject_id_literal = EscapeCppStringLiteral(m.Artifact.SubjectId),
                    method_name_literal = EscapeCppStringLiteral(methodName),
                    return_type_literal = EscapeCppStringLiteral(returnType),
                    parameter_count = m.Artifact.ParameterCount,
                    flags = methodFlags,
                };
            }).ToArray();

            var fieldEntries = typeFieldMap.TryGetValue(typeSubjectId, out var tFields)
                ? tFields.Select(f => new
                {
                    metadata_token_hex = f.Token > 0 ? "0x" + f.Token.ToString("X8") : "0u",
                    subject_id_literal = EscapeCppStringLiteral(f.SubjectId),
                    name_literal = EscapeCppStringLiteral(f.Name),
                    type_literal = EscapeCppStringLiteral(f.Type),
                    constant_value = f.Value,
                    flags = f.Flags,
                }).ToArray()
                : System.Array.Empty<object>();

            var eventEntries = typeEventMap.TryGetValue(typeSubjectId, out var tEvents)
                ? tEvents.Select(e => new
                {
                    subject_id_literal = EscapeCppStringLiteral(e.SubjectId),
                    name_literal = EscapeCppStringLiteral(e.Name),
                    type_literal = EscapeCppStringLiteral(e.Type),
                }).ToArray()
                : System.Array.Empty<object>();

            typeGroups.Add(new
            {
                metadata_token_hex = typeToken > 0 ? "0x" + typeToken.ToString("X8") : "0u",
                safe_name = safeName,
                method_count = methodsInType.Count,
                subject_id_literal = EscapeCppStringLiteral(typeSubjectId),
                namespace_literal = EscapeCppStringLiteral(methodsInType[0].TypeNs),
                name_literal = EscapeCppStringLiteral(methodsInType[0].TypeName),
                methods = methodEntries,
                fields = fieldEntries,
                events = eventEntries,
                event_count = eventEntries.Length,
            });
        }

        var typeGroupIndices = Enumerable.Range(0, typeIndexToMethods.Count).ToArray();

        var model = new ScriptObject
        {
            ["type_groups"] = typeGroups,
            ["type_group_count"] = typeIndexToMethods.Count,
            ["type_group_indices"] = typeGroupIndices,
            ["assembly_name_literal"] = EscapeCppStringLiteral(_assemblyName),
        };

        return ScribanTemplateRenderer.RenderTemplate(
            NativeAotTemplateCatalog.GetReflectionQueryImageTemplate(), model);
    }

/// <summary>
/// Generate the C++ header for pre-computed enum metadata tables.
/// Collects enum type data from ALL resolved assemblies (not just the entry
/// assembly) so that SPC enum types (DayOfWeek, BindingFlags, etc.) are
/// included alongside the entry assembly's own enum types.
/// Returns empty string if no enum types or field data are available.
/// </summary>
private string GenerateEnumMetadataHeader()
{
    // Start with entry assembly data (already collected in _moduleTypeFlags/SubjectIds).
    // Then augment with enum type data from all other resolved assemblies.
    // The seenSubjectIds set prevents duplicates when an assembly's types
    // overlap (e.g., when the entry assembly is also in resolved paths).
    var mergedFlags = new List<uint>(_moduleTypeFlags);
    var mergedSubjectIds = new List<string>(_moduleTypeSubjectIds);
    var seenSubjectIds = new HashSet<string>(_moduleTypeSubjectIds, StringComparer.Ordinal);
    // Collect field entries from PE metadata for all enum types (used when
    // _reflectionMemberSupport.FieldEntries is empty for stub-based families).
    var enumFieldEntries = new List<ReflectionMemberFieldEntry>();

    foreach (var assemblyPath in _cachedClosureAssemblyPaths)
    {
        try
        {
            CollectEnumTypesAndFieldsFromAssembly(assemblyPath, seenSubjectIds,
                mergedFlags, mergedSubjectIds, enumFieldEntries);
        }
        catch
        {
            // Skip assemblies that can't be read (e.g., native images, missing files)
            continue;
        }
    }

    if (mergedFlags.Count == 0)
        return string.Empty;

    // Prefer reflection member field entries when available (they include data
    // from all closure assemblies); fall back to PE-metadata field entries.
    var fieldEntries = _reflectionMemberSupport.FieldEntries.Count > 0
        ? _reflectionMemberSupport.FieldEntries
        : enumFieldEntries;

    if (fieldEntries.Count == 0)
        return string.Empty;

    var header = EnumMetadataExtractor.GenerateHeader(
        mergedFlags,
        mergedSubjectIds,
        fieldEntries);

    // Post-process: if the header generator places compute_enum_hash24
    // inside the chaos::il2cpp::codegen namespace (as newer versions of
    // EnumMetadataExtractor do), but the registration code that calls it
    // is generated OUTSIDE the namespace, inject a using-declaration so
    // the unqualified call compiles.
    const string namespaceClose = "}}}  // namespace chaos::il2cpp::codegen";
    int nsCloseIdx = header.IndexOf(namespaceClose, StringComparison.Ordinal);
    if (nsCloseIdx >= 0)
    {
        int callIdx = header.IndexOf("compute_enum_hash24(", nsCloseIdx, StringComparison.Ordinal);
        if (callIdx >= 0)
        {
            // The hash function is called outside its defining namespace.
            // Insert a using-declaration right after the namespace close.
            int insertPos = nsCloseIdx + namespaceClose.Length;
            // Skip past any trailing whitespace/newline
            while (insertPos < header.Length &&
                   (header[insertPos] == '\r' || header[insertPos] == '\n'))
                insertPos++;
            header = header[..insertPos] + "\n" +
                "using chaos::il2cpp::codegen::compute_enum_hash24;" +
                header[insertPos..];
        }
    }

    return header;
}

/// <summary>
/// Read assembly PE metadata and collect enum type flags, subjectIds,
/// AND field name/value entries for each enum type.
/// Populates the provided lists, skipping types already in seenSubjectIds.
/// </summary>
private static void CollectEnumTypesAndFieldsFromAssembly(
    string assemblyPath,
    HashSet<string> seenSubjectIds,
    List<uint> mergedFlags,
    List<string> mergedSubjectIds,
    List<ReflectionMemberFieldEntry> enumFieldEntries)
{
    if (string.IsNullOrEmpty(assemblyPath) || !File.Exists(assemblyPath))
        return;

    using var stream = File.OpenRead(assemblyPath);
    using var peReader = new System.Reflection.PortableExecutable.PEReader(stream);
    if (!peReader.HasMetadata)
        return;

    var metadataReader = peReader.GetMetadataReader();
    if (!metadataReader.IsAssembly)
        return;

    var assemblyName = metadataReader.GetString(metadataReader.GetAssemblyDefinition().Name);

    foreach (var handle in metadataReader.TypeDefinitions)
    {
        var typeDef = metadataReader.GetTypeDefinition(handle);
        var parentHandle = typeDef.BaseType;

        // Only interested in enum types
        if (parentHandle.IsNil)
            continue;

        var parentFullName = ResolveBaseTypeName(metadataReader, parentHandle);
        if (parentFullName == null ||
            !string.Equals(parentFullName, "System.Enum", StringComparison.Ordinal))
            continue;

        var subjectId = ComputeTypeDefSubjectId(metadataReader, handle, assemblyName);
        if (string.IsNullOrEmpty(subjectId) || !seenSubjectIds.Add(subjectId))
            continue;

        uint flags = ComputeTypeFlags(metadataReader, typeDef, parentHandle);
        mergedFlags.Add(flags);
        mergedSubjectIds.Add(subjectId);

        // Read field entries (names + constant values) from PE metadata
        foreach (var fieldHandle in typeDef.GetFields())
        {
            var fieldDef = metadataReader.GetFieldDefinition(fieldHandle);
            var fieldName = metadataReader.GetString(fieldDef.Name);

            // Skip the implicit "value__" instance field
            if (string.Equals(fieldName, "value__", StringComparison.Ordinal))
                continue;

            long? constantValue = ReadFieldConstantValue(metadataReader, fieldDef);
            if (!constantValue.HasValue)
                continue;

            enumFieldEntries.Add(new ReflectionMemberFieldEntry(
                subjectId, fieldName, MetadataTokens.GetToken(fieldHandle), constantValue));
        }
    }
}

/// <summary>
/// Fallback: scan PE metadata of all closure assemblies to collect enum field
/// entries (name + constant value) when _reflectionMemberSupport.FieldEntries
/// is empty (e.g. during foundation-dll codegen for stub-based families).
/// </summary>
private IReadOnlyList<ReflectionMemberFieldEntry> CollectEnumFieldEntriesFromMetadata()
{
    var seenSubjectIds = new HashSet<string>(_moduleTypeSubjectIds, StringComparer.Ordinal);
    var enumFieldEntries = new List<ReflectionMemberFieldEntry>();
    var dummyFlags = new List<uint>();
    var dummySubjectIds = new List<string>();

    foreach (var assemblyPath in _cachedClosureAssemblyPaths)
    {
        try
        {
            CollectEnumTypesAndFieldsFromAssembly(assemblyPath, seenSubjectIds,
                dummyFlags, dummySubjectIds, enumFieldEntries);
        }
        catch
        {
            continue;
        }
    }

    return enumFieldEntries;
}

/// <summary>
/// Read the constant value from a field definition's Constant metadata.
/// Returns null if the field has no constant or the type is unsupported.
/// </summary>
private static long? ReadFieldConstantValue(
    System.Reflection.Metadata.MetadataReader reader,
    System.Reflection.Metadata.FieldDefinition fieldDef)
{
    try
    {
        var constHandle = fieldDef.GetDefaultValue();
        if (constHandle.IsNil)
            return null;

        var constant = reader.GetConstant(constHandle);
        var blobReader = reader.GetBlobReader(constant.Value);

        // PrimitiveTypeCode matches the ECMA 335 constant type codes
        // used by System.Reflection.Metadata (not to be confused with
        // the unrelated ConstantTypeCode enum from a different namespace).
        switch ((System.Reflection.Metadata.PrimitiveTypeCode)constant.TypeCode)
        {
            case System.Reflection.Metadata.PrimitiveTypeCode.Boolean:
                return blobReader.ReadBoolean() ? 1L : 0L;
            case System.Reflection.Metadata.PrimitiveTypeCode.Byte:
                return blobReader.ReadByte();
            case System.Reflection.Metadata.PrimitiveTypeCode.SByte:
                return blobReader.ReadSByte();
            case System.Reflection.Metadata.PrimitiveTypeCode.Int16:
                return blobReader.ReadInt16();
            case System.Reflection.Metadata.PrimitiveTypeCode.UInt16:
                return blobReader.ReadUInt16();
            case System.Reflection.Metadata.PrimitiveTypeCode.Char:
                return blobReader.ReadChar();
            case System.Reflection.Metadata.PrimitiveTypeCode.Int32:
                return blobReader.ReadInt32();
            case System.Reflection.Metadata.PrimitiveTypeCode.UInt32:
                return blobReader.ReadUInt32();
            case System.Reflection.Metadata.PrimitiveTypeCode.Int64:
                return blobReader.ReadInt64();
            case System.Reflection.Metadata.PrimitiveTypeCode.UInt64:
                return unchecked((long)blobReader.ReadUInt64());
            default:
                return null;
        }
    }
    catch
    {
        return null;
    }
}
    /// <summary>
    /// Collect enum type subject IDs from reflection member support data (preferred)
    /// or fall back to scanning PE metadata of closure assemblies.
    /// </summary>
    private static IReadOnlySet<string> CollectEnumTypeSubjectIds(
        ReflectionMemberSupportModel reflectionMemberSupport,
        IReadOnlyList<string> closureAssemblyPaths)
    {
        // Prefer reflection member field entries when available
        if (reflectionMemberSupport.FieldEntries.Count > 0)
        {
            return new HashSet<string>(
                reflectionMemberSupport.FieldEntries.Select(f => f.DeclaringTypeSubjectId),
                StringComparer.Ordinal);
        }

        // Fallback: scan PE metadata of closure assemblies for enum types
        var enumSubjectIds = new HashSet<string>(StringComparer.Ordinal);
        var seenSubjectIds = new HashSet<string>(StringComparer.Ordinal);

        foreach (var assemblyPath in closureAssemblyPaths)
        {
            try
            {
                CollectEnumTypeSubjectIdsFromAssembly(assemblyPath, seenSubjectIds, enumSubjectIds);
            }
            catch
            {
                // Skip assemblies that can't be read
                continue;
            }
        }

        return enumSubjectIds;
    }

    /// <summary>
    /// Build a type → {value → name} map for all enum types in the closure.
    /// Prefers FieldEntries from reflection member support data; falls back
    /// to scanning PE metadata of closure assemblies.
    /// Used by S2 (BoxToString switch) and other value→name lookups.
    /// </summary>
    private Dictionary<string, Dictionary<long, string>> BuildEnumValueToNameMap()
    {
        var map = new Dictionary<string, Dictionary<long, string>>(StringComparer.Ordinal);

        IReadOnlyList<ReflectionMemberFieldEntry> fieldEntries;
        if (_reflectionMemberSupport.FieldEntries.Count > 0)
        {
            fieldEntries = _reflectionMemberSupport.FieldEntries;
        }
        else
        {
            fieldEntries = CollectEnumFieldEntriesFromMetadata();
        }

        foreach (var field in fieldEntries)
        {
            if (field.ConstantValue == null)
                continue;

            if (!map.TryGetValue(field.DeclaringTypeSubjectId, out var valueToName))
            {
                valueToName = new Dictionary<long, string>();
                map[field.DeclaringTypeSubjectId] = valueToName;
            }

            // First field with this value wins (consistent with Enum.GetName behavior).
            if (!valueToName.ContainsKey(field.ConstantValue.Value))
                valueToName[field.ConstantValue.Value] = field.FieldName;
        }

        return map;
    }

    /// <summary>
    /// Scan a single assembly's PE metadata for enum type definitions and
    /// add their subject IDs to the provided set.
    /// </summary>
    private static void CollectEnumTypeSubjectIdsFromAssembly(
        string assemblyPath,
        HashSet<string> seenSubjectIds,
        HashSet<string> enumSubjectIds)
    {
        if (string.IsNullOrEmpty(assemblyPath) || !File.Exists(assemblyPath))
            return;

        using var stream = File.OpenRead(assemblyPath);
        using var peReader = new System.Reflection.PortableExecutable.PEReader(stream);
        if (!peReader.HasMetadata)
            return;

        var metadataReader = peReader.GetMetadataReader();
        if (!metadataReader.IsAssembly)
            return;

        var assemblyName = metadataReader.GetString(metadataReader.GetAssemblyDefinition().Name);

        foreach (var handle in metadataReader.TypeDefinitions)
        {
            var typeDef = metadataReader.GetTypeDefinition(handle);
            var parentHandle = typeDef.BaseType;

            if (parentHandle.IsNil)
                continue;

            var parentFullName = ResolveBaseTypeName(metadataReader, parentHandle);
            if (parentFullName == null ||
                !string.Equals(parentFullName, "System.Enum", StringComparison.Ordinal))
                continue;

            var subjectId = ComputeTypeDefSubjectId(metadataReader, handle, assemblyName);
            if (string.IsNullOrEmpty(subjectId) || !seenSubjectIds.Add(subjectId))
                continue;

            enumSubjectIds.Add(subjectId);
        }
    }

    private void LogHotpatchCoverage()
    {
        if (HotpatchEligibleMethodCount <= 0)
            return;

        var pct = (double)HotpatchEntryCount / HotpatchEligibleMethodCount * 100;
        Console.Error.WriteLine(
            $"[hotpatch] dispatch coverage: {HotpatchEntryCount}/{HotpatchEligibleMethodCount} ({pct:F1}%)");

        if (HotpatchEntryCount < HotpatchEligibleMethodCount)
        {
            var coveredSymbols = new HashSet<string>(
                GetHotpatchableMethods().Select(m => m.NativeSymbol),
                StringComparer.Ordinal);
            var eligibleSymbols = new HashSet<string>(StringComparer.Ordinal);
            var missing = _methodsBySubjectId.Values
                .Where(m => (m.Instructions.Count > 0 || m.IsPInvoke) && eligibleSymbols.Add(m.NativeSymbol))
                .Where(m => !coveredSymbols.Contains(m.NativeSymbol))
                .Select(m => m.SubjectId)
                .Take(10)
                .ToList();
            if (missing.Count > 0)
            {
                Console.Error.WriteLine($"[hotpatch] missing {HotpatchEligibleMethodCount - HotpatchEntryCount} methods from dispatch table (first {missing.Count}):");
                foreach (var m in missing)
                    Console.Error.WriteLine($"  {m}");
            }
        }
    }
}

public sealed record NativeAotTemplateModel
{
    public required IReadOnlyList<string> Includes { get; init; }

    public required string ObjectModelCode { get; init; }

/// <summary>
/// Alternative to <see cref="ObjectModelCode"/>: a StringBuilder that holds the
/// same content but avoids the 2x memory overhead of ToString().  When set,
/// the emitter reads chunks via <c>GetChunks()</c> directly, eliminating the
/// need to allocate a second 3+ GB string on the Large Object Heap.
/// Only one of <see cref="ObjectModelCode"/> or <see cref="ObjectModelCodeBuilder"/>
/// must be non-null.
/// </summary>
public StringBuilder? ObjectModelCodeBuilder { get; init; }

    public required IReadOnlyList<string> MethodDeclarations { get; init; }

    public required IReadOnlyList<NativeAotMethodTemplateModel> Methods { get; init; }

    public required string EntrySubjectId { get; init; }

    public required string EntrySymbol { get; init; }

    public required string EntryNativeSymbol { get; init; }

    public required string NativeEntryFunctionName { get; init; }

    public required string EntryBridgeArguments { get; init; }

    public required string ShapeDispatchHeaderContent { get; init; }

    /// <summary>
    /// C++ header content for pre-computed enum metadata tables.
    /// Generated by EnumMetadataExtractor — enables runtime stubs to
    /// bypass reflection API for known enum types.
    /// Empty string when no enum types are found in the closure.
    /// </summary>
    public string EnumMetadataHeaderContent { get; init; } = "";

    /// <summary>
    /// C++ extern declarations for TypeInfoV0 symbols used across translation units.
    /// Emitted in the shared header when TU paging is active, so non-page-0 TUs
    /// can reference types defined in page 0 without ODR violations.
    /// Empty string when no types are emitted or paging is not active.
    /// </summary>
    public string TypeDeclarationsCode { get; init; } = "";

    public required string WorkloadAbi { get; init; }

    /// <summary>
    /// C++ code for generic registration helper (data arrays + init function).
    /// Emitted inside the named namespace alongside all other generated code.
    /// Empty string when there are no generic arrays to expose.
    /// </summary>
    public required string GenericRegistrationCode { get; init; }

    /// <summary>
    /// C++ code for per-DLL module registration via RegisterModule().
    /// Emitted inside the named namespace so the static initializer
    /// runs at module load time. Includes a ModuleDescriptor and the
    /// registration call. Empty string for audit/inventory plan kinds.
    /// </summary>
    public required string ModuleRegistrationCode { get; init; }

    /// <summary>
    /// C++ code emitted at file scope (outside the codegen namespace)
    /// for global variables shared across translation units. Currently
    /// used for the __chaos_assert_failures counter in verification builds.
    /// Empty string when no globals are needed.
    /// </summary>
    public string GlobalDeclarations { get; init; } = "";

    /// <summary>
    /// C++ code for the native entry function (RunNativeAot) emitted at file
    /// scope. Only included in the first translation unit page to avoid
    /// duplicate symbol errors across paged TUs.
    /// </summary>
    public string EntryFunctionCode { get; init; } = "";

/// <summary>
/// JSON manifest of methods for verification dispatch generation.
/// Consumed by generate_verification_dispatch.py outside core codegen.
/// Empty string when no methods are present.
/// </summary>
public string ManifestJson { get; init; } = "";

    /// <summary>
    /// C++ namespace for the generated translation unit.
    /// Derived from the entry native symbol to provide a unique, named
    /// scope for ODR protection across multi-TU builds. Follows the
    /// project's <c>chaos::il2cpp::codegen::*</c> convention.
    /// </summary>
    public string CodegenNamespace { get; init; } = "";

    /// <summary>
    /// C++ header content for the A1 typed dispatch table (chaos_generated_module.h).
    /// Contains typed function pointer arrays grouped by declaring type, enabling
    /// typed dispatch (ChaosRuntimeHost) instead of generic RunNativeAot() path.
    /// Empty string when module generation is not active.
    /// </summary>
    public string GeneratedModuleHeaderContent { get; init; } = "";

    /// <summary>
    /// C++ source content for the A2 dispatch wiring (chaos_generated_module.cpp).
    /// Contains extern "C" symbol wiring, proxy wrapper class (ChaosRuntimeHost)
    /// and ChaosGeneratedModuleActivate entry point.
    /// Empty string when module generation is not active.
    /// </summary>
    public string GeneratedModuleSourceContent { get; init; } = "";
}

public sealed record NativeAotMethodTemplateModel
{
    public required string SubjectId { get; init; }

    public required string MethodSource { get; init; }

    public string? NativeSymbol { get; init; }
}


