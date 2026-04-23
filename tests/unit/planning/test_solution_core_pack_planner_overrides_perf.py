from tests.unit.planning.solution_core_pack_planner_test_support import *


class TestSolutionCorePackPlannerOverridesPerf(SolutionCorePackPlannerTestSupport):
    def test_planner_can_override_declared_unit_entry_and_emit_family_specific_report_roots(self) -> None:
        planner_module = load_module(PLANNER_MODULE_PATH, "chaos_solution_core_pack_planner_declared_unit_selection")
        run_id = "20260413-mainline-array-ops-entry-selection-001"
        stable_id = "SolutionCorePack::CoreRuntimeFeatures::CoreRuntimeFeatures.ArrayOpsProofEntry::Run()"

        plan = planner_module.build_plan(
            REPO_ROOT,
            "SolutionCorePack",
            goal_id="correctness.dev",
            matrix_id="windows-native-check",
            run_id=run_id,
            source_entry="CoreRuntimeFeatures/ArrayOpsProofEntry::Run()",
            entry_selection={
                "family": "declared-unit-test",
                "stableId": stable_id,
                "alias": "array-ops-proof",
            },
        )

        self.assertEqual("CoreRuntimeFeatures/ArrayOpsProofEntry::Run()", plan["selection"]["source"]["entry"])
        self.assertEqual(
            {
                "family": "declared-unit-test",
                "stableId": stable_id,
                "alias": "array-ops-proof",
            },
            plan["selection"]["entrySelection"],
        )
        self.assertEqual(
            f"artifacts/subjects/SolutionCorePack/runs/{run_id}/matrices/windows-native-check/declared/unit/array-ops-proof/report.json",
            plan["artifactsRoot"]["entryReportPath"],
        )
        self.assertEqual(
            f"artifacts/subjects/SolutionCorePack/runs/{run_id}/declared/unit/array-ops-proof/summary.json",
            plan["artifactsRoot"]["entrySummaryPath"],
        )

    def test_planner_can_override_declared_benchmark_entry_and_emit_family_specific_report_roots(self) -> None:
        planner_module = load_module(PLANNER_MODULE_PATH, "chaos_solution_core_pack_planner_declared_benchmark_selection")
        run_id = "20260413-performance-generic-bench-entry-selection-001"
        stable_id = "SolutionCorePack::CoreRuntimeBenchmarks::CoreRuntimeBenchmarks.GenericBenchmarkEntry::RunWorkload()"

        plan = planner_module.build_plan(
            REPO_ROOT,
            "SolutionCorePack",
            goal_id="perf.release",
            matrix_id="windows-native-perf",
            run_id=run_id,
            source_entry="CoreRuntimeBenchmarks/GenericBenchmarkEntry::RunWorkload()",
            workload_entry="CoreRuntimeBenchmarks/GenericBenchmarkEntry::RunWorkload()",
            entry_selection={
                "family": "declared-benchmark",
                "stableId": stable_id,
                "alias": "generic-bench",
            },
        )

        self.assertEqual("CoreRuntimeBenchmarks/GenericBenchmarkEntry::RunWorkload()", plan["selection"]["source"]["entry"])
        self.assertEqual("CoreRuntimeBenchmarks/GenericBenchmarkEntry::RunWorkload()", plan["selection"]["workloadEntry"])
        self.assertEqual(
            {
                "family": "declared-benchmark",
                "stableId": stable_id,
                "alias": "generic-bench",
            },
            plan["selection"]["entrySelection"],
        )
        self.assertEqual(
            f"artifacts/subjects/SolutionCorePack/runs/{run_id}/matrices/windows-native-perf/declared/benchmark/generic-bench/report.json",
            plan["artifactsRoot"]["entryReportPath"],
        )
        self.assertEqual(
            f"artifacts/subjects/SolutionCorePack/runs/{run_id}/declared/benchmark/generic-bench/summary.json",
            plan["artifactsRoot"]["entrySummaryPath"],
        )

    def test_planner_surfaces_matrix_subject_entry_selection_for_solution_core_pack_managed_output(self) -> None:
        planner_module = load_module(PLANNER_MODULE_PATH, "chaos_solution_core_pack_planner_subject_entry_selection")

        plan = planner_module.build_plan(
            REPO_ROOT,
            "SolutionCorePack",
            goal_id="correctness.dev",
            matrix_id="windows-archetype-simple-lib-managed-output",
            run_id="20260413-solution-core-simple-lib-entry-selection-001",
        )

        self.assertEqual("SolutionCorePack", plan["selection"]["subjectId"])
        self.assertEqual(
            "subjects/SolutionCorePack/source/EngineeringScenarios/SimpleLibrarySolution/App/GoldenSimpleLib.App.csproj",
            plan["selection"]["source"]["primaryProjectPath"],
        )
        self.assertEqual("GoldenSimpleLib.App/Program::Main()", plan["selection"]["source"]["entry"])
        self.assertEqual(
            {
                "entryKind": 1,
                "entrySlice": 4,
            },
            plan["selection"]["source"]["entrySelection"],
        )

    def test_planner_surfaces_matrix_subject_entry_selection_for_solution_core_pack_reference_assembly_output(self) -> None:
        planner_module = load_module(PLANNER_MODULE_PATH, "chaos_solution_core_pack_planner_reference_entry_selection")

        plan = planner_module.build_plan(
            REPO_ROOT,
            "SolutionCorePack",
            goal_id="correctness.dev",
            matrix_id="windows-archetype-reference-assembly-managed-output",
            run_id="20260414-solution-core-reference-entry-selection-001",
        )

        self.assertEqual("SolutionCorePack", plan["selection"]["subjectId"])
        self.assertEqual(
            "subjects/SolutionCorePack/source/EngineeringScenarios/ReferenceAssemblySolution/App/GoldenReferenceAssembly.App.csproj",
            plan["selection"]["source"]["primaryProjectPath"],
        )
        self.assertEqual("GoldenReferenceAssembly.App/Program::Main()", plan["selection"]["source"]["entry"])
        self.assertEqual(
            {
                "entryKind": 1,
                "entrySlice": 8,
            },
            plan["selection"]["source"]["entrySelection"],
        )

    def test_native_benchmark_matrix_uses_workload_entry_as_source_entry(self) -> None:
        planner_module = load_module(PLANNER_MODULE_PATH, "chaos_solution_core_pack_planner_native_workload_entry")
        plan = planner_module.build_plan(
            REPO_ROOT,
            "SolutionCorePack",
            goal_id="perf.release",
            matrix_id="windows-native-perf",
            run_id="20260413-solutioncorepack-native-workload-entry-001",
        )

        self.assertEqual("SolutionCorePack", plan["selection"]["subjectId"])
        self.assertEqual(
            "subjects/SolutionCorePack/source/Benchmarks/CoreRuntimeBenchmarks/CoreRuntimeBenchmarks.csproj",
            plan["selection"]["source"]["primaryProjectPath"],
        )
        self.assertEqual("CoreRuntimeBenchmarks/ArithmeticBenchmarkEntry::RunWorkload()", plan["selection"]["source"]["entry"])
        self.assertEqual("CoreRuntimeBenchmarks/ArithmeticBenchmarkEntry::RunWorkload()", plan["selection"]["workloadEntry"])

    def test_planner_selects_native_perf_matrix_for_solution_core_pack(self) -> None:
        planner_module = load_module(PLANNER_MODULE_PATH, "chaos_solution_core_pack_planner_native_perf")
        subject_id = "SolutionCorePack"
        run_id = "20260413-fixture-solution-core-native-perf-001"

        plan = planner_module.build_plan(
            REPO_ROOT,
            subject_id,
            goal_id="perf.release",
            validation_profile_id="perf-profile",
            variant="PROFILE",
            run_id=run_id,
        )

        self.assertEqual(subject_id, plan["selection"]["subjectId"])
        self.assertEqual("perf.release", plan["selection"]["goalId"])
        self.assertEqual("windows-native-perf", plan["selection"]["matrixId"])
        self.assertEqual("perf-profile", plan["selection"]["validationProfileId"])
        self.assertEqual("PROFILE", plan["selection"]["variant"])
        self.assertEqual("native-benchmark", plan["selection"]["pipelineId"])
        self.assertEqual(
            "subjects/SolutionCorePack/source/Benchmarks/CoreRuntimeBenchmarks/CoreRuntimeBenchmarks.csproj",
            plan["selection"]["source"]["primaryProjectPath"],
        )
        self.assertEqual(
            [
                "source-resolve",
                "host-input-build",
                "analysis-frontend",
                "generated-native-aot",
                "build-target",
                "native-runtime-perf",
                "report-assemble",
            ],
            [stage["stageId"] for stage in plan["stagePlan"]],
        )
        native_perf_stage = next(stage for stage in plan["stagePlan"] if stage["kind"] == "native-runtime-perf")
        self.assertEqual(
            f"artifacts/subjects/{subject_id}/runs/{run_id}/matrices/windows-native-perf/runtime/runtime.manifest.json",
            native_perf_stage["paths"]["manifestPath"],
        )
        self.assertEqual("report", plan["selection"]["artifactPlan"]["evidenceTerminalBucket"])

    def test_planner_uses_goal_aware_perf_defaults_for_solution_core_pack_native_perf(self) -> None:
        planner_module = load_module(PLANNER_MODULE_PATH, "chaos_solution_core_pack_planner_native_perf_goal_defaults")
        subject_id = "SolutionCorePack"
        run_id = "20260413-fixture-solution-core-native-perf-goal-defaults-001"

        plan = planner_module.build_plan(
            REPO_ROOT,
            subject_id,
            goal_id="perf.release",
            run_id=run_id,
        )

        self.assertEqual(subject_id, plan["selection"]["subjectId"])
        self.assertEqual("perf.release", plan["selection"]["goalId"])
        self.assertEqual("windows-native-perf", plan["selection"]["matrixId"])
        self.assertEqual("perf-profile", plan["selection"]["validationProfileId"])
        self.assertEqual("perf", plan["selection"]["validationKind"])
        self.assertEqual("PROFILE", plan["selection"]["variant"])
        self.assertEqual("native-benchmark", plan["selection"]["pipelineId"])
