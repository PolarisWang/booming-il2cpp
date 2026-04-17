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
SUBJECT_TEMPLATES_ROOT = REPO_ROOT / "build" / "toolchains" / "run" / "subject" / "templates"
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
    benchmark_text: str | None = None,
) -> None:
    generated_project = configure_root / "generated" / "chaos_subject_generated_native.vcxproj"
    proof_project = configure_root / "proof" / "chaos_subject_reference_proof.vcxproj"
    generated_project.parent.mkdir(parents=True, exist_ok=True)
    proof_project.parent.mkdir(parents=True, exist_ok=True)
    generated_project.write_text(generated_text, encoding="utf-8")
    proof_project.write_text(proof_text, encoding="utf-8")
    if benchmark_text is not None:
        benchmark_project = configure_root / "benchmark" / "chaos_subject_native_aot.vcxproj"
        benchmark_project.parent.mkdir(parents=True, exist_ok=True)
        benchmark_project.write_text(benchmark_text, encoding="utf-8")


def declared_catalog_fixture(subject_id: str = "FixtureSubject") -> dict:
    return {
        "subjectId": subject_id,
        "frameworkReferenced": True,
        "subjectKind": "declared-test",
        "warningCodes": [],
        "declaredUnitTests": [
            {
                "stableId": f"{subject_id}::FixtureSubject::FixtureSubject.Proofs::Verify()",
                "entryIndex": 0,
                "alias": "fixture-proof",
                "assemblyName": subject_id,
                "declaringType": "FixtureSubject.Proofs",
                "methodName": "Verify",
                "methodSignature": "Verify()",
                "category": 1,
                "capabilityFamily": 1,
                "capabilityItem": 1,
                "archetype": 1,
                "hotUpdateCapability": 0,
                "requires": 0,
                "evidence": 1,
                "priority": 1,
            }
        ],
        "declaredBenchmarks": [
            {
                "stableId": f"{subject_id}::FixtureSubject::FixtureSubject.Benchmarks::Run()",
                "entryIndex": 0,
                "alias": "fixture-benchmark",
                "assemblyName": subject_id,
                "declaringType": "FixtureSubject.Benchmarks",
                "methodName": "Run",
                "methodSignature": "Run()",
                "category": 1,
                "capabilityFamily": 1,
                "capabilityItem": 1,
                "archetype": 1,
                "hotUpdateCapability": 0,
                "requires": 0,
                "metrics": 1,
                "modes": 1,
                "warmupCount": 2,
                "iterationCount": 5,
                "invocationCount": 10,
            }
        ],
    }


