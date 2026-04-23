from tests.unit.planning.subject_planner_test_support import *


class TestSubjectPlannerFingerprintEntrySelection(SubjectPlannerTestSupport):
    def test_stage_fingerprint_changes_when_declared_entry_selection_changes(self) -> None:
        planner_module = load_module(PLANNER_MODULE_PATH, "chaos_subject_planner_declared_entry_fingerprint")
        repo_root = REPO_ROOT / "artifacts" / ".tmp-tests" / "subject-planner" / f"declared-entry-selection-{uuid.uuid4().hex}"
        manifest_path = repo_root / "subjects" / "FixtureDeclaredEntrySelection" / "subject.manifest.json"
        manifest_path.parent.mkdir(parents=True, exist_ok=True)

        manifest = {
            "subjectId": "FixtureDeclaredEntrySelection",
            "displayName": "FixtureDeclaredEntrySelection",
            "category": "benchmark",
            "defaultGoal": "perf.release",
            "defaultMatrix": "windows-managed-perf",
            "defaultValidationProfile": "perf-profile",
            "source": {
                "type": "dotnet-project",
                "path": "subjects/FixtureDeclaredEntrySelection/source/FixtureDeclaredEntrySelection.csproj",
                "entry": "FixtureDeclaredEntrySelection/Program::Main()",
            },
            "workloadEntry": "FixtureDeclaredEntrySelection/Program::RunWorkload()",
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

            first_plan = planner_module.build_plan(
                repo_root,
                "FixtureDeclaredEntrySelection",
                goal_id="perf.release",
                matrix_id="windows-managed-perf",
                run_id="fixture-declared-entry-selection",
                entry_selection={
                    "family": "declared-benchmark",
                    "stableId": "bench/a",
                    "alias": "bench-a",
                    "entryIndex": 1,
                },
            )
            second_plan = planner_module.build_plan(
                repo_root,
                "FixtureDeclaredEntrySelection",
                goal_id="perf.release",
                matrix_id="windows-managed-perf",
                run_id="fixture-declared-entry-selection",
                entry_selection={
                    "family": "declared-benchmark",
                    "stableId": "bench/b",
                    "alias": "bench-b",
                    "entryIndex": 2,
                },
            )

            first_runtime_stage = next(stage for stage in first_plan["stagePlan"] if stage["kind"] == "runtime-perf-collect")
            second_runtime_stage = next(stage for stage in second_plan["stagePlan"] if stage["kind"] == "runtime-perf-collect")

            self.assertEqual(
                {
                    "family": "declared-benchmark",
                    "stableId": "bench/a",
                    "alias": "bench-a",
                    "entryIndex": 1,
                },
                first_plan["selection"]["entrySelection"],
            )
            self.assertEqual(
                {
                    "family": "declared-benchmark",
                    "stableId": "bench/b",
                    "alias": "bench-b",
                    "entryIndex": 2,
                },
                second_plan["selection"]["entrySelection"],
            )
            self.assertNotEqual(first_runtime_stage["fingerprint"], second_runtime_stage["fingerprint"])
        finally:
            shutil.rmtree(repo_root, ignore_errors=True)

    def test_planner_runtime_stage_fingerprint_changes_when_subject_entry_selection_changes(self) -> None:
        planner_module = load_module(PLANNER_MODULE_PATH, "chaos_subject_planner_subject_entry_fingerprint")
        repo_root = REPO_ROOT / "artifacts" / ".tmp-tests" / "subject-planner" / f"subject-entry-selection-{uuid.uuid4().hex}"
        manifest_path = repo_root / "subjects" / "FixtureSubjectEntrySelection" / "subject.manifest.json"
        manifest_path.parent.mkdir(parents=True, exist_ok=True)

        manifest = {
            "subjectId": "FixtureSubjectEntrySelection",
            "displayName": "FixtureSubjectEntrySelection",
            "category": "canonical",
            "defaultGoal": "correctness.dev",
            "defaultMatrix": "windows-managed-output",
            "defaultValidationProfile": "managed-output",
            "source": {
                "type": "dotnet-project",
                "path": "subjects/FixtureSubjectEntrySelection/source/FixtureSubjectEntrySelection.csproj",
                "entry": "FixtureSubjectEntrySelection/Program::Main()",
            },
            "validationProfiles": {
                "managed-output": ["proof"],
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
                    "source": {
                        "entrySelection": {
                            "entryKind": 1,
                            "entrySlice": 1,
                        },
                    },
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
                },
            ],
        }

        try:
            manifest_path.write_text(json.dumps(manifest, ensure_ascii=False, indent=2) + "\n", encoding="utf-8")

            first_plan = planner_module.build_plan(
                repo_root,
                "FixtureSubjectEntrySelection",
                matrix_id="windows-managed-output",
                run_id="fixture-subject-entry-selection-001",
            )
            manifest["environmentMatrices"][0]["source"]["entrySelection"]["entrySlice"] = 2
            manifest_path.write_text(json.dumps(manifest, ensure_ascii=False, indent=2) + "\n", encoding="utf-8")
            second_plan = planner_module.build_plan(
                repo_root,
                "FixtureSubjectEntrySelection",
                matrix_id="windows-managed-output",
                run_id="fixture-subject-entry-selection-001",
            )

            first_runtime_stage = next(stage for stage in first_plan["stagePlan"] if stage["kind"] == "runtime-managed-output")
            second_runtime_stage = next(stage for stage in second_plan["stagePlan"] if stage["kind"] == "runtime-managed-output")

            self.assertEqual(
                {
                    "entryKind": 1,
                    "entrySlice": 1,
                },
                first_plan["selection"]["source"]["entrySelection"],
            )
            self.assertEqual(
                {
                    "entryKind": 1,
                    "entrySlice": 2,
                },
                second_plan["selection"]["source"]["entrySelection"],
            )
            self.assertNotEqual(first_runtime_stage["fingerprint"], second_runtime_stage["fingerprint"])
        finally:
            shutil.rmtree(repo_root, ignore_errors=True)
