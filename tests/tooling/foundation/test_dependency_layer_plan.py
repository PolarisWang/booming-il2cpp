from __future__ import annotations

import json
import shutil
import subprocess
import sys
import unittest
import uuid
from pathlib import Path


REPO_ROOT = Path(__file__).resolve().parents[3]
PROJECT_PATH = REPO_ROOT / "src" / "tools" / "Chaos.IL2CPP.Tools.FoundationPack.Analysis" / "Chaos.IL2CPP.Tools.FoundationPack.Analysis.csproj"
PHASE0_CATALOG_PATH = (
    REPO_ROOT
    / "docs"
    / "dev"
    / "completed"
    / "20260418-02-phase-0-framework-pack-catalog-and-asset-policy"
    / "framework-catalog-v1-01.json"
)
PHASE1_DIR = (
    REPO_ROOT
    / "docs"
    / "dev"
    / "completed"
    / "20260418-03-phase-1-contract-facade-and-shim-certification-lane"
)
PHASE2_DIR = (
    REPO_ROOT
    / "docs"
    / "dev"
    / "completed"
    / "20260418-04-phase-2-system-private-corelib-certification-substrate"
)
PHASE3_DIR = (
    REPO_ROOT
    / "docs"
    / "dev"
    / "completed"
    / "20260418-05-phase-3-system-private-corelib-semantic-family-nativeization"
)
TEST_TMP_ROOT = REPO_ROOT / "artifacts" / ".tmp-tests" / "foundation-pack-dependency-layer"


