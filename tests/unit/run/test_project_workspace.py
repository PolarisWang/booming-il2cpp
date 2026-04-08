from __future__ import annotations

import importlib.util
import json
import shutil
import subprocess
import sys
import unittest
import uuid
from pathlib import Path
from unittest.mock import patch


REPO_ROOT = Path(__file__).resolve().parents[3]
PROJECT_WORKSPACE_MODULE_PATH = REPO_ROOT / "build" / "toolchains" / "run" / "project_workspace.py"
TEST_TMP_ROOT = REPO_ROOT / "artifacts" / ".tmp-tests" / "project-workspace"


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


class ProjectWorkspaceTests(unittest.TestCase):
    @classmethod
    def setUpClass(cls) -> None:
        TEST_TMP_ROOT.mkdir(parents=True, exist_ok=True)

    def _make_repo_root(self, prefix: str) -> Path:
        repo_root = TEST_TMP_ROOT / f"{prefix}-{uuid.uuid4().hex}"
        repo_root.mkdir(parents=True, exist_ok=False)
        return repo_root

    def _write_subject_fixture(self, repo_root: Path, *, subject_id: str = "FixtureSubject") -> None:
        source_project = repo_root / "subjects" / subject_id / "source" / f"{subject_id}.csproj"
        unit_project = (
            repo_root
            / "subjects"
            / subject_id
            / "validation"
            / "unit"
            / f"{subject_id}.Subject.UnitTests"
            / f"{subject_id}.Subject.UnitTests.csproj"
        )
        native_reference_root = repo_root / "subjects" / subject_id / "validation" / "proof" / "native-reference"
        generated_root = repo_root / "artifacts" / "subjects" / subject_id / "runs" / "subject-exec" / "analysis" / "generated"

        source_project.parent.mkdir(parents=True, exist_ok=True)
        source_project.write_text("<Project />\n", encoding="utf-8")
        unit_project.parent.mkdir(parents=True, exist_ok=True)
        unit_project.write_text("<Project />\n", encoding="utf-8")
        native_reference_root.mkdir(parents=True, exist_ok=True)
        (native_reference_root / "CMakeLists.txt").write_text("cmake_minimum_required(VERSION 3.20)\n", encoding="utf-8")
        (generated_root / "generated").mkdir(parents=True, exist_ok=True)
        (generated_root / "generated" / "native-reference.generated.cpp").write_text("// generated\n", encoding="utf-8")

        write_json(
            repo_root / "subjects" / subject_id / "subject.manifest.json",
            {
                "subjectId": subject_id,
                "displayName": subject_id,
                "defaultGoal": "correctness.dev",
                "defaultMatrix": "windows-dev-output",
                "defaultValidationProfile": "proof-dev",
                "source": {
                    "type": "dotnet-project",
                    "path": f"subjects/{subject_id}/source/{subject_id}.csproj",
                    "entry": f"{subject_id}/Program::Main(System.String[])",
                },
                "validationProfiles": {
                    "proof-dev": ["proof", "unit"],
                },
                "validation": {
                    "proof": {
                        "kind": "proof",
                        "defaultVariant": "CHECK",
                    },
                    "unit": {
                        "kind": "unit",
                        "project": f"subjects/{subject_id}/validation/unit/{subject_id}.Subject.UnitTests/{subject_id}.Subject.UnitTests.csproj",
                        "framework": "xunit",
                        "defaultVariant": "CHECK",
                    },
                },
                "executionPipelines": [
                    {
                        "pipelineId": "proof-runtime-output",
                        "stages": [
                            {"stageId": "source-resolve", "kind": "source-resolve", "scope": "shared", "bucket": "source"},
                            {"stageId": "host-input-build", "kind": "host-input-build", "scope": "shared", "bucket": "host-input"},
                            {"stageId": "analysis-frontend", "kind": "analysis-frontend", "scope": "shared", "bucket": "analysis"},
                            {"stageId": "generated-native-proof", "kind": "generated-native-proof", "scope": "shared", "bucket": "generated"},
                        ],
                    },
                ],
                "environmentMatrices": [
                    {
                        "matrixId": "windows-dev-output",
                        "pipelineId": "proof-runtime-output",
                        "supportedGoals": ["correctness.dev"],
                        "executionContext": {
                            "hostPlatform": "windows-x64",
                            "targetPlatform": "windows-x64",
                            "toolchainProfile": "msvc-reference",
                        },
                        "artifactPlan": {
                            "evidenceTerminalBucket": "runtime",
                        },
                    },
                    {
                        "matrixId": "windows-linux-buildable",
                        "pipelineId": "proof-runtime-output",
                        "supportedGoals": ["correctness.platform"],
                        "executionContext": {
                            "hostPlatform": "windows-x64",
                            "targetPlatform": "linux-x64",
                            "toolchainProfile": "linux-cross",
                        },
                        "artifactPlan": {
                            "evidenceTerminalBucket": "build",
                        },
                    },
                ],
            },
        )

        (repo_root / "build" / "toolchains").mkdir(parents=True, exist_ok=True)
        (repo_root / "build" / "toolchains" / "linux-x64.cmake").write_text("# linux\n", encoding="utf-8")

    def _write_core_fixture(self, repo_root: Path) -> None:
        managed_projects = [
            "Chaos.IL2CPP.Contracts",
            "Chaos.IL2CPP.Loader",
            "Chaos.IL2CPP.SemanticWorld",
            "Chaos.IL2CPP.MetadataWriter",
            "Chaos.IL2CPP.CodeGen",
            "Chaos.IL2CPP.Linker",
            "Chaos.IL2CPP.Pipeline",
            "Chaos.IL2CPP.Driver",
        ]
        for project_name in managed_projects:
            project_dir = repo_root / "src" / "managed" / project_name
            project_dir.mkdir(parents=True, exist_ok=True)
            (project_dir / f"{project_name}.csproj").write_text("<Project />\n", encoding="utf-8")

        (repo_root / "build" / "toolchains").mkdir(parents=True, exist_ok=True)
        for toolchain_name in [
            "windows-x64-reference.cmake",
            "android-arm64.cmake",
            "linux-x64.cmake",
        ]:
            (repo_root / "build" / "toolchains" / toolchain_name).write_text("# toolchain\n", encoding="utf-8")

    def test_generate_subject_workspace_writes_manifest_solution_and_native_configure_root(self) -> None:
        workspace_module = load_module(PROJECT_WORKSPACE_MODULE_PATH, "chaos_project_workspace_subject_generate")
        repo_root = self._make_repo_root("subject-generate")
        self._write_subject_fixture(repo_root)
        completed = subprocess.CompletedProcess(["cmake"], 0, "", "")

        try:
            with patch.object(workspace_module.tooling_module, "cmake_environment", return_value=("cmake", {})):
                with patch.object(workspace_module, "run_process", return_value=completed) as run_process_mock:
                    result = workspace_module.generate_subject_workspace(
                        repo_root,
                        "windows",
                        {"id": "subject/FixtureSubject"},
                    )

            manifest_path = repo_root / result["manifestPath"]
            self.assertTrue(manifest_path.is_file())
            manifest = json.loads(manifest_path.read_text(encoding="utf-8"))
            self.assertEqual("FixtureSubject", manifest["subjectId"])
            self.assertEqual("CHECK", manifest["variant"])
            self.assertEqual(
                "artifacts/projects/subjects/FixtureSubject/managed/FixtureSubject.sln",
                manifest["managedSolutionPath"],
            )
            self.assertEqual(
                [
                    "subjects/FixtureSubject/source/FixtureSubject.csproj",
                    "subjects/FixtureSubject/validation/unit/FixtureSubject.Subject.UnitTests/FixtureSubject.Subject.UnitTests.csproj",
                ],
                manifest["managedProjects"],
            )
            self.assertEqual(1, len(manifest["matrices"]))
            self.assertEqual("windows-dev-output", manifest["matrices"][0]["matrixId"])
            self.assertEqual(
                "artifacts/projects/subjects/FixtureSubject/native/windows-dev-output",
                manifest["matrices"][0]["configureRoot"],
            )
            self.assertTrue((repo_root / manifest["managedSolutionPath"]).is_file())
            self.assertEqual(
                [
                    "cmake",
                    "-S",
                    str(repo_root / "subjects" / "FixtureSubject" / "validation" / "proof" / "native-reference"),
                    "-B",
                    str(repo_root / "artifacts" / "projects" / "subjects" / "FixtureSubject" / "native" / "windows-dev-output"),
                    "-G",
                    "Visual Studio 17 2022",
                    "-DCHAOS_SUBJECT_VARIANT=CHECK",
                    f"-DCHAOS_SUBJECT_GENERATED_ROOT={repo_root / 'artifacts' / 'subjects' / 'FixtureSubject' / 'runs' / 'subject-exec' / 'analysis' / 'generated'}",
                    f"-DCHAOS_SUBJECT_BUILD_OUT_ROOT={repo_root / 'artifacts' / 'projects' / 'subjects' / 'FixtureSubject' / 'native' / 'windows-dev-output' / 'out'}",
                    f"-DCHAOS_SUBJECT_RUNTIME_ROOT={repo_root / 'artifacts' / 'projects' / 'subjects' / 'FixtureSubject' / 'native' / 'windows-dev-output' / 'runtime'}",
                ],
                run_process_mock.call_args.args[0],
            )
        finally:
            shutil.rmtree(repo_root, ignore_errors=True)

    def test_generate_subject_workspace_refreshes_subject_exec_only_when_requested(self) -> None:
        workspace_module = load_module(PROJECT_WORKSPACE_MODULE_PATH, "chaos_project_workspace_subject_refresh")
        repo_root = self._make_repo_root("subject-refresh")
        self._write_subject_fixture(repo_root)

        try:
            with patch.object(
                workspace_module,
                "_subject_generated_source_path",
                return_value=repo_root / "artifacts" / "subjects" / "FixtureSubject" / "runs" / "subject-exec" / "analysis" / "generated" / "generated" / "missing.cpp",
            ):
                with self.assertRaisesRegex(RuntimeError, "subject-exec generated root is missing"):
                    workspace_module.generate_subject_workspace(
                        repo_root,
                        "windows",
                        {"id": "subject/FixtureSubject"},
                    )

            def refresh_side_effect(repo_root: Path, subject_id: str, matrix_id: str, variant: str) -> None:
                del matrix_id, variant
                generated_root = (
                    repo_root / "artifacts" / "subjects" / subject_id / "runs" / "subject-exec" / "analysis" / "generated" / "generated"
                )
                generated_root.mkdir(parents=True, exist_ok=True)
                (generated_root / "native-reference.generated.cpp").write_text("// generated\n", encoding="utf-8")

            with patch.object(workspace_module, "refresh_subject_generated_root", side_effect=refresh_side_effect) as refresh_mock:
                with patch.object(workspace_module.tooling_module, "cmake_environment", return_value=("cmake", {})):
                    with patch.object(workspace_module, "run_process", return_value=subprocess.CompletedProcess(["cmake"], 0, "", "")):
                        result = workspace_module.generate_subject_workspace(
                            repo_root,
                            "windows",
                            {"id": "subject/FixtureSubject", "refresh-generated": True},
                        )

            self.assertTrue((repo_root / result["manifestPath"]).is_file())
            refresh_mock.assert_called_once()
        finally:
            shutil.rmtree(repo_root, ignore_errors=True)

    def test_build_subject_workspace_uses_manifest_and_selected_matrix(self) -> None:
        workspace_module = load_module(PROJECT_WORKSPACE_MODULE_PATH, "chaos_project_workspace_subject_build")
        repo_root = self._make_repo_root("subject-build")
        self._write_subject_fixture(repo_root)
        workspace_root = repo_root / "artifacts" / "projects" / "subjects" / "FixtureSubject"
        build_root = workspace_root / "native" / "windows-dev-output"
        build_root.mkdir(parents=True, exist_ok=True)
        manifest_path = workspace_root / "workspace.manifest.json"
        write_json(
            manifest_path,
            {
                "kind": "subject-workspace",
                "subjectId": "FixtureSubject",
                "managedSolutionPath": "artifacts/projects/subjects/FixtureSubject/managed/FixtureSubject.sln",
                "managedProjects": [
                    "subjects/FixtureSubject/source/FixtureSubject.csproj",
                    "subjects/FixtureSubject/validation/unit/FixtureSubject.Subject.UnitTests/FixtureSubject.Subject.UnitTests.csproj",
                ],
                "matrices": [
                    {
                        "matrixId": "windows-dev-output",
                        "configureRoot": "artifacts/projects/subjects/FixtureSubject/native/windows-dev-output",
                        "buildArgs": ["--config", "Release", "--target", "chaos_subject_reference_proof"],
                    },
                    {
                        "matrixId": "windows-linux-buildable",
                        "configureRoot": "artifacts/projects/subjects/FixtureSubject/native/windows-linux-buildable",
                        "buildArgs": [],
                    },
                ],
            },
        )

        try:
            with patch.object(workspace_module.tooling_module, "ensure_dotnet_available", return_value=workspace_module.tooling_module.ToolBootstrapResult(ready=True)):
                with patch.object(
                    workspace_module,
                    "run_process",
                    return_value=subprocess.CompletedProcess(["tool"], 0, "", ""),
                ) as run_process_mock:
                    result = workspace_module.build_subject_workspace(
                        repo_root,
                        "windows",
                        {"id": "subject/FixtureSubject", "matrix": "windows-dev-output"},
                    )

            self.assertEqual("artifacts/projects/subjects/FixtureSubject/build.report.json", result["buildReportPath"])
            report = json.loads((repo_root / result["buildReportPath"]).read_text(encoding="utf-8"))
            self.assertEqual(["windows-dev-output"], report["builtMatrices"])
            self.assertEqual(
                ["dotnet", "build", str(repo_root / "subjects" / "FixtureSubject" / "source" / "FixtureSubject.csproj"), "-c", "Release"],
                run_process_mock.call_args_list[0].args[0],
            )
            self.assertEqual(
                [
                    "cmake",
                    "--build",
                    str(build_root),
                    "--config",
                    "Release",
                    "--target",
                    "chaos_subject_reference_proof",
                ],
                run_process_mock.call_args_list[2].args[0],
            )
        finally:
            shutil.rmtree(repo_root, ignore_errors=True)

    def test_generate_core_workspace_selects_all_host_targets_and_configures_them(self) -> None:
        workspace_module = load_module(PROJECT_WORKSPACE_MODULE_PATH, "chaos_project_workspace_core_generate")
        repo_root = self._make_repo_root("core-generate")
        self._write_core_fixture(repo_root)

        try:
            with patch.object(workspace_module.tooling_module, "cmake_environment", return_value=("cmake", {})):
                with patch.object(
                    workspace_module,
                    "run_process",
                    return_value=subprocess.CompletedProcess(["cmake"], 0, "", ""),
                ) as run_process_mock:
                    result = workspace_module.generate_core_workspace(
                        repo_root,
                        "windows",
                        {"all-targets": True},
                    )

            manifest = json.loads((repo_root / result["manifestPath"]).read_text(encoding="utf-8"))
            self.assertEqual("core-workspace", manifest["kind"])
            self.assertEqual("windows", manifest["hostPlatform"])
            self.assertEqual(
                [
                    "windows-x64-reference",
                    "android-arm64",
                    "linux-x64",
                ],
                [item["targetId"] for item in manifest["nativeTargets"]],
            )
            self.assertEqual(
                [
                    "direct-run-host",
                    "project-package-root",
                    "project-package-root",
                ],
                [item["deliveryKind"] for item in manifest["nativeTargets"]],
            )
            self.assertEqual(3, run_process_mock.call_count)
        finally:
            shutil.rmtree(repo_root, ignore_errors=True)

    def test_build_core_workspace_uses_manifest_and_selected_target(self) -> None:
        workspace_module = load_module(PROJECT_WORKSPACE_MODULE_PATH, "chaos_project_workspace_core_build")
        repo_root = self._make_repo_root("core-build")
        self._write_core_fixture(repo_root)
        workspace_root = repo_root / "artifacts" / "projects" / "core" / "windows"
        (workspace_root / "native" / "windows-x64-reference").mkdir(parents=True, exist_ok=True)
        manifest_path = workspace_root / "workspace.manifest.json"
        write_json(
            manifest_path,
            {
                "kind": "core-workspace",
                "hostPlatform": "windows",
                "managedSolutionPath": "artifacts/projects/core/windows/managed/chaos-il2cpp-core.sln",
                "managedProjects": [
                    "src/managed/Chaos.IL2CPP.Driver/Chaos.IL2CPP.Driver.csproj",
                    "src/managed/Chaos.IL2CPP.CodeGen/Chaos.IL2CPP.CodeGen.csproj",
                ],
                "nativeTargets": [
                    {
                        "targetId": "windows-x64-reference",
                        "configureRoot": "artifacts/projects/core/windows/native/windows-x64-reference",
                        "buildArgs": ["--config", "Release"],
                    },
                    {
                        "targetId": "linux-x64",
                        "configureRoot": "artifacts/projects/core/windows/native/linux-x64",
                        "buildArgs": [],
                    },
                ],
            },
        )

        try:
            with patch.object(workspace_module.tooling_module, "ensure_dotnet_available", return_value=workspace_module.tooling_module.ToolBootstrapResult(ready=True)):
                with patch.object(
                    workspace_module,
                    "run_process",
                    return_value=subprocess.CompletedProcess(["tool"], 0, "", ""),
                ) as run_process_mock:
                    result = workspace_module.build_core_workspace(
                        repo_root,
                        "windows",
                        {"target": "windows-x64-reference"},
                    )

            self.assertEqual("artifacts/projects/core/windows/build.report.json", result["buildReportPath"])
            report = json.loads((repo_root / result["buildReportPath"]).read_text(encoding="utf-8"))
            self.assertEqual(["windows-x64-reference"], report["builtTargets"])
            self.assertEqual(
                [
                    "cmake",
                    "--build",
                    str(repo_root / "artifacts" / "projects" / "core" / "windows" / "native" / "windows-x64-reference"),
                    "--config",
                    "Release",
                ],
                run_process_mock.call_args_list[2].args[0],
            )
        finally:
            shutil.rmtree(repo_root, ignore_errors=True)


if __name__ == "__main__":
    unittest.main()
