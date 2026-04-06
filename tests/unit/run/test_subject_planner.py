from __future__ import annotations

import importlib.util
import sys
import unittest
from pathlib import Path


REPO_ROOT = Path(__file__).resolve().parents[3]
PLANNER_MODULE_PATH = REPO_ROOT / "build" / "toolchains" / "run" / "testing" / "subject_planner.py"


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


class SubjectPlannerTests(unittest.TestCase):
    def test_planner_uses_subject_defaults_and_subject_artifact_layout(self) -> None:
        planner_module = load_module(PLANNER_MODULE_PATH, "booming_subject_planner_default")

        plan = planner_module.build_plan(REPO_ROOT, "HelloWorldObject")

        self.assertEqual("v1", plan["planVersion"])
        self.assertEqual("HelloWorldObject", plan["request"]["subjectId"])
        self.assertIsNone(plan["request"]["goalId"])
        self.assertIsNone(plan["request"]["matrixId"])

        selection = plan["selection"]
        self.assertEqual("HelloWorldObject", selection["subjectId"])
        self.assertEqual("correctness.dev", selection["goalId"])
        self.assertEqual("windows-dev-output", selection["matrixId"])
        self.assertEqual("proof-runtime-output", selection["pipelineId"])
        self.assertEqual("runtime", selection["artifactPlan"]["evidenceTerminalBucket"])

        self.assertEqual(
            "artifacts/subjects/HelloWorldObject",
            plan["artifactsRoot"]["subjectRoot"],
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
        self.assertEqual(
            "artifacts/subjects/HelloWorldObject/shared/source/source.manifest.json",
            plan["stagePlan"][0]["paths"]["manifestPath"],
        )
        self.assertEqual(
            "artifacts/subjects/HelloWorldObject/shared/generated/generated.manifest.json",
            plan["stagePlan"][3]["paths"]["manifestPath"],
        )
        self.assertEqual(
            "artifacts/subjects/HelloWorldObject/matrices/windows-dev-output/report.json",
            plan["stagePlan"][-1]["paths"]["manifestPath"],
        )
        self.assertTrue(all(stage["executionMode"] in {"executed", "reused", "invalidated"} for stage in plan["stagePlan"]))

    def test_planner_selects_trace_pipeline_for_windows_reference_trace(self) -> None:
        planner_module = load_module(PLANNER_MODULE_PATH, "booming_subject_planner_trace")

        plan = planner_module.build_plan(
            REPO_ROOT,
            "HelloWorldObject",
            goal_id="correctness.platform",
            matrix_id="windows-reference-trace",
        )

        self.assertEqual("windows-reference-trace", plan["selection"]["matrixId"])
        self.assertEqual("proof-runtime-trace", plan["selection"]["pipelineId"])
        self.assertEqual(
            [
                "source-resolve",
                "host-input-build",
                "analysis-frontend",
                "generated-native-proof",
                "build-target",
                "runtime-observe",
                "runtime-trace-compare",
                "report-assemble",
            ],
            [stage["stageId"] for stage in plan["stagePlan"]],
        )
        self.assertEqual(
            "artifacts/subjects/HelloWorldObject/matrices/windows-reference-trace/runtime/runtime.manifest.json",
            plan["stagePlan"][6]["paths"]["manifestPath"],
        )
        self.assertEqual(
            ["artifacts/subjects/HelloWorldObject/matrices/windows-reference-trace/runtime/trace-compare.report.json"],
            plan["stagePlan"][6]["paths"]["reportPaths"],
        )

    def test_planner_rejects_goal_matrix_mismatch(self) -> None:
        planner_module = load_module(PLANNER_MODULE_PATH, "booming_subject_planner_mismatch")

        with self.assertRaisesRegex(ValueError, "does not support goal"):
            planner_module.build_plan(
                REPO_ROOT,
                "HelloWorldObject",
                goal_id="correctness.platform",
                matrix_id="windows-dev-output",
            )


if __name__ == "__main__":
    unittest.main()
