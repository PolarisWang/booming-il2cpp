from __future__ import annotations

from pathlib import Path

from tests._support.fs import make_temp_repo_root, write_json


PROJECT_WORKSPACE_AREA = "project-workspace"


def make_project_workspace_repo_root(prefix: str) -> Path:
    return make_temp_repo_root(PROJECT_WORKSPACE_AREA, prefix)


def write_shared_runtime_fixture(repo_root: Path) -> None:
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


def write_subject_fixture(
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

    write_shared_runtime_fixture(repo_root)
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


def write_core_fixture(repo_root: Path) -> None:
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


def write_managed_only_subject_fixture(repo_root: Path, *, subject_id: str = "ManagedOnlySubject") -> None:
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

    write_shared_runtime_fixture(repo_root)
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


def _write_hotupdate_source_project(path: Path, assembly_name: str) -> None:
    path.parent.mkdir(parents=True, exist_ok=True)
    path.write_text(
        "\n".join(
            [
                "<Project>",
                "  <PropertyGroup>",
                f"    <AssemblyName>{assembly_name}</AssemblyName>",
                "  </PropertyGroup>",
                "</Project>",
                "",
            ]
        ),
        encoding="utf-8",
    )


def write_hotupdate_subject_fixture(
    repo_root: Path,
    *,
    subject_id: str = "FixtureHotUpdateSubject",
) -> None:
    source_root = repo_root / "subjects" / subject_id / "source"
    host_project = source_root / "Host" / f"{subject_id}.Host.csproj"
    patch_project = source_root / "Patch" / f"{subject_id}.Patch.csproj"
    shared_project = source_root / "Shared" / f"{subject_id}.Shared.csproj"
    solution_path = source_root / f"{subject_id}.sln"

    _write_hotupdate_source_project(host_project, f"{subject_id}.Host")
    _write_hotupdate_source_project(patch_project, f"{subject_id}.Patch")
    _write_hotupdate_source_project(shared_project, f"{subject_id}.Shared")
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
        },
    )
