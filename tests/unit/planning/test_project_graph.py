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
FIXTURE_SUBJECTS_ROOT = REPO_ROOT / "tests" / "fixtures" / "subjects"
SOLUTION_SIMPLE_LIB_FIXTURE_ROOT = FIXTURE_SUBJECTS_ROOT / "solution-simple-lib"
SOLUTION_MULTI_PROJECT_FIXTURE_ROOT = FIXTURE_SUBJECTS_ROOT / "solution-multi-project"
SOLUTION_PACKAGE_REFERENCE_FIXTURE_ROOT = FIXTURE_SUBJECTS_ROOT / "solution-package-reference"


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

    def _run_convert(self, subject_root: Path, run_label: str) -> Path:
        output_root = self._make_output_root(run_label)
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

    def test_archetype_convert_fixtures_target_solution_core_pack_sources(self) -> None:
        self.assertTrue((SOLUTION_SIMPLE_LIB_FIXTURE_ROOT / "subject.manifest.json").is_file())
        self.assertTrue((SOLUTION_MULTI_PROJECT_FIXTURE_ROOT / "subject.manifest.json").is_file())
        self.assertTrue((SOLUTION_PACKAGE_REFERENCE_FIXTURE_ROOT / "subject.manifest.json").is_file())
        self.assertFalse((REPO_ROOT / "subjects" / "SolutionSimpleLib").exists())
        self.assertFalse((REPO_ROOT / "subjects" / "SolutionMultiProject").exists())
        self.assertFalse((REPO_ROOT / "subjects" / "SolutionPackageReference").exists())

    def test_convert_solution_simple_lib_loads_entry_and_project_reference_assemblies(self) -> None:
        output_root = self._run_convert(SOLUTION_SIMPLE_LIB_FIXTURE_ROOT, "solution-simple-lib")

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
        output_root = self._run_convert(SOLUTION_MULTI_PROJECT_FIXTURE_ROOT, "solution-multi-project")

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
        output_root = self._run_convert(SOLUTION_PACKAGE_REFERENCE_FIXTURE_ROOT, "solution-package-reference")

        graph = load_json(output_root / "project-graph.json")
        self.assertEqual("GoldenWithPackage.App", graph["entryProject"]["assemblyName"])
        self.assertEqual(["GoldenWithPackage.App"], [project["assemblyName"] for project in graph["projects"]])
        self.assertEqual(["package-reference"], [dependency["dependencyKind"] for dependency in graph["dependencies"]])
        self.assertEqual("Newtonsoft.Json", graph["dependencies"][0]["packageId"])
        self.assertTrue(graph["dependencies"][0]["assemblyPath"].endswith("Newtonsoft.Json.dll"))

    def test_convert_solution_multi_project_preserves_golden_assembly_names(self) -> None:
        output_root = self._run_convert(SOLUTION_MULTI_PROJECT_FIXTURE_ROOT, "solution-multi-project-canonical")

        graph = load_json(output_root / "project-graph.json")
        self.assertEqual("GoldenMultiProject.App", graph["entryProject"]["assemblyName"])
        self.assertEqual(
            ["GoldenMultiProject.App", "GoldenMultiProject.Middle", "GoldenMultiProject.Core"],
            [project["assemblyName"] for project in graph["projects"]],
        )


if __name__ == "__main__":
    unittest.main()
