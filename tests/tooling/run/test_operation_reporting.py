from __future__ import annotations

import importlib.util
import json
import shutil
import sys
import unittest
import uuid
from pathlib import Path
from unittest.mock import patch


REPO_ROOT = Path(__file__).resolve().parents[3]
OPERATION_REPORTING_MODULE_PATH = REPO_ROOT / "build" / "toolchains" / "run" / "core" / "operation_reporting.py"
EVENTS_MODULE_PATH = REPO_ROOT / "build" / "toolchains" / "run" / "testing" / "events.py"
TEST_TMP_ROOT = REPO_ROOT / "artifacts" / ".tmp_test_operation_reporting"


def load_module(path: Path, module_name: str):
    if not path.is_file():
        raise FileNotFoundError(f"module missing: {path}")

    sys.path.insert(0, str(path.parent))
    spec = importlib.util.spec_from_file_location(module_name, path)
    if spec is None or spec.loader is None:
        raise RuntimeError(f"unable to load module: {path}")

    module = importlib.util.module_from_spec(spec)
    sys.modules[spec.name] = module
    spec.loader.exec_module(module)
    return module


class OperationReportingTests(unittest.TestCase):
    def test_append_operation_event_backfills_missing_run_id(self) -> None:
        operation_reporting_module = load_module(OPERATION_REPORTING_MODULE_PATH, "chaos_run_operation_reporting")
        events_module = load_module(EVENTS_MODULE_PATH, "chaos_run_operation_events")

        TEST_TMP_ROOT.mkdir(parents=True, exist_ok=True)
        repo_root = TEST_TMP_ROOT / f"repo-{uuid.uuid4().hex}"
        repo_root.mkdir(parents=True, exist_ok=False)
        try:
            run_context = operation_reporting_module.start_operation_report(
                repo_root=repo_root,
                host_platform="windows",
                command_text="build all",
            )
            event = events_module.build_event(
                "progress",
                {
                    "completedUnits": 1,
                    "totalUnits": 5,
                    "activeUnit": "build abi",
                    "suiteStatus": "ok",
                },
                status="ok",
            )

            operation_reporting_module.append_operation_event(run_context, event)

            events_path = Path(run_context["eventsPath"])
            written = [json.loads(line) for line in events_path.read_text(encoding="utf-8").splitlines() if line.strip()]
        finally:
            shutil.rmtree(repo_root, ignore_errors=True)

        self.assertEqual(1, len(written))
        self.assertEqual(run_context["runId"], written[0]["runId"])
        self.assertEqual("build abi", written[0]["payload"]["activeUnit"])
        self.assertEqual("ok", written[0]["status"])

    def test_finalize_operation_report_rewrites_current_pointer_when_delete_is_denied(self) -> None:
        operation_reporting_module = load_module(OPERATION_REPORTING_MODULE_PATH, "chaos_run_operation_reporting_finalize")

        TEST_TMP_ROOT.mkdir(parents=True, exist_ok=True)
        repo_root = TEST_TMP_ROOT / f"repo-{uuid.uuid4().hex}"
        repo_root.mkdir(parents=True, exist_ok=False)
        try:
            run_context = operation_reporting_module.start_operation_report(
                repo_root=repo_root,
                host_platform="windows",
                command_text="build all",
            )
            with patch("pathlib.Path.unlink", side_effect=PermissionError(13, "Access is denied")):
                operation_reporting_module.finalize_operation_report(
                    repo_root=repo_root,
                    host_platform="windows",
                    command_text="build all",
                    status="ok",
                    errors=[],
                    artifacts=["artifacts/output.bin"],
                    important_outputs=[],
                    console_text="build ok\n",
                    run_context=run_context,
                )

            current_payload = json.loads(
                (repo_root / "artifacts" / "logs" / "run" / "current.json").read_text(encoding="utf-8")
            )
        finally:
            shutil.rmtree(repo_root, ignore_errors=True)

        self.assertEqual("ok", current_payload["status"])
        self.assertEqual(run_context["runId"], current_payload["runId"])


if __name__ == "__main__":
    unittest.main()
