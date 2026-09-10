"""Tests for verification.tools.baseline_store and verification.tools.nightly_triage.

Covers:
  - baseline_store: record/load round-trip, provenance capture, comparability gate
  - nightly_triage: noise-band significance, per-domain analyzers, graded report,
    design-review trigger at the >=30% ratio
"""

from __future__ import annotations

import json
import sys
from pathlib import Path

import pytest

_HERE = Path(__file__).resolve().parent          # verification/tests/
_VERIFY = _HERE.parent                            # verification/
_E2E = _VERIFY.parent
for _p in (_E2E, _VERIFY):
    if str(_p) not in sys.path:
        sys.path.insert(0, str(_p))

from verification.tools import baseline_store as bs  # noqa: E402
from verification.tools import nightly_triage as nt  # noqa: E402


# ── helpers ───────────────────────────────────────────────────────────────────

def _make_results_base(tmp_path: Path, spec: dict) -> Path:
    """Build a synthetic <asm>/chunks/<slug>/results tree.

    spec: { "<asm>/<slug>": {"fact": {...}, "benchmark": {...},
                             "hotupdate": {...}, "provenance": {...}} }
    """
    base = tmp_path / "foundation-dll"
    for key, arts in spec.items():
        asm, slug = key.split("/")
        rdir = base / asm / "chunks" / slug / "results"
        rdir.mkdir(parents=True, exist_ok=True)
        if "fact" in arts:
            (rdir / "fact.json").write_text(json.dumps(arts["fact"]), encoding="utf-8")
        if "benchmark" in arts:
            (rdir / "benchmark.json").write_text(json.dumps(arts["benchmark"]),
                                                 encoding="utf-8")
        if "hotupdate" in arts:
            (rdir / "hotupdate.json").write_text(json.dumps(arts["hotupdate"]),
                                                 encoding="utf-8")
        if "fact-results" in arts:
            (rdir / "fact-results.json").write_text(json.dumps(arts["fact-results"]),
                                                    encoding="utf-8")
        if "provenance" in arts:
            (rdir / "provenance.json").write_text(json.dumps(arts["provenance"]),
                                                  encoding="utf-8")
    return base


_PROV = {"platform": "windows-x64", "deviceId": "PC-1", "nativeConfig": "check",
         "gitCommit": "aaaa1111", "gitBranch": "main"}


def _mk_baseline(run_id: str, ts: str, chunks: dict, prov: dict | None = None) -> bs.Baseline:
    return bs.Baseline(
        run_id=run_id,
        timestamp=ts,
        provenance=prov or dict(_PROV),
        chunks={k: bs.ChunkBaseline.from_dict(k, v) for k, v in chunks.items()},
    )


def _write_baseline(report_dir: Path, b: bs.Baseline) -> None:
    d = bs.baseline_dir(report_dir)
    d.mkdir(parents=True, exist_ok=True)
    bs._run_path(report_dir, b.run_id).write_text(
        json.dumps(b.to_dict(), ensure_ascii=False, indent=2), encoding="utf-8")
    bs._update_index(report_dir, b)


# ── baseline_store ────────────────────────────────────────────────────────────

