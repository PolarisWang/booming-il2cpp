from __future__ import annotations

import importlib.util
import json
import shutil
import subprocess
import sys
import unittest
from pathlib import Path
from typing import Any

from tests.support import clone_registry_fixture_tree, make_temp_repo_root, materialize_subject_manifest, write_json


REPO_ROOT = Path(__file__).resolve().parents[3]
REGISTRY_MODULE_PATH = REPO_ROOT / "build" / "toolchains" / "run" / "testing" / "registry.py"
PUBLIC_SPECS_MODULE_PATH = REPO_ROOT / "build" / "toolchains" / "run" / "testing" / "public_specs.py"
HOT_UPDATE_HOST_PACK_PROJECT_PATH = REPO_ROOT / "subjects" / "HotUpdateHostPack" / "source" / "HotUpdateHostPack.csproj"


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


def run_checked(arguments: list[str], *, cwd: Path) -> subprocess.CompletedProcess[str]:
    completed = subprocess.run(
        arguments,
        cwd=cwd,
        capture_output=True,
        text=True,
        encoding="utf-8",
        errors="replace",
        check=False,
    )
    if completed.returncode != 0:
        combined_output = "\n".join(part for part in [completed.stdout, completed.stderr] if part)
        raise AssertionError(f"command failed ({completed.returncode}): {' '.join(arguments)}\n{combined_output}")
    return completed


def make_stage(
    stage_id: str,
    kind: str,
    bucket: str,
    *,
    scope: str = "matrix",
    depends_on: list[str] | None = None,
) -> dict[str, Any]:
    return {
        "stageId": stage_id,
        "kind": kind,
        "scope": scope,
        "bucket": bucket,
        "dependsOn": list(depends_on or []),
    }


def make_pipeline(pipeline_id: str, stages: list[dict[str, Any]]) -> dict[str, Any]:
    return {
        "pipelineId": pipeline_id,
        "displayName": pipeline_id,
        "stages": list(stages),
    }


def build_registry_subject_manifest(
    subject_id: str,
    *,
    default_goal: str,
    default_matrix: str,
    execution_pipelines: list[dict[str, Any]],
    environment_matrices: list[dict[str, Any]],
    default_validation_profile: str = "proof-dev",
    validation_profiles: dict[str, list[str]] | None = None,
    validation: dict[str, dict[str, Any]] | None = None,
    source_path: str | None = None,
    primary_project_path: str | None = None,
    entry: str | None = None,
    category: str = "canonical",
    source_model: str = "dotnet-project-set",
    dependency_model: str = "project-reference",
    executable_plan: str = "generated-native",
    engineering_profile: str = "native-executable",
    availability: dict[str, str] | None = None,
    workload_entry: str | None = None,
    test_declaration_mode: str = "none",
) -> dict[str, Any]:
    resolved_source_path = source_path or f"subjects/{subject_id}/source/{subject_id}.csproj"
    resolved_primary_project_path = primary_project_path or resolved_source_path
    resolved_validation_profiles = validation_profiles or {default_validation_profile: ["proof"]}
    resolved_validation = validation or {
        "proof": {
            "kind": "proof",
            "defaultVariant": "CHECK",
        }
    }
    manifest = {
        "subjectId": subject_id,
        "displayName": subject_id,
        "category": category,
        "defaultGoal": default_goal,
        "defaultMatrix": default_matrix,
        "defaultValidationProfile": default_validation_profile,
        "sourceModel": source_model,
        "dependencyModel": dependency_model,
        "executablePlan": executable_plan,
        "engineeringProfile": engineering_profile,
        "availability": availability or {},
        "testDeclarationMode": test_declaration_mode,
        "source": {
            "type": "dotnet-project",
            "path": resolved_source_path,
            "primaryProjectPath": resolved_primary_project_path,
            "entry": entry or f"{subject_id}/Program::Main()",
        },
        "validationProfiles": resolved_validation_profiles,
        "validation": resolved_validation,
        "executionPipelines": list(execution_pipelines),
        "environmentMatrices": list(environment_matrices),
    }
    if workload_entry:
        manifest["workloadEntry"] = workload_entry
    return manifest


