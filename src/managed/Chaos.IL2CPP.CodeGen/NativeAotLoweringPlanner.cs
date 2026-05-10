using System.Collections.Immutable;
using System.Globalization;
using System.Reflection;
using System.Reflection.Metadata;
using System.Reflection.Metadata.Ecma335;
using System.Reflection.PortableExecutable;
using System.Text;
using System.Text.Json;
using System.Threading.Tasks;
using Chaos.IL2CPP.Contracts;

namespace Chaos.IL2CPP.CodeGen;

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

    private CustomAttributeSupportModel _customAttributeSupport = CustomAttributeSupportModel.Empty;
    private AssemblyReflectionSupportModel _assemblyReflectionSupport = AssemblyReflectionSupportModel.Empty;
    private ReflectionMemberSupportModel _reflectionMemberSupport = ReflectionMemberSupportModel.Empty;
    private StaticFieldDataSupportModel _staticFieldDataSupport = StaticFieldDataSupportModel.Empty;

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


	private static readonly System.Collections.Concurrent.ConcurrentDictionary<string, string> _cppStringLiteralCache =
		new System.Collections.Concurrent.ConcurrentDictionary<string, string>(StringComparer.Ordinal);

	private static readonly System.Collections.Concurrent.ConcurrentDictionary<string, string> _sanitizedSubjectIdCache =
		new System.Collections.Concurrent.ConcurrentDictionary<string, string>(StringComparer.Ordinal);

	private static readonly System.Collections.Concurrent.ConcurrentDictionary<string, string> _pseudoMetadataHandleCache =
		new System.Collections.Concurrent.ConcurrentDictionary<string, string>(StringComparer.Ordinal);


    private StringBuilder _sharedMethodSourceBuilder = new(4096);

    private IReadOnlyList<IGrouping<string, AotCoreIrMethodArtifact>> _methodsGroupedByDeclaringType =
        Array.Empty<IGrouping<string, AotCoreIrMethodArtifact>>();

    private IReadOnlyList<AotCoreIrMethodArtifact> _genericStaticMethodCandidates =
        Array.Empty<AotCoreIrMethodArtifact>();

    private IReadOnlyDictionary<string, int>? _vtableSlotMap;
    private IReadOnlyDictionary<string, int>? _vtableLengths;
    private IReadOnlySet<string>? _vtableTypes;
    private IReadOnlySet<string>? _interfaceTypeSubjectIds;
    private IReadOnlySet<string>? _sealedTypeSubjectIds;

    // ── A4-Dual+V2 Header kind / vtable variant decision engine ──
    /// <summary>
    /// Determines the ObjectHeader kind for a given type. PureType = no sync (value types,
    /// boxed primitives, sealed types with no finalizer). ThinLockable = default for reference
    /// types. Fat = types with finalizers, types needing explicit vtable*, string, array, delegate.
    /// </summary>
    private enum HeaderKind { PureType, ThinLockable, Fat }

    /// <summary>
    /// VTable variant: V0 (no vtable — interfaces, pure value types in V0 emission),
    /// V1 (indirect — vtable_array points to external VTable_symbol[]),
    /// V2 (inline — TypeInfoV2 with inline_slots[6] for ≤6 virtual methods).
    /// </summary>
    private enum VTableVariant { V0, V1, V2 }

    private HeaderKind GetHeaderKind(string typeSubjectId)
    {
        // Arrays, delegates always use Fat (need explicit vtable* and sync)
        if (typeSubjectId.Contains("/System.Array") ||
            typeSubjectId.Contains("/System.Delegate") ||
            typeSubjectId.Contains("/System.MulticastDelegate") ||
            typeSubjectId.Contains("/System.Collections.IEnumerator"))
            return HeaderKind.Fat;

        // String: ThinLockable (sealed, no virtual dispatch, but supports sync via lock())
        if (typeSubjectId.Contains("/System.String"))
            return HeaderKind.ThinLockable;

        // Value types → PureType (no sync needed)
        if (_valueTypeSubjectIds.Contains(typeSubjectId))
            return HeaderKind.PureType;

        // Interface types → PureType
        if (_interfaceTypeSubjectIds?.Contains(typeSubjectId) == true)
            return HeaderKind.PureType;

        // Sealed types with no virtual methods → PureType
        if (_sealedTypeSubjectIds?.Contains(typeSubjectId) == true)
        {
            // Check if type has any virtual methods
            bool hasVirtual = _methodsBySubjectId.Values.Any(m =>
                !m.IsStatic &&
                string.Equals(m.Identity.DeclaringTypeSubjectId, typeSubjectId, StringComparison.Ordinal));
            if (!hasVirtual)
                return HeaderKind.PureType;
        }

        // Types with finalizer → Fat
        // (finalizer detection: check for virtual Finalize method)
        if (_methodsBySubjectId.Values.Any(m =>
            !m.IsStatic &&
            string.Equals(m.Identity.DeclaringTypeSubjectId, typeSubjectId, StringComparison.Ordinal) &&
            m.SubjectId.Contains("Finalize")))
            return HeaderKind.Fat;

        // If the type has a non-empty vtable (virtual methods), it needs FatHeader
        // to store the vtable pointer. ThinLockableHeader has no vtable field.
        if (_vtableLengths != null &&
            _vtableLengths.TryGetValue(typeSubjectId, out int vtLen) &&
            vtLen > 0)
            return HeaderKind.Fat;

        // Everything else → ThinLockable (default)
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

    public NativeAotTemplateModel Create(
        NativeAotLoweringPlanArtifact loweringPlan,
        AotCoreIrArtifact aotCoreIr,
        AotCoreIrMethodArtifact entryMethod,
        ManagedClosureManifestArtifact closureManifest,
        MetadataRegistrationArtifact metadataRegistration,
        SupplementalMetadataTemplateArtifact supplementalMetadataTemplate,
        bool fullAssemblyMode = false)
    {
        ArgumentNullException.ThrowIfNull(loweringPlan);
        ArgumentNullException.ThrowIfNull(aotCoreIr);
        ArgumentNullException.ThrowIfNull(closureManifest);
        ArgumentNullException.ThrowIfNull(metadataRegistration);
        ArgumentNullException.ThrowIfNull(supplementalMetadataTemplate);

        if (!fullAssemblyMode)
            ArgumentNullException.ThrowIfNull(entryMethod);

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
        _assemblyName = loweringPlan.AssemblyName;
        _attributeStorageFieldIndex = BuildAttributeStorageFieldIndex(_methodsBySubjectId);
        _referenceTypeBaseSubjectIds = CollectReferenceTypeBaseSubjectIds(aotCoreIr);
        _referenceTypeImplementedInterfaceSubjectIds = CollectReferenceTypeImplementedInterfaceSubjectIds(aotCoreIr);
        _valueTypeSubjectIds = CollectValueTypeSubjectIds(aotCoreIr);
        _sealedTypeSubjectIds = CollectSealedTypeSubjectIds(aotCoreIr);

        var methodsForLowering = fullAssemblyMode
            ? CollectAllMethods(aotCoreIr)
            : CollectReachableMethods(aotCoreIr, entryMethod);
        _methodNativeSymbolToManifestIndex = methodsForLowering
            .Select((method, idx) => (method.NativeSymbol, idx))
            .DistinctBy(t => t.NativeSymbol)
            .ToDictionary(t => t.NativeSymbol, t => t.idx);
        _nativeSymbolToDispatchSlot = BuildDispatchSlotMap(methodsForLowering, metadataRegistration);
        var stringLiterals = CollectStringLiterals(methodsForLowering);
        _stringIdMapping = BuildStringIdMapping(stringLiterals);
        _cachedClosureAssemblyPaths = EnumerateClosureAssemblyPaths(closureManifest).ToArray();
        _closureAssemblyPathByName = BuildClosureAssemblyPathByNameCore(_cachedClosureAssemblyPaths);
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
        CustomAttributeSupportModel? customAttributeSupport = null;
        AssemblyReflectionSupportModel? assemblyReflectionSupport = null;
        ReflectionMemberSupportModel? reflectionMemberSupport = null;
        StaticFieldDataSupportModel? staticFieldDataSupport = null;

        Parallel.Invoke(
            () => customAttributeSupport = BuildCustomAttributeSupportModel(
                methodsForLowering,
                supplementalMetadataTemplate),
            () => assemblyReflectionSupport = BuildAssemblyReflectionSupportModel(
                methodsForLowering,
                supplementalMetadataTemplate),
            () => reflectionMemberSupport = BuildReflectionMemberSupportModel(
                methodsForLowering,
                supplementalMetadataTemplate),
            () => staticFieldDataSupport = BuildStaticFieldDataSupportModel(
                methodsForLowering,
                metadataRegistration));

        _customAttributeSupport = customAttributeSupport!;
        _assemblyReflectionSupport = assemblyReflectionSupport!;
        _reflectionMemberSupport = reflectionMemberSupport!;
        _staticFieldDataSupport = staticFieldDataSupport!;
        _staticInitializationSupport = BuildStaticInitializationSupportModel(
            methodsForLowering,
            closureManifest);
        var externalRuntimeHelpers = CollectExternalRuntimeHelpers(methodsForLowering, _staticInitializationSupport);
        var objectModelBuilder = new StringBuilder(65536);
        EmitRuntimePrelude(objectModelBuilder, externalRuntimeHelpers, _staticFieldDataSupport);
        EmitObjectModelDeclarations(objectModelBuilder, methodsForLowering, externalRuntimeHelpers);
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

        var methodDeclarations = BuildMethodDeclarations(methodsForLowering);
        var methods = methodsForLowering
            .Select(method =>
            {
                // Collect UnmanagedCallersOnly methods for reverse P/Invoke registration.
                if (method.IsUnmanagedCallersOnly)
                {
                    _reversePInvokeEntries.Add((method.SubjectId, method.NativeSymbol));
                }

                return new NativeAotMethodTemplateModel
                {
                    SubjectId = method.SubjectId,
                    MethodSource = BuildMethodSource(method),
                };
            })
            .ToList();
        var entryBridgeArguments = fullAssemblyMode ? "" : BuildEntryBridgeArguments(entryMethod);

        var abiManifestCode = BuildAbiManifest(methodsForLowering);
        var nameIndexCode = BuildHotpatchTable(methodsForLowering, metadataRegistration);
        var moduleRegistrationCode = BuildModuleRegistration();
        if (!string.IsNullOrEmpty(nameIndexCode))
        {
            moduleRegistrationCode += Environment.NewLine + nameIndexCode;
        }
        if (!string.IsNullOrEmpty(aotRegistrationCode))
        {
            moduleRegistrationCode += Environment.NewLine + aotRegistrationCode;
        }
        if (!string.IsNullOrEmpty(abiManifestCode))
        {
            moduleRegistrationCode = abiManifestCode + Environment.NewLine + moduleRegistrationCode;
        }
        if (_methodTableEntries.Count > 0)
        {
            moduleRegistrationCode += BuildMethodTableInitialization();
        }

        // Phase 1 diagnostics: log StructuredIR coverage summary
        LogPhase1Summary();

        return new NativeAotTemplateModel
        {
            Includes =
            [
                "<chaos/common.h>",
                "<chaos/type_info.h>",
                "\"runtime_core.h\"",
                "\"codegen_bridge.h\"",
                "\"module_registry.h\"",
                "\"abi_manifest.h\"",
                "\"hotpatch_table.h\"",
                "\"runtime_vtable.h\"",
                "\"runtime_instantiation.h\"",
                "\"load_store_chaos_bridge.h\"",
            ],
            ObjectModelCode = objectModelBuilder.ToString().TrimEnd(),
            GenericRegistrationCode = genericRegistrationHelperCode,
            MethodDeclarations = methodDeclarations,
            Methods = methods,
            EntrySubjectId = loweringPlan.EntrySubjectId,
            EntrySymbol = loweringPlan.EntrySymbol,
            EntryNativeSymbol = entryMethod.NativeSymbol,
            NativeEntryFunctionName = loweringPlan.NativeEntryFunctionName,
            EntryBridgeArguments = entryBridgeArguments,
            ShapeDispatchHeaderContent = _shapeRegistry.GenerateCppShapeHeader(),
            ModuleRegistrationCode = moduleRegistrationCode,
            WorkloadAbi = loweringPlan.WorkloadAbi,
            GlobalDeclarations = string.Empty,
            CodegenNamespace = SanitizeCppIdentifier(loweringPlan.AssemblyName),
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
                    ? "&" + GetNativeTypeInfoSymbol(subjectId)
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
    private Dictionary<string, int> BuildDispatchSlotMap(
        IReadOnlyList<AotCoreIrMethodArtifact> reachableMethods,
        MetadataRegistrationArtifact metadataRegistration)
    {
        var tokenLookup = new MetadataTokenLookup(metadataRegistration.Registrations);

        var entries = new List<(string TypeName, string NativeSymbol, uint Token)>();
        foreach (var method in reachableMethods)
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
            uint token = tokenLookup.TryGetMethodToken(method.SubjectId);
            if (token == 0)
                continue;

            entries.Add((typeName, method.NativeSymbol, token));
        }

        // Same grouping + sort as BuildHotpatchTable
        var grouped = entries
            .GroupBy(e => e.TypeName, StringComparer.Ordinal)
            .OrderBy(g => g.Key, StringComparer.Ordinal)
            .ToList();

        var result = new Dictionary<string, int>(entries.Count, StringComparer.Ordinal);
        int slot = 0;
        foreach (var group in grouped)
        {
            foreach (var entry in group)
            {
                result[entry.NativeSymbol] = slot++;
            }
        }

        return result;
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
        var sb = new System.Text.StringBuilder(512);
        sb.AppendLine();
        sb.AppendLine("// ── Method table initialization ────────────────────────────────");
        sb.AppendLine("// Fills the global method table with function pointers for");
        sb.AppendLine("// cross-module dispatch targets.");
        sb.AppendLine("static const CHAOS_IL2CPP_UINT32 s_method_table_init = []()");
        sb.AppendLine("{");
        foreach (var (entryIndex, nativeSymbol) in _methodTableEntries)
        {
            sb.Append("    ::chaos::il2cpp::method_table::WriteMethodTable(");
            sb.Append(entryIndex);
            sb.Append(", reinterpret_cast<void*>(");
            sb.Append(nativeSymbol);
            sb.AppendLine("), 1u);");
            // Record ABI origin for cross-module validation
            if (_methodNativeSymbolToManifestIndex.TryGetValue(nativeSymbol, out int manifestIdx))
            {
                sb.Append("    ::chaos::il2cpp::method_table::SetMethodOrigin(");
                sb.Append(entryIndex);
                sb.Append(", s_native_aot_module_id, ");
                sb.Append(manifestIdx);
                sb.AppendLine("u);");
            }
        }
        sb.AppendLine("    return 0u;");
        sb.AppendLine("}();");
        return sb.ToString();
    }

    private string BuildMethodSource(AotCoreIrMethodArtifact method)
    {
        _sharedMethodSourceBuilder.Clear();
        var builder = _sharedMethodSourceBuilder;
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

        builder.AppendLine("using chaos_delegate_invocation_list = CHAOS_IL2CPP_VECTOR(CHAOS_IL2CPP_INTPTR);");
        builder.AppendLine();

        // Emit struct definition for System.Delegate so the helper functions below can use it
        builder.AppendLine("struct chaos_type_System_Private_CoreLib_System_Delegate");
        builder.AppendLine("{");
        builder.AppendLine("    FatHeader header{};");
        builder.AppendLine("    CHAOS_IL2CPP_INTPTR chaos_delegate_target = 0;");
        builder.AppendLine("    CHAOS_IL2CPP_INTPTR chaos_delegate_method_ptr = 0;");
        builder.AppendLine("    CHAOS_IL2CPP_INTPTR chaos_delegate_invocation_list = 0;");
        builder.AppendLine("    CHAOS_IL2CPP_INTPTR chaos_delegate_invocation_count = 0;");
        builder.AppendLine("};");
        builder.AppendLine();
        builder.AppendLine("chaos_type_System_Private_CoreLib_System_Delegate* chaos_require_delegate(CHAOS_IL2CPP_INTPTR chaos_delegate_value)");
        builder.AppendLine("{");
        builder.AppendLine("    if (chaos_delegate_value == static_cast<CHAOS_IL2CPP_INTPTR>(0))");
        builder.AppendLine("    {");
        builder.AppendLine("        CHAOS_IL2CPP_FAIL();");
        builder.AppendLine("    }");
        builder.AppendLine();
        builder.AppendLine("    return reinterpret_cast<chaos_type_System_Private_CoreLib_System_Delegate*>(chaos_delegate_value);");
        builder.AppendLine("}");
        builder.AppendLine();
        builder.AppendLine("const chaos_delegate_invocation_list* chaos_try_get_delegate_invocation_list(");
        builder.AppendLine("    const chaos_type_System_Private_CoreLib_System_Delegate* chaos_delegate) noexcept");
        builder.AppendLine("{");
        builder.AppendLine("    if (chaos_delegate == nullptr ||");
        builder.AppendLine("        chaos_delegate->chaos_delegate_invocation_list == static_cast<CHAOS_IL2CPP_INTPTR>(0) ||");
        builder.AppendLine("        chaos_delegate->chaos_delegate_invocation_count <= static_cast<CHAOS_IL2CPP_INTPTR>(0))");
        builder.AppendLine("    {");
        builder.AppendLine("        return nullptr;");
        builder.AppendLine("    }");
        builder.AppendLine();
        builder.AppendLine(
            "    return reinterpret_cast<const chaos_delegate_invocation_list*>(chaos_delegate->chaos_delegate_invocation_list);");
        builder.AppendLine("}");
        builder.AppendLine();
        builder.AppendLine("bool chaos_delegate_single_entry_equals(CHAOS_IL2CPP_INTPTR chaos_left_value, CHAOS_IL2CPP_INTPTR chaos_right_value) noexcept");
        builder.AppendLine("{");
        builder.AppendLine("    if (chaos_left_value == chaos_right_value)");
        builder.AppendLine("    {");
        builder.AppendLine("        return true;");
        builder.AppendLine("    }");
        builder.AppendLine();
        builder.AppendLine("    if (chaos_left_value == static_cast<CHAOS_IL2CPP_INTPTR>(0) ||");
        builder.AppendLine("        chaos_right_value == static_cast<CHAOS_IL2CPP_INTPTR>(0))");
        builder.AppendLine("    {");
        builder.AppendLine("        return false;");
        builder.AppendLine("    }");
        builder.AppendLine();
        builder.AppendLine("    const auto* chaos_left = reinterpret_cast<const chaos_type_System_Private_CoreLib_System_Delegate*>(chaos_left_value);");
        builder.AppendLine("    const auto* chaos_right = reinterpret_cast<const chaos_type_System_Private_CoreLib_System_Delegate*>(chaos_right_value);");
        builder.AppendLine("    return chaos_left->header.type_info == chaos_right->header.type_info &&");
        builder.AppendLine("           chaos_left->chaos_delegate_target == chaos_right->chaos_delegate_target &&");
        builder.AppendLine("           chaos_left->chaos_delegate_method_ptr == chaos_right->chaos_delegate_method_ptr;");
        builder.AppendLine("}");
        builder.AppendLine();
        builder.AppendLine("void chaos_delegate_append_flattened_entries(");
        builder.AppendLine("    chaos_delegate_invocation_list& chaos_entries,");
        builder.AppendLine("    CHAOS_IL2CPP_INTPTR chaos_delegate_value)");
        builder.AppendLine("{");
        builder.AppendLine("    if (chaos_delegate_value == static_cast<CHAOS_IL2CPP_INTPTR>(0))");
        builder.AppendLine("    {");
        builder.AppendLine("        return;");
        builder.AppendLine("    }");
        builder.AppendLine();
        builder.AppendLine("    auto* chaos_delegate = chaos_require_delegate(chaos_delegate_value);");
        builder.AppendLine("    const auto* chaos_invocation_list = chaos_try_get_delegate_invocation_list(chaos_delegate);");
        builder.AppendLine("    if (chaos_invocation_list == nullptr)");
        builder.AppendLine("    {");
        builder.AppendLine("        if (chaos_delegate->chaos_delegate_method_ptr == static_cast<CHAOS_IL2CPP_INTPTR>(0))");
        builder.AppendLine("        {");
        builder.AppendLine("            CHAOS_IL2CPP_FAIL();");
        builder.AppendLine("        }");
        builder.AppendLine();
        builder.AppendLine("        chaos_entries.push_back(chaos_delegate_value);");
        builder.AppendLine("        return;");
        builder.AppendLine("    }");
        builder.AppendLine();
        builder.AppendLine(
            "    if (static_cast<CHAOS_IL2CPP_INTPTR>(chaos_invocation_list->size()) != chaos_delegate->chaos_delegate_invocation_count)");
        builder.AppendLine("    {");
        builder.AppendLine("        CHAOS_IL2CPP_FAIL();");
        builder.AppendLine("    }");
        builder.AppendLine();
        builder.AppendLine("    for (const auto chaos_entry_value : *chaos_invocation_list)");
        builder.AppendLine("    {");
        builder.AppendLine("        chaos_delegate_append_flattened_entries(chaos_entries, chaos_entry_value);");
        builder.AppendLine("    }");
        builder.AppendLine("}");
        builder.AppendLine();
        builder.AppendLine("void chaos_delegate_validate_entry_types(const chaos_delegate_invocation_list& chaos_entries)");
        builder.AppendLine("{");
        builder.AppendLine("    if (chaos_entries.empty())");
        builder.AppendLine("    {");
        builder.AppendLine("        return;");
        builder.AppendLine("    }");
        builder.AppendLine();
        builder.AppendLine("    const auto* chaos_first = chaos_require_delegate(chaos_entries.front());");
        builder.AppendLine("    for (const auto chaos_entry_value : chaos_entries)");
        builder.AppendLine("    {");
        builder.AppendLine("        const auto* chaos_entry = chaos_require_delegate(chaos_entry_value);");
        builder.AppendLine("        if (chaos_entry->header.type_info != chaos_first->header.type_info)");
        builder.AppendLine("        {");
        builder.AppendLine("            CHAOS_IL2CPP_FAIL();");
        builder.AppendLine("        }");
        builder.AppendLine("    }");
        builder.AppendLine("}");
        builder.AppendLine();
        builder.AppendLine("CHAOS_IL2CPP_INTPTR chaos_delegate_allocate_with_type_info(const TypeInfo* chaos_delegate_type_info)");
        builder.AppendLine("{");
        builder.AppendLine("    switch (chaos_delegate_type_info->stable_id)");
        builder.AppendLine("    {");
        foreach (var delegateTypeSubjectId in delegateTypeSubjectIds)
        {
            builder.AppendLine($"        case {GetNativeTypeIdSymbol(delegateTypeSubjectId)}:");
            builder.AppendLine("        {");
            builder.AppendLine($"            auto* chaos_delegate = new {GetNativeTypeSymbol(delegateTypeSubjectId)}{{}};");
            builder.AppendLine($"            chaos_delegate->header.type_info = &{GetNativeTypeInfoSymbol(delegateTypeSubjectId)};");
            if (_vtableTypes?.Contains(delegateTypeSubjectId) == true)
            {
                builder.AppendLine($"            chaos_delegate->header.vtable = {GetNativeVTableSymbol(delegateTypeSubjectId)};");
            }
            builder.AppendLine("            return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_delegate);");
            builder.AppendLine("        }");
        }

        builder.AppendLine("        default:");
        builder.AppendLine("            CHAOS_IL2CPP_FAIL();");
        builder.AppendLine("    }");
        builder.AppendLine("}");
        builder.AppendLine();
        builder.AppendLine("CHAOS_IL2CPP_INTPTR chaos_delegate_create_multicast_like(");
        builder.AppendLine("    CHAOS_IL2CPP_INTPTR chaos_template_delegate_value,");
        builder.AppendLine("    const chaos_delegate_invocation_list& chaos_entries)");
        builder.AppendLine("{");
        builder.AppendLine("    if (chaos_entries.empty())");
        builder.AppendLine("    {");
        builder.AppendLine("        return static_cast<CHAOS_IL2CPP_INTPTR>(0);");
        builder.AppendLine("    }");
        builder.AppendLine();
        builder.AppendLine("    if (chaos_entries.size() == 1)");
        builder.AppendLine("    {");
        builder.AppendLine("        return chaos_entries.front();");
        builder.AppendLine("    }");
        builder.AppendLine();
        builder.AppendLine("    const auto* chaos_template_delegate = chaos_require_delegate(chaos_template_delegate_value);");
        builder.AppendLine(
            "    const auto chaos_delegate_value = chaos_delegate_allocate_with_type_info(chaos_template_delegate->header.type_info);");
        builder.AppendLine("    auto* chaos_delegate = chaos_require_delegate(chaos_delegate_value);");
        builder.AppendLine("    auto* chaos_invocation_list = new chaos_delegate_invocation_list(chaos_entries);");
        builder.AppendLine("    chaos_delegate->chaos_delegate_target = static_cast<CHAOS_IL2CPP_INTPTR>(0);");
        builder.AppendLine("    chaos_delegate->chaos_delegate_method_ptr = static_cast<CHAOS_IL2CPP_INTPTR>(0);");
        builder.AppendLine(
            "    chaos_delegate->chaos_delegate_invocation_list = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_invocation_list);");
        builder.AppendLine(
            "    chaos_delegate->chaos_delegate_invocation_count = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_invocation_list->size());");
        builder.AppendLine("    return chaos_delegate_value;");
        builder.AppendLine("}");
        builder.AppendLine();
        builder.AppendLine("CHAOS_IL2CPP_INTPTR chaos_delegate_combine(CHAOS_IL2CPP_INTPTR chaos_left_value, CHAOS_IL2CPP_INTPTR chaos_right_value)");
        builder.AppendLine("{");
        builder.AppendLine("    if (chaos_left_value == static_cast<CHAOS_IL2CPP_INTPTR>(0))");
        builder.AppendLine("    {");
        builder.AppendLine("        return chaos_right_value;");
        builder.AppendLine("    }");
        builder.AppendLine();
        builder.AppendLine("    if (chaos_right_value == static_cast<CHAOS_IL2CPP_INTPTR>(0))");
        builder.AppendLine("    {");
        builder.AppendLine("        return chaos_left_value;");
        builder.AppendLine("    }");
        builder.AppendLine();
        builder.AppendLine("    chaos_delegate_invocation_list chaos_entries{};");
        builder.AppendLine("    chaos_delegate_append_flattened_entries(chaos_entries, chaos_left_value);");
        builder.AppendLine("    chaos_delegate_append_flattened_entries(chaos_entries, chaos_right_value);");
        builder.AppendLine("    chaos_delegate_validate_entry_types(chaos_entries);");
        builder.AppendLine("    return chaos_delegate_create_multicast_like(chaos_left_value, chaos_entries);");
        builder.AppendLine("}");
        builder.AppendLine();
        builder.AppendLine("CHAOS_IL2CPP_INTPTR chaos_delegate_remove(CHAOS_IL2CPP_INTPTR chaos_source_value, CHAOS_IL2CPP_INTPTR chaos_value_to_remove)");
        builder.AppendLine("{");
        builder.AppendLine("    if (chaos_source_value == static_cast<CHAOS_IL2CPP_INTPTR>(0))");
        builder.AppendLine("    {");
        builder.AppendLine("        return static_cast<CHAOS_IL2CPP_INTPTR>(0);");
        builder.AppendLine("    }");
        builder.AppendLine();
        builder.AppendLine("    if (chaos_value_to_remove == static_cast<CHAOS_IL2CPP_INTPTR>(0))");
        builder.AppendLine("    {");
        builder.AppendLine("        return chaos_source_value;");
        builder.AppendLine("    }");
        builder.AppendLine();
        builder.AppendLine("    chaos_delegate_invocation_list chaos_source_entries{};");
        builder.AppendLine("    chaos_delegate_invocation_list chaos_remove_entries{};");
        builder.AppendLine("    chaos_delegate_append_flattened_entries(chaos_source_entries, chaos_source_value);");
        builder.AppendLine("    chaos_delegate_append_flattened_entries(chaos_remove_entries, chaos_value_to_remove);");
        builder.AppendLine("    if (chaos_remove_entries.empty() || chaos_source_entries.size() < chaos_remove_entries.size())");
        builder.AppendLine("    {");
        builder.AppendLine("        return chaos_source_value;");
        builder.AppendLine("    }");
        builder.AppendLine();
        builder.AppendLine(
            "    for (CHAOS_IL2CPP_INTPTR chaos_start = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_source_entries.size() - chaos_remove_entries.size());");
        builder.AppendLine("         chaos_start >= static_cast<CHAOS_IL2CPP_INTPTR>(0);");
        builder.AppendLine("         --chaos_start)");
        builder.AppendLine("    {");
        builder.AppendLine("        bool chaos_matches = true;");
        builder.AppendLine("        for (CHAOS_IL2CPP_SIZE chaos_index = 0; chaos_index < chaos_remove_entries.size(); ++chaos_index)");
        builder.AppendLine("        {");
        builder.AppendLine(
            "            if (!chaos_delegate_single_entry_equals(chaos_source_entries[static_cast<CHAOS_IL2CPP_SIZE>(chaos_start) + chaos_index], chaos_remove_entries[chaos_index]))");
        builder.AppendLine("            {");
        builder.AppendLine("                chaos_matches = false;");
        builder.AppendLine("                break;");
        builder.AppendLine("            }");
        builder.AppendLine("        }");
        builder.AppendLine();
        builder.AppendLine("        if (!chaos_matches)");
        builder.AppendLine("        {");
        builder.AppendLine("            continue;");
        builder.AppendLine("        }");
        builder.AppendLine();
        builder.AppendLine(
            "        chaos_source_entries.erase(chaos_source_entries.begin() + static_cast<CHAOS_IL2CPP_SIZE>(chaos_start), chaos_source_entries.begin() + static_cast<CHAOS_IL2CPP_SIZE>(chaos_start) + chaos_remove_entries.size());");
        builder.AppendLine("        if (chaos_source_entries.empty())");
        builder.AppendLine("        {");
        builder.AppendLine("            return static_cast<CHAOS_IL2CPP_INTPTR>(0);");
        builder.AppendLine("        }");
        builder.AppendLine();
        builder.AppendLine("        if (chaos_source_entries.size() == 1)");
        builder.AppendLine("        {");
        builder.AppendLine("            return chaos_source_entries.front();");
        builder.AppendLine("        }");
        builder.AppendLine();
        builder.AppendLine("        return chaos_delegate_create_multicast_like(chaos_source_value, chaos_source_entries);");
        builder.AppendLine("    }");
        builder.AppendLine();
        builder.AppendLine("    return chaos_source_value;");
        builder.AppendLine("}");
        builder.AppendLine();
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
                attributeHandle,
                attributeTypeIdentity.SubjectId));
        }
    }

    private void CollectSyntheticMethodCustomAttributeMaterializations(
        MetadataReader metadataReader,
        string assemblyName,
        string targetSubjectId,
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
                materializations.Add(CreateDllImportAttributeMaterializationPlan(metadataReader, targetSubjectId, methodDefinition));
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
        MethodDefinition methodDefinition)
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

        return new CustomAttributeMaterializationPlan(
            CustomAttributeTargetKind.Method,
            targetSubjectId,
            DllImportAttributeTypeSubjectId,
            assignments);
    }

    private CustomAttributeMaterializationPlan CreateCustomAttributeMaterializationPlan(
        MetadataReader metadataReader,
        string targetSubjectId,
        CustomAttributeTargetKind targetKind,
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
                    throw new NotSupportedException(
                        $"native-aot custom-attribute IsDefined requires a direct typeof(T) attribute argument in '{method.SubjectId}' at IL offset {instruction.IlOffset}.");
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
            throw new NotSupportedException(
                $"native-aot custom-attribute lookup found ambiguous attribute type '{displayName}'.");
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

public sealed record NativeAotTemplateModel
{
    public required IReadOnlyList<string> Includes { get; init; }

    public required string ObjectModelCode { get; init; }

    public required IReadOnlyList<string> MethodDeclarations { get; init; }

    public required IReadOnlyList<NativeAotMethodTemplateModel> Methods { get; init; }

    public required string EntrySubjectId { get; init; }

    public required string EntrySymbol { get; init; }

    public required string EntryNativeSymbol { get; init; }

    public required string NativeEntryFunctionName { get; init; }

    public required string EntryBridgeArguments { get; init; }

    public required string ShapeDispatchHeaderContent { get; init; }

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
    /// C++ namespace for the generated translation unit.
    /// Derived from the entry native symbol to provide a unique, named
    /// scope for ODR protection across multi-TU builds. Follows the
    /// project's <c>chaos::il2cpp::codegen::*</c> convention.
    /// </summary>
    public string CodegenNamespace { get; init; } = "";
}

public sealed record NativeAotMethodTemplateModel
{
    public required string SubjectId { get; init; }

    public required string MethodSource { get; init; }
}