class DependencyLayerNativeizationGeneratorTests(unittest.TestCase):
    @classmethod
    def setUpClass(cls) -> None:
        TEST_TMP_ROOT.mkdir(parents=True, exist_ok=True)

    def setUp(self) -> None:
        self.output_dir = TEST_TMP_ROOT / f"run-{uuid.uuid4().hex}"
        self.output_dir.mkdir(parents=True, exist_ok=False)

    def tearDown(self) -> None:
        shutil.rmtree(self.output_dir, ignore_errors=True)

    def _run_generator(self) -> dict[str, object]:
        refresh_completed = subprocess.run(
            [
                sys.executable,
                "build/toolchains/run/run.py",
                "test",
                "registry",
                "refresh",
                "--json",
            ],
            cwd=REPO_ROOT,
            capture_output=True,
            text=True,
            check=False,
        )
        self.assertEqual(
            0,
            refresh_completed.returncode,
            msg=f"registry refresh stdout:\n{refresh_completed.stdout}\n\nstderr:\n{refresh_completed.stderr}",
        )

        completed = subprocess.run(
            [
                "dotnet",
                "run",
                "--project",
                str(PROJECT_PATH),
                "--",
                "dependency-layer",
                "--catalog",
                str(PHASE0_CATALOG_PATH),
                "--phase1-dir",
                str(PHASE1_DIR),
                "--phase2-dir",
                str(PHASE2_DIR),
                "--phase3-dir",
                str(PHASE3_DIR),
                "--output-dir",
                str(self.output_dir),
                "--task-id",
                "dependency-layer-nativeization-test",
            ],
            cwd=REPO_ROOT,
            capture_output=True,
            text=True,
            check=False,
        )

        self.assertEqual(
            0,
            completed.returncode,
            msg=f"stdout:\n{completed.stdout}\n\nstderr:\n{completed.stderr}",
        )

        def load_json(path: Path) -> object:
            self.assertTrue(path.is_file(), msg=f"missing artifact: {path}")
            return json.loads(path.read_text(encoding="utf-8"))

        assembly_plan_dir = self.output_dir / "assembly-nativeization-plan"
        self.assertTrue(assembly_plan_dir.is_dir(), msg=f"missing artifact directory: {assembly_plan_dir}")
        assembly_plans = {
            path.name: load_json(path)
            for path in sorted(assembly_plan_dir.glob("*.json"))
        }

        return {
            "layer_plan": load_json(self.output_dir / "layer-plan-v1-01.json"),
            "proof_benchmark_lane": load_json(self.output_dir / "proof-benchmark-lane-v1-01.json"),
            "assembly_plans": assembly_plans,
        }

    def test_generator_emits_layer_plan_and_first_ready_assembly_plans(self) -> None:
        artifacts = self._run_generator()

        layer_plan = dict(artifacts["layer_plan"])
        assembly_plans = dict(artifacts["assembly_plans"])

        input_authorities = dict(layer_plan["inputAuthorities"])
        self.assertTrue(str(input_authorities["phase0CatalogPath"]).endswith("framework-catalog-v1-01.json"))
        self.assertTrue(str(input_authorities["phase1Directory"]).endswith("20260418-03-phase-1-contract-facade-and-shim-certification-lane"))
        self.assertTrue(str(input_authorities["phase2Directory"]).endswith("20260418-04-phase-2-system-private-corelib-certification-substrate"))
        self.assertTrue(str(input_authorities["phase3Directory"]).endswith("20260418-05-phase-3-system-private-corelib-semantic-family-nativeization"))
        self.assertGreater(len(assembly_plans), 0)

        for tfm in ("net8.0", "net10.0"):
            tfm_plan = dict(dict(layer_plan["targetFrameworks"])[tfm])
            layers = dict(tfm_plan["layers"])

            self.assertGreater(tfm_plan["candidateAssemblyCount"], 0)
            self.assertGreater(tfm_plan["layerCount"], 0)
            self.assertIn(tfm_plan["firstReadyLayerName"], layers)
            self.assertTrue(any(dict(layer)["blockedAssemblyCount"] > 0 for layer in layers.values()))

            first_ready_layer = dict(layers[tfm_plan["firstReadyLayerName"]])
            self.assertEqual("ready", first_ready_layer["readinessStatus"])
            self.assertGreater(first_ready_layer["readyAssemblyCount"], 0)
            self.assertGreater(len(first_ready_layer["assemblyPlanPaths"]), 0)

        for file_name, payload in assembly_plans.items():
            plan = dict(payload)
            self.assertEqual("first-ready-layer", plan["selectionReason"], msg=file_name)
            self.assertEqual("SolutionCorePack", plan["ownerSubjectId"], msg=file_name)
            self.assertGreater(len(dict(plan["targetFrameworks"])), 0, msg=file_name)
            self.assertGreater(len(plan["nativeProofs"]), 0, msg=file_name)
            self.assertGreater(len(plan["benchmarks"]), 0, msg=file_name)
            self.assertGreater(len(plan["conformanceGates"]), 0, msg=file_name)
            self.assertGreater(len(plan["engineeringWorkloads"]), 0, msg=file_name)

    def test_generator_emits_per_layer_proof_and_benchmark_contracts(self) -> None:
        artifacts = self._run_generator()

        layer_plan = dict(artifacts["layer_plan"])
        proof_benchmark_lane = dict(artifacts["proof_benchmark_lane"])

        self.assertEqual("SolutionCorePack", proof_benchmark_lane["ownerSubjectId"])
        self.assertTrue(
            str(proof_benchmark_lane["registrySnapshotPath"]).endswith("artifacts/tests/registry/current/index.json")
        )

        for tfm in ("net8.0", "net10.0"):
            tfm_plan = dict(dict(layer_plan["targetFrameworks"])[tfm])
            tfm_lane = dict(dict(proof_benchmark_lane["targetFrameworks"])[tfm])
            self.assertEqual(tfm_plan["layerCount"], tfm_lane["layerCount"])

            ready_layer = dict(dict(tfm_lane["layers"])[tfm_plan["firstReadyLayerName"]])
            proof_aliases = {item["alias"] for item in ready_layer["nativeProofs"]}
            benchmark_aliases = {item["alias"] for item in ready_layer["benchmarks"]}
            conformance_ids = {item["id"] for item in ready_layer["conformanceGates"]}
            workload_ids = {item["id"] for item in ready_layer["engineeringWorkloads"]}
            monitor_signals = set(ready_layer["monitorSignals"])

            self.assertIn("aot-closure-validation-proof", proof_aliases)
            self.assertIn("required-instantiation-closure-proof", proof_aliases)
            self.assertIn("required-instantiation-closure-bench", benchmark_aliases)
            self.assertIn("engineering-validation/SolutionCorePack/native-executable-smoke", conformance_ids)
            self.assertIn("engineering-workload/SolutionCorePack/native-link", workload_ids)
            self.assertIn("certified-assembly-count", monitor_signals)
            self.assertIn("layer-nativeization-throughput", monitor_signals)


if __name__ == "__main__":
    unittest.main()
