from tests.unit.execution.subject_workers_test_support import *


class TestSubjectWorkersBuildReferenceWindows(SubjectWorkersTestSupport):
    def test_perf_defaults_bias_toward_short_benchmark_batches(self) -> None:
        workers_module = load_module(SUBJECT_WORKERS_MODULE_PATH, "chaos_subject_workers_perf_defaults")

        self.assertEqual(3, workers_module._perf_sample_count("managed-perf-release"))
        self.assertEqual(1, workers_module._perf_sample_count("native-perf-profile"))
        self.assertEqual(1, workers_module._perf_sample_count("managed-perf-dev"))
        self.assertEqual(1000, workers_module._perf_harness_iterations("managed-perf-release"))
        self.assertEqual(100, workers_module._perf_harness_iterations("interpreter-perf-dev"))

    def test_windows_build_target_uses_cmake_reference_route_and_records_build_strategy(self) -> None:
        workers_module = load_module(SUBJECT_WORKERS_MODULE_PATH, "chaos_subject_workers_windows_build")
        subject_id = "FixtureNativeSubject"
        run_id = "fixture-run-native-build-001"
        matrix_id = "windows-reference-check"
        expected_cmake_dir = self._make_non_repo_path("cmake-builds", "subject-reference-1234")
        expected_cmake_path = self._make_non_repo_path("cmake", "bin", "cmake.exe")
        instance_spec = f"{self._make_non_repo_path('visual-studio', '18', 'Professional')},version=18.4.11626.88"
        expected_env = {
            "Path": r"C:\VS\bin;C:\Windows\System32",
            "INCLUDE": r"C:\VS\include",
            "LIB": r"C:\VS\lib",
        }

        request = {
            "selection": {
                "subjectId": subject_id,
                "matrixId": matrix_id,
                "variant": "CHECK",
                "executionContext": {
                    "targetPlatform": "windows-x64",
                    "toolchainProfile": "msvc-reference",
                },
            },
            "upstream": {
                "generated": {
                    "manifestPath": subject_run_path(subject_id, run_id, "analysis", "generated", "generated.manifest.json"),
                }
            },
            "paths": {
                "bucketRoot": subject_run_path(subject_id, run_id, "matrices", matrix_id, "build"),
                "manifestPath": subject_run_path(subject_id, run_id, "matrices", matrix_id, "build", "build.manifest.json"),
                "reportPaths": [],
            },
        }

        repo_root = self._make_repo_root("windows-build")
        try:
            generated_source_path = (
                repo_root
                / "artifacts"
                / "subjects"
                / subject_id
                / "runs"
                / run_id
                / "analysis"
                / "generated"
                / "generated"
                / "native-reference.generated.cpp"
            )
            generated_source_path.parent.mkdir(parents=True, exist_ok=True)
            generated_source_path.write_text("// fixture\n", encoding="utf-8")
            expected_cmake_source_root = (
                repo_root
                / "artifacts"
                / "subjects"
                / subject_id
                / "runs"
                / run_id
                / "matrices"
                / matrix_id
                / "build"
                / "cmake-src"
            )

            with patch.object(workers_module.tooling_module, "cmake_environment", return_value=(str(expected_cmake_path), {})):
                with patch.object(workers_module.tooling_module, "windows_developer_environment", return_value=expected_env):
                    with patch.object(workers_module.tooling_module, "detect_visual_studio_generator", return_value="Visual Studio 18 2026"):
                        with patch.object(workers_module.tooling_module, "detect_visual_studio_instance_spec", return_value=instance_spec):
                            with patch.object(workers_module.tooling_module, "allocate_cmake_binary_dir", return_value=expected_cmake_dir):
                                with patch.object(workers_module, "_workspace_manifest_is_stale", return_value=False):
                                    with patch.object(workers_module, "_run_checked") as run_checked_mock:
                                        result = workers_module.run_build_target(repo_root=repo_root, request=request)

            self.assertEqual("ok", result["status"])
            self.assertEqual(
                [
                    str(expected_cmake_path),
                    "-S",
                    str(expected_cmake_source_root),
                    "-B",
                    str(expected_cmake_dir),
                    "-G",
                    "Visual Studio 18 2026",
                    f"-DCHAOS_SUBJECT_REPO_ROOT={repo_root}",
                    "-DCHAOS_SUBJECT_VARIANT=CHECK",
                    f"-DCHAOS_SUBJECT_BUILD_OUT_ROOT={repo_root / 'artifacts' / 'subjects' / subject_id / 'runs' / run_id / 'matrices' / matrix_id / 'build' / 'out'}",
                    f"-DCHAOS_SUBJECT_RUNTIME_ROOT={repo_root / 'artifacts' / 'subjects' / subject_id / 'runs' / run_id / 'matrices' / matrix_id / 'runtime'}",
                    f"-DCMAKE_GENERATOR_INSTANCE={instance_spec}",
                ],
                run_checked_mock.call_args_list[0].args[0],
            )
            self.assertEqual(
                [
                    str(expected_cmake_path),
                    "--build",
                    str(expected_cmake_dir),
                    "--config",
                    "Release",
                    "--target",
                    WINDOWS_REFERENCE_BUILD_TARGET,
                ],
                run_checked_mock.call_args_list[1].args[0],
            )
            self.assertEqual(expected_env, run_checked_mock.call_args_list[0].kwargs["env"])
            self.assertEqual(expected_env, run_checked_mock.call_args_list[1].kwargs["env"])

            manifest = json.loads((repo_root / request["paths"]["manifestPath"]).read_text(encoding="utf-8"))
            self.assertEqual("CHECK", manifest["variant"])
            self.assertEqual(
                {
                    "codegen": ["CHAOS_VARIANT_CHECK", "CHAOS_VARIANT_NAME=CHECK"],
                    "native": ["CHAOS_VARIANT_CHECK", "CHAOS_VARIANT_NAME=CHECK"],
                },
                manifest["variantMacros"],
            )
            self.assertEqual(WINDOWS_REFERENCE_CMAKE_BUILD_STRATEGY, manifest["buildStrategy"])
            self.assertEqual("native-reference", manifest["buildKind"])
            self.assertEqual(
                subject_run_path(subject_id, run_id, "matrices", matrix_id, "build", "out"),
                manifest["binaryRoot"],
            )
            self.assertEqual(
                [subject_run_path(subject_id, run_id, "matrices", matrix_id, "build", "out", f"{WINDOWS_REFERENCE_BUILD_TARGET}.exe")],
                manifest["outputs"],
            )
            self.assertEqual(
                subject_run_path(subject_id, run_id, "matrices", matrix_id, "build", "cmake-src", "proof", "main.cpp"),
                manifest["hostSourcePath"],
            )
            self.assertEqual(expected_cmake_dir.as_posix(), manifest["cmakeBinaryDir"])
        finally:
            shutil.rmtree(repo_root, ignore_errors=True)

    def test_windows_build_target_uses_workspace_reference_proof_host_for_assembly_bound_dispatch(self) -> None:
        workers_module = load_module(SUBJECT_WORKERS_MODULE_PATH, "chaos_subject_workers_workspace_reference_build")
        subject_id = "FixtureAssemblyBoundSubject"
        run_id = "fixture-run-native-reference-workspace-build-001"
        matrix_id = "windows-foundation-dll-translation-native-proof"
        expected_cmake_path = self._make_non_repo_path("cmake", "bin", "cmake.exe")
        expected_configure_root = posix_path("verification", "workspaces", "subjects", subject_id, "native", matrix_id)
        instance_spec = f"{self._make_non_repo_path('visual-studio', '18', 'Professional')},version=18.4.11626.88"
        expected_env = {
            "Path": r"C:\VS\bin;C:\Windows\System32",
            "INCLUDE": r"C:\VS\include",
            "LIB": r"C:\VS\lib",
        }
        first_subject_id = f"{subject_id}/Program+<>c::.cctor:System.Void()"
        preferred_subject_id = f"{subject_id}/Program::Main:System.Int32()"
        generated_source_path_text = subject_run_path(
            subject_id,
            run_id,
            "analysis",
            "generated",
            "generated",
            "native-reference.generated.cpp",
        )

        request = {
            "selection": {
                "subjectId": subject_id,
                "matrixId": matrix_id,
                "variant": "CHECK",
                "executionContext": {
                    "hostPlatform": "windows-x64",
                    "targetPlatform": "windows-x64",
                    "toolchainProfile": "msvc-reference",
                },
            },
            "upstream": {
                "generated": {
                    "manifestPath": subject_run_path(subject_id, run_id, "analysis", "generated", "generated.manifest.json"),
                }
            },
            "paths": {
                "bucketRoot": subject_run_path(subject_id, run_id, "matrices", matrix_id, "build"),
                "manifestPath": subject_run_path(subject_id, run_id, "matrices", matrix_id, "build", "build.manifest.json"),
                "reportPaths": [],
            },
        }

        repo_root = self._make_repo_root("windows-reference-workspace-build")
        try:
            generated_source_path = repo_root / generated_source_path_text
            generated_source_path.parent.mkdir(parents=True, exist_ok=True)
            generated_source_path.write_text("// generated native-reference fixture\n", encoding="utf-8")
            generated_page_path_text = subject_run_path(
                subject_id,
                run_id,
                "analysis",
                "generated",
                "generated",
                "runtime",
                "native-reference.runtime-skeleton.page-0001.cpp",
            )
            generated_page_path = repo_root / generated_page_path_text
            generated_page_path.parent.mkdir(parents=True, exist_ok=True)
            generated_page_path.write_text("// generated runtime page fixture\n", encoding="utf-8")

            generated_manifest_path = repo_root / request["upstream"]["generated"]["manifestPath"]
            generated_manifest_path.parent.mkdir(parents=True, exist_ok=True)
            generated_manifest_path.write_text(
                json.dumps(
                    {
                        "generatedSourcePath": generated_source_path_text,
                        "generatedSourcePaths": [
                            generated_source_path_text,
                            generated_page_path_text,
                        ],
                    }
                ),
                encoding="utf-8",
            )
            (generated_manifest_path.parent / "native-reference.manifest.json").write_text(
                json.dumps(
                    {
                        "runtimeExecutionKind": "assembly-bound-native-reference-skeleton",
                        "preferredAssemblyDispatchSubjectId": preferred_subject_id,
                        "translationUnitPages": [
                            {
                                "pageNumber": 1,
                                "methodCount": 1,
                                "firstMethodSubjectId": first_subject_id,
                                "lastMethodSubjectId": first_subject_id,
                            }
                        ],
                    }
                ),
                encoding="utf-8",
            )

            workspace_manifest_path = repo_root / "verification" / "workspaces" / "subjects" / subject_id / "workspace.manifest.json"
            workspace_manifest_path.parent.mkdir(parents=True, exist_ok=True)
            workspace_manifest_path.write_text(
                json.dumps(
                    {
                        "workspaceVersion": 2,
                        "subjectId": subject_id,
                        "nativeTestProjects": [
                            {
                                "projectId": f"native-test/{subject_id}/{matrix_id}/proof-host",
                                "matrixId": matrix_id,
                                "projectPath": posix_path(
                                    "verification", "workspaces",
                                    "subjects",
                                    subject_id,
                                    "native",
                                    matrix_id,
                                    "proof",
                                    "chaos_subject_reference_proof.vcxproj",
                                ),
                                "configureRoot": expected_configure_root,
                                "hostKind": "proof-host",
                            }
                        ],
                    }
                ),
                encoding="utf-8",
            )

            expected_cmake_dir = repo_root / expected_configure_root
            expected_cmake_source_root = repo_root / "verification" / "workspaces" / "subjects" / subject_id / "native-source" / matrix_id
            with patch.object(workers_module.tooling_module, "cmake_environment", return_value=(str(expected_cmake_path), {})):
                with patch.object(workers_module.tooling_module, "windows_developer_environment", return_value=expected_env):
                    with patch.object(workers_module.tooling_module, "detect_visual_studio_generator", return_value="Visual Studio 18 2026"):
                        with patch.object(workers_module.tooling_module, "detect_visual_studio_instance_spec", return_value=instance_spec):
                            with patch.object(workers_module.tooling_module, "allocate_cmake_binary_dir") as allocate_cmake_binary_dir_mock:
                                with patch.object(workers_module, "_workspace_manifest_is_stale", return_value=False):
                                    with patch.object(workers_module, "_run_checked") as run_checked_mock:
                                        result = workers_module.run_build_target(repo_root=repo_root, request=request)

            self.assertEqual("ok", result["status"])
            self.assertEqual(
                [
                    str(expected_cmake_path),
                    "-S",
                    str(expected_cmake_source_root),
                    "-B",
                    str(expected_cmake_dir),
                    "-G",
                    "Visual Studio 18 2026",
                    f"-DCHAOS_SUBJECT_REPO_ROOT={repo_root}",
                    "-DCHAOS_SUBJECT_VARIANT=CHECK",
                    f"-DCHAOS_SUBJECT_BUILD_OUT_ROOT={repo_root / 'artifacts' / 'subjects' / subject_id / 'runs' / run_id / 'matrices' / matrix_id / 'build' / 'out'}",
                    f"-DCHAOS_SUBJECT_RUNTIME_ROOT={repo_root / 'artifacts' / 'subjects' / subject_id / 'runs' / run_id / 'matrices' / matrix_id / 'runtime'}",
                    f"-DCMAKE_GENERATOR_INSTANCE={instance_spec}",
                ],
                run_checked_mock.call_args_list[0].args[0],
            )
            self.assertEqual(
                [
                    str(expected_cmake_path),
                    "--build",
                    str(expected_cmake_dir),
                    "--config",
                    "Release",
                    "--target",
                    WINDOWS_REFERENCE_BUILD_TARGET,
                ],
                run_checked_mock.call_args_list[1].args[0],
            )
            allocate_cmake_binary_dir_mock.assert_not_called()

            proof_host_text = (expected_cmake_source_root / "proof" / "main.cpp").read_text(encoding="utf-8")
            self.assertIn("NativeReferenceAssemblyDispatchRequestV0 request = {};", proof_host_text)
            self.assertIn("RunNativeReferenceAssembly(", proof_host_text)
            self.assertIn(preferred_subject_id, proof_host_text)
            generated_cmake_text = (expected_cmake_source_root / "generated" / "CMakeLists.txt").read_text(encoding="utf-8")
            self.assertIn(generated_source_path.as_posix(), generated_cmake_text)
            self.assertIn(generated_page_path.as_posix(), generated_cmake_text)

            manifest = json.loads((repo_root / request["paths"]["manifestPath"]).read_text(encoding="utf-8"))
            self.assertEqual(WINDOWS_REFERENCE_CMAKE_BUILD_STRATEGY, manifest["buildStrategy"])
            self.assertEqual("native-reference", manifest["buildKind"])
            self.assertEqual(
                [
                    generated_source_path_text,
                    generated_page_path_text,
                ],
                manifest["generatedSourcePaths"],
            )
            self.assertEqual(
                posix_path("verification", "workspaces", "subjects", subject_id, "native-source", matrix_id, "proof", "main.cpp"),
                manifest["hostSourcePath"],
            )
            self.assertEqual(expected_configure_root, manifest["cmakeBinaryDir"])
        finally:
            shutil.rmtree(repo_root, ignore_errors=True)


