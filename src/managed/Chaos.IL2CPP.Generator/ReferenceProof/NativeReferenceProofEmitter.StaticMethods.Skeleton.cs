using System.Text;
using System.Text.Json;
using System.Runtime.CompilerServices;
using System.Reflection.Metadata;
using System.Reflection.Metadata.Ecma335;
using System.Reflection.PortableExecutable;
using Chaos.IL2CPP.Contracts;
using Scriban;
using Scriban.Runtime;

namespace Chaos.IL2CPP.Generator;

public sealed partial class NativeReferenceProofEmitter
{





    private static string BuildAssemblyFullClosureRuntimeSkeletonSummaryTranslationUnit(
        NativeReferenceLoweringPlanArtifact loweringPlan,
        IReadOnlyList<RuntimeSkeletonPageEmission> pageEmissions,
        int emittedMethodCount,
        CodeRegistrationArtifact codeRegistration)
    {
        ValidateAssemblyFullClosureRuntimeSkeletonPlan(loweringPlan);
        var emittedPages = pageEmissions
            .Select(pageEmission => pageEmission.Page)
            .ToArray();
        var pageDispatchDeclarations = emittedPages
            .Select(page =>
            {
                var pageDispatchName = BuildAssemblyFullClosureRuntimeSkeletonPageDispatchFunctionName(page.PageNumber);
                return $"""
CHAOS_IL2CPP_INT32 CHAOS_RUNTIME_ABI_CALL {pageDispatchName}(
    const CodegenBridgeV0* bridge,
    const CodeRegistrationV0* code_registration,
    const MetadataRegistrationV0* metadata_registration,
    const CodegenRegistrationOptionsV0* options,
    RuntimeState* runtime,
    ThreadState* thread,
    CHAOS_IL2CPP_UINT32 method_slot,
    void* managed_args);
""";
            })
            .ToArray();
        var methodDispatchCatalogEntries = pageEmissions
            .SelectMany(pageEmission =>
            {
                var pageDispatchName = BuildAssemblyFullClosureRuntimeSkeletonPageDispatchFunctionName(pageEmission.Page.PageNumber);
                return pageEmission.SupportedMethods.Select(method =>
                    $"    {{ {method.MethodId}u, {ToCppStringLiteral(method.SubjectId)}, &{pageDispatchName}, {method.DispatchSlot}u }},");
            })
            .ToArray();
        var typeCapabilityEntries = codeRegistration.TypeCapabilities
            .Select(capability =>
                $"    {{ {FormatCppTokenLiteral(capability.TypeToken)}, {{ sizeof(RuntimeTypeCapabilityInfoV0), {capability.CapabilityBits}u, {capability.ValueSizeBytes}u, {capability.VectorWidthBytes}u, {capability.VectorLaneCount}u, {capability.VectorLaneKind}u, {capability.ScalarKind}u }} }},")
            .ToArray();
        var model = new ScriptObject
        {
            ["assembly_name_literal"] = ToCppStringLiteral(loweringPlan.AssemblyName),
            ["plan_kind_literal"] = ToCppStringLiteral(loweringPlan.PlanKind),
            ["runtime_execution_kind_literal"] = ToCppStringLiteral(loweringPlan.RuntimeExecutionKind ?? "assembly-bound-native-reference-skeleton"),
            ["translation_unit_mode_literal"] = ToCppStringLiteral(loweringPlan.TranslationUnitMode ?? "runtime-skeleton"),
            ["translation_unit_method_count"] = emittedMethodCount,
            ["translation_unit_page_size"] = loweringPlan.TranslationUnitPageSize ?? AuditTranslationUnitPageSize,
            ["translation_unit_page_count"] = emittedPages.Length,
            ["has_method_dispatch_entries"] = methodDispatchCatalogEntries.Length > 0,
            ["page_dispatch_declarations"] = pageDispatchDeclarations,
            ["method_dispatch_catalog_entries"] = methodDispatchCatalogEntries,
            ["has_type_capability_entries"] = typeCapabilityEntries.Length > 0,
            ["type_capability_entries"] = typeCapabilityEntries,
            ["type_capability_entry_count"] = typeCapabilityEntries.Length,
            ["native_entry_function_name"] = loweringPlan.NativeEntryFunctionName,
        };
        return ScribanTemplateRenderer.RenderTemplate(
            ScribanTemplateRenderer.LoadTemplate(NativeReferenceProofCatalog.AssemblyFullClosureRuntimeSkeletonSummaryTemplateRelativePath),
            model);
    }







