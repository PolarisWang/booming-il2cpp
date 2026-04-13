from __future__ import annotations

import importlib.util
import shutil
import sys
import tempfile
import unittest
from pathlib import Path
from unittest.mock import patch


REPO_ROOT = Path(__file__).resolve().parents[3]
CATALOG_MODULE_PATH = REPO_ROOT / "build" / "toolchains" / "run" / "testing" / "catalog.py"
FIXTURE_REPO_ROOT = REPO_ROOT / "tests" / "fixtures" / "integration" / "catalog" / "repo"


def load_catalog_module():
    if not CATALOG_MODULE_PATH.is_file():
        raise FileNotFoundError(f"catalog module missing: {CATALOG_MODULE_PATH}")

    spec = importlib.util.spec_from_file_location("chaos_run_catalog", CATALOG_MODULE_PATH)
    if spec is None or spec.loader is None:
        raise RuntimeError(f"unable to load catalog module: {CATALOG_MODULE_PATH}")

    module = importlib.util.module_from_spec(spec)
    sys.modules[spec.name] = module
    spec.loader.exec_module(module)
    return module


class CatalogScanTests(unittest.TestCase):
    def test_catalog_lists_valid_and_invalid_suites_without_failing_whole_scan(self) -> None:
        catalog_module = load_catalog_module()

        with tempfile.TemporaryDirectory() as temp_dir:
            repo_root = Path(temp_dir) / "repo"
            shutil.copytree(FIXTURE_REPO_ROOT, repo_root)

            catalog = catalog_module.scan_catalog(repo_root, host_platform="macos", strict=False)

        self.assertEqual(["smoke/hello-world"], catalog.valid_suite_ids)
        self.assertEqual(["contract/bad-manifest"], catalog.invalid_suite_ids)
        self.assertEqual(["contract", "smoke"], catalog.family_names)

    def test_catalog_family_names_match_new_public_families_in_repo_root(self) -> None:
        catalog_module = load_catalog_module()

        with patch.object(catalog_module, "write_json"):
            catalog = catalog_module.scan_catalog(REPO_ROOT, host_platform="macos", strict=False)

        self.assertIn("integration", catalog.family_names)
        self.assertNotIn("contracts", catalog.family_names)


if __name__ == "__main__":
    unittest.main()
