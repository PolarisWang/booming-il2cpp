from tests.unit.planning.solution_core_pack_planner_test_support import *


class TestSolutionCorePackPlannerNativeProofA(SolutionCorePackPlannerTestSupport):
    def test_planner_surfaces_solution_core_pack_foundation_dll_translation_native_proof_matrix(self) -> None:
        planner_module = load_module(PLANNER_MODULE_PATH, "chaos_solution_core_pack_planner_foundation_dll_translation")

        plan = planner_module.build_plan(
            REPO_ROOT,
            "SolutionCorePack",
            matrix_id="windows-foundation-dll-translation-native-proof",
            run_id="20260419-solution-core-pack-foundation-dll-native-proof-001",
        )

        self.assertEqual("SolutionCorePack", plan["selection"]["subjectId"])
        self.assertEqual("windows-foundation-dll-translation-native-proof", plan["selection"]["matrixId"])
        self.assertEqual("proof-runtime-output", plan["selection"]["pipelineId"])
        self.assertEqual(SOLUTION_CORE_PACK_HOST_SOLUTION, plan["selection"]["source"]["path"])
        self.assertEqual(
            solution_core_pack_scenario_project_path("FoundationDllTranslationSolution", "FoundationDllTranslation.App"),
            plan["selection"]["source"]["primaryProjectPath"],
        )
        self.assertEqual("", plan["selection"]["source"]["entry"])
        self.assertTrue(plan["selection"]["source"]["fullAssemblyClosure"])
        self.assertNotIn("entrySelection", plan["selection"]["source"])

    def test_planner_surfaces_solution_core_pack_static_call_ctor_getter_native_proof_matrix(self) -> None:
        planner_module = load_module(PLANNER_MODULE_PATH, "chaos_solution_core_pack_planner_static_call_ctor_getter")

        plan = planner_module.build_plan(
            REPO_ROOT,
            "SolutionCorePack",
            matrix_id="windows-static-call-ctor-getter-native-proof",
            run_id="20260419-solution-core-pack-static-call-ctor-getter-native-proof-001",
        )

        self.assertEqual("SolutionCorePack", plan["selection"]["subjectId"])
        self.assertEqual("windows-static-call-ctor-getter-native-proof", plan["selection"]["matrixId"])
        self.assertEqual("proof-runtime-output", plan["selection"]["pipelineId"])
        self.assertEqual(SOLUTION_CORE_PACK_HOST_SOLUTION, plan["selection"]["source"]["path"])
        self.assertEqual(
            solution_core_pack_scenario_project_path("StaticCallCtorGetterSolution", "StaticCallCtorGetter.App"),
            plan["selection"]["source"]["primaryProjectPath"],
        )
        self.assertEqual("", plan["selection"]["source"]["entry"])
        self.assertTrue(plan["selection"]["source"]["fullAssemblyClosure"])
        self.assertNotIn("entrySelection", plan["selection"]["source"])

    def test_planner_surfaces_solution_core_pack_constructor_then_instance_call_native_proof_matrix(self) -> None:
        planner_module = load_module(PLANNER_MODULE_PATH, "chaos_solution_core_pack_planner_constructor_then_instance_call")

        plan = planner_module.build_plan(
            REPO_ROOT,
            "SolutionCorePack",
            matrix_id="windows-constructor-then-instance-call-native-proof",
            run_id="20260419-solution-core-pack-constructor-then-instance-call-native-proof-001",
        )

        self.assertEqual("SolutionCorePack", plan["selection"]["subjectId"])
        self.assertEqual("windows-constructor-then-instance-call-native-proof", plan["selection"]["matrixId"])
        self.assertEqual("proof-runtime-output", plan["selection"]["pipelineId"])
        self.assertEqual(SOLUTION_CORE_PACK_HOST_SOLUTION, plan["selection"]["source"]["path"])
        self.assertEqual(
            solution_core_pack_scenario_project_path("ConstructorThenInstanceCallSolution", "ConstructorThenInstanceCall.App"),
            plan["selection"]["source"]["primaryProjectPath"],
        )
        self.assertEqual("", plan["selection"]["source"]["entry"])
        self.assertTrue(plan["selection"]["source"]["fullAssemblyClosure"])
        self.assertNotIn("entrySelection", plan["selection"]["source"])

    def test_planner_surfaces_solution_core_pack_delegate_closed_target_relay_native_proof_matrix(self) -> None:
        planner_module = load_module(PLANNER_MODULE_PATH, "chaos_solution_core_pack_planner_delegate_closed_target_relay")

        plan = planner_module.build_plan(
            REPO_ROOT,
            "SolutionCorePack",
            matrix_id="windows-delegate-closed-target-relay-native-proof",
            run_id="20260419-solution-core-pack-delegate-closed-target-relay-native-proof-001",
        )

        self.assertEqual("SolutionCorePack", plan["selection"]["subjectId"])
        self.assertEqual("windows-delegate-closed-target-relay-native-proof", plan["selection"]["matrixId"])
        self.assertEqual("proof-runtime-output", plan["selection"]["pipelineId"])
        self.assertEqual(SOLUTION_CORE_PACK_HOST_SOLUTION, plan["selection"]["source"]["path"])
        self.assertEqual(
            solution_core_pack_scenario_project_path("DelegateClosedTargetRelaySolution", "DelegateClosedTargetRelay.App"),
            plan["selection"]["source"]["primaryProjectPath"],
        )
        self.assertEqual("", plan["selection"]["source"]["entry"])
        self.assertTrue(plan["selection"]["source"]["fullAssemblyClosure"])
        self.assertNotIn("entrySelection", plan["selection"]["source"])

    def test_planner_surfaces_solution_core_pack_exception_throw_catch_finally_native_proof_matrix(self) -> None:
        planner_module = load_module(PLANNER_MODULE_PATH, "chaos_solution_core_pack_planner_exception_throw_catch_finally")

        plan = planner_module.build_plan(
            REPO_ROOT,
            "SolutionCorePack",
            matrix_id="windows-exception-throw-catch-finally-native-proof",
            run_id="20260419-solution-core-pack-exception-throw-catch-finally-native-proof-001",
        )

        self.assertEqual("SolutionCorePack", plan["selection"]["subjectId"])
        self.assertEqual("windows-exception-throw-catch-finally-native-proof", plan["selection"]["matrixId"])
        self.assertEqual("proof-runtime-output", plan["selection"]["pipelineId"])
        self.assertEqual(SOLUTION_CORE_PACK_HOST_SOLUTION, plan["selection"]["source"]["path"])
        self.assertEqual(
            solution_core_pack_scenario_project_path("ExceptionThrowCatchFinallySolution", "ExceptionThrowCatchFinally.App"),
            plan["selection"]["source"]["primaryProjectPath"],
        )
        self.assertEqual("", plan["selection"]["source"]["entry"])
        self.assertTrue(plan["selection"]["source"]["fullAssemblyClosure"])
        self.assertNotIn("entrySelection", plan["selection"]["source"])

    def test_planner_surfaces_solution_core_pack_nested_exception_throw_catch_finally_native_proof_matrix(self) -> None:
        planner_module = load_module(PLANNER_MODULE_PATH, "chaos_solution_core_pack_planner_nested_exception_throw_catch_finally")

        plan = planner_module.build_plan(
            REPO_ROOT,
            "SolutionCorePack",
            matrix_id="windows-nested-exception-throw-catch-finally-native-proof",
            run_id="20260419-solution-core-pack-nested-exception-throw-catch-finally-native-proof-001",
        )

        self.assertEqual("SolutionCorePack", plan["selection"]["subjectId"])
        self.assertEqual("windows-nested-exception-throw-catch-finally-native-proof", plan["selection"]["matrixId"])
        self.assertEqual("proof-runtime-output", plan["selection"]["pipelineId"])
        self.assertEqual(SOLUTION_CORE_PACK_HOST_SOLUTION, plan["selection"]["source"]["path"])
        self.assertEqual(
            solution_core_pack_scenario_project_path("NestedExceptionThrowCatchFinallySolution", "NestedExceptionThrowCatchFinally.App"),
            plan["selection"]["source"]["primaryProjectPath"],
        )
        self.assertEqual("", plan["selection"]["source"]["entry"])
        self.assertTrue(plan["selection"]["source"]["fullAssemblyClosure"])
        self.assertNotIn("entrySelection", plan["selection"]["source"])

    def test_planner_surfaces_solution_core_pack_corelib_reference_native_proof_matrix(self) -> None:
        planner_module = load_module(PLANNER_MODULE_PATH, "chaos_solution_core_pack_planner_corelib_reference_native")

        plan = planner_module.build_plan(
            REPO_ROOT,
            "SolutionCorePack",
            matrix_id="windows-corelib-reference-native-proof",
            run_id="20260419-solution-core-pack-corelib-reference-native-proof-001",
        )

        self.assertEqual("SolutionCorePack", plan["selection"]["subjectId"])
        self.assertEqual("windows-corelib-reference-native-proof", plan["selection"]["matrixId"])
        self.assertEqual("proof-runtime-output", plan["selection"]["pipelineId"])
        self.assertEqual(SOLUTION_CORE_PACK_HOST_SOLUTION, plan["selection"]["source"]["path"])
        self.assertEqual(
            "verification/catalog/scenarios/SolutionCorePack/CoreLibReferenceSolution/NativeProofApp/GoldenCoreLibReference.NativeProofApp.csproj",
            plan["selection"]["source"]["primaryProjectPath"],
        )
        self.assertEqual("", plan["selection"]["source"]["entry"])
        self.assertTrue(plan["selection"]["source"]["fullAssemblyClosure"])
        self.assertNotIn("entrySelection", plan["selection"]["source"])

    def test_planner_surfaces_solution_core_pack_marshaling_utf8_export_native_proof_matrix(self) -> None:
        planner_module = load_module(PLANNER_MODULE_PATH, "chaos_solution_core_pack_planner_marshaling_utf8_export")

        plan = planner_module.build_plan(
            REPO_ROOT,
            "SolutionCorePack",
            matrix_id="windows-marshaling-utf8-export-native-proof",
            run_id="20260419-solution-core-pack-marshaling-utf8-export-native-proof-001",
        )

        self.assertEqual("SolutionCorePack", plan["selection"]["subjectId"])
        self.assertEqual("windows-marshaling-utf8-export-native-proof", plan["selection"]["matrixId"])
        self.assertEqual("proof-runtime-output", plan["selection"]["pipelineId"])
        self.assertEqual(SOLUTION_CORE_PACK_HOST_SOLUTION, plan["selection"]["source"]["path"])
        self.assertEqual(
            solution_core_pack_scenario_project_path("MarshalingUtf8ExportSolution", "MarshalingUtf8Export.App"),
            plan["selection"]["source"]["primaryProjectPath"],
        )
        self.assertEqual("", plan["selection"]["source"]["entry"])
        self.assertTrue(plan["selection"]["source"]["fullAssemblyClosure"])
        self.assertNotIn("entrySelection", plan["selection"]["source"])
