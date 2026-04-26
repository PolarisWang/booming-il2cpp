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
        convert_split_source = NATIVE_REFERENCE_EMITTER_CONVERT_FAMILY_PATH.read_text(encoding="utf-8")
        memory_extensions_split_source = NATIVE_REFERENCE_EMITTER_MEMORY_EXTENSIONS_MANAGED_INVOKE_FAMILY_PATH.read_text(encoding="utf-8")
        spanhelpers_split_source = NATIVE_REFERENCE_EMITTER_SPANHELPERS_KERNEL_FAMILY_PATH.read_text(encoding="utf-8")
        array_memorymarshal_split_source = NATIVE_REFERENCE_EMITTER_ARRAY_AND_MEMORYMARSHAL_FAMILY_PATH.read_text(encoding="utf-8")
        half_split_source = NATIVE_REFERENCE_EMITTER_HALF_MANAGED_INVOKE_FAMILY_PATH.read_text(encoding="utf-8")
        floating_split_source = NATIVE_REFERENCE_EMITTER_FLOATING_SCALAR_MANAGED_INVOKE_FAMILY_PATH.read_text(encoding="utf-8")
        number_split_source = NATIVE_REFERENCE_EMITTER_NUMBER_MANAGED_INVOKE_FAMILY_PATH.read_text(encoding="utf-8")
        decimal_split_source = NATIVE_REFERENCE_EMITTER_DECIMAL_MANAGED_INVOKE_FAMILY_PATH.read_text(encoding="utf-8")
        char_split_source = NATIVE_REFERENCE_EMITTER_CHAR_MANAGED_INVOKE_FAMILY_PATH.read_text(encoding="utf-8")
        companion_split_source = NATIVE_REFERENCE_EMITTER_COMPANION_MANAGED_INVOKE_FAMILY_PATH.read_text(encoding="utf-8")
        datetime_engine_split_source = NATIVE_REFERENCE_EMITTER_DATETIME_SEMANTIC_ENGINE_FAMILY_PATH.read_text(encoding="utf-8")
        globalization_datetime_split_source = NATIVE_REFERENCE_EMITTER_GLOBALIZATION_DATETIME_SUPPORT_FAMILY_PATH.read_text(encoding="utf-8")
        valuetype_kernel_split_source = NATIVE_REFERENCE_EMITTER_VALUETYPE_KERNEL_FAMILY_PATH.read_text(encoding="utf-8")
        enum_split_source = NATIVE_REFERENCE_EMITTER_ENUM_MANAGED_INVOKE_FAMILY_PATH.read_text(encoding="utf-8")
        string_split_source = NATIVE_REFERENCE_EMITTER_STRING_MANAGED_INVOKE_FAMILY_PATH.read_text(encoding="utf-8")
        unsafe_split_source = NATIVE_REFERENCE_EMITTER_UNSAFE_MANAGED_INVOKE_FAMILY_PATH.read_text(encoding="utf-8")
        combined_source = emitter_source + "\n" + convert_split_source + "\n" + memory_extensions_split_source + "\n" + spanhelpers_split_source + "\n" + array_memorymarshal_split_source + "\n" + half_split_source + "\n" + floating_split_source + "\n" + number_split_source + "\n" + decimal_split_source + "\n" + char_split_source + "\n" + companion_split_source + "\n" + datetime_engine_split_source + "\n" + globalization_datetime_split_source + "\n" + valuetype_kernel_split_source + "\n" + enum_split_source + "\n" + string_split_source + "\n" + unsafe_split_source

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
            '"convert-runtime-helper-unsupported-shape"',
            '"memory-extensions-managed-invoke-unsupported-shape"',
            '"spanhelpers-kernel-unsupported-shape"',
            '"array-memorymarshal-family-unsupported-shape"',
            '"half-managed-invoke-unsupported-shape"',
            '"floating-scalar-managed-invoke-unsupported-shape"',
            '"number-managed-invoke-unsupported-shape"',
            '"decimal-managed-invoke-unsupported-shape"',
            '"char-managed-invoke-unsupported-shape"',
            '"companion-managed-invoke-unsupported-shape"',
            '"datetime-semantic-engine-unsupported-shape"',
            '"globalization-datetime-support-unsupported-shape"',
            '"valuetype-kernel-unsupported-shape"',
            '"enum-managed-invoke-unsupported-shape"',
            '"unsafe-managed-invoke-unsupported-shape"',
            '"string-managed-invoke-unsupported-shape"',
            '"exception-family-owned-unsupported-shape"',
        ]:
            self.assertIn(required_fragment, combined_source)

        self.assertNotIn('"convert-family-owned-unsupported-shape"', combined_source)

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

    def test_runtime_skeleton_convert_like_family_has_internal_handler_ordering(self) -> None:
        emitter_source = NATIVE_REFERENCE_EMITTER_PATH.read_text(encoding="utf-8")

        for required_fragment in [
            "private delegate RuntimeSkeletonFamilyHandlerResult RuntimeSkeletonConvertLikeFamilyHandler(",
            "private static readonly RuntimeSkeletonConvertLikeFamilyHandler[] RuntimeSkeletonConvertLikeFamilyHandlers =",
            "TryBuildRuntimeSkeletonConvertLikeIntForwarderHandler",
            "TryBuildRuntimeSkeletonConvertLikeBoolIdentityForwarderHandler",
            "TryBuildRuntimeSkeletonConvertLikeBoolProducerForwarderHandler",
            "TryBuildRuntimeSkeletonConvertLikePrimitiveHandler",
            "TryBuildRuntimeSkeletonConvertLikeCheckedPrimitiveHandler",
            "TryBuildRuntimeSkeletonConvertLikeByteForwarderHandler",
            "foreach (var convertLikeFamilyHandler in RuntimeSkeletonConvertLikeFamilyHandlers)",
        ]:
            self.assertIn(required_fragment, emitter_source)

        self.assertNotIn("RuntimeSkeletonConvertFamilyHandlers", emitter_source)
        self.assertNotIn("TryBuildRuntimeSkeletonConvertLegacyHandler", emitter_source)
        self.assertLess(
            emitter_source.index("TryBuildRuntimeSkeletonConvertLikePrimitiveHandler"),
            emitter_source.index("TryBuildRuntimeSkeletonConvertLikeByteForwarderHandler"),
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

    def test_runtime_skeleton_string_managed_invoke_family_precedes_string_micro_family(self) -> None:
        emitter_source = NATIVE_REFERENCE_EMITTER_PATH.read_text(encoding="utf-8")
        split_source = NATIVE_REFERENCE_EMITTER_STRING_MANAGED_INVOKE_FAMILY_PATH.read_text(encoding="utf-8")

        self.assertIn("TryBuildRuntimeSkeletonStringManagedInvokeFamilyHandler", emitter_source)
        self.assertLess(
            emitter_source.index("TryBuildRuntimeSkeletonStringManagedInvokeFamilyHandler"),
            emitter_source.index("TryBuildRuntimeSkeletonStringFamilyHandler"),
        )
        self.assertIn("CreateUnsupported(\"string-managed-invoke-unsupported-shape\")", split_source)
        self.assertIn("SubjectId.Contains(\"/System.String::\", StringComparison.Ordinal)", split_source)

    def test_runtime_skeleton_unsafe_managed_invoke_family_precedes_string_families(self) -> None:
        emitter_source = NATIVE_REFERENCE_EMITTER_PATH.read_text(encoding="utf-8")
        split_source = NATIVE_REFERENCE_EMITTER_UNSAFE_MANAGED_INVOKE_FAMILY_PATH.read_text(encoding="utf-8")

        self.assertIn("TryBuildRuntimeSkeletonUnsafeManagedInvokeFamilyHandler", emitter_source)
        self.assertLess(
            emitter_source.index("TryBuildRuntimeSkeletonUnsafeManagedInvokeFamilyHandler"),
            emitter_source.index("TryBuildRuntimeSkeletonStringManagedInvokeFamilyHandler"),
        )
        self.assertIn("CreateUnsupported(\"unsafe-managed-invoke-unsupported-shape\")", split_source)
        self.assertIn(
            "SubjectId.Contains(\"/System.Runtime.CompilerServices.Unsafe::\", StringComparison.Ordinal)",
            split_source,
        )

    def test_runtime_skeleton_memory_extensions_managed_invoke_family_precedes_string_families(self) -> None:
        emitter_source = NATIVE_REFERENCE_EMITTER_PATH.read_text(encoding="utf-8")
        split_source = NATIVE_REFERENCE_EMITTER_MEMORY_EXTENSIONS_MANAGED_INVOKE_FAMILY_PATH.read_text(encoding="utf-8")

        self.assertIn("TryBuildRuntimeSkeletonMemoryExtensionsManagedInvokeFamilyHandler", emitter_source)
        self.assertLess(
            emitter_source.index("TryBuildRuntimeSkeletonMemoryExtensionsManagedInvokeFamilyHandler"),
            emitter_source.index("TryBuildRuntimeSkeletonStringManagedInvokeFamilyHandler"),
        )
        self.assertIn("CreateUnsupported(\"memory-extensions-managed-invoke-unsupported-shape\")", split_source)
        self.assertIn(
            "SubjectId.Contains(\"/System.MemoryExtensions::\", StringComparison.Ordinal)",
            split_source,
        )

    def test_runtime_skeleton_spanhelpers_kernel_family_precedes_string_families(self) -> None:
        emitter_source = NATIVE_REFERENCE_EMITTER_PATH.read_text(encoding="utf-8")
        split_source = NATIVE_REFERENCE_EMITTER_SPANHELPERS_KERNEL_FAMILY_PATH.read_text(encoding="utf-8")

        self.assertIn("TryBuildRuntimeSkeletonSpanHelpersKernelFamilyHandler", emitter_source)
        self.assertLess(
            emitter_source.index("TryBuildRuntimeSkeletonSpanHelpersKernelFamilyHandler"),
            emitter_source.index("TryBuildRuntimeSkeletonStringManagedInvokeFamilyHandler"),
        )
        self.assertIn("CreateUnsupported(\"spanhelpers-kernel-unsupported-shape\")", split_source)
        self.assertIn(
            "SubjectId.Contains(\"/System.SpanHelpers::\", StringComparison.Ordinal)",
            split_source,
        )

    def test_runtime_skeleton_array_and_memorymarshal_family_precedes_string_families(self) -> None:
        emitter_source = NATIVE_REFERENCE_EMITTER_PATH.read_text(encoding="utf-8")
        split_source = NATIVE_REFERENCE_EMITTER_ARRAY_AND_MEMORYMARSHAL_FAMILY_PATH.read_text(encoding="utf-8")

        self.assertIn("TryBuildRuntimeSkeletonArrayAndMemoryMarshalFamilyHandler", emitter_source)
        self.assertLess(
            emitter_source.index("TryBuildRuntimeSkeletonArrayAndMemoryMarshalFamilyHandler"),
            emitter_source.index("TryBuildRuntimeSkeletonStringManagedInvokeFamilyHandler"),
        )
        self.assertIn("CreateUnsupported(\"array-memorymarshal-family-unsupported-shape\")", split_source)
        self.assertIn("SubjectId.Contains(\"/System.Array::\", StringComparison.Ordinal)", split_source)
        self.assertIn(
            "SubjectId.Contains(\"/System.Runtime.InteropServices.MemoryMarshal::\", StringComparison.Ordinal)",
            split_source,
        )

    def test_runtime_skeleton_half_managed_invoke_family_precedes_string_families(self) -> None:
        emitter_source = NATIVE_REFERENCE_EMITTER_PATH.read_text(encoding="utf-8")
        split_source = NATIVE_REFERENCE_EMITTER_HALF_MANAGED_INVOKE_FAMILY_PATH.read_text(encoding="utf-8")

        self.assertIn("TryBuildRuntimeSkeletonHalfManagedInvokeFamilyHandler", emitter_source)
        self.assertLess(
            emitter_source.index("TryBuildRuntimeSkeletonHalfManagedInvokeFamilyHandler"),
            emitter_source.index("TryBuildRuntimeSkeletonStringManagedInvokeFamilyHandler"),
        )
        self.assertIn("CreateUnsupported(\"half-managed-invoke-unsupported-shape\")", split_source)
        self.assertIn("SubjectId.Contains(\"/System.Half::\", StringComparison.Ordinal)", split_source)

    def test_runtime_skeleton_floating_scalar_managed_invoke_family_precedes_string_families(self) -> None:
        emitter_source = NATIVE_REFERENCE_EMITTER_PATH.read_text(encoding="utf-8")
        split_source = NATIVE_REFERENCE_EMITTER_FLOATING_SCALAR_MANAGED_INVOKE_FAMILY_PATH.read_text(encoding="utf-8")

        self.assertIn("TryBuildRuntimeSkeletonFloatingScalarManagedInvokeFamilyHandler", emitter_source)
        self.assertLess(
            emitter_source.index("TryBuildRuntimeSkeletonFloatingScalarManagedInvokeFamilyHandler"),
            emitter_source.index("TryBuildRuntimeSkeletonStringManagedInvokeFamilyHandler"),
        )
        self.assertIn("CreateUnsupported(\"floating-scalar-managed-invoke-unsupported-shape\")", split_source)
        self.assertIn("SubjectId.Contains(\"/System.Single::\", StringComparison.Ordinal)", split_source)
        self.assertIn("SubjectId.Contains(\"/System.Double::\", StringComparison.Ordinal)", split_source)

    def test_runtime_skeleton_number_managed_invoke_family_precedes_string_families(self) -> None:
        emitter_source = NATIVE_REFERENCE_EMITTER_PATH.read_text(encoding="utf-8")
        split_source = NATIVE_REFERENCE_EMITTER_NUMBER_MANAGED_INVOKE_FAMILY_PATH.read_text(encoding="utf-8")

        self.assertIn("TryBuildRuntimeSkeletonNumberManagedInvokeFamilyHandler", emitter_source)
        self.assertLess(
            emitter_source.index("TryBuildRuntimeSkeletonNumberManagedInvokeFamilyHandler"),
            emitter_source.index("TryBuildRuntimeSkeletonStringManagedInvokeFamilyHandler"),
        )
        self.assertIn("CreateUnsupported(\"number-managed-invoke-unsupported-shape\")", split_source)
        self.assertIn("SubjectId.Contains(\"/System.Number::\", StringComparison.Ordinal)", split_source)

    def test_runtime_skeleton_decimal_managed_invoke_family_precedes_string_families(self) -> None:
        emitter_source = NATIVE_REFERENCE_EMITTER_PATH.read_text(encoding="utf-8")
        split_source = NATIVE_REFERENCE_EMITTER_DECIMAL_MANAGED_INVOKE_FAMILY_PATH.read_text(encoding="utf-8")

        self.assertIn("TryBuildRuntimeSkeletonDecimalManagedInvokeFamilyHandler", emitter_source)
        self.assertLess(
            emitter_source.index("TryBuildRuntimeSkeletonDecimalManagedInvokeFamilyHandler"),
            emitter_source.index("TryBuildRuntimeSkeletonStringManagedInvokeFamilyHandler"),
        )
        self.assertIn("CreateUnsupported(\"decimal-managed-invoke-unsupported-shape\")", split_source)
        self.assertIn("SubjectId.Contains(\"/System.Decimal\", StringComparison.Ordinal)", split_source)

    def test_runtime_skeleton_char_managed_invoke_family_precedes_string_families(self) -> None:
        emitter_source = NATIVE_REFERENCE_EMITTER_PATH.read_text(encoding="utf-8")
        split_source = NATIVE_REFERENCE_EMITTER_CHAR_MANAGED_INVOKE_FAMILY_PATH.read_text(encoding="utf-8")

        self.assertIn("TryBuildRuntimeSkeletonCharManagedInvokeFamilyHandler", emitter_source)
        self.assertLess(
            emitter_source.index("TryBuildRuntimeSkeletonCharManagedInvokeFamilyHandler"),
            emitter_source.index("TryBuildRuntimeSkeletonStringManagedInvokeFamilyHandler"),
        )
        self.assertIn("CreateUnsupported(\"char-managed-invoke-unsupported-shape\")", split_source)
        self.assertIn("SubjectId.Contains(\"/System.Char::\", StringComparison.Ordinal)", split_source)

    def test_runtime_skeleton_companion_managed_invoke_family_precedes_string_families(self) -> None:
        emitter_source = NATIVE_REFERENCE_EMITTER_PATH.read_text(encoding="utf-8")
        split_source = NATIVE_REFERENCE_EMITTER_COMPANION_MANAGED_INVOKE_FAMILY_PATH.read_text(encoding="utf-8")

        self.assertIn("TryBuildRuntimeSkeletonCompanionManagedInvokeFamilyHandler", emitter_source)
        self.assertLess(
            emitter_source.index("TryBuildRuntimeSkeletonCompanionManagedInvokeFamilyHandler"),
            emitter_source.index("TryBuildRuntimeSkeletonStringManagedInvokeFamilyHandler"),
        )
        self.assertIn("CreateUnsupported(\"companion-managed-invoke-unsupported-shape\")", split_source)
        self.assertIn("SubjectId.Contains(\"/System.DateTime+LeapSecondCache::\", StringComparison.Ordinal)", split_source)
        self.assertIn("SubjectId.Contains(\"/System.DateOnly+<>c::\", StringComparison.Ordinal)", split_source)

    def test_runtime_skeleton_datetime_semantic_engine_family_precedes_string_families(self) -> None:
        emitter_source = NATIVE_REFERENCE_EMITTER_PATH.read_text(encoding="utf-8")
        split_source = NATIVE_REFERENCE_EMITTER_DATETIME_SEMANTIC_ENGINE_FAMILY_PATH.read_text(encoding="utf-8")

        self.assertIn("TryBuildRuntimeSkeletonDateTimeSemanticEngineFamilyHandler", emitter_source)
        self.assertLess(
            emitter_source.index("TryBuildRuntimeSkeletonDateTimeSemanticEngineFamilyHandler"),
            emitter_source.index("TryBuildRuntimeSkeletonStringManagedInvokeFamilyHandler"),
        )
        self.assertIn("CreateUnsupported(\"datetime-semantic-engine-unsupported-shape\")", split_source)
        self.assertIn("SubjectId.Contains(\"/System.DateTimeFormat::\", StringComparison.Ordinal)", split_source)
        self.assertIn("SubjectId.Contains(\"/System.DateTimeParse::\", StringComparison.Ordinal)", split_source)

    def test_runtime_skeleton_globalization_datetime_support_family_precedes_string_families(self) -> None:
        emitter_source = NATIVE_REFERENCE_EMITTER_PATH.read_text(encoding="utf-8")
        split_source = NATIVE_REFERENCE_EMITTER_GLOBALIZATION_DATETIME_SUPPORT_FAMILY_PATH.read_text(encoding="utf-8")

        self.assertIn("TryBuildRuntimeSkeletonGlobalizationDateTimeSupportFamilyHandler", emitter_source)
        self.assertLess(
            emitter_source.index("TryBuildRuntimeSkeletonGlobalizationDateTimeSupportFamilyHandler"),
            emitter_source.index("TryBuildRuntimeSkeletonStringManagedInvokeFamilyHandler"),
        )
        self.assertIn("CreateUnsupported(\"globalization-datetime-support-unsupported-shape\")", split_source)
        self.assertIn("SubjectId.Contains(\"/System.Globalization.CultureData::\", StringComparison.Ordinal)", split_source)
        self.assertIn("SubjectId.Contains(\"/System.Globalization.DateTimeFormatInfo::\", StringComparison.Ordinal)", split_source)

    def test_runtime_skeleton_valuetype_kernel_family_precedes_string_families(self) -> None:
        emitter_source = NATIVE_REFERENCE_EMITTER_PATH.read_text(encoding="utf-8")
        split_source = NATIVE_REFERENCE_EMITTER_VALUETYPE_KERNEL_FAMILY_PATH.read_text(encoding="utf-8")
        kernel_core_source = RUNTIME_SKELETON_KERNEL_CORE_PATH.read_text(encoding="utf-8")

        self.assertIn("TryBuildRuntimeSkeletonValueTypeKernelFamilyHandler", emitter_source)
        self.assertLess(
            emitter_source.index("TryBuildRuntimeSkeletonValueTypeKernelFamilyHandler"),
            emitter_source.index("TryBuildRuntimeSkeletonStringManagedInvokeFamilyHandler"),
        )
        self.assertIn("CreateUnsupported(\"valuetype-kernel-unsupported-shape\")", split_source)
        self.assertIn("RuntimeSkeletonKernelCore.", split_source)
        self.assertIn("char-classification", kernel_core_source)
        self.assertIn("floating-classification", kernel_core_source)
        self.assertIn("bit-arithmetic", kernel_core_source)
        self.assertIn("tick-arithmetic", kernel_core_source)

    def test_runtime_skeleton_enum_managed_invoke_family_precedes_string_families(self) -> None:
        emitter_source = NATIVE_REFERENCE_EMITTER_PATH.read_text(encoding="utf-8")
        split_source = NATIVE_REFERENCE_EMITTER_ENUM_MANAGED_INVOKE_FAMILY_PATH.read_text(encoding="utf-8")

        self.assertIn("TryBuildRuntimeSkeletonEnumManagedInvokeFamilyHandler", emitter_source)
        self.assertLess(
            emitter_source.index("TryBuildRuntimeSkeletonEnumManagedInvokeFamilyHandler"),
            emitter_source.index("TryBuildRuntimeSkeletonStringManagedInvokeFamilyHandler"),
        )
        self.assertIn("CreateUnsupported(\"enum-managed-invoke-unsupported-shape\")", split_source)
        self.assertIn("SubjectId.Contains(\"/System.Enum::\", StringComparison.Ordinal)", split_source)

    def test_runtime_skeleton_string_family_owns_string_executable_plan_lanes(self) -> None:
        emitter_source = NATIVE_REFERENCE_EMITTER_PATH.read_text(encoding="utf-8")
        split_source = NATIVE_REFERENCE_EMITTER_STRING_MICRO_FAMILY_PATH.read_text(encoding="utf-8")

        self.assertIn("TryBuildRuntimeSkeletonStaticCallCtorGetterExecutableHandler", emitter_source)
        self.assertIn("TryBuildRuntimeSkeletonConstructorThenInstanceCallExecutableHandler", emitter_source)
        self.assertIn(
            'string.Equals(executableLoweringPlan.PlanKind, "staticCallCtorGetter", StringComparison.Ordinal)',
            split_source,
        )
        self.assertIn(
            'string.Equals(executableLoweringPlan.PlanKind, "constructorThenInstanceCall", StringComparison.Ordinal)',
            split_source,
        )

        string_family_handlers_start = emitter_source.index(
            "private static readonly RuntimeSkeletonStringFamilyHandler[] RuntimeSkeletonStringFamilyHandlers =",
        )
        string_family_handlers_end = emitter_source.index(
            "private static RuntimeSkeletonFamilyHandlerResult TryBuildRuntimeSkeletonConvertFamilyHandler(",
        )
        string_family_handlers_slice = emitter_source[string_family_handlers_start:string_family_handlers_end]
        string_family_handler_start = emitter_source.index(
            "private static RuntimeSkeletonFamilyHandlerResult TryBuildRuntimeSkeletonStringFamilyHandler(",
        )
        string_managed_invoke_handler_start = emitter_source.index(
            "private static RuntimeSkeletonFamilyHandlerResult TryBuildRuntimeSkeletonStringManagedInvokeFamilyHandler(",
        )
        string_family_handler_slice = emitter_source[string_family_handler_start:string_managed_invoke_handler_start]

        self.assertIn("TryBuildRuntimeSkeletonStaticCallCtorGetterExecutableHandler", string_family_handlers_slice)
        self.assertIn("TryBuildRuntimeSkeletonConstructorThenInstanceCallExecutableHandler", string_family_handlers_slice)
        self.assertIn("SubjectId.Contains(\"/System.String::\", StringComparison.Ordinal)", string_family_handler_slice)
        self.assertIn("return RuntimeSkeletonFamilyHandlerResult.NoMatch;", string_family_handler_slice)
        self.assertIn("TryBuildRuntimeSkeletonStaticCallCtorGetterExecutableCore(", split_source)
        self.assertIn("TryBuildRuntimeSkeletonConstructorThenInstanceCallExecutableCore(", split_source)

        convert_like_family_start = emitter_source.index(
            "private static RuntimeSkeletonFamilyHandlerResult TryBuildRuntimeSkeletonConvertLikeFamilyHandler(",
        )
        convert_int_handler_start = emitter_source.index(
            "private static RuntimeSkeletonFamilyHandlerResult TryBuildRuntimeSkeletonConvertLikeIntForwarderHandler(",
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
            "private static RuntimeSkeletonFamilyHandlerResult TryBuildRuntimeSkeletonConvertLikeIntForwarderHandler(",
        )

        family_handlers_slice = emitter_source[family_handlers_start:convert_family_start]
        convert_like_family_slice = emitter_source[convert_like_family_start:convert_int_handler_start]

        for required_fragment in [
            "TryBuildRuntimeSkeletonConvertLikeFamilyHandler",
            "foreach (var convertLikeFamilyHandler in RuntimeSkeletonConvertLikeFamilyHandlers)",
            "return RuntimeSkeletonFamilyHandlerResult.NoMatch;",
        ]:
            self.assertIn(required_fragment, emitter_source)

        self.assertIn("TryBuildRuntimeSkeletonConvertLikeFamilyHandler", family_handlers_slice)
        self.assertIn("foreach (var convertLikeFamilyHandler in RuntimeSkeletonConvertLikeFamilyHandlers)", convert_like_family_slice)

    def test_runtime_skeleton_convert_families_share_handler_loop_helper(self) -> None:
        emitter_source = NATIVE_REFERENCE_EMITTER_PATH.read_text(encoding="utf-8")
        convert_split_source = NATIVE_REFERENCE_EMITTER_CONVERT_FAMILY_PATH.read_text(encoding="utf-8")

        convert_family_start = emitter_source.index(
            "private static RuntimeSkeletonFamilyHandlerResult TryBuildRuntimeSkeletonConvertFamilyHandler(",
        )
        convert_like_family_start = emitter_source.index(
            "private static RuntimeSkeletonFamilyHandlerResult TryBuildRuntimeSkeletonConvertLikeFamilyHandler(",
        )
        convert_family_slice = emitter_source[convert_family_start:convert_like_family_start]

        for required_fragment in [
            "private static RuntimeSkeletonFamilyHandlerResult TryBuildRuntimeSkeletonConvertLikeHandlers(",
            "RuntimeSkeletonFamilyHandlerResult missResult)",
            "foreach (var convertLikeFamilyHandler in RuntimeSkeletonConvertLikeFamilyHandlers)",
            "return missResult;",
            "return TryBuildRuntimeSkeletonConvertLikeHandlers(",
            "return TryBuildRuntimeSkeletonConvertLikeHandlers(buildContext, RuntimeSkeletonFamilyHandlerResult.NoMatch);",
        ]:
            self.assertIn(required_fragment, emitter_source)

        self.assertIn("TryBuildRuntimeSkeletonConvertFamilyCore(buildContext);", convert_family_slice)
        self.assertIn("TryBuildRuntimeSkeletonConvertRuntimeHelperCore(", convert_split_source)
        self.assertNotIn("TryBuildRuntimeSkeletonConvertLikeHandlers(", convert_family_slice)

    def test_runtime_skeleton_convert_uses_hotupdate_stable_runtime_helper_abi(self) -> None:
        emitter_source = NATIVE_REFERENCE_EMITTER_PATH.read_text(encoding="utf-8")
        convert_split_source = NATIVE_REFERENCE_EMITTER_CONVERT_FAMILY_PATH.read_text(encoding="utf-8")
        catalog_source = (
            REPO_ROOT
            / "src"
            / "managed"
            / "Chaos.IL2CPP.CodeGen"
            / "ReferenceProof"
            / "NativeReferenceProofCatalog.cs"
        ).read_text(encoding="utf-8")
        helper_source = (
            REPO_ROOT
            / "src"
            / "managed"
            / "Chaos.IL2CPP.CodeGen"
            / "ReferenceProof"
            / "RuntimeSkeletonConvertRuntimeHelperAbi.cs"
        ).read_text(encoding="utf-8")
        template_source = (
            REPO_ROOT
            / "src"
            / "managed"
            / "Chaos.IL2CPP.CodeGen"
            / "Templates"
            / "NativeReferenceProof.RuntimeSkeleton.ConvertRuntimeHelperStub.cpp.scriban"
        ).read_text(encoding="utf-8")

        for required_fragment in [
            "TryBuildRuntimeSkeletonConvertRuntimeHelperHandler",
            "TryBuildRuntimeSkeletonConvertRuntimeHelperCore(",
            "TryBuildAssemblyBoundConvertRuntimeHelperStub(",
            "TryBuildAssemblyBoundConvertRuntimeHelperCore(",
            "RuntimeSkeletonConvertRuntimeHelperAbi.TryCreate(",
            "GetRuntimeSkeletonConvertRuntimeHelperStubTemplate(",
        ]:
            self.assertIn(required_fragment, emitter_source + "\n" + convert_split_source)

        convert_family_start = emitter_source.index(
            "private static RuntimeSkeletonFamilyHandlerResult TryBuildRuntimeSkeletonConvertFamilyHandler(",
        )
        convert_like_family_start = emitter_source.index(
            "private static RuntimeSkeletonFamilyHandlerResult TryBuildRuntimeSkeletonConvertLikeFamilyHandler(",
        )
        convert_family_slice = emitter_source[convert_family_start:convert_like_family_start]
        convert_like_handlers_start = emitter_source.index(
            "private static readonly RuntimeSkeletonConvertLikeFamilyHandler[] RuntimeSkeletonConvertLikeFamilyHandlers =",
        )
        convert_like_handlers_end = emitter_source.index(
            "private static readonly RuntimeSkeletonBindingFamilyHandler[] RuntimeSkeletonBindingFamilyHandlers =",
        )
        convert_like_handlers_slice = emitter_source[convert_like_handlers_start:convert_like_handlers_end]

        self.assertIn("TryBuildRuntimeSkeletonConvertFamilyCore(buildContext)", convert_family_slice)
        self.assertNotIn("TryBuildRuntimeSkeletonConvertRuntimeHelperHandler", convert_like_handlers_slice)

        self.assertIn("RuntimeSkeletonConvertRuntimeHelperStubTemplateRelativePath", catalog_source)
        self.assertIn("convert-runtime-helper-v1", helper_source)
        self.assertIn("HotUpdateStableContractId", helper_source)
        self.assertIn("System.ReadOnlySpan<System.Byte>", helper_source)
        self.assertIn("System.Base64FormattingOptions", helper_source)

        for required_fragment in [
            "target_method_token",
            "{{ arg_refs }}",
            "abi->method_invoke(",
            "return_value_size",
        ]:
            self.assertIn(required_fragment, template_source)

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

    def test_native_reference_emitter_convert_like_family_is_split_from_root_emitter(self) -> None:
        emitter_source = NATIVE_REFERENCE_EMITTER_PATH.read_text(encoding="utf-8")
        split_source = NATIVE_REFERENCE_EMITTER_CONVERTLIKE_FAMILY_PATH.read_text(encoding="utf-8")

        self.assertTrue(
            NATIVE_REFERENCE_EMITTER_CONVERTLIKE_FAMILY_PATH.is_file(),
            msg=f"missing NativeReference emitter split file: {NATIVE_REFERENCE_EMITTER_CONVERTLIKE_FAMILY_PATH}",
        )

        self.assertIn("public sealed partial class NativeReferenceProofEmitter", emitter_source)
        self.assertIn("public sealed partial class NativeReferenceProofEmitter", split_source)
        self.assertIn("TryBuildRuntimeSkeletonConvertLikePrimitiveCore(", split_source)
        self.assertIn("TryBuildRuntimeSkeletonConvertLikeByteForwarderCore(", split_source)
        self.assertIn("TryBuildRuntimeSkeletonConvertLikePrimitiveCore(buildContext);", emitter_source)
        self.assertIn("TryBuildRuntimeSkeletonConvertLikeByteForwarderCore(buildContext);", emitter_source)

    def test_native_reference_emitter_convert_family_is_split_from_root_emitter(self) -> None:
        emitter_source = NATIVE_REFERENCE_EMITTER_PATH.read_text(encoding="utf-8")
        split_source = NATIVE_REFERENCE_EMITTER_CONVERT_FAMILY_PATH.read_text(encoding="utf-8")

        self.assertTrue(
            NATIVE_REFERENCE_EMITTER_CONVERT_FAMILY_PATH.is_file(),
            msg=f"missing NativeReference emitter split file: {NATIVE_REFERENCE_EMITTER_CONVERT_FAMILY_PATH}",
        )

        self.assertIn("public sealed partial class NativeReferenceProofEmitter", emitter_source)
        self.assertIn("public sealed partial class NativeReferenceProofEmitter", split_source)
        self.assertIn("TryBuildRuntimeSkeletonConvertFamilyCore(", split_source)
        self.assertIn("TryBuildRuntimeSkeletonConvertRuntimeHelperCore(", split_source)
        self.assertIn("TryBuildAssemblyBoundConvertRuntimeHelperCore(", split_source)
        self.assertIn("TryBuildRuntimeSkeletonConvertFamilyCore(buildContext);", emitter_source)
        self.assertIn("TryBuildRuntimeSkeletonConvertRuntimeHelperCore(buildContext);", emitter_source)
        self.assertIn("TryBuildAssemblyBoundConvertRuntimeHelperCore(", emitter_source)

    def test_native_reference_emitter_string_managed_invoke_family_is_split_from_root_emitter(self) -> None:
        emitter_source = NATIVE_REFERENCE_EMITTER_PATH.read_text(encoding="utf-8")
        split_source = NATIVE_REFERENCE_EMITTER_STRING_MANAGED_INVOKE_FAMILY_PATH.read_text(encoding="utf-8")

        self.assertTrue(
            NATIVE_REFERENCE_EMITTER_STRING_MANAGED_INVOKE_FAMILY_PATH.is_file(),
            msg=f"missing NativeReference emitter split file: {NATIVE_REFERENCE_EMITTER_STRING_MANAGED_INVOKE_FAMILY_PATH}",
        )

        self.assertIn("public sealed partial class NativeReferenceProofEmitter", emitter_source)
        self.assertIn("public sealed partial class NativeReferenceProofEmitter", split_source)
        self.assertIn("TryBuildRuntimeSkeletonStringManagedInvokeFamilyCore(", split_source)
        self.assertIn("TryBuildRuntimeSkeletonStringManagedInvokeCore(", split_source)
        self.assertIn("TryBuildAssemblyBoundStringManagedInvokeCore(", split_source)
        self.assertIn("TryBuildRuntimeSkeletonStringManagedInvokeFamilyCore(buildContext);", emitter_source)
        self.assertIn("TryBuildRuntimeSkeletonStringManagedInvokeCore(buildContext);", emitter_source)
        self.assertIn("TryBuildAssemblyBoundStringManagedInvokeCore(", emitter_source)

    def test_native_reference_emitter_unsafe_managed_invoke_family_is_split_from_root_emitter(self) -> None:
        emitter_source = NATIVE_REFERENCE_EMITTER_PATH.read_text(encoding="utf-8")
        split_source = NATIVE_REFERENCE_EMITTER_UNSAFE_MANAGED_INVOKE_FAMILY_PATH.read_text(encoding="utf-8")

        self.assertTrue(
            NATIVE_REFERENCE_EMITTER_UNSAFE_MANAGED_INVOKE_FAMILY_PATH.is_file(),
            msg=f"missing NativeReference emitter split file: {NATIVE_REFERENCE_EMITTER_UNSAFE_MANAGED_INVOKE_FAMILY_PATH}",
        )

        self.assertIn("public sealed partial class NativeReferenceProofEmitter", emitter_source)
        self.assertIn("public sealed partial class NativeReferenceProofEmitter", split_source)
        self.assertIn("TryBuildRuntimeSkeletonUnsafeManagedInvokeFamilyCore(", split_source)
        self.assertIn("TryBuildRuntimeSkeletonUnsafeManagedInvokeCore(", split_source)
        self.assertIn("TryBuildAssemblyBoundUnsafeManagedInvokeCore(", split_source)
        self.assertIn("TryBuildRuntimeSkeletonUnsafeManagedInvokeFamilyCore(buildContext);", emitter_source)
        self.assertIn("TryBuildRuntimeSkeletonUnsafeManagedInvokeCore(buildContext);", emitter_source)
        self.assertIn("TryBuildAssemblyBoundUnsafeManagedInvokeCore(", emitter_source)

    def test_native_reference_emitter_memory_extensions_managed_invoke_family_is_split_from_root_emitter(self) -> None:
        emitter_source = NATIVE_REFERENCE_EMITTER_PATH.read_text(encoding="utf-8")
        split_source = NATIVE_REFERENCE_EMITTER_MEMORY_EXTENSIONS_MANAGED_INVOKE_FAMILY_PATH.read_text(encoding="utf-8")

        self.assertTrue(
            NATIVE_REFERENCE_EMITTER_MEMORY_EXTENSIONS_MANAGED_INVOKE_FAMILY_PATH.is_file(),
            msg=f"missing NativeReference emitter split file: {NATIVE_REFERENCE_EMITTER_MEMORY_EXTENSIONS_MANAGED_INVOKE_FAMILY_PATH}",
        )

        self.assertIn("public sealed partial class NativeReferenceProofEmitter", emitter_source)
        self.assertIn("public sealed partial class NativeReferenceProofEmitter", split_source)
        self.assertIn("TryBuildRuntimeSkeletonMemoryExtensionsManagedInvokeFamilyCore(", split_source)
        self.assertIn("TryBuildRuntimeSkeletonMemoryExtensionsManagedInvokeCore(", split_source)
        self.assertIn("TryBuildAssemblyBoundMemoryExtensionsManagedInvokeCore(", split_source)
        self.assertIn("TryBuildRuntimeSkeletonMemoryExtensionsManagedInvokeFamilyCore(buildContext);", emitter_source)
        self.assertIn("TryBuildRuntimeSkeletonMemoryExtensionsManagedInvokeCore(buildContext);", emitter_source)
        self.assertIn("TryBuildAssemblyBoundMemoryExtensionsManagedInvokeCore(", emitter_source)

    def test_native_reference_emitter_spanhelpers_kernel_family_is_split_from_root_emitter(self) -> None:
        emitter_source = NATIVE_REFERENCE_EMITTER_PATH.read_text(encoding="utf-8")
        split_source = NATIVE_REFERENCE_EMITTER_SPANHELPERS_KERNEL_FAMILY_PATH.read_text(encoding="utf-8")

        self.assertTrue(
            NATIVE_REFERENCE_EMITTER_SPANHELPERS_KERNEL_FAMILY_PATH.is_file(),
            msg=f"missing NativeReference emitter split file: {NATIVE_REFERENCE_EMITTER_SPANHELPERS_KERNEL_FAMILY_PATH}",
        )

        self.assertIn("public sealed partial class NativeReferenceProofEmitter", emitter_source)
        self.assertIn("public sealed partial class NativeReferenceProofEmitter", split_source)
        self.assertIn("TryBuildRuntimeSkeletonSpanHelpersKernelFamilyCore(", split_source)
        self.assertIn("TryBuildRuntimeSkeletonSpanHelpersKernelCore(", split_source)
        self.assertIn("TryBuildAssemblyBoundSpanHelpersKernelCore(", split_source)
        self.assertIn("TryBuildRuntimeSkeletonSpanHelpersKernelFamilyCore(buildContext);", emitter_source)
        self.assertIn("TryBuildRuntimeSkeletonSpanHelpersKernelCore(buildContext);", emitter_source)
        self.assertIn("TryBuildAssemblyBoundSpanHelpersKernelCore(", emitter_source)

    def test_native_reference_emitter_array_and_memorymarshal_family_is_split_from_root_emitter(self) -> None:
        emitter_source = NATIVE_REFERENCE_EMITTER_PATH.read_text(encoding="utf-8")
        split_source = NATIVE_REFERENCE_EMITTER_ARRAY_AND_MEMORYMARSHAL_FAMILY_PATH.read_text(encoding="utf-8")

        self.assertTrue(
            NATIVE_REFERENCE_EMITTER_ARRAY_AND_MEMORYMARSHAL_FAMILY_PATH.is_file(),
            msg=f"missing NativeReference emitter split file: {NATIVE_REFERENCE_EMITTER_ARRAY_AND_MEMORYMARSHAL_FAMILY_PATH}",
        )

        self.assertIn("public sealed partial class NativeReferenceProofEmitter", emitter_source)
        self.assertIn("public sealed partial class NativeReferenceProofEmitter", split_source)
        self.assertIn("TryBuildRuntimeSkeletonArrayAndMemoryMarshalFamilyCore(", split_source)
        self.assertIn("TryBuildRuntimeSkeletonArrayAndMemoryMarshalCore(", split_source)
        self.assertIn("TryBuildAssemblyBoundArrayAndMemoryMarshalCore(", split_source)
        self.assertIn("TryBuildRuntimeSkeletonArrayAndMemoryMarshalFamilyCore(buildContext);", emitter_source)
        self.assertIn("TryBuildRuntimeSkeletonArrayAndMemoryMarshalCore(buildContext);", emitter_source)
        self.assertIn("TryBuildAssemblyBoundArrayAndMemoryMarshalCore(", emitter_source)

    def test_native_reference_emitter_half_managed_invoke_family_is_split_from_root_emitter(self) -> None:
        emitter_source = NATIVE_REFERENCE_EMITTER_PATH.read_text(encoding="utf-8")
        split_source = NATIVE_REFERENCE_EMITTER_HALF_MANAGED_INVOKE_FAMILY_PATH.read_text(encoding="utf-8")

        self.assertTrue(
            NATIVE_REFERENCE_EMITTER_HALF_MANAGED_INVOKE_FAMILY_PATH.is_file(),
            msg=f"missing NativeReference emitter split file: {NATIVE_REFERENCE_EMITTER_HALF_MANAGED_INVOKE_FAMILY_PATH}",
        )

        self.assertIn("public sealed partial class NativeReferenceProofEmitter", emitter_source)
        self.assertIn("public sealed partial class NativeReferenceProofEmitter", split_source)
        self.assertIn("TryBuildRuntimeSkeletonHalfManagedInvokeFamilyCore(", split_source)
        self.assertIn("TryBuildRuntimeSkeletonHalfManagedInvokeCore(", split_source)
        self.assertIn("TryBuildAssemblyBoundHalfManagedInvokeCore(", split_source)
        self.assertIn("TryBuildRuntimeSkeletonHalfManagedInvokeFamilyCore(buildContext);", emitter_source)
        self.assertIn("TryBuildRuntimeSkeletonHalfManagedInvokeCore(buildContext);", emitter_source)
        self.assertIn("TryBuildAssemblyBoundHalfManagedInvokeCore(", emitter_source)

    def test_native_reference_emitter_number_managed_invoke_family_is_split_from_root_emitter(self) -> None:
        emitter_source = NATIVE_REFERENCE_EMITTER_PATH.read_text(encoding="utf-8")
        split_source = NATIVE_REFERENCE_EMITTER_NUMBER_MANAGED_INVOKE_FAMILY_PATH.read_text(encoding="utf-8")

        self.assertTrue(
            NATIVE_REFERENCE_EMITTER_NUMBER_MANAGED_INVOKE_FAMILY_PATH.is_file(),
            msg=f"missing NativeReference emitter split file: {NATIVE_REFERENCE_EMITTER_NUMBER_MANAGED_INVOKE_FAMILY_PATH}",
        )

        self.assertIn("public sealed partial class NativeReferenceProofEmitter", emitter_source)
        self.assertIn("public sealed partial class NativeReferenceProofEmitter", split_source)
        self.assertIn("TryBuildRuntimeSkeletonNumberManagedInvokeFamilyCore(", split_source)
        self.assertIn("TryBuildRuntimeSkeletonNumberManagedInvokeCore(", split_source)
        self.assertIn("TryBuildAssemblyBoundNumberManagedInvokeCore(", split_source)
        self.assertIn("TryBuildRuntimeSkeletonNumberManagedInvokeFamilyCore(buildContext);", emitter_source)
        self.assertIn("TryBuildRuntimeSkeletonNumberManagedInvokeCore(buildContext);", emitter_source)
        self.assertIn("TryBuildAssemblyBoundNumberManagedInvokeCore(", emitter_source)

    def test_native_reference_emitter_decimal_managed_invoke_family_is_split_from_root_emitter(self) -> None:
        emitter_source = NATIVE_REFERENCE_EMITTER_PATH.read_text(encoding="utf-8")
        split_source = NATIVE_REFERENCE_EMITTER_DECIMAL_MANAGED_INVOKE_FAMILY_PATH.read_text(encoding="utf-8")

        self.assertTrue(
            NATIVE_REFERENCE_EMITTER_DECIMAL_MANAGED_INVOKE_FAMILY_PATH.is_file(),
            msg=f"missing NativeReference emitter split file: {NATIVE_REFERENCE_EMITTER_DECIMAL_MANAGED_INVOKE_FAMILY_PATH}",
        )

        self.assertIn("public sealed partial class NativeReferenceProofEmitter", emitter_source)
        self.assertIn("public sealed partial class NativeReferenceProofEmitter", split_source)
        self.assertIn("TryBuildRuntimeSkeletonDecimalManagedInvokeFamilyCore(", split_source)
        self.assertIn("TryBuildRuntimeSkeletonDecimalManagedInvokeCore(", split_source)
        self.assertIn("TryBuildAssemblyBoundDecimalManagedInvokeCore(", split_source)
        self.assertIn("TryBuildRuntimeSkeletonDecimalManagedInvokeFamilyCore(buildContext);", emitter_source)
        self.assertIn("TryBuildRuntimeSkeletonDecimalManagedInvokeCore(buildContext);", emitter_source)
        self.assertIn("TryBuildAssemblyBoundDecimalManagedInvokeCore(", emitter_source)

    def test_native_reference_emitter_char_managed_invoke_family_is_split_from_root_emitter(self) -> None:
        emitter_source = NATIVE_REFERENCE_EMITTER_PATH.read_text(encoding="utf-8")
        split_source = NATIVE_REFERENCE_EMITTER_CHAR_MANAGED_INVOKE_FAMILY_PATH.read_text(encoding="utf-8")

        self.assertTrue(
            NATIVE_REFERENCE_EMITTER_CHAR_MANAGED_INVOKE_FAMILY_PATH.is_file(),
            msg=f"missing NativeReference emitter split file: {NATIVE_REFERENCE_EMITTER_CHAR_MANAGED_INVOKE_FAMILY_PATH}",
        )

        self.assertIn("public sealed partial class NativeReferenceProofEmitter", emitter_source)
        self.assertIn("public sealed partial class NativeReferenceProofEmitter", split_source)
        self.assertIn("TryBuildRuntimeSkeletonCharManagedInvokeFamilyCore(", split_source)
        self.assertIn("TryBuildRuntimeSkeletonCharManagedInvokeCore(", split_source)
        self.assertIn("TryBuildAssemblyBoundCharManagedInvokeCore(", split_source)
        self.assertIn("TryBuildRuntimeSkeletonCharManagedInvokeFamilyCore(buildContext);", emitter_source)
        self.assertIn("TryBuildRuntimeSkeletonCharManagedInvokeCore(buildContext);", emitter_source)
        self.assertIn("TryBuildAssemblyBoundCharManagedInvokeCore(", emitter_source)

    def test_native_reference_emitter_companion_managed_invoke_family_is_split_from_root_emitter(self) -> None:
        emitter_source = NATIVE_REFERENCE_EMITTER_PATH.read_text(encoding="utf-8")
        split_source = NATIVE_REFERENCE_EMITTER_COMPANION_MANAGED_INVOKE_FAMILY_PATH.read_text(encoding="utf-8")

        self.assertTrue(
            NATIVE_REFERENCE_EMITTER_COMPANION_MANAGED_INVOKE_FAMILY_PATH.is_file(),
            msg=f"missing NativeReference emitter split file: {NATIVE_REFERENCE_EMITTER_COMPANION_MANAGED_INVOKE_FAMILY_PATH}",
        )

        self.assertIn("public sealed partial class NativeReferenceProofEmitter", emitter_source)
        self.assertIn("public sealed partial class NativeReferenceProofEmitter", split_source)
        self.assertIn("TryBuildRuntimeSkeletonCompanionManagedInvokeFamilyCore(", split_source)
        self.assertIn("TryBuildRuntimeSkeletonCompanionManagedInvokeCore(", split_source)
        self.assertIn("TryBuildAssemblyBoundCompanionManagedInvokeCore(", split_source)
        self.assertIn("TryBuildRuntimeSkeletonCompanionManagedInvokeFamilyCore(buildContext);", emitter_source)
        self.assertIn("TryBuildRuntimeSkeletonCompanionManagedInvokeCore(buildContext);", emitter_source)
        self.assertIn("TryBuildAssemblyBoundCompanionManagedInvokeCore(", emitter_source)

    def test_native_reference_emitter_datetime_semantic_engine_family_is_split_from_root_emitter(self) -> None:
        emitter_source = NATIVE_REFERENCE_EMITTER_PATH.read_text(encoding="utf-8")
        split_source = NATIVE_REFERENCE_EMITTER_DATETIME_SEMANTIC_ENGINE_FAMILY_PATH.read_text(encoding="utf-8")

        self.assertTrue(
            NATIVE_REFERENCE_EMITTER_DATETIME_SEMANTIC_ENGINE_FAMILY_PATH.is_file(),
            msg=f"missing NativeReference emitter split file: {NATIVE_REFERENCE_EMITTER_DATETIME_SEMANTIC_ENGINE_FAMILY_PATH}",
        )

        self.assertIn("public sealed partial class NativeReferenceProofEmitter", emitter_source)
        self.assertIn("public sealed partial class NativeReferenceProofEmitter", split_source)
        self.assertIn("TryBuildRuntimeSkeletonDateTimeSemanticEngineFamilyCore(", split_source)
        self.assertIn("TryBuildRuntimeSkeletonDateTimeSemanticEngineCore(", split_source)
        self.assertIn("TryBuildAssemblyBoundDateTimeSemanticEngineCore(", split_source)
        self.assertIn("TryBuildRuntimeSkeletonDateTimeSemanticEngineFamilyCore(buildContext);", emitter_source)
        self.assertIn("TryBuildRuntimeSkeletonDateTimeSemanticEngineCore(buildContext);", emitter_source)
        self.assertIn("TryBuildAssemblyBoundDateTimeSemanticEngineCore(", emitter_source)

    def test_native_reference_emitter_globalization_datetime_support_family_is_split_from_root_emitter(self) -> None:
        emitter_source = NATIVE_REFERENCE_EMITTER_PATH.read_text(encoding="utf-8")
        split_source = NATIVE_REFERENCE_EMITTER_GLOBALIZATION_DATETIME_SUPPORT_FAMILY_PATH.read_text(encoding="utf-8")

        self.assertTrue(
            NATIVE_REFERENCE_EMITTER_GLOBALIZATION_DATETIME_SUPPORT_FAMILY_PATH.is_file(),
            msg=f"missing NativeReference emitter split file: {NATIVE_REFERENCE_EMITTER_GLOBALIZATION_DATETIME_SUPPORT_FAMILY_PATH}",
        )

        self.assertIn("public sealed partial class NativeReferenceProofEmitter", emitter_source)
        self.assertIn("public sealed partial class NativeReferenceProofEmitter", split_source)
        self.assertIn("TryBuildRuntimeSkeletonGlobalizationDateTimeSupportFamilyCore(", split_source)
        self.assertIn("TryBuildRuntimeSkeletonGlobalizationDateTimeSupportCore(", split_source)
        self.assertIn("TryBuildAssemblyBoundGlobalizationDateTimeSupportCore(", split_source)
        self.assertIn("TryBuildRuntimeSkeletonGlobalizationDateTimeSupportFamilyCore(buildContext);", emitter_source)
        self.assertIn("TryBuildRuntimeSkeletonGlobalizationDateTimeSupportCore(buildContext);", emitter_source)
        self.assertIn("TryBuildAssemblyBoundGlobalizationDateTimeSupportCore(", emitter_source)

    def test_native_reference_emitter_valuetype_kernel_family_is_split_from_root_emitter(self) -> None:
        emitter_source = NATIVE_REFERENCE_EMITTER_PATH.read_text(encoding="utf-8")
        split_source = NATIVE_REFERENCE_EMITTER_VALUETYPE_KERNEL_FAMILY_PATH.read_text(encoding="utf-8")

        self.assertTrue(
            NATIVE_REFERENCE_EMITTER_VALUETYPE_KERNEL_FAMILY_PATH.is_file(),
            msg=f"missing NativeReference emitter split file: {NATIVE_REFERENCE_EMITTER_VALUETYPE_KERNEL_FAMILY_PATH}",
        )

        self.assertIn("public sealed partial class NativeReferenceProofEmitter", emitter_source)
        self.assertIn("public sealed partial class NativeReferenceProofEmitter", split_source)
        self.assertIn("TryBuildRuntimeSkeletonValueTypeKernelFamilyCore(", split_source)
        self.assertIn("TryBuildRuntimeSkeletonValueTypeKernelCore(", split_source)
        self.assertIn("TryBuildAssemblyBoundValueTypeKernelCore(", split_source)
        self.assertIn("TryBuildRuntimeSkeletonValueTypeKernelFamilyCore(buildContext);", emitter_source)
        self.assertIn("TryBuildRuntimeSkeletonValueTypeKernelCore(buildContext);", emitter_source)
        self.assertIn("TryBuildAssemblyBoundValueTypeKernelCore(", emitter_source)

    def test_runtime_skeleton_kernel_registry_no_longer_uses_method_spec_table(self) -> None:
        kernel_core_source = RUNTIME_SKELETON_KERNEL_CORE_PATH.read_text(encoding="utf-8")
        split_source = NATIVE_REFERENCE_EMITTER_VALUETYPE_KERNEL_FAMILY_PATH.read_text(encoding="utf-8")

        self.assertNotIn("RuntimeSkeletonKernelMethodSpec", kernel_core_source)
        self.assertNotIn("RuntimeSkeletonKernelMethodSpec", split_source)
        self.assertIn("RuntimeSkeletonKernelCore.TryCreate(", split_source)
        self.assertIn("RuntimeSkeletonKernelEmissionPlan", kernel_core_source)
        self.assertIn("RuntimeSkeletonKernelCarrierSchema", kernel_core_source)

    def test_runtime_skeleton_half_kernel_operators_do_not_reuse_equals_semantics(self) -> None:
        kernel_core_source = RUNTIME_SKELETON_KERNEL_CORE_PATH.read_text(encoding="utf-8")

        self.assertIn("HalfOperatorEquals(request->arg0, request->arg1)", kernel_core_source)
        self.assertIn("HalfOperatorLessThan(request->arg0, request->arg1)", kernel_core_source)
        self.assertIn("HalfOperatorLessThanOrEqual(request->arg0, request->arg1)", kernel_core_source)
        self.assertIn("HalfOperatorGreaterThan(request->arg0, request->arg1)", kernel_core_source)
        self.assertIn("HalfOperatorGreaterThanOrEqual(request->arg0, request->arg1)", kernel_core_source)
        self.assertNotIn("HalfEquals(request->arg0, request->arg1)", kernel_core_source)

    def test_native_reference_emitter_floating_scalar_managed_invoke_family_is_split_from_root_emitter(self) -> None:
        emitter_source = NATIVE_REFERENCE_EMITTER_PATH.read_text(encoding="utf-8")
        split_source = NATIVE_REFERENCE_EMITTER_FLOATING_SCALAR_MANAGED_INVOKE_FAMILY_PATH.read_text(encoding="utf-8")

        self.assertTrue(
            NATIVE_REFERENCE_EMITTER_FLOATING_SCALAR_MANAGED_INVOKE_FAMILY_PATH.is_file(),
            msg=f"missing NativeReference emitter split file: {NATIVE_REFERENCE_EMITTER_FLOATING_SCALAR_MANAGED_INVOKE_FAMILY_PATH}",
        )

        self.assertIn("public sealed partial class NativeReferenceProofEmitter", emitter_source)
        self.assertIn("public sealed partial class NativeReferenceProofEmitter", split_source)
        self.assertIn("TryBuildRuntimeSkeletonFloatingScalarManagedInvokeFamilyCore(", split_source)
        self.assertIn("TryBuildRuntimeSkeletonFloatingScalarManagedInvokeCore(", split_source)
        self.assertIn("TryBuildAssemblyBoundFloatingScalarManagedInvokeCore(", split_source)
        self.assertIn("TryBuildRuntimeSkeletonFloatingScalarManagedInvokeFamilyCore(buildContext);", emitter_source)
        self.assertIn("TryBuildRuntimeSkeletonFloatingScalarManagedInvokeCore(buildContext);", emitter_source)
        self.assertIn("TryBuildAssemblyBoundFloatingScalarManagedInvokeCore(", emitter_source)

    def test_native_reference_emitter_enum_managed_invoke_family_is_split_from_root_emitter(self) -> None:
        emitter_source = NATIVE_REFERENCE_EMITTER_PATH.read_text(encoding="utf-8")
        split_source = NATIVE_REFERENCE_EMITTER_ENUM_MANAGED_INVOKE_FAMILY_PATH.read_text(encoding="utf-8")

        self.assertTrue(
            NATIVE_REFERENCE_EMITTER_ENUM_MANAGED_INVOKE_FAMILY_PATH.is_file(),
            msg=f"missing NativeReference emitter split file: {NATIVE_REFERENCE_EMITTER_ENUM_MANAGED_INVOKE_FAMILY_PATH}",
        )

        self.assertIn("public sealed partial class NativeReferenceProofEmitter", emitter_source)
        self.assertIn("public sealed partial class NativeReferenceProofEmitter", split_source)
        self.assertIn("TryBuildRuntimeSkeletonEnumManagedInvokeFamilyCore(", split_source)
        self.assertIn("TryBuildRuntimeSkeletonEnumManagedInvokeCore(", split_source)
        self.assertIn("TryBuildAssemblyBoundEnumManagedInvokeCore(", split_source)
        self.assertIn("TryBuildRuntimeSkeletonEnumManagedInvokeFamilyCore(buildContext);", emitter_source)
        self.assertIn("TryBuildRuntimeSkeletonEnumManagedInvokeCore(buildContext);", emitter_source)
        self.assertIn("TryBuildAssemblyBoundEnumManagedInvokeCore(", emitter_source)
        self.assertIn("TryBuildRuntimeSkeletonEnumImportedBridgeCore(", split_source)
        self.assertIn("GetOrAddRuntimeSkeletonTypeDescriptor(", split_source)

    def test_runtime_skeleton_c_plus_managed_invoke_core_is_shared(self) -> None:
        for required_path in [
            RUNTIME_SKELETON_MANAGED_INVOKE_CORE_PATH,
            RUNTIME_SKELETON_VALUE_TYPE_MANAGED_INVOKE_CORE_PATH,
            RUNTIME_SKELETON_SEMANTIC_REGISTRY_PATH,
            RUNTIME_SKELETON_BACKEND_SELECTOR_PATH,
            RUNTIME_SKELETON_KERNEL_CORE_PATH,
        ]:
            self.assertTrue(required_path.is_file(), msg=f"missing runtime skeleton C+ core file: {required_path}")

        core_source = RUNTIME_SKELETON_MANAGED_INVOKE_CORE_PATH.read_text(encoding="utf-8")
        value_type_core_source = RUNTIME_SKELETON_VALUE_TYPE_MANAGED_INVOKE_CORE_PATH.read_text(encoding="utf-8")
        registry_source = RUNTIME_SKELETON_SEMANTIC_REGISTRY_PATH.read_text(encoding="utf-8")
        selector_source = RUNTIME_SKELETON_BACKEND_SELECTOR_PATH.read_text(encoding="utf-8")
        kernel_core_source = RUNTIME_SKELETON_KERNEL_CORE_PATH.read_text(encoding="utf-8")
        half_source = (
            REPO_ROOT
            / "src"
            / "managed"
            / "Chaos.IL2CPP.CodeGen"
            / "ReferenceProof"
            / "RuntimeSkeletonHalfManagedInvokeAbi.cs"
        ).read_text(encoding="utf-8")
        number_source = (
            REPO_ROOT
            / "src"
            / "managed"
            / "Chaos.IL2CPP.CodeGen"
            / "ReferenceProof"
            / "RuntimeSkeletonNumberManagedInvokeAbi.cs"
        ).read_text(encoding="utf-8")
        decimal_source = (
            REPO_ROOT
            / "src"
            / "managed"
            / "Chaos.IL2CPP.CodeGen"
            / "ReferenceProof"
            / "RuntimeSkeletonDecimalManagedInvokeAbi.cs"
        ).read_text(encoding="utf-8")
        char_source = (
            REPO_ROOT
            / "src"
            / "managed"
            / "Chaos.IL2CPP.CodeGen"
            / "ReferenceProof"
            / "RuntimeSkeletonCharManagedInvokeAbi.cs"
        ).read_text(encoding="utf-8")
        enum_source = (
            REPO_ROOT
            / "src"
            / "managed"
            / "Chaos.IL2CPP.CodeGen"
            / "ReferenceProof"
            / "RuntimeSkeletonEnumManagedInvokeAbi.cs"
        ).read_text(encoding="utf-8")

        for required_fragment in [
            "enum RuntimeSkeletonExecutionBackendKind",
            "ManagedInvoke",
            "MetadataHelper",
            "Kernel",
            "Intrinsic",
            "ImportedBridge",
            "record RuntimeSkeletonSemanticFamilyDescriptor(",
            "record RuntimeSkeletonManagedInvokePointerArgShape(",
            "record RuntimeSkeletonManagedInvokeReturnContract(",
            "record RuntimeSkeletonValueTypeManagedInvokeDescriptor(",
            "enum RuntimeSkeletonKernelBackendKind",
            "Generic",
            "record RuntimeSkeletonKernelSemanticDescriptor(",
            "record RuntimeSkeletonKernelCarrierSchema(",
            "record RuntimeSkeletonKernelEmissionPlan(",
            "CreatePointerBackedArgumentShapes(",
            "TryCreateStandardReturnContract(",
        ]:
            self.assertIn(required_fragment, core_source + "\n" + value_type_core_source + "\n" + registry_source + "\n" + selector_source + "\n" + kernel_core_source)

        for required_fragment in [
            "RuntimeSkeletonSemanticRegistry.",
            "RuntimeSkeletonBackendSelector.",
            "RuntimeSkeletonManagedInvokeAbiCore.",
        ]:
            self.assertIn(required_fragment, half_source)
            self.assertIn(required_fragment, number_source)
            self.assertIn(required_fragment, decimal_source)
            self.assertIn(required_fragment, char_source)
            self.assertIn(required_fragment, enum_source)

        for required_fragment in [
            "RuntimeSkeletonSemanticRegistry.",
            "RuntimeSkeletonBackendSelector.",
            "RuntimeSkeletonManagedInvokeAbiCore.",
            "RuntimeSkeletonValueTypeManagedInvokeCore.",
        ]:
            self.assertIn(required_fragment, half_source)
            self.assertIn(required_fragment, decimal_source)
            self.assertIn(required_fragment, char_source)
            self.assertIn(required_fragment, enum_source)

    def test_native_reference_emitter_string_micro_family_is_split_from_root_emitter(self) -> None:
        emitter_source = NATIVE_REFERENCE_EMITTER_PATH.read_text(encoding="utf-8")
        split_source = NATIVE_REFERENCE_EMITTER_STRING_MICRO_FAMILY_PATH.read_text(encoding="utf-8")

        self.assertTrue(
            NATIVE_REFERENCE_EMITTER_STRING_MICRO_FAMILY_PATH.is_file(),
            msg=f"missing NativeReference emitter split file: {NATIVE_REFERENCE_EMITTER_STRING_MICRO_FAMILY_PATH}",
        )

        self.assertIn("public sealed partial class NativeReferenceProofEmitter", emitter_source)
        self.assertIn("public sealed partial class NativeReferenceProofEmitter", split_source)
        self.assertIn("TryBuildRuntimeSkeletonStaticStringReturnForwarderCore(", split_source)
        self.assertIn("TryBuildRuntimeSkeletonStaticStringProducerConsoleWriteLineCore(", split_source)
        self.assertIn("TryBuildRuntimeSkeletonStaticCallCtorGetterExecutableCore(", split_source)
        self.assertIn("TryBuildRuntimeSkeletonConstructorThenInstanceCallExecutableCore(", split_source)
        self.assertIn("TryBuildRuntimeSkeletonStaticStringReturnForwarderCore(buildContext);", emitter_source)
        self.assertIn("TryBuildRuntimeSkeletonConstructorThenInstanceCallExecutableCore(buildContext);", emitter_source)

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
