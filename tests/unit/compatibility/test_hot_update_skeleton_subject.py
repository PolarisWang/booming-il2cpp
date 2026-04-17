from __future__ import annotations

import importlib.util
import json
import sys
import unittest
import xml.etree.ElementTree as ET
from pathlib import Path


REPO_ROOT = Path(__file__).resolve().parents[3]
CORE_SOLUTION_PATH = REPO_ROOT / "solutions" / "core" / "windows" / "chaos-il2cpp-core.sln"
ROOT_CMAKE_PATH = REPO_ROOT / "CMakeLists.txt"

HOT_UPDATE_ROOT = REPO_ROOT / "src" / "managed" / "Chaos.IL2CPP.HotUpdate"
HOT_UPDATE_PROJECT_PATH = HOT_UPDATE_ROOT / "Chaos.IL2CPP.HotUpdate.csproj"
HOT_UPDATE_PACKAGE_PATH = HOT_UPDATE_ROOT / "HotUpdatePackage.cs"
PACKAGE_READER_PATH = HOT_UPDATE_ROOT / "PackageReader.cs"
PACKAGE_VALIDATOR_PATH = HOT_UPDATE_ROOT / "PackageValidator.cs"
RUNTIME_MANAGER_PATH = HOT_UPDATE_ROOT / "RuntimeManager.cs"
INTERPRETER_STUB_PATH = HOT_UPDATE_ROOT / "ConstantInt32InterpreterStub.cs"

NATIVE_HOT_UPDATE_ROOT = REPO_ROOT / "src" / "native" / "hot-update"
NATIVE_HOT_UPDATE_CMAKE_PATH = NATIVE_HOT_UPDATE_ROOT / "CMakeLists.txt"
NATIVE_HOT_UPDATE_HEADER_PATH = NATIVE_HOT_UPDATE_ROOT / "hot_update.h"
NATIVE_HOT_UPDATE_SOURCE_PATH = NATIVE_HOT_UPDATE_ROOT / "hot_update.cpp"
RUNTIME_CORE_HEADER_PATH = REPO_ROOT / "src" / "native" / "runtime-core" / "runtime_core.h"
RUNTIME_CORE_SOURCE_PATH = REPO_ROOT / "src" / "native" / "runtime-core" / "runtime_core.cpp"

CANONICAL_SUBJECT_ROOT = REPO_ROOT / "subjects" / "HotUpdateHostPack"
CANONICAL_SUBJECT_MANIFEST_PATH = CANONICAL_SUBJECT_ROOT / "subject.manifest.json"
CANONICAL_SUBJECT_PROJECT_PATH = CANONICAL_SUBJECT_ROOT / "source" / "HotUpdateHostPack.csproj"
CANONICAL_SUBJECT_PROGRAM_PATH = CANONICAL_SUBJECT_ROOT / "source" / "Host" / "Program.cs"
CANONICAL_SKELETON_ENTRY_PATH = CANONICAL_SUBJECT_ROOT / "source" / "Host" / "Proofs" / "HotUpdateSkeletonProofEntry.cs"
CANONICAL_PATCH_INTEGRITY_ENTRY_PATH = CANONICAL_SUBJECT_ROOT / "source" / "Host" / "Proofs" / "PatchIntegrityProofEntry.cs"
CANONICAL_PATCH_CALLBACK_ENTRY_PATH = CANONICAL_SUBJECT_ROOT / "source" / "Host" / "Proofs" / "PatchCallbackFlowProofEntry.cs"
CANONICAL_METADATA_SUPPLEMENT_ENTRY_PATH = (
    CANONICAL_SUBJECT_ROOT / "source" / "Host" / "Proofs" / "MetadataSupplementProofEntry.cs"
)
CANONICAL_ARCHETYPE_ROOT = CANONICAL_SUBJECT_ROOT / "source" / "EngineeringScenarios" / "FullProjectHotUpdateSolution"
CANONICAL_ARCHETYPE_SOLUTION_PATH = CANONICAL_ARCHETYPE_ROOT / "FullProjectHotUpdateSolution.sln"
CANONICAL_ARCHETYPE_HOST_PROJECT_PATH = CANONICAL_ARCHETYPE_ROOT / "HostApp" / "GoldenHotUpdateHost.App.csproj"
CANONICAL_ARCHETYPE_SHARED_PROJECT_PATH = CANONICAL_ARCHETYPE_ROOT / "SharedContracts" / "GoldenHotUpdate.SharedContracts.csproj"
CANONICAL_ARCHETYPE_PATCH_PROJECT_PATH = CANONICAL_ARCHETYPE_ROOT / "Patch" / "GoldenHotUpdate.PatchModule.csproj"
SUBJECTS_MODULE_PATH = REPO_ROOT / "build" / "toolchains" / "run" / "testing" / "subjects.py"


