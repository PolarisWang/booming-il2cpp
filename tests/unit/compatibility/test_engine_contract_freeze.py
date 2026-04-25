from __future__ import annotations

import unittest
from pathlib import Path


REPO_ROOT = Path(__file__).resolve().parents[3]
ENGINE_CONTRACT_ROOT = REPO_ROOT / "contracts" / "engine" / "v0"
LIFECYCLE_HEADER_PATH = ENGINE_CONTRACT_ROOT / "lifecycle.h"
OWNERSHIP_HEADER_PATH = ENGINE_CONTRACT_ROOT / "ownership.h"
CALLBACK_HEADER_PATH = ENGINE_CONTRACT_ROOT / "callback.h"
THREAD_MODEL_DOC_PATH = ENGINE_CONTRACT_ROOT / "thread-model.md"
RUNTIME_CORE_HEADER_PATH = REPO_ROOT / "src" / "native" / "runtime-core" / "runtime_core.h"
RUNTIME_CORE_SOURCE_PATH = REPO_ROOT / "src" / "native" / "runtime-core" / "runtime_core.cpp"


class EngineContractFreezeTests(unittest.TestCase):
    def test_engine_contract_directory_and_files_exist(self) -> None:
        self.assertTrue(ENGINE_CONTRACT_ROOT.is_dir(), msg=f"missing engine contract root: {ENGINE_CONTRACT_ROOT}")
        for path in [
            LIFECYCLE_HEADER_PATH,
            OWNERSHIP_HEADER_PATH,
            CALLBACK_HEADER_PATH,
            THREAD_MODEL_DOC_PATH,
        ]:
            self.assertTrue(path.is_file(), msg=f"missing engine contract artifact: {path}")

    def test_engine_contract_headers_freeze_abi_names(self) -> None:
        lifecycle_source = LIFECYCLE_HEADER_PATH.read_text(encoding="utf-8")
        ownership_source = OWNERSHIP_HEADER_PATH.read_text(encoding="utf-8")
        callback_source = CALLBACK_HEADER_PATH.read_text(encoding="utf-8")
        thread_model_source = THREAD_MODEL_DOC_PATH.read_text(encoding="utf-8")

        for abi_name in [
            "engine_init",
            "engine_shutdown",
            "engine_reload",
            "engine_scene_attach",
        ]:
            self.assertIn(abi_name, lifecycle_source)

        for abi_name in [
            "engine_retain_object",
            "engine_release_object",
            "engine_transfer_ownership",
        ]:
            self.assertIn(abi_name, ownership_source)

        for abi_name in [
            "engine_register_callback",
            "engine_dispatch_callback",
        ]:
            self.assertIn(abi_name, callback_source)

        self.assertIn("engine_post_to_main_thread", thread_model_source)
        self.assertIn("engine_is_main_thread", thread_model_source)
        self.assertIn("主线程", thread_model_source)

    def test_runtime_core_helpers_still_anchor_batch1_contract_slice(self) -> None:
        runtime_core_header = RUNTIME_CORE_HEADER_PATH.read_text(encoding="utf-8")
        runtime_core_source = RUNTIME_CORE_SOURCE_PATH.read_text(encoding="utf-8")

        for helper_name in [
            "CreateEngineObjectHandle",
            "ResolveEngineObjectHandle",
            "RegisterEngineLifecycleCallback",
            "DispatchEngineLifecycleCallbacks",
            "IsMainThreadLane",
        ]:
            self.assertIn(helper_name, runtime_core_header)
            self.assertIn(helper_name, runtime_core_source)


if __name__ == "__main__":
    unittest.main()