def create_registry_repo(
    prefix: str,
    *,
    subjects: list[dict[str, Any]],
    copy_registry_fixtures: bool = False,
) -> Path:
    repo_root = make_temp_repo_root("registry-scan", prefix)
    if copy_registry_fixtures:
        clone_registry_fixture_tree(repo_root)
    for manifest in subjects:
        materialize_subject_manifest(repo_root, manifest)
    return repo_root


class RegistryScanTests(unittest.TestCase):
    def test_registry_scan_collects_suites_modules_systems_and_pipelines(self) -> None:
        registry_module = load_module(REGISTRY_MODULE_PATH, "chaos_run_registry")
        specs_module = load_module(PUBLIC_SPECS_MODULE_PATH, "chaos_run_public_specs")

        index = registry_module.scan_registry(
            REPO_ROOT,
            host_platform="macos",
            public_suite_specs=specs_module.PUBLIC_TEST_SPECS,
        )

        object_ids = {item["id"] for item in index.flat_items}
        self.assertIn("contract/analysis-schema", object_ids)
        self.assertIn("contract/trace-schema", object_ids)
        self.assertIn("contract/native-abi", object_ids)
        self.assertIn("contract/native-bridge", object_ids)
        self.assertIn("module/analysis/basic", object_ids)
        self.assertIn("smoke/managed-entry-basic", object_ids)
        self.assertIn("module/managed-smoke/basic", object_ids)
        self.assertIn("module/reflection/basic", object_ids)
        self.assertIn("module/interop/basic", object_ids)
        self.assertIn("module/hosted-runtime/basic", object_ids)
        self.assertIn("module/trace-export/macos", object_ids)
        self.assertIn("gate/ios-arm64-packaging", object_ids)
        self.assertIn("gate/macos-reference-desktop", object_ids)
        self.assertIn("system/hosted-runtime-smoke", object_ids)
        self.assertIn("system/ios-packaging-gate", object_ids)
        self.assertIn("gate/linux-x64-packaging", object_ids)
        self.assertIn("system/linux-packaging-gate", object_ids)
        self.assertIn("system/macos-reference-gate", object_ids)
        self.assertIn("system/runtime-baseline-macos", object_ids)
        self.assertIn("system/trace-export-macos-smoke", object_ids)
        self.assertIn("pipeline/completion-runtime-core", object_ids)
        self.assertIn("pipeline/completion-runtime-trace-macos", object_ids)
        self.assertIn("pipeline/trace-export-macos-runtime", object_ids)
        self.assertNotIn("module/trace-export/windows", object_ids)
        self.assertNotIn("system/runtime-baseline-windows", object_ids)
        self.assertNotIn("system/trace-export-windows-smoke", object_ids)
        self.assertNotIn("pipeline/completion-runtime-trace-windows", object_ids)
        self.assertNotIn("pipeline/trace-export-windows-runtime", object_ids)
        self.assertNotIn("gate/android-arm64-smoke", object_ids)
        self.assertNotIn("gate/windows-reference-desktop", object_ids)
        self.assertNotIn("system/android-startup-gate", object_ids)
        self.assertNotIn("system/windows-reference-gate", object_ids)
        self.assertIn("subject/HotUpdateHostPack", object_ids)
        self.assertIn("subject/SolutionCorePack", object_ids)

    def test_registry_scan_filters_subjects_by_host_for_synthetic_repo(self) -> None:
        registry_module = load_module(REGISTRY_MODULE_PATH, "chaos_run_registry_host_filtering")
        windows_subject = build_registry_subject_manifest(
            "FixtureWindowsOnlySubject",
            default_goal="correctness.dev",
            default_matrix="windows-proof",
            executable_plan="managed-host",
            engineering_profile="managed-output",
            execution_pipelines=[
                make_pipeline(
                    "proof-runtime-output",
                    [
                        make_stage("host-input-build", "host-input-build", "host-input", scope="shared"),
                        make_stage("runtime-managed-output", "runtime-managed-output", "runtime", depends_on=["host-input-build"]),
                        make_stage("report-assemble", "report-assemble", "report", depends_on=["runtime-managed-output"]),
                    ],
                )
            ],
            environment_matrices=[
                {
                    "matrixId": "windows-proof",
                    "pipelineId": "proof-runtime-output",
                    "supportedGoals": ["correctness.dev"],
                    "executionContext": {
                        "hostPlatform": "windows-x64",
                        "targetPlatform": "windows-x64",
                    },
                }
            ],
        )
        macos_subject = build_registry_subject_manifest(
            "FixtureMacosOnlySubject",
            default_goal="correctness.dev",
            default_matrix="macos-proof",
            executable_plan="managed-host",
            engineering_profile="managed-output",
            execution_pipelines=[
                make_pipeline(
                    "proof-runtime-output",
                    [
                        make_stage("host-input-build", "host-input-build", "host-input", scope="shared"),
                        make_stage("runtime-managed-output", "runtime-managed-output", "runtime", depends_on=["host-input-build"]),
                        make_stage("report-assemble", "report-assemble", "report", depends_on=["runtime-managed-output"]),
                    ],
                )
            ],
            environment_matrices=[
                {
                    "matrixId": "macos-proof",
                    "pipelineId": "proof-runtime-output",
                    "supportedGoals": ["correctness.dev"],
                    "executionContext": {
                        "hostPlatform": "macos-arm64",
                        "targetPlatform": "macos-arm64",
                    },
                }
            ],
        )

        repo_root = create_registry_repo(
            "host-filtering",
            subjects=[windows_subject, macos_subject],
        )

        try:
            windows_index = registry_module.scan_registry(
                repo_root,
                host_platform="windows",
                public_suite_specs=[],
            )
            macos_index = registry_module.scan_registry(
                repo_root,
                host_platform="macos",
                public_suite_specs=[],
            )

            self.assertEqual(["subject/FixtureWindowsOnlySubject"], [item["id"] for item in windows_index.subjects])
            self.assertEqual(["windows"], windows_index.subjects[0]["supportedHosts"])
            self.assertEqual(["subject/FixtureMacosOnlySubject"], [item["id"] for item in macos_index.subjects])
            self.assertEqual(["macos"], macos_index.subjects[0]["supportedHosts"])
        finally:
            shutil.rmtree(repo_root, ignore_errors=True)

    def test_registry_scan_collects_windows_android_gate_objects(self) -> None:
        registry_module = load_module(REGISTRY_MODULE_PATH, "chaos_run_registry_windows_android_gate")
        specs_module = load_module(PUBLIC_SPECS_MODULE_PATH, "chaos_run_public_specs_windows_android_gate")
        canonical_subject = build_registry_subject_manifest(
            "FixtureManagedOutputSubject",
            default_goal="correctness.dev",
            default_matrix="windows-output",
            source_model="dotnet-solution",
            dependency_model="project-reference",
            executable_plan="managed-host",
            engineering_profile="managed-output",
            availability={"windows-x64": "ready"},
            execution_pipelines=[
                make_pipeline(
                    "managed-output",
                    [
                        make_stage("host-input-build", "host-input-build", "host-input", scope="shared"),
                        make_stage("runtime-managed-output", "runtime-managed-output", "runtime", depends_on=["host-input-build"]),
                        make_stage("report-assemble", "report-assemble", "report", depends_on=["runtime-managed-output"]),
                    ],
                )
            ],
            environment_matrices=[
                {
                    "matrixId": "windows-output",
                    "pipelineId": "managed-output",
                    "supportedGoals": ["correctness.dev"],
                    "executionContext": {
                        "hostPlatform": "windows-x64",
                        "targetPlatform": "windows-x64",
                    },
                }
            ],
            source_path="subjects/FixtureManagedOutputSubject/source/FixtureManagedOutputSubject.sln",
            primary_project_path="subjects/FixtureManagedOutputSubject/source/App/App.csproj",
            entry="FixtureManagedOutputSubject/Program::Main()",
        )
        native_proof_subject = build_registry_subject_manifest(
            "FixtureNativeProofSubject",
            default_goal="correctness.dev",
            default_matrix="windows-native-proof",
            source_model="dotnet-project-set",
            dependency_model="project-reference",
            executable_plan="generated-native",
            engineering_profile="native-executable",
            availability={"windows-x64": "ready"},
            execution_pipelines=[
                make_pipeline(
                    "native-proof",
                    [
                        make_stage("analysis-frontend", "analysis-frontend", "analysis", scope="shared"),
                        make_stage("generated-native-proof", "generated-native-proof", "generated", scope="shared", depends_on=["analysis-frontend"]),
                        make_stage("build-target", "build-target", "build", depends_on=["generated-native-proof"]),
                        make_stage("runtime-observe", "runtime-observe", "runtime", depends_on=["build-target"]),
                        make_stage("report-assemble", "report-assemble", "report", depends_on=["runtime-observe"]),
                    ],
                )
            ],
            environment_matrices=[
                {
                    "matrixId": "windows-native-proof",
                    "pipelineId": "native-proof",
                    "supportedGoals": ["correctness.dev"],
                    "executionContext": {
                        "hostPlatform": "windows-x64",
                        "targetPlatform": "windows-x64",
                    },
                }
            ],
        )
        benchmark_subject = build_registry_subject_manifest(
            "FixtureInterpreterPerfSubject",
            default_goal="perf.release",
            default_matrix="windows-interpreter-perf",
            default_validation_profile="perf-profile",
            validation_profiles={"perf-profile": ["perf"]},
            validation={
                "perf": {
                    "kind": "perf",
                    "defaultVariant": "PROFILE",
                    "driver": "native-runtime-perf",
                }
            },
            source_model="dotnet-project-set",
            dependency_model="project-reference",
            executable_plan="generated-native",
            engineering_profile="native-executable",
            availability={"windows-x64": "ready"},
            execution_pipelines=[
                make_pipeline(
                    "interpreter-perf",
                    [
                        make_stage("analysis-frontend", "analysis-frontend", "analysis", scope="shared"),
                        make_stage("generated-native-proof", "generated-native-proof", "generated", scope="shared", depends_on=["analysis-frontend"]),
                        make_stage("interpreter-runtime-perf", "interpreter-runtime-perf", "runtime", depends_on=["generated-native-proof"]),
                        make_stage("report-assemble", "report-assemble", "report", depends_on=["interpreter-runtime-perf"]),
                    ],
                )
            ],
            environment_matrices=[
                {
                    "matrixId": "windows-interpreter-perf",
                    "pipelineId": "interpreter-perf",
                    "supportedGoals": ["perf.release"],
                    "executionContext": {
                        "hostPlatform": "windows-x64",
                        "targetPlatform": "windows-x64",
                    },
                }
            ],
            workload_entry="FixtureInterpreterPerfSubject/Benchmarks::RunWorkload()",
        )
        repo_root = create_registry_repo(
            "windows-android-gates",
            subjects=[canonical_subject, native_proof_subject, benchmark_subject],
            copy_registry_fixtures=True,
        )

        try:
            index = registry_module.scan_registry(
                repo_root,
                host_platform="windows",
                public_suite_specs=specs_module.PUBLIC_TEST_SPECS,
            )

            object_ids = {item["id"] for item in index.flat_items}
            self.assertIn("gate/android-arm64-smoke", object_ids)
            self.assertIn("gate/windows-reference-desktop", object_ids)
            self.assertIn("system/android-startup-gate", object_ids)
            self.assertIn("system/windows-reference-gate", object_ids)
            self.assertEqual(
                {
                    "subject/FixtureManagedOutputSubject",
                    "subject/FixtureNativeProofSubject",
                    "subject/FixtureInterpreterPerfSubject",
                },
                {item["id"] for item in index.subjects},
            )
            self.assertNotIn("system/linux-packaging-gate-macos-only", object_ids)

            canonical_item = next(item for item in index.flat_items if item["id"] == "subject/FixtureManagedOutputSubject")
            self.assertEqual("canonical", canonical_item["category"])
            self.assertEqual("correctness.dev", canonical_item["defaultGoalId"])
            self.assertEqual("windows-output", canonical_item["defaultMatrixId"])
            self.assertEqual("dotnet-solution", canonical_item["sourceModel"])
            self.assertEqual("managed-output", canonical_item["engineeringProfile"])

            subject_item = next(item for item in index.flat_items if item["id"] == "subject/FixtureNativeProofSubject")
            self.assertEqual("subject", subject_item["type"])
            self.assertEqual(
                "run test subject --id subject/FixtureNativeProofSubject",
                subject_item["canonicalCommand"],
            )
            self.assertEqual(["windows"], sorted(subject_item["supportedHosts"]))

            benchmark_item = next(item for item in index.flat_items if item["id"] == "subject/FixtureInterpreterPerfSubject")
            self.assertEqual("canonical", benchmark_item["category"])
            self.assertEqual("perf.release", benchmark_item["defaultGoalId"])
            self.assertEqual("windows-interpreter-perf", benchmark_item["defaultMatrixId"])
            self.assertEqual("generated-native", benchmark_item["executablePlan"])
            self.assertEqual({"windows-x64": "ready"}, benchmark_item["availability"])
            self.assertEqual(
                "run test subject --id subject/FixtureInterpreterPerfSubject",
                benchmark_item["canonicalCommand"],
            )
            self.assertEqual(["perf.release"], sorted(benchmark_item["goalIds"]))

            analysis_module_item = next(item for item in index.flat_items if item["id"] == "module/analysis/basic")
            runtime_baseline_item = next(item for item in index.flat_items if item["id"] == "system/windows-reference-gate")
            completion_pipeline_item = next(item for item in index.flat_items if item["id"] == "pipeline/completion-runtime-core")
            self.assertIn(
                "tests/fixtures/registry/modules/analysis/basic/verification.manifest.json",
                analysis_module_item["manifestPath"].replace("\\", "/"),
            )
            self.assertIn(
                "tests/fixtures/registry/systems/windows-reference-gate/scenario.manifest.json",
                runtime_baseline_item["manifestPath"].replace("\\", "/"),
            )
            self.assertIn(
                "tests/fixtures/registry/pipelines/completion-runtime-core/pipeline.manifest.json",
                completion_pipeline_item["manifestPath"].replace("\\", "/"),
            )
        finally:
            shutil.rmtree(repo_root, ignore_errors=True)

    def test_pipeline_execution_plan_deduplicates_suite_runs(self) -> None:
        registry_module = load_module(REGISTRY_MODULE_PATH, "chaos_run_registry_for_pipeline_plan")
        specs_module = load_module(PUBLIC_SPECS_MODULE_PATH, "chaos_run_public_specs_for_pipeline_plan")

        index = registry_module.scan_registry(
            REPO_ROOT,
            host_platform="macos",
            public_suite_specs=specs_module.PUBLIC_TEST_SPECS,
        )

        plan = registry_module.expand_execution_plan(index, "pipeline/completion-runtime-core")

        self.assertEqual(
            [
                "smoke/managed-entry-basic",
                "smoke/managed-generics-basic",
                "smoke/reflection-basic",
                "smoke/native-interop-basic",
                "smoke/host-embedding-basic",
                "contract/analysis-schema",
                "contract/native-abi",
                "contract/native-bridge",
            ],
            [item["id"] for item in plan],
        )

    def test_trace_export_pipeline_execution_plan_is_host_specific(self) -> None:
        registry_module = load_module(REGISTRY_MODULE_PATH, "chaos_run_registry_for_trace_export_plan")
        specs_module = load_module(PUBLIC_SPECS_MODULE_PATH, "chaos_run_public_specs_for_trace_export_plan")

        index = registry_module.scan_registry(
            REPO_ROOT,
            host_platform="macos",
            public_suite_specs=specs_module.PUBLIC_TEST_SPECS,
        )

        plan = registry_module.expand_execution_plan(index, "pipeline/trace-export-macos-runtime")

        self.assertEqual(
            [
                "smoke/host-embedding-basic",
                "contract/trace-schema",
                "contract/trace-compare-macos",
            ],
            [item["id"] for item in plan],
        )

    def test_completion_trace_pipeline_extends_completion_core_on_supported_hosts(self) -> None:
        registry_module = load_module(REGISTRY_MODULE_PATH, "chaos_run_registry_for_completion_trace_plan")
        specs_module = load_module(PUBLIC_SPECS_MODULE_PATH, "chaos_run_public_specs_for_completion_trace_plan")

        index = registry_module.scan_registry(
            REPO_ROOT,
            host_platform="macos",
            public_suite_specs=specs_module.PUBLIC_TEST_SPECS,
        )

        plan = registry_module.expand_execution_plan(index, "pipeline/completion-runtime-trace-macos")

        self.assertEqual(
            [
                "smoke/managed-entry-basic",
                "smoke/managed-generics-basic",
                "smoke/reflection-basic",
                "smoke/native-interop-basic",
                "smoke/host-embedding-basic",
                "contract/analysis-schema",
                "contract/native-abi",
                "contract/native-bridge",
                "contract/trace-schema",
                "contract/trace-compare-macos",
                "gate/macos-reference-desktop",
            ],
            [item["id"] for item in plan],
        )

    def test_refresh_writes_current_and_history_snapshots(self) -> None:
        registry_module = load_module(REGISTRY_MODULE_PATH, "chaos_run_registry_for_snapshot")
        specs_module = load_module(PUBLIC_SPECS_MODULE_PATH, "chaos_run_public_specs_for_snapshot")

        index = registry_module.scan_registry(
            REPO_ROOT,
            host_platform="macos",
            public_suite_specs=specs_module.PUBLIC_TEST_SPECS,
        )
        snapshot = registry_module.write_registry_snapshot(
            REPO_ROOT,
            index,
            stamp="20260404-210000",
        )

        current_path = REPO_ROOT / "artifacts" / "tests" / "registry" / "current" / "index.json"
        history_path = REPO_ROOT / "artifacts" / "tests" / "registry" / "history" / "20260404-210000" / "index.json"

        self.assertEqual(current_path, snapshot["currentPath"])
        self.assertEqual(history_path, snapshot["historyPath"])
        self.assertTrue(current_path.is_file())
        self.assertTrue(history_path.is_file())

        payload = json.loads(current_path.read_text(encoding="utf-8"))
        self.assertIn("flatItems", payload)
        self.assertTrue(any(item["id"] == "module/analysis/basic" for item in payload["flatItems"]))
        self.assertTrue(any(item["id"] == "module/managed-smoke/basic" for item in payload["flatItems"]))
        self.assertTrue(any(item["id"] == "module/reflection/basic" for item in payload["flatItems"]))
        self.assertTrue(any(item["id"] == "module/interop/basic" for item in payload["flatItems"]))
        self.assertTrue(any(item["id"] == "module/hosted-runtime/basic" for item in payload["flatItems"]))

    def test_registry_scan_collects_subjects_declared_by_shared_orchestration_profiles(self) -> None:
        registry_module = load_module(REGISTRY_MODULE_PATH, "chaos_run_registry_shared_profiles")
        repo_root = make_temp_repo_root("registry-scan", "shared-profiles")
        manifest_path = repo_root / "subjects" / "FixtureSharedRegistry" / "subject.manifest.json"
        pipeline_profile_path = repo_root / "testing" / "orchestration" / "pipelines" / "proof-core.json"
        matrix_profile_path = repo_root / "testing" / "orchestration" / "matrices" / "proof-core.json"
        manifest_path.parent.mkdir(parents=True, exist_ok=True)
        pipeline_profile_path.parent.mkdir(parents=True, exist_ok=True)
        matrix_profile_path.parent.mkdir(parents=True, exist_ok=True)

        pipeline_profile = {
            "executionPipelines": [
                {
                    "pipelineId": "proof-runtime-output",
                    "displayName": "Profile Runtime Output",
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
            ]
        }
        matrix_profile = {
            "environmentMatrices": [
                {
                    "matrixId": "windows-dev-output",
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
            ]
        }
        manifest = {
            "subjectId": "FixtureSharedRegistry",
            "displayName": "FixtureSharedRegistry",
            "category": "canonical",
            "defaultGoal": "correctness.dev",
            "defaultMatrix": "windows-dev-output",
            "defaultValidationProfile": "proof-dev",
            "source": {
                "type": "dotnet-project",
                "path": "subjects/FixtureSharedRegistry/source/FixtureSharedRegistry.csproj",
                "entry": "FixtureSharedRegistry/Program::Main()",
            },
            "orchestration": {
                "matrixProfile": "proof-core",
                "pipelineProfile": "proof-core",
                "budgetProfile": "budget-standard",
                "baselineProfile": "baseline-standard",
            },
            "validationProfiles": {
                "proof-dev": ["proof"],
            },
            "validation": {
                "proof": {
                    "kind": "proof",
                    "defaultVariant": "CHECK",
                }
            },
        }

        try:
            write_json(pipeline_profile_path, pipeline_profile)
            write_json(matrix_profile_path, matrix_profile)
            write_json(manifest_path, manifest)

            index = registry_module.scan_registry(
                repo_root,
                host_platform="windows",
                public_suite_specs=[],
            )

            subject_item = next(item for item in index.subjects if item["id"] == "subject/FixtureSharedRegistry")
            self.assertEqual(["windows"], subject_item["supportedHosts"])
            self.assertEqual("windows-dev-output", subject_item["defaultMatrixId"])
            self.assertEqual("canonical", subject_item["category"])
            self.assertEqual(
                {
                    "matrixProfile": "proof-core",
                    "pipelineProfile": "proof-core",
                    "budgetProfile": "budget-standard",
                    "baselineProfile": "baseline-standard",
                },
                subject_item["orchestration"],
            )
        finally:
            shutil.rmtree(repo_root, ignore_errors=True)

    def test_registry_scan_surfaces_retained_subject_default_entry_metadata(self) -> None:
        registry_module = load_module(REGISTRY_MODULE_PATH, "chaos_run_registry_retained_subject_defaults")
        specs_module = load_module(PUBLIC_SPECS_MODULE_PATH, "chaos_run_public_specs_retained_subject_defaults")

        index = registry_module.scan_registry(
            REPO_ROOT,
            host_platform="windows",
            public_suite_specs=specs_module.PUBLIC_TEST_SPECS,
        )

        hot_update_item = next(item for item in index.subjects if item["id"] == "subject/HotUpdateHostPack")
        mixed_execution_item = next(item for item in index.subjects if item["id"] == "subject/MixedExecutionFeaturePack")
        solution_core_item = next(item for item in index.subjects if item["id"] == "subject/SolutionCorePack")

        self.assertEqual(
            "subjects/HotUpdateHostPack/source/HotUpdateHostPack.csproj",
            hot_update_item["defaultPrimaryProjectPath"],
        )
        self.assertEqual(
            "HotUpdateHostPack/Program::Main()",
            hot_update_item["displaySourceEntry"],
        )
        self.assertNotIn("displaySubjectEntrySelection", hot_update_item)
        self.assertNotIn("displayWorkloadEntry", hot_update_item)
        self.assertNotIn("defaultSourceEntry", hot_update_item)
        self.assertNotIn("defaultSubjectEntrySelection", hot_update_item)
        self.assertNotIn("defaultWorkloadEntry", hot_update_item)

        self.assertEqual(
            "subjects/MixedExecutionFeaturePack/source/MixedExecutionFeaturePack.csproj",
            mixed_execution_item["defaultPrimaryProjectPath"],
        )
        self.assertEqual(
            "MixedExecutionFeaturePack/MixedExecutionProofEntry::Run()",
            mixed_execution_item["displaySourceEntry"],
        )
        self.assertNotIn("displaySubjectEntrySelection", mixed_execution_item)
        self.assertNotIn("displayWorkloadEntry", mixed_execution_item)
        self.assertNotIn("defaultSourceEntry", mixed_execution_item)
        self.assertNotIn("defaultSubjectEntrySelection", mixed_execution_item)
        self.assertNotIn("defaultWorkloadEntry", mixed_execution_item)

        self.assertEqual(
            "subjects/SolutionCorePack/source/Host/SolutionCorePack.csproj",
            solution_core_item["defaultPrimaryProjectPath"],
        )
        self.assertEqual(
            "CoreRuntimeFeatures/InterfaceDispatchProofEntry::Run()",
            solution_core_item["displaySourceEntry"],
        )
        self.assertNotIn("displaySubjectEntrySelection", solution_core_item)
        self.assertNotIn("displayWorkloadEntry", solution_core_item)
        self.assertNotIn("defaultSourceEntry", solution_core_item)
        self.assertNotIn("defaultSubjectEntrySelection", solution_core_item)
        self.assertNotIn("defaultWorkloadEntry", solution_core_item)

    def test_registry_scan_projects_engineering_and_declared_catalog_object_families(self) -> None:
        registry_module = load_module(REGISTRY_MODULE_PATH, "chaos_run_registry_compiled_object_families")
        specs_module = load_module(PUBLIC_SPECS_MODULE_PATH, "chaos_run_public_specs_compiled_object_families")

        run_checked(
            [
                "dotnet",
                "build",
                str(HOT_UPDATE_HOST_PACK_PROJECT_PATH),
                "-c",
                "Debug",
                "-m:1",
            ],
            cwd=REPO_ROOT,
        )

        index = registry_module.scan_registry(
            REPO_ROOT,
            host_platform="windows",
            public_suite_specs=specs_module.PUBLIC_TEST_SPECS,
        )

        object_ids = {item["id"] for item in index.flat_items}
        self.assertIn("engineering-validation/SolutionCorePack/project-graph", object_ids)
        self.assertIn("engineering-workload/SolutionCorePack/codegen", object_ids)
        self.assertIn(
            "declared-unit-test/SolutionCorePack::CoreRuntimeFeatures::CoreRuntimeFeatures.ArrayOpsProofEntry::Run()",
            object_ids,
        )
        self.assertIn(
            "declared-benchmark/SolutionCorePack::CoreRuntimeBenchmarks::CoreRuntimeBenchmarks.GenericBenchmarkEntry::RunWorkload()",
            object_ids,
        )

        declared_unit_item = next(
            item
            for item in index.flat_items
            if item["id"] == "declared-unit-test/SolutionCorePack::CoreRuntimeFeatures::CoreRuntimeFeatures.ArrayOpsProofEntry::Run()"
        )
        self.assertEqual("declared-unit-test", declared_unit_item["type"])
        self.assertEqual("SolutionCorePack", declared_unit_item["subjectId"])
        self.assertEqual(
            "run test declared-unit-test --id declared-unit-test/SolutionCorePack::CoreRuntimeFeatures::CoreRuntimeFeatures.ArrayOpsProofEntry::Run()",
            declared_unit_item["canonicalCommand"],
        )
        self.assertEqual(1, declared_unit_item["category"])
        self.assertEqual("Runtime Contract", declared_unit_item["categoryLabel"])
        self.assertIsInstance(declared_unit_item["entryIndex"], int)
        self.assertGreaterEqual(declared_unit_item["entryIndex"], 0)
        self.assertEqual(0, declared_unit_item["archetype"])
        self.assertEqual("Unspecified", declared_unit_item["archetypeLabel"])
        self.assertEqual([], declared_unit_item["hotUpdateCapabilityLabels"])

        engineering_workload_item = next(
            item
            for item in index.flat_items
            if item["id"] == "engineering-workload/SolutionCorePack/codegen"
        )
        self.assertEqual("engineering-workload", engineering_workload_item["type"])
        self.assertEqual("SolutionCorePack", engineering_workload_item["subjectId"])
        self.assertEqual(
            "run test engineering-workload --id engineering-workload/SolutionCorePack/codegen",
            engineering_workload_item["canonicalCommand"],
        )

        hot_update_benchmark_item = next(
            item
            for item in index.flat_items
            if item["id"] == "declared-benchmark/HotUpdateHostPack::HotUpdateHostPack::HotUpdateHostPack.HotUpdateLoadBenchmarkEntry::RunWorkload()"
        )
        self.assertEqual(4, hot_update_benchmark_item["category"])
        self.assertEqual("Hot Update", hot_update_benchmark_item["categoryLabel"])
        self.assertEqual(7, hot_update_benchmark_item["archetype"])
        self.assertEqual("Skeleton Patch Solution", hot_update_benchmark_item["archetypeLabel"])
        self.assertEqual(17, hot_update_benchmark_item["hotUpdateCapability"])
        self.assertEqual(
            ["Package Load", "Patch Integrity"],
            hot_update_benchmark_item["hotUpdateCapabilityLabels"],
        )
        self.assertIsInstance(hot_update_benchmark_item["entryIndex"], int)
        self.assertGreaterEqual(hot_update_benchmark_item["entryIndex"], 0)
        self.assertEqual(["managed"], hot_update_benchmark_item["supportedModes"])
        self.assertEqual(["Wall Clock"], hot_update_benchmark_item["metricLabels"])
        self.assertEqual(["Hot Update"], hot_update_benchmark_item["requirementLabels"])

        shared_contract_unit_item = next(
            item
            for item in index.flat_items
            if item["id"] == "declared-unit-test/HotUpdateHostPack::HotUpdateHostPack::HotUpdateHostPack.SharedContractProofEntry::Run()"
        )
        self.assertEqual(5, shared_contract_unit_item["category"])
        self.assertEqual("Hot Update Contract", shared_contract_unit_item["categoryLabel"])
        self.assertEqual(8, shared_contract_unit_item["archetype"])
        self.assertEqual("Full Project Hot-Update Solution", shared_contract_unit_item["archetypeLabel"])
        self.assertEqual(66, shared_contract_unit_item["hotUpdateCapability"])
        self.assertEqual(
            ["Shared Contract Binding", "Patch Callback Flow"],
            shared_contract_unit_item["hotUpdateCapabilityLabels"],
        )
        self.assertEqual(["Hot Update"], shared_contract_unit_item["requirementLabels"])


if __name__ == "__main__":
    unittest.main()

