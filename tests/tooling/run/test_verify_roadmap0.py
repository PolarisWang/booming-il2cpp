from __future__ import annotations

import importlib.util
import subprocess
import sys
import unittest
from pathlib import Path
from unittest.mock import patch


REPO_ROOT = Path(__file__).resolve().parents[3]
VERIFY_MODULE_PATH = REPO_ROOT / "build" / "toolchains" / "run" / "commands" / "verify.py"
VERIFY_WRAPPER_PATH = REPO_ROOT / "build" / "scripts" / "verify-roadmap-0.sh"


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


class VerifyRoadmap0Tests(unittest.TestCase):
    def test_posix_verify_wrapper_runs_without_pwsh(self) -> None:
        completed = subprocess.run(
            ["sh", str(VERIFY_WRAPPER_PATH), "--help"],
            cwd=REPO_ROOT,
            capture_output=True,
            text=True,
        )

        self.assertEqual(0, completed.returncode, msg=completed.stdout + completed.stderr)
        self.assertIn("host-profile", completed.stdout)
        self.assertNotIn("pwsh", completed.stderr)

    def test_verify_command_uses_python_entrypoint_on_macos(self) -> None:
        verify_module = load_module(VERIFY_MODULE_PATH, "booming_run_verify_route")
        bootstrap = verify_module.tooling_module.ToolBootstrapResult(
            ready=True,
            output="",
            errors=[],
        )
        completed = subprocess.CompletedProcess(["python3"], 0, stdout="verify ok", stderr="")

        with patch.object(verify_module.tooling_module, "ensure_dotnet_available", return_value=bootstrap):
            with patch.object(verify_module, "run_process", return_value=completed) as run_process_mock:
                result = verify_module.handle(
                    {"id": "verify-roadmap-0-macos", "host_profile": "macos", "target": "macos"},
                    REPO_ROOT,
                    "macos",
                    "verify roadmap-0 --host macos",
                )

        args = run_process_mock.call_args.args[0]
        self.assertEqual("ok", result.status)
        self.assertEqual(sys.executable, args[0])
        self.assertTrue(str(args[1]).endswith("build/scripts/verify-roadmap-0.py"))


if __name__ == "__main__":
    unittest.main()
