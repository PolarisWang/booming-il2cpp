from tests.unit.performance.subject_workers_perf_test_support import *


class TestSubjectWorkersPerfMobile(SubjectWorkersPerfTestSupport):
    def test_mobile_native_perf_android_uses_target_platform_baseline_and_records_collector_details(self) -> None:
        workers_module = load_module(SUBJECT_WORKERS_MODULE_PATH, "chaos_subject_workers_mobile_android_perf")
        subject_id = "FixtureMobilePerfSubject"
        run_id = "fixture-run-mobile-android-perf-001"
        matrix_id = "android-arm64-native-perf"
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
                "variant": "PROFILE",
                "executionContext": {
                    "hostPlatform": "windows-x64",
                    "targetPlatform": "android-arm64",
                    "runtimeProfile": "android-native-perf-profile",
                    "runtimeArguments": ["--subject-id=FixtureMobilePerfSubject"],
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

        repo_root = self._make_repo_root("mobile-native-perf-android")
        try:
            build_manifest_path = repo_root / request["upstream"]["build"]["manifestPath"]
            build_manifest_path.parent.mkdir(parents=True, exist_ok=True)
            build_manifest_path.write_text(
                json.dumps(
                    {
                        "subjectId": subject_id,
                        "matrixId": matrix_id,
                        "buildStrategy": "android-native-cmake",
                        "outputs": [executable_path],
                    }
                ),
                encoding="utf-8",
            )

            collector_result = {
                "exitCode": 0,
                "stdout": "android mobile perf ok\n",
                "stderr": "",
                "samples": [
                    {"sampleIndex": 1, "durationMs": 11.0},
                    {"sampleIndex": 2, "durationMs": 13.0},
                ],
                "details": {"collector": "adb", "platform": "android-arm64"},
                "evidencePaths": [
                    subject_run_path(subject_id, run_id, "matrices", matrix_id, "runtime", "collector-result.json"),
                ],
            }
            perf_result = {
                "baselinePath": posix_path("subjects", subject_id, "baselines", "perf", matrix_id, "android-arm64.json"),
                "baseline": {"meanDurationMs": 10.0},
                "metrics": {"sampleCount": 2, "meanDurationMs": 12.0, "minDurationMs": 11.0, "maxDurationMs": 13.0},
                "baselineUpdated": False,
                "regressionStatus": "regressed",
                "regressions": [{"metric": "meanDurationMs", "baseline": 10.0, "actual": 12.0, "delta": 2.0}],
            }

            with patch.object(workers_module.mobile_perf_collector, "collect_android_perf", return_value=collector_result) as collect_mock:
                with patch.object(workers_module.perf_module, "evaluate_perf_subject", return_value=perf_result) as evaluate_mock:
                    result = workers_module.run_mobile_native_perf(repo_root=repo_root, request=request)

            self.assertEqual("ok", result["status"])
            collect_mock.assert_called_once()
            evaluate_mock.assert_called_once_with(
                repo_root=repo_root,
                subject_id=subject_id,
                matrix_id=matrix_id,
                host_platform="android-arm64",
                metrics={"sampleCount": 2, "meanDurationMs": 12.0, "minDurationMs": 11.0, "maxDurationMs": 13.0},
                update_baseline=False,
            )

            manifest = json.loads((repo_root / request["paths"]["manifestPath"]).read_text(encoding="utf-8"))
            self.assertEqual("android-arm64", manifest["targetPlatform"])
            self.assertEqual("regressed", manifest["regressionStatus"])
            self.assertEqual(
                [subject_run_path(subject_id, run_id, "matrices", matrix_id, "runtime", "collector-result.json")],
                manifest["collectorEvidencePaths"],
            )
            self.assertEqual("adb", manifest["collectorDetails"]["collector"])
        finally:
            shutil.rmtree(repo_root, ignore_errors=True)

    def test_mobile_native_perf_ios_uses_cmake_binary_dir_and_target_platform_baseline(self) -> None:
        workers_module = load_module(SUBJECT_WORKERS_MODULE_PATH, "chaos_subject_workers_mobile_ios_perf")
        subject_id = "FixtureMobilePerfSubject"
        run_id = "fixture-run-mobile-ios-perf-001"
        matrix_id = "ios-arm64-native-perf"
        request = {
            "selection": {
                "subjectId": subject_id,
                "matrixId": matrix_id,
                "variant": "PROFILE",
                "executionContext": {
                    "hostPlatform": "macos-arm64",
                    "targetPlatform": "ios-arm64",
                    "runtimeProfile": "ios-native-perf-profile",
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

        repo_root = self._make_repo_root("mobile-native-perf-ios")
        try:
            build_manifest_path = repo_root / request["upstream"]["build"]["manifestPath"]
            build_manifest_path.parent.mkdir(parents=True, exist_ok=True)
            build_manifest_path.write_text(
                json.dumps(
                    {
                        "subjectId": subject_id,
                        "matrixId": matrix_id,
                        "cmakeBinaryDir": self._make_non_repo_path("ios", "cmake", "fixture-build").as_posix(),
                        "outputs": [subject_run_path(subject_id, run_id, "matrices", matrix_id, "build", "validate-only.success.txt")],
                    }
                ),
                encoding="utf-8",
            )

            collector_result = {
                "exitCode": 0,
                "stdout": "ios mobile perf ok\n",
                "stderr": "",
                "samples": [
                    {"sampleIndex": 1, "durationMs": 21.0},
                    {"sampleIndex": 2, "durationMs": 22.0},
                ],
                "details": {"collector": "xcodebuild", "platform": "ios-arm64"},
                "evidencePaths": [
                    subject_run_path(subject_id, run_id, "matrices", matrix_id, "runtime", "xcode-test-result.json"),
                ],
            }
            perf_result = {
                "baselinePath": posix_path("subjects", subject_id, "baselines", "perf", matrix_id, "ios-arm64.json"),
                "baseline": {"meanDurationMs": 20.0},
                "metrics": {"sampleCount": 2, "meanDurationMs": 21.5, "minDurationMs": 21.0, "maxDurationMs": 22.0},
                "baselineUpdated": False,
                "regressionStatus": "regressed",
                "regressions": [{"metric": "meanDurationMs", "baseline": 20.0, "actual": 21.5, "delta": 1.5}],
            }

            with patch.object(workers_module.mobile_perf_collector, "collect_ios_perf", return_value=collector_result) as collect_mock:
                with patch.object(workers_module.perf_module, "evaluate_perf_subject", return_value=perf_result) as evaluate_mock:
                    result = workers_module.run_mobile_native_perf(repo_root=repo_root, request=request)

            self.assertEqual("ok", result["status"])
            collect_mock.assert_called_once()
            evaluate_mock.assert_called_once_with(
                repo_root=repo_root,
                subject_id=subject_id,
                matrix_id=matrix_id,
                host_platform="ios-arm64",
                metrics={"sampleCount": 2, "meanDurationMs": 21.5, "minDurationMs": 21.0, "maxDurationMs": 22.0},
                update_baseline=False,
            )

            manifest = json.loads((repo_root / request["paths"]["manifestPath"]).read_text(encoding="utf-8"))
            self.assertEqual("ios-arm64", manifest["targetPlatform"])
            self.assertEqual("xcodebuild", manifest["collectorDetails"]["collector"])
        finally:
            shutil.rmtree(repo_root, ignore_errors=True)
