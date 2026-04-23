from tests.unit.planning.subject_planner_test_support import *


class TestSubjectPlannerFingerprintProfiles(SubjectPlannerTestSupport):
    def test_planner_expands_shared_orchestration_profiles_before_building_plan(self) -> None:
        planner_module = load_module(PLANNER_MODULE_PATH, "chaos_subject_planner_shared_profiles")
        repo_root = REPO_ROOT / "artifacts" / ".tmp-tests" / "subject-planner" / f"shared-profiles-{uuid.uuid4().hex}"
        manifest_path = repo_root / "subjects" / "FixtureSharedPlanner" / "subject.manifest.json"
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
            "subjectId": "FixtureSharedPlanner",
            "displayName": "FixtureSharedPlanner",
            "category": "canonical",
            "defaultGoal": "correctness.dev",
            "defaultMatrix": "windows-dev-output",
            "defaultValidationProfile": "proof-dev",
            "source": {
                "type": "dotnet-project",
                "path": "subjects/FixtureSharedPlanner/source/FixtureSharedPlanner.csproj",
                "entry": "FixtureSharedPlanner/Program::Main()",
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

            plan = planner_module.build_plan(
                repo_root,
                "FixtureSharedPlanner",
                run_id="fixture-shared-planner",
            )

            self.assertEqual("FixtureSharedPlanner", plan["selection"]["subjectId"])
            self.assertEqual("windows-dev-output", plan["selection"]["matrixId"])
            self.assertEqual("proof-runtime-output", plan["selection"]["pipelineId"])
            self.assertEqual(
                {
                    "matrixProfile": "proof-core",
                    "pipelineProfile": "proof-core",
                    "budgetProfile": "budget-standard",
                    "baselineProfile": "baseline-standard",
                },
                plan["selection"]["orchestration"],
            )
            self.assertEqual(
                [
                    "source-resolve",
                    "host-input-build",
                    "analysis-frontend",
                    "generated-native-proof",
                    "build-target",
                    "runtime-observe",
                    "report-assemble",
                ],
                [stage["stageId"] for stage in plan["stagePlan"]],
            )
        finally:
            shutil.rmtree(repo_root, ignore_errors=True)
