from tests.unit.performance.benchmark_dashboard_generator_test_support import *


class TestBenchmarkDashboardGeneratorContracts(BenchmarkDashboardGeneratorTestSupport):
    def test_update_docs_marks_cross_commit_records_as_stale_and_hides_relative_comparison(self) -> None:
        generator_module = load_module(
            BENCHMARK_DASHBOARD_GENERATOR_MODULE_PATH,
            "chaos_benchmark_dashboard_generator_cross_commit_stale",
        )
        repo_root = self._make_repo_root()
        docs_root = repo_root / "verification" / "projections" / "benchmark"
        workload_entry = "CoreRuntimeBenchmarks/AllocationBenchmarkEntry::RunWorkload()"

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
                recorded_at="2026-04-15T08:00:00+00:00",
                git_commit="new456",
            )
            self._write_record(
                repo_root,
                subject_id="SolutionCorePack",
                mode="native",
                mean_duration_ms=4.0,
                mean_ops_per_second=2000.0,
                recorded_at="2026-04-14T08:00:00+00:00",
                git_commit="old123",
            )
            benchmark_case = {
                "stableId": "allocation",
                "alias": "allocation-bench",
                "workloadEntry": workload_entry,
                "assemblyName": "CoreRuntimeBenchmarks",
                "declaringType": "CoreRuntimeBenchmarks.AllocationBenchmarkEntry",
                "methodName": "RunWorkload",
                "methodSignature": "RunWorkload()",
                "supportedModes": ["managed", "native"],
            }
            self._write_record(
                repo_root,
                subject_id="SolutionCorePack",
                mode="managed",
                mean_duration_ms=2.0,
                mean_ops_per_second=5000.0,
                benchmark_case=benchmark_case,
                recorded_at="2026-04-15T08:05:00+00:00",
                git_commit="new456",
            )
            self._write_record(
                repo_root,
                subject_id="SolutionCorePack",
                mode="native",
                mean_duration_ms=1.0,
                mean_ops_per_second=10000.0,
                benchmark_case=benchmark_case,
                recorded_at="2026-04-14T08:05:00+00:00",
                git_commit="old123",
            )

            generator_module.update_docs(repo_root)

            overview_payload = json.loads((docs_root / "overview.json").read_text(encoding="utf-8"))
            overview_subject = overview_payload["subjects"]["SolutionCorePack"]
            self.assertTrue(overview_subject["isStale"])
            self.assertEqual(["native"], overview_subject["staleModes"])
            self.assertEqual(1, overview_subject["coverage"]["staleModeCount"])
            self.assertTrue(overview_subject["coverage"]["needsAttention"])
            self.assertTrue(overview_subject["modeStatus"]["native"]["isStale"])
            self.assertEqual(
                "stale",
                overview_subject["keyMetrics"]["relativeToManaged"]["native"]["status"],
            )
            self.assertEqual(
                "cross-commit-record",
                overview_subject["keyMetrics"]["relativeToManaged"]["native"]["reasonCode"],
            )

            subject_payload = json.loads((docs_root / "subjects" / "SolutionCorePack.json").read_text(encoding="utf-8"))
            case_payload = subject_payload["benchmarkCasesByDevice"]["windows-x64-test-device"]["allocation"]
            self.assertTrue(case_payload["isStale"])
            self.assertEqual(["native"], case_payload["staleModes"])
            self.assertTrue(case_payload["modeStatus"]["native"]["isStale"])
            self.assertEqual(
                "stale",
                case_payload["keyMetrics"]["relativeToManaged"]["native"]["status"],
            )
            self.assertEqual(
                1,
                subject_payload["caseSummaryByDevice"]["windows-x64-test-device"]["staleCaseCount"],
            )
            self.assertEqual(
                1,
                subject_payload["caseSummaryByDevice"]["windows-x64-test-device"]["attentionCaseCount"],
            )

            dashboard_html = (docs_root / "dashboard.html").read_text(encoding="utf-8")
            self.assertIn("Stale record", dashboard_html)
            self.assertIn('"staleModes": ["native"]', dashboard_html)
        finally:
            shutil.rmtree(repo_root, ignore_errors=True)

    def test_update_docs_exposes_archetype_and_hot_update_case_metadata_for_dashboard_layout(self) -> None:
        generator_module = load_module(
            BENCHMARK_DASHBOARD_GENERATOR_MODULE_PATH,
            "chaos_benchmark_dashboard_generator_case_contract_labels",
        )
        repo_root = self._make_repo_root()
        docs_root = repo_root / "verification" / "projections" / "benchmark"
        workload_entry = "HotUpdateHostPack/HotUpdateLoadBenchmarkEntry::RunWorkload()"

        try:
            self._write_testing_support(repo_root)
            self._write_subject_manifest(
                repo_root,
                subject_id="HotUpdateHostPack",
                supported_modes=["managed"],
                workload_entry=workload_entry,
            )
            self._write_record(
                repo_root,
                subject_id="HotUpdateHostPack",
                mode="managed",
                mean_duration_ms=42.0,
                mean_ops_per_second=250.0,
            )
            self._write_record(
                repo_root,
                subject_id="HotUpdateHostPack",
                mode="managed",
                mean_duration_ms=18.5,
                mean_ops_per_second=540.0,
                benchmark_case={
                    "stableId": "hot-update-load",
                    "alias": "hot-update-load-bench",
                    "workloadEntry": workload_entry,
                    "assemblyName": "HotUpdateHostPack",
                    "declaringType": "HotUpdateHostPack.HotUpdateLoadBenchmarkEntry",
                    "category": 4,
                    "metrics": 1,
                    "requires": 32,
                    "archetype": 7,
                    "hotUpdateCapability": 17,
                    "supportedModes": ["managed"],
                },
            )

            generator_module.update_docs(repo_root)

            subject_payload = json.loads((docs_root / "subjects" / "HotUpdateHostPack.json").read_text(encoding="utf-8"))
            self.assertEqual(
                "Skeleton Patch Solution",
                subject_payload["summaryBenchmarkCase"]["archetypeLabel"],
            )
            self.assertEqual(
                ["Package Load", "Patch Integrity"],
                subject_payload["summaryBenchmarkCase"]["hotUpdateCapabilityLabels"],
            )
            case_payload = subject_payload["benchmarkCasesByDevice"]["windows-x64-test-device"]["hot-update-load"]
            self.assertEqual("Hot Update", case_payload["categoryLabel"])
            self.assertEqual("Skeleton Patch Solution", case_payload["archetypeLabel"])
            self.assertEqual(["Package Load", "Patch Integrity"], case_payload["hotUpdateCapabilityLabels"])
            self.assertEqual(["Hot Update"], case_payload["requirementLabels"])

            dashboard_html = (docs_root / "dashboard.html").read_text(encoding="utf-8")
            self.assertIn("Solution Archetype", dashboard_html)
            self.assertIn("Hot-Update Capability", dashboard_html)
        finally:
            shutil.rmtree(repo_root, ignore_errors=True)

    def test_update_docs_exposes_capability_contract_and_mode_reason_codes_for_declared_cases(self) -> None:
        generator_module = load_module(
            BENCHMARK_DASHBOARD_GENERATOR_MODULE_PATH,
            "chaos_benchmark_dashboard_generator_capability_contracts",
        )
        repo_root = self._make_repo_root()
        docs_root = repo_root / "verification" / "projections" / "benchmark"
        workload_entry = "InteropBenchmarks/NativeInteropBenchmarkEntry::RunWorkload()"

        try:
            self._write_testing_support(repo_root)
            self._write_compiled_catalog(
                repo_root,
                declared_benchmarks=[
                    {
                        "stableId": "interop",
                        "alias": "native-interop-bench",
                        "assemblyName": "InteropBenchmarks",
                        "declaringType": "InteropBenchmarks.NativeInteropBenchmarkEntry",
                        "methodName": "RunWorkload",
                        "methodSignature": "RunWorkload()",
                        "category": 1,
                        "capabilityFamily": 7,
                        "capabilityItem": 31,
                        "archetype": 1,
                        "hotUpdateCapability": 0,
                        "requires": 0,
                        "metrics": 3,
                        "modes": 3,
                        "warmupCount": 2,
                        "iterationCount": 5,
                        "invocationCount": 10,
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
                mean_duration_ms=9.0,
                mean_ops_per_second=1000.0,
            )
            self._write_record(
                repo_root,
                subject_id="SolutionCorePack",
                mode="managed",
                mean_duration_ms=3.0,
                mean_ops_per_second=3000.0,
                benchmark_case={
                    "stableId": "interop",
                    "alias": "native-interop-bench",
                    "workloadEntry": workload_entry,
                    "assemblyName": "InteropBenchmarks",
                    "declaringType": "InteropBenchmarks.NativeInteropBenchmarkEntry",
                    "methodName": "RunWorkload",
                    "methodSignature": "RunWorkload()",
                    "category": 1,
                    "capabilityFamily": 7,
                    "capabilityItem": 31,
                    "metrics": 3,
                    "modes": 3,
                },
            )

            generator_module.update_docs(repo_root)

            overview_payload = json.loads((docs_root / "overview.json").read_text(encoding="utf-8"))
            subject_overview = overview_payload["subjects"]["SolutionCorePack"]
            self.assertEqual("missing-record", subject_overview["modeStatus"]["native"]["reasonCode"])
            self.assertEqual(
                "unsupported-by-contract",
                subject_overview["modeStatus"]["interpreter"]["reasonCode"],
            )

            subject_payload = json.loads((docs_root / "subjects" / "SolutionCorePack.json").read_text(encoding="utf-8"))
            case_payload = subject_payload["benchmarkCasesByDevice"]["windows-x64-test-device"]["interop"]
            self.assertEqual(7, case_payload["capabilityFamily"])
            self.assertEqual("Interop And Marshaling", case_payload["capabilityFamilyLabel"])
            self.assertEqual(31, case_payload["capabilityItem"])
            self.assertEqual("Native Call Interop", case_payload["capabilityItemLabel"])
            self.assertEqual("SolutionCorePack", case_payload["ownerSubjectId"])
            self.assertEqual([1, 5], case_payload["supportStates"])
            self.assertEqual(["NativeGenerated", "ExternalRuntime"], case_payload["supportStateLabels"])
            self.assertTrue(case_payload["proofRequired"])
            self.assertTrue(case_payload["benchmarkRequired"])
            self.assertEqual("missing-record", case_payload["modeStatus"]["native"]["reasonCode"])
            self.assertEqual(
                "Declared by case contract, but no benchmark record was found.",
                case_payload["modeStatus"]["native"]["reasonLabel"],
            )
            self.assertEqual("unsupported-by-contract", case_payload["modeStatus"]["interpreter"]["reasonCode"])

            summary_case = subject_payload["summaryBenchmarkCase"]
            self.assertEqual("SolutionCorePack", summary_case["ownerSubjectId"])
            self.assertEqual("Native Call Interop", summary_case["capabilityItemLabel"])
        finally:
            shutil.rmtree(repo_root, ignore_errors=True)

    def test_update_docs_dashboard_html_surfaces_contract_ownership_support_state_and_reason_copy(self) -> None:
        generator_module = load_module(
            BENCHMARK_DASHBOARD_GENERATOR_MODULE_PATH,
            "chaos_benchmark_dashboard_generator_contract_reason_html",
        )
        repo_root = self._make_repo_root()
        docs_root = repo_root / "verification" / "projections" / "benchmark"
        workload_entry = "InteropBenchmarks/NativeInteropBenchmarkEntry::RunWorkload()"

        try:
            self._write_testing_support(repo_root)
            self._write_compiled_catalog(
                repo_root,
                declared_benchmarks=[
                    {
                        "stableId": "interop",
                        "alias": "native-interop-bench",
                        "assemblyName": "InteropBenchmarks",
                        "declaringType": "InteropBenchmarks.NativeInteropBenchmarkEntry",
                        "methodName": "RunWorkload",
                        "methodSignature": "RunWorkload()",
                        "category": 1,
                        "capabilityFamily": 7,
                        "capabilityItem": 31,
                        "archetype": 1,
                        "hotUpdateCapability": 0,
                        "requires": 0,
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
                mean_duration_ms=4.0,
                mean_ops_per_second=4000.0,
                benchmark_case={
                    "stableId": "interop",
                    "alias": "native-interop-bench",
                    "workloadEntry": workload_entry,
                    "assemblyName": "InteropBenchmarks",
                    "declaringType": "InteropBenchmarks.NativeInteropBenchmarkEntry",
                    "methodName": "RunWorkload",
                    "methodSignature": "RunWorkload()",
                    "category": 1,
                    "capabilityFamily": 7,
                    "capabilityItem": 31,
                    "metrics": 3,
                    "modes": 3,
                },
            )

            generator_module.update_docs(repo_root)

            dashboard_html = (docs_root / "dashboard.html").read_text(encoding="utf-8")
            self.assertIn("Owner Subject", dashboard_html)
            self.assertIn("Capability", dashboard_html)
            self.assertIn("Support State", dashboard_html)
            self.assertIn("Requirement Contract", dashboard_html)
            self.assertIn("Benchmark Required", dashboard_html)
            self.assertIn("Proof Required", dashboard_html)
            self.assertIn("unsupported-by-contract", dashboard_html)
            self.assertIn("missing-record", dashboard_html)
        finally:
            shutil.rmtree(repo_root, ignore_errors=True)

