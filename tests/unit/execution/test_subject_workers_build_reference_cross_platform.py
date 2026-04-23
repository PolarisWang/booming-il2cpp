from tests.unit.execution.subject_workers_test_support import *


class TestSubjectWorkersBuildReferenceCrossPlatform(SubjectWorkersTestSupport):
    def test_windows_validate_only_build_uses_visual_studio_generator_and_records_non_repo_cmake_binary_dir(self) -> None:
        workers_module = load_module(SUBJECT_WORKERS_MODULE_PATH, "chaos_subject_workers_windows_validate_only")
        subject_id = "FixtureValidateOnlySubject"
        run_id = "fixture-run-validate-only-001"
        matrix_id = "windows-android-check"
        expected_cmake_dir = self._make_non_repo_path("cmake-builds", "platform-gate-1234")
        instance_spec = f"{self._make_non_repo_path('visual-studio', '18', 'Professional')},version=18.4.11626.88"

        request = {
            "selection": {
                "subjectId": subject_id,
                "matrixId": matrix_id,
                "variant": "CHECK",
                "executionContext": {
                    "hostPlatform": "windows-x64",
                    "targetPlatform": "android-arm64",
                    "toolchainProfile": "android-ndk-r26",
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

        repo_root = self._make_repo_root("windows-validate-only")
        try:
            expected_android_host_root = (
                repo_root / "subjects" / subject_id / "validation" / "mobile" / "android-host"
            )
            expected_android_host_root.mkdir(parents=True, exist_ok=True)
            with patch.object(workers_module.tooling_module, "detect_visual_studio_generator", return_value="Visual Studio 18 2026"):
                with patch.object(workers_module.tooling_module, "detect_visual_studio_instance_spec", return_value=instance_spec):
                    with patch.object(workers_module.tooling_module, "allocate_cmake_binary_dir", return_value=expected_cmake_dir):
                        with patch.object(workers_module, "_run_checked") as run_checked_mock:
                            result = workers_module.run_build_target(repo_root=repo_root, request=request)

            self.assertEqual("ok", result["status"])
            self.assertEqual(
                [
                    "cmake",
                    "-S",
                    str(repo_root),
                    "-B",
                    str(expected_cmake_dir),
                    "-G",
                    "Visual Studio 18 2026",
                    "-DROADMAP0_PRESET_TARGET=android-arm64-smoke",
                    "-DROADMAP0_TOOLCHAIN_VALIDATE_ONLY=ON",
                    "-DCHAOS_SUBJECT_VARIANT=CHECK",
                    f"-DCHAOS_SUBJECT_ANDROID_HOST_ROOT={expected_android_host_root}",
                    f"-DCMAKE_TOOLCHAIN_FILE={repo_root / 'build' / 'toolchains' / 'android-arm64.cmake'}",
                    f"-DCMAKE_GENERATOR_INSTANCE={instance_spec}",
                ],
                run_checked_mock.call_args_list[0].args[0],
            )
            self.assertEqual(
                ["cmake", "--build", str(expected_cmake_dir)],
                run_checked_mock.call_args_list[1].args[0],
            )

            manifest = json.loads((repo_root / request["paths"]["manifestPath"]).read_text(encoding="utf-8"))
            self.assertEqual("CHECK", manifest["variant"])
            self.assertEqual(expected_cmake_dir.as_posix(), manifest["cmakeBinaryDir"])
        finally:
            shutil.rmtree(repo_root, ignore_errors=True)

    def test_windows_validate_only_build_supports_ios_target_and_subject_scoped_host_root(self) -> None:
        workers_module = load_module(SUBJECT_WORKERS_MODULE_PATH, "chaos_subject_workers_windows_ios_validate_only")
        subject_id = "FixtureIosValidateOnlySubject"
        run_id = "fixture-run-ios-validate-only-001"
        matrix_id = "windows-ios-check"
        expected_cmake_dir = self._make_non_repo_path("cmake-builds", "ios-platform-gate-1234")
        instance_spec = f"{self._make_non_repo_path('visual-studio', '18', 'Professional')},version=18.4.11626.88"

        request = {
            "selection": {
                "subjectId": subject_id,
                "matrixId": matrix_id,
                "variant": "CHECK",
                "executionContext": {
                    "hostPlatform": "windows-x64",
                    "targetPlatform": "ios-arm64",
                    "toolchainProfile": "ios-xcode",
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

        repo_root = self._make_repo_root("windows-ios-validate-only")
        try:
            expected_ios_host_root = repo_root / "subjects" / subject_id / "validation" / "mobile" / "ios-host"
            expected_ios_host_root.mkdir(parents=True, exist_ok=True)
            with patch.object(workers_module.tooling_module, "detect_visual_studio_generator", return_value="Visual Studio 18 2026"):
                with patch.object(workers_module.tooling_module, "detect_visual_studio_instance_spec", return_value=instance_spec):
                    with patch.object(workers_module.tooling_module, "allocate_cmake_binary_dir", return_value=expected_cmake_dir):
                        with patch.object(workers_module, "_run_checked") as run_checked_mock:
                            result = workers_module.run_build_target(repo_root=repo_root, request=request)

            self.assertEqual("ok", result["status"])
            self.assertEqual(
                [
                    "cmake",
                    "-S",
                    str(repo_root),
                    "-B",
                    str(expected_cmake_dir),
                    "-G",
                    "Visual Studio 18 2026",
                    "-DROADMAP0_PRESET_TARGET=ios-arm64-packaging",
                    "-DROADMAP0_TOOLCHAIN_VALIDATE_ONLY=ON",
                    "-DCHAOS_SUBJECT_VARIANT=CHECK",
                    f"-DCHAOS_SUBJECT_IOS_HOST_ROOT={expected_ios_host_root}",
                    f"-DCMAKE_TOOLCHAIN_FILE={repo_root / 'build' / 'toolchains' / 'ios-arm64.cmake'}",
                    f"-DCMAKE_GENERATOR_INSTANCE={instance_spec}",
                ],
                run_checked_mock.call_args_list[0].args[0],
            )
            self.assertEqual(
                ["cmake", "--build", str(expected_cmake_dir)],
                run_checked_mock.call_args_list[1].args[0],
            )

            manifest = json.loads((repo_root / request["paths"]["manifestPath"]).read_text(encoding="utf-8"))
            self.assertEqual("ios-arm64", manifest["targetPlatform"])
            self.assertEqual(expected_cmake_dir.as_posix(), manifest["cmakeBinaryDir"])
        finally:
            shutil.rmtree(repo_root, ignore_errors=True)

    def test_windows_android_runtime_build_uses_ninja_and_records_runtime_output(self) -> None:
        workers_module = load_module(SUBJECT_WORKERS_MODULE_PATH, "chaos_subject_workers_windows_android_runtime_build")
        subject_id = "FixtureAndroidRuntimeSubject"
        run_id = "fixture-run-android-runtime-build-001"
        matrix_id = "windows-android-runtime"
        expected_cmake_dir = self._make_non_repo_path("cmake-builds", "android-runtime-1234")
        expected_cmake_path = self._make_non_repo_path("cmake", "bin", "cmake.exe")
        expected_ninja_path = self._make_non_repo_path("cmake", "bin", "ninja.exe")
        expected_env = {
            "ANDROID_SDK_ROOT": r"C:\android\sdk",
            "ANDROID_NDK_ROOT": r"C:\android\sdk\ndk\26.3.11579264",
            "PATH": r"C:\android\sdk\platform-tools;C:\android\sdk\emulator",
        }

        request = {
            "selection": {
                "subjectId": subject_id,
                "matrixId": matrix_id,
                "variant": "CHECK",
                "executionContext": {
                    "hostPlatform": "windows-x64",
                    "targetPlatform": "android-arm64",
                    "toolchainProfile": "android-ndk-r26",
                    "runtimeProfile": "android-native-runtime",
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

        repo_root = self._make_repo_root("windows-android-runtime-build")
        try:
            expected_android_host_root = repo_root / "subjects" / subject_id / "validation" / "mobile" / "android-host"
            expected_android_host_root.mkdir(parents=True, exist_ok=True)

            with patch.object(workers_module.tooling_module, "cmake_environment", return_value=(str(expected_cmake_path), expected_env)):
                with patch.object(workers_module.tooling_module, "find_ninja_executable", return_value=str(expected_ninja_path)):
                    with patch.object(workers_module.tooling_module, "allocate_cmake_binary_dir", return_value=expected_cmake_dir):
                        with patch.object(workers_module, "_run_checked") as run_checked_mock:
                            result = workers_module.run_build_target(repo_root=repo_root, request=request)

            self.assertEqual("ok", result["status"])
            expected_output_root = (
                repo_root
                / "artifacts"
                / "subjects"
                / subject_id
                / "runs"
                / run_id
                / "matrices"
                / matrix_id
                / "build"
                / "out"
            )
            self.assertEqual(
                [
                    str(expected_cmake_path),
                    "-S",
                    str(repo_root),
                    "-B",
                    str(expected_cmake_dir),
                    "-G",
                    "Ninja",
                    "-DROADMAP0_PRESET_TARGET=android-arm64-smoke",
                    "-DCHAOS_SUBJECT_VARIANT=CHECK",
                    f"-DCHAOS_SUBJECT_ANDROID_HOST_ROOT={expected_android_host_root}",
                    f"-DCHAOS_SUBJECT_ANDROID_ARTIFACT_ROOT={expected_output_root}",
                    f"-DCMAKE_TOOLCHAIN_FILE={repo_root / 'build' / 'toolchains' / 'android-arm64.cmake'}",
                    "-DCMAKE_BUILD_TYPE=Release",
                    f"-DCMAKE_MAKE_PROGRAM={expected_ninja_path}",
                ],
                run_checked_mock.call_args_list[0].args[0],
            )
            self.assertEqual(
                [
                    str(expected_cmake_path),
                    "--build",
                    str(expected_cmake_dir),
                    "--target",
                    "mobile_hello_world_android_host_runtime",
                ],
                run_checked_mock.call_args_list[1].args[0],
            )
            self.assertEqual(expected_env, run_checked_mock.call_args_list[0].kwargs["env"])

            manifest = json.loads((repo_root / request["paths"]["manifestPath"]).read_text(encoding="utf-8"))
            self.assertEqual("android-native-cmake", manifest["buildStrategy"])
            self.assertEqual("android-arm64", manifest["targetPlatform"])
            self.assertEqual(expected_cmake_dir.as_posix(), manifest["cmakeBinaryDir"])
            self.assertEqual(
                [
                    subject_run_path(
                        subject_id,
                        run_id,
                        "matrices",
                        matrix_id,
                        "build",
                        "out",
                        "mobile_hello_world_android_host_runtime",
                    )
                ],
                manifest["outputs"],
            )
        finally:
            shutil.rmtree(repo_root, ignore_errors=True)
