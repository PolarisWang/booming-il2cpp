from tests.unit.performance.benchmark_dashboard_generator_test_support import *


class TestBenchmarkDashboardGeneratorModes(BenchmarkDashboardGeneratorTestSupport):
    def test_update_docs_records_subject_mode_when_data_exists_even_if_manifest_excludes_mode(self) -> None:
        generator_module = load_module(
            BENCHMARK_DASHBOARD_GENERATOR_MODULE_PATH,
            "chaos_benchmark_dashboard_generator_subject_recorded_modes",
        )
        repo_root = self._make_repo_root()
        docs_root = repo_root / "verification" / "projections" / "benchmark"

        try:
            self._write_testing_support(repo_root)
            self._write_subject_manifest(
                repo_root,
                subject_id="SolutionCorePack",
                supported_modes=["managed"],
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

            generator_module.update_docs(repo_root)

            overview_payload = json.loads((docs_root / "overview.json").read_text(encoding="utf-8"))
            subject_payload = overview_payload["subjects"]["SolutionCorePack"]
            self.assertEqual(["managed", "native"], subject_payload["supportedModes"])
            self.assertEqual(["managed", "native"], subject_payload["recordedModes"])
            self.assertEqual([], subject_payload["missingModes"])
            self.assertEqual(["interpreter"], subject_payload["unsupportedModes"])
            self.assertEqual("recorded", subject_payload["modeStatus"]["native"]["status"])

            dashboard_html = (docs_root / "dashboard.html").read_text(encoding="utf-8")
            self.assertIn('"supportedModes":["managed","native"]', dashboard_html.replace(" ", ""))
        finally:
            shutil.rmtree(repo_root, ignore_errors=True)

    def test_update_docs_records_case_mode_when_data_exists_even_if_record_metadata_excludes_mode(self) -> None:
        generator_module = load_module(
            BENCHMARK_DASHBOARD_GENERATOR_MODULE_PATH,
            "chaos_benchmark_dashboard_generator_case_supported_modes",
        )
        repo_root = self._make_repo_root()
        docs_root = repo_root / "verification" / "projections" / "benchmark"

        try:
            self._write_testing_support(repo_root)
            self._write_subject_manifest(
                repo_root,
                subject_id="SolutionCorePack",
                supported_modes=["managed", "native"],
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
                mean_duration_ms=6.0,
                mean_ops_per_second=2500.0,
                benchmark_case={
                    "stableId": "allocation",
                    "alias": "allocation-bench",
                    "workloadEntry": "CoreRuntimeBenchmarks/AllocationBenchmarkEntry::RunWorkload()",
                    "supportedModes": ["managed"],
                    "category": 3,
                    "metrics": 3,
                },
            )
            self._write_record(
                repo_root,
                subject_id="SolutionCorePack",
                mode="native",
                mean_duration_ms=3.0,
                mean_ops_per_second=5000.0,
                benchmark_case={
                    "stableId": "allocation",
                    "alias": "allocation-bench",
                    "workloadEntry": "CoreRuntimeBenchmarks/AllocationBenchmarkEntry::RunWorkload()",
                    "supportedModes": ["managed"],
                    "category": 3,
                    "metrics": 3,
                },
            )

            generator_module.update_docs(repo_root)

            subject_payload = json.loads((docs_root / "subjects" / "SolutionCorePack.json").read_text(encoding="utf-8"))
            case_payload = subject_payload["benchmarkCasesByDevice"]["windows-x64-test-device"]["allocation"]
            self.assertEqual(["managed", "native"], case_payload["supportedModes"])
            self.assertEqual(["managed", "native"], case_payload["recordedModes"])
            self.assertEqual([], case_payload["missingModes"])
            self.assertEqual(["interpreter"], case_payload["unsupportedModes"])
            self.assertEqual("recorded", case_payload["modeStatus"]["native"]["status"])
            self.assertEqual(0, subject_payload["caseSummaryByDevice"]["windows-x64-test-device"]["managedOnlyCaseCount"])
            self.assertEqual(1, subject_payload["caseSummaryByDevice"]["windows-x64-test-device"]["crossModeCaseCount"])
            self.assertEqual(2.0, case_payload["keyMetrics"]["nativeSpeedup"]["value"])

            dashboard_html = (docs_root / "dashboard.html").read_text(encoding="utf-8")
            self.assertIn('"crossModeCaseCount": 1', dashboard_html)
            self.assertIn('"supportedModes":["managed","native"]', dashboard_html.replace(" ", ""))
        finally:
            shutil.rmtree(repo_root, ignore_errors=True)

    def test_update_docs_exposes_summary_workload_contract_for_dashboard_layout(self) -> None:
        generator_module = load_module(
            BENCHMARK_DASHBOARD_GENERATOR_MODULE_PATH,
            "chaos_benchmark_dashboard_generator_summary_workload",
        )
        repo_root = self._make_repo_root()
        docs_root = repo_root / "verification" / "projections" / "benchmark"
        workload_entry = "CoreRuntimeBenchmarks/ArithmeticBenchmarkEntry::RunWorkload()"

        try:
            self._write_testing_support(repo_root)
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
                    "assemblyName": "CoreRuntimeBenchmarks",
                    "declaringType": "CoreRuntimeBenchmarks.ArithmeticBenchmarkEntry",
                    "supportedModes": ["managed", "native"],
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
                    "assemblyName": "CoreRuntimeBenchmarks",
                    "declaringType": "CoreRuntimeBenchmarks.ArithmeticBenchmarkEntry",
                    "supportedModes": ["managed", "native"],
                },
            )

            generator_module.update_docs(repo_root)

            subject_payload = json.loads((docs_root / "subjects" / "SolutionCorePack.json").read_text(encoding="utf-8"))
            self.assertEqual(workload_entry, subject_payload["summaryWorkloadEntry"])
            self.assertEqual("arithmetic-bench", subject_payload["summaryBenchmarkCase"]["displayName"])
            self.assertEqual("CoreRuntimeBenchmarks", subject_payload["summaryBenchmarkCase"]["assemblyName"])

            overview_payload = json.loads((docs_root / "overview.json").read_text(encoding="utf-8"))
            self.assertEqual(
                "arithmetic-bench",
                overview_payload["subjects"]["SolutionCorePack"]["summaryBenchmarkCase"]["displayName"],
            )

            dashboard_html = (docs_root / "dashboard.html").read_text(encoding="utf-8")
            self.assertIn("Summary Workload", dashboard_html)
            self.assertIn("Solution Slice Breakdown", dashboard_html)
            self.assertIn("Latency vs Managed", dashboard_html)
        finally:
            shutil.rmtree(repo_root, ignore_errors=True)

    def test_update_docs_prefers_declared_case_summary_when_manifest_workload_entry_is_stale(self) -> None:
        generator_module = load_module(
            BENCHMARK_DASHBOARD_GENERATOR_MODULE_PATH,
            "chaos_benchmark_dashboard_generator_stale_manifest_summary_workload",
        )
        repo_root = self._make_repo_root()
        docs_root = repo_root / "verification" / "projections" / "benchmark"
        stale_workload_entry = "LegacyFeaturePack/DispatchBenchmarkEntry::RunWorkload()"
        declared_workload_entry = "CoreRuntimeBenchmarks/ArithmeticBenchmarkEntry::RunWorkload()"

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
                workload_entry=stale_workload_entry,
            )
            self._write_record(
                repo_root,
                subject_id="SolutionCorePack",
                mode="managed",
                mean_duration_ms=3.0,
                mean_ops_per_second=3000.0,
                benchmark_case={
                    "stableId": "arith",
                    "alias": "arithmetic-bench",
                    "workloadEntry": declared_workload_entry,
                    "assemblyName": "CoreRuntimeBenchmarks",
                    "declaringType": "CoreRuntimeBenchmarks.ArithmeticBenchmarkEntry",
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
                    "stableId": "arith",
                    "alias": "arithmetic-bench",
                    "workloadEntry": declared_workload_entry,
                    "assemblyName": "CoreRuntimeBenchmarks",
                    "declaringType": "CoreRuntimeBenchmarks.ArithmeticBenchmarkEntry",
                    "methodName": "RunWorkload",
                    "methodSignature": "RunWorkload()",
                    "supportedModes": ["managed", "native"],
                },
            )

            generator_module.update_docs(repo_root)

            subject_payload = json.loads((docs_root / "subjects" / "SolutionCorePack.json").read_text(encoding="utf-8"))
            self.assertEqual(declared_workload_entry, subject_payload["summaryWorkloadEntry"])
            self.assertEqual("arith", subject_payload["summaryBenchmarkCase"]["caseId"])
            self.assertEqual(declared_workload_entry, subject_payload["summaryBenchmarkCase"]["workloadEntry"])
            self.assertNotEqual(stale_workload_entry, subject_payload["summaryWorkloadEntry"])
        finally:
            shutil.rmtree(repo_root, ignore_errors=True)

