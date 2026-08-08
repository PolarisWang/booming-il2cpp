"""Track 0 tests — profile-linkage foundation.

Verifies:
  1. All perf-tree producers/consumers resolve `_RESULTS_BASE` to the SAME
     foundation-rooted `results_base()` (single source of truth — no more
     divergent per-file `Path(__file__)…/results/foundation-dll` roots).
  2. `PerfBaseline.establish` → `regression_report.generate_report` linkage:
     a baseline established under a results base is picked up by the report
     chain (perfComparison is not "no_baseline"), i.e. the previously-orphaned
     baseline/regression data flow now actually connects.
  3. The profile stage refuses (skipped, not stub-zero) to run under a
     non-`profile` native config, so no fake baseline pollutes the chain.
"""
from __future__ import annotations

import sys
from pathlib import Path

import pytest

_HERE = Path(__file__).resolve().parent
for _d in (_HERE, _HERE.parent):
    if str(_d) not in sys.path:
        sys.path.insert(0, str(_d))

from _path import results_base  # noqa: E402


def test_results_base_is_shared_single_source() -> None:
    """T0a: the perf tree root is unified and foundation-rooted."""
    import verification.analysis.perf_baseline as pb
    import verification.analysis.regression_report as rr
    import verification.stages.benchmark_chunk as bc
    import verification.stages.benchmark_report as br
    import verification.stages.managed_benchmark as mb
    import verification.stages.profile as prof

    expected = results_base()
    got = {
        "perf_baseline": pb._RESULTS_BASE,
        "regression_report": rr._RESULTS_BASE,
        "benchmark_chunk": bc._RESULTS_BASE,
        "benchmark_report": br._RESULTS_BASE,
        "managed_benchmark": mb._RESULTS_BASE,
        "profile": prof._RESULTS_BASE,
    }
    for name, actual in got.items():
        assert actual == expected, f"{name} diverged: {actual} != {expected}"
    # Must be under the foundation root (not package-local verification/results)
    assert "verification" not in expected.parts, f"root still package-local: {expected}"


def test_baseline_chain_surfaces_perf_comparison(tmp_path: Path) -> None:
    """T0b: establishing a baseline makes regression_report no longer 'no_baseline'."""
    from verification.analysis.perf_baseline import PerfBaseline
    import verification.analysis.regression_report as rr

    assembly = "System.Private.CoreLib"
    slug = "numerics"
    profile_data = [
        {"methodIndex": 0, "gcPauseNs": 1_000_000, "maxGcPauseNs": 3_000_000,
         "gcPauseCount": 5, "nurseryAllocBytes": 2048, "oldGenAllocBytes": 0,
         "largeObjBytes": 0, "allocCount": 42, "heapBefore": 1, "heapAfter": 1,
         "fastPathCount": 90, "slowPathCount": 10},
        {"methodIndex": 1, "gcPauseNs": 0, "maxGcPauseNs": 0, "gcPauseCount": 0,
         "nurseryAllocBytes": 512, "oldGenAllocBytes": 0, "largeObjBytes": 0,
         "allocCount": 8, "heapBefore": 1, "heapAfter": 1,
         "fastPathCount": 100, "slowPathCount": 0},
    ]

    # Establish under an explicit temp base → writes baseline.json.
    bl = PerfBaseline(results_base=tmp_path)
    baseline_path = bl.establish(
        assembly, slug, profile_data, metadata={"nativeConfig": "profile"})
    assert baseline_path.exists()

    # Build a chunk dir with profile.json so generate_report has profile_data.
    chunk_dir = tmp_path / "chunks" / slug
    results_dir = chunk_dir / "results"
    results_dir.mkdir(parents=True)
    (results_dir / "profile.json").write_text(
        '{"nativeConfig": "profile", "profileData": ['
        '{"methodIndex":0,"gcPauseNs":1000000,"maxGcPauseNs":4000000,'
        '"gcPauseCount":6,"nurseryAllocBytes":4096,"allocCount":50,'
        '"fastPathCount":80,"slowPathCount":20}],'
        '"summary": {"methodCount":1,"totalGcPauseNs":1000000,'
        '"maxGcPauseNs":4000000,"fastPathRate":0.8}}',
        encoding="utf-8")

    # Point generate_report's baseline lookup + chunk dir at the temp tree.
    # PerfBaseline() reads perf_baseline._RESULTS_BASE at __init__ time.
    import verification.analysis.perf_baseline as pb
    pb._RESULTS_BASE = tmp_path
    report = rr.generate_report(assembly, slug, foundation_dir=tmp_path)
    # The no-baseline placeholder is exactly {"status": "no_baseline"}; a real
    # comparison returns regressions/improvements/unchanged/summary keys.
    perf = report["perfComparison"]
    assert perf != {"status": "no_baseline"}, f"baseline still not wired: {perf}"
    assert "summary" in perf or "regressions" in perf


def test_profile_stage_refuses_non_profile_config(
        temp_chunk_ctx, monkeypatch) -> None:
    """T0b: profile stage skips (not stub-zero) under a non-profile native config."""
    from verification.stages.profile import run_profile

    temp_chunk_ctx.native_config = "check"
    # entry.exe doesn't exist → would skip on missing exe before config gate;
    # create a fake to exercise the config gate specifically.
    native = temp_chunk_ctx.chunk_dir / "native"
    (native / "entry.exe").write_bytes(b"fake")
    result = run_profile(temp_chunk_ctx, {})
    assert result.status == "skipped"
    assert "native-config profile" in result.summary
    # And no stub profile.json/baseline was produced.
    assert not (temp_chunk_ctx.results_dir / "profile.json").exists()
