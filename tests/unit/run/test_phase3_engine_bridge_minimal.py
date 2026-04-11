from __future__ import annotations

import unittest
from pathlib import Path


REPO_ROOT = Path(__file__).resolve().parents[3]
ROOT_CMAKE_PATH = REPO_ROOT / "CMakeLists.txt"
ENGINE_BRIDGE_ROOT = REPO_ROOT / "src" / "native" / "engine-bridge"
ENGINE_BRIDGE_CMAKE_PATH = ENGINE_BRIDGE_ROOT / "CMakeLists.txt"
ENGINE_BRIDGE_HEADER_PATH = ENGINE_BRIDGE_ROOT / "engine_bridge.h"
ENGINE_BRIDGE_SOURCE_PATH = ENGINE_BRIDGE_ROOT / "engine_bridge.cpp"


class Phase3EngineBridgeMinimalTests(unittest.TestCase):
    def test_engine_bridge_module_files_exist_and_root_cmake_wires_it(self) -> None:
        root_cmake_source = ROOT_CMAKE_PATH.read_text(encoding="utf-8")

        self.assertTrue(ENGINE_BRIDGE_ROOT.is_dir(), msg=f"missing engine bridge root: {ENGINE_BRIDGE_ROOT}")
        self.assertTrue(ENGINE_BRIDGE_CMAKE_PATH.is_file(), msg=f"missing engine bridge CMake file: {ENGINE_BRIDGE_CMAKE_PATH}")
        self.assertTrue(ENGINE_BRIDGE_HEADER_PATH.is_file(), msg=f"missing engine bridge header: {ENGINE_BRIDGE_HEADER_PATH}")
        self.assertTrue(ENGINE_BRIDGE_SOURCE_PATH.is_file(), msg=f"missing engine bridge source: {ENGINE_BRIDGE_SOURCE_PATH}")
        self.assertIn("add_subdirectory(src/native/engine-bridge)", root_cmake_source)

    def test_engine_bridge_header_exposes_contract_umbrella(self) -> None:
        header_source = ENGINE_BRIDGE_HEADER_PATH.read_text(encoding="utf-8")

        self.assertIn('#include "lifecycle.h"', header_source)
        self.assertIn('#include "ownership.h"', header_source)
        self.assertIn('#include "callback.h"', header_source)

    def test_engine_bridge_source_anchors_lifecycle_callback_and_ownership_minimal_impl(self) -> None:
        source = ENGINE_BRIDGE_SOURCE_PATH.read_text(encoding="utf-8")

        for required_fragment in [
            "chaos_runtime_get_abi_v0",
            "DispatchEngineLifecycleCallbacks",
            "engine_register_callback",
            "engine_dispatch_callback",
            "engine_retain_object",
            "engine_release_object",
            "engine_transfer_ownership",
        ]:
            self.assertIn(required_fragment, source)


if __name__ == "__main__":
    unittest.main()