    private static RuntimeSkeletonPageEmission BuildAssemblyFullClosureRuntimeSkeletonPageTranslationUnit(
        NativeReferenceLoweringPlanArtifact loweringPlan,
        ManagedClosureManifestArtifact closureManifest,
        AssemblyMetadataTokenResolver externalMetadataTokenResolver,
        MetadataRegistrationArtifact metadataRegistration,
        IReadOnlyList<TypedIlMethodArtifact> methods,
        IReadOnlyDictionary<string, TypedIlMethodArtifact> methodsBySubjectId,
        IReadOnlyList<CodeRegistrationEntry> methodPointers,
        IReadOnlyDictionary<string, string> allMethodStubNamesBySubjectId,
        int pageNumber,
        string pagePath,
        IReadOnlyList<string> requestedMethodSubjectIds,
        int pageStartIndex,
        int pageItemCount)
    {
        var methodStubNamesBySubjectId = new Dictionary<string, string>(pageItemCount, StringComparer.Ordinal);
        var methodStubNamesByIndex = new string[pageItemCount];
        for (var index = 0; index < pageItemCount; index++)
        {
            var subjectId = requestedMethodSubjectIds[pageStartIndex + index];
            var stubName = BuildAssemblyFullClosureRuntimeSkeletonMethodStubName(pageNumber, index + 1);
            methodStubNamesByIndex[index] = stubName;
            methodStubNamesBySubjectId.Add(subjectId, stubName);
        }

        var emittedMethods = new List<RuntimeSkeletonMethodEmission>(pageItemCount);
        var unsupportedMethods = new List<RuntimeSkeletonUnsupportedMethodEmission>();
        var pageSupportBuilder = new RuntimeSkeletonPageSupportBuilder();
        for (var index = 0; index < pageItemCount; index++)
        {
            var subjectId = requestedMethodSubjectIds[pageStartIndex + index];
            var stubName = methodStubNamesByIndex[index];
            var stubDefinition = TryBuildAssemblyFullClosureRuntimeSkeletonMethodStub(
                loweringPlan,
                closureManifest,
                externalMetadataTokenResolver,
                metadataRegistration,
                methodPointers,
                methods,
                methodsBySubjectId,
                allMethodStubNamesBySubjectId,
                pageSupportBuilder,
                stubName,
                subjectId);
            if (stubDefinition is null)
            {
                if (methodsBySubjectId.TryGetValue(subjectId, out var requestedMethod) &&
                    IsRuntimeSkeletonNonExecutableDeclaration(requestedMethod))
                {
                    continue;
                }

                unsupportedMethods.Add(new RuntimeSkeletonUnsupportedMethodEmission(subjectId, "unsupportedShapeOrCapability"));
                stubDefinition = BuildAssemblyFullClosureRuntimeSkeletonFallbackStubDefinition(stubName);
            }

            emittedMethods.Add(new RuntimeSkeletonMethodEmission(
                pageStartIndex + emittedMethods.Count + 1,
                subjectId,
                stubName,
                emittedMethods.Count,
                BuildAssemblyFullClosureRuntimeSkeletonMethodStubDeclaration(stubName),
                stubDefinition));
        }

        var methodStubDeclarations = new List<string>(emittedMethods.Count);
        var externalMethodStubDeclarations = new Dictionary<string, RuntimeSkeletonExternalMethodStubDeclaration>(StringComparer.Ordinal);
        var methodStubDefinitions = new List<string>(emittedMethods.Count);
        var dispatchEntries = new List<string>(emittedMethods.Count);
        var supportedMethods = new List<RuntimeSkeletonSupportedMethodDispatch>(emittedMethods.Count);
        foreach (var emission in emittedMethods)
        {
            supportedMethods.Add(new RuntimeSkeletonSupportedMethodDispatch(emission.MethodId, emission.SubjectId, emission.DispatchSlot));
            methodStubDeclarations.Add(emission.StubDeclaration);
            methodStubDefinitions.Add(emission.StubDefinition);
            dispatchEntries.Add($"    &{emission.StubName},");

            foreach (var referencedSubjectId in GetReferencedRuntimeSkeletonMethodSubjectIds(methodsBySubjectId, emission.SubjectId))
            {
                if (methodStubNamesBySubjectId.ContainsKey(referencedSubjectId) ||
                    !allMethodStubNamesBySubjectId.TryGetValue(referencedSubjectId, out var referencedStubName))
                {
                    continue;
                }

                externalMethodStubDeclarations.TryAdd(
                    referencedSubjectId,
                    new RuntimeSkeletonExternalMethodStubDeclaration(
                        referencedSubjectId,
                        referencedStubName,
                        BuildAssemblyFullClosureRuntimeSkeletonMethodStubDeclaration(referencedStubName)));
            }
        }

        var firstEmittedSubjectId = supportedMethods.Count > 0 ? supportedMethods[0].SubjectId : null;
        var lastEmittedSubjectId = supportedMethods.Count > 0 ? supportedMethods[^1].SubjectId : null;
        var model = new ScriptObject
        {
            ["runtime_execution_kind_literal"] = ToCppStringLiteral(loweringPlan.RuntimeExecutionKind ?? "assembly-bound-native-reference-skeleton"),
            ["page_number"] = pageNumber,
            ["page_item_count"] = supportedMethods.Count,
            ["method_stub_declarations"] = methodStubDeclarations.ToArray(),
            ["method_stub_definitions"] = methodStubDefinitions.ToArray(),
            ["field_binding_descriptors"] = pageSupportBuilder.GetFieldBindingDescriptors(),
            ["field_binding_descriptor_count"] = pageSupportBuilder.FieldBindingDescriptorCount,
            ["field_binding_storage_count"] = Math.Max(1, pageSupportBuilder.FieldBindingDescriptorCount),
            ["type_descriptors"] = pageSupportBuilder.GetTypeDescriptors(),
            ["type_descriptor_count"] = pageSupportBuilder.TypeDescriptorCount,
            ["type_storage_count"] = Math.Max(1, pageSupportBuilder.TypeDescriptorCount),
            ["constructor_field_setter_descriptors"] = pageSupportBuilder.GetConstructorFieldSetterDescriptors(),
            ["constructor_field_setter_descriptor_count"] = pageSupportBuilder.ConstructorFieldSetterDescriptorCount,
            ["field_getter_string_return_descriptors"] = pageSupportBuilder.GetFieldGetterStringReturnDescriptors(),
            ["field_getter_string_return_descriptor_count"] = pageSupportBuilder.FieldGetterStringReturnDescriptorCount,
            ["field_backed_string_return_descriptors"] = pageSupportBuilder.GetFieldBackedStringReturnDescriptors(),
            ["field_backed_string_return_descriptor_count"] = pageSupportBuilder.FieldBackedStringReturnDescriptorCount,
            ["field_argument_string_return_descriptors"] = pageSupportBuilder.GetFieldArgumentStringReturnDescriptors(),
            ["field_argument_string_return_descriptor_count"] = pageSupportBuilder.FieldArgumentStringReturnDescriptorCount,
            ["static_call_ctor_getter_descriptors"] = pageSupportBuilder.GetStaticCallCtorGetterDescriptors(),
            ["static_call_ctor_getter_descriptor_count"] = pageSupportBuilder.StaticCallCtorGetterDescriptorCount,
            ["static_string_literal_append_descriptors"] = pageSupportBuilder.GetStaticStringLiteralAppendDescriptors(),
            ["static_string_literal_append_descriptor_count"] = pageSupportBuilder.StaticStringLiteralAppendDescriptorCount,
            ["constructor_then_instance_call_descriptors"] = pageSupportBuilder.GetConstructorThenInstanceCallDescriptors(),
            ["constructor_then_instance_call_descriptor_count"] = pageSupportBuilder.ConstructorThenInstanceCallDescriptorCount,
            ["delegate_closed_target_relay_descriptors"] = pageSupportBuilder.GetDelegateClosedTargetRelayDescriptors(),
            ["delegate_closed_target_relay_descriptor_count"] = pageSupportBuilder.DelegateClosedTargetRelayDescriptorCount,
            ["array_boxing_reference_array_descriptors"] = pageSupportBuilder.GetArrayBoxingReferenceArrayDescriptors(),
            ["array_boxing_reference_array_descriptor_count"] = pageSupportBuilder.ArrayBoxingReferenceArrayDescriptorCount,
            ["array_reverse_reference_array_descriptors"] = pageSupportBuilder.GetArrayReverseReferenceArrayDescriptors(),
            ["array_reverse_reference_array_descriptor_count"] = pageSupportBuilder.ArrayReverseReferenceArrayDescriptorCount,
            ["array_clear_reference_array_descriptors"] = pageSupportBuilder.GetArrayClearReferenceArrayDescriptors(),
            ["array_clear_reference_array_descriptor_count"] = pageSupportBuilder.ArrayClearReferenceArrayDescriptorCount,
            ["array_copy_reference_array_descriptors"] = pageSupportBuilder.GetArrayCopyReferenceArrayDescriptors(),
            ["array_copy_reference_array_descriptor_count"] = pageSupportBuilder.ArrayCopyReferenceArrayDescriptorCount,
            ["external_method_stub_declarations"] = externalMethodStubDeclarations.Values.Select(item => item.Declaration).ToArray(),
            ["dispatch_entries"] = dispatchEntries.ToArray(),
            ["page_dispatch_function_name"] = BuildAssemblyFullClosureRuntimeSkeletonPageDispatchFunctionName(pageNumber),
        };
        return new RuntimeSkeletonPageEmission(
            new AuditTranslationUnitPageArtifact
            {
                PageNumber = pageNumber,
                MethodCount = supportedMethods.Count,
                Path = pagePath,
                FirstMethodSubjectId = firstEmittedSubjectId,
                LastMethodSubjectId = lastEmittedSubjectId,
            },
            ScribanTemplateRenderer.RenderTemplate(
                ScribanTemplateRenderer.LoadTemplate(NativeReferenceProofCatalog.AssemblyFullClosureRuntimeSkeletonPageTemplateRelativePath),
                model),
            supportedMethods,
            unsupportedMethods);
    }







