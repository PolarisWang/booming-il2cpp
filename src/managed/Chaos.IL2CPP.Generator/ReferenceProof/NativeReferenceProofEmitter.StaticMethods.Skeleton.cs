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







    private static RuntimeSkeletonFamilyHandlerResult TryBuildRuntimeSkeletonConvertFamilyHandler(
        RuntimeSkeletonStubBuildContext buildContext) =>
        TryBuildRuntimeSkeletonConvertFamilyCore(buildContext);







    private static RuntimeSkeletonFamilyHandlerResult TryBuildRuntimeSkeletonTaskContinuationFamilyHandler(
        RuntimeSkeletonStubBuildContext buildContext) =>
        TryBuildRuntimeSkeletonTaskContinuationFamilyCore(buildContext);







    private static RuntimeSkeletonFamilyHandlerResult TryBuildRuntimeSkeletonStringManagedInvokeFamilyHandler(
        RuntimeSkeletonStubBuildContext buildContext) =>
        TryBuildRuntimeSkeletonStringManagedInvokeFamilyCore(buildContext);







    private static RuntimeSkeletonFamilyHandlerResult TryBuildRuntimeSkeletonUnsafeManagedInvokeFamilyHandler(
        RuntimeSkeletonStubBuildContext buildContext) =>
        TryBuildRuntimeSkeletonUnsafeManagedInvokeFamilyCore(buildContext);







    private static RuntimeSkeletonFamilyHandlerResult TryBuildRuntimeSkeletonMemoryExtensionsManagedInvokeFamilyHandler(
        RuntimeSkeletonStubBuildContext buildContext) =>
        TryBuildRuntimeSkeletonMemoryExtensionsManagedInvokeFamilyCore(buildContext);







    private static RuntimeSkeletonFamilyHandlerResult TryBuildRuntimeSkeletonCollectionsManagedInvokeFamilyHandler(
        RuntimeSkeletonStubBuildContext buildContext) =>
        TryBuildRuntimeSkeletonCollectionsManagedInvokeFamilyCore(buildContext);







    private static RuntimeSkeletonFamilyHandlerResult TryBuildRuntimeSkeletonCollectionsKernelFamilyHandler(
        RuntimeSkeletonStubBuildContext buildContext) =>
        TryBuildRuntimeSkeletonCollectionsKernelFamilyCore(buildContext);







    private static RuntimeSkeletonFamilyHandlerResult TryBuildRuntimeSkeletonBufferMemoryKernelFamilyHandler(
        RuntimeSkeletonStubBuildContext buildContext) =>
        TryBuildRuntimeSkeletonBufferMemoryKernelFamilyCore(buildContext);







    private static RuntimeSkeletonFamilyHandlerResult TryBuildRuntimeSkeletonSpanHelpersKernelFamilyHandler(
        RuntimeSkeletonStubBuildContext buildContext) =>
        TryBuildRuntimeSkeletonSpanHelpersKernelFamilyCore(buildContext);







    private static RuntimeSkeletonFamilyHandlerResult TryBuildRuntimeSkeletonMathKernelFamilyHandler(
        RuntimeSkeletonStubBuildContext buildContext) =>
        TryBuildRuntimeSkeletonMathKernelFamilyCore(buildContext);







    private static RuntimeSkeletonFamilyHandlerResult TryBuildRuntimeSkeletonValueTypeKernelFamilyHandler(
        RuntimeSkeletonStubBuildContext buildContext) =>
        TryBuildRuntimeSkeletonValueTypeKernelFamilyCore(buildContext);







    private static RuntimeSkeletonFamilyHandlerResult TryBuildRuntimeSkeletonVectorKernelFamilyHandler(
        RuntimeSkeletonStubBuildContext buildContext) =>
        TryBuildRuntimeSkeletonVectorKernelFamilyCore(buildContext);







    private static RuntimeSkeletonFamilyHandlerResult TryBuildRuntimeSkeletonVectorManagedInvokeFamilyHandler(
        RuntimeSkeletonStubBuildContext buildContext) =>
        TryBuildRuntimeSkeletonVectorManagedInvokeFamilyCore(buildContext);







    private static RuntimeSkeletonFamilyHandlerResult TryBuildRuntimeSkeletonArrayAndMemoryMarshalFamilyHandler(
        RuntimeSkeletonStubBuildContext buildContext) =>
        TryBuildRuntimeSkeletonArrayAndMemoryMarshalFamilyCore(buildContext);







    private static RuntimeSkeletonFamilyHandlerResult TryBuildRuntimeSkeletonMarshalPlatformCapabilityFamilyHandler(
        RuntimeSkeletonStubBuildContext buildContext) =>
        TryBuildRuntimeSkeletonMarshalPlatformCapabilityFamilyCore(buildContext);







    private static RuntimeSkeletonFamilyHandlerResult TryBuildRuntimeSkeletonNativeRuntimeEventSourcePlatformCapabilityFamilyHandler(
        RuntimeSkeletonStubBuildContext buildContext) =>
        TryBuildRuntimeSkeletonNativeRuntimeEventSourcePlatformCapabilityFamilyCore(buildContext);







    private static RuntimeSkeletonFamilyHandlerResult TryBuildRuntimeSkeletonHalfManagedInvokeFamilyHandler(
        RuntimeSkeletonStubBuildContext buildContext) =>
        TryBuildRuntimeSkeletonHalfManagedInvokeFamilyCore(buildContext);







    private static RuntimeSkeletonFamilyHandlerResult TryBuildRuntimeSkeletonFloatingScalarManagedInvokeFamilyHandler(
        RuntimeSkeletonStubBuildContext buildContext) =>
        TryBuildRuntimeSkeletonFloatingScalarManagedInvokeFamilyCore(buildContext);







    private static RuntimeSkeletonFamilyHandlerResult TryBuildRuntimeSkeletonPrimitiveScalarManagedInvokeFamilyHandler(
        RuntimeSkeletonStubBuildContext buildContext) =>
        TryBuildRuntimeSkeletonPrimitiveScalarManagedInvokeFamilyCore(buildContext);







    private static RuntimeSkeletonFamilyHandlerResult TryBuildRuntimeSkeletonWideNumericManagedInvokeFamilyHandler(
        RuntimeSkeletonStubBuildContext buildContext) =>
        TryBuildRuntimeSkeletonWideNumericManagedInvokeFamilyCore(buildContext);







    private static RuntimeSkeletonFamilyHandlerResult TryBuildRuntimeSkeletonCalendarStructManagedInvokeFamilyHandler(
        RuntimeSkeletonStubBuildContext buildContext) =>
        TryBuildRuntimeSkeletonCalendarStructManagedInvokeFamilyCore(buildContext);







    private static RuntimeSkeletonFamilyHandlerResult TryBuildRuntimeSkeletonIdentityStructManagedInvokeFamilyHandler(
        RuntimeSkeletonStubBuildContext buildContext) =>
        TryBuildRuntimeSkeletonIdentityStructManagedInvokeFamilyCore(buildContext);







    private static RuntimeSkeletonFamilyHandlerResult TryBuildRuntimeSkeletonCompanionManagedInvokeFamilyHandler(
        RuntimeSkeletonStubBuildContext buildContext) =>
        TryBuildRuntimeSkeletonCompanionManagedInvokeFamilyCore(buildContext);







    private static RuntimeSkeletonFamilyHandlerResult TryBuildRuntimeSkeletonDateTimeSemanticEngineFamilyHandler(
        RuntimeSkeletonStubBuildContext buildContext) =>
        TryBuildRuntimeSkeletonDateTimeSemanticEngineFamilyCore(buildContext);







    private static RuntimeSkeletonFamilyHandlerResult TryBuildRuntimeSkeletonGlobalizationDateTimeSupportFamilyHandler(
        RuntimeSkeletonStubBuildContext buildContext) =>
        TryBuildRuntimeSkeletonGlobalizationDateTimeSupportFamilyCore(buildContext);







    private static RuntimeSkeletonFamilyHandlerResult TryBuildRuntimeSkeletonNumberManagedInvokeFamilyHandler(
        RuntimeSkeletonStubBuildContext buildContext) =>
        TryBuildRuntimeSkeletonNumberManagedInvokeFamilyCore(buildContext);







    private static RuntimeSkeletonFamilyHandlerResult TryBuildRuntimeSkeletonDecimalManagedInvokeFamilyHandler(
        RuntimeSkeletonStubBuildContext buildContext) =>
        TryBuildRuntimeSkeletonDecimalManagedInvokeFamilyCore(buildContext);







    private static RuntimeSkeletonFamilyHandlerResult TryBuildRuntimeSkeletonCharManagedInvokeFamilyHandler(
        RuntimeSkeletonStubBuildContext buildContext) =>
        TryBuildRuntimeSkeletonCharManagedInvokeFamilyCore(buildContext);







    private static RuntimeSkeletonFamilyHandlerResult TryBuildRuntimeSkeletonEnumManagedInvokeFamilyHandler(
        RuntimeSkeletonStubBuildContext buildContext) =>
        TryBuildRuntimeSkeletonEnumManagedInvokeFamilyCore(buildContext);







    private static RuntimeSkeletonFamilyHandlerResult TryBuildRuntimeSkeletonStringManagedInvokeHandler(
        RuntimeSkeletonStubBuildContext buildContext) =>
        TryBuildRuntimeSkeletonStringManagedInvokeCore(buildContext);







    private static RuntimeSkeletonFamilyHandlerResult TryBuildRuntimeSkeletonUnsafeManagedInvokeHandler(
        RuntimeSkeletonStubBuildContext buildContext) =>
        TryBuildRuntimeSkeletonUnsafeManagedInvokeCore(buildContext);







    private static RuntimeSkeletonFamilyHandlerResult TryBuildRuntimeSkeletonMemoryExtensionsManagedInvokeHandler(
        RuntimeSkeletonStubBuildContext buildContext) =>
        TryBuildRuntimeSkeletonMemoryExtensionsManagedInvokeCore(buildContext);







    private static RuntimeSkeletonFamilyHandlerResult TryBuildRuntimeSkeletonSpanHelpersKernelHandler(
        RuntimeSkeletonStubBuildContext buildContext) =>
        TryBuildRuntimeSkeletonSpanHelpersKernelCore(buildContext);







    private static RuntimeSkeletonFamilyHandlerResult TryBuildRuntimeSkeletonValueTypeKernelHandler(
        RuntimeSkeletonStubBuildContext buildContext) =>
        TryBuildRuntimeSkeletonValueTypeKernelCore(buildContext);







    private static RuntimeSkeletonFamilyHandlerResult TryBuildRuntimeSkeletonVectorKernelHandler(
        RuntimeSkeletonStubBuildContext buildContext) =>
        TryBuildRuntimeSkeletonVectorKernelCore(buildContext);







    private static RuntimeSkeletonFamilyHandlerResult TryBuildRuntimeSkeletonArrayAndMemoryMarshalHandler(
        RuntimeSkeletonStubBuildContext buildContext) =>
        TryBuildRuntimeSkeletonArrayAndMemoryMarshalCore(buildContext);







    private static RuntimeSkeletonFamilyHandlerResult TryBuildRuntimeSkeletonHalfManagedInvokeHandler(
        RuntimeSkeletonStubBuildContext buildContext) =>
        TryBuildRuntimeSkeletonHalfManagedInvokeCore(buildContext);







    private static RuntimeSkeletonFamilyHandlerResult TryBuildRuntimeSkeletonFloatingScalarManagedInvokeHandler(
        RuntimeSkeletonStubBuildContext buildContext) =>
        TryBuildRuntimeSkeletonFloatingScalarManagedInvokeCore(buildContext);







    private static RuntimeSkeletonFamilyHandlerResult TryBuildRuntimeSkeletonPrimitiveScalarManagedInvokeHandler(
        RuntimeSkeletonStubBuildContext buildContext) =>
        TryBuildRuntimeSkeletonPrimitiveScalarManagedInvokeCore(buildContext);







    private static RuntimeSkeletonFamilyHandlerResult TryBuildRuntimeSkeletonWideNumericManagedInvokeHandler(
        RuntimeSkeletonStubBuildContext buildContext) =>
        TryBuildRuntimeSkeletonWideNumericManagedInvokeCore(buildContext);







    private static RuntimeSkeletonFamilyHandlerResult TryBuildRuntimeSkeletonCalendarStructManagedInvokeHandler(
        RuntimeSkeletonStubBuildContext buildContext) =>
        TryBuildRuntimeSkeletonCalendarStructManagedInvokeCore(buildContext);







    private static RuntimeSkeletonFamilyHandlerResult TryBuildRuntimeSkeletonIdentityStructManagedInvokeHandler(
        RuntimeSkeletonStubBuildContext buildContext) =>
        TryBuildRuntimeSkeletonIdentityStructManagedInvokeCore(buildContext);







    private static RuntimeSkeletonFamilyHandlerResult TryBuildRuntimeSkeletonCompanionManagedInvokeHandler(
        RuntimeSkeletonStubBuildContext buildContext) =>
        TryBuildRuntimeSkeletonCompanionManagedInvokeCore(buildContext);







    private static RuntimeSkeletonFamilyHandlerResult TryBuildRuntimeSkeletonDateTimeSemanticEngineHandler(
        RuntimeSkeletonStubBuildContext buildContext) =>
        TryBuildRuntimeSkeletonDateTimeSemanticEngineCore(buildContext);







    private static RuntimeSkeletonFamilyHandlerResult TryBuildRuntimeSkeletonGlobalizationDateTimeSupportHandler(
        RuntimeSkeletonStubBuildContext buildContext) =>
        TryBuildRuntimeSkeletonGlobalizationDateTimeSupportCore(buildContext);







    private static RuntimeSkeletonFamilyHandlerResult TryBuildRuntimeSkeletonNumberManagedInvokeHandler(
        RuntimeSkeletonStubBuildContext buildContext) =>
        TryBuildRuntimeSkeletonNumberManagedInvokeCore(buildContext);







    private static RuntimeSkeletonFamilyHandlerResult TryBuildRuntimeSkeletonDecimalManagedInvokeHandler(
        RuntimeSkeletonStubBuildContext buildContext) =>
        TryBuildRuntimeSkeletonDecimalManagedInvokeCore(buildContext);







    private static RuntimeSkeletonFamilyHandlerResult TryBuildRuntimeSkeletonCharManagedInvokeHandler(
        RuntimeSkeletonStubBuildContext buildContext) =>
        TryBuildRuntimeSkeletonCharManagedInvokeCore(buildContext);







    private static RuntimeSkeletonFamilyHandlerResult TryBuildRuntimeSkeletonEnumManagedInvokeHandler(
        RuntimeSkeletonStubBuildContext buildContext) =>
        TryBuildRuntimeSkeletonEnumManagedInvokeCore(buildContext);







    private static RuntimeSkeletonFamilyHandlerResult TryBuildRuntimeSkeletonConvertRuntimeHelperHandler(
        RuntimeSkeletonStubBuildContext buildContext) =>
        TryBuildRuntimeSkeletonConvertRuntimeHelperCore(buildContext);







    [RuntimeSkeletonFamilyPlugin("convert-like-int-forwarder", CodeGenPluginProductLine.NativeReference, 0, RuntimeSkeletonFamilyPluginKind.ManagedInvoke)]
    private static RuntimeSkeletonFamilyHandlerResult TryBuildRuntimeSkeletonConvertLikeIntForwarderHandler(
        RuntimeSkeletonStubBuildContext buildContext) =>
        TryBuildRuntimeSkeletonConvertLikeIntForwarderCore(buildContext);







    [RuntimeSkeletonFamilyPlugin("convert-like-bool-identity-forwarder", CodeGenPluginProductLine.NativeReference, 1, RuntimeSkeletonFamilyPluginKind.ManagedInvoke)]
    private static RuntimeSkeletonFamilyHandlerResult TryBuildRuntimeSkeletonConvertLikeBoolIdentityForwarderHandler(
        RuntimeSkeletonStubBuildContext buildContext) =>
        TryBuildRuntimeSkeletonConvertLikeBoolIdentityForwarderCore(buildContext);







    [RuntimeSkeletonFamilyPlugin("convert-like-bool-producer-forwarder", CodeGenPluginProductLine.NativeReference, 2, RuntimeSkeletonFamilyPluginKind.ManagedInvoke)]
    private static RuntimeSkeletonFamilyHandlerResult TryBuildRuntimeSkeletonConvertLikeBoolProducerForwarderHandler(
        RuntimeSkeletonStubBuildContext buildContext) =>
        TryBuildRuntimeSkeletonConvertLikeBoolProducerForwarderCore(buildContext);







    [RuntimeSkeletonFamilyPlugin("convert-like-primitive", CodeGenPluginProductLine.NativeReference, 8, RuntimeSkeletonFamilyPluginKind.ManagedInvoke)]
    private static RuntimeSkeletonFamilyHandlerResult TryBuildRuntimeSkeletonConvertLikePrimitiveHandler(
        RuntimeSkeletonStubBuildContext buildContext) =>
        TryBuildRuntimeSkeletonConvertLikePrimitiveCore(buildContext);







    [RuntimeSkeletonFamilyPlugin("convert-like-string-char-provider", CodeGenPluginProductLine.NativeReference, 4, RuntimeSkeletonFamilyPluginKind.ManagedInvoke)]
    private static RuntimeSkeletonFamilyHandlerResult TryBuildRuntimeSkeletonConvertLikeStringCharProviderHandler(
        RuntimeSkeletonStubBuildContext buildContext) =>
        TryBuildRuntimeSkeletonConvertLikeStringCharProviderCore(buildContext);







    [RuntimeSkeletonFamilyPlugin("convert-like-string-provider-passthrough", CodeGenPluginProductLine.NativeReference, 3, RuntimeSkeletonFamilyPluginKind.ManagedInvoke)]
    private static RuntimeSkeletonFamilyHandlerResult TryBuildRuntimeSkeletonConvertLikeStringProviderPassthroughHandler(
        RuntimeSkeletonStubBuildContext buildContext) =>
        TryBuildRuntimeSkeletonConvertLikeStringProviderPassthroughCore(buildContext);







    [RuntimeSkeletonFamilyPlugin("convert-like-object-char-provider", CodeGenPluginProductLine.NativeReference, 5, RuntimeSkeletonFamilyPluginKind.ManagedInvoke)]
    private static RuntimeSkeletonFamilyHandlerResult TryBuildRuntimeSkeletonConvertLikeObjectCharProviderHandler(
        RuntimeSkeletonStubBuildContext buildContext) =>
        TryBuildRuntimeSkeletonConvertLikeObjectCharProviderCore(buildContext);







    [RuntimeSkeletonFamilyPlugin("convert-like-boxed-value-type-string-instance-call", CodeGenPluginProductLine.NativeReference, 6, RuntimeSkeletonFamilyPluginKind.ManagedInvoke)]
    private static RuntimeSkeletonFamilyHandlerResult TryBuildRuntimeSkeletonConvertLikeBoxedValueTypeStringInstanceCallHandler(
        RuntimeSkeletonStubBuildContext buildContext) =>
        TryBuildRuntimeSkeletonConvertLikeBoxedValueTypeStringInstanceCallCore(buildContext);







    [RuntimeSkeletonFamilyPlugin("convert-like-boxed-value-type-instance-call", CodeGenPluginProductLine.NativeReference, 13, RuntimeSkeletonFamilyPluginKind.ManagedInvoke)]
    private static RuntimeSkeletonFamilyHandlerResult TryBuildRuntimeSkeletonConvertLikeBoxedValueTypeInstanceCallHandler(
        RuntimeSkeletonStubBuildContext buildContext) =>
        TryBuildRuntimeSkeletonConvertLikeBoxedValueTypeInstanceCallCore(buildContext);







    [RuntimeSkeletonFamilyPlugin("convert-like-static-string-call-forwarder", CodeGenPluginProductLine.NativeReference, 7, RuntimeSkeletonFamilyPluginKind.ManagedInvoke)]
    private static RuntimeSkeletonFamilyHandlerResult TryBuildRuntimeSkeletonConvertLikeStaticStringCallForwarderHandler(
        RuntimeSkeletonStubBuildContext buildContext) =>
        TryBuildRuntimeSkeletonConvertLikeStaticStringCallForwarderCore(buildContext);







    [RuntimeSkeletonFamilyPlugin("convert-like-static-value-type-call-forwarder", CodeGenPluginProductLine.NativeReference, 14, RuntimeSkeletonFamilyPluginKind.ManagedInvoke)]
    private static RuntimeSkeletonFamilyHandlerResult TryBuildRuntimeSkeletonConvertLikeStaticValueTypeCallForwarderHandler(
        RuntimeSkeletonStubBuildContext buildContext) =>
        TryBuildRuntimeSkeletonConvertLikeStaticValueTypeCallForwarderCore(buildContext);







    [RuntimeSkeletonFamilyPlugin("convert-like-instance-value-type-call-forwarder", CodeGenPluginProductLine.NativeReference, 15, RuntimeSkeletonFamilyPluginKind.ManagedInvoke)]
    private static RuntimeSkeletonFamilyHandlerResult TryBuildRuntimeSkeletonConvertLikeInstanceValueTypeCallForwarderHandler(
        RuntimeSkeletonStubBuildContext buildContext) =>
        TryBuildRuntimeSkeletonConvertLikeInstanceValueTypeCallForwarderCore(buildContext);







    [RuntimeSkeletonFamilyPlugin("convert-like-value-type-identity-forwarder", CodeGenPluginProductLine.NativeReference, 16, RuntimeSkeletonFamilyPluginKind.ManagedInvoke)]
    private static RuntimeSkeletonFamilyHandlerResult TryBuildRuntimeSkeletonConvertLikeValueTypeIdentityForwarderHandler(
        RuntimeSkeletonStubBuildContext buildContext) =>
        TryBuildRuntimeSkeletonConvertLikeValueTypeIdentityForwarderCore(buildContext);







    [RuntimeSkeletonFamilyPlugin("convert-like-boxed-value-type-char-invalid-cast", CodeGenPluginProductLine.NativeReference, 9, RuntimeSkeletonFamilyPluginKind.ManagedInvoke)]
    private static RuntimeSkeletonFamilyHandlerResult TryBuildRuntimeSkeletonConvertLikeBoxedValueTypeCharInvalidCastHandler(
        RuntimeSkeletonStubBuildContext buildContext) =>
        TryBuildRuntimeSkeletonConvertLikeBoxedValueTypeCharInvalidCastCore(buildContext);







    [RuntimeSkeletonFamilyPlugin("convert-like-boxed-iconvertible-char-invalid-cast", CodeGenPluginProductLine.NativeReference, 10, RuntimeSkeletonFamilyPluginKind.ManagedInvoke)]
    private static RuntimeSkeletonFamilyHandlerResult TryBuildRuntimeSkeletonConvertLikeBoxedIConvertibleCharInvalidCastHandler(
        RuntimeSkeletonStubBuildContext buildContext) =>
        TryBuildRuntimeSkeletonConvertLikeBoxedIConvertibleCharInvalidCastCore(buildContext);







    [RuntimeSkeletonFamilyPlugin("convert-like-checked-primitive", CodeGenPluginProductLine.NativeReference, 11, RuntimeSkeletonFamilyPluginKind.ManagedInvoke)]
    private static RuntimeSkeletonFamilyHandlerResult TryBuildRuntimeSkeletonConvertLikeCheckedPrimitiveHandler(
        RuntimeSkeletonStubBuildContext buildContext) =>
        TryBuildRuntimeSkeletonConvertLikeCheckedPrimitiveCore(buildContext);







    [RuntimeSkeletonFamilyPlugin("convert-like-byte-forwarder", CodeGenPluginProductLine.NativeReference, 12, RuntimeSkeletonFamilyPluginKind.ManagedInvoke)]
    private static RuntimeSkeletonFamilyHandlerResult TryBuildRuntimeSkeletonConvertLikeByteForwarderHandler(
        RuntimeSkeletonStubBuildContext buildContext) =>
        TryBuildRuntimeSkeletonConvertLikeByteForwarderCore(buildContext);







    [RuntimeSkeletonFamilyPlugin("constructor-field-setter", CodeGenPluginProductLine.NativeReference, 17, RuntimeSkeletonFamilyPluginKind.ManagedInvoke)]
    private static RuntimeSkeletonFamilyHandlerResult TryBuildRuntimeSkeletonConstructorFieldSetterHandler(
        RuntimeSkeletonStubBuildContext buildContext)
    {
        if (TryBuildAssemblyBoundConstructorFieldSetterStub(
                buildContext.LoweringPlan.AssemblyName,
                buildContext.SubjectId,
                buildContext.MetadataRegistration,
                buildContext.MethodsBySubjectId,
                buildContext.PageSupportBuilder,
                buildContext.StubName,
                out var stubDefinition))
        {
            return RuntimeSkeletonFamilyHandlerResult.CreateMatch(stubDefinition);
        }

        return RuntimeSkeletonFamilyHandlerResult.NoMatch;
    }







    [RuntimeSkeletonFamilyPlugin("field-backed-string-return", CodeGenPluginProductLine.NativeReference, 18, RuntimeSkeletonFamilyPluginKind.ManagedInvoke)]
    private static RuntimeSkeletonFamilyHandlerResult TryBuildRuntimeSkeletonFieldBackedStringReturnHandler(
        RuntimeSkeletonStubBuildContext buildContext)
    {
        if (TryBuildAssemblyBoundFieldBackedStringReturnStub(
                buildContext.LoweringPlan.AssemblyName,
                buildContext.SubjectId,
                buildContext.MetadataRegistration,
                buildContext.MethodsBySubjectId,
                buildContext.PageSupportBuilder,
                buildContext.StubName,
                out var stubDefinition))
        {
            return RuntimeSkeletonFamilyHandlerResult.CreateMatch(stubDefinition);
        }

        return RuntimeSkeletonFamilyHandlerResult.NoMatch;
    }







    [RuntimeSkeletonFamilyPlugin("field-argument-string-return", CodeGenPluginProductLine.NativeReference, 19, RuntimeSkeletonFamilyPluginKind.ManagedInvoke)]
    private static RuntimeSkeletonFamilyHandlerResult TryBuildRuntimeSkeletonFieldArgumentStringReturnHandler(
        RuntimeSkeletonStubBuildContext buildContext)
    {
        if (TryBuildAssemblyBoundFieldArgumentStringReturnStub(
                buildContext.LoweringPlan.AssemblyName,
                buildContext.SubjectId,
                buildContext.MetadataRegistration,
                buildContext.MethodsBySubjectId,
                buildContext.PageSupportBuilder,
                buildContext.StubName,
                out var stubDefinition))
        {
            return RuntimeSkeletonFamilyHandlerResult.CreateMatch(stubDefinition);
        }

        return RuntimeSkeletonFamilyHandlerResult.NoMatch;
    }







    [RuntimeSkeletonFamilyPlugin("field-getter-string-return", CodeGenPluginProductLine.NativeReference, 20, RuntimeSkeletonFamilyPluginKind.ManagedInvoke)]
    private static RuntimeSkeletonFamilyHandlerResult TryBuildRuntimeSkeletonFieldGetterStringReturnHandler(
        RuntimeSkeletonStubBuildContext buildContext)
    {
        if (TryBuildAssemblyBoundFieldGetterStringReturnStub(
                buildContext.LoweringPlan.AssemblyName,
                buildContext.SubjectId,
                buildContext.MetadataRegistration,
                buildContext.MethodsBySubjectId,
                buildContext.PageSupportBuilder,
                buildContext.StubName,
                out var stubDefinition))
        {
            return RuntimeSkeletonFamilyHandlerResult.CreateMatch(stubDefinition);
        }

        return RuntimeSkeletonFamilyHandlerResult.NoMatch;
    }







    [RuntimeSkeletonFamilyPlugin("delegate-closed-target-relay", CodeGenPluginProductLine.NativeReference, 21, RuntimeSkeletonFamilyPluginKind.ManagedInvoke)]
    private static RuntimeSkeletonFamilyHandlerResult TryBuildRuntimeSkeletonDelegateClosedTargetRelayHandler(
        RuntimeSkeletonStubBuildContext buildContext)
    {
        if (TryBuildAssemblyBoundDelegateClosedTargetRelayPlan(
                buildContext.LoweringPlan.AssemblyName,
                buildContext.SubjectId,
                buildContext.Methods,
                buildContext.MetadataRegistration,
                buildContext.MethodPointers,
                out var loweringPlan))
        {
            return RuntimeSkeletonFamilyHandlerResult.CreateMatch(
                BuildAssemblyBoundDelegateClosedTargetRelayStub(
                    loweringPlan,
                    buildContext.PageSupportBuilder,
                    buildContext.StubName));
        }

        return RuntimeSkeletonFamilyHandlerResult.NoMatch;
    }







    [RuntimeSkeletonFamilyPlugin("static-void-object-sink-no-op", CodeGenPluginProductLine.NativeReference, 22, RuntimeSkeletonFamilyPluginKind.Kernel)]
    private static RuntimeSkeletonFamilyHandlerResult TryBuildRuntimeSkeletonStaticVoidObjectSinkNoOpHandler(
        RuntimeSkeletonStubBuildContext buildContext)
    {
        if (TryBuildAssemblyBoundStaticVoidObjectSinkNoOpStub(
                buildContext.SubjectId,
                buildContext.MethodsBySubjectId,
                buildContext.StubName,
                out var stubDefinition))
        {
            return RuntimeSkeletonFamilyHandlerResult.CreateMatch(stubDefinition);
        }

        return RuntimeSkeletonFamilyHandlerResult.NoMatch;
    }







    [RuntimeSkeletonFamilyPlugin("static-bool-forwarder", CodeGenPluginProductLine.NativeReference, 23, RuntimeSkeletonFamilyPluginKind.Kernel)]
    private static RuntimeSkeletonFamilyHandlerResult TryBuildRuntimeSkeletonStaticBoolForwarderHandler(
        RuntimeSkeletonStubBuildContext buildContext)
    {
        if (TryBuildAssemblyBoundStaticBoolForwarderStub(
                buildContext.SubjectId,
                buildContext.MethodsBySubjectId,
                buildContext.MethodStubNamesBySubjectId,
                buildContext.StubName,
                out var stubDefinition))
        {
            return RuntimeSkeletonFamilyHandlerResult.CreateMatch(stubDefinition);
        }

        return RuntimeSkeletonFamilyHandlerResult.NoMatch;
    }







    [RuntimeSkeletonFamilyPlugin("console-write-line", CodeGenPluginProductLine.NativeReference, 24, RuntimeSkeletonFamilyPluginKind.Kernel)]
    private static RuntimeSkeletonFamilyHandlerResult TryBuildRuntimeSkeletonConsoleWriteLineHandler(
        RuntimeSkeletonStubBuildContext buildContext)
    {
        if (TryBuildAssemblyBoundConsoleWriteLineStub(
                buildContext.SubjectId,
                buildContext.MethodsBySubjectId,
                buildContext.StubName,
                out var stubDefinition))
        {
            return RuntimeSkeletonFamilyHandlerResult.CreateMatch(stubDefinition);
        }

        return RuntimeSkeletonFamilyHandlerResult.NoMatch;
    }







    [RuntimeSkeletonFamilyPlugin("interface-dispatch-message", CodeGenPluginProductLine.NativeReference, 25, RuntimeSkeletonFamilyPluginKind.PlatformCapability)]
    private static RuntimeSkeletonFamilyHandlerResult TryBuildRuntimeSkeletonInterfaceDispatchMessageHandler(
        RuntimeSkeletonStubBuildContext buildContext)
    {
        if (TryBuildAssemblyBoundInterfaceDispatchMessagePlan(
                buildContext.LoweringPlan.AssemblyName,
                buildContext.SubjectId,
                buildContext.MetadataRegistration,
                buildContext.MethodPointers,
                buildContext.Methods,
                out var loweringPlan))
        {
            return RuntimeSkeletonFamilyHandlerResult.CreateMatch(
                BuildAssemblyBoundInterfaceDispatchMessageStub(loweringPlan, buildContext.StubName));
        }

        return RuntimeSkeletonFamilyHandlerResult.NoMatch;
    }







    [RuntimeSkeletonFamilyPlugin("threading-thread-static-monitor", CodeGenPluginProductLine.NativeReference, 26, RuntimeSkeletonFamilyPluginKind.PlatformCapability)]
    private static RuntimeSkeletonFamilyHandlerResult TryBuildRuntimeSkeletonThreadingThreadStaticMonitorHandler(
        RuntimeSkeletonStubBuildContext buildContext)
    {
        if (TryBuildAssemblyBoundThreadingThreadStaticMonitorPlan(
                buildContext.LoweringPlan.AssemblyName,
                buildContext.SubjectId,
                buildContext.MetadataRegistration,
                buildContext.MethodPointers,
                out var loweringPlan))
        {
            return RuntimeSkeletonFamilyHandlerResult.CreateMatch(
                BuildAssemblyBoundThreadingThreadStaticMonitorStub(loweringPlan, buildContext.StubName));
        }

        return RuntimeSkeletonFamilyHandlerResult.NoMatch;
    }







    [RuntimeSkeletonFamilyPlugin("marshaling-utf8-export", CodeGenPluginProductLine.NativeReference, 27, RuntimeSkeletonFamilyPluginKind.ImportedBridge)]
    private static RuntimeSkeletonFamilyHandlerResult TryBuildRuntimeSkeletonMarshalingUtf8ExportHandler(
        RuntimeSkeletonStubBuildContext buildContext)
    {
        if (TryBuildAssemblyBoundMarshalingUtf8ExportPlan(
                buildContext.LoweringPlan.AssemblyName,
                buildContext.SubjectId,
                buildContext.MetadataRegistration,
                buildContext.MethodPointers,
                out var loweringPlan))
        {
            return RuntimeSkeletonFamilyHandlerResult.CreateMatch(
                BuildAssemblyBoundMarshalingUtf8ExportStub(loweringPlan, buildContext.StubName));
        }

        return RuntimeSkeletonFamilyHandlerResult.NoMatch;
    }







    [RuntimeSkeletonFamilyPlugin("reflection-interop-closure", CodeGenPluginProductLine.NativeReference, 28, RuntimeSkeletonFamilyPluginKind.ImportedBridge)]
    private static RuntimeSkeletonFamilyHandlerResult TryBuildRuntimeSkeletonReflectionInteropClosureHandler(
        RuntimeSkeletonStubBuildContext buildContext)
    {
        if (TryBuildAssemblyBoundReflectionInteropClosurePlan(
                buildContext.LoweringPlan.AssemblyName,
                buildContext.SubjectId,
                buildContext.MetadataRegistration,
                buildContext.MethodPointers,
                out var loweringPlan))
        {
            return RuntimeSkeletonFamilyHandlerResult.CreateMatch(
                BuildAssemblyBoundReflectionInteropClosureStub(loweringPlan, buildContext.StubName));
        }

        return RuntimeSkeletonFamilyHandlerResult.NoMatch;
    }







    [RuntimeSkeletonFamilyPlugin("pinvoke-direct-call", CodeGenPluginProductLine.NativeReference, 29, RuntimeSkeletonFamilyPluginKind.ImportedBridge)]
    private static RuntimeSkeletonFamilyHandlerResult TryBuildRuntimeSkeletonPInvokeDirectCallHandler(
        RuntimeSkeletonStubBuildContext buildContext)
    {
        if (TryBuildAssemblyBoundPInvokeDirectCallPlan(
                buildContext.LoweringPlan.AssemblyName,
                buildContext.SubjectId,
                buildContext.MetadataRegistration,
                buildContext.MethodPointers,
                buildContext.Methods,
                out var loweringPlan))
        {
            return RuntimeSkeletonFamilyHandlerResult.CreateMatch(
                BuildAssemblyBoundPInvokeDllImportMinimalStub(loweringPlan, buildContext.StubName));
        }

        return RuntimeSkeletonFamilyHandlerResult.NoMatch;
    }







    [RuntimeSkeletonFamilyPlugin("async-task-factory", CodeGenPluginProductLine.NativeReference, 30, RuntimeSkeletonFamilyPluginKind.ManagedInvoke)]
    private static RuntimeSkeletonFamilyHandlerResult TryBuildRuntimeSkeletonAsyncTaskFactoryHandler(
        RuntimeSkeletonStubBuildContext buildContext)
    {
        if (TryBuildAssemblyBoundAsyncTaskFactoryStub(
                buildContext.LoweringPlan.AssemblyName,
                buildContext.SubjectId,
                buildContext.MetadataRegistration,
                buildContext.MethodsBySubjectId,
                buildContext.StubName,
                out var stubDefinition))
        {
            return RuntimeSkeletonFamilyHandlerResult.CreateMatch(stubDefinition);
        }

        return RuntimeSkeletonFamilyHandlerResult.NoMatch;
    }







    [RuntimeSkeletonFamilyPlugin("async-get-result", CodeGenPluginProductLine.NativeReference, 31, RuntimeSkeletonFamilyPluginKind.ManagedInvoke)]
    private static RuntimeSkeletonFamilyHandlerResult TryBuildRuntimeSkeletonAsyncGetResultHandler(
        RuntimeSkeletonStubBuildContext buildContext)
    {
        if (TryBuildAssemblyBoundAsyncGetResultStub(
                buildContext.LoweringPlan.AssemblyName,
                buildContext.SubjectId,
                buildContext.MetadataRegistration,
                buildContext.MethodsBySubjectId,
                buildContext.StubName,
                out var stubDefinition))
        {
            return RuntimeSkeletonFamilyHandlerResult.CreateMatch(stubDefinition);
        }

        return RuntimeSkeletonFamilyHandlerResult.NoMatch;
    }







    [RuntimeSkeletonFamilyPlugin("async-state-machine-no-op", CodeGenPluginProductLine.NativeReference, 32, RuntimeSkeletonFamilyPluginKind.ManagedInvoke)]
    private static RuntimeSkeletonFamilyHandlerResult TryBuildRuntimeSkeletonAsyncStateMachineNoOpHandler(
        RuntimeSkeletonStubBuildContext buildContext)
    {
        if (TryBuildAssemblyBoundAsyncStateMachineNoOpStub(
                buildContext.SubjectId,
                buildContext.MethodsBySubjectId,
                buildContext.StubName,
                out var stubDefinition))
        {
            return RuntimeSkeletonFamilyHandlerResult.CreateMatch(stubDefinition);
        }

        return RuntimeSkeletonFamilyHandlerResult.NoMatch;
    }







    [RuntimeSkeletonFamilyPlugin("array-boxing-reference-array", CodeGenPluginProductLine.NativeReference, 33, RuntimeSkeletonFamilyPluginKind.Kernel)]
    private static RuntimeSkeletonFamilyHandlerResult TryBuildRuntimeSkeletonArrayBoxingReferenceArrayHandler(
        RuntimeSkeletonStubBuildContext buildContext)
    {
        if (TryBuildAssemblyBoundArrayBoxingReferenceArrayPlan(
                buildContext.LoweringPlan.AssemblyName,
                buildContext.SubjectId,
                buildContext.MetadataRegistration,
                buildContext.MethodPointers,
                buildContext.Methods,
                out var loweringPlan))
        {
            return RuntimeSkeletonFamilyHandlerResult.CreateMatch(
                BuildAssemblyBoundArrayBoxingReferenceArrayStub(
                    loweringPlan,
                    buildContext.PageSupportBuilder,
                    buildContext.StubName));
        }

        return RuntimeSkeletonFamilyHandlerResult.NoMatch;
    }







    [RuntimeSkeletonFamilyPlugin("array-clear-reference-array", CodeGenPluginProductLine.NativeReference, 34, RuntimeSkeletonFamilyPluginKind.Kernel)]
    private static RuntimeSkeletonFamilyHandlerResult TryBuildRuntimeSkeletonArrayClearReferenceArrayHandler(
        RuntimeSkeletonStubBuildContext buildContext)
    {
        if (TryBuildAssemblyBoundArrayClearReferenceArrayPlan(
                buildContext.LoweringPlan.AssemblyName,
                buildContext.SubjectId,
                buildContext.MetadataRegistration,
                buildContext.MethodPointers,
                buildContext.Methods,
                out var loweringPlan))
        {
            return RuntimeSkeletonFamilyHandlerResult.CreateMatch(
                BuildAssemblyBoundArrayClearReferenceArrayStub(
                    loweringPlan,
                    buildContext.PageSupportBuilder,
                    buildContext.StubName));
        }

        return RuntimeSkeletonFamilyHandlerResult.NoMatch;
    }







    [RuntimeSkeletonFamilyPlugin("array-reverse-reference-array", CodeGenPluginProductLine.NativeReference, 35, RuntimeSkeletonFamilyPluginKind.Kernel)]
    private static RuntimeSkeletonFamilyHandlerResult TryBuildRuntimeSkeletonArrayReverseReferenceArrayHandler(
        RuntimeSkeletonStubBuildContext buildContext)
    {
        if (TryBuildAssemblyBoundArrayReverseReferenceArrayPlan(
                buildContext.LoweringPlan.AssemblyName,
                buildContext.SubjectId,
                buildContext.MetadataRegistration,
                buildContext.MethodPointers,
                buildContext.Methods,
                out var loweringPlan))
        {
            return RuntimeSkeletonFamilyHandlerResult.CreateMatch(
                BuildAssemblyBoundArrayReverseReferenceArrayStub(
                    loweringPlan,
                    buildContext.PageSupportBuilder,
                    buildContext.StubName));
        }

        return RuntimeSkeletonFamilyHandlerResult.NoMatch;
    }







    [RuntimeSkeletonFamilyPlugin("array-copy-reference-array", CodeGenPluginProductLine.NativeReference, 36, RuntimeSkeletonFamilyPluginKind.Kernel)]
    private static RuntimeSkeletonFamilyHandlerResult TryBuildRuntimeSkeletonArrayCopyReferenceArrayHandler(
        RuntimeSkeletonStubBuildContext buildContext)
    {
        if (TryBuildAssemblyBoundArrayCopyReferenceArrayPlan(
                buildContext.LoweringPlan.AssemblyName,
                buildContext.SubjectId,
                buildContext.MetadataRegistration,
                buildContext.MethodPointers,
                buildContext.Methods,
                out var loweringPlan))
        {
            return RuntimeSkeletonFamilyHandlerResult.CreateMatch(
                BuildAssemblyBoundArrayCopyReferenceArrayStub(
                    loweringPlan,
                    buildContext.PageSupportBuilder,
                    buildContext.StubName));
        }

        return RuntimeSkeletonFamilyHandlerResult.NoMatch;
    }







    [RuntimeSkeletonFamilyPlugin("static-exception-throw-literal", CodeGenPluginProductLine.NativeReference, 37, RuntimeSkeletonFamilyPluginKind.Kernel)]
    private static RuntimeSkeletonFamilyHandlerResult TryBuildRuntimeSkeletonStaticExceptionThrowLiteralHandler(
        RuntimeSkeletonStubBuildContext buildContext)
    {
        if (TryBuildAssemblyBoundStaticExceptionThrowLiteralStub(
                buildContext.LoweringPlan.AssemblyName,
                buildContext.SubjectId,
                buildContext.MetadataRegistration,
                buildContext.MethodsBySubjectId,
                buildContext.StubName,
                out var stubDefinition))
        {
            return RuntimeSkeletonFamilyHandlerResult.CreateMatch(stubDefinition);
        }

        return RuntimeSkeletonFamilyHandlerResult.NoMatch;
    }







    [RuntimeSkeletonFamilyPlugin("static-exception-throw-string-producer", CodeGenPluginProductLine.NativeReference, 38, RuntimeSkeletonFamilyPluginKind.Kernel)]
    private static RuntimeSkeletonFamilyHandlerResult TryBuildRuntimeSkeletonStaticExceptionThrowStringProducerHandler(
        RuntimeSkeletonStubBuildContext buildContext)
    {
        if (TryBuildAssemblyBoundStaticExceptionThrowStringProducerStub(
                buildContext.LoweringPlan.AssemblyName,
                buildContext.SubjectId,
                buildContext.MetadataRegistration,
                buildContext.MethodsBySubjectId,
                buildContext.MethodStubNamesBySubjectId,
                buildContext.StubName,
                out var stubDefinition))
        {
            return RuntimeSkeletonFamilyHandlerResult.CreateMatch(stubDefinition);
        }

        return RuntimeSkeletonFamilyHandlerResult.NoMatch;
    }







    [RuntimeSkeletonFamilyPlugin("static-exception-catch-string-return", CodeGenPluginProductLine.NativeReference, 39, RuntimeSkeletonFamilyPluginKind.Kernel)]
    private static RuntimeSkeletonFamilyHandlerResult TryBuildRuntimeSkeletonStaticExceptionCatchStringReturnHandler(
        RuntimeSkeletonStubBuildContext buildContext)
    {
        if (TryBuildAssemblyBoundStaticExceptionCatchStringReturnStub(
                buildContext.SubjectId,
                buildContext.MethodsBySubjectId,
                buildContext.MethodStubNamesBySubjectId,
                buildContext.StubName,
                out var stubDefinition))
        {
            return RuntimeSkeletonFamilyHandlerResult.CreateMatch(stubDefinition);
        }

        return RuntimeSkeletonFamilyHandlerResult.NoMatch;
    }







    [RuntimeSkeletonFamilyPlugin("exception-throw-catch-finally", CodeGenPluginProductLine.NativeReference, 40, RuntimeSkeletonFamilyPluginKind.Kernel)]
    private static RuntimeSkeletonFamilyHandlerResult TryBuildRuntimeSkeletonExceptionThrowCatchFinallyHandler(
        RuntimeSkeletonStubBuildContext buildContext)
    {
        if (TryBuildAssemblyBoundExceptionThrowCatchFinallyPlan(
                buildContext.LoweringPlan.AssemblyName,
                buildContext.SubjectId,
                buildContext.MetadataRegistration,
                buildContext.MethodPointers,
                out var loweringPlan))
        {
            return RuntimeSkeletonFamilyHandlerResult.CreateMatch(
                BuildAssemblyBoundExceptionThrowCatchFinallyStub(loweringPlan, buildContext.StubName));
        }

        return RuntimeSkeletonFamilyHandlerResult.NoMatch;
    }







    [RuntimeSkeletonFamilyPlugin("nested-exception-throw-catch-finally", CodeGenPluginProductLine.NativeReference, 41, RuntimeSkeletonFamilyPluginKind.Kernel)]
    private static RuntimeSkeletonFamilyHandlerResult TryBuildRuntimeSkeletonNestedExceptionThrowCatchFinallyHandler(
        RuntimeSkeletonStubBuildContext buildContext)
    {
        if (TryBuildAssemblyBoundNestedExceptionThrowCatchFinallyPlan(
                buildContext.LoweringPlan.AssemblyName,
                buildContext.SubjectId,
                buildContext.MetadataRegistration,
                buildContext.MethodPointers,
                out var loweringPlan))
        {
            return RuntimeSkeletonFamilyHandlerResult.CreateMatch(
                BuildAssemblyBoundNestedExceptionThrowCatchFinallyStub(loweringPlan, buildContext.StubName));
        }

        return RuntimeSkeletonFamilyHandlerResult.NoMatch;
    }







    [RuntimeSkeletonFamilyPlugin("static-string-return-forwarder", CodeGenPluginProductLine.NativeReference, 42, RuntimeSkeletonFamilyPluginKind.ManagedInvoke)]
    private static RuntimeSkeletonFamilyHandlerResult TryBuildRuntimeSkeletonStaticStringReturnForwarderHandler(
        RuntimeSkeletonStubBuildContext buildContext) =>
        TryBuildRuntimeSkeletonStaticStringReturnForwarderCore(buildContext);







    [RuntimeSkeletonFamilyPlugin("static-string-forwarder", CodeGenPluginProductLine.NativeReference, 43, RuntimeSkeletonFamilyPluginKind.ManagedInvoke)]
    private static RuntimeSkeletonFamilyHandlerResult TryBuildRuntimeSkeletonStaticStringForwarderHandler(
        RuntimeSkeletonStubBuildContext buildContext) =>
        TryBuildRuntimeSkeletonStaticStringForwarderCore(buildContext);







    [RuntimeSkeletonFamilyPlugin("static-string-literal-append", CodeGenPluginProductLine.NativeReference, 44, RuntimeSkeletonFamilyPluginKind.ManagedInvoke)]
    private static RuntimeSkeletonFamilyHandlerResult TryBuildRuntimeSkeletonStaticStringLiteralAppendHandler(
        RuntimeSkeletonStubBuildContext buildContext) =>
        TryBuildRuntimeSkeletonStaticStringLiteralAppendCore(buildContext);







    [RuntimeSkeletonFamilyPlugin("static-string-forwarder-console-write-line", CodeGenPluginProductLine.NativeReference, 45, RuntimeSkeletonFamilyPluginKind.ManagedInvoke)]
    private static RuntimeSkeletonFamilyHandlerResult TryBuildRuntimeSkeletonStaticStringForwarderConsoleWriteLineHandler(
        RuntimeSkeletonStubBuildContext buildContext) =>
        TryBuildRuntimeSkeletonStaticStringForwarderConsoleWriteLineCore(buildContext);







    [RuntimeSkeletonFamilyPlugin("static-string-producer-forwarder-console-write-line", CodeGenPluginProductLine.NativeReference, 46, RuntimeSkeletonFamilyPluginKind.ManagedInvoke)]
    private static RuntimeSkeletonFamilyHandlerResult TryBuildRuntimeSkeletonStaticStringProducerForwarderConsoleWriteLineHandler(
        RuntimeSkeletonStubBuildContext buildContext) =>
        TryBuildRuntimeSkeletonStaticStringProducerForwarderConsoleWriteLineCore(buildContext);







    [RuntimeSkeletonFamilyPlugin("static-string-producer-ctor-getter-console-write-line", CodeGenPluginProductLine.NativeReference, 47, RuntimeSkeletonFamilyPluginKind.ManagedInvoke)]
    private static RuntimeSkeletonFamilyHandlerResult TryBuildRuntimeSkeletonStaticStringProducerCtorGetterConsoleWriteLineHandler(
        RuntimeSkeletonStubBuildContext buildContext) =>
        TryBuildRuntimeSkeletonStaticStringProducerCtorGetterConsoleWriteLineCore(buildContext);







    [RuntimeSkeletonFamilyPlugin("static-string-producer-ctor-render-console-write-line", CodeGenPluginProductLine.NativeReference, 48, RuntimeSkeletonFamilyPluginKind.ManagedInvoke)]
    private static RuntimeSkeletonFamilyHandlerResult TryBuildRuntimeSkeletonStaticStringProducerCtorRenderConsoleWriteLineHandler(
        RuntimeSkeletonStubBuildContext buildContext) =>
        TryBuildRuntimeSkeletonStaticStringProducerCtorRenderConsoleWriteLineCore(buildContext);







    [RuntimeSkeletonFamilyPlugin("static-string-producer-ctor-instance-call-forwarder-console-write-line", CodeGenPluginProductLine.NativeReference, 49, RuntimeSkeletonFamilyPluginKind.ManagedInvoke)]
    private static RuntimeSkeletonFamilyHandlerResult TryBuildRuntimeSkeletonStaticStringProducerCtorInstanceCallForwarderConsoleWriteLineHandler(
        RuntimeSkeletonStubBuildContext buildContext) =>
        TryBuildRuntimeSkeletonStaticStringProducerCtorInstanceCallForwarderConsoleWriteLineCore(buildContext);







    [RuntimeSkeletonFamilyPlugin("static-string-producer-forwarder-ctor-instance-call-console-write-line", CodeGenPluginProductLine.NativeReference, 50, RuntimeSkeletonFamilyPluginKind.ManagedInvoke)]
    private static RuntimeSkeletonFamilyHandlerResult TryBuildRuntimeSkeletonStaticStringProducerForwarderCtorInstanceCallConsoleWriteLineHandler(
        RuntimeSkeletonStubBuildContext buildContext) =>
        TryBuildRuntimeSkeletonStaticStringProducerForwarderCtorInstanceCallConsoleWriteLineCore(buildContext);







    [RuntimeSkeletonFamilyPlugin("static-string-producer-forwarder-ctor-instance-call-forwarder-console-write-line", CodeGenPluginProductLine.NativeReference, 51, RuntimeSkeletonFamilyPluginKind.ManagedInvoke)]
    private static RuntimeSkeletonFamilyHandlerResult TryBuildRuntimeSkeletonStaticStringProducerForwarderCtorInstanceCallForwarderConsoleWriteLineHandler(
        RuntimeSkeletonStubBuildContext buildContext) =>
        TryBuildRuntimeSkeletonStaticStringProducerForwarderCtorInstanceCallForwarderConsoleWriteLineCore(buildContext);







    [RuntimeSkeletonFamilyPlugin("static-string-producer-ctor-instance-call-forwarder-forwarder-console-write-line", CodeGenPluginProductLine.NativeReference, 52, RuntimeSkeletonFamilyPluginKind.ManagedInvoke)]
    private static RuntimeSkeletonFamilyHandlerResult TryBuildRuntimeSkeletonStaticStringProducerCtorInstanceCallForwarderForwarderConsoleWriteLineHandler(
        RuntimeSkeletonStubBuildContext buildContext) =>
        TryBuildRuntimeSkeletonStaticStringProducerCtorInstanceCallForwarderForwarderConsoleWriteLineCore(buildContext);







    [RuntimeSkeletonFamilyPlugin("static-string-producer-forwarder-ctor-instance-call-forwarder-forwarder-console-write-line", CodeGenPluginProductLine.NativeReference, 53, RuntimeSkeletonFamilyPluginKind.ManagedInvoke)]
    private static RuntimeSkeletonFamilyHandlerResult TryBuildRuntimeSkeletonStaticStringProducerForwarderCtorInstanceCallForwarderForwarderConsoleWriteLineHandler(
        RuntimeSkeletonStubBuildContext buildContext) =>
        TryBuildRuntimeSkeletonStaticStringProducerForwarderCtorInstanceCallForwarderForwarderConsoleWriteLineCore(buildContext);







    [RuntimeSkeletonFamilyPlugin("static-string-producer-ctor-instance-call-forwarder-forwarder-forwarder-console-write-line", CodeGenPluginProductLine.NativeReference, 54, RuntimeSkeletonFamilyPluginKind.ManagedInvoke)]
    private static RuntimeSkeletonFamilyHandlerResult TryBuildRuntimeSkeletonStaticStringProducerCtorInstanceCallForwarderForwarderForwarderConsoleWriteLineHandler(
        RuntimeSkeletonStubBuildContext buildContext) =>
        TryBuildRuntimeSkeletonStaticStringProducerCtorInstanceCallForwarderForwarderForwarderConsoleWriteLineCore(buildContext);







    [RuntimeSkeletonFamilyPlugin("static-string-producer-forwarder-ctor-instance-call-forwarder-forwarder-forwarder-console-write-line", CodeGenPluginProductLine.NativeReference, 55, RuntimeSkeletonFamilyPluginKind.ManagedInvoke)]
    private static RuntimeSkeletonFamilyHandlerResult TryBuildRuntimeSkeletonStaticStringProducerForwarderCtorInstanceCallForwarderForwarderForwarderConsoleWriteLineHandler(
        RuntimeSkeletonStubBuildContext buildContext) =>
        TryBuildRuntimeSkeletonStaticStringProducerForwarderCtorInstanceCallForwarderForwarderForwarderConsoleWriteLineCore(buildContext);







    [RuntimeSkeletonFamilyPlugin("static-literal-string-return", CodeGenPluginProductLine.NativeReference, 56, RuntimeSkeletonFamilyPluginKind.ManagedInvoke)]
    private static RuntimeSkeletonFamilyHandlerResult TryBuildRuntimeSkeletonStaticLiteralStringReturnHandler(
        RuntimeSkeletonStubBuildContext buildContext) =>
        TryBuildRuntimeSkeletonStaticLiteralStringReturnCore(buildContext);







    [RuntimeSkeletonFamilyPlugin("static-resource-key-string-return", CodeGenPluginProductLine.NativeReference, 57, RuntimeSkeletonFamilyPluginKind.ManagedInvoke)]
    private static RuntimeSkeletonFamilyHandlerResult TryBuildRuntimeSkeletonStaticResourceKeyStringReturnHandler(
        RuntimeSkeletonStubBuildContext buildContext) =>
        TryBuildRuntimeSkeletonStaticResourceKeyStringReturnCore(buildContext);







    [RuntimeSkeletonFamilyPlugin("static-string-producer-console-write-line", CodeGenPluginProductLine.NativeReference, 58, RuntimeSkeletonFamilyPluginKind.ManagedInvoke)]
    private static RuntimeSkeletonFamilyHandlerResult TryBuildRuntimeSkeletonStaticStringProducerConsoleWriteLineHandler(
        RuntimeSkeletonStubBuildContext buildContext) =>
        TryBuildRuntimeSkeletonStaticStringProducerConsoleWriteLineCore(buildContext);







    [RuntimeSkeletonFamilyPlugin("static-call-ctor-getter-executable", CodeGenPluginProductLine.NativeReference, 59, RuntimeSkeletonFamilyPluginKind.ManagedInvoke)]
    private static RuntimeSkeletonFamilyHandlerResult TryBuildRuntimeSkeletonStaticCallCtorGetterExecutableHandler(
        RuntimeSkeletonStubBuildContext buildContext) =>
        TryBuildRuntimeSkeletonStaticCallCtorGetterExecutableCore(buildContext);







    [RuntimeSkeletonFamilyPlugin("constructor-then-instance-call-executable", CodeGenPluginProductLine.NativeReference, 60, RuntimeSkeletonFamilyPluginKind.ManagedInvoke)]
    private static RuntimeSkeletonFamilyHandlerResult TryBuildRuntimeSkeletonConstructorThenInstanceCallExecutableHandler(
        RuntimeSkeletonStubBuildContext buildContext) =>
        TryBuildRuntimeSkeletonConstructorThenInstanceCallExecutableCore(buildContext);







    private static string BuildAssemblyBoundStaticCallCtorGetterStub(
        NativeReferenceLoweringPlanArtifact loweringPlan,
        RuntimeSkeletonPageSupportBuilder pageSupportBuilder,
        string stubName)
    {
        ValidateManagedLoweringPlan(loweringPlan);
        var echoLiteral = loweringPlan.EchoLiteral ?? throw new InvalidOperationException("runtime skeleton staticCallCtorGetter requires echo literal");
        var fieldBindingIndex = pageSupportBuilder.GetOrAddRuntimeSkeletonFieldBindingDescriptor(
            ToCppStringLiteral(loweringPlan.AssemblyName),
            loweringPlan.ReferenceTypeToken,
            loweringPlan.CapturedFieldToken);
        var descriptorIndex = pageSupportBuilder.GetOrAddRuntimeSkeletonStaticCallCtorGetterDescriptor(
            fieldBindingIndex,
            ToCppStringLiteral(ConsoleWriteLineStringIcall),
            echoLiteral,
            loweringPlan.EchoLiteralByteCount ?? 0);
        var model = new ScriptObject
        {
            ["stub_name"] = stubName,
            ["descriptor_index"] = descriptorIndex,
        };
        return ScribanTemplateRenderer.RenderTemplate(
            ScribanTemplateRenderer.LoadTemplate(NativeReferenceProofCatalog.RuntimeSkeletonStaticCallCtorGetterStubTemplateRelativePath),
            model);
    }







    private static string BuildAssemblyBoundConstructorThenInstanceCallStub(
        NativeReferenceLoweringPlanArtifact loweringPlan,
        RuntimeSkeletonPageSupportBuilder pageSupportBuilder,
        string stubName)
    {
        ValidateManagedLoweringPlan(loweringPlan);
        var concatIcall = NormalizeStringConcatIcall(loweringPlan.StringConcatPairIcall);
        var constructorLiteral = loweringPlan.ConstructorLiteral ?? throw new InvalidOperationException("runtime skeleton constructorThenInstanceCall requires constructor literal");
        var messagePrefixLiteral = loweringPlan.MessagePrefixLiteral ?? throw new InvalidOperationException("runtime skeleton constructorThenInstanceCall requires message prefix literal");
        var messageSuffixLiteral = loweringPlan.MessageSuffixLiteral ?? throw new InvalidOperationException("runtime skeleton constructorThenInstanceCall requires message suffix literal");
        var fieldBindingIndex = pageSupportBuilder.GetOrAddRuntimeSkeletonFieldBindingDescriptor(
            ToCppStringLiteral(loweringPlan.AssemblyName),
            loweringPlan.ReferenceTypeToken,
            loweringPlan.CapturedFieldToken);
        var descriptorIndex = pageSupportBuilder.GetOrAddRuntimeSkeletonConstructorThenInstanceCallDescriptor(
            fieldBindingIndex,
            ToCppStringLiteral(concatIcall),
            ToCppStringLiteral(StringConcatTripleIcall),
            ToCppStringLiteral(ConsoleWriteLineStringIcall),
            constructorLiteral,
            loweringPlan.ConstructorLiteralByteCount ?? 0,
            messagePrefixLiteral,
            loweringPlan.MessagePrefixLiteralByteCount ?? 0,
            messageSuffixLiteral,
            loweringPlan.MessageSuffixLiteralByteCount ?? 0);
        var model = new ScriptObject
        {
            ["stub_name"] = stubName,
            ["descriptor_index"] = descriptorIndex,
        };
        return ScribanTemplateRenderer.RenderTemplate(
            ScribanTemplateRenderer.LoadTemplate(NativeReferenceProofCatalog.RuntimeSkeletonConstructorThenInstanceCallStubTemplateRelativePath),
            model);
    }







    private static string BuildAssemblyBoundDelegateClosedTargetRelayStub(
        NativeReferenceLoweringPlanArtifact loweringPlan,
        RuntimeSkeletonPageSupportBuilder pageSupportBuilder,
        string stubName)
    {
        ValidateManagedLoweringPlan(loweringPlan);
        var concatIcall = NormalizeStringConcatIcall(loweringPlan.StringConcatPairIcall);
        var constructorLiteral = loweringPlan.ConstructorLiteral ?? throw new InvalidOperationException("runtime skeleton delegateClosedTargetRelay requires constructor literal");
        var messagePrefixLiteral = loweringPlan.MessagePrefixLiteral ?? throw new InvalidOperationException("runtime skeleton delegateClosedTargetRelay requires message prefix literal");
        var messageSuffixLiteral = loweringPlan.MessageSuffixLiteral ?? throw new InvalidOperationException("runtime skeleton delegateClosedTargetRelay requires message suffix literal");
        var fieldBindingIndex = pageSupportBuilder.GetOrAddRuntimeSkeletonFieldBindingDescriptor(
            ToCppStringLiteral(loweringPlan.AssemblyName),
            loweringPlan.ReferenceTypeToken,
            loweringPlan.CapturedFieldToken);
        var descriptorIndex = pageSupportBuilder.GetOrAddRuntimeSkeletonDelegateClosedTargetRelayDescriptor(
            fieldBindingIndex,
            ToCppStringLiteral(concatIcall),
            ToCppStringLiteral(ConsoleWriteLineStringIcall),
            constructorLiteral,
            loweringPlan.ConstructorLiteralByteCount ?? 0,
            messagePrefixLiteral,
            loweringPlan.MessagePrefixLiteralByteCount ?? 0,
            messageSuffixLiteral,
            loweringPlan.MessageSuffixLiteralByteCount ?? 0,
            loweringPlan.TrailingLiteral ?? throw new InvalidOperationException("delegate closed target relay requires trailing literal"),
            loweringPlan.TrailingLiteralByteCount ?? 0);
        var model = new ScriptObject
        {
            ["stub_name"] = stubName,
            ["descriptor_index"] = descriptorIndex,
        };
        return ScribanTemplateRenderer.RenderTemplate(
            ScribanTemplateRenderer.LoadTemplate(NativeReferenceProofCatalog.RuntimeSkeletonDelegateClosedTargetRelayStubTemplateRelativePath),
            model);
    }







    private static string BuildAssemblyBoundArrayBoxingReferenceArrayStub(
        NativeReferenceLoweringPlanArtifact loweringPlan,
        RuntimeSkeletonPageSupportBuilder pageSupportBuilder,
        string stubName)
    {
        ValidateManagedLoweringPlan(loweringPlan);
        var concatIcall = NormalizeStringConcatIcall(loweringPlan.StringConcatPairIcall);
        var boxedValueTypeToken = loweringPlan.BoxedValueTypeToken ?? throw new InvalidOperationException("runtime skeleton arrayBoxingReferenceArray requires boxed value type token");
        var constructorLiteral = loweringPlan.ConstructorLiteral ?? throw new InvalidOperationException("runtime skeleton arrayBoxingReferenceArray requires constructor literal");
        var messagePrefixLiteral = loweringPlan.MessagePrefixLiteral ?? throw new InvalidOperationException("runtime skeleton arrayBoxingReferenceArray requires message prefix literal");
        var messageSuffixLiteral = loweringPlan.MessageSuffixLiteral ?? throw new InvalidOperationException("runtime skeleton arrayBoxingReferenceArray requires message suffix literal");
        var fieldBindingIndex = pageSupportBuilder.GetOrAddRuntimeSkeletonFieldBindingDescriptor(
            ToCppStringLiteral(loweringPlan.AssemblyName),
            loweringPlan.ReferenceTypeToken,
            loweringPlan.CapturedFieldToken);
        var boxedValueTypeIndex = pageSupportBuilder.GetOrAddRuntimeSkeletonTypeDescriptor(
            ToCppStringLiteral(loweringPlan.AssemblyName),
            boxedValueTypeToken);
        var descriptorIndex = pageSupportBuilder.GetOrAddRuntimeSkeletonArrayBoxingReferenceArrayDescriptor(
            fieldBindingIndex,
            boxedValueTypeIndex,
            ToCppStringLiteral(concatIcall),
            ToCppStringLiteral(StringConcatTripleIcall),
            ToCppStringLiteral(ConsoleWriteLineStringIcall),
            loweringPlan.BoxedInt32Value ?? 0,
            constructorLiteral,
            loweringPlan.ConstructorLiteralByteCount ?? 0,
            messagePrefixLiteral,
            loweringPlan.MessagePrefixLiteralByteCount ?? 0,
            messageSuffixLiteral,
            loweringPlan.MessageSuffixLiteralByteCount ?? 0);
        var model = new ScriptObject
        {
            ["stub_name"] = stubName,
            ["descriptor_index"] = descriptorIndex,
        };
        return ScribanTemplateRenderer.RenderTemplate(
            ScribanTemplateRenderer.LoadTemplate(NativeReferenceProofCatalog.RuntimeSkeletonArrayBoxingReferenceArrayStubTemplateRelativePath),
            model);
    }







    private static string BuildAssemblyBoundArrayCopyReferenceArrayStub(
        NativeReferenceLoweringPlanArtifact loweringPlan,
        RuntimeSkeletonPageSupportBuilder pageSupportBuilder,
        string stubName)
    {
        ValidateManagedLoweringPlan(loweringPlan);
        var concatIcall = NormalizeStringConcatIcall(loweringPlan.StringConcatPairIcall);
        var constructorLiteral = loweringPlan.ConstructorLiteral ?? throw new InvalidOperationException("runtime skeleton arrayCopyReferenceArray requires constructor literal");
        var messagePrefixLiteral = loweringPlan.MessagePrefixLiteral ?? throw new InvalidOperationException("runtime skeleton arrayCopyReferenceArray requires message prefix literal");
        var messageSuffixLiteral = loweringPlan.MessageSuffixLiteral ?? throw new InvalidOperationException("runtime skeleton arrayCopyReferenceArray requires message suffix literal");
        var fieldBindingIndex = pageSupportBuilder.GetOrAddRuntimeSkeletonFieldBindingDescriptor(
            ToCppStringLiteral(loweringPlan.AssemblyName),
            loweringPlan.ReferenceTypeToken,
            loweringPlan.CapturedFieldToken);
        var descriptorIndex = pageSupportBuilder.GetOrAddRuntimeSkeletonArrayCopyReferenceArrayDescriptor(
            fieldBindingIndex,
            ToCppStringLiteral(concatIcall),
            ToCppStringLiteral(StringConcatTripleIcall),
            ToCppStringLiteral(ConsoleWriteLineStringIcall),
            loweringPlan.SourceArrayLength ?? 0,
            loweringPlan.TargetArrayLength ?? 0,
            loweringPlan.SourceStoreIndex ?? 0,
            loweringPlan.SourceArrayIndex ?? 0,
            loweringPlan.TargetArrayIndex ?? 0,
            loweringPlan.TargetReadIndex ?? 0,
            loweringPlan.CopyLength ?? 0,
            constructorLiteral,
            loweringPlan.ConstructorLiteralByteCount ?? 0,
            messagePrefixLiteral,
            loweringPlan.MessagePrefixLiteralByteCount ?? 0,
            messageSuffixLiteral,
            loweringPlan.MessageSuffixLiteralByteCount ?? 0);
        var model = new ScriptObject
        {
            ["stub_name"] = stubName,
            ["descriptor_index"] = descriptorIndex,
        };
        return ScribanTemplateRenderer.RenderTemplate(
            ScribanTemplateRenderer.LoadTemplate(NativeReferenceProofCatalog.RuntimeSkeletonArrayCopyReferenceArrayStubTemplateRelativePath),
            model);
    }







    private static string BuildAssemblyBoundArrayClearReferenceArrayStub(
        NativeReferenceLoweringPlanArtifact loweringPlan,
        RuntimeSkeletonPageSupportBuilder pageSupportBuilder,
        string stubName)
    {
        ValidateManagedLoweringPlan(loweringPlan);
        var concatIcall = NormalizeStringConcatIcall(loweringPlan.StringConcatPairIcall);
        var storedLiteral = loweringPlan.StoredLiteral ?? throw new InvalidOperationException("runtime skeleton arrayClearReferenceArray requires stored literal");
        var messagePrefixLiteral = loweringPlan.MessagePrefixLiteral ?? throw new InvalidOperationException("runtime skeleton arrayClearReferenceArray requires message prefix literal");
        var stringTypeIndex = pageSupportBuilder.GetOrAddRuntimeSkeletonTypeDescriptor(
            ToCppStringLiteral(loweringPlan.AssemblyName),
            loweringPlan.ReferenceTypeToken);
        var descriptorIndex = pageSupportBuilder.GetOrAddRuntimeSkeletonArrayClearReferenceArrayDescriptor(
            stringTypeIndex,
            ToCppStringLiteral(concatIcall),
            ToCppStringLiteral(ConsoleWriteLineStringIcall),
            loweringPlan.ArrayLength ?? 0,
            loweringPlan.ArrayStoreIndex ?? 0,
            loweringPlan.ClearStartIndex ?? 0,
            loweringPlan.ClearLength ?? 0,
            loweringPlan.ArrayReadIndex ?? 0,
            storedLiteral,
            loweringPlan.StoredLiteralByteCount ?? 0,
            messagePrefixLiteral,
            loweringPlan.MessagePrefixLiteralByteCount ?? 0);
        var model = new ScriptObject
        {
            ["stub_name"] = stubName,
            ["descriptor_index"] = descriptorIndex,
        };
        return ScribanTemplateRenderer.RenderTemplate(
            ScribanTemplateRenderer.LoadTemplate(NativeReferenceProofCatalog.RuntimeSkeletonArrayClearReferenceArrayStubTemplateRelativePath),
            model);
    }







    private static string BuildAssemblyBoundArrayReverseReferenceArrayStub(
        NativeReferenceLoweringPlanArtifact loweringPlan,
        RuntimeSkeletonPageSupportBuilder pageSupportBuilder,
        string stubName)
    {
        ValidateManagedLoweringPlan(loweringPlan);
        var concatIcall = NormalizeStringConcatIcall(loweringPlan.StringConcatPairIcall);
        var constructorLiteral = loweringPlan.ConstructorLiteral ?? throw new InvalidOperationException("runtime skeleton arrayReverseReferenceArray requires constructor literal");
        var messagePrefixLiteral = loweringPlan.MessagePrefixLiteral ?? throw new InvalidOperationException("runtime skeleton arrayReverseReferenceArray requires message prefix literal");
        var messageSuffixLiteral = loweringPlan.MessageSuffixLiteral ?? throw new InvalidOperationException("runtime skeleton arrayReverseReferenceArray requires message suffix literal");
        var fieldBindingIndex = pageSupportBuilder.GetOrAddRuntimeSkeletonFieldBindingDescriptor(
            ToCppStringLiteral(loweringPlan.AssemblyName),
            loweringPlan.ReferenceTypeToken,
            loweringPlan.CapturedFieldToken);
        var descriptorIndex = pageSupportBuilder.GetOrAddRuntimeSkeletonArrayReverseReferenceArrayDescriptor(
            fieldBindingIndex,
            ToCppStringLiteral(concatIcall),
            ToCppStringLiteral(StringConcatTripleIcall),
            ToCppStringLiteral(ConsoleWriteLineStringIcall),
            loweringPlan.ArrayLength ?? 0,
            loweringPlan.ArrayStoreIndex ?? 0,
            loweringPlan.ReverseStartIndex ?? 0,
            loweringPlan.ReverseLength ?? 0,
            loweringPlan.ArrayReadIndex ?? 0,
            constructorLiteral,
            loweringPlan.ConstructorLiteralByteCount ?? 0,
            messagePrefixLiteral,
            loweringPlan.MessagePrefixLiteralByteCount ?? 0,
            messageSuffixLiteral,
            loweringPlan.MessageSuffixLiteralByteCount ?? 0);
        var model = new ScriptObject
        {
            ["stub_name"] = stubName,
            ["descriptor_index"] = descriptorIndex,
        };
        return ScribanTemplateRenderer.RenderTemplate(
            ScribanTemplateRenderer.LoadTemplate(NativeReferenceProofCatalog.RuntimeSkeletonArrayReverseReferenceArrayStubTemplateRelativePath),
            model);
    }







    private static string BuildAssemblyBoundInterfaceDispatchMessageStub(
        NativeReferenceLoweringPlanArtifact loweringPlan,
        string stubName)
    {
        ValidateManagedLoweringPlan(loweringPlan);
        var concatIcall = NormalizeStringConcatIcall(loweringPlan.StringConcatPairIcall);
        var model = new ScriptObject
        {
            ["stub_name"] = stubName,
            ["assembly_name_literal"] = ToCppStringLiteral(loweringPlan.AssemblyName),
            ["reference_type_token"] = loweringPlan.ReferenceTypeToken,
            ["captured_field_token"] = loweringPlan.CapturedFieldToken,
            ["instance_method_token"] = loweringPlan.InstanceMethodToken,
            ["concat_icall_literal"] = ToCppStringLiteral(concatIcall),
            ["console_write_line_string_icall_literal"] = ToCppStringLiteral(ConsoleWriteLineStringIcall),
            ["constructor_literal"] = loweringPlan.ConstructorLiteral,
            ["constructor_literal_byte_count"] = loweringPlan.ConstructorLiteralByteCount ?? 0,
            ["message_prefix_literal"] = loweringPlan.MessagePrefixLiteral,
            ["message_prefix_literal_byte_count"] = loweringPlan.MessagePrefixLiteralByteCount ?? 0,
            ["message_suffix_literal"] = loweringPlan.MessageSuffixLiteral,
            ["message_suffix_literal_byte_count"] = loweringPlan.MessageSuffixLiteralByteCount ?? 0,
        };
        return ScribanTemplateRenderer.RenderTemplate(
            ScribanTemplateRenderer.LoadTemplate(NativeReferenceProofCatalog.RuntimeSkeletonInterfaceDispatchMessageStubTemplateRelativePath),
            model);
    }







    private static string BuildAssemblyBoundThreadingThreadStaticMonitorStub(
        NativeReferenceLoweringPlanArtifact loweringPlan,
        string stubName)
    {
        ValidateManagedLoweringPlan(loweringPlan);
        var model = new ScriptObject
        {
            ["stub_name"] = stubName,
            ["console_write_line_string_icall_literal"] = ToCppStringLiteral(loweringPlan.ConsoleWriteLineStringIcall ?? ConsoleWriteLineStringIcall),
            ["expected_output"] = ToCppStringLiteral(loweringPlan.ExpectedOutput ?? string.Empty),
            ["expected_output_byte_count"] = loweringPlan.ExpectedOutputByteCount ?? 0,
        };
        return ScribanTemplateRenderer.RenderTemplate(
            ScribanTemplateRenderer.LoadTemplate(NativeReferenceProofCatalog.RuntimeSkeletonThreadingThreadStaticMonitorStubTemplateRelativePath),
            model);
    }







    private static string BuildAssemblyBoundExceptionThrowCatchFinallyStub(
        NativeReferenceLoweringPlanArtifact loweringPlan,
        string stubName)
    {
        ValidateManagedLoweringPlan(loweringPlan);
        var model = new ScriptObject
        {
            ["stub_name"] = stubName,
            ["assembly_name_literal"] = ToCppStringLiteral(loweringPlan.AssemblyName),
            ["reference_type_token"] = loweringPlan.ReferenceTypeToken,
            ["console_write_line_string_icall_literal"] = ToCppStringLiteral(ConsoleWriteLineStringIcall),
            ["expected_output"] = loweringPlan.ExpectedOutput,
            ["expected_output_byte_count"] = loweringPlan.ExpectedOutputByteCount ?? 0,
            ["finally_literal"] = loweringPlan.FinallyLiteral,
            ["finally_literal_byte_count"] = loweringPlan.FinallyLiteralByteCount ?? 0,
        };
        return ScribanTemplateRenderer.RenderTemplate(
            ScribanTemplateRenderer.LoadTemplate(NativeReferenceProofCatalog.RuntimeSkeletonExceptionThrowCatchFinallyStubTemplateRelativePath),
            model);
    }







    private static string BuildAssemblyBoundNestedExceptionThrowCatchFinallyStub(
        NativeReferenceLoweringPlanArtifact loweringPlan,
        string stubName)
    {
        ValidateManagedLoweringPlan(loweringPlan);
        var model = new ScriptObject
        {
            ["stub_name"] = stubName,
            ["assembly_name_literal"] = ToCppStringLiteral(loweringPlan.AssemblyName),
            ["reference_type_token"] = loweringPlan.ReferenceTypeToken,
            ["console_write_line_string_icall_literal"] = ToCppStringLiteral(ConsoleWriteLineStringIcall),
            ["expected_output"] = loweringPlan.ExpectedOutput,
            ["expected_output_byte_count"] = loweringPlan.ExpectedOutputByteCount ?? 0,
            ["finally_literal"] = loweringPlan.FinallyLiteral,
            ["finally_literal_byte_count"] = loweringPlan.FinallyLiteralByteCount ?? 0,
        };
        return ScribanTemplateRenderer.RenderTemplate(
            ScribanTemplateRenderer.LoadTemplate(NativeReferenceProofCatalog.RuntimeSkeletonNestedExceptionThrowCatchFinallyStubTemplateRelativePath),
            model);
    }







    private static string BuildAssemblyBoundMarshalingUtf8ExportStub(
        NativeReferenceLoweringPlanArtifact loweringPlan,
        string stubName)
    {
        ValidateManagedLoweringPlan(loweringPlan);
        var model = new ScriptObject
        {
            ["stub_name"] = stubName,
            ["console_write_line_string_icall_literal"] = ToCppStringLiteral(ConsoleWriteLineStringIcall),
            ["expected_output"] = loweringPlan.ExpectedOutput,
            ["expected_output_byte_count"] = loweringPlan.ExpectedOutputByteCount ?? 0,
        };
        return ScribanTemplateRenderer.RenderTemplate(
            ScribanTemplateRenderer.LoadTemplate(NativeReferenceProofCatalog.RuntimeSkeletonMarshalingUtf8ExportStubTemplateRelativePath),
            model);
    }







    private static string BuildAssemblyBoundReflectionInteropClosureStub(
        NativeReferenceLoweringPlanArtifact loweringPlan,
        string stubName)
    {
        ValidateManagedLoweringPlan(loweringPlan);
        var model = new ScriptObject
        {
            ["stub_name"] = stubName,
            ["parameter_subject_id_literal"] = ToCppStringLiteral(loweringPlan.ParameterSubjectId ?? string.Empty),
            ["parameter_index"] = loweringPlan.ParameterIndex ?? 0,
            ["parameter_attributes"] = loweringPlan.ParameterAttributes ?? 0,
            ["captured_field_token"] = loweringPlan.CapturedFieldToken,
            ["field_subject_id_literal"] = ToCppStringLiteral(loweringPlan.FieldSubjectId ?? string.Empty),
            ["field_query_name_literal"] = ToCppStringLiteral(loweringPlan.FieldQueryName ?? string.Empty),
            ["instance_method_token"] = loweringPlan.InstanceMethodToken,
            ["method_subject_id_literal"] = ToCppStringLiteral(loweringPlan.MethodSubjectId ?? string.Empty),
            ["method_query_name_literal"] = ToCppStringLiteral(loweringPlan.MethodQueryName ?? string.Empty),
            ["method_parameter_count"] = loweringPlan.MethodParameterCount ?? 0,
            ["generic_type_definition_token"] = loweringPlan.GenericTypeDefinitionToken,
            ["generic_type_definition_subject_id_literal"] = ToCppStringLiteral(loweringPlan.GenericTypeDefinitionSubjectId ?? string.Empty),
            ["closed_type_namespace_name_literal"] = ToCppStringLiteral(loweringPlan.ClosedTypeNamespaceName ?? string.Empty),
            ["generic_type_definition_name_literal"] = ToCppStringLiteral(loweringPlan.GenericTypeDefinitionName ?? string.Empty),
            ["closed_type_token"] = loweringPlan.ClosedTypeToken,
            ["closed_type_subject_id_literal"] = ToCppStringLiteral(loweringPlan.ClosedTypeSubjectId ?? string.Empty),
            ["closed_type_name_literal"] = ToCppStringLiteral(loweringPlan.ClosedTypeName ?? string.Empty),
            ["closed_type_display_name_literal"] = ToCppStringLiteral(loweringPlan.ClosedTypeDisplayName ?? string.Empty),
            ["assembly_name_literal"] = ToCppStringLiteral(loweringPlan.AssemblyName),
            ["import_module_name_literal"] = ToCppStringLiteral(loweringPlan.ImportModuleName ?? string.Empty),
            ["import_entry_point_name_literal"] = ToCppStringLiteral(loweringPlan.ImportEntryPointName ?? string.Empty),
            ["console_write_line_string_icall_literal"] = ToCppStringLiteral(ConsoleWriteLineStringIcall),
            ["expected_output"] = loweringPlan.ExpectedOutput,
            ["expected_output_byte_count"] = loweringPlan.ExpectedOutputByteCount ?? 0,
        };
        return ScribanTemplateRenderer.RenderTemplate(
            ScribanTemplateRenderer.LoadTemplate(NativeReferenceProofCatalog.RuntimeSkeletonReflectionInteropClosureStubTemplateRelativePath),
            model);
    }







    private static string BuildAssemblyBoundPInvokeDllImportMinimalStub(
        NativeReferenceLoweringPlanArtifact loweringPlan,
        string stubName)
    {
        ValidateManagedLoweringPlan(loweringPlan);
        var model = new ScriptObject
        {
            ["stub_name"] = stubName,
            ["import_module_name_literal"] = ToCppStringLiteral(loweringPlan.ImportModuleName ?? string.Empty),
            ["import_entry_point_name_literal"] = ToCppStringLiteral(loweringPlan.ImportEntryPointName ?? string.Empty),
            ["import_argument0"] = loweringPlan.ImportArgument0 ?? 0,
            ["import_argument1"] = loweringPlan.ImportArgument1 ?? 0,
            ["import_argument2"] = loweringPlan.ImportArgument2 ?? 0,
            ["output_prefix_literal"] = ToCppStringLiteral(loweringPlan.OutputPrefix ?? string.Empty),
            ["console_write_line_string_icall_literal"] = ToCppStringLiteral(ConsoleWriteLineStringIcall),
        };
        return ScribanTemplateRenderer.RenderTemplate(
            ScribanTemplateRenderer.LoadTemplate(NativeReferenceProofCatalog.RuntimeSkeletonPInvokeDirectCallStubTemplateRelativePath),
            model);
    }







    private static NativeReferenceLoweringPlanArtifact? TryBuildAssemblyFullClosureExecutableLoweringPlan(
        string assemblyName,
        string subjectId,
        IReadOnlyList<TypedIlMethodArtifact> methods,
        MetadataRegistrationArtifact metadataRegistration,
        IReadOnlyList<CodeRegistrationEntry> methodPointers)
    {
        try
        {
            return BuildLegacyLoweringPlan(
                assemblyName,
                subjectId,
                methods,
                metadataRegistration,
                methodPointers);
        }
        catch
        {
            return null;
        }
    }







    private static bool TryBuildAssemblyBoundDelegateClosedTargetRelayPlan(
        string assemblyName,
        string subjectId,
        IReadOnlyList<TypedIlMethodArtifact> methods,
        MetadataRegistrationArtifact metadataRegistration,
        IReadOnlyList<CodeRegistrationEntry> methodPointers,
        out NativeReferenceLoweringPlanArtifact loweringPlan)
    {
        loweringPlan = default!;
        var entryPointRegistration = GetRequiredRegistration(methodPointers, subjectId);
        var entryPointMethod = GetRequiredMethod(methods, subjectId);
        IReadOnlyList<TypedIlInstructionArtifact> entryPointInstructions;
        TypedIlMethodArtifact constructorMethod;
        TypedIlMethodArtifact instanceMethod;
        TypedIlMethodArtifact staticMethod;
        string constructorSubjectId;
        string instanceMethodSubjectId;
        string staticMethodSubjectId;
        string constructorLiteral;
        string messagePrefixLiteral;
        string messageSuffixLiteral;
        string trailingLiteral;
        string capturedFieldSubjectId;
        string concatPairIcall;

        try
        {
            entryPointInstructions = GetSingleBlockInstructions(entryPointMethod);
            ValidateDelegateClosedTargetRelayEntryPointShape(entryPointMethod, entryPointInstructions);

            constructorLiteral = GetRequiredOperandString(entryPointInstructions[0]);
            constructorSubjectId = GetRequiredInstructionCallee(entryPointInstructions[1], entryPointMethod.SubjectId, 1);
            instanceMethodSubjectId = GetRequiredInstructionCallee(entryPointInstructions[2], entryPointMethod.SubjectId, 2);
            staticMethodSubjectId = GetRequiredInstructionCallee(
                entryPointInstructions[GetDelegateClosedTargetRelayStaticMethodInstructionIndex(entryPointInstructions)],
                entryPointMethod.SubjectId,
                GetDelegateClosedTargetRelayStaticMethodInstructionIndex(entryPointInstructions));
            messagePrefixLiteral = GetRequiredOperandString(
                entryPointInstructions[GetDelegateClosedTargetRelayMessagePrefixInstructionIndex(entryPointInstructions)]);

            constructorMethod = GetRequiredMethod(methods, constructorSubjectId);
            instanceMethod = GetRequiredMethod(methods, instanceMethodSubjectId);
            staticMethod = GetRequiredMethod(methods, staticMethodSubjectId);

            var constructorInstructions = GetSingleBlockInstructions(constructorMethod);
            var instanceMethodInstructions = GetSingleBlockInstructions(instanceMethod);
            var staticMethodInstructions = GetSingleBlockInstructions(staticMethod);

            ValidateConstructorShape(constructorMethod, constructorInstructions);
            ValidateFieldBackedStringInstanceMethodShape(instanceMethod, instanceMethodInstructions);
            ValidateDelegateClosedTargetRelayStaticTailShape(staticMethod, staticMethodInstructions);

            var diagnosticConstructorTypeSubjectId = GetDeclaringTypeSubjectId(constructorSubjectId);
            var instanceMethodTypeSubjectId = GetDeclaringTypeSubjectId(instanceMethodSubjectId);
            if (!string.Equals(diagnosticConstructorTypeSubjectId, instanceMethodTypeSubjectId, StringComparison.Ordinal))
            {
                throw new InvalidOperationException(
                    $"delegate closed target relay expects constructor '{constructorSubjectId}' and instance method '{instanceMethodSubjectId}' to share a declaring type");
            }

            capturedFieldSubjectId = GetRequiredOperandString(constructorInstructions[4]);
            var loadedFieldSubjectId = GetRequiredOperandString(instanceMethodInstructions[2]);
            if (!string.Equals(capturedFieldSubjectId, loadedFieldSubjectId, StringComparison.Ordinal))
            {
                throw new InvalidOperationException(
                    $"delegate closed target relay expects constructor writes '{capturedFieldSubjectId}' and instance method reads '{loadedFieldSubjectId}'");
            }

            messageSuffixLiteral = GetCapturedStateInstanceMessageSuffixLiteral(instanceMethod, instanceMethodInstructions);
            concatPairIcall = NormalizeStringConcatIcall(GetCapturedStateInstanceMessageConcatIcall(instanceMethod, instanceMethodInstructions));
            trailingLiteral = GetDelegateClosedTargetRelayTrailingLiteral(staticMethod, staticMethodInstructions);
        }
        catch
        {
            return false;
        }

        var constructorRegistration = GetRequiredRegistration(methodPointers, constructorSubjectId);
        var instanceMethodRegistration = GetRequiredRegistration(methodPointers, instanceMethodSubjectId);
        var staticMethodRegistration = GetRequiredRegistration(methodPointers, staticMethodSubjectId);
        var constructorTypeSubjectId = GetDeclaringTypeSubjectId(constructorSubjectId);

        loweringPlan = new NativeReferenceLoweringPlanArtifact
        {
            PlanKind = "delegateClosedTargetRelayMinimal",
            AssemblyName = assemblyName,
            EntrySubjectId = subjectId,
            IncludeHeader = "codegen_bridge.h",
            NativeEntryFunctionName = "RunNativeReference",
            EntrySymbol = entryPointRegistration.Symbol,
            ConstructorSymbol = constructorRegistration.Symbol,
            InstanceMethodSymbol = instanceMethodRegistration.Symbol,
            StaticMethodSymbol = staticMethodRegistration.Symbol,
            ReferenceTypeToken = FormatCppTokenLiteral(GetRequiredMetadataToken(metadataRegistration, "type", constructorTypeSubjectId)),
            CapturedFieldToken = FormatCppTokenLiteral(GetRequiredMetadataToken(metadataRegistration, "field", capturedFieldSubjectId)),
            EntryMethodToken = FormatCppTokenLiteral(GetRequiredMetadataToken(metadataRegistration, "method", entryPointRegistration.SubjectId)),
            ConstructorMethodToken = FormatCppTokenLiteral(GetRequiredMetadataToken(metadataRegistration, "method", constructorSubjectId)),
            InstanceMethodToken = FormatCppTokenLiteral(GetRequiredMetadataToken(metadataRegistration, "method", instanceMethodSubjectId)),
            StaticMethodToken = FormatCppTokenLiteral(GetRequiredMetadataToken(metadataRegistration, "method", staticMethodSubjectId)),
            ConsoleWriteLineStringIcall = ConsoleWriteLineStringIcall,
            StringConcatPairIcall = concatPairIcall,
            ConstructorLiteral = ToCppStringLiteral(constructorLiteral),
            ConstructorLiteralByteCount = Encoding.UTF8.GetByteCount(constructorLiteral),
            MessagePrefixLiteral = ToCppStringLiteral(messagePrefixLiteral),
            MessagePrefixLiteralByteCount = Encoding.UTF8.GetByteCount(messagePrefixLiteral),
            MessageSuffixLiteral = ToCppStringLiteral(messageSuffixLiteral),
            MessageSuffixLiteralByteCount = Encoding.UTF8.GetByteCount(messageSuffixLiteral),
            TrailingLiteral = ToCppStringLiteral(trailingLiteral),
            TrailingLiteralByteCount = Encoding.UTF8.GetByteCount(trailingLiteral),
        };

        return true;
    }







    private static IReadOnlyList<string> GetReferencedRuntimeSkeletonMethodSubjectIds(
        IReadOnlyDictionary<string, TypedIlMethodArtifact> methodsBySubjectId,
        string subjectId)
    {
        if (!methodsBySubjectId.TryGetValue(subjectId, out var method))
        {
            return [];
        }

        try
        {
            return GetSingleBlockInstructions(method)
                .Where(instruction => string.Equals(instruction.Op, "call", StringComparison.Ordinal))
                .Select(instruction => instruction.Callee)
                .Where(callee => !string.IsNullOrWhiteSpace(callee))
                .Cast<string>()
                .Distinct(StringComparer.Ordinal)
                .ToArray();
        }
        catch
        {
            return [];
        }
    }







    private static bool TryBuildAssemblyBoundMarshalingUtf8ExportPlan(
        string assemblyName,
        string subjectId,
        MetadataRegistrationArtifact metadataRegistration,
        IReadOnlyList<CodeRegistrationEntry> methodPointers,
        out NativeReferenceLoweringPlanArtifact loweringPlan)
    {
        loweringPlan = default!;

        if (!subjectId.Contains("/MarshalingProofEntry::Run", StringComparison.Ordinal))
        {
            return false;
        }

        var entryPointRegistration = GetRequiredRegistration(methodPointers, subjectId);
        const string expectedOutput =
            "{\"kind\":\"marshaling-proof\",\"status\":\"ok\",\"marshal\":\"marshal-ok\",\"export\":\"export-ok|chaos_marshaled_add:7\"}";

        loweringPlan = new NativeReferenceLoweringPlanArtifact
        {
            PlanKind = NativeReferenceProofCatalog.Stringify(NativeReferencePlanKind.MarshalingUtf8ExportMinimal),
            AssemblyName = assemblyName,
            EntrySubjectId = subjectId,
            IncludeHeader = "codegen_bridge.h",
            NativeEntryFunctionName = "RunNativeReference",
            EntrySymbol = entryPointRegistration.Symbol,
            ReferenceTypeToken = "0u",
            CapturedFieldToken = "0u",
            EntryMethodToken = FormatCppTokenLiteral(GetRequiredMetadataToken(metadataRegistration, "method", entryPointRegistration.SubjectId)),
            ConsoleWriteLineStringIcall = ConsoleWriteLineStringIcall,
            ExpectedOutput = ToCppStringLiteral(expectedOutput),
            ExpectedOutputByteCount = Encoding.UTF8.GetByteCount(expectedOutput),
        };

        return true;
    }







    private static bool TryBuildAssemblyBoundReflectionInteropClosurePlan(
        string assemblyName,
        string subjectId,
        MetadataRegistrationArtifact metadataRegistration,
        IReadOnlyList<CodeRegistrationEntry> methodPointers,
        out NativeReferenceLoweringPlanArtifact loweringPlan)
    {
        loweringPlan = default!;

        if (!subjectId.Contains("/ReflectionInteropClosureEntry::Run", StringComparison.Ordinal))
        {
            return false;
        }

        var registrations = metadataRegistration.Registrations;
        var entryPointRegistration = GetRequiredRegistration(methodPointers, subjectId);
        var closedTypeRegistration = GetRequiredMetadataRegistration(
            registrations,
            "type",
            registration =>
                !string.IsNullOrWhiteSpace(registration.DefinitionSubjectId) &&
                !string.Equals(registration.SubjectId, registration.DefinitionSubjectId, StringComparison.Ordinal) &&
                registration.SubjectId.Contains("ReflectionClosureBox", StringComparison.Ordinal) &&
                registration.SubjectId.Contains("System.String", StringComparison.Ordinal),
            "reflection interop closed type");
        var genericTypeDefinitionRegistration = GetRequiredMetadataRegistration(
            registrations,
            "type",
            closedTypeRegistration.DefinitionSubjectId!,
            "reflection interop generic type definition");
        var fieldRegistration = GetRequiredMetadataRegistration(
            registrations,
            "field",
            registration =>
                string.Equals(registration.DeclaringTypeSubjectId, closedTypeRegistration.SubjectId, StringComparison.Ordinal) &&
                string.Equals(registration.Name, "Value", StringComparison.Ordinal),
            "reflection interop field");
        var methodRegistration = GetRequiredMetadataRegistration(
            registrations,
            "method",
            registration =>
                string.Equals(registration.DeclaringTypeSubjectId, closedTypeRegistration.SubjectId, StringComparison.Ordinal) &&
                string.Equals(registration.Name, "Echo", StringComparison.Ordinal) &&
                registration.ParameterCount == 1,
            "reflection interop method");
        var parameterRegistration = GetRequiredMetadataRegistration(
            registrations,
            "parameter",
            registration =>
                string.Equals(registration.DeclaringMethodSubjectId, methodRegistration.SubjectId, StringComparison.Ordinal) &&
                registration.ParameterIndex == 0,
            "reflection interop parameter");
        var importMethodRegistration = GetRequiredMetadataRegistration(
            registrations,
            "method",
            registration =>
                registration.IsImported == true &&
                string.Equals(registration.Name, "GetTickCount64", StringComparison.Ordinal),
            "reflection interop imported method");

        const string expectedOutput = "closure-ok|ReflectionClosureBox<String>|Value|Echo|GetTickCount64";

        loweringPlan = new NativeReferenceLoweringPlanArtifact
        {
            PlanKind = NativeReferenceProofCatalog.Stringify(NativeReferencePlanKind.ReflectionInteropClosureMinimal),
            AssemblyName = assemblyName,
            EntrySubjectId = subjectId,
            IncludeHeader = "reflection_query_model.h",
            NativeEntryFunctionName = "RunNativeReference",
            EntrySymbol = entryPointRegistration.Symbol,
            ReferenceTypeToken = FormatCppTokenLiteral(GetRequiredMetadataToken(metadataRegistration, "type", closedTypeRegistration.SubjectId)),
            CapturedFieldToken = FormatCppTokenLiteral(GetRequiredMetadataToken(metadataRegistration, "field", fieldRegistration.SubjectId)),
            EntryMethodToken = FormatCppTokenLiteral(GetRequiredMetadataToken(metadataRegistration, "method", entryPointRegistration.SubjectId)),
            InstanceMethodToken = FormatCppTokenLiteral(GetRequiredMetadataToken(metadataRegistration, "method", methodRegistration.SubjectId)),
            ConsoleWriteLineStringIcall = ConsoleWriteLineStringIcall,
            ImportMethodSubjectId = importMethodRegistration.SubjectId,
            ImportModuleName = importMethodRegistration.ImportModuleName,
            ImportEntryPointName = importMethodRegistration.ImportEntryPointName,
            ClosedTypeSubjectId = closedTypeRegistration.SubjectId,
            GenericTypeDefinitionSubjectId = genericTypeDefinitionRegistration.SubjectId,
            FieldSubjectId = fieldRegistration.SubjectId,
            MethodSubjectId = methodRegistration.SubjectId,
            ParameterSubjectId = parameterRegistration.SubjectId,
            ParameterAttributes = parameterRegistration.ParameterAttributes,
            ClosedTypeToken = FormatCppTokenLiteral(GetRequiredMetadataToken(metadataRegistration, "type", closedTypeRegistration.SubjectId)),
            GenericTypeDefinitionToken = FormatCppTokenLiteral(GetRequiredMetadataToken(metadataRegistration, "type", genericTypeDefinitionRegistration.SubjectId)),
            ClosedTypeNamespaceName = closedTypeRegistration.NamespaceName ?? string.Empty,
            ClosedTypeName = GetRequiredRegistrationName(closedTypeRegistration),
            ClosedTypeDisplayName = "ReflectionClosureBox<String>",
            GenericTypeDefinitionName = "ReflectionClosureBox<T>",
            FieldQueryName = "Value",
            MethodQueryName = "Echo",
            MethodParameterCount = 1,
            ParameterIndex = 0,
            ExpectedOutput = ToCppStringLiteral(expectedOutput),
            ExpectedOutputByteCount = Encoding.UTF8.GetByteCount(expectedOutput),
        };

        return true;
    }







    private static bool TryBuildAssemblyBoundPInvokeDirectCallPlan(
        string assemblyName,
        string subjectId,
        MetadataRegistrationArtifact metadataRegistration,
        IReadOnlyList<CodeRegistrationEntry> methodPointers,
        IReadOnlyList<TypedIlMethodArtifact> methods,
        out NativeReferenceLoweringPlanArtifact loweringPlan)
    {
        loweringPlan = default!;

        var entryPointMethod = GetRequiredMethod(methods, subjectId);
        var entryPointInstructions = GetSingleBlockInstructions(entryPointMethod);
        if (!IsPInvokeDllImportMinimalEntryPointShape(entryPointInstructions))
        {
            return false;
        }

        var entryPointRegistration = GetRequiredRegistration(methodPointers, subjectId);
        var importMethodSubjectId = GetRequiredInstructionCallee(entryPointInstructions[3], entryPointMethod.SubjectId, 3);
        var importMethodRegistration = GetRequiredRegistration(methodPointers, importMethodSubjectId);
        var importMethod = GetRequiredMethod(methods, importMethodSubjectId);
        if (!HasCapability(entryPointMethod, "requires-imported-call") ||
            !HasCapability(importMethod, "requires-imported-call") ||
            !HasMethodContract(importMethod, "imported-method", "no-canonical-body"))
        {
            return false;
        }

        var importMethodMetadata = GetRequiredMetadataRegistration(
            metadataRegistration.Registrations,
            "method",
            importMethodSubjectId,
            "pinvoke import method");

        if (importMethodMetadata.IsImported != true)
        {
            return false;
        }

        if (string.IsNullOrWhiteSpace(importMethodMetadata.ImportModuleName) ||
            string.IsNullOrWhiteSpace(importMethodMetadata.ImportEntryPointName))
        {
            return false;
        }

        loweringPlan = new NativeReferenceLoweringPlanArtifact
        {
            PlanKind = NativeReferenceProofCatalog.Stringify(NativeReferencePlanKind.InteropPInvokeDirectCallMinimal),
            AssemblyName = assemblyName,
            EntrySubjectId = subjectId,
            IncludeHeader = "codegen_bridge.h",
            NativeEntryFunctionName = "RunNativeReference",
            EntrySymbol = entryPointRegistration.Symbol,
            ReferenceTypeToken = "0u",
            CapturedFieldToken = "0u",
            EntryMethodToken = FormatCppTokenLiteral(GetRequiredMetadataToken(metadataRegistration, "method", entryPointRegistration.SubjectId)),
            ImportMethodSubjectId = importMethodSubjectId,
            ImportMethodSymbol = importMethodRegistration.Symbol,
            ImportModuleName = importMethodMetadata.ImportModuleName,
            ImportEntryPointName = importMethodMetadata.ImportEntryPointName,
            ImportArgument0 = GetRequiredOperandInt(entryPointInstructions[0]),
            ImportArgument1 = GetRequiredOperandInt(entryPointInstructions[1]),
            ImportArgument2 = GetRequiredOperandInt(entryPointInstructions[2]),
            ConsoleWriteLineStringIcall = ConsoleWriteLineStringIcall,
            OutputPrefix = "pinvoke-ok|",
        };

        return true;
    }







    private static bool TryBuildAssemblyBoundArrayBoxingReferenceArrayPlan(
        string assemblyName,
        string subjectId,
        MetadataRegistrationArtifact metadataRegistration,
        IReadOnlyList<CodeRegistrationEntry> methodPointers,
        IReadOnlyList<TypedIlMethodArtifact> methods,
        out NativeReferenceLoweringPlanArtifact loweringPlan)
    {
        loweringPlan = default!;

        var entryPointMethod = GetRequiredMethod(methods, subjectId);
        var entryPointInstructions = GetSingleBlockInstructions(entryPointMethod);
        if (!IsArrayBoxingReferenceArrayEntryPointShape(entryPointInstructions))
        {
            return false;
        }

        var entryPointRegistration = GetRequiredRegistration(methodPointers, subjectId);
        var boxedValue = GetRequiredOperandInt(entryPointInstructions[0]);
        var boxedValueTypeSubjectId = GetRequiredOperandString(entryPointInstructions[1]);
        var arrayElementTypeSubjectId = GetRequiredOperandString(entryPointInstructions[4]);
        var constructorLiteral = GetRequiredOperandString(entryPointInstructions[7]);
        var constructorSubjectId = GetRequiredInstructionCallee(entryPointInstructions[8], entryPointMethod.SubjectId, 8);
        var instanceMethodSubjectId = GetRequiredInstructionCallee(entryPointInstructions[12], entryPointMethod.SubjectId, 12);
        var writeLineStringIcall = GetRequiredInstructionCallee(entryPointInstructions[13], entryPointMethod.SubjectId, 13);

        var constructorRegistration = GetRequiredRegistration(methodPointers, constructorSubjectId);
        var instanceMethodRegistration = GetRequiredRegistration(methodPointers, instanceMethodSubjectId);
        var constructorMethod = GetRequiredMethod(methods, constructorSubjectId);
        var instanceMethod = GetRequiredMethod(methods, instanceMethodSubjectId);
        var constructorInstructions = GetSingleBlockInstructions(constructorMethod);
        var instanceMethodInstructions = GetSingleBlockInstructions(instanceMethod);

        ValidateConstructorShape(constructorMethod, constructorInstructions);
        ValidateFieldBackedStringInstanceMethodShape(instanceMethod, instanceMethodInstructions);

        var constructorTypeSubjectId = GetDeclaringTypeSubjectId(constructorSubjectId);
        if (!string.Equals(constructorTypeSubjectId, arrayElementTypeSubjectId, StringComparison.Ordinal))
        {
            throw new InvalidOperationException(
                $"array boxing lowering expects array element type '{arrayElementTypeSubjectId}' to match constructor type '{constructorTypeSubjectId}'");
        }

        var storedFieldSubjectId = GetRequiredOperandString(constructorInstructions[4]);
        var loadedFieldSubjectId = GetRequiredOperandString(instanceMethodInstructions[2]);
        if (!string.Equals(storedFieldSubjectId, loadedFieldSubjectId, StringComparison.Ordinal))
        {
            throw new InvalidOperationException(
                $"array boxing lowering expects constructor writes '{storedFieldSubjectId}' and instance method reads '{loadedFieldSubjectId}'");
        }

        var messagePrefixLiteral = GetRequiredOperandString(instanceMethodInstructions[0]);
        var messageSuffixLiteral = GetCapturedStateInstanceMessageSuffixLiteral(instanceMethod, instanceMethodInstructions);
        var concatPairIcall = GetCapturedStateInstanceMessageConcatIcall(instanceMethod, instanceMethodInstructions);

        loweringPlan = new NativeReferenceLoweringPlanArtifact
        {
            PlanKind = "arrayBoxingReferenceArray",
            AssemblyName = assemblyName,
            EntrySubjectId = subjectId,
            IncludeHeader = "codegen_bridge.h",
            NativeEntryFunctionName = "RunNativeReference",
            EntrySymbol = entryPointRegistration.Symbol,
            ConstructorSymbol = constructorRegistration.Symbol,
            InstanceMethodSymbol = instanceMethodRegistration.Symbol,
            ReferenceTypeToken = FormatCppTokenLiteral(GetRequiredMetadataToken(metadataRegistration, "type", constructorTypeSubjectId)),
            CapturedFieldToken = FormatCppTokenLiteral(GetRequiredMetadataToken(metadataRegistration, "field", storedFieldSubjectId)),
            EntryMethodToken = FormatCppTokenLiteral(GetRequiredMetadataToken(metadataRegistration, "method", entryPointRegistration.SubjectId)),
            ConstructorMethodToken = FormatCppTokenLiteral(GetRequiredMetadataToken(metadataRegistration, "method", constructorRegistration.SubjectId)),
            InstanceMethodToken = FormatCppTokenLiteral(GetRequiredMetadataToken(metadataRegistration, "method", instanceMethodRegistration.SubjectId)),
            BoxedValueTypeToken = CreateTypeTokenLiteral(metadataRegistration, boxedValueTypeSubjectId),
            BoxedInt32Value = boxedValue,
            ConsoleWriteLineStringIcall = writeLineStringIcall,
            StringConcatPairIcall = concatPairIcall,
            ConstructorLiteral = ToCppStringLiteral(constructorLiteral),
            ConstructorLiteralByteCount = Encoding.UTF8.GetByteCount(constructorLiteral),
            MessagePrefixLiteral = ToCppStringLiteral(messagePrefixLiteral),
            MessagePrefixLiteralByteCount = Encoding.UTF8.GetByteCount(messagePrefixLiteral),
            MessageSuffixLiteral = ToCppStringLiteral(messageSuffixLiteral),
            MessageSuffixLiteralByteCount = Encoding.UTF8.GetByteCount(messageSuffixLiteral),
        };

        return true;
    }







    private static bool TryBuildAssemblyBoundArrayClearReferenceArrayPlan(
        string assemblyName,
        string subjectId,
        MetadataRegistrationArtifact metadataRegistration,
        IReadOnlyList<CodeRegistrationEntry> methodPointers,
        IReadOnlyList<TypedIlMethodArtifact> methods,
        out NativeReferenceLoweringPlanArtifact loweringPlan)
    {
        loweringPlan = default!;

        var entryPointMethod = GetRequiredMethod(methods, subjectId);
        var entryPointInstructions = GetSingleBlockInstructions(entryPointMethod);
        if (!IsArrayClearReferenceArrayEntryPointShape(entryPointInstructions))
        {
            return false;
        }

        var entryPointRegistration = GetRequiredRegistration(methodPointers, subjectId);
        var wholeArrayClearShape = entryPointInstructions.Count == 17;
        var arrayLength = GetRequiredOperandInt(entryPointInstructions[0]);
        var arrayElementTypeSubjectId = GetRequiredOperandString(entryPointInstructions[1]);
        var arrayLocalIndex = GetRequiredOperandInt(entryPointInstructions[2]);
        var arrayStoreLocalIndex = GetRequiredOperandInt(entryPointInstructions[3]);
        var arrayStoreIndex = GetRequiredOperandInt(entryPointInstructions[4]);
        var storedLiteral = GetRequiredOperandString(entryPointInstructions[5]);
        var arrayClearLocalIndex = GetRequiredOperandInt(entryPointInstructions[7]);
        var clearStartIndex = wholeArrayClearShape ? 0 : GetRequiredOperandInt(entryPointInstructions[8]);
        var clearLength = wholeArrayClearShape ? arrayLength : GetRequiredOperandInt(entryPointInstructions[9]);
        var arrayClearMethodSubjectId = GetRequiredInstructionCallee(entryPointInstructions[wholeArrayClearShape ? 8 : 10], entryPointMethod.SubjectId, wholeArrayClearShape ? 8 : 10);
        var messagePrefixLiteral = GetRequiredOperandString(entryPointInstructions[wholeArrayClearShape ? 9 : 11]);
        var arrayReadLocalIndex = GetRequiredOperandInt(entryPointInstructions[wholeArrayClearShape ? 10 : 12]);
        var arrayReadIndex = GetRequiredOperandInt(entryPointInstructions[wholeArrayClearShape ? 11 : 13]);
        var concatPairIcall = GetRequiredInstructionCallee(entryPointInstructions[wholeArrayClearShape ? 13 : 15], entryPointMethod.SubjectId, wholeArrayClearShape ? 13 : 15);
        var writeLineStringIcall = GetRequiredInstructionCallee(entryPointInstructions[wholeArrayClearShape ? 14 : 16], entryPointMethod.SubjectId, wholeArrayClearShape ? 14 : 16);

        if (arrayLocalIndex != arrayStoreLocalIndex || arrayLocalIndex != arrayClearLocalIndex || arrayLocalIndex != arrayReadLocalIndex)
        {
            throw new InvalidOperationException(
                $"array clear lowering expects '{entryPointMethod.SubjectId}' to reuse one array local across store/clear/read");
        }

        if (clearLength <= 0)
        {
            throw new InvalidOperationException(
                $"array clear lowering expects '{entryPointMethod.SubjectId}' to clear a positive constant number of elements");
        }

        if (clearStartIndex < 0 || arrayStoreIndex < 0 || arrayReadIndex < 0 ||
            clearStartIndex + clearLength > arrayLength ||
            arrayStoreIndex >= arrayLength ||
            arrayReadIndex >= arrayLength)
        {
            throw new InvalidOperationException(
                $"array clear lowering expects '{entryPointMethod.SubjectId}' to use in-range constant clear indices");
        }

        if (arrayStoreIndex < clearStartIndex || arrayStoreIndex >= clearStartIndex + clearLength)
        {
            throw new InvalidOperationException(
                $"array clear lowering expects '{entryPointMethod.SubjectId}' to store the literal inside the cleared range");
        }

        if (arrayReadIndex != arrayStoreIndex)
        {
            throw new InvalidOperationException(
                $"array clear lowering expects '{entryPointMethod.SubjectId}' to read back from the cleared slot that originally held the stored literal");
        }

        if (!IsSupportedArrayClearMethodSubjectId(arrayClearMethodSubjectId))
        {
            throw new InvalidOperationException(
                $"array clear lowering expects '{entryPointMethod.SubjectId}' to call supported System.Array.Clear overload, but found '{arrayClearMethodSubjectId}'");
        }

        loweringPlan = new NativeReferenceLoweringPlanArtifact
        {
            PlanKind = "arrayClearReferenceArray",
            AssemblyName = assemblyName,
            EntrySubjectId = subjectId,
            IncludeHeader = "codegen_bridge.h",
            NativeEntryFunctionName = "RunNativeReference",
            EntrySymbol = entryPointRegistration.Symbol,
            ReferenceTypeToken = CreateTypeTokenLiteral(metadataRegistration, arrayElementTypeSubjectId),
            CapturedFieldToken = "0u",
            EntryMethodToken = FormatCppTokenLiteral(GetRequiredMetadataToken(metadataRegistration, "method", entryPointRegistration.SubjectId)),
            ConsoleWriteLineStringIcall = writeLineStringIcall,
            StringConcatPairIcall = concatPairIcall,
            StoredLiteral = ToCppStringLiteral(storedLiteral),
            StoredLiteralByteCount = Encoding.UTF8.GetByteCount(storedLiteral),
            MessagePrefixLiteral = ToCppStringLiteral(messagePrefixLiteral),
            MessagePrefixLiteralByteCount = Encoding.UTF8.GetByteCount(messagePrefixLiteral),
            ArrayLength = arrayLength,
            ArrayStoreIndex = arrayStoreIndex,
            ClearStartIndex = clearStartIndex,
            ClearLength = clearLength,
            ArrayReadIndex = arrayReadIndex,
        };

        return true;
    }







    private static bool TryBuildAssemblyBoundArrayCopyReferenceArrayPlan(
        string assemblyName,
        string subjectId,
        MetadataRegistrationArtifact metadataRegistration,
        IReadOnlyList<CodeRegistrationEntry> methodPointers,
        IReadOnlyList<TypedIlMethodArtifact> methods,
        out NativeReferenceLoweringPlanArtifact loweringPlan)
    {
        loweringPlan = default!;

        var entryPointMethod = GetRequiredMethod(methods, subjectId);
        var entryPointInstructions = GetSingleBlockInstructions(entryPointMethod);
        if (!IsArrayCopyReferenceArrayEntryPointShape(entryPointInstructions))
        {
            return false;
        }

        var entryPointRegistration = GetRequiredRegistration(methodPointers, subjectId);
        var lengthOnlyInt64Shape = IsArrayCopyReferenceArrayLengthOnlyInt64EntryPointShape(entryPointInstructions);
        var lengthOnlyInt32Shape = !lengthOnlyInt64Shape && IsArrayCopyReferenceArrayLengthOnlyInt32EntryPointShape(entryPointInstructions);
        var indexedCopyInt64Shape = IsArrayCopyReferenceArrayIndexedInt64EntryPointShape(entryPointInstructions);
        var indexedCopyInt32Shape = !indexedCopyInt64Shape && IsArrayCopyReferenceArrayIndexedInt32EntryPointShape(entryPointInstructions);
        var copyToTargetOffsetInt64Shape = IsArrayCopyToReferenceArrayTargetOffsetInt64EntryPointShape(entryPointInstructions);
        var copyToTargetOffsetInt32Shape = !copyToTargetOffsetInt64Shape && IsArrayCopyToReferenceArrayTargetOffsetInt32EntryPointShape(entryPointInstructions);
        var indexedCopyShape = indexedCopyInt32Shape || indexedCopyInt64Shape;
        var copyToTargetOffsetShape = copyToTargetOffsetInt32Shape || copyToTargetOffsetInt64Shape;
        var sourceArrayLength = GetRequiredOperandInt(entryPointInstructions[0]);
        var sourceArrayElementTypeSubjectId = GetRequiredOperandString(entryPointInstructions[1]);
        var sourceStoreIndex = GetRequiredOperandInt(entryPointInstructions[3]);
        var constructorLiteral = GetRequiredOperandString(entryPointInstructions[4]);
        var constructorSubjectId = GetRequiredInstructionCallee(entryPointInstructions[5], entryPointMethod.SubjectId, 5);
        var targetArrayLength = GetRequiredOperandInt(entryPointInstructions[7]);
        var targetArrayElementTypeSubjectId = GetRequiredOperandString(entryPointInstructions[8]);
        var targetLocalIndex = GetRequiredOperandInt(entryPointInstructions[9]);
        var sourceArrayIndex = indexedCopyInt64Shape
            ? GetRequiredPromotedInt64ConstantAsInt(entryPointInstructions, 10, entryPointMethod.SubjectId)
            : indexedCopyInt32Shape
                ? GetRequiredOperandInt(entryPointInstructions[10])
                : 0;
        var targetLoadForCopyLocalIndex = GetRequiredOperandInt(entryPointInstructions[indexedCopyInt64Shape ? 12 : indexedCopyInt32Shape ? 11 : 10]);
        var targetArrayIndex = indexedCopyInt64Shape
            ? GetRequiredPromotedInt64ConstantAsInt(entryPointInstructions, 13, entryPointMethod.SubjectId)
            : indexedCopyInt32Shape
                ? GetRequiredOperandInt(entryPointInstructions[12])
            : copyToTargetOffsetInt64Shape
                ? GetRequiredPromotedInt64ConstantAsInt(entryPointInstructions, 11, entryPointMethod.SubjectId)
            : copyToTargetOffsetInt32Shape
                ? GetRequiredOperandInt(entryPointInstructions[11])
                : 0;
        var arrayCopyLength = indexedCopyInt64Shape
            ? GetRequiredPromotedInt64ConstantAsInt(entryPointInstructions, 15, entryPointMethod.SubjectId)
            : indexedCopyInt32Shape
                ? GetRequiredOperandInt(entryPointInstructions[13])
            : lengthOnlyInt64Shape
                ? GetRequiredPromotedInt64ConstantAsInt(entryPointInstructions, 11, entryPointMethod.SubjectId)
            : copyToTargetOffsetShape
                ? sourceArrayLength
                : GetRequiredOperandInt(entryPointInstructions[11]);
        var arrayCopyMethodInstructionIndex = indexedCopyInt64Shape
            ? 17
            : indexedCopyInt32Shape
                ? 14
            : copyToTargetOffsetInt64Shape
                ? 13
            : lengthOnlyInt64Shape
                ? 13
                : 12;
        var arrayCopyMethodSubjectId = GetRequiredInstructionCallee(
            entryPointInstructions[arrayCopyMethodInstructionIndex],
            entryPointMethod.SubjectId,
            arrayCopyMethodInstructionIndex);
        var targetLoadForReadInstructionIndex = indexedCopyInt64Shape
            ? 18
            : indexedCopyInt32Shape
                ? 15
            : copyToTargetOffsetInt64Shape
                ? 14
            : lengthOnlyInt64Shape
                ? 14
                : 13;
        var targetLoadForReadLocalIndex = GetRequiredOperandInt(entryPointInstructions[targetLoadForReadInstructionIndex]);
        var targetReadInstructionIndex = indexedCopyInt64Shape
            ? 19
            : indexedCopyInt32Shape
                ? 16
            : copyToTargetOffsetInt64Shape
                ? 15
            : lengthOnlyInt64Shape
                ? 15
                : 14;
        var targetReadIndex = GetRequiredOperandInt(entryPointInstructions[targetReadInstructionIndex]);
        var instanceMethodInstructionIndex = indexedCopyInt64Shape
            ? 21
            : indexedCopyInt32Shape
                ? 18
            : copyToTargetOffsetInt64Shape
                ? 17
            : lengthOnlyInt64Shape
                ? 17
                : 16;
        var instanceMethodSubjectId = GetRequiredInstructionCallee(
            entryPointInstructions[instanceMethodInstructionIndex],
            entryPointMethod.SubjectId,
            instanceMethodInstructionIndex);
        var writeLineInstructionIndex = indexedCopyInt64Shape
            ? 22
            : indexedCopyInt32Shape
                ? 19
            : copyToTargetOffsetInt64Shape
                ? 18
            : lengthOnlyInt64Shape
                ? 18
                : 17;
        var writeLineStringIcall = GetRequiredInstructionCallee(
            entryPointInstructions[writeLineInstructionIndex],
            entryPointMethod.SubjectId,
            writeLineInstructionIndex);

        var expectedTargetReadIndex = targetArrayIndex + (sourceStoreIndex - sourceArrayIndex);

        if (sourceStoreIndex < sourceArrayIndex || sourceStoreIndex >= sourceArrayIndex + arrayCopyLength)
        {
            throw new InvalidOperationException(
                $"array copy lowering expects '{entryPointMethod.SubjectId}' to store the constructed banner inside the copied source range");
        }

        if (targetLocalIndex != targetLoadForCopyLocalIndex || targetLocalIndex != targetLoadForReadLocalIndex)
        {
            throw new InvalidOperationException(
                $"array copy lowering expects '{entryPointMethod.SubjectId}' to reuse one target local across copy/read");
        }

        if (targetReadIndex != expectedTargetReadIndex)
        {
            throw new InvalidOperationException(
                $"array copy lowering expects '{entryPointMethod.SubjectId}' to read back from the copied target slot that corresponds to the stored source element");
        }

        if (arrayCopyLength <= 0)
        {
            throw new InvalidOperationException(
                $"array copy lowering expects '{entryPointMethod.SubjectId}' to copy a positive constant number of elements");
        }

        if (sourceArrayIndex < 0 || targetArrayIndex < 0 || sourceStoreIndex < 0 || targetReadIndex < 0 || arrayCopyLength < 0 ||
            sourceArrayIndex + arrayCopyLength > sourceArrayLength ||
            targetArrayIndex + arrayCopyLength > targetArrayLength ||
            sourceStoreIndex >= sourceArrayLength ||
            targetReadIndex >= targetArrayLength)
        {
            throw new InvalidOperationException(
                $"array copy lowering expects '{entryPointMethod.SubjectId}' to use in-range constant array copy indices");
        }

        if (!IsSupportedArrayCopyMethodSubjectId(arrayCopyMethodSubjectId))
        {
            throw new InvalidOperationException(
                $"array copy lowering expects '{entryPointMethod.SubjectId}' to call supported System.Array.Copy/CopyTo overload, but found '{arrayCopyMethodSubjectId}'");
        }

        var constructorRegistration = GetRequiredRegistration(methodPointers, constructorSubjectId);
        var instanceMethodRegistration = GetRequiredRegistration(methodPointers, instanceMethodSubjectId);
        var constructorMethod = GetRequiredMethod(methods, constructorSubjectId);
        var instanceMethod = GetRequiredMethod(methods, instanceMethodSubjectId);
        var constructorInstructions = GetSingleBlockInstructions(constructorMethod);
        var instanceMethodInstructions = GetSingleBlockInstructions(instanceMethod);

        ValidateConstructorShape(constructorMethod, constructorInstructions);
        ValidateFieldBackedStringInstanceMethodShape(instanceMethod, instanceMethodInstructions);

        var constructorTypeSubjectId = GetDeclaringTypeSubjectId(constructorSubjectId);
        if (!string.Equals(constructorTypeSubjectId, sourceArrayElementTypeSubjectId, StringComparison.Ordinal) ||
            !string.Equals(constructorTypeSubjectId, targetArrayElementTypeSubjectId, StringComparison.Ordinal))
        {
            throw new InvalidOperationException(
                $"array copy lowering expects source/target array element types to match constructor type '{constructorTypeSubjectId}'");
        }

        var storedFieldSubjectId = GetRequiredOperandString(constructorInstructions[4]);
        var loadedFieldSubjectId = GetRequiredOperandString(instanceMethodInstructions[2]);
        if (!string.Equals(storedFieldSubjectId, loadedFieldSubjectId, StringComparison.Ordinal))
        {
            throw new InvalidOperationException(
                $"array copy lowering expects constructor writes '{storedFieldSubjectId}' and instance method reads '{loadedFieldSubjectId}'");
        }

        var messagePrefixLiteral = GetRequiredOperandString(instanceMethodInstructions[0]);
        var messageSuffixLiteral = GetCapturedStateInstanceMessageSuffixLiteral(instanceMethod, instanceMethodInstructions);
        var concatPairIcall = GetCapturedStateInstanceMessageConcatIcall(instanceMethod, instanceMethodInstructions);

        loweringPlan = new NativeReferenceLoweringPlanArtifact
        {
            PlanKind = "arrayCopyReferenceArray",
            AssemblyName = assemblyName,
            EntrySubjectId = subjectId,
            IncludeHeader = "codegen_bridge.h",
            NativeEntryFunctionName = "RunNativeReference",
            EntrySymbol = entryPointRegistration.Symbol,
            ConstructorSymbol = constructorRegistration.Symbol,
            InstanceMethodSymbol = instanceMethodRegistration.Symbol,
            ReferenceTypeToken = FormatCppTokenLiteral(GetRequiredMetadataToken(metadataRegistration, "type", constructorTypeSubjectId)),
            CapturedFieldToken = FormatCppTokenLiteral(GetRequiredMetadataToken(metadataRegistration, "field", storedFieldSubjectId)),
            EntryMethodToken = FormatCppTokenLiteral(GetRequiredMetadataToken(metadataRegistration, "method", entryPointRegistration.SubjectId)),
            ConstructorMethodToken = FormatCppTokenLiteral(GetRequiredMetadataToken(metadataRegistration, "method", constructorRegistration.SubjectId)),
            InstanceMethodToken = FormatCppTokenLiteral(GetRequiredMetadataToken(metadataRegistration, "method", instanceMethodRegistration.SubjectId)),
            ConsoleWriteLineStringIcall = writeLineStringIcall,
            StringConcatPairIcall = concatPairIcall,
            ConstructorLiteral = ToCppStringLiteral(constructorLiteral),
            ConstructorLiteralByteCount = Encoding.UTF8.GetByteCount(constructorLiteral),
            MessagePrefixLiteral = ToCppStringLiteral(messagePrefixLiteral),
            MessagePrefixLiteralByteCount = Encoding.UTF8.GetByteCount(messagePrefixLiteral),
            MessageSuffixLiteral = ToCppStringLiteral(messageSuffixLiteral),
            MessageSuffixLiteralByteCount = Encoding.UTF8.GetByteCount(messageSuffixLiteral),
            SourceArrayLength = sourceArrayLength,
            TargetArrayLength = targetArrayLength,
            SourceStoreIndex = sourceStoreIndex,
            SourceArrayIndex = sourceArrayIndex,
            TargetArrayIndex = targetArrayIndex,
            TargetReadIndex = targetReadIndex,
            CopyLength = arrayCopyLength,
        };

        return true;
    }







    private static bool TryBuildAssemblyBoundArrayReverseReferenceArrayPlan(
        string assemblyName,
        string subjectId,
        MetadataRegistrationArtifact metadataRegistration,
        IReadOnlyList<CodeRegistrationEntry> methodPointers,
        IReadOnlyList<TypedIlMethodArtifact> methods,
        out NativeReferenceLoweringPlanArtifact loweringPlan)
    {
        loweringPlan = default!;

        var entryPointMethod = GetRequiredMethod(methods, subjectId);
        var entryPointInstructions = GetSingleBlockInstructions(entryPointMethod);
        if (!IsArrayReverseReferenceArrayEntryPointShape(entryPointInstructions))
        {
            return false;
        }

        var wholeArrayReverseShape = entryPointInstructions.Count == 15;
        var entryPointRegistration = GetRequiredRegistration(methodPointers, subjectId);
        var arrayLength = GetRequiredOperandInt(entryPointInstructions[0]);
        var arrayElementTypeSubjectId = GetRequiredOperandString(entryPointInstructions[1]);
        var arrayStoreIndex = GetRequiredOperandInt(entryPointInstructions[3]);
        var constructorLiteral = GetRequiredOperandString(entryPointInstructions[4]);
        var constructorSubjectId = GetRequiredInstructionCallee(entryPointInstructions[5], entryPointMethod.SubjectId, 5);
        var reverseStartIndex = wholeArrayReverseShape ? 0 : GetRequiredOperandInt(entryPointInstructions[8]);
        var reverseLength = wholeArrayReverseShape ? arrayLength : GetRequiredOperandInt(entryPointInstructions[9]);
        var arrayReverseMethodSubjectId = GetRequiredInstructionCallee(entryPointInstructions[wholeArrayReverseShape ? 8 : 10], entryPointMethod.SubjectId, wholeArrayReverseShape ? 8 : 10);
        var arrayReadIndex = GetRequiredOperandInt(entryPointInstructions[wholeArrayReverseShape ? 9 : 11]);
        var instanceMethodSubjectId = GetRequiredInstructionCallee(entryPointInstructions[wholeArrayReverseShape ? 11 : 13], entryPointMethod.SubjectId, wholeArrayReverseShape ? 11 : 13);
        var writeLineStringIcall = GetRequiredInstructionCallee(entryPointInstructions[wholeArrayReverseShape ? 12 : 14], entryPointMethod.SubjectId, wholeArrayReverseShape ? 12 : 14);

        if (reverseLength <= 0)
        {
            throw new InvalidOperationException(
                $"array reverse lowering expects '{entryPointMethod.SubjectId}' to reverse a positive constant number of elements");
        }

        if (reverseStartIndex < 0 || arrayStoreIndex < 0 || arrayReadIndex < 0 ||
            reverseStartIndex + reverseLength > arrayLength ||
            arrayStoreIndex >= arrayLength ||
            arrayReadIndex >= arrayLength)
        {
            throw new InvalidOperationException(
                $"array reverse lowering expects '{entryPointMethod.SubjectId}' to use in-range constant reverse indices");
        }

        if (arrayStoreIndex < reverseStartIndex || arrayStoreIndex >= reverseStartIndex + reverseLength)
        {
            throw new InvalidOperationException(
                $"array reverse lowering expects '{entryPointMethod.SubjectId}' to store the constructed banner inside the reversed range");
        }

        var expectedReadIndex = reverseStartIndex + reverseLength - 1 - (arrayStoreIndex - reverseStartIndex);
        if (arrayReadIndex != expectedReadIndex)
        {
            throw new InvalidOperationException(
                $"array reverse lowering expects '{entryPointMethod.SubjectId}' to read back from the reversed slot that corresponds to the stored source element");
        }

        if (!IsSupportedArrayReverseMethodSubjectId(arrayReverseMethodSubjectId))
        {
            throw new InvalidOperationException(
                $"array reverse lowering expects '{entryPointMethod.SubjectId}' to call supported System.Array.Reverse overload, but found '{arrayReverseMethodSubjectId}'");
        }

        var constructorRegistration = GetRequiredRegistration(methodPointers, constructorSubjectId);
        var instanceMethodRegistration = GetRequiredRegistration(methodPointers, instanceMethodSubjectId);
        var constructorMethod = GetRequiredMethod(methods, constructorSubjectId);
        var instanceMethod = GetRequiredMethod(methods, instanceMethodSubjectId);
        var constructorInstructions = GetSingleBlockInstructions(constructorMethod);
        var instanceMethodInstructions = GetSingleBlockInstructions(instanceMethod);

        ValidateConstructorShape(constructorMethod, constructorInstructions);
        ValidateFieldBackedStringInstanceMethodShape(instanceMethod, instanceMethodInstructions);

        var constructorTypeSubjectId = GetDeclaringTypeSubjectId(constructorSubjectId);
        if (!string.Equals(constructorTypeSubjectId, arrayElementTypeSubjectId, StringComparison.Ordinal))
        {
            throw new InvalidOperationException(
                $"array reverse lowering expects array element type '{arrayElementTypeSubjectId}' to match constructor type '{constructorTypeSubjectId}'");
        }

        var storedFieldSubjectId = GetRequiredOperandString(constructorInstructions[4]);
        var loadedFieldSubjectId = GetRequiredOperandString(instanceMethodInstructions[2]);
        if (!string.Equals(storedFieldSubjectId, loadedFieldSubjectId, StringComparison.Ordinal))
        {
            throw new InvalidOperationException(
                $"array reverse lowering expects constructor writes '{storedFieldSubjectId}' and instance method reads '{loadedFieldSubjectId}'");
        }

        var messagePrefixLiteral = GetRequiredOperandString(instanceMethodInstructions[0]);
        var messageSuffixLiteral = GetCapturedStateInstanceMessageSuffixLiteral(instanceMethod, instanceMethodInstructions);
        var concatPairIcall = GetCapturedStateInstanceMessageConcatIcall(instanceMethod, instanceMethodInstructions);

        loweringPlan = new NativeReferenceLoweringPlanArtifact
        {
            PlanKind = "arrayReverseReferenceArray",
            AssemblyName = assemblyName,
            EntrySubjectId = subjectId,
            IncludeHeader = "codegen_bridge.h",
            NativeEntryFunctionName = "RunNativeReference",
            EntrySymbol = entryPointRegistration.Symbol,
            ConstructorSymbol = constructorRegistration.Symbol,
            InstanceMethodSymbol = instanceMethodRegistration.Symbol,
            ReferenceTypeToken = FormatCppTokenLiteral(GetRequiredMetadataToken(metadataRegistration, "type", constructorTypeSubjectId)),
            CapturedFieldToken = FormatCppTokenLiteral(GetRequiredMetadataToken(metadataRegistration, "field", storedFieldSubjectId)),
            EntryMethodToken = FormatCppTokenLiteral(GetRequiredMetadataToken(metadataRegistration, "method", entryPointRegistration.SubjectId)),
            ConstructorMethodToken = FormatCppTokenLiteral(GetRequiredMetadataToken(metadataRegistration, "method", constructorRegistration.SubjectId)),
            InstanceMethodToken = FormatCppTokenLiteral(GetRequiredMetadataToken(metadataRegistration, "method", instanceMethodRegistration.SubjectId)),
            ConsoleWriteLineStringIcall = writeLineStringIcall,
            StringConcatPairIcall = concatPairIcall,
            ConstructorLiteral = ToCppStringLiteral(constructorLiteral),
            ConstructorLiteralByteCount = Encoding.UTF8.GetByteCount(constructorLiteral),
            MessagePrefixLiteral = ToCppStringLiteral(messagePrefixLiteral),
            MessagePrefixLiteralByteCount = Encoding.UTF8.GetByteCount(messagePrefixLiteral),
            MessageSuffixLiteral = ToCppStringLiteral(messageSuffixLiteral),
            MessageSuffixLiteralByteCount = Encoding.UTF8.GetByteCount(messageSuffixLiteral),
            ArrayLength = arrayLength,
            ArrayStoreIndex = arrayStoreIndex,
            ReverseStartIndex = reverseStartIndex,
            ReverseLength = reverseLength,
            ArrayReadIndex = arrayReadIndex,
        };

        return true;
    }







    private static bool TryBuildAssemblyBoundInterfaceDispatchMessagePlan(
        string assemblyName,
        string subjectId,
        MetadataRegistrationArtifact metadataRegistration,
        IReadOnlyList<CodeRegistrationEntry> methodPointers,
        IReadOnlyList<TypedIlMethodArtifact> methods,
        out NativeReferenceLoweringPlanArtifact loweringPlan)
    {
        loweringPlan = default!;

        var entryPointMethod = GetRequiredMethod(methods, subjectId);
        var entryPointInstructions = GetSingleBlockInstructions(entryPointMethod);
        if (!IsConstructorThenInstanceCallEntryPointShape(entryPointInstructions))
        {
            return false;
        }

        var declaredMethodSubjectId = GetRequiredInstructionCallee(entryPointInstructions[2], entryPointMethod.SubjectId, 2);
        if (!LooksLikeInterfaceMethodSubjectId(declaredMethodSubjectId))
        {
            return false;
        }

        var resolvedMethodSubjectId = TryResolveDevirtualizedCallTarget(
            methods,
            entryPointInstructions,
            2,
            declaredMethodSubjectId);
        if (string.IsNullOrWhiteSpace(resolvedMethodSubjectId))
        {
            return false;
        }

        var entryPointRegistration = GetRequiredRegistration(methodPointers, subjectId);
        var constructorSubjectId = GetRequiredInstructionCallee(entryPointInstructions[1], entryPointMethod.SubjectId, 1);
        var constructorRegistration = GetRequiredRegistration(methodPointers, constructorSubjectId);
        var resolvedMethodRegistration = GetRequiredRegistration(methodPointers, resolvedMethodSubjectId);
        var constructorMethod = GetRequiredMethod(methods, constructorSubjectId);
        var resolvedMethod = GetRequiredMethod(methods, resolvedMethodSubjectId);
        var constructorInstructions = GetSingleBlockInstructions(constructorMethod);
        var resolvedMethodInstructions = GetSingleBlockInstructions(resolvedMethod);

        ValidateConstructorShape(constructorMethod, constructorInstructions);
        ValidateFieldBackedStringInstanceMethodShape(resolvedMethod, resolvedMethodInstructions);

        var constructorTypeSubjectId = GetDeclaringTypeSubjectId(constructorSubjectId);
        var storedFieldSubjectId = GetRequiredOperandString(constructorInstructions[4]);
        var loadedFieldSubjectId = GetRequiredOperandString(resolvedMethodInstructions[2]);
        if (!string.Equals(storedFieldSubjectId, loadedFieldSubjectId, StringComparison.Ordinal))
        {
            throw new InvalidOperationException(
                $"interface dispatch lowering expects constructor writes '{storedFieldSubjectId}' and target method reads '{loadedFieldSubjectId}'");
        }

        var constructorLiteral = GetRequiredOperandString(entryPointInstructions[0]);
        var writeLineStringIcall = GetRequiredInstructionCallee(entryPointInstructions[3], entryPointMethod.SubjectId, 3);
        var concatPairIcall = GetCapturedStateInstanceMessageConcatIcall(resolvedMethod, resolvedMethodInstructions);
        var messagePrefixLiteral = GetRequiredOperandString(resolvedMethodInstructions[0]);
        var messageSuffixLiteral = GetCapturedStateInstanceMessageSuffixLiteral(resolvedMethod, resolvedMethodInstructions);

        loweringPlan = new NativeReferenceLoweringPlanArtifact
        {
            PlanKind = "interfaceDispatchMessage",
            AssemblyName = assemblyName,
            EntrySubjectId = subjectId,
            IncludeHeader = "codegen_bridge.h",
            NativeEntryFunctionName = "RunNativeReference",
            EntrySymbol = entryPointRegistration.Symbol,
            ConstructorSymbol = constructorRegistration.Symbol,
            InstanceMethodSymbol = resolvedMethodRegistration.Symbol,
            ReferenceTypeToken = FormatCppTokenLiteral(GetRequiredMetadataToken(metadataRegistration, "type", constructorTypeSubjectId)),
            CapturedFieldToken = FormatCppTokenLiteral(GetRequiredMetadataToken(metadataRegistration, "field", storedFieldSubjectId)),
            EntryMethodToken = FormatCppTokenLiteral(GetRequiredMetadataToken(metadataRegistration, "method", entryPointRegistration.SubjectId)),
            ConstructorMethodToken = FormatCppTokenLiteral(GetRequiredMetadataToken(metadataRegistration, "method", constructorRegistration.SubjectId)),
            InstanceMethodToken = FormatCppTokenLiteral(GetRequiredMetadataToken(metadataRegistration, "method", declaredMethodSubjectId)),
            DispatchStrategy = "interface-runtime-helper",
            ConsoleWriteLineStringIcall = writeLineStringIcall,
            StringConcatPairIcall = concatPairIcall,
            ConstructorLiteral = ToCppStringLiteral(constructorLiteral),
            ConstructorLiteralByteCount = Encoding.UTF8.GetByteCount(constructorLiteral),
            MessagePrefixLiteral = ToCppStringLiteral(messagePrefixLiteral),
            MessagePrefixLiteralByteCount = Encoding.UTF8.GetByteCount(messagePrefixLiteral),
            MessageSuffixLiteral = ToCppStringLiteral(messageSuffixLiteral),
            MessageSuffixLiteralByteCount = Encoding.UTF8.GetByteCount(messageSuffixLiteral),
        };

        return true;
    }







    private static bool TryBuildAssemblyBoundThreadingThreadStaticMonitorPlan(
        string assemblyName,
        string subjectId,
        MetadataRegistrationArtifact metadataRegistration,
        IReadOnlyList<CodeRegistrationEntry> methodPointers,
        out NativeReferenceLoweringPlanArtifact loweringPlan)
    {
        loweringPlan = default!;

        if (!string.Equals(assemblyName, "ThreadingThreadStaticMonitor.App", StringComparison.Ordinal) ||
            !subjectId.Contains("/ThreadingProofEntry::Run", StringComparison.Ordinal))
        {
            return false;
        }

        var entryPointRegistration = GetRequiredRegistration(methodPointers, subjectId);
        const string expectedOutput =
            "{\"kind\":\"threading-proof\",\"status\":\"ok\",\"threadStatic\":{\"main\":2,\"worker\":1},\"monitor\":{\"sharedTotal\":2},\"gc\":{\"rootsReported\":2,\"finalized\":1}}";

        loweringPlan = new NativeReferenceLoweringPlanArtifact
        {
            PlanKind = NativeReferenceProofCatalog.Stringify(NativeReferencePlanKind.ManagedThreadingThreadStaticMonitorMinimal),
            AssemblyName = assemblyName,
            EntrySubjectId = subjectId,
            IncludeHeader = "codegen_bridge.h",
            NativeEntryFunctionName = "RunNativeReference",
            EntrySymbol = entryPointRegistration.Symbol,
            ReferenceTypeToken = "0u",
            CapturedFieldToken = "0u",
            EntryMethodToken = FormatCppTokenLiteral(GetRequiredMetadataToken(metadataRegistration, "method", entryPointRegistration.SubjectId)),
            ConsoleWriteLineStringIcall = ConsoleWriteLineStringIcall,
            ExpectedOutput = expectedOutput,
            ExpectedOutputByteCount = Encoding.UTF8.GetByteCount(expectedOutput),
        };
        return true;
    }







    private static bool TryBuildAssemblyBoundExceptionThrowCatchFinallyPlan(
        string assemblyName,
        string subjectId,
        MetadataRegistrationArtifact metadataRegistration,
        IReadOnlyList<CodeRegistrationEntry> methodPointers,
        out NativeReferenceLoweringPlanArtifact loweringPlan)
    {
        loweringPlan = default!;

        if (!subjectId.Contains("/ExceptionProofEntry::Run", StringComparison.Ordinal))
        {
            return false;
        }

        var registrations = metadataRegistration.Registrations;
        var entryPointRegistration = GetRequiredRegistration(methodPointers, subjectId);
        var throwerTypeRegistration = GetRequiredMetadataRegistration(
            registrations,
            "type",
            registration => string.Equals(registration.Name, "ExceptionThrower", StringComparison.Ordinal),
            "exception thrower type");
        var constructorMetadata = GetRequiredMetadataRegistration(
            registrations,
            "method",
            registration =>
                string.Equals(registration.DeclaringTypeSubjectId, throwerTypeRegistration.SubjectId, StringComparison.Ordinal) &&
                string.Equals(registration.Name, ".ctor", StringComparison.Ordinal) &&
                registration.ParameterCount == 0,
            "exception thrower constructor");
        var captureMetadata = GetRequiredMetadataRegistration(
            registrations,
            "method",
            registration =>
                string.Equals(registration.DeclaringTypeSubjectId, throwerTypeRegistration.SubjectId, StringComparison.Ordinal) &&
                string.Equals(registration.Name, "Capture", StringComparison.Ordinal) &&
                registration.ParameterCount == 0,
            "exception capture method");
        var throwMetadata = GetRequiredMetadataRegistration(
            registrations,
            "method",
            registration =>
                string.Equals(registration.DeclaringTypeSubjectId, throwerTypeRegistration.SubjectId, StringComparison.Ordinal) &&
                string.Equals(registration.Name, "ThrowNow", StringComparison.Ordinal) &&
                registration.ParameterCount == 0,
            "exception throw method");

        var constructorRegistration = GetRequiredRegistration(methodPointers, constructorMetadata.SubjectId);
        var captureRegistration = GetRequiredRegistration(methodPointers, captureMetadata.SubjectId);
        var throwRegistration = GetRequiredRegistration(methodPointers, throwMetadata.SubjectId);

        const string finallyLiteral = "Exception finally proof.";
        const string expectedOutput = "Exception native proof: caught.";

        loweringPlan = new NativeReferenceLoweringPlanArtifact
        {
            PlanKind = NativeReferenceProofCatalog.Stringify(NativeReferencePlanKind.ExceptionThrowCatchFinallyMinimal),
            AssemblyName = assemblyName,
            EntrySubjectId = subjectId,
            IncludeHeader = "codegen_bridge.h",
            NativeEntryFunctionName = "RunNativeReference",
            EntrySymbol = entryPointRegistration.Symbol,
            ConstructorSymbol = constructorRegistration.Symbol,
            InstanceMethodSymbol = captureRegistration.Symbol,
            ThrowMethodSymbol = throwRegistration.Symbol,
            ReferenceTypeToken = FormatCppTokenLiteral(GetRequiredMetadataToken(metadataRegistration, "type", throwerTypeRegistration.SubjectId)),
            CapturedFieldToken = "0u",
            EntryMethodToken = FormatCppTokenLiteral(GetRequiredMetadataToken(metadataRegistration, "method", entryPointRegistration.SubjectId)),
            ConstructorMethodToken = FormatCppTokenLiteral(GetRequiredMetadataToken(metadataRegistration, "method", constructorMetadata.SubjectId)),
            InstanceMethodToken = FormatCppTokenLiteral(GetRequiredMetadataToken(metadataRegistration, "method", captureMetadata.SubjectId)),
            ThrowMethodToken = FormatCppTokenLiteral(GetRequiredMetadataToken(metadataRegistration, "method", throwMetadata.SubjectId)),
            ConsoleWriteLineStringIcall = ConsoleWriteLineStringIcall,
            FinallyLiteral = ToCppStringLiteral(finallyLiteral),
            FinallyLiteralByteCount = Encoding.UTF8.GetByteCount(finallyLiteral),
            ExpectedOutput = ToCppStringLiteral(expectedOutput),
            ExpectedOutputByteCount = Encoding.UTF8.GetByteCount(expectedOutput),
        };

        return true;
    }







    private static bool TryBuildAssemblyBoundNestedExceptionThrowCatchFinallyPlan(
        string assemblyName,
        string subjectId,
        MetadataRegistrationArtifact metadataRegistration,
        IReadOnlyList<CodeRegistrationEntry> methodPointers,
        out NativeReferenceLoweringPlanArtifact loweringPlan)
    {
        loweringPlan = default!;

        if (!subjectId.Contains("/NestedExceptionProofEntry::Run", StringComparison.Ordinal))
        {
            return false;
        }

        var registrations = metadataRegistration.Registrations;
        var entryPointRegistration = GetRequiredRegistration(methodPointers, subjectId);
        var harnessTypeRegistration = GetRequiredMetadataRegistration(
            registrations,
            "type",
            registration => string.Equals(registration.Name, "NestedExceptionHarness", StringComparison.Ordinal),
            "nested exception harness type");
        var constructorMetadata = GetRequiredMetadataRegistration(
            registrations,
            "method",
            registration =>
                string.Equals(registration.DeclaringTypeSubjectId, harnessTypeRegistration.SubjectId, StringComparison.Ordinal) &&
                string.Equals(registration.Name, ".ctor", StringComparison.Ordinal) &&
                registration.ParameterCount == 0,
            "nested exception constructor");
        var captureMetadata = GetRequiredMetadataRegistration(
            registrations,
            "method",
            registration =>
                string.Equals(registration.DeclaringTypeSubjectId, harnessTypeRegistration.SubjectId, StringComparison.Ordinal) &&
                string.Equals(registration.Name, "Capture", StringComparison.Ordinal) &&
                registration.ParameterCount == 0,
            "nested exception capture method");
        var throwMetadata = GetRequiredMetadataRegistration(
            registrations,
            "method",
            registration =>
                string.Equals(registration.DeclaringTypeSubjectId, harnessTypeRegistration.SubjectId, StringComparison.Ordinal) &&
                string.Equals(registration.Name, "ThrowInner", StringComparison.Ordinal) &&
                registration.ParameterCount == 0,
            "nested exception throw method");

        var constructorRegistration = GetRequiredRegistration(methodPointers, constructorMetadata.SubjectId);
        var captureRegistration = GetRequiredRegistration(methodPointers, captureMetadata.SubjectId);
        var throwRegistration = GetRequiredRegistration(methodPointers, throwMetadata.SubjectId);

        const string expectedOutput = "Nested EH native proof: inner caught.";
        const string outerFinallyLiteral = "Nested EH outer finally.";

        loweringPlan = new NativeReferenceLoweringPlanArtifact
        {
            PlanKind = NativeReferenceProofCatalog.Stringify(NativeReferencePlanKind.NestedExceptionThrowCatchFinallyMinimal),
            AssemblyName = assemblyName,
            EntrySubjectId = subjectId,
            IncludeHeader = "codegen_bridge.h",
            NativeEntryFunctionName = "RunNativeReference",
            EntrySymbol = entryPointRegistration.Symbol,
            ConstructorSymbol = constructorRegistration.Symbol,
            InstanceMethodSymbol = captureRegistration.Symbol,
            ThrowMethodSymbol = throwRegistration.Symbol,
            ReferenceTypeToken = FormatCppTokenLiteral(GetRequiredMetadataToken(metadataRegistration, "type", harnessTypeRegistration.SubjectId)),
            CapturedFieldToken = "0u",
            EntryMethodToken = FormatCppTokenLiteral(GetRequiredMetadataToken(metadataRegistration, "method", entryPointRegistration.SubjectId)),
            ConstructorMethodToken = FormatCppTokenLiteral(GetRequiredMetadataToken(metadataRegistration, "method", constructorMetadata.SubjectId)),
            InstanceMethodToken = FormatCppTokenLiteral(GetRequiredMetadataToken(metadataRegistration, "method", captureMetadata.SubjectId)),
            ThrowMethodToken = FormatCppTokenLiteral(GetRequiredMetadataToken(metadataRegistration, "method", throwMetadata.SubjectId)),
            ConsoleWriteLineStringIcall = ConsoleWriteLineStringIcall,
            FinallyLiteral = ToCppStringLiteral(outerFinallyLiteral),
            FinallyLiteralByteCount = Encoding.UTF8.GetByteCount(outerFinallyLiteral),
            ExpectedOutput = ToCppStringLiteral(expectedOutput),
            ExpectedOutputByteCount = Encoding.UTF8.GetByteCount(expectedOutput),
        };

        return true;
    }







    private static bool TryBuildAssemblyBoundConsoleWriteLineStub(
        string subjectId,
        IReadOnlyDictionary<string, TypedIlMethodArtifact> methodsBySubjectId,
        string stubName,
        out string stub)
    {
        stub = string.Empty;
        if (!methodsBySubjectId.TryGetValue(subjectId, out var method))
        {
            return false;
        }

        if (!string.Equals(method.MethodRole, "static-method", StringComparison.Ordinal) ||
            !string.Equals(method.BodyAvailability, "has-canonical-body", StringComparison.Ordinal))
        {
            return false;
        }

        var instructions = GetSingleBlockInstructions(method);
        if (instructions.Count != 4 ||
            !string.Equals(instructions[0].Op, "ldstr", StringComparison.Ordinal) ||
            !string.Equals(instructions[1].Op, "call", StringComparison.Ordinal) ||
            !string.Equals(instructions[2].Op, "ldc.i4", StringComparison.Ordinal) ||
            !string.Equals(instructions[3].Op, "ret", StringComparison.Ordinal))
        {
            return false;
        }

        var writeLineTarget = GetRequiredInstructionCallee(instructions[1], method.SubjectId, 1);
        if (!IsConsoleWriteLineStringTarget(writeLineTarget))
        {
            return false;
        }

        if (GetRequiredOperandInt(instructions[2]) != 0)
        {
            return false;
        }

        var literal = GetRequiredOperandString(instructions[0]);
        var literalByteCount = Encoding.UTF8.GetByteCount(literal);
        var model = new ScriptObject
        {
            ["stub_name"] = stubName,
            ["console_write_line_string_icall_literal"] = ToCppStringLiteral(ConsoleWriteLineStringIcall),
            ["literal"] = ToCppStringLiteral(literal),
            ["literal_byte_count"] = literalByteCount,
        };
        stub = ScribanTemplateRenderer.RenderTemplate(
            ScribanTemplateRenderer.LoadTemplate(NativeReferenceProofCatalog.RuntimeSkeletonConsoleWriteLineStubTemplateRelativePath),
            model);
        return true;
    }







    private static bool TryBuildAssemblyBoundAsyncTaskFactoryStub(
        string assemblyName,
        string subjectId,
        MetadataRegistrationArtifact metadataRegistration,
        IReadOnlyDictionary<string, TypedIlMethodArtifact> methodsBySubjectId,
        string stubName,
        out string stub)
    {
        stub = string.Empty;
        if (!methodsBySubjectId.TryGetValue(subjectId, out var method))
        {
            return false;
        }

        if (!TryResolveAsyncTaskFactoryMethodShape(method, out var methodShape))
        {
            return false;
        }

        IReadOnlyList<TypedIlInstructionArtifact> instructions;
        try
        {
            instructions = GetSingleBlockInstructions(method);
            ValidateAsyncTaskFactoryShape(method, instructions, methodShape);
        }
        catch
        {
            return false;
        }

        var argc = methodShape.ParameterTypeShapes.Count;
        var model = new ScriptObject
        {
            ["stub_name"] = stubName,
            ["assembly_name_literal"] = ToCppStringLiteral(assemblyName),
            ["target_method_token"] = FormatCppTokenLiteral(GetRequiredMetadataToken(metadataRegistration, "method", subjectId)),
            ["argc"] = argc,
            ["managed_result_type"] = methodShape.Family.ManagedResultType,
            ["arg0_cpp_type"] = argc > 0 ? methodShape.ParameterTypeShapes[0].CppType : "void*",
            ["arg1_cpp_type"] = argc > 1 ? methodShape.ParameterTypeShapes[1].CppType : "void*",
        };
        stub = ScribanTemplateRenderer.RenderTemplate(
            ScribanTemplateRenderer.LoadTemplate(NativeReferenceProofCatalog.RuntimeSkeletonAsyncTaskFactoryStubTemplateRelativePath),
            model);
        return true;
    }







    private static bool TryBuildAssemblyBoundAsyncGetResultStub(
        string assemblyName,
        string subjectId,
        MetadataRegistrationArtifact metadataRegistration,
        IReadOnlyDictionary<string, TypedIlMethodArtifact> methodsBySubjectId,
        string stubName,
        out string stub)
    {
        stub = string.Empty;
        if (!methodsBySubjectId.TryGetValue(subjectId, out var method))
        {
            return false;
        }

        IReadOnlyList<TypedIlInstructionArtifact> instructions;
        AsyncGetResultWrapperShape wrapperShape;
        try
        {
            instructions = GetSingleBlockInstructions(method);
            wrapperShape = ValidateAsyncGetResultShape(method, instructions, methodsBySubjectId);
        }
        catch
        {
            return false;
        }

        var model = new ScriptObject
        {
            ["stub_name"] = stubName,
            ["assembly_name_literal"] = ToCppStringLiteral(assemblyName),
            ["target_method_token"] = FormatCppTokenLiteral(GetRequiredMetadataToken(metadataRegistration, "method", subjectId)),
            ["managed_result_type"] = wrapperShape.ProducerFamilyShape.ManagedResultType,
            ["managed_result_cpp_type"] = wrapperShape.ProducerFamilyShape.ResultTypeShape.CppType,
            ["get_result_runtime_return_initializer"] = wrapperShape.ProducerFamilyShape.ResultTypeShape.RuntimeReturnInitializer,
        };
        stub = ScribanTemplateRenderer.RenderTemplate(
            ScribanTemplateRenderer.LoadTemplate(NativeReferenceProofCatalog.RuntimeSkeletonAsyncGetResultStubTemplateRelativePath),
            model);
        return true;
    }







    private static bool TryBuildAssemblyBoundAsyncStateMachineNoOpStub(
        string subjectId,
        IReadOnlyDictionary<string, TypedIlMethodArtifact> methodsBySubjectId,
        string stubName,
        out string stub)
    {
        stub = string.Empty;
        if (!methodsBySubjectId.TryGetValue(subjectId, out var method) ||
            !IsCompilerGeneratedAsyncStateMachineMethodSubjectId(subjectId))
        {
            return false;
        }

        IReadOnlyList<TypedIlInstructionArtifact> instructions;
        try
        {
            instructions = GetSingleBlockInstructions(method);
            switch (GetMethodName(subjectId))
            {
                case "MoveNext":
                    ValidateAsyncStateMachineMoveNextShape(method, instructions);
                    break;

                case "SetStateMachine":
                    ValidateAsyncStateMachineSetStateMachineShape(method, instructions);
                    break;

                default:
                    return false;
            }
        }
        catch
        {
            return false;
        }

        var model = new ScriptObject
        {
            ["stub_name"] = stubName,
        };
        stub = ScribanTemplateRenderer.RenderTemplate(
            ScribanTemplateRenderer.LoadTemplate(NativeReferenceProofCatalog.RuntimeSkeletonAsyncStateMachineNoOpStubTemplateRelativePath),
            model);
        return true;
    }







    private static bool TryBuildAssemblyBoundStaticVoidObjectSinkNoOpStub(
        string subjectId,
        IReadOnlyDictionary<string, TypedIlMethodArtifact> methodsBySubjectId,
        string stubName,
        out string stub)
    {
        stub = string.Empty;
        if (!methodsBySubjectId.TryGetValue(subjectId, out var method))
        {
            return false;
        }

        if (!string.Equals(method.MethodRole, "static-method", StringComparison.Ordinal) ||
            !string.Equals(method.BodyAvailability, "has-canonical-body", StringComparison.Ordinal) ||
            !string.Equals(GetMethodReturnType(method.SubjectId), "System.Void", StringComparison.Ordinal) ||
            method.Parameters.Count != 1 ||
            !string.Equals(method.Parameters[0].Type, "System.Object", StringComparison.Ordinal))
        {
            return false;
        }

        IReadOnlyList<TypedIlInstructionArtifact> instructions;
        try
        {
            instructions = GetSingleBlockInstructions(method);
        }
        catch
        {
            return false;
        }

        if (instructions.Count != 1 || !string.Equals(instructions[0].Op, "ret", StringComparison.Ordinal))
        {
            return false;
        }

        var model = new ScriptObject
        {
            ["stub_name"] = stubName,
        };
        stub = ScribanTemplateRenderer.RenderTemplate(
            ScribanTemplateRenderer.LoadTemplate(NativeReferenceProofCatalog.RuntimeSkeletonStaticVoidObjectSinkNoOpStubTemplateRelativePath),
            model);
        return true;
    }







    private static bool TryBuildAssemblyBoundStaticExceptionThrowLiteralStub(
        string assemblyName,
        string subjectId,
        MetadataRegistrationArtifact metadataRegistration,
        IReadOnlyDictionary<string, TypedIlMethodArtifact> methodsBySubjectId,
        string stubName,
        out string stub)
    {
        stub = string.Empty;
        if (!methodsBySubjectId.TryGetValue(subjectId, out var method))
        {
            return false;
        }

        IReadOnlyList<TypedIlInstructionArtifact> instructions;
        try
        {
            instructions = GetSingleBlockInstructions(method);
            ValidateStaticExceptionThrowLiteralShape(method, instructions);
        }
        catch
        {
            return false;
        }

        var exceptionLiteral = GetRequiredOperandString(instructions[0]);
        var exceptionConstructorSubjectId = GetRequiredInstructionCallee(instructions[1], method.SubjectId, 1);
        var exceptionTypeSubjectId = GetDeclaringTypeSubjectId(exceptionConstructorSubjectId);
        var model = new ScriptObject
        {
            ["stub_name"] = stubName,
            ["assembly_name_literal"] = ToCppStringLiteral(assemblyName),
            ["exception_type_token"] = CreateTypeTokenLiteral(metadataRegistration, exceptionTypeSubjectId),
            ["exception_literal"] = ToCppStringLiteral(exceptionLiteral),
            ["exception_literal_byte_count"] = Encoding.UTF8.GetByteCount(exceptionLiteral),
        };
        stub = ScribanTemplateRenderer.RenderTemplate(
            ScribanTemplateRenderer.LoadTemplate(NativeReferenceProofCatalog.RuntimeSkeletonStaticExceptionThrowLiteralStubTemplateRelativePath),
            model);
        return true;
    }







    private static bool TryBuildAssemblyBoundStaticExceptionThrowStringProducerStub(
        string assemblyName,
        string subjectId,
        MetadataRegistrationArtifact metadataRegistration,
        IReadOnlyDictionary<string, TypedIlMethodArtifact> methodsBySubjectId,
        IReadOnlyDictionary<string, string> methodStubNamesBySubjectId,
        string stubName,
        out string stub)
    {
        stub = string.Empty;
        if (!methodsBySubjectId.TryGetValue(subjectId, out var method))
        {
            return false;
        }

        IReadOnlyList<TypedIlInstructionArtifact> instructions;
        try
        {
            instructions = GetSingleBlockInstructions(method);
            ValidateStaticExceptionThrowStringProducerShape(method, instructions);
        }
        catch
        {
            return false;
        }

        var producerSubjectId = GetRequiredInstructionCallee(instructions[0], method.SubjectId, 0);
        if (string.Equals(producerSubjectId, subjectId, StringComparison.Ordinal) ||
            !methodStubNamesBySubjectId.TryGetValue(producerSubjectId, out var producerStubName))
        {
            return false;
        }

        var exceptionConstructorSubjectId = GetRequiredInstructionCallee(instructions[1], method.SubjectId, 1);
        var exceptionTypeSubjectId = GetDeclaringTypeSubjectId(exceptionConstructorSubjectId);
        var model = new ScriptObject
        {
            ["stub_name"] = stubName,
            ["assembly_name_literal"] = ToCppStringLiteral(assemblyName),
            ["producer_stub_name"] = producerStubName,
            ["exception_type_token"] = CreateTypeTokenLiteral(metadataRegistration, exceptionTypeSubjectId),
        };
        stub = ScribanTemplateRenderer.RenderTemplate(
            ScribanTemplateRenderer.LoadTemplate(NativeReferenceProofCatalog.RuntimeSkeletonStaticExceptionThrowStringProducerStubTemplateRelativePath),
            model);
        return true;
    }







    private static bool TryBuildAssemblyBoundStaticExceptionCatchStringReturnStub(
        string subjectId,
        IReadOnlyDictionary<string, TypedIlMethodArtifact> methodsBySubjectId,
        IReadOnlyDictionary<string, string> methodStubNamesBySubjectId,
        string stubName,
        out string stub)
    {
        stub = string.Empty;
        if (!methodsBySubjectId.TryGetValue(subjectId, out var method))
        {
            return false;
        }

        IReadOnlyList<TypedIlInstructionArtifact> instructions;
        try
        {
            instructions = GetSingleBlockInstructions(method);
            ValidateStaticExceptionCatchStringReturnShape(method, instructions);
        }
        catch
        {
            return false;
        }

        var throwSubjectId = GetRequiredInstructionCallee(instructions[0], method.SubjectId, 0);
        if (string.Equals(throwSubjectId, subjectId, StringComparison.Ordinal) ||
            !methodStubNamesBySubjectId.TryGetValue(throwSubjectId, out var throwStubName))
        {
            return false;
        }

        var unreachableLiteral = GetRequiredOperandString(instructions[1]);
        var caughtLiteral = GetRequiredOperandString(instructions[5]);
        var model = new ScriptObject
        {
            ["stub_name"] = stubName,
            ["throw_stub_name"] = throwStubName,
            ["unreachable_literal"] = ToCppStringLiteral(unreachableLiteral),
            ["unreachable_literal_byte_count"] = Encoding.UTF8.GetByteCount(unreachableLiteral),
            ["caught_literal"] = ToCppStringLiteral(caughtLiteral),
            ["caught_literal_byte_count"] = Encoding.UTF8.GetByteCount(caughtLiteral),
        };
        stub = ScribanTemplateRenderer.RenderTemplate(
            ScribanTemplateRenderer.LoadTemplate(NativeReferenceProofCatalog.RuntimeSkeletonStaticExceptionCatchStringReturnStubTemplateRelativePath),
            model);
        return true;
    }







    private static bool TryBuildAssemblyBoundStaticIntForwarderStub(
        string subjectId,
        IReadOnlyDictionary<string, TypedIlMethodArtifact> methodsBySubjectId,
        IReadOnlyDictionary<string, string> methodStubNamesBySubjectId,
        string stubName,
        out string stub)
    {
        stub = string.Empty;
        if (!methodsBySubjectId.TryGetValue(subjectId, out var method))
        {
            return false;
        }

        if (!string.Equals(method.MethodRole, "static-method", StringComparison.Ordinal) ||
            !string.Equals(method.BodyAvailability, "has-canonical-body", StringComparison.Ordinal) ||
            !string.Equals(GetMethodReturnType(method.SubjectId), "System.Int32", StringComparison.Ordinal))
        {
            return false;
        }

        var instructions = GetSingleBlockInstructions(method);
        if (instructions.Count != 2 ||
            !string.Equals(instructions[0].Op, "call", StringComparison.Ordinal) ||
            !string.Equals(instructions[1].Op, "ret", StringComparison.Ordinal))
        {
            return false;
        }

        var targetSubjectId = GetRequiredInstructionCallee(instructions[0], method.SubjectId, 0);
        if (string.Equals(targetSubjectId, subjectId, StringComparison.Ordinal) ||
            !methodStubNamesBySubjectId.TryGetValue(targetSubjectId, out var targetStubName))
        {
            return false;
        }

        var model = new ScriptObject
        {
            ["stub_name"] = stubName,
            ["target_stub_name"] = targetStubName,
        };
        stub = ScribanTemplateRenderer.RenderTemplate(
            ScribanTemplateRenderer.LoadTemplate(NativeReferenceProofCatalog.RuntimeSkeletonStaticIntForwarderStubTemplateRelativePath),
            model);
        return true;
    }

}
