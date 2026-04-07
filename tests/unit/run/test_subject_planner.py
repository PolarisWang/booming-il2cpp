from __future__ import annotations

import importlib.util
import sys
import unittest
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


if __name__ == "__main__":
    unittest.main()
