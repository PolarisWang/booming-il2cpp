from __future__ import annotations

import importlib.util
import subprocess
import sys
import unittest
from pathlib import Path
from unittest.mock import patch

from tests.support import select_public_suite_spec


REPO_ROOT = Path(__file__).resolve().parents[3]
TOOLING_MODULE_PATH = REPO_ROOT / "build" / "toolchains" / "run" / "core" / "tooling.py"
BUILD_MODULE_PATH = REPO_ROOT / "build" / "toolchains" / "run" / "commands" / "build.py"
TEST_MODULE_PATH = REPO_ROOT / "build" / "toolchains" / "run" / "commands" / "test.py"


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


class DotnetBootstrapTests(unittest.TestCase):
    def test_macos_interactive_session_can_bootstrap_dotnet_with_homebrew(self) -> None:
        tooling_module = load_module(TOOLING_MODULE_PATH, "chaos_run_tooling_install")
        smoke_spec = select_public_suite_spec(
            "chaos_dotnet_bootstrap_install_suite",
            host_platform="macos",
            family="smoke",
            required_stages=["run"],
        )
        install = subprocess.CompletedProcess(
            ["brew", "install", "--cask", "dotnet-sdk"],
            0,
            stdout="brew install completed",
            stderr="",
        )
        commands: list[list[str]] = []
        dotnet_locations = [None, "/opt/homebrew/bin/dotnet"]

        def fake_which(executable: str) -> str | None:
            if executable == "dotnet":
                return dotnet_locations.pop(0)
            if executable == "brew":
                return "/opt/homebrew/bin/brew"
            return None

        def fake_run(arguments: list[str], cwd: Path | None = None) -> subprocess.CompletedProcess[str]:
            del cwd
            commands.append(arguments)
            return install

        outcome = tooling_module.ensure_dotnet_available(
            f"test {smoke_spec['family']} {smoke_spec['suite']}",
            "macos",
            interactive=True,
            prompt=lambda _: "y",
            which=fake_which,
            run=fake_run,
        )

        self.assertTrue(outcome.ready)
        self.assertEqual([["brew", "install", "--cask", "dotnet-sdk"]], commands)
        self.assertIn("brew install completed", outcome.output)

    def test_macos_non_interactive_session_returns_manual_install_guidance(self) -> None:
        tooling_module = load_module(TOOLING_MODULE_PATH, "chaos_run_tooling_noninteractive")

        outcome = tooling_module.ensure_dotnet_available(
            "test workflow roadmap-0-macos",
            "macos",
            interactive=False,
            which=lambda executable: None,
        )

        self.assertFalse(outcome.ready)
        self.assertEqual(["dotnet SDK is not installed"], outcome.errors)
        self.assertIn("brew install --cask dotnet-sdk", outcome.output)
        self.assertIn("non-interactive", outcome.output)

    def test_macos_interactive_session_requires_homebrew_for_auto_install(self) -> None:
        tooling_module = load_module(TOOLING_MODULE_PATH, "chaos_run_tooling_missing_brew")
        smoke_spec = select_public_suite_spec(
            "chaos_dotnet_bootstrap_missing_brew_suite",
            host_platform="macos",
            family="smoke",
            required_stages=["build"],
        )

        outcome = tooling_module.ensure_dotnet_available(
            f"build {smoke_spec['family']} {smoke_spec['suite']}",
            "macos",
            interactive=True,
            prompt=lambda _: "y",
            which=lambda executable: None,
        )

        self.assertFalse(outcome.ready)
        self.assertEqual(["dotnet SDK is not installed"], outcome.errors)
        self.assertIn("Homebrew", outcome.output)
        self.assertIn("https://brew.sh", outcome.output)

    def test_build_smoke_project_checks_dotnet_before_running(self) -> None:
        build_module = load_module(BUILD_MODULE_PATH, "chaos_run_build_dotnet_gate")
        smoke_spec = select_public_suite_spec(
            "chaos_dotnet_bootstrap_build_suite",
            host_platform="macos",
            family="smoke",
            required_stages=["build", "run"],
        )
        suite_name = str(smoke_spec["suite"])
        legacy_commands = dict(smoke_spec.get("legacy_commands") or {})
        bootstrap = build_module.tooling_module.ToolBootstrapResult(
            ready=False,
            output="Run `brew install --cask dotnet-sdk`, then retry.\n",
            errors=["dotnet SDK is not installed"],
        )

        with patch.object(build_module.tooling_module, "ensure_dotnet_available", return_value=bootstrap):
            gated = build_module.handle(
                {
                    "id": str(legacy_commands["build"]),
                    "kind": "smoke-project",
                    "project_path": f"subjects/{suite_name}/source/{suite_name}.csproj",
                    "artifact_path": f"artifacts/smoke/bin/{suite_name}/Release/net8.0/{suite_name}.dll",
                    "target": suite_name,
                },
                REPO_ROOT,
                "macos",
                f"build {smoke_spec['family']} {suite_name}",
            )

        self.assertIn("brew install --cask dotnet-sdk", gated.text or "")
        self.assertEqual(["dotnet SDK is not installed"], gated.errors)

    def test_smoke_test_checks_dotnet_before_running(self) -> None:
        test_module = load_module(TEST_MODULE_PATH, "chaos_run_test_dotnet_gate")
        smoke_spec = select_public_suite_spec(
            "chaos_dotnet_bootstrap_test_suite",
            host_platform="macos",
            family="smoke",
            required_stages=["run"],
        )
        suite_name = str(smoke_spec["suite"])
        legacy_commands = dict(smoke_spec.get("legacy_commands") or {})
        bootstrap = test_module.tooling_module.ToolBootstrapResult(
            ready=False,
            output="Run `brew install --cask dotnet-sdk`, then retry.\n",
            errors=["dotnet SDK is not installed"],
        )

        with patch.object(test_module.tooling_module, "ensure_dotnet_available", return_value=bootstrap):
            with patch.object(test_module, "run_process") as run_process_mock:
                gated = test_module.handle(
                    {
                        "id": str(legacy_commands["run"]),
                        "kind": "smoke-run",
                        "project_path": f"subjects/{suite_name}/source/{suite_name}.csproj",
                        "dll_path": f"artifacts/smoke/bin/{suite_name}/Release/net8.0/{suite_name}.dll",
                        "expected_patterns": ["Hello from managed smoke"],
                        "target": suite_name,
                    },
                    REPO_ROOT,
                    "macos",
                    f"test {smoke_spec['family']} {suite_name}",
                )

        self.assertIn("brew install --cask dotnet-sdk", gated.text or "")
        self.assertEqual(["dotnet SDK is not installed"], gated.errors)
        run_process_mock.assert_not_called()

    def test_workflow_suite_checks_dotnet_before_running(self) -> None:
        manifest_module = load_module(REPO_ROOT / "build" / "toolchains" / "run" / "core" / "manifest.py", "chaos_run_manifest_dotnet_gate_tooling")
        test_module = load_module(TEST_MODULE_PATH, "chaos_run_test_workflow_dotnet_gate_tooling")
        manifest = manifest_module.load_run_manifest(REPO_ROOT, REPO_ROOT / "build" / "toolchains" / "run" / "run_manifest.json")
        bootstrap = test_module.tooling_module.ToolBootstrapResult(
            ready=False,
            output="Run `brew install --cask dotnet-sdk`, then retry.\n",
            errors=["dotnet SDK is not installed"],
        )

        with patch.object(test_module.build_commands.tooling_module, "ensure_dotnet_available", return_value=bootstrap):
            with patch.object(test_module.tooling_module, "ensure_dotnet_available", return_value=bootstrap):
                with patch.object(test_module, "run_process") as run_process_mock:
                    gated = test_module.handle(
                        {
                            "id": "test-workflow-roadmap-0-macos",
                            "handler": "test.dispatch",
                            "kind": "registry-object-alias",
                            "target": "roadmap-0-macos",
                            "registry_object_kind": "system",
                            "registry_object_id": "system/roadmap-0-macos",
                        },
                        REPO_ROOT,
                        "macos",
                        "test workflow roadmap-0-macos",
                        manifest,
                    )

        self.assertIn("brew install --cask dotnet-sdk", gated.text or "")
        self.assertEqual(["dotnet SDK is not installed"], gated.errors)
        run_process_mock.assert_not_called()


if __name__ == "__main__":
    unittest.main()
