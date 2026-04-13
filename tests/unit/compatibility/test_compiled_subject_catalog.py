from __future__ import annotations

import importlib.util
import json
import shutil
import sys
import unittest
from pathlib import Path


REPO_ROOT = Path(__file__).resolve().parents[3]
COMPILED_CATALOG_MODULE_PATH = REPO_ROOT / "build" / "toolchains" / "run" / "testing" / "compiled_catalog.py"


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
    path.write_text(json.dumps(payload, ensure_ascii=False, indent=2) + "\n", encoding="utf-8")


class CompiledSubjectCatalogTests(unittest.TestCase):
    def test_compiled_catalog_projects_managed_output_engineering_entries(self) -> None:
        compiled_catalog_module = load_module(COMPILED_CATALOG_MODULE_PATH, "chaos_compiled_subject_catalog_managed")
        repo_root = REPO_ROOT / "artifacts" / ".tmp-tests" / "compiled-subject-catalog" / "managed-output"
        manifest_path = repo_root / "subjects" / "FixtureManagedCatalog" / "subject.manifest.json"

        manifest = {
            "subjectId": "FixtureManagedCatalog",
            "displayName": "FixtureManagedCatalog",
            "category": "canonical",
            "defaultGoal": "correctness.dev",
            "defaultMatrix": "windows-managed-output",
            "defaultValidationProfile": "proof-dev",
            "source": {
                "type": "dotnet-project",
                "path": "subjects/FixtureManagedCatalog/source/FixtureManagedCatalog.csproj",
                "entry": "FixtureManagedCatalog/Program::Main()",
            },
            "engineeringProfile": "managed-output",
            "validationProfiles": {
                "proof-dev": ["proof"],
            },
            "validation": {
                "proof": {
                    "kind": "proof",
                    "defaultVariant": "CHECK",
                }
            },
            "executionPipelines": [
                {
                    "pipelineId": "managed-runtime-output",
                    "stages": [
                        {"stageId": "source-resolve", "kind": "source-resolve", "scope": "shared", "bucket": "source", "dependsOn": []},
                        {"stageId": "host-input-build", "kind": "host-input-build", "scope": "shared", "bucket": "host-input", "dependsOn": ["source-resolve"]},
                        {"stageId": "runtime-managed-output", "kind": "runtime-managed-output", "scope": "matrix", "bucket": "runtime", "dependsOn": ["host-input-build"]},
                        {"stageId": "report-assemble", "kind": "report-assemble", "scope": "matrix", "bucket": "report", "dependsOn": ["runtime-managed-output"]},
                    ],
                }
            ],
            "environmentMatrices": [
                {
                    "matrixId": "windows-managed-output",
                    "pipelineId": "managed-runtime-output",
                    "supportedGoals": ["correctness.dev"],
                    "executionContext": {
                        "hostPlatform": "windows-x64",
                        "targetPlatform": "windows-x64",
                        "toolchainProfile": "dotnet-managed",
                        "runtimeProfile": "managed-output",
                    },
                    "validationIntent": {
                        "validationMode": "output",
                        "adaptationLevel": "observable-output",
                        "expectedOutcome": "pass",
                    },
                    "artifactPlan": {
                        "requiredBuckets": ["source", "host-input", "runtime", "report"],
                        "evidenceTerminalBucket": "runtime",
                    },
                }
            ],
        }

        try:
            write_json(manifest_path, manifest)

            catalog = compiled_catalog_module.build_compiled_subject_catalog(
                repo_root=repo_root,
                subject_id="FixtureManagedCatalog",
                declared_catalog={
                    "subjectId": "FixtureManagedCatalog",
                    "declaredUnitTests": [],
                    "declaredBenchmarks": [],
                },
            )

            self.assertEqual("FixtureManagedCatalog", catalog["subjectId"])
            self.assertEqual("managed-output", catalog["engineeringProfile"])
            self.assertEqual(
                ["managed-build", "managed-runtime-output", "project-graph"],
                sorted(entry["kind"] for entry in catalog["engineeringValidations"]),
            )
            self.assertEqual([], catalog["engineeringWorkloads"])
            self.assertEqual([], catalog["declaredUnitTests"])
            self.assertEqual([], catalog["declaredBenchmarks"])
        finally:
            shutil.rmtree(repo_root, ignore_errors=True)

    def test_compiled_catalog_projects_native_executable_entries_and_merges_declared_tests(self) -> None:
        compiled_catalog_module = load_module(COMPILED_CATALOG_MODULE_PATH, "chaos_compiled_subject_catalog_native")
        repo_root = REPO_ROOT / "artifacts" / ".tmp-tests" / "compiled-subject-catalog" / "native-executable"
        manifest_path = repo_root / "subjects" / "FixtureNativeCatalog" / "subject.manifest.json"

        manifest = {
            "subjectId": "FixtureNativeCatalog",
            "displayName": "FixtureNativeCatalog",
            "category": "canonical",
            "defaultGoal": "correctness.dev",
            "defaultMatrix": "windows-native-output",
            "defaultValidationProfile": "proof-dev",
            "source": {
                "type": "dotnet-project",
                "path": "subjects/FixtureNativeCatalog/source/FixtureNativeCatalog.csproj",
                "entry": "FixtureNativeCatalog/Program::Main()",
            },
            "engineeringProfile": "native-executable",
            "validationProfiles": {
                "proof-dev": ["proof"],
            },
            "validation": {
                "proof": {
                    "kind": "proof",
                    "defaultVariant": "CHECK",
                }
            },
            "executionPipelines": [
                {
                    "pipelineId": "proof-runtime-output",
                    "stages": [
                        {"stageId": "source-resolve", "kind": "source-resolve", "scope": "shared", "bucket": "source", "dependsOn": []},
                        {"stageId": "host-input-build", "kind": "host-input-build", "scope": "shared", "bucket": "host-input", "dependsOn": ["source-resolve"]},
                        {"stageId": "analysis-frontend", "kind": "analysis-frontend", "scope": "shared", "bucket": "analysis", "dependsOn": ["host-input-build"]},
                        {"stageId": "generated-native-proof", "kind": "generated-native-proof", "scope": "shared", "bucket": "generated", "dependsOn": ["analysis-frontend"]},
                        {"stageId": "build-target", "kind": "build-target", "scope": "matrix", "bucket": "build", "dependsOn": ["generated-native-proof"]},
                        {"stageId": "runtime-observe", "kind": "runtime-observe", "scope": "matrix", "bucket": "runtime", "dependsOn": ["build-target"]},
                        {"stageId": "report-assemble", "kind": "report-assemble", "scope": "matrix", "bucket": "report", "dependsOn": ["runtime-observe"]},
                    ],
                }
            ],
            "environmentMatrices": [
                {
                    "matrixId": "windows-native-output",
                    "pipelineId": "proof-runtime-output",
                    "supportedGoals": ["correctness.dev"],
                    "executionContext": {
                        "hostPlatform": "windows-x64",
                        "targetPlatform": "windows-x64",
                        "toolchainProfile": "msvc-reference",
                        "runtimeProfile": "native-proof-output",
                    },
                    "validationIntent": {
                        "validationMode": "output",
                        "adaptationLevel": "observable-output",
                        "expectedOutcome": "pass",
                    },
                    "artifactPlan": {
                        "requiredBuckets": ["source", "host-input", "analysis", "generated", "build", "runtime", "report"],
                        "evidenceTerminalBucket": "runtime",
                    },
                }
            ],
        }

        try:
            write_json(manifest_path, manifest)

            catalog = compiled_catalog_module.build_compiled_subject_catalog(
                repo_root=repo_root,
                subject_id="FixtureNativeCatalog",
                declared_catalog={
                    "subjectId": "FixtureNativeCatalog",
                    "declaredUnitTests": [{"stableId": "unit-1"}],
                    "declaredBenchmarks": [{"stableId": "bench-1"}],
                },
            )

            self.assertEqual(
                ["native-executable-smoke", "project-graph"],
                sorted(entry["kind"] for entry in catalog["engineeringValidations"]),
            )
            self.assertEqual(
                ["codegen", "convert", "native-link"],
                sorted(entry["kind"] for entry in catalog["engineeringWorkloads"]),
            )
            self.assertEqual([{"stableId": "unit-1"}], catalog["declaredUnitTests"])
            self.assertEqual([{"stableId": "bench-1"}], catalog["declaredBenchmarks"])
        finally:
            shutil.rmtree(repo_root, ignore_errors=True)

    def test_compiled_catalog_projects_hot_update_host_entries(self) -> None:
        compiled_catalog_module = load_module(COMPILED_CATALOG_MODULE_PATH, "chaos_compiled_subject_catalog_hot_update")
        repo_root = REPO_ROOT / "artifacts" / ".tmp-tests" / "compiled-subject-catalog" / "hot-update-host"
        manifest_path = repo_root / "subjects" / "FixtureHotUpdateCatalog" / "subject.manifest.json"

        manifest = {
            "subjectId": "FixtureHotUpdateCatalog",
            "displayName": "FixtureHotUpdateCatalog",
            "category": "canonical",
            "defaultGoal": "correctness.dev",
            "defaultMatrix": "windows-hot-update",
            "defaultValidationProfile": "proof-dev",
            "source": {
                "type": "dotnet-project",
                "path": "subjects/FixtureHotUpdateCatalog/source/FixtureHotUpdateCatalog.csproj",
                "entry": "FixtureHotUpdateCatalog/Program::Main()",
            },
            "engineeringProfile": "hot-update-host",
            "executablePlan": "host-plus-hot-update-patch",
            "validationProfiles": {
                "proof-dev": ["proof"],
            },
            "validation": {
                "proof": {
                    "kind": "proof",
                    "defaultVariant": "CHECK",
                }
            },
            "executionPipelines": [
                {
                    "pipelineId": "hot-update-host",
                    "stages": [
                        {"stageId": "source-resolve", "kind": "source-resolve", "scope": "shared", "bucket": "source", "dependsOn": []},
                        {"stageId": "host-input-build", "kind": "host-input-build", "scope": "shared", "bucket": "host-input", "dependsOn": ["source-resolve"]},
                        {"stageId": "patch-generate", "kind": "patch-generate", "scope": "shared", "bucket": "generated", "dependsOn": ["host-input-build"]},
                        {"stageId": "patch-load", "kind": "patch-load", "scope": "matrix", "bucket": "runtime", "dependsOn": ["patch-generate"]},
                        {"stageId": "patch-roundtrip", "kind": "patch-roundtrip", "scope": "matrix", "bucket": "runtime", "dependsOn": ["patch-load"]},
                        {"stageId": "patch-rollback", "kind": "patch-rollback", "scope": "matrix", "bucket": "runtime", "dependsOn": ["patch-roundtrip"]},
                        {"stageId": "report-assemble", "kind": "report-assemble", "scope": "matrix", "bucket": "report", "dependsOn": ["patch-rollback"]},
                    ],
                }
            ],
            "environmentMatrices": [
                {
                    "matrixId": "windows-hot-update",
                    "pipelineId": "hot-update-host",
                    "supportedGoals": ["correctness.dev"],
                    "executionContext": {
                        "hostPlatform": "windows-x64",
                        "targetPlatform": "windows-x64",
                        "toolchainProfile": "dotnet-managed",
                        "runtimeProfile": "hot-update-host",
                    },
                    "validationIntent": {
                        "validationMode": "output",
                        "adaptationLevel": "observable-output",
                        "expectedOutcome": "pass",
                    },
                    "artifactPlan": {
                        "requiredBuckets": ["source", "host-input", "generated", "runtime", "report"],
                        "evidenceTerminalBucket": "runtime",
                    },
                }
            ],
        }

        try:
            write_json(manifest_path, manifest)

            catalog = compiled_catalog_module.build_compiled_subject_catalog(
                repo_root=repo_root,
                subject_id="FixtureHotUpdateCatalog",
                declared_catalog={
                    "subjectId": "FixtureHotUpdateCatalog",
                    "declaredUnitTests": [],
                    "declaredBenchmarks": [],
                },
            )

            self.assertEqual(
                ["host-build", "patch-load", "patch-rollback", "patch-roundtrip"],
                sorted(entry["kind"] for entry in catalog["engineeringValidations"]),
            )
            self.assertEqual(
                ["patch-generation"],
                sorted(entry["kind"] for entry in catalog["engineeringWorkloads"]),
            )
        finally:
            shutil.rmtree(repo_root, ignore_errors=True)

    def test_compiled_catalog_projects_native_workloads_from_non_default_perf_matrix(self) -> None:
        compiled_catalog_module = load_module(COMPILED_CATALOG_MODULE_PATH, "chaos_compiled_subject_catalog_mixed_perf")
        repo_root = REPO_ROOT / "artifacts" / ".tmp-tests" / "compiled-subject-catalog" / "mixed-perf"
        manifest_path = repo_root / "subjects" / "FixtureMixedPerfCatalog" / "subject.manifest.json"

        manifest = {
            "subjectId": "FixtureMixedPerfCatalog",
            "displayName": "FixtureMixedPerfCatalog",
            "category": "canonical",
            "defaultGoal": "correctness.dev",
            "defaultMatrix": "windows-managed-output",
            "defaultValidationProfile": "proof-dev",
            "source": {
                "type": "dotnet-project",
                "path": "subjects/FixtureMixedPerfCatalog/source/FixtureMixedPerfCatalog.csproj",
                "entry": "FixtureMixedPerfCatalog/Program::Main()",
            },
            "engineeringProfile": "managed-output",
            "validationProfiles": {
                "proof-dev": ["proof"],
                "perf-profile": ["perf"],
            },
            "validation": {
                "proof": {
                    "kind": "proof",
                    "defaultVariant": "CHECK",
                },
                "perf": {
                    "kind": "perf",
                    "driver": "native-runtime-perf",
                    "defaultVariant": "PROFILE",
                },
            },
            "executionPipelines": [
                {
                    "pipelineId": "managed-runtime-output",
                    "stages": [
                        {"stageId": "source-resolve", "kind": "source-resolve", "scope": "shared", "bucket": "source", "dependsOn": []},
                        {"stageId": "host-input-build", "kind": "host-input-build", "scope": "shared", "bucket": "host-input", "dependsOn": ["source-resolve"]},
                        {"stageId": "runtime-managed-output", "kind": "runtime-managed-output", "scope": "matrix", "bucket": "runtime", "dependsOn": ["host-input-build"]},
                        {"stageId": "report-assemble", "kind": "report-assemble", "scope": "matrix", "bucket": "report", "dependsOn": ["runtime-managed-output"]},
                    ],
                },
                {
                    "pipelineId": "native-benchmark",
                    "stages": [
                        {"stageId": "source-resolve", "kind": "source-resolve", "scope": "shared", "bucket": "source", "dependsOn": []},
                        {"stageId": "host-input-build", "kind": "host-input-build", "scope": "shared", "bucket": "host-input", "dependsOn": ["source-resolve"]},
                        {"stageId": "analysis-frontend", "kind": "analysis-frontend", "scope": "shared", "bucket": "analysis", "dependsOn": ["host-input-build"]},
                        {"stageId": "generated-native-aot", "kind": "generated-native-aot", "scope": "shared", "bucket": "generated", "dependsOn": ["analysis-frontend"]},
                        {"stageId": "build-target", "kind": "build-target", "scope": "matrix", "bucket": "build", "dependsOn": ["generated-native-aot"]},
                        {"stageId": "native-runtime-perf", "kind": "native-runtime-perf", "scope": "matrix", "bucket": "runtime", "dependsOn": ["build-target"]},
                        {"stageId": "report-assemble", "kind": "report-assemble", "scope": "matrix", "bucket": "report", "dependsOn": ["native-runtime-perf"]},
                    ],
                },
            ],
            "environmentMatrices": [
                {
                    "matrixId": "windows-managed-output",
                    "pipelineId": "managed-runtime-output",
                    "supportedGoals": ["correctness.dev"],
                    "executionContext": {
                        "hostPlatform": "windows-x64",
                        "targetPlatform": "windows-x64",
                        "toolchainProfile": "dotnet-managed",
                        "runtimeProfile": "managed-output",
                    },
                    "validationIntent": {
                        "validationMode": "output",
                        "adaptationLevel": "observable-output",
                        "expectedOutcome": "pass",
                    },
                    "artifactPlan": {
                        "requiredBuckets": ["source", "host-input", "runtime", "report"],
                        "evidenceTerminalBucket": "runtime",
                    },
                },
                {
                    "matrixId": "windows-native-perf",
                    "pipelineId": "native-benchmark",
                    "supportedGoals": ["perf.release"],
                    "executionContext": {
                        "hostPlatform": "windows-x64",
                        "targetPlatform": "windows-x64",
                        "toolchainProfile": "msvc-reference",
                        "runtimeProfile": "native-perf-profile",
                    },
                    "validationIntent": {
                        "validationMode": "perf",
                        "adaptationLevel": "native-runtime",
                        "expectedOutcome": "pass",
                    },
                    "artifactPlan": {
                        "requiredBuckets": ["source", "host-input", "analysis", "generated", "build", "runtime", "report"],
                        "evidenceTerminalBucket": "report",
                    },
                },
            ],
        }

        try:
            write_json(manifest_path, manifest)

            catalog = compiled_catalog_module.build_compiled_subject_catalog(
                repo_root=repo_root,
                subject_id="FixtureMixedPerfCatalog",
                declared_catalog={
                    "subjectId": "FixtureMixedPerfCatalog",
                    "declaredUnitTests": [],
                    "declaredBenchmarks": [],
                },
            )

            self.assertEqual(
                ["codegen", "convert", "native-link"],
                sorted(entry["kind"] for entry in catalog["engineeringWorkloads"]),
            )
        finally:
            shutil.rmtree(repo_root, ignore_errors=True)

    def test_declared_catalog_resolves_primary_project_assembly_from_solution_source(self) -> None:
        compiled_catalog_module = load_module(COMPILED_CATALOG_MODULE_PATH, "chaos_compiled_subject_catalog_solution_source")
        repo_root = REPO_ROOT / "artifacts" / ".tmp-tests" / "compiled-subject-catalog" / "solution-source"
        manifest_path = repo_root / "subjects" / "FixtureSolutionCatalog" / "subject.manifest.json"
        project_path = repo_root / "subjects" / "FixtureSolutionCatalog" / "source" / "App" / "FixtureSolutionCatalog.App.csproj"
        assembly_path = (
            repo_root
            / "subjects"
            / "FixtureSolutionCatalog"
            / "source"
            / "App"
            / "bin"
            / "Debug"
            / "net8.0"
            / "FixtureSolutionCatalog.App.dll"
        )
        solution_path = repo_root / "subjects" / "FixtureSolutionCatalog" / "source" / "FixtureSolutionCatalog.sln"

        manifest = {
            "subjectId": "FixtureSolutionCatalog",
            "displayName": "FixtureSolutionCatalog",
            "category": "canonical",
            "defaultGoal": "correctness.dev",
            "defaultMatrix": "windows-managed-output",
            "defaultValidationProfile": "proof-dev",
            "source": {
                "type": "dotnet-project",
                "path": "subjects/FixtureSolutionCatalog/source/FixtureSolutionCatalog.sln",
                "primaryProjectPath": "subjects/FixtureSolutionCatalog/source/App/FixtureSolutionCatalog.App.csproj",
                "entry": "FixtureSolutionCatalog.App/Program::Main()",
            },
            "sourceModel": "dotnet-solution",
            "engineeringProfile": "managed-output",
            "validationProfiles": {
                "proof-dev": ["proof"],
            },
            "validation": {
                "proof": {
                    "kind": "proof",
                    "defaultVariant": "CHECK",
                }
            },
            "executionPipelines": [
                {
                    "pipelineId": "managed-runtime-output",
                    "stages": [
                        {"stageId": "source-resolve", "kind": "source-resolve", "scope": "shared", "bucket": "source", "dependsOn": []},
                        {"stageId": "host-input-build", "kind": "host-input-build", "scope": "shared", "bucket": "host-input", "dependsOn": ["source-resolve"]},
                        {"stageId": "runtime-managed-output", "kind": "runtime-managed-output", "scope": "matrix", "bucket": "runtime", "dependsOn": ["host-input-build"]},
                        {"stageId": "report-assemble", "kind": "report-assemble", "scope": "matrix", "bucket": "report", "dependsOn": ["runtime-managed-output"]},
                    ],
                }
            ],
            "environmentMatrices": [
                {
                    "matrixId": "windows-managed-output",
                    "pipelineId": "managed-runtime-output",
                    "supportedGoals": ["correctness.dev"],
                    "executionContext": {
                        "hostPlatform": "windows-x64",
                        "targetPlatform": "windows-x64",
                        "toolchainProfile": "dotnet-managed",
                        "runtimeProfile": "managed-output",
                    },
                    "validationIntent": {
                        "validationMode": "output",
                        "adaptationLevel": "observable-output",
                        "expectedOutcome": "pass",
                    },
                    "artifactPlan": {
                        "requiredBuckets": ["source", "host-input", "runtime", "report"],
                        "evidenceTerminalBucket": "runtime",
                    },
                }
            ],
        }

        try:
            solution_path.parent.mkdir(parents=True, exist_ok=True)
            solution_path.write_text("", encoding="utf-8")
            project_path.parent.mkdir(parents=True, exist_ok=True)
            project_path.write_text("<Project Sdk=\"Microsoft.NET.Sdk\" />\n", encoding="utf-8")
            assembly_path.parent.mkdir(parents=True, exist_ok=True)
            assembly_path.write_text("", encoding="utf-8")
            write_json(manifest_path, manifest)

            assembly_paths = compiled_catalog_module._resolve_subject_assembly_paths(
                repo_root,
                manifest,
                build_if_missing=False,
            )

            self.assertEqual([assembly_path.resolve()], assembly_paths)
        finally:
            shutil.rmtree(repo_root, ignore_errors=True)

    def test_declared_catalog_resolves_all_solution_assemblies_from_primary_output_directory(self) -> None:
        compiled_catalog_module = load_module(COMPILED_CATALOG_MODULE_PATH, "chaos_compiled_subject_catalog_solution_multi_assembly")
        repo_root = REPO_ROOT / "artifacts" / ".tmp-tests" / "compiled-subject-catalog" / "solution-multi-assembly"
        manifest_path = repo_root / "subjects" / "FixtureSolutionCatalog" / "subject.manifest.json"
        solution_path = repo_root / "subjects" / "FixtureSolutionCatalog" / "source" / "FixtureSolutionCatalog.sln"
        app_project_path = repo_root / "subjects" / "FixtureSolutionCatalog" / "source" / "App" / "FixtureSolutionCatalog.App.csproj"
        support_project_path = repo_root / "subjects" / "FixtureSolutionCatalog" / "source" / "Support" / "FixtureSolutionCatalog.Support.csproj"
        app_assembly_path = (
            repo_root
            / "subjects"
            / "FixtureSolutionCatalog"
            / "source"
            / "App"
            / "bin"
            / "Debug"
            / "net8.0"
            / "FixtureSolutionCatalog.App.dll"
        )
        support_assembly_path = app_assembly_path.parent / "FixtureSolutionCatalog.Support.dll"

        manifest = {
            "subjectId": "FixtureSolutionCatalog",
            "displayName": "FixtureSolutionCatalog",
            "category": "canonical",
            "defaultGoal": "correctness.dev",
            "defaultMatrix": "windows-managed-output",
            "defaultValidationProfile": "proof-dev",
            "source": {
                "type": "dotnet-project",
                "path": "subjects/FixtureSolutionCatalog/source/FixtureSolutionCatalog.sln",
                "primaryProjectPath": "subjects/FixtureSolutionCatalog/source/App/FixtureSolutionCatalog.App.csproj",
                "entry": "FixtureSolutionCatalog.App/Program::Main()",
            },
            "sourceModel": "dotnet-solution",
            "engineeringProfile": "managed-output",
            "validationProfiles": {
                "proof-dev": ["proof"],
            },
            "validation": {
                "proof": {
                    "kind": "proof",
                    "defaultVariant": "CHECK",
                }
            },
            "executionPipelines": [
                {
                    "pipelineId": "managed-runtime-output",
                    "stages": [
                        {"stageId": "source-resolve", "kind": "source-resolve", "scope": "shared", "bucket": "source", "dependsOn": []},
                        {"stageId": "host-input-build", "kind": "host-input-build", "scope": "shared", "bucket": "host-input", "dependsOn": ["source-resolve"]},
                        {"stageId": "runtime-managed-output", "kind": "runtime-managed-output", "scope": "matrix", "bucket": "runtime", "dependsOn": ["host-input-build"]},
                        {"stageId": "report-assemble", "kind": "report-assemble", "scope": "matrix", "bucket": "report", "dependsOn": ["runtime-managed-output"]},
                    ],
                }
            ],
            "environmentMatrices": [
                {
                    "matrixId": "windows-managed-output",
                    "pipelineId": "managed-runtime-output",
                    "supportedGoals": ["correctness.dev"],
                    "executionContext": {
                        "hostPlatform": "windows-x64",
                        "targetPlatform": "windows-x64",
                        "toolchainProfile": "dotnet-managed",
                        "runtimeProfile": "managed-output",
                    },
                    "validationIntent": {
                        "validationMode": "output",
                        "adaptationLevel": "observable-output",
                        "expectedOutcome": "pass",
                    },
                    "artifactPlan": {
                        "requiredBuckets": ["source", "host-input", "runtime", "report"],
                        "evidenceTerminalBucket": "runtime",
                    },
                }
            ],
        }

        solution_text = """
Microsoft Visual Studio Solution File, Format Version 12.00
Project("{FAE04EC0-301F-11D3-BF4B-00C04F79EFBC}") = "FixtureSolutionCatalog.App", "App\\FixtureSolutionCatalog.App.csproj", "{11111111-1111-1111-1111-111111111111}"
EndProject
Project("{FAE04EC0-301F-11D3-BF4B-00C04F79EFBC}") = "FixtureSolutionCatalog.Support", "Support\\FixtureSolutionCatalog.Support.csproj", "{22222222-2222-2222-2222-222222222222}"
EndProject
Global
EndGlobal
""".lstrip()

        try:
            solution_path.parent.mkdir(parents=True, exist_ok=True)
            app_project_path.parent.mkdir(parents=True, exist_ok=True)
            support_project_path.parent.mkdir(parents=True, exist_ok=True)
            solution_path.write_text(solution_text, encoding="utf-8")
            app_project_path.write_text(
                "<Project Sdk=\"Microsoft.NET.Sdk\"><PropertyGroup><AssemblyName>FixtureSolutionCatalog.App</AssemblyName></PropertyGroup></Project>\n",
                encoding="utf-8",
            )
            support_project_path.write_text(
                "<Project Sdk=\"Microsoft.NET.Sdk\"><PropertyGroup><AssemblyName>FixtureSolutionCatalog.Support</AssemblyName></PropertyGroup></Project>\n",
                encoding="utf-8",
            )
            app_assembly_path.parent.mkdir(parents=True, exist_ok=True)
            app_assembly_path.write_text("", encoding="utf-8")
            support_assembly_path.write_text("", encoding="utf-8")
            write_json(manifest_path, manifest)

            assembly_paths = compiled_catalog_module._resolve_subject_assembly_paths(
                repo_root,
                manifest,
                build_if_missing=False,
            )

            self.assertEqual(
                [
                    app_assembly_path.resolve(),
                    support_assembly_path.resolve(),
                ],
                assembly_paths,
            )
        finally:
            shutil.rmtree(repo_root, ignore_errors=True)


if __name__ == "__main__":
    unittest.main()
