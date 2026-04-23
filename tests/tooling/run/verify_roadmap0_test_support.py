from __future__ import annotations

import importlib.util
import shutil
import subprocess
import sys
import tempfile
import unittest
import uuid
from pathlib import Path
from unittest.mock import patch

from tests.support import select_public_suite_spec


REPO_ROOT = Path(__file__).resolve().parents[3]
VERIFY_MODULE_PATH = REPO_ROOT / "build" / "toolchains" / "run" / "commands" / "verify.py"
VERIFY_WRAPPER_PATH = REPO_ROOT / "build" / "scripts" / "verify-runtime-baseline.sh"
VERIFY_SCRIPT_PATH = REPO_ROOT / "build" / "scripts" / "verify-runtime-baseline.py"
VERIFY_SCRIPT_PS1_PATH = REPO_ROOT / "build" / "scripts" / "verify-runtime-baseline.ps1"
LEGACY_VERIFY_WRAPPER_PATH = REPO_ROOT / "build" / "scripts" / "verify-roadmap-0.sh"
LEGACY_VERIFY_SCRIPT_PS1_PATH = REPO_ROOT / "build" / "scripts" / "verify-roadmap-0.ps1"
TEST_TMP_ROOT = REPO_ROOT / "artifacts" / ".tmp-tests" / "verify-roadmap0"


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


class VerifyRoadmap0TestSupport(unittest.TestCase):
    @classmethod
    def setUpClass(cls) -> None:
        TEST_TMP_ROOT.mkdir(parents=True, exist_ok=True)

    def _make_test_dir(self, prefix: str) -> Path:
        path = TEST_TMP_ROOT / f"{prefix}-{uuid.uuid4().hex}"
        path.mkdir(parents=True, exist_ok=False)
        return path
