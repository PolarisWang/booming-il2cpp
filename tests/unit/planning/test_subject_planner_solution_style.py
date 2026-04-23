from tests.unit.planning.subject_planner_test_support import *


class TestSubjectPlannerSolutionStyle(SubjectPlannerTestSupport):
    def test_planner_surfaces_workload_entry_for_solution_style_benchmark_subject(self) -> None:
        planner_module = load_module(PLANNER_MODULE_PATH, "chaos_subject_planner_workload_entry")
        subject_id = "FixtureSolutionStyleBenchmarkSubject"
        workload_entry = f"{subject_id}/Benchmarks::RunDefault()"
        repo_root, manifest = create_subject_repo(
            "solution-style-benchmark",
            build_solution_style_managed_perf_subject_manifest(
                subject_id,
                source_entry=workload_entry,
                workload_entry=workload_entry,
            ),
        )

        try:
            plan = planner_module.build_plan(
                repo_root,
                subject_id,
                goal_id="perf.release",
                matrix_id="windows-managed-perf",
                run_id="20260413-solution-style-benchmark-workload-entry-001",
            )

            self.assertEqual(str(manifest["subjectId"]), plan["selection"]["subjectId"])
            self.assertEqual(str(manifest["source"]["path"]), plan["selection"]["source"]["path"])
            self.assertEqual(str(manifest["source"]["primaryProjectPath"]), plan["selection"]["source"]["primaryProjectPath"])
            self.assertEqual(workload_entry, plan["selection"]["source"]["entry"])
            self.assertEqual(workload_entry, plan["selection"]["workloadEntry"])
        finally:
            shutil.rmtree(repo_root, ignore_errors=True)

    def test_planner_uses_explicit_workload_entry_as_effective_source_entry_for_solution_style_perf_subject(self) -> None:
        planner_module = load_module(PLANNER_MODULE_PATH, "chaos_subject_planner_explicit_solution_workload_entry_only")
        subject_id = "FixtureExplicitPerfSubject"
        repo_root, manifest = create_subject_repo(
            "explicit-solution-workload",
            build_solution_style_managed_perf_subject_manifest(subject_id),
        )
        explicit_entry = f"{subject_id}/Benchmarks::RunHotPath()"

        try:
            plan = planner_module.build_plan(
                repo_root,
                subject_id,
                goal_id="perf.release",
                matrix_id="windows-managed-perf",
                run_id="20260413-explicit-solution-workload-entry-001",
                workload_entry=explicit_entry,
            )

            self.assertEqual(str(manifest["subjectId"]), plan["selection"]["subjectId"])
            self.assertEqual(str(manifest["source"]["path"]), plan["selection"]["source"]["path"])
            self.assertEqual(str(manifest["source"]["primaryProjectPath"]), plan["selection"]["source"]["primaryProjectPath"])
            self.assertEqual(explicit_entry, plan["selection"]["source"]["entry"])
            self.assertEqual(explicit_entry, plan["selection"]["workloadEntry"])
        finally:
            shutil.rmtree(repo_root, ignore_errors=True)

    def test_planner_preserves_explicit_source_entry_when_workload_entry_is_also_provided(self) -> None:
        planner_module = load_module(
            PLANNER_MODULE_PATH,
            "chaos_subject_planner_preserve_explicit_source_entry_with_workload_override",
        )
        subject_id = "FixtureBenchmarkHostPerfSubject"
        benchmark_host_entry = (
            f"{subject_id}.DeclaredBenchmarkHost/"
            f"Chaos.Generated.ManagedTests.{subject_id}.{subject_id}DeclaredBenchmarkHost::Execute(System.Int32)"
        )
        workload_entry = f"{subject_id}/Benchmarks::RunHotPath()"
        repo_root, manifest = create_subject_repo(
            "preserve-explicit-source-entry-with-workload",
            build_solution_style_managed_perf_subject_manifest(
                subject_id,
                source_entry=f"{subject_id}/Program::Main()",
                workload_entry=f"{subject_id}/Benchmarks::RunDefault()",
            ),
        )

        try:
            plan = planner_module.build_plan(
                repo_root,
                subject_id,
                goal_id="perf.release",
                matrix_id="windows-managed-perf",
                run_id="20260417-preserve-explicit-source-entry-001",
                source_entry=benchmark_host_entry,
                workload_entry=workload_entry,
                entry_selection={
                    "family": "declared-benchmark",
                    "stableId": "fixture-benchmark-hot-path",
                    "alias": "hot-path",
                    "entryIndex": 7,
                },
            )

            self.assertEqual(str(manifest["subjectId"]), plan["selection"]["subjectId"])
            self.assertEqual(str(manifest["source"]["path"]), plan["selection"]["source"]["path"])
            self.assertEqual(benchmark_host_entry, plan["selection"]["source"]["entry"])
            self.assertEqual(workload_entry, plan["selection"]["workloadEntry"])
            self.assertEqual(
                {
                    "family": "declared-benchmark",
                    "stableId": "fixture-benchmark-hot-path",
                    "alias": "hot-path",
                    "entryIndex": 7,
                },
                plan["selection"]["entrySelection"],
            )
        finally:
            shutil.rmtree(repo_root, ignore_errors=True)

    def test_declared_benchmark_selection_ignores_manifest_workload_entry_without_explicit_override(self) -> None:
        planner_module = load_module(
            PLANNER_MODULE_PATH,
            "chaos_subject_planner_declared_entry_ignores_manifest_workload",
        )
        repo_root = REPO_ROOT / "artifacts" / ".tmp-tests" / "subject-planner" / f"declared-entry-workload-{uuid.uuid4().hex}"
        manifest_path = repo_root / "subjects" / "FixtureDeclaredWorkloadIsolation" / "subject.manifest.json"
        manifest_path.parent.mkdir(parents=True, exist_ok=True)

        manifest = {
            "subjectId": "FixtureDeclaredWorkloadIsolation",
            "displayName": "FixtureDeclaredWorkloadIsolation",
            "category": "benchmark",
            "defaultGoal": "perf.release",
            "defaultMatrix": "windows-managed-perf",
            "defaultValidationProfile": "perf-profile",
            "source": {
                "type": "dotnet-project",
                "path": "subjects/FixtureDeclaredWorkloadIsolation/source/FixtureDeclaredWorkloadIsolation.csproj",
                "entry": "FixtureDeclaredWorkloadIsolation/Program::Main()",
            },
            "workloadEntry": "FixtureDeclaredWorkloadIsolation/Program::RunWorkloadA()",
            "validationProfiles": {
                "perf-profile": ["perf"],
            },
            "validation": {
                "perf": {
                    "kind": "perf",
                    "driver": "csharp-perf-harness",
                    "defaultVariant": "PROFILE",
                }
            },
            "executionPipelines": [
                {
                    "pipelineId": "managed-benchmark",
                    "stages": [
                        {"stageId": "source-resolve", "kind": "source-resolve", "scope": "shared", "bucket": "source", "dependsOn": []},
                        {"stageId": "host-input-build", "kind": "host-input-build", "scope": "shared", "bucket": "host-input", "dependsOn": ["source-resolve"]},
                        {"stageId": "runtime-perf-collect", "kind": "runtime-perf-collect", "scope": "matrix", "bucket": "runtime", "dependsOn": ["host-input-build"]},
                        {"stageId": "report-assemble", "kind": "report-assemble", "scope": "matrix", "bucket": "report", "dependsOn": ["runtime-perf-collect"]},
                    ],
                }
            ],
            "environmentMatrices": [
                {
                    "matrixId": "windows-managed-perf",
                    "pipelineId": "managed-benchmark",
                    "supportedGoals": ["perf.release"],
                    "executionContext": {
                        "hostPlatform": "windows-x64",
                        "targetPlatform": "windows-x64",
                        "toolchainProfile": "dotnet-managed",
                        "runtimeProfile": "managed-perf-release",
                    },
                    "validationIntent": {
                        "validationMode": "perf",
                        "adaptationLevel": "managed-runtime",
                        "expectedOutcome": "pass",
                    },
                    "artifactPlan": {
                        "requiredBuckets": ["source", "host-input", "runtime", "report"],
                        "evidenceTerminalBucket": "report",
                    },
                }
            ],
        }

        try:
            manifest_path.write_text(json.dumps(manifest, ensure_ascii=False, indent=2) + "\n", encoding="utf-8")

            plan = planner_module.build_plan(
                repo_root,
                "FixtureDeclaredWorkloadIsolation",
                goal_id="perf.release",
                matrix_id="windows-managed-perf",
                run_id="fixture-declared-workload-isolation",
                entry_selection={
                    "family": "declared-benchmark",
                    "stableId": "bench/a",
                    "alias": "bench-a",
                    "entryIndex": 1,
                },
            )

            manifest["workloadEntry"] = "FixtureDeclaredWorkloadIsolation/Program::RunWorkloadB()"
            manifest_path.write_text(json.dumps(manifest, ensure_ascii=False, indent=2) + "\n", encoding="utf-8")
            updated_plan = planner_module.build_plan(
                repo_root,
                "FixtureDeclaredWorkloadIsolation",
                goal_id="perf.release",
                matrix_id="windows-managed-perf",
                run_id="fixture-declared-workload-isolation",
                entry_selection={
                    "family": "declared-benchmark",
                    "stableId": "bench/a",
                    "alias": "bench-a",
                    "entryIndex": 1,
                },
            )

            self.assertEqual("", plan["selection"]["workloadEntry"])
            self.assertEqual("", updated_plan["selection"]["workloadEntry"])
            self.assertEqual(plan["stagePlan"][0]["fingerprint"], updated_plan["stagePlan"][0]["fingerprint"])
        finally:
            shutil.rmtree(repo_root, ignore_errors=True)

    def test_planner_surfaces_matrix_workload_entry_for_solution_style_perf_subject(self) -> None:
        planner_module = load_module(PLANNER_MODULE_PATH, "chaos_subject_planner_solution_style_matrix_workload_entry")
        subject_id = "FixtureMatrixPerfSubject"
        matrix_entry = f"{subject_id}/Benchmarks::RunMatrixDefault()"
        repo_root, manifest = create_subject_repo(
            "matrix-solution-workload",
            build_solution_style_managed_perf_subject_manifest(
                subject_id,
                matrix_source_entry=matrix_entry,
                matrix_workload_entry=matrix_entry,
            ),
        )

        try:
            plan = planner_module.build_plan(
                repo_root,
                subject_id,
                goal_id="perf.release",
                matrix_id="windows-managed-perf",
                run_id="20260413-matrix-solution-workload-entry-001",
            )

            self.assertEqual(str(manifest["subjectId"]), plan["selection"]["subjectId"])
            self.assertEqual(str(manifest["source"]["path"]), plan["selection"]["source"]["path"])
            self.assertEqual(str(manifest["source"]["primaryProjectPath"]), plan["selection"]["source"]["primaryProjectPath"])
            self.assertEqual(matrix_entry, plan["selection"]["source"]["entry"])
            self.assertEqual(matrix_entry, plan["selection"]["workloadEntry"])
        finally:
            shutil.rmtree(repo_root, ignore_errors=True)

    def test_planner_surfaces_native_perf_override_for_solution_style_perf_subject(self) -> None:
        planner_module = load_module(PLANNER_MODULE_PATH, "chaos_subject_planner_solution_style_native_perf_entry")
        subject_id = "FixtureNativePerfSubject"
        native_entry = f"{subject_id}/Benchmarks::RunNativePerf()"
        manifest = build_solution_style_managed_perf_subject_manifest(subject_id)
        manifest["executionPipelines"].append(
            {
                "pipelineId": "native-benchmark",
                "stages": [
                    {"stageId": "source-resolve", "kind": "source-resolve", "scope": "shared", "bucket": "source", "dependsOn": []},
                    {"stageId": "host-input-build", "kind": "host-input-build", "scope": "shared", "bucket": "host-input", "dependsOn": ["source-resolve"]},
                    {"stageId": "build-target", "kind": "build-target", "scope": "matrix", "bucket": "build", "dependsOn": ["host-input-build"]},
                    {"stageId": "native-runtime-perf", "kind": "native-runtime-perf", "scope": "matrix", "bucket": "runtime", "dependsOn": ["build-target"]},
                    {"stageId": "report-assemble", "kind": "report-assemble", "scope": "matrix", "bucket": "report", "dependsOn": ["native-runtime-perf"]},
                ],
            }
        )
        manifest["environmentMatrices"].append(
            {
                "matrixId": "windows-native-perf",
                "pipelineId": "native-benchmark",
                "supportedGoals": ["perf.release"],
                "source": {"entry": native_entry},
                "workloadEntry": native_entry,
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
                    "requiredBuckets": ["source", "host-input", "build", "runtime", "report"],
                    "evidenceTerminalBucket": "report",
                },
            }
        )
        repo_root, manifest = create_subject_repo("solution-style-native-perf", manifest)

        try:
            plan = planner_module.build_plan(
                repo_root,
                subject_id,
                goal_id="perf.release",
                matrix_id="windows-native-perf",
                run_id="20260413-solution-style-native-perf-workload-entry-001",
            )

            self.assertEqual(str(manifest["subjectId"]), plan["selection"]["subjectId"])
            self.assertEqual(str(manifest["source"]["path"]), plan["selection"]["source"]["path"])
            self.assertEqual(str(manifest["source"]["primaryProjectPath"]), plan["selection"]["source"]["primaryProjectPath"])
            self.assertEqual(native_entry, plan["selection"]["source"]["entry"])
            self.assertEqual(native_entry, plan["selection"]["workloadEntry"])
        finally:
            shutil.rmtree(repo_root, ignore_errors=True)

    def test_planner_surfaces_explicit_proof_entry_for_solution_style_output_subject(self) -> None:
        planner_module = load_module(PLANNER_MODULE_PATH, "chaos_subject_planner_solution_style_explicit_proof_entry")
        subject_id = "FixtureExplicitProofSubject"
        repo_root, manifest = create_subject_repo(
            "explicit-solution-proof",
            build_solution_style_managed_output_subject_manifest(subject_id),
        )
        explicit_entry = f"{subject_id}/Proofs::RunExplicit()"

        try:
            plan = planner_module.build_plan(
                repo_root,
                subject_id,
                goal_id="correctness.dev",
                matrix_id="windows-managed-output",
                run_id="20260413-explicit-solution-proof-entry-001",
                source_entry=explicit_entry,
            )

            self.assertEqual(str(manifest["subjectId"]), plan["selection"]["subjectId"])
            self.assertEqual(explicit_entry, plan["selection"]["source"]["entry"])
        finally:
            shutil.rmtree(repo_root, ignore_errors=True)

    def test_planner_surfaces_matrix_proof_entry_for_solution_style_output_subject(self) -> None:
        planner_module = load_module(PLANNER_MODULE_PATH, "chaos_subject_planner_solution_style_matrix_proof_entry")
        subject_id = "FixtureMatrixProofSubject"
        matrix_entry = f"{subject_id}/Proofs::RunMatrixDefault()"
        repo_root, manifest = create_subject_repo(
            "matrix-solution-proof",
            build_solution_style_managed_output_subject_manifest(
                subject_id,
                matrix_source_entry=matrix_entry,
            ),
        )

        try:
            plan = planner_module.build_plan(
                repo_root,
                subject_id,
                goal_id="correctness.dev",
                matrix_id="windows-managed-output",
                run_id="20260413-matrix-solution-proof-entry-001",
            )

            self.assertEqual(str(manifest["subjectId"]), plan["selection"]["subjectId"])
            self.assertEqual(str(manifest["source"]["path"]), plan["selection"]["source"]["path"])
            self.assertEqual(str(manifest["source"]["primaryProjectPath"]), plan["selection"]["source"]["primaryProjectPath"])
            self.assertEqual(matrix_entry, plan["selection"]["source"]["entry"])
        finally:
            shutil.rmtree(repo_root, ignore_errors=True)