class TestBaselineStore:
    def test_record_and_load_roundtrip(self, tmp_path):
        report_dir = tmp_path / "nightly-build-report"
        base = _make_results_base(tmp_path, {
            "System.Linq/global-ns": {
                "fact": {"total": 215, "passed": 215, "realVerified": 180,
                         "unassertable": 0, "smokeUnknown": 35, "failed": 0},
                "benchmark": {"methodCount": 100, "stubCount": 0, "nonStubCount": 100,
                              "exitCode": 0},
                "provenance": _PROV,
            }
        })
        p = bs.record_from_run("run-1", report_dir, base, "check")
        assert p is not None and p.exists()

        loaded = bs.load_recent(report_dir, n=5)
        assert len(loaded) == 1
        b = loaded[0]
        assert b.run_id == "run-1"
        # provenance came from the chunk's provenance.json
        assert b.provenance["deviceId"] == "PC-1"
        assert b.provenance["gitCommit"] == "aaaa1111"
        cb = b.chunks["System.Linq__global-ns"]
        assert cb.fact["realVerified"] == 180

    def test_record_empty_tree_returns_none(self, tmp_path):
        report_dir = tmp_path / "r"
        empty = tmp_path / "empty"
        empty.mkdir()
        assert bs.record_from_run("run-x", report_dir, empty, "check") is None

    def test_is_comparable_matches(self):
        a = _mk_baseline("a", "t1", {})
        b = _mk_baseline("b", "t2", {})
        ok, reason = bs.is_comparable(a, b)
        assert ok and reason == "ok"

    def test_is_comparable_device_mismatch(self):
        a = _mk_baseline("a", "t1", {}, {**_PROV, "deviceId": "PC-1"})
        b = _mk_baseline("b", "t2", {}, {**_PROV, "deviceId": "PC-2"})
        ok, reason = bs.is_comparable(a, b)
        assert not ok and "deviceId-mismatch" in reason

    def test_is_comparable_config_mismatch(self):
        a = _mk_baseline("a", "t1", {}, {**_PROV, "nativeConfig": "check"})
        b = _mk_baseline("b", "t2", {}, {**_PROV, "nativeConfig": "ship"})
        ok, reason = bs.is_comparable(a, b)
        assert not ok and "nativeConfig-mismatch" in reason

    def test_is_comparable_git_commit_ignored(self):
        """Different commits are the whole point — must remain comparable."""
        a = _mk_baseline("a", "t1", {}, {**_PROV, "gitCommit": "11111111"})
        b = _mk_baseline("b", "t2", {}, {**_PROV, "gitCommit": "22222222"})
        ok, _ = bs.is_comparable(a, b)
        assert ok

    def test_is_comparable_empty_provenance_tolerated(self):
        """A missing provenance field is 'unknown', not a mismatch."""
        a = _mk_baseline("a", "t1", {}, {"platform": "", "deviceId": "", "nativeConfig": ""})
        b = _mk_baseline("b", "t2", {}, _PROV)
        ok, _ = bs.is_comparable(a, b)
        assert ok

    def test_mark_reference(self, tmp_path):
        report_dir = tmp_path / "r"
        _write_baseline(report_dir, _mk_baseline("run-1", "t1", {}))
        assert bs.mark_reference(report_dir, "run-1") is True
        ref = bs.load_reference(report_dir)
        assert ref is not None and ref.run_id == "run-1"
        assert bs.load_recent(report_dir, 1)[0].reference is True

    def test_mark_reference_missing_returns_false(self, tmp_path):
        assert bs.mark_reference(tmp_path / "r", "nope") is False

    def test_crosstech_extraction(self, tmp_path):
        report_dir = tmp_path / "r"
        base = _make_results_base(tmp_path, {
            "A/c": {
                "fact-results": {
                    "aot": [{"si": 0, "passed": True}, {"si": 1, "passed": False}],
                    "jit": [{"si": 0, "passed": True}, {"si": 1, "passed": True}],
                },
                "provenance": _PROV,
            }
        })
        bs.record_from_run("r1", report_dir, base, "check")
        cb = bs.load_recent(report_dir, 1)[0].chunks["A__c"]
        assert cb.crosstech["aotPassed"] == 1
        assert cb.crosstech["jitPassed"] == 2
        assert cb.crosstech["diffCount"] == 1


# ── nightly_triage: statistics ────────────────────────────────────────────────

class TestNoiseBand:
    def test_band_with_stable_history(self):
        med, lo, hi = nt._noise_band([10.0, 10.0, 10.0, 10.0])
        # MAD == 0 → degenerate band at the median
        assert med == 10.0 and lo == 10.0 and hi == 10.0

    def test_band_widens_with_variance(self):
        med, lo, hi = nt._noise_band([10.0, 12.0, 8.0, 10.0])
        assert lo < med < hi
        assert med == 10.0

    def test_beyond_band(self):
        band = (10.0, 8.0, 12.0)
        assert nt._beyond_band(15.0, band) is True
        assert nt._beyond_band(11.0, band) is False
        assert nt._beyond_band(5.0, band) is True

    def test_single_sample_has_no_spread(self):
        med, lo, hi = nt._noise_band([42.0])
        assert med == lo == hi == 42.0

    def test_pct_delta_zero_base(self):
        assert nt._pct_delta(5.0, 0.0) == 0.0

    def test_pct_delta_normal(self):
        assert nt._pct_delta(130.0, 100.0) == pytest.approx(0.30)


# ── nightly_triage: per-domain analyzers ─────────────────────────────────────

