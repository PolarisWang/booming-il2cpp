from __future__ import annotations

import unittest
from pathlib import Path
from unittest.mock import patch

from tests.support import load_module, load_public_specs_module, select_public_suite_spec, select_registry_item


REPO_ROOT = Path(__file__).resolve().parents[3]
TEST_COMMAND_MODULE_PATH = REPO_ROOT / "build" / "toolchains" / "run" / "commands" / "test.py"
MANIFEST_MODULE_PATH = REPO_ROOT / "build" / "toolchains" / "run" / "core" / "manifest.py"
SESSION_MODULE_PATH = REPO_ROOT / "build" / "toolchains" / "run" / "testing" / "session.py"
RUN_MANIFEST_PATH = REPO_ROOT / "build" / "toolchains" / "run" / "run_manifest.json"


class RegistryCommandTestSupport(unittest.TestCase):
    pass
