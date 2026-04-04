from __future__ import annotations

import importlib.util
import json
import sys
import tempfile
import unittest
from pathlib import Path


REPO_ROOT = Path(__file__).resolve().parents[3]
PERF_MODULE_PATH = REPO_ROOT / "build" / "toolchains" / "run" / "testing" / "perf.py"


def load_perf_module():
    if not PERF_MODULE_PATH.is_file():
        raise FileNotFoundError(f"perf module missing: {PERF_MODULE_PATH}")

    spec = importlib.util.spec_from_file_location("booming_run_perf", PERF_MODULE_PATH)
    if spec is None or spec.loader is None:
        raise RuntimeError(f"unable to load perf module: {PERF_MODULE_PATH}")

    module = importlib.util.module_from_spec(spec)
    sys.modules[spec.name] = module
    spec.loader.exec_module(module)
    return module


class PerfPolicyTests(unittest.TestCase):
    def test_perf_suite_uses_repo_baseline_file_without_auto_rewriting(self) -> None:
        perf_module = load_perf_module()

        with tempfile.TemporaryDirectory() as temp_dir:
            repo_root = Path(temp_dir)
            baseline_path = repo_root / "tests" / "perf" / "startup" / "baselines" / "macos.json"
            baseline_path.parent.mkdir(parents=True, exist_ok=True)
            baseline_path.write_text(json.dumps({"startupMs": 120}), encoding="utf-8")

            result = perf_module.evaluate_perf_suite(
                repo_root=repo_root,
                suite="startup",
                host_platform="macos",
                metrics={"startupMs": 118},
                update_baseline=False,
            )

            self.assertTrue(result["baselinePath"].endswith("/tests/perf/startup/baselines/macos.json"))
            self.assertFalse(result["baselineUpdated"])
            self.assertEqual({"startupMs": 120}, json.loads(baseline_path.read_text(encoding="utf-8")))


if __name__ == "__main__":
    unittest.main()
