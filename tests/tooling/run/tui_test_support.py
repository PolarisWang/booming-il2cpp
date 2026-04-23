from __future__ import annotations

import importlib.util
import re
import sys
import unittest
from pathlib import Path
from unittest.mock import patch

from tests.support import select_public_suite_spec

from .command_manifest_test_support import RUN_MANIFEST_PATH, load_manifest_module


REPO_ROOT = Path(__file__).resolve().parents[3]
TUI_MODULE_PATH = REPO_ROOT / "build" / "toolchains" / "run" / "tui.py"


def load_tui_module():
    if not TUI_MODULE_PATH.is_file():
        raise FileNotFoundError(f"tui module missing: {TUI_MODULE_PATH}")

    spec = importlib.util.spec_from_file_location("chaos_run_tui", TUI_MODULE_PATH)
    if spec is None or spec.loader is None:
        raise RuntimeError(f"unable to load tui module: {TUI_MODULE_PATH}")

    module = importlib.util.module_from_spec(spec)
    sys.modules[spec.name] = module
    spec.loader.exec_module(module)
    return module


def smoke_binary_artifact_path(suite_name: str) -> str:
    return Path("artifacts", "smoke", "bin", suite_name, "Release", "net8.0", f"{suite_name}.dll").as_posix()


def make_project_subject_record(
    subject_id: str = "FixtureProjectSubject",
    *,
    matrix_id: str = "windows-native-check",
) -> dict[str, object]:
    return {
        "subject_id": subject_id,
        "title": subject_id,
        "manifest": {"defaultMatrix": matrix_id},
        "matrices": [
            {
                "matrixId": matrix_id,
                "executionContext": {
                    "hostPlatform": "windows-x64",
                    "targetPlatform": "windows-x64",
                },
            }
        ],
    }


def make_subject_registry_index(tui_module, subject_id: str = "FixtureSubject"):
    return tui_module.registry_module.RegistryIndex(
        host_platform="windows",
        suites=[],
        subjects=[
            {
                "id": f"subject/{subject_id}",
                "type": "subject",
                "displayName": subject_id,
                "subjectId": subject_id,
                "defaultGoalId": "correctness.dev",
                "defaultMatrixId": "windows-native-check",
                "goalIds": ["correctness.dev"],
                "matrixIds": ["windows-native-check"],
                "supportedHosts": ["windows"],
                "level": "subject",
                "primaryModuleId": None,
                "moduleIds": [],
                "subsystemIds": [],
                "docRefs": [],
            }
        ],
        module_verifications=[],
        system_scenarios=[],
        errors=[],
        warnings=[],
    )

@unittest.skip("legacy assertions superseded by unified test menu coverage")
class LegacyTuiTestSupport(unittest.TestCase):
    pass


class TuiUnifiedMenuTestSupport(unittest.TestCase):
    pass
