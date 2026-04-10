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
PROJECT_WORKSPACE_MODULE_PATH = REPO_ROOT / "build" / "toolchains" / "run" / "subject" / "project_workspace.py"
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


def write_windows_subject_native_project_stubs(
    configure_root: Path,
    *,
    generated_text: str = "<Project />\n",
    proof_text: str = "<Project />\n",
) -> None:
    generated_project = configure_root / "generated" / "chaos_subject_generated_native.vcxproj"
    proof_project = configure_root / "proof" / "chaos_subject_reference_proof.vcxproj"
    generated_project.parent.mkdir(parents=True, exist_ok=True)
    proof_project.parent.mkdir(parents=True, exist_ok=True)
    generated_project.write_text(generated_text, encoding="utf-8")
    proof_project.write_text(proof_text, encoding="utf-8")


def write_windows_subject_native_solution_graph_stubs(configure_root: Path) -> None:
    project_specs = [
        ("ALL_BUILD.vcxproj", "ALL_BUILD"),
        ("ZERO_CHECK.vcxproj", "ZERO_CHECK"),
        ("bootstrap/chaos_bootstrap.vcxproj", "chaos_bootstrap"),
        ("runtime-core/chaos_runtime_core.vcxproj", "chaos_runtime_core"),
        ("generated/chaos_subject_generated_native.vcxproj", "chaos_subject_generated_native"),
        ("proof/chaos_subject_reference_proof.vcxproj", "chaos_subject_reference_proof"),
        ("proof/chaos_subject_reference_proof_run.vcxproj", "chaos_subject_reference_proof_run"),
        ("support/chaos_support.vcxproj", "chaos_support"),
    ]
    reference_specs = {
        "ALL_BUILD.vcxproj": [
            "ZERO_CHECK.vcxproj",
            "bootstrap/chaos_bootstrap.vcxproj",
            "runtime-core/chaos_runtime_core.vcxproj",
            "support/chaos_support.vcxproj",
        ],
        "bootstrap/chaos_bootstrap.vcxproj": [
            "ZERO_CHECK.vcxproj",
            "runtime-core/chaos_runtime_core.vcxproj",
            "support/chaos_support.vcxproj",
        ],
        "generated/chaos_subject_generated_native.vcxproj": [
            "ZERO_CHECK.vcxproj",
        ],
        "proof/chaos_subject_reference_proof.vcxproj": [
            "ZERO_CHECK.vcxproj",
            "bootstrap/chaos_bootstrap.vcxproj",
            "runtime-core/chaos_runtime_core.vcxproj",
            "generated/chaos_subject_generated_native.vcxproj",
            "support/chaos_support.vcxproj",
        ],
        "proof/chaos_subject_reference_proof_run.vcxproj": [
            "ZERO_CHECK.vcxproj",
            "proof/chaos_subject_reference_proof.vcxproj",
        ],
        "runtime-core/chaos_runtime_core.vcxproj": [
            "ZERO_CHECK.vcxproj",
        ],
        "support/chaos_support.vcxproj": [
            "ZERO_CHECK.vcxproj",
            "runtime-core/chaos_runtime_core.vcxproj",
        ],
    }

    materialized_specs: list[tuple[str, str, str]] = []
    for relative_path, project_name in project_specs:
        project_guid = "{" + str(uuid.uuid5(uuid.NAMESPACE_URL, f"{configure_root.as_posix()}::{relative_path}")).upper() + "}"
        materialized_specs.append((relative_path, project_name, project_guid))
    guid_by_relative_path = {relative_path: project_guid for relative_path, _, project_guid in materialized_specs}
    name_by_relative_path = {relative_path: project_name for relative_path, project_name, _ in materialized_specs}

    for relative_path, project_name, project_guid in materialized_specs:
        project_path = configure_root / Path(relative_path)
        project_path.parent.mkdir(parents=True, exist_ok=True)
        reference_lines = []
        for reference_relative_path in reference_specs.get(relative_path, []):
            reference_path = configure_root / Path(reference_relative_path)
            reference_lines.extend(
                [
                    "  <ItemGroup>",
                    f'    <ProjectReference Include="{reference_path.as_posix()}">',
                    f"      <Project>{guid_by_relative_path[reference_relative_path]}</Project>",
                    f"      <Name>{name_by_relative_path[reference_relative_path]}</Name>",
                    "    </ProjectReference>",
                    "  </ItemGroup>",
                ]
            )
        project_path.write_text(
            "\n".join(
                [
                    "<Project>",
                    "  <PropertyGroup Label=\"Globals\">",
                    f"    <ProjectGuid>{project_guid}</ProjectGuid>",
                    f"    <ProjectName>{project_name}</ProjectName>",
                    "  </PropertyGroup>",
                    *reference_lines,
                    "</Project>",
                    "",
                ]
            ),
            encoding="utf-8",
        )

    solution_lines = [
        "Microsoft Visual Studio Solution File, Format Version 12.00",
        "# Visual Studio Version 17",
        "VisualStudioVersion = 17.0.31903.59",
        "MinimumVisualStudioVersion = 10.0.40219.1",
    ]
    for relative_path, project_name, project_guid in materialized_specs:
        solution_lines.append(
            f'Project("{{8BC9CEB8-8B4A-11D0-8D11-00A0C91BC942}}") = "{project_name}", "{relative_path.replace("/", "\\")}", "{project_guid}"'
        )
        solution_lines.append("EndProject")
    solution_lines.extend(
        [
            "Global",
            "\tGlobalSection(SolutionConfigurationPlatforms) = preSolution",
            "\t\tDebug|x64 = Debug|x64",
            "\t\tRelease|x64 = Release|x64",
            "\tEndGlobalSection",
            "EndGlobal",
        ]
    )
    (configure_root / "chaos_subject_native_workspace.sln").write_text(
        "\n".join(solution_lines) + "\n",
        encoding="utf-8",
    )


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
        subject_exec_root = repo_root / "artifacts" / "subjects" / subject_id / "runs" / "subject-exec"
        generated_root = subject_exec_root / "analysis" / "generated"

        source_project.parent.mkdir(parents=True, exist_ok=True)
        source_project.write_text("<Project />\n", encoding="utf-8")
        unit_project.parent.mkdir(parents=True, exist_ok=True)
        unit_project.write_text("<Project />\n", encoding="utf-8")
        native_reference_root.mkdir(parents=True, exist_ok=True)
        (native_reference_root / "CMakeLists.txt").write_text("cmake_minimum_required(VERSION 3.20)\n", encoding="utf-8")
        (native_reference_root / "main.cpp").write_text("int main() { return 0; }\n", encoding="utf-8")
        (generated_root / "generated").mkdir(parents=True, exist_ok=True)
        (generated_root / "generated.manifest.json").write_text("{}\n", encoding="utf-8")
        (generated_root / "native-reference.manifest.json").write_text("{}\n", encoding="utf-8")
        (generated_root / "native-reference.plan.json").write_text("{}\n", encoding="utf-8")
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

    def _write_managed_only_subject_fixture(self, repo_root: Path, *, subject_id: str = "ManagedOnlySubject") -> None:
        source_project = repo_root / "subjects" / subject_id / "source" / f"{subject_id}.csproj"
        perf_project = (
            repo_root
            / "subjects"
            / subject_id
            / "validation"
            / "perf"
            / f"{subject_id}.Subject.PerfHarness"
            / f"{subject_id}.Subject.PerfHarness.csproj"
        )

        source_project.parent.mkdir(parents=True, exist_ok=True)
        source_project.write_text("<Project />\n", encoding="utf-8")
        perf_project.parent.mkdir(parents=True, exist_ok=True)
        perf_project.write_text("<Project />\n", encoding="utf-8")

        write_json(
            repo_root / "subjects" / subject_id / "subject.manifest.json",
            {
                "subjectId": subject_id,
                "displayName": subject_id,
                "defaultGoal": "perf.dev",
                "defaultMatrix": "windows-perf-dev",
                "defaultValidationProfile": "perf-dev",
                "source": {
                    "type": "dotnet-project",
                    "path": f"subjects/{subject_id}/source/{subject_id}.csproj",
                    "entry": f"{subject_id}/Program::Main()",
                },
                "validationProfiles": {
                    "perf-dev": ["perf"],
                },
                "validation": {
                    "perf": {
                        "kind": "perf",
                        "project": f"subjects/{subject_id}/validation/perf/{subject_id}.Subject.PerfHarness/{subject_id}.Subject.PerfHarness.csproj",
                        "driver": "csharp-perf-harness",
                        "defaultVariant": "PROFILE",
                    },
                },
                "executionPipelines": [
                    {
                        "pipelineId": "managed-runtime-perf",
                        "stages": [
                            {"stageId": "source-resolve", "kind": "source-resolve", "scope": "shared", "bucket": "source"},
                            {"stageId": "host-input-build", "kind": "host-input-build", "scope": "shared", "bucket": "host-input"},
                            {"stageId": "runtime-perf-collect", "kind": "runtime-perf-collect", "scope": "matrix", "bucket": "runtime"},
                        ],
                    },
                ],
                "environmentMatrices": [
                    {
                        "matrixId": "windows-perf-dev",
                        "pipelineId": "managed-runtime-perf",
                        "supportedGoals": ["perf.dev"],
                        "executionContext": {
                            "hostPlatform": "windows-x64",
                            "targetPlatform": "windows-x64",
                            "toolchainProfile": "dotnet-managed",
                        },
                        "validationIntent": {
                            "validationMode": "perf",
                            "adaptationLevel": "managed-runtime",
                            "expectedOutcome": "pass",
                        },
                        "artifactPlan": {
                            "evidenceTerminalBucket": "runtime",
                        },
                    },
                ],
            },
        )

    def test_generate_subject_workspace_writes_manifest_solution_and_native_configure_root(self) -> None:
        workspace_module = load_module(PROJECT_WORKSPACE_MODULE_PATH, "chaos_project_workspace_subject_generate")
        repo_root = self._make_repo_root("subject-generate")
        self._write_subject_fixture(repo_root)
        completed = subprocess.CompletedProcess(["cmake"], 0, "", "")

        try:
            def configure_side_effect(arguments: list[str], cwd: Path, env: dict[str, str] | None = None):
                del cwd, env
                configure_root = Path(arguments[arguments.index("-B") + 1])
                write_windows_subject_native_project_stubs(configure_root)
                return completed

            with patch.object(workspace_module.tooling_module, "cmake_environment", return_value=("cmake", {})):
                with patch.object(workspace_module, "run_process", side_effect=configure_side_effect) as run_process_mock:
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
            self.assertEqual(8, manifest["visualStudioStateVersion"])
            self.assertEqual(
                "solutions/subjects/FixtureSubject/generated/subject-exec",
                manifest["generatedRoot"],
            )
            self.assertEqual(
                "solutions/subjects/FixtureSubject/FixtureSubject.sln",
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
                "solutions/subjects/FixtureSubject/native/windows-dev-output",
                manifest["matrices"][0]["configureRoot"],
            )
            self.assertEqual(
                [
                    {
                        "buildArgs": ["--config", "Release", "--target", "chaos_subject_generated_native"],
                        "kind": "generated-native",
                        "projectPath": "solutions/subjects/FixtureSubject/native/windows-dev-output/generated/chaos_subject_generated_native.vcxproj",
                        "targetId": "chaos_subject_generated_native",
                    },
                    {
                        "buildArgs": ["--config", "Release", "--target", "chaos_subject_reference_proof"],
                        "kind": "proof-native",
                        "projectPath": "solutions/subjects/FixtureSubject/native/windows-dev-output/proof/chaos_subject_reference_proof.vcxproj",
                        "targetId": "chaos_subject_reference_proof",
                    },
                ],
                manifest["matrices"][0]["nativeProjects"],
            )
            self.assertEqual("chaos_subject_reference_proof", manifest["matrices"][0]["defaultOpenNativeProject"])
            self.assertEqual("chaos_subject_reference_proof", manifest["matrices"][0]["defaultBuildNativeProject"])
            self.assertEqual(
                "solutions/subjects/FixtureSubject/native/windows-dev-output/generated/chaos_subject_generated_native.vcxproj",
                manifest["matrices"][0]["generatedNativeProjectPath"],
            )
            self.assertEqual(
                "solutions/subjects/FixtureSubject/native/windows-dev-output/proof/chaos_subject_reference_proof.vcxproj",
                manifest["matrices"][0]["proofNativeProjectPath"],
            )
            self.assertEqual(
                "solutions/subjects/FixtureSubject/native/windows-dev-output/proof/chaos_subject_reference_proof.vcxproj",
                manifest["matrices"][0]["nativeProjectPath"],
            )
            self.assertTrue((repo_root / manifest["managedSolutionPath"]).is_file())
            self.assertTrue((repo_root / manifest["matrices"][0]["generatedNativeProjectPath"]).is_file())
            self.assertTrue((repo_root / manifest["matrices"][0]["proofNativeProjectPath"]).is_file())
            mirrored_subject_exec_root = repo_root / "solutions" / "subjects" / "FixtureSubject" / "generated" / "subject-exec"
            self.assertTrue((mirrored_subject_exec_root / "analysis" / "generated" / "generated.manifest.json").is_file())
            self.assertTrue((mirrored_subject_exec_root / "analysis" / "generated" / "native-reference.plan.json").is_file())
            self.assertTrue(
                (mirrored_subject_exec_root / "analysis" / "generated" / "generated" / "native-reference.generated.cpp").is_file()
            )
            solution_text = (repo_root / manifest["managedSolutionPath"]).read_text(encoding="utf-8")
            self.assertIn("FixtureSubject.csproj", solution_text)
            self.assertIn("FixtureSubject.Subject.UnitTests.csproj", solution_text)
            self.assertIn("native/windows-dev-output/generated/chaos_subject_generated_native.vcxproj", solution_text)
            self.assertIn("native/windows-dev-output/proof/chaos_subject_reference_proof.vcxproj", solution_text)
            self.assertEqual(
                [
                    "cmake",
                    "-S",
                    str(repo_root / "solutions" / "subjects" / "FixtureSubject" / "native-source" / "windows-dev-output"),
                    "-B",
                    str(repo_root / "solutions" / "subjects" / "FixtureSubject" / "native" / "windows-dev-output"),
                    "-G",
                    "Visual Studio 17 2022",
                    f"-DCHAOS_SUBJECT_REPO_ROOT={repo_root.as_posix()}",
                    "-DCHAOS_SUBJECT_VARIANT=CHECK",
                    f"-DCHAOS_SUBJECT_BUILD_OUT_ROOT={repo_root / 'solutions' / 'subjects' / 'FixtureSubject' / 'native' / 'windows-dev-output' / 'out'}",
                    f"-DCHAOS_SUBJECT_RUNTIME_ROOT={repo_root / 'solutions' / 'subjects' / 'FixtureSubject' / 'native' / 'windows-dev-output' / 'runtime'}",
                ],
                run_process_mock.call_args.args[0],
            )
        finally:
            shutil.rmtree(repo_root, ignore_errors=True)

    def test_generate_subject_workspace_clears_stale_visual_studio_state_when_manifest_version_changes(self) -> None:
        workspace_module = load_module(PROJECT_WORKSPACE_MODULE_PATH, "chaos_project_workspace_subject_clear_stale_vs_state")
        repo_root = self._make_repo_root("subject-clear-stale-vs-state")
        self._write_subject_fixture(repo_root)
        completed = subprocess.CompletedProcess(["cmake"], 0, "", "")
        workspace_root = repo_root / "solutions" / "subjects" / "FixtureSubject"
        stale_vs_state_file = workspace_root / ".vs" / "FixtureSubject" / "v17" / ".suo"
        stale_vs_state_file.parent.mkdir(parents=True, exist_ok=True)
        stale_vs_state_file.write_text("stale\n", encoding="utf-8")
        write_json(
            workspace_root / "workspace.manifest.json",
            {
                "kind": "subject-workspace",
                "subjectId": "FixtureSubject",
                "visualStudioStateVersion": 1,
            },
        )

        try:
            def configure_side_effect(arguments: list[str], cwd: Path, env: dict[str, str] | None = None):
                del cwd, env
                configure_root = Path(arguments[arguments.index("-B") + 1])
                configure_root.mkdir(parents=True, exist_ok=True)
                (configure_root / "chaos_subject_generated_native.vcxproj").write_text("<Project />\n", encoding="utf-8")
                (configure_root / "chaos_subject_reference_proof.vcxproj").write_text("<Project />\n", encoding="utf-8")
                return completed

            with patch.object(workspace_module.tooling_module, "cmake_environment", return_value=("cmake", {})):
                with patch.object(workspace_module, "run_process", side_effect=configure_side_effect):
                    workspace_module.generate_subject_workspace(
                        repo_root,
                        "windows",
                        {"id": "subject/FixtureSubject"},
                    )

            self.assertFalse((workspace_root / ".vs").exists())
            updated_manifest = json.loads((workspace_root / "workspace.manifest.json").read_text(encoding="utf-8"))
            self.assertEqual(8, updated_manifest["visualStudioStateVersion"])
        finally:
            shutil.rmtree(repo_root, ignore_errors=True)

    def test_generate_subject_workspace_supports_generated_open_target(self) -> None:
        workspace_module = load_module(PROJECT_WORKSPACE_MODULE_PATH, "chaos_project_workspace_subject_generate_open_target")
        repo_root = self._make_repo_root("subject-generate-open-target")
        self._write_subject_fixture(repo_root)
        completed = subprocess.CompletedProcess(["cmake"], 0, "", "")

        try:
            def configure_side_effect(arguments: list[str], cwd: Path, env: dict[str, str] | None = None):
                del cwd, env
                configure_root = Path(arguments[arguments.index("-B") + 1])
                write_windows_subject_native_project_stubs(configure_root)
                return completed

            with patch.object(workspace_module.tooling_module, "cmake_environment", return_value=("cmake", {})):
                with patch.object(workspace_module, "run_process", side_effect=configure_side_effect):
                    result = workspace_module.generate_subject_workspace(
                        repo_root,
                        "windows",
                        {"id": "subject/FixtureSubject", "open-native-target": "generated"},
                    )

            manifest = json.loads((repo_root / result["manifestPath"]).read_text(encoding="utf-8"))
            matrix = manifest["matrices"][0]
            self.assertEqual("chaos_subject_generated_native", matrix["defaultOpenNativeProject"])
            self.assertEqual("chaos_subject_generated_native", matrix["primaryOpenTarget"])
            self.assertEqual(matrix["generatedNativeProjectPath"], matrix["nativeProjectPath"])
            self.assertIn(
                {
                    "label": "Default native project",
                    "path": "solutions/subjects/FixtureSubject/native/windows-dev-output/generated/chaos_subject_generated_native.vcxproj",
                },
                result["importantOutputs"],
            )
        finally:
            shutil.rmtree(repo_root, ignore_errors=True)

    def test_generate_subject_workspace_disambiguates_duplicate_native_project_names_in_solution(self) -> None:
        workspace_module = load_module(PROJECT_WORKSPACE_MODULE_PATH, "chaos_project_workspace_subject_generate_duplicate_native_names")
        repo_root = self._make_repo_root("subject-generate-duplicate-native-names")
        self._write_subject_fixture(repo_root)
        completed = subprocess.CompletedProcess(["cmake"], 0, "", "")
        subject_manifest_path = repo_root / "subjects" / "FixtureSubject" / "subject.manifest.json"
        subject_manifest = json.loads(subject_manifest_path.read_text(encoding="utf-8"))
        subject_manifest["environmentMatrices"].append(
            {
                "matrixId": "windows-reference-trace",
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
            }
        )
        write_json(subject_manifest_path, subject_manifest)

        try:
            def configure_side_effect(arguments: list[str], cwd: Path, env: dict[str, str] | None = None):
                del cwd, env
                configure_root = Path(arguments[arguments.index("-B") + 1])
                configure_root.mkdir(parents=True, exist_ok=True)
                write_windows_subject_native_project_stubs(configure_root)
                (configure_root / "linux-x64-packaging.vcxproj").write_text("<Project />\n", encoding="utf-8")
                return completed

            def refresh_side_effect(
                refresh_repo_root: Path,
                subject_id: str,
                matrix_id: str,
                variant: str,
                *,
                run_id: str | None = None,
            ) -> None:
                del matrix_id, variant
                generated_root = (
                    refresh_repo_root
                    / "artifacts"
                    / "subjects"
                    / subject_id
                    / "runs"
                    / (run_id or "subject-exec")
                    / "analysis"
                    / "generated"
                    / "generated"
                )
                generated_root.mkdir(parents=True, exist_ok=True)
                (generated_root / "native-reference.generated.cpp").write_text("// generated\n", encoding="utf-8")

            with patch.object(workspace_module, "refresh_subject_generated_root", side_effect=refresh_side_effect):
                with patch.object(workspace_module.tooling_module, "cmake_environment", return_value=("cmake", {})):
                    with patch.object(workspace_module, "run_process", side_effect=configure_side_effect):
                        result = workspace_module.generate_subject_workspace(
                            repo_root,
                            "windows",
                            {"id": "subject/FixtureSubject", "all-targets": True},
                        )

            solution_text = (repo_root / result["manifestPath"]).parent.joinpath("FixtureSubject.sln").read_text(encoding="utf-8")
            self.assertIn('"chaos_subject_generated_native (windows-dev-output)"', solution_text)
            self.assertIn('"chaos_subject_generated_native (windows-reference-trace)"', solution_text)
            self.assertIn('"chaos_subject_reference_proof (windows-dev-output)"', solution_text)
            self.assertIn('"chaos_subject_reference_proof (windows-reference-trace)"', solution_text)
            self.assertNotIn(
                'Project("{8BC9CEB8-8B4A-11D0-8D11-00A0C91BC942}") = "chaos_subject_generated_native",',
                solution_text,
            )
            self.assertNotIn(
                'Project("{8BC9CEB8-8B4A-11D0-8D11-00A0C91BC942}") = "chaos_subject_reference_proof",',
                solution_text,
            )
        finally:
            shutil.rmtree(repo_root, ignore_errors=True)

    def test_generate_subject_workspace_assigns_unique_native_project_names_per_matrix(self) -> None:
        workspace_module = load_module(PROJECT_WORKSPACE_MODULE_PATH, "chaos_project_workspace_subject_unique_vcxproj_names")
        repo_root = self._make_repo_root("subject-unique-vcxproj-names")
        self._write_subject_fixture(repo_root)
        completed = subprocess.CompletedProcess(["cmake"], 0, "", "")
        subject_manifest_path = repo_root / "subjects" / "FixtureSubject" / "subject.manifest.json"
        subject_manifest = json.loads(subject_manifest_path.read_text(encoding="utf-8"))
        subject_manifest["environmentMatrices"].append(
            {
                "matrixId": "windows-reference-trace",
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
            }
        )
        write_json(subject_manifest_path, subject_manifest)

        try:
            def refresh_side_effect(
                refresh_repo_root: Path,
                subject_id: str,
                matrix_id: str,
                variant: str,
                *,
                run_id: str | None = None,
            ) -> None:
                del matrix_id, variant
                generated_root = (
                    refresh_repo_root
                    / "artifacts"
                    / "subjects"
                    / subject_id
                    / "runs"
                    / (run_id or "subject-exec")
                    / "analysis"
                    / "generated"
                    / "generated"
                )
                generated_root.mkdir(parents=True, exist_ok=True)
                (generated_root / "native-reference.generated.cpp").write_text("// generated\n", encoding="utf-8")

            with patch.object(workspace_module, "refresh_subject_generated_root", side_effect=refresh_side_effect):
                with patch.object(workspace_module.tooling_module, "cmake_environment", return_value=("cmake", {})):
                    def configure_side_effect(arguments: list[str], cwd: Path, env: dict[str, str] | None = None):
                        del cwd, env
                        configure_root = Path(arguments[arguments.index("-B") + 1])
                        configure_root.mkdir(parents=True, exist_ok=True)
                        write_windows_subject_native_project_stubs(
                            configure_root,
                            generated_text=
                            "<Project><PropertyGroup Label=\"Globals\"><ProjectName>chaos_subject_generated_native</ProjectName></PropertyGroup></Project>\n",
                            proof_text=
                            "<Project><PropertyGroup Label=\"Globals\"><ProjectName>chaos_subject_reference_proof</ProjectName></PropertyGroup></Project>\n",
                        )
                        (configure_root / "linux-x64-packaging.vcxproj").write_text(
                            "<Project><PropertyGroup Label=\"Globals\"><ProjectName>linux-x64-packaging</ProjectName></PropertyGroup></Project>\n",
                            encoding="utf-8",
                        )
                        return completed

                    with patch.object(workspace_module, "run_process", side_effect=configure_side_effect):
                        result = workspace_module.generate_subject_workspace(
                            repo_root,
                            "windows",
                            {"id": "subject/FixtureSubject", "all-targets": True},
                        )

            solution_root = (repo_root / result["manifestPath"]).parent
            generated_dev_output_vcxproj = solution_root / "native" / "windows-dev-output" / "generated" / "chaos_subject_generated_native.vcxproj"
            generated_reference_trace_vcxproj = solution_root / "native" / "windows-reference-trace" / "generated" / "chaos_subject_generated_native.vcxproj"
            proof_dev_output_vcxproj = solution_root / "native" / "windows-dev-output" / "proof" / "chaos_subject_reference_proof.vcxproj"
            proof_reference_trace_vcxproj = solution_root / "native" / "windows-reference-trace" / "proof" / "chaos_subject_reference_proof.vcxproj"

            self.assertIn(
                "<ProjectName>chaos_subject_generated_native_windows_dev_output</ProjectName>",
                generated_dev_output_vcxproj.read_text(encoding="utf-8"),
            )
            self.assertIn(
                "<ProjectName>chaos_subject_generated_native_windows_reference_trace</ProjectName>",
                generated_reference_trace_vcxproj.read_text(encoding="utf-8"),
            )
            self.assertIn(
                "<ProjectName>chaos_subject_reference_proof_windows_dev_output</ProjectName>",
                proof_dev_output_vcxproj.read_text(encoding="utf-8"),
            )
            self.assertIn(
                "<ProjectName>chaos_subject_reference_proof_windows_reference_trace</ProjectName>",
                proof_reference_trace_vcxproj.read_text(encoding="utf-8"),
            )
        finally:
            shutil.rmtree(repo_root, ignore_errors=True)

    def test_generate_subject_workspace_assigns_unique_internal_names_for_full_windows_native_graph(self) -> None:
        workspace_module = load_module(PROJECT_WORKSPACE_MODULE_PATH, "chaos_project_workspace_subject_unique_full_native_graph_names")
        repo_root = self._make_repo_root("subject-unique-full-native-graph-names")
        self._write_subject_fixture(repo_root)
        completed = subprocess.CompletedProcess(["cmake"], 0, "", "")
        subject_manifest_path = repo_root / "subjects" / "FixtureSubject" / "subject.manifest.json"
        subject_manifest = json.loads(subject_manifest_path.read_text(encoding="utf-8"))
        subject_manifest["environmentMatrices"].append(
            {
                "matrixId": "windows-reference-trace",
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
            }
        )
        write_json(subject_manifest_path, subject_manifest)

        try:
            def refresh_side_effect(
                refresh_repo_root: Path,
                subject_id: str,
                matrix_id: str,
                variant: str,
                *,
                run_id: str | None = None,
            ) -> None:
                del matrix_id, variant
                generated_root = (
                    refresh_repo_root
                    / "artifacts"
                    / "subjects"
                    / subject_id
                    / "runs"
                    / (run_id or "subject-exec")
                    / "analysis"
                    / "generated"
                    / "generated"
                )
                generated_root.mkdir(parents=True, exist_ok=True)
                (generated_root / "native-reference.generated.cpp").write_text("// generated\n", encoding="utf-8")

            def configure_side_effect(arguments: list[str], cwd: Path, env: dict[str, str] | None = None):
                del cwd, env
                configure_root = Path(arguments[arguments.index("-B") + 1])
                configure_root.mkdir(parents=True, exist_ok=True)
                if str(configure_root).endswith("windows-linux-buildable"):
                    (configure_root / "linux-x64-packaging.vcxproj").write_text(
                        "<Project><PropertyGroup Label=\"Globals\"><ProjectName>linux-x64-packaging</ProjectName></PropertyGroup></Project>\n",
                        encoding="utf-8",
                    )
                else:
                    write_windows_subject_native_solution_graph_stubs(configure_root)
                return completed

            with patch.object(workspace_module, "refresh_subject_generated_root", side_effect=refresh_side_effect):
                with patch.object(workspace_module.tooling_module, "cmake_environment", return_value=("cmake", {})):
                    with patch.object(workspace_module, "run_process", side_effect=configure_side_effect):
                        result = workspace_module.generate_subject_workspace(
                            repo_root,
                            "windows",
                            {"id": "subject/FixtureSubject", "all-targets": True},
                        )

            solution_root = (repo_root / result["manifestPath"]).parent
            all_build_dev_output = solution_root / "native" / "windows-dev-output" / "ALL_BUILD.vcxproj"
            all_build_reference_trace = solution_root / "native" / "windows-reference-trace" / "ALL_BUILD.vcxproj"
            zero_check_dev_output = solution_root / "native" / "windows-dev-output" / "ZERO_CHECK.vcxproj"
            bootstrap_reference_trace = solution_root / "native" / "windows-reference-trace" / "bootstrap" / "chaos_bootstrap.vcxproj"
            runtime_reference_trace = solution_root / "native" / "windows-reference-trace" / "runtime-core" / "chaos_runtime_core.vcxproj"
            support_reference_trace = solution_root / "native" / "windows-reference-trace" / "support" / "chaos_support.vcxproj"
            proof_run_reference_trace = solution_root / "native" / "windows-reference-trace" / "proof" / "chaos_subject_reference_proof_run.vcxproj"

            self.assertIn("<ProjectName>ALL_BUILD_windows_dev_output</ProjectName>", all_build_dev_output.read_text(encoding="utf-8"))
            self.assertIn(
                "<ProjectName>ALL_BUILD_windows_reference_trace</ProjectName>",
                all_build_reference_trace.read_text(encoding="utf-8"),
            )
            self.assertIn("<ProjectName>ZERO_CHECK_windows_dev_output</ProjectName>", zero_check_dev_output.read_text(encoding="utf-8"))
            self.assertIn(
                "<ProjectName>chaos_bootstrap_windows_reference_trace</ProjectName>",
                bootstrap_reference_trace.read_text(encoding="utf-8"),
            )
            self.assertIn(
                "<ProjectName>chaos_runtime_core_windows_reference_trace</ProjectName>",
                runtime_reference_trace.read_text(encoding="utf-8"),
            )
            self.assertIn(
                "<ProjectName>chaos_support_windows_reference_trace</ProjectName>",
                support_reference_trace.read_text(encoding="utf-8"),
            )
            self.assertIn(
                "<ProjectName>chaos_subject_reference_proof_run_windows_reference_trace</ProjectName>",
                proof_run_reference_trace.read_text(encoding="utf-8"),
            )
        finally:
            shutil.rmtree(repo_root, ignore_errors=True)

    def test_generate_subject_workspace_rewrites_project_reference_names_for_full_windows_native_graph(self) -> None:
        workspace_module = load_module(PROJECT_WORKSPACE_MODULE_PATH, "chaos_project_workspace_subject_unique_full_native_reference_names")
        repo_root = self._make_repo_root("subject-unique-full-native-reference-names")
        self._write_subject_fixture(repo_root)
        completed = subprocess.CompletedProcess(["cmake"], 0, "", "")
        subject_manifest_path = repo_root / "subjects" / "FixtureSubject" / "subject.manifest.json"
        subject_manifest = json.loads(subject_manifest_path.read_text(encoding="utf-8"))
        subject_manifest["environmentMatrices"].append(
            {
                "matrixId": "windows-reference-trace",
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
            }
        )
        write_json(subject_manifest_path, subject_manifest)

        try:
            def refresh_side_effect(
                refresh_repo_root: Path,
                subject_id: str,
                matrix_id: str,
                variant: str,
                *,
                run_id: str | None = None,
            ) -> None:
                del matrix_id, variant
                generated_root = (
                    refresh_repo_root
                    / "artifacts"
                    / "subjects"
                    / subject_id
                    / "runs"
                    / (run_id or "subject-exec")
                    / "analysis"
                    / "generated"
                    / "generated"
                )
                generated_root.mkdir(parents=True, exist_ok=True)
                (generated_root / "native-reference.generated.cpp").write_text("// generated\n", encoding="utf-8")

            def configure_side_effect(arguments: list[str], cwd: Path, env: dict[str, str] | None = None):
                del cwd, env
                configure_root = Path(arguments[arguments.index("-B") + 1])
                configure_root.mkdir(parents=True, exist_ok=True)
                if str(configure_root).endswith("windows-linux-buildable"):
                    (configure_root / "linux-x64-packaging.vcxproj").write_text(
                        "<Project><PropertyGroup Label=\"Globals\"><ProjectName>linux-x64-packaging</ProjectName></PropertyGroup></Project>\n",
                        encoding="utf-8",
                    )
                else:
                    write_windows_subject_native_solution_graph_stubs(configure_root)
                return completed

            with patch.object(workspace_module, "refresh_subject_generated_root", side_effect=refresh_side_effect):
                with patch.object(workspace_module.tooling_module, "cmake_environment", return_value=("cmake", {})):
                    with patch.object(workspace_module, "run_process", side_effect=configure_side_effect):
                        result = workspace_module.generate_subject_workspace(
                            repo_root,
                            "windows",
                            {"id": "subject/FixtureSubject", "all-targets": True},
                        )

            solution_root = (repo_root / result["manifestPath"]).parent
            proof_reference_trace = solution_root / "native" / "windows-reference-trace" / "proof" / "chaos_subject_reference_proof.vcxproj"
            all_build_reference_trace = solution_root / "native" / "windows-reference-trace" / "ALL_BUILD.vcxproj"
            bootstrap_reference_trace = solution_root / "native" / "windows-reference-trace" / "bootstrap" / "chaos_bootstrap.vcxproj"

            self.assertNotIn("<ProjectReference Include=", proof_reference_trace.read_text(encoding="utf-8"))
            self.assertIn(
                "<Name>chaos_bootstrap_windows_reference_trace</Name>",
                all_build_reference_trace.read_text(encoding="utf-8"),
            )
            self.assertIn(
                "<Name>chaos_runtime_core_windows_reference_trace</Name>",
                bootstrap_reference_trace.read_text(encoding="utf-8"),
            )
        finally:
            shutil.rmtree(repo_root, ignore_errors=True)

    def test_generate_subject_workspace_strips_project_reference_includes_from_subject_facing_vcxprojs(self) -> None:
        workspace_module = load_module(PROJECT_WORKSPACE_MODULE_PATH, "chaos_project_workspace_subject_strips_subject_facing_references")
        repo_root = self._make_repo_root("subject-strip-subject-facing-references")
        self._write_subject_fixture(repo_root)
        completed = subprocess.CompletedProcess(["cmake"], 0, "", "")
        subject_manifest_path = repo_root / "subjects" / "FixtureSubject" / "subject.manifest.json"
        subject_manifest = json.loads(subject_manifest_path.read_text(encoding="utf-8"))
        subject_manifest["environmentMatrices"].append(
            {
                "matrixId": "windows-reference-trace",
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
            }
        )
        write_json(subject_manifest_path, subject_manifest)

        try:
            def refresh_side_effect(
                refresh_repo_root: Path,
                subject_id: str,
                matrix_id: str,
                variant: str,
                *,
                run_id: str | None = None,
            ) -> None:
                del matrix_id, variant
                generated_root = (
                    refresh_repo_root
                    / "artifacts"
                    / "subjects"
                    / subject_id
                    / "runs"
                    / (run_id or "subject-exec")
                    / "analysis"
                    / "generated"
                    / "generated"
                )
                generated_root.mkdir(parents=True, exist_ok=True)
                (generated_root / "native-reference.generated.cpp").write_text("// generated\n", encoding="utf-8")

            def configure_side_effect(arguments: list[str], cwd: Path, env: dict[str, str] | None = None):
                del cwd, env
                configure_root = Path(arguments[arguments.index("-B") + 1])
                configure_root.mkdir(parents=True, exist_ok=True)
                if str(configure_root).endswith("windows-linux-buildable"):
                    (configure_root / "linux-x64-packaging.vcxproj").write_text("<Project />\n", encoding="utf-8")
                else:
                    write_windows_subject_native_solution_graph_stubs(configure_root)
                return completed

            with patch.object(workspace_module, "refresh_subject_generated_root", side_effect=refresh_side_effect):
                with patch.object(workspace_module.tooling_module, "cmake_environment", return_value=("cmake", {})):
                    with patch.object(workspace_module, "run_process", side_effect=configure_side_effect):
                        result = workspace_module.generate_subject_workspace(
                            repo_root,
                            "windows",
                            {"id": "subject/FixtureSubject", "all-targets": True},
                        )

            solution_root = (repo_root / result["manifestPath"]).parent
            generated_reference_trace = solution_root / "native" / "windows-reference-trace" / "generated" / "chaos_subject_generated_native.vcxproj"
            proof_reference_trace = solution_root / "native" / "windows-reference-trace" / "proof" / "chaos_subject_reference_proof.vcxproj"
            all_build_reference_trace = solution_root / "native" / "windows-reference-trace" / "ALL_BUILD.vcxproj"

            self.assertNotIn("<ProjectReference Include=", generated_reference_trace.read_text(encoding="utf-8"))
            self.assertNotIn("<ProjectReference Include=", proof_reference_trace.read_text(encoding="utf-8"))
            self.assertIn("<ProjectReference Include=", all_build_reference_trace.read_text(encoding="utf-8"))
        finally:
            shutil.rmtree(repo_root, ignore_errors=True)

    def test_generate_subject_workspace_includes_only_top_level_native_projects_in_solution(self) -> None:
        workspace_module = load_module(PROJECT_WORKSPACE_MODULE_PATH, "chaos_project_workspace_subject_top_level_native_graph")
        repo_root = self._make_repo_root("subject-full-native-graph")
        self._write_subject_fixture(repo_root)
        completed = subprocess.CompletedProcess(["cmake"], 0, "", "")
        subject_manifest_path = repo_root / "subjects" / "FixtureSubject" / "subject.manifest.json"
        subject_manifest = json.loads(subject_manifest_path.read_text(encoding="utf-8"))
        subject_manifest["environmentMatrices"].append(
            {
                "matrixId": "windows-reference-trace",
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
            }
        )
        write_json(subject_manifest_path, subject_manifest)

        try:
            def refresh_side_effect(
                refresh_repo_root: Path,
                subject_id: str,
                matrix_id: str,
                variant: str,
                *,
                run_id: str | None = None,
            ) -> None:
                del matrix_id, variant
                generated_root = (
                    refresh_repo_root
                    / "artifacts"
                    / "subjects"
                    / subject_id
                    / "runs"
                    / (run_id or "subject-exec")
                    / "analysis"
                    / "generated"
                    / "generated"
                )
                generated_root.mkdir(parents=True, exist_ok=True)
                (generated_root / "native-reference.generated.cpp").write_text("// generated\n", encoding="utf-8")

            def configure_side_effect(arguments: list[str], cwd: Path, env: dict[str, str] | None = None):
                del cwd, env
                configure_root = Path(arguments[arguments.index("-B") + 1])
                configure_root.mkdir(parents=True, exist_ok=True)
                if str(configure_root).endswith("windows-linux-buildable"):
                    (configure_root / "linux-x64-packaging.vcxproj").write_text("<Project />\n", encoding="utf-8")
                else:
                    write_windows_subject_native_solution_graph_stubs(configure_root)
                return completed

            with patch.object(workspace_module, "refresh_subject_generated_root", side_effect=refresh_side_effect):
                with patch.object(workspace_module.tooling_module, "cmake_environment", return_value=("cmake", {})):
                    with patch.object(workspace_module, "run_process", side_effect=configure_side_effect):
                        result = workspace_module.generate_subject_workspace(
                            repo_root,
                            "windows",
                            {"id": "subject/FixtureSubject", "all-targets": True},
                        )

            solution_text = (repo_root / result["manifestPath"]).parent.joinpath("FixtureSubject.sln").read_text(encoding="utf-8")
            self.assertIn('"chaos_subject_generated_native (windows-dev-output)"', solution_text)
            self.assertIn('"chaos_subject_reference_proof (windows-dev-output)"', solution_text)
            self.assertIn('"chaos_subject_generated_native (windows-reference-trace)"', solution_text)
            self.assertIn('"chaos_subject_reference_proof (windows-reference-trace)"', solution_text)
            self.assertNotIn('"ALL_BUILD (windows-dev-output)"', solution_text)
            self.assertNotIn('"ALL_BUILD (windows-reference-trace)"', solution_text)
            self.assertNotIn('"ZERO_CHECK (windows-dev-output)"', solution_text)
            self.assertNotIn('"ZERO_CHECK (windows-reference-trace)"', solution_text)
            self.assertNotIn('"chaos_bootstrap (windows-dev-output/bootstrap)"', solution_text)
            self.assertNotIn('"chaos_bootstrap (windows-reference-trace/bootstrap)"', solution_text)
            self.assertNotIn('"chaos_runtime_core (windows-dev-output/runtime-core)"', solution_text)
            self.assertNotIn('"chaos_runtime_core (windows-reference-trace/runtime-core)"', solution_text)
            self.assertNotIn('"chaos_subject_reference_proof_run (windows-dev-output)"', solution_text)
            self.assertNotIn('"chaos_subject_reference_proof_run (windows-reference-trace)"', solution_text)
            self.assertNotIn('"chaos_support (windows-dev-output/support)"', solution_text)
            self.assertNotIn('"chaos_support (windows-reference-trace/support)"', solution_text)
            self.assertNotIn('"native/windows-linux-buildable/linux-x64-packaging.vcxproj"', solution_text)
            project_count = solution_text.count('Project("{')
            folder_count = solution_text.count('Project("{2150E333-8FDC-42A3-9474-1A3956D46DE8}")')
            self.assertEqual(6, project_count - folder_count)
            self.assertIn("NestedProjects", solution_text)
        finally:
            shutil.rmtree(repo_root, ignore_errors=True)

    def test_generate_subject_workspace_uses_native_vcxproj_guids_in_solution(self) -> None:
        workspace_module = load_module(PROJECT_WORKSPACE_MODULE_PATH, "chaos_project_workspace_subject_native_vcxproj_guids")
        repo_root = self._make_repo_root("subject-native-vcxproj-guids")
        self._write_subject_fixture(repo_root)
        completed = subprocess.CompletedProcess(["cmake"], 0, "", "")

        try:
            generated_guid = "{11111111-1111-1111-1111-111111111111}"
            proof_guid = "{22222222-2222-2222-2222-222222222222}"

            def configure_side_effect(arguments: list[str], cwd: Path, env: dict[str, str] | None = None):
                del cwd, env
                configure_root = Path(arguments[arguments.index("-B") + 1])
                configure_root.mkdir(parents=True, exist_ok=True)
                write_windows_subject_native_project_stubs(
                    configure_root,
                    generated_text=
                    f"<Project><PropertyGroup Label=\"Globals\"><ProjectGuid>{generated_guid}</ProjectGuid></PropertyGroup></Project>\n",
                    proof_text=
                    f"<Project><PropertyGroup Label=\"Globals\"><ProjectGuid>{proof_guid}</ProjectGuid></PropertyGroup></Project>\n",
                )
                return completed

            with patch.object(workspace_module.tooling_module, "cmake_environment", return_value=("cmake", {})):
                with patch.object(workspace_module, "run_process", side_effect=configure_side_effect):
                    result = workspace_module.generate_subject_workspace(
                        repo_root,
                        "windows",
                        {"id": "subject/FixtureSubject"},
                    )

            solution_text = (repo_root / result["manifestPath"]).parent.joinpath("FixtureSubject.sln").read_text(encoding="utf-8")
            self.assertIn(f'"native/windows-dev-output/generated/chaos_subject_generated_native.vcxproj", "{generated_guid}"', solution_text)
            self.assertIn(f'"native/windows-dev-output/proof/chaos_subject_reference_proof.vcxproj", "{proof_guid}"', solution_text)
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

            def refresh_side_effect(
                repo_root: Path,
                subject_id: str,
                matrix_id: str,
                variant: str,
                *,
                run_id: str | None = None,
            ) -> None:
                del matrix_id, variant
                generated_root = (
                    repo_root
                    / "artifacts"
                    / "subjects"
                    / subject_id
                    / "runs"
                    / (run_id or "subject-exec")
                    / "analysis"
                    / "generated"
                    / "generated"
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

    def test_generate_subject_workspace_materializes_generic_native_reference_project_when_subject_cmake_missing(self) -> None:
        workspace_module = load_module(PROJECT_WORKSPACE_MODULE_PATH, "chaos_project_workspace_subject_materialized_native_source")
        repo_root = self._make_repo_root("subject-materialized-native-source")
        self._write_subject_fixture(repo_root)
        completed = subprocess.CompletedProcess(["cmake"], 0, "", "")
        proof_root = repo_root / "subjects" / "FixtureSubject" / "validation" / "proof" / "native-reference"
        (proof_root / "CMakeLists.txt").unlink()
        (proof_root / "main.cpp").write_text("int main() { return 0; }\n", encoding="utf-8")

        try:
            def configure_side_effect(arguments: list[str], cwd: Path, env: dict[str, str] | None = None):
                del cwd, env
                configure_root = Path(arguments[arguments.index("-B") + 1])
                write_windows_subject_native_project_stubs(configure_root)
                return completed

            with patch.object(workspace_module.tooling_module, "cmake_environment", return_value=("cmake", {})):
                with patch.object(workspace_module, "run_process", side_effect=configure_side_effect) as run_process_mock:
                    result = workspace_module.generate_subject_workspace(
                        repo_root,
                        "windows",
                        {"id": "subject/FixtureSubject"},
                    )

            materialized_source_root = repo_root / "solutions" / "subjects" / "FixtureSubject" / "native-source" / "windows-dev-output"
            self.assertTrue((materialized_source_root / "CMakeLists.txt").is_file())
            self.assertTrue((materialized_source_root / "generated" / "CMakeLists.txt").is_file())
            self.assertTrue((materialized_source_root / "proof" / "CMakeLists.txt").is_file())
            self.assertTrue((materialized_source_root / "proof" / "RunSubjectProof.cmake").is_file())
            self.assertFalse((materialized_source_root / "SubjectGeneratedNative.cpp.in").exists())
            self.assertIn(
                'add_subdirectory("${REPO_ROOT}/src/native/runtime-core" "runtime-core")',
                (materialized_source_root / "CMakeLists.txt").read_text(encoding="utf-8"),
            )
            self.assertIn(
                'add_subdirectory("${REPO_ROOT}/src/native/support" "support")',
                (materialized_source_root / "CMakeLists.txt").read_text(encoding="utf-8"),
            )
            self.assertIn(
                'add_subdirectory("${REPO_ROOT}/src/native/bootstrap" "bootstrap")',
                (materialized_source_root / "CMakeLists.txt").read_text(encoding="utf-8"),
            )
            self.assertIn(
                "add_subdirectory(generated)",
                (materialized_source_root / "CMakeLists.txt").read_text(encoding="utf-8"),
            )
            self.assertIn(
                "add_subdirectory(proof)",
                (materialized_source_root / "CMakeLists.txt").read_text(encoding="utf-8"),
            )
            self.assertIn(
                str(
                    (
                        repo_root
                        / "solutions"
                        / "subjects"
                        / "FixtureSubject"
                        / "generated"
                        / "subject-exec"
                        / "analysis"
                        / "generated"
                        / "generated"
                        / "native-reference.generated.cpp"
                    ).as_posix()
                ),
                (materialized_source_root / "generated" / "CMakeLists.txt").read_text(encoding="utf-8"),
            )
            self.assertIn(
                "add_library(chaos_subject_generated_native STATIC EXCLUDE_FROM_ALL",
                (materialized_source_root / "generated" / "CMakeLists.txt").read_text(encoding="utf-8"),
            )
            self.assertNotIn(
                "CHAOS_SUBJECT_PROOF_",
                (materialized_source_root / "generated" / "CMakeLists.txt").read_text(encoding="utf-8"),
            )
            self.assertNotIn(
                "RunSubjectProof.cmake",
                (materialized_source_root / "generated" / "CMakeLists.txt").read_text(encoding="utf-8"),
            )
            self.assertIn(
                'set(CHAOS_SUBJECT_PROOF_RUN_SCRIPT "${CMAKE_CURRENT_LIST_DIR}/RunSubjectProof.cmake")',
                (materialized_source_root / "proof" / "CMakeLists.txt").read_text(encoding="utf-8"),
            )
            self.assertEqual(
                [
                    "cmake",
                    "-S",
                    str(materialized_source_root),
                    "-B",
                    str(repo_root / "solutions" / "subjects" / "FixtureSubject" / "native" / "windows-dev-output"),
                    "-G",
                    "Visual Studio 17 2022",
                    f"-DCHAOS_SUBJECT_REPO_ROOT={repo_root.as_posix()}",
                    "-DCHAOS_SUBJECT_VARIANT=CHECK",
                    f"-DCHAOS_SUBJECT_BUILD_OUT_ROOT={repo_root / 'solutions' / 'subjects' / 'FixtureSubject' / 'native' / 'windows-dev-output' / 'out'}",
                    f"-DCHAOS_SUBJECT_RUNTIME_ROOT={repo_root / 'solutions' / 'subjects' / 'FixtureSubject' / 'native' / 'windows-dev-output' / 'runtime'}",
                ],
                run_process_mock.call_args.args[0],
            )
            self.assertTrue((repo_root / result["manifestPath"]).is_file())
        finally:
            shutil.rmtree(repo_root, ignore_errors=True)

    def test_generate_subject_workspace_refreshes_stale_generated_native_reference_output(self) -> None:
        workspace_module = load_module(PROJECT_WORKSPACE_MODULE_PATH, "chaos_project_workspace_subject_refresh_stale_generated")
        repo_root = self._make_repo_root("subject-refresh-stale-generated")
        self._write_subject_fixture(repo_root)
        completed = subprocess.CompletedProcess(["cmake"], 0, "", "")
        generated_source_path = (
            repo_root
            / "artifacts"
            / "subjects"
            / "FixtureSubject"
            / "runs"
            / "subject-exec"
            / "analysis"
            / "generated"
            / "generated"
            / "native-reference.generated.cpp"
        )
        generated_source_path.write_text("int RunNativeReferenceProof() { return 0; }\n", encoding="utf-8")

        try:
            def configure_side_effect(arguments: list[str], cwd: Path, env: dict[str, str] | None = None):
                del cwd, env
                configure_root = Path(arguments[arguments.index("-B") + 1])
                write_windows_subject_native_project_stubs(configure_root)
                return completed

            def refresh_side_effect(
                refresh_repo_root: Path,
                subject_id: str,
                matrix_id: str,
                variant: str,
                *,
                run_id: str | None = None,
            ) -> None:
                self.assertEqual("FixtureSubject", subject_id)
                self.assertEqual("windows-dev-output", matrix_id)
                self.assertEqual("CHECK", variant)
                self.assertEqual("subject-exec", run_id)
                refreshed_generated_root = (
                    refresh_repo_root
                    / "artifacts"
                    / "subjects"
                    / subject_id
                    / "runs"
                    / (run_id or "subject-exec")
                    / "analysis"
                    / "generated"
                    / "generated"
                )
                refreshed_generated_root.mkdir(parents=True, exist_ok=True)
                (refreshed_generated_root / "native-reference.generated.cpp").write_text(
                    "int RunNativeReference() { return 0; }\n",
                    encoding="utf-8",
                )

            with patch.object(workspace_module, "refresh_subject_generated_root", side_effect=refresh_side_effect) as refresh_mock:
                with patch.object(workspace_module.tooling_module, "cmake_environment", return_value=("cmake", {})):
                    with patch.object(workspace_module, "run_process", side_effect=configure_side_effect):
                        result = workspace_module.generate_subject_workspace(
                            repo_root,
                            "windows",
                            {"id": "subject/FixtureSubject"},
                        )

            refresh_mock.assert_called_once()
            mirrored_generated_source = (
                repo_root
                / "solutions"
                / "subjects"
                / "FixtureSubject"
                / "generated"
                / "subject-exec"
                / "analysis"
                / "generated"
                / "generated"
                / "native-reference.generated.cpp"
            )
            self.assertEqual("int RunNativeReference() { return 0; }\n", mirrored_generated_source.read_text(encoding="utf-8"))
            self.assertTrue((repo_root / result["manifestPath"]).is_file())
        finally:
            shutil.rmtree(repo_root, ignore_errors=True)

    def test_generate_subject_workspace_refreshes_and_mirrors_generated_source_per_matrix_when_all_targets_requested(self) -> None:
        workspace_module = load_module(PROJECT_WORKSPACE_MODULE_PATH, "chaos_project_workspace_subject_per_matrix_generated_source")
        repo_root = self._make_repo_root("subject-per-matrix-generated-source")
        self._write_subject_fixture(repo_root)
        completed = subprocess.CompletedProcess(["cmake"], 0, "", "")
        subject_manifest_path = repo_root / "subjects" / "FixtureSubject" / "subject.manifest.json"
        subject_manifest = json.loads(subject_manifest_path.read_text(encoding="utf-8"))
        subject_manifest["environmentMatrices"].append(
            {
                "matrixId": "windows-reference-trace",
                "pipelineId": "proof-runtime-output",
                "supportedGoals": ["correctness.dev"],
                "source": {
                    "entry": "FixtureSubject/TraceEntry::Run()",
                },
                "executionContext": {
                    "hostPlatform": "windows-x64",
                    "targetPlatform": "windows-x64",
                    "toolchainProfile": "msvc-reference",
                },
                "artifactPlan": {
                    "evidenceTerminalBucket": "runtime",
                },
            }
        )
        write_json(subject_manifest_path, subject_manifest)

        try:
            def refresh_side_effect(
                refresh_repo_root: Path,
                subject_id: str,
                matrix_id: str,
                variant: str,
                *,
                run_id: str | None = None,
            ) -> None:
                del variant
                generated_root = (
                    refresh_repo_root
                    / "artifacts"
                    / "subjects"
                    / subject_id
                    / "runs"
                    / (run_id or "subject-exec")
                    / "analysis"
                    / "generated"
                )
                (generated_root / "generated").mkdir(parents=True, exist_ok=True)
                (generated_root / "generated.manifest.json").write_text(
                    json.dumps({"matrixId": matrix_id}, indent=2) + "\n",
                    encoding="utf-8",
                )
                (generated_root / "native-reference.plan.json").write_text(
                    json.dumps({"matrixId": matrix_id}, indent=2) + "\n",
                    encoding="utf-8",
                )
                (generated_root / "generated" / "native-reference.generated.cpp").write_text(
                    f"// generated for {matrix_id}\n",
                    encoding="utf-8",
                )

            def configure_side_effect(arguments: list[str], cwd: Path, env: dict[str, str] | None = None):
                del cwd, env
                configure_root = Path(arguments[arguments.index("-B") + 1])
                configure_root.mkdir(parents=True, exist_ok=True)
                write_windows_subject_native_project_stubs(configure_root)
                return completed

            with patch.object(workspace_module, "refresh_subject_generated_root", side_effect=refresh_side_effect) as refresh_mock:
                with patch.object(workspace_module.tooling_module, "cmake_environment", return_value=("cmake", {})):
                    with patch.object(workspace_module, "run_process", side_effect=configure_side_effect):
                        result = workspace_module.generate_subject_workspace(
                            repo_root,
                            "windows",
                            {"id": "subject/FixtureSubject", "all-targets": True, "refresh-generated": True},
                        )

            self.assertEqual(
                ["windows-dev-output", "windows-reference-trace"],
                [call.args[2] for call in refresh_mock.call_args_list],
            )
            self.assertEqual(
                [
                    workspace_module._subject_generated_run_id("windows-dev-output"),
                    workspace_module._subject_generated_run_id("windows-reference-trace"),
                ],
                [call.kwargs["run_id"] for call in refresh_mock.call_args_list],
            )

            manifest = json.loads((repo_root / result["manifestPath"]).read_text(encoding="utf-8"))
            matrices_by_id = {matrix["matrixId"]: matrix for matrix in manifest["matrices"]}

            self.assertEqual(
                "solutions/subjects/FixtureSubject/generated/windows-dev-output/subject-exec",
                matrices_by_id["windows-dev-output"]["generatedRoot"],
            )
            self.assertEqual(
                "solutions/subjects/FixtureSubject/generated/windows-reference-trace/subject-exec",
                matrices_by_id["windows-reference-trace"]["generatedRoot"],
            )

            dev_generated_source = (
                repo_root
                / matrices_by_id["windows-dev-output"]["generatedRoot"]
                / "analysis"
                / "generated"
                / "generated"
                / "native-reference.generated.cpp"
            )
            trace_generated_source = (
                repo_root
                / matrices_by_id["windows-reference-trace"]["generatedRoot"]
                / "analysis"
                / "generated"
                / "generated"
                / "native-reference.generated.cpp"
            )
            self.assertEqual("// generated for windows-dev-output\n", dev_generated_source.read_text(encoding="utf-8"))
            self.assertEqual("// generated for windows-reference-trace\n", trace_generated_source.read_text(encoding="utf-8"))
            self.assertIn(
                "generated/windows-dev-output/subject-exec/analysis/generated/generated/native-reference.generated.cpp",
                (
                    repo_root
                    / "solutions"
                    / "subjects"
                    / "FixtureSubject"
                    / "native-source"
                    / "windows-dev-output"
                    / "generated"
                    / "CMakeLists.txt"
                ).read_text(encoding="utf-8"),
            )
            self.assertIn(
                "generated/windows-reference-trace/subject-exec/analysis/generated/generated/native-reference.generated.cpp",
                (
                    repo_root
                    / "solutions"
                    / "subjects"
                    / "FixtureSubject"
                    / "native-source"
                    / "windows-reference-trace"
                    / "generated"
                    / "CMakeLists.txt"
                ).read_text(encoding="utf-8"),
            )
        finally:
            shutil.rmtree(repo_root, ignore_errors=True)

    def test_build_subject_workspace_supports_generated_native_target_selection(self) -> None:
        workspace_module = load_module(PROJECT_WORKSPACE_MODULE_PATH, "chaos_project_workspace_subject_build_generated_target")
        repo_root = self._make_repo_root("subject-build-generated-target")
        self._write_subject_fixture(repo_root)
        workspace_root = repo_root / "solutions" / "subjects" / "FixtureSubject"
        build_root = workspace_root / "native" / "windows-dev-output"
        build_root.mkdir(parents=True, exist_ok=True)
        manifest_path = workspace_root / "workspace.manifest.json"
        write_json(
            manifest_path,
            {
                "kind": "subject-workspace",
                "subjectId": "FixtureSubject",
                "defaultMatrix": "windows-dev-output",
                "managedSolutionPath": "solutions/subjects/FixtureSubject/FixtureSubject.sln",
                "managedProjects": [
                    "subjects/FixtureSubject/source/FixtureSubject.csproj",
                    "subjects/FixtureSubject/validation/unit/FixtureSubject.Subject.UnitTests/FixtureSubject.Subject.UnitTests.csproj",
                ],
                "matrices": [
                    {
                        "matrixId": "windows-dev-output",
                        "targetPlatform": "windows-x64",
                        "configureRoot": "solutions/subjects/FixtureSubject/native/windows-dev-output",
                        "buildArgs": ["--config", "Release", "--target", "chaos_subject_reference_proof"],
                        "defaultBuildNativeProject": "chaos_subject_reference_proof",
                        "nativeProjects": [
                            {
                                "targetId": "chaos_subject_generated_native",
                                "kind": "generated-native",
                                "projectPath": "solutions/subjects/FixtureSubject/native/windows-dev-output/generated/chaos_subject_generated_native.vcxproj",
                                "buildArgs": ["--config", "Release", "--target", "chaos_subject_generated_native"],
                            },
                            {
                                "targetId": "chaos_subject_reference_proof",
                                "kind": "proof-native",
                                "projectPath": "solutions/subjects/FixtureSubject/native/windows-dev-output/proof/chaos_subject_reference_proof.vcxproj",
                                "buildArgs": ["--config", "Release", "--target", "chaos_subject_reference_proof"],
                            },
                        ],
                    }
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
                        {"id": "subject/FixtureSubject", "matrix": "windows-dev-output", "native-target": "generated"},
                    )

            self.assertEqual("solutions/subjects/FixtureSubject/build.report.json", result["buildReportPath"])
            self.assertEqual(
                [
                    "cmake",
                    "--build",
                    str(build_root),
                    "--config",
                    "Release",
                    "--target",
                    "chaos_subject_generated_native",
                ],
                run_process_mock.call_args_list[2].args[0],
            )
        finally:
            shutil.rmtree(repo_root, ignore_errors=True)

    def test_build_subject_workspace_uses_manifest_and_selected_matrix(self) -> None:
        workspace_module = load_module(PROJECT_WORKSPACE_MODULE_PATH, "chaos_project_workspace_subject_build")
        repo_root = self._make_repo_root("subject-build")
        self._write_subject_fixture(repo_root)
        workspace_root = repo_root / "solutions" / "subjects" / "FixtureSubject"
        build_root = workspace_root / "native" / "windows-dev-output"
        build_root.mkdir(parents=True, exist_ok=True)
        manifest_path = workspace_root / "workspace.manifest.json"
        write_json(
            manifest_path,
            {
                "kind": "subject-workspace",
                "subjectId": "FixtureSubject",
                "managedSolutionPath": "solutions/subjects/FixtureSubject/FixtureSubject.sln",
                "managedProjects": [
                    "subjects/FixtureSubject/source/FixtureSubject.csproj",
                    "subjects/FixtureSubject/validation/unit/FixtureSubject.Subject.UnitTests/FixtureSubject.Subject.UnitTests.csproj",
                ],
                "matrices": [
                    {
                        "matrixId": "windows-dev-output",
                        "configureRoot": "solutions/subjects/FixtureSubject/native/windows-dev-output",
                        "buildArgs": ["--config", "Release", "--target", "chaos_subject_reference_proof"],
                    },
                    {
                        "matrixId": "windows-linux-buildable",
                        "configureRoot": "solutions/subjects/FixtureSubject/native/windows-linux-buildable",
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

            self.assertEqual("solutions/subjects/FixtureSubject/build.report.json", result["buildReportPath"])
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
            self.assertEqual("solutions/core/windows/chaos-il2cpp-core.sln", manifest["managedSolutionPath"])
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
            self.assertEqual(
                [
                    "solutions/core/windows/native/windows-x64-reference",
                    "solutions/core/windows/native/android-arm64",
                    "solutions/core/windows/native/linux-x64",
                ],
                [item["configureRoot"] for item in manifest["nativeTargets"]],
            )
            self.assertEqual(3, run_process_mock.call_count)
        finally:
            shutil.rmtree(repo_root, ignore_errors=True)

    def test_build_core_workspace_uses_manifest_and_selected_target(self) -> None:
        workspace_module = load_module(PROJECT_WORKSPACE_MODULE_PATH, "chaos_project_workspace_core_build")
        repo_root = self._make_repo_root("core-build")
        self._write_core_fixture(repo_root)
        workspace_root = repo_root / "solutions" / "core" / "windows"
        (workspace_root / "native" / "windows-x64-reference").mkdir(parents=True, exist_ok=True)
        manifest_path = workspace_root / "workspace.manifest.json"
        write_json(
            manifest_path,
            {
                "kind": "core-workspace",
                "hostPlatform": "windows",
                "managedSolutionPath": "solutions/core/windows/chaos-il2cpp-core.sln",
                "managedProjects": [
                    "src/managed/Chaos.IL2CPP.Driver/Chaos.IL2CPP.Driver.csproj",
                    "src/managed/Chaos.IL2CPP.CodeGen/Chaos.IL2CPP.CodeGen.csproj",
                ],
                "nativeTargets": [
                    {
                        "targetId": "windows-x64-reference",
                        "configureRoot": "solutions/core/windows/native/windows-x64-reference",
                        "buildArgs": ["--config", "Release"],
                    },
                    {
                        "targetId": "linux-x64",
                        "configureRoot": "solutions/core/windows/native/linux-x64",
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

            self.assertEqual("solutions/core/windows/build.report.json", result["buildReportPath"])
            report = json.loads((repo_root / result["buildReportPath"]).read_text(encoding="utf-8"))
            self.assertEqual(["windows-x64-reference"], report["builtTargets"])
            self.assertEqual(
                [
                    "cmake",
                    "--build",
                    str(repo_root / "solutions" / "core" / "windows" / "native" / "windows-x64-reference"),
                    "--config",
                    "Release",
                ],
                run_process_mock.call_args_list[2].args[0],
            )
        finally:
            shutil.rmtree(repo_root, ignore_errors=True)

    def test_generate_all_workspaces_writes_root_manifest_and_generation_report(self) -> None:
        workspace_module = load_module(PROJECT_WORKSPACE_MODULE_PATH, "chaos_project_workspace_all_generate")
        repo_root = self._make_repo_root("all-generate")
        self._write_subject_fixture(repo_root)
        self._write_core_fixture(repo_root)
        completed = subprocess.CompletedProcess(["cmake"], 0, "", "")

        try:
            def configure_side_effect(arguments: list[str], cwd: Path, env: dict[str, str] | None = None):
                del cwd, env
                configure_root = Path(arguments[arguments.index("-B") + 1])
                (configure_root / "chaos_subject_reference_proof.vcxproj").parent.mkdir(parents=True, exist_ok=True)
                (configure_root / "chaos_subject_reference_proof.vcxproj").write_text("<Project />\n", encoding="utf-8")
                return completed

            with patch.object(workspace_module, "refresh_subject_generated_root", return_value=None):
                with patch.object(workspace_module.tooling_module, "cmake_environment", return_value=("cmake", {})):
                    with patch.object(workspace_module, "run_process", side_effect=configure_side_effect):
                        result = workspace_module.generate_all_workspaces(
                            repo_root,
                            "windows",
                            {"host": "windows"},
                        )

            self.assertEqual("solutions/manifest.json", result["manifestPath"])
            aggregate_manifest = json.loads((repo_root / "solutions" / "manifest.json").read_text(encoding="utf-8"))
            generation_report = json.loads((repo_root / "solutions" / "all" / "generation.report.json").read_text(encoding="utf-8"))
            self.assertEqual("all-workspaces", aggregate_manifest["kind"])
            self.assertEqual("windows", aggregate_manifest["hostPlatform"])
            self.assertEqual(
                ["solutions/subjects/FixtureSubject/workspace.manifest.json"],
                aggregate_manifest["subjectWorkspaceManifests"],
            )
            self.assertEqual(
                "solutions/core/windows/workspace.manifest.json",
                aggregate_manifest["coreWorkspaceManifest"],
            )
            self.assertEqual(["FixtureSubject"], generation_report["generatedSubjectIds"])
            self.assertEqual(
                ["windows-x64-reference", "android-arm64", "linux-x64"],
                generation_report["generatedCoreTargets"],
            )
            self.assertEqual("completed", aggregate_manifest["status"])
            self.assertEqual("completed", generation_report["status"])
        finally:
            shutil.rmtree(repo_root, ignore_errors=True)

    def test_generate_all_workspaces_materializes_root_outputs_before_subject_generation(self) -> None:
        workspace_module = load_module(PROJECT_WORKSPACE_MODULE_PATH, "chaos_project_workspace_all_early_root_outputs")
        repo_root = self._make_repo_root("all-early-root-outputs")
        self._write_subject_fixture(repo_root)
        self._write_core_fixture(repo_root)

        try:
            def subject_side_effect(
                subject_repo_root: Path,
                subject_host_platform: str,
                options: dict[str, object],
            ) -> dict[str, object]:
                del subject_host_platform, options
                aggregate_manifest_path = subject_repo_root / "solutions" / "manifest.json"
                generation_report_path = subject_repo_root / "solutions" / "all" / "generation.report.json"

                self.assertTrue(aggregate_manifest_path.is_file())
                self.assertTrue(generation_report_path.is_file())

                aggregate_manifest = json.loads(aggregate_manifest_path.read_text(encoding="utf-8"))
                generation_report = json.loads(generation_report_path.read_text(encoding="utf-8"))
                self.assertEqual("running", aggregate_manifest["status"])
                self.assertEqual("running", generation_report["status"])
                self.assertEqual([], aggregate_manifest["subjectWorkspaceManifests"])
                self.assertEqual("", aggregate_manifest["coreWorkspaceManifest"])
                self.assertEqual([], generation_report["generatedSubjectIds"])
                self.assertEqual([], generation_report["generatedCoreTargets"])

                subject_manifest_path = subject_repo_root / "solutions" / "subjects" / "FixtureSubject" / "workspace.manifest.json"
                write_json(subject_manifest_path, {"kind": "subject-workspace"})
                return {
                    "manifestPath": "solutions/subjects/FixtureSubject/workspace.manifest.json",
                    "artifacts": ["solutions/subjects/FixtureSubject/workspace.manifest.json"],
                    "importantOutputs": [],
                    "consoleText": "",
                }

            def core_side_effect(
                core_repo_root: Path,
                core_host_platform: str,
                options: dict[str, object],
            ) -> dict[str, object]:
                del core_host_platform, options
                core_manifest_path = core_repo_root / "solutions" / "core" / "windows" / "workspace.manifest.json"
                write_json(
                    core_manifest_path,
                    {
                        "kind": "core-workspace",
                        "nativeTargets": [{"targetId": "windows-x64-reference"}],
                    },
                )
                return {
                    "manifestPath": "solutions/core/windows/workspace.manifest.json",
                    "artifacts": ["solutions/core/windows/workspace.manifest.json"],
                    "importantOutputs": [],
                    "consoleText": "",
                }

            with patch.object(workspace_module, "generate_subject_workspace", side_effect=subject_side_effect):
                with patch.object(workspace_module, "generate_core_workspace", side_effect=core_side_effect):
                    result = workspace_module.generate_all_workspaces(
                        repo_root,
                        "windows",
                        {"host": "windows"},
                    )

            self.assertEqual("solutions/manifest.json", result["manifestPath"])
            aggregate_manifest = json.loads((repo_root / "solutions" / "manifest.json").read_text(encoding="utf-8"))
            generation_report = json.loads((repo_root / "solutions" / "all" / "generation.report.json").read_text(encoding="utf-8"))
            self.assertEqual("completed", aggregate_manifest["status"])
            self.assertEqual("completed", generation_report["status"])
            self.assertEqual(
                ["solutions/subjects/FixtureSubject/workspace.manifest.json"],
                aggregate_manifest["subjectWorkspaceManifests"],
            )
            self.assertEqual("solutions/core/windows/workspace.manifest.json", aggregate_manifest["coreWorkspaceManifest"])
            self.assertEqual(["FixtureSubject"], generation_report["generatedSubjectIds"])
            self.assertEqual(["windows-x64-reference"], generation_report["generatedCoreTargets"])
        finally:
            shutil.rmtree(repo_root, ignore_errors=True)

    def test_generate_all_workspaces_refreshes_missing_subject_generated_when_needed(self) -> None:
        workspace_module = load_module(PROJECT_WORKSPACE_MODULE_PATH, "chaos_project_workspace_all_refresh_missing")
        repo_root = self._make_repo_root("all-refresh-missing")
        self._write_subject_fixture(repo_root)
        self._write_core_fixture(repo_root)
        completed = subprocess.CompletedProcess(["cmake"], 0, "", "")
        generated_source_path = (
            repo_root
            / "artifacts"
            / "subjects"
            / "FixtureSubject"
            / "runs"
            / "subject-exec"
            / "analysis"
            / "generated"
            / "generated"
            / "native-reference.generated.cpp"
        )
        generated_source_path.unlink()

        try:
            def configure_side_effect(arguments: list[str], cwd: Path, env: dict[str, str] | None = None):
                del cwd, env
                configure_root = Path(arguments[arguments.index("-B") + 1])
                (configure_root / "chaos_subject_reference_proof.vcxproj").parent.mkdir(parents=True, exist_ok=True)
                (configure_root / "chaos_subject_reference_proof.vcxproj").write_text("<Project />\n", encoding="utf-8")
                return completed

            def refresh_side_effect(
                repo_root: Path,
                subject_id: str,
                matrix_id: str,
                variant: str,
                *,
                run_id: str | None = None,
            ) -> None:
                self.assertEqual("FixtureSubject", subject_id)
                self.assertEqual("windows-dev-output", matrix_id)
                self.assertEqual("CHECK", variant)
                self.assertEqual("subject-exec", run_id)
                generated_root = (
                    repo_root
                    / "artifacts"
                    / "subjects"
                    / subject_id
                    / "runs"
                    / (run_id or "subject-exec")
                    / "analysis"
                    / "generated"
                    / "generated"
                )
                generated_root.mkdir(parents=True, exist_ok=True)
                (generated_root / "native-reference.generated.cpp").write_text("// generated\n", encoding="utf-8")

            with patch.object(workspace_module, "refresh_subject_generated_root", side_effect=refresh_side_effect) as refresh_mock:
                with patch.object(workspace_module.tooling_module, "cmake_environment", return_value=("cmake", {})):
                    with patch.object(workspace_module, "run_process", side_effect=configure_side_effect):
                        result = workspace_module.generate_all_workspaces(
                            repo_root,
                            "windows",
                            {"host": "windows"},
                        )

            self.assertEqual("solutions/manifest.json", result["manifestPath"])
            self.assertTrue((repo_root / "solutions" / "manifest.json").is_file())
            refresh_mock.assert_called_once()
        finally:
            shutil.rmtree(repo_root, ignore_errors=True)

    def test_generate_all_workspaces_skips_subjects_without_workspace_generation_prerequisites(self) -> None:
        workspace_module = load_module(PROJECT_WORKSPACE_MODULE_PATH, "chaos_project_workspace_all_skip_unsupported")
        repo_root = self._make_repo_root("all-skip-unsupported")
        self._write_subject_fixture(repo_root)
        self._write_managed_only_subject_fixture(repo_root)
        self._write_core_fixture(repo_root)
        completed = subprocess.CompletedProcess(["cmake"], 0, "", "")

        try:
            def configure_side_effect(arguments: list[str], cwd: Path, env: dict[str, str] | None = None):
                del cwd, env
                configure_root = Path(arguments[arguments.index("-B") + 1])
                (configure_root / "chaos_subject_reference_proof.vcxproj").parent.mkdir(parents=True, exist_ok=True)
                (configure_root / "chaos_subject_reference_proof.vcxproj").write_text("<Project />\n", encoding="utf-8")
                return completed

            with patch.object(workspace_module, "refresh_subject_generated_root", return_value=None):
                with patch.object(workspace_module.tooling_module, "cmake_environment", return_value=("cmake", {})):
                    with patch.object(workspace_module, "run_process", side_effect=configure_side_effect):
                        result = workspace_module.generate_all_workspaces(
                            repo_root,
                            "windows",
                            {"host": "windows"},
                        )

            aggregate_manifest = json.loads((repo_root / result["manifestPath"]).read_text(encoding="utf-8"))
            generation_report = json.loads((repo_root / "solutions" / "all" / "generation.report.json").read_text(encoding="utf-8"))
            self.assertEqual(
                ["solutions/subjects/FixtureSubject/workspace.manifest.json"],
                aggregate_manifest["subjectWorkspaceManifests"],
            )
            self.assertEqual(["FixtureSubject"], generation_report["generatedSubjectIds"])
        finally:
            shutil.rmtree(repo_root, ignore_errors=True)

    def test_refresh_subject_generated_root_accepts_generated_stage_variants(self) -> None:
        workspace_module = load_module(PROJECT_WORKSPACE_MODULE_PATH, "chaos_project_workspace_refresh_generated_stage_variants")
        repo_root = self._make_repo_root("refresh-generated-stage-variants")
        self._write_subject_fixture(repo_root)

        try:
            plan = {
                "stagePlan": [
                    {"stageId": "source-resolve", "kind": "source-resolve", "bucket": "source"},
                    {"stageId": "analysis-frontend", "kind": "analysis-frontend", "bucket": "analysis"},
                    {"stageId": "generated-engine-proof", "kind": "generated-engine-proof", "bucket": "generated"},
                    {"stageId": "runtime-engine-observe", "kind": "runtime-engine-observe", "bucket": "runtime"},
                ],
            }

            with patch.object(workspace_module.subject_planner_module, "build_plan", return_value=plan):
                with patch.object(
                    workspace_module.subject_executor_module,
                    "execute_plan",
                    return_value={"status": "ok"},
                ) as execute_mock:
                    workspace_module.refresh_subject_generated_root(
                        repo_root,
                        "FixtureSubject",
                        "windows-dev-output",
                        "CHECK",
                    )

            executed_plan = execute_mock.call_args.args[1]
            self.assertEqual(
                ["source-resolve", "analysis-frontend", "generated-engine-proof"],
                [stage["kind"] for stage in executed_plan["stagePlan"]],
            )
        finally:
            shutil.rmtree(repo_root, ignore_errors=True)

    def test_refresh_subject_generated_root_clears_existing_run_root_before_planning(self) -> None:
        workspace_module = load_module(PROJECT_WORKSPACE_MODULE_PATH, "chaos_project_workspace_refresh_generated_root_clears_run_root")
        repo_root = self._make_repo_root("refresh-generated-root-clears-run-root")
        self._write_subject_fixture(repo_root)
        run_id = "subject-exec-windows_native_check"
        stale_manifest_path = (
            repo_root
            / "artifacts"
            / "subjects"
            / "FixtureSubject"
            / "runs"
            / run_id
            / "analysis"
            / "source"
            / "source.manifest.json"
        )
        stale_manifest_path.parent.mkdir(parents=True, exist_ok=True)
        stale_manifest_path.write_text("{}\n", encoding="utf-8")

        try:
            plan = {
                "stagePlan": [
                    {"stageId": "source-resolve", "kind": "source-resolve", "bucket": "source"},
                    {"stageId": "analysis-frontend", "kind": "analysis-frontend", "bucket": "analysis"},
                    {"stageId": "generated-native-proof", "kind": "generated-native-proof", "bucket": "generated"},
                ],
            }

            def build_plan_side_effect(*args, **kwargs):
                self.assertFalse(stale_manifest_path.exists())
                self.assertEqual(run_id, kwargs["run_id"])
                return plan

            with patch.object(workspace_module.subject_planner_module, "build_plan", side_effect=build_plan_side_effect):
                with patch.object(
                    workspace_module.subject_executor_module,
                    "execute_plan",
                    return_value={"status": "ok"},
                ):
                    workspace_module.refresh_subject_generated_root(
                        repo_root,
                        "FixtureSubject",
                        "windows-dev-output",
                        "CHECK",
                        run_id=run_id,
                    )
        finally:
            shutil.rmtree(repo_root, ignore_errors=True)


if __name__ == "__main__":
    unittest.main()
