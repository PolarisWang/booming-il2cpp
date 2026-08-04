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
    /// <summary>
    /// Optional namespace filter: comma-separated prefixes (e.g. "System.Collections.Frozen").
    /// When set, only AOT IR methods whose declaring type namespace matches one of the
    /// prefixes are included in lowering.  Set before calling <see cref="Create"/>.
    /// </summary>
    public string? NamespaceFilter { get; set; }

    private IReadOnlyDictionary<string, AotCoreIrMethodArtifact> _methodsBySubjectId = null!;

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

    private CodegenMode _codegenMode = CodegenMode.Aot;
    private List<string>? _subjectMethodSubjectIds;
    private IReadOnlyDictionary<string, ManagedMethodModel>? _allManagedMethods;
    private readonly List<(string SubjectId, string Json)> _cryptoAotIrEntries = new();

    /// <summary>
    /// AOT Core IR JSON data for external runtime methods that are NOT AOT-compiled.
    /// Populated in TryCreateExternalRuntimeHelperDefinition catch-all fallback path
    /// and consumed by BuildExternalRuntimeDispatchTable to populate json_data in
    /// kChaosExternalRuntimeIlData[] entries.  This enables the interpreter to execute
    /// BCL/referenced-assembly methods via InterpreterEntryDirect at runtime.
    /// </summary>
    private readonly Dictionary<string, string> _externalRuntimeIlDataJson = new(StringComparer.Ordinal);

    /// <summary>
    /// Value type subject IDs discovered by BuildGeneratedModuleModel's ABI slot scan
    /// (GeneratedModule.cs:205-318). These may include types referenced via
    /// chaos_resolve_managed_value_pointer{T} that are not in the AOT IR type
    /// metadata. Merged into _emittedValueTypeSubjectIds during EmitObjectModelDeclarations
    /// to prevent C2065/C2672.
    /// </summary>
    internal HashSet<string>? _emittedValueTypeSubjectIdsFromAbi;

    // Verification dispatch manifest (populated by BuildDispatchEntryCode)
    private string? _manifestJson;

    // ── Hot-update static field helpers ───────────────────────────────────

}
