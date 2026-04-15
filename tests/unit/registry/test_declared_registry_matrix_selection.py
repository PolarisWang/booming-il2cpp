from __future__ import annotations

import importlib.util
import sys
import unittest
from pathlib import Path


REPO_ROOT = Path(__file__).resolve().parents[3]
REGISTRY_MODULE_PATH = REPO_ROOT / "build" / "toolchains" / "run" / "testing" / "registry.py"


def load_registry_module():
    if not REGISTRY_MODULE_PATH.is_file():
        raise FileNotFoundError(f"registry module missing: {REGISTRY_MODULE_PATH}")

    spec = importlib.util.spec_from_file_location("chaos_run_declared_registry_matrix_selection", REGISTRY_MODULE_PATH)
    if spec is None or spec.loader is None:
        raise RuntimeError(f"unable to load registry module: {REGISTRY_MODULE_PATH}")

    module = importlib.util.module_from_spec(spec)
    sys.modules[spec.name] = module
    spec.loader.exec_module(module)
    return module


def make_manifest() -> dict[str, object]:
    return {
        "subjectId": "FixtureBenchSubject",
        "defaultGoal": "correctness.dev",
        "defaultMatrix": "windows-proof",
        "executionPipelines": [
            {
                "pipelineId": "proof-runtime-output",
                "stages": [{"kind": "runtime-observe"}],
            },
            {
                "pipelineId": "native-benchmark",
                "stages": [{"kind": "native-runtime-perf"}],
            },
            {
                "pipelineId": "managed-benchmark",
                "stages": [{"kind": "runtime-perf-collect"}],
            },
            {
                "pipelineId": "interpreter-benchmark",
                "stages": [{"kind": "interpreter-runtime-perf"}],
            },
        ],
        "environmentMatrices": [
            {
                "matrixId": "windows-proof",
                "pipelineId": "proof-runtime-output",
                "supportedGoals": ["correctness.dev"],
            },
            {
                "matrixId": "windows-native-perf",
                "pipelineId": "native-benchmark",
                "supportedGoals": ["perf.release"],
            },
            {
                "matrixId": "windows-managed-perf",
                "pipelineId": "managed-benchmark",
                "supportedGoals": ["perf.release"],
            },
            {
                "matrixId": "windows-interpreter-perf",
                "pipelineId": "interpreter-benchmark",
                "supportedGoals": ["perf.release"],
            },
        ],
    }


class DeclaredRegistryMatrixSelectionTests(unittest.TestCase):
    def test_declared_benchmark_prefers_matching_mode_matrix_without_workload_string_matching(self) -> None:
        registry_module = load_registry_module()
        manifest = make_manifest()

        matrix_id, goal_id = registry_module._select_declared_matrix(
            manifest,
            family="declared-benchmark",
            payload={
                "modes": registry_module.declared_metadata_labels_module.MODE_FLAGS["managed"],
            },
            source_entry="",
        )

        self.assertEqual("windows-managed-perf", matrix_id)
        self.assertEqual("perf.release", goal_id)

    def test_declared_benchmark_with_all_modes_keeps_subject_perf_matrix_preference_order(self) -> None:
        registry_module = load_registry_module()
        manifest = make_manifest()

        matrix_id, goal_id = registry_module._select_declared_matrix(
            manifest,
            family="declared-benchmark",
            payload={"modes": 0},
            source_entry="",
        )

        self.assertEqual("windows-native-perf", matrix_id)
        self.assertEqual("perf.release", goal_id)


if __name__ == "__main__":
    unittest.main()
