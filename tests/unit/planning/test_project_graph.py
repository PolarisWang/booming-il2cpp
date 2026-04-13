from __future__ import annotations

import json
import shutil
import subprocess
import unittest
import uuid
from pathlib import Path


REPO_ROOT = Path(__file__).resolve().parents[3]
DRIVER_PROJECT_PATH = REPO_ROOT / "src" / "managed" / "Chaos.IL2CPP.Driver" / "Chaos.IL2CPP.Driver.csproj"
TEST_TMP_ROOT = REPO_ROOT / "artifacts" / ".tmp-tests" / "project-graph"
SUBJECTS_ROOT = REPO_ROOT / "subjects"


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


class ProjectGraphTests(unittest.TestCase):
    maxDiff = None

    @classmethod
    def setUpClass(cls) -> None:
        TEST_TMP_ROOT.mkdir(parents=True, exist_ok=True)

    def _make_output_root(self, subject_id: str) -> Path:
        output_root = TEST_TMP_ROOT / f"{subject_id}-{uuid.uuid4().hex}"
        if output_root.exists():
            shutil.rmtree(output_root)
        return output_root

    def _run_convert(self, subject_root: Path, subject_id: str) -> Path:
        output_root = self._make_output_root(subject_id)
        run_checked(
            [
                "dotnet",
                "run",
                "--project",
                str(DRIVER_PROJECT_PATH),
                "--",
                "convert",
                str(subject_root),
                "--output",
                str(output_root),
            ],
            cwd=REPO_ROOT,
        )
        return output_root

    def test_golden_subjects_exist(self) -> None:
        expected_subjects = {"SolutionSimpleLib", "SolutionMultiProject", "SolutionPackageReference"}
        actual_subjects = {path.name for path in SUBJECTS_ROOT.iterdir() if path.is_dir()}
        self.assertTrue(expected_subjects.issubset(actual_subjects))

    def test_convert_solution_simple_lib_loads_entry_and_project_reference_assemblies(self) -> None:
        output_root = self._run_convert(SUBJECTS_ROOT / "SolutionSimpleLib", "SolutionSimpleLib")

        graph = load_json(output_root / "project-graph.json")
        self.assertEqual("GoldenSimpleLib.App", graph["entryProject"]["assemblyName"])
        self.assertEqual(
            ["GoldenSimpleLib.App", "GoldenSimpleLib.Library"],
            [project["assemblyName"] for project in graph["projects"]],
        )

        metadata_registration = load_json(output_root / "analysis" / "metadata-registration.json")
        registered_subject_ids = {
            registration["subjectId"]
            for registration in metadata_registration["registrations"]
        }
        self.assertIn("GoldenSimpleLib.App/Program::Main()", registered_subject_ids)
        self.assertIn("GoldenSimpleLib.Library/Greeter::BuildMessage()", registered_subject_ids)

        code_registration = load_json(output_root / "analysis" / "code-registration.json")
        self.assertEqual(
            ["GoldenSimpleLib.App.dll", "GoldenSimpleLib.Library.dll"],
            [module["moduleName"] for module in code_registration["modules"]],
        )

    def test_convert_solution_multi_project_tracks_transitive_cross_assembly_references(self) -> None:
        output_root = self._run_convert(SUBJECTS_ROOT / "SolutionMultiProject", "SolutionMultiProject")

        graph = load_json(output_root / "project-graph.json")
        self.assertEqual("GoldenMultiProject.App", graph["entryProject"]["assemblyName"])
        self.assertEqual(
            ["GoldenMultiProject.App", "GoldenMultiProject.Middle", "GoldenMultiProject.Core"],
            [project["assemblyName"] for project in graph["projects"]],
        )
        self.assertEqual(
            [
                "project-reference",
                "project-reference",
            ],
            [dependency["dependencyKind"] for dependency in graph["dependencies"]],
        )

        metadata_registration = load_json(output_root / "analysis" / "metadata-registration.json")
        registered_subject_ids = {
            registration["subjectId"]
            for registration in metadata_registration["registrations"]
        }
        self.assertIn("GoldenMultiProject.Middle/MessageComposer::Compose()", registered_subject_ids)
        self.assertIn("GoldenMultiProject.Core/SuffixFactory::Create()", registered_subject_ids)

        native_generated_root = output_root / "generated" / "assemblies"
        self.assertTrue((native_generated_root / "GoldenMultiProject.App").is_dir())
        self.assertTrue((native_generated_root / "GoldenMultiProject.Middle").is_dir())
        self.assertTrue((native_generated_root / "GoldenMultiProject.Core").is_dir())

    def test_convert_solution_package_reference_exposes_package_reference_in_project_graph(self) -> None:
        output_root = self._run_convert(SUBJECTS_ROOT / "SolutionPackageReference", "SolutionPackageReference")

        graph = load_json(output_root / "project-graph.json")
        self.assertEqual("GoldenWithPackage.App", graph["entryProject"]["assemblyName"])
        self.assertEqual(["GoldenWithPackage.App"], [project["assemblyName"] for project in graph["projects"]])
        self.assertEqual(["package-reference"], [dependency["dependencyKind"] for dependency in graph["dependencies"]])
        self.assertEqual("Newtonsoft.Json", graph["dependencies"][0]["packageId"])
        self.assertTrue(graph["dependencies"][0]["assemblyPath"].endswith("Newtonsoft.Json.dll"))

    def test_convert_solution_multi_project_preserves_golden_assembly_names(self) -> None:
        output_root = self._run_convert(SUBJECTS_ROOT / "SolutionMultiProject", "SolutionMultiProject-canonical")

        graph = load_json(output_root / "project-graph.json")
        self.assertEqual("GoldenMultiProject.App", graph["entryProject"]["assemblyName"])
        self.assertEqual(
            ["GoldenMultiProject.App", "GoldenMultiProject.Middle", "GoldenMultiProject.Core"],
            [project["assemblyName"] for project in graph["projects"]],
        )


if __name__ == "__main__":
    unittest.main()
