from __future__ import annotations

import importlib.util
import json
import shutil
import subprocess
import sys
import unittest
import uuid
from pathlib import Path


REPO_ROOT = Path(__file__).resolve().parents[3]
HOT_UPDATE_ROOT = REPO_ROOT / "src" / "managed" / "Chaos.IL2CPP.HotUpdate"

PACKAGE_VALIDATOR_PATH = HOT_UPDATE_ROOT / "PackageValidator.cs"
RUNTIME_MANAGER_PATH = HOT_UPDATE_ROOT / "RuntimeManager.cs"
VERSION_HELPER_PATH = HOT_UPDATE_ROOT / "HotUpdateVersionCompatibility.cs"

IOS_POLICY_PATH = REPO_ROOT / "docs" / "architecture" / "ios-distribution-policy.md"
RELEASE_CHECKLIST_PATH = REPO_ROOT / "docs" / "architecture" / "release-checklist.md"
VERSION_MATRIX_PATH = REPO_ROOT / "docs" / "architecture" / "version-compatibility-matrix.md"

HOT_UPDATE_SKELETON_PROJECT_PATH = (
    REPO_ROOT / "subjects" / "HotUpdateHostPack" / "source" / "HotUpdateHostPack.csproj"
)
HOT_UPDATE_SKELETON_PROOF_PATH = (
    REPO_ROOT / "subjects" / "HotUpdateHostPack" / "source" / "Host" / "Proofs" / "HotUpdateSkeletonProofEntry.cs"
)
COMPATIBILITY_MATRIX_RUNNER_PATH = (
    REPO_ROOT / "build" / "toolchains" / "run" / "testing" / "compatibility_matrix_runner.py"
)
COMPATIBILITY_MATRIX_CONFIG_PATH = (
    REPO_ROOT / "tests" / "fixtures" / "subjects" / "CompatibilityMatrixProof" / "compatibility-matrix.json"
)
COMPATIBILITY_MATRIX_SUBJECT_MANIFEST_PATH = (
    REPO_ROOT / "tests" / "fixtures" / "subjects" / "CompatibilityMatrixProof" / "subject.manifest.json"
)
COMPATIBILITY_MATRIX_TMP_ROOT = REPO_ROOT / "artifacts" / ".tmp-tests" / "compatibility-matrix"
PERF_DASHBOARD_MODULE_PATH = REPO_ROOT / "build" / "toolchains" / "run" / "testing" / "perf_dashboard.py"
UNSUPPORTED_FEATURE_REPORT_MODULE_PATH = (
    REPO_ROOT / "build" / "toolchains" / "run" / "testing" / "unsupported_feature_report.py"
)
SOAK_HARNESS_MODULE_PATH = REPO_ROOT / "build" / "toolchains" / "run" / "testing" / "soak_harness.py"
BATCH4_TMP_ROOT = REPO_ROOT / "artifacts" / ".tmp-tests" / "phase8-productization-batch4"


def run_checked(arguments: list[str], *, cwd: Path) -> subprocess.CompletedProcess[str]:
    completed = subprocess.run(
        arguments,
        cwd=cwd,
        capture_output=True,
        text=True,
        encoding="utf-8",
        errors="replace",
        check=False,
    )
    if completed.returncode != 0:
        combined_output = "\n".join(part for part in [completed.stdout, completed.stderr] if part)
        raise AssertionError(f"command failed ({completed.returncode}): {' '.join(arguments)}\n{combined_output}")
    return completed


def load_module(path: Path, module_name: str):
    if not path.is_file():
        raise FileNotFoundError(f"module missing: {path}")

    spec = importlib.util.spec_from_file_location(module_name, path)
    if spec is None or spec.loader is None:
        raise RuntimeError(f"unable to load module: {path}")

    module = importlib.util.module_from_spec(spec)
    sys.modules[spec.name] = module
    spec.loader.exec_module(module)
    return module


