from tests.unit.compatibility.hot_update_productization_gates_test_support import *


class TestHotUpdateProductizationGatesReports(ProductizationGatesTestSupport):
    def test_compatibility_matrix_assets_define_config_schema_and_proof_subject(self) -> None:
        self.assertTrue(
            COMPATIBILITY_MATRIX_CONFIG_PATH.is_file(),
            msg=f"missing compatibility matrix config: {COMPATIBILITY_MATRIX_CONFIG_PATH}",
        )
        self.assertTrue(
            COMPATIBILITY_MATRIX_SUBJECT_MANIFEST_PATH.is_file(),
            msg=f"missing compatibility matrix subject manifest: {COMPATIBILITY_MATRIX_SUBJECT_MANIFEST_PATH}",
        )

        config = json.loads(COMPATIBILITY_MATRIX_CONFIG_PATH.read_text(encoding="utf-8"))
        manifest = json.loads(COMPATIBILITY_MATRIX_SUBJECT_MANIFEST_PATH.read_text(encoding="utf-8"))

        for required_key in [
            "schemaVersion",
            "subjectId",
            "projectPath",
            "aotVersions",
            "hotUpdateVersions",
            "platforms",
            "additionalTestCases",
        ]:
            self.assertIn(required_key, config)

        self.assertEqual("v1", config["schemaVersion"])
        self.assertEqual("CompatibilityMatrixProof", config["subjectId"])
        self.assertEqual(3, len(config["aotVersions"]))
        self.assertEqual(3, len(config["hotUpdateVersions"]))
        self.assertEqual(
            ["windows-x64", "android-arm64", "ios-arm64"],
            config["platforms"],
        )

        first_patch = config["hotUpdateVersions"][0]
        self.assertIn("patchId", first_patch)
        self.assertIn("targetAotVersion", first_patch)
        self.assertEqual("CompatibilityMatrixProof", manifest["subjectId"])
        self.assertEqual("windows-managed-output", manifest["defaultMatrix"])

    def test_compatibility_matrix_runner_executes_proof_subject_and_writes_json_report(self) -> None:
        runner_module = load_module(
            COMPATIBILITY_MATRIX_RUNNER_PATH,
            "chaos_compatibility_matrix_runner_productization",
        )

        run_id = f"productization-{uuid.uuid4().hex}"
        result = runner_module.run_compatibility_matrix(
            REPO_ROOT,
            COMPATIBILITY_MATRIX_CONFIG_PATH,
            run_id=run_id,
            artifact_root=COMPATIBILITY_MATRIX_TMP_ROOT,
        )

        report_path = REPO_ROOT / result["reportPath"]
        self.assertTrue(report_path.is_file(), msg=f"missing compatibility matrix report: {report_path}")

        report = json.loads(report_path.read_text(encoding="utf-8"))
        self.assertEqual("v1", report["reportVersion"])
        self.assertEqual("CompatibilityMatrixProof", report["subjectId"])
        self.assertEqual("ok", report["status"])
        self.assertEqual(0, report["statusCounts"]["fail"])
        self.assertEqual(3, report["matrixDimensions"]["aotVersionCount"])
        self.assertEqual(3, report["matrixDimensions"]["hotUpdateVersionCount"])
        self.assertEqual(3, report["matrixDimensions"]["platformCount"])
        self.assertEqual(30, report["statusCounts"]["total"])

        platforms = {case_result["platform"] for case_result in report["caseResults"]}
        self.assertEqual({"windows-x64", "android-arm64", "ios-arm64"}, platforms)
        self.assertTrue(
            any(
                case_result["expected"] == "reject"
                and case_result["actual"] == "reject"
                and case_result["status"] == "ok"
                for case_result in report["caseResults"]
            ),
            msg=report_path.read_text(encoding="utf-8"),
        )

    def test_perf_dashboard_builder_discovers_perf_subject_matrix_entries(self) -> None:
        dashboard_module = load_module(
            PERF_DASHBOARD_MODULE_PATH,
            "chaos_perf_dashboard_productization",
        )

        config = dashboard_module.build_perf_dashboard_config(REPO_ROOT)

        self.assertEqual("v1", config["configVersion"])
        self.assertEqual("ok", config["status"])
        self.assertGreaterEqual(config["statusCounts"]["total"], 3)
        self.assertEqual(0, config["statusCounts"]["fail"])

        discovered_entries = {
            (entry["subjectId"], entry["matrixId"], entry["goalId"])
            for entry in config["entries"]
        }
        self.assertTrue(
            {
                ("SolutionCorePack", "windows-native-perf", "perf.release"),
                ("HotUpdateHostPack", "windows-managed-perf", "perf.release"),
                ("MixedExecutionFeaturePack", "windows-managed-perf", "perf.release"),
                ("MixedExecutionFeaturePack", "windows-native-perf", "perf.release"),
                ("MixedExecutionFeaturePack", "windows-interpreter-perf", "perf.release"),
            }.issubset(discovered_entries)
        )

        solution_core_pack_perf = next(
            entry
            for entry in config["entries"]
            if entry["subjectId"] == "SolutionCorePack" and entry["matrixId"] == "windows-native-perf"
        )
        self.assertTrue(
            solution_core_pack_perf["baselinePath"].startswith(
                "verification/catalog/owners/SolutionCorePack/benchmark-baselines/perf/"
            )
        )
        self.assertIn("meanDurationMs", solution_core_pack_perf["metricKeys"])
        self.assertEqual(
                    "CoreRuntimeBenchmarks/ArithmeticBenchmarkEntry::RunWorkload()",
            solution_core_pack_perf["summaryWorkloadEntry"],
        )
        self.assertEqual(
            "arithmetic-bench",
            solution_core_pack_perf["summaryBenchmarkCase"]["displayName"],
        )
        self.assertEqual(
            ["managed", "native"],
            solution_core_pack_perf["summaryBenchmarkCase"]["supportedModes"],
        )
        self.assertEqual(
            "SolutionCorePack",
            solution_core_pack_perf["summaryBenchmarkCase"]["ownerSubjectId"],
        )
        self.assertEqual(
            "Primitives And Ops",
            solution_core_pack_perf["summaryBenchmarkCase"]["capabilityFamilyLabel"],
        )
        self.assertEqual(
            "Arithmetic Ops",
            solution_core_pack_perf["summaryBenchmarkCase"]["capabilityItemLabel"],
        )
        self.assertEqual(
            ["NativeGenerated"],
            solution_core_pack_perf["summaryBenchmarkCase"]["supportStateLabels"],
        )
        self.assertTrue(solution_core_pack_perf["summaryBenchmarkCase"]["benchmarkRequired"])

        hot_update_perf = next(
            entry
            for entry in config["entries"]
            if entry["subjectId"] == "HotUpdateHostPack" and entry["matrixId"] == "windows-managed-perf"
        )
        self.assertEqual(
            "HotUpdateHostPack/HotUpdateLoadBenchmarkEntry::RunWorkload()",
            hot_update_perf["summaryWorkloadEntry"],
        )
        self.assertEqual(
            "hot-update-load-bench",
            hot_update_perf["summaryBenchmarkCase"]["displayName"],
        )
        self.assertEqual(7, hot_update_perf["summaryBenchmarkCase"]["archetype"])
        self.assertEqual(
            "Skeleton Patch Solution",
            hot_update_perf["summaryBenchmarkCase"]["archetypeLabel"],
        )
        self.assertEqual(17, hot_update_perf["summaryBenchmarkCase"]["hotUpdateCapability"])
        self.assertEqual(
            ["Package Load", "Patch Integrity"],
            hot_update_perf["summaryBenchmarkCase"]["hotUpdateCapabilityLabels"],
        )
        self.assertEqual(
            ["Hot Update"],
            hot_update_perf["summaryBenchmarkCase"]["requirementLabels"],
        )
        self.assertEqual(
            "HotUpdateHostPack",
            hot_update_perf["summaryBenchmarkCase"]["ownerSubjectId"],
        )
        self.assertEqual(
            "Hot Update Workflow",
            hot_update_perf["summaryBenchmarkCase"]["capabilityFamilyLabel"],
        )
        self.assertEqual(
            "Package Load",
            hot_update_perf["summaryBenchmarkCase"]["capabilityItemLabel"],
        )
        self.assertEqual(
            ["BridgeDispatch", "InterpreterReady"],
            hot_update_perf["summaryBenchmarkCase"]["supportStateLabels"],
        )
        self.assertTrue(hot_update_perf["summaryBenchmarkCase"]["benchmarkRequired"])

    def test_unsupported_feature_report_scanner_flags_fixture_patterns(self) -> None:
        report_module = load_module(
            UNSUPPORTED_FEATURE_REPORT_MODULE_PATH,
            "chaos_unsupported_feature_report_productization",
        )

        fixture_root = BATCH4_TMP_ROOT / f"unsupported-feature-{uuid.uuid4().hex}"
        source_root = fixture_root / "FixtureUnsupportedSubject" / "source"
        source_root.mkdir(parents=True, exist_ok=False)
        try:
            (source_root / "Program.cs").write_text(
                "\n".join(
                    [
                        "using System;",
                        "using System.Reflection.Emit;",
                        "",
                        "internal static class Program",
                        "{",
                        "    private static void Main()",
                        "    {",
                        "        _ = AppDomain.CurrentDomain;",
                        "        _ = new DynamicMethod(\"fixture\", typeof(void), Type.EmptyTypes);",
                        "    }",
                        "}",
                        "",
                    ]
                ),
                encoding="utf-8",
            )

            report = report_module.build_unsupported_feature_report_for_source_tree(
                source_root,
                subject_id="FixtureUnsupportedSubject",
            )
            self.assertEqual("v1", report["reportVersion"])
            self.assertEqual("fail", report["status"])
            self.assertEqual(2, report["statusCounts"]["fail"])
            self.assertEqual(
                {"appdomain", "reflection-emit"},
                {finding["ruleId"] for finding in report["findings"]},
            )

            output_path = fixture_root / "unsupported-feature-report.json"
            report_module.write_unsupported_feature_report(output_path, report)
            self.assertTrue(output_path.is_file())
        finally:
            shutil.rmtree(fixture_root, ignore_errors=True)

    def test_declared_contract_report_distinguishes_reporting_owned_missing_proof_and_missing_benchmark(self) -> None:
        report_module = load_module(
            UNSUPPORTED_FEATURE_REPORT_MODULE_PATH,
            "chaos_unsupported_feature_report_productization_contracts",
        )

        report = report_module.build_declared_contract_status_report(
            declared_unit_tests=[
                {
                    "subjectId": "SolutionCorePack",
                    "stableId": "solution-core/native-call-proof",
                    "capabilityFamily": 7,
                    "capabilityItem": 31,
                },
                {
                    "subjectId": "SolutionCorePack",
                    "stableId": "solution-core/closure-proof",
                    "capabilityFamily": 12,
                    "capabilityItem": 51,
                },
                {
                    "subjectId": "SolutionCorePack",
                    "stableId": "solution-core/runtime-trace-proof",
                    "capabilityFamily": 11,
                    "capabilityItem": 47,
                },
            ],
            declared_benchmarks=[
                {
                    "subjectId": "SolutionCorePack",
                    "stableId": "solution-core/native-call-bench",
                    "capabilityFamily": 7,
                    "capabilityItem": 31,
                },
                {
                    "subjectId": "HotUpdateHostPack",
                    "stableId": "hot-update/package-load-bench",
                    "capabilityFamily": 13,
                    "capabilityItem": 54,
                },
            ],
        )

        self.assertEqual("v1", report["reportVersion"])
        self.assertEqual("fail", report["status"])
        self.assertEqual(4, report["statusCounts"]["total"])
        self.assertEqual(2, report["statusCounts"]["ok"])
        self.assertEqual(2, report["statusCounts"]["fail"])
        self.assertEqual(1, report["classificationCounts"]["proofOptional"])
        self.assertEqual(1, report["classificationCounts"]["missingProof"])
        self.assertEqual(1, report["classificationCounts"]["missingBenchmark"])
        self.assertEqual(1, report["classificationCounts"]["ok"])

        results_by_item = {
            int(item["capabilityItem"]): item
            for item in report["contractResults"]
        }
        self.assertEqual("ok", results_by_item[31]["classification"])
        self.assertEqual("missingBenchmark", results_by_item[51]["classification"])
        self.assertEqual("missingProof", results_by_item[54]["classification"])
        self.assertEqual("proofOptional", results_by_item[47]["classification"])
        self.assertEqual(
            "reporting-owned-proof-optional",
            results_by_item[47]["reasonCode"],
        )
        self.assertTrue(results_by_item[47]["proofOptional"])
        self.assertTrue(results_by_item[47]["reportingOwned"])
        self.assertFalse(results_by_item[47]["missingProof"])
        self.assertFalse(results_by_item[47]["missingBenchmark"])
        self.assertFalse(results_by_item[47]["unsupported"])

    def test_soak_harness_collects_samples_for_successful_iterations(self) -> None:
        soak_module = load_module(
            SOAK_HARNESS_MODULE_PATH,
            "chaos_soak_harness_productization_success",
        )

        report = soak_module.run_soak_harness(
            [sys.executable, "-c", "import time; time.sleep(0.3)"],
            cwd=REPO_ROOT,
            duration_seconds=0.3,
            poll_interval_seconds=0.05,
            max_iterations=1,
        )

        self.assertEqual("v1", report["reportVersion"])
        self.assertEqual("ok", report["status"])
        self.assertEqual(1, report["statusCounts"]["totalIterations"])
        self.assertEqual(0, report["statusCounts"]["fail"])
        self.assertGreaterEqual(report["sampleCount"], 1)
        self.assertIsNotNone(report["peakWorkingSetBytes"])

    def test_soak_harness_marks_non_zero_exit_as_crash(self) -> None:
        soak_module = load_module(
            SOAK_HARNESS_MODULE_PATH,
            "chaos_soak_harness_productization_failure",
        )

        report = soak_module.run_soak_harness(
            [sys.executable, "-c", "import sys; sys.exit(3)"],
            cwd=REPO_ROOT,
            duration_seconds=0.1,
            poll_interval_seconds=0.02,
            max_iterations=1,
        )

        self.assertEqual("fail", report["status"])
        self.assertEqual(1, report["statusCounts"]["totalIterations"])
        self.assertEqual(1, report["statusCounts"]["fail"])
        self.assertEqual(1, report["crashedIterations"])
        self.assertEqual(3, report["iterations"][0]["exitCode"])
