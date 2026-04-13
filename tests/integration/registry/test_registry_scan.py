from __future__ import annotations

import importlib.util
import json
import shutil
import sys
import unittest
from pathlib import Path

from tests.support import select_subject_record


REPO_ROOT = Path(__file__).resolve().parents[3]
REGISTRY_MODULE_PATH = REPO_ROOT / "build" / "toolchains" / "run" / "testing" / "registry.py"
PUBLIC_SPECS_MODULE_PATH = REPO_ROOT / "build" / "toolchains" / "run" / "testing" / "public_specs.py"


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


class RegistryScanTests(unittest.TestCase):
    def test_registry_scan_collects_suites_modules_systems_and_pipelines(self) -> None:
        registry_module = load_module(REGISTRY_MODULE_PATH, "chaos_run_registry")
        specs_module = load_module(PUBLIC_SPECS_MODULE_PATH, "chaos_run_public_specs")
        windows_only_subject = select_subject_record(
            "chaos_registry_scan_windows_only_subject",
            category="canonical",
            source_type="dotnet-project",
            required_stage_kinds=["interpreter-runtime-perf"],
            required_host_platforms=["windows-x64"],
        )

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
        self.assertNotIn(f"subject/{windows_only_subject['subjectId']}", object_ids)

    def test_registry_scan_collects_windows_android_gate_objects(self) -> None:
        registry_module = load_module(REGISTRY_MODULE_PATH, "chaos_run_registry_windows_android_gate")
        specs_module = load_module(PUBLIC_SPECS_MODULE_PATH, "chaos_run_public_specs_windows_android_gate")
        canonical_subject = select_subject_record(
            "chaos_registry_scan_canonical_subject",
            category="canonical",
            source_type="dotnet-project",
            required_stage_kinds=["runtime-managed-output"],
            required_host_platforms=["macos-arm64", "windows-x64"],
        )
        native_proof_subject = select_subject_record(
            "chaos_registry_scan_windows_native_proof_subject",
            category="canonical",
            source_type="dotnet-project",
            required_stage_kinds=["generated-native-proof"],
            required_host_platforms=["windows-x64"],
        )
        benchmark_subject = select_subject_record(
            "chaos_registry_scan_benchmark_subject",
            category="canonical",
            source_type="dotnet-project",
            required_goal_ids=["perf.release"],
            required_validation_kinds=["perf"],
            required_stage_kinds=["interpreter-runtime-perf"],
            required_host_platforms=["windows-x64"],
        )

        index = registry_module.scan_registry(
            REPO_ROOT,
            host_platform="windows",
            public_suite_specs=specs_module.PUBLIC_TEST_SPECS,
        )

        object_ids = {item["id"] for item in index.flat_items}
        self.assertIn("gate/android-arm64-smoke", object_ids)
        self.assertIn("gate/windows-reference-desktop", object_ids)
        self.assertIn("system/android-startup-gate", object_ids)
        self.assertIn("system/windows-reference-gate", object_ids)
        self.assertIn(f"subject/{canonical_subject['subjectId']}", object_ids)
        self.assertIn(f"subject/{native_proof_subject['subjectId']}", object_ids)
        self.assertIn(f"subject/{benchmark_subject['subjectId']}", object_ids)
        self.assertNotIn("subject/SolutionSimpleLib", object_ids)
        self.assertNotIn("subject/SolutionMultiProject", object_ids)
        self.assertNotIn("subject/SolutionPackageReference", object_ids)
        self.assertNotIn("subject/GoldenMultiProject", object_ids)
        self.assertNotIn("system/linux-packaging-gate-macos-only", object_ids)
        canonical_item = next(
            item for item in index.flat_items if item["id"] == f"subject/{canonical_subject['subjectId']}"
        )
        self.assertEqual("canonical", canonical_item["category"])
        self.assertEqual("correctness.dev", canonical_item["defaultGoalId"])
        self.assertEqual(str(canonical_subject["manifest"]["defaultMatrix"]), canonical_item["defaultMatrixId"])
        self.assertEqual(canonical_subject["manifest"]["sourceModel"], canonical_item["sourceModel"])
        self.assertEqual(canonical_subject["manifest"]["engineeringProfile"], canonical_item["engineeringProfile"])
        subject_item = next(
            item for item in index.flat_items if item["id"] == f"subject/{native_proof_subject['subjectId']}"
        )
        self.assertEqual("subject", subject_item["type"])
        self.assertEqual(
            f"run test subject --id subject/{native_proof_subject['subjectId']}",
            subject_item["canonicalCommand"],
        )
        expected_supported_hosts = sorted(
            {
                "windows" if str(host).startswith("windows")
                else "macos" if str(host).startswith("macos")
                else str(host)
                for host in native_proof_subject["capabilities"]["hostPlatforms"]
            }
        )
        self.assertEqual(expected_supported_hosts, sorted(subject_item["supportedHosts"]))
        benchmark_item = next(
            item for item in index.flat_items if item["id"] == f"subject/{benchmark_subject['subjectId']}"
        )
        self.assertEqual("canonical", benchmark_item["category"])
        self.assertEqual(str(benchmark_subject["manifest"]["defaultGoal"]), benchmark_item["defaultGoalId"])
        self.assertEqual(str(benchmark_subject["manifest"]["defaultMatrix"]), benchmark_item["defaultMatrixId"])
        self.assertEqual(benchmark_subject["manifest"]["executablePlan"], benchmark_item["executablePlan"])
        self.assertEqual(benchmark_subject["manifest"]["availability"], benchmark_item["availability"])
        self.assertEqual(
            f"run test subject --id subject/{benchmark_subject['subjectId']}",
            benchmark_item["canonicalCommand"],
        )
        self.assertEqual(sorted(str(value) for value in benchmark_subject["capabilities"]["goalIds"]), sorted(benchmark_item["goalIds"]))

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
        repo_root = REPO_ROOT / "artifacts" / ".tmp-tests" / "registry-scan" / "shared-profiles"
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
            pipeline_profile_path.write_text(json.dumps(pipeline_profile, ensure_ascii=False, indent=2) + "\n", encoding="utf-8")
            matrix_profile_path.write_text(json.dumps(matrix_profile, ensure_ascii=False, indent=2) + "\n", encoding="utf-8")
            manifest_path.write_text(json.dumps(manifest, ensure_ascii=False, indent=2) + "\n", encoding="utf-8")

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
            hot_update_item["defaultSourceEntry"],
        )
        self.assertEqual(
            {
                "entryKind": 1,
                "entrySlice": 1,
            },
            hot_update_item["defaultSubjectEntrySelection"],
        )
        self.assertEqual(
            "HotUpdateHostPack/HotUpdateLoadBenchmarkEntry::RunWorkload()",
            hot_update_item["defaultWorkloadEntry"],
        )

        self.assertEqual(
            "subjects/MixedExecutionFeaturePack/source/MixedExecutionFeaturePack.csproj",
            mixed_execution_item["defaultPrimaryProjectPath"],
        )
        self.assertEqual(
            "MixedExecutionFeaturePack/MixedExecutionProofEntry::Run()",
            mixed_execution_item["defaultSourceEntry"],
        )
        self.assertEqual(
            {
                "entryKind": 1,
                "entrySlice": 2,
            },
            mixed_execution_item["defaultSubjectEntrySelection"],
        )
        self.assertEqual(
            "MixedExecutionFeaturePack/MixedExecutionBenchmarkEntry::RunWorkload()",
            mixed_execution_item["defaultWorkloadEntry"],
        )

        self.assertEqual(
            "subjects/SolutionCorePack/source/Launcher/SolutionCorePack.csproj",
            solution_core_item["defaultPrimaryProjectPath"],
        )
        self.assertEqual(
            "MainlineFeaturePack/ProofEntry::Run()",
            solution_core_item["defaultSourceEntry"],
        )
        self.assertEqual(
            {
                "entryKind": 1,
                "entrySlice": 7,
            },
            solution_core_item["defaultSubjectEntrySelection"],
        )
        self.assertEqual(
            "PerformanceFeaturePack/ArithmeticBenchmarkEntry::RunWorkload()",
            solution_core_item["defaultWorkloadEntry"],
        )

    def test_registry_scan_projects_engineering_and_declared_catalog_object_families(self) -> None:
        registry_module = load_module(REGISTRY_MODULE_PATH, "chaos_run_registry_compiled_object_families")
        specs_module = load_module(PUBLIC_SPECS_MODULE_PATH, "chaos_run_public_specs_compiled_object_families")

        index = registry_module.scan_registry(
            REPO_ROOT,
            host_platform="windows",
            public_suite_specs=specs_module.PUBLIC_TEST_SPECS,
        )

        object_ids = {item["id"] for item in index.flat_items}
        self.assertIn("engineering-validation/SolutionCorePack/project-graph", object_ids)
        self.assertIn("engineering-workload/SolutionCorePack/codegen", object_ids)
        self.assertIn(
            "declared-unit-test/SolutionCorePack::MainlineFeaturePack::MainlineFeaturePack.ArrayOpsProofEntry::Run()",
            object_ids,
        )
        self.assertIn(
            "declared-benchmark/SolutionCorePack::PerformanceFeaturePack::PerformanceFeaturePack.GenericBenchmarkEntry::RunWorkload()",
            object_ids,
        )

        declared_unit_item = next(
            item
            for item in index.flat_items
            if item["id"] == "declared-unit-test/SolutionCorePack::MainlineFeaturePack::MainlineFeaturePack.ArrayOpsProofEntry::Run()"
        )
        self.assertEqual("declared-unit-test", declared_unit_item["type"])
        self.assertEqual("SolutionCorePack", declared_unit_item["subjectId"])
        self.assertEqual(
            "run test declared-unit-test --id declared-unit-test/SolutionCorePack::MainlineFeaturePack::MainlineFeaturePack.ArrayOpsProofEntry::Run()",
            declared_unit_item["canonicalCommand"],
        )

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


if __name__ == "__main__":
    unittest.main()
