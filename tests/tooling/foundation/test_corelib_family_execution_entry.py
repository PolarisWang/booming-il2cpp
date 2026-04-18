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
PHASE2_DIR = (
    REPO_ROOT
    / "docs"
    / "dev"
    / "completed"
    / "20260418-04-phase-2-system-private-corelib-certification-substrate"
)
TEST_TMP_ROOT = REPO_ROOT / "artifacts" / ".tmp-tests" / "foundation-pack-family-execution"


class CorelibSemanticFamilyExecutionEntryTests(unittest.TestCase):
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
                "phase3",
                "--phase2-dir",
                str(PHASE2_DIR),
                "--output-dir",
                str(self.output_dir),
                "--task-id",
                "corelib-semantic-family-execution-entry-test",
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

        path = self.output_dir / "semantic-family-execution-entry-v1-01.json"
        self.assertTrue(path.is_file(), msg=f"missing artifact: {path}")
        return json.loads(path.read_text(encoding="utf-8"))

    def test_generator_emits_first_family_batch_execution_entry(self) -> None:
        execution_entry = dict(self._run_generator())

        self.assertEqual("corelib-semantic-family-batch-01", execution_entry["batchName"])
        self.assertEqual(
            ["managed-cil", "internal-call", "field-metadata"],
            execution_entry["selectedBatchFamilyNames"],
        )
        self.assertTrue(
            str(execution_entry["registrySnapshotPath"]).endswith("artifacts/tests/registry/current/index.json")
        )

        for tfm in ("net8.0", "net10.0"):
            tfm_entry = dict(dict(execution_entry["targetFrameworks"])[tfm])
            families = dict(tfm_entry["selectedFamilies"])

            self.assertEqual(3, tfm_entry["selectedFamilyCount"])
            self.assertGreater(tfm_entry["selectedMemberCount"], 0)
            self.assertIn("managed-cil", families)
            self.assertIn("internal-call", families)
            self.assertIn("field-metadata", families)

    def test_execution_entry_wires_native_proof_conformance_and_benchmark_commands(self) -> None:
        execution_entry = dict(self._run_generator())

        for tfm in ("net8.0", "net10.0"):
            families = dict(dict(dict(execution_entry["targetFrameworks"])[tfm])["selectedFamilies"])

            managed_cil = dict(families["managed-cil"])
            managed_proofs = {item["alias"]: item for item in managed_cil["nativeProofs"]}
            managed_conformance = {item["id"]: item for item in managed_cil["conformanceGates"]}
            managed_benchmarks = {item["alias"]: item for item in managed_cil["benchmarks"]}

            self.assertIn("async-await-proof", managed_proofs)
            self.assertIn("interface-dispatch-proof", managed_proofs)
            self.assertEqual("windows-native-check", managed_proofs["async-await-proof"]["defaultMatrixId"])
            self.assertIn(
                "engineering-validation/SolutionCorePack/native-executable-smoke",
                managed_conformance,
            )
            self.assertEqual(
                "windows-archetype-simple-lib-managed-output",
                managed_conformance["engineering-validation/SolutionCorePack/managed-build"]["defaultMatrixId"],
            )
            self.assertIn("arithmetic-bench", managed_benchmarks)
            self.assertIn("dispatch-bench", managed_benchmarks)
            self.assertEqual("windows-managed-perf", managed_benchmarks["dispatch-bench"]["defaultMatrixId"])

            internal_call = dict(families["internal-call"])
            internal_proofs = {item["alias"]: item for item in internal_call["nativeProofs"]}
            internal_benchmarks = {item["alias"]: item for item in internal_call["benchmarks"]}

            self.assertIn("runtime-helpers-proof", internal_proofs)
            self.assertIn("threading-proof", internal_proofs)
            self.assertIn("monitor-locking-bench", internal_benchmarks)
            self.assertIn("task-scheduling-bench", internal_benchmarks)

            field_metadata = dict(families["field-metadata"])
            field_proofs = {item["alias"]: item for item in field_metadata["nativeProofs"]}
            field_benchmarks = {item["alias"]: item for item in field_metadata["benchmarks"]}

            self.assertIn("member-metadata-lookup-proof", field_proofs)
            self.assertIn("custom-attribute-lookup-proof", field_proofs)
            self.assertIn("member-metadata-lookup-bench", field_benchmarks)
            self.assertEqual(
                "run test engineering-workload --id engineering-workload/SolutionCorePack/codegen",
                field_metadata["engineeringWorkloads"][0]["canonicalCommand"],
            )


if __name__ == "__main__":
    unittest.main()
