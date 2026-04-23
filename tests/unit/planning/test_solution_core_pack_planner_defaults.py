from tests.unit.planning.solution_core_pack_planner_test_support import *


class TestSolutionCorePackPlannerDefaults(SolutionCorePackPlannerTestSupport):
    def test_planner_surfaces_solution_core_pack_default_native_proof_entry(self) -> None:
        planner_module = load_module(PLANNER_MODULE_PATH, "chaos_solution_core_pack_planner_default")

        plan = planner_module.build_plan(
            REPO_ROOT,
            "SolutionCorePack",
            run_id="20260413-solution-core-pack-default-001",
        )

        self.assertEqual("SolutionCorePack", plan["selection"]["subjectId"])
        self.assertEqual("subjects/SolutionCorePack/source/SolutionCorePack.sln", plan["selection"]["source"]["path"])
        self.assertEqual(
            "subjects/SolutionCorePack/source/Host/SolutionCorePack.csproj",
            plan["selection"]["source"]["primaryProjectPath"],
        )
        self.assertEqual("CoreRuntimeFeatures/InterfaceDispatchProofEntry::Run()", plan["selection"]["source"]["entry"])
        self.assertEqual(
            {
                "entryKind": 1,
                "entrySlice": 11,
            },
            plan["selection"]["source"]["entrySelection"],
        )

    def test_planner_surfaces_solution_core_pack_benchmark_entry(self) -> None:
        planner_module = load_module(PLANNER_MODULE_PATH, "chaos_solution_core_pack_planner_benchmark")

        plan = planner_module.build_plan(
            REPO_ROOT,
            "SolutionCorePack",
            goal_id="perf.release",
            matrix_id="windows-native-perf",
            run_id="20260413-solution-core-pack-benchmark-001",
        )

        self.assertEqual("SolutionCorePack", plan["selection"]["subjectId"])
        self.assertEqual("windows-native-perf", plan["selection"]["matrixId"])
        self.assertEqual(
            "CoreRuntimeBenchmarks/ArithmeticBenchmarkEntry::RunWorkload()",
            plan["selection"]["source"]["entry"],
        )
        self.assertEqual(
            "CoreRuntimeBenchmarks/ArithmeticBenchmarkEntry::RunWorkload()",
            plan["selection"]["workloadEntry"],
        )

    def test_planner_selection_surfaces_thin_manifest_fields(self) -> None:
        planner_module = load_module(PLANNER_MODULE_PATH, "chaos_solution_core_pack_planner_thin_manifest_selection")
        subjects_module = load_subjects_module("chaos_solution_core_pack_planner_thin_manifest_subjects")
        manifest = subjects_module.load_subject_manifest(REPO_ROOT, "SolutionCorePack")

        plan = planner_module.build_plan(
            REPO_ROOT,
            "SolutionCorePack",
            goal_id="perf.release",
            matrix_id="windows-native-perf",
            run_id="20260413-solutioncorepack-thin-selection-001",
        )

        self.assertEqual(manifest["sourceModel"], plan["selection"]["sourceModel"])
        self.assertEqual(manifest["dependencyModel"], plan["selection"]["dependencyModel"])
        self.assertEqual(manifest["executablePlan"], plan["selection"]["executablePlan"])
        self.assertEqual(manifest["engineeringProfile"], plan["selection"]["engineeringProfile"])
        self.assertEqual(manifest["availability"], plan["selection"]["availability"])
        self.assertEqual(manifest["compatibility"], plan["selection"]["compatibility"])
