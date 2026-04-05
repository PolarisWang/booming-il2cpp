from __future__ import annotations

import json
import shutil
import subprocess
import unittest
from pathlib import Path


REPO_ROOT = Path(__file__).resolve().parents[3]
DRIVER_PROJECT_PATH = REPO_ROOT / "src" / "managed" / "Chaos.IL2CPP.Driver" / "Chaos.IL2CPP.Driver.csproj"
PROOF_PROJECT_PATH = REPO_ROOT / "tests" / "proof" / "input" / "HelloWorldObject" / "HelloWorldObject.csproj"
PROOF_DLL_PATH = REPO_ROOT / "tests" / "proof" / "input" / "HelloWorldObject" / "bin" / "Release" / "net8.0" / "HelloWorldObject.dll"
OUTPUT_ROOT = REPO_ROOT / "artifacts" / "proof" / "managed-closure" / "HelloWorldObject"
CONTRACT_OVERVIEW_PATH = REPO_ROOT / "contracts" / "docs" / "v0" / "overview.md"
SPEC_DOC_PATH = REPO_ROOT / "docs" / "architecture" / "roadmap-0" / "managed-minimal-closure-v0.md"

EXPECTED_ARTIFACTS = {
    "typed-il-ir.json": REPO_ROOT / "contracts" / "examples" / "v0" / "artifacts" / "typed-il-ir.min.json",
    "aot-manifest.json": REPO_ROOT / "contracts" / "examples" / "v0" / "artifacts" / "aot-manifest.min.json",
    "metadata-registration.json": REPO_ROOT / "contracts" / "examples" / "v0" / "artifacts" / "metadata-registration.min.json",
    "code-registration.json": REPO_ROOT / "contracts" / "examples" / "v0" / "artifacts" / "code-registration.min.json",
}


def run_checked(arguments: list[str], *, cwd: Path) -> subprocess.CompletedProcess[str]:
    completed = subprocess.run(
        arguments,
        cwd=cwd,
        capture_output=True,
        text=True,
        encoding="utf-8",
        errors="replace",
        check=False,
    )
    if completed.returncode != 0:
        combined_output = "\n".join(part for part in [completed.stdout, completed.stderr] if part)
        raise AssertionError(f"command failed ({completed.returncode}): {' '.join(arguments)}\n{combined_output}")
    return completed


def load_json(path: Path) -> object:
    return json.loads(path.read_text(encoding="utf-8"))


class Stage3ManagedMinimalClosureTests(unittest.TestCase):
    maxDiff = None

    def test_stage3_spec_doc_exists_and_is_linked_from_contract_overview(self) -> None:
        self.assertTrue(SPEC_DOC_PATH.is_file(), msg=f"missing spec doc: {SPEC_DOC_PATH}")

        spec_text = SPEC_DOC_PATH.read_text(encoding="utf-8")
        overview_text = CONTRACT_OVERVIEW_PATH.read_text(encoding="utf-8")

        self.assertIn("HelloWorldObject.dll", spec_text)
        self.assertIn("artifacts/proof/managed-closure/HelloWorldObject/typed-il-ir.json", spec_text)
        self.assertIn("closure.manifest.json", spec_text)
        self.assertIn("docs/architecture/roadmap-0/managed-minimal-closure-v0.md", overview_text)

    def test_driver_generates_stage3_bundle_from_real_proof_dll(self) -> None:
        if OUTPUT_ROOT.exists():
            shutil.rmtree(OUTPUT_ROOT)

        run_checked(["dotnet", "build", str(PROOF_PROJECT_PATH), "-c", "Release"], cwd=REPO_ROOT)
        self.assertTrue(PROOF_DLL_PATH.is_file(), msg=f"missing proof dll: {PROOF_DLL_PATH}")

        run_checked(
            [
                "dotnet",
                "run",
                "--project",
                str(DRIVER_PROJECT_PATH),
                "-c",
                "Release",
                "--",
                str(PROOF_DLL_PATH),
                str(OUTPUT_ROOT),
            ],
            cwd=REPO_ROOT,
        )

        for artifact_name in [*EXPECTED_ARTIFACTS.keys(), "closure.manifest.json"]:
            artifact_path = OUTPUT_ROOT / artifact_name
            self.assertTrue(artifact_path.is_file(), msg=f"missing closure artifact: {artifact_path}")

        manifest = load_json(OUTPUT_ROOT / "closure.manifest.json")
        self.assertEqual("v0", manifest["formatVersion"])
        self.assertEqual("managedClosureManifest", manifest["artifactKind"])
        self.assertEqual("HelloWorldObject", manifest["assemblyName"])
        self.assertEqual("HelloWorldObject/Program::Main(System.String[])", manifest["entrySubjectId"])
        self.assertEqual(str(PROOF_DLL_PATH.relative_to(REPO_ROOT)).replace("\\", "/"), manifest["inputAssemblyPath"])
        self.assertEqual(
            [
                "typed-il-ir.json",
                "aot-manifest.json",
                "metadata-registration.json",
                "code-registration.json",
            ],
            [artifact["path"] for artifact in manifest["artifacts"]],
        )
        self.assertTrue(manifest["inputModuleVersionId"])

    def test_generated_core_artifacts_match_stage2_canonical_examples(self) -> None:
        self.test_driver_generates_stage3_bundle_from_real_proof_dll()

        for generated_name, expected_path in EXPECTED_ARTIFACTS.items():
            generated_path = OUTPUT_ROOT / generated_name
            self.assertEqual(load_json(expected_path), load_json(generated_path), msg=f"artifact mismatch: {generated_name}")


if __name__ == "__main__":
    unittest.main()
