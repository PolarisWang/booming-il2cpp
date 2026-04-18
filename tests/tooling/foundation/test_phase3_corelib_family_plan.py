from __future__ import annotations

import json
import shutil
import subprocess
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
TEST_TMP_ROOT = REPO_ROOT / "artifacts" / ".tmp-tests" / "foundation-pack-phase3"


class Phase3CorelibFamilyPlanGeneratorTests(unittest.TestCase):
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
                "phase3",
                "--phase2-dir",
                str(PHASE2_DIR),
                "--output-dir",
                str(self.output_dir),
                "--task-id",
                "phase3-corelib-family-plan-test",
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
            "family_plan": load_json("semantic-family-plan-v1-01.json"),
            "proof_matrix": load_json("family-proof-matrix-v1-01.json"),
            "benchmark": load_json("family-priority-benchmark-v1-01.json"),
        }

    def test_phase3_generator_emits_family_plan_and_proof_matrix(self) -> None:
        artifacts = self._run_generator()

        family_plan = dict(artifacts["family_plan"])
        proof_matrix = dict(artifacts["proof_matrix"])

        for tfm in ("net8.0", "net10.0"):
            tfm_plan = dict(dict(family_plan["targetFrameworks"])[tfm])
            tfm_matrix = dict(dict(proof_matrix["targetFrameworks"])[tfm])
            families = dict(tfm_plan["families"])

            self.assertGreater(tfm_plan["familyCount"], 0)
            self.assertGreater(tfm_plan["totalPublicMemberCount"], 0)
            self.assertIn("managed-cil", families)
            self.assertIn("internal-call", families)
            self.assertIn("field-metadata", families)
            self.assertEqual(tfm_plan["familyCount"], tfm_matrix["familyCount"])
            self.assertEqual(0, tfm_matrix["issueCount"])

    def test_phase3_generator_emits_priority_benchmark(self) -> None:
        artifacts = self._run_generator()

        benchmark = dict(artifacts["benchmark"])

        for tfm in ("net8.0", "net10.0"):
            tfm_benchmark = dict(dict(benchmark["targetFrameworks"])[tfm])
            self.assertGreater(tfm_benchmark["familyCount"], 0)
            self.assertGreater(tfm_benchmark["totalPublicMemberCount"], 0)
            self.assertGreaterEqual(tfm_benchmark["elapsedMs"], 0)


if __name__ == "__main__":
    unittest.main()
