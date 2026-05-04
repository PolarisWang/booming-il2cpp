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
        public static readonly DevirtualizationHint None = new(false, null!, null!);
        public bool CanDevirtualize { get; }
        public string ImplementationMethodSubjectId { get; }
        public string DeclaringTypeSubjectId { get; }

        public DevirtualizationHint(
            bool canDevirtualize,
            string implementationMethodSubjectId,
            string declaringTypeSubjectId)
        {
            CanDevirtualize = canDevirtualize;
            ImplementationMethodSubjectId = implementationMethodSubjectId;
            DeclaringTypeSubjectId = declaringTypeSubjectId;
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

	private static readonly System.Collections.Concurrent.ConcurrentDictionary<ulong, string> _nativeStringIdSymbolCache =
		new System.Collections.Concurrent.ConcurrentDictionary<ulong, string>();

    private StringBuilder _sharedMethodSourceBuilder = new(4096);

    private IReadOnlyList<IGrouping<string, AotCoreIrMethodArtifact>> _methodsGroupedByDeclaringType =
        Array.Empty<IGrouping<string, AotCoreIrMethodArtifact>>();

    private IReadOnlyList<AotCoreIrMethodArtifact> _genericStaticMethodCandidates =
        Array.Empty<AotCoreIrMethodArtifact>();

    private IReadOnlyDictionary<string, int>? _vtableSlotMap;
    private IReadOnlySet<string>? _vtableTypes;

    private readonly RuntimeHelperShapeRegistry _shapeRegistry = RuntimeHelperShapeRegistry.BuildDefault();

    public NativeAotTemplateModel Create(
        NativeAotLoweringPlanArtifact loweringPlan,
        AotCoreIrArtifact aotCoreIr,
        AotCoreIrMethodArtifact entryMethod,
        ManagedClosureManifestArtifact closureManifest,
        MetadataRegistrationArtifact metadataRegistration,
        SupplementalMetadataTemplateArtifact supplementalMetadataTemplate)
    {
        ArgumentNullException.ThrowIfNull(loweringPlan);
        ArgumentNullException.ThrowIfNull(aotCoreIr);
        ArgumentNullException.ThrowIfNull(entryMethod);
        ArgumentNullException.ThrowIfNull(closureManifest);
        ArgumentNullException.ThrowIfNull(metadataRegistration);
        ArgumentNullException.ThrowIfNull(supplementalMetadataTemplate);

        ValidateEntryMethod(entryMethod);
        if (!string.Equals(entryMethod.NativeSymbol, loweringPlan.EntrySymbol, StringComparison.Ordinal))
        {
            throw new InvalidOperationException(
                $"native-aot entry symbol '{loweringPlan.EntrySymbol}' does not match aot-core-ir symbol '{entryMethod.NativeSymbol}'");
        }

        _methodsBySubjectId = aotCoreIr.Methods.ToDictionary(method => method.SubjectId, StringComparer.Ordinal);
        _attributeStorageFieldIndex = BuildAttributeStorageFieldIndex(_methodsBySubjectId);
        _referenceTypeBaseSubjectIds = CollectReferenceTypeBaseSubjectIds(aotCoreIr);
        _referenceTypeImplementedInterfaceSubjectIds = CollectReferenceTypeImplementedInterfaceSubjectIds(aotCoreIr);
        _valueTypeSubjectIds = CollectValueTypeSubjectIds(aotCoreIr);

        var reachableMethods = CollectReachableMethods(aotCoreIr, entryMethod);
        var stringLiterals = CollectStringLiterals(reachableMethods);
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
                reachableMethods,
                supplementalMetadataTemplate),
            () => assemblyReflectionSupport = BuildAssemblyReflectionSupportModel(
                reachableMethods,
                supplementalMetadataTemplate),
            () => reflectionMemberSupport = BuildReflectionMemberSupportModel(
                reachableMethods,
                supplementalMetadataTemplate),
            () => staticFieldDataSupport = BuildStaticFieldDataSupportModel(
                reachableMethods,
                metadataRegistration));

        _customAttributeSupport = customAttributeSupport!;
        _assemblyReflectionSupport = assemblyReflectionSupport!;
        _reflectionMemberSupport = reflectionMemberSupport!;
        _staticFieldDataSupport = staticFieldDataSupport!;
        _staticInitializationSupport = BuildStaticInitializationSupportModel(
            reachableMethods,
            closureManifest);
        var externalRuntimeHelpers = CollectExternalRuntimeHelpers(reachableMethods, _staticInitializationSupport);
        var objectModelBuilder = new StringBuilder(65536);
        EmitRuntimePrelude(objectModelBuilder, externalRuntimeHelpers, _staticFieldDataSupport);
        EmitObjectModelDeclarations(objectModelBuilder, reachableMethods, externalRuntimeHelpers);
        EmitReachableMethodForwardDeclarations(objectModelBuilder, reachableMethods);
        EmitStringIdTable(objectModelBuilder, stringLiterals);
        if (externalRuntimeHelpers.Any(helper => IsSpanRuntimeHelperSubjectId(helper.SubjectId)))
        {
            EmitSpanRuntimePrelude(objectModelBuilder, _staticFieldDataSupport);
        }
        EmitDelegateRuntimeSupportDefinitions(objectModelBuilder, reachableMethods, externalRuntimeHelpers);
        EmitExternalRuntimeHelperDefinitions(objectModelBuilder, externalRuntimeHelpers);
        EmitStaticInitializationDefinitions(objectModelBuilder);
        EmitGenericRegistration(objectModelBuilder, supplementalMetadataTemplate, metadataRegistration, out var genericRegistrationHelperCode);

        var methodDeclarations = BuildMethodDeclarations(reachableMethods);
        var methods = reachableMethods
            .Select(method => new NativeAotMethodTemplateModel
            {
                SubjectId = method.SubjectId,
                MethodSource = BuildMethodSource(method),
            })
            .ToList();
        var entryBridgeArguments = BuildEntryBridgeArguments(entryMethod);

        var moduleRegistrationCode = BuildModuleRegistration(loweringPlan);

        return new NativeAotTemplateModel
        {
            Includes =
            [
                "<chaos/common.h>",
                "\"runtime_core.h\"",
                "\"codegen_bridge.h\"",
                "\"module_registry.h\"",
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
        };
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
        if (!externalRuntimeHelpers.Any(helper =>
                string.Equals(helper.SubjectId, DelegateCombineMethodSubjectId, StringComparison.Ordinal) ||
                string.Equals(helper.SubjectId, DelegateRemoveMethodSubjectId, StringComparison.Ordinal)))
        {
            return;
        }

        var delegateTypeSubjectIds = CollectReachableDelegateTypeSubjectIds(reachableMethods)
            .Where(subjectId =>
                !string.Equals(subjectId, DelegateTypeSubjectId, StringComparison.Ordinal) &&
                !string.Equals(subjectId, MulticastDelegateTypeSubjectId, StringComparison.Ordinal));

        builder.AppendLine("using chaos_delegate_invocation_list = CHAOS_IL2CPP_VECTOR(CHAOS_IL2CPP_INTPTR);");
        builder.AppendLine();
        builder.AppendLine("chaos_type_System_Private_CoreLib_System_Delegate* chaos_require_delegate(CHAOS_IL2CPP_INTPTR chaos_delegate_value)");
        builder.AppendLine("{");
        builder.AppendLine("    if (chaos_delegate_value == static_cast<CHAOS_IL2CPP_INTPTR>(0))");
        builder.AppendLine("    {");
        builder.AppendLine("        CHAOS_IL2CPP_ABORT();");
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
        builder.AppendLine("            CHAOS_IL2CPP_ABORT();");
        builder.AppendLine("        }");
        builder.AppendLine();
        builder.AppendLine("        chaos_entries.push_back(chaos_delegate_value);");
        builder.AppendLine("        return;");
        builder.AppendLine("    }");
        builder.AppendLine();
        builder.AppendLine(
            "    if (static_cast<CHAOS_IL2CPP_INTPTR>(chaos_invocation_list->size()) != chaos_delegate->chaos_delegate_invocation_count)");
        builder.AppendLine("    {");
        builder.AppendLine("        CHAOS_IL2CPP_ABORT();");
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
        builder.AppendLine("            CHAOS_IL2CPP_ABORT();");
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
        builder.AppendLine("            CHAOS_IL2CPP_ABORT();");
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
            sbyte byteValue => new CustomAttributeLiteralValue(CustomAttributeLiteralKind.Int16, (short)byteValue),
            short shortValue => new CustomAttributeLiteralValue(CustomAttributeLiteralKind.Int16, shortValue),
            int intValue => new CustomAttributeLiteralValue(CustomAttributeLiteralKind.Int32, intValue),
            long longValue => new CustomAttributeLiteralValue(CustomAttributeLiteralKind.Int64, longValue),
            ushort shortValue => new CustomAttributeLiteralValue(CustomAttributeLiteralKind.UInt16, shortValue),
            uint intValue => new CustomAttributeLiteralValue(CustomAttributeLiteralKind.UInt32, intValue),
            ulong longValue => new CustomAttributeLiteralValue(CustomAttributeLiteralKind.UInt64, longValue),
            char charValue => new CustomAttributeLiteralValue(CustomAttributeLiteralKind.UInt16, (ushort)charValue),
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

    /// <summary>
    /// C++ code emitted outside the anonymous namespace that exposes
    /// generic registration arrays (kGenericTypeEntries, etc.) to the
    /// proof host via an extern "C" helper function. Empty string when
    /// there are no generic arrays to expose.
    /// </summary>
    public required string GenericRegistrationCode { get; init; }

    /// <summary>
    /// C++ code for per-DLL module registration via RegisterModule().
    /// Emitted inside the anonymous namespace so the static initializer
    /// runs at module load time. Includes a ModuleDescriptor and the
    /// registration call. Empty string for audit/inventory plan kinds.
    /// </summary>
    public required string ModuleRegistrationCode { get; init; }
}

public sealed record NativeAotMethodTemplateModel
{
    public required string SubjectId { get; init; }

    public required string MethodSource { get; init; }
}


