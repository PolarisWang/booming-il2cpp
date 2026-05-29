"""Tests for engine.py — pipeline stage execution, aggregation, coverage, dashboard, regression."""

import json
from pathlib import Path

from verification.orchestration.context import FamilyContext, StageResult, UnifiedReport
from verification.orchestration.engine import (
    REQUIRED_STAGES_STANDARD,
    REQUIRED_STAGES_STRICT,
    VerificationPipeline,
    aggregate_report,
    build_dashboard_metrics,
    compute_coverage,
    detect_regression,
)


# ── Pipeline Execution ─────────────────────────────────────────────

class TestVerificationPipeline:
    def test_skip_stages(self, mock_ctx):
        ctx = FamilyContext(
            slug=mock_ctx.slug,
            assembly=mock_ctx.assembly,
            family_dir=mock_ctx.family_dir,
            mode=mock_ctx.mode,
            skip_stages={"benchmark", "hotupdate"},
        )
        pipeline = VerificationPipeline(ctx)
        skipped = {name for name, _, _ in pipeline.STAGES if name in ctx.skip_stages}
        assert "benchmark" in skipped
        assert "hotupdate" in skipped
        assert "preflight" not in skipped

    def test_stage_list_order(self):
        ctx = FamilyContext(slug="test", assembly="Test.Asm", family_dir="/tmp")
        pipeline = VerificationPipeline(ctx)
        names = [name for name, _, _ in pipeline.STAGES]
        assert names[0] == "preflight"
        assert names[2] == "jit_codegen"
        assert names[3] == "managed_fact"
        assert names[4] == "cross_verify"
        assert names[-1] == "hotupdate_jit_benchmark"
        assert len(names) == 15

    def test_fatal_stages_contains_preflight(self):
        assert "preflight" in VerificationPipeline.FATAL_STAGES

    def test_run_returns_report(self, tmp_path):
        contract_path = tmp_path / "capability-family-contract.json"
        contract_path.write_text('{"methodSubjectIds": ["Test::Method"]}', encoding="utf-8")
        ctx = FamilyContext(
            slug="integration-test",
            assembly="Test.Asm",
            family_dir=tmp_path,
            skip_stages={name for name, _, _ in VerificationPipeline.STAGES if name != "preflight"},
        )
        pipeline = VerificationPipeline(ctx)
        report = pipeline.run()
        assert report.overall_status == "passed"
        assert report.family == "integration-test"
        assert report.assembly == "Test.Asm"


# ── Coverage ───────────────────────────────────────────────────────

class TestComputeCoverage:
    def test_all_passed(self, sample_stages):
        stages = {name: StageResult(stage=name, status="passed") for name in sample_stages}
        cov = compute_coverage(stages)
        assert cov["stagePassRate"] == 100.0
        assert cov["stagesPassed"] == 5
        assert cov["stagesFailed"] == 0
        assert cov["stagesTotal"] == 5

    def test_mixed_status(self, sample_stages):
        cov = compute_coverage(sample_stages)
        assert cov["stagesPassed"] == 3
        assert cov["stagesFailed"] == 1
        assert cov["stagesSkipped"] == 1
        assert cov["stagesTotal"] == 5

    def test_empty_stages(self):
        cov = compute_coverage({})
        assert cov["stagePassRate"] == 100.0
        assert cov["stagesTotal"] == 0


# ── Dashboard ──────────────────────────────────────────────────────