class TestDomainAnalyzers:
    def test_fact_real_drop_confirmed(self):
        cur = bs.ChunkBaseline(key="A/c", fact={
            "total": 100, "realVerified": 10, "smokeUnknown": 90, "passed": 100})
        hist = [_mk_baseline(f"h{i}", f"t{i}", {
            "A/c": {"fact": {"total": 100, "realVerified": 90,
                             "smokeUnknown": 10, "passed": 100}}})
            for i in range(5)]
        findings = nt._analyze_fact(cur, hist)
        assert any(f.metric == "realVerified/total" and f.confirmed for f in findings)

    def test_fact_no_change_no_finding(self):
        cur = bs.ChunkBaseline(key="A/c", fact={
            "total": 100, "realVerified": 90, "smokeUnknown": 10, "passed": 100})
        hist = [_mk_baseline(f"h{i}", f"t{i}", {
            "A/c": {"fact": {"total": 100, "realVerified": 90,
                             "smokeUnknown": 10, "passed": 100}}})
            for i in range(5)]
        assert nt._analyze_fact(cur, hist) == []

    def test_fact_smoke_rise_flagged(self):
        cur = bs.ChunkBaseline(key="A/c", fact={
            "total": 100, "realVerified": 50, "smokeUnknown": 50, "passed": 100})
        hist = [_mk_baseline(f"h{i}", f"t{i}", {
            "A/c": {"fact": {"total": 100, "realVerified": 90,
                             "smokeUnknown": 10, "passed": 100}}})
            for i in range(5)]
        findings = nt._analyze_fact(cur, hist)
        assert any(f.metric == "smokeUnknown" for f in findings)

    def test_benchmark_below_noise_floor_ignored(self):
        cur = bs.ChunkBaseline(key="A/c", benchmark={"meanDurationMs": 0.001})
        hist = [_mk_baseline(f"h{i}", f"t{i}", {
            "A/c": {"benchmark": {"meanDurationMs": 0.0005}}}) for i in range(5)]
        assert nt._analyze_benchmark(cur, hist) == []

    def test_benchmark_slowdown_confirmed(self):
        cur = bs.ChunkBaseline(key="A/c", benchmark={"meanDurationMs": 200.0})
        hist = [_mk_baseline(f"h{i}", f"t{i}", {
            "A/c": {"benchmark": {"meanDurationMs": 100.0 + i}}}) for i in range(5)]
        findings = nt._analyze_benchmark(cur, hist)
        assert any(f.metric == "meanDurationMs" and f.confirmed for f in findings)

    def test_hotupdate_patch_failed_is_confirmed(self):
        cur = bs.ChunkBaseline(key="A/c", hotupdate={"patchFailed": True})
        findings = nt._analyze_hotupdate(cur, [])
        assert any(f.metric == "patchFailed" and f.confirmed for f in findings)

    def test_hotupdate_crash_is_confirmed(self):
        cur = bs.ChunkBaseline(key="A/c", hotupdate={"crash": True})
        findings = nt._analyze_hotupdate(cur, [])
        assert any(f.metric == "crash" and f.confirmed for f in findings)

    def test_crosstech_divergence_flagged(self):
        cur = bs.ChunkBaseline(key="A/c", crosstech={
            "aotPassed": 100, "jitPassed": 50, "diffCount": 90})
        findings = nt._analyze_crosstech(cur, [])
        assert any(f.domain == "crosstech" and f.confirmed for f in findings)

    def test_crosstech_within_tolerance_ok(self):
        cur = bs.ChunkBaseline(key="A/c", crosstech={
            "aotPassed": 100, "jitPassed": 95, "diffCount": 5})
        assert nt._analyze_crosstech(cur, []) == []


# ── nightly_triage: end-to-end analyze + render ──────────────────────────────

class TestAnalyzeEndToEnd:
    def _seed(self, report_dir: Path, cur_real: int):
        chunks_hist = {"A/c": {"fact": {"total": 100, "realVerified": 90,
                                        "smokeUnknown": 10, "passed": 100}}}
        for i in range(4):
            _write_baseline(report_dir, _mk_baseline(f"h{i}", f"2026-09-0{i+1}T00:00:00Z",
                                                     chunks_hist))
        chunks_cur = {"A/c": {"fact": {"total": 100, "realVerified": cur_real,
                                       "smokeUnknown": 100 - cur_real, "passed": 100}}}
        _write_baseline(report_dir, _mk_baseline("cur", "2026-09-05T00:00:00Z",
                                                 chunks_cur))

    def test_regression_detected(self, tmp_path):
        rd = tmp_path / "r"
        self._seed(rd, cur_real=5)
        result = nt.analyze(rd)
        assert result is not None
        assert result.comparable
        assert any(f.metric == "realVerified/total" for f in result.confirmed())

    def test_no_regression_clean(self, tmp_path):
        rd = tmp_path / "r"
        self._seed(rd, cur_real=90)
        result = nt.analyze(rd)
        assert result is not None
        assert result.confirmed() == []

    def test_insufficient_history_returns_none(self, tmp_path):
        rd = tmp_path / "r"
        _write_baseline(rd, _mk_baseline("only", "t1", {}))
        assert nt.analyze(rd) is None

    def test_incomparable_skips_analysis(self, tmp_path):
        rd = tmp_path / "r"
        _write_baseline(rd, _mk_baseline("h0", "2026-09-01T00:00:00Z",
                                         {"A/c": {"fact": {"total": 10, "realVerified": 1}}},
                                         {**_PROV, "deviceId": "PC-1"}))
        _write_baseline(rd, _mk_baseline("cur", "2026-09-02T00:00:00Z",
                                         {"A/c": {"fact": {"total": 10, "realVerified": 0}}},
                                         {**_PROV, "deviceId": "PC-2"}))
        result = nt.analyze(rd)
        assert result is not None and not result.comparable
        assert result.confirmed() == []


