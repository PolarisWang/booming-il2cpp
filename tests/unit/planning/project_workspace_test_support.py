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


def write_owner_manifest_fixture(repo_root: Path, subject_id: str, payload: dict) -> None:
    write_json(repo_root / "verification" / "catalog" / "owners" / subject_id / "owner.manifest.json", payload)
    write_json(
        repo_root / "verification" / "catalog" / "owners" / subject_id / "owner.features.json",
        {
            "subjectId": subject_id,
            "features": [],
        },
    )


def owner_manifest_path(repo_root: Path, subject_id: str) -> Path:
    return repo_root / "verification" / "catalog" / "owners" / subject_id / "owner.manifest.json"


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


def write_windows_subject_native_aot_project_stub(
    configure_root: Path,
    *,
    host_subdir: str,
    project_text: str = "<Project />\n",
) -> None:
    native_aot_project = configure_root / host_subdir / "chaos_subject_native_aot.vcxproj"
    native_aot_project.parent.mkdir(parents=True, exist_ok=True)
    native_aot_project.write_text(project_text, encoding="utf-8")


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

class ProjectWorkspaceTestSupport(unittest.TestCase):
    @classmethod
    def setUpClass(cls) -> None:
        TEST_TMP_ROOT.mkdir(parents=True, exist_ok=True)

    def _make_repo_root(self, prefix: str) -> Path:
        repo_root = TEST_TMP_ROOT / f"{prefix}-{uuid.uuid4().hex}"
        repo_root.mkdir(parents=True, exist_ok=False)
        return repo_root

    def _write_shared_runtime_fixture(self, repo_root: Path) -> None:
        runtime_project = repo_root / "src" / "reference" / "Chaos.TestFramework.Runtime" / "Chaos.TestFramework.Runtime.csproj"
        runtime_project.parent.mkdir(parents=True, exist_ok=True)
        runtime_project.write_text(
            "\n".join(
                [
                    '<Project Sdk="Microsoft.NET.Sdk">',
                    "  <PropertyGroup>",
                    "    <TargetFramework>net8.0</TargetFramework>",
                    "    <OutputType>Exe</OutputType>",
                    "  </PropertyGroup>",
                    "</Project>",
                    "",
                ]
            ),
            encoding="utf-8",
        )

    def _write_subject_fixture(
        self,
        repo_root: Path,
        *,
        subject_id: str = "FixtureSubject",
        include_legacy_native_reference: bool = True,
    ) -> None:
        owner_root = repo_root / "verification" / "catalog" / "owners" / subject_id
        source_project = owner_root / "support" / "host" / f"{subject_id}.csproj"
        unit_project = owner_root / "support" / "unit" / f"{subject_id}.Subject.UnitTests" / f"{subject_id}.Subject.UnitTests.csproj"
        native_reference_root = repo_root / "subjects" / subject_id / "validation" / "proof" / "native-reference"
        subject_exec_root = repo_root / "artifacts" / "subjects" / subject_id / "runs" / "subject-exec"
        generated_root = subject_exec_root / "analysis" / "generated"

        self._write_shared_runtime_fixture(repo_root)
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
        write_json(
            generated_root / "native-reference.manifest.json",
            {
                "runtimeExecutionKind": "assembly-bound-native-reference-skeleton",
                "preferredAssemblyDispatchSubjectId": "FixtureSubject/Program::Main:System.Int32(System.String[])",
                "translationUnitPages": [
                    {
                        "pageNumber": 1,
                        "methodCount": 1,
                        "path": "generated/runtime/native-reference.runtime-skeleton.page-0001.cpp",
                        "firstMethodSubjectId": "FixtureSubject/Program::Main:System.Int32(System.String[])",
                        "lastMethodSubjectId": "FixtureSubject/Program::Main:System.Int32(System.String[])",
                    }
                ],
            },
        )
        (generated_root / "native-reference.plan.json").write_text("{}\n", encoding="utf-8")
        (generated_root / "generated" / "native-reference.generated.cpp").write_text("// generated\n", encoding="utf-8")

        manifest_payload = {
            "subjectId": subject_id,
            "displayName": subject_id,
            "defaultGoal": "correctness.dev",
            "defaultMatrix": "windows-dev-output",
            "defaultValidationProfile": "proof-dev",
            "source": {
                "type": "dotnet-project",
                "path": f"verification/catalog/owners/{subject_id}/support/host/{subject_id}.csproj",
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
                    "project": f"verification/catalog/owners/{subject_id}/support/unit/{subject_id}.Subject.UnitTests/{subject_id}.Subject.UnitTests.csproj",
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
        }
        write_owner_manifest_fixture(repo_root, subject_id, manifest_payload)

        (repo_root / "build" / "toolchains").mkdir(parents=True, exist_ok=True)
        (repo_root / "build" / "toolchains" / "linux-x64.cmake").write_text("# linux\n", encoding="utf-8")

    def _write_core_fixture(self, repo_root: Path) -> None:
        managed_projects = [
            "Chaos.IL2CPP.Contracts",
            "Chaos.IL2CPP.Loader",
            "Chaos.IL2CPP.SemanticWorld",
            "Chaos.IL2CPP.MetadataWriter",
            "Chaos.IL2CPP.Generator",
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
        owner_root = repo_root / "verification" / "catalog" / "owners" / subject_id
        source_project = owner_root / "support" / "host" / f"{subject_id}.csproj"
        perf_project = owner_root / "support" / "perf" / f"{subject_id}.Subject.PerfHarness" / f"{subject_id}.Subject.PerfHarness.csproj"

        self._write_shared_runtime_fixture(repo_root)
        source_project.parent.mkdir(parents=True, exist_ok=True)
        source_project.write_text("<Project />\n", encoding="utf-8")
        perf_project.parent.mkdir(parents=True, exist_ok=True)
        perf_project.write_text("<Project />\n", encoding="utf-8")

        manifest_payload = {
            "subjectId": subject_id,
            "displayName": subject_id,
            "defaultGoal": "perf.dev",
            "defaultMatrix": "windows-perf-dev",
            "defaultValidationProfile": "perf-dev",
            "source": {
                "type": "dotnet-project",
                "path": f"verification/catalog/owners/{subject_id}/support/host/{subject_id}.csproj",
                "entry": f"{subject_id}/Program::Main()",
            },
            "validationProfiles": {
                "perf-dev": ["perf"],
            },
            "validation": {
                "perf": {
                    "kind": "perf",
                    "project": f"verification/catalog/owners/{subject_id}/support/perf/{subject_id}.Subject.PerfHarness/{subject_id}.Subject.PerfHarness.csproj",
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
        }
        write_owner_manifest_fixture(repo_root, subject_id, manifest_payload)

    def _write_hotupdate_subject_fixture(
        self,
        repo_root: Path,
        *,
        subject_id: str = "FixtureHotUpdateSubject",
    ) -> None:
        owner_root = repo_root / "verification" / "catalog" / "owners" / subject_id
        scenario_root = repo_root / "verification" / "catalog" / "scenarios" / subject_id / "FixtureHotUpdateSolution"
        host_root = owner_root / "support" / "host"
        host_project = host_root / f"{subject_id}.Host.csproj"
        patch_project = scenario_root / "Patch" / f"{subject_id}.Patch.csproj"
        shared_project = scenario_root / "Shared" / f"{subject_id}.Shared.csproj"
        solution_path = host_root / f"{subject_id}.sln"

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

        manifest_payload = {
            "subjectId": subject_id,
            "displayName": subject_id,
            "defaultGoal": "correctness.dev",
            "defaultMatrix": "windows-hotupdate-proof",
            "defaultValidationProfile": "proof-dev",
            "engineeringProfile": "hot-update-host",
            "source": {
                "type": "dotnet-project",
                "path": f"verification/catalog/owners/{subject_id}/support/host/{subject_id}.sln",
                "primaryProjectPath": f"verification/catalog/owners/{subject_id}/support/host/{subject_id}.Host.csproj",
                "entry": f"{subject_id}.Host/Program::Main()",
            },
            "hotUpdate": {
                "patchProjectPaths": [
                    f"verification/catalog/scenarios/{subject_id}/FixtureHotUpdateSolution/Patch/{subject_id}.Patch.csproj",
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
                    "project": "src/tools/Chaos.IL2CPP.Tools.Benchmark.WorkloadEntry.PerfHarness/Chaos.IL2CPP.Tools.Benchmark.WorkloadEntry.PerfHarness.csproj",
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
        }
        write_owner_manifest_fixture(repo_root, subject_id, manifest_payload)
