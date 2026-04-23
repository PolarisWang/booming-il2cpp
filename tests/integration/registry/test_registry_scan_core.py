from tests.integration.registry.registry_scan_test_support import *


class TestRegistryScanCore(RegistryScanTestSupport):
    def test_registry_scan_collects_suites_modules_and_systems_without_pipeline_objects(self) -> None:
        registry_module = load_module(REGISTRY_MODULE_PATH, "chaos_run_registry")
        specs_module = load_module(PUBLIC_SPECS_MODULE_PATH, "chaos_run_public_specs")

        index = registry_module.scan_registry(
            REPO_ROOT,
            host_platform="macos",
            public_suite_specs=specs_module.PUBLIC_TEST_SPECS,
        )

        object_ids = {item["id"] for item in index.flat_items}
        self.assertIn("contract/analysis-schema", object_ids)
        self.assertIn("contract/trace-schema", object_ids)
        self.assertIn("contract/native-abi", object_ids)
        self.assertIn("contract/native-bridge", object_ids)
        self.assertIn("module/analysis/basic", object_ids)
        self.assertIn("smoke/managed-entry-basic", object_ids)
        self.assertIn("module/managed-smoke/basic", object_ids)
        self.assertIn("module/reflection/basic", object_ids)
        self.assertIn("module/interop/basic", object_ids)
        self.assertIn("module/hosted-runtime/basic", object_ids)
        self.assertIn("module/trace-export/macos", object_ids)
        self.assertIn("gate/ios-arm64-packaging", object_ids)
        self.assertIn("gate/macos-reference-desktop", object_ids)
        self.assertIn("system/hosted-runtime-smoke", object_ids)
        self.assertIn("system/ios-packaging-gate", object_ids)
        self.assertIn("gate/linux-x64-packaging", object_ids)
        self.assertIn("system/linux-packaging-gate", object_ids)
        self.assertIn("system/macos-reference-gate", object_ids)
        self.assertIn("system/runtime-baseline-macos", object_ids)
        self.assertIn("system/trace-export-macos-smoke", object_ids)
        self.assertNotIn("module/trace-export/windows", object_ids)
        self.assertNotIn("system/runtime-baseline-windows", object_ids)
        self.assertNotIn("system/trace-export-windows-smoke", object_ids)
        self.assertTrue(all(not object_id.startswith("pipeline/") for object_id in object_ids))
        self.assertNotIn("gate/android-arm64-smoke", object_ids)
        self.assertNotIn("gate/windows-reference-desktop", object_ids)
        self.assertNotIn("system/android-startup-gate", object_ids)
        self.assertNotIn("system/windows-reference-gate", object_ids)
        self.assertIn("subject/HotUpdateHostPack", object_ids)
        self.assertIn("subject/SolutionCorePack", object_ids)

    def test_registry_scan_filters_subjects_by_host_for_synthetic_repo(self) -> None:
        registry_module = load_module(REGISTRY_MODULE_PATH, "chaos_run_registry_host_filtering")
        windows_subject = build_registry_subject_manifest(
            "FixtureWindowsOnlySubject",
            default_goal="correctness.dev",
            default_matrix="windows-proof",
            executable_plan="managed-host",
            engineering_profile="managed-output",
            execution_pipelines=[
                make_pipeline(
                    "proof-runtime-output",
                    [
                        make_stage("host-input-build", "host-input-build", "host-input", scope="shared"),
                        make_stage("runtime-managed-output", "runtime-managed-output", "runtime", depends_on=["host-input-build"]),
                        make_stage("report-assemble", "report-assemble", "report", depends_on=["runtime-managed-output"]),
                    ],
                )
            ],
            environment_matrices=[
                {
                    "matrixId": "windows-proof",
                    "pipelineId": "proof-runtime-output",
                    "supportedGoals": ["correctness.dev"],
                    "executionContext": {
                        "hostPlatform": "windows-x64",
                        "targetPlatform": "windows-x64",
                    },
                }
            ],
        )
        macos_subject = build_registry_subject_manifest(
            "FixtureMacosOnlySubject",
            default_goal="correctness.dev",
            default_matrix="macos-proof",
            executable_plan="managed-host",
            engineering_profile="managed-output",
            execution_pipelines=[
                make_pipeline(
                    "proof-runtime-output",
                    [
                        make_stage("host-input-build", "host-input-build", "host-input", scope="shared"),
                        make_stage("runtime-managed-output", "runtime-managed-output", "runtime", depends_on=["host-input-build"]),
                        make_stage("report-assemble", "report-assemble", "report", depends_on=["runtime-managed-output"]),
                    ],
                )
            ],
            environment_matrices=[
                {
                    "matrixId": "macos-proof",
                    "pipelineId": "proof-runtime-output",
                    "supportedGoals": ["correctness.dev"],
                    "executionContext": {
                        "hostPlatform": "macos-arm64",
                        "targetPlatform": "macos-arm64",
                    },
                }
            ],
        )

        repo_root = create_registry_repo(
            "host-filtering",
            subjects=[windows_subject, macos_subject],
        )

        try:
            windows_index = registry_module.scan_registry(
                repo_root,
                host_platform="windows",
                public_suite_specs=[],
            )
            macos_index = registry_module.scan_registry(
                repo_root,
                host_platform="macos",
                public_suite_specs=[],
            )

            self.assertEqual(["subject/FixtureWindowsOnlySubject"], [item["id"] for item in windows_index.subjects])
            self.assertEqual(["windows"], windows_index.subjects[0]["supportedHosts"])
            self.assertEqual(["subject/FixtureMacosOnlySubject"], [item["id"] for item in macos_index.subjects])
            self.assertEqual(["macos"], macos_index.subjects[0]["supportedHosts"])
        finally:
            shutil.rmtree(repo_root, ignore_errors=True)

    def test_registry_scan_collects_windows_android_gate_objects(self) -> None:
        registry_module = load_module(REGISTRY_MODULE_PATH, "chaos_run_registry_windows_android_gate")
        specs_module = load_module(PUBLIC_SPECS_MODULE_PATH, "chaos_run_public_specs_windows_android_gate")
        canonical_subject = build_registry_subject_manifest(
            "FixtureManagedOutputSubject",
            default_goal="correctness.dev",
            default_matrix="windows-output",
            source_model="dotnet-solution",
            dependency_model="project-reference",
            executable_plan="managed-host",
            engineering_profile="managed-output",
            availability={"windows-x64": "ready"},
            execution_pipelines=[
                make_pipeline(
                    "managed-output",
                    [
                        make_stage("host-input-build", "host-input-build", "host-input", scope="shared"),
                        make_stage("runtime-managed-output", "runtime-managed-output", "runtime", depends_on=["host-input-build"]),
                        make_stage("report-assemble", "report-assemble", "report", depends_on=["runtime-managed-output"]),
                    ],
                )
            ],
            environment_matrices=[
                {
                    "matrixId": "windows-output",
                    "pipelineId": "managed-output",
                    "supportedGoals": ["correctness.dev"],
                    "executionContext": {
                        "hostPlatform": "windows-x64",
                        "targetPlatform": "windows-x64",
                    },
                }
            ],
            source_path="subjects/FixtureManagedOutputSubject/source/FixtureManagedOutputSubject.sln",
            primary_project_path="subjects/FixtureManagedOutputSubject/source/App/App.csproj",
            entry="FixtureManagedOutputSubject/Program::Main()",
        )
        native_proof_subject = build_registry_subject_manifest(
            "FixtureNativeProofSubject",
            default_goal="correctness.dev",
            default_matrix="windows-native-proof",
            source_model="dotnet-project-set",
            dependency_model="project-reference",
            executable_plan="generated-native",
            engineering_profile="native-executable",
            availability={"windows-x64": "ready"},
            execution_pipelines=[
                make_pipeline(
                    "native-proof",
                    [
                        make_stage("analysis-frontend", "analysis-frontend", "analysis", scope="shared"),
                        make_stage("generated-native-proof", "generated-native-proof", "generated", scope="shared", depends_on=["analysis-frontend"]),
                        make_stage("build-target", "build-target", "build", depends_on=["generated-native-proof"]),
                        make_stage("runtime-observe", "runtime-observe", "runtime", depends_on=["build-target"]),
                        make_stage("report-assemble", "report-assemble", "report", depends_on=["runtime-observe"]),
                    ],
                )
            ],
            environment_matrices=[
                {
                    "matrixId": "windows-native-proof",
                    "pipelineId": "native-proof",
                    "supportedGoals": ["correctness.dev"],
                    "executionContext": {
                        "hostPlatform": "windows-x64",
                        "targetPlatform": "windows-x64",
                    },
                }
            ],
        )
        benchmark_subject = build_registry_subject_manifest(
            "FixtureInterpreterPerfSubject",
            default_goal="perf.release",
            default_matrix="windows-interpreter-perf",
            default_validation_profile="perf-profile",
            validation_profiles={"perf-profile": ["perf"]},
            validation={
                "perf": {
                    "kind": "perf",
                    "defaultVariant": "PROFILE",
                    "driver": "native-runtime-perf",
                }
            },
            source_model="dotnet-project-set",
            dependency_model="project-reference",
            executable_plan="generated-native",
            engineering_profile="native-executable",
            availability={"windows-x64": "ready"},
            execution_pipelines=[
                make_pipeline(
                    "interpreter-perf",
                    [
                        make_stage("analysis-frontend", "analysis-frontend", "analysis", scope="shared"),
                        make_stage("generated-native-proof", "generated-native-proof", "generated", scope="shared", depends_on=["analysis-frontend"]),
                        make_stage("interpreter-runtime-perf", "interpreter-runtime-perf", "runtime", depends_on=["generated-native-proof"]),
                        make_stage("report-assemble", "report-assemble", "report", depends_on=["interpreter-runtime-perf"]),
                    ],
                )
            ],
            environment_matrices=[
                {
                    "matrixId": "windows-interpreter-perf",
                    "pipelineId": "interpreter-perf",
                    "supportedGoals": ["perf.release"],
                    "executionContext": {
                        "hostPlatform": "windows-x64",
                        "targetPlatform": "windows-x64",
                    },
                }
            ],
            workload_entry="FixtureInterpreterPerfSubject/Benchmarks::RunWorkload()",
        )
        repo_root = create_registry_repo(
            "windows-android-gates",
            subjects=[canonical_subject, native_proof_subject, benchmark_subject],
            copy_registry_fixtures=True,
        )

        try:
            index = registry_module.scan_registry(
                repo_root,
                host_platform="windows",
                public_suite_specs=specs_module.PUBLIC_TEST_SPECS,
            )

            object_ids = {item["id"] for item in index.flat_items}
            self.assertIn("gate/android-arm64-smoke", object_ids)
            self.assertIn("gate/windows-reference-desktop", object_ids)
            self.assertIn("system/android-startup-gate", object_ids)
            self.assertIn("system/windows-reference-gate", object_ids)
            self.assertEqual(
                {
                    "subject/FixtureManagedOutputSubject",
                    "subject/FixtureNativeProofSubject",
                    "subject/FixtureInterpreterPerfSubject",
                },
                {item["id"] for item in index.subjects},
            )
            self.assertNotIn("system/linux-packaging-gate-macos-only", object_ids)

            canonical_item = next(item for item in index.flat_items if item["id"] == "subject/FixtureManagedOutputSubject")
            self.assertEqual("canonical", canonical_item["category"])
            self.assertEqual("correctness.dev", canonical_item["defaultGoalId"])
            self.assertEqual("windows-output", canonical_item["defaultMatrixId"])
            self.assertEqual("dotnet-solution", canonical_item["sourceModel"])
            self.assertEqual("managed-output", canonical_item["engineeringProfile"])

            subject_item = next(item for item in index.flat_items if item["id"] == "subject/FixtureNativeProofSubject")
            self.assertEqual("subject", subject_item["type"])
            self.assertEqual(
                "run test subject --id subject/FixtureNativeProofSubject",
                subject_item["canonicalCommand"],
            )
            self.assertEqual(["windows"], sorted(subject_item["supportedHosts"]))

            benchmark_item = next(item for item in index.flat_items if item["id"] == "subject/FixtureInterpreterPerfSubject")
            self.assertEqual("canonical", benchmark_item["category"])
            self.assertEqual("perf.release", benchmark_item["defaultGoalId"])
            self.assertEqual("windows-interpreter-perf", benchmark_item["defaultMatrixId"])
            self.assertEqual("generated-native", benchmark_item["executablePlan"])
            self.assertEqual({"windows-x64": "ready"}, benchmark_item["availability"])
            self.assertEqual(
                "run test subject --id subject/FixtureInterpreterPerfSubject",
                benchmark_item["canonicalCommand"],
            )
            self.assertEqual(["perf.release"], sorted(benchmark_item["goalIds"]))

            analysis_module_item = next(item for item in index.flat_items if item["id"] == "module/analysis/basic")
            runtime_baseline_item = next(item for item in index.flat_items if item["id"] == "system/windows-reference-gate")
            self.assertIn(
                "tests/fixtures/registry/modules/analysis/basic/verification.manifest.json",
                analysis_module_item["manifestPath"].replace("\\", "/"),
            )
            self.assertIn(
                "tests/fixtures/registry/systems/windows-reference-gate/scenario.manifest.json",
                runtime_baseline_item["manifestPath"].replace("\\", "/"),
            )
            self.assertTrue(all(not str(item["id"]).startswith("pipeline/") for item in index.flat_items))
        finally:
            shutil.rmtree(repo_root, ignore_errors=True)
