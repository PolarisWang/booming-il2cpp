from tests.unit.performance.benchmark_dashboard_generator_test_support import *


class TestBenchmarkDashboardGeneratorLegacy(BenchmarkDashboardGeneratorTestSupport):
    def test_update_docs_filters_out_historical_cases_not_in_current_declared_catalog(self) -> None:
        generator_module = load_module(
            BENCHMARK_DASHBOARD_GENERATOR_MODULE_PATH,
            "chaos_benchmark_dashboard_generator_declared_case_filter",
        )
        repo_root = self._make_repo_root()
        docs_root = repo_root / "docs" / "benchmark"
        workload_entry = "CoreRuntimeBenchmarks/ArithmeticBenchmarkEntry::RunWorkload()"

        try:
            self._write_testing_support(repo_root)
            self._write_compiled_catalog(
                repo_root,
                declared_benchmarks=[
                    {
                        "stableId": "arith",
                        "alias": "arithmetic-bench",
                        "assemblyName": "CoreRuntimeBenchmarks",
                        "declaringType": "CoreRuntimeBenchmarks.ArithmeticBenchmarkEntry",
                        "methodName": "RunWorkload",
                        "methodSignature": "RunWorkload()",
                        "category": 1,
                        "metrics": 3,
                        "modes": 3,
                    }
                ],
            )
            self._write_subject_manifest(
                repo_root,
                subject_id="SolutionCorePack",
                supported_modes=["managed", "native"],
                workload_entry=workload_entry,
            )
            self._write_record(
                repo_root,
                subject_id="SolutionCorePack",
                mode="managed",
                mean_duration_ms=8.0,
                mean_ops_per_second=1000.0,
            )
            self._write_record(
                repo_root,
                subject_id="SolutionCorePack",
                mode="native",
                mean_duration_ms=4.0,
                mean_ops_per_second=2000.0,
            )
            self._write_record(
                repo_root,
                subject_id="SolutionCorePack",
                mode="managed",
                mean_duration_ms=2.0,
                mean_ops_per_second=5000.0,
                benchmark_case={
                    "stableId": "arith",
                    "alias": "arithmetic-bench",
                    "workloadEntry": workload_entry,
                },
            )
            self._write_record(
                repo_root,
                subject_id="SolutionCorePack",
                mode="native",
                mean_duration_ms=1.0,
                mean_ops_per_second=10000.0,
                benchmark_case={
                    "stableId": "arith",
                    "alias": "arithmetic-bench",
                    "workloadEntry": workload_entry,
                },
            )
            self._write_record(
                repo_root,
                subject_id="SolutionCorePack",
                mode="managed",
                mean_duration_ms=7.5,
                mean_ops_per_second=900.0,
                benchmark_case={
                    "stableId": "legacy-dispatch",
                    "alias": "legacy-dispatch-bench",
                    "workloadEntry": "PerformanceFeaturePack/LegacyDispatchBenchmarkEntry::RunWorkload()",
                },
            )

            generator_module.update_docs(repo_root)

            subject_payload = json.loads((docs_root / "subjects" / "SolutionCorePack.json").read_text(encoding="utf-8"))
            case_payload = subject_payload["benchmarkCasesByDevice"]["windows-x64-test-device"]
            self.assertEqual({"arith"}, set(case_payload))
            self.assertEqual({"arith"}, set(subject_payload["declaredBenchmarkCases"]))

            dashboard_html = (docs_root / "dashboard.html").read_text(encoding="utf-8")
            self.assertIn("arithmetic-bench", dashboard_html)
            self.assertNotIn("legacy-dispatch-bench", dashboard_html)
            self.assertNotIn("legacy-dispatch", dashboard_html)
        finally:
            shutil.rmtree(repo_root, ignore_errors=True)

    def test_update_docs_rekeys_legacy_record_identity_into_declared_case_by_alias(self) -> None:
        generator_module = load_module(
            BENCHMARK_DASHBOARD_GENERATOR_MODULE_PATH,
            "chaos_benchmark_dashboard_generator_declared_case_rekey",
        )
        repo_root = self._make_repo_root()
        docs_root = repo_root / "docs" / "benchmark"
        workload_entry = "CoreRuntimeBenchmarks/ArithmeticBenchmarkEntry::RunWorkload()"
        case_id = "SolutionCorePack::CoreRuntimeBenchmarks::CoreRuntimeBenchmarks.ArithmeticBenchmarkEntry::RunWorkload()"

        try:
            self._write_testing_support(repo_root)
            self._write_compiled_catalog(
                repo_root,
                declared_benchmarks=[
                    {
                        "stableId": case_id,
                        "alias": "arithmetic-bench",
                        "assemblyName": "CoreRuntimeBenchmarks",
                        "declaringType": "CoreRuntimeBenchmarks.ArithmeticBenchmarkEntry",
                        "methodName": "RunWorkload",
                        "methodSignature": "RunWorkload()",
                        "category": 1,
                        "metrics": 3,
                        "modes": 3,
                    }
                ],
            )
            self._write_subject_manifest(
                repo_root,
                subject_id="SolutionCorePack",
                supported_modes=["managed", "native"],
                workload_entry=workload_entry,
            )
            self._write_record(
                repo_root,
                subject_id="SolutionCorePack",
                mode="managed",
                mean_duration_ms=3.0,
                mean_ops_per_second=3000.0,
                benchmark_case={
                    "stableId": "SolutionCorePack::PerformanceFeaturePack::PerformanceFeaturePack.ArithmeticBenchmarkEntry::RunWorkload()",
                    "alias": "arithmetic-bench",
                    "displayName": "arithmetic-bench",
                    "workloadEntry": "PerformanceFeaturePack/ArithmeticBenchmarkEntry::RunWorkload()",
                    "assemblyName": "PerformanceFeaturePack",
                    "declaringType": "PerformanceFeaturePack.ArithmeticBenchmarkEntry",
                    "methodName": "RunWorkload",
                    "methodSignature": "RunWorkload()",
                    "supportedModes": ["managed", "native"],
                },
            )
            self._write_record(
                repo_root,
                subject_id="SolutionCorePack",
                mode="native",
                mean_duration_ms=1.5,
                mean_ops_per_second=6000.0,
                benchmark_case={
                    "stableId": "SolutionCorePack::PerformanceFeaturePack::PerformanceFeaturePack.ArithmeticBenchmarkEntry::RunWorkload()",
                    "alias": "arithmetic-bench",
                    "displayName": "arithmetic-bench",
                    "workloadEntry": "PerformanceFeaturePack/ArithmeticBenchmarkEntry::RunWorkload()",
                    "assemblyName": "PerformanceFeaturePack",
                    "declaringType": "PerformanceFeaturePack.ArithmeticBenchmarkEntry",
                    "methodName": "RunWorkload",
                    "methodSignature": "RunWorkload()",
                    "supportedModes": ["managed", "native"],
                },
            )

            generator_module.update_docs(repo_root)

            subject_payload = json.loads((docs_root / "subjects" / "SolutionCorePack.json").read_text(encoding="utf-8"))
            case_payload = subject_payload["benchmarkCasesByDevice"]["windows-x64-test-device"]
            self.assertEqual({case_id}, set(case_payload))
            self.assertEqual(["managed", "native"], case_payload[case_id]["recordedModes"])
            self.assertEqual(workload_entry, case_payload[case_id]["workloadEntry"])
            self.assertEqual("CoreRuntimeBenchmarks", case_payload[case_id]["assemblyName"])
            self.assertEqual(3.0, case_payload[case_id]["keyMetrics"]["managedBaseline"]["durationMs"])
            self.assertEqual(2.0, case_payload[case_id]["keyMetrics"]["nativeSpeedup"]["value"])

            self.assertEqual(case_id, subject_payload["summaryBenchmarkCase"]["caseId"])
            self.assertEqual(workload_entry, subject_payload["summaryBenchmarkCase"]["workloadEntry"])

            dashboard_html = (docs_root / "dashboard.html").read_text(encoding="utf-8")
            self.assertIn(case_id, dashboard_html)
            self.assertNotIn("PerformanceFeaturePack/ArithmeticBenchmarkEntry::RunWorkload()", dashboard_html)
        finally:
            shutil.rmtree(repo_root, ignore_errors=True)

    def test_update_docs_rekeys_legacy_record_identity_into_declared_case_by_entry_index(self) -> None:
        generator_module = load_module(
            BENCHMARK_DASHBOARD_GENERATOR_MODULE_PATH,
            "chaos_benchmark_dashboard_generator_declared_case_entry_index_rekey",
        )
        repo_root = self._make_repo_root()
        docs_root = repo_root / "docs" / "benchmark"
        workload_entry = "CoreRuntimeBenchmarks/ArithmeticBenchmarkEntry::RunWorkload()"
        case_id = "SolutionCorePack::CoreRuntimeBenchmarks::CoreRuntimeBenchmarks.ArithmeticBenchmarkEntry::RunWorkload()"

        try:
            self._write_testing_support(repo_root)
            self._write_compiled_catalog(
                repo_root,
                declared_benchmarks=[
                    {
                        "stableId": case_id,
                        "entryIndex": 11,
                        "alias": "arithmetic-bench",
                        "assemblyName": "CoreRuntimeBenchmarks",
                        "declaringType": "CoreRuntimeBenchmarks.ArithmeticBenchmarkEntry",
                        "methodName": "RunWorkload",
                        "methodSignature": "RunWorkload()",
                        "category": 1,
                        "metrics": 3,
                        "modes": 3,
                    }
                ],
            )
            self._write_subject_manifest(
                repo_root,
                subject_id="SolutionCorePack",
                supported_modes=["managed", "native"],
                workload_entry=workload_entry,
            )
            self._write_record(
                repo_root,
                subject_id="SolutionCorePack",
                mode="managed",
                mean_duration_ms=3.0,
                mean_ops_per_second=3000.0,
                benchmark_case={
                    "stableId": "legacy/arithmetic",
                    "entryIndex": 11,
                    "alias": "legacy-arithmetic-bench",
                    "displayName": "legacy-arithmetic-bench",
                    "workloadEntry": "PerformanceFeaturePack/ArithmeticBenchmarkEntry::RunWorkload()",
                    "assemblyName": "PerformanceFeaturePack",
                    "declaringType": "PerformanceFeaturePack.ArithmeticBenchmarkEntry",
                    "methodName": "RunWorkload",
                    "methodSignature": "RunWorkload()",
                    "supportedModes": ["managed", "native"],
                },
            )
            self._write_record(
                repo_root,
                subject_id="SolutionCorePack",
                mode="native",
                mean_duration_ms=1.5,
                mean_ops_per_second=6000.0,
                benchmark_case={
                    "stableId": "legacy/arithmetic",
                    "entryIndex": 11,
                    "alias": "legacy-arithmetic-bench",
                    "displayName": "legacy-arithmetic-bench",
                    "workloadEntry": "PerformanceFeaturePack/ArithmeticBenchmarkEntry::RunWorkload()",
                    "assemblyName": "PerformanceFeaturePack",
                    "declaringType": "PerformanceFeaturePack.ArithmeticBenchmarkEntry",
                    "methodName": "RunWorkload",
                    "methodSignature": "RunWorkload()",
                    "supportedModes": ["managed", "native"],
                },
            )

            generator_module.update_docs(repo_root)

            subject_payload = json.loads((docs_root / "subjects" / "SolutionCorePack.json").read_text(encoding="utf-8"))
            case_payload = subject_payload["benchmarkCasesByDevice"]["windows-x64-test-device"]
            self.assertEqual({case_id}, set(case_payload))
            self.assertEqual(["managed", "native"], case_payload[case_id]["recordedModes"])
            self.assertEqual(11, case_payload[case_id]["entryIndex"])
            self.assertEqual(workload_entry, case_payload[case_id]["workloadEntry"])
            self.assertEqual("CoreRuntimeBenchmarks", case_payload[case_id]["assemblyName"])

            dashboard_html = (docs_root / "dashboard.html").read_text(encoding="utf-8")
            self.assertIn(case_id, dashboard_html)
            self.assertNotIn("legacy/arithmetic", dashboard_html)
            self.assertNotIn("legacy-arithmetic-bench", dashboard_html)
        finally:
            shutil.rmtree(repo_root, ignore_errors=True)
