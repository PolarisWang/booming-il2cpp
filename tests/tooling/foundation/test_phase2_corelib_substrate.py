from __future__ import annotations

import json
import shutil
import subprocess
import unittest
import uuid
from pathlib import Path


REPO_ROOT = Path(__file__).resolve().parents[3]
PROJECT_PATH = REPO_ROOT / "src" / "tools" / "Chaos.IL2CPP.Tools.FoundationPack.Analysis" / "Chaos.IL2CPP.Tools.FoundationPack.Analysis.csproj"
CATALOG_PATH = (
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
TEST_TMP_ROOT = REPO_ROOT / "artifacts" / ".tmp-tests" / "foundation-pack-phase2"


class Phase2CorelibSubstrateGeneratorTests(unittest.TestCase):
    @classmethod
    def setUpClass(cls) -> None:
        TEST_TMP_ROOT.mkdir(parents=True, exist_ok=True)

    def setUp(self) -> None:
        self.output_dir = TEST_TMP_ROOT / f"run-{uuid.uuid4().hex}"
        self.output_dir.mkdir(parents=True, exist_ok=False)

    def tearDown(self) -> None:
        shutil.rmtree(self.output_dir, ignore_errors=True)

    def _run_generator(self) -> dict[str, object]:
        completed = subprocess.run(
            [
                "dotnet",
                "run",
                "--project",
                str(PROJECT_PATH),
                "--",
                "phase2",
                "--catalog",
                str(CATALOG_PATH),
                "--phase1-dir",
                str(PHASE1_DIR),
                "--output-dir",
                str(self.output_dir),
                "--task-id",
                "phase2-corelib-substrate-test",
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

        def load_json(file_name: str) -> object:
            path = self.output_dir / file_name
            self.assertTrue(path.is_file(), msg=f"missing artifact: {path}")
            return json.loads(path.read_text(encoding="utf-8"))

        return {
            "semantic_ir": load_json("assembly-semantic-ir-system-private-corelib-v1-01.json"),
            "helper_contracts": load_json("runtime-helper-contracts-v1-01.json"),
            "surface_proof": load_json("corelib-surface-proof-report-v1-01.json"),
            "semantic_proof": load_json("corelib-semantic-proof-report-v1-01.json"),
            "benchmark": load_json("corelib-substrate-benchmark-v1-01.json"),
        }

    def test_phase2_generator_emits_corelib_semantic_ir_and_helper_contracts(self) -> None:
        artifacts = self._run_generator()

        semantic_ir = dict(artifacts["semantic_ir"])
        helper_contracts = dict(artifacts["helper_contracts"])

        for tfm in ("net8.0", "net10.0"):
            tfm_semantic = dict(dict(semantic_ir["targetFrameworks"])[tfm])
            tfm_helper = dict(dict(helper_contracts["targetFrameworks"])[tfm])
            public_types = dict(tfm_semantic["publicTypes"])
            helper_kind_counts = dict(tfm_helper["helperKindCounts"])

            self.assertEqual("System.Private.CoreLib", tfm_semantic["assemblyName"])
            self.assertIn("System.Object", public_types)
            self.assertGreater(tfm_semantic["publicTypeCount"], 0)
            self.assertGreater(tfm_semantic["publicMemberCount"], 0)
            self.assertGreater(tfm_semantic["phase1CandidateAssemblyCount"], 0)
            self.assertGreaterEqual(helper_kind_counts["managed-cil"], 0)
            self.assertGreaterEqual(helper_kind_counts["internal-call"], 0)
            self.assertGreaterEqual(helper_kind_counts["pinvoke"], 0)
            self.assertGreater(tfm_helper["classifiedPublicMemberCount"], 0)

    def test_phase2_generator_closes_corelib_surface_and_semantic_proofs(self) -> None:
        artifacts = self._run_generator()

        surface_proof = dict(artifacts["surface_proof"])
        semantic_proof = dict(artifacts["semantic_proof"])
        benchmark = dict(artifacts["benchmark"])

        for tfm in ("net8.0", "net10.0"):
            tfm_surface = dict(dict(surface_proof["targetFrameworks"])[tfm])
            tfm_semantic = dict(dict(semantic_proof["targetFrameworks"])[tfm])
            tfm_benchmark = dict(dict(benchmark["targetFrameworks"])[tfm])

            self.assertEqual(0, tfm_surface["unresolvedPublicTypeCount"])
            self.assertEqual(0, tfm_surface["issueCount"])
            self.assertEqual(0, tfm_semantic["unclassifiedPublicMemberCount"])
            self.assertEqual(0, tfm_semantic["issueCount"])
            self.assertGreater(tfm_semantic["classifiedPublicMemberCount"], 0)
            self.assertGreaterEqual(tfm_benchmark["elapsedMs"], 0)
            self.assertGreater(tfm_benchmark["publicMemberCount"], 0)


if __name__ == "__main__":
    unittest.main()
