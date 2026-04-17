from __future__ import annotations

import unittest
from pathlib import Path

REPO_ROOT = Path(__file__).resolve().parents[3]

BOOTSTRAP_DIR = REPO_ROOT / "src" / "native" / "bootstrap"
BOOTSTRAP_CMAKE_PATH = BOOTSTRAP_DIR / "CMakeLists.txt"
BOOTSTRAP_HEADER_PATH = BOOTSTRAP_DIR / "bootstrap.h"
BOOTSTRAP_SOURCE_PATH = BOOTSTRAP_DIR / "bootstrap.cpp"
CODEGEN_BRIDGE_HEADER_PATH = REPO_ROOT / "contracts" / "native" / "v0" / "codegen_bridge.h"

SUPPORT_DIR = REPO_ROOT / "src" / "native" / "support"
SUPPORT_CMAKE_PATH = SUPPORT_DIR / "CMakeLists.txt"
SUPPORT_HEADER_PATH = SUPPORT_DIR / "support.h"
SUPPORT_SOURCE_PATH = SUPPORT_DIR / "support.cpp"
SUBJECT_TEMPLATE_ROOT = REPO_ROOT / "build" / "toolchains" / "run" / "subject" / "templates"
PROOF_WORKSPACE_TEMPLATE_PATH = SUBJECT_TEMPLATE_ROOT / "native-reference-workspace.cmake.tmpl"
PROOF_GENERATED_TEMPLATE_PATH = SUBJECT_TEMPLATE_ROOT / "native-generated.cmake.tmpl"
PROOF_HOST_TEMPLATE_PATH = SUBJECT_TEMPLATE_ROOT / "native-proof-main.cpp.tmpl"
PROOF_CMAKE_TEMPLATE_PATH = SUBJECT_TEMPLATE_ROOT / "native-proof.cmake.tmpl"
PROOF_RUN_SCRIPT_TEMPLATE_PATH = SUBJECT_TEMPLATE_ROOT / "native-proof-run.cmake.tmpl"


