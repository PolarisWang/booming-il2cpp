from __future__ import annotations

import json
import unittest
from pathlib import Path

from tests.support import SOLUTION_CORE_PACK_OWNER_MANIFEST_PATH, SOLUTION_CORE_PACK_OWNER_ROOT

REPO_ROOT = Path(__file__).resolve().parents[3]
INTERFACE_DISPATCH_SUBJECT_ROOT = SOLUTION_CORE_PACK_OWNER_ROOT
INTERFACE_DISPATCH_MANIFEST_PATH = SOLUTION_CORE_PACK_OWNER_MANIFEST_PATH
INTERFACE_DISPATCH_TEMPLATE_PATH = (
    REPO_ROOT
    / "src"
    / "managed"
    / "Chaos.IL2CPP.CodeGen"
    / "Templates"
    / "NativeReferenceProof.InterfaceDispatchMessage.cpp.scriban"
)
INTERFACE_DISPATCH_BASELINE_PATH = (
    INTERFACE_DISPATCH_SUBJECT_ROOT
    / "benchmark-baselines"
    / "perf"
    / "windows-native-profile"
    / "windows.json"
)
CONVERT_PERF_BASELINE_PATH = (
    REPO_ROOT
    / "tests"
    / "perf"
    / "convert-golden-multi-project"
    / "baselines"
    / "windows.json"
)
CONVERT_PERF_SCRIPT_PATH = (
    REPO_ROOT
    / "tests"
    / "perf"
    / "convert-golden-multi-project"
    / "check.py"
)


class Phase2PerfGovernanceTests(unittest.TestCase):
    def test_interface_dispatch_subject_declares_native_perf_profile_and_baseline(self) -> None:
        self.assertTrue(INTERFACE_DISPATCH_MANIFEST_PATH.is_file(), msg=f"missing subject manifest: {INTERFACE_DISPATCH_MANIFEST_PATH}")
        self.assertTrue(INTERFACE_DISPATCH_BASELINE_PATH.is_file(), msg=f"missing interface dispatch perf baseline: {INTERFACE_DISPATCH_BASELINE_PATH}")

        manifest = json.loads(INTERFACE_DISPATCH_MANIFEST_PATH.read_text(encoding="utf-8"))
        validation_profiles = dict(manifest.get("validationProfiles") or {})
        validation = dict(manifest.get("validation") or {})
        pipeline_ids = {
            str(pipeline.get("pipelineId") or "")
            for pipeline in list(manifest.get("executionPipelines") or [])
        }
        matrix_ids = {
            str(matrix.get("matrixId") or "")
            for matrix in list(manifest.get("environmentMatrices") or [])
        }

        self.assertEqual(["perf"], validation_profiles["perf-profile"])
        self.assertEqual("native-runtime-perf", validation["perf"]["driver"])
        self.assertIn("native-benchmark", pipeline_ids)
        self.assertIn("windows-native-perf", matrix_ids)

        baseline = json.loads(INTERFACE_DISPATCH_BASELINE_PATH.read_text(encoding="utf-8"))
        self.assertLessEqual(float(baseline["meanDurationMs"]), 80.0)

    def test_interface_dispatch_template_exposes_profile_dispatch_metric_output(self) -> None:
        self.assertTrue(INTERFACE_DISPATCH_TEMPLATE_PATH.is_file(), msg=f"missing template: {INTERFACE_DISPATCH_TEMPLATE_PATH}")
        template_source = INTERFACE_DISPATCH_TEMPLATE_PATH.read_text(encoding="utf-8")

        self.assertIn("CHAOS_VARIANT_PROFILE", template_source)
        self.assertIn("dispatchNanoseconds", template_source)
        self.assertIn("resolve_virtual_method", template_source)
        self.assertIn("invoke_virtual", template_source)

    def test_convert_perf_suite_wires_real_golden_multi_project_check(self) -> None:
        self.assertTrue(CONVERT_PERF_BASELINE_PATH.is_file(), msg=f"missing convert perf baseline: {CONVERT_PERF_BASELINE_PATH}")
        self.assertTrue(CONVERT_PERF_SCRIPT_PATH.is_file(), msg=f"missing convert perf script: {CONVERT_PERF_SCRIPT_PATH}")

        baseline = json.loads(CONVERT_PERF_BASELINE_PATH.read_text(encoding="utf-8"))
        script_source = CONVERT_PERF_SCRIPT_PATH.read_text(encoding="utf-8")

        self.assertLessEqual(float(baseline["convertDurationMs"]), 10000.0)
        self.assertIn("tests", script_source)
        self.assertIn("fixtures", script_source)
        self.assertIn("solution-multi-project", script_source)
        self.assertNotIn('REPO_ROOT / "subjects" / "SolutionMultiProject"', script_source)
        self.assertIn("evaluate_perf_suite", script_source)
        self.assertIn("convert-golden-multi-project", script_source)


if __name__ == "__main__":
    unittest.main()
