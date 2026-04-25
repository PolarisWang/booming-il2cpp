from __future__ import annotations

import importlib.util
import json
import shutil
import subprocess
import sys
import unittest
import uuid
from pathlib import Path
from unittest.mock import patch


REPO_ROOT = Path(__file__).resolve().parents[3]
SUBJECT_WORKERS_MODULE_PATH = REPO_ROOT / "build" / "toolchains" / "run" / "testing" / "subject_workers.py"
MOBILE_PERF_COLLECTOR_MODULE_PATH = REPO_ROOT / "build" / "toolchains" / "run" / "testing" / "mobile_perf_collector.py"
TEST_TMP_ROOT = REPO_ROOT / "artifacts" / ".tmp-tests" / "subject-workers-perf"


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


def owner_manifest_path(subject_id: str) -> Path:
    return Path("verification") / "catalog" / "owners" / subject_id / "owner.manifest.json"


def write_owner_manifest(repo_root: Path, subject_id: str, payload: dict) -> Path:
    manifest_path = repo_root / owner_manifest_path(subject_id)
    manifest_path.parent.mkdir(parents=True, exist_ok=True)
    manifest_path.write_text(json.dumps(payload), encoding="utf-8")
    features_path = manifest_path.parent / "owner.features.json"
    features_path.write_text(
        json.dumps(
            {
                "subjectId": subject_id,
                "features": [],
            }
        ),
        encoding="utf-8",
    )
    return manifest_path

class SubjectWorkersPerfTestSupport(unittest.TestCase):
    @classmethod
    def setUpClass(cls) -> None:
        TEST_TMP_ROOT.mkdir(parents=True, exist_ok=True)

    def _make_repo_root(self, prefix: str) -> Path:
        repo_root = TEST_TMP_ROOT / f"{prefix}-{uuid.uuid4().hex}"
        repo_root.mkdir(parents=True, exist_ok=False)
        return repo_root

    def _make_non_repo_path(self, *parts: str) -> Path:
        return TEST_TMP_ROOT / "_external" / Path(*parts)
