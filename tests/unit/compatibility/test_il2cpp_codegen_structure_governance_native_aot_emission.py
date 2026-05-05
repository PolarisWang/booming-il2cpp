from tests.unit.compatibility.il2cpp_codegen_structure_governance_test_support import *


class TestIl2CppCodeGenStructureGovernanceNativeAotEmission(Il2CppCodeGenStructureGovernanceTestSupport):
    def test_native_aot_collection_and_reflection_simple_helpers_prefer_scriban_templates(self) -> None:
        catalog_source = NATIVE_AOT_TEMPLATE_CATALOG_PATH.read_text(encoding="utf-8")
        collection_runtime_source = NATIVE_AOT_EXTERNAL_RUNTIME_COLLECTION_AND_REFLECTION_PATH.read_text(encoding="utf-8")

        self.assertTrue(
            NATIVE_AOT_SIMPLE_EXTERNAL_RUNTIME_HELPER_TEMPLATE_PATH.is_file(),
            msg=f"missing NativeAot simple helper template: {NATIVE_AOT_SIMPLE_EXTERNAL_RUNTIME_HELPER_TEMPLATE_PATH}",
        )

        for required_fragment in [
            'SimpleExternalRuntimeHelperTemplateRelativePath = "Templates/NativeAot.SimpleExternalRuntimeHelper.cpp.scriban"',
            "public static Template GetSimpleExternalRuntimeHelperTemplate()",
        ]:
            self.assertIn(required_fragment, catalog_source)

        for required_fragment in [
            "RenderSimpleExternalRuntimeHelper(",
        ]:
            self.assertIn(required_fragment, collection_runtime_source)

        for legacy_fragment in [
            '"extern \\"C\\" void " + GetExternalRuntimeHelperSymbol(callee) + "(CHAOS_IL2CPP_INTPTR chaos_arg_0, CHAOS_IL2CPP_INTPTR chaos_arg_1)\\n{\\n    chaos_initialize_array_from_field_data_int32(chaos_arg_0, chaos_arg_1);\\n}"',
            '"extern \\"C\\" CHAOS_IL2CPP_INTPTR " + GetExternalRuntimeHelperSymbol(callee) + "(CHAOS_IL2CPP_INTPTR chaos_arg_0)\\n{\\n    return chaos_create_field_data_span_int32(chaos_arg_0);\\n}"',
            '"extern \\"C\\" CHAOS_IL2CPP_INTPTR " + GetExternalRuntimeHelperSymbol(callee) + "(CHAOS_IL2CPP_INTPTR chaos_arg_0, CHAOS_IL2CPP_INT32 chaos_arg_1, CHAOS_IL2CPP_INT32 chaos_arg_2)\\n{\\n    return chaos_create_array_memory_int32(chaos_arg_0, chaos_arg_1, chaos_arg_2);\\n}"',
            '"extern \\"C\\" CHAOS_IL2CPP_INTPTR " + GetExternalRuntimeHelperSymbol(callee) + "(CHAOS_IL2CPP_INTPTR chaos_arg_0)\\n{\\n    if (chaos_arg_0 == static_cast<CHAOS_IL2CPP_INTPTR>(0))\\n    {\\n        return chaos_create_raw_span_int32(nullptr, 0, false);\\n    }',
            '"extern \\"C\\" CHAOS_IL2CPP_INTPTR " + GetExternalRuntimeHelperSymbol(callee) + "(CHAOS_IL2CPP_INTPTR chaos_arg_0, CHAOS_IL2CPP_INT32 chaos_arg_1)\\n{\\n    const auto chaos_span_handle = *chaos_resolve_native_int_slot(chaos_arg_0);\\n    return chaos_span_int32_get_item_address(chaos_span_handle, chaos_arg_1);\\n}"',
            '"extern \\"C\\" CHAOS_IL2CPP_INT32 " + GetExternalRuntimeHelperSymbol(callee) + "(CHAOS_IL2CPP_INTPTR chaos_arg_0)\\n{\\n    const auto chaos_span_handle = *chaos_resolve_native_int_slot(chaos_arg_0);\\n    return chaos_span_int32_get_length(chaos_span_handle);\\n}"',
            '"extern \\"C\\" void " + GetExternalRuntimeHelperSymbol(callee) + "(CHAOS_IL2CPP_INTPTR chaos_arg_0, CHAOS_IL2CPP_INTPTR chaos_arg_1, CHAOS_IL2CPP_INT32 chaos_arg_2)\\n{\\n    auto* chaos_span_slot = chaos_resolve_native_int_slot(chaos_arg_0);\\n    *chaos_span_slot = chaos_create_raw_span_int32(reinterpret_cast<void*>(chaos_arg_1), chaos_arg_2, false);\\n}"',
            '"extern \\"C\\" CHAOS_IL2CPP_INTPTR " + GetExternalRuntimeHelperSymbol(callee) + "(CHAOS_IL2CPP_INTPTR chaos_arg_0)\\n{\\n    const auto chaos_memory_handle = *chaos_resolve_native_int_slot(chaos_arg_0);\\n    return chaos_memory_int32_get_span(chaos_memory_handle);\\n}"',
            '"extern \\"C\\" CHAOS_IL2CPP_INTPTR " + GetExternalRuntimeHelperSymbol(callee) + "(CHAOS_IL2CPP_INTPTR chaos_arg_0, CHAOS_IL2CPP_INTPTR chaos_arg_1)\\n{\\n    return chaos_reflection_get_type_from_assembly(chaos_arg_0, chaos_arg_1);\\n}"',
            '"extern \\"C\\" CHAOS_IL2CPP_INTPTR " + GetExternalRuntimeHelperSymbol(callee) + "(CHAOS_IL2CPP_INTPTR chaos_arg_0)\\n{\\n    return chaos_reflection_get_type_by_name(chaos_arg_0);\\n}"',
            '"extern \\"C\\" CHAOS_IL2CPP_INTPTR " + GetExternalRuntimeHelperSymbol(callee) + "(CHAOS_IL2CPP_INTPTR chaos_arg_0, CHAOS_IL2CPP_INTPTR chaos_arg_1, CHAOS_IL2CPP_INTPTR chaos_arg_2)\\n{\\n    return chaos_reflection_invoke_method(chaos_arg_0, chaos_arg_1, chaos_arg_2);\\n}"',
            '"extern \\"C\\" CHAOS_IL2CPP_INT32 " + GetExternalRuntimeHelperSymbol(callee) + "(CHAOS_IL2CPP_INTPTR chaos_arg_0)\\n{\\n    return chaos_reflection_get_metadata_token(chaos_arg_0);\\n}"',
            '$"extern \\"C\\" CHAOS_IL2CPP_INT32 {GetExternalRuntimeHelperSymbol(callee)}(CHAOS_IL2CPP_INTPTR chaos_arg_0, CHAOS_IL2CPP_INTPTR chaos_arg_1, CHAOS_IL2CPP_INT32 chaos_arg_2)\\n{{\\n    (void)chaos_arg_2;\\n    if (chaos_arg_0 == static_cast<CHAOS_IL2CPP_INTPTR>(0) || chaos_arg_1 == static_cast<CHAOS_IL2CPP_INTPTR>(0))',
            '$"extern \\"C\\" CHAOS_IL2CPP_INTPTR {GetExternalRuntimeHelperSymbol(callee)}(CHAOS_IL2CPP_INTPTR chaos_arg_0)\\n{{\\n    return chaos_reflection_get_custom_attribute(chaos_arg_0, {GetTypeHandleLiteral(value)});\\n}}"',
            '$"extern \\"C\\" CHAOS_IL2CPP_INTPTR {GetExternalRuntimeHelperSymbol(callee)}(CHAOS_IL2CPP_INTPTR chaos_arg_0)\\n{{\\n    if (chaos_arg_0 == static_cast<CHAOS_IL2CPP_INTPTR>(0))\\n    {{\\n        return static_cast<CHAOS_IL2CPP_INTPTR>(0);\\n    }}\\n\\n    auto* chaos_attribute = reinterpret_cast<{GetNativeTypeSymbol(declaringTypeSubjectId)}*>(chaos_arg_0);',
            '"extern \\"C\\" CHAOS_IL2CPP_INTPTR " + GetExternalRuntimeHelperSymbol(callee) + "()\\n{\\n    return chaos_async_task_int32_create();\\n}"',
            '"extern \\"C\\" CHAOS_IL2CPP_INTPTR " + GetExternalRuntimeHelperSymbol(callee) + "(CHAOS_IL2CPP_INTPTR chaos_arg_0)\\n{\\n    return chaos_async_task_int32_builder_get_task(chaos_arg_0);\\n}"',
            '"extern \\"C\\" void " + GetExternalRuntimeHelperSymbol(callee) + "(CHAOS_IL2CPP_INTPTR chaos_arg_0, CHAOS_IL2CPP_INT32 chaos_arg_1)\\n{\\n    chaos_async_task_int32_builder_set_result(chaos_arg_0, chaos_arg_1);\\n}"',
            '"extern \\"C\\" void " + GetExternalRuntimeHelperSymbol(callee) + "(CHAOS_IL2CPP_INTPTR chaos_arg_0, CHAOS_IL2CPP_INTPTR chaos_arg_1)\\n{\\n    chaos_async_task_int32_builder_set_exception(chaos_arg_0, chaos_arg_1);\\n}"',
            '"extern \\"C\\" CHAOS_IL2CPP_INTPTR " + GetExternalRuntimeHelperSymbol(callee) + "()\\n{\\n    return chaos_async_yield_create();\\n}"',
            '"extern \\"C\\" CHAOS_IL2CPP_INTPTR " + GetExternalRuntimeHelperSymbol(callee) + "(CHAOS_IL2CPP_INTPTR chaos_arg_0)\\n{\\n    return chaos_async_yield_get_awaiter(chaos_arg_0);\\n}"',
            '"extern \\"C\\" CHAOS_IL2CPP_INTPTR " + GetExternalRuntimeHelperSymbol(callee) + "(CHAOS_IL2CPP_INTPTR chaos_arg_0)\\n{\\n    return chaos_async_yield_get_is_completed(chaos_arg_0);\\n}"',
            '"extern \\"C\\" void " + GetExternalRuntimeHelperSymbol(callee) + "(CHAOS_IL2CPP_INTPTR chaos_arg_0)\\n{\\n    chaos_async_yield_get_result(chaos_arg_0);\\n}"',
            '"extern \\"C\\" CHAOS_IL2CPP_INTPTR " + GetExternalRuntimeHelperSymbol(callee) + "(CHAOS_IL2CPP_INTPTR chaos_arg_0)\\n{\\n    return chaos_async_task_int32_get_awaiter(chaos_arg_0);\\n}"',
            '$"extern \\"C\\" CHAOS_IL2CPP_INTPTR {GetExternalRuntimeHelperSymbol(callee)}(CHAOS_IL2CPP_INTPTR chaos_arg_0)\\n{{\\n    const auto chaos_task_handle = *chaos_resolve_native_int_slot(chaos_arg_0);\\n    (void)chaos_require_async_task_int32(chaos_task_handle);\\n    return chaos_task_handle;\\n}}"',
            '"extern \\"C\\" CHAOS_IL2CPP_INTPTR " + GetExternalRuntimeHelperSymbol(callee) + "(CHAOS_IL2CPP_INTPTR chaos_arg_0)\\n{\\n    return chaos_async_task_int32_awaiter_get_is_completed(chaos_arg_0);\\n}"',
            '"extern \\"C\\" CHAOS_IL2CPP_INT32 " + GetExternalRuntimeHelperSymbol(callee) + "(CHAOS_IL2CPP_INTPTR chaos_arg_0)\\n{\\n    return chaos_async_task_int32_awaiter_get_result(chaos_arg_0);\\n}"',
            '"extern \\"C\\" CHAOS_IL2CPP_INT32 " + GetExternalRuntimeHelperSymbol(callee) + "()\\n{\\n    return chaos_current_managed_thread_id;\\n}"',
            '"extern \\"C\\" void " + GetExternalRuntimeHelperSymbol(callee) + "(CHAOS_IL2CPP_INTPTR chaos_arg_0, CHAOS_IL2CPP_INTPTR chaos_arg_1)\\n{\\n    auto& chaos_thread_entry = chaos_require_thread_runtime_entry(chaos_arg_0);\\n    std::lock_guard<std::mutex> chaos_guard(chaos_thread_entry.mutex);\\n    chaos_thread_entry.thread_start_delegate = chaos_arg_1;',
            '"extern \\"C\\" void " + GetExternalRuntimeHelperSymbol(callee) + "(CHAOS_IL2CPP_INTPTR chaos_arg_0)\\n{\\n    auto& chaos_thread_entry = chaos_require_thread_runtime_entry(chaos_arg_0);\\n    std::thread* chaos_worker = nullptr;',
            '"extern \\"C\\" CHAOS_IL2CPP_INTPTR " + GetExternalRuntimeHelperSymbol(callee) + "(CHAOS_IL2CPP_INTPTR chaos_arg_0)\\n{\\n    auto& chaos_thread_entry = chaos_require_thread_runtime_entry(chaos_arg_0);\\n    std::lock_guard<std::mutex> chaos_guard(chaos_thread_entry.mutex);\\n    return chaos_thread_entry.name;\\n}"',
            '"extern \\"C\\" void " + GetExternalRuntimeHelperSymbol(callee) + "(CHAOS_IL2CPP_INTPTR chaos_arg_0, CHAOS_IL2CPP_INTPTR chaos_arg_1)\\n{\\n    auto& chaos_thread_entry = chaos_require_thread_runtime_entry(chaos_arg_0);\\n    std::lock_guard<std::mutex> chaos_guard(chaos_thread_entry.mutex);\\n    chaos_thread_entry.name = chaos_arg_1;\\n}"',
            '"extern \\"C\\" void " + GetExternalRuntimeHelperSymbol(callee) + "()\\n{\\n    CHAOS_IL2CPP_ABORT();\\n}"',
            'helperDefinition = new ExternalRuntimeHelperDefinition(callee, GetExternalRuntimeHelperSymbol(callee), "extern \\"C\\" void " + GetExternalRuntimeHelperSymbol(callee) + "(CHAOS_IL2CPP_INTPTR chaos_arg_0)\\n{\\n    (void)chaos_arg_0;\\n}", new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType))',
        ]:
            self.assertNotIn(legacy_fragment, collection_runtime_source)

        invocation_planning_source = NATIVE_AOT_INVOCATION_PLANNING_PATH.read_text(encoding="utf-8")
        self.assertIn("sealed partial class NativeAotLoweringPlanner", invocation_planning_source)

        for required_fragment in [
            "private IReadOnlyList<AotCoreIrMethodArtifact> CollectReachableMethods(",
            "private IReadOnlyList<AotCoreIrMethodArtifact> ResolveReachableMethods(",
            "private IReadOnlyList<AotCoreIrMethodArtifact> ResolveDirectReachableMethods(",
            "private IReadOnlyList<AotCoreIrMethodArtifact> ResolveVirtualDispatchTargets(",
            "private InvocationTarget ResolveDirectInvocationTarget(",
            "private InvocationTarget? TryResolveDirectInvocationTarget(",
            "private AotCoreIrMethodArtifact? TryGetLowerableMethod(",
        ]:
            self.assertIn(required_fragment, invocation_planning_source)

    def test_native_aot_assertion_runtime_helpers_use_generic_external_runtime_template(self) -> None:
        catalog_source = NATIVE_AOT_TEMPLATE_CATALOG_PATH.read_text(encoding="utf-8")
        planner_source = NATIVE_AOT_PLANNER_PATH.read_text(encoding="utf-8")
        object_model_source = NATIVE_AOT_OBJECT_MODEL_EMISSION_PATH.read_text(encoding="utf-8")
        assertion_runtime_source = NATIVE_AOT_EXTERNAL_RUNTIME_ASSERTIONS_PATH.read_text(encoding="utf-8")
        external_helpers_source = NATIVE_AOT_EXTERNAL_RUNTIME_HELPERS_PATH.read_text(encoding="utf-8")
        registry_source = NATIVE_AOT_RUNTIME_HELPER_SHAPE_REGISTRY_PATH.read_text(encoding="utf-8")
        combined_search = planner_source + "\n" + assertion_runtime_source + "\n" + registry_source
        self.assertTrue(
            NATIVE_AOT_EXTERNAL_RUNTIME_ASSERTIONS_PATH.is_file(),
            msg=f"missing assertion helper source: {NATIVE_AOT_EXTERNAL_RUNTIME_ASSERTIONS_PATH}",
        )
        self.assertFalse(
            TEST_FRAMEWORK_SUBJECT_FAMILY_PATH.is_file(),
            msg=f"legacy TestFramework subject helper should be removed: {TEST_FRAMEWORK_SUBJECT_FAMILY_PATH}",
        )
        self.assertFalse(
            NATIVE_AOT_TESTFRAMEWORK_EQUAL_TEMPLATE_PATH.is_file(),
            msg=f"legacy TestFramework template asset should be removed: {NATIVE_AOT_TESTFRAMEWORK_EQUAL_TEMPLATE_PATH}",
        )

        for required_fragment in [
            "TryParseKnownEqualityAssertionContract(",
            "CreateEqualityAssertionRuntimeHelperBodyLines(",
            "IsSupportedEqualityAssertionAbi(",
            "RenderSimpleExternalRuntimeHelper(",
            "new HashSet<string>(StringComparer.Ordinal)",
            "ManagedNaming.CreateFieldSubjectId(",
        ]:
            self.assertIn(required_fragment, combined_search)

        for forbidden_fragment in [
            'TestFrameworkEqualTemplateRelativePath = "Templates/NativeAot.TestFrameworkEqual.cpp.scriban"',
            "public static Template GetTestFrameworkEqualTemplate()",
            "NativeAotTemplateCatalog.GetTestFrameworkEqualTemplate()",
            "ScribanTemplateRenderer.RenderTemplate(",
            "TestFrameworkSubjectFamily",
            "TryCreateTestFrameworkRuntimeHelperDefinition(",
            "TryParseTestFrameworkEqualSubject(",
            "TryReadTestFrameworkEqualComparedType(",
            "CreateTestFrameworkEqualRuntimeHelperDefinition(",
            "CreateTestFrameworkEqualRuntimeHelperBodyLines(",
            "IsSupportedTestFrameworkGenericEqualAbi(",
            "TestFrameworkEqualSubject",
        ]:
            self.assertNotIn(forbidden_fragment, catalog_source + "\n" + assertion_runtime_source + "\n" + planner_source)

        for forbidden_fragment in [
            "LegacyTestFramework",
            "SdkTestFramework",
            "new System.Text.StringBuilder()",
            "sourceBuilder.Append(",
            "sourceBuilder.AppendLine(",
        ]:
            self.assertNotIn(forbidden_fragment, assertion_runtime_source)

        self.assertNotIn("TryGetTestFrameworkAssertStateFieldSubjectId(instruction.Callee", object_model_source)

    def test_native_aot_async_runtime_helpers_use_open_generic_families(self) -> None:
        object_model_utilities_source = NATIVE_AOT_OBJECT_MODEL_UTILITIES_PATH.read_text(encoding="utf-8")
        collection_runtime_source = NATIVE_AOT_EXTERNAL_RUNTIME_COLLECTION_AND_REFLECTION_PATH.read_text(
            encoding="utf-8"
        )
        type_resolution_source = NATIVE_AOT_EXTERNAL_RUNTIME_TYPE_RESOLUTION_PATH.read_text(encoding="utf-8")
        async_runtime_prelude_source = NATIVE_AOT_ASYNC_RUNTIME_PRELUDE_TEMPLATE_PATH.read_text(encoding="utf-8")

        for required_fragment in [
            'StartsWith("System.Private.CoreLib/System.Runtime.CompilerServices.AsyncTaskMethodBuilder<"',
            'StartsWith("System.Private.CoreLib/System.Runtime.CompilerServices.AsyncValueTaskMethodBuilder<"',
            'StartsWith("System.Private.CoreLib/System.Runtime.CompilerServices.TaskAwaiter<"',
            'StartsWith("System.Private.CoreLib/System.Runtime.CompilerServices.ValueTaskAwaiter<"',
            'StartsWith("System.Private.CoreLib/System.Threading.Tasks.Task<"',
            'StartsWith("System.Private.CoreLib/System.Threading.Tasks.ValueTask<"',
        ]:
            self.assertIn(required_fragment, object_model_utilities_source)

        for required_fragment in [
            "ResolveInterfaceDispatchTargets(",
            "MatchesMethodSubject(",
            "CreateVoidAbiSlot()",
        ]:
            self.assertIn(required_fragment, collection_runtime_source)

        for forbidden_fragment in [
            "TryCreateSupportedAsyncRuntimeResultShape(",
            "AotCoreIrAbiCarrierKind.Int8 =>",
            "AotCoreIrAbiCarrierKind.UInt8 =>",
            "AotCoreIrAbiCarrierKind.Int16 =>",
            "AotCoreIrAbiCarrierKind.UInt16 =>",
            "AotCoreIrAbiCarrierKind.Float32 =>",
            "AotCoreIrAbiCarrierKind.Float64 =>",
            "AotCoreIrAbiCarrierKind.Int64 =>",
            "AotCoreIrAbiCarrierKind.UInt64 =>",
            'chaos_store_float32(',
            'chaos_load_float32(',
            'chaos_store_float64(',
            'chaos_load_float64(',
            'chaos_store_int64(',
            'chaos_load_int64(',
            'chaos_store_uint64(',
            'chaos_load_uint64(',
            'TryCreateAsyncBuilderCreateHelperDefinition(callee, "System.Private.CoreLib/System.Runtime.CompilerServices.AsyncTaskMethodBuilder"',
            'TryCreateAsyncBuilderCreateHelperDefinition(callee, "System.Private.CoreLib/System.Runtime.CompilerServices.AsyncValueTaskMethodBuilder"',
            'TryCreateAsyncTaskGetAwaiterHelperDefinition(callee, "System.Private.CoreLib/System.Threading.Tasks.Task"',
            'TryCreateAsyncTaskGetAwaiterHelperDefinition(callee, "System.Private.CoreLib/System.Threading.Tasks.ValueTask"',
            'TryCreateAsyncAwaiterGetResultHelperDefinition(callee, "System.Private.CoreLib/System.Runtime.CompilerServices.TaskAwaiter"',
            'TryCreateAsyncAwaiterGetResultHelperDefinition(callee, "System.Private.CoreLib/System.Runtime.CompilerServices.ValueTaskAwaiter"',
            'TryReadSingleGenericTypeArgument(callee, openGenericBuilderTypePrefix + "<", out string resultTypeName)',
            'TryReadSingleGenericTypeArgument(callee, openGenericAwaiterTypePrefix + "<", out string resultTypeName)',
        ]:
            self.assertNotIn(forbidden_fragment, collection_runtime_source)

        self.assertNotIn(
            "if (resultAbi.CarrierKindCode != AotCoreIrAbiCarrierKind.Int32 && resultAbi.CarrierKindCode != AotCoreIrAbiCarrierKind.NativeInt)",
            collection_runtime_source,
        )

        for required_fragment in [
            'TryReadSingleGenericTypeArgument(callee, openGenericBuilderTypePrefix + "<", out string builderResultTypeNameValue)',
            'string marker = $"{openGenericBuilderTypePrefix}<{builderResultTypeNameValue}>::Start<";',
            'string marker = $"{openGenericBuilderTypePrefix}<{builderResultTypeNameValue}>::AwaitUnsafeOnCompleted<";',
        ]:
            self.assertIn(required_fragment, type_resolution_source)

        for required_fragment in [
            "struct chaos_async_task",
            "chaos_async_task_builder_set_result_raw",
            "chaos_async_task_awaiter_get_result_raw",
        ]:
            self.assertIn(required_fragment, async_runtime_prelude_source)

        for forbidden_fragment in [
            "AsyncTaskMethodBuilder<System.Int32>",
            "AsyncValueTaskMethodBuilder<System.Int32>",
            "TaskAwaiter<System.Int32>",
            "ValueTaskAwaiter<System.Int32>",
            "Task<System.Int32>::GetAwaiter()",
            "ValueTask<System.Int32>::GetAwaiter()",
        ]:
            self.assertNotIn(forbidden_fragment, object_model_utilities_source)
            self.assertNotIn(forbidden_fragment, collection_runtime_source)
            self.assertNotIn(forbidden_fragment, type_resolution_source)

        self.assertNotIn("chaos_async_task_int32", async_runtime_prelude_source)

    def test_native_aot_method_emission_helpers_are_split_from_root_planner(self) -> None:
        planner_source = NATIVE_AOT_PLANNER_PATH.read_text(encoding="utf-8")

        self.assertTrue(
            NATIVE_AOT_METHOD_EMISSION_PATH.is_file(),
            msg=f"missing method emission split file: {NATIVE_AOT_METHOD_EMISSION_PATH}",
        )

        for required_fragment in [
            "private static string FormatMethodDeclaration(",
            "private void EmitManagedMethod(",
            "private void EmitInstruction(",
            "private void EmitNewObject(",
            "private static void EmitNewArray(",
            "private static void EmitInitObj(",
            "private void EmitInstructionRange(",
        ]:
            self.assertNotIn(required_fragment, planner_source)

        method_emission_source = NATIVE_AOT_METHOD_EMISSION_PATH.read_text(encoding="utf-8")
        self.assertIn("sealed partial class NativeAotLoweringPlanner", method_emission_source)

        for required_fragment in [
            "private static string FormatMethodDeclaration(",
            "private void EmitManagedMethod(",
            "private void EmitInstruction(",
            "private void EmitNewObject(",
            "private static void EmitNewArray(",
            "private static void EmitInitObj(",
            "private void EmitInstructionRange(",
        ]:
            self.assertIn(required_fragment, method_emission_source)

    def test_native_aot_exception_emission_helpers_are_split_from_method_emission(self) -> None:
        method_emission_source = NATIVE_AOT_METHOD_EMISSION_PATH.read_text(encoding="utf-8")

        self.assertTrue(
            NATIVE_AOT_EXCEPTION_EMISSION_PATH.is_file(),
            msg=f"missing exception emission split file: {NATIVE_AOT_EXCEPTION_EMISSION_PATH}",
        )

        for required_fragment in [
            "private void EmitCatchOnlyExceptionMethodBody(",
            "private void EmitFilterOnlyExceptionMethodBody(",
            "private void EmitLinearInstructionSequence(",
            "private static IReadOnlyDictionary<int, int?> CreateNextOffsets(",
            "private static bool TryCreateCatchOnlyExceptionMethodShape(",
            "private static bool TryCreateFinallyHandlerEmissionPlan(",
        ]:
            self.assertNotIn(required_fragment, method_emission_source)

        exception_emission_source = NATIVE_AOT_EXCEPTION_EMISSION_PATH.read_text(encoding="utf-8")
        self.assertIn("sealed partial class NativeAotLoweringPlanner", exception_emission_source)

        for required_fragment in [
            "private void EmitCatchOnlyExceptionMethodBody(",
            "private void EmitFilterOnlyExceptionMethodBody(",
            "private void EmitLinearInstructionSequence(",
            "private static IReadOnlyDictionary<int, int?> CreateNextOffsets(",
            "private static bool TryCreateCatchOnlyExceptionMethodShape(",
            "private static bool TryCreateFinallyHandlerEmissionPlan(",
        ]:
            self.assertIn(required_fragment, exception_emission_source)

    def test_native_aot_external_runtime_helpers_are_split_from_root_planner(self) -> None:
        planner_source = NATIVE_AOT_PLANNER_PATH.read_text(encoding="utf-8")

        self.assertTrue(
            NATIVE_AOT_EXTERNAL_RUNTIME_HELPERS_PATH.is_file(),
            msg=f"missing external runtime helper split file: {NATIVE_AOT_EXTERNAL_RUNTIME_HELPERS_PATH}",
        )

        for required_fragment in [
            "private bool TryCreateExternalRuntimeHelperDefinition(",
            "private bool TryCreateStringRuntimeHelperDefinition(",
            "private bool TryCreateCollectionRuntimeHelperDefinition(",
            "private bool TryCreateAsyncRuntimeHelperDefinition(",
            "private string GetVirtualDispatchInstanceExpression(",
            "private static void EmitAbiArgumentInitialization(",
            "private static string FormatAbiInvocationArgumentList(",
        ]:
            self.assertNotIn(required_fragment, planner_source)

        external_runtime_helper_source = NATIVE_AOT_EXTERNAL_RUNTIME_HELPERS_PATH.read_text(encoding="utf-8")
        self.assertIn("sealed partial class NativeAotLoweringPlanner", external_runtime_helper_source)
        self.assertIn("private bool TryCreateExternalRuntimeHelperDefinition(", external_runtime_helper_source)
        for removed_fragment in [
            "private bool TryCreateStringRuntimeHelperDefinition(",
            "private bool TryCreateCollectionRuntimeHelperDefinition(",
            "private bool TryCreateAsyncRuntimeHelperDefinition(",
            "private string GetVirtualDispatchInstanceExpression(",
            "private static void EmitAbiArgumentInitialization(",
            "private static string FormatAbiInvocationArgumentList(",
        ]:
            self.assertNotIn(removed_fragment, external_runtime_helper_source)

    def test_native_aot_external_runtime_helpers_are_further_split_by_family(self) -> None:
        root_source = NATIVE_AOT_EXTERNAL_RUNTIME_HELPERS_PATH.read_text(encoding="utf-8")

        for required_path in [
            NATIVE_AOT_EXTERNAL_RUNTIME_STRING_AND_PLATFORM_PATH,
            NATIVE_AOT_EXTERNAL_RUNTIME_COLLECTION_AND_REFLECTION_PATH,
            NATIVE_AOT_EXTERNAL_RUNTIME_TYPE_RESOLUTION_PATH,
            NATIVE_AOT_EXTERNAL_RUNTIME_INVOCATION_ABI_PATH,
        ]:
            self.assertTrue(required_path.is_file(), msg=f"missing external runtime helper family split file: {required_path}")

        for required_fragment in [
            "private ExternalRuntimeHelperDefinition CreateStringJoinInt32EnumerableRuntimeHelperDefinition(",
            "private bool TryCreateCollectionRuntimeHelperDefinition(",
            "private static bool TryParseAsyncTaskBuilderStartStateMachineType(",
            "private string GetVirtualDispatchInstanceExpression(",
            "private static void EmitAbiArgumentInitialization(",
            "private static T LoadRequiredJson<T>(",
        ]:
            self.assertNotIn(required_fragment, root_source)

        string_and_platform_source = NATIVE_AOT_EXTERNAL_RUNTIME_STRING_AND_PLATFORM_PATH.read_text(encoding="utf-8")
        for required_fragment in [
            "private ExternalRuntimeHelperDefinition CreateStringJoinInt32EnumerableRuntimeHelperDefinition(",
            "private static string RenderSimpleExternalRuntimeHelper(",
            "MarshalCopyElementTypeMap",
        ]:
            self.assertIn(required_fragment, string_and_platform_source)

        for forbidden_fragment in [
            "private bool TryCreateStringRuntimeHelperDefinition(",
            "private bool TryCreateExceptionRuntimeHelperDefinition(",
            "private bool TryCreatePlatformRuntimeHelperDefinition(",
        ]:
            self.assertNotIn(forbidden_fragment, string_and_platform_source)

        collection_and_reflection_source = NATIVE_AOT_EXTERNAL_RUNTIME_COLLECTION_AND_REFLECTION_PATH.read_text(encoding="utf-8")
        for required_fragment in [
            "private bool TryCreateCustomAttributeRuntimeHelperDefinition(",
            "private IReadOnlyList<AotCoreIrMethodArtifact> ResolveInterfaceDispatchTargets(",
            "private static bool MatchesMethodSubject(",
            "CreateVoidAbiSlot()",
        ]:
            self.assertIn(required_fragment, collection_and_reflection_source)

        for forbidden_fragment in [
            "private bool TryCreateSpanRuntimeHelperDefinition(",
            "private bool TryCreateCollectionRuntimeHelperDefinition(",
            "private bool TryCreateAsyncRuntimeHelperDefinition(",
        ]:
            self.assertNotIn(forbidden_fragment, collection_and_reflection_source)

        type_resolution_source = NATIVE_AOT_EXTERNAL_RUNTIME_TYPE_RESOLUTION_PATH.read_text(encoding="utf-8")
        for required_fragment in [
            "private static bool TryParseAsyncTaskBuilderStartStateMachineType(",
            "private static IReadOnlyList<string> SplitTopLevelGenericArguments(",
            "private bool TryResolveReferenceTypeSubjectId(",
        ]:
            self.assertIn(required_fragment, type_resolution_source)

        invocation_abi_source = NATIVE_AOT_EXTERNAL_RUNTIME_INVOCATION_ABI_PATH.read_text(encoding="utf-8")
        for required_fragment in [
            "private string GetVirtualDispatchTargetTypeIdSymbol(",
            "private string GetVirtualDispatchInstanceExpression(",
            "private static void EmitMethodReturn(",
            "private static void EmitAbiArgumentInitialization(",
            "private static string FormatAbiInvocationArgumentList(",
            "private static T LoadRequiredJson<T>(",
        ]:
            self.assertIn(required_fragment, invocation_abi_source)

    def test_native_aot_object_model_emission_helpers_are_split_from_root_planner(self) -> None:
        planner_source = NATIVE_AOT_PLANNER_PATH.read_text(encoding="utf-8")

        self.assertTrue(
            NATIVE_AOT_OBJECT_MODEL_EMISSION_PATH.is_file(),
            msg=f"missing object model emission split file: {NATIVE_AOT_OBJECT_MODEL_EMISSION_PATH}",
        )

        for required_fragment in [
            "private void EmitObjectModelDeclarations(",
            "private void EmitObjectEqualityHelpers(",
            "private void EmitReflectionObjectHelpers(",
            "private static IReadOnlyList<string> GetReferenceTypeEmissionOrder(",
            "private IReadOnlyList<AotCoreIrAbiSlotArtifact> ResolveDelegateInvokeParameterAbis(",
            "private AotCoreIrAbiSlotArtifact ResolveDelegateInvokeReturnAbi(",
            "private static string GetRuntimeTypeIdExpression(",
        ]:
            self.assertNotIn(required_fragment, planner_source)

        object_model_emission_source = NATIVE_AOT_OBJECT_MODEL_EMISSION_PATH.read_text(encoding="utf-8")
        self.assertIn("sealed partial class NativeAotLoweringPlanner", object_model_emission_source)
        self.assertIn("private void EmitObjectModelDeclarations(", object_model_emission_source)
        for removed_fragment in [
            "private void EmitObjectEqualityHelpers(",
            "private void EmitReflectionObjectHelpers(",
            "private static IReadOnlyList<string> GetReferenceTypeEmissionOrder(",
            "private IReadOnlyList<AotCoreIrAbiSlotArtifact> ResolveDelegateInvokeParameterAbis(",
            "private AotCoreIrAbiSlotArtifact ResolveDelegateInvokeReturnAbi(",
            "private static string GetRuntimeTypeIdExpression(",
        ]:
            self.assertNotIn(removed_fragment, object_model_emission_source)

    def test_native_aot_object_model_emission_is_further_split_by_responsibility(self) -> None:
        object_model_source = NATIVE_AOT_OBJECT_MODEL_EMISSION_PATH.read_text(encoding="utf-8")

        for required_path in [
            NATIVE_AOT_OBJECT_EQUALITY_EMISSION_PATH,
            NATIVE_AOT_REFLECTION_OBJECT_EMISSION_PATH,
            NATIVE_AOT_OBJECT_MODEL_UTILITIES_PATH,
        ]:
            self.assertTrue(required_path.is_file(), msg=f"missing object-model split file: {required_path}")

        for required_fragment in [
            "private void EmitObjectEqualityHelpers(",
            "private void EmitReflectionObjectHelpers(",
            "private void EmitExceptionMetadataHelpers(",
            "private static IReadOnlyList<string> GetReferenceTypeEmissionOrder(",
            "private static string GetRuntimeTypeIdExpression(",
        ]:
            self.assertNotIn(required_fragment, object_model_source)

        object_equality_source = NATIVE_AOT_OBJECT_EQUALITY_EMISSION_PATH.read_text(encoding="utf-8")
        self.assertIn("private void EmitObjectEqualityHelpers(", object_equality_source)

        reflection_object_source = NATIVE_AOT_REFLECTION_OBJECT_EMISSION_PATH.read_text(encoding="utf-8")
        for required_fragment in [
            "private void EmitReflectionObjectHelpers(",
            "private void EmitExceptionMetadataHelpers(",
            "private bool RequiresManagedStringFactory(",
            "private bool RequiresManagedStringRuntime(",
        ]:
            self.assertIn(required_fragment, reflection_object_source)

        object_model_utilities_source = NATIVE_AOT_OBJECT_MODEL_UTILITIES_PATH.read_text(encoding="utf-8")
        for required_fragment in [
            "private static IReadOnlyList<string> GetReferenceTypeEmissionOrder(",
            "private IReadOnlyList<AotCoreIrAbiSlotArtifact> ResolveDelegateInvokeParameterAbis(",
            "private static string GetRuntimeTypeIdExpression(",
            "private static string FormatGenericArgumentList(",
        ]:
            self.assertIn(required_fragment, object_model_utilities_source)

    def test_native_aot_generic_method_aot_registration_is_wired_to_codegen_and_contract(self) -> None:
        generic_registration_source = NATIVE_AOT_GENERIC_REGISTRATION_PATH.read_text(encoding="utf-8")
        codegen_bridge_source = CODEGEN_BRIDGE_HEADER_PATH.read_text(encoding="utf-8")

        self.assertTrue(
            NATIVE_AOT_GENERIC_REGISTRATION_PATH.is_file(),
            msg=f"missing generic registration split file: {NATIVE_AOT_GENERIC_REGISTRATION_PATH}",
        )

        # The codegen must define BuildMethodAotRegistration to build AOT entries.
        self.assertIn(
            "private void BuildMethodAotRegistration(",
            generic_registration_source,
        )

        # The codegen must reference GenericMethodAotEntryV0 (the contract struct).
        self.assertIn(
            "GenericMethodAotEntryV0",
            generic_registration_source,
        )

        # The emitted AOT registration code must call RegisterMethodAotEntries.
        self.assertIn(
            "RegisterMethodAotEntries(",
            generic_registration_source,
        )

        # The module registration fields must include method_aot_entries pointers.
        self.assertIn(
            ".method_aot_entries",
            generic_registration_source,
        )

        # The contract header must define GenericMethodAotEntryV0 struct.
        self.assertIn(
            "struct GenericMethodAotEntryV0",
            codegen_bridge_source,
        )

        # The contract header must define ModuleGenericRegistrationV0 with
        # method_aot_entries fields (wired to the GenericMethodAotEntryV0[] arrays).
        self.assertIn(
            "method_aot_entries",
            codegen_bridge_source,
        )

        # The codegen must reference the runtime_instantiation.h include
        # (which declares RegisterMethodAotEntries and QueryAotMethod).
        planner_source = NATIVE_AOT_PLANNER_PATH.read_text(encoding="utf-8")
        self.assertIn(
            "runtime_instantiation.h",
            planner_source,
        )
