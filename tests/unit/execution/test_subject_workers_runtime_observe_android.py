from tests.unit.execution.subject_workers_test_support import *


class TestSubjectWorkersRuntimeObserveAndroid(SubjectWorkersTestSupport):
    def test_runtime_observe_executes_android_native_build_output_via_adb_shell(self) -> None:
        workers_module = load_module(SUBJECT_WORKERS_MODULE_PATH, "chaos_subject_workers_runtime_observe_android")
        subject_id = "FixtureAndroidRuntimeSubject"
        run_id = "fixture-run-android-runtime-observe-001"
        matrix_id = "windows-android-runtime"
        executable_path = subject_run_path(
            subject_id,
            run_id,
            "matrices",
            matrix_id,
            "build",
            "out",
            "mobile_hello_world_android_host_runtime",
        )

        request = {
            "selection": {
                "subjectId": subject_id,
                "matrixId": matrix_id,
                "variant": "CHECK",
                "executionContext": {
                    "hostPlatform": "windows-x64",
                    "targetPlatform": "android-arm64",
                    "runtimeProfile": "android-native-runtime",
                    "runtimeArguments": [
                        "--soak-duration-seconds=180",
                        "--heartbeat-interval-seconds=15",
                    ],
                },
            },
            "upstream": {
                "build": {
                    "manifestPath": subject_run_path(subject_id, run_id, "matrices", matrix_id, "build", "build.manifest.json"),
                }
            },
            "paths": {
                "bucketRoot": subject_run_path(subject_id, run_id, "matrices", matrix_id, "runtime"),
                "manifestPath": subject_run_path(subject_id, run_id, "matrices", matrix_id, "runtime", "runtime.manifest.json"),
                "reportPaths": [],
            },
        }

        repo_root = self._make_repo_root("runtime-observe-android")
        try:
            build_manifest_path = repo_root / request["upstream"]["build"]["manifestPath"]
            build_manifest_path.parent.mkdir(parents=True, exist_ok=True)
            build_manifest_path.write_text(
                json.dumps(
                    {
                        "buildStrategy": "android-native-cmake",
                        "outputs": [executable_path],
                    }
                ),
                encoding="utf-8",
            )

            android_result = subprocess.CompletedProcess(
                ["adb", "shell"],
                0,
                "mobile-host|stage=shared-host-bootstrap|detail=MobileHelloWorldProof|exitCode=0\n__CHAOS_EXIT_CODE__=0\n",
                "",
            )

            with patch.object(
                workers_module.tooling_module,
                "ensure_android_host_tooling_available",
                return_value=workers_module.tooling_module.ToolBootstrapResult(ready=True, output="android host ready\n"),
            ):
                with patch.object(
                    workers_module.tooling_module,
                    "android_environment_overrides",
                    return_value={
                        "ANDROID_SDK_ROOT": r"C:\android\sdk",
                        "ANDROID_AVD_HOME": r"C:\android\.android\avd",
                        "ANDROID_EMULATOR_HOME": r"C:\android\.android",
                        "PATH": r"C:\android\sdk\platform-tools;C:\android\sdk\emulator",
                    },
                ):
                    with patch.object(workers_module, "_launch_android_emulator", return_value=("emulator-5560", object(), None, None)) as launch_mock:
                        with patch.object(workers_module, "_wait_for_android_boot_completed") as wait_mock:
                            with patch.object(workers_module, "_run_android_binary_via_adb", return_value=android_result) as adb_mock:
                                with patch.object(workers_module, "_shutdown_android_emulator") as shutdown_mock:
                                    result = workers_module.run_runtime_observe(repo_root=repo_root, request=request)

            self.assertEqual("ok", result["status"])
            launch_mock.assert_called_once()
            wait_mock.assert_called_once()
            adb_mock.assert_called_once_with(
                repo_root=repo_root,
                executable_path=repo_root / executable_path,
                serial="emulator-5560",
                env={
                    "ANDROID_SDK_ROOT": r"C:\android\sdk",
                    "ANDROID_AVD_HOME": r"C:\android\.android\avd",
                    "ANDROID_EMULATOR_HOME": r"C:\android\.android",
                    "PATH": r"C:\android\sdk\platform-tools;C:\android\sdk\emulator",
                },
                runtime_arguments=[
                    "--soak-duration-seconds=180",
                    "--heartbeat-interval-seconds=15",
                ],
            )
            shutdown_mock.assert_called_once()

            manifest = json.loads((repo_root / request["paths"]["manifestPath"]).read_text(encoding="utf-8"))
            self.assertEqual("emulator-5560", manifest["androidSerial"])
            self.assertEqual(workers_module.tooling_module.ANDROID_AVD_NAME, manifest["androidAvdName"])
            self.assertEqual(
                ["--soak-duration-seconds=180", "--heartbeat-interval-seconds=15"],
                manifest["runtimeArguments"],
            )
            self.assertEqual(
                ["mobile-host|stage=shared-host-bootstrap|detail=MobileHelloWorldProof|exitCode=0"],
                manifest["outputLines"],
            )
            self.assertEqual(
                "mobile-host|stage=shared-host-bootstrap|detail=MobileHelloWorldProof|exitCode=0\n",
                (repo_root / manifest["stdoutPath"]).read_text(encoding="utf-8"),
            )
            self.assertEqual(
                "0\n",
                (repo_root / manifest["exitCodePath"]).read_text(encoding="utf-8"),
            )
        finally:
            shutil.rmtree(repo_root, ignore_errors=True)

    def test_run_android_binary_via_adb_prefixes_known_runtime_arguments_as_environment_variables(self) -> None:
        workers_module = load_module(SUBJECT_WORKERS_MODULE_PATH, "chaos_subject_workers_android_runtime_arguments")
        repo_root = self._make_repo_root("android-runtime-arguments")
        executable_path = repo_root / "artifacts" / "mobile_hello_world_android_host_runtime"
        executable_path.parent.mkdir(parents=True, exist_ok=True)
        executable_path.write_text("fixture", encoding="utf-8")
        env = {
            "ANDROID_SDK_ROOT": r"C:\android\sdk",
            "PATH": r"C:\android\sdk\platform-tools",
        }

        try:
            with patch.object(workers_module, "_run_android_host_command") as host_command_mock:
                with patch.object(
                    workers_module,
                    "run_process",
                    return_value=subprocess.CompletedProcess(["adb", "shell"], 0, "__CHAOS_EXIT_CODE__=0\n", ""),
                ) as run_process_mock:
                    workers_module._run_android_binary_via_adb(
                        repo_root=repo_root,
                        executable_path=executable_path,
                        serial="emulator-5554",
                        env=env,
                        runtime_arguments=[
                            "--soak-duration-seconds=180",
                            "--heartbeat-interval-seconds=15",
                            "--subject-id=Probe",
                        ],
                    )

            self.assertEqual(3, host_command_mock.call_count)
            run_process_mock.assert_called_once()
            self.assertEqual(
                [
                    workers_module._android_adb_executable(repo_root),
                    "-s",
                    "emulator-5554",
                    "shell",
                    "sh",
                    "-c",
                    (
                        "export CHAOS_MOBILE_HOST_SOAK_DURATION_SECONDS=180 >/dev/null; "
                        "export CHAOS_MOBILE_HOST_HEARTBEAT_INTERVAL_SECONDS=15 >/dev/null; "
                        "export CHAOS_MOBILE_HOST_SUBJECT_ID=Probe >/dev/null; "
                        "/data/local/tmp/chaos-subjects/mobile_hello_world_android_host_runtime "
                        "--soak-duration-seconds=180 "
                        "--heartbeat-interval-seconds=15 "
                        "--subject-id=Probe; "
                        "status=$?; printf '\\n__CHAOS_EXIT_CODE__=%s\\n' \"$status\""
                    ),
                ],
                run_process_mock.call_args.args[0],
            )
        finally:
            shutil.rmtree(repo_root, ignore_errors=True)
