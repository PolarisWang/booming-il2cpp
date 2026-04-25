from tests.unit.planning.solution_core_pack_planner_test_support import *


class TestSolutionCorePackPlannerNativeProofB(SolutionCorePackPlannerTestSupport):
    def test_planner_surfaces_solution_core_pack_reflection_interop_closure_native_proof_matrix(self) -> None:
        planner_module = load_module(PLANNER_MODULE_PATH, "chaos_solution_core_pack_planner_reflection_interop_closure")

        plan = planner_module.build_plan(
            REPO_ROOT,
            "SolutionCorePack",
            matrix_id="windows-reflection-interop-closure-native-proof",
            run_id="20260419-solution-core-pack-reflection-interop-closure-native-proof-001",
        )

        self.assertEqual("SolutionCorePack", plan["selection"]["subjectId"])
        self.assertEqual("windows-reflection-interop-closure-native-proof", plan["selection"]["matrixId"])
        self.assertEqual("proof-runtime-output", plan["selection"]["pipelineId"])
        self.assertEqual(SOLUTION_CORE_PACK_HOST_SOLUTION, plan["selection"]["source"]["path"])
        self.assertEqual(
            solution_core_pack_scenario_project_path("ReflectionInteropClosureSolution", "ReflectionInteropClosure.App"),
            plan["selection"]["source"]["primaryProjectPath"],
        )
        self.assertEqual("", plan["selection"]["source"]["entry"])
        self.assertTrue(plan["selection"]["source"]["fullAssemblyClosure"])
        self.assertNotIn("entrySelection", plan["selection"]["source"])

    def test_planner_surfaces_solution_core_pack_pinvoke_direct_call_native_proof_matrix(self) -> None:
        planner_module = load_module(PLANNER_MODULE_PATH, "chaos_solution_core_pack_planner_pinvoke_direct_call")

        plan = planner_module.build_plan(
            REPO_ROOT,
            "SolutionCorePack",
            matrix_id="windows-pinvoke-direct-call-native-proof",
            run_id="20260419-solution-core-pack-pinvoke-direct-call-native-proof-001",
        )

        self.assertEqual("SolutionCorePack", plan["selection"]["subjectId"])
        self.assertEqual("windows-pinvoke-direct-call-native-proof", plan["selection"]["matrixId"])
        self.assertEqual("proof-runtime-output", plan["selection"]["pipelineId"])
        self.assertEqual(SOLUTION_CORE_PACK_HOST_SOLUTION, plan["selection"]["source"]["path"])
        self.assertEqual(
            solution_core_pack_scenario_project_path("PInvokeDirectCallSolution", "PInvokeDirectCall.App"),
            plan["selection"]["source"]["primaryProjectPath"],
        )
        self.assertEqual("", plan["selection"]["source"]["entry"])
        self.assertTrue(plan["selection"]["source"]["fullAssemblyClosure"])
        self.assertNotIn("entrySelection", plan["selection"]["source"])

    def test_planner_surfaces_solution_core_pack_array_boxing_reference_array_native_proof_matrix(self) -> None:
        planner_module = load_module(PLANNER_MODULE_PATH, "chaos_solution_core_pack_planner_array_boxing_reference_array")

        plan = planner_module.build_plan(
            REPO_ROOT,
            "SolutionCorePack",
            matrix_id="windows-array-boxing-reference-array-native-proof",
            run_id="20260419-solution-core-pack-array-boxing-reference-array-native-proof-001",
        )

        self.assertEqual("SolutionCorePack", plan["selection"]["subjectId"])
        self.assertEqual("windows-array-boxing-reference-array-native-proof", plan["selection"]["matrixId"])
        self.assertEqual("proof-runtime-output", plan["selection"]["pipelineId"])
        self.assertEqual(SOLUTION_CORE_PACK_HOST_SOLUTION, plan["selection"]["source"]["path"])
        self.assertEqual(
            solution_core_pack_scenario_project_path("ArrayBoxingReferenceArraySolution", "ArrayBoxingReferenceArray.App"),
            plan["selection"]["source"]["primaryProjectPath"],
        )
        self.assertEqual("", plan["selection"]["source"]["entry"])
        self.assertTrue(plan["selection"]["source"]["fullAssemblyClosure"])
        self.assertNotIn("entrySelection", plan["selection"]["source"])

    def test_planner_surfaces_solution_core_pack_interface_dispatch_message_native_proof_matrix(self) -> None:
        planner_module = load_module(PLANNER_MODULE_PATH, "chaos_solution_core_pack_planner_interface_dispatch_message")

        plan = planner_module.build_plan(
            REPO_ROOT,
            "SolutionCorePack",
            matrix_id="windows-interface-dispatch-message-native-proof",
            run_id="20260419-solution-core-pack-interface-dispatch-message-native-proof-001",
        )

        self.assertEqual("SolutionCorePack", plan["selection"]["subjectId"])
        self.assertEqual("windows-interface-dispatch-message-native-proof", plan["selection"]["matrixId"])
        self.assertEqual("proof-runtime-output", plan["selection"]["pipelineId"])
        self.assertEqual(SOLUTION_CORE_PACK_HOST_SOLUTION, plan["selection"]["source"]["path"])
        self.assertEqual(
            solution_core_pack_scenario_project_path("InterfaceDispatchMessageSolution", "InterfaceDispatchMessage.App"),
            plan["selection"]["source"]["primaryProjectPath"],
        )
        self.assertEqual("", plan["selection"]["source"]["entry"])
        self.assertTrue(plan["selection"]["source"]["fullAssemblyClosure"])
        self.assertNotIn("entrySelection", plan["selection"]["source"])

    def test_planner_surfaces_solution_core_pack_threading_threadstatic_monitor_native_proof_matrix(self) -> None:
        planner_module = load_module(
            PLANNER_MODULE_PATH,
            "chaos_solution_core_pack_planner_threading_threadstatic_monitor",
        )

        plan = planner_module.build_plan(
            REPO_ROOT,
            "SolutionCorePack",
            matrix_id="windows-threading-threadstatic-monitor-native-proof",
            run_id="20260420-solution-core-pack-threading-threadstatic-monitor-native-proof-001",
        )

        self.assertEqual("SolutionCorePack", plan["selection"]["subjectId"])
        self.assertEqual("windows-threading-threadstatic-monitor-native-proof", plan["selection"]["matrixId"])
        self.assertEqual("proof-runtime-output", plan["selection"]["pipelineId"])
        self.assertEqual(SOLUTION_CORE_PACK_HOST_SOLUTION, plan["selection"]["source"]["path"])
        self.assertEqual(
            solution_core_pack_scenario_project_path("ThreadingThreadStaticMonitorSolution", "ThreadingThreadStaticMonitor.App"),
            plan["selection"]["source"]["primaryProjectPath"],
        )
        self.assertEqual("", plan["selection"]["source"]["entry"])
        self.assertTrue(plan["selection"]["source"]["fullAssemblyClosure"])
        self.assertNotIn("entrySelection", plan["selection"]["source"])

    def test_planner_surfaces_solution_core_pack_corelib_reference_hotupdate_proof_matrix(self) -> None:
        planner_module = load_module(PLANNER_MODULE_PATH, "chaos_solution_core_pack_planner_corelib_reference_hotupdate")

        plan = planner_module.build_plan(
            REPO_ROOT,
            "SolutionCorePack",
            matrix_id="windows-corelib-reference-hotupdate-proof",
            run_id="20260419-solution-core-pack-corelib-reference-hotupdate-proof-001",
        )

        self.assertEqual("SolutionCorePack", plan["selection"]["subjectId"])
        self.assertEqual("windows-corelib-reference-hotupdate-proof", plan["selection"]["matrixId"])
        self.assertEqual("managed-runtime-output", plan["selection"]["pipelineId"])
        self.assertEqual(SOLUTION_CORE_PACK_HOST_SOLUTION, plan["selection"]["source"]["path"])
        self.assertEqual(SOLUTION_CORE_PACK_HOST_PROJECT, plan["selection"]["source"]["primaryProjectPath"])
        self.assertEqual("dotnet-managed", plan["selection"]["executionContext"]["toolchainProfile"])
        self.assertEqual("hotupdate-proof-output", plan["selection"]["executionContext"]["runtimeProfile"])

    def test_planner_surfaces_solution_core_pack_corelib_reference_native_hotupdate_proof_matrix(self) -> None:
        planner_module = load_module(
            PLANNER_MODULE_PATH,
            "chaos_solution_core_pack_planner_corelib_reference_native_hotupdate",
        )

        plan = planner_module.build_plan(
            REPO_ROOT,
            "SolutionCorePack",
            matrix_id="windows-corelib-reference-native-hotupdate-proof",
            run_id="20260420-solution-core-pack-corelib-reference-native-hotupdate-proof-001",
        )

        self.assertEqual("SolutionCorePack", plan["selection"]["subjectId"])
        self.assertEqual("windows-corelib-reference-native-hotupdate-proof", plan["selection"]["matrixId"])
        self.assertEqual("native-hotupdate-proof-output", plan["selection"]["pipelineId"])
        self.assertEqual(SOLUTION_CORE_PACK_HOST_SOLUTION, plan["selection"]["source"]["path"])
        self.assertEqual(
            "verification/catalog/scenarios/SolutionCorePack/CoreLibReferenceSolution/NativeProofApp/GoldenCoreLibReference.NativeProofApp.csproj",
            plan["selection"]["source"]["primaryProjectPath"],
        )
        self.assertEqual("", plan["selection"]["source"]["entry"])
        self.assertTrue(plan["selection"]["source"]["fullAssemblyClosure"])
        self.assertEqual("msvc-reference", plan["selection"]["executionContext"]["toolchainProfile"])
        self.assertEqual("native-hotupdate-proof-output", plan["selection"]["executionContext"]["runtimeProfile"])
        self.assertEqual(
            {
                "family": "declared-unit-test",
                "stableId": "SolutionCorePack::GoldenCoreLibReference.HotUpdatePatch::GoldenCoreLibReference.HotUpdatePatch.CoreLibHotUpdateProof::Run()",
                "alias": "corelib-reference-hotupdate-proof",
                "entryIndex": 60,
            },
            plan["selection"]["entrySelection"],
        )
