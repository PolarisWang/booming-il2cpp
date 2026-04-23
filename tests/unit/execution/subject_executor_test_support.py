from __future__ import annotations

import json
import shutil
import unittest
import uuid
from pathlib import Path

from tests.support import load_module

REPO_ROOT = Path(__file__).resolve().parents[3]
EXECUTOR_MODULE_PATH = REPO_ROOT / "build" / "toolchains" / "run" / "testing" / "subject_executor.py"
TEST_TMP_ROOT = REPO_ROOT / "artifacts" / ".tmp_test_subject_executor"
TRACE_SUBJECT_ID = "FixtureTraceSubject"
TRACE_MATRIX_ID = "windows-reference-trace"
TRACE_GOAL_ID = "correctness.platform"
PERF_SUBJECT_ID = "FixturePerfSubject"
PERF_MATRIX_ID = "windows-perf-release"
PERF_GOAL_ID = "perf.release"

def run_bucket_path(subject_id: str, run_id: str, *parts: str) -> str:
    return Path("artifacts", "subjects", subject_id, "runs", run_id, *parts).as_posix()


def source_project_path(subject_id: str) -> str:
    return Path("subjects", subject_id, "source", f"{subject_id}.csproj").as_posix()


class SubjectExecutorTestSupport(unittest.TestCase):
    pass
