from __future__ import annotations

import importlib.util
import json
import unittest
from pathlib import Path

from tests.support import load_public_specs_module, select_public_suite_spec


REPO_ROOT = Path(__file__).resolve().parents[3]
MANIFEST_MODULE_PATH = REPO_ROOT / "build" / "toolchains" / "run" / "core" / "manifest.py"
RUN_MANIFEST_PATH = REPO_ROOT / "build" / "toolchains" / "run" / "run_manifest.json"


def load_manifest_module():
    if not MANIFEST_MODULE_PATH.is_file():
        raise FileNotFoundError(f"manifest module missing: {MANIFEST_MODULE_PATH}")

    spec = importlib.util.spec_from_file_location("chaos_run_manifest", MANIFEST_MODULE_PATH)
    if spec is None or spec.loader is None:
        raise RuntimeError(f"unable to load manifest module: {MANIFEST_MODULE_PATH}")

    module = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(module)
    return module


class CommandManifestTestSupport(unittest.TestCase):
    pass
