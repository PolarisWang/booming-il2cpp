from __future__ import annotations

import unittest
from pathlib import Path


REPO_ROOT = Path(__file__).resolve().parents[3]
ROOT_CMAKE_PATH = REPO_ROOT / "CMakeLists.txt"
RUNTIME_CORE_DIR = REPO_ROOT / "src" / "native" / "runtime-core"
RUNTIME_CORE_CMAKE_PATH = RUNTIME_CORE_DIR / "CMakeLists.txt"
RUNTIME_CORE_HEADER_PATH = RUNTIME_CORE_DIR / "runtime_core.h"
RUNTIME_CORE_SOURCE_PATH = RUNTIME_CORE_DIR / "runtime_core.cpp"


class NativeRuntimeCoreTests(unittest.TestCase):
    def test_runtime_core_is_wired_into_windows_reference_router(self) -> None:
        self.assertTrue(RUNTIME_CORE_DIR.is_dir(), msg=f"missing runtime-core dir: {RUNTIME_CORE_DIR}")
        self.assertTrue(RUNTIME_CORE_CMAKE_PATH.is_file(), msg=f"missing runtime-core CMakeLists: {RUNTIME_CORE_CMAKE_PATH}")
        self.assertTrue(RUNTIME_CORE_HEADER_PATH.is_file(), msg=f"missing runtime-core header: {RUNTIME_CORE_HEADER_PATH}")
        self.assertTrue(RUNTIME_CORE_SOURCE_PATH.is_file(), msg=f"missing runtime-core source: {RUNTIME_CORE_SOURCE_PATH}")

        root_cmake_text = ROOT_CMAKE_PATH.read_text(encoding="utf-8")
        self.assertIn("add_subdirectory(src/native/runtime-core)", root_cmake_text)

    def test_runtime_core_exports_first_proof_runtime_abi_table(self) -> None:
        runtime_core_text = RUNTIME_CORE_SOURCE_PATH.read_text(encoding="utf-8")

        required_markers = [
            "chaos_runtime_get_abi_v0",
            "GetRuntimeAbiV0",
            "kRuntimeAbiV0",
            "RuntimeInit",
            "ThreadAttach",
            "ObjectNew",
            "StringNewUtf8",
            "ClassInit",
            "FieldGetValue",
            "FieldSetValue",
            "AssemblyGetImage",
            "ImageFindType",
            "TypeFindMethod",
            "TypeFindField",
            "RaiseManagedException",
            "MethodInvoke",
            "field_storage",
            "std::memset(object->field_storage, 0, sizeof(object->field_storage));",
            "std::memcpy(out_value, object->field_storage, out_value_size);",
            "std::memcpy(object->field_storage, value, value_size);",
            "CHAOS_RUNTIME_STATUS_NOT_SUPPORTED",
            "return &kRuntimeAbiV0;",
        ]

        for marker in required_markers:
            self.assertIn(marker, runtime_core_text)


if __name__ == "__main__":
    unittest.main()
