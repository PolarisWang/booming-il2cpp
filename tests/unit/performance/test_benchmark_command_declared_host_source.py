from tests.unit.performance.benchmark_command_test_support import *


class TestBenchmarkCommandDeclaredHostSource(BenchmarkCommandTestSupport):
    def test_declared_benchmark_host_source_entry_uses_workspace_benchmark_host_assembly_name(self) -> None:
        benchmark_module = load_module(BENCHMARK_MODULE_PATH, "chaos_benchmark_command_benchmark_host_source_entry")
        repo_root = self._make_repo_root("benchmark-host-source-entry")
        try:
            self._write_workspace_benchmark_fixture(
                repo_root,
                subject_id="SolutionCorePack",
                stable_id="solution-core::arith",
                alias="arithmetic-bench",
                entry_index=11,
                assembly_name="CoreRuntimeBenchmarks",
                declaring_type="CoreRuntimeBenchmarks.ArithmeticBenchmarkEntry",
                method_name="RunWorkload",
                method_signature="RunWorkload()",
            )

            source_entry = benchmark_module._declared_benchmark_host_source_entry(
                repo_root,
                "SolutionCorePack",
            )

            self.assertEqual(
                "SolutionCorePack.DeclaredBenchmarkHost/"
                "Chaos.Generated.ManagedTests.SolutionCorePack.SolutionCorePackDeclaredBenchmarkHost::Execute(System.Int32)",
                source_entry,
            )
        finally:
            shutil.rmtree(repo_root, ignore_errors=True)

    def test_declared_benchmark_host_source_entry_prefers_native_test_managed_host_for_native_matrix(self) -> None:
        benchmark_module = load_module(BENCHMARK_MODULE_PATH, "chaos_benchmark_command_native_benchmark_host_source_entry")
        repo_root = self._make_repo_root("native-benchmark-host-source-entry")
        try:
            self._write_workspace_benchmark_fixture(
                repo_root,
                subject_id="SolutionCorePack",
                stable_id="solution-core::arith",
                alias="arithmetic-bench",
                entry_index=11,
                assembly_name="CoreRuntimeBenchmarks",
                declaring_type="CoreRuntimeBenchmarks.ArithmeticBenchmarkEntry",
                method_name="RunWorkload",
                method_signature="RunWorkload()",
                matrix_id="windows-native-perf",
                native_host_assembly_name="SolutionCorePack.DeclaredBenchmarkNativeHost",
            )

            source_entry = benchmark_module._declared_benchmark_host_source_entry(
                repo_root,
                "SolutionCorePack",
                matrix_id="windows-native-perf",
            )

            self.assertEqual(
                "SolutionCorePack.DeclaredBenchmarkNativeHost/"
                "Chaos.Generated.ManagedTests.SolutionCorePack.SolutionCorePackDeclaredBenchmarkHost::Execute(System.Int32)",
                source_entry,
            )
        finally:
            shutil.rmtree(repo_root, ignore_errors=True)

    def test_declared_benchmark_host_source_entry_refreshes_workspace_when_native_binding_is_missing(self) -> None:
        benchmark_module = load_module(BENCHMARK_MODULE_PATH, "chaos_benchmark_command_refresh_native_benchmark_host_source_entry")
        repo_root = self._make_repo_root("refresh-native-benchmark-host-source-entry")
        regenerate_calls: list[tuple[str, str, dict[str, object]]] = []
        try:
            self._write_workspace_benchmark_fixture(
                repo_root,
                subject_id="SolutionCorePack",
                stable_id="solution-core::arith",
                alias="arithmetic-bench",
                entry_index=11,
                assembly_name="CoreRuntimeBenchmarks",
                declaring_type="CoreRuntimeBenchmarks.ArithmeticBenchmarkEntry",
                method_name="RunWorkload",
                method_signature="RunWorkload()",
                matrix_id="windows-native-perf",
            )

            class FakeProjectWorkspaceModule:
                @staticmethod
                def generate_subject_workspace(repo_root: Path, host_platform: str, options: dict[str, object], *, progress_callback=None):
                    del progress_callback
                    regenerate_calls.append((str(repo_root), host_platform, dict(options)))
                    self._write_workspace_benchmark_fixture(
                        repo_root,
                        subject_id="SolutionCorePack",
                        stable_id="solution-core::arith",
                        alias="arithmetic-bench",
                        entry_index=11,
                        assembly_name="CoreRuntimeBenchmarks",
                        declaring_type="CoreRuntimeBenchmarks.ArithmeticBenchmarkEntry",
                        method_name="RunWorkload",
                        method_signature="RunWorkload()",
                        matrix_id="windows-native-perf",
                        native_host_assembly_name="SolutionCorePack.DeclaredBenchmarkNativeHost",
                    )
                    return {}

            source_entry = benchmark_module._declared_benchmark_host_source_entry(
                repo_root,
                "SolutionCorePack",
                matrix_id="windows-native-perf",
                host_platform="windows-x64",
                project_workspace_module=FakeProjectWorkspaceModule(),
            )

            self.assertEqual(1, len(regenerate_calls))
            self.assertEqual(
                (
                    str(repo_root),
                    "windows",
                    {
                        "id": "subject/SolutionCorePack",
                        "matrix": "windows-native-perf",
                        "variant": "PROFILE",
                        "auto-refresh-missing-generated": True,
                    },
                ),
                regenerate_calls[0],
            )
            self.assertEqual(
                "SolutionCorePack.DeclaredBenchmarkNativeHost/"
                "Chaos.Generated.ManagedTests.SolutionCorePack.SolutionCorePackDeclaredBenchmarkHost::Execute(System.Int32)",
                source_entry,
            )
        finally:
            shutil.rmtree(repo_root, ignore_errors=True)
