from __future__ import annotations

import importlib.util
import sys
import unittest
from pathlib import Path
from unittest.mock import patch


REPO_ROOT = Path(__file__).resolve().parents[3]
PROJECT_COMMAND_MODULE_PATH = REPO_ROOT / "build" / "toolchains" / "run" / "commands" / "project.py"
BUILD_COMMAND_MODULE_PATH = REPO_ROOT / "build" / "toolchains" / "run" / "commands" / "build.py"
DEPLOY_COMMAND_MODULE_PATH = REPO_ROOT / "build" / "toolchains" / "run" / "commands" / "deploy.py"


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


class ProjectCommandTests(unittest.TestCase):
    def test_project_dispatch_routes_subject_generation_to_workspace_helper(self) -> None:
        project_module = load_module(PROJECT_COMMAND_MODULE_PATH, "chaos_project_command_subject_dispatch")

        with patch.object(
            project_module.workspace_module,
            "generate_subject_workspace",
            return_value={
                "manifestPath": "solutions/subjects/FixtureSubject/workspace.manifest.json",
                "artifacts": ["solutions/subjects/FixtureSubject/workspace.manifest.json"],
                "importantOutputs": [],
                "consoleText": "",
            },
        ) as generate_mock:
            result = project_module.handle(
                {"kind": "subject-workspace"},
                REPO_ROOT,
                "windows",
                "generate project subject --id subject/FixtureSubject",
                {"id": "subject/FixtureSubject", "refresh-generated": True},
            )

        self.assertEqual("ok", result.status)
        self.assertIn("workspace.manifest.json", result.payload["artifacts"][0])
        generate_mock.assert_called_once_with(
            REPO_ROOT,
            "windows",
            {"id": "subject/FixtureSubject", "refresh-generated": True},
        )

    def test_project_dispatch_routes_aggregate_generation_to_workspace_helper(self) -> None:
        project_module = load_module(PROJECT_COMMAND_MODULE_PATH, "chaos_project_command_all_dispatch")

        with patch.object(
            project_module.workspace_module,
            "generate_all_workspaces",
            return_value={
                "manifestPath": "solutions/manifest.json",
                "artifacts": ["solutions/manifest.json", "solutions/all/generation.report.json"],
                "importantOutputs": [],
                "consoleText": "",
            },
        ) as generate_mock:
            result = project_module.handle(
                {"kind": "all-workspaces"},
                REPO_ROOT,
                "windows",
                "generate project all --host windows --refresh-generated",
                {"host": "windows", "refresh-generated": True},
            )

        self.assertEqual("ok", result.status)
        self.assertEqual("solutions/manifest.json", result.payload["manifestPath"])
        generate_mock.assert_called_once_with(
            REPO_ROOT,
            "windows",
            {"host": "windows", "refresh-generated": True},
        )

    def test_build_dispatch_routes_project_workspace_to_subject_and_core_helpers(self) -> None:
        build_module = load_module(BUILD_COMMAND_MODULE_PATH, "chaos_build_command_project_workspace")

        with patch.object(
            build_module.workspace_module,
            "build_subject_workspace",
            return_value={
                "buildReportPath": "solutions/subjects/FixtureSubject/build.report.json",
                "artifacts": ["solutions/subjects/FixtureSubject/build.report.json"],
                "importantOutputs": [],
                "consoleText": "",
            },
        ) as build_subject_mock:
            subject_result = build_module.handle(
                {"kind": "project-workspace", "project_kind": "subject"},
                REPO_ROOT,
                "windows",
                "build project subject --id subject/FixtureSubject",
            )

        with patch.object(
            build_module.workspace_module,
            "build_core_workspace",
            return_value={
                "buildReportPath": "solutions/core/windows/build.report.json",
                "artifacts": ["solutions/core/windows/build.report.json"],
                "importantOutputs": [],
                "consoleText": "",
            },
        ) as build_core_mock:
            core_result = build_module.handle(
                {"kind": "project-workspace", "project_kind": "core"},
                REPO_ROOT,
                "windows",
                "build project core --host windows --target windows-x64-reference",
            )

        self.assertEqual("ok", subject_result.status)
        self.assertEqual("ok", core_result.status)
        build_subject_mock.assert_called_once()
        build_core_mock.assert_called_once()

    def test_deploy_dispatch_routes_core_deploy_to_layout_helper(self) -> None:
        deploy_module = load_module(DEPLOY_COMMAND_MODULE_PATH, "chaos_deploy_command_dispatch")

        with patch.object(
            deploy_module.deploy_layout_module,
            "deploy_core_outputs",
            return_value={
                "artifacts": ["deploy/core/windows/windows-x64-reference/deploy.manifest.json"],
                "importantOutputs": [],
                "consoleText": "",
            },
        ) as deploy_mock:
            result = deploy_module.handle(
                {"kind": "core-deploy"},
                REPO_ROOT,
                "windows",
                "deploy core --host windows --all-targets",
                {"host": "windows", "all-targets": True},
            )

        self.assertEqual("ok", result.status)
        self.assertIn("deploy.manifest.json", result.payload["artifacts"][0])
        deploy_mock.assert_called_once_with(REPO_ROOT, "windows", {"host": "windows", "all-targets": True})


if __name__ == "__main__":
    unittest.main()
