from tests.unit.planning.subject_planner_test_support import *


class TestSubjectPlannerDefaults(SubjectPlannerTestSupport):
    def test_planner_uses_subject_defaults_and_subject_artifact_layout(self) -> None:
        planner_module = load_module(PLANNER_MODULE_PATH, "chaos_subject_planner_default")
        subjects_module = load_subjects_module("chaos_subject_planner_default_subjects")
        repo_root, manifest = create_subject_repo("default", build_native_proof_subject_manifest())
        subject_id = str(manifest["subjectId"])
        run_id = "20260407-fixture-default-001"

        try:
            plan = planner_module.build_plan(repo_root, subject_id, run_id=run_id)
            expected_matrix = subjects_module.find_matrix(manifest, str(manifest["defaultMatrix"]))
            expected_pipeline = subjects_module.find_pipeline(manifest, str(expected_matrix["pipelineId"]))

            self.assertEqual("v1", plan["planVersion"])
            self.assertEqual(subject_id, plan["request"]["subjectId"])
            self.assertIsNone(plan["request"]["goalId"])
            self.assertIsNone(plan["request"]["matrixId"])
            self.assertEqual(run_id, plan["request"]["runId"])
            self.assertEqual(subject_id, plan["selection"]["subjectId"])
            self.assertEqual(str(manifest["defaultGoal"]), plan["selection"]["goalId"])
            self.assertEqual(str(manifest["defaultMatrix"]), plan["selection"]["matrixId"])
            self.assertEqual(str(expected_matrix["pipelineId"]), plan["selection"]["pipelineId"])
            self.assertEqual(
                str(dict(expected_matrix["artifactPlan"])["evidenceTerminalBucket"]),
                plan["selection"]["artifactPlan"]["evidenceTerminalBucket"],
            )
            self.assertEqual(f"artifacts/subjects/{subject_id}", plan["artifactsRoot"]["subjectRoot"])
            self.assertEqual(f"artifacts/subjects/{subject_id}/runs/{run_id}", plan["artifactsRoot"]["runRoot"])
            self.assertEqual(
                [str(stage["stageId"]) for stage in list(expected_pipeline.get("stages") or [])],
                [stage["stageId"] for stage in plan["stagePlan"]],
            )
            self.assertEqual(
                subjects_module.stage_paths(subject_id, str(expected_matrix["matrixId"]), run_id=run_id, bucket="source", scope="shared", kind="source-resolve")["manifestPath"],
                plan["stagePlan"][0]["paths"]["manifestPath"],
            )
            self.assertEqual(
                subjects_module.stage_paths(subject_id, str(expected_matrix["matrixId"]), run_id=run_id, bucket="report", scope="matrix", kind="report-assemble")["manifestPath"],
                plan["stagePlan"][-1]["paths"]["manifestPath"],
            )
            self.assertTrue(all(stage["executionMode"] in {"executed", "reused", "invalidated"} for stage in plan["stagePlan"]))
        finally:
            shutil.rmtree(repo_root, ignore_errors=True)

    def test_planner_selects_requested_trace_matrix_without_subject_name_coupling(self) -> None:
        planner_module = load_module(PLANNER_MODULE_PATH, "chaos_subject_planner_trace")
        subjects_module = load_subjects_module("chaos_subject_planner_trace_subjects")
        repo_root, manifest = create_subject_repo("trace", build_native_proof_subject_manifest("FixtureTraceSubject"))
        subject_id = str(manifest["subjectId"])
        trace_matrix = find_matrix_for_goal(subjects_module, manifest, "correctness.platform", required_stage_kind="runtime-trace-compare")
        run_id = "20260407-fixture-trace-001"

        try:
            plan = planner_module.build_plan(
                repo_root,
                subject_id,
                goal_id="correctness.platform",
                matrix_id=str(trace_matrix["matrixId"]),
                run_id=run_id,
            )
            expected_pipeline = subjects_module.find_pipeline(manifest, str(trace_matrix["pipelineId"]))

            self.assertEqual(str(trace_matrix["matrixId"]), plan["selection"]["matrixId"])
            self.assertEqual(str(trace_matrix["pipelineId"]), plan["selection"]["pipelineId"])
            self.assertEqual(
                [str(stage["stageId"]) for stage in list(expected_pipeline.get("stages") or [])],
                [stage["stageId"] for stage in plan["stagePlan"]],
            )
            trace_stage = next(stage for stage in plan["stagePlan"] if stage["kind"] == "runtime-trace-compare")
            self.assertEqual(
                subjects_module.stage_paths(subject_id, str(trace_matrix["matrixId"]), run_id=run_id, bucket="runtime", scope="matrix", kind="runtime-trace-compare")["manifestPath"],
                trace_stage["paths"]["manifestPath"],
            )
            self.assertEqual(
                subjects_module.stage_paths(subject_id, str(trace_matrix["matrixId"]), run_id=run_id, bucket="runtime", scope="matrix", kind="runtime-trace-compare")["reportPaths"],
                trace_stage["paths"]["reportPaths"],
            )
        finally:
            shutil.rmtree(repo_root, ignore_errors=True)

    def test_planner_rejects_goal_matrix_mismatch(self) -> None:
        planner_module = load_module(PLANNER_MODULE_PATH, "chaos_subject_planner_mismatch")
        repo_root, manifest = create_subject_repo("mismatch", build_native_proof_subject_manifest("FixtureMismatchSubject"))
        subject_id = str(manifest["subjectId"])
        matrix_id, unsupported_goal_id = find_goal_matrix_mismatch(manifest)

        try:
            with self.assertRaisesRegex(ValueError, "does not support goal"):
                planner_module.build_plan(repo_root, subject_id, goal_id=unsupported_goal_id, matrix_id=matrix_id)
        finally:
            shutil.rmtree(repo_root, ignore_errors=True)

    def test_planner_uses_perf_defaults_without_benchmark_name_coupling(self) -> None:
        planner_module = load_module(PLANNER_MODULE_PATH, "chaos_subject_planner_perf_default")
        repo_root = REPO_ROOT / "artifacts" / ".tmp-tests" / "subject-planner" / f"perf-default-{uuid.uuid4().hex}"
        manifest_path = repo_root / "subjects" / "FixtureBenchDefaults" / "subject.manifest.json"
        manifest_path.parent.mkdir(parents=True, exist_ok=True)
        manifest = {
            "subjectId": "FixtureBenchDefaults",
            "displayName": "FixtureBenchDefaults",
            "category": "benchmark",
            "defaultGoal": "perf.release",
            "defaultMatrix": "windows-managed-perf",
            "defaultValidationProfile": "perf-profile",
            "source": {
                "type": "dotnet-project",
                "path": "subjects/FixtureBenchDefaults/source/FixtureBenchDefaults.csproj",
                "entry": "FixtureBenchDefaults/Program::Main()",
            },
            "workloadEntry": "FixtureBenchDefaults/Program::RunWorkload()",
            "validationProfiles": {
                "perf-profile": ["perf"],
            },
            "validation": {
                "perf": {
                    "kind": "perf",
                    "driver": "native-runtime-perf",
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
            run_id = "20260407-fixture-perf-default-001"
            plan = planner_module.build_plan(repo_root, "FixtureBenchDefaults", run_id=run_id)

            self.assertEqual("FixtureBenchDefaults", plan["selection"]["subjectId"])
            self.assertEqual("perf.release", plan["selection"]["goalId"])
            self.assertEqual("windows-managed-perf", plan["selection"]["matrixId"])
            self.assertEqual("managed-benchmark", plan["selection"]["pipelineId"])
            self.assertEqual("report", plan["selection"]["artifactPlan"]["evidenceTerminalBucket"])
            self.assertEqual("perf-profile", plan["selection"]["validationProfileId"])
            self.assertEqual("PROFILE", plan["selection"]["variant"])
        finally:
            shutil.rmtree(repo_root, ignore_errors=True)

    def test_planner_selects_release_perf_matrix_without_subject_name_coupling(self) -> None:
        planner_module = load_module(PLANNER_MODULE_PATH, "chaos_subject_planner_perf_release")
        subjects_module = load_subjects_module("chaos_subject_planner_perf_release_subjects")
        repo_root, manifest = create_subject_repo("perf-release", build_managed_perf_subject_manifest())
        subject_id = str(manifest["subjectId"])
        selected_matrix = find_matrix_for_goal(subjects_module, manifest, "perf.release")
        run_id = "20260407-fixture-perf-release-001"

        try:
            plan = planner_module.build_plan(
                repo_root,
                subject_id,
                goal_id="perf.release",
                matrix_id=str(selected_matrix["matrixId"]),
                run_id=run_id,
            )

            self.assertEqual(str(selected_matrix["matrixId"]), plan["selection"]["matrixId"])
            self.assertEqual(str(selected_matrix["pipelineId"]), plan["selection"]["pipelineId"])
            self.assertEqual(
                subjects_module.stage_paths(subject_id, str(selected_matrix["matrixId"]), run_id=run_id, bucket="report", scope="matrix", kind="report-assemble")["manifestPath"],
                plan["stagePlan"][-1]["paths"]["manifestPath"],
            )
        finally:
            shutil.rmtree(repo_root, ignore_errors=True)

    def test_planner_selects_first_matrix_supporting_requested_goal_when_matrix_is_omitted(self) -> None:
        planner_module = load_module(PLANNER_MODULE_PATH, "chaos_subject_planner_goal_only")
        repo_root, manifest = create_subject_repo("goal-only", build_managed_perf_subject_manifest("FixtureGoalOnlySubject"))
        subject_id = str(manifest["subjectId"])
        expected_matrix = expected_matrix_for_goal(manifest, "perf.release")

        try:
            plan = planner_module.build_plan(repo_root, subject_id, goal_id="perf.release", run_id="20260407-fixture-auto-001")

            self.assertEqual("perf.release", plan["selection"]["goalId"])
            self.assertEqual(str(expected_matrix["matrixId"]), plan["selection"]["matrixId"])
            self.assertEqual(str(expected_matrix["pipelineId"]), plan["selection"]["pipelineId"])
        finally:
            shutil.rmtree(repo_root, ignore_errors=True)

    def test_planner_uses_managed_output_defaults_without_subject_name_coupling(self) -> None:
        planner_module = load_module(PLANNER_MODULE_PATH, "chaos_subject_planner_managed_output")
        repo_root, manifest = create_subject_repo("managed-output", build_managed_output_subject_manifest())
        subject_id = str(manifest["subjectId"])
        expected_matrix = expected_matrix_for_goal(manifest, "correctness.dev")
        run_id = "20260407-fixture-managed-output-001"

        try:
            plan = planner_module.build_plan(repo_root, subject_id, run_id=run_id)

            self.assertEqual(subject_id, plan["selection"]["subjectId"])
            self.assertEqual("correctness.dev", plan["selection"]["goalId"])
            self.assertEqual(str(expected_matrix["matrixId"]), plan["selection"]["matrixId"])
            self.assertEqual(str(expected_matrix["pipelineId"]), plan["selection"]["pipelineId"])
        finally:
            shutil.rmtree(repo_root, ignore_errors=True)

    def test_planner_uses_default_validation_profile_and_variant(self) -> None:
        planner_module = load_module(PLANNER_MODULE_PATH, "chaos_subject_planner_variant_default")
        subjects_module = load_subjects_module("chaos_subject_planner_variant_default_subjects")
        repo_root, manifest = create_subject_repo("variant-default", build_managed_output_subject_manifest("FixtureVariantDefaultSubject"))
        subject_id = str(manifest["subjectId"])
        validation_selection = subjects_module.resolve_validation_selection(manifest)

        try:
            plan = planner_module.build_plan(repo_root, subject_id, run_id="20260407-fixture-variant-default-001")

            self.assertIsNone(plan["request"].get("validationProfileId"))
            self.assertIsNone(plan["request"].get("validationKind"))
            self.assertIsNone(plan["request"].get("variant"))
            self.assertEqual(str(validation_selection["validationProfileId"]), plan["selection"]["validationProfileId"])
            self.assertEqual(list(validation_selection["validationKinds"]), plan["selection"]["validationKinds"])
            self.assertEqual(str(validation_selection["variant"]), plan["selection"]["variant"])
        finally:
            shutil.rmtree(repo_root, ignore_errors=True)

    def test_planner_allows_explicit_validation_profile_and_variant_override(self) -> None:
        planner_module = load_module(PLANNER_MODULE_PATH, "chaos_subject_planner_variant_override")
        subjects_module = load_subjects_module("chaos_subject_planner_variant_override_subjects")
        repo_root, manifest = create_subject_repo("variant-override", build_managed_perf_subject_manifest("FixtureVariantOverrideSubject"))
        subject_id = str(manifest["subjectId"])
        selected_matrix = find_matrix_for_goal(subjects_module, manifest, "perf.release")
        profile_id = "perf-profile"
        validation_kind = str(list(dict(manifest["validationProfiles"])[profile_id])[0])

        try:
            plan = planner_module.build_plan(
                repo_root,
                subject_id,
                goal_id="perf.release",
                matrix_id=str(selected_matrix["matrixId"]),
                validation_profile_id=profile_id,
                validation_kind=validation_kind,
                variant="SHIP",
                run_id="20260407-fixture-variant-override-001",
            )

            self.assertEqual(profile_id, plan["request"]["validationProfileId"])
            self.assertEqual(validation_kind, plan["request"]["validationKind"])
            self.assertEqual("SHIP", plan["request"]["variant"])
            self.assertEqual(profile_id, plan["selection"]["validationProfileId"])
            self.assertEqual([validation_kind], plan["selection"]["validationKinds"])
            self.assertEqual(validation_kind, plan["selection"]["validationKind"])
            self.assertEqual("SHIP", plan["selection"]["variant"])
        finally:
            shutil.rmtree(repo_root, ignore_errors=True)

    def test_planner_uses_goal_matching_perf_profile_when_goal_changes_perf_matrix(self) -> None:
        planner_module = load_module(PLANNER_MODULE_PATH, "chaos_subject_planner_goal_matching_perf_profile")
        repo_root = REPO_ROOT / "artifacts" / ".tmp-tests" / "subject-planner" / f"goal-matching-perf-profile-{uuid.uuid4().hex}"
        manifest_path = repo_root / "subjects" / "FixturePerfProfile" / "subject.manifest.json"
        manifest_path.parent.mkdir(parents=True, exist_ok=True)

        manifest = {
            "subjectId": "FixturePerfProfile",
            "displayName": "FixturePerfProfile",
            "category": "canonical",
            "defaultGoal": "perf.dev",
            "defaultMatrix": "windows-managed-perf-dev",
            "defaultValidationProfile": "perf-dev",
            "source": {
                "type": "dotnet-project",
                "path": "subjects/FixturePerfProfile/source/FixturePerfProfile.csproj",
                "entry": "FixturePerfProfile/Program::Main()",
            },
            "workloadEntry": "FixturePerfProfile/Program::RunWorkload()",
            "validationProfiles": {
                "perf-dev": ["perf"],
                "perf-release": ["perf"],
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
                    "matrixId": "windows-managed-perf-dev",
                    "pipelineId": "managed-benchmark",
                    "supportedGoals": ["perf.dev"],
                    "executionContext": {
                        "hostPlatform": "windows-x64",
                        "targetPlatform": "windows-x64",
                        "toolchainProfile": "dotnet-managed",
                        "runtimeProfile": "managed-perf-dev",
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
                },
                {
                    "matrixId": "windows-managed-perf-release",
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
                },
            ],
        }

        try:
            manifest_path.write_text(json.dumps(manifest, ensure_ascii=False, indent=2) + "\n", encoding="utf-8")

            plan = planner_module.build_plan(
                repo_root,
                "FixturePerfProfile",
                goal_id="perf.release",
                run_id="20260413-fixture-goal-matching-perf-profile-001",
            )

            self.assertEqual("perf.release", plan["selection"]["goalId"])
            self.assertEqual("windows-managed-perf-release", plan["selection"]["matrixId"])
            self.assertEqual("perf-release", plan["selection"]["validationProfileId"])
            self.assertEqual("perf", plan["selection"]["validationKind"])
            self.assertEqual("PROFILE", plan["selection"]["variant"])
        finally:
            shutil.rmtree(repo_root, ignore_errors=True)
