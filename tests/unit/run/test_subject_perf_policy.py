from __future__ import annotations

import importlib.util
import json
import shutil
import sys
import unittest
import uuid
from pathlib import Path


REPO_ROOT = Path(__file__).resolve().parents[3]
PERF_MODULE_PATH = REPO_ROOT / "build" / "toolchains" / "run" / "testing" / "perf.py"
TEST_TMP_ROOT = REPO_ROOT / "artifacts" / ".tmp-tests" / "subject-perf-policy"


def load_perf_module():
    if not PERF_MODULE_PATH.is_file():
        raise FileNotFoundError(f"perf module missing: {PERF_MODULE_PATH}")

    spec = importlib.util.spec_from_file_location("booming_run_subject_perf", PERF_MODULE_PATH)
    if spec is None or spec.loader is None:
        raise RuntimeError(f"unable to load perf module: {PERF_MODULE_PATH}")

    module = importlib.util.module_from_spec(spec)
    sys.modules[spec.name] = module
    spec.loader.exec_module(module)
    return module


class SubjectPerfPolicyTests(unittest.TestCase):
    @classmethod
    def setUpClass(cls) -> None:
        TEST_TMP_ROOT.mkdir(parents=True, exist_ok=True)

    def test_perf_subject_uses_subject_aware_baseline_path_and_detects_regression(self) -> None:
        perf_module = load_perf_module()

        repo_root = TEST_TMP_ROOT / f"repo-{uuid.uuid4().hex}"
        repo_root.mkdir(parents=True, exist_ok=False)
        try:
            baseline_path = (
                repo_root
                / "tests"
                / "perf"
                / "subjects"
                / "GenericEcho"
                / "windows-perf-release"
                / "baselines"
                / "windows.json"
            )
            baseline_path.parent.mkdir(parents=True, exist_ok=True)
            baseline_path.write_text(
                json.dumps({"meanDurationMs": 11.5, "maxDurationMs": 13.0}),
                encoding="utf-8",
            )

            result = perf_module.evaluate_perf_subject(
                repo_root=repo_root,
                subject_id="GenericEcho",
                matrix_id="windows-perf-release",
                host_platform="windows",
                metrics={"meanDurationMs": 15.25, "maxDurationMs": 16.0},
                update_baseline=False,
            )

            self.assertTrue(
                result["baselinePath"].endswith(
                    "/tests/perf/subjects/GenericEcho/windows-perf-release/baselines/windows.json"
                )
            )
            self.assertFalse(result["baselineUpdated"])
            self.assertEqual("regressed", result["regressionStatus"])
            self.assertEqual(
                ["maxDurationMs", "meanDurationMs"],
                sorted(item["metric"] for item in result["regressions"]),
            )
            self.assertEqual({"meanDurationMs": 11.5, "maxDurationMs": 13.0}, result["baseline"])
        finally:
            shutil.rmtree(repo_root, ignore_errors=True)


if __name__ == "__main__":
    unittest.main()
