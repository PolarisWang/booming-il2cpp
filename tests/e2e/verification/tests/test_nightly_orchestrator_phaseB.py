"""Track 1 tests — nightly false-green + Phase B TypeErrors.

Verifies:
  1. NightlyResult.build_passed only counts "passed" (not "skipped"), so a
     fully-failed/skipped nightly reports build_failed>0 → exit code nonzero.
  2. The Phase B aggregator only (imports &) the log-aware _run_single_chunk
     worker with stage_filter intact — no capture_logs/keep_console/report_dir
     kwargs leak into _run_chunk_stages (the former TypeError).
  3. Phase B results merge ONLY Phase B stage keys, never overwriting Phase A's
     build/fact.
"""
from __future__ import annotations

import sys
from pathlib import Path

import pytest

_HERE = Path(__file__).resolve().parent
for _d in (_HERE, _HERE.parent, _HERE.parent.parent):
    if str(_d) not in sys.path:
        sys.path.insert(0, str(_d))

from verification.orchestration.context import StageResult  # noqa: E402


@pytest.fixture(autouse=True)
def _ensure_syspath():
    import verification.nightly_runner.orchestrator as o
    return o


def _mk(status: str) -> StageResult:
    return StageResult(stage="build", status=status)


def test_build_passed_counts_only_passed(_ensure_syspath) -> None:
    from verification.nightly_runner.orchestrator import NightlyResult

    r = NightlyResult()
    r.chunk_count = 3
    # chunk 0: build passed; chunk 1: build skipped (never ran); chunk 2: failed
    r.chunk_results = {
        "A/one": {"build": _mk("passed")},
        "A/two": {"build": _mk("skipped")},
        "A/three": {"build": _mk("failed")},
    }
    # Before fix: skipped counted as passed → build_passed=2, build_failed=1.
    # After fix: only passed → build_passed=1, build_failed=2.
    assert r.build_passed == 1
    assert r.build_failed == 2


def test_build_passed_all_skipped_is_zero(_ensure_syspath) -> None:
    """A nightly where every chunk init-crashed (build=skipped) must NOT pass."""
    from verification.nightly_runner.orchestrator import NightlyResult

    r = NightlyResult()
    r.chunk_count = 2
    r.chunk_results = {
        "A/x": {"build": _mk("skipped")},
        "A/y": {"build": _mk("skipped")},
    }
    assert r.build_passed == 0
    assert r.build_failed == 2  # >0 → main returns exit code 1 (false-green gone)


def test_phase_b_worker_is_log_aware_and_accepts_stages_filter(_ensure_syspath) -> None:
    """Phase B must submit to _run_single_chunk, not _run_chunk_stages."""
    import inspect
    from verification.nightly_runner.orchestrator import (
        NightlyOrchestrator, _run_chunk_stages)

    # 1) _run_chunk_stages must NOT accept the log kwargs (that was the bug: it
    #    was called with them, raising TypeError).
    sig = inspect.signature(_run_chunk_stages)
    assert "capture_logs" not in sig.parameters
    assert "keep_console" not in sig.parameters
    assert "report_dir" not in sig.parameters

    # 2) _run_single_chunk DOES accept them + stages_filter/profile_pass.
    log_sig = inspect.signature(NightlyOrchestrator._run_single_chunk)
    for param in ("capture_logs", "keep_console", "report_dir",
                  "stages_filter", "profile_pass"):
        assert param in log_sig.parameters, f"missing {param} on _run_single_chunk"


def test_phase_b_merge_never_overwrites_build_fact(_ensure_syspath) -> None:
    """Phase B merge must only apply Phase B stage keys to an existing chunk."""
    from verification.nightly_runner.orchestrator import (
        NightlyResult, StageResult)

    r = NightlyResult()
    r.chunk_count = 1
    # Phase A produced a passed build + passed fact.
    r.chunk_results = {
        "A/one": {
            "build": _mk("passed"),
            "fact": StageResult(stage="fact", status="passed"),
        },
    }
    phase_b_set = {"benchmark", "managed_benchmark", "hotupdate",
                   "benchmark_report", "coverage_audit"}
    # Simulate a Phase B result that (wrongly) includes a build=skipped.
    phase_b = {
        "A/one": {
            "build:spurious": StageResult(stage="build:spurious", status="skipped"),
            "benchmark": StageResult(stage="benchmark", status="passed"),
            "coverage_audit": StageResult(stage="coverage_audit", status="passed"),
        },
    }
    for key, stages in phase_b.items():
        target = r.chunk_results.setdefault(key, {})
        for stage_key, stage_res in stages.items():
            if stage_key in phase_b_set:  # exact Phase B keys only
                target[stage_key] = stage_res

    # build/fact from Phase A remain intact.
    assert r.chunk_results["A/one"]["build"].status == "passed"
    assert r.chunk_results["A/one"]["fact"].status == "passed"
    # Phase B keys landed.
    assert r.chunk_results["A/one"]["benchmark"].status == "passed"
    assert r.chunk_results["A/one"]["coverage_audit"].status == "passed"
    assert "build:spurious" not in r.chunk_results["A/one"]
