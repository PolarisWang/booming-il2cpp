from tests.unit.planning.subject_planner_test_support import *


class TestSubjectPlannerFingerprintVariants(SubjectPlannerTestSupport):
    def test_stage_fingerprint_changes_when_variant_changes(self) -> None:
        planner_module = load_module(PLANNER_MODULE_PATH, "chaos_subject_planner_variant_fingerprint")
        subjects_module = load_subjects_module("chaos_subject_planner_variant_fingerprint_subjects")
        repo_root, manifest = create_subject_repo("variant-fingerprint", build_managed_perf_subject_manifest("FixtureVariantFingerprintSubject"))
        subject_id = str(manifest["subjectId"])
        selected_matrix = find_matrix_for_goal(subjects_module, manifest, "perf.release")
        profile_id = "perf-profile"

        try:
            default_plan = planner_module.build_plan(
                repo_root,
                subject_id,
                goal_id="perf.release",
                matrix_id=str(selected_matrix["matrixId"]),
                validation_profile_id=profile_id,
                run_id="20260407-fixture-variant-fingerprint-001",
            )
            ship_plan = planner_module.build_plan(
                repo_root,
                subject_id,
                goal_id="perf.release",
                matrix_id=str(selected_matrix["matrixId"]),
                validation_profile_id=profile_id,
                variant="SHIP",
                run_id="20260407-fixture-variant-fingerprint-001",
            )

            self.assertNotEqual(default_plan["selection"]["variant"], ship_plan["selection"]["variant"])
            self.assertNotEqual(default_plan["stagePlan"][0]["fingerprint"], ship_plan["stagePlan"][0]["fingerprint"])
        finally:
            shutil.rmtree(repo_root, ignore_errors=True)

    def test_stage_fingerprint_changes_when_workload_entry_changes(self) -> None:
        planner_module = load_module(PLANNER_MODULE_PATH, "chaos_subject_planner_workload_fingerprint")
        repo_root = REPO_ROOT / "artifacts" / ".tmp-tests" / "subject-planner" / f"workload-fingerprint-{uuid.uuid4().hex}"
        subject_id = "FixtureBench"

        manifest = {
            "subjectId": "FixtureBench",
            "displayName": "FixtureBench",
            "category": "benchmark",
            "defaultGoal": "perf.release",
            "defaultMatrix": "windows-managed-perf",
            "defaultValidationProfile": "perf-profile",
            "source": {
                "type": "dotnet-project",
                "path": "verification/catalog/owners/FixtureBench/support/host/FixtureBench.csproj",
                "entry": "FixtureBench/Program::Main()",
            },
            "workloadEntry": "FixtureBench/Program::RunWorkload()",
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
            manifest_path = write_owner_manifest(repo_root, subject_id, manifest)
            plan = planner_module.build_plan(
                repo_root,
                subject_id,
                goal_id="perf.release",
                matrix_id="windows-managed-perf",
                run_id="fixture-workload-fingerprint",
            )

            manifest["workloadEntry"] = "FixtureBench/Program::RunHotPath()"
            manifest_path.write_text(json.dumps(manifest, ensure_ascii=False, indent=2) + "\n", encoding="utf-8")
            updated_plan = planner_module.build_plan(
                repo_root,
                subject_id,
                goal_id="perf.release",
                matrix_id="windows-managed-perf",
                run_id="fixture-workload-fingerprint",
            )

            self.assertNotEqual(plan["selection"]["workloadEntry"], updated_plan["selection"]["workloadEntry"])
            self.assertNotEqual(plan["stagePlan"][0]["fingerprint"], updated_plan["stagePlan"][0]["fingerprint"])
        finally:
            shutil.rmtree(repo_root, ignore_errors=True)
