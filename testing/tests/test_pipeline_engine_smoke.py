#!/usr/bin/env python3
"""Smoke tests for the verification pipeline engine.

Tests core engine logic (stage counting, resume, timeout, failure summary)
without requiring real family directories or native builds.
"""

from __future__ import annotations

import json
import time
import tempfile
from pathlib import Path

from verification.orchestration.context import FamilyContext, StageResult, UnifiedReport, ParallelGroup
from verification.orchestration.engine import VerificationPipeline, print_failure_summary, format_duration


def make_context(slug: str = "smoke-test", **kw) -> FamilyContext:
    """Create a minimal FamilyContext for testing."""
    return FamilyContext(
        slug=slug,
        assembly="System.Private.CoreLib",
        family_dir=Path(tempfile.mkdtemp(prefix="smoke_")),
        **kw,
    )


def dummy_runner(status: str = "passed", summary: str = "ok", delay: float = 0):
    """Create a stage runner function that returns a fixed StageResult."""
    def runner(ctx, stages):
        if delay:
            time.sleep(delay)
        return StageResult(stage="dummy", status=status, summary=summary)
    return runner


# ── Test: _count_stages ───────────────────────────────────────────

def test_count_stages_all_sequential():
    stages = [
        ("a", dummy_runner(), "A"),
        ("b", dummy_runner(), "B"),
        ("c", dummy_runner(), "C"),
    ]
    assert VerificationPipeline.count_stages(stages) == 3


def test_count_stages_with_parallel():
    stages = [
        ("a", dummy_runner(), "A"),
        ParallelGroup([
            ("b1", dummy_runner(), "B1"),
            ("b2", dummy_runner(), "B2"),
        ]),
        ("c", dummy_runner(), "C"),
    ]
    assert VerificationPipeline.count_stages(stages) == 4


# ── Test: _format_duration ────────────────────────────────────────

def test_format_seconds():
    assert format_duration(45) == "45s"


def test_format_minutes():
    assert format_duration(125) == "2m5s"


# ── Test: StageResult dataclass ───────────────────────────────────

def test_stage_result_to_dict():
    sr = StageResult(stage="preflight", status="passed", summary="all good", duration_ms=42)
    d = sr.to_dict()
    assert d["stage"] == "preflight"
    assert d["status"] == "passed"
    assert d["duration_ms"] == 42


# ── Test: UnifiedReport load_from_file ────────────────────────────

def test_report_load_valid():
    data = {
        "family": "test", "assembly": "X", "mode": "standard",
        "overall_status": "passed", "duration_ms": 100,
        "stages": {"codegen": {"stage": "codegen", "status": "passed"}},
    }
    with tempfile.NamedTemporaryFile(mode="w", suffix=".json", delete=False) as f:
        json.dump(data, f)
        p = Path(f.name)
    try:
        report = UnifiedReport.load_from_file(p)
        assert report is not None
        assert report.overall_status == "passed"
        assert "codegen" in report.stages
    finally:
        p.unlink()


def test_report_load_missing():
    assert UnifiedReport.load_from_file(Path("/nonexistent/report.json")) is None


# ── Test: Resume logic ────────────────────────────────────────────

def test_pipeline_resume():
    """Verify that a stage marked as 'passed' in the previous report is skipped."""
    prev = {
        "family": "smoke", "assembly": "X", "mode": "standard",
        "overall_status": "passed", "stages": {
            "preflight": {"stage": "preflight", "status": "passed"},
            "codegen": {"stage": "codegen", "status": "passed"},
        },
        "coverage": {}, "dashboard": {}, "regression": {},
    }

    family_dir = Path(tempfile.mkdtemp(prefix="smoke_resume_"))

    executed = []

    def tracking_runner(name):
        def runner(ctx, stages):
            executed.append(name)
            return StageResult(stage=name, status="passed")
        return runner

    ctx = FamilyContext(
        slug="smoke", assembly="X", family_dir=family_dir,
        resume=True,
    )
    # Write previous report at the actual results_dir path
    actual_results_dir = ctx.results_dir
    actual_results_dir.mkdir(parents=True, exist_ok=True)
    report_path = actual_results_dir / "unified-verification-report.json"
    report_path.write_text(json.dumps(prev), encoding="utf-8")

    pipeline = VerificationPipeline(ctx)
    pipeline.STAGES = [
        ("preflight", tracking_runner("preflight"), "Preflight"),
        ("codegen", tracking_runner("codegen"), "Codegen"),
        ("audit", tracking_runner("audit"), "Audit"),
    ]

    report = pipeline.run()
    assert report.overall_status == "passed"
    assert executed == ["audit"], f"Expected only audit to execute, got: {executed}"


