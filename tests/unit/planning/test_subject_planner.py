from __future__ import annotations

import importlib.util
import json
import shutil
import sys
import unittest
import uuid
from pathlib import Path


REPO_ROOT = Path(__file__).resolve().parents[3]
PLANNER_MODULE_PATH = REPO_ROOT / "build" / "toolchains" / "run" / "testing" / "subject_planner.py"
SUBJECTS_MODULE_PATH = REPO_ROOT / "build" / "toolchains" / "run" / "testing" / "subjects.py"


def load_module(path: Path, module_name: str):
    if not path.is_file():
        raise FileNotFoundError(f"module missing: {path}")

    spec = importlib.util.spec_from_file_location(module_name, path)
    if spec is None or spec.loader is None:
        raise RuntimeError(f"unable to load module: {path}")

    module = importlib.util.module_from_spec(spec)
    sys.modules[spec.name] = module
    spec.loader.exec_module(module)
    return module


def load_subjects_module(module_name: str):
    return load_module(SUBJECTS_MODULE_PATH, module_name)


def select_subject_record(module_name: str, **filters: object):
    subjects_module = load_subjects_module(module_name)
    records = subjects_module.query_subject_records(
        subjects_module.load_subject_records(REPO_ROOT),
        **filters,
    )
    if not records:
        raise AssertionError(f"no subject record matched filters: {filters}")
    return subjects_module, sorted(records, key=lambda item: str(item["subjectId"]))[0]


def expected_matrix_for_goal(manifest: dict, goal_id: str) -> dict:
    default_matrix_id = str(manifest["defaultMatrix"])
    for matrix in list(manifest.get("environmentMatrices") or []):
        if str(matrix.get("matrixId") or "") == default_matrix_id and goal_id in list(matrix.get("supportedGoals") or []):
            return dict(matrix)
    for matrix in list(manifest.get("environmentMatrices") or []):
        if goal_id in list(matrix.get("supportedGoals") or []):
            return dict(matrix)
    raise AssertionError(f"no matrix supports goal: {goal_id}")


def find_matrix_for_goal(subjects_module, manifest: dict, goal_id: str, *, required_stage_kind: str | None = None) -> dict:
    for matrix in list(manifest.get("environmentMatrices") or []):
        if goal_id not in list(matrix.get("supportedGoals") or []):
            continue
        if required_stage_kind is not None:
            pipeline = subjects_module.find_pipeline(manifest, str(matrix.get("pipelineId") or ""))
            if required_stage_kind not in subjects_module.pipeline_stage_kinds(pipeline):
                continue
        return dict(matrix)
    raise AssertionError(f"no matrix matched goal={goal_id}, required_stage_kind={required_stage_kind}")


def find_goal_matrix_mismatch(manifest: dict) -> tuple[str, str]:
    matrices = [dict(matrix) for matrix in list(manifest.get("environmentMatrices") or [])]
    for candidate_matrix in matrices:
        candidate_goals = set(candidate_matrix.get("supportedGoals") or [])
        for other_matrix in matrices:
            if other_matrix["matrixId"] == candidate_matrix["matrixId"]:
                continue
            for goal_id in list(other_matrix.get("supportedGoals") or []):
                if goal_id not in candidate_goals:
                    return str(candidate_matrix["matrixId"]), str(goal_id)
    raise AssertionError("no mismatched goal/matrix pair found")


