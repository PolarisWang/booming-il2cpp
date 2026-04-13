from __future__ import annotations

import json
import unittest
from pathlib import Path


REPO_ROOT = Path(__file__).resolve().parents[3]
SUBJECT_ROOT = REPO_ROOT / "subjects" / "SolutionCorePack"
MANIFEST_PATH = SUBJECT_ROOT / "subject.manifest.json"
SOLUTION_PATH = SUBJECT_ROOT / "source" / "SolutionCorePack.sln"
PRIMARY_PROJECT_PATH = SUBJECT_ROOT / "source" / "Launcher" / "SolutionCorePack.csproj"
ARCHETYPE_ROOT = SUBJECT_ROOT / "source" / "Archetypes"
PROOF_HOST_PATH = SUBJECT_ROOT / "validation" / "proof" / "native-reference" / "main.cpp"
PROOF_CMAKE_PATH = SUBJECT_ROOT / "validation" / "proof" / "native-reference" / "CMakeLists.txt"
PROOF_RUN_SCRIPT_PATH = SUBJECT_ROOT / "validation" / "proof" / "native-reference" / "RunNativeReferenceProof.cmake"


class SolutionCorePackSubjectTests(unittest.TestCase):
    def test_solution_core_pack_manifest_declares_solution_mode_subject(self) -> None:
        self.assertTrue(MANIFEST_PATH.is_file(), msg=f"missing manifest: {MANIFEST_PATH}")

        manifest = json.loads(MANIFEST_PATH.read_text(encoding="utf-8"))

        self.assertEqual("SolutionCorePack", manifest["subjectId"])
        self.assertEqual("canonical", manifest["category"])
        self.assertEqual("dotnet-solution", manifest["sourceModel"])
        self.assertEqual("subjects/SolutionCorePack/source/SolutionCorePack.sln", manifest["source"]["path"])
        self.assertEqual(
            "subjects/SolutionCorePack/source/Launcher/SolutionCorePack.csproj",
            manifest["source"]["primaryProjectPath"],
        )
        self.assertEqual("MainlineFeaturePack/ProofEntry::Run()", manifest["source"]["entry"])
        self.assertEqual(
            "PerformanceFeaturePack/ArithmeticBenchmarkEntry::RunWorkload()",
            manifest["workloadEntry"],
        )

    def test_solution_core_pack_solution_includes_internal_mainline_performance_and_archetype_projects(self) -> None:
        self.assertTrue(SOLUTION_PATH.is_file(), msg=f"missing solution: {SOLUTION_PATH}")
        self.assertTrue(PRIMARY_PROJECT_PATH.is_file(), msg=f"missing primary project: {PRIMARY_PROJECT_PATH}")

        solution_text = SOLUTION_PATH.read_text(encoding="utf-8")
        project_text = PRIMARY_PROJECT_PATH.read_text(encoding="utf-8")

        self.assertIn(r"Launcher\SolutionCorePack.csproj", solution_text)
        self.assertIn(r"Slices\MainlineFeaturePack\MainlineFeaturePack.csproj", solution_text)
        self.assertIn(r"Slices\PerformanceFeaturePack\PerformanceFeaturePack.csproj", solution_text)
        self.assertIn(r"Archetypes\SolutionSimpleLib\App\GoldenSimpleLib.App.csproj", solution_text)
        self.assertIn(r"Archetypes\SolutionMultiProject\App\GoldenMultiProject.App.csproj", solution_text)
        self.assertIn(r"Archetypes\SolutionPackageReference\App\GoldenWithPackage.App.csproj", solution_text)
        self.assertIn(r"..\Slices\MainlineFeaturePack\MainlineFeaturePack.csproj", project_text)
        self.assertIn(r"..\Slices\PerformanceFeaturePack\PerformanceFeaturePack.csproj", project_text)
        self.assertNotIn(r"..\..\..\MainlineFeaturePack\source\MainlineFeaturePack.csproj", project_text)
        self.assertNotIn(r"..\..\..\PerformanceFeaturePack\source\PerformanceFeaturePack.csproj", project_text)

    def test_solution_core_pack_owns_internal_slices_and_legacy_subject_roots_are_removed(self) -> None:
        self.assertTrue((ARCHETYPE_ROOT / "SolutionSimpleLib" / "SolutionSimpleLib.sln").is_file())
        self.assertTrue((ARCHETYPE_ROOT / "SolutionMultiProject" / "SolutionMultiProject.sln").is_file())
        self.assertTrue((ARCHETYPE_ROOT / "SolutionPackageReference" / "SolutionPackageReference.sln").is_file())
        self.assertTrue((SUBJECT_ROOT / "source" / "Slices" / "MainlineFeaturePack" / "MainlineFeaturePack.csproj").is_file())
        self.assertTrue((SUBJECT_ROOT / "source" / "Slices" / "PerformanceFeaturePack" / "PerformanceFeaturePack.csproj").is_file())
        self.assertFalse((REPO_ROOT / "subjects" / "SolutionSimpleLib").exists())
        self.assertFalse((REPO_ROOT / "subjects" / "SolutionMultiProject").exists())
        self.assertFalse((REPO_ROOT / "subjects" / "SolutionPackageReference").exists())
        self.assertFalse((REPO_ROOT / "subjects" / "MainlineFeaturePack").exists())
        self.assertFalse((REPO_ROOT / "subjects" / "PerformanceFeaturePack").exists())

    def test_solution_core_pack_declares_native_reference_proof_host(self) -> None:
        self.assertTrue(PROOF_HOST_PATH.is_file(), msg=f"missing proof host source: {PROOF_HOST_PATH}")
        self.assertTrue(PROOF_CMAKE_PATH.is_file(), msg=f"missing proof cmake: {PROOF_CMAKE_PATH}")
        self.assertTrue(PROOF_RUN_SCRIPT_PATH.is_file(), msg=f"missing proof run script: {PROOF_RUN_SCRIPT_PATH}")

        proof_host_text = PROOF_HOST_PATH.read_text(encoding="utf-8")
        proof_cmake_text = PROOF_CMAKE_PATH.read_text(encoding="utf-8")

        self.assertIn('options.image_name_utf8 = "SolutionCorePack";', proof_host_text)
        self.assertIn("RunNativeReference", proof_host_text)
        self.assertIn("chaos_subject_reference_proof", proof_cmake_text)
        self.assertIn("CHAOS_SUBJECT_GENERATED_SOURCE", proof_cmake_text)


if __name__ == "__main__":
    unittest.main()
