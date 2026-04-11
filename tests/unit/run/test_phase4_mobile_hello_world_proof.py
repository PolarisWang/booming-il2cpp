from __future__ import annotations

import importlib.util
import json
import sys
import unittest
from pathlib import Path


REPO_ROOT = Path(__file__).resolve().parents[3]
SUBJECT_ROOT = REPO_ROOT / "subjects" / "MobileHelloWorldProof"
MANIFEST_PATH = SUBJECT_ROOT / "subject.manifest.json"
SOURCE_PROJECT_PATH = SUBJECT_ROOT / "source" / "MobileHelloWorldProof.csproj"
SOURCE_PROGRAM_PATH = SUBJECT_ROOT / "source" / "Program.cs"
ANDROID_HOST_ROOT = SUBJECT_ROOT / "validation" / "mobile" / "android-host"
IOS_HOST_ROOT = SUBJECT_ROOT / "validation" / "mobile" / "ios-host"
SUBJECTS_MODULE_PATH = REPO_ROOT / "build" / "toolchains" / "run" / "testing" / "subjects.py"
CMAKE_PATH = REPO_ROOT / "CMakeLists.txt"
WORKERS_PATH = REPO_ROOT / "build" / "toolchains" / "run" / "testing" / "subject_workers.py"


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


class Phase4MobileHelloWorldProofTests(unittest.TestCase):
    def test_mobile_hello_world_proof_manifest_and_tree_are_realized(self) -> None:
        manifest = json.loads(MANIFEST_PATH.read_text(encoding="utf-8"))
        validation_profiles = dict(manifest.get("validationProfiles") or {})
        pipeline_ids = {
            str(pipeline["pipelineId"])
            for pipeline in list(manifest.get("executionPipelines") or [])
        }
        matrix_ids = {
            str(matrix["matrixId"])
            for matrix in list(manifest.get("environmentMatrices") or [])
        }

        self.assertEqual("MobileHelloWorldProof", manifest["subjectId"])
        self.assertEqual("canonical", manifest["category"])
        self.assertEqual("windows-managed-output", manifest["defaultMatrix"])
        self.assertEqual("managed-output", manifest["defaultValidationProfile"])
        self.assertEqual("MobileHelloWorldProof/Program::Main(System.String[])", manifest["source"]["entry"])
        self.assertEqual(["proof"], validation_profiles["managed-output"])
        self.assertEqual({"managed-runtime-output", "platform-buildable", "android-runtime-observe"}, pipeline_ids)
        self.assertEqual(
            {"windows-managed-output", "windows-android-buildable", "windows-android-runtime", "windows-ios-buildable"},
            matrix_ids,
        )

        android_runtime_matrix = next(
            matrix for matrix in list(manifest.get("environmentMatrices") or [])
            if str(matrix.get("matrixId") or "") == "windows-android-runtime"
        )
        self.assertEqual("android-runtime-observe", str(android_runtime_matrix["pipelineId"]))
        self.assertEqual("runtime", str(dict(android_runtime_matrix["artifactPlan"])["evidenceTerminalBucket"]))

        self.assertTrue(SOURCE_PROJECT_PATH.is_file())
        self.assertTrue(SOURCE_PROGRAM_PATH.is_file())
        self.assertTrue((ANDROID_HOST_ROOT / "CMakeLists.txt").is_file())
        self.assertTrue((ANDROID_HOST_ROOT / "mobile_host_entry.cpp").is_file())
        self.assertTrue((ANDROID_HOST_ROOT / "mobile_runtime_main.cpp").is_file())
        self.assertTrue((IOS_HOST_ROOT / "CMakeLists.txt").is_file())
        self.assertTrue((IOS_HOST_ROOT / "mobile_host_entry.mm").is_file())

    def test_subject_scoped_mobile_host_entries_forward_subject_id_to_shared_host(self) -> None:
        android_entry = (ANDROID_HOST_ROOT / "mobile_host_entry.cpp").read_text(encoding="utf-8")
        ios_entry = (IOS_HOST_ROOT / "mobile_host_entry.mm").read_text(encoding="utf-8")

        for source_text in [android_entry, ios_entry]:
            self.assertIn('"MobileHelloWorldProof"', source_text)
            self.assertIn("il2cpp_host_run(1, argv)", source_text)

    def test_mobile_source_stays_on_supported_captured_state_lowering_shape(self) -> None:
        source_program = SOURCE_PROGRAM_PATH.read_text(encoding="utf-8")

        self.assertIn("namespace MobileHelloWorldProof;", source_program)
        self.assertIn("internal sealed class MobileBanner", source_program)
        self.assertIn("private readonly string _name;", source_program)
        self.assertIn("public MobileBanner(string name)", source_program)
        self.assertIn('return "Mobile native proof: " + _name + ".";', source_program)
        self.assertIn('var banner = new MobileBanner("hello world");', source_program)
        self.assertIn("Console.WriteLine(banner.BuildMessage());", source_program)
        self.assertNotIn("ReadOnlyCollection", source_program)
        self.assertNotIn("AsReadOnly", source_program)
        self.assertNotIn("$\"", source_program)
        self.assertNotIn("nameof(Main)", source_program)

    def test_subject_scoped_android_host_declares_runtime_executable_target(self) -> None:
        android_cmake = (ANDROID_HOST_ROOT / "CMakeLists.txt").read_text(encoding="utf-8")
        android_runtime_main = (ANDROID_HOST_ROOT / "mobile_runtime_main.cpp").read_text(encoding="utf-8")

        self.assertIn("CHAOS_SUBJECT_ANDROID_ARTIFACT_ROOT", android_cmake)
        self.assertIn("add_executable(", android_cmake)
        self.assertIn("mobile_hello_world_android_host_runtime", android_cmake)
        self.assertIn("mobile_runtime_main.cpp", android_cmake)
        self.assertIn('"MobileHelloWorldProof"', android_runtime_main)
        self.assertIn("il2cpp_host_run(1, argv)", android_runtime_main)

    def test_subject_query_finds_mobile_runtime_host_surface_without_subject_name_coupling(self) -> None:
        subjects_module = load_module(SUBJECTS_MODULE_PATH, "chaos_subject_manifest_phase4_mobile_host")

        record = subjects_module.require_single_subject_record(
            subjects_module.load_subject_records(REPO_ROOT),
            category="canonical",
            source_type="dotnet-project",
            required_stage_kinds=["runtime-managed-output", "build-target"],
            required_goal_ids=["correctness.dev", "correctness.platform"],
            required_host_platforms=["windows-x64"],
            required_validation_profile_ids=["managed-output"],
            required_validation_kinds=["proof"],
        )
        capabilities = record["capabilities"]

        self.assertEqual("MobileHelloWorldProof", record["subjectId"])
        self.assertEqual({"managed-runtime-output", "platform-buildable", "android-runtime-observe"}, set(capabilities["pipelineIds"]))
        self.assertEqual({"windows-x64"}, set(capabilities["hostPlatforms"]))

    def test_mobile_subject_host_roots_are_routed_from_worker_to_root_cmake(self) -> None:
        cmake_source = CMAKE_PATH.read_text(encoding="utf-8")
        workers_source = WORKERS_PATH.read_text(encoding="utf-8")

        for required_fragment in [
            "CHAOS_SUBJECT_ANDROID_HOST_ROOT",
            'add_subdirectory("${CHAOS_SUBJECT_ANDROID_HOST_ROOT}"',
            "CHAOS_SUBJECT_IOS_HOST_ROOT",
            'add_subdirectory("${CHAOS_SUBJECT_IOS_HOST_ROOT}"',
        ]:
            self.assertIn(required_fragment, cmake_source)

        for required_fragment in [
            'validation" / "mobile" / "android-host"',
            'validation" / "mobile" / "ios-host"',
            "CHAOS_SUBJECT_ANDROID_HOST_ROOT",
            "CHAOS_SUBJECT_IOS_HOST_ROOT",
        ]:
            self.assertIn(required_fragment, workers_source)


if __name__ == "__main__":
    unittest.main()
