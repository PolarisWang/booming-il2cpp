from __future__ import annotations

import json
import shutil
import subprocess
import unittest
import uuid
from pathlib import Path


REPO_ROOT = Path(__file__).resolve().parents[3]
PROJECT_PATH = REPO_ROOT / "src" / "tools" / "Chaos.FoundationPack.Analysis" / "Chaos.FoundationPack.Analysis.csproj"
CATALOG_PATH = (
    REPO_ROOT
    / "docs"
    / "dev"
    / "completed"
    / "20260418-02-phase-0-framework-pack-catalog-and-asset-policy"
    / "framework-catalog-v1-01.json"
)
TEST_TMP_ROOT = REPO_ROOT / "artifacts" / ".tmp-tests" / "foundation-pack-phase1"


class Phase1ContractLaneGeneratorTests(unittest.TestCase):
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
                "phase1",
                "--catalog",
                str(CATALOG_PATH),
                "--output-dir",
                str(self.output_dir),
                "--task-id",
                "phase1-contract-lane-test",
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
            "classification": load_json("assembly-classification-v1-01.json"),
            "surface_manifest": load_json("public-surface-manifest-v1-01.json"),
            "surface_proof": load_json("surface-proof-report-v1-01.json"),
            "binding_proof": load_json("native-binding-proof-report-v1-01.json"),
            "benchmark": load_json("manifest-benchmark-v1-01.json"),
        }

    def test_phase1_generator_emits_expected_seed_and_candidate_artifacts(self) -> None:
        artifacts = self._run_generator()

        classification = dict(artifacts["classification"])
        target_frameworks = dict(classification["targetFrameworks"])

        for tfm in ("net8.0", "net10.0"):
            tfm_payload = dict(target_frameworks[tfm])
            candidate_assemblies = dict(tfm_payload["candidateAssemblies"])

            for assembly_name in ("mscorlib", "netstandard", "System.Runtime", "System", "System.Core"):
                self.assertIn(assembly_name, candidate_assemblies, msg=f"{tfm} missing seed assembly {assembly_name}")
                classification_kind = dict(candidate_assemblies[assembly_name])["classification"]
                self.assertIn(classification_kind, {"contract-facade", "compat-shim"})

            self.assertNotIn("System.Private.CoreLib", candidate_assemblies)
            self.assertGreater(dict(tfm_payload)["candidateAssemblyCount"], 0)

    def test_phase1_generator_closes_surface_and_binding_proofs_without_unresolved_targets(self) -> None:
        artifacts = self._run_generator()

        surface_proof = dict(artifacts["surface_proof"])
        binding_proof = dict(artifacts["binding_proof"])
        benchmark = dict(artifacts["benchmark"])

        for tfm in ("net8.0", "net10.0"):
            tfm_surface = dict(dict(surface_proof["targetFrameworks"])[tfm])
            tfm_binding = dict(dict(binding_proof["targetFrameworks"])[tfm])
            tfm_benchmark = dict(dict(benchmark["targetFrameworks"])[tfm])

            self.assertEqual(0, tfm_surface["unresolvedForwardedTypeCount"])
            self.assertEqual(0, tfm_surface["issueCount"])
            self.assertEqual(0, tfm_binding["unresolvedBindingTargetCount"])
            self.assertGreater(tfm_binding["bindingsCheckedCount"], 0)
            self.assertGreater(tfm_benchmark["candidateAssemblyCount"], 0)
            self.assertGreaterEqual(tfm_benchmark["elapsedMs"], 0)

    def test_phase1_generator_marks_external_forward_targets_as_resolved_owner_assemblies(self) -> None:
        artifacts = self._run_generator()

        surface_manifest = dict(artifacts["surface_manifest"])

        for tfm in ("net8.0", "net10.0"):
            tfm_manifest = dict(dict(surface_manifest["targetFrameworks"])[tfm])
            system_assembly = dict(dict(tfm_manifest["assemblies"])["System"])
            forwarded_types = dict(system_assembly["forwardedTypes"])
            code_dom_type = dict(forwarded_types["System.CodeDom.CodeArgumentReferenceExpression"])

            self.assertEqual("System.CodeDom", code_dom_type["canonicalOwnerAssembly"])
            self.assertEqual("resolved-external-owner-assembly", code_dom_type["resolutionStatus"])


if __name__ == "__main__":
    unittest.main()
