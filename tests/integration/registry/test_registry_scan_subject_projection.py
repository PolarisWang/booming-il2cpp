from tests.integration.registry.registry_scan_test_support import *


class TestRegistryScanSubjectProjection(RegistryScanTestSupport):
    def test_registry_scan_collects_subjects_declared_by_shared_orchestration_profiles(self) -> None:
        registry_module = load_module(REGISTRY_MODULE_PATH, "chaos_run_registry_shared_profiles")
        repo_root = make_temp_repo_root("registry-scan", "shared-profiles")
        manifest_path = repo_root / "subjects" / "FixtureSharedRegistry" / "subject.manifest.json"
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
            "subjectId": "FixtureSharedRegistry",
            "displayName": "FixtureSharedRegistry",
            "category": "canonical",
            "defaultGoal": "correctness.dev",
            "defaultMatrix": "windows-dev-output",
            "defaultValidationProfile": "proof-dev",
            "source": {
                "type": "dotnet-project",
                "path": "subjects/FixtureSharedRegistry/source/FixtureSharedRegistry.csproj",
                "entry": "FixtureSharedRegistry/Program::Main()",
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
            write_json(pipeline_profile_path, pipeline_profile)
            write_json(matrix_profile_path, matrix_profile)
            write_json(manifest_path, manifest)

            index = registry_module.scan_registry(
                repo_root,
                host_platform="windows",
                public_suite_specs=[],
            )

            subject_item = next(item for item in index.subjects if item["id"] == "subject/FixtureSharedRegistry")
            self.assertEqual(["windows"], subject_item["supportedHosts"])
            self.assertEqual("windows-dev-output", subject_item["defaultMatrixId"])
            self.assertEqual("canonical", subject_item["category"])
            self.assertEqual(
                {
                    "matrixProfile": "proof-core",
                    "pipelineProfile": "proof-core",
                    "budgetProfile": "budget-standard",
                    "baselineProfile": "baseline-standard",
                },
                subject_item["orchestration"],
            )
        finally:
            shutil.rmtree(repo_root, ignore_errors=True)

    def test_registry_scan_surfaces_retained_subject_default_entry_metadata(self) -> None:
        registry_module = load_module(REGISTRY_MODULE_PATH, "chaos_run_registry_retained_subject_defaults")
        specs_module = load_module(PUBLIC_SPECS_MODULE_PATH, "chaos_run_public_specs_retained_subject_defaults")

        index = registry_module.scan_registry(
            REPO_ROOT,
            host_platform="windows",
            public_suite_specs=specs_module.PUBLIC_TEST_SPECS,
        )

        hot_update_item = next(item for item in index.subjects if item["id"] == "subject/HotUpdateHostPack")
        mixed_execution_item = next(item for item in index.subjects if item["id"] == "subject/MixedExecutionFeaturePack")
        solution_core_item = next(item for item in index.subjects if item["id"] == "subject/SolutionCorePack")

        self.assertEqual(
            "subjects/HotUpdateHostPack/source/HotUpdateHostPack.csproj",
            hot_update_item["defaultPrimaryProjectPath"],
        )
        self.assertEqual(
            "HotUpdateHostPack/Program::Main()",
            hot_update_item["displaySourceEntry"],
        )
        self.assertNotIn("displaySubjectEntrySelection", hot_update_item)
        self.assertNotIn("displayWorkloadEntry", hot_update_item)
        self.assertNotIn("defaultSourceEntry", hot_update_item)
        self.assertNotIn("defaultSubjectEntrySelection", hot_update_item)
        self.assertNotIn("defaultWorkloadEntry", hot_update_item)

        self.assertEqual(
            "subjects/MixedExecutionFeaturePack/source/MixedExecutionFeaturePack.csproj",
            mixed_execution_item["defaultPrimaryProjectPath"],
        )
        self.assertEqual(
            "MixedExecutionFeaturePack/MixedExecutionProofEntry::Run()",
            mixed_execution_item["displaySourceEntry"],
        )
        self.assertNotIn("displaySubjectEntrySelection", mixed_execution_item)
        self.assertNotIn("displayWorkloadEntry", mixed_execution_item)
        self.assertNotIn("defaultSourceEntry", mixed_execution_item)
        self.assertNotIn("defaultSubjectEntrySelection", mixed_execution_item)
        self.assertNotIn("defaultWorkloadEntry", mixed_execution_item)

        self.assertEqual(
            "subjects/SolutionCorePack/source/Host/SolutionCorePack.csproj",
            solution_core_item["defaultPrimaryProjectPath"],
        )
        self.assertEqual(
            "CoreRuntimeFeatures/InterfaceDispatchProofEntry::Run()",
            solution_core_item["displaySourceEntry"],
        )
        self.assertNotIn("displaySubjectEntrySelection", solution_core_item)
        self.assertNotIn("displayWorkloadEntry", solution_core_item)
        self.assertNotIn("defaultSourceEntry", solution_core_item)
        self.assertNotIn("defaultSubjectEntrySelection", solution_core_item)
        self.assertNotIn("defaultWorkloadEntry", solution_core_item)

    def test_registry_scan_projects_engineering_and_declared_catalog_object_families(self) -> None:
        registry_module = load_module(REGISTRY_MODULE_PATH, "chaos_run_registry_compiled_object_families")
        specs_module = load_module(PUBLIC_SPECS_MODULE_PATH, "chaos_run_public_specs_compiled_object_families")

        run_checked(
            [
                "dotnet",
                "build",
                str(HOT_UPDATE_HOST_PACK_PROJECT_PATH),
                "-c",
                "Debug",
                "-m:1",
            ],
            cwd=REPO_ROOT,
        )

        index = registry_module.scan_registry(
            REPO_ROOT,
            host_platform="windows",
            public_suite_specs=specs_module.PUBLIC_TEST_SPECS,
        )

        object_ids = {item["id"] for item in index.flat_items}
        self.assertIn("engineering-validation/SolutionCorePack/project-graph", object_ids)
        self.assertIn("engineering-workload/SolutionCorePack/codegen", object_ids)
        self.assertIn(
            "declared-unit-test/SolutionCorePack::CoreRuntimeFeatures::CoreRuntimeFeatures.ArrayOpsProofEntry::Run()",
            object_ids,
        )
        self.assertIn(
            "declared-benchmark/SolutionCorePack::CoreRuntimeBenchmarks::CoreRuntimeBenchmarks.GenericBenchmarkEntry::RunWorkload()",
            object_ids,
        )

        declared_unit_item = next(
            item
            for item in index.flat_items
            if item["id"] == "declared-unit-test/SolutionCorePack::CoreRuntimeFeatures::CoreRuntimeFeatures.ArrayOpsProofEntry::Run()"
        )
        self.assertEqual("declared-unit-test", declared_unit_item["type"])
        self.assertEqual("SolutionCorePack", declared_unit_item["subjectId"])
        self.assertEqual(
            "run test declared-unit-test --id declared-unit-test/SolutionCorePack::CoreRuntimeFeatures::CoreRuntimeFeatures.ArrayOpsProofEntry::Run()",
            declared_unit_item["canonicalCommand"],
        )
        self.assertEqual(1, declared_unit_item["category"])
        self.assertEqual("Runtime Contract", declared_unit_item["categoryLabel"])
        self.assertIsInstance(declared_unit_item["entryIndex"], int)
        self.assertGreaterEqual(declared_unit_item["entryIndex"], 0)
        self.assertEqual(0, declared_unit_item["archetype"])
        self.assertEqual("Unspecified", declared_unit_item["archetypeLabel"])
        self.assertEqual([], declared_unit_item["hotUpdateCapabilityLabels"])

        engineering_workload_item = next(
            item
            for item in index.flat_items
            if item["id"] == "engineering-workload/SolutionCorePack/codegen"
        )
        self.assertEqual("engineering-workload", engineering_workload_item["type"])
        self.assertEqual("SolutionCorePack", engineering_workload_item["subjectId"])
        self.assertEqual(
            "run test engineering-workload --id engineering-workload/SolutionCorePack/codegen",
            engineering_workload_item["canonicalCommand"],
        )

        hot_update_benchmark_item = next(
            item
            for item in index.flat_items
            if item["id"] == "declared-benchmark/HotUpdateHostPack::HotUpdateHostPack::HotUpdateHostPack.HotUpdateLoadBenchmarkEntry::RunWorkload()"
        )
        self.assertEqual(4, hot_update_benchmark_item["category"])
        self.assertEqual("Hot Update", hot_update_benchmark_item["categoryLabel"])
        self.assertEqual(7, hot_update_benchmark_item["archetype"])
        self.assertEqual("Skeleton Patch Solution", hot_update_benchmark_item["archetypeLabel"])
        self.assertEqual(17, hot_update_benchmark_item["hotUpdateCapability"])
        self.assertEqual(
            ["Package Load", "Patch Integrity"],
            hot_update_benchmark_item["hotUpdateCapabilityLabels"],
        )
        self.assertIsInstance(hot_update_benchmark_item["entryIndex"], int)
        self.assertGreaterEqual(hot_update_benchmark_item["entryIndex"], 0)
        self.assertEqual(["managed"], hot_update_benchmark_item["supportedModes"])
        self.assertEqual(["Wall Clock"], hot_update_benchmark_item["metricLabels"])
        self.assertEqual(["Hot Update"], hot_update_benchmark_item["requirementLabels"])

        shared_contract_unit_item = next(
            item
            for item in index.flat_items
            if item["id"] == "declared-unit-test/HotUpdateHostPack::HotUpdateHostPack::HotUpdateHostPack.SharedContractProofEntry::Run()"
        )
        self.assertEqual(5, shared_contract_unit_item["category"])
        self.assertEqual("Hot Update Contract", shared_contract_unit_item["categoryLabel"])
        self.assertEqual(8, shared_contract_unit_item["archetype"])
        self.assertEqual("Full Project Hot-Update Solution", shared_contract_unit_item["archetypeLabel"])
        self.assertEqual(66, shared_contract_unit_item["hotUpdateCapability"])
        self.assertEqual(
            ["Shared Contract Binding", "Patch Callback Flow"],
            shared_contract_unit_item["hotUpdateCapabilityLabels"],
        )
        self.assertEqual(["Hot Update"], shared_contract_unit_item["requirementLabels"])