class SubjectPlannerTests(unittest.TestCase):
    def test_planner_uses_subject_defaults_and_subject_artifact_layout(self) -> None:
        planner_module = load_module(PLANNER_MODULE_PATH, "chaos_subject_planner_default")
        subjects_module, record = select_subject_record(
            "chaos_subject_planner_default_record",
            category="canonical",
            source_type="dotnet-project",
            required_stage_kinds=["analysis-frontend", "generated-native-proof", "runtime-observe"],
        )
        manifest = record["manifest"]
        subject_id = str(record["subjectId"])
        run_id = "20260407-fixture-default-001"

        plan = planner_module.build_plan(REPO_ROOT, subject_id, run_id=run_id)
        expected_matrix = subjects_module.find_matrix(manifest, str(manifest["defaultMatrix"]))
        expected_pipeline = subjects_module.find_pipeline(manifest, str(expected_matrix["pipelineId"]))

        self.assertEqual("v1", plan["planVersion"])
        self.assertEqual(subject_id, plan["request"]["subjectId"])
        self.assertIsNone(plan["request"]["goalId"])
        self.assertIsNone(plan["request"]["matrixId"])
        self.assertEqual(run_id, plan["request"]["runId"])
        self.assertEqual(subject_id, plan["selection"]["subjectId"])
        self.assertEqual(str(manifest["defaultGoal"]), plan["selection"]["goalId"])
        self.assertEqual(str(manifest["defaultMatrix"]), plan["selection"]["matrixId"])
        self.assertEqual(str(expected_matrix["pipelineId"]), plan["selection"]["pipelineId"])
        self.assertEqual(
            str(dict(expected_matrix["artifactPlan"])["evidenceTerminalBucket"]),
            plan["selection"]["artifactPlan"]["evidenceTerminalBucket"],
        )
        self.assertEqual(f"artifacts/subjects/{subject_id}", plan["artifactsRoot"]["subjectRoot"])
        self.assertEqual(f"artifacts/subjects/{subject_id}/runs/{run_id}", plan["artifactsRoot"]["runRoot"])
        self.assertEqual(
            [str(stage["stageId"]) for stage in list(expected_pipeline.get("stages") or [])],
            [stage["stageId"] for stage in plan["stagePlan"]],
        )
        self.assertEqual(
            subjects_module.stage_paths(subject_id, str(expected_matrix["matrixId"]), run_id=run_id, bucket="source", scope="shared", kind="source-resolve")["manifestPath"],
            plan["stagePlan"][0]["paths"]["manifestPath"],
        )
        self.assertEqual(
            subjects_module.stage_paths(subject_id, str(expected_matrix["matrixId"]), run_id=run_id, bucket="report", scope="matrix", kind="report-assemble")["manifestPath"],
            plan["stagePlan"][-1]["paths"]["manifestPath"],
        )
        self.assertTrue(all(stage["executionMode"] in {"executed", "reused", "invalidated"} for stage in plan["stagePlan"]))

    def test_planner_selects_requested_trace_matrix_without_subject_name_coupling(self) -> None:
        planner_module = load_module(PLANNER_MODULE_PATH, "chaos_subject_planner_trace")
        subjects_module, record = select_subject_record(
            "chaos_subject_planner_trace_record",
            category="canonical",
            source_type="dotnet-project",
            required_stage_kinds=["runtime-trace-compare"],
        )
        manifest = record["manifest"]
        subject_id = str(record["subjectId"])
        trace_matrix = find_matrix_for_goal(subjects_module, manifest, "correctness.platform", required_stage_kind="runtime-trace-compare")
        run_id = "20260407-fixture-trace-001"

        plan = planner_module.build_plan(
            REPO_ROOT,
            subject_id,
            goal_id="correctness.platform",
            matrix_id=str(trace_matrix["matrixId"]),
            run_id=run_id,
        )
        expected_pipeline = subjects_module.find_pipeline(manifest, str(trace_matrix["pipelineId"]))

        self.assertEqual(str(trace_matrix["matrixId"]), plan["selection"]["matrixId"])
        self.assertEqual(str(trace_matrix["pipelineId"]), plan["selection"]["pipelineId"])
        self.assertEqual(
            [str(stage["stageId"]) for stage in list(expected_pipeline.get("stages") or [])],
            [stage["stageId"] for stage in plan["stagePlan"]],
        )
        trace_stage = next(stage for stage in plan["stagePlan"] if stage["kind"] == "runtime-trace-compare")
        self.assertEqual(
            subjects_module.stage_paths(subject_id, str(trace_matrix["matrixId"]), run_id=run_id, bucket="runtime", scope="matrix", kind="runtime-trace-compare")["manifestPath"],
            trace_stage["paths"]["manifestPath"],
        )
        self.assertEqual(
            subjects_module.stage_paths(subject_id, str(trace_matrix["matrixId"]), run_id=run_id, bucket="runtime", scope="matrix", kind="runtime-trace-compare")["reportPaths"],
            trace_stage["paths"]["reportPaths"],
        )

    def test_planner_rejects_goal_matrix_mismatch(self) -> None:
        planner_module = load_module(PLANNER_MODULE_PATH, "chaos_subject_planner_mismatch")
        _, record = select_subject_record(
            "chaos_subject_planner_mismatch_record",
            source_type="dotnet-project",
            required_goal_ids=["correctness.dev", "correctness.platform"],
        )
        subject_id = str(record["subjectId"])
        matrix_id, unsupported_goal_id = find_goal_matrix_mismatch(record["manifest"])

        with self.assertRaisesRegex(ValueError, "does not support goal"):
            planner_module.build_plan(REPO_ROOT, subject_id, goal_id=unsupported_goal_id, matrix_id=matrix_id)

    def test_planner_uses_perf_defaults_without_benchmark_name_coupling(self) -> None:
        planner_module = load_module(PLANNER_MODULE_PATH, "chaos_subject_planner_perf_default")
        subjects_module, record = select_subject_record(
            "chaos_subject_planner_perf_default_record",
            category="benchmark",
            source_type="dotnet-project",
            required_stage_kinds=["runtime-perf-collect"],
            required_goal_ids=["perf.dev", "perf.release"],
        )
        manifest = record["manifest"]
        subject_id = str(record["subjectId"])
        run_id = "20260407-fixture-perf-default-001"

        plan = planner_module.build_plan(REPO_ROOT, subject_id, run_id=run_id)
        expected_matrix = subjects_module.find_matrix(manifest, str(manifest["defaultMatrix"]))

        self.assertEqual(subject_id, plan["selection"]["subjectId"])
        self.assertEqual(str(manifest["defaultGoal"]), plan["selection"]["goalId"])
        self.assertEqual(str(expected_matrix["matrixId"]), plan["selection"]["matrixId"])
        self.assertEqual(str(expected_matrix["pipelineId"]), plan["selection"]["pipelineId"])
        self.assertEqual(
            str(dict(expected_matrix["artifactPlan"])["evidenceTerminalBucket"]),
            plan["selection"]["artifactPlan"]["evidenceTerminalBucket"],
        )

    def test_planner_selects_release_perf_matrix_without_subject_name_coupling(self) -> None:
        planner_module = load_module(PLANNER_MODULE_PATH, "chaos_subject_planner_perf_release")
        subjects_module, record = select_subject_record(
            "chaos_subject_planner_perf_release_record",
            category="benchmark",
            source_type="dotnet-project",
            required_goal_ids=["perf.release"],
            required_stage_kinds=["runtime-perf-collect"],
        )
        manifest = record["manifest"]
        subject_id = str(record["subjectId"])
        selected_matrix = find_matrix_for_goal(subjects_module, manifest, "perf.release")
        run_id = "20260407-fixture-perf-release-001"

        plan = planner_module.build_plan(
            REPO_ROOT,
            subject_id,
            goal_id="perf.release",
            matrix_id=str(selected_matrix["matrixId"]),
            run_id=run_id,
        )

        self.assertEqual(str(selected_matrix["matrixId"]), plan["selection"]["matrixId"])
        self.assertEqual(str(selected_matrix["pipelineId"]), plan["selection"]["pipelineId"])
        self.assertEqual(
            subjects_module.stage_paths(subject_id, str(selected_matrix["matrixId"]), run_id=run_id, bucket="report", scope="matrix", kind="report-assemble")["manifestPath"],
            plan["stagePlan"][-1]["paths"]["manifestPath"],
        )

    def test_planner_surfaces_workload_entry_for_benchmark_subject(self) -> None:
        planner_module = load_module(PLANNER_MODULE_PATH, "chaos_subject_planner_workload_entry")
        plan = planner_module.build_plan(
            REPO_ROOT,
            "PerformanceFeaturePack",
            goal_id="perf.release",
            matrix_id="windows-managed-perf",
            run_id="20260412-bencharithmetic-workload-entry-001",
        )

        self.assertEqual("PerformanceFeaturePack", plan["selection"]["subjectId"])
        self.assertEqual("PerformanceFeaturePack/ArithmeticBenchmarkEntry::RunWorkload()", plan["selection"]["source"]["entry"])
        self.assertEqual("PerformanceFeaturePack/ArithmeticBenchmarkEntry::RunWorkload()", plan["selection"]["workloadEntry"])

    def test_planner_surfaces_explicit_workload_entry_for_hot_update_host_pack(self) -> None:
        planner_module = load_module(PLANNER_MODULE_PATH, "chaos_subject_planner_hot_update_workload_entry")
        plan = planner_module.build_plan(
            REPO_ROOT,
            "HotUpdateHostPack",
            goal_id="correctness.dev",
            matrix_id="windows-managed-output",
            run_id="20260413-benchhotupdateload-workload-entry-001",
            source_entry="HotUpdateHostPack/HotUpdateLoadBenchmarkEntry::RunWorkload()",
            workload_entry="HotUpdateHostPack/HotUpdateLoadBenchmarkEntry::RunWorkload()",
        )

        self.assertEqual("HotUpdateHostPack", plan["selection"]["subjectId"])
        self.assertEqual("subjects/HotUpdateHostPack/source/HotUpdateHostPack.csproj", plan["selection"]["source"]["path"])
        self.assertEqual("HotUpdateHostPack/HotUpdateLoadBenchmarkEntry::RunWorkload()", plan["selection"]["source"]["entry"])
        self.assertEqual("HotUpdateHostPack/HotUpdateLoadBenchmarkEntry::RunWorkload()", plan["selection"]["workloadEntry"])

    def test_planner_surfaces_workload_entry_for_mixed_execution_feature_pack(self) -> None:
        planner_module = load_module(PLANNER_MODULE_PATH, "chaos_subject_planner_mixed_execution_workload_entry")
        plan = planner_module.build_plan(
            REPO_ROOT,
            "MixedExecutionFeaturePack",
            goal_id="perf.release",
            matrix_id="windows-managed-perf",
            run_id="20260413-benchmixed-workload-entry-001",
        )

        self.assertEqual("MixedExecutionFeaturePack", plan["selection"]["subjectId"])
        self.assertEqual("subjects/MixedExecutionFeaturePack/source/MixedExecutionFeaturePack.csproj", plan["selection"]["source"]["path"])
        self.assertEqual("MixedExecutionFeaturePack/MixedExecutionBenchmarkEntry::RunWorkload()", plan["selection"]["source"]["entry"])
        self.assertEqual("MixedExecutionFeaturePack/MixedExecutionBenchmarkEntry::RunWorkload()", plan["selection"]["workloadEntry"])

    def test_planner_surfaces_explicit_proof_entry_for_hot_update_host_pack(self) -> None:
        planner_module = load_module(PLANNER_MODULE_PATH, "chaos_subject_planner_hot_update_proof_shell")
        plan = planner_module.build_plan(
            REPO_ROOT,
            "HotUpdateHostPack",
            goal_id="correctness.dev",
            matrix_id="windows-managed-output",
            run_id="20260413-hotupdateskeletonproof-shell-entry-001",
            source_entry="HotUpdateHostPack/HotUpdateSkeletonProofEntry::Run()",
        )

        self.assertEqual("HotUpdateHostPack", plan["selection"]["subjectId"])
        self.assertEqual("HotUpdateHostPack/HotUpdateSkeletonProofEntry::Run()", plan["selection"]["source"]["entry"])

    def test_planner_surfaces_entry_for_mixed_execution_feature_pack_proof(self) -> None:
        planner_module = load_module(PLANNER_MODULE_PATH, "chaos_subject_planner_mixed_execution_proof_shell")
        plan = planner_module.build_plan(
            REPO_ROOT,
            "MixedExecutionFeaturePack",
            goal_id="correctness.dev",
            matrix_id="windows-managed-output",
            run_id="20260413-mixedexecutionproof-shell-entry-001",
        )

        self.assertEqual("MixedExecutionFeaturePack", plan["selection"]["subjectId"])
        self.assertEqual("subjects/MixedExecutionFeaturePack/source/MixedExecutionFeaturePack.csproj", plan["selection"]["source"]["path"])
        self.assertEqual("MixedExecutionFeaturePack/MixedExecutionProofEntry::Run()", plan["selection"]["source"]["entry"])

    def test_planner_can_override_declared_unit_entry_and_emit_family_specific_report_roots(self) -> None:
        planner_module = load_module(PLANNER_MODULE_PATH, "chaos_subject_planner_declared_unit_selection")
        run_id = "20260413-mainline-array-ops-entry-selection-001"
        stable_id = "MainlineFeaturePack::MainlineFeaturePack::MainlineFeaturePack.ArrayOpsProofEntry::Run()"

        plan = planner_module.build_plan(
            REPO_ROOT,
            "MainlineFeaturePack",
            goal_id="correctness.dev",
            matrix_id="windows-array-ops-check",
            run_id=run_id,
            source_entry="MainlineFeaturePack/ArrayOpsProofEntry::Run()",
            entry_selection={
                "family": "declared-unit-test",
                "stableId": stable_id,
                "alias": "array-ops-proof",
            },
        )

        self.assertEqual("MainlineFeaturePack/ArrayOpsProofEntry::Run()", plan["selection"]["source"]["entry"])
        self.assertEqual(
            {
                "family": "declared-unit-test",
                "stableId": stable_id,
                "alias": "array-ops-proof",
            },
            plan["selection"]["entrySelection"],
        )
        self.assertEqual(
            f"artifacts/subjects/MainlineFeaturePack/runs/{run_id}/matrices/windows-array-ops-check/declared/unit/array-ops-proof/report.json",
            plan["artifactsRoot"]["entryReportPath"],
        )
        self.assertEqual(
            f"artifacts/subjects/MainlineFeaturePack/runs/{run_id}/declared/unit/array-ops-proof/summary.json",
            plan["artifactsRoot"]["entrySummaryPath"],
        )

    def test_planner_can_override_declared_benchmark_entry_and_emit_family_specific_report_roots(self) -> None:
        planner_module = load_module(PLANNER_MODULE_PATH, "chaos_subject_planner_declared_benchmark_selection")
        run_id = "20260413-performance-generic-bench-entry-selection-001"
        stable_id = "PerformanceFeaturePack::PerformanceFeaturePack::PerformanceFeaturePack.GenericBenchmarkEntry::RunWorkload()"

        plan = planner_module.build_plan(
            REPO_ROOT,
            "PerformanceFeaturePack",
            goal_id="perf.release",
            matrix_id="windows-native-perf",
            run_id=run_id,
            source_entry="PerformanceFeaturePack/GenericBenchmarkEntry::RunWorkload()",
            workload_entry="PerformanceFeaturePack/GenericBenchmarkEntry::RunWorkload()",
            entry_selection={
                "family": "declared-benchmark",
                "stableId": stable_id,
                "alias": "generic-bench",
            },
        )

        self.assertEqual("PerformanceFeaturePack/GenericBenchmarkEntry::RunWorkload()", plan["selection"]["source"]["entry"])
        self.assertEqual("PerformanceFeaturePack/GenericBenchmarkEntry::RunWorkload()", plan["selection"]["workloadEntry"])
        self.assertEqual(
            {
                "family": "declared-benchmark",
                "stableId": stable_id,
                "alias": "generic-bench",
            },
            plan["selection"]["entrySelection"],
        )
        self.assertEqual(
            f"artifacts/subjects/PerformanceFeaturePack/runs/{run_id}/matrices/windows-native-perf/declared/benchmark/generic-bench/report.json",
            plan["artifactsRoot"]["entryReportPath"],
        )
        self.assertEqual(
            f"artifacts/subjects/PerformanceFeaturePack/runs/{run_id}/declared/benchmark/generic-bench/summary.json",
            plan["artifactsRoot"]["entrySummaryPath"],
        )

    def test_planner_uses_canonical_pack_source_while_interpreter_support_project_remains_nested(self) -> None:
        planner_module = load_module(PLANNER_MODULE_PATH, "chaos_subject_planner_interpreter_arithmetic_shell")
        plan = planner_module.build_plan(
            REPO_ROOT,
            "MixedExecutionFeaturePack",
            goal_id="correctness.dev",
            matrix_id="windows-managed-output",
            run_id="20260413-interpreterarithmeticproof-shell-entry-001",
        )

        support_project_path = REPO_ROOT / "subjects" / "MixedExecutionFeaturePack" / "source" / "InterpreterArithmeticProof" / "InterpreterArithmeticProof.csproj"

        self.assertEqual("MixedExecutionFeaturePack", plan["selection"]["subjectId"])
        self.assertEqual(
            "subjects/MixedExecutionFeaturePack/source/MixedExecutionFeaturePack.csproj",
            plan["selection"]["source"]["path"],
        )
        self.assertEqual("MixedExecutionFeaturePack/MixedExecutionProofEntry::Run()", plan["selection"]["source"]["entry"])
        self.assertTrue(support_project_path.is_file())

    def test_planner_selection_surfaces_thin_manifest_fields(self) -> None:
        planner_module = load_module(PLANNER_MODULE_PATH, "chaos_subject_planner_thin_manifest_selection")
        subjects_module = load_subjects_module("chaos_subject_planner_thin_manifest_subjects")
        manifest = subjects_module.load_subject_manifest(REPO_ROOT, "PerformanceFeaturePack")

        plan = planner_module.build_plan(
            REPO_ROOT,
            "PerformanceFeaturePack",
            goal_id="perf.release",
            matrix_id="windows-managed-perf",
            run_id="20260412-bencharithmetic-thin-selection-001",
        )

        self.assertEqual(manifest["sourceModel"], plan["selection"]["sourceModel"])
        self.assertEqual(manifest["dependencyModel"], plan["selection"]["dependencyModel"])
        self.assertEqual(manifest["executablePlan"], plan["selection"]["executablePlan"])
        self.assertEqual(manifest["engineeringProfile"], plan["selection"]["engineeringProfile"])
        self.assertEqual(manifest["availability"], plan["selection"]["availability"])
        self.assertEqual(manifest["compatibility"], plan["selection"]["compatibility"])

    def test_native_benchmark_matrix_uses_workload_entry_as_source_entry(self) -> None:
        planner_module = load_module(PLANNER_MODULE_PATH, "chaos_subject_planner_native_workload_entry")
        plan = planner_module.build_plan(
            REPO_ROOT,
            "PerformanceFeaturePack",
            goal_id="perf.release",
            matrix_id="windows-native-perf",
            run_id="20260412-bencharithmetic-native-workload-entry-001",
        )

        self.assertEqual("PerformanceFeaturePack", plan["selection"]["subjectId"])
        self.assertEqual("PerformanceFeaturePack/ArithmeticBenchmarkEntry::RunWorkload()", plan["selection"]["source"]["entry"])
        self.assertEqual("PerformanceFeaturePack/ArithmeticBenchmarkEntry::RunWorkload()", plan["selection"]["workloadEntry"])

    def test_planner_selects_first_matrix_supporting_requested_goal_when_matrix_is_omitted(self) -> None:
        planner_module = load_module(PLANNER_MODULE_PATH, "chaos_subject_planner_goal_only")
        _, record = select_subject_record(
            "chaos_subject_planner_goal_only_record",
            category="benchmark",
            source_type="dotnet-project",
            required_goal_ids=["perf.release"],
        )
        subject_id = str(record["subjectId"])
        expected_matrix = expected_matrix_for_goal(record["manifest"], "perf.release")

        plan = planner_module.build_plan(REPO_ROOT, subject_id, goal_id="perf.release", run_id="20260407-fixture-auto-001")

        self.assertEqual("perf.release", plan["selection"]["goalId"])
        self.assertEqual(str(expected_matrix["matrixId"]), plan["selection"]["matrixId"])
        self.assertEqual(str(expected_matrix["pipelineId"]), plan["selection"]["pipelineId"])

    def test_planner_uses_managed_output_defaults_without_subject_name_coupling(self) -> None:
        planner_module = load_module(PLANNER_MODULE_PATH, "chaos_subject_planner_managed_output")
        _, record = select_subject_record(
            "chaos_subject_planner_managed_output_record",
            source_type="dotnet-project",
            required_stage_kinds=["runtime-managed-output"],
            required_goal_ids=["correctness.dev"],
        )
        subject_id = str(record["subjectId"])
        expected_matrix = expected_matrix_for_goal(record["manifest"], "correctness.dev")
        run_id = "20260407-fixture-managed-output-001"

        plan = planner_module.build_plan(REPO_ROOT, subject_id, run_id=run_id)

        self.assertEqual(subject_id, plan["selection"]["subjectId"])
        self.assertEqual("correctness.dev", plan["selection"]["goalId"])
        self.assertEqual(str(expected_matrix["matrixId"]), plan["selection"]["matrixId"])
        self.assertEqual(str(expected_matrix["pipelineId"]), plan["selection"]["pipelineId"])

    def test_planner_selects_mobile_android_buildable_matrix_without_subject_name_coupling(self) -> None:
        planner_module = load_module(PLANNER_MODULE_PATH, "chaos_subject_planner_mobile_buildable")
        _, record = select_subject_record(
            "chaos_subject_planner_mobile_buildable_record",
            category="canonical",
            source_type="dotnet-project",
            required_stage_kinds=["runtime-managed-output", "build-target"],
            required_goal_ids=["correctness.dev", "correctness.platform"],
            required_validation_profile_ids=["managed-output"],
        )
        subject_id = str(record["subjectId"])
        run_id = "20260411-fixture-mobile-build-001"

        plan = planner_module.build_plan(
            REPO_ROOT,
            subject_id,
            goal_id="correctness.platform",
            run_id=run_id,
        )

        self.assertEqual(subject_id, plan["selection"]["subjectId"])
        self.assertEqual("correctness.platform", plan["selection"]["goalId"])
        self.assertEqual("windows-android-buildable", plan["selection"]["matrixId"])
        self.assertEqual("platform-buildable", plan["selection"]["pipelineId"])
        self.assertEqual(
            [
                "source-resolve",
                "host-input-build",
                "analysis-frontend",
                "generated-native-proof",
                "build-target",
                "report-assemble",
            ],
            [stage["stageId"] for stage in plan["stagePlan"]],
        )
        self.assertEqual("build", plan["selection"]["artifactPlan"]["evidenceTerminalBucket"])

    def test_planner_uses_default_validation_profile_and_variant(self) -> None:
        planner_module = load_module(PLANNER_MODULE_PATH, "chaos_subject_planner_variant_default")
        subjects_module, record = select_subject_record(
            "chaos_subject_planner_variant_default_record",
            source_type="dotnet-project",
            required_validation_profile_ids=["proof-dev"],
        )
        manifest = record["manifest"]
        subject_id = str(record["subjectId"])
        validation_selection = subjects_module.resolve_validation_selection(manifest)

        plan = planner_module.build_plan(REPO_ROOT, subject_id, run_id="20260407-fixture-variant-default-001")

        self.assertIsNone(plan["request"].get("validationProfileId"))
        self.assertIsNone(plan["request"].get("validationKind"))
        self.assertIsNone(plan["request"].get("variant"))
        self.assertEqual(str(validation_selection["validationProfileId"]), plan["selection"]["validationProfileId"])
        self.assertEqual(list(validation_selection["validationKinds"]), plan["selection"]["validationKinds"])
        self.assertEqual(str(validation_selection["variant"]), plan["selection"]["variant"])

    def test_planner_allows_explicit_validation_profile_and_variant_override(self) -> None:
        planner_module = load_module(PLANNER_MODULE_PATH, "chaos_subject_planner_variant_override")
        subjects_module, record = select_subject_record(
            "chaos_subject_planner_variant_override_record",
            category="benchmark",
            source_type="dotnet-project",
            required_goal_ids=["perf.release"],
            required_validation_profile_ids=["perf-release"],
        )
        manifest = record["manifest"]
        subject_id = str(record["subjectId"])
        selected_matrix = find_matrix_for_goal(subjects_module, manifest, "perf.release")
        profile_id = "perf-release"
        validation_kind = str(list(dict(manifest["validationProfiles"])[profile_id])[0])

        plan = planner_module.build_plan(
            REPO_ROOT,
            subject_id,
            goal_id="perf.release",
            matrix_id=str(selected_matrix["matrixId"]),
            validation_profile_id=profile_id,
            validation_kind=validation_kind,
            variant="SHIP",
            run_id="20260407-fixture-variant-override-001",
        )

        self.assertEqual(profile_id, plan["request"]["validationProfileId"])
        self.assertEqual(validation_kind, plan["request"]["validationKind"])
        self.assertEqual("SHIP", plan["request"]["variant"])
        self.assertEqual(profile_id, plan["selection"]["validationProfileId"])
        self.assertEqual([validation_kind], plan["selection"]["validationKinds"])
        self.assertEqual(validation_kind, plan["selection"]["validationKind"])
        self.assertEqual("SHIP", plan["selection"]["variant"])

    def test_stage_fingerprint_changes_when_variant_changes(self) -> None:
        planner_module = load_module(PLANNER_MODULE_PATH, "chaos_subject_planner_variant_fingerprint")
        subjects_module, record = select_subject_record(
            "chaos_subject_planner_variant_fingerprint_record",
            category="benchmark",
            source_type="dotnet-project",
            required_goal_ids=["perf.release"],
            required_validation_profile_ids=["perf-release"],
        )
        manifest = record["manifest"]
        subject_id = str(record["subjectId"])
        selected_matrix = find_matrix_for_goal(subjects_module, manifest, "perf.release")
        profile_id = "perf-release"

        default_plan = planner_module.build_plan(
            REPO_ROOT,
            subject_id,
            goal_id="perf.release",
            matrix_id=str(selected_matrix["matrixId"]),
            validation_profile_id=profile_id,
            run_id="20260407-fixture-variant-fingerprint-001",
        )
        ship_plan = planner_module.build_plan(
            REPO_ROOT,
            subject_id,
            goal_id="perf.release",
            matrix_id=str(selected_matrix["matrixId"]),
            validation_profile_id=profile_id,
            variant="SHIP",
            run_id="20260407-fixture-variant-fingerprint-001",
        )

        self.assertNotEqual(default_plan["selection"]["variant"], ship_plan["selection"]["variant"])
        self.assertNotEqual(default_plan["stagePlan"][0]["fingerprint"], ship_plan["stagePlan"][0]["fingerprint"])

    def test_stage_fingerprint_changes_when_workload_entry_changes(self) -> None:
        planner_module = load_module(PLANNER_MODULE_PATH, "chaos_subject_planner_workload_fingerprint")
        repo_root = REPO_ROOT / "artifacts" / ".tmp-tests" / "subject-planner" / f"workload-fingerprint-{uuid.uuid4().hex}"
        manifest_path = repo_root / "subjects" / "FixtureBench" / "subject.manifest.json"
        manifest_path.parent.mkdir(parents=True, exist_ok=True)

        manifest = {
            "subjectId": "FixtureBench",
            "displayName": "FixtureBench",
            "category": "benchmark",
            "defaultGoal": "perf.release",
            "defaultMatrix": "windows-managed-perf",
            "defaultValidationProfile": "perf-profile",
            "source": {
                "type": "dotnet-project",
                "path": "subjects/FixtureBench/source/FixtureBench.csproj",
                "entry": "FixtureBench/Program::Main()",
            },
            "workloadEntry": "FixtureBench/Program::RunWorkload()",
            "validationProfiles": {
                "perf-profile": ["perf"],
            },
            "validation": {
                "perf": {
                    "kind": "perf",
                    "driver": "csharp-perf-harness",
                    "defaultVariant": "PROFILE",
                }
            },
            "executionPipelines": [
                {
                    "pipelineId": "managed-benchmark",
                    "stages": [
                        {"stageId": "source-resolve", "kind": "source-resolve", "scope": "shared", "bucket": "source", "dependsOn": []},
                        {"stageId": "host-input-build", "kind": "host-input-build", "scope": "shared", "bucket": "host-input", "dependsOn": ["source-resolve"]},
                        {"stageId": "runtime-perf-collect", "kind": "runtime-perf-collect", "scope": "matrix", "bucket": "runtime", "dependsOn": ["host-input-build"]},
                        {"stageId": "report-assemble", "kind": "report-assemble", "scope": "matrix", "bucket": "report", "dependsOn": ["runtime-perf-collect"]},
                    ],
                }
            ],
            "environmentMatrices": [
                {
                    "matrixId": "windows-managed-perf",
                    "pipelineId": "managed-benchmark",
                    "supportedGoals": ["perf.release"],
                    "executionContext": {
                        "hostPlatform": "windows-x64",
                        "targetPlatform": "windows-x64",
                        "toolchainProfile": "dotnet-managed",
                        "runtimeProfile": "managed-perf-release",
                    },
                    "validationIntent": {
                        "validationMode": "perf",
                        "adaptationLevel": "managed-runtime",
                        "expectedOutcome": "pass",
                    },
                    "artifactPlan": {
                        "requiredBuckets": ["source", "host-input", "runtime", "report"],
                        "evidenceTerminalBucket": "report",
                    },
                }
            ],
        }

        try:
            manifest_path.write_text(json.dumps(manifest, ensure_ascii=False, indent=2) + "\n", encoding="utf-8")
            plan = planner_module.build_plan(
                repo_root,
                "FixtureBench",
                goal_id="perf.release",
                matrix_id="windows-managed-perf",
                run_id="fixture-workload-fingerprint",
            )

            manifest["workloadEntry"] = "FixtureBench/Program::RunHotPath()"
            manifest_path.write_text(json.dumps(manifest, ensure_ascii=False, indent=2) + "\n", encoding="utf-8")
            updated_plan = planner_module.build_plan(
                repo_root,
                "FixtureBench",
                goal_id="perf.release",
                matrix_id="windows-managed-perf",
                run_id="fixture-workload-fingerprint",
            )

            self.assertNotEqual(plan["selection"]["workloadEntry"], updated_plan["selection"]["workloadEntry"])
            self.assertNotEqual(plan["stagePlan"][0]["fingerprint"], updated_plan["stagePlan"][0]["fingerprint"])
        finally:
            shutil.rmtree(repo_root, ignore_errors=True)

    def test_planner_selects_native_perf_matrix_for_mainline_feature_pack(self) -> None:
        planner_module = load_module(PLANNER_MODULE_PATH, "chaos_subject_planner_mainline_native_perf")
        _, record = select_subject_record(
            "chaos_subject_planner_mainline_native_perf_record",
            category="mainline",
            source_type="dotnet-project",
            required_stage_kinds=["native-runtime-perf"],
            required_goal_ids=["perf.profile"],
            required_validation_profile_ids=["perf-profile"],
            required_validation_drivers=["native-runtime-perf"],
        )
        subject_id = str(record["subjectId"])
        run_id = "20260409-fixture-mainline-native-perf-001"

        plan = planner_module.build_plan(
            REPO_ROOT,
            subject_id,
            goal_id="perf.profile",
            validation_profile_id="perf-profile",
            variant="PROFILE",
            run_id=run_id,
        )

        self.assertEqual(subject_id, plan["selection"]["subjectId"])
        self.assertEqual("perf.profile", plan["selection"]["goalId"])
        self.assertEqual("windows-native-profile", plan["selection"]["matrixId"])
        self.assertEqual("perf-profile", plan["selection"]["validationProfileId"])
        self.assertEqual("PROFILE", plan["selection"]["variant"])
        self.assertEqual("native-runtime-perf", plan["selection"]["pipelineId"])
        self.assertEqual(
            [
                "source-resolve",
                "host-input-build",
                "analysis-frontend",
                "generated-native-proof",
                "build-target",
                "native-runtime-perf",
                "report-assemble",
            ],
            [stage["stageId"] for stage in plan["stagePlan"]],
        )
        native_perf_stage = next(stage for stage in plan["stagePlan"] if stage["kind"] == "native-runtime-perf")
        self.assertEqual(
            f"artifacts/subjects/{subject_id}/runs/{run_id}/matrices/windows-native-profile/runtime/runtime.manifest.json",
            native_perf_stage["paths"]["manifestPath"],
        )
        self.assertEqual("report", plan["selection"]["artifactPlan"]["evidenceTerminalBucket"])

    def test_planner_uses_goal_aware_perf_defaults_for_native_perf_subject(self) -> None:
        planner_module = load_module(PLANNER_MODULE_PATH, "chaos_subject_planner_native_perf_goal_defaults")
        _, record = select_subject_record(
            "chaos_subject_planner_native_perf_goal_defaults_record",
            category="mainline",
            source_type="dotnet-project",
            required_goal_ids=["perf.profile"],
            required_validation_profile_ids=["perf-profile"],
            required_validation_drivers=["native-runtime-perf"],
        )
        subject_id = str(record["subjectId"])
        run_id = "20260411-fixture-native-perf-goal-defaults-001"

        plan = planner_module.build_plan(
            REPO_ROOT,
            subject_id,
            goal_id="perf.profile",
            run_id=run_id,
        )

        self.assertEqual(subject_id, plan["selection"]["subjectId"])
        self.assertEqual("perf.profile", plan["selection"]["goalId"])
        self.assertEqual("windows-native-profile", plan["selection"]["matrixId"])
        self.assertEqual("perf-profile", plan["selection"]["validationProfileId"])
        self.assertEqual("perf", plan["selection"]["validationKind"])
        self.assertEqual("PROFILE", plan["selection"]["variant"])
        self.assertEqual("native-runtime-perf", plan["selection"]["pipelineId"])

    def test_planner_uses_goal_matching_perf_profile_when_goal_changes_perf_matrix(self) -> None:
        planner_module = load_module(PLANNER_MODULE_PATH, "chaos_subject_planner_goal_matching_perf_profile")
        _, record = select_subject_record(
            "chaos_subject_planner_goal_matching_perf_profile_record",
            category="benchmark",
            source_type="dotnet-project",
            required_goal_ids=["perf.dev", "perf.release"],
            required_validation_profile_ids=["perf-dev", "perf-release"],
            required_validation_drivers=["csharp-perf-harness"],
        )
        subject_id = str(record["subjectId"])

        plan = planner_module.build_plan(
            REPO_ROOT,
            subject_id,
            goal_id="perf.release",
            run_id="20260411-fixture-goal-matching-perf-profile-001",
        )

        self.assertEqual("perf.release", plan["selection"]["goalId"])
        self.assertEqual("windows-perf-release", plan["selection"]["matrixId"])
        self.assertEqual("perf-release", plan["selection"]["validationProfileId"])
        self.assertEqual("perf", plan["selection"]["validationKind"])
        self.assertEqual("PROFILE", plan["selection"]["variant"])

    def test_planner_overlays_matrix_source_entry_for_mainline_feature_pack_capability_slice(self) -> None:
        planner_module = load_module(PLANNER_MODULE_PATH, "chaos_subject_planner_mainline_capability_slice")
        _, record = select_subject_record(
            "chaos_subject_planner_mainline_capability_slice_record",
            category="mainline",
            source_type="dotnet-project",
            required_goal_ids=["correctness.dev"],
            required_validation_profile_ids=["proof-dispatch"],
        )
        subject_id = str(record["subjectId"])
        run_id = "20260409-fixture-mainline-dispatch-slice-001"

        plan = planner_module.build_plan(
            REPO_ROOT,
            subject_id,
            goal_id="correctness.dev",
            matrix_id="windows-dispatch-check",
            validation_profile_id="proof-dispatch",
            run_id=run_id,
        )

        self.assertEqual("windows-dispatch-check", plan["selection"]["matrixId"])
        self.assertEqual("proof-runtime-output", plan["selection"]["pipelineId"])
        self.assertEqual("proof-dispatch", plan["selection"]["validationProfileId"])
        self.assertEqual(
            "MainlineFeaturePack/DispatchProofEntry::Run()",
            plan["selection"]["source"]["entry"],
        )

    def test_planner_overlays_matrix_source_entry_for_mainline_feature_pack_delegate_slice(self) -> None:
        planner_module = load_module(PLANNER_MODULE_PATH, "chaos_subject_planner_mainline_delegate_slice")
        _, record = select_subject_record(
            "chaos_subject_planner_mainline_delegate_slice_record",
            category="mainline",
            source_type="dotnet-project",
            required_goal_ids=["correctness.dev"],
            required_validation_profile_ids=["proof-delegate"],
        )
        subject_id = str(record["subjectId"])
        run_id = "20260409-fixture-mainline-delegate-slice-001"

        plan = planner_module.build_plan(
            REPO_ROOT,
            subject_id,
            goal_id="correctness.dev",
            matrix_id="windows-delegate-check",
            validation_profile_id="proof-delegate",
            run_id=run_id,
        )

        self.assertEqual("windows-delegate-check", plan["selection"]["matrixId"])
        self.assertEqual("proof-runtime-output", plan["selection"]["pipelineId"])
        self.assertEqual("proof-delegate", plan["selection"]["validationProfileId"])
        self.assertEqual(
            "MainlineFeaturePack/DelegateProofEntry::Run()",
            plan["selection"]["source"]["entry"],
        )

    def test_planner_overlays_matrix_source_entry_for_mainline_feature_pack_exception_slice(self) -> None:
        planner_module = load_module(PLANNER_MODULE_PATH, "chaos_subject_planner_mainline_exception_slice")
        _, record = select_subject_record(
            "chaos_subject_planner_mainline_exception_slice_record",
            category="mainline",
            source_type="dotnet-project",
            required_goal_ids=["correctness.dev"],
            required_validation_profile_ids=["proof-exception"],
        )
        subject_id = str(record["subjectId"])
        run_id = "20260409-fixture-mainline-exception-slice-001"

        plan = planner_module.build_plan(
            REPO_ROOT,
            subject_id,
            goal_id="correctness.dev",
            matrix_id="windows-exception-check",
            validation_profile_id="proof-exception",
            run_id=run_id,
        )

        self.assertEqual("windows-exception-check", plan["selection"]["matrixId"])
        self.assertEqual("proof-runtime-output", plan["selection"]["pipelineId"])
        self.assertEqual("proof-exception", plan["selection"]["validationProfileId"])
        self.assertEqual(
            "MainlineFeaturePack/ExceptionProofEntry::Run()",
            plan["selection"]["source"]["entry"],
        )

    def test_planner_overlays_matrix_source_entry_for_mainline_feature_pack_reflection_interop_closure_slice(self) -> None:
        planner_module = load_module(PLANNER_MODULE_PATH, "chaos_subject_planner_mainline_reflection_interop_closure_slice")
        _, record = select_subject_record(
            "chaos_subject_planner_mainline_reflection_interop_closure_slice_record",
            category="mainline",
            source_type="dotnet-project",
            required_goal_ids=["correctness.dev"],
            required_validation_profile_ids=["proof-reflection-interop-closure"],
        )
        subject_id = str(record["subjectId"])
        run_id = "20260409-fixture-mainline-reflection-interop-closure-slice-001"

        plan = planner_module.build_plan(
            REPO_ROOT,
            subject_id,
            goal_id="correctness.dev",
            matrix_id="windows-reflection-interop-closure-check",
            validation_profile_id="proof-reflection-interop-closure",
            run_id=run_id,
        )

        self.assertEqual("windows-reflection-interop-closure-check", plan["selection"]["matrixId"])
        self.assertEqual("proof-runtime-output", plan["selection"]["pipelineId"])
        self.assertEqual("proof-reflection-interop-closure", plan["selection"]["validationProfileId"])
        self.assertEqual(
            "MainlineFeaturePack/ReflectionInteropClosureEntry::Run()",
            plan["selection"]["source"]["entry"],
        )

    def test_planner_overlays_matrix_source_entry_for_mainline_feature_pack_interface_dispatch_slice(self) -> None:
        planner_module = load_module(PLANNER_MODULE_PATH, "chaos_subject_planner_mainline_interface_dispatch_slice")
        _, record = select_subject_record(
            "chaos_subject_planner_mainline_interface_dispatch_slice_record",
            category="mainline",
            source_type="dotnet-project",
            required_goal_ids=["correctness.dev"],
            required_validation_profile_ids=["proof-interface-dispatch"],
        )
        subject_id = str(record["subjectId"])
        run_id = "20260413-fixture-mainline-interface-dispatch-slice-001"

        plan = planner_module.build_plan(
            REPO_ROOT,
            subject_id,
            goal_id="correctness.dev",
            matrix_id="windows-interface-dispatch-check",
            validation_profile_id="proof-interface-dispatch",
            run_id=run_id,
        )

        self.assertEqual("windows-interface-dispatch-check", plan["selection"]["matrixId"])
        self.assertEqual("proof-runtime-output", plan["selection"]["pipelineId"])
        self.assertEqual("proof-interface-dispatch", plan["selection"]["validationProfileId"])
        self.assertEqual(
            "MainlineFeaturePack/InterfaceDispatchProofEntry::Run()",
            plan["selection"]["source"]["entry"],
        )

    def test_planner_overlays_matrix_source_entry_for_mainline_feature_pack_marshaling_slice(self) -> None:
        planner_module = load_module(PLANNER_MODULE_PATH, "chaos_subject_planner_mainline_marshaling_slice")
        _, record = select_subject_record(
            "chaos_subject_planner_mainline_marshaling_slice_record",
            category="mainline",
            source_type="dotnet-project",
            required_goal_ids=["correctness.dev"],
            required_validation_profile_ids=["proof-marshaling"],
        )
        subject_id = str(record["subjectId"])
        run_id = "20260413-fixture-mainline-marshaling-slice-001"

        plan = planner_module.build_plan(
            REPO_ROOT,
            subject_id,
            goal_id="correctness.dev",
            matrix_id="windows-marshaling-check",
            validation_profile_id="proof-marshaling",
            run_id=run_id,
        )

        self.assertEqual("windows-marshaling-check", plan["selection"]["matrixId"])
        self.assertEqual("proof-runtime-output", plan["selection"]["pipelineId"])
        self.assertEqual("proof-marshaling", plan["selection"]["validationProfileId"])
        self.assertEqual(
            "MainlineFeaturePack/MarshalingProofEntry::Run()",
            plan["selection"]["source"]["entry"],
        )

    def test_planner_selects_mainline_feature_pack_array_ops_slice_without_legacy_shell(self) -> None:
        planner_module = load_module(PLANNER_MODULE_PATH, "chaos_subject_planner_mainline_array_ops_slice")

        plan = planner_module.build_plan(
            REPO_ROOT,
            "MainlineFeaturePack",
            goal_id="correctness.dev",
            matrix_id="windows-array-ops-check",
            validation_profile_id="proof-array-ops",
            run_id="20260413-fixture-legacy-array-ops-shell-001",
        )

        self.assertEqual("MainlineFeaturePack", plan["selection"]["subjectId"])
        self.assertEqual("windows-array-ops-check", plan["selection"]["matrixId"])
        self.assertEqual("proof-array-ops", plan["selection"]["validationProfileId"])
        self.assertEqual("subjects/MainlineFeaturePack/source/MainlineFeaturePack.csproj", plan["selection"]["source"]["path"])
        self.assertEqual("MainlineFeaturePack/ArrayOpsProofEntry::Run()", plan["selection"]["source"]["entry"])
        self.assertEqual({}, plan["selection"]["compatibility"])

    def test_planner_selects_mainline_feature_pack_async_await_slice_without_legacy_shell(self) -> None:
        planner_module = load_module(PLANNER_MODULE_PATH, "chaos_subject_planner_mainline_async_await_slice")

        plan = planner_module.build_plan(
            REPO_ROOT,
            "MainlineFeaturePack",
            goal_id="correctness.dev",
            matrix_id="windows-async-await-check",
            validation_profile_id="proof-async-await",
            run_id="20260413-fixture-legacy-async-await-shell-001",
        )

        self.assertEqual("MainlineFeaturePack", plan["selection"]["subjectId"])
        self.assertEqual("windows-async-await-check", plan["selection"]["matrixId"])
        self.assertEqual("proof-async-await", plan["selection"]["validationProfileId"])
        self.assertEqual("subjects/MainlineFeaturePack/source/MainlineFeaturePack.csproj", plan["selection"]["source"]["path"])
        self.assertEqual("MainlineFeaturePack/AsyncAwaitProofEntry::Run()", plan["selection"]["source"]["entry"])
        self.assertEqual({}, plan["selection"]["compatibility"])

    def test_planner_selects_mainline_feature_pack_interface_dispatch_slice_without_legacy_shell(self) -> None:
        planner_module = load_module(PLANNER_MODULE_PATH, "chaos_subject_planner_mainline_interface_dispatch_without_shell")

        plan = planner_module.build_plan(
            REPO_ROOT,
            "MainlineFeaturePack",
            goal_id="correctness.dev",
            matrix_id="windows-interface-dispatch-check",
            validation_profile_id="proof-interface-dispatch",
            run_id="20260413-fixture-legacy-interface-dispatch-shell-001",
        )

        self.assertEqual("MainlineFeaturePack", plan["selection"]["subjectId"])
        self.assertEqual("windows-interface-dispatch-check", plan["selection"]["matrixId"])
        self.assertEqual("proof-interface-dispatch", plan["selection"]["validationProfileId"])
        self.assertEqual("subjects/MainlineFeaturePack/source/MainlineFeaturePack.csproj", plan["selection"]["source"]["path"])
        self.assertEqual("MainlineFeaturePack/InterfaceDispatchProofEntry::Run()", plan["selection"]["source"]["entry"])
        self.assertEqual({}, plan["selection"]["compatibility"])

    def test_planner_selects_mainline_feature_pack_marshaling_slice_without_legacy_shell(self) -> None:
        planner_module = load_module(PLANNER_MODULE_PATH, "chaos_subject_planner_mainline_marshaling_without_shell")

        plan = planner_module.build_plan(
            REPO_ROOT,
            "MainlineFeaturePack",
            goal_id="correctness.dev",
            matrix_id="windows-marshaling-check",
            validation_profile_id="proof-marshaling",
            run_id="20260413-fixture-legacy-marshaling-shell-001",
        )

        self.assertEqual("MainlineFeaturePack", plan["selection"]["subjectId"])
        self.assertEqual("windows-marshaling-check", plan["selection"]["matrixId"])
        self.assertEqual("proof-marshaling", plan["selection"]["validationProfileId"])
        self.assertEqual("subjects/MainlineFeaturePack/source/MainlineFeaturePack.csproj", plan["selection"]["source"]["path"])
        self.assertEqual("MainlineFeaturePack/MarshalingProofEntry::Run()", plan["selection"]["source"]["entry"])
        self.assertEqual({}, plan["selection"]["compatibility"])

    def test_planner_selects_mainline_feature_pack_cross_boundary_exception_slice_without_legacy_shell(self) -> None:
        planner_module = load_module(PLANNER_MODULE_PATH, "chaos_subject_planner_mainline_cross_boundary_exception_without_shell")

        plan = planner_module.build_plan(
            REPO_ROOT,
            "MainlineFeaturePack",
            goal_id="correctness.dev",
            matrix_id="windows-cross-boundary-eh-check",
            validation_profile_id="proof-cross-boundary-exception",
            run_id="20260413-fixture-legacy-cross-boundary-exception-shell-001",
        )

        self.assertEqual("MainlineFeaturePack", plan["selection"]["subjectId"])
        self.assertEqual("windows-cross-boundary-eh-check", plan["selection"]["matrixId"])
        self.assertEqual("proof-cross-boundary-exception", plan["selection"]["validationProfileId"])
        self.assertEqual("subjects/MainlineFeaturePack/source/MainlineFeaturePack.csproj", plan["selection"]["source"]["path"])
        self.assertEqual("MainlineFeaturePack/CrossBoundaryExceptionProofEntry::Run()", plan["selection"]["source"]["entry"])
        self.assertEqual({}, plan["selection"]["compatibility"])

    def test_planner_selects_engine_output_pipeline_without_copying_engine_profile_into_selection(self) -> None:
        planner_module = load_module(PLANNER_MODULE_PATH, "chaos_subject_planner_engine_output")
        _, record = select_subject_record(
            "chaos_subject_planner_engine_output_record",
            category="canonical",
            source_type="dotnet-project",
            required_stage_kinds=["generated-engine-proof", "runtime-engine-observe"],
            required_goal_ids=["correctness.dev"],
        )
        subject_id = str(record["subjectId"])
        run_id = "20260410-fixture-engine-output-001"

        plan = planner_module.build_plan(REPO_ROOT, subject_id, run_id=run_id)

        self.assertEqual(subject_id, plan["selection"]["subjectId"])
        self.assertEqual("correctness.dev", plan["selection"]["goalId"])
        self.assertEqual("windows-dev-output", plan["selection"]["matrixId"])
        self.assertEqual("engine-runtime-output", plan["selection"]["pipelineId"])
        self.assertNotIn("engineProofProfile", plan["selection"])
        self.assertEqual(
            [
                "source-resolve",
                "host-input-build",
                "analysis-frontend",
                "generated-engine-proof",
                "build-target",
                "runtime-engine-observe",
                "report-assemble",
            ],
            [stage["stageId"] for stage in plan["stagePlan"]],
        )

    def test_planner_selects_engine_trace_matrix_and_paths_without_subject_name_coupling(self) -> None:
        planner_module = load_module(PLANNER_MODULE_PATH, "chaos_subject_planner_engine_trace")
        subjects_module, record = select_subject_record(
            "chaos_subject_planner_engine_trace_record",
            category="canonical",
            source_type="dotnet-project",
            required_stage_kinds=["runtime-engine-trace-compare"],
            required_goal_ids=["correctness.platform"],
        )
        manifest = record["manifest"]
        subject_id = str(record["subjectId"])
        trace_matrix = find_matrix_for_goal(
            subjects_module,
            manifest,
            "correctness.platform",
            required_stage_kind="runtime-engine-trace-compare",
        )
        run_id = "20260410-fixture-engine-trace-001"

        plan = planner_module.build_plan(
            REPO_ROOT,
            subject_id,
            goal_id="correctness.platform",
            matrix_id=str(trace_matrix["matrixId"]),
            run_id=run_id,
        )

        self.assertEqual("windows-reference-trace", plan["selection"]["matrixId"])
        self.assertEqual("engine-runtime-trace", plan["selection"]["pipelineId"])
        self.assertEqual(
            [
                "source-resolve",
                "host-input-build",
                "analysis-frontend",
                "generated-engine-proof",
                "build-target",
                "runtime-engine-observe",
                "runtime-engine-trace-compare",
                "report-assemble",
            ],
            [stage["stageId"] for stage in plan["stagePlan"]],
        )
        trace_stage = next(stage for stage in plan["stagePlan"] if stage["kind"] == "runtime-engine-trace-compare")
        self.assertEqual(
            subjects_module.stage_paths(
                subject_id,
                "windows-reference-trace",
                run_id=run_id,
                bucket="runtime",
                scope="matrix",
                kind="runtime-engine-trace-compare",
            )["reportPaths"],
            trace_stage["paths"]["reportPaths"],
        )

    def test_planner_expands_shared_orchestration_profiles_before_building_plan(self) -> None:
        planner_module = load_module(PLANNER_MODULE_PATH, "chaos_subject_planner_shared_profiles")
        repo_root = REPO_ROOT / "artifacts" / ".tmp-tests" / "subject-planner" / f"shared-profiles-{uuid.uuid4().hex}"
        manifest_path = repo_root / "subjects" / "FixtureSharedPlanner" / "subject.manifest.json"
        pipeline_profile_path = repo_root / "testing" / "orchestration" / "pipelines" / "proof-core.json"
        matrix_profile_path = repo_root / "testing" / "orchestration" / "matrices" / "proof-core.json"
        manifest_path.parent.mkdir(parents=True, exist_ok=True)
        pipeline_profile_path.parent.mkdir(parents=True, exist_ok=True)
        matrix_profile_path.parent.mkdir(parents=True, exist_ok=True)

        pipeline_profile = {
            "executionPipelines": [
                {
                    "pipelineId": "proof-runtime-output",
                    "displayName": "Profile Runtime Output",
                    "stages": [
                        {"stageId": "source-resolve", "kind": "source-resolve", "scope": "shared", "bucket": "source", "dependsOn": []},
                        {"stageId": "host-input-build", "kind": "host-input-build", "scope": "shared", "bucket": "host-input", "dependsOn": ["source-resolve"]},
                        {"stageId": "analysis-frontend", "kind": "analysis-frontend", "scope": "shared", "bucket": "analysis", "dependsOn": ["host-input-build"]},
                        {"stageId": "generated-native-proof", "kind": "generated-native-proof", "scope": "shared", "bucket": "generated", "dependsOn": ["analysis-frontend"]},
                        {"stageId": "build-target", "kind": "build-target", "scope": "matrix", "bucket": "build", "dependsOn": ["generated-native-proof"]},
                        {"stageId": "runtime-observe", "kind": "runtime-observe", "scope": "matrix", "bucket": "runtime", "dependsOn": ["build-target"]},
                        {"stageId": "report-assemble", "kind": "report-assemble", "scope": "matrix", "bucket": "report", "dependsOn": ["runtime-observe"]},
                    ],
                }
            ]
        }
        matrix_profile = {
            "environmentMatrices": [
                {
                    "matrixId": "windows-dev-output",
                    "pipelineId": "proof-runtime-output",
                    "supportedGoals": ["correctness.dev"],
                    "executionContext": {
                        "hostPlatform": "windows-x64",
                        "targetPlatform": "windows-x64",
                        "toolchainProfile": "msvc-reference",
                        "runtimeProfile": "native-proof-output",
                    },
                    "validationIntent": {
                        "validationMode": "output",
                        "adaptationLevel": "observable-output",
                        "expectedOutcome": "pass",
                    },
                    "artifactPlan": {
                        "requiredBuckets": ["source", "host-input", "analysis", "generated", "build", "runtime", "report"],
                        "evidenceTerminalBucket": "runtime",
                    },
                }
            ]
        }
        manifest = {
            "subjectId": "FixtureSharedPlanner",
            "displayName": "FixtureSharedPlanner",
            "category": "canonical",
            "defaultGoal": "correctness.dev",
            "defaultMatrix": "windows-dev-output",
            "defaultValidationProfile": "proof-dev",
            "source": {
                "type": "dotnet-project",
                "path": "subjects/FixtureSharedPlanner/source/FixtureSharedPlanner.csproj",
                "entry": "FixtureSharedPlanner/Program::Main()",
            },
            "orchestration": {
                "matrixProfile": "proof-core",
                "pipelineProfile": "proof-core",
                "budgetProfile": "budget-standard",
                "baselineProfile": "baseline-standard",
            },
            "validationProfiles": {
                "proof-dev": ["proof"],
            },
            "validation": {
                "proof": {
                    "kind": "proof",
                    "defaultVariant": "CHECK",
                }
            },
        }

        try:
            pipeline_profile_path.write_text(json.dumps(pipeline_profile, ensure_ascii=False, indent=2) + "\n", encoding="utf-8")
            matrix_profile_path.write_text(json.dumps(matrix_profile, ensure_ascii=False, indent=2) + "\n", encoding="utf-8")
            manifest_path.write_text(json.dumps(manifest, ensure_ascii=False, indent=2) + "\n", encoding="utf-8")

            plan = planner_module.build_plan(
                repo_root,
                "FixtureSharedPlanner",
                run_id="fixture-shared-planner",
            )

            self.assertEqual("FixtureSharedPlanner", plan["selection"]["subjectId"])
            self.assertEqual("windows-dev-output", plan["selection"]["matrixId"])
            self.assertEqual("proof-runtime-output", plan["selection"]["pipelineId"])
            self.assertEqual(
                {
                    "matrixProfile": "proof-core",
                    "pipelineProfile": "proof-core",
                    "budgetProfile": "budget-standard",
                    "baselineProfile": "baseline-standard",
                },
                plan["selection"]["orchestration"],
            )
            self.assertEqual(
                [
                    "source-resolve",
                    "host-input-build",
                    "analysis-frontend",
                    "generated-native-proof",
                    "build-target",
                    "runtime-observe",
                    "report-assemble",
                ],
                [stage["stageId"] for stage in plan["stagePlan"]],
            )
        finally:
            shutil.rmtree(repo_root, ignore_errors=True)


if __name__ == "__main__":
    unittest.main()
