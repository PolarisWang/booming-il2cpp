from tests.tooling.run.registry_command_test_support import *


class TestRegistryCommandModuleDispatch(RegistryCommandTestSupport):
    def test_module_dispatch_expands_registered_plan(self) -> None:
        test_module = load_module(TEST_COMMAND_MODULE_PATH, "chaos_run_test_command_module_dispatch")
        manifest_module = load_module(MANIFEST_MODULE_PATH, "chaos_run_manifest_module_dispatch")
        session_module = load_module(SESSION_MODULE_PATH, "chaos_run_session_module_dispatch")
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
                {"id": "test-module", "handler": "test.dispatch"},
                REPO_ROOT,
                "macos",
                "test module --id module/analysis/basic",
                manifest,
                {"id": "module/analysis/basic"},
            )

        self.assertEqual("ok", result.status)
        self.assertEqual("module/analysis/basic", result.target)
        self.assertEqual("module/analysis/basic", result.payload["selectedObject"]["id"])
        self.assertEqual(
            ["contract/analysis-schema"],
            [item["id"] for item in result.payload["items"]],
        )

    def test_managed_closure_module_dispatch_expands_contract_plan(self) -> None:
        test_module = load_module(TEST_COMMAND_MODULE_PATH, "chaos_run_test_command_managed_closure_module_dispatch")
        manifest_module = load_module(MANIFEST_MODULE_PATH, "chaos_run_manifest_managed_closure_module_dispatch")
        session_module = load_module(SESSION_MODULE_PATH, "chaos_run_session_managed_closure_module_dispatch")
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
                {"id": "test-module", "handler": "test.dispatch"},
                REPO_ROOT,
                "macos",
                "test module --id module/managed-closure/basic",
                manifest,
                {"id": "module/managed-closure/basic"},
            )

        self.assertEqual("ok", result.status)
        self.assertEqual("module/managed-closure/basic", result.target)
        self.assertEqual("module/managed-closure/basic", result.payload["selectedObject"]["id"])
        self.assertEqual(
            ["contract/managed-closure-bundle"],
            [item["id"] for item in result.payload["items"]],
        )

    def test_interop_module_dispatch_expands_contract_and_runtime_plan(self) -> None:
        test_module = load_module(TEST_COMMAND_MODULE_PATH, "chaos_run_test_command_interop_module_dispatch")
        manifest_module = load_module(MANIFEST_MODULE_PATH, "chaos_run_manifest_interop_module_dispatch")
        session_module = load_module(SESSION_MODULE_PATH, "chaos_run_session_interop_module_dispatch")
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
                {"id": "test-module", "handler": "test.dispatch"},
                REPO_ROOT,
                "macos",
                "test module --id module/interop/basic",
                manifest,
                {"id": "module/interop/basic"},
            )

        self.assertEqual("ok", result.status)
        runtime_suite = select_public_suite_spec(
            "chaos_registry_interop_runtime_suite",
            host_platform="macos",
            family="smoke",
            required_module_ids=["interop"],
        )
        self.assertEqual(
            ["contract/native-abi", "contract/native-bridge", str(runtime_suite["id"])],
            [item["id"] for item in result.payload["items"]],
        )

    def test_trace_export_module_dispatch_includes_trace_schema_contract(self) -> None:
        test_module = load_module(TEST_COMMAND_MODULE_PATH, "chaos_run_test_command_trace_export_module_dispatch")
        manifest_module = load_module(MANIFEST_MODULE_PATH, "chaos_run_manifest_trace_export_module_dispatch")
        session_module = load_module(SESSION_MODULE_PATH, "chaos_run_session_trace_export_module_dispatch")
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
                {"id": "test-module", "handler": "test.dispatch"},
                REPO_ROOT,
                "macos",
                "test module --id module/trace-export/macos",
                manifest,
                {"id": "module/trace-export/macos"},
            )

        self.assertEqual("ok", result.status)
        self.assertEqual(
            ["contract/trace-schema", "contract/trace-compare-macos"],
            [item["id"] for item in result.payload["items"]],
        )
