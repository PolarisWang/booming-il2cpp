from tests.unit.performance.subject_workers_perf_test_support import *


class TestSubjectWorkersPerfCommands(SubjectWorkersPerfTestSupport):
    def test_perf_harness_command_can_use_declared_benchmark_metadata_instead_of_workload_entry(self) -> None:
        workers_module = load_module(SUBJECT_WORKERS_MODULE_PATH, "chaos_subject_workers_perf_harness_declared_metadata")

        command = workers_module._perf_harness_command(
            harness_dll_path=Path("artifacts/harness/Chaos.IL2CPP.Tools.Benchmark.WorkloadEntry.PerfHarness.dll"),
            iterations=7,
            assembly_path=Path("artifacts/subjects/SolutionCorePack/CoreRuntimeBenchmarks.dll"),
            workload_entry="CoreRuntimeBenchmarks/ArithmeticBenchmarkEntry::RunWorkload()",
            mode="managed",
            declared_benchmark={
                "stableId": "SolutionCorePack::CoreRuntimeBenchmarks::CoreRuntimeBenchmarks.ArithmeticBenchmarkEntry::RunWorkload()",
                "entryIndex": 11,
                "assemblyName": "CoreRuntimeBenchmarks",
                "declaringType": "CoreRuntimeBenchmarks.ArithmeticBenchmarkEntry",
                "methodName": "RunWorkload",
                "methodSignature": "RunWorkload()",
            },
        )

        self.assertEqual(
            [
                "dotnet",
                str(Path("artifacts/harness/Chaos.IL2CPP.Tools.Benchmark.WorkloadEntry.PerfHarness.dll")),
                "7",
                "--assembly",
                str(Path("artifacts/subjects/SolutionCorePack/CoreRuntimeBenchmarks.dll")),
                "--assembly-name",
                "CoreRuntimeBenchmarks",
                "--declaring-type",
                "CoreRuntimeBenchmarks.ArithmeticBenchmarkEntry",
                "--method-name",
                "RunWorkload",
                "--method-signature",
                "RunWorkload()",
                "--mode",
                "managed",
            ],
            command,
        )

    def test_selection_declared_entry_selection_preserves_entry_index_for_declared_benchmark(self) -> None:
        workers_module = load_module(SUBJECT_WORKERS_MODULE_PATH, "chaos_subject_workers_declared_entry_selection_entry_index")

        selection = workers_module._selection_declared_entry_selection(
            {
                "entrySelection": {
                    "family": "declared-benchmark",
                    "stableId": "solution-core::arith",
                    "alias": "arithmetic-bench",
                    "entryIndex": 11,
                }
            }
        )

        self.assertEqual(
            {
                "family": "declared-benchmark",
                "stableId": "solution-core::arith",
                "alias": "arithmetic-bench",
                "entryIndex": 11,
            },
            selection,
        )

    def test_native_perf_command_can_include_entry_index(self) -> None:
        workers_module = load_module(SUBJECT_WORKERS_MODULE_PATH, "chaos_subject_workers_native_perf_command_entry_index")

        command = workers_module._native_perf_command(
            native_executable_path=Path("artifacts/subjects/SolutionCorePack/native/chaos_subject_native_aot.exe"),
            iterations=7,
            entry_index=11,
        )

        self.assertEqual(
            [
                str(Path("artifacts/subjects/SolutionCorePack/native/chaos_subject_native_aot.exe")),
                "--iterations",
                "7",
                "--entry-index",
                "11",
            ],
            command,
        )
