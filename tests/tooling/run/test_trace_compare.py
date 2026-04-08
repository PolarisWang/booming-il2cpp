from __future__ import annotations

import base64
import importlib.util
import json
import shutil
import subprocess
import sys
import tempfile
import unittest
from pathlib import Path
from unittest.mock import patch


REPO_ROOT = Path(__file__).resolve().parents[3]
TEST_MODULE_PATH = REPO_ROOT / "build" / "toolchains" / "run" / "commands" / "test.py"
COMPARE_WRAPPER_PATH = REPO_ROOT / "tests" / "contracts" / "trace" / "compare-warmup-trace.sh"
EXPECTED_TRACE_PATH = REPO_ROOT / "tests" / "contracts" / "trace" / "snapshots" / "macos-warmup-trace.snapshot.json"


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


def _warmup_session_marker(sample_id: str, event: dict[str, object]) -> str:
    encoded_subject_id = base64.b64encode(str(event["subjectId"]).encode("utf-8")).decode("ascii")
    return (
        f"warmup-trace|sampleId={sample_id}|order={event['order']}|eventName={event['eventName']}"
        f"|phase={event['phase']}|subjectKind={event['subjectKind']}|subjectId={encoded_subject_id}|status={event['status']}"
    )


def _build_runtime_trace(snapshot_path: Path, actual_path: Path) -> None:
    snapshot = json.loads(snapshot_path.read_text(encoding="utf-8"))
    session_trace = [
        "host:start",
        "registration:assemblies",
        "registration:managed-exports",
        "registration:callbacks",
        "managed-entry:begin",
        "managed-entry:end",
    ]
    for sample in snapshot["samples"]:
        for event in sample["events"]:
            session_trace.append(_warmup_session_marker(str(sample["sampleId"]), event))

    runtime_trace = dict(snapshot)
    runtime_trace["traceSource"] = "host-embedding-session"
    runtime_trace["sessionTrace"] = session_trace
    actual_path.write_text(json.dumps(runtime_trace, indent=2), encoding="utf-8")


class TraceCompareTests(unittest.TestCase):
    def test_posix_compare_wrapper_runs_without_pwsh(self) -> None:
        if shutil.which("sh") is None:
            self.skipTest("POSIX shell is not available in this environment")

        with tempfile.TemporaryDirectory() as temp_dir:
            actual_path = Path(temp_dir) / "macos-warmup-trace.runtime.json"
            _build_runtime_trace(EXPECTED_TRACE_PATH, actual_path)

            completed = subprocess.run(
                ["sh", str(COMPARE_WRAPPER_PATH), str(EXPECTED_TRACE_PATH), str(actual_path)],
                cwd=REPO_ROOT,
                capture_output=True,
                text=True,
            )

        self.assertEqual(0, completed.returncode, msg=completed.stdout + completed.stderr)
        self.assertIn("Warmup trace compare passed", completed.stdout)

    def test_trace_compare_command_uses_subject_artifacts_on_macos(self) -> None:
        test_module = load_module(TEST_MODULE_PATH, "chaos_run_trace_compare")
        bootstrap = test_module.tooling_module.ToolBootstrapResult(
            ready=True,
            output="",
            errors=[],
        )
        run_id = "20260408-macos-0001"
        command = {
            "id": "test-trace-compare-macos",
            "kind": "trace-compare",
            "subject_id": "FixtureTraceSubject",
            "goal_id": "correctness.platform",
            "matrix_id": "macos-managed-trace",
            "target": "macos",
        }
        execution_result = {
            "subjectId": "FixtureTraceSubject",
            "matrixId": "macos-managed-trace",
            "goalId": "correctness.platform",
            "status": "ok",
            "stageResults": [],
            "errors": [],
        }
        trace_output = REPO_ROOT / "artifacts" / "subjects" / "FixtureTraceSubject" / "runs" / run_id / "matrices" / "macos-managed-trace" / "runtime" / "trace.runtime.json"

        with patch.object(test_module.tooling_module, "ensure_dotnet_available", return_value=bootstrap):
            with patch.object(test_module.reporting_module, "build_run_id", return_value=run_id):
                with patch.object(test_module.subject_executor_module, "execute_subject_matrix", return_value=execution_result) as execute_subject_matrix_mock:
                    with patch.object(
                        test_module.subject_executor_module,
                        "trace_paths_from_execution",
                        return_value=["artifacts/subjects/FixtureTraceSubject/runs/20260408-macos-0001/matrices/macos-managed-trace/runtime/trace.runtime.json"],
                    ) as trace_paths_mock:
                        with patch.object(test_module, "run_process") as run_process_mock:
                            result = test_module.handle(command, REPO_ROOT, "macos", "test contract trace-compare-macos")

        self.assertEqual("ok", result.status)
        execute_subject_matrix_mock.assert_called_once_with(
            REPO_ROOT,
            "FixtureTraceSubject",
            goal_id="correctness.platform",
            matrix_id="macos-managed-trace",
            run_id=run_id,
        )
        trace_paths_mock.assert_called_once_with(REPO_ROOT, execution_result)
        run_process_mock.assert_not_called()
        self.assertEqual([str(trace_output)], result.payload["artifacts"])


if __name__ == "__main__":
    unittest.main()
