from __future__ import annotations

import importlib.util
import json
import shutil
import subprocess
import sys
import unittest
import uuid
from pathlib import Path

from tests.support import (
    HOT_UPDATE_HOST_PACK_HOST_PROJECT_PATH,
    HOT_UPDATE_SKELETON_PROOF_PATH,
)


REPO_ROOT = Path(__file__).resolve().parents[3]
HOT_UPDATE_ROOT = REPO_ROOT / "src" / "managed" / "Chaos.IL2CPP.HotUpdate"

PACKAGE_VALIDATOR_PATH = HOT_UPDATE_ROOT / "PackageValidator.cs"
RUNTIME_MANAGER_PATH = HOT_UPDATE_ROOT / "RuntimeManager.cs"
VERSION_HELPER_PATH = HOT_UPDATE_ROOT / "HotUpdateVersionCompatibility.cs"

IOS_POLICY_PATH = REPO_ROOT / "docs" / "architecture" / "ios-distribution-policy.md"
RELEASE_CHECKLIST_PATH = REPO_ROOT / "docs" / "architecture" / "release-checklist.md"
VERSION_MATRIX_PATH = REPO_ROOT / "docs" / "architecture" / "version-compatibility-matrix.md"

HOT_UPDATE_SKELETON_PROJECT_PATH = HOT_UPDATE_HOST_PACK_HOST_PROJECT_PATH
COMPATIBILITY_MATRIX_RUNNER_PATH = (
    REPO_ROOT / "build" / "toolchains" / "run" / "testing" / "compatibility_matrix_runner.py"
)
COMPATIBILITY_MATRIX_CONFIG_PATH = (
    REPO_ROOT / "tests" / "fixtures" / "subjects" / "CompatibilityMatrixProof" / "compatibility-matrix.json"
)
COMPATIBILITY_MATRIX_SUBJECT_MANIFEST_PATH = (
    REPO_ROOT / "tests" / "fixtures" / "subjects" / "CompatibilityMatrixProof" / "subject.manifest.json"
)
COMPATIBILITY_MATRIX_TMP_ROOT = REPO_ROOT / "artifacts" / ".tmp-tests" / "compatibility-matrix"
PERF_DASHBOARD_MODULE_PATH = REPO_ROOT / "build" / "toolchains" / "run" / "testing" / "perf_dashboard.py"
UNSUPPORTED_FEATURE_REPORT_MODULE_PATH = (
    REPO_ROOT / "build" / "toolchains" / "run" / "testing" / "unsupported_feature_report.py"
)
SOAK_HARNESS_MODULE_PATH = REPO_ROOT / "build" / "toolchains" / "run" / "testing" / "soak_harness.py"
BATCH4_TMP_ROOT = REPO_ROOT / "artifacts" / ".tmp-tests" / "hot-update-productization-gates"


def run_checked(arguments: list[str], *, cwd: Path) -> subprocess.CompletedProcess[str]:
    completed = subprocess.run(
        arguments,
        cwd=cwd,
        capture_output=True,
        text=True,
        encoding="utf-8",
        errors="replace",
        check=False,
    )
    if completed.returncode != 0:
        combined_output = "\n".join(part for part in [completed.stdout, completed.stderr] if part)
        raise AssertionError(f"command failed ({completed.returncode}): {' '.join(arguments)}\n{combined_output}")
    return completed


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

class ProductizationGatesTestSupport(unittest.TestCase):
    pass