def hotupdate_declared_catalog_fixture(subject_id: str = "FixtureHotUpdateSubject") -> dict:
    patch_assembly_name = f"{subject_id}.Patch"
    return {
        "subjectId": subject_id,
        "frameworkReferenced": True,
        "subjectKind": "declared-test",
        "warningCodes": [],
        "declaredUnitTests": [
            {
                "stableId": f"{subject_id}::{patch_assembly_name}::{patch_assembly_name}.Proofs::Verify()",
                "entryIndex": 0,
                "alias": "hotupdate-proof",
                "assemblyName": patch_assembly_name,
                "declaringType": f"{patch_assembly_name}.Proofs",
                "methodName": "Verify",
                "methodSignature": "Verify()",
                "category": 5,
                "capabilityFamily": 13,
                "capabilityItem": 54,
                "archetype": 8,
                "hotUpdateCapability": 1,
                "requires": 32,
                "evidence": 1,
                "priority": 1,
            }
        ],
        "declaredBenchmarks": [
            {
                "stableId": f"{subject_id}::{patch_assembly_name}::{patch_assembly_name}.Benchmarks::RunWorkload()",
                "entryIndex": 0,
                "alias": "hotupdate-benchmark",
                "assemblyName": patch_assembly_name,
                "declaringType": f"{patch_assembly_name}.Benchmarks",
                "methodName": "RunWorkload",
                "methodSignature": "RunWorkload()",
                "category": 4,
                "capabilityFamily": 13,
                "capabilityItem": 54,
                "archetype": 8,
                "hotUpdateCapability": 1,
                "requires": 32,
                "metrics": 1,
                "modes": 5,
                "warmupCount": 1,
                "iterationCount": 5,
                "invocationCount": 1,
            }
        ],
    }


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
    def test_native_workspace_materialization_is_backed_by_template_assets(self) -> None:
        module_source = PROJECT_WORKSPACE_MODULE_PATH.read_text(encoding="utf-8")

        self.assertIn("templates/native-reference-workspace.cmake.tmpl", module_source)
        self.assertIn("templates/native-generated.cmake.tmpl", module_source)
        self.assertIn("templates/native-proof.cmake.tmpl", module_source)
        self.assertIn("templates/native-proof-main.cpp.tmpl", module_source)
        self.assertIn("templates/native-proof-run.cmake.tmpl", module_source)
        self.assertIn("templates/native-aot-workspace.cmake.tmpl", module_source)
        self.assertIn("templates/native-benchmark.cmake.tmpl", module_source)
        self.assertTrue((SUBJECT_TEMPLATES_ROOT / "native-reference-workspace.cmake.tmpl").is_file())
        self.assertTrue((SUBJECT_TEMPLATES_ROOT / "native-generated.cmake.tmpl").is_file())
        self.assertTrue((SUBJECT_TEMPLATES_ROOT / "native-proof.cmake.tmpl").is_file())
        self.assertTrue((SUBJECT_TEMPLATES_ROOT / "native-proof-main.cpp.tmpl").is_file())
        self.assertTrue((SUBJECT_TEMPLATES_ROOT / "native-proof-run.cmake.tmpl").is_file())
        self.assertTrue((SUBJECT_TEMPLATES_ROOT / "native-aot-workspace.cmake.tmpl").is_file())
        self.assertTrue((SUBJECT_TEMPLATES_ROOT / "native-benchmark.cmake.tmpl").is_file())

    @classmethod
    def setUpClass(cls) -> None:
        TEST_TMP_ROOT.mkdir(parents=True, exist_ok=True)

    def _make_repo_root(self, prefix: str) -> Path:
        repo_root = TEST_TMP_ROOT / f"{prefix}-{uuid.uuid4().hex}"
        repo_root.mkdir(parents=True, exist_ok=False)
        return repo_root

    def _write_subject_fixture(
        self,
        repo_root: Path,
        *,
        subject_id: str = "FixtureSubject",
        include_legacy_native_reference: bool = True,
    ) -> None:
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
        if include_legacy_native_reference:
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

    def _write_hotupdate_subject_fixture(
        self,
        repo_root: Path,
        *,
        subject_id: str = "FixtureHotUpdateSubject",
    ) -> None:
        source_root = repo_root / "subjects" / subject_id / "source"
        host_project = source_root / "Host" / f"{subject_id}.Host.csproj"
        patch_project = source_root / "Patch" / f"{subject_id}.Patch.csproj"
        shared_project = source_root / "Shared" / f"{subject_id}.Shared.csproj"
        solution_path = source_root / f"{subject_id}.sln"

        host_project.parent.mkdir(parents=True, exist_ok=True)
        patch_project.parent.mkdir(parents=True, exist_ok=True)
        shared_project.parent.mkdir(parents=True, exist_ok=True)
        host_project.write_text(
            "\n".join(
                [
                    "<Project>",
                    "  <PropertyGroup>",
                    f"    <AssemblyName>{subject_id}.Host</AssemblyName>",
                    "  </PropertyGroup>",
                    "</Project>",
                    "",
                ]
            ),
            encoding="utf-8",
        )
        patch_project.write_text(
            "\n".join(
                [
                    "<Project>",
                    "  <PropertyGroup>",
                    f"    <AssemblyName>{subject_id}.Patch</AssemblyName>",
                    "  </PropertyGroup>",
                    "</Project>",
                    "",
                ]
            ),
            encoding="utf-8",
        )
        shared_project.write_text(
            "\n".join(
                [
                    "<Project>",
                    "  <PropertyGroup>",
                    f"    <AssemblyName>{subject_id}.Shared</AssemblyName>",
                    "  </PropertyGroup>",
                    "</Project>",
                    "",
                ]
            ),
            encoding="utf-8",
        )
        solution_path.write_text(
            "\n".join(
                [
                    "Project(\"{FAE04EC0-301F-11D3-BF4B-00C04F79EFBC}\") = \"Host\", \"Host\\\\"
                    f"{subject_id}.Host.csproj\", \"{{11111111-1111-1111-1111-111111111111}}\"",
                    "EndProject",
                    "Project(\"{FAE04EC0-301F-11D3-BF4B-00C04F79EFBC}\") = \"Patch\", \"Patch\\\\"
                    f"{subject_id}.Patch.csproj\", \"{{22222222-2222-2222-2222-222222222222}}\"",
                    "EndProject",
                    "Project(\"{FAE04EC0-301F-11D3-BF4B-00C04F79EFBC}\") = \"Shared\", \"Shared\\\\"
                    f"{subject_id}.Shared.csproj\", \"{{33333333-3333-3333-3333-333333333333}}\"",
                    "EndProject",
                    "",
                ]
            ),
            encoding="utf-8",
        )

        write_json(
            repo_root / "subjects" / subject_id / "subject.manifest.json",
            {
                "subjectId": subject_id,
                "displayName": subject_id,
                "defaultGoal": "correctness.dev",
                "defaultMatrix": "windows-hotupdate-proof",
                "defaultValidationProfile": "proof-dev",
                "engineeringProfile": "hot-update-host",
                "source": {
                    "type": "dotnet-project",
                    "path": f"subjects/{subject_id}/source/{subject_id}.sln",
                    "primaryProjectPath": f"subjects/{subject_id}/source/Host/{subject_id}.Host.csproj",
                    "entry": f"{subject_id}.Host/Program::Main()",
                },
                "hotUpdate": {
                    "patchProjectPaths": [
                        f"subjects/{subject_id}/source/Patch/{subject_id}.Patch.csproj",
                    ]
                },
                "validationProfiles": {
                    "proof-dev": ["proof"],
                    "perf-dev": ["perf"],
                },
                "validation": {
                    "proof": {
                        "kind": "proof",
                        "defaultVariant": "CHECK",
                    },
                    "perf": {
                        "kind": "perf",
                        "driver": "interpreter-runtime-perf",
                        "project": "src/validation/perf/Benchmark.WorkloadEntry.PerfHarness/Benchmark.WorkloadEntry.PerfHarness.csproj",
                        "defaultVariant": "PROFILE",
                    },
                },
                "executionPipelines": [
                    {
                        "pipelineId": "hotupdate-proof",
                        "stages": [
                            {"stageId": "source-resolve", "kind": "source-resolve", "scope": "shared", "bucket": "source"},
                            {"stageId": "host-input-build", "kind": "host-input-build", "scope": "shared", "bucket": "host-input"},
                            {"stageId": "runtime-managed-output", "kind": "runtime-managed-output", "scope": "matrix", "bucket": "runtime"},
                        ],
                    },
                    {
                        "pipelineId": "hotupdate-benchmark",
                        "stages": [
                            {"stageId": "source-resolve", "kind": "source-resolve", "scope": "shared", "bucket": "source"},
                            {"stageId": "host-input-build", "kind": "host-input-build", "scope": "shared", "bucket": "host-input"},
                            {"stageId": "interpreter-runtime-perf", "kind": "interpreter-runtime-perf", "scope": "matrix", "bucket": "runtime"},
                        ],
                    },
                ],
                "environmentMatrices": [
                    {
                        "matrixId": "windows-hotupdate-proof",
                        "pipelineId": "hotupdate-proof",
                        "supportedGoals": ["correctness.dev"],
                        "executionContext": {
                            "hostPlatform": "windows-x64",
                            "targetPlatform": "windows-x64",
                            "toolchainProfile": "dotnet-managed",
                            "runtimeProfile": "hot-update-proof",
                        },
                        "validationIntent": {
                            "validationMode": "output",
                            "adaptationLevel": "hot-update-host",
                            "expectedOutcome": "pass",
                        },
                        "artifactPlan": {
                            "evidenceTerminalBucket": "runtime",
                        },
                    },
                    {
                        "matrixId": "windows-hotupdate-benchmark",
                        "pipelineId": "hotupdate-benchmark",
                        "supportedGoals": ["perf.release"],
                        "executionContext": {
                            "hostPlatform": "windows-x64",
                            "targetPlatform": "windows-x64",
                            "toolchainProfile": "dotnet-managed",
                            "runtimeProfile": "hot-update-benchmark",
                        },
                        "validationIntent": {
                            "validationMode": "perf",
                            "adaptationLevel": "hot-update-host",
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

            with patch.object(workspace_module.compiled_catalog_module, "build_subject_declared_test_catalog", return_value=declared_catalog_fixture()):
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
            self.assertEqual(2, manifest["workspaceVersion"])
            self.assertEqual("FixtureSubject", manifest["subjectId"])
            self.assertEqual("CHECK", manifest["variant"])
            self.assertEqual("windows-dev-output", manifest["defaultMatrixId"])
            self.assertEqual(
                "solutions/subjects/FixtureSubject/FixtureSubject.sln",
                manifest["managedSolutionPath"],
            )
            self.assertEqual(
                [
                    {
                        "projectId": "managed/FixtureSubject/FixtureSubject",
                        "projectPath": "subjects/FixtureSubject/source/FixtureSubject.csproj",
                        "assemblyName": "FixtureSubject",
                        "isPrimary": True,
                    },
                ],
                manifest["managedProjects"],
            )
            self.assertEqual(
                [
                    {
                        "projectId": "managed-test/FixtureSubject/proof-host",
                        "projectPath": "solutions/subjects/FixtureSubject/managed-tests/FixtureSubject.DeclaredProofHost.csproj",
                        "assemblyName": "FixtureSubject.DeclaredProofHost",
                        "hostKind": "proof-host",
                    "collectionPath": "solutions/subjects/FixtureSubject/managed-tests/Generated/declared-tests.collection.json",
                        "generatedSourcePath": "solutions/subjects/FixtureSubject/managed-tests/Generated/ChaosGeneratedDeclaredTests.g.cs",
                    },
                    {
                        "projectId": "managed-test/FixtureSubject/benchmark-host",
                        "projectPath": "solutions/subjects/FixtureSubject/managed-tests/FixtureSubject.DeclaredBenchmarkHost.csproj",
                        "assemblyName": "FixtureSubject.DeclaredBenchmarkHost",
                        "hostKind": "benchmark-host",
                    "collectionPath": "solutions/subjects/FixtureSubject/managed-tests/Generated/declared-tests.collection.json",
                        "generatedSourcePath": "solutions/subjects/FixtureSubject/managed-tests/Generated/ChaosGeneratedDeclaredBenchmarks.g.cs",
                    },
                ],
                manifest["managedTestProjects"],
            )
            self.assertEqual(
                [
                    {
                        "projectId": "native/FixtureSubject/windows-dev-output/generated-native",
                        "matrixId": "windows-dev-output",
                        "projectPath": "solutions/subjects/FixtureSubject/native/windows-dev-output/generated/chaos_subject_generated_native.vcxproj",
                        "configureRoot": "solutions/subjects/FixtureSubject/native/windows-dev-output",
                        "targetPlatform": "windows-x64",
                        "toolchainProfile": "msvc-reference",
                        "deliveryKind": "generated-static-library",
                        "buildArgs": ["--config", "Release", "--target", "chaos_subject_generated_native"],
                    },
                ],
                manifest["nativeProjects"],
            )
            self.assertEqual(
                [
                    {
                        "projectId": "native-test/FixtureSubject/windows-dev-output/proof-host",
                        "matrixId": "windows-dev-output",
                        "projectPath": "solutions/subjects/FixtureSubject/native/windows-dev-output/proof/chaos_subject_reference_proof.vcxproj",
                        "configureRoot": "solutions/subjects/FixtureSubject/native/windows-dev-output",
                        "targetPlatform": "windows-x64",
                        "toolchainProfile": "msvc-reference",
                        "deliveryKind": "direct-run-host",
                        "hostKind": "proof-host",
                        "managedTestProjectId": "managed-test/FixtureSubject/proof-host",
                        "buildArgs": ["--config", "Release", "--target", "chaos_subject_reference_proof"],
                    },
                ],
                manifest["nativeTestProjects"],
            )
            self.assertEqual(1, len(manifest["matrices"]))
            self.assertEqual(
                {
                    "matrixId": "windows-dev-output",
                    "goalIds": ["correctness.dev"],
                    "hostPlatform": "windows-x64",
                    "targetPlatform": "windows-x64",
                    "toolchainProfile": "msvc-reference",
                    "managedProjectIds": ["managed/FixtureSubject/FixtureSubject"],
                    "managedTestProjectIds": [
                        "managed-test/FixtureSubject/proof-host",
                        "managed-test/FixtureSubject/benchmark-host",
                    ],
                    "nativeProjectIds": ["native/FixtureSubject/windows-dev-output/generated-native"],
                    "nativeTestProjectIds": ["native-test/FixtureSubject/windows-dev-output/proof-host"],
                },
                manifest["matrices"][0],
            )
            self.assertNotIn("visualStudioStateVersion", manifest)
            self.assertNotIn("generatedRoot", manifest)
            self.assertNotIn("defaultOpenNativeProject", manifest["matrices"][0])
            self.assertNotIn("defaultBuildNativeProject", manifest["matrices"][0])
            self.assertNotIn("generatedNativeProjectPath", manifest["matrices"][0])
            self.assertNotIn("proofNativeProjectPath", manifest["matrices"][0])
            self.assertNotIn("nativeProjectPath", manifest["matrices"][0])
            self.assertTrue((repo_root / manifest["managedSolutionPath"]).is_file())
            self.assertTrue((repo_root / manifest["nativeProjects"][0]["projectPath"]).is_file())
            self.assertTrue((repo_root / manifest["nativeTestProjects"][0]["projectPath"]).is_file())
            self.assertTrue((repo_root / manifest["managedTestProjects"][0]["projectPath"]).is_file())
            self.assertTrue((repo_root / manifest["managedTestProjects"][1]["projectPath"]).is_file())
            self.assertTrue((repo_root / manifest["managedTestProjects"][0]["generatedSourcePath"]).is_file())
            self.assertTrue((repo_root / manifest["managedTestProjects"][1]["generatedSourcePath"]).is_file())
            self.assertTrue((repo_root / manifest["managedTestProjects"][0]["collectionPath"]).is_file())
            proof_project_text = (repo_root / manifest["managedTestProjects"][0]["projectPath"]).read_text(encoding="utf-8")
            benchmark_project_text = (repo_root / manifest["managedTestProjects"][1]["projectPath"]).read_text(encoding="utf-8")
            self.assertIn("<EnableDefaultCompileItems>false</EnableDefaultCompileItems>", proof_project_text)
            self.assertIn(
                '<Compile Include="Generated/ChaosGeneratedDeclaredTests.g.cs" />',
                proof_project_text,
            )
            self.assertIn(
                '<Compile Include="Generated/ChaosGeneratedDeclaredBenchmarks.g.cs" />',
                benchmark_project_text,
            )
            self.assertIn(
                '<ProjectReference Include="../../../../subjects/FixtureSubject/source/FixtureSubject.csproj" />',
                proof_project_text,
            )
            mirrored_subject_exec_root = repo_root / "solutions" / "subjects" / "FixtureSubject" / "generated" / "subject-exec"
            self.assertTrue((mirrored_subject_exec_root / "analysis" / "generated" / "generated.manifest.json").is_file())
            self.assertTrue((mirrored_subject_exec_root / "analysis" / "generated" / "native-reference.plan.json").is_file())
            self.assertTrue(
                (mirrored_subject_exec_root / "analysis" / "generated" / "generated" / "native-reference.generated.cpp").is_file()
            )
            solution_text = (repo_root / manifest["managedSolutionPath"]).read_text(encoding="utf-8")
            self.assertIn("FixtureSubject.csproj", solution_text)
            self.assertIn("FixtureSubject.DeclaredProofHost.csproj", solution_text)
            self.assertIn("FixtureSubject.DeclaredBenchmarkHost.csproj", solution_text)
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

    def test_generate_subject_workspace_materializes_template_based_proof_host_without_legacy_subject_source(self) -> None:
        workspace_module = load_module(PROJECT_WORKSPACE_MODULE_PATH, "chaos_project_workspace_subject_generate_template_proof_host")
        repo_root = self._make_repo_root("subject-generate-template-proof-host")
        self._write_subject_fixture(repo_root, include_legacy_native_reference=False)
        completed = subprocess.CompletedProcess(["cmake"], 0, "", "")

        try:
            def configure_side_effect(arguments: list[str], cwd: Path, env: dict[str, str] | None = None):
                del cwd, env
                configure_root = Path(arguments[arguments.index("-B") + 1])
                write_windows_subject_native_project_stubs(configure_root)
                return completed

            with patch.object(workspace_module.compiled_catalog_module, "build_subject_declared_test_catalog", return_value=declared_catalog_fixture()):
                with patch.object(workspace_module.tooling_module, "cmake_environment", return_value=("cmake", {})):
                    with patch.object(workspace_module, "run_process", side_effect=configure_side_effect):
                        result = workspace_module.generate_subject_workspace(
                            repo_root,
                            "windows",
                            {"id": "subject/FixtureSubject"},
                        )

            manifest = json.loads((repo_root / result["manifestPath"]).read_text(encoding="utf-8"))
            proof_host_path = (
                repo_root
                / "solutions"
                / "subjects"
                / "FixtureSubject"
                / "native-source"
                / "windows-dev-output"
                / "proof"
                / "main.cpp"
            )
            self.assertTrue(proof_host_path.is_file())
            self.assertTrue((repo_root / manifest["nativeTestProjects"][0]["projectPath"]).is_file())
            proof_host_text = proof_host_path.read_text(encoding="utf-8")
            self.assertIn('options.image_name_utf8 = "FixtureSubject";', proof_host_text)
            self.assertIn('constexpr const char* kRuntimeTag = "subject-reference-proof";', proof_host_text)
        finally:
            shutil.rmtree(repo_root, ignore_errors=True)

    def test_generate_subject_workspace_writes_hotupdate_patch_and_test_projects(self) -> None:
        workspace_module = load_module(PROJECT_WORKSPACE_MODULE_PATH, "chaos_project_workspace_subject_generate_hotupdate")
        repo_root = self._make_repo_root("subject-generate-hotupdate")
        self._write_hotupdate_subject_fixture(repo_root)

        try:
            with patch.object(
                workspace_module.compiled_catalog_module,
                "build_subject_declared_test_catalog",
                return_value=hotupdate_declared_catalog_fixture(),
            ):
                result = workspace_module.generate_subject_workspace(
                    repo_root,
                    "windows",
                    {"id": "subject/FixtureHotUpdateSubject"},
                )

            manifest = json.loads((repo_root / result["manifestPath"]).read_text(encoding="utf-8"))
            self.assertEqual(
                [
                    {
                        "projectId": "hotupdate-patch/FixtureHotUpdateSubject/FixtureHotUpdateSubject_Patch",
                        "managedProjectId": "managed/FixtureHotUpdateSubject/FixtureHotUpdateSubject_Patch",
                        "projectPath": "subjects/FixtureHotUpdateSubject/source/Patch/FixtureHotUpdateSubject.Patch.csproj",
                        "assemblyName": "FixtureHotUpdateSubject.Patch",
                    }
                ],
                manifest["hotupdatePatchProjects"],
            )
            self.assertEqual(
                [
                    "hotupdate-test/FixtureHotUpdateSubject/proof-host",
                    "hotupdate-test/FixtureHotUpdateSubject/benchmark-host",
                ],
                [item["projectId"] for item in manifest["hotupdateTestProjects"]],
            )
            self.assertEqual(
                [
                    "hotupdate-patch/FixtureHotUpdateSubject/FixtureHotUpdateSubject_Patch",
                ],
                manifest["matrices"][0]["hotupdatePatchProjectIds"],
            )
            self.assertEqual(
                [
                    "hotupdate-test/FixtureHotUpdateSubject/proof-host",
                    "hotupdate-test/FixtureHotUpdateSubject/benchmark-host",
                ],
                manifest["matrices"][0]["hotupdateTestProjectIds"],
            )
            for host_project in manifest["hotupdateTestProjects"]:
                self.assertTrue((repo_root / host_project["projectPath"]).is_file())
                self.assertTrue((repo_root / host_project["generatedSourcePath"]).is_file())
                self.assertTrue((repo_root / host_project["bindingManifestPath"]).is_file())
                self.assertEqual(
                    [
                        "hotupdate-patch/FixtureHotUpdateSubject/FixtureHotUpdateSubject_Patch",
                    ],
                    host_project["patchProjectIds"],
                )
            proof_project_text = (repo_root / manifest["hotupdateTestProjects"][0]["projectPath"]).read_text(encoding="utf-8")
            benchmark_project_text = (repo_root / manifest["hotupdateTestProjects"][1]["projectPath"]).read_text(encoding="utf-8")
            self.assertIn("<EnableDefaultCompileItems>false</EnableDefaultCompileItems>", proof_project_text)
            self.assertIn(
                '<Compile Include="Generated/ChaosGeneratedHotUpdateProofHost.g.cs" />',
                proof_project_text,
            )
            self.assertIn(
                '<Compile Include="Generated/ChaosGeneratedHotUpdateBenchmarkHost.g.cs" />',
                benchmark_project_text,
            )
        finally:
            shutil.rmtree(repo_root, ignore_errors=True)

    def test_generate_subject_workspace_overwrites_existing_hotupdate_collection_with_latest_catalog(self) -> None:
        workspace_module = load_module(
            PROJECT_WORKSPACE_MODULE_PATH,
            "chaos_project_workspace_subject_generate_hotupdate_overwrite_collection",
        )
        subject_id = "FixtureHotUpdateSubject"
        repo_root = self._make_repo_root("subject-generate-hotupdate-overwrite-collection")
        self._write_hotupdate_subject_fixture(repo_root, subject_id=subject_id)
        stale_collection_path = (
            repo_root
            / "solutions"
            / "subjects"
            / subject_id
            / "hotupdate-tests"
            / "Generated"
            / "declared-tests.collection.json"
        )
        write_json(
            stale_collection_path,
            {
                "subjectId": subject_id,
                "declaredUnitTests": [],
                "declaredBenchmarks": [
                    {
                        "stableId": f"{subject_id}::stale",
                        "entryIndex": 0,
                        "alias": "stale-hotupdate-benchmark",
                    }
                ],
            },
        )
        refreshed_catalog = hotupdate_declared_catalog_fixture(subject_id)
        refreshed_catalog["declaredBenchmarks"][0]["entryIndex"] = 3
        refreshed_catalog["declaredBenchmarks"][0]["alias"] = "hotupdate-benchmark-refreshed"

        try:
            with patch.object(
                workspace_module.compiled_catalog_module,
                "build_subject_declared_test_catalog",
                return_value=refreshed_catalog,
            ):
                workspace_module.generate_subject_workspace(
                    repo_root,
                    "windows",
                    {"id": f"subject/{subject_id}"},
                )

            collection_payload = json.loads(stale_collection_path.read_text(encoding="utf-8"))
            binding_manifest_path = stale_collection_path.with_name("declared-tests.binding.json")
            binding_payload = json.loads(binding_manifest_path.read_text(encoding="utf-8"))

            self.assertEqual(refreshed_catalog, collection_payload)
            self.assertEqual(
                3,
                next(
                    entry["entryIndex"]
                    for entry in binding_payload["entryBindings"]
                    if entry["hostKind"] == "benchmark-host"
                ),
            )
            self.assertEqual(
                "hotupdate-benchmark-refreshed",
                collection_payload["declaredBenchmarks"][0]["alias"],
            )
        finally:
            shutil.rmtree(repo_root, ignore_errors=True)

    def test_generate_subject_workspace_overwrites_existing_managed_benchmark_host_with_latest_template(self) -> None:
        workspace_module = load_module(
            PROJECT_WORKSPACE_MODULE_PATH,
            "chaos_project_workspace_subject_generate_managed_benchmark_host_refresh",
        )
        subject_id = "FixtureSubject"
        repo_root = self._make_repo_root("subject-generate-managed-benchmark-host-refresh")
        self._write_subject_fixture(repo_root, subject_id=subject_id)
        stale_host_path = (
            repo_root
            / "solutions"
            / "subjects"
            / subject_id
            / "managed-tests"
            / "Generated"
            / "ChaosGeneratedDeclaredBenchmarks.g.cs"
        )
        stale_host_path.parent.mkdir(parents=True, exist_ok=True)
        stale_host_path.write_text(
            "\n".join(
                [
                    "public sealed record DeclaredBenchmarkEntry(int EntryIndex);",
                    "public static class FixtureSubjectDeclaredBenchmarkHost",
                    "{",
                    "    public static DeclaredBenchmarkEntry[] Entries { get; } = new DeclaredBenchmarkEntry[]",
                    "    {",
                    "        new(0),",
                    "    };",
                    "}",
                    "",
                ]
            ),
            encoding="utf-8",
        )
        completed = subprocess.CompletedProcess(["cmake"], 0, "", "")

        try:
            def configure_side_effect(arguments: list[str], cwd: Path, env: dict[str, str] | None = None):
                del cwd, env
                configure_root = Path(arguments[arguments.index("-B") + 1])
                write_windows_subject_native_project_stubs(configure_root)
                return completed

            with patch.object(
                workspace_module.compiled_catalog_module,
                "build_subject_declared_test_catalog",
                return_value=declared_catalog_fixture(subject_id),
            ):
                with patch.object(workspace_module.tooling_module, "cmake_environment", return_value=("cmake", {})):
                    with patch.object(workspace_module, "run_process", side_effect=configure_side_effect):
                        workspace_module.generate_subject_workspace(
                            repo_root,
                            "windows",
                            {"id": f"subject/{subject_id}"},
                        )

            refreshed_host_text = stale_host_path.read_text(encoding="utf-8")
            self.assertIn("public static int Execute(int entryIndex)", refreshed_host_text)
            self.assertIn("switch (entryIndex)", refreshed_host_text)
            self.assertNotIn("DeclaredBenchmarkEntry", refreshed_host_text)
            self.assertNotIn("Entries { get; }", refreshed_host_text)
        finally:
            shutil.rmtree(repo_root, ignore_errors=True)

    def test_generate_subject_workspace_allows_managed_only_windows_subject_without_native_workspace(self) -> None:
        workspace_module = load_module(PROJECT_WORKSPACE_MODULE_PATH, "chaos_project_workspace_subject_generate_managed_only")
        repo_root = self._make_repo_root("subject-generate-managed-only")
        self._write_managed_only_subject_fixture(repo_root)

        try:
            with patch.object(
                workspace_module.compiled_catalog_module,
                "build_subject_declared_test_catalog",
                return_value=declared_catalog_fixture("ManagedOnlySubject"),
            ):
                with patch.object(
                    workspace_module.tooling_module,
                    "cmake_environment",
                    side_effect=AssertionError("managed-only workspace generation should not request cmake"),
                ):
                    with patch.object(
                        workspace_module,
                        "refresh_subject_generated_root",
                        side_effect=AssertionError("managed-only workspace generation should not refresh generated source"),
                    ):
                        with patch.object(
                            workspace_module,
                            "run_process",
                            side_effect=AssertionError("managed-only workspace generation should not configure native projects"),
                        ):
                            result = workspace_module.generate_subject_workspace(
                                repo_root,
                                "windows",
                                {"id": "subject/ManagedOnlySubject"},
                            )

            manifest_path = repo_root / result["manifestPath"]
            self.assertTrue(manifest_path.is_file())
            manifest = json.loads(manifest_path.read_text(encoding="utf-8"))
            self.assertEqual("ManagedOnlySubject", manifest["subjectId"])
            self.assertEqual("PROFILE", manifest["variant"])
            self.assertEqual("windows-perf-dev", manifest["defaultMatrixId"])
            self.assertEqual([], manifest["nativeProjects"])
            self.assertEqual([], manifest["nativeTestProjects"])
            self.assertEqual(1, len(manifest["matrices"]))
            self.assertEqual(
                {
                    "matrixId": "windows-perf-dev",
                    "goalIds": ["perf.dev"],
                    "hostPlatform": "windows-x64",
                    "targetPlatform": "windows-x64",
                    "toolchainProfile": "dotnet-managed",
                    "managedProjectIds": ["managed/ManagedOnlySubject/ManagedOnlySubject"],
                    "managedTestProjectIds": [
                        "managed-test/ManagedOnlySubject/proof-host",
                        "managed-test/ManagedOnlySubject/benchmark-host",
                    ],
                    "nativeProjectIds": [],
                    "nativeTestProjectIds": [],
                },
                manifest["matrices"][0],
            )
            self.assertTrue((repo_root / manifest["managedSolutionPath"]).is_file())
        finally:
            shutil.rmtree(repo_root, ignore_errors=True)

    def test_generate_subject_workspace_adds_native_benchmark_host_when_native_benchmarks_exist(self) -> None:
        workspace_module = load_module(
            PROJECT_WORKSPACE_MODULE_PATH,
            "chaos_project_workspace_subject_generate_native_benchmark_host",
        )
        repo_root = self._make_repo_root("subject-generate-native-benchmark-host")
        self._write_subject_fixture(repo_root)
        completed = subprocess.CompletedProcess(["cmake"], 0, "", "")
        subject_manifest_path = repo_root / "subjects" / "FixtureSubject" / "subject.manifest.json"
        subject_manifest = json.loads(subject_manifest_path.read_text(encoding="utf-8"))
        subject_manifest["executionPipelines"].append(
            {
                "pipelineId": "native-aot-benchmark",
                "stages": [
                    {"stageId": "source-resolve", "kind": "source-resolve", "scope": "shared", "bucket": "source"},
                    {"stageId": "host-input-build", "kind": "host-input-build", "scope": "shared", "bucket": "host-input"},
                    {"stageId": "analysis-frontend", "kind": "analysis-frontend", "scope": "shared", "bucket": "analysis"},
                    {"stageId": "generated-native-aot", "kind": "generated-native-aot", "scope": "shared", "bucket": "generated"},
                ],
            }
        )
        subject_manifest["environmentMatrices"].append(
            {
                "matrixId": "windows-native-perf",
                "pipelineId": "native-aot-benchmark",
                "supportedGoals": ["perf.release"],
                "executionContext": {
                    "hostPlatform": "windows-x64",
                    "targetPlatform": "windows-x64",
                    "toolchainProfile": "msvc-reference",
                },
                "artifactPlan": {
                    "evidenceTerminalBucket": "report",
                },
            }
        )
        write_json(subject_manifest_path, subject_manifest)
        native_catalog = declared_catalog_fixture()
        native_catalog["declaredBenchmarks"] = [
            {
                **native_catalog["declaredBenchmarks"][0],
                "entryIndex": 5,
                "alias": "native-benchmark",
                "declaringType": "FixtureSubject.NativeBenchmarks",
                "methodName": "RunNative",
                "methodSignature": "RunNative()",
                "modes": 3,
            },
            {
                **native_catalog["declaredBenchmarks"][0],
                "entryIndex": 9,
                "alias": "managed-only-benchmark",
                "stableId": "FixtureSubject::FixtureSubject::FixtureSubject.ManagedBenchmarks::RunManaged()",
                "declaringType": "FixtureSubject.ManagedBenchmarks",
                "methodName": "RunManaged",
                "methodSignature": "RunManaged()",
                "modes": 1,
            },
        ]

        try:
            def configure_side_effect(arguments: list[str], cwd: Path, env: dict[str, str] | None = None):
                del cwd, env
                configure_root = Path(arguments[arguments.index("-B") + 1])
                if str(configure_root).endswith("windows-dev-output"):
                    write_windows_subject_native_project_stubs(configure_root)
                elif str(configure_root).endswith("windows-native-perf"):
                    write_windows_subject_native_project_stubs(configure_root, benchmark_text="<Project />\n")
                else:
                    (configure_root / "linux-x64-packaging.vcxproj").parent.mkdir(parents=True, exist_ok=True)
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
                if matrix_id == "windows-native-perf":
                    (generated_root / "native-aot.manifest.json").write_text(
                        json.dumps({"matrixId": matrix_id}, indent=2) + "\n",
                        encoding="utf-8",
                    )
                    (generated_root / "native-aot.plan.json").write_text(
                        json.dumps({"matrixId": matrix_id}, indent=2) + "\n",
                        encoding="utf-8",
                    )
                    (generated_root / "generated" / "native-aot.generated.cpp").write_text(
                        f"// native aot for {matrix_id}\n",
                        encoding="utf-8",
                    )
                else:
                    (generated_root / "native-reference.manifest.json").write_text(
                        json.dumps({"matrixId": matrix_id}, indent=2) + "\n",
                        encoding="utf-8",
                    )
                    (generated_root / "native-reference.plan.json").write_text(
                        json.dumps({"matrixId": matrix_id}, indent=2) + "\n",
                        encoding="utf-8",
                    )
                    (generated_root / "generated" / "native-reference.generated.cpp").write_text(
                        f"// proof for {matrix_id}\n",
                        encoding="utf-8",
                    )

            with patch.object(
                workspace_module.compiled_catalog_module,
                "build_subject_declared_test_catalog",
                return_value=native_catalog,
            ):
                with patch.object(workspace_module.tooling_module, "cmake_environment", return_value=("cmake", {})):
                    with patch.object(workspace_module, "run_process", side_effect=configure_side_effect):
                        with patch.object(workspace_module, "refresh_subject_generated_root", side_effect=refresh_side_effect):
                            result = workspace_module.generate_subject_workspace(
                                repo_root,
                                "windows",
                                {"id": "subject/FixtureSubject", "all-targets": True, "refresh-generated": True},
                            )

            manifest = json.loads((repo_root / result["manifestPath"]).read_text(encoding="utf-8"))
            managed_test_projects = {
                item["projectId"]: item
                for item in manifest["managedTestProjects"]
            }
            self.assertIn("managed-test/FixtureSubject/benchmark-host", managed_test_projects)
            self.assertIn("managed-test/FixtureSubject/benchmark-host-native", managed_test_projects)

            native_benchmark_host = managed_test_projects["managed-test/FixtureSubject/benchmark-host-native"]
            native_benchmark_host_source = (
                repo_root / native_benchmark_host["generatedSourcePath"]
            ).read_text(encoding="utf-8")
            self.assertIn("case 5:", native_benchmark_host_source)
            self.assertIn("global::FixtureSubject.NativeBenchmarks.RunNative();", native_benchmark_host_source)
            self.assertNotIn("case 9:", native_benchmark_host_source)
            self.assertNotIn("ManagedBenchmarks.RunManaged", native_benchmark_host_source)

            benchmark_native_test_project = next(
                item
                for item in manifest["nativeTestProjects"]
                if item["projectId"] == "native-test/FixtureSubject/windows-native-perf/benchmark-host"
            )
            self.assertEqual(
                "managed-test/FixtureSubject/benchmark-host-native",
                benchmark_native_test_project["managedTestProjectId"],
            )
        finally:
            shutil.rmtree(repo_root, ignore_errors=True)

    def test_generate_subject_workspace_all_targets_skips_native_configure_for_managed_windows_matrix(self) -> None:
        workspace_module = load_module(PROJECT_WORKSPACE_MODULE_PATH, "chaos_project_workspace_subject_generate_mixed_windows_matrices")
        repo_root = self._make_repo_root("subject-generate-mixed-windows-matrices")
        self._write_subject_fixture(repo_root)
        completed = subprocess.CompletedProcess(["cmake"], 0, "", "")
        subject_manifest_path = repo_root / "subjects" / "FixtureSubject" / "subject.manifest.json"
        subject_manifest = json.loads(subject_manifest_path.read_text(encoding="utf-8"))
        subject_manifest["executionPipelines"].append(
            {
                "pipelineId": "managed-runtime-output",
                "stages": [
                    {"stageId": "source-resolve", "kind": "source-resolve", "scope": "shared", "bucket": "source"},
                    {"stageId": "host-input-build", "kind": "host-input-build", "scope": "shared", "bucket": "host-input"},
                    {"stageId": "runtime-managed-output", "kind": "runtime-managed-output", "scope": "matrix", "bucket": "runtime"},
                ],
            }
        )
        subject_manifest["environmentMatrices"].append(
            {
                "matrixId": "windows-managed-output",
                "pipelineId": "managed-runtime-output",
                "supportedGoals": ["correctness.dev"],
                "executionContext": {
                    "hostPlatform": "windows-x64",
                    "targetPlatform": "windows-x64",
                    "toolchainProfile": "dotnet-managed",
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
                if str(configure_root).endswith("windows-dev-output"):
                    write_windows_subject_native_project_stubs(configure_root)
                else:
                    (configure_root / "linux-x64-packaging.vcxproj").parent.mkdir(parents=True, exist_ok=True)
                    (configure_root / "linux-x64-packaging.vcxproj").write_text("<Project />\n", encoding="utf-8")
                return completed

            with patch.object(workspace_module.compiled_catalog_module, "build_subject_declared_test_catalog", return_value=declared_catalog_fixture()):
                with patch.object(workspace_module, "refresh_subject_generated_root", return_value=None) as refresh_mock:
                    with patch.object(workspace_module.tooling_module, "cmake_environment", return_value=("cmake", {})):
                        with patch.object(workspace_module, "run_process", side_effect=configure_side_effect) as run_process_mock:
                            result = workspace_module.generate_subject_workspace(
                                repo_root,
                                "windows",
                                {"id": "subject/FixtureSubject", "all-targets": True},
                            )

            self.assertEqual(0, refresh_mock.call_count)
            self.assertEqual(2, run_process_mock.call_count)
            manifest = json.loads((repo_root / result["manifestPath"]).read_text(encoding="utf-8"))
            matrices_by_id = {entry["matrixId"]: entry for entry in manifest["matrices"]}
            self.assertEqual([], matrices_by_id["windows-managed-output"]["nativeProjectIds"])
            self.assertEqual([], matrices_by_id["windows-managed-output"]["nativeTestProjectIds"])
            self.assertEqual(
                ["native/FixtureSubject/windows-dev-output/generated-native"],
                matrices_by_id["windows-dev-output"]["nativeProjectIds"],
            )
            self.assertEqual(
                ["native-test/FixtureSubject/windows-dev-output/proof-host"],
                matrices_by_id["windows-dev-output"]["nativeTestProjectIds"],
            )
        finally:
            shutil.rmtree(repo_root, ignore_errors=True)

    def test_generate_subject_workspace_all_targets_preserves_manifest_default_matrix(self) -> None:
        workspace_module = load_module(PROJECT_WORKSPACE_MODULE_PATH, "chaos_project_workspace_subject_generate_preserve_default_matrix")
        repo_root = self._make_repo_root("subject-generate-preserve-default-matrix")
        self._write_subject_fixture(repo_root)
        completed = subprocess.CompletedProcess(["cmake"], 0, "", "")
        subject_manifest_path = repo_root / "subjects" / "FixtureSubject" / "subject.manifest.json"
        subject_manifest = json.loads(subject_manifest_path.read_text(encoding="utf-8"))
        subject_manifest["executionPipelines"].append(
            {
                "pipelineId": "managed-runtime-output",
                "stages": [
                    {"stageId": "source-resolve", "kind": "source-resolve", "scope": "shared", "bucket": "source"},
                    {"stageId": "host-input-build", "kind": "host-input-build", "scope": "shared", "bucket": "host-input"},
                    {"stageId": "runtime-managed-output", "kind": "runtime-managed-output", "scope": "matrix", "bucket": "runtime"},
                ],
            }
        )
        subject_manifest["environmentMatrices"] = [
            {
                "matrixId": "windows-managed-output",
                "pipelineId": "managed-runtime-output",
                "supportedGoals": ["correctness.dev"],
                "executionContext": {
                    "hostPlatform": "windows-x64",
                    "targetPlatform": "windows-x64",
                    "toolchainProfile": "dotnet-managed",
                },
                "artifactPlan": {
                    "evidenceTerminalBucket": "runtime",
                },
            },
            *subject_manifest["environmentMatrices"],
        ]
        write_json(subject_manifest_path, subject_manifest)

        try:
            def configure_side_effect(arguments: list[str], cwd: Path, env: dict[str, str] | None = None):
                del cwd, env
                configure_root = Path(arguments[arguments.index("-B") + 1])
                if str(configure_root).endswith("windows-dev-output"):
                    write_windows_subject_native_project_stubs(configure_root)
                else:
                    (configure_root / "linux-x64-packaging.vcxproj").parent.mkdir(parents=True, exist_ok=True)
                    (configure_root / "linux-x64-packaging.vcxproj").write_text("<Project />\n", encoding="utf-8")
                return completed

            with patch.object(workspace_module.compiled_catalog_module, "build_subject_declared_test_catalog", return_value=declared_catalog_fixture()):
                with patch.object(workspace_module.tooling_module, "cmake_environment", return_value=("cmake", {})):
                    with patch.object(workspace_module, "run_process", side_effect=configure_side_effect):
                        result = workspace_module.generate_subject_workspace(
                            repo_root,
                            "windows",
                            {"id": "subject/FixtureSubject", "all-targets": True},
                        )

            manifest = json.loads((repo_root / result["manifestPath"]).read_text(encoding="utf-8"))
            self.assertEqual("windows-dev-output", manifest["defaultMatrixId"])
        finally:
            shutil.rmtree(repo_root, ignore_errors=True)

    def test_generate_subject_workspace_all_targets_includes_native_aot_workspace_configure(self) -> None:
        workspace_module = load_module(PROJECT_WORKSPACE_MODULE_PATH, "chaos_project_workspace_subject_generate_native_aot_workspace")
        repo_root = self._make_repo_root("subject-generate-native-aot-matrix")
        self._write_subject_fixture(repo_root)
        completed = subprocess.CompletedProcess(["cmake"], 0, "", "")
        subject_manifest_path = repo_root / "subjects" / "FixtureSubject" / "subject.manifest.json"
        subject_manifest = json.loads(subject_manifest_path.read_text(encoding="utf-8"))
        subject_manifest["executionPipelines"].append(
            {
                "pipelineId": "native-aot-benchmark",
                "stages": [
                    {"stageId": "source-resolve", "kind": "source-resolve", "scope": "shared", "bucket": "source"},
                    {"stageId": "host-input-build", "kind": "host-input-build", "scope": "shared", "bucket": "host-input"},
                    {"stageId": "analysis-frontend", "kind": "analysis-frontend", "scope": "shared", "bucket": "analysis"},
                    {"stageId": "generated-native-aot", "kind": "generated-native-aot", "scope": "shared", "bucket": "generated"},
                ],
            }
        )
        subject_manifest["environmentMatrices"].append(
            {
                "matrixId": "windows-native-perf",
                "pipelineId": "native-aot-benchmark",
                "supportedGoals": ["perf.release"],
                "executionContext": {
                    "hostPlatform": "windows-x64",
                    "targetPlatform": "windows-x64",
                    "toolchainProfile": "msvc-reference",
                },
                "artifactPlan": {
                    "evidenceTerminalBucket": "report",
                },
            }
        )
        write_json(subject_manifest_path, subject_manifest)

        try:
            def configure_side_effect(arguments: list[str], cwd: Path, env: dict[str, str] | None = None):
                del cwd, env
                configure_root = Path(arguments[arguments.index("-B") + 1])
                if str(configure_root).endswith("windows-dev-output"):
                    write_windows_subject_native_project_stubs(configure_root)
                elif str(configure_root).endswith("windows-native-perf"):
                    write_windows_subject_native_project_stubs(configure_root, benchmark_text="<Project />\n")
                else:
                    (configure_root / "linux-x64-packaging.vcxproj").parent.mkdir(parents=True, exist_ok=True)
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
                if matrix_id == "windows-native-perf":
                    (generated_root / "native-aot.manifest.json").write_text(
                        json.dumps({"matrixId": matrix_id}, indent=2) + "\n",
                        encoding="utf-8",
                    )
                    (generated_root / "native-aot.plan.json").write_text(
                        json.dumps({"matrixId": matrix_id}, indent=2) + "\n",
                        encoding="utf-8",
                    )
                    (generated_root / "generated" / "native-aot.generated.cpp").write_text(
                        f"// native aot for {matrix_id}\n",
                        encoding="utf-8",
                    )
                    return
                (generated_root / "native-reference.manifest.json").write_text(
                    json.dumps({"matrixId": matrix_id}, indent=2) + "\n",
                    encoding="utf-8",
                )
                (generated_root / "native-reference.plan.json").write_text(
                    json.dumps({"matrixId": matrix_id}, indent=2) + "\n",
                    encoding="utf-8",
                )
                (generated_root / "generated" / "native-reference.generated.cpp").write_text(
                    f"// proof for {matrix_id}\n",
                    encoding="utf-8",
                )

            with patch.object(workspace_module.compiled_catalog_module, "build_subject_declared_test_catalog", return_value=declared_catalog_fixture()):
                with patch.object(workspace_module, "refresh_subject_generated_root", side_effect=refresh_side_effect) as refresh_mock:
                    with patch.object(workspace_module.tooling_module, "cmake_environment", return_value=("cmake", {})):
                        with patch.object(workspace_module, "run_process", side_effect=configure_side_effect) as run_process_mock:
                            result = workspace_module.generate_subject_workspace(
                                repo_root,
                                "windows",
                                {"id": "subject/FixtureSubject", "all-targets": True, "refresh-generated": True},
                            )

            self.assertEqual(
                ["windows-dev-output", "windows-native-perf"],
                [call.args[2] for call in refresh_mock.call_args_list],
            )
            self.assertEqual(3, run_process_mock.call_count)
            manifest = json.loads((repo_root / result["manifestPath"]).read_text(encoding="utf-8"))
            matrices_by_id = {entry["matrixId"]: entry for entry in manifest["matrices"]}
            self.assertEqual(
                ["native/FixtureSubject/windows-native-perf/generated-native"],
                matrices_by_id["windows-native-perf"]["nativeProjectIds"],
            )
            self.assertEqual(
                ["native-test/FixtureSubject/windows-native-perf/benchmark-host"],
                matrices_by_id["windows-native-perf"]["nativeTestProjectIds"],
            )
            benchmark_native_test_project = next(
                item
                for item in manifest["nativeTestProjects"]
                if item["projectId"] == "native-test/FixtureSubject/windows-native-perf/benchmark-host"
            )
            self.assertEqual("benchmark-host", benchmark_native_test_project["hostKind"])
            self.assertEqual(
                "managed-test/FixtureSubject/benchmark-host",
                benchmark_native_test_project["managedTestProjectId"],
            )
            self.assertEqual(
                "solutions/subjects/FixtureSubject/native/windows-native-perf/benchmark/chaos_subject_native_aot.vcxproj",
                benchmark_native_test_project["projectPath"],
            )
            self.assertTrue((repo_root / benchmark_native_test_project["projectPath"]).is_file())
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
            self.assertEqual(2, updated_manifest["workspaceVersion"])
            self.assertNotIn("visualStudioStateVersion", updated_manifest)
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
            self.assertIn(
                {
                    "label": "Default native project",
                    "path": "solutions/subjects/FixtureSubject/native/windows-dev-output/generated/chaos_subject_generated_native.vcxproj",
                },
                result["importantOutputs"],
            )
        finally:
            shutil.rmtree(repo_root, ignore_errors=True)

    def test_build_subject_workspace_supports_manifest_v2_project_routing(self) -> None:
        workspace_module = load_module(PROJECT_WORKSPACE_MODULE_PATH, "chaos_project_workspace_subject_build_v2")
        repo_root = self._make_repo_root("subject-build-v2")
        self._write_subject_fixture(repo_root)
        workspace_root = repo_root / "solutions" / "subjects" / "FixtureSubject"
        build_root = workspace_root / "native" / "windows-dev-output"
        build_root.mkdir(parents=True, exist_ok=True)
        manifest_path = workspace_root / "workspace.manifest.json"
        write_json(
            manifest_path,
            {
                "workspaceVersion": 2,
                "kind": "subject-workspace",
                "subjectId": "FixtureSubject",
                "variant": "CHECK",
                "defaultMatrixId": "windows-dev-output",
                "managedSolutionPath": "solutions/subjects/FixtureSubject/FixtureSubject.sln",
                "managedProjects": [
                    {
                        "projectId": "managed/FixtureSubject/FixtureSubject",
                        "projectPath": "subjects/FixtureSubject/source/FixtureSubject.csproj",
                        "assemblyName": "FixtureSubject",
                        "isPrimary": True,
                    }
                ],
                "managedTestProjects": [
                    {
                        "projectId": "managed-test/FixtureSubject/proof-host",
                        "projectPath": "solutions/subjects/FixtureSubject/managed-tests/FixtureSubject.DeclaredProofHost.csproj",
                        "assemblyName": "FixtureSubject.DeclaredProofHost",
                        "hostKind": "proof-host",
                    "collectionPath": "solutions/subjects/FixtureSubject/managed-tests/Generated/declared-tests.collection.json",
                        "generatedSourcePath": "solutions/subjects/FixtureSubject/managed-tests/Generated/ChaosGeneratedDeclaredTests.g.cs",
                    }
                ],
                "nativeProjects": [
                    {
                        "projectId": "native/FixtureSubject/windows-dev-output/generated-native",
                        "matrixId": "windows-dev-output",
                        "projectPath": "solutions/subjects/FixtureSubject/native/windows-dev-output/generated/chaos_subject_generated_native.vcxproj",
                        "configureRoot": "solutions/subjects/FixtureSubject/native/windows-dev-output",
                        "targetPlatform": "windows-x64",
                        "toolchainProfile": "msvc-reference",
                        "deliveryKind": "generated-static-library",
                        "buildArgs": ["--config", "Release", "--target", "chaos_subject_generated_native"],
                    }
                ],
                "nativeTestProjects": [
                    {
                        "projectId": "native-test/FixtureSubject/windows-dev-output/proof-host",
                        "matrixId": "windows-dev-output",
                        "projectPath": "solutions/subjects/FixtureSubject/native/windows-dev-output/proof/chaos_subject_reference_proof.vcxproj",
                        "configureRoot": "solutions/subjects/FixtureSubject/native/windows-dev-output",
                        "targetPlatform": "windows-x64",
                        "toolchainProfile": "msvc-reference",
                        "deliveryKind": "direct-run-host",
                        "hostKind": "proof-host",
                        "managedTestProjectId": "managed-test/FixtureSubject/proof-host",
                        "buildArgs": ["--config", "Release", "--target", "chaos_subject_reference_proof"],
                    }
                ],
                "matrices": [
                    {
                        "matrixId": "windows-dev-output",
                        "goalIds": ["correctness.dev"],
                        "hostPlatform": "windows-x64",
                        "targetPlatform": "windows-x64",
                        "toolchainProfile": "msvc-reference",
                        "managedProjectIds": ["managed/FixtureSubject/FixtureSubject"],
                        "managedTestProjectIds": ["managed-test/FixtureSubject/proof-host"],
                        "nativeProjectIds": ["native/FixtureSubject/windows-dev-output/generated-native"],
                        "nativeTestProjectIds": ["native-test/FixtureSubject/windows-dev-output/proof-host"],
                    }
                ],
            },
        )

        managed_test_project_path = repo_root / "solutions" / "subjects" / "FixtureSubject" / "managed-tests" / "FixtureSubject.DeclaredProofHost.csproj"
        managed_test_project_path.parent.mkdir(parents=True, exist_ok=True)
        managed_test_project_path.write_text("<Project />\n", encoding="utf-8")

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
                ["dotnet", "build", str(managed_test_project_path), "-c", "Release"],
                run_process_mock.call_args_list[1].args[0],
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
            self.assertEqual(5, project_count - folder_count)
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

            self.assertNotIn("generatedRoot", matrices_by_id["windows-dev-output"])
            self.assertNotIn("generatedRoot", matrices_by_id["windows-reference-trace"])

            workspace_root = repo_root / "solutions" / "subjects" / "FixtureSubject"
            dev_generated_root = workspace_module._subject_generated_solution_root(
                workspace_root,
                matrix_id="windows-dev-output",
                multi_matrix=True,
            )
            trace_generated_root = workspace_module._subject_generated_solution_root(
                workspace_root,
                matrix_id="windows-reference-trace",
                multi_matrix=True,
            )

            dev_generated_source = (
                dev_generated_root
                / "analysis"
                / "generated"
                / "generated"
                / "native-reference.generated.cpp"
            )
            trace_generated_source = (
                trace_generated_root
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

    def test_build_subject_workspace_skips_cmake_for_managed_only_matrix(self) -> None:
        workspace_module = load_module(PROJECT_WORKSPACE_MODULE_PATH, "chaos_project_workspace_subject_build_managed_only")
        repo_root = self._make_repo_root("subject-build-managed-only")
        self._write_managed_only_subject_fixture(repo_root)
        workspace_root = repo_root / "solutions" / "subjects" / "ManagedOnlySubject"
        manifest_path = workspace_root / "workspace.manifest.json"
        managed_test_project_path = workspace_root / "managed-tests" / "ManagedOnlySubject.DeclaredBenchmarkHost.csproj"
        managed_test_project_path.parent.mkdir(parents=True, exist_ok=True)
        managed_test_project_path.write_text("<Project />\n", encoding="utf-8")
        write_json(
            manifest_path,
            {
                "kind": "subject-workspace",
                "subjectId": "ManagedOnlySubject",
                "defaultMatrix": "windows-perf-dev",
                "managedSolutionPath": "solutions/subjects/ManagedOnlySubject/ManagedOnlySubject.sln",
                "managedProjects": [
                    {
                        "projectId": "managed/ManagedOnlySubject/ManagedOnlySubject",
                        "projectPath": "subjects/ManagedOnlySubject/source/ManagedOnlySubject.csproj",
                    }
                ],
                "managedTestProjects": [
                    {
                        "projectId": "managed-test/ManagedOnlySubject/benchmark-host",
                        "projectPath": "solutions/subjects/ManagedOnlySubject/managed-tests/ManagedOnlySubject.DeclaredBenchmarkHost.csproj",
                    }
                ],
                "nativeProjects": [],
                "nativeTestProjects": [],
                "matrices": [
                    {
                        "matrixId": "windows-perf-dev",
                        "managedProjectIds": ["managed/ManagedOnlySubject/ManagedOnlySubject"],
                        "managedTestProjectIds": ["managed-test/ManagedOnlySubject/benchmark-host"],
                        "nativeProjectIds": [],
                        "nativeTestProjectIds": [],
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
                        {"id": "subject/ManagedOnlySubject", "matrix": "windows-perf-dev"},
                    )

            self.assertEqual("solutions/subjects/ManagedOnlySubject/build.report.json", result["buildReportPath"])
            report = json.loads((repo_root / result["buildReportPath"]).read_text(encoding="utf-8"))
            self.assertEqual(["windows-perf-dev"], report["builtMatrices"])
            self.assertEqual(2, run_process_mock.call_count)
            self.assertEqual(
                ["dotnet", "build", str(repo_root / "subjects" / "ManagedOnlySubject" / "source" / "ManagedOnlySubject.csproj"), "-c", "Release"],
                run_process_mock.call_args_list[0].args[0],
            )
            self.assertEqual(
                ["dotnet", "build", str(managed_test_project_path), "-c", "Release"],
                run_process_mock.call_args_list[1].args[0],
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
                **kwargs: object,
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
                **kwargs: object,
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

    def test_generate_all_workspaces_includes_managed_only_subjects_without_native_workspace_generation(self) -> None:
        workspace_module = load_module(PROJECT_WORKSPACE_MODULE_PATH, "chaos_project_workspace_all_include_managed_only")
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
                [
                    "solutions/subjects/FixtureSubject/workspace.manifest.json",
                    "solutions/subjects/ManagedOnlySubject/workspace.manifest.json",
                ],
                aggregate_manifest["subjectWorkspaceManifests"],
            )
            self.assertEqual(["FixtureSubject", "ManagedOnlySubject"], generation_report["generatedSubjectIds"])
        finally:
            shutil.rmtree(repo_root, ignore_errors=True)

    def test_generate_all_workspaces_discovers_subject_without_legacy_native_reference_source(self) -> None:
        workspace_module = load_module(PROJECT_WORKSPACE_MODULE_PATH, "chaos_project_workspace_all_include_template_proof_host_subject")
        repo_root = self._make_repo_root("all-include-template-proof-host-subject")
        self._write_subject_fixture(repo_root, include_legacy_native_reference=False)
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
