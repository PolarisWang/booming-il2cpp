from tests.unit.performance.benchmark_dashboard_generator_test_support import *
from tests.unit.performance.testing_inventory_test_support import (
    inventory_fixture,
    load_inventory_generator_module,
    write_inventory_fixture_repo,
)


class TestBenchmarkDashboardGeneratorOverview(BenchmarkDashboardGeneratorTestSupport):
    def test_update_docs_rebuilds_dashboard_from_formal_source_without_legacy_benchmark_docs(self) -> None:
        generator_module = load_module(
            BENCHMARK_DASHBOARD_GENERATOR_MODULE_PATH,
            "chaos_benchmark_dashboard_generator_formal_source_rebuild",
        )
        inventory_generator_module = load_inventory_generator_module(
            "chaos_testing_inventory_generator_formal_source_rebuild"
        )
        repo_root = self._make_repo_root()
        docs_root = repo_root / "verification" / "projections" / "benchmark"
        fixture = inventory_fixture()

        try:
            write_inventory_fixture_repo(repo_root, fixture)
            inventory_generator_module.write_inventory_outputs(
                repo_root,
                host_platform="windows",
                output_root=repo_root / "docs" / "testing-inventory",
            )
            self._write_testing_support(repo_root)
            (docs_root / "subjects").mkdir(parents=True, exist_ok=True)
            (docs_root / "overview.json").write_text('{"subjects":{"LegacyPack":{"displayName":"Legacy Pack"}}}\n', encoding="utf-8")
            (docs_root / "subjects" / f"{fixture['subjectId']}.json").write_text(
                '{"displayName":"legacy-fixture","summaryBenchmarkCase":{"alias":"legacy-bench"}}\n',
                encoding="utf-8",
            )

            generator_module.update_docs(repo_root)

            overview_payload = json.loads((docs_root / "overview.json").read_text(encoding="utf-8"))
            self.assertEqual({"FixtureSubject"}, set(overview_payload["subjects"]))

            overview_subject = overview_payload["subjects"][fixture["subjectId"]]
            self.assertEqual(fixture["displayName"], overview_subject["displayName"])
            self.assertEqual(fixture["platformId"], overview_subject["defaultPlatform"])
            self.assertEqual(fixture["benchmarkAlias"], overview_subject["summaryBenchmarkCase"]["alias"])
            self.assertEqual(["managed", "native"], overview_subject["supportedModes"])
            self.assertEqual(["managed"], overview_subject["recordedModes"])
            self.assertEqual(["native"], overview_subject["missingModes"])
            self.assertEqual(["interpreter"], overview_subject["unsupportedModes"])
            self.assertEqual("recorded", overview_subject["modeStatus"]["managed"]["status"])
            self.assertEqual("missing", overview_subject["modeStatus"]["native"]["status"])
            self.assertEqual("recorded", overview_subject["keyMetrics"]["managedBaseline"]["status"])
            self.assertEqual("missing", overview_subject["keyMetrics"]["relativeToManaged"]["native"]["status"])

            subject_payload = json.loads(
                (docs_root / "subjects" / f"{fixture['subjectId']}.json").read_text(encoding="utf-8")
            )
            self.assertIn(fixture["platformId"], subject_payload["platforms"])
            self.assertEqual(fixture["workloadEntry"], subject_payload["summaryWorkloadEntry"])

            case_payload = subject_payload["benchmarkCasesByDevice"][fixture["deviceId"]][fixture["benchmarkStableId"]]
            self.assertEqual(fixture["benchmarkAlias"], case_payload["displayName"])
            self.assertEqual("recorded", case_payload["modeStatus"]["managed"]["status"])
            self.assertEqual("missing", case_payload["modeStatus"]["native"]["status"])
            self.assertEqual("unsupported", case_payload["modeStatus"]["interpreter"]["status"])
            self.assertEqual("recorded", case_payload["keyMetrics"]["managedBaseline"]["status"])
            self.assertEqual("missing", case_payload["keyMetrics"]["relativeToManaged"]["native"]["status"])
            self.assertEqual(
                "unsupported",
                case_payload["keyMetrics"]["relativeToManaged"]["interpreter"]["status"],
            )

            dashboard_html = (docs_root / "dashboard.html").read_text(encoding="utf-8")
            self.assertIn("FixtureSubject", dashboard_html)
            self.assertIn("native-interop-bench", dashboard_html)
            self.assertIn('"missingModes":["native"]', dashboard_html.replace(" ", ""))
        finally:
            shutil.rmtree(repo_root, ignore_errors=True)

    def test_update_docs_removes_stale_subject_json_and_rewrites_dashboard_payload(self) -> None:
        generator_module = load_module(
            BENCHMARK_DASHBOARD_GENERATOR_MODULE_PATH,
            "chaos_benchmark_dashboard_generator_cleanup",
        )
        repo_root = self._make_repo_root()
        docs_root = repo_root / "verification" / "projections" / "benchmark"
        subjects_doc_root = docs_root / "subjects"
        stale_subject_path = subjects_doc_root / "BenchAllocation.json"

        try:
            self._write_testing_support(repo_root)
            self._write_subject_manifest(
                repo_root,
                subject_id="SolutionCorePack",
                supported_modes=["managed", "native"],
            )
            self._write_record(repo_root, subject_id="SolutionCorePack", mode="managed", mean_duration_ms=2.5)
            self._write_record(repo_root, subject_id="SolutionCorePack", mode="native", mean_duration_ms=1.25)

            subjects_doc_root.mkdir(parents=True, exist_ok=True)
            stale_subject_path.write_text('{"stale":true}\n', encoding="utf-8")

            generator_module.update_docs(repo_root)

            generated_subject_paths = sorted(path.name for path in subjects_doc_root.glob("*.json"))
            self.assertIn("SolutionCorePack.json", generated_subject_paths)

            subject_payload = json.loads((subjects_doc_root / "SolutionCorePack.json").read_text(encoding="utf-8"))
            self.assertIn("latestByDevice", subject_payload)
            self.assertIn("windows-x64-test-device", subject_payload["latestByDevice"])

            overview_payload = json.loads((docs_root / "overview.json").read_text(encoding="utf-8"))
            self.assertEqual({"SolutionCorePack"}, set(overview_payload["subjects"]))

            dashboard_html = (docs_root / "dashboard.html").read_text(encoding="utf-8")
            self.assertIn("SolutionCorePack", dashboard_html)
            self.assertNotIn("BenchAllocation", dashboard_html)
        finally:
            shutil.rmtree(repo_root, ignore_errors=True)

    def test_update_docs_overview_distinguishes_recorded_missing_and_unsupported_modes(self) -> None:
        generator_module = load_module(
            BENCHMARK_DASHBOARD_GENERATOR_MODULE_PATH,
            "chaos_benchmark_dashboard_generator_mode_status",
        )
        repo_root = self._make_repo_root()
        docs_root = repo_root / "verification" / "projections" / "benchmark"

        try:
            self._write_testing_support(repo_root)
            self._write_subject_manifest(
                repo_root,
                subject_id="HotUpdateHostPack",
                supported_modes=["managed"],
            )
            self._write_subject_manifest(
                repo_root,
                subject_id="MixedExecutionFeaturePack",
                supported_modes=["managed", "native", "interpreter"],
            )
            self._write_subject_manifest(
                repo_root,
                subject_id="SolutionCorePack",
                supported_modes=["native"],
            )
            self._write_subject_manifest(
                repo_root,
                subject_id="SparsePack",
                supported_modes=["managed", "native"],
            )

            self._write_record(repo_root, subject_id="HotUpdateHostPack", mode="managed", mean_duration_ms=57.5)
            self._write_record(
                repo_root,
                subject_id="MixedExecutionFeaturePack",
                mode="managed",
                mean_duration_ms=2.287,
                mean_ops_per_second=2624.106,
            )
            self._write_record(
                repo_root,
                subject_id="MixedExecutionFeaturePack",
                mode="native",
                mean_duration_ms=0.0,
                mean_ops_per_second=60000000.0,
            )
            self._write_record(
                repo_root,
                subject_id="MixedExecutionFeaturePack",
                mode="interpreter",
                mean_duration_ms=5.176,
                mean_ops_per_second=1226.199,
            )
            self._write_record(repo_root, subject_id="SolutionCorePack", mode="native", mean_duration_ms=5.846)
            self._write_record(repo_root, subject_id="SparsePack", mode="managed", mean_duration_ms=8.0)

            generator_module.update_docs(repo_root)

            overview_payload = json.loads((docs_root / "overview.json").read_text(encoding="utf-8"))
            summary = overview_payload["summary"]
            self.assertEqual(4, summary["subjectCount"])
            self.assertEqual(3, summary["fullyRecordedCount"])
            self.assertEqual(1, summary["subjectsWithMissingRecords"])
            self.assertEqual(6, summary["recordedModeCount"])
            self.assertEqual(1, summary["missingModeCount"])
            self.assertEqual(5, summary["unsupportedModeCount"])

            subjects_payload = overview_payload["subjects"]

            hot_update = subjects_payload["HotUpdateHostPack"]
            self.assertEqual(["managed"], hot_update["supportedModes"])
            self.assertEqual(["managed"], hot_update["recordedModes"])
            self.assertEqual([], hot_update["missingModes"])
            self.assertEqual(["native", "interpreter"], hot_update["unsupportedModes"])
            self.assertEqual("recorded", hot_update["modeStatus"]["managed"]["status"])
            self.assertEqual("unsupported", hot_update["modeStatus"]["native"]["status"])
            self.assertEqual("unsupported", hot_update["modeStatus"]["interpreter"]["status"])

            mixed = subjects_payload["MixedExecutionFeaturePack"]
            self.assertEqual(["managed", "native", "interpreter"], mixed["supportedModes"])
            self.assertEqual([], mixed["missingModes"])
            self.assertEqual("recorded", mixed["keyMetrics"]["managedBaseline"]["status"])
            self.assertEqual(2.287, mixed["keyMetrics"]["managedBaseline"]["durationMs"])
            self.assertEqual(2624.106, mixed["keyMetrics"]["managedBaseline"]["opsPerSecond"])
            self.assertEqual("recorded", mixed["keyMetrics"]["relativeToManaged"]["native"]["status"])
            self.assertEqual("faster", mixed["keyMetrics"]["relativeToManaged"]["native"]["direction"])
            self.assertEqual("throughput", mixed["keyMetrics"]["relativeToManaged"]["native"]["ratio"]["basis"])
            self.assertGreater(mixed["keyMetrics"]["relativeToManaged"]["native"]["ratio"]["value"], 1000)
            self.assertEqual("recorded", mixed["keyMetrics"]["relativeToManaged"]["interpreter"]["status"])
            self.assertEqual("slower", mixed["keyMetrics"]["relativeToManaged"]["interpreter"]["direction"])
            self.assertEqual("latency", mixed["keyMetrics"]["relativeToManaged"]["interpreter"]["ratio"]["basis"])
            self.assertEqual(2.26, mixed["keyMetrics"]["relativeToManaged"]["interpreter"]["ratio"]["value"])
            self.assertEqual("throughput", mixed["keyMetrics"]["nativeSpeedup"]["basis"])
            self.assertGreater(mixed["keyMetrics"]["nativeSpeedup"]["value"], 1000)
            self.assertEqual("latency", mixed["keyMetrics"]["interpreterOverhead"]["basis"])
            self.assertEqual("recorded", mixed["modeStatus"]["native"]["status"])

            solution = subjects_payload["SolutionCorePack"]
            self.assertEqual(["native"], solution["supportedModes"])
            self.assertEqual(["managed", "interpreter"], solution["unsupportedModes"])
            self.assertEqual("unsupported", solution["keyMetrics"]["managedBaseline"]["status"])
            self.assertEqual(
                "baseline-unavailable",
                solution["keyMetrics"]["relativeToManaged"]["native"]["status"],
            )

            sparse = subjects_payload["SparsePack"]
            self.assertEqual(["managed", "native"], sparse["supportedModes"])
            self.assertEqual(["native"], sparse["missingModes"])
            self.assertEqual("recorded", sparse["keyMetrics"]["managedBaseline"]["status"])
            self.assertEqual("missing", sparse["keyMetrics"]["relativeToManaged"]["native"]["status"])
            self.assertEqual("missing", sparse["modeStatus"]["native"]["status"])

            dashboard_html = (docs_root / "dashboard.html").read_text(encoding="utf-8")
            self.assertIn('"missingModeCount": 1', dashboard_html)
            self.assertIn('"unsupportedModes": ["native", "interpreter"]', dashboard_html)
            self.assertIn("meanOpsPerSecond", dashboard_html)
            self.assertIn("Managed Baseline", dashboard_html)
            self.assertIn("Native Latency vs Managed", dashboard_html)
            self.assertIn("Interpreter Latency vs Managed", dashboard_html)
            self.assertIn("Summary Workload", dashboard_html)
            self.assertNotIn('value="opsPerSecond"', dashboard_html)
        finally:
            shutil.rmtree(repo_root, ignore_errors=True)

    def test_update_docs_keeps_subject_summary_and_exposes_case_level_benchmark_records(self) -> None:
        generator_module = load_module(
            BENCHMARK_DASHBOARD_GENERATOR_MODULE_PATH,
            "chaos_benchmark_dashboard_generator_case_detail",
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
                mean_duration_ms=2.0,
                mean_ops_per_second=5000.0,
                benchmark_case={
                    "stableId": "arith",
                    "alias": "arithmetic-bench",
                    "workloadEntry": "CoreRuntimeBenchmarks/ArithmeticBenchmarkEntry::RunWorkload()",
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
                    "workloadEntry": "CoreRuntimeBenchmarks/ArithmeticBenchmarkEntry::RunWorkload()",
                },
            )
            self._write_record(
                repo_root,
                subject_id="SolutionCorePack",
                mode="managed",
                mean_duration_ms=6.0,
                mean_ops_per_second=2500.0,
                benchmark_case={
                    "stableId": "dispatch",
                    "alias": "dispatch-bench",
                    "workloadEntry": "CoreRuntimeBenchmarks/DispatchBenchmarkEntry::RunWorkload()",
                },
            )
            self._write_record(
                repo_root,
                subject_id="SolutionCorePack",
                mode="native",
                mean_duration_ms=3.0,
                mean_ops_per_second=5000.0,
                benchmark_case={
                    "stableId": "dispatch",
                    "alias": "dispatch-bench",
                    "workloadEntry": "CoreRuntimeBenchmarks/DispatchBenchmarkEntry::RunWorkload()",
                },
            )

            generator_module.update_docs(repo_root)

            overview_payload = json.loads((docs_root / "overview.json").read_text(encoding="utf-8"))
            solution_summary = overview_payload["subjects"]["SolutionCorePack"]
            self.assertEqual(8.0, solution_summary["keyMetrics"]["managedBaseline"]["durationMs"])
            self.assertEqual(2.0, solution_summary["keyMetrics"]["nativeSpeedup"]["value"])

            subject_payload = json.loads((docs_root / "subjects" / "SolutionCorePack.json").read_text(encoding="utf-8"))
            case_payload = subject_payload["benchmarkCasesByDevice"]["windows-x64-test-device"]
            self.assertEqual({"arith", "dispatch"}, set(case_payload))
            self.assertEqual("arithmetic-bench", case_payload["arith"]["displayName"])
            self.assertEqual(2.0, case_payload["arith"]["keyMetrics"]["managedBaseline"]["durationMs"])
            self.assertEqual(2.0, case_payload["arith"]["keyMetrics"]["nativeSpeedup"]["value"])
            self.assertEqual("dispatch-bench", case_payload["dispatch"]["displayName"])
            self.assertEqual(6.0, case_payload["dispatch"]["keyMetrics"]["managedBaseline"]["durationMs"])

            dashboard_html = (docs_root / "dashboard.html").read_text(encoding="utf-8")
            self.assertIn("Benchmark Case Matrix", dashboard_html)
            self.assertIn("benchmarkCasesByDevice", dashboard_html)
        finally:
            shutil.rmtree(repo_root, ignore_errors=True)

    def test_update_docs_uses_workspace_catalog_when_compiled_catalog_is_unavailable(self) -> None:
        generator_module = load_module(
            BENCHMARK_DASHBOARD_GENERATOR_MODULE_PATH,
            "chaos_benchmark_dashboard_generator_workspace_catalog",
        )
        repo_root = self._make_repo_root()
        docs_root = repo_root / "verification" / "projections" / "benchmark"
        testing_root = repo_root / "build" / "toolchains" / "run" / "testing"

        try:
            self._write_testing_support(repo_root)
            self._write_subject_manifest(
                repo_root,
                subject_id="SolutionCorePack",
                supported_modes=["managed", "native"],
            )
            self._write_workspace_benchmark_fixture(
                repo_root,
                subject_id="SolutionCorePack",
                stable_id="solution-core::arith",
                alias="arithmetic-bench",
                entry_index=9,
                assembly_name="CoreRuntimeBenchmarks",
                declaring_type="CoreRuntimeBenchmarks.ArithmeticBenchmarkEntry",
                method_name="RunWorkload",
                method_signature="RunWorkload()",
            )
            (testing_root / "compiled_catalog.py").write_text(
                "\n".join(
                    [
                        "from __future__ import annotations",
                        "",
                        "",
                        "def build_subject_declared_test_catalog(*, repo_root, subject_id, force_build=False):",
                        "    del repo_root, subject_id, force_build",
                        "    raise RuntimeError('compiled collection unavailable')",
                        "",
                    ]
                ),
                encoding="utf-8",
            )
            self._write_record(
                repo_root,
                subject_id="SolutionCorePack",
                mode="managed",
                mean_duration_ms=2.0,
                mean_ops_per_second=5000.0,
                benchmark_case={
                    "stableId": "solution-core::arith",
                    "workloadEntry": "CoreRuntimeBenchmarks/ArithmeticBenchmarkEntry::RunWorkload()",
                },
            )
            self._write_record(
                repo_root,
                subject_id="SolutionCorePack",
                mode="native",
                mean_duration_ms=1.0,
                mean_ops_per_second=10000.0,
                benchmark_case={
                    "stableId": "solution-core::arith",
                    "workloadEntry": "CoreRuntimeBenchmarks/ArithmeticBenchmarkEntry::RunWorkload()",
                },
            )

            generator_module.update_docs(repo_root)

            subject_payload = json.loads((docs_root / "subjects" / "SolutionCorePack.json").read_text(encoding="utf-8"))
            case_payload = subject_payload["benchmarkCasesByDevice"]["windows-x64-test-device"]["solution-core::arith"]
            self.assertEqual("arithmetic-bench", case_payload["displayName"])
            self.assertEqual(["managed", "native"], case_payload["supportedModes"])
            self.assertEqual(2.0, case_payload["keyMetrics"]["managedBaseline"]["durationMs"])
            self.assertEqual(2.0, case_payload["keyMetrics"]["nativeSpeedup"]["value"])
        finally:
            shutil.rmtree(repo_root, ignore_errors=True)

