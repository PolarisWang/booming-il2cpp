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
                "pipelineId": "managed-runtime-output",
                "stages": [{"kind": "runtime-managed-output"}],
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
                "matrixId": "windows-hotupdate-proof",
                "pipelineId": "managed-runtime-output",
                "supportedGoals": ["correctness.dev"],
                "executionContext": {
                    "runtimeProfile": "hot-update-proof",
                    "toolchainProfile": "dotnet-managed",
                },
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


def make_subject_item() -> dict[str, object]:
    return {
        "subjectId": "FixtureBenchSubject",
        "category": "canonical",
        "sourceModel": "dotnet-solution",
        "dependencyModel": "project-reference",
        "executablePlan": "generated-native",
        "engineeringProfile": "native-executable",
        "orchestration": {},
        "availability": {"windows-x64": "ready"},
        "compatibility": {},
        "defaultGoalId": "correctness.dev",
        "defaultMatrixId": "windows-proof",
        "goalIds": ["correctness.dev", "perf.release"],
        "matrixIds": [
            "windows-proof",
            "windows-hotupdate-proof",
            "windows-native-perf",
            "windows-managed-perf",
            "windows-interpreter-perf",
        ],
        "tags": [],
        "supportedHosts": ["windows"],
        "docRefs": [],
        "moduleIds": [],
        "subsystemIds": [],
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

    def test_declared_unit_test_with_hotupdate_capability_prefers_hotupdate_matrix(self) -> None:
        registry_module = load_registry_module()
        manifest = make_manifest()

        matrix_id, goal_id = registry_module._select_declared_matrix(
            manifest,
            family="declared-unit-test",
            payload={
                "hotUpdateCapability": 1,
            },
            source_entry="",
        )

        self.assertEqual("windows-hotupdate-proof", matrix_id)
        self.assertEqual("correctness.dev", goal_id)

    def test_declared_unit_test_prefers_real_managed_proof_matrix_over_subject_native_default(self) -> None:
        registry_module = load_registry_module()
        manifest = make_manifest()
        manifest["environmentMatrices"].insert(
            1,
            {
                "matrixId": "windows-managed-proof",
                "pipelineId": "managed-runtime-output",
                "supportedGoals": ["correctness.dev"],
                "executionContext": {
                    "hostPlatform": "windows-x64",
                    "targetPlatform": "windows-x64",
                    "toolchainProfile": "dotnet-managed",
                    "runtimeProfile": "managed-proof-output",
                },
            },
        )

        matrix_id, goal_id = registry_module._select_declared_matrix(
            manifest,
            family="declared-unit-test",
            payload={
                "hotUpdateCapability": 0,
            },
            source_entry="FixtureBenchSubject/AsyncAwaitProof::Run()",
        )

        self.assertEqual("windows-managed-proof", matrix_id)
        self.assertEqual("correctness.dev", goal_id)

    def test_declared_registry_item_exposes_capability_owner_support_and_labels(self) -> None:
        registry_module = load_registry_module()
        manifest = make_manifest()

        item = registry_module._declared_registry_item(
            subject_item=make_subject_item(),
            manifest=manifest,
            family="declared-unit-test",
            payload={
                "stableId": "FixtureBenchSubject::Fixture.Tests::Fixture.InterOp::Run()",
                "alias": "native-call-interop-proof",
                "assemblyName": "Fixture.Tests",
                "declaringType": "Fixture.InterOp",
                "methodName": "Run",
                "methodSignature": "Run()",
                "category": 3,
                "capabilityFamily": 7,
                "capabilityItem": 31,
                "archetype": 0,
                "hotUpdateCapability": 0,
                "requires": 16,
                "evidence": 4,
                "priority": 2,
            },
        )

        self.assertEqual(7, item["capabilityFamily"])
        self.assertEqual("Interop And Marshaling", item["capabilityFamilyLabel"])
        self.assertEqual(31, item["capabilityItem"])
        self.assertEqual("Native Call Interop", item["capabilityItemLabel"])
        self.assertEqual("SolutionCorePack", item["ownerSubjectId"])
        self.assertEqual([1, 5], item["supportStates"])
        self.assertEqual(["NativeGenerated", "ExternalRuntime"], item["supportStateLabels"])
        self.assertTrue(item["proofRequired"])
        self.assertTrue(item["benchmarkRequired"])


if __name__ == "__main__":
    unittest.main()
