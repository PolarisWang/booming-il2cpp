from tests.unit.compatibility.il2cpp_codegen_structure_governance_test_support import *


class TestIl2CppCodeGenStructureGovernanceNativeAotPlanning(Il2CppCodeGenStructureGovernanceTestSupport):
    def test_native_aot_runtime_support_helpers_are_split_from_root_planner(self) -> None:
        planner_source = NATIVE_AOT_PLANNER_PATH.read_text(encoding="utf-8")

        self.assertTrue(
            NATIVE_AOT_RUNTIME_PRELUDE_PATH.is_file(),
            msg=f"missing runtime support split file: {NATIVE_AOT_RUNTIME_PRELUDE_PATH}",
        )
        self.assertTrue(
            NATIVE_AOT_RUNTIME_PRELUDE_AGGREGATOR_PATH.is_file(),
            msg=f"missing runtime prelude aggregator split file: {NATIVE_AOT_RUNTIME_PRELUDE_AGGREGATOR_PATH}",
        )
        for required_path in [
            NATIVE_AOT_RUNTIME_PRELUDE_TEMPLATE_PATH,
            NATIVE_AOT_SPAN_RUNTIME_PRELUDE_TEMPLATE_PATH,
            NATIVE_AOT_COLLECTION_RUNTIME_PRELUDE_TEMPLATE_PATH,
            NATIVE_AOT_MONITOR_RUNTIME_PRELUDE_TEMPLATE_PATH,
            NATIVE_AOT_THREAD_RUNTIME_PRELUDE_TEMPLATE_PATH,
            NATIVE_AOT_ASYNC_RUNTIME_PRELUDE_TEMPLATE_PATH,
        ]:
            self.assertTrue(required_path.is_file(), msg=f"missing NativeAot runtime support template: {required_path}")
        self.assertIn("public sealed partial class NativeAotLoweringPlanner", planner_source)
        self.assertNotIn("private static void EmitCollectionRuntimePrelude(", planner_source)
        self.assertNotIn("private static void EmitMonitorRuntimePrelude(", planner_source)
        self.assertNotIn("private static void EmitThreadRuntimePrelude(", planner_source)
        self.assertNotIn("private static void EmitAsyncRuntimePrelude(", planner_source)
        self.assertNotIn("private static void EmitRuntimePrelude(", planner_source)
        self.assertNotIn("private static void EmitSpanRuntimePrelude(", planner_source)

        runtime_prelude_source = NATIVE_AOT_RUNTIME_PRELUDE_PATH.read_text(encoding="utf-8")
        self.assertIn("sealed partial class NativeAotLoweringPlanner", runtime_prelude_source)

        for required_fragment in [
            "private static void EmitCollectionRuntimePrelude(",
            "private static void EmitMonitorRuntimePrelude(",
            "private static void EmitThreadRuntimePrelude(",
            "private static void EmitAsyncRuntimePrelude(",
            "NativeAotTemplateCatalog.GetCollectionRuntimePreludeTemplate()",
            "NativeAotTemplateCatalog.GetMonitorRuntimePreludeTemplate()",
            "NativeAotTemplateCatalog.GetThreadRuntimePreludeTemplate()",
            "NativeAotTemplateCatalog.GetAsyncRuntimePreludeTemplate()",
        ]:
            self.assertIn(required_fragment, runtime_prelude_source)

        runtime_prelude_aggregator_source = NATIVE_AOT_RUNTIME_PRELUDE_AGGREGATOR_PATH.read_text(encoding="utf-8")
        self.assertIn("sealed partial class NativeAotLoweringPlanner", runtime_prelude_aggregator_source)
        self.assertIn("private static void EmitRuntimePrelude(", runtime_prelude_aggregator_source)
        self.assertIn("private static void EmitSpanRuntimePrelude(", runtime_prelude_aggregator_source)
        self.assertIn("NativeAotTemplateCatalog.GetRuntimePreludeTemplate()", runtime_prelude_aggregator_source)
        self.assertIn("NativeAotTemplateCatalog.GetSpanRuntimePreludeTemplate()", runtime_prelude_aggregator_source)

    def test_native_aot_static_initialization_emission_prefers_scriban_templates(self) -> None:
        catalog_source = NATIVE_AOT_TEMPLATE_CATALOG_PATH.read_text(encoding="utf-8")
        static_initialization_source = NATIVE_AOT_STATIC_INITIALIZATION_EMISSION_PATH.read_text(encoding="utf-8")

        for required_path in [
            NATIVE_AOT_STATIC_INITIALIZATION_DEFINITION_TEMPLATE_PATH,
            NATIVE_AOT_STATIC_INITIALIZATION_ACTION_TEMPLATE_PATH,
            NATIVE_AOT_STATIC_INITIALIZATION_CALL_TEMPLATE_PATH,
        ]:
            self.assertTrue(
                required_path.is_file(),
                msg=f"missing NativeAot static initialization template: {required_path}",
            )

        for required_fragment in [
            'StaticInitializationDefinitionTemplateRelativePath = "Templates/NativeAot.StaticInitializationDefinition.cpp.scriban"',
            'StaticInitializationActionTemplateRelativePath = "Templates/NativeAot.StaticInitializationAction.cpp.scriban"',
            'StaticInitializationCallTemplateRelativePath = "Templates/NativeAot.StaticInitializationCall.cpp.scriban"',
            "public static Template GetStaticInitializationDefinitionTemplate()",
            "public static Template GetStaticInitializationActionTemplate()",
            "public static Template GetStaticInitializationCallTemplate()",
        ]:
            self.assertIn(required_fragment, catalog_source)

        for required_fragment in [
            "NativeAotTemplateCatalog.GetStaticInitializationDefinitionTemplate()",
            "NativeAotTemplateCatalog.GetStaticInitializationActionTemplate()",
            "NativeAotTemplateCatalog.GetStaticInitializationCallTemplate()",
            "ScribanTemplateRenderer.RenderTemplate(",
        ]:
            self.assertIn(required_fragment, static_initialization_source)

        for legacy_fragment in [
            'builder.AppendLine($"CHAOS_IL2CPP_ONCE_FLAG',
            'builder.AppendLine($"{indentation}if (',
            'builder.AppendLine($"{indentation}    auto* chaos_object = new',
        ]:
            self.assertNotIn(legacy_fragment, static_initialization_source)

    def test_native_aot_metadata_support_builders_are_split_from_root_planner(self) -> None:
        planner_source = NATIVE_AOT_PLANNER_PATH.read_text(encoding="utf-8")

        self.assertTrue(
            NATIVE_AOT_METADATA_SUPPORT_PATH.is_file(),
            msg=f"missing metadata support split file: {NATIVE_AOT_METADATA_SUPPORT_PATH}",
        )

        for required_fragment in [
            "private CustomAttributeSupportModel BuildCustomAttributeSupportModel(",
            "private AssemblyReflectionSupportModel BuildAssemblyReflectionSupportModel(",
            "private ReflectionMemberSupportModel BuildReflectionMemberSupportModel(",
            "private StaticFieldDataSupportModel BuildStaticFieldDataSupportModel(",
        ]:
            self.assertNotIn(required_fragment, planner_source)

        metadata_support_source = NATIVE_AOT_METADATA_SUPPORT_PATH.read_text(encoding="utf-8")
        self.assertIn("sealed partial class NativeAotLoweringPlanner", metadata_support_source)

        for required_fragment in [
            "private CustomAttributeSupportModel BuildCustomAttributeSupportModel(",
            "private AssemblyReflectionSupportModel BuildAssemblyReflectionSupportModel(",
            "private ReflectionMemberSupportModel BuildReflectionMemberSupportModel(",
            "private StaticFieldDataSupportModel BuildStaticFieldDataSupportModel(",
        ]:
            self.assertIn(required_fragment, metadata_support_source)

    def test_native_aot_invocation_abi_helpers_prefer_scriban_templates(self) -> None:
        catalog_source = NATIVE_AOT_TEMPLATE_CATALOG_PATH.read_text(encoding="utf-8")
        invocation_abi_source = NATIVE_AOT_INVOCATION_ABI_PATH.read_text(encoding="utf-8")

        for required_path in [
            NATIVE_AOT_METHOD_RETURN_TEMPLATE_PATH,
            NATIVE_AOT_ABI_RETURN_PUSH_TEMPLATE_PATH,
            NATIVE_AOT_ABI_ARGUMENT_INITIALIZATION_TEMPLATE_PATH,
        ]:
            self.assertTrue(
                required_path.is_file(),
                msg=f"missing NativeAot invocation ABI template: {required_path}",
            )

        for required_fragment in [
            'MethodReturnTemplateRelativePath = "Templates/NativeAot.MethodReturn.cpp.scriban"',
            'AbiReturnPushTemplateRelativePath = "Templates/NativeAot.AbiReturnPush.cpp.scriban"',
            'AbiArgumentInitializationTemplateRelativePath = "Templates/NativeAot.AbiArgumentInitialization.cpp.scriban"',
            "public static Template GetMethodReturnTemplate()",
            "public static Template GetAbiReturnPushTemplate()",
            "public static Template GetAbiArgumentInitializationTemplate()",
        ]:
            self.assertIn(required_fragment, catalog_source)

        for required_fragment in [
            "NativeAotTemplateCatalog.GetMethodReturnTemplate()",
            "NativeAotTemplateCatalog.GetAbiReturnPushTemplate()",
            "NativeAotTemplateCatalog.GetAbiArgumentInitializationTemplate()",
            "ScribanTemplateRenderer.RenderTemplate(",
        ]:
            self.assertIn(required_fragment, invocation_abi_source)

        for legacy_fragment in [
            'builder.AppendLine("    return;',
            'builder.AppendLine("    return static_cast<CHAOS_IL2CPP_INT32>(',
            'handler.AppendLiteral("chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(',
            'handler.AppendLiteral("    chaos_args[',
        ]:
            self.assertNotIn(legacy_fragment, invocation_abi_source)

    def test_native_aot_invocation_planning_helpers_are_split_from_root_planner(self) -> None:
        planner_source = NATIVE_AOT_PLANNER_PATH.read_text(encoding="utf-8")

        self.assertTrue(
            NATIVE_AOT_INVOCATION_PLANNING_PATH.is_file(),
            msg=f"missing invocation planning split file: {NATIVE_AOT_INVOCATION_PLANNING_PATH}",
        )

        for required_fragment in [
            "private IReadOnlyList<AotCoreIrMethodArtifact> CollectReachableMethods(",
            "private IReadOnlyList<AotCoreIrMethodArtifact> ResolveReachableMethods(",
            "private IReadOnlyList<AotCoreIrMethodArtifact> ResolveDirectReachableMethods(",
            "private IReadOnlyList<AotCoreIrMethodArtifact> ResolveVirtualDispatchTargets(",
            "private InvocationTarget ResolveDirectInvocationTarget(",
            "private InvocationTarget? TryResolveDirectInvocationTarget(",
            "private AotCoreIrMethodArtifact? TryGetLowerableMethod(",
        ]:
            self.assertNotIn(required_fragment, planner_source)

    def test_native_aot_string_join_runtime_helpers_prefer_scriban_templates(self) -> None:
        catalog_source = NATIVE_AOT_TEMPLATE_CATALOG_PATH.read_text(encoding="utf-8")
        string_runtime_source = NATIVE_AOT_STRING_AND_PLATFORM_PATH.read_text(encoding="utf-8")

        for required_path in [
            NATIVE_AOT_STRING_JOIN_INT32_ENUMERABLE_TEMPLATE_PATH,
            NATIVE_AOT_STRING_JOIN_STRING_ENUMERABLE_TEMPLATE_PATH,
            NATIVE_AOT_SIMPLE_EXTERNAL_RUNTIME_HELPER_TEMPLATE_PATH,
        ]:
            self.assertTrue(
                required_path.is_file(),
                msg=f"missing NativeAot string join template: {required_path}",
            )

        for required_fragment in [
            'StringJoinInt32EnumerableTemplateRelativePath = "Templates/NativeAot.StringJoinInt32Enumerable.cpp.scriban"',
            'StringJoinStringEnumerableTemplateRelativePath = "Templates/NativeAot.StringJoinStringEnumerable.cpp.scriban"',
            'SimpleExternalRuntimeHelperTemplateRelativePath = "Templates/NativeAot.SimpleExternalRuntimeHelper.cpp.scriban"',
            "public static Template GetStringJoinInt32EnumerableTemplate()",
            "public static Template GetStringJoinStringEnumerableTemplate()",
            "public static Template GetSimpleExternalRuntimeHelperTemplate()",
        ]:
            self.assertIn(required_fragment, catalog_source)

        for required_fragment in [
            "NativeAotTemplateCatalog.GetStringJoinInt32EnumerableTemplate()",
            "NativeAotTemplateCatalog.GetStringJoinStringEnumerableTemplate()",
            "NativeAotTemplateCatalog.GetSimpleExternalRuntimeHelperTemplate()",
            "ScribanTemplateRenderer.RenderTemplate(",
        ]:
            self.assertIn(required_fragment, string_runtime_source)

        for legacy_fragment in [
            '"extern \\"C\\" CHAOS_IL2CPP_INTPTR " + GetExternalRuntimeHelperSymbol(callee) + "(CHAOS_IL2CPP_INTPTR chaos_arg_0, CHAOS_IL2CPP_INTPTR chaos_arg_1)\\n{\\n    return chaos_reflection_concat_string_pair_values(chaos_arg_0, chaos_arg_1);\\n}"',
            '$"extern \\"C\\" CHAOS_IL2CPP_INTPTR {GetExternalRuntimeHelperSymbol(callee)}(CHAOS_IL2CPP_INTPTR chaos_arg_0, CHAOS_IL2CPP_INTPTR chaos_arg_1, CHAOS_IL2CPP_INT32 chaos_arg_2)\\n{{\\n    if (chaos_arg_2 != {4})',
            '"extern \\"C\\" CHAOS_IL2CPP_INTPTR " + GetExternalRuntimeHelperSymbol(callee) + "(CHAOS_IL2CPP_INTPTR chaos_arg_0)\\n{\\n    auto* chaos_value_slot = chaos_resolve_native_int_slot(chaos_arg_0);\\n    char chaos_buffer[64]{};\\n    std::snprintf(chaos_buffer, sizeof(chaos_buffer), \\"%d\\", static_cast<CHAOS_IL2CPP_INT32>(*chaos_value_slot));\\n    return chaos_reflection_create_string_literal(chaos_buffer);\\n}"',
            'std::snprintf(chaos_buffer, sizeof(chaos_buffer), "%.1f", static_cast<double>(chaos_load_float32(chaos_arg_0)));',
            'std::snprintf(chaos_buffer, sizeof(chaos_buffer), "%.1f", chaos_load_float64(chaos_arg_0));',
            '"extern \\"C\\" void " + GetExternalRuntimeHelperSymbol(callee) + "(CHAOS_IL2CPP_INTPTR chaos_arg_0, CHAOS_IL2CPP_INTPTR chaos_arg_1)\\n{\\n    chaos_reflection_set_exception_metadata(',
            '"extern \\"C\\" CHAOS_IL2CPP_INTPTR " + GetExternalRuntimeHelperSymbol(callee) + "(CHAOS_IL2CPP_INTPTR chaos_arg_0)\\n{\\n    return chaos_reflection_get_exception_message(chaos_arg_0);\\n}"',
            '"extern \\"C\\" void " + GetExternalRuntimeHelperSymbol(callee) + "(CHAOS_IL2CPP_INTPTR chaos_arg_0, CHAOS_IL2CPP_INT32 chaos_arg_1, CHAOS_IL2CPP_INT32 chaos_arg_2)\\n{\\n    (void)chaos_arg_1;\\n    (void)chaos_arg_2;\\n    chaos_default_interpolated_string_handler_reset(chaos_arg_0);\\n}"',
            '"extern \\"C\\" void " + GetExternalRuntimeHelperSymbol(callee) + "(CHAOS_IL2CPP_INTPTR chaos_arg_0, CHAOS_IL2CPP_INTPTR chaos_arg_1)\\n{\\n    chaos_default_interpolated_string_handler_append_string(chaos_arg_0, chaos_arg_1);\\n}"',
            '$"extern \\"C\\" CHAOS_IL2CPP_INTPTR {GetExternalRuntimeHelperSymbol(callee)}()\\n{{\\n{text}\\n}}"',
            '"extern \\"C\\" CHAOS_IL2CPP_INTPTR " + GetExternalRuntimeHelperSymbol(callee) + "(CHAOS_IL2CPP_INTPTR chaos_arg_0, CHAOS_IL2CPP_INTPTR chaos_arg_1)\\n{\\n    return chaos_delegate_combine(chaos_arg_0, chaos_arg_1);\\n}"',
            '$"extern \\"C\\" CHAOS_IL2CPP_INT32 {GetExternalRuntimeHelperSymbol(callee)}()\\n{{\\n    return static_cast<CHAOS_IL2CPP_INT32>(sizeof({GetRequiredAbiValueTypeSymbol(valueTypeAbi)}));\\n}}"',
            '$"extern \\"C\\" CHAOS_IL2CPP_INTPTR {GetExternalRuntimeHelperSymbol(callee)}(CHAOS_IL2CPP_INT32 chaos_arg_0)\\n{{\\n    if (chaos_arg_0 < 0)',
            '"extern \\"C\\" void " + GetExternalRuntimeHelperSymbol(callee) + "(CHAOS_IL2CPP_INTPTR chaos_arg_0)\\n{\\n    if (chaos_arg_0 != static_cast<CHAOS_IL2CPP_INTPTR>(0))\\n    {\\n        CHAOS_IL2CPP_FREE(reinterpret_cast<void*>(chaos_arg_0));\\n    }\\n}"',
            '$"extern \\"C\\" void {GetExternalRuntimeHelperSymbol(callee)}({valueTypeSymbol} chaos_arg_0, CHAOS_IL2CPP_INTPTR chaos_arg_1, CHAOS_IL2CPP_INTPTR chaos_arg_2)\\n{{\\n    if (chaos_arg_1 == static_cast<CHAOS_IL2CPP_INTPTR>(0) || chaos_arg_2 != static_cast<CHAOS_IL2CPP_INTPTR>(0))',
            '$"extern \\"C\\" {valueTypeSymbol2} {GetExternalRuntimeHelperSymbol(callee)}(CHAOS_IL2CPP_INTPTR chaos_arg_0)\\n{{\\n    if (chaos_arg_0 == static_cast<CHAOS_IL2CPP_INTPTR>(0))',
            '$"extern \\"C\\" CHAOS_IL2CPP_INTPTR {GetExternalRuntimeHelperSymbol(callee)}(CHAOS_IL2CPP_INTPTR chaos_arg_0)\\n{{\\n    if (chaos_arg_0 == static_cast<CHAOS_IL2CPP_INTPTR>(0))',
            '"extern \\"C\\" CHAOS_IL2CPP_INTPTR " + GetExternalRuntimeHelperSymbol(callee) + "(CHAOS_IL2CPP_INTPTR chaos_arg_0)\\n{\\n    return chaos_arg_0 == static_cast<CHAOS_IL2CPP_INTPTR>(0)\\n        ? static_cast<CHAOS_IL2CPP_INTPTR>(0)\\n        : chaos_reflection_create_string_literal(reinterpret_cast<const char*>(chaos_arg_0));\\n}"',
            '"extern \\"C\\" CHAOS_IL2CPP_INTPTR " + GetExternalRuntimeHelperSymbol(callee) + "(CHAOS_IL2CPP_INTPTR chaos_arg_0, CHAOS_IL2CPP_INTPTR chaos_arg_1, CHAOS_IL2CPP_INTPTR chaos_arg_2)\\n{\\n    auto* chaos_slot = chaos_resolve_native_int_slot(chaos_arg_0);',
            'stringBuilder.AppendLine("    auto* chaos_enumerable_header = reinterpret_cast<chaos_object_header*>(chaos_arg_1);");',
            'stringBuilder.AppendLine("        while (true)");',
            'stringBuilder.AppendLine("            chaos_append_separator();");',
        ]:
            self.assertNotIn(legacy_fragment, string_runtime_source)

    def test_native_aot_generic_string_and_interpolation_helpers_use_family_recognition(self) -> None:
        invocation_planning_source = NATIVE_AOT_INVOCATION_PLANNING_PATH.read_text(encoding="utf-8")
        string_runtime_source = NATIVE_AOT_STRING_AND_PLATFORM_PATH.read_text(encoding="utf-8")
        reflection_object_source = NATIVE_AOT_REFLECTION_OBJECT_EMISSION_PATH.read_text(encoding="utf-8")
        object_model_utilities_source = NATIVE_AOT_OBJECT_MODEL_UTILITIES_PATH.read_text(encoding="utf-8")
        type_resolution_source = NATIVE_AOT_EXTERNAL_RUNTIME_TYPE_RESOLUTION_PATH.read_text(encoding="utf-8")

        for required_fragment in [
            "private static bool TryGetStringJoinEnumerableElementType(",
            "TryGetStringJoinEnumerableElementType(callee, out var elementTypeDisplayName)",
        ]:
            self.assertIn(required_fragment, invocation_planning_source)

        for required_fragment in [
            "TryGetStringJoinEnumerableElementType(callee, out var elementTypeDisplayName)",
            "IsSupportedDefaultInterpolatedStringHandlerAppendFormattedSubjectId(subjectId)",
        ]:
            self.assertIn(required_fragment, invocation_planning_source + "\n" + object_model_utilities_source + "\n" + type_resolution_source)

        self.assertIn(
            "TryGetStringJoinEnumerableElementType(instruction.Callee ?? string.Empty, out elementTypeDisplayName)",
            reflection_object_source,
        )
        self.assertIn(
            "IsSupportedDefaultInterpolatedStringHandlerAppendFormattedSubjectId(subjectId)",
            object_model_utilities_source,
        )
        for required_fragment in [
            "private static bool TryParseDefaultInterpolatedStringHandlerAppendFormattedType(",
            "private static bool IsSupportedDefaultInterpolatedStringHandlerAppendFormattedSubjectId(",
            "DefaultInterpolatedStringHandlerAppendFormattedMethodSubjectPrefix",
        ]:
            self.assertIn(required_fragment, type_resolution_source)
