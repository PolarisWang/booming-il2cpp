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
CANONICAL_SUBJECT_PROGRAM_PATH = CANONICAL_SUBJECT_ROOT / "source" / "Program.cs"
CANONICAL_SKELETON_ENTRY_PATH = CANONICAL_SUBJECT_ROOT / "source" / "HotUpdateSkeletonProofEntry.cs"
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
    def test_hot_update_project_isolation_and_solution_wiring(self) -> None:
        solution_source = CORE_SOLUTION_PATH.read_text(encoding="utf-8")

        self.assertTrue(HOT_UPDATE_ROOT.is_dir(), msg=f"missing hot update root: {HOT_UPDATE_ROOT}")
        self.assertTrue(HOT_UPDATE_PROJECT_PATH.is_file(), msg=f"missing hot update project: {HOT_UPDATE_PROJECT_PATH}")
        self.assertEqual(["Chaos.IL2CPP.Contracts"], parse_project_references(HOT_UPDATE_PROJECT_PATH))
        self.assertIn("Chaos.IL2CPP.HotUpdate", solution_source)

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
        self.assertEqual("subjects/HotUpdateHostPack/source/HotUpdateHostPack.csproj", canonical_manifest["source"]["path"])
        self.assertEqual("HotUpdateHostPack/Program::Main()", canonical_manifest["source"]["entry"])
        self.assertEqual("require", canonical_manifest["testDeclarationMode"])
        self.assertEqual(["proof"], validation_profiles["managed-output"])
        self.assertEqual(["proof"], validation_profiles["corruption-check"])
        self.assertEqual({"managed-runtime-output"}, pipeline_ids)
        self.assertEqual(
            {"windows-managed-output", "macos-managed-output", "linux-managed-output"},
            matrix_ids,
        )

        self.assertTrue(CANONICAL_SUBJECT_PROJECT_PATH.is_file())
        self.assertTrue(CANONICAL_SUBJECT_PROGRAM_PATH.is_file())
        self.assertTrue(CANONICAL_SKELETON_ENTRY_PATH.is_file())

    def test_hot_update_skeleton_proof_program_exercises_aot_mixed_aot_and_corruption_rejection(self) -> None:
        program_source = CANONICAL_SKELETON_ENTRY_PATH.read_text(encoding="utf-8")

        for required_fragment in [
            "before-load=",
            "after-load=",
            "after-unload=",
            "corruption=rejected",
            "LoadPackage",
            "UnloadPackage",
            "DispatchInt32",
            "[HotUpdateSubjectId] = 42",
        ]:
            self.assertIn(required_fragment, program_source)

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
        self.assertEqual({"managed-runtime-output"}, set(capabilities["pipelineIds"]))
        self.assertEqual({"windows-x64", "macos-arm64", "linux-x64"}, set(capabilities["hostPlatforms"]))


if __name__ == "__main__":
    unittest.main()
