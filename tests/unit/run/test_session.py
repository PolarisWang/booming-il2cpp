from __future__ import annotations

import importlib.util
import sys
import unittest
from pathlib import Path


REPO_ROOT = Path(__file__).resolve().parents[3]
SESSION_MODULE_PATH = REPO_ROOT / "build" / "toolchains" / "run" / "testing" / "session.py"
RESULT_MODULE_PATH = REPO_ROOT / "build" / "toolchains" / "run" / "result.py"


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


class SessionTests(unittest.TestCase):
    def test_stage_run_marks_implicit_build_when_build_cache_missing(self) -> None:
        session_module = load_module(SESSION_MODULE_PATH, "booming_run_session")
        result_module = load_module(RESULT_MODULE_PATH, "booming_run_result_for_session")
        legacy_result = result_module.CommandResult.success(
            command="test smoke HelloWorld --stage run",
            host_platform="macos",
            target="HelloWorld",
            payload={"artifacts": ["artifacts/smoke/bin/HelloWorld/Release/net8.0/HelloWorld.dll"]},
            text="managed smoke ok\n",
        )

        request = session_module.TestRequest(
            family="smoke",
            suite="HelloWorld",
            stage="run",
            command_text="test smoke HelloWorld --stage run",
        )

        session_result = session_module.execute_suite_session(
            request,
            repo_root=REPO_ROOT,
            host_platform="macos",
            suite_spec={
                "id": "smoke/HelloWorld",
                "family": "smoke",
                "suite": "HelloWorld",
                "stages": ["all", "build", "run"],
                "supported_hosts": ["windows", "macos", "linux"],
                "legacy_commands": {"build": "build-smoke-helloworld", "run": "test-smoke-helloworld", "all": "test-smoke-helloworld"},
            },
            legacy_command={"id": "test-smoke-helloworld", "target": "HelloWorld"},
            legacy_executor=lambda command, repo_root, host_platform, command_text: legacy_result,
        )

        self.assertEqual(0, session_result.exit_code)
        self.assertEqual("ok", session_result.status)
        self.assertEqual("ok", session_result.suite_results[0]["stageResults"]["build"]["status"])
        self.assertTrue(session_result.suite_results[0]["stageResults"]["build"]["implicit"])
        self.assertEqual("ok", session_result.suite_results[0]["stageResults"]["run"]["status"])


if __name__ == "__main__":
    unittest.main()