def parse_project_references(project_path: Path) -> list[str]:
    root = ET.fromstring(project_path.read_text(encoding="utf-8"))
    references: list[str] = []
    for element in root.findall(".//ProjectReference"):
        include_value = element.attrib.get("Include", "")
        references.append(Path(include_value).stem)
    return sorted(references)


def load_module(path: Path, module_name: str):
    if not path.is_file():
        raise FileNotFoundError(f"module missing: {path}")

    spec = importlib.util.spec_from_file_location(module_name, path)
    if spec is None or spec.loader is None:
        raise RuntimeError(f"unable to load module: {path}")

    module = importlib.util.module_from_spec(spec)
    sys.modules[spec.name] = module
    spec.loader.exec_module(module)
    return module


class Phase5HotUpdateSkeletonTests(unittest.TestCase):
    def test_hot_update_project_isolation_and_legacy_solution_cutover(self) -> None:
        self.assertTrue(HOT_UPDATE_ROOT.is_dir(), msg=f"missing hot update root: {HOT_UPDATE_ROOT}")
        self.assertTrue(HOT_UPDATE_PROJECT_PATH.is_file(), msg=f"missing hot update project: {HOT_UPDATE_PROJECT_PATH}")
        self.assertEqual(["Chaos.IL2CPP.Contracts"], parse_project_references(HOT_UPDATE_PROJECT_PATH))
        self.assertFalse(
            CORE_SOLUTION_PATH.exists(),
            msg=f"legacy static core solution should not exist anymore: {CORE_SOLUTION_PATH}",
        )

    def test_hot_update_sources_define_package_reader_validator_runtime_manager_and_interpreter_stub(self) -> None:
        package_source = HOT_UPDATE_PACKAGE_PATH.read_text(encoding="utf-8")
        reader_source = PACKAGE_READER_PATH.read_text(encoding="utf-8")
        validator_source = PACKAGE_VALIDATOR_PATH.read_text(encoding="utf-8")
        runtime_manager_source = RUNTIME_MANAGER_PATH.read_text(encoding="utf-8")
        interpreter_source = INTERPRETER_STUB_PATH.read_text(encoding="utf-8")

        for required_fragment in [
            "record HotUpdatePackage",
            "TargetAotVersion",
            "Assemblies",
            "Signature",
        ]:
            self.assertIn(required_fragment, package_source)

        for required_fragment in [
            "ReadFromDirectory",
            "ComputeFileHash",
            "SHA256",
            "LoadedAssemblies",
        ]:
            self.assertIn(required_fragment, reader_source)

        for required_fragment in [
            "ValidateCompatibleTargetAotVersion",
            "target AOT version",
            "InvalidOperationException",
        ]:
            self.assertIn(required_fragment, validator_source)

        for required_fragment in [
            "enum RuntimeMode",
            "LoadPackage",
            "UnloadPackage",
            "DispatchInt32",
            "IsMixedMode",
        ]:
            self.assertIn(required_fragment, runtime_manager_source)

        for required_fragment in [
            "class ConstantInt32InterpreterStub",
            "Execute",
            "return _constantValue;",
        ]:
            self.assertIn(required_fragment, interpreter_source)

    def test_native_hot_update_module_and_runtime_core_mode_query_are_wired(self) -> None:
        root_cmake_source = ROOT_CMAKE_PATH.read_text(encoding="utf-8")
        native_cmake_source = NATIVE_HOT_UPDATE_CMAKE_PATH.read_text(encoding="utf-8")
        native_header_source = NATIVE_HOT_UPDATE_HEADER_PATH.read_text(encoding="utf-8")
        native_source = NATIVE_HOT_UPDATE_SOURCE_PATH.read_text(encoding="utf-8")
        runtime_core_header_source = RUNTIME_CORE_HEADER_PATH.read_text(encoding="utf-8")
        runtime_core_source = RUNTIME_CORE_SOURCE_PATH.read_text(encoding="utf-8")

        self.assertIn("add_subdirectory(src/native/hot-update)", root_cmake_source)
        self.assertIn("chaos_hot_update", native_cmake_source)

        for required_fragment in [
            "struct HotUpdateAssemblyImage",
            "LoadAssemblyImageFromPath",
            "ReleaseAssemblyImage",
        ]:
            self.assertIn(required_fragment, native_header_source)

        for required_fragment in [
            "std::ifstream",
            "LoadAssemblyImageFromPath",
            "ReleaseAssemblyImage",
        ]:
            self.assertIn(required_fragment, native_source)

        for required_fragment in [
            "GetRuntimeMode",
            "SetRuntimeMode",
            "IsMixedMode",
        ]:
            self.assertIn(required_fragment, runtime_core_source)
        self.assertIn("enum class RuntimeMode", runtime_core_header_source)

    def test_hot_update_skeleton_proof_manifest_and_tree_are_realized(self) -> None:
        canonical_manifest = json.loads(CANONICAL_SUBJECT_MANIFEST_PATH.read_text(encoding="utf-8"))
        validation_profiles = dict(canonical_manifest.get("validationProfiles") or {})
        pipeline_ids = {
            str(pipeline["pipelineId"])
            for pipeline in list(canonical_manifest.get("executionPipelines") or [])
        }
        matrix_ids = {
            str(matrix["matrixId"])
            for matrix in list(canonical_manifest.get("environmentMatrices") or [])
        }

        self.assertEqual("HotUpdateHostPack", canonical_manifest["subjectId"])
        self.assertEqual("canonical", canonical_manifest["category"])
        self.assertEqual("windows-managed-output", canonical_manifest["defaultMatrix"])
        self.assertEqual("managed-output", canonical_manifest["defaultValidationProfile"])
        self.assertEqual("subjects/HotUpdateHostPack/source/HotUpdateHostPack.sln", canonical_manifest["source"]["path"])
        self.assertEqual("subjects/HotUpdateHostPack/source/HotUpdateHostPack.csproj", canonical_manifest["source"]["primaryProjectPath"])
        self.assertEqual("HotUpdateHostPack/Program::Main()", canonical_manifest["source"]["entry"])
        self.assertEqual("HotUpdateHostPack/HotUpdateLoadBenchmarkEntry::RunWorkload()", canonical_manifest["workloadEntry"])
        self.assertEqual("dotnet-solution", canonical_manifest["sourceModel"])
        self.assertEqual("require", canonical_manifest["testDeclarationMode"])
        self.assertEqual(["proof"], validation_profiles["managed-output"])
        self.assertEqual(["proof"], validation_profiles["corruption-check"])
        self.assertEqual(["perf"], validation_profiles["perf-profile"])
        self.assertEqual({"managed-runtime-output", "managed-benchmark"}, pipeline_ids)
        self.assertEqual(
            {
                "windows-managed-output",
                "macos-managed-output",
                "linux-managed-output",
                "windows-archetype-full-project-managed-output",
                "windows-managed-perf",
            },
            matrix_ids,
        )

        self.assertTrue(CANONICAL_SUBJECT_PROJECT_PATH.is_file())
        self.assertTrue(CANONICAL_SUBJECT_PROGRAM_PATH.is_file())
        self.assertTrue(CANONICAL_SKELETON_ENTRY_PATH.is_file())
        self.assertTrue(CANONICAL_PATCH_INTEGRITY_ENTRY_PATH.is_file())
        self.assertTrue(CANONICAL_PATCH_CALLBACK_ENTRY_PATH.is_file())
        self.assertTrue(CANONICAL_METADATA_SUPPLEMENT_ENTRY_PATH.is_file())
        self.assertTrue(CANONICAL_ARCHETYPE_SOLUTION_PATH.is_file())
        self.assertTrue(CANONICAL_ARCHETYPE_HOST_PROJECT_PATH.is_file())
        self.assertTrue(CANONICAL_ARCHETYPE_SHARED_PROJECT_PATH.is_file())
        self.assertTrue(CANONICAL_ARCHETYPE_PATCH_PROJECT_PATH.is_file())

        full_project_matrix = next(
            matrix
            for matrix in list(canonical_manifest.get("environmentMatrices") or [])
            if str(matrix["matrixId"]) == "windows-archetype-full-project-managed-output"
        )
        self.assertEqual("managed-runtime-output", full_project_matrix["pipelineId"])
        self.assertEqual(
            "subjects/HotUpdateHostPack/source/EngineeringScenarios/FullProjectHotUpdateSolution/HostApp/GoldenHotUpdateHost.App.csproj",
            dict(full_project_matrix.get("source") or {})["primaryProjectPath"],
        )
        self.assertEqual(
            "GoldenHotUpdateHost.App/Program::Main()",
            dict(full_project_matrix.get("source") or {})["entry"],
        )

    def test_hot_update_full_project_archetype_solution_is_wired_into_canonical_subject(self) -> None:
        solution_text = (CANONICAL_SUBJECT_ROOT / "source" / "HotUpdateHostPack.sln").read_text(encoding="utf-8")

        self.assertIn(r"Host\Program.cs", CANONICAL_SUBJECT_PROGRAM_PATH.as_posix().replace("/", "\\"))
        self.assertIn(r"Host\Proofs\HotUpdateSkeletonProofEntry.cs", CANONICAL_SKELETON_ENTRY_PATH.as_posix().replace("/", "\\"))
        self.assertIn(r"EngineeringScenarios\FullProjectHotUpdateSolution\HostApp\GoldenHotUpdateHost.App.csproj", solution_text)
        self.assertIn(r"EngineeringScenarios\FullProjectHotUpdateSolution\SharedContracts\GoldenHotUpdate.SharedContracts.csproj", solution_text)
        self.assertIn(r"EngineeringScenarios\FullProjectHotUpdateSolution\Patch\GoldenHotUpdate.PatchModule.csproj", solution_text)

    def test_hot_update_host_program_uses_compact_subject_entry_selection_only(self) -> None:
        host_program_source = CANONICAL_SUBJECT_PROGRAM_PATH.read_text(encoding="utf-8")

        for required_fragment in [
            "ChaosSubjectEntryArguments.TryParse",
            "HotUpdateSkeletonProofEntry.Run",
            "PatchIntegrityProofEntry.Run",
            "PatchCallbackFlowProofEntry.Run",
            "MetadataSupplementProofEntry.Run",
            "MethodReplacementProofEntry.Run",
            "SharedContractProofEntry.Run",
            "VersionRollbackProofEntry.Run",
        ]:
            self.assertIn(required_fragment, host_program_source)
        self.assertNotIn("ChaosSourceEntryArguments.TryParse", host_program_source)
        self.assertNotIn("DeclaredProofEntriesBySourceEntry", host_program_source)

    def test_patch_integrity_proof_declares_hot_update_integrity_slice(self) -> None:
        proof_source = CANONICAL_PATCH_INTEGRITY_ENTRY_PATH.read_text(encoding="utf-8")

        self.assertIn("[ChaosUnitTest(", proof_source)
        self.assertIn('Alias = "patch-integrity-proof"', proof_source)
        self.assertIn("Capability = ChaosCapabilityItem.PatchIntegrity", proof_source)
        self.assertIn("HotUpdateCapability = ChaosHotUpdateCapability.PatchIntegrity", proof_source)
        self.assertIn("PackageReader.ReadFromDirectory", proof_source)
        self.assertIn("Assert.Throws<InvalidDataException>", proof_source)

    def test_subject_query_finds_hot_update_surface_without_subject_name_coupling(self) -> None:
        subjects_module = load_module(SUBJECTS_MODULE_PATH, "chaos_subject_manifest_phase5_hot_update")

        record = subjects_module.require_single_subject_record(
            subjects_module.load_subject_records(REPO_ROOT),
            category="canonical",
            source_type="dotnet-project",
            required_stage_kinds=["runtime-managed-output"],
            required_goal_ids=["correctness.dev"],
            required_host_platforms=["windows-x64", "macos-arm64", "linux-x64"],
            required_validation_profile_ids=["managed-output", "corruption-check"],
            required_validation_kinds=["proof"],
        )
        capabilities = record["capabilities"]

        self.assertEqual("HotUpdateHostPack", record["subjectId"])
        self.assertEqual({"managed-runtime-output", "managed-benchmark"}, set(capabilities["pipelineIds"]))
        self.assertEqual({"windows-x64", "macos-arm64", "linux-x64"}, set(capabilities["hostPlatforms"]))


if __name__ == "__main__":
    unittest.main()
