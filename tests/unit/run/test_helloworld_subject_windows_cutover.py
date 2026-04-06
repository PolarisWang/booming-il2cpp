from __future__ import annotations

import unittest
from pathlib import Path


REPO_ROOT = Path(__file__).resolve().parents[3]
PROOF_HOST_CMAKE_PATH = REPO_ROOT / "tests" / "proof" / "native-reference" / "HelloWorldObject" / "CMakeLists.txt"
VERIFY_PY_PATH = REPO_ROOT / "build" / "scripts" / "verify-roadmap-0.py"
VERIFY_PS1_PATH = REPO_ROOT / "build" / "scripts" / "verify-roadmap-0.ps1"
LEGACY_INPUT_PROJECT_PATH = REPO_ROOT / "tests" / "proof" / "input" / "HelloWorldObject" / "HelloWorldObject.csproj"
SUBJECT_INPUT_PROJECT_PATH = REPO_ROOT / "subjects" / "HelloWorldObject" / "source" / "HelloWorldObject.csproj"
SUBJECT_PROGRAM_PATH = REPO_ROOT / "subjects" / "HelloWorldObject" / "source" / "Program.cs"


class HelloWorldSubjectWindowsCutoverTests(unittest.TestCase):
    def test_proof_host_cmake_consumes_subject_generated_bucket_without_stage4_hidden_codegen(self) -> None:
        proof_host_cmake_text = PROOF_HOST_CMAKE_PATH.read_text(encoding="utf-8")

        required_markers = [
            "artifacts/subjects/HelloWorldObject/shared/generated",
            "native-reference.generated.cpp",
            "chaos_stage4_hello_world_object_proof",
            "chaos_stage4_hello_world_object_proof_run",
            "stdout.log",
            "stderr.log",
            "exit-code.txt",
        ]
        forbidden_markers = [
            "dotnet build",
            "emit-native-reference",
            "artifacts/proof/managed-closure/HelloWorldObject",
            "artifacts/proof/native-reference/HelloWorldObject",
            "tests/proof/input/HelloWorldObject/HelloWorldObject.csproj",
            "add_custom_target(chaos_stage4_hello_world_object_host_input_build",
            "add_custom_target(chaos_stage4_hello_world_object_driver_build",
            "add_custom_target(chaos_stage4_hello_world_object_managed_closure",
            "add_custom_target(chaos_stage4_hello_world_object_codegen",
        ]

        for marker in required_markers:
            self.assertIn(marker, proof_host_cmake_text)
        for marker in forbidden_markers:
            self.assertNotIn(marker, proof_host_cmake_text)

    def test_verify_roadmap0_windows_branch_runs_subject_matrices_instead_of_stage4_codegen_chain(self) -> None:
        verify_text = VERIFY_PY_PATH.read_text(encoding="utf-8")

        required_markers = [
            "subject_executor",
            "subject_planner",
            "windows-dev-output",
            "windows-reference-trace",
            "windows-android-buildable",
            "windows-linux-buildable",
            "windows-stage4-native-reference.gate.json",
            "windows-reference-desktop.gate.json",
            "android-startup-smoke.gate.json",
            "linux-packaging.gate.json",
        ]
        forbidden_markers = [
            "tests/proof/input/HelloWorldObject/bin/Release/net8.0/HelloWorldObject.dll",
            "artifacts/proof/managed-closure/HelloWorldObject",
            "artifacts/proof/native-reference/HelloWorldObject",
            "invoke_stage4_native_reference_codegen",
        ]

        for marker in required_markers:
            self.assertIn(marker, verify_text)
        for marker in forbidden_markers:
            self.assertNotIn(marker, verify_text)

    def test_powershell_verify_wrapper_forwards_to_python_entrypoint(self) -> None:
        verify_text = VERIFY_PS1_PATH.read_text(encoding="utf-8")

        self.assertIn("verify-roadmap-0.py", verify_text)
        self.assertIn("--host-profile", verify_text)
        self.assertNotIn("Invoke-Stage4NativeReferenceCodegen", verify_text)

    def test_legacy_proof_input_project_is_now_only_a_compatibility_shell(self) -> None:
        project_text = LEGACY_INPUT_PROJECT_PATH.read_text(encoding="utf-8")

        self.assertIn("<EnableDefaultCompileItems>false</EnableDefaultCompileItems>", project_text)
        self.assertIn("subjects/HelloWorldObject/source/Program.cs", project_text)
        self.assertIn("subjects/HelloWorldObject/source/HelloWorldObject.csproj", project_text)

    def test_subject_input_project_declares_target_framework(self) -> None:
        project_text = SUBJECT_INPUT_PROJECT_PATH.read_text(encoding="utf-8")

        self.assertIn("<TargetFramework>net8.0</TargetFramework>", project_text)

    def test_subject_program_declares_console_namespace(self) -> None:
        program_text = SUBJECT_PROGRAM_PATH.read_text(encoding="utf-8")

        self.assertIn("using System;", program_text)


if __name__ == "__main__":
    unittest.main()