    private static string BuildAssemblyFullClosureRuntimeSkeletonMethodStubName(
        int pageNumber,
        int itemNumber)
    {
        return $"NativeReferenceStub_Page{pageNumber:D4}_Item{itemNumber:D4}";
    }







    private static string BuildAssemblyFullClosureRuntimeSkeletonPageDispatchFunctionName(int pageNumber)
    {
        return $"DispatchRuntimeSkeletonPage{pageNumber:D4}";
    }







    private static string BuildAssemblyFullClosureRuntimeSkeletonCoverageReport(
        NativeReferenceLoweringPlanArtifact loweringPlan,
        int emittedMethodCount,
        IReadOnlyList<RuntimeSkeletonUnsupportedMethodEmission> unsupportedMethods)
    {
        var canonicalSubjectIds = BuildCanonicalSubjectIdLookup(loweringPlan.TranslationUnitMethodSubjectIds ?? []);
        var uncoveredReasonCounts = unsupportedMethods
            .GroupBy(method => method.ReasonCode, StringComparer.Ordinal)
            .ToDictionary(group => group.Key, group => group.Count(), StringComparer.Ordinal);
        var payload = new Dictionary<string, object?>
        {
            ["formatVersion"] = "v0",
            ["artifactKind"] = "nativeReferenceRuntimeSkeletonCoverage",
            ["assemblyName"] = loweringPlan.AssemblyName,
            ["planKind"] = loweringPlan.PlanKind,
            ["translationUnitMode"] = loweringPlan.TranslationUnitMode,
            ["requestedMethodCount"] = loweringPlan.TranslationUnitMethodCount ?? 0,
            ["emittedMethodCount"] = emittedMethodCount,
            ["uncoveredMethodCount"] = unsupportedMethods.Count,
            ["uncoveredReasonCounts"] = uncoveredReasonCounts,
            ["uncoveredMethodSubjectIds"] = unsupportedMethods
                .Select(method => ResolveCanonicalSubjectId(canonicalSubjectIds, method.SubjectId))
                .ToArray(),
            ["uncoveredMethods"] = unsupportedMethods.Select(method => new Dictionary<string, object?>
            {
                ["subjectId"] = method.SubjectId,
                ["canonicalSubjectId"] = ResolveCanonicalSubjectId(canonicalSubjectIds, method.SubjectId),
                ["reasonCode"] = method.ReasonCode,
            }).ToArray(),
        };
        return JsonSerializer.Serialize(payload, new JsonSerializerOptions
        {
            WriteIndented = true,
        });
    }