class NativeReferenceBootstrapSupportTests(unittest.TestCase):
    def test_bootstrap_support_and_generic_proof_host_templates_exist(self) -> None:
        expected_paths = [
            BOOTSTRAP_DIR,
            BOOTSTRAP_CMAKE_PATH,
            BOOTSTRAP_HEADER_PATH,
            BOOTSTRAP_SOURCE_PATH,
            SUPPORT_DIR,
            SUPPORT_CMAKE_PATH,
            SUPPORT_HEADER_PATH,
            SUPPORT_SOURCE_PATH,
            SUBJECT_TEMPLATE_ROOT,
            PROOF_WORKSPACE_TEMPLATE_PATH,
            PROOF_GENERATED_TEMPLATE_PATH,
            PROOF_HOST_TEMPLATE_PATH,
            PROOF_CMAKE_TEMPLATE_PATH,
            PROOF_RUN_SCRIPT_TEMPLATE_PATH,
        ]

        for path in expected_paths:
            self.assertTrue(path.exists(), msg=f"missing native reference skeleton path: {path}")

    def test_bootstrap_source_exposes_first_proof_bridge_surface(self) -> None:
        bootstrap_text = BOOTSTRAP_SOURCE_PATH.read_text(encoding="utf-8")

        required_markers = [
            "chaos_codegen_get_bridge_v0",
            "GetCodegenBridgeV0",
            "kCodegenBridgeV0",
            "MakeOpaqueHandle",
            "RegisterCodegen",
            "BootstrapRuntime",
            "ResolveTypeByToken",
            "ResolveMethodByToken",
            "ResolveFieldByToken",
            "ResolveIcall",
            "CHAOS_BRIDGE_STATUS_NOT_READY",
            "CHAOS_BRIDGE_STATUS_NOT_SUPPORTED",
            "type_token == 0u",
            "method_token == 0u",
            "field_token == 0u",
            "System.Console/System.Console::WriteLine(System.String)",
            "System.Private.CoreLib/System.String::Concat(System.String,System.String)",
        ]

        for marker in required_markers:
            self.assertIn(marker, bootstrap_text)

    def test_codegen_bridge_header_declares_not_supported_status(self) -> None:
        bridge_header_text = CODEGEN_BRIDGE_HEADER_PATH.read_text(encoding="utf-8")

        self.assertIn("CHAOS_BRIDGE_STATUS_NOT_FOUND = 3", bridge_header_text)
        self.assertIn("CHAOS_BRIDGE_STATUS_NOT_SUPPORTED = 4", bridge_header_text)
        self.assertIn("CHAOS_BRIDGE_STATUS_MANAGED_EXCEPTION = 5", bridge_header_text)
        self.assertIn("CHAOS_BRIDGE_STATUS_INTERNAL_ERROR = 6", bridge_header_text)

    def test_support_source_exposes_narrow_string_and_console_glue(self) -> None:
        support_text = SUPPORT_SOURCE_PATH.read_text(encoding="utf-8")

        required_markers = [
            "TryGetUtf8View",
            "ConcatStringPair",
            "WriteLineString",
            "chaos_runtime_get_abi_v0",
            "string_new_utf8",
            "std::fwrite",
            "std::fputc",
        ]

        for marker in required_markers:
            self.assertIn(marker, support_text)

    def test_proof_host_templates_consume_generated_translation_unit(self) -> None:
        proof_workspace_text = PROOF_WORKSPACE_TEMPLATE_PATH.read_text(encoding="utf-8")
        proof_generated_text = PROOF_GENERATED_TEMPLATE_PATH.read_text(encoding="utf-8")
        proof_host_main_text = PROOF_HOST_TEMPLATE_PATH.read_text(encoding="utf-8")
        proof_host_cmake_text = PROOF_CMAKE_TEMPLATE_PATH.read_text(encoding="utf-8")
        proof_host_run_script_text = PROOF_RUN_SCRIPT_TEMPLATE_PATH.read_text(encoding="utf-8")

        self.assertIn("add_subdirectory(generated)", proof_workspace_text)
        self.assertIn("add_subdirectory(proof)", proof_workspace_text)
        self.assertIn("CHAOS_SUBJECT_REPO_ROOT", proof_workspace_text)
        self.assertIn("CHAOS_SUBJECT_GENERATED_INPUT_SOURCE", proof_generated_text)
        self.assertIn("chaos_subject_generated_native", proof_generated_text)
        self.assertIn("chaos_runtime_core", proof_host_cmake_text)
        self.assertIn("chaos_bootstrap", proof_host_cmake_text)
        self.assertIn("chaos_support", proof_host_cmake_text)
        self.assertIn("stdout.log", proof_host_cmake_text)
        self.assertIn("stderr.log", proof_host_cmake_text)
        self.assertIn("exit-code.txt", proof_host_cmake_text)

        required_markers = [
            "RunNativeReference",
            "chaos_runtime_get_abi_v0",
            "chaos_codegen_get_bridge_v0",
            "kHostName = \"windows-x64-reference\"",
            "CodeRegistrationV0",
            "MetadataRegistrationV0",
            "CodegenRegistrationOptionsV0",
            "System.Console/System.Console::WriteLine(System.String)",
            "System.Private.CoreLib/System.String::Concat(System.String,System.String)",
        ]

        for marker in required_markers:
            self.assertIn(marker, proof_host_main_text)

        run_script_markers = [
            "execute_process(",
            "RESULT_VARIABLE proof_exit_code",
            "OUTPUT_FILE",
            "ERROR_FILE",
            "CHAOS_SUBJECT_PROOF_EXIT_CODE_PATH",
        ]

        for marker in run_script_markers:
            self.assertIn(marker, proof_host_run_script_text)