class TestBuildDashboard:
    def test_no_benchmark(self):
        stages = {"preflight": StageResult(stage="preflight", status="passed")}
        dash = build_dashboard_metrics(stages)
        assert dash == {}

    def test_benchmark_summary(self):
        stages = {
            "benchmark": StageResult(
                stage="benchmark", status="passed",
                details={
                    "native-aot": {
                        "status": "passed", "label": "native-aot",
                        "results": [
                            {"opsPerSecond": 100_000_000},
                            {"opsPerSecond": 200_000_000},
                            {"opsPerSecond": 300_000_000},
                        ],
                        "okCount": 3, "failCount": 0, "totalMethods": 3,
                        "averageOpsPerSecond": 200_000_000,
                    },
                    "native-jit": {
                        "status": "passed", "label": "native-jit",
                        "results": [
                            {"opsPerSecond": 50_000_000},
                            {"opsPerSecond": 100_000_000},
                            {"opsPerSecond": 25_000_000},
                        ],
                        "okCount": 3, "failCount": 0, "totalMethods": 3,
                        "averageOpsPerSecond": 58_333_333,
                    },
                },
            ),
            "asm_compare": StageResult(
                stage="asm_compare", status="passed",
                details={
                    "status": "passed",
                    "details": {
                        "overallIrExpansionRatio": 0.42,
                        "asmPassRate": 100.0,
                    },
                },
            ),
        }
        dash = build_dashboard_metrics(stages)
        ratios = dash["keyRatios"]
        assert ratios["nativeFasterRatio"] == 1.0
        assert ratios["nativeAotOpsPerSecond"] == 200_000_000
        assert ratios["nativeJitOpsPerSecond"] == 58_333_333
        assert dash["irExpansionRatio"] == 0.42
        assert dash["asmPassRate"] == 100.0
        assert dash["averageSpeedupPercent"] == 433.3

    def test_benchmark_summary_empty_details(self):
        stages = {"benchmark": StageResult(stage="benchmark", status="passed", details={})}
        dash = build_dashboard_metrics(stages)
        ratios = dash["keyRatios"]
        assert ratios["nativeFasterRatio"] == 0
        assert ratios["managedFasterRatio"] == 0
        assert ratios.get("nativeAotOpsPerSecond", 0) == 0
        assert dash["averageSpeedupPercent"] == 0


# ── Regression ─────────────────────────────────────────────────────

class TestDetectRegression:
    def _deep_ctx(self, base: Path) -> FamilyContext:
        return FamilyContext(
            slug="test-family",
            assembly="Test.Assembly",
            family_dir=base / "assembly" / "test-family",
        )

    def test_no_baseline(self, tmp_path):
        reg = detect_regression(self._deep_ctx(tmp_path))
        assert reg["hasRegression"] is False
        assert reg["benchmark"]["status"] == "no_baseline"

    def test_baseline_read_error(self, tmp_path):
        family_dir = tmp_path / "assembly" / "test-family"
        ctx = FamilyContext(
            slug="test-family",
            assembly="Test.Assembly",
            family_dir=family_dir,
        )
        reg = detect_regression(ctx)
        assert reg["benchmark"]["status"] == "no_baseline"


# ── Aggregation ────────────────────────────────────────────────────

class TestAggregate:
    @staticmethod
    def _ctx(slug="test-family", assembly="Test.Assembly", mode="standard",
             family_dir=None, tmp_path=None) -> FamilyContext:
        base = (tmp_path / "testing" / "foundation-dll") if tmp_path else Path("/tmp/testing/foundation-dll")
        return FamilyContext(
            slug=slug, assembly=assembly,
            family_dir=family_dir or (base / assembly / slug),
            mode=mode,
        )

    def test_all_pass_standard(self, tmp_path, sample_stages):
        ctx = self._ctx(tmp_path=tmp_path)
        stages = {name: StageResult(stage=name, status="passed") for name in sample_stages}
        report = aggregate_report(ctx, stages, 1000)
        assert report.overall_status == "passed"
        assert report.family == "test-family"
        assert report.assembly == "Test.Assembly"
        assert report.mode == "standard"
        assert report.duration_ms == 1000

    def test_required_failure(self, tmp_path, sample_stages):
        ctx = self._ctx(tmp_path=tmp_path)
        report = aggregate_report(ctx, sample_stages, 500)
        assert report.overall_status == "failed"

    def test_strict_mode_requires_hotupdate(self, tmp_path, sample_stages):
        ctx = self._ctx(mode="strict", tmp_path=tmp_path)
        stages = {name: StageResult(stage=name, status="passed") for name in sample_stages}
        for hu in ("hotupdate", "hotupdate_aot_benchmark", "hotupdate_jit_fact", "hotupdate_jit_benchmark"):
            stages[hu] = StageResult(stage=hu, status="passed")
        report = aggregate_report(ctx, stages, 1000)
        assert report.overall_status == "passed"

    def test_report_contains_coverage(self, tmp_path, sample_stages):
        ctx = self._ctx(tmp_path=tmp_path)
        stages = {name: StageResult(stage=name, status="passed") for name in sample_stages}
        report = aggregate_report(ctx, stages, 100)
        assert report.coverage["stagePassRate"] == 100.0
        assert report.coverage["stagesTotal"] == 5

    def test_report_contains_stages(self, tmp_path, sample_stages):
        ctx = self._ctx(tmp_path=tmp_path)
        report = aggregate_report(ctx, sample_stages, 200)
        assert "preflight" in report.stages
        assert "codegen" in report.stages
        assert report.stages["preflight"]["status"] == "passed"
