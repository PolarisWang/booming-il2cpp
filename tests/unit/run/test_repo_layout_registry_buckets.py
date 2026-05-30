from tests.unit.run.repo_layout_test_support import *


class RepoLayoutRegistryBucketsTests(RepoLayoutTestSupport):
    def test_phase1_registry_buckets_and_integration_fixtures_exist(self) -> None:
        integration_fixture_root = REPO_ROOT / "tests" / "fixtures" / "integration" / "catalog" / "repo"
        unit_registry_root = REPO_ROOT / "tests" / "unit" / "registry"
        integration_registry_root = REPO_ROOT / "tests" / "integration" / "registry"

        self.assertTrue(integration_fixture_root.is_dir(), msg=f"missing integration fixture root: {integration_fixture_root}")
        self.assertFalse((REPO_ROOT / "tests" / "integration" / "run" / "fixtures").exists())
        self.assertTrue((unit_registry_root / "test_adapter_registry.py").is_file())
        self.assertTrue((unit_registry_root / "test_suite_manifest.py").is_file())
        self.assertTrue((integration_registry_root / "test_catalog_scan.py").is_file())
        self.assertTrue((integration_registry_root / "test_case_discovery.py").is_file())
        self.assertNotEqual(
            [],
            list(integration_registry_root.glob("test_registry_scan*.py")),
        )

    def test_phase8_platform_hosts_use_integration_canonical_root_with_gate_wrappers_only(self) -> None:
        cmake_router = (REPO_ROOT / "CMakeLists.txt").read_text(encoding="utf-8")
        canonical_root = REPO_ROOT / "tests" / "integration" / "platform-hosts"

        self.assertIn("tests/integration/platform-hosts/android-smoke", cmake_router)
        self.assertIn("tests/integration/platform-hosts/ios-smoke", cmake_router)
        self.assertIn("tests/integration/platform-hosts/linux-packaging", cmake_router)
        self.assertNotIn("tests/gate/android-smoke", cmake_router)
        self.assertNotIn("tests/gate/ios-smoke", cmake_router)
        self.assertNotIn("tests/gate/linux-packaging", cmake_router)

        for relative_dir in ["android-smoke", "ios-smoke", "linux-packaging"]:
            self.assertTrue((canonical_root / relative_dir / "CMakeLists.txt").is_file())

        gate_wrapper = (REPO_ROOT / "tests" / "gate" / "android-smoke" / "CMakeLists.txt").read_text(encoding="utf-8")
        self.assertIn("../../integration/platform-hosts/android-smoke", gate_wrapper)
        self.assertFalse((REPO_ROOT / "tests" / "platform").exists())

    def test_phase8_registry_manifests_use_fixtures_root_only(self) -> None:
        fixture_registry_root = REPO_ROOT / "tests" / "fixtures" / "registry"

        self.assertTrue((fixture_registry_root / "modules").is_dir())
        self.assertTrue((fixture_registry_root / "systems").is_dir())
        self.assertTrue((fixture_registry_root / "pipelines").is_dir())
        self.assertTrue((fixture_registry_root / "systems" / "runtime-baseline-macos" / "scenario.manifest.json").is_file())
        self.assertTrue((fixture_registry_root / "systems" / "runtime-baseline-windows" / "scenario.manifest.json").is_file())
        self.assertEqual(
            [],
            [path.name for path in (fixture_registry_root / "systems").iterdir() if path.is_dir() and path.name.startswith("roadmap-0-")],
        )
        self.assertFalse((REPO_ROOT / "tests" / "registry").exists())

    def test_phase1_generic_unit_buckets_exist(self) -> None:
        self.assertNotEqual([], list((REPO_ROOT / "tests" / "unit" / "execution").glob("test_subject_executor*.py")))
        self.assertNotEqual([], list((REPO_ROOT / "tests" / "unit" / "reporting").glob("test_subject_reporting*.py")))
        self.assertTrue((REPO_ROOT / "tests" / "unit" / "reporting" / "test_events_schema.py").is_file())
        self.assertNotEqual([], list((REPO_ROOT / "tests" / "unit" / "performance").glob("test_benchmark_command*.py")))
        self.assertNotEqual([], list((REPO_ROOT / "tests" / "unit" / "planning").glob("test_subject_planner*.py")))
        self.assertNotEqual([], list((REPO_ROOT / "tests" / "unit" / "planning").glob("test_project_workspace*.py")))
        self.assertTrue((REPO_ROOT / "tests" / "unit" / "planning" / "test_fingerprints.py").is_file())
        self.assertTrue((REPO_ROOT / "tests" / "unit" / "planning" / "test_project_graph.py").is_file())
        self.assertTrue((REPO_ROOT / "tests" / "unit" / "selection" / "test_path_resolver.py").is_file())
        self.assertTrue((REPO_ROOT / "tests" / "unit" / "selection" / "test_session.py").is_file())
        self.assertTrue((REPO_ROOT / "tests" / "unit" / "compatibility" / "test_managed_closure_contract_bundle.py").is_file())
        self.assertTrue((REPO_ROOT / "tests" / "unit" / "compatibility" / "test_native_reference_bootstrap_support.py").is_file())
        self.assertTrue((REPO_ROOT / "tests" / "unit" / "compatibility" / "test_native_runtime_core.py").is_file())
        self.assertNotEqual([], list((REPO_ROOT / "tests" / "unit" / "compatibility").glob("test_subject_manifest_schema*.py")))

        compatibility_root = REPO_ROOT / "tests" / "unit" / "compatibility"
        for file_name in MIGRATED_COMPATIBILITY_TESTS:
            file_stem = Path(file_name).stem
            self.assertNotEqual([], list(compatibility_root.glob(f"{file_stem}*.py")), msg=file_name)

        for file_name in MIGRATED_PERFORMANCE_TESTS:
            self.assertTrue((REPO_ROOT / "tests" / "unit" / "performance" / file_name).is_file(), msg=file_name)

    def test_phase5_run_bucket_keeps_only_repo_layout(self) -> None:
        entries = sorted(path.name for path in (REPO_ROOT / "tests" / "unit" / "run").glob("test_*.py"))
        self.assertIn("test_test_command_workspace.py", entries)
        self.assertNotEqual([], [name for name in entries if name.startswith("test_repo_layout")])
        self.assertEqual(
            [],
            [name for name in entries if not (name.startswith("test_repo_layout") or name == "test_test_command_workspace.py")],
        )

    def test_phase5_integration_run_bucket_does_not_keep_registry_shims(self) -> None:
        for file_name in ["test_case_discovery.py", "test_catalog_scan.py", "test_registry_scan.py"]:
            self.assertFalse((REPO_ROOT / "tests" / "integration" / "run" / file_name).exists(), msg=file_name)

    def test_phase1_root_pytest_collection_is_scoped_to_repo_tests(self) -> None:
        pytest_ini = REPO_ROOT / "pytest.ini"
        self.assertTrue(pytest_ini.is_file(), msg="pytest.ini must exist at repo root")

        content = pytest_ini.read_text(encoding="utf-8")
        for snippet in EXPECTED_PYTEST_INI_SNIPPETS:
            self.assertIn(snippet, content)
