from __future__ import annotations

import importlib.util
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

            self.assertIn(manifest_path, manifest_paths)
            self.assertEqual(subject_id, manifest["subjectId"])
            self.assertEqual(subject_id, manifest_path.parent.name)
            self.assertEqual("dotnet-project", manifest["source"]["type"])
            self.assertTrue(str(manifest["source"]["path"]).startswith(f"subjects/{subject_id}/source/"))
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
                    self.assertTrue(project_path.startswith(f"subjects/{subject_id}/validation/"))
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
            self.assertIn(capabilities["category"], {"canonical", "diagnostic"})
            self.assertTrue(str(manifest["source"]["path"]).startswith(f"subjects/{record['subjectId']}/source/"))
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
        self.assertEqual({"proof-runtime-output", "proof-runtime-trace", "native-runtime-perf"}, pipeline_ids)
        self.assertEqual(
            {
                "windows-native-check",
                "windows-reference-trace",
                "windows-dispatch-check",
                "windows-generic-layout-check",
                "windows-array-boxing-check",
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
                "windows-dispatch-check",
                "windows-generic-layout-check",
                "windows-array-boxing-check",
            },
            matrix_ids,
        )
        self.assertEqual(
            "MainlineFeaturePack/DispatchProofEntry::Run()",
            str(dict(dispatch_matrix.get("source") or {})["entry"]),
        )


if __name__ == "__main__":
    unittest.main()
