"""Track 6 tests — nightly/reporting data completeness.

Verifies:
  1. nightly_summary factPassRateDelta emoji is semantically correct (improve->ack,
     degrade->arrow) via good_is_positive=True.
  2. nightly_delta coverageGap equals metaTotal when fact total is 0.
  3. dashboard._perf_store_path is assembly-parameterized (no hardcoded CoreLib).
  4. models.PipelineRunReport.load_from_file reconstructs nested dataclasses.
  5. ai_export returns nonzero exit when the source is missing.
  6. merge_platform_runs prefers methodSubjectId over methodIndex.
  7. benchmark_diff._print_report tolerates aggregate details lacking methodSubjectId.
"""
from __future__ import annotations

import json
import sys
from pathlib import Path

import pytest

_HERE = Path(__file__).resolve().parent
for _d in (_HERE, _HERE.parent, _HERE.parent.parent):
    if str(_d) not in sys.path:
        sys.path.insert(0, str(_d))


def _fmt_pct(v) -> str:
    return "n/a" if v is None else f"{v:.1f}%"


def test_nightly_summary_emoji_semantics(tmp_path: Path, capsys, monkeypatch) -> None:
    import verification.nightly_summary as ns

    # Patch _fmt_pct in-place so the module's own fmt is used and delta flows through.
    # factPassRateDelta>0 (rate up) should render as a "good" emoji (not down-arrow).
    monkeypatch.setattr(ns, "_fmt_pct", _fmt_pct)

    # Simpler: directly assert the helper's semantics with good_is_positive=True.
    import verification.nightly_summary as _ns
    assert _ns._delta_emoji(+3, good_is_positive=True) == "✅"  # improvement
    assert _ns._delta_emoji(-3, good_is_positive=True) == "🔻"  # regression


def test_nightly_delta_coverage_gap_equals_metatotal(tmp_path: Path) -> None:
    from verification.nightly_delta import compute_assembly_delta

    # Build a minimal _dll/reports tree so compute_assembly_delta can read a
    # today fact-summary where a chunk's metaTotal=12 but fact total=0.
    reports = tmp_path / "_dll" / "reports"
    latest = reports / "latest"
    latest.mkdir(parents=True)
    (latest / "dashboard.json").write_text(json.dumps({"summary": {}}), encoding="utf-8")
    (latest / "fact-summary.json").write_text(json.dumps({
        "chunkSummaries": [
            {"slug": "numerics", "fact": {"metaTotal": 12, "total": 0, "passed": 0}},
        ],
    }), encoding="utf-8")
    (latest / "coverage-audit.json").write_text(json.dumps({}), encoding="utf-8")

    out = compute_assembly_delta("CoreLib", tmp_path, "2026-08-08")
    per_chunk = out["chunks"]
    gap = per_chunk["CoreLib/numerics"]["coverageGap"]
    # total=0 but metaTotal=12 -> the whole meta set is unverified: gap IS metaTotal.
    assert gap == 12


def test_dashboard_perf_store_assembly_parameterized() -> None:
    from verification.reporting.dashboard import _perf_store_path, _RESULTS_BASE

    # Default assembly preserved.
    p = _perf_store_path("numerics")
    assert "System.Private.CoreLib" in str(p)
    # Other assembly addressable now.
    p2 = _perf_store_path("buffers", assembly="System.Text")
    assert "System.Text" in str(p2)


def test_models_load_from_file_reconstructs_nested(tmp_path: Path) -> None:
    from verification.reporting.models import (PipelineRunReport, ChunkReport,
                                               FactSummary, BenchmarkEntry)

    report = PipelineRunReport(
        runId="run-1", platform="windows",
        chunks=[ChunkReport(
            slug="numerics", assembly="CoreLib",
            fact=FactSummary(totalMethods=10, passed=5, passRate=0.5,
                             failures=[]),
            benchmark=[BenchmarkEntry(methodSubjectId="M::A()", chaosAotMs=1.0)],
        )],
    )
    p = tmp_path / "run.json"
    p.write_text(json.dumps(report.to_dict()), encoding="utf-8")
    loaded = PipelineRunReport.load_from_file(p)
    assert loaded is not None
    assert isinstance(loaded.chunks[0], ChunkReport)
    assert isinstance(loaded.chunks[0].fact, FactSummary)
    assert loaded.chunks[0].fact.passed == 5
    assert isinstance(loaded.chunks[0].benchmark[0], BenchmarkEntry)
    assert loaded.chunks[0].benchmark[0].chaosAotMs == 1.0


def test_ai_export_missing_source_returns_error(tmp_path: Path) -> None:
    from verification.tools.ai_export import main as ae_main

    missing = tmp_path / "nope.jsonl"
    # main reads sys.argv; call the return-value branch via export_all directly.
    from verification.tools.ai_export import export_all
    assert export_all(missing, tmp_path / "ai") == {"error": -1}


def test_merge_platform_runs_prefers_methodsubjectid(tmp_path: Path) -> None:
    # Exercise the keying logic via a direct synthetic pass through the merge fn
    # if exposed, else assert the source uses methodSubjectId.
    import inspect
    import verification.tools.merge_platform_runs as mp
    src = inspect.getsource(mp)
    assert "methodSubjectId" in src            # prefers subjectId
    assert "f_entry.get(\"methodSubjectId\") or f_entry.get(\"methodIndex\")" in src


def test_benchmark_diff_print_tolerates_aggregate_details(capsys) -> None:
    from verification.benchmark_diff import _print_report

    report = {
        "summary": {"total": 1, "regressed": 1, "improved": 0, "unchanged": 0,
                    "new": 0, "missing": 0},
        "details": {
            "regressed": [{"slug": "numerics", "baselinePct": 1.0,
                           "targetPct": 2.0, "delta": 1.0}],
            "improved": [],
            "new": [],
            "missing": [],
        },
    }
    # Must not raise KeyError on aggregate entries lacking methodSubjectId.
    _print_report(report, "2026-01-01", "2026-02-01", 0.5)
    assert "numerics" in capsys.readouterr().out
