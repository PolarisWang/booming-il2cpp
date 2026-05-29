#!/usr/bin/env python3
"""Smoke tests for the verification pipeline engine.

Tests core engine logic (stage counting, resume, timeout, failure summary)
without requiring real family directories or native builds.
"""

from __future__ import annotations

import json
import time
import sys
import tempfile
from pathlib import Path
from unittest.mock import MagicMock

# ── Path setup — allow running from repo root ─────────────────────
_HERE = Path(__file__).resolve().parent
_REPO = _HERE.parent.parent
sys.path.insert(0, str(_REPO / "testing" / "foundation-dll"))

from verification.orchestration.context import FamilyContext, StageResult, UnifiedReport, ParallelGroup
from verification.orchestration.engine import VerificationPipeline, _print_failure_summary, _format_duration

# ── Test helpers ──────────────────────────────────────────────────

PASSED = 0
FAILED = 0
ERRORS: list[str] = []


def _test(name: str):
    """Decorate a test function.  Appends to ERRORS on failure."""
    def decorator(fn):
        def wrapper(*a, **kw):
            global PASSED, FAILED
            try:
                fn(*a, **kw)
                PASSED += 1
            except Exception as e:
                FAILED += 1
                ERRORS.append(f"{name}: {e}")
        wrapper._test_name = name
        return wrapper
    return decorator


def _make_ctx(slug: str = "smoke-test", **kw) -> FamilyContext:
    """Create a minimal FamilyContext for testing."""
    return FamilyContext(
        slug=slug,
        assembly="System.Private.CoreLib",
        family_dir=Path(tempfile.mkdtemp(prefix="smoke_")),
        **kw,
    )


def _dummy_runner(status: str = "passed", summary: str = "ok", delay: float = 0):
    """Create a stage runner function that returns a fixed StageResult."""
    def runner(ctx, stages):
        if delay:
            time.sleep(delay)
        return StageResult(stage="dummy", status=status, summary=summary)
    return runner


# ── Test: _count_stages ───────────────────────────────────────────

@_test("count_stages: all sequential")
def test_count_stages_all_sequential():
    stages = [
        ("a", _dummy_runner(), "A"),
        ("b", _dummy_runner(), "B"),
        ("c", _dummy_runner(), "C"),
    ]
    assert VerificationPipeline._count_stages(stages) == 3


@_test("count_stages: with parallel group")
def test_count_stages_with_parallel():
    stages = [
        ("a", _dummy_runner(), "A"),
        ParallelGroup([
            ("b1", _dummy_runner(), "B1"),
            ("b2", _dummy_runner(), "B2"),
        ]),
        ("c", _dummy_runner(), "C"),
    ]
    assert VerificationPipeline._count_stages(stages) == 4


# ── Test: _format_duration ────────────────────────────────────────

@_test("format_duration: seconds")
def test_format_seconds():
    assert _format_duration(45) == "45s"


@_test("format_duration: minutes")
def test_format_minutes():
    assert _format_duration(125) == "2m5s"


# ── Test: StageResult dataclass ───────────────────────────────────

@_test("StageResult to_dict")
def test_stage_result_to_dict():
    sr = StageResult(stage="preflight", status="passed", summary="all good", duration_ms=42)
    d = sr.to_dict()
    assert d["stage"] == "preflight"
    assert d["status"] == "passed"
    assert d["duration_ms"] == 42


# ── Test: UnifiedReport load_from_file ────────────────────────────

@_test("UnifiedReport load_from_file valid")
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


@_test("UnifiedReport load_from_file missing")
def test_report_load_missing():
    assert UnifiedReport.load_from_file(Path("/nonexistent/report.json")) is None


# ── Test: Resume logic ────────────────────────────────────────────

@_test("pipeline resume skips passed stages")
def test_pipeline_resume():
    """Verify that a stage marked as 'passed' in the previous report is skipped."""
    # Create a previous report with preflight and codegen passed
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

    # Build a mini-pipeline
    pipeline = VerificationPipeline(ctx)
    pipeline.STAGES = [
        ("preflight", tracking_runner("preflight"), "Preflight"),
        ("codegen", tracking_runner("codegen"), "Codegen"),
        ("audit", tracking_runner("audit"), "Audit"),
    ]

    report = pipeline.run()
    assert report.overall_status == "passed"
    # preflight and codegen were in resume_passed, so only audit should execute
    assert executed == ["audit"], f"Expected only audit to execute, got: {executed}"


