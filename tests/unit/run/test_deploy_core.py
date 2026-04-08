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
DEPLOY_LAYOUT_MODULE_PATH = REPO_ROOT / "build" / "toolchains" / "run" / "deploy_layout.py"
TEST_TMP_ROOT = REPO_ROOT / "artifacts" / ".tmp-tests" / "deploy-core"


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


def write_json(path: Path, payload: dict) -> None:
    path.parent.mkdir(parents=True, exist_ok=True)
    path.write_text(json.dumps(payload, indent=2) + "\n", encoding="utf-8")


class DeployCoreTests(unittest.TestCase):
    @classmethod
    def setUpClass(cls) -> None:
        TEST_TMP_ROOT.mkdir(parents=True, exist_ok=True)

    def _make_repo_root(self, prefix: str) -> Path:
        repo_root = TEST_TMP_ROOT / f"{prefix}-{uuid.uuid4().hex}"
        repo_root.mkdir(parents=True, exist_ok=False)
        return repo_root

    def _write_workspace_fixture(self, repo_root: Path) -> tuple[str, str]:
        driver_output_root = repo_root / "src" / "managed" / "Chaos.IL2CPP.Driver" / "bin" / "Release" / "net8.0"
        driver_output_root.mkdir(parents=True, exist_ok=True)
        for file_name in [
            "Chaos.IL2CPP.Driver.exe",
            "Chaos.IL2CPP.Driver.dll",
            "Chaos.IL2CPP.Driver.deps.json",
            "Chaos.IL2CPP.Driver.runtimeconfig.json",
            "Chaos.IL2CPP.CodeGen.dll",
        ]:
            (driver_output_root / file_name).write_text(file_name + "\n", encoding="utf-8")

        windows_configure_root = repo_root / "artifacts" / "projects" / "core" / "windows" / "native" / "windows-x64-reference"
        android_configure_root = repo_root / "artifacts" / "projects" / "core" / "windows" / "native" / "android-arm64"
        for configure_root in [windows_configure_root, android_configure_root]:
            configure_root.mkdir(parents=True, exist_ok=True)
            (configure_root / "CMakeCache.txt").write_text("cache\n", encoding="utf-8")

        workspace_manifest_path = repo_root / "artifacts" / "projects" / "core" / "windows" / "workspace.manifest.json"
        build_report_path = repo_root / "artifacts" / "projects" / "core" / "windows" / "build.report.json"
        write_json(
            workspace_manifest_path,
            {
                "kind": "core-workspace",
                "hostPlatform": "windows",
                "managedSolutionPath": "artifacts/projects/core/windows/managed/chaos-il2cpp-core.sln",
                "nativeTargets": [
                    {
                        "targetId": "windows-x64-reference",
                        "deliveryKind": "direct-run-host",
                        "configureRoot": "artifacts/projects/core/windows/native/windows-x64-reference",
                    },
                    {
                        "targetId": "android-arm64",
                        "deliveryKind": "project-package-root",
                        "configureRoot": "artifacts/projects/core/windows/native/android-arm64",
                    },
                ],
            },
        )
        write_json(
            build_report_path,
            {
                "hostPlatform": "windows",
                "builtTargets": ["windows-x64-reference", "android-arm64"],
            },
        )
        return (
            workspace_manifest_path.relative_to(repo_root).as_posix(),
            build_report_path.relative_to(repo_root).as_posix(),
        )

    def test_materialize_core_deploy_writes_direct_run_host_and_package_root_outputs(self) -> None:
        deploy_layout_module = load_module(DEPLOY_LAYOUT_MODULE_PATH, "chaos_deploy_layout_materialize")
        repo_root = self._make_repo_root("materialize")
        workspace_manifest_path, build_report_path = self._write_workspace_fixture(repo_root)

        try:
            result = deploy_layout_module.materialize_core_deploy(
                repo_root=repo_root,
                host_platform="windows",
                workspace_manifest_path=workspace_manifest_path,
                build_report_path=build_report_path,
                target_ids=["windows-x64-reference", "android-arm64"],
            )

            reference_root = repo_root / "deploy" / "core" / "windows" / "windows-x64-reference"
            self.assertTrue((reference_root / "bin" / "chaos-il2cpp.exe").is_file())
            self.assertTrue((reference_root / "bin" / "Chaos.IL2CPP.Driver.dll").is_file())
            self.assertTrue((reference_root / "package-root" / "CMakeCache.txt").is_file())

            reference_manifest = json.loads((reference_root / "deploy.manifest.json").read_text(encoding="utf-8"))
            self.assertEqual("direct-run-host", reference_manifest["deliveryKind"])
            self.assertEqual(
                "deploy/core/windows/windows-x64-reference/bin/chaos-il2cpp.exe",
                reference_manifest["entryExecutable"],
            )
            self.assertEqual(workspace_manifest_path, reference_manifest["workspaceManifestPath"])
            self.assertEqual(build_report_path, reference_manifest["buildReportPath"])

            android_root = repo_root / "deploy" / "core" / "windows" / "android-arm64"
            self.assertTrue((android_root / "package-root" / "CMakeCache.txt").is_file())
            android_manifest = json.loads((android_root / "deploy.manifest.json").read_text(encoding="utf-8"))
            self.assertEqual("project-package-root", android_manifest["deliveryKind"])
            self.assertIsNone(android_manifest["entryExecutable"])
            self.assertTrue(result["artifacts"])
        finally:
            shutil.rmtree(repo_root, ignore_errors=True)

    def test_deploy_core_outputs_generates_workspace_and_builds_before_materializing(self) -> None:
        deploy_layout_module = load_module(DEPLOY_LAYOUT_MODULE_PATH, "chaos_deploy_layout_dispatch")
        repo_root = self._make_repo_root("dispatch")

        try:
            with patch.object(
                deploy_layout_module.workspace_module,
                "generate_core_workspace",
                return_value={"manifestPath": "artifacts/projects/core/windows/workspace.manifest.json"},
            ) as generate_mock:
                with patch.object(
                    deploy_layout_module.workspace_module,
                    "build_core_workspace",
                    return_value={"buildReportPath": "artifacts/projects/core/windows/build.report.json"},
                ) as build_mock:
                    with patch.object(
                        deploy_layout_module,
                        "materialize_core_deploy",
                        return_value={"artifacts": ["deploy/core/windows/windows-x64-reference/deploy.manifest.json"]},
                    ) as materialize_mock:
                        result = deploy_layout_module.deploy_core_outputs(
                            repo_root,
                            "windows",
                            {"all-targets": True},
                        )

            self.assertEqual(["deploy/core/windows/windows-x64-reference/deploy.manifest.json"], result["artifacts"])
            generate_mock.assert_called_once_with(repo_root, "windows", {"all-targets": True})
            build_mock.assert_called_once_with(repo_root, "windows", {"all-targets": True})
            materialize_mock.assert_called_once()
        finally:
            shutil.rmtree(repo_root, ignore_errors=True)


if __name__ == "__main__":
    unittest.main()
