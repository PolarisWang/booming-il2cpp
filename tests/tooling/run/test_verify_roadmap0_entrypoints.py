from tests.tooling.run.verify_roadmap0_test_support import *


class VerifyRoadmap0EntrypointTests(VerifyRoadmap0TestSupport):
    def test_posix_wrapper_runs_without_pwsh(self) -> None:
        if shutil.which("sh") is None:
            self.skipTest("POSIX shell is not available in this environment")
        completed = subprocess.run(
            ["sh", str(VERIFY_WRAPPER_PATH), "--help"],
            cwd=REPO_ROOT,
            capture_output=True,
            text=True,
        )

        self.assertEqual(0, completed.returncode, msg=completed.stdout + completed.stderr)
        self.assertIn("host-profile", completed.stdout)
        self.assertNotIn("pwsh", completed.stderr)

    def test_low_level_command_uses_python_entrypoint_on_macos(self) -> None:
        verify_module = load_module(VERIFY_MODULE_PATH, "chaos_run_verify_route")
        bootstrap = verify_module.tooling_module.ToolBootstrapResult(
            ready=True,
            output="",
            errors=[],
        )
        completed = subprocess.CompletedProcess(["python3"], 0, stdout="verify ok", stderr="")

        with patch.object(verify_module.tooling_module, "ensure_dotnet_available", return_value=bootstrap):
            with patch.object(verify_module, "run_process", return_value=completed) as run_process_mock:
                result = verify_module.handle(
                    {"id": "runtime-baseline-low-level-macos", "host_profile": "macos", "target": "macos"},
                    REPO_ROOT,
                    "macos",
                    "runtime-baseline low-level script (macos)",
                )

        args = run_process_mock.call_args.args[0]
        self.assertEqual("ok", result.status)
        self.assertEqual(sys.executable, args[0])
        self.assertEqual(REPO_ROOT / "build" / "scripts" / "verify-runtime-baseline.py", Path(args[1]))

    def test_low_level_command_uses_execution_policy_bypass_on_windows(self) -> None:
        verify_module = load_module(VERIFY_MODULE_PATH, "chaos_run_verify_route_windows")
        bootstrap = verify_module.tooling_module.ToolBootstrapResult(
            ready=True,
            output="",
            errors=[],
        )
        completed = subprocess.CompletedProcess(["powershell"], 0, stdout="verify ok", stderr="")

        with patch.object(verify_module.tooling_module, "ensure_dotnet_available", return_value=bootstrap):
            with patch.object(verify_module.tooling_module, "cmake_environment", return_value=("cmake", {})):
                with patch.object(verify_module, "run_process", return_value=completed) as run_process_mock:
                    result = verify_module.handle(
                        {"id": "runtime-baseline-low-level-windows", "host_profile": "windows", "target": "windows"},
                        REPO_ROOT,
                        "windows",
                        "runtime-baseline low-level script (windows)",
                    )

        args = run_process_mock.call_args.args[0]
        self.assertEqual("ok", result.status)
        self.assertEqual(
            [
                "powershell",
                "-NoLogo",
                "-NoProfile",
                "-NonInteractive",
                "-ExecutionPolicy",
                "Bypass",
                "-File",
            ],
            args[:7],
        )
        self.assertEqual(REPO_ROOT / "build" / "scripts" / "verify-runtime-baseline.ps1", Path(args[7]))
        self.assertEqual(["-HostProfile", "windows"], args[8:])

    def test_low_level_failure_text_includes_underlying_output(self) -> None:
        verify_module = load_module(VERIFY_MODULE_PATH, "chaos_run_verify_route_failure_text")
        bootstrap = verify_module.tooling_module.ToolBootstrapResult(
            ready=True,
            output="",
            errors=[],
        )
        completed = subprocess.CompletedProcess(
            ["powershell"],
            1,
            stdout="",
            stderr="running scripts is disabled on this system",
        )

        with patch.object(verify_module.tooling_module, "ensure_dotnet_available", return_value=bootstrap):
            with patch.object(verify_module.tooling_module, "cmake_environment", return_value=("cmake", {})):
                with patch.object(verify_module, "run_process", return_value=completed):
                    result = verify_module.handle(
                        {"id": "runtime-baseline-low-level-windows", "host_profile": "windows", "target": "windows"},
                        REPO_ROOT,
                        "windows",
                        "runtime-baseline low-level script (windows)",
                    )

        self.assertEqual("error", result.status)
        self.assertIn("running scripts is disabled on this system", result.payload["consoleText"])
        self.assertIn("running scripts is disabled on this system", result.text)

    def test_low_level_powershell_script_is_a_python_forwarder(self) -> None:
        script_text = VERIFY_SCRIPT_PS1_PATH.read_text(encoding="utf-8")

        required_markers = [
            "Get-Command python",
            "Get-Command py",
            "verify-runtime-baseline.py",
            "--host-profile",
        ]
        forbidden_markers = [
            "Assert-Stage4ProofRunArtifacts",
            "Invoke-Stage4NativeReferenceProofRun",
            "Invoke-Stage4NativeReferenceCodegen",
        ]

        for marker in required_markers:
            self.assertIn(marker, script_text)
        for marker in forbidden_markers:
            self.assertNotIn(marker, script_text)

    def test_legacy_powershell_wrapper_forwards_to_runtime_baseline_script(self) -> None:
        script_text = LEGACY_VERIFY_SCRIPT_PS1_PATH.read_text(encoding="utf-8")

        self.assertIn("verify-runtime-baseline.py", script_text)

    def test_legacy_shell_wrapper_forwards_to_runtime_baseline_script(self) -> None:
        script_text = LEGACY_VERIFY_WRAPPER_PATH.read_text(encoding="utf-8")

        self.assertIn("verify-runtime-baseline.py", script_text)
