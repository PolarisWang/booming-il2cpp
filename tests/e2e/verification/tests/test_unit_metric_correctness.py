"""Track 5 tests — unit & metric correctness.

Verifies:
  1. fast_path_rate is 0.0 (not 1.0) when there is no path data, so an empty
     sample never fabricates a "perfect fast path".
  2. _geometric_mean excludes non-positive values from BOTH product & count.
  3. _get_elapsed normalizes total-for-batch -> per-iteration exactly once.
  4. benchmark_scanner tolerates perMethodStats:null (does not crash sorted()).
  5. gc_check.print_report reports bySeverity counts (not method-with-GC counts).
  6. code_size_tracker backfills the last function's textSize from the .text
     section size instead of leaving it 0.
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


def test_fast_path_rate_empty_is_zero() -> None:
    from verification.stages.profile import _compute_summary
    from verification.analysis.perf_baseline import PerfBaseline
    from verification.stages.benchmark_report import _fast_path_rate

    # Empty method list -> no summary at all (fast_path_rate absent).
    assert _compute_summary([]) == {}
    # Methods present but zero path counts -> 0.0, not 1.0.
    s = _compute_summary([{"fastPathCount": 0, "slowPathCount": 0}])
    assert s["fastPathRate"] == 0.0
    assert s["pathObservations"] == 0
    # _fast_path_rate on empty profile -> 0.0.
    assert _fast_path_rate({}) == 0.0


def test_geometric_mean_excludes_non_positive_fully(tmp_path: Path) -> None:
    from verification.reporting.dashboard import _geometric_mean

    # [1,2,4] -> exp(ln8/3) = 8^(1/3) = 2.0. Non-positive values (0,-1) excluded.
    assert _geometric_mean([1, 2, 4, 0, -1]) == pytest.approx(2.0)
    assert _geometric_mean([1, 2, 4]) == pytest.approx(2.0)
    assert _geometric_mean([]) == 0.0
    assert _geometric_mean([0, -1, -2]) == 0.0


def test_get_elapsed_normalizes_once() -> None:
    from verification.stages.benchmark_report import _get_elapsed

    # total-for-batch 100ms over 10 iterations -> 10ms per iteration.
    assert _get_elapsed({"metrics": {"elapsedMilliseconds": 100.0}, "iterations": 10}) == 10.0
    # Missing iterations -> treated as 1.
    assert _get_elapsed({"metrics": {"elapsedMilliseconds": 100.0}}) == 100.0
    assert _get_elapsed({"metrics": {}}) is None
    assert _get_elapsed(None) is None


def test_benchmark_scanner_tolerates_null_permethodstats(tmp_path: Path) -> None:
    from unittest.mock import patch
    from verification.analysis import benchmark_scanner as bs

    # scan_all iterates _find_all_chunks() + _load_benchmark(); patch both so we
    # can feed a controlled benchmark.json with perMethodStats:null.
    with patch.object(bs, "_find_all_chunks", return_value=[("Test.Assembly", "c1")]), \
         patch.object(bs, "_load_benchmark", return_value={
             "summary": {"elapsedMilliseconds": 10.0},
             "perMethodStats": None,          # explicit null -> must not crash
             "methodCount": 0,
         }), \
         patch.object(bs, "_load_fact", return_value={"passed": 0, "total": 0}):
        out = bs.scan_all(output_path=None)
        # The scan must complete without TypeError from sorted(None).
        assert isinstance(out, list)


def test_gc_check_print_report_uses_byseverity(tmp_path: Path, capsys) -> None:
    from verification.analysis.gc_check import print_report

    result = {
        "status": "unhealthy",
        "issueCount": 5,
        "bySeverity": {"error": 2, "warning": 3, "info": 0},
        "summary": {},
        "issues": [
            {"check": "GC-01", "severity": "error", "methodIndex": 0, "actual": 1, "message": "payload"},
            {"check": "GC-02", "severity": "error", "methodIndex": 1, "actual": 2, "message": "payload"},
            {"check": "GC-03", "severity": "warning", "methodIndex": 2, "actual": 3, "message": "payload"},
            {"check": "GC-03", "severity": "warning", "methodIndex": 3, "actual": 4, "message": "payload"},
            {"check": "GC-03", "severity": "warning", "methodIndex": 4, "actual": 5, "message": "payload"},
        ],
    }
    print_report(result)
    out = capsys.readouterr().out
    assert "error=2, warning=3, info=0" in out


def test_code_size_tracker_backfills_last_function(tmp_path: Path) -> None:
    from verification.analysis.code_size_tracker import CodeSizeTracker

    tracker = CodeSizeTracker()
    # Feed a controlled functionSizes + .text section size; verify the last
    # function (which _parse_map_file stubs to 0) is backfilled from .text end.
    fake_exe = tmp_path / "entry.exe"
    fake_exe.write_bytes(b"\x00" * 64)
    (fake_exe.with_suffix(".map")).write_text(
        "  Publics by Value  Rva+Base       Lib:Object\n\n"
        "  0001:00000010       ?Small@@YAHXZ     00400010     f    a.obj\n"
        "  0001:00000030       ?Big@@YAHXZ       00400030     f    b.obj\n",
        encoding="utf-8",
    )
    tracker._parse_map_file = lambda path: [
        {"symbol": "?Small@@YAHXZ", "offset": 0x10, "textSize": 0x20},
        {"symbol": "?Big@@YAHXZ", "offset": 0x30, "textSize": 0},  # last stub
    ]
    tracker._dumpbin_sections = lambda exe: {"text": 0x1000, "data": 0, "pdata": 0, "reloc": 0, "total": 0x1000}

    result = tracker.collect(fake_exe)
    funcs = result.get("functionSizes") or []
    assert len(funcs) == 2
    assert funcs[0]["textSize"] == 0x20
    # Last function backfilled from .text section end (0x1000) minus its offset.
    assert funcs[-1]["textSize"] == (0x1000 - 0x30)
