using System;
using System.Collections.Generic;
using System.Collections.Immutable;
using System.Diagnostics;
using System.Globalization;
using System.IO;
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
    /// When true, method emission runs sequentially (single-threaded).
    /// Set by --force-serial CLI flag.  Used as fallback if parallel
    /// emission causes heap corruption (0xC000037D).
    /// </summary>
    internal bool _forceSerial;

    /// <summary>
    /// Max degree of parallelism for method emission. Defaults to
    /// Environment.ProcessorCount - 2 (reserve cores for GC/OS).
    /// Set via CLI --parallelism N. Minimum 1.
    /// </summary>
    internal int _maxParallelism = Math.Max(1, Environment.ProcessorCount - 2);

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

    internal HashSet<string>? _emittedValueTypeSubjectIdsFromAbi;

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
    private HashSet<string>? _seenStructSymbols;

    /// <summary>
    /// Deduplication set for inline constexpr chaos_type_id_* symbols.
    /// When methods from multiple assemblies are merged into one translation unit,
    /// the same type ID (e.g. System.Object) may be emitted by each assembly.
    /// Prevents C2371 redefinition errors by emitting each type_id only once.
    /// </summary>
    private HashSet<string> _emittedTypeIds => _emittedTypeIdsCache ??= new(StringComparer.Ordinal);
    private HashSet<string>? _emittedTypeIdsCache;

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
    /// Suffix for unique symbol names in per-assembly modules (e.g. "_System_Collections_NonGeneric").
    /// Empty for the root assembly.  Appended to code_registration, metadata_registration,
    /// options, and activation function names to avoid linker conflicts with the root module.
    /// </summary>
    private string _assemblySuffix = string.Empty;

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
    private readonly object _reversePInvokeLock = new();
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

    /// <summary>Current method artifact, used by inlining budget checks.</summary>

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
    /// <summary>Set to true after first scan of aotCoreIr.Methods for external
    /// runtime subjects.  Prevents O(N) re-scan on consecutive runs.</summary>
    private bool _externalRuntimeSubjectsCached;

    /// <summary>
    /// Collects all chaos_external_runtime_* symbols referenced during method body emission.
    /// Populated by <see cref="EmitInvocation"/> when DirectNativeSymbol is set to an
    /// external runtime stub.  Used by <see cref="BuildTypeDeclarationsCode"/> to emit
    /// fallback static inline declarations for symbols the normal post-scan misses.
    /// Value is the return type's ABI carrier kind (needed to emit correct C++ return type).
    /// </summary>
    private readonly System.Collections.Concurrent.ConcurrentDictionary<string, AotCoreIrAbiCarrierKind> _emittedExternalRuntimeSymbols = new(System.StringComparer.Ordinal);

    /// <summary>
    /// Cache for TryCreateExternalRuntimeHelperDefinition results (P0 optimization).
    /// External runtime helper definitions are pure functions of the normalized subjectId;
    /// caching avoids redundant shape registry matching when the same callee is called
    /// from multiple methods.
    /// </summary>
    private readonly Dictionary<string, ExternalRuntimeHelperDefinition?> _externalRuntimeHelperCache
        = new(StringComparer.Ordinal);

    /// <summary>
    /// Cache for ManagedNaming.NormalizeSubjectIdAssembly — SubjectId assembly prefix
    /// normalization is pure (same input → same output), caching avoids repeated parsing.
    /// </summary>
    private readonly Dictionary<string, string> _normalizedSubjectIdCache = new(StringComparer.Ordinal);

    /// <summary>
    /// Path to the .jdata binary output file for JIT mode.
    /// When non-null, BuildJitMethodRegistration writes AotCoreIr JSON data
    /// to this binary file instead of embedding C++ string literals.
    /// Set before calling Create() by NativeAotEmitter.
    /// </summary>
    private string? _jitDataOutputPath;

    private string NormalizeSubjectIdAssemblyCached(string subjectId)
    {
        if (_normalizedSubjectIdCache.TryGetValue(subjectId, out var cached))
            return cached;
        return _normalizedSubjectIdCache[subjectId] = ManagedNaming.NormalizeSubjectIdAssembly(subjectId);
    }

    /// <summary>
    /// Bridge/import thunks: C++ wrapper functions for calls crossing the managed/native
    /// boundary. Populated by <see cref="CollectBridgeImportThunks"/> and emitted after
    /// all method bodies in Create.
    /// </summary>
    private Dictionary<string, BridgeImportThunkDefinition>? _bridgeImportThunks;

    // ── Gold Direct Link (A3) — profile-guided hot method direct call cache ──
    private HashSet<string> _goldDirectCallCache = new(StringComparer.Ordinal);

    public void LoadGoldDirectCallProfile(string profilePath)
    {
        if (!System.IO.File.Exists(profilePath))
        {
            System.Console.Error.WriteLine($"[GoldDirectLink] Profile not found: {profilePath}");
            return;
        }
        try
        {
            var json = System.IO.File.ReadAllText(profilePath);
            using var doc = System.Text.Json.JsonDocument.Parse(json);
            var root = doc.RootElement;
            if (root.TryGetProperty("hotMethods", out var methods) && methods.ValueKind == System.Text.Json.JsonValueKind.Array)
            {
                int count = 0;
                foreach (var method in methods.EnumerateArray())
                {
                    var callee = method.GetString();
                    if (!string.IsNullOrEmpty(callee)) { _goldDirectCallCache.Add(callee); count++; }
                }
                System.Console.Error.WriteLine($"[GoldDirectLink] Loaded {count} hot methods from {profilePath}");
            }
        }
        catch (System.Exception ex)
        {
            System.Console.Error.WriteLine($"[GoldDirectLink] Failed to load profile: {ex.Message}");
        }
    }

    public void AddGoldDirectCall(string calleeSubjectId)
    {
        if (!string.IsNullOrEmpty(calleeSubjectId))
            _goldDirectCallCache.Add(calleeSubjectId);
    }

    /// <summary>
    /// Set the output path for the JIT .jdata binary file.
    /// When set, BuildJitMethodRegistration writes AotCoreIr JSON data
    /// to this file instead of embedding C++ string literals, greatly
    /// reducing generated C++ source size and compilation time.
    /// </summary>
    public void SetJitDataOutputPath(string jitDataOutputPath)
    {
        _jitDataOutputPath = jitDataOutputPath;
    }

    public NativeAotTemplateModel Create(
        NativeAotLoweringPlanArtifact loweringPlan,
        AotCoreIrArtifact aotCoreIr,
        AotCoreIrMethodArtifact entryMethod,
        ManagedClosureManifestArtifact closureManifest,
        MetadataRegistrationArtifact metadataRegistration,
        SupplementalMetadataTemplateArtifact supplementalMetadataTemplate,
        bool fullAssemblyMode = false,
        CodegenMode mode = CodegenMode.Aot,
        List<string>? subjectMethods = null,
        IReadOnlyDictionary<string, ManagedMethodModel>? allManagedMethods = null)
    {
        ArgumentNullException.ThrowIfNull(loweringPlan);
        ArgumentNullException.ThrowIfNull(aotCoreIr);
        ArgumentNullException.ThrowIfNull(closureManifest);
        ArgumentNullException.ThrowIfNull(metadataRegistration);
        ArgumentNullException.ThrowIfNull(supplementalMetadataTemplate);

        if (!fullAssemblyMode)
            ArgumentNullException.ThrowIfNull(entryMethod);

        // ── Namespace filter: pre-filter AOT IR methods ──
        // When set, only keep methods whose declaring type's portion of the SubjectId
        // (after the assembly prefix) matches one of the comma-separated prefixes.
        IReadOnlyList<AotCoreIrMethodArtifact> filteredMethods = aotCoreIr.Methods;
        if (!string.IsNullOrEmpty(NamespaceFilter))
        {
            var nsPrefixes = NamespaceFilter.Split(',', StringSplitOptions.RemoveEmptyEntries | StringSplitOptions.TrimEntries);
            if (nsPrefixes.Length > 0)
            {
                var filtered = new List<AotCoreIrMethodArtifact>(aotCoreIr.Methods.Count);
                foreach (var m in aotCoreIr.Methods)
                {
                    if (m.SubjectId is null) continue;
                    int slashIdx = m.SubjectId.IndexOf('/');
                    if (slashIdx < 0) continue;
                    var typeName = m.SubjectId.Substring(slashIdx + 1);
                    bool match = false;
                    foreach (var prefix in nsPrefixes)
                    {
                        if (typeName.StartsWith(prefix, StringComparison.Ordinal))
                        { match = true; break; }
                    }
                    if (match) filtered.Add(m);
                }
                Console.Error.WriteLine($"[namespace-filter] kept {filtered.Count}/{aotCoreIr.Methods.Count} methods (filter: {NamespaceFilter})");
                filteredMethods = filtered;
            }
        }

        // Clear per-run collision tracking for pseudo-metadata handles.
        // Each codegen invocation (chunk) starts with a clean hash space.
        _usedPseudoMetadataHandles.Clear();

        _codegenMode = mode;
        _subjectMethodSubjectIds = subjectMethods; // keep List order for correct slot map
        _allManagedMethods = allManagedMethods;

        // Skip entry ABI validation for full-closure assembly translation
        if (!closureManifest.FullAssemblyClosure)
            ValidateEntryMethod(entryMethod);
        if (!fullAssemblyMode &&
            !string.Equals(entryMethod.NativeSymbol, loweringPlan.EntrySymbol, StringComparison.Ordinal))
        {
            throw new InvalidOperationException(
                $"native-aot entry symbol '{loweringPlan.EntrySymbol}' does not match aot-core-ir symbol '{entryMethod.NativeSymbol}'");
        }

        var methodsDict = new Dictionary<string, AotCoreIrMethodArtifact>(filteredMethods.Count, StringComparer.Ordinal);
        foreach (var m in filteredMethods)
            methodsDict.TryAdd(m.SubjectId, m);
        _methodsBySubjectId = methodsDict;
        // Build index of types with at least one instance method (O(m) once, then O(1) per type lookup).
        foreach (var method in filteredMethods)
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
        // Compute unique suffix for per-assembly modules.  The root assembly
        // (CombinedSubjects) keeps empty suffix; target assemblies get a
        // suffix so their symbols don't conflict at link time.
        _assemblySuffix = string.Equals(_assemblyName, "CombinedSubjects", StringComparison.Ordinal)
            ? ""
            : "_" + SanitizeCppIdentifier(_assemblyName);

        // Build module-local symbol table: all methods in this codegen output
        // that belong to the current assembly get a subjectId → nativeSymbol
        // entry so EmitLinearCall can detect same-module callees and emit
        // direct C++ calls instead of routing through the extern table.
        // In flat-merge mode (all assemblies in one TU), all methods are included.
        _moduleSymbolTable = _methodsBySubjectId
            .Where(kvp => kvp.Value is { Instructions.Count: > 0 })
            .ToDictionary(kvp => kvp.Key, kvp => kvp.Value.NativeSymbol, StringComparer.Ordinal);

        // Build reverse symbol table for inlining SubjectId resolution.
        // Use safe-dedup: when multiple methods produce the same NativeSymbol
        // (e.g., from closure assemblies loaded alongside the entry assembly),
        // keep the first occurrence rather than crashing.
        _nativeSymbolToSubjectId = new Dictionary<string, string>(_moduleSymbolTable.Count, StringComparer.Ordinal);
        foreach (var kvp in _moduleSymbolTable)
        {
            if (!_nativeSymbolToSubjectId.ContainsKey(kvp.Value))
                _nativeSymbolToSubjectId[kvp.Value] = kvp.Key;
        }

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

        // Start with methods reachable from the entry point
        var methodsForLowering = fullAssemblyMode
            ? CollectAllMethods(aotCoreIr, null)  // all methods in full assembly mode
            : CollectReachableMethods(aotCoreIr, entryMethod);

        // In subject mode (--subject-methods), also include subject wrapper methods and
        // their transitive target assembly callees.  Subject wrappers are not reachable
        // from the synthetic entry point but must get real AOT bodies.
        if (_subjectMethodSubjectIds is { Count: > 0 } && !fullAssemblyMode)
        {
            // Collect all subject wrappers (target assembly + CombinedSubjects)
            var extraMethods = new List<AotCoreIrMethodArtifact>();
            foreach (var m in aotCoreIr.Methods)
            {
                if (_subjectMethodSubjectIds.Contains(m.SubjectId))
                    extraMethods.Add(m);
                // Also include CombinedSubjects wrapper methods (these have different
                // subjectIds from the target assembly methods in _subjectMethodSubjectIds)
                if (m.SubjectId is { } sid && sid.StartsWith("CombinedSubjects/", StringComparison.Ordinal))
                    extraMethods.Add(m);
            }
            // Also include their transitive callees from target assemblies
            var seeds = extraMethods.Select(m => m.SubjectId).ToArray();
            var transitiveIds = ComputeAotReachableSubjectIds(
                null, aotCoreIr.Methods, seeds);
            foreach (var m in aotCoreIr.Methods)
            {
                if (transitiveIds.Contains(m.SubjectId) && !methodsForLowering.Any(mm => mm.SubjectId == m.SubjectId))
                    extraMethods.Add(m);
            }
            ((List<AotCoreIrMethodArtifact>)methodsForLowering).AddRange(extraMethods);
        }

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

        // Compute hotpatch coverage statistics — number of unique NativeSymbols
        // eligible for hotpatch (deduplicated).  GetHotpatchableMethods() no longer
        // deduplicates (to keep s_hotpatch_entries aligned with kMethodTable), so
        // HotpatchEntryCount may exceed HotpatchEligibleMethodCount when methods
        // share NativeSymbols (shared generic instantiations).
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
            // Also force-include subject methods with 0 instructions. These are
            // CombinedSubjects wrappers whose C# body was optimized away or whose
            // compilation produced empty IL (e.g., methods with only Assert.Throws
            // where the VERIFY-enabled body was reduced to nothing).  Without this,
            // they get a CHAOS_IL2CPP_FAIL stub that aborts at runtime.
            foreach (var m in methodsForLowering)
            {
                if (_subjectMethodSubjectIds.Contains(m.SubjectId) && m.Instructions.Count == 0)
                {
                    if (aotReachableSubjectIds.Add(m.SubjectId))
                    {
                        forcedCount++;
                        Console.Error.WriteLine($"[SUBJECT-EMPTY] {m.SubjectId} has 0 IL instructions — will emit return-only body");
                    }
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
            _methodsBySubjectId, _referenceTypeBaseSubjectIds, _referenceTypeImplementedInterfaceSubjectIds!);
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
        var objectModelBuilder = StringBuilderPool.Rent(65536);
        EmitRuntimePrelude(objectModelBuilder, externalRuntimeHelpers, _staticFieldDataSupport);
        EmitObjectModelDeclarations(objectModelBuilder, methodsForLowering, externalRuntimeHelpers, metadataRegistration);
        // Collect static field references from hotpatchable methods not in methodsForLowering.
        // These methods appear in the hotpatch dispatch table but are not emitted as AOT code.
        // Without extern declarations for their referenced static fields, the linker fails with
        // C2065 (undefined symbol) when page files reference chaos_static_* symbols.
        var loweredIds = new HashSet<string>(StringComparer.Ordinal);
        foreach (var m in methodsForLowering)
            if (m.SubjectId is { Length: > 0 } sid)
                loweredIds.Add(sid);
        foreach (var hotpatchMethod in GetHotpatchableMethods())
        {
            if (!loweredIds.Contains(hotpatchMethod.SubjectId))
            {
                if (hotpatchMethod.Instructions is null) continue;
                foreach (var instr in hotpatchMethod.Instructions)
                {
                    if (instr?.TargetReference is { } targetRef &&
                        targetRef.FieldTypeSubjectId is { Length: > 0 } ftsid &&
                        !string.IsNullOrEmpty(targetRef.SubjectId))
                    {
                        _staticFieldDeclarations ??= new Dictionary<string, string?>(StringComparer.Ordinal);
                        _staticFieldDeclarations.TryAdd(targetRef.SubjectId, ftsid);
                    }
                }
            }
        }
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
        // Also skip crypto methods from C++ emission (AotCoreIr JSON only).
        var cryptoSubjectIds = new HashSet<string>(StringComparer.Ordinal);
        {
            var cryptoFiltered = new List<AotCoreIrMethodArtifact>(emitMethods.Count);
            foreach (var m in emitMethods)
            {
                if (m.SubjectId.StartsWith("System.Security.Cryptography/", System.StringComparison.Ordinal))
                {
                    cryptoSubjectIds.Add(m.SubjectId);
                    continue;
                }
                cryptoFiltered.Add(m);
            }
            if (cryptoFiltered.Count < emitMethods.Count)
            {
                Console.Error.WriteLine($"[CRYPTO-SKIP] Skipped {emitMethods.Count - cryptoFiltered.Count} crypto methods");
                emitMethods = cryptoFiltered;
            }
        }
        var allMethods = new NativeAotMethodTemplateModel[emitMethods.Count];
        if (_forceSerial)
        {
            for (int i = 0; i < emitMethods.Count; i++)
                allMethods[i] = EmitOneMethod(emitMethods[i], aotReachableSubjectIds);
        }
        else
        {
            int dop = Math.Max(1, _maxParallelism);
            System.Threading.Tasks.Parallel.For(0, emitMethods.Count,
                new System.Threading.Tasks.ParallelOptions { MaxDegreeOfParallelism = dop },
                i => allMethods[i] = EmitOneMethod(emitMethods[i], aotReachableSubjectIds));
        }
        List<NativeAotMethodTemplateModel> methods = new List<NativeAotMethodTemplateModel>(allMethods);

        _tPhase4 = _sw.ElapsedMilliseconds;
        long phase4Ms = _tPhase4 - _tPhase3;
        if (_forceSerial)
            System.Console.Error.WriteLine($"[PARALLEL] SERIAL mode: {emitMethods.Count} methods in {phase4Ms}ms ({phase4Ms / Math.Max(1, emitMethods.Count)} ms/method)");
        else
            System.Console.Error.WriteLine($"[PARALLEL] PARALLEL mode DOP={Math.Max(1, _maxParallelism)}: {emitMethods.Count} methods in {phase4Ms}ms ({phase4Ms / Math.Max(1, emitMethods.Count)} ms/method)");

        // Capture pc-dispatch count from the static counter.
        // Incremented during BuildMethodSourceSafe → EmitViaStructuredIR → EmitPcDispatch.
        PcDispatchCount = (int)Interlocked.Read(ref s_pcDispatchCount);


        var entryBridgeArguments = fullAssemblyMode ? "" : BuildEntryBridgeArguments(entryMethod!);

        var abiManifestCode = BuildAbiManifest(methodsForLowering);
        // Parallel build for independent dispatch/registration data.
        // BuildHotpatchTable, BuildExternalRuntimeDispatchTable, BuildGcSlotMapSection
        // are independent of each other (no shared mutable state) and only read
        // methodsForLowering / metadataRegistration (read-only after lowering).
        var cryptoAotIrCode = BuildCryptoAotIrCode();
        string nameIndexCode = "", externalRuntimeTableCode = "", gcSlotMapCode = "";
        System.Threading.Tasks.Parallel.Invoke(
            () => { nameIndexCode = BuildHotpatchTable(methodsForLowering, metadataRegistration); },
            () => { externalRuntimeTableCode = BuildExternalRuntimeDispatchTable(
                helperSymbolBySubjectId: externalRuntimeHelpers?
                    .Where(h => !string.IsNullOrEmpty(h.TargetSymbol))
                    .ToDictionary(h => h.SubjectId, h => h.TargetSymbol, StringComparer.Ordinal)); },
            () => { gcSlotMapCode = BuildGcSlotMapSection(methodsForLowering); }
        );
        var moduleRegistrationCode = BuildModuleRegistration();
        var moduleRegSb = StringBuilderPool.Rent(65536);
        moduleRegSb.Append(moduleRegistrationCode);
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
        if (!string.IsNullOrEmpty(cryptoAotIrCode))
        {
            moduleRegSb.Append(Environment.NewLine);
            moduleRegSb.Append(cryptoAotIrCode);
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

        // Step 1.5: Emit GC slot map section (already computed in parallel above).
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

        // Emit kCodegenFailureCount as a C++ extern so the TPG can detect
        // when methods were silently replaced with stubs during codegen.
        if (CodegenFailureCount > 0)
        {
            globalDeclarations += $"// Codegen stub count — pipeline checks this\nextern \"C\" const int kCodegenFailureCount = {CodegenFailureCount};\n";
        }

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
            // Include jit_data_loader.h at file scope (outside function) when .jdata file is used.
            if (_jitDataOutputPath != null)
                globalDeclarations += "#include \"jit_data_loader.h\"\n";
            globalDeclarations += "\n" + BuildJitMethodRegistration(methodsForLowering, metadataRegistration, _jitDataOutputPath);

            // Forward declaration for the hotpatch module symbol (used in
            // ChaosJitRegisterAll below).  Defined elsewhere in the same
            // translation unit by the module registration code.
            globalDeclarations += "extern \"C\" const HotpatchModuleV0* chaos_il2cpp_aot_hotpatch_module;\n";

            // Build ChaosJitRegisterAll body.
            // When using .jdata file, load it at startup and pass the pointer.
            string registerBodyClose = "}";
            string registerBody;
            if (_jitDataOutputPath != null)
            {
                string jitDataFilename = Path.GetFileName(_jitDataOutputPath);
                registerBody = $@"
    // Load JIT method data from {jitDataFilename} file.
    // Phase 2: switched to ABI table
    chaos_runtime_get_abi_v0()->register_hotpatch_module(chaos_il2cpp_aot_hotpatch_module);
    uint64_t jit_data_size = 0;
    void* jit_data = ChaosJitDataLoad(""{jitDataFilename}"", &jit_data_size);
    RegisterJitEntryMethods(kChaosJitEntries, kChaosJitEntryCount,
                            static_cast<const char*>(jit_data));
" + registerBodyClose;
            }
            else
            {
                registerBody = @"
    // Phase 2: switched to ABI table
    chaos_runtime_get_abi_v0()->register_hotpatch_module(chaos_il2cpp_aot_hotpatch_module);
    RegisterJitEntryMethods(kChaosJitEntries, kChaosJitEntryCount);
" + registerBodyClose;
            }
            globalDeclarations += $@"
extern ""C"" void ChaosJitRegisterAll() {{
{registerBody}
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
            if (_externalRuntimeSubjects is { Count: > 0 })
                namespacePreamble.AppendLine("extern \"C\" CHAOS_IL2CPP_INT32 kChaosExternalRuntimeCount;");
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

        // Ensure _externalRuntimeSubjects includes callees from ALL methods
        // (not just methodsForLowering, which may exclude filtered methods).
        // Missing entries cause C3861 in the generated header.
        // Also collect static field declarations from all methods
        // for chaos_static_* extern declarations.
        // Skip if we already scanned (cached flag) to avoid O(N) re-scan
        // on every codegen run — for large chunks with 5000+ methods this
        // saves 500K+ iterations during the registration_dispatch phase.
        if (_externalRuntimeSubjectsCached != true)
        {
            _externalRuntimeSubjectsCached = true;
            var _seen = new HashSet<string>(StringComparer.Ordinal);
            int _nextIdx = _externalRuntimeSubjects.Count;
            foreach (var _method in aotCoreIr.Methods)
            {
                foreach (var _inst in _method.Instructions ?? [])
                {
                    // ExternalRuntime callees — register all cross-assembly call targets
                    // in the dispatch table so extern "C" declarations are emitted for
                    // their chaos_external_runtime_* symbols.  Without this, calls that
                    // the invocation planning resolves via TargetSymbol derivation would
                    // reference undeclared symbols, causing C3861 at compile time.
                    string? _callee = _inst.Callee ?? _inst.TargetReference?.SubjectId;
                    if (!string.IsNullOrEmpty(_callee) && _seen.Add(_callee))
                    {
                        // Skip type-only subjects (no ::method:) — these are JIT type-loading
                        // references (constrained., runtime type resolution, etc.) that should
                        // NOT be in the external runtime dispatch table.  Including them
                        // causes interpreter crashes in ChaosExternalRuntimeFallback.
                        if (!_callee.Contains("::"))
                            continue;
                        string normCallee = ManagedNaming.NormalizeSubjectIdAssembly(_callee);
                        if (!_externalRuntimeSubjects.ContainsKey(normCallee))
                        {
                            _externalRuntimeSubjects[normCallee] = _nextIdx++;
                        }
                    }
                    // Static field references (for chaos_static_* declarations)
                    if (_inst?.TargetReference is { } _tr &&
                        !string.IsNullOrEmpty(_tr.SubjectId))
                    {
                        _staticFieldDeclarations ??= new Dictionary<string, string?>(StringComparer.Ordinal);
                        _staticFieldDeclarations.TryAdd(_tr.SubjectId, _tr.FieldTypeSubjectId);
                    }
                }
            }
        }

        // Build A1 typed dispatch table header + A2 dispatch wiring source.
        // These are emitted as separate files (chaos_generated_module.h/.cpp) for
        // typed dispatch via ChaosRuntimeHost. Empty when methodsForLowering is empty.
        var moduleHeader = BuildGeneratedModuleHeader(methodsForLowering, objectModelBuilder.ToString(), _assemblySuffix);
        var moduleSource = BuildGeneratedModuleSource(methodsForLowering, objectModelBuilder.ToString(), _assemblySuffix);

        // Build include list — stable runtime headers go into chaos_pch.h
        // (precompiled header). Only conditional/per-run headers are here.
        var includes_ = new List<string>
        {
            // PCH provides: all <chaos/*.h>, runtime_core.h, codegen_bridge.h,
            //   module_registry.h, abi_manifest.h, hotpatch_table.h, runtime_vtable.h,
            //   runtime_instantiation.h, reflection_query_model.h,
            //   load_store_chaos_bridge.h, interpreter_entry.h, gc/*.h,
            //   ChaosGeneratedRuntimePrelude.h, runtime_stubs/*.h,
            //   chaos_runtime_host.h, gc/gc_layout.h
            "\"chaos_pch.h\"",
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
            externalRuntimeHelpers!
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

        // Deduplicate inline constexpr chaos_type_id_* and MethodTable chaos_mt_* symbols.
        // When methods from multiple assemblies are merged into one translation unit,
        // the same type/MT symbol can be emitted multiple times with identical value,
        // causing C2374/C2086 errors.  Remove all but the first occurrence.
        if (objectModelCode is { Length: > 0 })
        {
            var _dedup = new System.Collections.Generic.HashSet<string>(StringComparer.Ordinal);
            var _sb2 = new System.Text.StringBuilder(objectModelCode.Length);
            foreach (string _line in objectModelCode.Split('\n'))
            {
                string _t = _line.Trim();
                if ((_t.StartsWith("inline constexpr CHAOS_IL2CPP_UINT64 chaos_type_id_") ||
                     _t.StartsWith("inline constexpr CHAOS_IL2CPP_UINT64 chaos_mt_") ||
                     _t.StartsWith("inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_") ||
                     _t.StartsWith("inline constexpr CHAOS_IL2CPP_INTPTR chaos_mt_") ||
                     _t.StartsWith("MethodTable chaos_mt_")))
                {
                    // Extract the symbol name (between 'chaos_type_id_' or 'chaos_mt_' and ' =')
                    int _eq = _t.IndexOf(" =");
                    if (_eq > 0)
                    {
                        string _sym = _t.Substring(0, _eq);
                        if (!_dedup.Add(_sym)) continue;
                    }
                }
                _sb2.AppendLine(_line);
            }
            objectModelCode = _sb2.ToString().TrimEnd();
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
            Methods = allMethods,
            EntrySubjectId = loweringPlan.EntrySubjectId,
            EntrySymbol = loweringPlan.EntrySymbol,
            EntryNativeSymbol = entryMethod!.NativeSymbol,
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

    /// <summary>Remove duplicate struct/boxed-type definitions from C++ code.
    /// FSharp.Core type forwarding may produce the same chaos_type_* symbol
    /// from different SubjectIds. Keeps the first definition, removes subsequent
    /// ones along with their body lines.</summary>

    /// <summary>Collect all chaos_type_ / chaos_boxed_type_ symbols from a struct definition string.
    /// Used to seed _seenStructSymbols so the inline loop doesn't re-emit struct definitions
    /// that were already emitted via _referenceTypeStructCode.</summary>

    /// <summary>
    /// Phase 0: Collect ModuleRegistry Tier 0 type data from the assembly PE metadata.
    /// Populates _moduleTypeFlags, _moduleTypeNames, _moduleTypeNamespaces,
    /// _moduleTypeParentTokens, _moduleTypeInfoSymbols indexed by TokenToIndex(token).
    /// On failure (e.g., missing assembly), Tier 0 arrays remain empty — the runtime
    /// falls back to Tier 2 metadata (ReflectionQueryImageDescriptor).
    /// </summary>

    /// <summary>
    /// Resolve a TypeDef/TypeRef parent handle to a "Namespace.Name" string.
    /// Returns null for nil handles or unsupported handle kinds (TypeSpec).
    /// </summary>

    /// <summary>
    /// Compute the type-flags bitmask for a TypeDef entry.
    /// Flags correspond to the kFlag* constants in module_registry.h.
    /// </summary>

    /// <summary>
    /// Compute the SubjectId for a TypeDef from its metadata row.
    /// Format: "AssemblyName/Namespace.TypeName" or "AssemblyName/DeclaringType+NestedName".
    /// </summary>

    /// <summary>
    /// Build a NativeSymbol → dispatch table slot index mapping by replicating
    /// the same sorting logic used in <see cref="BuildHotpatchTable"/>.
    ///
    /// Only methods with metadata tokens are included (the only ones
    /// that appear in s_hotpatch_entries).
    /// </summary>
    // ── Pre-computed O(1) index builders for invocation planning ──

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
    /// NOTE: We intentionally do NOT deduplicate by NativeSymbol here so that
    /// s_hotpatch_entries[] has the same count and ordering as kMethodTable[]
    /// (which is built from methodsForLowering with the same sort).  Without
    /// this, the subject slot map (kSubjectSlotMap → kMethodTable index) would
    /// be misaligned when used against s_hotpatch_entries at dispatch time,
    /// causing the wrong method to be called for all entries past the first
    /// duplicate NativeSymbol.  Method body deduplication is handled separately
    /// at the emission level (NativeSymbol → function definition).
    /// </summary>

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

    /// <summary>
    /// Returns true if the given subjectId belongs to the current AOT module,
    /// based on the "AssemblyName/..." prefix convention.
    /// </summary>

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

    /// <summary>
    /// Determines whether a call target should use method_table dispatch (cross-module call).
    /// If so, allocates or retrieves the method table index.
    /// </summary>

    /// <summary>
    /// Builds C++ code that initializes method table entries for cross-module calls.
    /// Called once during template model creation, after all methods are emitted.
    /// </summary>

    /// <summary>
    /// Computes the set of method subjectIds reachable via AOT call graph
    /// traversal from the given entry point. BFS through call/callvirt/newobj/
    /// ldftn/ldvirtftn instructions, resolving callees through _methodsBySubjectId.
    /// </summary>

    /// <summary>
    /// Builds a minimal C++ function body stub for AOT-unreachable methods.
    /// Unreachable methods still need a dispatchable entry point (for the
    /// interpreter dispatch table) but do not require a full native body.
    /// Returns a default value to avoid crashing when the fact loop runs
    /// every dispatch-table entry including unreachable interface stubs.
    /// </summary>

    /// <summary>
    /// Emit one method's body, collecting reverse-P/Invoke entries as a side effect.
    /// Extracted as a separate method to give the JIT a clear stack-cleanup boundary
    /// (avoiding stack accumulation observed with Select().ToList() lambda closure).
    /// </summary>
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

    // ── Shared hotpatch entry reference emission ─────────────────────────

    /// <summary>
    /// Check whether a method is a subject method.
    /// When _subjectMethodSubjectIds is set (from --subject-methods), uses set-based
    /// lookup first (most specific); falls back to naming conventions for chunks
    /// where the constructed SubjectIds don't match AOT IR naming (e.g., when
    /// AutoTestGenerator's generatedMethodId differs from actual C# method names).
    /// </summary>

    /// <summary>
    /// Extract subject index from a SubjectId string.
    /// Subject methods have SubjectIds ending like "::CustomEntrySubject_N:..." or "::Subject_N:...".
    /// Returns -1 if the SubjectId is not a subject method.
    /// </summary>

    /// <summary>
    /// Strip the trailing _N variant suffix from a SubjectId before the return type.
    /// CombinedSubjects AutoGenerated methods and ::Subject_N methods both have
    /// _0/_1/_2 variant suffixes (e.g., "ForEachAsync_40_..._0:System.Int64()").
    /// Normalizing to ":System.Int64()" lets _0 and _1 variants of the same subject
    /// match in the subjectIdsWithZero dedup set.
    /// </summary>

    // ── Step 2: CodeRegistrationV0 + MetadataRegistrationV0 + CodegenRegistrationOptionsV0 ──
    // Emitted as extern "C" symbols for RegisterCodegen + BootstrapRuntime path.
    // References the generic registration arrays emitted by EmitGenericRegistration.

    // ── Step 3: ReflectionQueryImageDescriptor ──────────────────────────────────
    // Emits ReflectionQueryMethodDescriptor[] and ReflectionQueryTypeDescriptor[]
    // arrays, and a ReflectionQueryImageDescriptor that module.image points to.
    // This enables ResolveSubjectId to find call_target via reflection query model.

    /// <summary>
    /// Generate the C++ header for pre-computed enum metadata tables.
    /// Collects enum type data from ALL resolved assemblies (not just the entry
    /// assembly) so that SPC enum types (DayOfWeek, BindingFlags, etc.) are
    /// included alongside the entry assembly's own enum types.
    /// Returns empty string if no enum types or field data are available.
    /// </summary>

    /// <summary>
    /// Read assembly PE metadata and collect enum type flags, subjectIds,
    /// AND field name/value entries for each enum type.
    /// Populates the provided lists, skipping types already in seenSubjectIds.
    /// </summary>

    /// <summary>
    /// Fallback: scan PE metadata of all closure assemblies to collect enum field
    /// entries (name + constant value) when _reflectionMemberSupport.FieldEntries
    /// is empty (e.g. during foundation-dll codegen for stub-based families).
    /// </summary>

    /// <summary>
    /// Read the constant value from a field definition's Constant metadata.
    /// Returns null if the field has no constant or the type is unsupported.
    /// </summary>
    /// <summary>
    /// Collect enum type subject IDs from reflection member support data (preferred)
    /// or fall back to scanning PE metadata of closure assemblies.
    /// </summary>

    /// <summary>
    /// Build a type → {value → name} map for all enum types in the closure.
    /// Prefers FieldEntries from reflection member support data; falls back
    /// to scanning PE metadata of closure assemblies.
    /// Used by S2 (BoxToString switch) and other value→name lookups.
    /// </summary>

    /// <summary>
    /// Scan a single assembly's PE metadata for enum type definitions and
    /// add their subject IDs to the provided set.
    /// </summary>
}
