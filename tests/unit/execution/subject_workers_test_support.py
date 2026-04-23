from __future__ import annotations

import importlib.util
import json
import os
import shutil
import subprocess
import sys
import time
import unittest
import uuid
from pathlib import Path
from unittest.mock import patch


REPO_ROOT = Path(__file__).resolve().parents[3]
SUBJECT_WORKERS_MODULE_PATH = REPO_ROOT / "build" / "toolchains" / "run" / "testing" / "subject_workers.py"
TEST_TMP_ROOT = REPO_ROOT / "artifacts" / ".tmp-tests" / "subject-workers"
WINDOWS_REFERENCE_BUILD_TARGET = "chaos_subject_reference_proof"
WINDOWS_REFERENCE_RUN_TARGET = "chaos_subject_reference_proof_run"
WINDOWS_REFERENCE_CMAKE_BUILD_STRATEGY = "windows-reference-cmake"
WINDOWS_NATIVE_AOT_BUILD_TARGET = "chaos_subject_native_aot"
SHARED_RUNTIME_PROJECT_PATH = "src/reference/Chaos.TestFramework.Runtime/Chaos.TestFramework.Runtime.csproj"
SHARED_RUNTIME_ASSEMBLY_NAME = "Chaos.TestFramework.Runtime"


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


def posix_path(*parts: str) -> str:
    return Path(*parts).as_posix()


def subject_run_path(subject_id: str, run_id: str, *parts: str) -> str:
    return posix_path("artifacts", "subjects", subject_id, "runs", run_id, *parts)


def subject_source_path(subject_id: str) -> str:
    return posix_path("subjects", subject_id, "source", f"{subject_id}.csproj")

class SubjectWorkersTestSupport(unittest.TestCase):
    @classmethod
    def setUpClass(cls) -> None:
        TEST_TMP_ROOT.mkdir(parents=True, exist_ok=True)

    def _make_repo_root(self, prefix: str) -> Path:
        repo_root = TEST_TMP_ROOT / f"{prefix}-{uuid.uuid4().hex}"
        repo_root.mkdir(parents=True, exist_ok=False)
        return repo_root

    def _make_non_repo_path(self, *parts: str) -> Path:
        return TEST_TMP_ROOT / "_external" / Path(*parts)
