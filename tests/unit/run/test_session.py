from __future__ import annotations

import importlib.util
import sys
import unittest
from pathlib import Path

from tests.support import select_public_suite_spec


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
        session_module = load_module(SESSION_MODULE_PATH, "chaos_run_session")
        result_module = load_module(RESULT_MODULE_PATH, "chaos_run_result_for_session")
        suite_spec = select_public_suite_spec(
            "chaos_session_smoke_suite",
            host_platform="macos",
            family="smoke",
            required_stages=["build", "run", "all"],
        )
        suite_name = str(suite_spec["suite"])
        suite_id = str(suite_spec["id"])
        legacy_commands = dict(suite_spec.get("legacy_commands") or {})
        artifact_path = Path("artifacts", "smoke", "bin", suite_name, "Release", "net8.0", f"{suite_name}.dll").as_posix()
        legacy_result = result_module.CommandResult.success(
            command=f"test {suite_spec['family']} {suite_name} --stage run",
            host_platform="macos",
            target=suite_name,
            payload={"artifacts": [artifact_path]},
            text="managed smoke ok\n",
        )

        request = session_module.TestRequest(
            family=str(suite_spec["family"]),
            suite=suite_name,
            stage="run",
            command_text=f"test {suite_spec['family']} {suite_name} --stage run",
        )

        session_result = session_module.execute_suite_session(
            request,
            repo_root=REPO_ROOT,
            host_platform="macos",
            suite_spec={
                "id": suite_id,
                "family": str(suite_spec["family"]),
                "suite": suite_name,
                "stages": list(suite_spec["stages"]),
                "supported_hosts": list(suite_spec["supported_hosts"]),
                "legacy_commands": legacy_commands,
            },
            legacy_command={"id": str(legacy_commands["run"]), "target": suite_name},
            legacy_executor=lambda command, repo_root, host_platform, command_text: legacy_result,
        )

        self.assertEqual(0, session_result.exit_code)
        self.assertEqual("ok", session_result.status)
        self.assertEqual("ok", session_result.suite_results[0]["stageResults"]["build"]["status"])
        self.assertTrue(session_result.suite_results[0]["stageResults"]["build"]["implicit"])
        self.assertEqual("ok", session_result.suite_results[0]["stageResults"]["run"]["status"])


if __name__ == "__main__":
    unittest.main()