# ── Test: Skip stages ─────────────────────────────────────────────

def test_pipeline_skip_stages():
    executed = []

    def tracking_runner(name):
        def runner(ctx, stages):
            executed.append(name)
            return StageResult(stage=name, status="passed")
        return runner

    ctx = make_context(skip_stages={"codegen"})
    pipeline = VerificationPipeline(ctx)
    pipeline.STAGES = [
        ("preflight", tracking_runner("preflight"), "Preflight"),
        ("codegen", tracking_runner("codegen"), "Codegen"),
        ("audit", tracking_runner("audit"), "Audit"),
    ]

    pipeline.run()
    assert "codegen" not in executed
    assert "preflight" in executed
    assert "audit" in executed


# ── Test: Timeout ─────────────────────────────────────────────────

def test_pipeline_timeout():
    ctx = make_context(stage_timeout_seconds=1)
    pipeline = VerificationPipeline(ctx)
    pipeline.STAGES = [
        ("preflight", dummy_runner(delay=3), "Preflight"),
    ]

    report = pipeline.run()
    preflight = report.stages.get("preflight", {})
    assert preflight.get("status") == "error", f"Expected timeout error, got: {preflight}"
    assert "timed out" in (preflight.get("summary", "") or "").lower()


# ── Test: ParallelGroup execution ─────────────────────────────────

def test_pipeline_parallel_group():
    order = []

    def tracking_runner(name, delay=0):
        def runner(ctx, stages):
            if delay:
                time.sleep(delay)
            order.append(name)
            return StageResult(stage=name, status="passed")
        return runner

    ctx = make_context()
    pipeline = VerificationPipeline(ctx)
    pipeline.STAGES = [
        ("first", tracking_runner("first"), "First"),
        ParallelGroup([
            ("p1", tracking_runner("p1", delay=0.2), "P1"),
            ("p2", tracking_runner("p2"), "P2"),
        ]),
        ("last", tracking_runner("last"), "Last"),
    ]

    report = pipeline.run()
    stages_dict = report.stages
    assert stages_dict.get("first", {}).get("status") == "passed"
    assert stages_dict.get("p1", {}).get("status") == "passed"
    assert stages_dict.get("p2", {}).get("status") == "passed"
    assert stages_dict.get("last", {}).get("status") == "passed"
    assert order.index("first") < order.index("p1")
    assert order.index("first") < order.index("p2")
    assert order.index("last") > order.index("p1")
    assert order.index("last") > order.index("p2")


# ── Test: Fatal stage termination ─────────────────────────────────

def test_pipeline_fatal():
    executed = []

    def tracking_runner(name):
        def runner(ctx, stages):
            executed.append(name)
            return StageResult(stage=name, status="failed")
        return runner

    ctx = make_context()
    pipeline = VerificationPipeline(ctx)
    pipeline.STAGES = [
        ("preflight", tracking_runner("preflight"), "Preflight"),
        ("codegen", tracking_runner("codegen"), "Codegen"),
    ]

    pipeline.run()
    assert executed == ["preflight"]


# ── Test: _print_failure_summary ──────────────────────────────────

def test_failure_summary():
    stages = {
        "preflight": StageResult(stage="preflight", status="passed"),
        "codegen": StageResult(stage="codegen", status="failed", summary="compile error"),
        "benchmark": StageResult(stage="benchmark", status="error", summary="OOM"),
    }
    try:
        print_failure_summary(stages)
    except Exception as e:
        raise AssertionError(f"_print_failure_summary crashed: {e}")
