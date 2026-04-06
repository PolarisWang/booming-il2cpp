from __future__ import annotations

import unittest
from pathlib import Path


REPO_ROOT = Path(__file__).resolve().parents[3]
ROOT_CMAKE_PATH = REPO_ROOT / "CMakeLists.txt"
PROOF_HOST_CMAKE_PATH = REPO_ROOT / "tests" / "proof" / "native-reference" / "HelloWorldObject" / "CMakeLists.txt"
RUNTIME_ABI_HEADER_PATH = REPO_ROOT / "contracts" / "native" / "v0" / "runtime_abi.h"
RUNTIME_CORE_CMAKE_PATH = REPO_ROOT / "src" / "native" / "runtime-core" / "CMakeLists.txt"
BOOTSTRAP_CMAKE_PATH = REPO_ROOT / "src" / "native" / "bootstrap" / "CMakeLists.txt"
SUPPORT_CMAKE_PATH = REPO_ROOT / "src" / "native" / "support" / "CMakeLists.txt"


class Stage4WindowsReferenceWiringTests(unittest.TestCase):
    def test_windows_reference_router_keeps_contract_smoke_and_adds_stage4_native_graph(self) -> None:
        root_cmake_text = ROOT_CMAKE_PATH.read_text(encoding="utf-8")

        required_markers = [
            'if(ROADMAP0_PRESET_TARGET STREQUAL "windows-x64-reference")',
            "add_subdirectory(src/native/runtime-core)",
            "add_subdirectory(src/native/support)",
            "add_subdirectory(src/native/bootstrap)",
            "add_subdirectory(tests/proof/native-reference/HelloWorldObject)",
            "add_subdirectory(tests/contracts/native/abi)",
        ]

        for marker in required_markers:
            self.assertIn(marker, root_cmake_text)

    def test_proof_host_cmake_materializes_stage4_generated_source_from_stage3_bundle(self) -> None:
        proof_host_cmake_text = PROOF_HOST_CMAKE_PATH.read_text(encoding="utf-8")

        required_markers = [
            "artifacts/proof/managed-closure/HelloWorldObject",
            "artifacts/proof/native-reference/HelloWorldObject",
            "typed-il-ir.json",
            "aot-manifest.json",
            "metadata-registration.json",
            "code-registration.json",
            "closure.manifest.json",
            "native-reference.generated.cpp",
            "native-proof.manifest.json",
            "Chaos.IL2CPP.Driver.csproj",
            "tests/proof/input/HelloWorldObject/HelloWorldObject.csproj",
            "emit-native-reference",
            "Chaos.IL2CPP.Driver.dll",
            "add_custom_target(chaos_stage4_hello_world_object_host_input_build",
            "add_custom_target(chaos_stage4_hello_world_object_driver_build",
            "add_custom_target(chaos_stage4_hello_world_object_managed_closure",
            "add_custom_target(chaos_stage4_hello_world_object_codegen",
            "add_custom_target(chaos_stage4_hello_world_object_proof_run",
            "RunNativeReferenceProof.cmake",
            "stdout.log",
            "stderr.log",
            "exit-code.txt",
            "BYPRODUCTS",
            "add_dependencies(chaos_stage4_hello_world_object_managed_closure",
            "add_dependencies(chaos_stage4_hello_world_object_codegen chaos_stage4_hello_world_object_managed_closure)",
            "EXCLUDE_FROM_ALL",
            "add_dependencies(chaos_stage4_hello_world_object_proof chaos_stage4_hello_world_object_codegen)",
        ]

        for marker in required_markers:
            self.assertIn(marker, proof_host_cmake_text)

    def test_static_native_targets_disable_dllimport_for_runtime_abi_headers(self) -> None:
        runtime_abi_header_text = RUNTIME_ABI_HEADER_PATH.read_text(encoding="utf-8")
        runtime_core_cmake_text = RUNTIME_CORE_CMAKE_PATH.read_text(encoding="utf-8")
        bootstrap_cmake_text = BOOTSTRAP_CMAKE_PATH.read_text(encoding="utf-8")
        support_cmake_text = SUPPORT_CMAKE_PATH.read_text(encoding="utf-8")

        self.assertIn("defined(CHAOS_RUNTIME_ABI_STATIC)", runtime_abi_header_text)
        self.assertIn("#define CHAOS_RUNTIME_ABI_EXPORT", runtime_abi_header_text)
        self.assertIn("PUBLIC CHAOS_RUNTIME_ABI_STATIC", runtime_core_cmake_text)
        self.assertIn("PUBLIC CHAOS_RUNTIME_ABI_STATIC", bootstrap_cmake_text)
        self.assertIn("PUBLIC CHAOS_RUNTIME_ABI_STATIC", support_cmake_text)


if __name__ == "__main__":
    unittest.main()
