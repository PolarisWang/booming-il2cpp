from __future__ import annotations

import importlib.util
import json
import shutil
import sys
import unittest
from pathlib import Path


REPO_ROOT = Path(__file__).resolve().parents[3]
SUBJECTS_MODULE_PATH = REPO_ROOT / "build" / "toolchains" / "run" / "testing" / "subjects.py"
RETAINED_SOLUTION_SUBJECT_IDS = {
    "SolutionCorePack",
    "HotUpdateHostPack",
    "MixedExecutionFeaturePack",
}
SOLUTION_CORE_PACK_PIPELINE_IDS = {
    "managed-runtime-output",
    "managed-runtime-trace",
    "proof-runtime-output",
    "native-benchmark",
}
SOLUTION_CORE_PACK_MATRIX_IDS = {
    "windows-archetype-simple-lib-managed-output",
    "windows-archetype-multi-project-managed-output",
    "windows-archetype-package-reference-managed-output",
    "windows-native-check",
    "windows-managed-trace",
    "macos-managed-trace",
    "windows-native-perf",
}


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


def assert_matrix_source_entry(
    test_case: unittest.TestCase,
    subjects_module,
    manifest: dict[str, object],
    matrix_id: str,
    expected_entry: str,
) -> None:
    matrix = subjects_module.find_matrix(manifest, matrix_id)
    test_case.assertEqual(expected_entry, str(dict(matrix.get("source") or {})["entry"]))