class TestRender:
    def test_render_insufficient(self):
        out = nt.render_markdown(None)
        assert "基线不足" in out

    def test_render_grades_and_actions(self, tmp_path):
        rd = tmp_path / "r"
        hist = {"A/c": {"fact": {"total": 100, "realVerified": 90,
                                 "smokeUnknown": 10, "passed": 100}}}
        for i in range(4):
            _write_baseline(rd, _mk_baseline(f"h{i}", f"2026-09-0{i+1}T00:00:00Z", hist))
        _write_baseline(rd, _mk_baseline("cur", "2026-09-05T00:00:00Z",
                                         {"A/c": {"fact": {"total": 100, "realVerified": 5,
                                                           "smokeUnknown": 95,
                                                           "passed": 100}}}))
        result = nt.analyze(rd)
        md = nt.render_markdown(result)
        assert "🔴 Confirmed Regressions" in md
        assert "📋 Suggested Actions" in md
        assert "📊 域裁决" in md
        assert "fact" in md

    def test_render_incomparable(self, tmp_path):
        rd = tmp_path / "r"
        _write_baseline(rd, _mk_baseline("h0", "2026-09-01T00:00:00Z",
                                         {"A/c": {"fact": {"total": 10, "realVerified": 1}}},
                                         {**_PROV, "deviceId": "PC-1"}))
        _write_baseline(rd, _mk_baseline("cur", "2026-09-02T00:00:00Z",
                                         {"A/c": {"fact": {"total": 10, "realVerified": 0}}},
                                         {**_PROV, "deviceId": "PC-2"}))
        md = nt.render_markdown(nt.analyze(rd))
        assert "⏭️ Skipped" in md


class TestDesignReviewTrigger:
    def test_ratio_above_threshold_flags_design_review(self, tmp_path):
        rd = tmp_path / "r"
        # 4 chunks; 2 regress -> ratio 0.50 >= 0.30
        hist_chunks = {f"A/c{i}": {"fact": {"total": 100, "realVerified": 90,
                                            "smokeUnknown": 10, "passed": 100}}
                       for i in range(4)}
        for i in range(4):
            _write_baseline(rd, _mk_baseline(f"h{i}", f"2026-09-0{i+1}T00:00:00Z",
                                             hist_chunks))
        cur_chunks = {}
        for i in range(4):
            real = 5 if i < 2 else 90
            cur_chunks[f"A/c{i}"] = {"fact": {"total": 100, "realVerified": real,
                                              "smokeUnknown": 100 - real, "passed": 100}}
        _write_baseline(rd, _mk_baseline("cur", "2026-09-05T00:00:00Z", cur_chunks))

        result = nt.analyze(rd)
        fact_verdict = next(v for v in result.verdicts if v.domain == "fact")
        assert fact_verdict.needs_design_review is True
        md = nt.render_markdown(result)
        assert "方案重整" in md

    def test_ratio_below_threshold_point_fix(self, tmp_path):
        rd = tmp_path / "r"
        hist_chunks = {f"A/c{i}": {"fact": {"total": 100, "realVerified": 90,
                                            "smokeUnknown": 10, "passed": 100}}
                       for i in range(10)}
        for i in range(4):
            _write_baseline(rd, _mk_baseline(f"h{i}", f"2026-09-0{i+1}T00:00:00Z",
                                             hist_chunks))
        cur_chunks = {}
        for i in range(10):
            real = 5 if i < 1 else 90  # 1/10 = 10% < 30%
            cur_chunks[f"A/c{i}"] = {"fact": {"total": 100, "realVerified": real,
                                              "smokeUnknown": 100 - real, "passed": 100}}
        _write_baseline(rd, _mk_baseline("cur", "2026-09-05T00:00:00Z", cur_chunks))

        result = nt.analyze(rd)
        fact_verdict = next(v for v in result.verdicts if v.domain == "fact")
        assert fact_verdict.needs_design_review is False
