from __future__ import annotations

import importlib.util
import json
import shutil
import sys
import unittest
import uuid
from pathlib import Path
from unittest.mock import patch


REPO_ROOT = Path(__file__).resolve().parents[3]
PERF_DASHBOARD_MODULE_PATH = REPO_ROOT / "build" / "toolchains" / "run" / "testing" / "perf_dashboard.py"
TEST_TMP_ROOT = REPO_ROOT / "artifacts" / ".tmp-tests" / "perf-dashboard"


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


class PerfDashboardTests(unittest.TestCase):
    @classmethod
    def setUpClass(cls) -> None:
        TEST_TMP_ROOT.mkdir(parents=True, exist_ok=True)

    def _make_repo_root(self) -> Path:
        repo_root = TEST_TMP_ROOT / uuid.uuid4().hex
        repo_root.mkdir(parents=True, exist_ok=False)
        return repo_root

    def test_build_perf_dashboard_config_prefers_declared_case_when_manifest_workload_entry_is_stale(self) -> None:
        dashboard_module = load_module(PERF_DASHBOARD_MODULE_PATH, "chaos_perf_dashboard_declared_summary")
        repo_root = self._make_repo_root()
        baseline_path = repo_root / "subjects" / "FixturePerfSubject" / "baselines" / "perf" / "windows-native-perf" / "windows.json"
        baseline_path.parent.mkdir(parents=True, exist_ok=True)
        baseline_path.write_text(json.dumps({"meanDurationMs": 1.0}) + "\n", encoding="utf-8")

        try:
            manifest = {
                "subjectId": "FixturePerfSubject",
                "workloadEntry": "LegacyBenchmarks/StaleEntry::RunWorkload()",
                "environmentMatrices": [
                    {
                        "matrixId": "windows-native-perf",
                        "supportedGoals": ["perf.release"],
                        "executionContext": {
                            "hostPlatform": "windows-x64",
                            "targetPlatform": "windows-x64",
                            "toolchainProfile": "msvc-reference",
                            "runtimeProfile": "native-perf-release",
                        },
                    }
                ],
            }
            declared_catalog = {
                "declaredBenchmarks": [
                    {
                        "stableId": "FixturePerfSubject::FixturePerfSubject::FixtureBenchmarks.LiveEntry::RunWorkload()",
                        "alias": "live-bench",
                        "assemblyName": "FixtureBenchmarks",
                        "declaringType": "FixtureBenchmarks.LiveEntry",
                        "methodName": "RunWorkload",
                        "methodSignature": "RunWorkload()",
                        "category": 1,
                        "capabilityFamily": 1,
                        "capabilityItem": 1,
                        "archetype": 1,
                        "hotUpdateCapability": 0,
                        "requires": 0,
                        "metrics": 1,
                        "modes": 3,
                        "warmupCount": 1,
                        "iterationCount": 2,
                        "invocationCount": 3,
                    }
                ]
            }

            with patch.object(
                dashboard_module.subjects_module,
                "discover_perf_subject_records",
                return_value=[{"subjectId": "FixturePerfSubject", "manifest": manifest}],
            ):
                with patch.object(
                    dashboard_module.compiled_catalog_module,
                    "build_subject_declared_test_catalog",
                    return_value=declared_catalog,
                ):
                    with patch.object(
                        dashboard_module.path_resolver_module,
                        "subject_perf_baseline_path",
                        return_value=baseline_path,
                    ):
                        config = dashboard_module.build_perf_dashboard_config(repo_root)

            self.assertEqual("ok", config["status"])
            self.assertEqual(1, len(config["entries"]))
            entry = config["entries"][0]
            self.assertEqual(
                "FixtureBenchmarks/LiveEntry::RunWorkload()",
                entry["summaryWorkloadEntry"],
            )
            self.assertEqual(
                "live-bench",
                entry["summaryBenchmarkCase"]["displayName"],
            )
        finally:
            shutil.rmtree(repo_root, ignore_errors=True)

    def test_build_perf_dashboard_config_falls_back_to_manifest_workload_entry_without_declared_case(self) -> None:
        dashboard_module = load_module(PERF_DASHBOARD_MODULE_PATH, "chaos_perf_dashboard_manifest_fallback")
        repo_root = self._make_repo_root()
        baseline_path = repo_root / "subjects" / "FixturePerfSubject" / "baselines" / "perf" / "windows-managed-perf" / "windows.json"
        baseline_path.parent.mkdir(parents=True, exist_ok=True)
        baseline_path.write_text(json.dumps({"meanDurationMs": 2.0}) + "\n", encoding="utf-8")

        try:
            manifest = {
                "subjectId": "FixturePerfSubject",
                "workloadEntry": "FixtureBenchmarks/ManifestEntry::RunWorkload()",
                "environmentMatrices": [
                    {
                        "matrixId": "windows-managed-perf",
                        "supportedGoals": ["perf.release"],
                        "executionContext": {
                            "hostPlatform": "windows-x64",
                            "targetPlatform": "windows-x64",
                            "toolchainProfile": "dotnet-managed",
                            "runtimeProfile": "managed-perf-release",
                        },
                    }
                ],
            }

            with patch.object(
                dashboard_module.subjects_module,
                "discover_perf_subject_records",
                return_value=[{"subjectId": "FixturePerfSubject", "manifest": manifest}],
            ):
                with patch.object(
                    dashboard_module.compiled_catalog_module,
                    "build_subject_declared_test_catalog",
                    return_value={"declaredBenchmarks": []},
                ):
                    with patch.object(
                        dashboard_module.path_resolver_module,
                        "subject_perf_baseline_path",
                        return_value=baseline_path,
                    ):
                        config = dashboard_module.build_perf_dashboard_config(repo_root)

            entry = config["entries"][0]
            self.assertEqual(
                "FixtureBenchmarks/ManifestEntry::RunWorkload()",
                entry["summaryWorkloadEntry"],
            )
            self.assertIsNone(entry["summaryBenchmarkCase"])
        finally:
            shutil.rmtree(repo_root, ignore_errors=True)


if __name__ == "__main__":
    unittest.main()