class SubjectManifestSchemaTests(unittest.TestCase):
    def test_all_discovered_subject_manifests_match_directory_identity(self) -> None:
        subjects_module = load_module(SUBJECTS_MODULE_PATH, "chaos_subject_manifest_schema_all")

        manifest_paths = subjects_module.discover_subject_manifests(REPO_ROOT)
        records = subjects_module.load_subject_records(REPO_ROOT)

        self.assertEqual(len(manifest_paths), len(records))
        self.assertGreater(len(records), 0)

        for record in records:
            manifest = record["manifest"]
            manifest_path = record["manifestPath"]
            subject_id = record["subjectId"]
            validation_profiles = dict(manifest.get("validationProfiles") or {})
            validation = dict(manifest.get("validation") or {})
            expected = dict(manifest.get("expected") or {})
            baselines = dict(manifest.get("baselines") or {})
            compatibility = dict(manifest.get("compatibility") or {})
            redirect_subject_id = str(compatibility.get("redirectToSubject") or "")
            source_owner = redirect_subject_id or subject_id

            self.assertIn(manifest_path, manifest_paths)
            self.assertEqual(subject_id, manifest["subjectId"])
            self.assertEqual(subject_id, manifest_path.parent.name)
            self.assertEqual("dotnet-project", manifest["source"]["type"])
            self.assertTrue(str(manifest["source"]["path"]).startswith(f"subjects/{source_owner}/source/"))
            self.assertTrue(manifest["displayName"])
            self.assertGreater(len(list(manifest.get("environmentMatrices") or [])), 0)
            self.assertTrue(str(manifest.get("defaultValidationProfile") or ""))
            self.assertIn(str(manifest["defaultValidationProfile"]), validation_profiles)
            self.assertGreater(len(validation_profiles), 0)
            self.assertGreater(len(validation), 0)

            for profile_id, validation_kinds in validation_profiles.items():
                self.assertTrue(profile_id)
                self.assertGreater(len(list(validation_kinds or [])), 0)
                for validation_kind in list(validation_kinds or []):
                    self.assertIn(str(validation_kind), validation)

            for validation_kind, validation_spec in validation.items():
                self.assertEqual(str(validation_spec.get("kind") or validation_kind), validation_kind)
                self.assertTrue(str(validation_spec.get("defaultVariant") or ""))
                project_path = str(validation_spec.get("project") or "")
                if project_path:
                    self.assertTrue(
                        project_path.startswith(f"subjects/{subject_id}/validation/")
                        or project_path.startswith("src/validation/perf/")
                    )
                    self.assertTrue((REPO_ROOT / project_path).is_file())

            for label, expected_path in expected.items():
                self.assertTrue(label)
                self.assertTrue(str(expected_path).startswith(f"subjects/{subject_id}/expected/"))

            for label, baseline_path in baselines.items():
                self.assertTrue(label)
                self.assertTrue(str(baseline_path).startswith(f"subjects/{subject_id}/baselines/"))

    def test_query_finds_trace_capable_retained_solution_subject_without_subject_name_coupling(self) -> None:
        subjects_module = load_module(SUBJECTS_MODULE_PATH, "chaos_subject_manifest_query_trace")
        run_id = "20260407-hello-001"

        record = subjects_module.require_single_subject_record(
            subjects_module.load_subject_records(REPO_ROOT),
            category="canonical",
            source_type="dotnet-project",
            required_stage_kinds=[
                "analysis-frontend",
                "generated-native-aot",
                "generated-native-proof",
                "native-runtime-perf",
                "runtime-trace-compare",
            ],
            required_validation_profile_ids=["proof-dev", "trace-platform", "perf-profile"],
            required_validation_kinds=["proof", "perf"],
            required_validation_drivers=["native-runtime-perf"],
        )
        manifest = record["manifest"]
        subject_id = record["subjectId"]
        trace_matrix = subjects_module.find_matrix_with_stage_kind(manifest, "runtime-trace-compare")
        matrix_id = str(trace_matrix["matrixId"])

        roots = subjects_module.subject_artifact_roots(subject_id, matrix_id, run_id=run_id)
        self.assertEqual(f"artifacts/subjects/{subject_id}", roots["subjectRoot"])
        self.assertEqual(f"artifacts/subjects/{subject_id}/runs", roots["runsRoot"])
        self.assertEqual(f"artifacts/subjects/{subject_id}/runs/{run_id}", roots["runRoot"])
        self.assertEqual(f"artifacts/subjects/{subject_id}/runs/{run_id}/analysis", roots["analysisRoot"])
        self.assertEqual(f"artifacts/subjects/{subject_id}/runs/{run_id}/matrices/{matrix_id}", roots["matrixRoot"])
        self.assertEqual(
            f"artifacts/subjects/{subject_id}/runs/{run_id}/matrices/{matrix_id}/pipeline-report",
            roots["pipelineReportRoot"],
        )
        self.assertEqual(
            f"artifacts/subjects/{subject_id}/runs/{run_id}/subject-report",
            roots["subjectReportRoot"],
        )

        analysis_paths = subjects_module.stage_paths(
            subject_id,
            matrix_id,
            run_id=run_id,
            bucket="analysis",
            scope="shared",
            kind="analysis-frontend",
        )
        self.assertEqual(
            f"artifacts/subjects/{subject_id}/runs/{run_id}/analysis/analysis/analysis.manifest.json",
            analysis_paths["manifestPath"],
        )
        self.assertEqual(
            [f"artifacts/subjects/{subject_id}/runs/{run_id}/analysis/analysis/contract-validate.report.json"],
            analysis_paths["reportPaths"],
        )

        runtime_paths = subjects_module.stage_paths(
            subject_id,
            matrix_id,
            run_id=run_id,
            bucket="runtime",
            scope="matrix",
            kind="runtime-trace-compare",
        )
        self.assertEqual(
            f"artifacts/subjects/{subject_id}/runs/{run_id}/matrices/{matrix_id}/runtime/runtime.manifest.json",
            runtime_paths["manifestPath"],
        )
        self.assertEqual(
            [f"artifacts/subjects/{subject_id}/runs/{run_id}/matrices/{matrix_id}/runtime/trace-compare.report.json"],
            runtime_paths["reportPaths"],
        )

    def test_query_finds_solution_core_pack_native_aot_surface_without_subject_name_coupling(self) -> None:
        subjects_module = load_module(SUBJECTS_MODULE_PATH, "chaos_subject_manifest_query_solution_core_pack")

        record = subjects_module.require_single_subject_record(
            subjects_module.load_subject_records(REPO_ROOT),
            category="canonical",
            source_type="dotnet-project",
            required_stage_kinds=[
                "analysis-frontend",
                "generated-native-proof",
                "generated-native-aot",
                "native-runtime-perf",
                "runtime-trace-compare",
            ],
            required_goal_ids=["correctness.dev", "correctness.platform", "perf.release"],
            required_host_platforms=["windows-x64", "macos-arm64"],
            required_validation_profile_ids=["proof-dev", "trace-platform", "managed-output", "perf-profile"],
            required_validation_kinds=["proof", "perf"],
            required_validation_drivers=["native-runtime-perf"],
        )
        manifest = record["manifest"]
        capabilities = record["capabilities"]
        pipeline_ids = {
            str(pipeline["pipelineId"])
            for pipeline in list(manifest.get("executionPipelines") or [])
        }
        matrix_ids = {
            str(matrix["matrixId"])
            for matrix in list(manifest.get("environmentMatrices") or [])
        }

        self.assertEqual("SolutionCorePack", record["subjectId"])
        self.assertEqual("canonical", capabilities["category"])
        self.assertEqual("dotnet-solution", capabilities["sourceModel"])
        self.assertEqual("mixed", capabilities["dependencyModel"])
        self.assertEqual("generated-native", capabilities["executablePlan"])
        self.assertEqual("native-executable", capabilities["engineeringProfile"])
        self.assertEqual("require", capabilities["testDeclarationMode"])
        self.assertEqual("subjects/SolutionCorePack/source/SolutionCorePack.sln", manifest["source"]["path"])
        self.assertEqual(
            "subjects/SolutionCorePack/source/Launcher/SolutionCorePack.csproj",
            manifest["source"]["primaryProjectPath"],
        )
        self.assertEqual("MainlineFeaturePack/ProofEntry::Run()", manifest["source"]["entry"])
        self.assertEqual("PerformanceFeaturePack/ArithmeticBenchmarkEntry::RunWorkload()", manifest["workloadEntry"])
        self.assertEqual(SOLUTION_CORE_PACK_PIPELINE_IDS, pipeline_ids)
        self.assertEqual(SOLUTION_CORE_PACK_MATRIX_IDS, matrix_ids)
        self.assertEqual({"macos-arm64", "windows-x64"}, set(capabilities["hostPlatforms"]))
        assert_matrix_source_entry(
            self,
            subjects_module,
            manifest,
            "windows-archetype-simple-lib-managed-output",
            "GoldenSimpleLib.App/Program::Main()",
        )
        assert_matrix_source_entry(
            self,
            subjects_module,
            manifest,
            "windows-managed-trace",
            "HostEmbeddingLite/Program::Main(System.String[])",
        )
        assert_matrix_source_entry(
            self,
            subjects_module,
            manifest,
            "windows-native-perf",
            "PerformanceFeaturePack/ArithmeticBenchmarkEntry::RunWorkload()",
        )

    def test_query_finds_managed_benchmark_solution_subjects_without_subject_name_list(self) -> None:
        subjects_module = load_module(SUBJECTS_MODULE_PATH, "chaos_subject_manifest_query_managed_benchmark")

        records = subjects_module.query_subject_records(
            subjects_module.load_subject_records(REPO_ROOT),
            category="canonical",
            source_type="dotnet-project",
            required_stage_kinds=["runtime-perf-collect"],
            required_goal_ids=["perf.release"],
            required_host_platforms=["windows-x64"],
            required_validation_profile_ids=["perf-profile"],
            required_validation_kinds=["perf"],
            required_validation_drivers=["native-runtime-perf"],
        )

        self.assertEqual(
            {"HotUpdateHostPack", "MixedExecutionFeaturePack"},
            {str(record["subjectId"]) for record in records},
        )

        for record in records:
            manifest = record["manifest"]
            capabilities = record["capabilities"]

            self.assertEqual("dotnet-solution", capabilities["sourceModel"])
            self.assertEqual("require", capabilities["testDeclarationMode"])
            self.assertIn("perf-profile", dict(manifest.get("validationProfiles") or {}))
            self.assertTrue(str(manifest["source"]["path"]).startswith(f"subjects/{record['subjectId']}/source/"))
            self.assertTrue(str(manifest.get("workloadEntry") or "").endswith("::RunWorkload()"))

    def test_retained_solution_subjects_declare_solution_mode_and_require_explicit_test_contract(self) -> None:
        subjects_module = load_module(SUBJECTS_MODULE_PATH, "chaos_subject_manifest_retained_solution_mode")

        records = [
            record
            for record in subjects_module.load_subject_records(REPO_ROOT)
            if str(record["capabilities"]["sourceModel"]) == "dotnet-solution"
        ]

        self.assertEqual(RETAINED_SOLUTION_SUBJECT_IDS, {str(record["subjectId"]) for record in records})

        for record in records:
            manifest = record["manifest"]
            capabilities = record["capabilities"]
            primary_project_path = REPO_ROOT / str(manifest["source"]["primaryProjectPath"])

            self.assertEqual("require", capabilities["testDeclarationMode"])
            self.assertEqual("dotnet-solution", manifest["sourceModel"])
            self.assertTrue(str(manifest["source"]["path"]).endswith(".sln"))
            self.assertTrue(primary_project_path.is_file(), msg=f"missing primary project: {primary_project_path}")

    def test_retained_solution_subject_manifests_project_expected_thin_fields(self) -> None:
        subjects_module = load_module(SUBJECTS_MODULE_PATH, "chaos_subject_manifest_retained_thin_fields")

        expected_contracts = {
            "SolutionCorePack": {
                "sourceModel": "dotnet-solution",
                "dependencyModel": "mixed",
                "executablePlan": "generated-native",
                "engineeringProfile": "native-executable",
                "availability": {"macos-arm64": "ready", "windows-x64": "ready"},
            },
            "HotUpdateHostPack": {
                "sourceModel": "dotnet-solution",
                "dependencyModel": "project-reference",
                "executablePlan": "managed-host",
                "engineeringProfile": "managed-output",
                "availability": {
                    "linux-x64": "ready",
                    "macos-arm64": "ready",
                    "windows-x64": "ready",
                },
            },
            "MixedExecutionFeaturePack": {
                "sourceModel": "dotnet-solution",
                "dependencyModel": "project-reference",
                "executablePlan": "managed-host",
                "engineeringProfile": "managed-output",
                "availability": {"windows-x64": "ready"},
            },
        }

        for subject_id, expected in expected_contracts.items():
            manifest = subjects_module.load_subject_manifest(REPO_ROOT, subject_id)
            capabilities = subjects_module.manifest_capabilities(manifest)

            self.assertEqual(expected["sourceModel"], manifest["sourceModel"])
            self.assertEqual(expected["dependencyModel"], manifest["dependencyModel"])
            self.assertEqual(expected["executablePlan"], manifest["executablePlan"])
            self.assertEqual(expected["engineeringProfile"], manifest["engineeringProfile"])
            self.assertEqual(expected["availability"], manifest["availability"])
            self.assertEqual(expected["sourceModel"], capabilities["sourceModel"])
            self.assertEqual(expected["dependencyModel"], capabilities["dependencyModel"])
            self.assertEqual(expected["executablePlan"], capabilities["executablePlan"])
            self.assertEqual(expected["engineeringProfile"], capabilities["engineeringProfile"])

    def test_retained_subject_sources_declare_chaos_attributes_in_csharp_api(self) -> None:
        expected_sources = {
            "subjects/SolutionCorePack/source/Slices/MainlineFeaturePack/Program.cs": [
                "[ChaosUnitTest(",
                'Alias = "mainline-proof"',
            ],
            "subjects/SolutionCorePack/source/Slices/PerformanceFeaturePack/ArithmeticBenchmark.cs": [
                "[ChaosBenchmark(",
                'Alias = "arithmetic-bench"',
            ],
            "subjects/HotUpdateHostPack/source/HotUpdateSkeletonProofEntry.cs": [
                "[ChaosUnitTest(",
                'Alias = "hot-update-skeleton-proof"',
            ],
            "subjects/HotUpdateHostPack/source/HotUpdateLoadBenchmark.cs": [
                "[ChaosBenchmark(",
                'Alias = "hot-update-load-bench"',
            ],
            "subjects/MixedExecutionFeaturePack/source/MixedExecutionProofEntry.cs": [
                "[ChaosUnitTest(",
                'Alias = "mixed-execution-proof"',
            ],
            "subjects/MixedExecutionFeaturePack/source/MixedExecutionBenchmark.cs": [
                "[ChaosBenchmark(",
                'Alias = "mixed-execution-bench"',
            ],
            "subjects/MixedExecutionFeaturePack/source/MixedExecutionNativeBenchmark.cs": [
                "[ChaosBenchmark(",
                'Alias = "mixed-execution-native-bench"',
            ],
        }

        for relative_path, required_fragments in expected_sources.items():
            source = (REPO_ROOT / relative_path).read_text(encoding="utf-8")
            for fragment in required_fragments:
                self.assertIn(fragment, source, msg=relative_path)

    def test_interpreter_arithmetic_subject_projects_support_assembly_from_mixed_execution_feature_pack(self) -> None:
        subjects_module = load_module(SUBJECTS_MODULE_PATH, "chaos_subject_manifest_interpreter_arithmetic")
        manifest = subjects_module.load_subject_manifest(REPO_ROOT, "MixedExecutionFeaturePack")
        project_path = (
            REPO_ROOT
            / "subjects"
            / "MixedExecutionFeaturePack"
            / "source"
            / "InterpreterArithmeticProof"
            / "InterpreterArithmeticProof.csproj"
        )
        program_path = (
            REPO_ROOT
            / "subjects"
            / "MixedExecutionFeaturePack"
            / "source"
            / "InterpreterArithmeticProof"
            / "Program.cs"
        )
        program_source = program_path.read_text(encoding="utf-8")

        self.assertTrue(project_path.is_file(), msg=f"missing nested interpreter arithmetic project: {project_path}")
        self.assertTrue(program_path.is_file(), msg=f"missing nested interpreter arithmetic program: {program_path}")
        self.assertEqual(
            "subjects/MixedExecutionFeaturePack/source/MixedExecutionFeaturePack.sln",
            manifest["source"]["path"],
        )
        self.assertEqual("subjects/MixedExecutionFeaturePack/source/MixedExecutionFeaturePack.csproj", manifest["source"]["primaryProjectPath"])
        self.assertEqual("dotnet-solution", manifest["sourceModel"])
        self.assertEqual("require", subjects_module.manifest_capabilities(manifest)["testDeclarationMode"])
        self.assertIn("namespace InterpreterArithmeticProof;", program_source)
        self.assertIn("public static int Main()", program_source)

    def test_explicit_thin_manifest_fields_override_legacy_projection(self) -> None:
        subjects_module = load_module(SUBJECTS_MODULE_PATH, "chaos_subject_manifest_thin_explicit_override")
        repo_root = REPO_ROOT / "artifacts" / ".tmp-tests" / "subject-manifest-schema" / "thin-explicit-override"
        manifest_path = repo_root / "subjects" / "FixtureThinOverride" / "subject.manifest.json"
        manifest_path.parent.mkdir(parents=True, exist_ok=True)

        manifest = {
            "subjectId": "FixtureThinOverride",
            "displayName": "FixtureThinOverride",
            "category": "canonical",
            "defaultGoal": "correctness.dev",
            "defaultMatrix": "windows-managed-output",
            "defaultValidationProfile": "proof-dev",
            "source": {
                "type": "dotnet-project",
                "path": "subjects/FixtureThinOverride/source/FixtureThinOverride.csproj",
                "entry": "FixtureThinOverride/Program::Main()",
            },
            "sourceModel": "dotnet-solution",
            "dependencyModel": "mixed",
            "executablePlan": "host-plus-hot-update-patch",
            "engineeringProfile": "hot-update-host",
            "availability": {
                "windows-x64": "planned",
            },
            "compatibility": {
                "legacySubjectIds": ["FixtureThinOverrideLegacy"],
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
            "executionPipelines": [
                {
                    "pipelineId": "managed-runtime-output",
                    "stages": [
                        {
                            "stageId": "source-resolve",
                            "kind": "source-resolve",
                            "scope": "shared",
                            "bucket": "source",
                            "dependsOn": [],
                        },
                        {
                            "stageId": "host-input-build",
                            "kind": "host-input-build",
                            "scope": "shared",
                            "bucket": "host-input",
                            "dependsOn": ["source-resolve"],
                        },
                        {
                            "stageId": "runtime-managed-output",
                            "kind": "runtime-managed-output",
                            "scope": "matrix",
                            "bucket": "runtime",
                            "dependsOn": ["host-input-build"],
                        },
                        {
                            "stageId": "report-assemble",
                            "kind": "report-assemble",
                            "scope": "matrix",
                            "bucket": "report",
                            "dependsOn": ["runtime-managed-output"],
                        },
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
            manifest_path.write_text(json.dumps(manifest, ensure_ascii=False, indent=2) + "\n", encoding="utf-8")

            loaded = subjects_module.load_subject_manifest(repo_root, "FixtureThinOverride")
            capabilities = subjects_module.manifest_capabilities(loaded)

            self.assertEqual("dotnet-solution", loaded["sourceModel"])
            self.assertEqual("mixed", loaded["dependencyModel"])
            self.assertEqual("host-plus-hot-update-patch", loaded["executablePlan"])
            self.assertEqual("hot-update-host", loaded["engineeringProfile"])
            self.assertEqual({"windows-x64": "planned"}, loaded["availability"])
            self.assertEqual(
                {"legacySubjectIds": ["FixtureThinOverrideLegacy"]},
                loaded["compatibility"],
            )
            self.assertEqual("dotnet-solution", capabilities["sourceModel"])
            self.assertEqual("mixed", capabilities["dependencyModel"])
            self.assertEqual("host-plus-hot-update-patch", capabilities["executablePlan"])
            self.assertEqual("hot-update-host", capabilities["engineeringProfile"])
        finally:
            shutil.rmtree(repo_root, ignore_errors=True)

    def test_shared_orchestration_profiles_expand_and_inline_subject_fields_override(self) -> None:
        subjects_module = load_module(SUBJECTS_MODULE_PATH, "chaos_subject_manifest_shared_profiles")
        repo_root = REPO_ROOT / "artifacts" / ".tmp-tests" / "subject-manifest-schema" / "shared-profiles"
        manifest_path = repo_root / "subjects" / "FixtureSharedProfiles" / "subject.manifest.json"
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
                },
                {
                    "pipelineId": "proof-runtime-trace",
                    "displayName": "Profile Runtime Trace",
                    "stages": [
                        {"stageId": "source-resolve", "kind": "source-resolve", "scope": "shared", "bucket": "source", "dependsOn": []},
                        {"stageId": "host-input-build", "kind": "host-input-build", "scope": "shared", "bucket": "host-input", "dependsOn": ["source-resolve"]},
                        {"stageId": "analysis-frontend", "kind": "analysis-frontend", "scope": "shared", "bucket": "analysis", "dependsOn": ["host-input-build"]},
                        {"stageId": "generated-native-proof", "kind": "generated-native-proof", "scope": "shared", "bucket": "generated", "dependsOn": ["analysis-frontend"]},
                        {"stageId": "build-target", "kind": "build-target", "scope": "matrix", "bucket": "build", "dependsOn": ["generated-native-proof"]},
                        {"stageId": "runtime-observe", "kind": "runtime-observe", "scope": "matrix", "bucket": "runtime", "dependsOn": ["build-target"]},
                        {"stageId": "runtime-trace-compare", "kind": "runtime-trace-compare", "scope": "matrix", "bucket": "runtime", "dependsOn": ["host-input-build", "runtime-observe"]},
                        {"stageId": "report-assemble", "kind": "report-assemble", "scope": "matrix", "bucket": "report", "dependsOn": ["runtime-trace-compare"]},
                    ],
                },
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
                },
                {
                    "matrixId": "windows-reference-trace",
                    "pipelineId": "proof-runtime-trace",
                    "supportedGoals": ["correctness.platform"],
                    "executionContext": {
                        "hostPlatform": "windows-x64",
                        "targetPlatform": "windows-x64",
                        "toolchainProfile": "msvc-reference",
                        "runtimeProfile": "reference-trace",
                    },
                    "validationIntent": {
                        "validationMode": "trace",
                        "adaptationLevel": "traceable",
                        "expectedOutcome": "pass",
                    },
                    "artifactPlan": {
                        "requiredBuckets": ["source", "host-input", "analysis", "generated", "build", "runtime", "report"],
                        "evidenceTerminalBucket": "runtime",
                    },
                },
            ]
        }
        manifest = {
            "subjectId": "FixtureSharedProfiles",
            "displayName": "FixtureSharedProfiles",
            "category": "canonical",
            "defaultGoal": "correctness.dev",
            "defaultMatrix": "windows-dev-output",
            "defaultValidationProfile": "proof-dev",
            "source": {
                "type": "dotnet-project",
                "path": "subjects/FixtureSharedProfiles/source/FixtureSharedProfiles.csproj",
                "entry": "FixtureSharedProfiles/Program::Main()",
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
            "executionPipelines": [
                {
                    "pipelineId": "proof-runtime-output",
                    "displayName": "Subject Runtime Output Override",
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
                    "matrixId": "windows-dev-output",
                    "pipelineId": "proof-runtime-output",
                    "supportedGoals": ["correctness.dev"],
                    "source": {
                        "entry": "FixtureSharedProfiles/CustomEntry::Run()",
                    },
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
            pipeline_profile_path.write_text(json.dumps(pipeline_profile, ensure_ascii=False, indent=2) + "\n", encoding="utf-8")
            matrix_profile_path.write_text(json.dumps(matrix_profile, ensure_ascii=False, indent=2) + "\n", encoding="utf-8")
            manifest_path.write_text(json.dumps(manifest, ensure_ascii=False, indent=2) + "\n", encoding="utf-8")

            loaded = subjects_module.load_subject_manifest(repo_root, "FixtureSharedProfiles")
            capabilities = subjects_module.manifest_capabilities(loaded)
            pipeline_ids = {
                str(pipeline["pipelineId"])
                for pipeline in list(loaded.get("executionPipelines") or [])
            }
            matrix_ids = {
                str(matrix["matrixId"])
                for matrix in list(loaded.get("environmentMatrices") or [])
            }
            default_matrix = subjects_module.find_matrix(loaded, "windows-dev-output")
            trace_pipeline = subjects_module.find_pipeline(loaded, "proof-runtime-trace")

            self.assertEqual(
                {
                    "matrixProfile": "proof-core",
                    "pipelineProfile": "proof-core",
                    "budgetProfile": "budget-standard",
                    "baselineProfile": "baseline-standard",
                },
                loaded["orchestration"],
            )
            self.assertEqual({"proof-runtime-output", "proof-runtime-trace"}, pipeline_ids)
            self.assertEqual({"windows-dev-output", "windows-reference-trace"}, matrix_ids)
            self.assertEqual("Subject Runtime Output Override", subjects_module.find_pipeline(loaded, "proof-runtime-output")["displayName"])
            self.assertEqual("Profile Runtime Trace", trace_pipeline["displayName"])
            self.assertEqual(
                "FixtureSharedProfiles/CustomEntry::Run()",
                str(dict(default_matrix.get("source") or {})["entry"]),
            )
            self.assertEqual({"proof-runtime-output", "proof-runtime-trace"}, set(capabilities["pipelineIds"]))
            self.assertEqual({"windows-dev-output", "windows-reference-trace"}, set(capabilities["matrixIds"]))
            self.assertEqual("generated-native", loaded["executablePlan"])
            self.assertEqual("native-executable", loaded["engineeringProfile"])
        finally:
            shutil.rmtree(repo_root, ignore_errors=True)


if __name__ == "__main__":
    unittest.main()
