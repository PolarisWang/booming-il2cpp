from tests.unit.performance.benchmark_command_test_support import *


class TestBenchmarkCommandDeclaredPipeline(BenchmarkCommandTestSupport):
    def test_run_subject_benchmark_pipeline_with_declared_case_override_uses_custom_workload_entry(self) -> None:
        benchmark_module = load_module(BENCHMARK_MODULE_PATH, "chaos_benchmark_command_declared_case")

        class FakeSubjectsModule:
            @staticmethod
            def load_subject_manifest(repo_root: Path, subject_id: str) -> dict[str, object]:
                del repo_root
                return {
                    "subjectId": subject_id,
                    "environmentMatrices": [
                        {
                            "matrixId": "windows-managed-perf",
                            "pipelineId": "managed-benchmark",
                            "supportedGoals": ["perf.release"],
                            "executionContext": {
                                "hostPlatform": "windows-x64",
                                "runtimeProfile": "managed-perf-release",
                            },
                        },
                    ],
                    "executionPipelines": [
                        {
                            "pipelineId": "managed-benchmark",
                            "stages": [
                                {
                                    "stageId": "runtime-perf-collect",
                                    "kind": "runtime-perf-collect",
                                    "scope": "matrix",
                                    "bucket": "runtime",
                                    "dependsOn": [],
                                }
                            ],
                        },
                    ],
                }

        class FakePlannerModule:
            def __init__(self) -> None:
                self.calls: list[dict[str, object]] = []

            def build_plan(self, repo_root: Path, subject_id: str, **kwargs):
                self.calls.append(
                    {
                        "repo_root": repo_root,
                        "subject_id": subject_id,
                        **kwargs,
                    }
                )
                return {
                    "selection": {
                        "subjectId": subject_id,
                        "matrixId": kwargs["matrix_id"],
                        "goalId": "perf.release",
                        "artifactPlan": {"evidenceTerminalBucket": "runtime"},
                    },
                    "stagePlan": [],
                }

        class FakeExecutorModule:
            def __init__(self) -> None:
                self.calls: list[dict[str, object]] = []

            def execute_plan(self, repo_root: Path, plan: dict[str, object], *, run_id: str | None = None):
                self.calls.append(
                    {
                        "repo_root": repo_root,
                        "plan": plan,
                        "run_id": run_id,
                    }
                )
                return {
                    "status": "ok",
                    "errors": [],
                    "stageResults": [
                        {
                            "stageId": "runtime-perf-collect",
                            "kind": "runtime-perf-collect",
                            "bucket": "runtime",
                            "details": {
                                "performance": {
                                    "metrics": {
                                        "meanDurationMs": 3.25,
                                        "meanOpsPerSecond": 3200,
                                    },
                                    "regressionStatus": "ok",
                                }
                            },
                        }
                    ],
                }

        planner_module = FakePlannerModule()
        executor_module = FakeExecutorModule()
        benchmark_case = {
            "stableId": "bench-arithmetic",
            "alias": "arithmetic-bench",
            "entryIndex": 7,
            "workloadEntry": "CoreRuntimeBenchmarks/ArithmeticBenchmarkEntry::RunWorkload()",
        }

        result = benchmark_module._run_subject_benchmark_pipeline(
            repo_root=REPO_ROOT,
            subject_id="SolutionCorePack",
            mode="managed",
            host_platform="windows",
            subjects_module=FakeSubjectsModule(),
            planner_module=planner_module,
            executor_module=executor_module,
            benchmark_case=benchmark_case,
        )

        self.assertEqual(
            "windows-managed-perf",
            planner_module.calls[0]["matrix_id"],
        )
        self.assertIsNone(planner_module.calls[0]["source_entry"])
        self.assertEqual(
            benchmark_case["workloadEntry"],
            planner_module.calls[0]["workload_entry"],
        )
        self.assertEqual(
            {
                "family": "declared-benchmark",
                "stableId": "bench-arithmetic",
                "alias": "arithmetic-bench",
                "entryIndex": 7,
            },
            planner_module.calls[0]["entry_selection"],
        )
        self.assertEqual(
            {"meanDurationMs": 3.25, "meanOpsPerSecond": 3200},
            result["metrics"],
        )
        self.assertFalse(bool(result["regressionFound"]))
