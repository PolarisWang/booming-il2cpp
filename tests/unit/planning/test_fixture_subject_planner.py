from __future__ import annotations

import shutil
import unittest
from pathlib import Path

from tests.support import clone_fixture_subject_repo, load_module


REPO_ROOT = Path(__file__).resolve().parents[3]
PLANNER_MODULE_PATH = REPO_ROOT / "build" / "toolchains" / "run" / "testing" / "subject_planner.py"
SUBJECTS_MODULE_PATH = REPO_ROOT / "build" / "toolchains" / "run" / "testing" / "subjects.py"


def load_subjects_module(module_name: str):
    return load_module(SUBJECTS_MODULE_PATH, module_name)


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


class FixtureSubjectPlannerTests(unittest.TestCase):
    def test_planner_selects_mobile_android_buildable_matrix_without_subject_name_coupling(self) -> None:
        planner_module = load_module(PLANNER_MODULE_PATH, "chaos_fixture_subject_planner_mobile_buildable")
        repo_root, manifest = clone_fixture_subject_repo("MobileHelloWorldProof")

        try:
            subject_id = str(manifest["subjectId"])
            run_id = "20260411-fixture-mobile-build-001"
            plan = planner_module.build_plan(
                repo_root,
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
        finally:
            shutil.rmtree(repo_root, ignore_errors=True)

    def test_planner_selects_engine_output_pipeline_without_copying_engine_profile_into_selection(self) -> None:
        planner_module = load_module(PLANNER_MODULE_PATH, "chaos_fixture_subject_planner_engine_output")
        repo_root, manifest = clone_fixture_subject_repo("EngineHostProof")

        try:
            subject_id = str(manifest["subjectId"])
            run_id = "20260410-fixture-engine-output-001"
            plan = planner_module.build_plan(repo_root, subject_id, run_id=run_id)

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
        finally:
            shutil.rmtree(repo_root, ignore_errors=True)

    def test_planner_selects_engine_trace_matrix_and_paths_without_subject_name_coupling(self) -> None:
        planner_module = load_module(PLANNER_MODULE_PATH, "chaos_fixture_subject_planner_engine_trace")
        repo_root, manifest = clone_fixture_subject_repo("EngineHostProof")

        try:
            subjects_module = load_subjects_module("chaos_fixture_subject_planner_engine_trace_subjects")
            subject_id = str(manifest["subjectId"])
            trace_matrix = find_matrix_for_goal(
                subjects_module,
                manifest,
                "correctness.platform",
                required_stage_kind="runtime-engine-trace-compare",
            )
            run_id = "20260410-fixture-engine-trace-001"

            plan = planner_module.build_plan(
                repo_root,
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
        finally:
            shutil.rmtree(repo_root, ignore_errors=True)


if __name__ == "__main__":
    unittest.main()
