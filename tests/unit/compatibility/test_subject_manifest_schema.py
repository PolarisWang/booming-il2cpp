from __future__ import annotations

import importlib.util
import json
import shutil
import sys
import unittest
from pathlib import Path


REPO_ROOT = Path(__file__).resolve().parents[3]
SUBJECTS_MODULE_PATH = REPO_ROOT / "build" / "toolchains" / "run" / "testing" / "subjects.py"


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

    def test_query_finds_trace_capable_canonical_subject_without_subject_name_coupling(self) -> None:
        subjects_module = load_module(SUBJECTS_MODULE_PATH, "chaos_subject_manifest_query_trace")
        run_id = "20260407-hello-001"

        record = subjects_module.require_single_subject_record(
            subjects_module.load_subject_records(REPO_ROOT),
            category="canonical",
            source_type="dotnet-project",
            required_stage_kinds=[
                "analysis-frontend",
                "generated-native-proof",
                "runtime-trace-compare",
            ],
            required_validation_profile_ids=["proof-dev"],
            required_validation_kinds=["proof", "unit"],
            required_validation_frameworks=["xunit"],
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

    def test_query_finds_perf_benchmark_subject_without_subject_name_coupling(self) -> None:
        subjects_module = load_module(SUBJECTS_MODULE_PATH, "chaos_subject_manifest_query_perf")

        record = subjects_module.require_single_subject_record(
            subjects_module.load_subject_records(REPO_ROOT),
            category="benchmark",
            source_type="dotnet-project",
            required_stage_kinds=["runtime-perf-collect"],
            required_goal_ids=["perf.dev", "perf.release"],
            required_host_platforms=["windows-x64"],
            required_validation_profile_ids=["perf-dev", "perf-release"],
            required_validation_kinds=["perf"],
            required_validation_drivers=["csharp-perf-harness"],
        )
        manifest = record["manifest"]
        capabilities = record["capabilities"]

        self.assertEqual("benchmark", capabilities["category"])
        self.assertIn("runtime-perf-collect", capabilities["stageKinds"])
        self.assertEqual({"perf.dev", "perf.release"}, set(capabilities["goalIds"]))
        self.assertEqual("perf-dev", capabilities["defaultValidationProfile"])
        self.assertEqual("dotnet-project", manifest["source"]["type"])
        self.assertTrue(str(manifest["source"]["path"]).startswith(f"subjects/{record['subjectId']}/source/"))

    def test_query_groups_managed_output_subjects_without_subject_name_list(self) -> None:
        subjects_module = load_module(SUBJECTS_MODULE_PATH, "chaos_subject_manifest_query_managed_output")

        records = subjects_module.query_subject_records(
            subjects_module.load_subject_records(REPO_ROOT),
            source_type="dotnet-project",
            required_stage_kinds=["runtime-managed-output"],
            required_goal_ids=["correctness.dev"],
            required_host_platforms=["windows-x64", "macos-arm64", "linux-x64"],
            required_validation_profile_ids=["managed-output"],
            required_validation_kinds=["proof"],
        )

        self.assertGreaterEqual(len(records), 4)
        for record in records:
            manifest = record["manifest"]
            capabilities = record["capabilities"]
            compatibility = dict(manifest.get("compatibility") or {})
            redirect_subject_id = str(compatibility.get("redirectToSubject") or "")
            source_owner = redirect_subject_id or record["subjectId"]
            self.assertIn(capabilities["category"], {"canonical", "diagnostic"})
            self.assertTrue(str(manifest["source"]["path"]).startswith(f"subjects/{source_owner}/source/"))
            self.assertEqual(
                {"windows-x64", "macos-arm64", "linux-x64"},
                set(capabilities["hostPlatforms"]),
            )

    def test_query_finds_mainline_feature_pack_planning_surface_without_subject_name_coupling(self) -> None:
        subjects_module = load_module(SUBJECTS_MODULE_PATH, "chaos_subject_manifest_query_mainline_feature_pack")

        record = subjects_module.require_single_subject_record(
            subjects_module.load_subject_records(REPO_ROOT),
            category="mainline",
            source_type="dotnet-project",
            required_stage_kinds=[
                "analysis-frontend",
                "generated-native-proof",
                "build-target",
                "native-runtime-perf",
            ],
            required_goal_ids=["correctness.dev", "correctness.platform", "perf.profile"],
            required_host_platforms=["windows-x64"],
            required_validation_kinds=["proof", "perf"],
            required_validation_profile_ids=["proof-dev", "trace-platform", "perf-profile"],
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
        native_matrix = subjects_module.find_matrix(manifest, "windows-native-profile")

        self.assertEqual("MainlineFeaturePack", record["subjectId"])
        self.assertEqual("mainline", capabilities["category"])
        self.assertEqual("require", capabilities["testDeclarationMode"])
        self.assertEqual({"proof-runtime-output", "proof-runtime-trace", "native-runtime-perf"}, pipeline_ids)
        self.assertEqual(
            {
                "windows-native-check",
                "windows-reference-trace",
                "windows-array-ops-check",
                "windows-async-await-check",
                "windows-bitwise-ops-check",
                "windows-branch-ops-check",
                "windows-cross-boundary-eh-check",
                "windows-conversion-ops-check",
                "windows-delegate-chain-check",
                "windows-generic-collection-check",
                "windows-interface-dispatch-check",
                "windows-linker-stripping-check",
                "windows-marshaling-check",
                "windows-nested-exception-check",
                "windows-object-ops-check",
                "windows-overflow-ops-check",
                "windows-threading-check",
                "windows-vtable-dispatch-check",
                "windows-dispatch-check",
                "windows-generic-layout-check",
                "windows-array-boxing-check",
                "windows-delegate-check",
                "windows-exception-check",
                "windows-reflection-interop-closure-check",
                "windows-native-profile",
            },
            matrix_ids,
        )
        self.assertEqual("native-runtime-perf", str(native_matrix["pipelineId"]))
        self.assertEqual(
            ["source", "host-input", "analysis", "generated", "build", "runtime", "report"],
            list(dict(native_matrix["artifactPlan"])["requiredBuckets"]),
        )
        self.assertEqual("report", str(dict(native_matrix["artifactPlan"])["evidenceTerminalBucket"]))

    def test_mainline_feature_pack_manifest_declares_phase5_capability_slice_profiles_and_matrices(self) -> None:
        subjects_module = load_module(SUBJECTS_MODULE_PATH, "chaos_subject_manifest_phase5_capability_slices")

        record = subjects_module.require_single_subject_record(
            subjects_module.load_subject_records(REPO_ROOT),
            category="mainline",
            source_type="dotnet-project",
            required_validation_profile_ids=[
                "proof-dispatch",
                "proof-generic-layout",
                "proof-array-boxing",
            ],
        )
        manifest = record["manifest"]
        validation_profiles = dict(manifest.get("validationProfiles") or {})
        matrix_ids = {
            str(matrix["matrixId"])
            for matrix in list(manifest.get("environmentMatrices") or [])
        }
        dispatch_matrix = subjects_module.find_matrix(manifest, "windows-dispatch-check")

        self.assertEqual(["proof"], validation_profiles["proof-dispatch"])
        self.assertEqual(["proof"], validation_profiles["proof-generic-layout"])
        self.assertEqual(["proof"], validation_profiles["proof-array-boxing"])
        self.assertEqual(
            {
                "windows-native-check",
                "windows-reference-trace",
                "windows-native-profile",
                "windows-array-ops-check",
                "windows-async-await-check",
                "windows-bitwise-ops-check",
                "windows-branch-ops-check",
                "windows-cross-boundary-eh-check",
                "windows-conversion-ops-check",
                "windows-delegate-chain-check",
                "windows-generic-collection-check",
                "windows-interface-dispatch-check",
                "windows-linker-stripping-check",
                "windows-marshaling-check",
                "windows-nested-exception-check",
                "windows-object-ops-check",
                "windows-overflow-ops-check",
                "windows-threading-check",
                "windows-vtable-dispatch-check",
                "windows-dispatch-check",
                "windows-generic-layout-check",
                "windows-array-boxing-check",
                "windows-delegate-check",
                "windows-exception-check",
                "windows-reflection-interop-closure-check",
            },
            matrix_ids,
        )
        self.assertEqual(
            "MainlineFeaturePack/DispatchProofEntry::Run()",
            str(dict(dispatch_matrix.get("source") or {})["entry"]),
        )

    def test_mainline_feature_pack_manifest_declares_phase6_capability_slice_profiles_and_matrices(self) -> None:
        subjects_module = load_module(SUBJECTS_MODULE_PATH, "chaos_subject_manifest_phase6_capability_slices")

        record = subjects_module.require_single_subject_record(
            subjects_module.load_subject_records(REPO_ROOT),
            category="mainline",
            source_type="dotnet-project",
            required_validation_profile_ids=[
                "proof-delegate",
                "proof-exception",
                "proof-reflection-interop-closure",
            ],
        )
        manifest = record["manifest"]
        validation_profiles = dict(manifest.get("validationProfiles") or {})
        matrix_ids = {
            str(matrix["matrixId"])
            for matrix in list(manifest.get("environmentMatrices") or [])
        }
        delegate_matrix = subjects_module.find_matrix(manifest, "windows-delegate-check")
        exception_matrix = subjects_module.find_matrix(manifest, "windows-exception-check")
        closure_matrix = subjects_module.find_matrix(manifest, "windows-reflection-interop-closure-check")

        self.assertEqual(["proof"], validation_profiles["proof-delegate"])
        self.assertEqual(["proof"], validation_profiles["proof-exception"])
        self.assertEqual(["proof"], validation_profiles["proof-reflection-interop-closure"])
        self.assertEqual(
            {
                "windows-native-check",
                "windows-reference-trace",
                "windows-native-profile",
                "windows-array-ops-check",
                "windows-async-await-check",
                "windows-bitwise-ops-check",
                "windows-branch-ops-check",
                "windows-cross-boundary-eh-check",
                "windows-conversion-ops-check",
                "windows-delegate-chain-check",
                "windows-generic-collection-check",
                "windows-interface-dispatch-check",
                "windows-linker-stripping-check",
                "windows-marshaling-check",
                "windows-nested-exception-check",
                "windows-object-ops-check",
                "windows-overflow-ops-check",
                "windows-threading-check",
                "windows-vtable-dispatch-check",
                "windows-dispatch-check",
                "windows-generic-layout-check",
                "windows-array-boxing-check",
                "windows-delegate-check",
                "windows-exception-check",
                "windows-reflection-interop-closure-check",
            },
            matrix_ids,
        )
        self.assertEqual(
            "MainlineFeaturePack/DelegateProofEntry::Run()",
            str(dict(delegate_matrix.get("source") or {})["entry"]),
        )
        self.assertEqual(
            "MainlineFeaturePack/ExceptionProofEntry::Run()",
            str(dict(exception_matrix.get("source") or {})["entry"]),
        )
        self.assertEqual(
            "MainlineFeaturePack/ReflectionInteropClosureEntry::Run()",
            str(dict(closure_matrix.get("source") or {})["entry"]),
        )

    def test_mainline_feature_pack_manifest_declares_batch2_legacy_slice_profiles_and_matrices(self) -> None:
        subjects_module = load_module(SUBJECTS_MODULE_PATH, "chaos_subject_manifest_phase5_batch2_legacy_slices")

        record = subjects_module.require_single_subject_record(
            subjects_module.load_subject_records(REPO_ROOT),
            category="mainline",
            source_type="dotnet-project",
            required_validation_profile_ids=[
                "proof-array-ops",
                "proof-bitwise-ops",
                "proof-branch-ops",
                "proof-conversion-ops",
                "proof-object-ops",
                "proof-overflow-ops",
            ],
        )
        manifest = record["manifest"]
        validation_profiles = dict(manifest.get("validationProfiles") or {})
        matrix_ids = {
            str(matrix["matrixId"])
            for matrix in list(manifest.get("environmentMatrices") or [])
        }
        array_matrix = subjects_module.find_matrix(manifest, "windows-array-ops-check")
        overflow_matrix = subjects_module.find_matrix(manifest, "windows-overflow-ops-check")

        self.assertEqual(["proof"], validation_profiles["proof-array-ops"])
        self.assertEqual(["proof"], validation_profiles["proof-bitwise-ops"])
        self.assertEqual(["proof"], validation_profiles["proof-branch-ops"])
        self.assertEqual(["proof"], validation_profiles["proof-conversion-ops"])
        self.assertEqual(["proof"], validation_profiles["proof-object-ops"])
        self.assertEqual(["proof"], validation_profiles["proof-overflow-ops"])
        self.assertTrue(
            {
                "windows-array-ops-check",
                "windows-bitwise-ops-check",
                "windows-branch-ops-check",
                "windows-conversion-ops-check",
                "windows-object-ops-check",
                "windows-overflow-ops-check",
            }.issubset(matrix_ids)
        )
        self.assertEqual(
            "MainlineFeaturePack/ArrayOpsProofEntry::Run()",
            str(dict(array_matrix.get("source") or {})["entry"]),
        )
        self.assertEqual(
            "MainlineFeaturePack/OverflowOpsProofEntry::Run()",
            str(dict(overflow_matrix.get("source") or {})["entry"]),
        )

    def test_mainline_feature_pack_manifest_declares_batch3_runtime_slice_profiles_and_matrices(self) -> None:
        subjects_module = load_module(SUBJECTS_MODULE_PATH, "chaos_subject_manifest_phase5_batch3_runtime_slices")

        record = subjects_module.require_single_subject_record(
            subjects_module.load_subject_records(REPO_ROOT),
            category="mainline",
            source_type="dotnet-project",
            required_validation_profile_ids=[
                "proof-async-await",
                "proof-threading",
                "proof-nested-exception",
                "proof-generic-collection",
                "proof-delegate-chain",
            ],
        )
        manifest = record["manifest"]
        validation_profiles = dict(manifest.get("validationProfiles") or {})
        matrix_ids = {
            str(matrix["matrixId"])
            for matrix in list(manifest.get("environmentMatrices") or [])
        }
        async_matrix = subjects_module.find_matrix(manifest, "windows-async-await-check")
        delegate_chain_matrix = subjects_module.find_matrix(manifest, "windows-delegate-chain-check")

        self.assertEqual(["proof"], validation_profiles["proof-async-await"])
        self.assertEqual(["proof"], validation_profiles["proof-threading"])
        self.assertEqual(["proof"], validation_profiles["proof-nested-exception"])
        self.assertEqual(["proof"], validation_profiles["proof-generic-collection"])
        self.assertEqual(["proof"], validation_profiles["proof-delegate-chain"])
        self.assertTrue(
            {
                "windows-async-await-check",
                "windows-threading-check",
                "windows-nested-exception-check",
                "windows-generic-collection-check",
                "windows-delegate-chain-check",
            }.issubset(matrix_ids)
        )
        self.assertEqual(
            "MainlineFeaturePack/AsyncAwaitProofEntry::Run()",
            str(dict(async_matrix.get("source") or {})["entry"]),
        )
        self.assertEqual(
            "MainlineFeaturePack/DelegateChainProofEntry::Run()",
            str(dict(delegate_chain_matrix.get("source") or {})["entry"]),
        )

    def test_mainline_feature_pack_manifest_declares_batch4_advanced_slice_profiles_and_matrices(self) -> None:
        subjects_module = load_module(SUBJECTS_MODULE_PATH, "chaos_subject_manifest_phase5_batch4_advanced_slices")

        record = subjects_module.require_single_subject_record(
            subjects_module.load_subject_records(REPO_ROOT),
            category="mainline",
            source_type="dotnet-project",
            required_validation_profile_ids=[
                "proof-interface-dispatch",
                "proof-vtable-dispatch",
                "proof-linker-stripping",
                "proof-marshaling",
                "proof-cross-boundary-exception",
            ],
        )
        manifest = record["manifest"]
        validation_profiles = dict(manifest.get("validationProfiles") or {})
        matrix_ids = {
            str(matrix["matrixId"])
            for matrix in list(manifest.get("environmentMatrices") or [])
        }
        interface_matrix = subjects_module.find_matrix(manifest, "windows-interface-dispatch-check")
        marshaling_matrix = subjects_module.find_matrix(manifest, "windows-marshaling-check")

        self.assertEqual(["proof"], validation_profiles["proof-interface-dispatch"])
        self.assertEqual(["proof"], validation_profiles["proof-vtable-dispatch"])
        self.assertEqual(["proof"], validation_profiles["proof-linker-stripping"])
        self.assertEqual(["proof"], validation_profiles["proof-marshaling"])
        self.assertEqual(["proof"], validation_profiles["proof-cross-boundary-exception"])
        self.assertTrue(
            {
                "windows-interface-dispatch-check",
                "windows-vtable-dispatch-check",
                "windows-linker-stripping-check",
                "windows-marshaling-check",
                "windows-cross-boundary-eh-check",
            }.issubset(matrix_ids)
        )
        self.assertEqual(
            "MainlineFeaturePack/InterfaceDispatchProofEntry::Run()",
            str(dict(interface_matrix.get("source") or {})["entry"]),
        )
        self.assertEqual(
            "MainlineFeaturePack/MarshalingProofEntry::Run()",
            str(dict(marshaling_matrix.get("source") or {})["entry"]),
        )

    def test_engine_binding_subject_manifests_declare_minimal_windows_only_surface(self) -> None:
        subjects_module = load_module(SUBJECTS_MODULE_PATH, "chaos_subject_manifest_phase7_engine_binding")

        records = subjects_module.query_subject_records(
            subjects_module.load_subject_records(REPO_ROOT),
            category="canonical",
            source_type="dotnet-project",
            required_stage_kinds=[
                "analysis-frontend",
                "generated-engine-proof",
                "runtime-engine-observe",
                "runtime-engine-trace-compare",
            ],
            required_goal_ids=["correctness.dev", "correctness.platform"],
            required_host_platforms=["windows-x64"],
            required_validation_profile_ids=["proof-dev", "trace-platform"],
            required_validation_kinds=["proof"],
        )

        self.assertEqual(
            {
                "EngineLogWriteLite",
                "EngineObjectHandleLite",
                "EngineLifecycleCallbackLite",
                "EngineHostProof",
            },
            {str(record["subjectId"]) for record in records},
        )

        expected_focus_area = {
            "EngineLogWriteLite": "service-call",
            "EngineObjectHandleLite": "object-handle",
            "EngineLifecycleCallbackLite": "lifecycle-callback",
            "EngineHostProof": "host-proof",
        }

        for record in records:
            manifest = record["manifest"]
            profile = dict(manifest.get("engineProofProfile") or {})
            pipeline_ids = {
                str(pipeline.get("pipelineId") or "")
                for pipeline in list(manifest.get("executionPipelines") or [])
            }
            matrix_ids = {
                str(matrix.get("matrixId") or "")
                for matrix in list(manifest.get("environmentMatrices") or [])
            }

            self.assertEqual("engine-binding", str(profile.get("proofKind") or ""))
            self.assertEqual(expected_focus_area[str(record["subjectId"])], str(profile.get("focusArea") or ""))
            self.assertGreater(len(list(profile.get("expectedCapabilityIds") or [])), 0)
            self.assertGreater(len(list(profile.get("expectedEvidenceKinds") or [])), 0)
            self.assertEqual(
                {"engine-runtime-output", "engine-runtime-trace"},
                pipeline_ids,
            )
            self.assertEqual(
                {"windows-dev-output", "windows-reference-trace"},
                matrix_ids,
            )

    def test_existing_subject_manifests_default_or_explicit_test_declaration_mode_match_contract(self) -> None:
        subjects_module = load_module(SUBJECTS_MODULE_PATH, "chaos_subject_manifest_declaration_mode_default")

        records = subjects_module.load_subject_records(REPO_ROOT)
        self.assertGreater(len(records), 0)

        explicit_modes = {
            "HotUpdateHostPack": "require",
            "MixedExecutionFeaturePack": "require",
            "PerformanceFeaturePack": "require",
            "MainlineFeaturePack": "require",
            "BenchArithmetic": "none",
            "BenchAllocation": "none",
            "BenchDispatch": "none",
            "BenchGeneric": "none",
            "BenchHotUpdateDispatch": "none",
            "BenchHotUpdateLoad": "none",
            "BenchHotUpdateRoundtrip": "none",
            "BenchMixed": "none",
            "HotUpdateSkeletonProof": "none",
            "MixedExecutionProof": "none",
            "InterpreterArithmeticProof": "none",
            "InterpreterLoweringProof": "none",
            "ArrayOpsProof": "none",
            "BitwiseOpsProof": "none",
            "BranchOpsProof": "none",
            "ConversionOpsProof": "none",
            "ObjectOpsProof": "none",
            "OverflowOpsProof": "none",
            "AsyncAwaitProof": "none",
            "ThreadingProof": "none",
            "NestedExceptionProof": "none",
            "GenericCollectionProof": "none",
            "DelegateChainProof": "none",
            "InterfaceDispatchProof": "none",
            "VTableDispatchProof": "none",
            "LinkerStrippingProof": "none",
            "MarshalingProof": "none",
            "CrossBoundaryExceptionProof": "none",
        }
        for record in records:
            subject_id = str(record["subjectId"])
            self.assertEqual(
                explicit_modes.get(subject_id, "auto"),
                record["capabilities"]["testDeclarationMode"],
            )

    def test_legacy_opcode_proof_subjects_can_project_canonical_mainline_source(self) -> None:
        subjects_module = load_module(SUBJECTS_MODULE_PATH, "chaos_subject_manifest_legacy_ops_shells")
        manifest = subjects_module.load_subject_manifest(REPO_ROOT, "MainlineFeaturePack")
        validation_profiles = dict(manifest.get("validationProfiles") or {})

        expected_entries = {
            "ArrayOpsProof": ("proof-array-ops", "windows-array-ops-check", "MainlineFeaturePack/ArrayOpsProofEntry::Run()"),
            "BitwiseOpsProof": ("proof-bitwise-ops", "windows-bitwise-ops-check", "MainlineFeaturePack/BitwiseOpsProofEntry::Run()"),
            "BranchOpsProof": ("proof-branch-ops", "windows-branch-ops-check", "MainlineFeaturePack/BranchOpsProofEntry::Run()"),
            "ConversionOpsProof": ("proof-conversion-ops", "windows-conversion-ops-check", "MainlineFeaturePack/ConversionOpsProofEntry::Run()"),
            "ObjectOpsProof": ("proof-object-ops", "windows-object-ops-check", "MainlineFeaturePack/ObjectOpsProofEntry::Run()"),
            "OverflowOpsProof": ("proof-overflow-ops", "windows-overflow-ops-check", "MainlineFeaturePack/OverflowOpsProofEntry::Run()"),
        }

        self.assertEqual("subjects/MainlineFeaturePack/source/MainlineFeaturePack.csproj", manifest["source"]["path"])
        self.assertEqual("require", subjects_module.manifest_capabilities(manifest)["testDeclarationMode"])

        for profile_id, matrix_id, entry in expected_entries.values():
            self.assertEqual(["proof"], validation_profiles[profile_id])
            assert_matrix_source_entry(self, subjects_module, manifest, matrix_id, entry)

    def test_legacy_pure_runtime_benchmark_subjects_can_project_canonical_benchmark_source(self) -> None:
        subjects_module = load_module(SUBJECTS_MODULE_PATH, "chaos_subject_manifest_legacy_benchmark_shells")
        manifest = subjects_module.load_subject_manifest(REPO_ROOT, "PerformanceFeaturePack")

        expected_sources = {
            "BenchArithmetic": ("subjects/PerformanceFeaturePack/source/ArithmeticBenchmark.cs", "ArithmeticBenchmarkEntry", 'Alias = "arithmetic-bench"'),
            "BenchAllocation": ("subjects/PerformanceFeaturePack/source/AllocationBenchmark.cs", "AllocationBenchmarkEntry", 'Alias = "allocation-bench"'),
            "BenchDispatch": ("subjects/PerformanceFeaturePack/source/DispatchBenchmark.cs", "DispatchBenchmarkEntry", 'Alias = "dispatch-bench"'),
            "BenchGeneric": ("subjects/PerformanceFeaturePack/source/GenericBenchmark.cs", "GenericBenchmarkEntry", 'Alias = "generic-bench"'),
        }

        self.assertEqual("subjects/PerformanceFeaturePack/source/PerformanceFeaturePack.csproj", manifest["source"]["path"])
        self.assertEqual("PerformanceFeaturePack/ArithmeticBenchmarkEntry::RunWorkload()", manifest["workloadEntry"])
        self.assertEqual("require", subjects_module.manifest_capabilities(manifest)["testDeclarationMode"])

        for source_path, entry_class, alias in expected_sources.values():
            source = (REPO_ROOT / source_path).read_text(encoding="utf-8")
            self.assertIn("[ChaosBenchmark(", source)
            self.assertIn(entry_class, source)
            self.assertIn(alias, source)

    def test_legacy_hot_update_benchmark_subjects_can_project_canonical_hot_update_host_source(self) -> None:
        subjects_module = load_module(SUBJECTS_MODULE_PATH, "chaos_subject_manifest_legacy_hot_update_benchmark_shells")
        manifest = subjects_module.load_subject_manifest(REPO_ROOT, "HotUpdateHostPack")

        expected_sources = {
            "BenchHotUpdateDispatch": ("subjects/HotUpdateHostPack/source/HotUpdateDispatchBenchmark.cs", "HotUpdateDispatchBenchmarkEntry", 'Alias = "hot-update-dispatch-bench"'),
            "BenchHotUpdateLoad": ("subjects/HotUpdateHostPack/source/HotUpdateLoadBenchmark.cs", "HotUpdateLoadBenchmarkEntry", 'Alias = "hot-update-load-bench"'),
            "BenchHotUpdateRoundtrip": ("subjects/HotUpdateHostPack/source/HotUpdateRoundtripBenchmark.cs", "HotUpdateRoundtripBenchmarkEntry", 'Alias = "hot-update-roundtrip-bench"'),
        }

        self.assertEqual("subjects/HotUpdateHostPack/source/HotUpdateHostPack.csproj", manifest["source"]["path"])
        self.assertEqual("HotUpdateHostPack/Program::Main()", manifest["source"]["entry"])
        self.assertEqual("require", subjects_module.manifest_capabilities(manifest)["testDeclarationMode"])

        for source_path, entry_class, alias in expected_sources.values():
            source = (REPO_ROOT / source_path).read_text(encoding="utf-8")
            self.assertIn("[ChaosBenchmark(", source)
            self.assertIn(entry_class, source)
            self.assertIn(alias, source)

    def test_legacy_hot_update_proof_subject_can_project_canonical_hot_update_host_source(self) -> None:
        subjects_module = load_module(SUBJECTS_MODULE_PATH, "chaos_subject_manifest_legacy_hot_update_proof_shells")
        manifest = subjects_module.load_subject_manifest(REPO_ROOT, "HotUpdateHostPack")
        source = (REPO_ROOT / "subjects" / "HotUpdateHostPack" / "source" / "HotUpdateSkeletonProofEntry.cs").read_text(encoding="utf-8")

        self.assertEqual("subjects/HotUpdateHostPack/source/HotUpdateHostPack.csproj", manifest["source"]["path"])
        self.assertEqual("HotUpdateHostPack/Program::Main()", manifest["source"]["entry"])
        self.assertEqual("require", subjects_module.manifest_capabilities(manifest)["testDeclarationMode"])
        self.assertIn("[ChaosUnitTest(", source)
        self.assertIn('Alias = "hot-update-skeleton-proof"', source)

    def test_legacy_mixed_execution_benchmark_subject_can_project_canonical_mixed_execution_source(self) -> None:
        subjects_module = load_module(SUBJECTS_MODULE_PATH, "chaos_subject_manifest_legacy_mixed_benchmark_shells")
        manifest = subjects_module.load_subject_manifest(REPO_ROOT, "MixedExecutionFeaturePack")
        source = (REPO_ROOT / "subjects" / "MixedExecutionFeaturePack" / "source" / "MixedExecutionBenchmark.cs").read_text(encoding="utf-8")

        self.assertEqual("subjects/MixedExecutionFeaturePack/source/MixedExecutionFeaturePack.csproj", manifest["source"]["path"])
        self.assertEqual("MixedExecutionFeaturePack/MixedExecutionBenchmarkEntry::RunWorkload()", manifest["workloadEntry"])
        self.assertEqual("require", subjects_module.manifest_capabilities(manifest)["testDeclarationMode"])
        self.assertIn("[ChaosBenchmark(", source)
        self.assertIn("MixedExecutionBenchmarkEntry", source)
        self.assertIn('Alias = "mixed-execution-bench"', source)

    def test_legacy_mixed_execution_proof_subjects_can_project_canonical_mixed_execution_source(self) -> None:
        subjects_module = load_module(SUBJECTS_MODULE_PATH, "chaos_subject_manifest_legacy_mixed_proof_shells")
        manifest = subjects_module.load_subject_manifest(REPO_ROOT, "MixedExecutionFeaturePack")

        proof_source = (REPO_ROOT / "subjects" / "MixedExecutionFeaturePack" / "source" / "MixedExecutionProofEntry.cs").read_text(encoding="utf-8")
        lowering_source = (REPO_ROOT / "subjects" / "MixedExecutionFeaturePack" / "source" / "InterpreterLoweringProofEntry.cs").read_text(encoding="utf-8")

        self.assertEqual("subjects/MixedExecutionFeaturePack/source/MixedExecutionFeaturePack.csproj", manifest["source"]["path"])
        self.assertEqual("MixedExecutionFeaturePack/MixedExecutionProofEntry::Run()", manifest["source"]["entry"])
        self.assertEqual("require", subjects_module.manifest_capabilities(manifest)["testDeclarationMode"])
        self.assertIn("[ChaosUnitTest(", proof_source)
        self.assertIn('Alias = "mixed-execution-proof"', proof_source)
        self.assertIn("[ChaosUnitTest(", lowering_source)
        self.assertIn('Alias = "interpreter-lowering-proof"', lowering_source)

    def test_interpreter_arithmetic_subject_projects_support_assembly_from_mixed_execution_feature_pack(self) -> None:
        subjects_module = load_module(SUBJECTS_MODULE_PATH, "chaos_subject_manifest_legacy_interpreter_arithmetic_shell")
        manifest = subjects_module.load_subject_manifest(REPO_ROOT, "MixedExecutionFeaturePack")
        project_path = REPO_ROOT / "subjects" / "MixedExecutionFeaturePack" / "source" / "InterpreterArithmeticProof" / "InterpreterArithmeticProof.csproj"
        program_path = REPO_ROOT / "subjects" / "MixedExecutionFeaturePack" / "source" / "InterpreterArithmeticProof" / "Program.cs"
        program_source = program_path.read_text(encoding="utf-8")

        self.assertTrue(project_path.is_file(), msg=f"missing nested interpreter arithmetic project: {project_path}")
        self.assertTrue(program_path.is_file(), msg=f"missing nested interpreter arithmetic program: {program_path}")
        self.assertEqual(
            "subjects/MixedExecutionFeaturePack/source/MixedExecutionFeaturePack.csproj",
            manifest["source"]["path"],
        )
        self.assertEqual("require", subjects_module.manifest_capabilities(manifest)["testDeclarationMode"])
        self.assertIn("namespace InterpreterArithmeticProof;", program_source)
        self.assertIn("public static int Main()", program_source)

    def test_legacy_runtime_proof_subjects_can_project_canonical_mainline_source(self) -> None:
        subjects_module = load_module(SUBJECTS_MODULE_PATH, "chaos_subject_manifest_legacy_runtime_shells")
        manifest = subjects_module.load_subject_manifest(REPO_ROOT, "MainlineFeaturePack")
        validation_profiles = dict(manifest.get("validationProfiles") or {})

        expected_entries = {
            "AsyncAwaitProof": ("proof-async-await", "windows-async-await-check", "MainlineFeaturePack/AsyncAwaitProofEntry::Run()"),
            "ThreadingProof": ("proof-threading", "windows-threading-check", "MainlineFeaturePack/ThreadingProofEntry::Run()"),
            "NestedExceptionProof": ("proof-nested-exception", "windows-nested-exception-check", "MainlineFeaturePack/NestedExceptionProofEntry::Run()"),
            "GenericCollectionProof": ("proof-generic-collection", "windows-generic-collection-check", "MainlineFeaturePack/GenericCollectionProofEntry::Run()"),
            "DelegateChainProof": ("proof-delegate-chain", "windows-delegate-chain-check", "MainlineFeaturePack/DelegateChainProofEntry::Run()"),
        }

        self.assertEqual("subjects/MainlineFeaturePack/source/MainlineFeaturePack.csproj", manifest["source"]["path"])
        self.assertEqual("require", subjects_module.manifest_capabilities(manifest)["testDeclarationMode"])

        for profile_id, matrix_id, entry in expected_entries.values():
            self.assertEqual(["proof"], validation_profiles[profile_id])
            assert_matrix_source_entry(self, subjects_module, manifest, matrix_id, entry)

    def test_legacy_advanced_mainline_proof_subjects_can_project_canonical_mainline_source(self) -> None:
        subjects_module = load_module(SUBJECTS_MODULE_PATH, "chaos_subject_manifest_legacy_advanced_shells")
        manifest = subjects_module.load_subject_manifest(REPO_ROOT, "MainlineFeaturePack")
        validation_profiles = dict(manifest.get("validationProfiles") or {})

        expected_entries = {
            "InterfaceDispatchProof": ("proof-interface-dispatch", "windows-interface-dispatch-check", "MainlineFeaturePack/InterfaceDispatchProofEntry::Run()"),
            "VTableDispatchProof": ("proof-vtable-dispatch", "windows-vtable-dispatch-check", "MainlineFeaturePack/VTableDispatchProofEntry::Run()"),
            "LinkerStrippingProof": ("proof-linker-stripping", "windows-linker-stripping-check", "MainlineFeaturePack/LinkerStrippingProofEntry::Run()"),
            "MarshalingProof": ("proof-marshaling", "windows-marshaling-check", "MainlineFeaturePack/MarshalingProofEntry::Run()"),
            "CrossBoundaryExceptionProof": ("proof-cross-boundary-exception", "windows-cross-boundary-eh-check", "MainlineFeaturePack/CrossBoundaryExceptionProofEntry::Run()"),
        }

        self.assertEqual("subjects/MainlineFeaturePack/source/MainlineFeaturePack.csproj", manifest["source"]["path"])
        self.assertEqual("require", subjects_module.manifest_capabilities(manifest)["testDeclarationMode"])

        for profile_id, matrix_id, entry in expected_entries.values():
            self.assertEqual(["proof"], validation_profiles[profile_id])
            assert_matrix_source_entry(self, subjects_module, manifest, matrix_id, entry)

    def test_legacy_native_subject_manifest_projects_thin_fields_by_default(self) -> None:
        subjects_module = load_module(SUBJECTS_MODULE_PATH, "chaos_subject_manifest_thin_native_defaults")

        manifest = subjects_module.load_subject_manifest(REPO_ROOT, "HelloWorldObject")
        capabilities = subjects_module.manifest_capabilities(manifest)

        self.assertEqual("dotnet-project-set", manifest["sourceModel"])
        self.assertEqual("project-reference", manifest["dependencyModel"])
        self.assertEqual("generated-native", manifest["executablePlan"])
        self.assertEqual("native-executable", manifest["engineeringProfile"])
        self.assertEqual(
            {
                "windows-x64": "ready",
                "android-arm64": "ready",
                "linux-x64": "ready",
            },
            manifest["availability"],
        )
        self.assertEqual({}, manifest["compatibility"])
        self.assertEqual("dotnet-project-set", capabilities["sourceModel"])
        self.assertEqual("project-reference", capabilities["dependencyModel"])
        self.assertEqual("generated-native", capabilities["executablePlan"])
        self.assertEqual("native-executable", capabilities["engineeringProfile"])

    def test_legacy_benchmark_manifest_projects_thin_fields_from_default_matrix(self) -> None:
        subjects_module = load_module(SUBJECTS_MODULE_PATH, "chaos_subject_manifest_thin_benchmark_defaults")

        manifest = subjects_module.load_subject_manifest(REPO_ROOT, "PerformanceFeaturePack")
        capabilities = subjects_module.manifest_capabilities(manifest)

        self.assertEqual("dotnet-project-set", manifest["sourceModel"])
        self.assertEqual("project-reference", manifest["dependencyModel"])
        self.assertEqual("managed-host", manifest["executablePlan"])
        self.assertEqual("managed-output", manifest["engineeringProfile"])
        self.assertEqual(
            {
                "windows-x64": "ready",
            },
            manifest["availability"],
        )
        self.assertEqual("managed-host", capabilities["executablePlan"])
        self.assertEqual("managed-output", capabilities["engineeringProfile"])

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
