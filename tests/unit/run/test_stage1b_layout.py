from __future__ import annotations

import unittest
import xml.etree.ElementTree as ET
from pathlib import Path


REPO_ROOT = Path(__file__).resolve().parents[3]

EXPECTED_MANAGED_PROJECTS: dict[str, list[str]] = {
    "Chaos.IL2CPP.Contracts": [],
    "Chaos.IL2CPP.Driver": [
        "Chaos.IL2CPP.Contracts",
        "Chaos.IL2CPP.Pipeline",
    ],
    "Chaos.IL2CPP.Loader": [
        "Chaos.IL2CPP.Contracts",
    ],
    "Chaos.IL2CPP.SemanticWorld": [
        "Chaos.IL2CPP.Contracts",
        "Chaos.IL2CPP.Loader",
    ],
    "Chaos.IL2CPP.Pipeline": [
        "Chaos.IL2CPP.Contracts",
        "Chaos.IL2CPP.Loader",
        "Chaos.IL2CPP.SemanticWorld",
        "Chaos.IL2CPP.Linker",
        "Chaos.IL2CPP.MetadataWriter",
        "Chaos.IL2CPP.CodeGen",
    ],
    "Chaos.IL2CPP.Linker": [
        "Chaos.IL2CPP.Contracts",
        "Chaos.IL2CPP.SemanticWorld",
    ],
    "Chaos.IL2CPP.MetadataWriter": [
        "Chaos.IL2CPP.Contracts",
        "Chaos.IL2CPP.Linker",
    ],
    "Chaos.IL2CPP.CodeGen": [
        "Chaos.IL2CPP.Contracts",
        "Chaos.IL2CPP.MetadataWriter",
    ],
}


def parse_project_references(project_path: Path) -> list[str]:
    root = ET.fromstring(project_path.read_text(encoding="utf-8"))
    references: list[str] = []
    for element in root.findall(".//ProjectReference"):
        include_value = element.attrib.get("Include", "")
        references.append(Path(include_value).stem)
    return sorted(references)


class Stage1BRepoLayoutTests(unittest.TestCase):
    def test_managed_projects_exist_with_expected_project_references(self) -> None:
        managed_root = REPO_ROOT / "src" / "managed"
        self.assertTrue(managed_root.is_dir(), msg=f"missing managed root: {managed_root}")

        for project_name, expected_references in EXPECTED_MANAGED_PROJECTS.items():
            project_dir = managed_root / project_name
            project_path = project_dir / f"{project_name}.csproj"

            self.assertTrue(project_dir.is_dir(), msg=f"missing managed project dir: {project_dir}")
            self.assertTrue(project_path.is_file(), msg=f"missing managed project file: {project_path}")
            self.assertEqual(sorted(expected_references), parse_project_references(project_path))

    def test_proof_and_smoke_layout_are_split(self) -> None:
        self.assertTrue(
            (REPO_ROOT / "tests" / "smoke" / "input" / "HelloWorld" / "HelloWorld.csproj").is_file()
        )
        self.assertTrue(
            (REPO_ROOT / "tests" / "proof" / "input" / "HelloWorldObject" / "HelloWorldObject.csproj").is_file()
        )
        self.assertTrue((REPO_ROOT / "tests" / "proof" / "hello-world-object").is_dir())

    def test_root_cmake_and_verify_scripts_use_canonical_contract_paths(self) -> None:
        cmake_router = (REPO_ROOT / "CMakeLists.txt").read_text(encoding="utf-8")
        verify_py = (REPO_ROOT / "build" / "scripts" / "verify-roadmap-0.py").read_text(encoding="utf-8")
        verify_ps1 = (REPO_ROOT / "build" / "scripts" / "verify-roadmap-0.ps1").read_text(encoding="utf-8")

        self.assertIn("tests/contracts/native/abi", cmake_router)
        self.assertIn("tests/contracts/native/bridge", cmake_router)
        self.assertNotIn("tests/contract/native/abi", cmake_router)
        self.assertNotIn("tests/contract/native/bridge", cmake_router)

        self.assertIn('"tests" / "contracts" / "trace" / "compare-warmup-trace.py"', verify_py)
        self.assertIn('"tests" / "contracts" / "trace" / "snapshots" / "windows-warmup-trace.snapshot.json"', verify_py)
        self.assertIn("tests\\contracts\\trace", verify_ps1)
        self.assertNotIn('"tests" / "contract" / "trace"', verify_py)
        self.assertNotIn("tests\\contract\\trace", verify_ps1)


if __name__ == "__main__":
    unittest.main()