# ── Test: Skip stages ─────────────────────────────────────────────

@_test("pipeline skip_stages config")
def test_pipeline_skip_stages():
    executed = []

    def tracking_runner(name):
        def runner(ctx, stages):
            executed.append(name)
            return StageResult(stage=name, status="passed")
        return runner

    ctx = _make_ctx(skip_stages={"codegen"})
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

@_test("pipeline stage timeout")
def test_pipeline_timeout():
    ctx = _make_ctx(stage_timeout_seconds=1)  # 1s timeout
    pipeline = VerificationPipeline(ctx)
    pipeline.STAGES = [
        ("preflight", _dummy_runner(delay=3), "Preflight"),  # 3s > 1s → timeout
    ]

    report = pipeline.run()
    preflight = report.stages.get("preflight", {})
    assert preflight.get("status") == "error", f"Expected timeout error, got: {preflight}"
    assert "timed out" in (preflight.get("summary", "") or "").lower()


# ── Test: ParallelGroup execution ─────────────────────────────────

@_test("pipeline parallel group")
def test_pipeline_parallel_group():
    order = []

    def tracking_runner(name, delay=0):
        def runner(ctx, stages):
            if delay:
                time.sleep(delay)
            order.append(name)
            return StageResult(stage=name, status="passed")
        return runner

    ctx = _make_ctx()
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
    # Custom stage names don't match required stages, so overall_status may be "skipped"
    # Check individual stage results instead
    stages_dict = report.stages
    assert stages_dict.get("first", {}).get("status") == "passed"
    assert stages_dict.get("p1", {}).get("status") == "passed"
    assert stages_dict.get("p2", {}).get("status") == "passed"
    assert stages_dict.get("last", {}).get("status") == "passed"
    # first runs before the group
    assert order.index("first") < order.index("p1")
    assert order.index("first") < order.index("p2")
    # last runs after both parallel stages
    assert order.index("last") > order.index("p1")
    assert order.index("last") > order.index("p2")
    # p1 and p2 may be in any order (concurrent), but p2 (no delay) should finish before p1 (0.2s delay)
    # This is probabilistic: make it a soft check since thread scheduling is not guaranteed
    if order.index("p2") > order.index("p1"):
        print("NOTE: p2 finished after p1 despite no delay (scheduler variance)")


# ── Test: Fatal stage termination ─────────────────────────────────

@_test("pipeline fatal stage stops pipeline")
def test_pipeline_fatal():
    executed = []

    def tracking_runner(name):
        def runner(ctx, stages):
            executed.append(name)
            return StageResult(stage=name, status="failed")
        return runner

    ctx = _make_ctx()
    pipeline = VerificationPipeline(ctx)
    pipeline.STAGES = [
        ("preflight", tracking_runner("preflight"), "Preflight"),
        ("codegen", tracking_runner("codegen"), "Codegen"),
    ]

    pipeline.run()
    # preflight is FATAL → pipeline stops, codegen never runs
    assert executed == ["preflight"]


# ── Test: _print_failure_summary ──────────────────────────────────

@_test("_print_failure_summary formats correctly")
def test_failure_summary():
    stages = {
        "preflight": StageResult(stage="preflight", status="passed"),
        "codegen": StageResult(stage="codegen", status="failed", summary="compile error"),
        "benchmark": StageResult(stage="benchmark", status="error", summary="OOM"),
    }
    # Test that this doesn't crash
    try:
        _print_failure_summary(stages)
    except Exception as e:
        raise AssertionError(f"_print_failure_summary crashed: {e}")


# ── Main ──────────────────────────────────────────────────────────

def main():
    global PASSED, FAILED
    # Collect all test_ functions
    test_fns = [(name, fn) for name, fn in globals().items()
                if name.startswith("test_") and callable(fn)]

    print(f"Running {len(test_fns)} pipeline engine smoke tests...\n")

    for name, fn in test_fns:
        fn()

    print(f"\n{'='*50}")
    print(f"  PASSED: {PASSED}")
    print(f"  FAILED: {FAILED}")
    print(f"{'='*50}")

    if ERRORS:
        print("\nErrors:")
        for err in ERRORS:
            print(f"  - {err}")

    return 0 if FAILED == 0 else 1


if __name__ == "__main__":
    sys.exit(main())
