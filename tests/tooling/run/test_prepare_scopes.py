from __future__ import annotations

import importlib.util
import unittest
from pathlib import Path


REPO_ROOT = Path(__file__).resolve().parents[3]
PREPARE_MODULE_PATH = REPO_ROOT / "build" / "toolchains" / "run" / "commands" / "prepare.py"
RUN_MANIFEST_PATH = REPO_ROOT / "build" / "toolchains" / "run" / "run_manifest.json"


def load_prepare_module():
    if not PREPARE_MODULE_PATH.is_file():
        raise FileNotFoundError(f"prepare module missing: {PREPARE_MODULE_PATH}")

    spec = importlib.util.spec_from_file_location("booming_run_prepare", PREPARE_MODULE_PATH)
    if spec is None or spec.loader is None:
        raise RuntimeError(f"unable to load prepare module: {PREPARE_MODULE_PATH}")

    module = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(module)
    return module


class PrepareScopeTests(unittest.TestCase):
    def test_prepare_scope_resolution_covers_global_and_targeted_forms(self) -> None:
        prepare_module = load_prepare_module()

        self.assertEqual("global", prepare_module.resolve_prepare_scope("prepare"))
        self.assertEqual("smoke", prepare_module.resolve_prepare_scope("prepare-smoke"))
        self.assertEqual(
            "verify-roadmap0-windows",
            prepare_module.resolve_prepare_scope("prepare-verify-roadmap-0-windows"),
        )
        self.assertEqual(
            "verify-roadmap0-macos",
            prepare_module.resolve_prepare_scope("prepare-verify-roadmap-0-macos"),
        )

    def test_clean_scope_paths_only_cover_managed_outputs(self) -> None:
        prepare_module = load_prepare_module()
        clean_paths = prepare_module.resolve_clean_paths(REPO_ROOT, "verify-roadmap0-windows")

        self.assertIn(
            REPO_ROOT / "artifacts" / "verify-roadmap-0" / "windows",
            clean_paths,
        )
        self.assertIn(
            REPO_ROOT / "artifacts" / "run" / "prepare" / "verify-roadmap0-windows.json",
            clean_paths,
        )
        self.assertNotIn(REPO_ROOT / "contracts", clean_paths)
        self.assertNotIn(REPO_ROOT / "third_party", clean_paths)


if __name__ == "__main__":
    unittest.main()
