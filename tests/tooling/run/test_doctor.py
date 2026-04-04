from __future__ import annotations

import importlib.util
import os
import sys
import unittest
from pathlib import Path
from unittest.mock import patch


REPO_ROOT = Path(__file__).resolve().parents[3]
DOCTOR_MODULE_PATH = REPO_ROOT / "build" / "toolchains" / "run" / "commands" / "doctor.py"


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


class DoctorCommandTests(unittest.TestCase):
    def test_doctor_reports_osc8_support_when_terminal_looks_capable(self) -> None:
        doctor_module = load_module(DOCTOR_MODULE_PATH, "booming_run_doctor_osc8_supported")

        with patch.object(
            doctor_module.runtime_module,
            "probe_runtime",
            return_value={"isInstalled": True, "pythonPath": "artifacts/python/bin/python3"},
        ):
            with patch.object(doctor_module.shutil, "which", side_effect=lambda exe: f"/usr/bin/{exe}"):
                with patch.dict(
                    os.environ,
                    {
                        "TERM_PROGRAM": "iTerm.app",
                        "TERM": "xterm-256color",
                    },
                    clear=False,
                ):
                    result = doctor_module.handle(REPO_ROOT, "macos", "doctor")

        self.assertEqual("ok", result.status)
        osc8_check = next(check for check in result.checks if check["name"] == "osc8-hyperlinks")
        self.assertEqual("ok", osc8_check["status"])
        self.assertIn("supported", osc8_check["detail"])
        self.assertIn("OSC 8 hyperlinks", result.text or "")
        self.assertIn("Current terminal looks compatible", result.text or "")

    def test_doctor_explains_that_osc8_is_not_installable_when_support_is_unknown(self) -> None:
        doctor_module = load_module(DOCTOR_MODULE_PATH, "booming_run_doctor_osc8_unknown")

        with patch.object(
            doctor_module.runtime_module,
            "probe_runtime",
            return_value={"isInstalled": True, "pythonPath": "artifacts/python/bin/python3"},
        ):
            with patch.object(doctor_module.shutil, "which", side_effect=lambda exe: f"/usr/bin/{exe}"):
                with patch.dict(
                    os.environ,
                    {
                        "TERM_PROGRAM": "Apple_Terminal",
                        "TERM": "xterm-256color",
                    },
                    clear=False,
                ):
                    result = doctor_module.handle(REPO_ROOT, "macos", "doctor")

        self.assertEqual("ok", result.status)
        osc8_check = next(check for check in result.checks if check["name"] == "osc8-hyperlinks")
        self.assertEqual("missing", osc8_check["status"])
        self.assertIn("not installable", osc8_check["detail"])
        self.assertIn("cannot be installed by run doctor", result.text or "")
        self.assertIn("Recommended terminals", result.text or "")
        self.assertIn("iTerm2", result.text or "")
        self.assertIn("WezTerm", result.text or "")
        self.assertIn("Apple_Terminal", result.text or "")


if __name__ == "__main__":
    unittest.main()