class Phase8ProductizationGatesTests(unittest.TestCase):
    def test_package_validator_uses_version_compatibility_band_instead_of_exact_string_equality(self) -> None:
        validator_source = PACKAGE_VALIDATOR_PATH.read_text(encoding="utf-8")

        self.assertTrue(
            VERSION_HELPER_PATH.is_file(),
            msg=f"missing hot update version compatibility helper: {VERSION_HELPER_PATH}",
        )

        helper_source = VERSION_HELPER_PATH.read_text(encoding="utf-8")
        for required_fragment in [
            "record struct HotUpdateSemanticVersion",
            "TryParse",
            "IsCompatibleWith",
            "Major",
            "Minor",
            "Patch",
        ]:
            self.assertIn(required_fragment, helper_source)

        for required_fragment in [
            "HotUpdateSemanticVersion",
            "TryParse",
            "IsCompatibleWith",
            "signature must not be empty",
        ]:
            self.assertIn(required_fragment, validator_source)

        self.assertNotIn("string.Equals(targetAotVersion, currentAotVersion", validator_source)

    def test_productization_docs_define_ios_distribution_boundary_and_version_matrix(self) -> None:
        self.assertTrue(IOS_POLICY_PATH.is_file(), msg=f"missing ios distribution policy doc: {IOS_POLICY_PATH}")
        self.assertTrue(VERSION_MATRIX_PATH.is_file(), msg=f"missing version compatibility matrix doc: {VERSION_MATRIX_PATH}")

        ios_policy_source = IOS_POLICY_PATH.read_text(encoding="utf-8")
        version_matrix_source = VERSION_MATRIX_PATH.read_text(encoding="utf-8")

        for required_fragment in [
            "App Store 2.5.2",
            "配置/数据热更新",
            "逻辑热更新",
            "发布合规",
        ]:
            self.assertIn(required_fragment, ios_policy_source)

        for required_fragment in [
            "AOT 主包版本",
            "hot update patch",
            "major.minor",
            "major 不兼容",
        ]:
            self.assertIn(required_fragment, version_matrix_source)

    def test_release_checklist_doc_covers_required_productization_gates(self) -> None:
        self.assertTrue(RELEASE_CHECKLIST_PATH.is_file(), msg=f"missing release checklist doc: {RELEASE_CHECKLIST_PATH}")

        checklist_source = RELEASE_CHECKLIST_PATH.read_text(encoding="utf-8")
        for required_fragment in [
            "版本兼容",
            "rollback",
            "mobile runtime host",
            "iOS distribution policy",
            "perf regression",
            "soak test",
            "unsupported feature report",
        ]:
            self.assertIn(required_fragment, checklist_source)

    def test_runtime_manager_defines_rollback_active_patch_listing_and_integrity_validation(self) -> None:
        runtime_manager_source = RUNTIME_MANAGER_PATH.read_text(encoding="utf-8")

        for required_fragment in [
            "record HotUpdateIntegrityReport",
            "Rollback()",
            "GetActivePatches()",
            "ValidateIntegrity()",
            "SupplementalMetadataLoader",
            "PackageValidator.ValidateCompatibleTargetAotVersion",
        ]:
            self.assertIn(required_fragment, runtime_manager_source)

    def test_hot_update_skeleton_proof_runs_apply_integrity_rollback_and_reapply(self) -> None:
        proof_source = HOT_UPDATE_SKELETON_PROOF_PATH.read_text(encoding="utf-8")
        for required_fragment in [
            "ValidateIntegrity()",
            "GetActivePatches().Count",
            "runtimeManager.Rollback();",
            'Assert.Equal(42, afterReapply);',
            'Assert.Equal(1, afterUnload);',
        ]:
            self.assertIn(required_fragment, proof_source)

        completed = run_checked(
            [
                "dotnet",
                "run",
                "--project",
                str(HOT_UPDATE_SKELETON_PROJECT_PATH),
                "--",
                "phase8",
            ],
            cwd=REPO_ROOT,
        )
        self.assertEqual("", completed.stdout.strip())
        self.assertEqual("", completed.stderr.strip())

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
            "chaos_compatibility_matrix_runner_phase8",
        )

        run_id = f"phase8-productization-{uuid.uuid4().hex}"
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
            "chaos_perf_dashboard_phase8",
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
        self.assertTrue(solution_core_pack_perf["baselinePath"].startswith("subjects/SolutionCorePack/baselines/perf/"))
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

    def test_unsupported_feature_report_scanner_flags_fixture_patterns(self) -> None:
        report_module = load_module(
            UNSUPPORTED_FEATURE_REPORT_MODULE_PATH,
            "chaos_unsupported_feature_report_phase8",
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

    def test_soak_harness_collects_samples_for_successful_iterations(self) -> None:
        soak_module = load_module(
            SOAK_HARNESS_MODULE_PATH,
            "chaos_soak_harness_phase8_success",
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
            "chaos_soak_harness_phase8_failure",
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


if __name__ == "__main__":
    unittest.main()
