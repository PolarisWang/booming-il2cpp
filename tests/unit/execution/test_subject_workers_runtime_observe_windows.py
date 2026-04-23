from tests.unit.execution.subject_workers_test_support import *


class TestSubjectWorkersRuntimeObserveWindows(SubjectWorkersTestSupport):
    def test_runtime_observe_accepts_non_repo_cmake_binary_dir_from_build_manifest(self) -> None:
        workers_module = load_module(SUBJECT_WORKERS_MODULE_PATH, "chaos_subject_workers_runtime_observe")
        subject_id = "FixtureRuntimeObserveSubject"
        run_id = "fixture-run-runtime-observe-001"
        matrix_id = "windows-reference-check"

        request = {
            "selection": {
                "subjectId": subject_id,
                "matrixId": matrix_id,
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

        repo_root = self._make_repo_root("runtime-observe")
        try:
            expected_cmake_dir = self._make_non_repo_path("cmake-builds", "native-reference-run-1234")
            expected_cmake_path = self._make_non_repo_path("cmake", "bin", "cmake.exe")
            expected_env = {
                "Path": r"C:\VS\bin;C:\Windows\System32",
                "INCLUDE": r"C:\VS\include",
                "LIB": r"C:\VS\lib",
            }
            build_manifest_path = repo_root / request["upstream"]["build"]["manifestPath"]
            build_manifest_path.parent.mkdir(parents=True, exist_ok=True)
            build_manifest_path.write_text(
                json.dumps(
                    {
                        "cmakeBinaryDir": expected_cmake_dir.as_posix(),
                        "outputs": [],
                    }
                ),
                encoding="utf-8",
            )

            with patch.object(workers_module.tooling_module, "cmake_environment", return_value=(str(expected_cmake_path), {})):
                with patch.object(workers_module.tooling_module, "windows_developer_environment", return_value=expected_env):
                    with patch.object(workers_module, "_run_checked") as run_checked_mock:
                        result = workers_module.run_runtime_observe(repo_root=repo_root, request=request)

            self.assertEqual("ok", result["status"])
            build_args = run_checked_mock.call_args.args[0]
            self.assertEqual([str(expected_cmake_path), "--build"], build_args[:2])
            self.assertEqual(expected_cmake_dir, Path(build_args[2]))
            self.assertEqual(
                ["--config", "Release", "--target", WINDOWS_REFERENCE_RUN_TARGET],
                build_args[3:],
            )
            self.assertEqual(expected_env, run_checked_mock.call_args.kwargs["env"])
        finally:
            shutil.rmtree(repo_root, ignore_errors=True)

    def test_runtime_observe_records_manifest_when_windows_reference_run_target_fails(self) -> None:
        workers_module = load_module(SUBJECT_WORKERS_MODULE_PATH, "chaos_subject_workers_runtime_observe_failure_manifest")
        subject_id = "FixtureRuntimeObserveSubject"
        run_id = "fixture-run-runtime-observe-fail-001"
        matrix_id = "windows-reference-check"

        request = {
            "selection": {
                "subjectId": subject_id,
                "matrixId": matrix_id,
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

        repo_root = self._make_repo_root("runtime-observe-fail")
        try:
            expected_cmake_dir = self._make_non_repo_path("cmake-builds", "native-reference-run-fail-1234")
            expected_cmake_path = self._make_non_repo_path("cmake", "bin", "cmake.exe")
            expected_env = {
                "Path": r"C:\VS\bin;C:\Windows\System32",
                "INCLUDE": r"C:\VS\include",
                "LIB": r"C:\VS\lib",
            }
            runtime_root = repo_root / request["paths"]["bucketRoot"]
            runtime_root.mkdir(parents=True, exist_ok=True)
            (runtime_root / "stdout.log").write_text("", encoding="utf-8")
            (runtime_root / "stderr.log").write_text("native reference failed\n", encoding="utf-8")
            (runtime_root / "exit-code.txt").write_text("4\n", encoding="utf-8")

            build_manifest_path = repo_root / request["upstream"]["build"]["manifestPath"]
            build_manifest_path.parent.mkdir(parents=True, exist_ok=True)
            build_manifest_path.write_text(
                json.dumps(
                    {
                        "cmakeBinaryDir": expected_cmake_dir.as_posix(),
                        "outputs": [],
                    }
                ),
                encoding="utf-8",
            )

            with patch.object(workers_module.tooling_module, "cmake_environment", return_value=(str(expected_cmake_path), {})):
                with patch.object(workers_module.tooling_module, "windows_developer_environment", return_value=expected_env):
                    with patch.object(workers_module, "_run_checked", side_effect=RuntimeError("subject proof run failed: windows-x64-reference")):
                        result = workers_module.run_runtime_observe(repo_root=repo_root, request=request)

            self.assertEqual("fail", result["status"])
            manifest = json.loads((repo_root / request["paths"]["manifestPath"]).read_text(encoding="utf-8"))
            self.assertEqual(
                subject_run_path(subject_id, run_id, "matrices", matrix_id, "runtime", "stdout.log"),
                manifest["stdoutPath"],
            )
            self.assertEqual(
                subject_run_path(subject_id, run_id, "matrices", matrix_id, "runtime", "stderr.log"),
                manifest["stderrPath"],
            )
            self.assertEqual(
                subject_run_path(subject_id, run_id, "matrices", matrix_id, "runtime", "exit-code.txt"),
                manifest["exitCodePath"],
            )
            self.assertIn(manifest["stderrPath"], result["primaryEvidencePaths"])
            self.assertIn("windows-x64-reference", result["failure"])
        finally:
            shutil.rmtree(repo_root, ignore_errors=True)

    def test_runtime_observe_executes_direct_msvc_build_output(self) -> None:
        workers_module = load_module(SUBJECT_WORKERS_MODULE_PATH, "chaos_subject_workers_runtime_observe_direct")
        subject_id = "FixtureRuntimeObserveSubject"
        run_id = "fixture-run-runtime-observe-direct-001"
        matrix_id = "windows-reference-check"
        executable_path = subject_run_path(
            subject_id,
            run_id,
            "matrices",
            matrix_id,
            "build",
            "out",
            "chaos_subject_reference_proof.exe",
        )

        request = {
            "selection": {
                "subjectId": subject_id,
                "matrixId": matrix_id,
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

        repo_root = self._make_repo_root("runtime-observe-direct")
        try:
            build_manifest_path = repo_root / request["upstream"]["build"]["manifestPath"]
            build_manifest_path.parent.mkdir(parents=True, exist_ok=True)
            build_manifest_path.write_text(
                json.dumps(
                    {
                        "buildStrategy": "direct-msvc",
                        "outputs": [executable_path],
                    }
                ),
                encoding="utf-8",
            )

            completed = subprocess.CompletedProcess(
                [str(repo_root / executable_path)],
                0,
                "native proof ok\n",
                "",
            )

            with patch.object(workers_module, "run_process", return_value=completed) as run_process_mock:
                result = workers_module.run_runtime_observe(repo_root=repo_root, request=request)

            self.assertEqual("ok", result["status"])
            run_process_mock.assert_called_once_with(
                [str(repo_root / executable_path)],
                cwd=repo_root / request["paths"]["bucketRoot"],
            )

            manifest = json.loads((repo_root / request["paths"]["manifestPath"]).read_text(encoding="utf-8"))
            self.assertEqual(
                subject_run_path(subject_id, run_id, "matrices", matrix_id, "runtime", "stdout.log"),
                manifest["stdoutPath"],
            )
            self.assertEqual(
                "native proof ok\n",
                (repo_root / manifest["stdoutPath"]).read_text(encoding="utf-8"),
            )
            self.assertEqual(
                "0\n",
                (repo_root / manifest["exitCodePath"]).read_text(encoding="utf-8"),
            )
        finally:
            shutil.rmtree(repo_root, ignore_errors=True)
