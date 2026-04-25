from __future__ import annotations

import json
import shutil
import unittest
import uuid
from pathlib import Path

from tests.support import SOLUTION_CORE_PACK_HOST_SOLUTION_PATH, load_module, write_json

REPO_ROOT = Path(__file__).resolve().parents[3]
SUBJECT_REPORTING_MODULE_PATH = REPO_ROOT / "build" / "toolchains" / "run" / "testing" / "subject_reporting.py"
TEST_TMP_ROOT = REPO_ROOT / "artifacts" / ".tmp-tests" / "subject-reporting"
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


def source_entry(subject_id: str) -> str:
    return f"{subject_id}/Program::Main(System.String[])"


def perf_baseline_path(subject_id: str, matrix_id: str) -> str:
    return Path("subjects", subject_id, "baselines", "perf", matrix_id, "windows.json").as_posix()


class SubjectReportingTestSupport(unittest.TestCase):
    @classmethod
    def setUpClass(cls) -> None:
        TEST_TMP_ROOT.mkdir(parents=True, exist_ok=True)
