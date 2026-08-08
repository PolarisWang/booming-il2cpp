"""Track 4 tests — fragment reporting correctness.

Verifies:
  1. reporting._enrich_with_chunk_facts now extracts per-method failures from
     fact-results.json (aot/jit) instead of fact.json's never-present "results"
     key, so fact-failures.jsonl is actually published.
  2. aggregate._try_load_json degrades gracefully on corrupt per-chunk files
     instead of crashing the stage.
  3. benchmark_report._classify_bottleneck reads aotAllocVsNet8Pct (pct) with
     an alloc_hot threshold, and only promotes an empty classification.
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


def _make_chunk_tree(tmp: Path, slug: str = "numerics") -> Path:
    results = tmp / "chunks" / slug / "results"
    results.mkdir(parents=True)
    return results


def test_reporting_extracts_failures_from_fact_results(tmp_path: Path) -> None:
    """A failed aot fact method must surface as a reporting failure."""
    from verification.stages import reporting

    results = _make_chunk_tree(tmp_path)
    # fact.json carries only aggregates (no "results" key).
    (results / "fact.json").write_text(json.dumps({
        "passed": 1, "total": 2, "factMethodCount": 2,
    }), encoding="utf-8")
    # Per-method outcomes live in fact-results.json.
    (results / "fact-results.json").write_text(json.dumps({
        "aot": [
            {"methodIndex": 0, "methodSubjectId": "M::A()", "passed": True},
            {"methodIndex": 1, "methodSubjectId": "M::B()", "passed": False,
             "value": 42, "message": "checksum mismatch"},
        ],
        "jit": [],
    }), encoding="utf-8")

    asm_data: dict = {}
    reporting._enrich_with_chunk_facts(asm_data, tmp_path / "chunks")
    failures = asm_data["chunks"][0]["fact"]["failures"]
    assert len(failures) == 1
    assert failures[0]["methodIndex"] == 1
    assert failures[0]["methodSubjectId"] == "M::B()"
    assert failures[0]["tech"] == "aot"


def test_reporting_no_fact_results_is_no_failures(tmp_path: Path) -> None:
    """Without fact-results.json there are no failures (not a crash)."""
    from verification.stages import reporting

    results = _make_chunk_tree(tmp_path)
    (results / "fact.json").write_text(json.dumps({"passed": 0, "total": 0}), encoding="utf-8")
    asm_data: dict = {}
    reporting._enrich_with_chunk_facts(asm_data, tmp_path / "chunks")
    assert asm_data["chunks"][0]["fact"]["failures"] == []


def test_aggregate_try_load_json_handles_corrupt(tmp_path: Path, capsys) -> None:
    from verification.stages.aggregate import _try_load_json

    bad = tmp_path / "fact.json"
    bad.write_text("not json {{", encoding="utf-8")
    assert _try_load_json(bad) is None
    assert "WARNING" in capsys.readouterr().out
    # Missing file -> None without error.
    assert _try_load_json(tmp_path / "missing.json") is None


def test_classify_bottleneck_alloc_hot_uses_pct_threshold() -> None:
    from verification.stages.benchmark_report import _classify_bottleneck as c

    # ratio 1.6 (>1.5, <2.0) + aotAllocVsNet8Pct 250 (>200) -> alloc_hot
    assert c(chaos_aot_ms=16.0, net8_ms=10.0, high_variance=False,
             gc_comparison={"aotAllocVsNet8Pct": 250.0}) == "alloc_hot"
    # pct 180 (<200) -> not alloc_hot
    assert c(chaos_aot_ms=16.0, net8_ms=10.0, high_variance=False,
             gc_comparison={"aotAllocVsNet8Pct": 180.0}) == ""
    # No gc_comparison -> no alloc_hot
    assert c(chaos_aot_ms=16.0, net8_ms=10.0, high_variance=False, gc_comparison=None) == ""
    # high_variance dominates
    assert c(chaos_aot_ms=16.0, net8_ms=10.0, high_variance=True,
             gc_comparison={"aotAllocVsNet8Pct": 250.0}) == "unstable"
