from tests.unit.compatibility.il2cpp_codegen_structure_governance_test_support import *


class TestIl2CppCodeGenStructureGovernanceReference(Il2CppCodeGenStructureGovernanceTestSupport):
    def test_native_reference_planner_and_emitter_share_catalog(self) -> None:
        planner_source = NATIVE_REFERENCE_PLANNER_PATH.read_text(encoding="utf-8")
        emitter_source = NATIVE_REFERENCE_EMITTER_PATH.read_text(encoding="utf-8")

        self.assertTrue(
            NATIVE_REFERENCE_CATALOG_PATH.is_file(),
            msg=f"missing NativeReference proof catalog: {NATIVE_REFERENCE_CATALOG_PATH}",
        )

        for source in [planner_source, emitter_source]:
            self.assertIn("NativeReferenceProofCatalog.", source)
            self.assertNotIn(
                'private const string ManagedAsyncAwaitIntMinimal = "managed-async.awaitable-int.minimal";',
                source,
            )
            self.assertNotIn(
                'private const string GeneratedTranslationUnitTemplateRelativePath = "Templates/NativeReferenceProof.cpp.scriban";',
                source,
            )

    def test_codegen_emitters_use_shared_scriban_renderer(self) -> None:
        planner_source = NATIVE_REFERENCE_PLANNER_PATH.read_text(encoding="utf-8")
        emitter_source = NATIVE_REFERENCE_EMITTER_PATH.read_text(encoding="utf-8")
        native_aot_emitter_source = NATIVE_AOT_EMITTER_PATH.read_text(encoding="utf-8")
        catalog_source = NATIVE_REFERENCE_CATALOG_PATH.read_text(encoding="utf-8")
        runtime_prelude_source = NATIVE_AOT_RUNTIME_PRELUDE_PATH.read_text(encoding="utf-8")
        scriban_renderer_source = SCRIBAN_RENDERER_PATH.read_text(encoding="utf-8")

        self.assertTrue(
            SCRIBAN_RENDERER_PATH.is_file(),
            msg=f"missing shared Scriban renderer: {SCRIBAN_RENDERER_PATH}",
        )

        self.assertIn("NativeReferenceProofCatalog.", planner_source)
        self.assertIn("ScribanTemplateRenderer", catalog_source)

        for source in [emitter_source, native_aot_emitter_source]:
            self.assertIn("ScribanTemplateRenderer", source)
            self.assertNotIn("private static Template LoadTemplate(", source)

        self.assertNotIn("private static Template LoadTemplate(", planner_source)
        self.assertNotIn("private static string RenderTemplate(", emitter_source)
        self.assertNotIn("private static string RenderTemplate(", native_aot_emitter_source)
        self.assertIn("ScribanTemplateRenderer", runtime_prelude_source)
        self.assertIn("NativeAotTemplateCatalog.", runtime_prelude_source)
        self.assertNotIn("private static string RenderTemplate(", runtime_prelude_source)
        self.assertIn("private const int TemplateLoopLimit = 100_000;", scriban_renderer_source)
        self.assertIn("context.LoopLimit = TemplateLoopLimit;", scriban_renderer_source)

    def test_full_closure_audit_and_runtime_skeleton_outputs_are_backed_by_scriban_templates(self) -> None:
        emitter_source = NATIVE_REFERENCE_EMITTER_PATH.read_text(encoding="utf-8")
        native_aot_emitter_source = NATIVE_AOT_EMITTER_PATH.read_text(encoding="utf-8")
        catalog_source = NATIVE_REFERENCE_CATALOG_PATH.read_text(encoding="utf-8")
        shared_audit_emitter_source = ASSEMBLY_FULL_CLOSURE_AUDIT_EMITTER_PATH.read_text(encoding="utf-8")
        runtime_skeleton_page_template_source = NATIVE_REFERENCE_RUNTIME_SKELETON_PAGE_TEMPLATE_PATH.read_text(encoding="utf-8")

        for required_path in [
            NATIVE_REFERENCE_AUDIT_SUMMARY_TEMPLATE_PATH,
            ASSEMBLY_FULL_CLOSURE_AUDIT_EMITTER_PATH,
            NATIVE_REFERENCE_RUNTIME_SKELETON_SUMMARY_TEMPLATE_PATH,
            NATIVE_REFERENCE_RUNTIME_SKELETON_PAGE_TEMPLATE_PATH,
            NATIVE_REFERENCE_RUNTIME_SKELETON_CONSOLE_WRITE_LINE_STUB_TEMPLATE_PATH,
            NATIVE_REFERENCE_RUNTIME_SKELETON_MARSHALING_UTF8_EXPORT_STUB_TEMPLATE_PATH,
            NATIVE_REFERENCE_RUNTIME_SKELETON_PINVOKE_DIRECT_CALL_STUB_TEMPLATE_PATH,
            NATIVE_REFERENCE_RUNTIME_SKELETON_STATIC_CALL_CTOR_GETTER_STUB_TEMPLATE_PATH,
            NATIVE_REFERENCE_RUNTIME_SKELETON_CONSTRUCTOR_THEN_INSTANCE_CALL_STUB_TEMPLATE_PATH,
            NATIVE_REFERENCE_RUNTIME_SKELETON_DELEGATE_CLOSED_TARGET_RELAY_STUB_TEMPLATE_PATH,
            NATIVE_REFERENCE_RUNTIME_SKELETON_INTERFACE_DISPATCH_MESSAGE_STUB_TEMPLATE_PATH,
            NATIVE_REFERENCE_RUNTIME_SKELETON_REFLECTION_INTEROP_CLOSURE_STUB_TEMPLATE_PATH,
            NATIVE_REFERENCE_RUNTIME_SKELETON_EXCEPTION_THROW_CATCH_FINALLY_STUB_TEMPLATE_PATH,
            NATIVE_REFERENCE_RUNTIME_SKELETON_NESTED_EXCEPTION_THROW_CATCH_FINALLY_STUB_TEMPLATE_PATH,
            NATIVE_REFERENCE_RUNTIME_SKELETON_ARRAY_BOXING_REFERENCE_ARRAY_STUB_TEMPLATE_PATH,
        ]:
            self.assertTrue(required_path.is_file(), msg=f"missing Scriban template: {required_path}")

        for required_fragment in [
            "AssemblyFullClosureRuntimeSkeletonSummaryTemplateRelativePath",
            "AssemblyFullClosureRuntimeSkeletonPageTemplateRelativePath",
            "GetAssemblyFullClosureRuntimeSkeletonSummaryTemplate(",
            "GetAssemblyFullClosureRuntimeSkeletonPageTemplate(",
            "RuntimeSkeletonConsoleWriteLineStubTemplateRelativePath",
            "RuntimeSkeletonMarshalingUtf8ExportStubTemplateRelativePath",
            "RuntimeSkeletonPInvokeDirectCallStubTemplateRelativePath",
            "GetRuntimeSkeletonConsoleWriteLineStubTemplate(",
            "GetRuntimeSkeletonMarshalingUtf8ExportStubTemplate(",
            "GetRuntimeSkeletonPInvokeDirectCallStubTemplate(",
            "RuntimeSkeletonStaticCallCtorGetterStubTemplateRelativePath",
            "RuntimeSkeletonConstructorThenInstanceCallStubTemplateRelativePath",
            "RuntimeSkeletonDelegateClosedTargetRelayStubTemplateRelativePath",
            "RuntimeSkeletonInterfaceDispatchMessageStubTemplateRelativePath",
            "RuntimeSkeletonReflectionInteropClosureStubTemplateRelativePath",
            "RuntimeSkeletonExceptionThrowCatchFinallyStubTemplateRelativePath",
            "RuntimeSkeletonNestedExceptionThrowCatchFinallyStubTemplateRelativePath",
            "RuntimeSkeletonArrayBoxingReferenceArrayStubTemplateRelativePath",
            "GetRuntimeSkeletonStaticCallCtorGetterStubTemplate(",
            "GetRuntimeSkeletonConstructorThenInstanceCallStubTemplate(",
            "GetRuntimeSkeletonDelegateClosedTargetRelayStubTemplate(",
            "GetRuntimeSkeletonInterfaceDispatchMessageStubTemplate(",
            "GetRuntimeSkeletonReflectionInteropClosureStubTemplate(",
            "GetRuntimeSkeletonExceptionThrowCatchFinallyStubTemplate(",
            "GetRuntimeSkeletonNestedExceptionThrowCatchFinallyStubTemplate(",
            "GetRuntimeSkeletonArrayBoxingReferenceArrayStubTemplate(",
        ]:
            self.assertIn(required_fragment, catalog_source)

        for required_fragment in [
            'private const string SummaryTemplateRelativePath = "Templates/AssemblyFullClosureAuditSummary.cpp.scriban";',
            "AssemblyFullClosureAuditPageManifestArtifact",
            "BuildGeneratedFiles(",
            "BuildSummaryTranslationUnit(",
            "BuildPageManifest(",
            "ScribanTemplateRenderer.LoadTemplate(",
            "JsonSerializer.Serialize(",
        ]:
            self.assertIn(required_fragment, shared_audit_emitter_source)

        for required_fragment in [
            "AssemblyFullClosureAuditEmitter.BuildGeneratedFiles(",
            "AssemblyFullClosureAuditEmitter.ValidatePlan(",
            "NativeReferenceProofCatalog.GetAssemblyFullClosureRuntimeSkeletonSummaryTemplate()",
            "NativeReferenceProofCatalog.GetAssemblyFullClosureRuntimeSkeletonPageTemplate()",
            "NativeReferenceProofCatalog.GetRuntimeSkeletonConsoleWriteLineStubTemplate()",
            "NativeReferenceProofCatalog.GetRuntimeSkeletonMarshalingUtf8ExportStubTemplate()",
            "NativeReferenceProofCatalog.GetRuntimeSkeletonPInvokeDirectCallStubTemplate()",
            "NativeReferenceProofCatalog.GetRuntimeSkeletonStaticCallCtorGetterStubTemplate()",
            "NativeReferenceProofCatalog.GetRuntimeSkeletonConstructorThenInstanceCallStubTemplate()",
            "NativeReferenceProofCatalog.GetRuntimeSkeletonDelegateClosedTargetRelayStubTemplate()",
            "NativeReferenceProofCatalog.GetRuntimeSkeletonInterfaceDispatchMessageStubTemplate()",
            "NativeReferenceProofCatalog.GetRuntimeSkeletonReflectionInteropClosureStubTemplate()",
            "NativeReferenceProofCatalog.GetRuntimeSkeletonExceptionThrowCatchFinallyStubTemplate()",
            "NativeReferenceProofCatalog.GetRuntimeSkeletonNestedExceptionThrowCatchFinallyStubTemplate()",
            "NativeReferenceProofCatalog.GetRuntimeSkeletonArrayBoxingReferenceArrayStubTemplate()",
            "BuildAssemblyFullClosureRuntimeSkeletonCoverageReport(",
            "TryBuildAssemblyFullClosureRuntimeSkeletonMethodStub(",
            "runtimeSkeletonCoverageReport",
            "RuntimeSkeletonUnsupportedMethodEmission",
        ]:
            self.assertIn(required_fragment, emitter_source)

        for required_fragment in [
            "ResolveRuntimeSkeletonFieldBinding(",
            "ResolveRuntimeSkeletonType(",
            "TryGetRuntimeSkeletonAbi(",
            "EnsureRuntimeSkeletonClassInitialized(",
        ]:
            self.assertIn(required_fragment, runtime_skeleton_page_template_source)

        for required_fragment in [
            "AssemblyFullClosureAuditEmitter.BuildGeneratedFiles(",
            "AssemblyFullClosureAuditEmitter.ValidatePlan(",
        ]:
            self.assertIn(required_fragment, native_aot_emitter_source)

    def test_runtime_skeleton_emitter_precomputes_global_indexes_outside_page_loop(self) -> None:
        emitter_source = NATIVE_REFERENCE_EMITTER_PATH.read_text(encoding="utf-8")

        for required_fragment in [
            "var methods = typedIl.Methods ?? [];",
            "var methodsBySubjectId = methods",
            "var methodPointers = codeRegistration.Modules",
            "_ = GetCodeRegistrationLookup(methodPointers);",
            "_ = GetMetadataRegistrationLookup(metadataRegistration.Registrations);",
            "var requestedMethodSubjectIds = loweringPlan.TranslationUnitMethodSubjectIds ?? [];",
            "var translationUnitPageSize = loweringPlan.TranslationUnitPageSize ?? AuditTranslationUnitPageSize;",
            "BuildAssemblyFullClosureRuntimeSkeletonPageTranslationUnit(",
            "IReadOnlyList<TypedIlMethodArtifact> methods,",
            "IReadOnlyDictionary<string, TypedIlMethodArtifact> methodsBySubjectId",
            "IReadOnlyList<CodeRegistrationEntry> methodPointers",
            "IReadOnlyList<string> requestedMethodSubjectIds,",
            "int pageStartIndex,",
            "int pageItemCount)",
        ]:
            self.assertIn(required_fragment, emitter_source)

        page_builder_index = emitter_source.index("private static RuntimeSkeletonPageEmission BuildAssemblyFullClosureRuntimeSkeletonPageTranslationUnit(")
        page_builder_body = emitter_source[page_builder_index: page_builder_index + 1600]
        self.assertNotIn("var methodsBySubjectId = (typedIl.Methods ?? [])", page_builder_body)
        self.assertNotIn("var methodPointers = codeRegistration.Modules", page_builder_body)
        self.assertNotIn(".Skip((page.PageNumber - 1)", emitter_source)
        self.assertNotIn(".Take(page.MethodCount)", emitter_source)
        self.assertIn("ConditionalWeakTable<IReadOnlyList<CodeRegistrationEntry>, IReadOnlyDictionary<string, CodeRegistrationEntry>>", emitter_source)
        self.assertIn("ConditionalWeakTable<IReadOnlyList<MetadataRegistrationEntry>, MetadataRegistrationLookup>", emitter_source)
        self.assertIn("GetMetadataRegistrationsForKind(", emitter_source)

    def test_runtime_skeleton_emitter_streams_pages_and_coverage_before_summary(self) -> None:
        emitter_source = NATIVE_REFERENCE_EMITTER_PATH.read_text(encoding="utf-8")

        for required_fragment in [
            "BuildAssemblyFullClosureRuntimeSkeletonGeneratedSources(",
            "string outputRootPath,",
            "WriteGeneratedSource(",
            "RelativePath = pageEmission.Page.Path,",
            "pageEmissionsByIndex[pageIndex] = new RuntimeSkeletonPageEmission(",
            "string.Empty,",
            "RelativePath = coverageReportRelativePath,",
            "RelativePath = summaryRelativePath,",
            "private static void WriteGeneratedSource(",
            "File.WriteAllText(targetPath, generatedSource.Contents);",
        ]:
            self.assertIn(required_fragment, emitter_source)

        self.assertNotIn("generatedSources.AddRange(pageEmissions.Select", emitter_source)

    def test_runtime_skeleton_emitter_reuses_precomputed_methods_list_in_try_builders(self) -> None:
        emitter_source = NATIVE_REFERENCE_EMITTER_PATH.read_text(encoding="utf-8")

        for required_fragment in [
            "TryBuildAssemblyFullClosureRuntimeSkeletonMethodStub(",
            "IReadOnlyList<TypedIlMethodArtifact> methods,",
            "TryBuildAssemblyBoundPInvokeDirectCallPlan(",
            "TryBuildAssemblyBoundArrayBoxingReferenceArrayPlan(",
            "TryBuildAssemblyBoundInterfaceDispatchMessagePlan(",
            "TryBuildAssemblyFullClosureExecutableLoweringPlan(",
        ]:
            self.assertIn(required_fragment, emitter_source)

        self.assertNotIn("methodsBySubjectId.Values.ToList()", emitter_source)

    def test_runtime_skeleton_emitter_uses_bounded_parallel_page_execution(self) -> None:
        emitter_source = NATIVE_REFERENCE_EMITTER_PATH.read_text(encoding="utf-8")

        for required_fragment in [
            "Parallel.For(",
            "MaxDegreeOfParallelism = GetRuntimeSkeletonPageParallelism(requestedPages.Length)",
            "var pageEmissionsByIndex = new RuntimeSkeletonPageEmission?[requestedPages.Length];",
            "var unsupportedMethodsByIndex = new IReadOnlyList<RuntimeSkeletonUnsupportedMethodEmission>[requestedPages.Length];",
            "private static int GetRuntimeSkeletonPageParallelism(int pageCount)",
            "var recommendedParallelism = Math.Max(1, Environment.ProcessorCount - 1);",
            "return Math.Min(pageCount, recommendedParallelism);",
        ]:
            self.assertIn(required_fragment, emitter_source)

    def test_runtime_skeleton_emitter_has_4c_foundation_dispatcher_skeleton(self) -> None:
        emitter_source = NATIVE_REFERENCE_EMITTER_PATH.read_text(encoding="utf-8")

        for required_fragment in [
            "private sealed record RuntimeSkeletonStubBuildContext(",
            "private enum RuntimeSkeletonFamilyHandlerMatchKind",
            "private sealed record RuntimeSkeletonFamilyHandlerResult(",
            "private delegate RuntimeSkeletonFamilyHandlerResult RuntimeSkeletonFamilyHandler(",
            "private static readonly RuntimeSkeletonFamilyHandler[] RuntimeSkeletonFamilyHandlers =",
            "TryBuildRuntimeSkeletonConvertFamilyHandler",
            "TryBuildRuntimeSkeletonConvertLikeFamilyHandler",
            "TryBuildAssemblyFullClosureRuntimeSkeletonMethodStubVia4CDispatcher(",
            "var buildContext = new RuntimeSkeletonStubBuildContext(",
        ]:
            self.assertIn(required_fragment, emitter_source)

        self.assertLess(
            emitter_source.index("TryBuildRuntimeSkeletonConvertFamilyHandler"),
            emitter_source.index("TryBuildRuntimeSkeletonConvertLikeFamilyHandler"),
        )

    def test_runtime_skeleton_family_handler_result_supports_unsupported_reason_contract(self) -> None:
        emitter_source = NATIVE_REFERENCE_EMITTER_PATH.read_text(encoding="utf-8")

        for required_fragment in [
            "private enum RuntimeSkeletonFamilyHandlerMatchKind",
            "Unsupported,",
            "private sealed record RuntimeSkeletonFamilyHandlerResult(",
            "string? StubDefinition,",
            "string? UnsupportedReason)",
            "new(RuntimeSkeletonFamilyHandlerMatchKind.NoMatch, null, null);",
            "new(RuntimeSkeletonFamilyHandlerMatchKind.Match, stubDefinition, null);",
            "public static RuntimeSkeletonFamilyHandlerResult CreateUnsupported(string unsupportedReason) =>",
            "new(RuntimeSkeletonFamilyHandlerMatchKind.Unsupported, null, unsupportedReason);",
            '"convert-family-owned-unsupported-shape"',
            '"exception-family-owned-unsupported-shape"',
        ]:
            self.assertIn(required_fragment, emitter_source)

        dispatcher_start = emitter_source.index(
            "private static string? TryBuildAssemblyFullClosureRuntimeSkeletonMethodStubVia4CDispatcher(",
        )
        convert_family_start = emitter_source.index(
            "private static RuntimeSkeletonFamilyHandlerResult TryBuildRuntimeSkeletonConvertFamilyHandler(",
        )
        dispatcher_slice = emitter_source[dispatcher_start:convert_family_start]

        self.assertIn(
            "if (result.MatchKind == RuntimeSkeletonFamilyHandlerMatchKind.Match)",
            dispatcher_slice,
        )
        self.assertNotIn(
            "if (result.MatchKind == RuntimeSkeletonFamilyHandlerMatchKind.Unsupported)",
            dispatcher_slice,
        )

    def test_runtime_skeleton_convert_family_has_internal_handler_ordering(self) -> None:
        emitter_source = NATIVE_REFERENCE_EMITTER_PATH.read_text(encoding="utf-8")

        for required_fragment in [
            "private delegate RuntimeSkeletonFamilyHandlerResult RuntimeSkeletonConvertFamilyHandler(",
            "private static readonly RuntimeSkeletonConvertFamilyHandler[] RuntimeSkeletonConvertFamilyHandlers =",
            "TryBuildRuntimeSkeletonConvertIntForwarderHandler",
            "TryBuildRuntimeSkeletonConvertBoolIdentityForwarderHandler",
            "TryBuildRuntimeSkeletonConvertBoolProducerForwarderHandler",
            "TryBuildRuntimeSkeletonConvertPrimitiveHandler",
            "TryBuildRuntimeSkeletonConvertCheckedByteHandler",
            "TryBuildRuntimeSkeletonConvertByteForwarderHandler",
            "foreach (var convertFamilyHandler in RuntimeSkeletonConvertFamilyHandlers)",
        ]:
            self.assertIn(required_fragment, emitter_source)

        self.assertNotIn("TryBuildRuntimeSkeletonConvertLegacyHandler", emitter_source)
        self.assertLess(
            emitter_source.index("TryBuildRuntimeSkeletonConvertPrimitiveHandler"),
            emitter_source.index("TryBuildRuntimeSkeletonConvertByteForwarderHandler"),
        )

    def test_runtime_skeleton_exception_family_has_internal_handler_ordering(self) -> None:
        emitter_source = NATIVE_REFERENCE_EMITTER_PATH.read_text(encoding="utf-8")

        for required_fragment in [
            "TryBuildRuntimeSkeletonExceptionFamilyHandler",
            "private delegate RuntimeSkeletonFamilyHandlerResult RuntimeSkeletonExceptionFamilyHandler(",
            "private static readonly RuntimeSkeletonExceptionFamilyHandler[] RuntimeSkeletonExceptionFamilyHandlers =",
            "TryBuildRuntimeSkeletonStaticExceptionThrowLiteralHandler",
            "TryBuildRuntimeSkeletonStaticExceptionThrowStringProducerHandler",
            "TryBuildRuntimeSkeletonStaticExceptionCatchStringReturnHandler",
            "TryBuildRuntimeSkeletonExceptionThrowCatchFinallyHandler",
            "TryBuildRuntimeSkeletonNestedExceptionThrowCatchFinallyHandler",
            "foreach (var exceptionFamilyHandler in RuntimeSkeletonExceptionFamilyHandlers)",
        ]:
            self.assertIn(required_fragment, emitter_source)

        self.assertNotIn("TryBuildRuntimeSkeletonExceptionLegacyHandler", emitter_source)
        self.assertLess(
            emitter_source.index("TryBuildRuntimeSkeletonStaticExceptionThrowLiteralHandler"),
            emitter_source.index("TryBuildRuntimeSkeletonNestedExceptionThrowCatchFinallyHandler"),
        )

    def test_runtime_skeleton_array_family_has_internal_handler_ordering(self) -> None:
        emitter_source = NATIVE_REFERENCE_EMITTER_PATH.read_text(encoding="utf-8")

        for required_fragment in [
            "TryBuildRuntimeSkeletonArrayFamilyHandler",
            "private delegate RuntimeSkeletonFamilyHandlerResult RuntimeSkeletonArrayFamilyHandler(",
            "private static readonly RuntimeSkeletonArrayFamilyHandler[] RuntimeSkeletonArrayFamilyHandlers =",
            "TryBuildRuntimeSkeletonArrayBoxingReferenceArrayHandler",
            "TryBuildRuntimeSkeletonArrayClearReferenceArrayHandler",
            "TryBuildRuntimeSkeletonArrayReverseReferenceArrayHandler",
            "TryBuildRuntimeSkeletonArrayCopyReferenceArrayHandler",
            "foreach (var arrayFamilyHandler in RuntimeSkeletonArrayFamilyHandlers)",
        ]:
            self.assertIn(required_fragment, emitter_source)

        self.assertNotIn("TryBuildRuntimeSkeletonArrayLegacyHandler", emitter_source)
        self.assertLess(
            emitter_source.index("TryBuildRuntimeSkeletonArrayBoxingReferenceArrayHandler"),
            emitter_source.index("TryBuildRuntimeSkeletonArrayCopyReferenceArrayHandler"),
        )

    def test_runtime_skeleton_async_family_has_internal_handler_ordering(self) -> None:
        emitter_source = NATIVE_REFERENCE_EMITTER_PATH.read_text(encoding="utf-8")

        for required_fragment in [
            "TryBuildRuntimeSkeletonAsyncFamilyHandler",
            "private delegate RuntimeSkeletonFamilyHandlerResult RuntimeSkeletonAsyncFamilyHandler(",
            "private static readonly RuntimeSkeletonAsyncFamilyHandler[] RuntimeSkeletonAsyncFamilyHandlers =",
            "TryBuildRuntimeSkeletonAsyncTaskFactoryHandler",
            "TryBuildRuntimeSkeletonAsyncGetResultHandler",
            "TryBuildRuntimeSkeletonAsyncStateMachineNoOpHandler",
            "foreach (var asyncFamilyHandler in RuntimeSkeletonAsyncFamilyHandlers)",
        ]:
            self.assertIn(required_fragment, emitter_source)

        self.assertNotIn("TryBuildRuntimeSkeletonAsyncLegacyHandler", emitter_source)
        self.assertLess(
            emitter_source.index("TryBuildRuntimeSkeletonAsyncTaskFactoryHandler"),
            emitter_source.index("TryBuildRuntimeSkeletonAsyncStateMachineNoOpHandler"),
        )

    def test_runtime_skeleton_interop_family_has_internal_handler_ordering(self) -> None:
        emitter_source = NATIVE_REFERENCE_EMITTER_PATH.read_text(encoding="utf-8")

        for required_fragment in [
            "TryBuildRuntimeSkeletonInteropFamilyHandler",
            "private delegate RuntimeSkeletonFamilyHandlerResult RuntimeSkeletonInteropFamilyHandler(",
            "private static readonly RuntimeSkeletonInteropFamilyHandler[] RuntimeSkeletonInteropFamilyHandlers =",
            "TryBuildRuntimeSkeletonMarshalingUtf8ExportHandler",
            "TryBuildRuntimeSkeletonReflectionInteropClosureHandler",
            "TryBuildRuntimeSkeletonPInvokeDirectCallHandler",
            "foreach (var interopFamilyHandler in RuntimeSkeletonInteropFamilyHandlers)",
        ]:
            self.assertIn(required_fragment, emitter_source)

        self.assertNotIn("TryBuildRuntimeSkeletonInteropLegacyHandler", emitter_source)
        self.assertLess(
            emitter_source.index("TryBuildRuntimeSkeletonMarshalingUtf8ExportHandler"),
            emitter_source.index("TryBuildRuntimeSkeletonPInvokeDirectCallHandler"),
        )

    def test_runtime_skeleton_binding_family_has_internal_handler_ordering(self) -> None:
        emitter_source = NATIVE_REFERENCE_EMITTER_PATH.read_text(encoding="utf-8")

        for required_fragment in [
            "TryBuildRuntimeSkeletonBindingFamilyHandler",
            "private delegate RuntimeSkeletonFamilyHandlerResult RuntimeSkeletonBindingFamilyHandler(",
            "private static readonly RuntimeSkeletonBindingFamilyHandler[] RuntimeSkeletonBindingFamilyHandlers =",
            "TryBuildRuntimeSkeletonConstructorFieldSetterHandler",
            "TryBuildRuntimeSkeletonFieldBackedStringReturnHandler",
            "TryBuildRuntimeSkeletonFieldArgumentStringReturnHandler",
            "TryBuildRuntimeSkeletonFieldGetterStringReturnHandler",
            "TryBuildRuntimeSkeletonDelegateClosedTargetRelayHandler",
            "foreach (var bindingFamilyHandler in RuntimeSkeletonBindingFamilyHandlers)",
        ]:
            self.assertIn(required_fragment, emitter_source)

        self.assertNotIn("TryBuildRuntimeSkeletonBindingLegacyHandler", emitter_source)
        self.assertLess(
            emitter_source.index("TryBuildRuntimeSkeletonConstructorFieldSetterHandler"),
            emitter_source.index("TryBuildRuntimeSkeletonDelegateClosedTargetRelayHandler"),
        )

    def test_runtime_skeleton_platform_family_has_internal_handler_ordering(self) -> None:
        emitter_source = NATIVE_REFERENCE_EMITTER_PATH.read_text(encoding="utf-8")

        for required_fragment in [
            "TryBuildRuntimeSkeletonPlatformFamilyHandler",
            "private delegate RuntimeSkeletonFamilyHandlerResult RuntimeSkeletonPlatformFamilyHandler(",
            "private static readonly RuntimeSkeletonPlatformFamilyHandler[] RuntimeSkeletonPlatformFamilyHandlers =",
            "TryBuildRuntimeSkeletonInterfaceDispatchMessageHandler",
            "TryBuildRuntimeSkeletonThreadingThreadStaticMonitorHandler",
            "foreach (var platformFamilyHandler in RuntimeSkeletonPlatformFamilyHandlers)",
        ]:
            self.assertIn(required_fragment, emitter_source)

        self.assertNotIn("TryBuildRuntimeSkeletonPlatformLegacyHandler", emitter_source)
        self.assertLess(
            emitter_source.index("TryBuildRuntimeSkeletonInterfaceDispatchMessageHandler"),
            emitter_source.index("TryBuildRuntimeSkeletonThreadingThreadStaticMonitorHandler"),
        )

    def test_runtime_skeleton_utility_family_has_internal_handler_ordering(self) -> None:
        emitter_source = NATIVE_REFERENCE_EMITTER_PATH.read_text(encoding="utf-8")

        for required_fragment in [
            "TryBuildRuntimeSkeletonUtilityFamilyHandler",
            "private delegate RuntimeSkeletonFamilyHandlerResult RuntimeSkeletonUtilityFamilyHandler(",
            "private static readonly RuntimeSkeletonUtilityFamilyHandler[] RuntimeSkeletonUtilityFamilyHandlers =",
            "TryBuildRuntimeSkeletonStaticVoidObjectSinkNoOpHandler",
            "TryBuildRuntimeSkeletonStaticBoolForwarderHandler",
            "TryBuildRuntimeSkeletonConsoleWriteLineHandler",
            "foreach (var utilityFamilyHandler in RuntimeSkeletonUtilityFamilyHandlers)",
        ]:
            self.assertIn(required_fragment, emitter_source)

        self.assertNotIn("TryBuildRuntimeSkeletonUtilityLegacyHandler", emitter_source)
        self.assertLess(
            emitter_source.index("TryBuildRuntimeSkeletonStaticVoidObjectSinkNoOpHandler"),
            emitter_source.index("TryBuildRuntimeSkeletonConsoleWriteLineHandler"),
        )

    def test_runtime_skeleton_string_family_has_internal_handler_ordering(self) -> None:
        emitter_source = NATIVE_REFERENCE_EMITTER_PATH.read_text(encoding="utf-8")

        for required_fragment in [
            "TryBuildRuntimeSkeletonStringFamilyHandler",
            "private delegate RuntimeSkeletonFamilyHandlerResult RuntimeSkeletonStringFamilyHandler(",
            "private static readonly RuntimeSkeletonStringFamilyHandler[] RuntimeSkeletonStringFamilyHandlers =",
            "TryBuildRuntimeSkeletonStaticStringReturnForwarderHandler",
            "TryBuildRuntimeSkeletonStaticStringForwarderHandler",
            "TryBuildRuntimeSkeletonStaticStringLiteralAppendHandler",
            "TryBuildRuntimeSkeletonStaticStringForwarderConsoleWriteLineHandler",
            "TryBuildRuntimeSkeletonStaticStringProducerForwarderConsoleWriteLineHandler",
            "TryBuildRuntimeSkeletonStaticStringProducerCtorGetterConsoleWriteLineHandler",
            "TryBuildRuntimeSkeletonStaticStringProducerCtorRenderConsoleWriteLineHandler",
            "TryBuildRuntimeSkeletonStaticStringProducerCtorInstanceCallForwarderConsoleWriteLineHandler",
            "TryBuildRuntimeSkeletonStaticStringProducerForwarderCtorInstanceCallConsoleWriteLineHandler",
            "TryBuildRuntimeSkeletonStaticStringProducerForwarderCtorInstanceCallForwarderConsoleWriteLineHandler",
            "TryBuildRuntimeSkeletonStaticStringProducerCtorInstanceCallForwarderForwarderConsoleWriteLineHandler",
            "TryBuildRuntimeSkeletonStaticStringProducerForwarderCtorInstanceCallForwarderForwarderConsoleWriteLineHandler",
            "TryBuildRuntimeSkeletonStaticStringProducerCtorInstanceCallForwarderForwarderForwarderConsoleWriteLineHandler",
            "TryBuildRuntimeSkeletonStaticStringProducerForwarderCtorInstanceCallForwarderForwarderForwarderConsoleWriteLineHandler",
            "TryBuildRuntimeSkeletonStaticLiteralStringReturnHandler",
            "TryBuildRuntimeSkeletonStaticResourceKeyStringReturnHandler",
            "TryBuildRuntimeSkeletonStaticStringProducerConsoleWriteLineHandler",
            "TryBuildRuntimeSkeletonStaticCallCtorGetterExecutableHandler",
            "TryBuildRuntimeSkeletonConstructorThenInstanceCallExecutableHandler",
            "foreach (var stringFamilyHandler in RuntimeSkeletonStringFamilyHandlers)",
        ]:
            self.assertIn(required_fragment, emitter_source)

        self.assertLess(
            emitter_source.index("TryBuildRuntimeSkeletonStaticStringReturnForwarderHandler"),
            emitter_source.index("TryBuildRuntimeSkeletonConstructorThenInstanceCallExecutableHandler"),
        )

    def test_runtime_skeleton_string_family_owns_string_executable_plan_lanes(self) -> None:
        emitter_source = NATIVE_REFERENCE_EMITTER_PATH.read_text(encoding="utf-8")

        self.assertIn("TryBuildRuntimeSkeletonStaticCallCtorGetterExecutableHandler", emitter_source)
        self.assertIn("TryBuildRuntimeSkeletonConstructorThenInstanceCallExecutableHandler", emitter_source)
        self.assertIn(
            'string.Equals(executableLoweringPlan.PlanKind, "staticCallCtorGetter", StringComparison.Ordinal)',
            emitter_source,
        )
        self.assertIn(
            'string.Equals(executableLoweringPlan.PlanKind, "constructorThenInstanceCall", StringComparison.Ordinal)',
            emitter_source,
        )

        string_family_handlers_start = emitter_source.index(
            "private static readonly RuntimeSkeletonStringFamilyHandler[] RuntimeSkeletonStringFamilyHandlers =",
        )
        string_family_handlers_end = emitter_source.index(
            "private static RuntimeSkeletonFamilyHandlerResult TryBuildRuntimeSkeletonConvertFamilyHandler(",
        )
        string_family_handlers_slice = emitter_source[string_family_handlers_start:string_family_handlers_end]

        self.assertIn("TryBuildRuntimeSkeletonStaticCallCtorGetterExecutableHandler", string_family_handlers_slice)
        self.assertIn("TryBuildRuntimeSkeletonConstructorThenInstanceCallExecutableHandler", string_family_handlers_slice)

        convert_like_family_start = emitter_source.index(
            "private static RuntimeSkeletonFamilyHandlerResult TryBuildRuntimeSkeletonConvertLikeFamilyHandler(",
        )
        convert_int_handler_start = emitter_source.index(
            "private static RuntimeSkeletonFamilyHandlerResult TryBuildRuntimeSkeletonConvertIntForwarderHandler(",
        )
        convert_like_family_slice = emitter_source[convert_like_family_start:convert_int_handler_start]

        self.assertNotIn(
            'string.Equals(executableLoweringPlan.PlanKind, "staticCallCtorGetter", StringComparison.Ordinal)',
            convert_like_family_slice,
        )
        self.assertNotIn(
            'string.Equals(executableLoweringPlan.PlanKind, "constructorThenInstanceCall", StringComparison.Ordinal)',
            convert_like_family_slice,
        )

    def test_runtime_skeleton_legacy_family_is_removed_after_convert_like_cutover(self) -> None:
        emitter_source = NATIVE_REFERENCE_EMITTER_PATH.read_text(encoding="utf-8")

        self.assertNotIn("TryBuildRuntimeSkeletonLegacyFamilyHandler", emitter_source)
        self.assertNotIn("TryBuildAssemblyFullClosureRuntimeSkeletonMethodStubLegacy(", emitter_source)

    def test_runtime_skeleton_convert_like_family_owns_generic_convert_fallback_lanes(self) -> None:
        emitter_source = NATIVE_REFERENCE_EMITTER_PATH.read_text(encoding="utf-8")

        family_handlers_start = emitter_source.index(
            "private static readonly RuntimeSkeletonFamilyHandler[] RuntimeSkeletonFamilyHandlers =",
        )
        convert_family_start = emitter_source.index(
            "private static RuntimeSkeletonFamilyHandlerResult TryBuildRuntimeSkeletonConvertFamilyHandler(",
        )
        convert_like_family_start = emitter_source.index(
            "private static RuntimeSkeletonFamilyHandlerResult TryBuildRuntimeSkeletonConvertLikeFamilyHandler(",
        )
        convert_int_handler_start = emitter_source.index(
            "private static RuntimeSkeletonFamilyHandlerResult TryBuildRuntimeSkeletonConvertIntForwarderHandler(",
        )

        family_handlers_slice = emitter_source[family_handlers_start:convert_family_start]
        convert_like_family_slice = emitter_source[convert_like_family_start:convert_int_handler_start]

        for required_fragment in [
            "TryBuildRuntimeSkeletonConvertLikeFamilyHandler",
            "foreach (var convertFamilyHandler in RuntimeSkeletonConvertFamilyHandlers)",
            "return RuntimeSkeletonFamilyHandlerResult.NoMatch;",
        ]:
            self.assertIn(required_fragment, emitter_source)

        self.assertIn("TryBuildRuntimeSkeletonConvertLikeFamilyHandler", family_handlers_slice)
        self.assertIn("foreach (var convertFamilyHandler in RuntimeSkeletonConvertFamilyHandlers)", convert_like_family_slice)

    def test_runtime_skeleton_convert_families_share_handler_loop_helper(self) -> None:
        emitter_source = NATIVE_REFERENCE_EMITTER_PATH.read_text(encoding="utf-8")

        for required_fragment in [
            "private static RuntimeSkeletonFamilyHandlerResult TryBuildRuntimeSkeletonConvertHandlers(",
            "RuntimeSkeletonFamilyHandlerResult missResult)",
            "foreach (var convertFamilyHandler in RuntimeSkeletonConvertFamilyHandlers)",
            "return missResult;",
            "return TryBuildRuntimeSkeletonConvertHandlers(",
            'RuntimeSkeletonFamilyHandlerResult.CreateUnsupported("convert-family-owned-unsupported-shape"));',
            "return TryBuildRuntimeSkeletonConvertHandlers(buildContext, RuntimeSkeletonFamilyHandlerResult.NoMatch);",
        ]:
            self.assertIn(required_fragment, emitter_source)

    def test_native_reference_planner_helpers_are_split_from_root_planner(self) -> None:
        planner_source = NATIVE_REFERENCE_PLANNER_PATH.read_text(encoding="utf-8")

        for required_path in [
            NATIVE_REFERENCE_ENGINE_AND_FAMILY_SELECTION_PATH,
            NATIVE_REFERENCE_PLAN_BUILDERS_PATH,
            NATIVE_REFERENCE_SHAPE_VALIDATION_PATH,
        ]:
            self.assertTrue(required_path.is_file(), msg=f"missing NativeReference split file: {required_path}")

        self.assertIn("public sealed partial class NativeReferenceLoweringPlanner", planner_source)
        self.assertIn("public NativeReferenceLoweringPlanArtifact Create(", planner_source)

        for required_fragment in [
            "private static bool TryCreateEngineLoweringPlan(",
            "private static string SelectLoweringFamily(",
            "private static NativeReferenceLoweringPlanArtifact CreateLoweringPlan(",
            "private static Template GetTemplateForPlan(",
            "private static IReadOnlyList<TypedIlInstructionArtifact> GetSingleBlockInstructions(",
            "private static string ValidateConstructorThenInstanceCallEntryPointShape(",
        ]:
            self.assertNotIn(required_fragment, planner_source)

        engine_and_family_selection_source = NATIVE_REFERENCE_ENGINE_AND_FAMILY_SELECTION_PATH.read_text(encoding="utf-8")
        for required_fragment in [
            "private static bool TryCreateEngineLoweringPlan(",
            "private static string SelectLoweringFamily(",
            "private static bool MatchesAsyncAwaitIntCandidate(",
            "private static bool MatchesInterfaceDispatchMessageCandidate(",
        ]:
            self.assertIn(required_fragment, engine_and_family_selection_source)

        plan_builders_source = NATIVE_REFERENCE_PLAN_BUILDERS_PATH.read_text(encoding="utf-8")
        for required_fragment in [
            "private static NativeReferenceLoweringPlanArtifact CreateLoweringPlan(",
            "private static bool TryCreateThreadingThreadStaticMonitorLoweringPlan(",
            "private static bool TryCreatePInvokeDllImportMinimalLoweringPlan(",
            "private static Template GetTemplateForPlan(",
            "private static ScriptObject CreateTemplateModel(",
        ]:
            self.assertIn(required_fragment, plan_builders_source)

        shape_validation_source = NATIVE_REFERENCE_SHAPE_VALIDATION_PATH.read_text(encoding="utf-8")
        for required_fragment in [
            "private static IReadOnlyList<TypedIlInstructionArtifact> GetSingleBlockInstructions(",
            "private static bool IsConstructorThenInstanceCallEntryPointShape(",
            "private static string ValidateConstructorThenInstanceCallEntryPointShape(",
            "private static MetadataRegistrationEntry GetRequiredMetadataRegistration(",
            "private static T LoadRequiredJson<T>(",
        ]:
            self.assertIn(required_fragment, shape_validation_source)

    def test_native_reference_shape_validation_removes_legacy_testframework_assert_shape(self) -> None:
        planner_source = NATIVE_REFERENCE_PLANNER_PATH.read_text(encoding="utf-8")
        plan_builders_source = NATIVE_REFERENCE_PLAN_BUILDERS_PATH.read_text(encoding="utf-8")
        shape_validation_source = NATIVE_REFERENCE_SHAPE_VALIDATION_PATH.read_text(encoding="utf-8")
        self.assertFalse(
            TEST_FRAMEWORK_SUBJECT_FAMILY_PATH.is_file(),
            msg=f"legacy shared TestFramework subject helper should be removed: {TEST_FRAMEWORK_SUBJECT_FAMILY_PATH}",
        )

        for source in [planner_source, shape_validation_source]:
            self.assertNotIn("LegacyTestFrameworkGenericEqualMethodSubjectPrefix", source)
            self.assertNotIn("SdkTestFrameworkGenericEqualMethodSubjectPrefix", source)
            self.assertNotIn("TestFrameworkSubjectFamily", source)

        for forbidden_fragment in [
            "IsConstructorThenAssertStringEqualityEntryPointShape(",
            "private static bool IsStringAssertEqualMethodSubjectId(",
            "private static bool TryReadTestFrameworkEqualComparedType(",
            "private static bool TryReadSingleGenericTypeArgument(",
            "private static bool TryReadGenericArgumentList(",
            "private static IReadOnlyList<string> SplitTopLevelArguments(",
            "private static IReadOnlyList<string> GetMethodParameterTypes(",
        ]:
            self.assertNotIn(forbidden_fragment, shape_validation_source + "\n" + plan_builders_source)
