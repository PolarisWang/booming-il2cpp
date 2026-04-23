from tests.unit.compatibility.subject_manifest_schema_test_support import *


class TestSubjectManifestSchemaOverrides(SubjectManifestSchemaTestSupport):
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
