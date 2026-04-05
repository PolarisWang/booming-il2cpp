from __future__ import annotations

import importlib.util
import sys
import unittest
from pathlib import Path
from unittest.mock import patch


REPO_ROOT = Path(__file__).resolve().parents[3]
BUILD_MODULE_PATH = REPO_ROOT / "build" / "toolchains" / "run" / "commands" / "build.py"


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


class BuildBatchTests(unittest.TestCase):
    def test_host_build_plan_uses_canonical_contract_paths(self) -> None:
        build_module = load_module(BUILD_MODULE_PATH, "booming_run_build_batch_plan")

        for host_platform in ("windows", "macos", "linux"):
            plan = build_module._host_build_plan(host_platform)
            native_steps = [step for step in plan if step["kind"] == "native-contract"]

            self.assertEqual(
                [
                    "tests/contracts/native/abi",
                    "tests/contracts/native/bridge",
                ],
                [step["source_dir"] for step in native_steps],
            )

    def test_build_all_executes_windows_recommended_plan(self) -> None:
        build_module = load_module(BUILD_MODULE_PATH, "booming_run_build_batch")
        calls: list[str] = []

        def fake_handle(step, repo_root, host_platform, command_text, progress_callback=None):
            del repo_root, progress_callback
            calls.append(f"{host_platform}:{step['target']}")
            return build_module.CommandResult.success(
                command=command_text,
                host_platform=host_platform,
                target=step["target"],
                payload={"artifacts": [step["target"]], "consoleText": command_text},
                text=f"Run completed: {command_text}\n",
            )

        with patch.object(build_module, "_host_build_plan", return_value=[
            {"kind": "native-contract", "target": "abi", "source_dir": "tests/contracts/native/abi", "binary_dir": "artifacts/run/native-contract-abi"},
            {"kind": "preset", "target": "windows-x64-reference", "preset": "windows-x64-reference", "binary_dir": "artifacts/presets/windows-x64-reference"},
        ]):
            with patch.object(build_module, "handle", side_effect=fake_handle):
                result = build_module._build_batch({"kind": "batch", "target": "all"}, REPO_ROOT, "windows", "build all")

        self.assertEqual("ok", result.status)
        self.assertEqual(["windows:abi", "windows:windows-x64-reference"], calls)
        self.assertEqual(["abi", "windows-x64-reference"], result.payload["artifacts"])
