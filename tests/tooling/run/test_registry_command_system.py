from tests.tooling.run.registry_command_test_support import *


class TestRegistryCommandSystem(RegistryCommandTestSupport):
    def test_system_dispatch_expands_registered_plan(self) -> None:
        test_module = load_module(TEST_COMMAND_MODULE_PATH, "chaos_run_test_command_system_dispatch")
        manifest_module = load_module(MANIFEST_MODULE_PATH, "chaos_run_manifest_system_dispatch")
        session_module = load_module(SESSION_MODULE_PATH, "chaos_run_session_system_dispatch")
        manifest = manifest_module.load_run_manifest(REPO_ROOT, RUN_MANIFEST_PATH)

        def fake_session(
            family: str,
            suite: str,
            stage: str,
            repo_root: Path,
            host_platform: str,
            command_text: str,
            manifest_payload: dict,
        ):
            del repo_root
            del manifest_payload
            request = session_module.TestRequest(
                family=family,
                suite=suite,
                stage=stage,
                command_text=command_text,
            )
            return session_module.SessionResult(
                request=request,
                host_platform=host_platform,
                status="ok",
                suite_results=[{"suiteId": request.suite_key, "status": "ok", "stageResults": {}}],
                text=f"{request.suite_key} ok\n",
                artifacts=[],
                exit_code=0,
            )

        with patch.object(test_module, "_execute_public_test_session", side_effect=fake_session):
            result = test_module.handle(
                {"id": "test-system", "handler": "test.dispatch"},
                REPO_ROOT,
                "macos",
                "test system --id system/runtime-baseline-macos",
                manifest,
                {"id": "system/runtime-baseline-macos"},
            )

        self.assertEqual("ok", result.status)
        self.assertEqual("system/runtime-baseline-macos", result.target)
        self.assertEqual("system/runtime-baseline-macos", result.payload["selectedObject"]["id"])
        self.assertEqual(
            "run test system --id system/runtime-baseline-macos",
            result.payload["selectedObject"]["canonicalCommand"],
        )
        self.assertEqual(
            [
                "contract/analysis-schema",
                "smoke/managed-entry-basic",
                "smoke/managed-generics-basic",
                "smoke/reflection-basic",
                "contract/native-abi",
                "contract/native-bridge",
                "smoke/native-interop-basic",
                "smoke/host-embedding-basic",
                "contract/trace-schema",
                "contract/trace-compare-macos",
                "gate/macos-reference-desktop",
                "gate/ios-arm64-packaging",
                "gate/linux-x64-packaging",
            ],
            [item["id"] for item in result.payload["items"]],
        )

    def test_removed_runtime_baseline_system_alias_returns_not_found(self) -> None:
        test_module = load_module(TEST_COMMAND_MODULE_PATH, "chaos_run_test_command_system_dispatch_legacy_alias")
        manifest_module = load_module(MANIFEST_MODULE_PATH, "chaos_run_manifest_system_dispatch_legacy_alias")
        manifest = manifest_module.load_run_manifest(REPO_ROOT, RUN_MANIFEST_PATH)
        result = test_module.handle(
            {"id": "test-system", "handler": "test.dispatch"},
            REPO_ROOT,
            "macos",
            "test system --id system/roadmap-0-macos",
            manifest,
            {"id": "system/roadmap-0-macos"},
        )

        self.assertEqual("error", result.status)
        self.assertEqual("system/roadmap-0-macos", result.target)
        self.assertEqual(["registry object not found: system/roadmap-0-macos"], result.errors)
        self.assertEqual(2, result.payload["exitCode"])

    def test_removed_pipeline_dispatch_returns_unsupported_public_command(self) -> None:
        test_module = load_module(TEST_COMMAND_MODULE_PATH, "chaos_run_test_command_removed_pipeline_dispatch")
        manifest_module = load_module(MANIFEST_MODULE_PATH, "chaos_run_manifest_removed_pipeline_dispatch")
        manifest = manifest_module.load_run_manifest(REPO_ROOT, RUN_MANIFEST_PATH)
        result = test_module.handle(
            {"id": "test-pipeline", "handler": "test.dispatch"},
            REPO_ROOT,
            "macos",
            "test pipeline --id pipeline/completion-runtime-core",
            manifest,
            {"id": "pipeline/completion-runtime-core"},
        )

        self.assertEqual("error", result.status)
        self.assertEqual("test-pipeline", result.target)
        self.assertEqual(["unsupported public test command: test-pipeline"], result.errors)

    def test_linux_gate_system_dispatch_expands_registered_plan(self) -> None:
        test_module = load_module(TEST_COMMAND_MODULE_PATH, "chaos_run_test_command_linux_gate_system_dispatch")
        manifest_module = load_module(MANIFEST_MODULE_PATH, "chaos_run_manifest_linux_gate_system_dispatch")
        session_module = load_module(SESSION_MODULE_PATH, "chaos_run_session_linux_gate_system_dispatch")
        manifest = manifest_module.load_run_manifest(REPO_ROOT, RUN_MANIFEST_PATH)

        def fake_session(
            family: str,
            suite: str,
            stage: str,
            repo_root: Path,
            host_platform: str,
            command_text: str,
            manifest_payload: dict,
        ):
            del repo_root
            del manifest_payload
            request = session_module.TestRequest(
                family=family,
                suite=suite,
                stage=stage,
                command_text=command_text,
            )
            return session_module.SessionResult(
                request=request,
                host_platform=host_platform,
                status="ok",
                suite_results=[{"suiteId": request.suite_key, "status": "ok", "stageResults": {}}],
                text=f"{request.suite_key} ok\n",
                artifacts=[],
                exit_code=0,
            )

        with patch.object(test_module, "_execute_public_test_session", side_effect=fake_session):
            result = test_module.handle(
                {"id": "test-system", "handler": "test.dispatch"},
                REPO_ROOT,
                "macos",
                "test system --id system/linux-packaging-gate",
                manifest,
                {"id": "system/linux-packaging-gate"},
            )

        self.assertEqual("ok", result.status)
        self.assertEqual("system/linux-packaging-gate", result.target)
        self.assertEqual("system/linux-packaging-gate", result.payload["selectedObject"]["id"])
        self.assertEqual(
            "run test system --id system/linux-packaging-gate",
            result.payload["selectedObject"]["canonicalCommand"],
        )
        self.assertEqual(
            ["gate/linux-x64-packaging"],
            [item["id"] for item in result.payload["items"]],
        )

    def test_removed_linux_gate_system_alias_returns_not_found(self) -> None:
        test_module = load_module(TEST_COMMAND_MODULE_PATH, "chaos_run_test_command_linux_gate_system_dispatch_legacy_alias")
        manifest_module = load_module(MANIFEST_MODULE_PATH, "chaos_run_manifest_linux_gate_system_dispatch_legacy_alias")
        manifest = manifest_module.load_run_manifest(REPO_ROOT, RUN_MANIFEST_PATH)
        result = test_module.handle(
            {"id": "test-system", "handler": "test.dispatch"},
            REPO_ROOT,
            "macos",
            "test system --id system/roadmap-0-linux-packaging-gate",
            manifest,
            {"id": "system/roadmap-0-linux-packaging-gate"},
        )

        self.assertEqual("error", result.status)
        self.assertEqual("system/roadmap-0-linux-packaging-gate", result.target)
        self.assertEqual(["registry object not found: system/roadmap-0-linux-packaging-gate"], result.errors)
        self.assertEqual(2, result.payload["exitCode"])

    def test_ios_gate_system_dispatch_expands_registered_plan(self) -> None:
        test_module = load_module(TEST_COMMAND_MODULE_PATH, "chaos_run_test_command_ios_gate_system_dispatch")
        manifest_module = load_module(MANIFEST_MODULE_PATH, "chaos_run_manifest_ios_gate_system_dispatch")
        session_module = load_module(SESSION_MODULE_PATH, "chaos_run_session_ios_gate_system_dispatch")
        manifest = manifest_module.load_run_manifest(REPO_ROOT, RUN_MANIFEST_PATH)

        def fake_session(
            family: str,
            suite: str,
            stage: str,
            repo_root: Path,
            host_platform: str,
            command_text: str,
            manifest_payload: dict,
        ):
            del repo_root
            del manifest_payload
            request = session_module.TestRequest(
                family=family,
                suite=suite,
                stage=stage,
                command_text=command_text,
            )
            return session_module.SessionResult(
                request=request,
                host_platform=host_platform,
                status="ok",
                suite_results=[{"suiteId": request.suite_key, "status": "ok", "stageResults": {}}],
                text=f"{request.suite_key} ok\n",
                artifacts=[],
                exit_code=0,
            )

        with patch.object(test_module, "_execute_public_test_session", side_effect=fake_session):
            result = test_module.handle(
                {"id": "test-system", "handler": "test.dispatch"},
                REPO_ROOT,
                "macos",
                "test system --id system/ios-packaging-gate",
                manifest,
                {"id": "system/ios-packaging-gate"},
            )

        self.assertEqual("ok", result.status)
        self.assertEqual("system/ios-packaging-gate", result.target)
        self.assertEqual("system/ios-packaging-gate", result.payload["selectedObject"]["id"])
        self.assertEqual(
            "run test system --id system/ios-packaging-gate",
            result.payload["selectedObject"]["canonicalCommand"],
        )
        self.assertEqual(
            ["gate/ios-arm64-packaging"],
            [item["id"] for item in result.payload["items"]],
        )

    def test_android_gate_system_dispatch_expands_registered_plan(self) -> None:
        test_module = load_module(TEST_COMMAND_MODULE_PATH, "chaos_run_test_command_android_gate_system_dispatch")
        manifest_module = load_module(MANIFEST_MODULE_PATH, "chaos_run_manifest_android_gate_system_dispatch")
        session_module = load_module(SESSION_MODULE_PATH, "chaos_run_session_android_gate_system_dispatch")
        manifest = manifest_module.load_run_manifest(REPO_ROOT, RUN_MANIFEST_PATH)

        def fake_session(
            family: str,
            suite: str,
            stage: str,
            repo_root: Path,
            host_platform: str,
            command_text: str,
            manifest_payload: dict,
        ):
            del repo_root
            del manifest_payload
            request = session_module.TestRequest(
                family=family,
                suite=suite,
                stage=stage,
                command_text=command_text,
            )
            return session_module.SessionResult(
                request=request,
                host_platform=host_platform,
                status="ok",
                suite_results=[{"suiteId": request.suite_key, "status": "ok", "stageResults": {}}],
                text=f"{request.suite_key} ok\n",
                artifacts=[],
                exit_code=0,
            )

        with patch.object(test_module, "_execute_public_test_session", side_effect=fake_session):
            result = test_module.handle(
                {"id": "test-system", "handler": "test.dispatch"},
                REPO_ROOT,
                "windows",
                "test system --id system/android-startup-gate",
                manifest,
                {"id": "system/android-startup-gate"},
            )

        self.assertEqual("ok", result.status)
        self.assertEqual("system/android-startup-gate", result.target)
        self.assertEqual("system/android-startup-gate", result.payload["selectedObject"]["id"])
        self.assertEqual(
            "run test system --id system/android-startup-gate",
            result.payload["selectedObject"]["canonicalCommand"],
        )
        self.assertEqual(
            ["gate/android-arm64-smoke"],
            [item["id"] for item in result.payload["items"]],
        )

    def test_windows_reference_gate_system_dispatch_expands_registered_plan(self) -> None:
        test_module = load_module(TEST_COMMAND_MODULE_PATH, "chaos_run_test_command_windows_reference_gate_system_dispatch")
        manifest_module = load_module(MANIFEST_MODULE_PATH, "chaos_run_manifest_windows_reference_gate_system_dispatch")
        session_module = load_module(SESSION_MODULE_PATH, "chaos_run_session_windows_reference_gate_system_dispatch")
        manifest = manifest_module.load_run_manifest(REPO_ROOT, RUN_MANIFEST_PATH)

        def fake_session(
            family: str,
            suite: str,
            stage: str,
            repo_root: Path,
            host_platform: str,
            command_text: str,
            manifest_payload: dict,
        ):
            del repo_root
            del manifest_payload
            request = session_module.TestRequest(
                family=family,
                suite=suite,
                stage=stage,
                command_text=command_text,
            )
            return session_module.SessionResult(
                request=request,
                host_platform=host_platform,
                status="ok",
                suite_results=[{"suiteId": request.suite_key, "status": "ok", "stageResults": {}}],
                text=f"{request.suite_key} ok\n",
                artifacts=[],
                exit_code=0,
            )

        with patch.object(test_module, "_execute_public_test_session", side_effect=fake_session):
            result = test_module.handle(
                {"id": "test-system", "handler": "test.dispatch"},
                REPO_ROOT,
                "windows",
                "test system --id system/windows-reference-gate",
                manifest,
                {"id": "system/windows-reference-gate"},
            )

        self.assertEqual("ok", result.status)
        self.assertEqual("system/windows-reference-gate", result.target)
        self.assertEqual("system/windows-reference-gate", result.payload["selectedObject"]["id"])
        self.assertEqual(
            "run test system --id system/windows-reference-gate",
            result.payload["selectedObject"]["canonicalCommand"],
        )
        self.assertEqual(
            ["gate/windows-reference-desktop"],
            [item["id"] for item in result.payload["items"]],
        )

    def test_macos_reference_gate_system_dispatch_expands_registered_plan(self) -> None:
        test_module = load_module(TEST_COMMAND_MODULE_PATH, "chaos_run_test_command_macos_reference_gate_system_dispatch")
        manifest_module = load_module(MANIFEST_MODULE_PATH, "chaos_run_manifest_macos_reference_gate_system_dispatch")
        session_module = load_module(SESSION_MODULE_PATH, "chaos_run_session_macos_reference_gate_system_dispatch")
        manifest = manifest_module.load_run_manifest(REPO_ROOT, RUN_MANIFEST_PATH)

        def fake_session(
            family: str,
            suite: str,
            stage: str,
            repo_root: Path,
            host_platform: str,
            command_text: str,
            manifest_payload: dict,
        ):
            del repo_root
            del manifest_payload
            request = session_module.TestRequest(
                family=family,
                suite=suite,
                stage=stage,
                command_text=command_text,
            )
            return session_module.SessionResult(
                request=request,
                host_platform=host_platform,
                status="ok",
                suite_results=[{"suiteId": request.suite_key, "status": "ok", "stageResults": {}}],
                text=f"{request.suite_key} ok\n",
                artifacts=[],
                exit_code=0,
            )

        with patch.object(test_module, "_execute_public_test_session", side_effect=fake_session):
            result = test_module.handle(
                {"id": "test-system", "handler": "test.dispatch"},
                REPO_ROOT,
                "macos",
                "test system --id system/macos-reference-gate",
                manifest,
                {"id": "system/macos-reference-gate"},
            )

        self.assertEqual("ok", result.status)
        self.assertEqual("system/macos-reference-gate", result.target)
        self.assertEqual("system/macos-reference-gate", result.payload["selectedObject"]["id"])
        self.assertEqual(
            "run test system --id system/macos-reference-gate",
            result.payload["selectedObject"]["canonicalCommand"],
        )
        self.assertEqual(
            ["gate/macos-reference-desktop"],
            [item["id"] for item in result.payload["items"]],
        )

    def test_missing_registry_object_returns_structured_hint(self) -> None:
        test_module = load_module(TEST_COMMAND_MODULE_PATH, "chaos_run_test_command_registry_missing")
        manifest_module = load_module(MANIFEST_MODULE_PATH, "chaos_run_manifest_registry_missing")
        manifest = manifest_module.load_run_manifest(REPO_ROOT, RUN_MANIFEST_PATH)

        result = test_module.handle(
            {"id": "test-module", "handler": "test.dispatch"},
            REPO_ROOT,
            "macos",
            "test module --id module/missing/basic",
            manifest,
            {"id": "module/missing/basic"},
        )

        self.assertEqual("error", result.status)
        self.assertEqual("test.registry.object_not_found", result.payload["failureCode"])
        self.assertIn("run test registry list --json", result.payload["parserHint"])
        self.assertIn("module/missing/basic", result.payload["humanHint"])
