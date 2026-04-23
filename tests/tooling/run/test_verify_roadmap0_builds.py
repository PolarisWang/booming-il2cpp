from tests.tooling.run.verify_roadmap0_test_support import *


class VerifyRoadmap0BuildTests(VerifyRoadmap0TestSupport):
    def test_low_level_script_allocates_run_scoped_binary_dir(self) -> None:
        script_module = load_module(VERIFY_SCRIPT_PATH, "chaos_verify_roadmap0_script_paths")
        base_dir = REPO_ROOT / "artifacts" / "verify-runtime-baseline" / "windows" / "common" / "native-abi-config"

        first = script_module.allocate_run_scoped_binary_dir(base_dir)
        second = script_module.allocate_run_scoped_binary_dir(base_dir)

        self.assertNotEqual(first, second)
        self.assertEqual(base_dir.parent, first.parent)
        self.assertTrue(first.name.startswith("native-abi-config-"))
        self.assertTrue(second.name.startswith("native-abi-config-"))

    def test_low_level_script_native_smoke_uses_run_scoped_binary_dir(self) -> None:
        script_module = load_module(VERIFY_SCRIPT_PATH, "chaos_verify_roadmap0_script_native")
        source_dir = REPO_ROOT / "tests" / "contracts" / "native" / "abi"
        requested_dir = REPO_ROOT / "artifacts" / "verify-runtime-baseline" / "windows" / "common" / "native-abi-config"
        allocated_dir = requested_dir.parent / "native-abi-config-test-run"

        with patch.object(script_module, "allocate_run_scoped_binary_dir", return_value=allocated_dir):
            with patch.object(script_module, "run_checked") as run_checked_mock:
                script_module.invoke_native_smoke_build(source_dir, requested_dir, REPO_ROOT)

        self.assertEqual(
            ["cmake", "-S", str(source_dir), "-B", str(allocated_dir)],
            run_checked_mock.call_args_list[0].args[0],
        )
        self.assertEqual(
            ["cmake", "--build", str(allocated_dir)],
            run_checked_mock.call_args_list[1].args[0],
        )

    def test_low_level_script_native_smoke_uses_visual_studio_compatible_binary_dir_on_windows(self) -> None:
        script_module = load_module(VERIFY_SCRIPT_PATH, "chaos_verify_roadmap0_script_native_windows_vs")
        source_dir = REPO_ROOT / "tests" / "contracts" / "native" / "abi"
        requested_dir = REPO_ROOT / "artifacts" / "verify-runtime-baseline" / "windows" / "common" / "native-abi-config"
        allocated_dir = TEST_TMP_ROOT / "cmake-builds" / "native-abi-config-1234"
        instance_spec = f"{TEST_TMP_ROOT / 'visual-studio' / '18' / 'Professional'},version=18.4.11626.88"

        with patch.object(script_module.tooling_module, "detect_visual_studio_generator", return_value="Visual Studio 18 2026"):
            with patch.object(script_module.tooling_module, "detect_visual_studio_instance_spec", return_value=instance_spec):
                with patch.object(script_module.tooling_module, "allocate_cmake_binary_dir", return_value=allocated_dir) as allocate_mock:
                    with patch.object(script_module, "run_checked") as run_checked_mock:
                        script_module.invoke_native_smoke_build(
                            source_dir,
                            requested_dir,
                            REPO_ROOT,
                            host_profile="windows",
                        )

        allocate_mock.assert_called_once_with(
            requested_dir,
            host_platform="windows",
            generator="Visual Studio 18 2026",
        )
        self.assertEqual(
            [
                "cmake",
                "-S",
                str(source_dir),
                "-B",
                str(allocated_dir),
                "-G",
                "Visual Studio 18 2026",
                f"-DCMAKE_GENERATOR_INSTANCE={instance_spec}",
            ],
            run_checked_mock.call_args_list[0].args[0],
        )

    def test_low_level_script_preset_build_uses_run_scoped_binary_dir(self) -> None:
        script_module = load_module(VERIFY_SCRIPT_PATH, "chaos_verify_roadmap0_script_preset")
        requested_dir = REPO_ROOT / "artifacts" / "presets" / "windows-x64-reference"
        allocated_dir = requested_dir.parent / "windows-x64-reference-test-run"

        with patch.object(script_module, "allocate_run_scoped_binary_dir", return_value=allocated_dir):
            with patch.object(script_module, "run_checked") as run_checked_mock:
                script_module.invoke_preset_build_smoke("windows-x64-reference", REPO_ROOT)

        self.assertEqual(
            ["cmake", "--preset", "windows-x64-reference", "-B", str(allocated_dir)],
            run_checked_mock.call_args_list[0].args[0],
        )
        self.assertEqual(
            ["cmake", "--build", str(allocated_dir)],
            run_checked_mock.call_args_list[1].args[0],
        )

    def test_low_level_script_dotnet_build_uses_temp_intermediate_root_on_windows(self) -> None:
        script_module = load_module(VERIFY_SCRIPT_PATH, "chaos_verify_roadmap0_script_dotnet_build")
        smoke_spec = select_public_suite_spec(
            "chaos_verify_roadmap0_dotnet_build_suite",
            host_platform="windows",
            family="smoke",
            required_stages=["build"],
        )
        suite_name = str(smoke_spec["suite"])
        project_path = REPO_ROOT / str(dict(smoke_spec["execution"])["project_path"])
        intermediate_root = TEST_TMP_ROOT / "dotnet-intermediates" / f"{suite_name}-1234"

        with patch.object(script_module.tooling_module, "allocate_dotnet_intermediate_dir", return_value=intermediate_root):
            with patch.object(script_module, "run_checked") as run_checked_mock:
                script_module.invoke_dotnet_build(project_path, REPO_ROOT, host_profile="windows")

        self.assertEqual(
            [
                "dotnet",
                "build",
                str(project_path),
                "-c",
                "Release",
                f"-p:BaseIntermediateOutputPath={intermediate_root.as_posix()}/$(MSBuildProjectName)/",
                f"-p:MSBuildProjectExtensionsPath={intermediate_root.as_posix()}/$(MSBuildProjectName)/",
            ],
            run_checked_mock.call_args.args[0],
        )

    def test_low_level_script_routing_build_uses_run_scoped_binary_dir(self) -> None:
        script_module = load_module(VERIFY_SCRIPT_PATH, "chaos_verify_roadmap0_script_routing")
        requested_dir = REPO_ROOT / "artifacts" / "verify-runtime-baseline" / "windows" / "common" / "linux-packaging-routing"
        allocated_dir = requested_dir.parent / "linux-packaging-routing-test-run"

        with patch.object(script_module, "allocate_run_scoped_binary_dir", return_value=allocated_dir):
            with patch.object(script_module, "run_checked") as run_checked_mock:
                script_module.invoke_routing_build_smoke(
                    "linux-x64-packaging",
                    REPO_ROOT / "build" / "toolchains" / "linux-x64.cmake",
                    requested_dir,
                    "Ninja",
                    REPO_ROOT,
                )

        self.assertEqual(
            [
                "cmake",
                "-S",
                str(REPO_ROOT),
                "-B",
                str(allocated_dir),
                "-G",
                "Ninja",
                "-DROADMAP0_PRESET_TARGET=linux-x64-packaging",
                "-DROADMAP0_TOOLCHAIN_VALIDATE_ONLY=ON",
                f"-DCMAKE_TOOLCHAIN_FILE={REPO_ROOT / 'build' / 'toolchains' / 'linux-x64.cmake'}",
            ],
            run_checked_mock.call_args_list[0].args[0],
        )
        self.assertEqual(
            ["cmake", "--build", str(allocated_dir)],
            run_checked_mock.call_args_list[1].args[0],
        )
