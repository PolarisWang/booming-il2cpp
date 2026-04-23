from tests.unit.compatibility.subject_manifest_schema_test_support import *


class TestSubjectManifestSchemaQueries(SubjectManifestSchemaTestSupport):
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
                        or project_path.startswith("src/tools/Chaos.IL2CPP.Tools.")
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
            "subjects/SolutionCorePack/source/Host/SolutionCorePack.csproj",
            manifest["source"]["primaryProjectPath"],
        )
        self.assertEqual("CoreRuntimeFeatures/ProofEntry::Run()", manifest["source"]["entry"])
        self.assertEqual("CoreRuntimeBenchmarks/ArithmeticBenchmarkEntry::RunWorkload()", manifest["workloadEntry"])
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
            "CoreRuntimeFeatures/Program::Main(System.String[])",
        )
        assert_matrix_source_entry(
            self,
            subjects_module,
            manifest,
            "windows-archetype-reference-assembly-managed-output",
            "GoldenReferenceAssembly.App/Program::Main()",
        )
        assert_matrix_source_entry(
            self,
            subjects_module,
            manifest,
            "windows-native-perf",
            "CoreRuntimeBenchmarks/ArithmeticBenchmarkEntry::RunWorkload()",
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
            {"HotUpdateHostPack", "MixedExecutionFeaturePack", "SolutionCorePack"},
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