    private static string BuildAssemblyFullClosureRuntimeSkeletonMethodStubDeclaration(string stubName)
    {
        return $@"CHAOS_IL2CPP_INT32 CHAOS_RUNTIME_ABI_CALL {stubName}(
    const CodegenBridgeV0* bridge,
    const CodeRegistrationV0* code_registration,
    const MetadataRegistrationV0* metadata_registration,
    const CodegenRegistrationOptionsV0* options,
    RuntimeState* runtime,
    ThreadState* thread,
    void* managed_args);";
    }







    private static string BuildAssemblyFullClosureRuntimeSkeletonFallbackStubDefinition(string stubName)
    {
        return $@"CHAOS_IL2CPP_INT32 CHAOS_RUNTIME_ABI_CALL {stubName}(
    const CodegenBridgeV0* bridge,
    const CodeRegistrationV0* code_registration,
    const MetadataRegistrationV0* metadata_registration,
    const CodegenRegistrationOptionsV0* options,
    RuntimeState* runtime,
    ThreadState* thread,
    void* managed_args)
{{
    (void)bridge;
    (void)code_registration;
    (void)metadata_registration;
    (void)options;
    (void)runtime;
    (void)thread;
    (void)managed_args;
    return CHAOS_BRIDGE_STATUS_NOT_SUPPORTED;
}}";
    }







    private static string? TryBuildAssemblyFullClosureRuntimeSkeletonMethodStub(
        NativeReferenceLoweringPlanArtifact loweringPlan,
        ManagedClosureManifestArtifact closureManifest,
        AssemblyMetadataTokenResolver externalMetadataTokenResolver,
        MetadataRegistrationArtifact metadataRegistration,
        IReadOnlyList<CodeRegistrationEntry> methodPointers,
        IReadOnlyList<TypedIlMethodArtifact> methods,
        IReadOnlyDictionary<string, TypedIlMethodArtifact> methodsBySubjectId,
        IReadOnlyDictionary<string, string> methodStubNamesBySubjectId,
        RuntimeSkeletonPageSupportBuilder pageSupportBuilder,
        string stubName,
        string subjectId)
    {
        if (subjectId.Contains("/System.Number::", StringComparison.Ordinal) &&
            TryBuildAssemblyBoundNumberManagedInvokeCore(
                loweringPlan.AssemblyName,
                subjectId,
                metadataRegistration,
                methodsBySubjectId,
                stubName,
                out var numberManagedInvokeStub))
        {
            return numberManagedInvokeStub;
        }

        if (subjectId.Contains("/System.Enum::", StringComparison.Ordinal) &&
            TryBuildAssemblyBoundEnumManagedInvokeCore(
                loweringPlan.AssemblyName,
                subjectId,
                metadataRegistration,
                methodPointers,
                methodsBySubjectId,
                pageSupportBuilder,
                stubName,
                out var enumManagedInvokeStub))
        {
            return enumManagedInvokeStub;
        }

        var buildContext = new RuntimeSkeletonStubBuildContext(
            loweringPlan,
            closureManifest,
            externalMetadataTokenResolver,
            metadataRegistration,
            methodPointers,
            methods,
            methodsBySubjectId,
            methodStubNamesBySubjectId,
            pageSupportBuilder,
            stubName,
            subjectId);
        return TryBuildAssemblyFullClosureRuntimeSkeletonMethodStubVia4CDispatcher(buildContext);
    }







    private static string? TryBuildAssemblyFullClosureRuntimeSkeletonMethodStubVia4CDispatcher(
        RuntimeSkeletonStubBuildContext buildContext)
    {
        foreach (var familyHandler in RuntimeSkeletonFamilyHandlers)
        {
            var result = familyHandler(buildContext);
            if (result.MatchKind == RuntimeSkeletonFamilyHandlerMatchKind.Match)
            {
                return result.StubDefinition;
            }
        }

        return null;
    }

}
