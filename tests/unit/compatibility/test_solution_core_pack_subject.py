from __future__ import annotations

import json
import unittest
from pathlib import Path


REPO_ROOT = Path(__file__).resolve().parents[3]
SUBJECT_ROOT = REPO_ROOT / "subjects" / "SolutionCorePack"
MANIFEST_PATH = SUBJECT_ROOT / "subject.manifest.json"
SOLUTION_PATH = SUBJECT_ROOT / "source" / "SolutionCorePack.sln"
PRIMARY_PROJECT_PATH = SUBJECT_ROOT / "source" / "Host" / "SolutionCorePack.csproj"
ARCHETYPE_ROOT = SUBJECT_ROOT / "source" / "EngineeringScenarios"
REFERENCE_BUNDLE_ROOT = REPO_ROOT / "assets" / "reference-bundles" / "dotnet-foundation"
LAUNCHER_PROGRAM_PATH = SUBJECT_ROOT / "source" / "Host" / "Program.cs"


class SolutionCorePackSubjectTests(unittest.TestCase):
    def test_solution_core_pack_manifest_declares_solution_mode_subject(self) -> None:
        self.assertTrue(MANIFEST_PATH.is_file(), msg=f"missing manifest: {MANIFEST_PATH}")

        manifest = json.loads(MANIFEST_PATH.read_text(encoding="utf-8"))

        self.assertEqual("SolutionCorePack", manifest["subjectId"])
        self.assertEqual("canonical", manifest["category"])
        self.assertEqual("dotnet-solution", manifest["sourceModel"])
        self.assertEqual("subjects/SolutionCorePack/source/SolutionCorePack.sln", manifest["source"]["path"])
        self.assertEqual(
            "subjects/SolutionCorePack/source/Host/SolutionCorePack.csproj",
            manifest["source"]["primaryProjectPath"],
        )
        self.assertEqual("CoreRuntimeFeatures/ProofEntry::Run()", manifest["source"]["entry"])
        self.assertEqual({"entryKind": 1, "entrySlice": 7}, manifest["source"]["entrySelection"])
        self.assertEqual(
            "CoreRuntimeBenchmarks/ArithmeticBenchmarkEntry::RunWorkload()",
            manifest["workloadEntry"],
        )

    def test_solution_core_pack_solution_includes_internal_mainline_performance_and_archetype_projects(self) -> None:
        self.assertTrue(SOLUTION_PATH.is_file(), msg=f"missing solution: {SOLUTION_PATH}")
        self.assertTrue(PRIMARY_PROJECT_PATH.is_file(), msg=f"missing primary project: {PRIMARY_PROJECT_PATH}")

        solution_text = SOLUTION_PATH.read_text(encoding="utf-8")
        project_text = PRIMARY_PROJECT_PATH.read_text(encoding="utf-8")

        self.assertIn(r"Host\SolutionCorePack.csproj", solution_text)
        self.assertIn(r"Proofs\CoreRuntimeFeatures\CoreRuntimeFeatures.csproj", solution_text)
        self.assertIn(r"Benchmarks\CoreRuntimeBenchmarks\CoreRuntimeBenchmarks.csproj", solution_text)
        self.assertIn(r"EngineeringScenarios\SimpleLibrarySolution\App\GoldenSimpleLib.App.csproj", solution_text)
        self.assertIn(r"EngineeringScenarios\MultiProjectSolution\App\GoldenMultiProject.App.csproj", solution_text)
        self.assertIn(r"EngineeringScenarios\PackageReferenceSolution\App\GoldenWithPackage.App.csproj", solution_text)
        self.assertIn(r"EngineeringScenarios\ReferenceAssemblySolution\App\GoldenReferenceAssembly.App.csproj", solution_text)
        self.assertIn(r"EngineeringScenarios\CoreLibReferenceSolution\App\GoldenCoreLibReference.App.csproj", solution_text)
        self.assertIn(r"EngineeringScenarios\MixedReferenceClosureSolution\App\GoldenMixedReference.App.csproj", solution_text)
        self.assertNotIn(r"Slices\HelloWorld\HelloWorld.csproj", solution_text)
        self.assertNotIn(r"Slices\GenericEcho\GenericEcho.csproj", solution_text)
        self.assertNotIn(r"Slices\HelloWorldObject\HelloWorldObject.csproj", solution_text)
        self.assertNotIn(r"Slices\ReflectionLite\ReflectionLite.csproj", solution_text)
        self.assertNotIn(r"Slices\PInvokeLite\PInvokeLite.csproj", solution_text)
        self.assertNotIn(r"Slices\HostEmbeddingLite\HostEmbeddingLite.csproj", solution_text)
        self.assertIn(r"..\Proofs\CoreRuntimeFeatures\CoreRuntimeFeatures.csproj", project_text)
        self.assertIn(r"..\Benchmarks\CoreRuntimeBenchmarks\CoreRuntimeBenchmarks.csproj", project_text)
        self.assertIn(r"..\EngineeringScenarios\ReferenceAssemblySolution\App\GoldenReferenceAssembly.App.csproj", project_text)
        self.assertIn(r"..\EngineeringScenarios\CoreLibReferenceSolution\App\GoldenCoreLibReference.App.csproj", project_text)
        self.assertIn(r"..\EngineeringScenarios\MixedReferenceClosureSolution\App\GoldenMixedReference.App.csproj", project_text)
        self.assertNotIn(r"..\..\..\MainlineFeaturePack\source\MainlineFeaturePack.csproj", project_text)
        self.assertNotIn(r"..\..\..\PerformanceFeaturePack\source\PerformanceFeaturePack.csproj", project_text)

    def test_solution_core_pack_owns_internal_slices_and_legacy_subject_roots_are_removed(self) -> None:
        self.assertTrue((ARCHETYPE_ROOT / "SimpleLibrarySolution" / "SimpleLibrarySolution.sln").is_file())
        self.assertTrue((ARCHETYPE_ROOT / "MultiProjectSolution" / "MultiProjectSolution.sln").is_file())
        self.assertTrue((ARCHETYPE_ROOT / "PackageReferenceSolution" / "PackageReferenceSolution.sln").is_file())
        self.assertTrue((ARCHETYPE_ROOT / "ReferenceAssemblySolution" / "ReferenceAssemblySolution.sln").is_file())
        self.assertTrue((ARCHETYPE_ROOT / "CoreLibReferenceSolution" / "CoreLibReferenceSolution.sln").is_file())
        self.assertTrue((ARCHETYPE_ROOT / "MixedReferenceClosureSolution" / "MixedReferenceClosureSolution.sln").is_file())
        self.assertTrue((SUBJECT_ROOT / "source" / "Proofs" / "CoreRuntimeFeatures" / "CoreRuntimeFeatures.csproj").is_file())
        self.assertTrue((SUBJECT_ROOT / "source" / "Benchmarks" / "CoreRuntimeBenchmarks" / "CoreRuntimeBenchmarks.csproj").is_file())
        self.assertFalse((SUBJECT_ROOT / "source" / "Slices").exists())
        self.assertFalse((SUBJECT_ROOT / "source" / "Slices" / "HelloWorld").exists())
        self.assertFalse((SUBJECT_ROOT / "source" / "Slices" / "GenericEcho").exists())
        self.assertFalse((SUBJECT_ROOT / "source" / "Slices" / "HelloWorldObject").exists())
        self.assertFalse((SUBJECT_ROOT / "source" / "Slices" / "ReflectionLite").exists())
        self.assertFalse((SUBJECT_ROOT / "source" / "Slices" / "PInvokeLite").exists())
        self.assertFalse((SUBJECT_ROOT / "source" / "Slices" / "HostEmbeddingLite").exists())
        self.assertFalse((REPO_ROOT / "subjects" / "SolutionSimpleLib").exists())
        self.assertFalse((REPO_ROOT / "subjects" / "SolutionMultiProject").exists())
        self.assertFalse((REPO_ROOT / "subjects" / "SolutionPackageReference").exists())
        self.assertFalse((REPO_ROOT / "subjects" / "MainlineFeaturePack").exists())
        self.assertFalse((REPO_ROOT / "subjects" / "PerformanceFeaturePack").exists())

    def test_solution_core_pack_owns_repository_reference_bundle_for_binary_reference_archetypes(self) -> None:
        self.assertTrue(REFERENCE_BUNDLE_ROOT.is_dir(), msg=f"missing reference bundle root: {REFERENCE_BUNDLE_ROOT}")
        self.assertTrue((REFERENCE_BUNDLE_ROOT / "lib" / "ReferenceGreeter.dll").is_file())
        self.assertTrue((REFERENCE_BUNDLE_ROOT / "net8.0" / "System.Runtime.dll").is_file())
        self.assertTrue((REFERENCE_BUNDLE_ROOT / "net8.0" / "System.Console.dll").is_file())
        self.assertTrue((REFERENCE_BUNDLE_ROOT / "README.md").is_file())

        corelib_project_path = (
            SUBJECT_ROOT / "source" / "EngineeringScenarios" / "CoreLibReferenceSolution" / "App" / "GoldenCoreLibReference.App.csproj"
        )
        corelib_project_text = corelib_project_path.read_text(encoding="utf-8")
        self.assertIn("<DisableImplicitFrameworkReferences>true</DisableImplicitFrameworkReferences>", corelib_project_text)
        self.assertIn('<FrameworkReference Include="Microsoft.NETCore.App" />', corelib_project_text)

    def test_solution_core_pack_deletes_legacy_subject_owned_native_reference_host(self) -> None:
        self.assertFalse((SUBJECT_ROOT / "validation").exists())

    def test_solution_core_launcher_uses_compact_subject_entry_selection_only(self) -> None:
        launcher_source = LAUNCHER_PROGRAM_PATH.read_text(encoding="utf-8")

        for required_fragment in [
            "ChaosSubjectEntryArguments.TryParse",
            "ChaosSubjectEntryKind.Proof",
            "ChaosSubjectSlice.CoreRuntimeFeaturesProof",
            "ChaosSubjectSlice.CoreRuntimeInterfaceDispatchProof",
        ]:
            self.assertIn(required_fragment, launcher_source)
        self.assertNotIn("ChaosSourceEntryArguments.TryParse", launcher_source)
        self.assertNotIn("InvokeSourceEntry(", launcher_source)


if __name__ == "__main__":
    unittest.main()


