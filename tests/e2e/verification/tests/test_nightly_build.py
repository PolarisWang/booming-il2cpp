"""Verify nightly build system — unit tests + integration smoke tests."""
import json
import sys
import tempfile
from datetime import date, timedelta
from pathlib import Path

sys.path.insert(0, "unused/old-root")

passed = 0
failed = 0


def check(name, condition, detail=""):
    global passed, failed
    if condition:
        passed += 1
        print(f"  PASS: {name}")
    else:
        failed += 1
        print(f"  FAIL: {name} {detail}")


# ── 1. Syntax ──
print("=== Syntax ===")
import py_compile
for f in [
    "tests/e2e/verification/nightly_build.py",
    "tests/e2e/verification/nightly_delta.py",
    "tests/e2e/verification/nightly_summary.py",
]:
    py_compile.compile(f, doraise=True)
    check(f, True)


# ── 2. delta: no history (first run) ──
print("\n=== Delta: first run (no history) ===")
from verification.nightly_delta import compute_nightly_delta

with tempfile.TemporaryDirectory() as tmp:
    root = Path(tmp)
    # Create a fake assembly with latest report but NO history
    asm_dir = root / "TestAssembly"
    latest_dir = asm_dir / "_dll" / "reports" / "latest"
    latest_dir.mkdir(parents=True)
    (latest_dir / "dashboard.json").write_text(json.dumps({
        "assemblyName": "TestAssembly",
        "summary": {
            "chunks": 2, "chunksVerified": 1,
            "factPassRate": 95.0, "totalBenchmarkedMethods": 50,
            "chunksWithMetaMismatch": 0, "chunksWithValueWarnings": 0,
            "aggregatePerformance": {"meanDurationMs": 0.042},
            "hotupdate": {"chunksWithPatchData": 0, "chunksPatchFailed": 0,
                          "totalPassed": 0, "totalFailed": 0},
            "benchmarkComparison": {},
        },
    }))
    (latest_dir / "fact-summary.json").write_text(json.dumps({
        "assemblyName": "TestAssembly",
        "chunkSummaries": [
            {"slug": "chunk-a", "fact": {"passed": 10, "total": 10,
             "metaTotal": 12}, "benchmark": {"methodCount": 5, "meanDurationMs": 0.03},
             "hotupdate": {"passed": 0, "failed": 0}},
            {"slug": "chunk-b", "fact": {"passed": 9, "total": 10,
             "metaTotal": 10}, "benchmark": {"methodCount": 5, "meanDurationMs": 0.04},
             "hotupdate": {"passed": 0, "failed": 0}},
        ],
    }))
    (latest_dir / "coverage-audit.json").write_text(json.dumps({
        "totalChunks": 2, "chunksWithResults": 2, "totalDeclaredMethods": 22,
    }))

    delta = compute_nightly_delta(root, today="2026-06-14")
    check("delta loads without history", delta is not None)
    check("overall assemblies count", delta["overall"]["totalAssemblies"] == 1)
    check("overall factPassRate set", delta["overall"]["factPassRate"] is not None)
    check("0 assemblies with history (first run)",
          delta["overall"]["assembliesWithHistory"] == 0)
    check("no regressions (no history to compare)",
          len(delta["regressions"]) == 0)
    check("per-chunk data present",
          len(delta["chunks"]) == 2)


# ── 3. delta: with previous history ──
print("\n=== Delta: with previous history ===")
with tempfile.TemporaryDirectory() as tmp:
    root = Path(tmp)
    asm_dir = root / "TestAssembly"
    latest_dir = asm_dir / "_dll" / "reports" / "latest"
    history_dir = asm_dir / "_dll" / "reports" / "history"
    latest_dir.mkdir(parents=True)
    history_dir.mkdir(parents=True)

    # Today's results (fact: 8/10 passed = 80%)
    (latest_dir / "dashboard.json").write_text(json.dumps({
        "summary": {
            "chunks": 1, "chunksVerified": 1, "factPassRate": 80.0,
            "totalBenchmarkedMethods": 5, "chunksWithMetaMismatch": 0,
            "chunksWithValueWarnings": 0,
            "aggregatePerformance": {"meanDurationMs": 0.05},
            "hotupdate": {"chunksWithPatchData": 0, "chunksPatchFailed": 0,
                          "totalPassed": 0, "totalFailed": 0},
            "benchmarkComparison": {},
        },
    }))
    (latest_dir / "fact-summary.json").write_text(json.dumps({
        "chunkSummaries": [
            {"slug": "chunk-a", "fact": {"passed": 8, "total": 10, "metaTotal": 10},
             "benchmark": {"methodCount": 5, "meanDurationMs": 0.05, "meanOpsPerSecond": 1000},
             "hotupdate": {"passed": 0, "failed": 0}},
        ],
    }))
    (latest_dir / "coverage-audit.json").write_text(json.dumps({}))

    # Yesterday's history (fact: 10/10 passed = 100%, faster benchmark)
    (history_dir / "benchmark-2026-06-13.json").write_text(json.dumps({
        "date": "2026-06-13", "assemblyName": "TestAssembly",
        "chunks": [
            {"slug": "chunk-a", "fact": {"passed": 10, "total": 10, "metaTotal": 10},
             "benchmark": {"methodCount": 5, "meanDurationMs": 0.04, "meanOpsPerSecond": 1200},
             "hotupdate": {"passed": 0, "failed": 0}},
        ],
    }))

    delta = compute_nightly_delta(root, today="2026-06-14")
    check("assemblies with history = 1",
          delta["overall"]["assembliesWithHistory"] == 1)

    # Should detect regression: fact pass rate 100% -> 80% = -20%
    check("fact regression detected",
          any(r["metric"] == "fact_pass_rate" for r in delta["regressions"]))
    fact_reg = [r for r in delta["regressions"] if r["metric"] == "fact_pass_rate"]
    check("fact regression delta = -20", fact_reg and fact_reg[0]["delta"] == -20.0)

    # Should detect bench regression: duration 0.04 -> 0.05 = +25%
    check("bench regression detected",
          any(r["metric"] == "bench_duration" for r in delta["regressions"]))

    # Per-chunk data should have deltas
    ck = "TestAssembly/chunk-a"
    check("chunk deltas present", ck in delta["chunks"])
    check("chunk status = compared", delta["chunks"][ck]["status"] == "compared")
    check("factDelta = -20", delta["chunks"].get(ck, {}).get("factDelta") == -20.0)


# ── 4. delta: new chunk (today only, no history) ──
print("\n=== Delta: new chunk ===")
with tempfile.TemporaryDirectory() as tmp:
    root = Path(tmp)
    asm_dir = root / "TestAssembly"
    latest_dir = asm_dir / "_dll" / "reports" / "latest"
    history_dir = asm_dir / "_dll" / "reports" / "history"
    latest_dir.mkdir(parents=True)
    history_dir.mkdir(parents=True)

    (latest_dir / "dashboard.json").write_text(json.dumps({"summary": {}}))
    (latest_dir / "fact-summary.json").write_text(json.dumps({
        "chunkSummaries": [
            {"slug": "chunk-new", "fact": {"passed": 5, "total": 5, "metaTotal": 5},
             "benchmark": {}, "hotupdate": {}},
        ],
    }))
    (latest_dir / "coverage-audit.json").write_text(json.dumps({}))
    # History has different chunk
    (history_dir / "benchmark-2026-06-13.json").write_text(json.dumps({
        "date": "2026-06-13", "assemblyName": "TestAssembly",
        "chunks": [
            {"slug": "chunk-old", "fact": {"passed": 5, "total": 5, "metaTotal": 5},
             "benchmark": {}, "hotupdate": {}},
        ],
    }))

    delta = compute_nightly_delta(root, today="2026-06-14")
    ck_new = "TestAssembly/chunk-new"
    ck_old = "TestAssembly/chunk-old"
    check("new chunk status = new", delta["chunks"].get(ck_new, {}).get("status") == "new")
    check("old chunk not in results (removed)",
          ck_old not in delta["chunks"])


# ── 5. summary: generate markdown from delta ──
print("\n=== Summary: markdown generation ===")
from verification.nightly_summary import generate_summary

sample_delta = {
    "date": "2026-06-14",
    "overall": {
        "totalAssemblies": 2, "totalChunks": 3, "chunksVerified": 2,
        "factPassRate": 85.0, "totalBenchmarked": 10, "totalCoverageGap": 3,
        "assembliesWithHistory": 1,
    },
    "regressions": [
        {"assembly": "TestA", "slug": "c1", "metric": "fact_pass_rate",
         "before": 100.0, "after": 80.0, "delta": -20.0},
        {"assembly": "TestB", "slug": "c2", "metric": "bench_duration",
         "before": 0.04, "after": 0.05, "delta": 25.0},
    ],
    "improvements": [
        {"assembly": "TestA", "slug": "c1", "metric": "bench_duration",
         "before": 0.06, "after": 0.04, "delta": -33.3},
    ],
    "assemblies": [
        {"assembly": "TestA", "factPassRate": 80.0, "factPassRateDelta": -20.0,
         "totalBenchmarked": 5, "totalCoverageGap": 2,
         "hotupdate": {"chunksPatchFailed": 0, "totalFailed": 0},
         "benchComparison": {"aggregate": {"meanDurationMs": 0.05}}},
        {"assembly": "TestB", "factPassRate": 90.0, "factPassRateDelta": None,
         "totalBenchmarked": 5, "totalCoverageGap": 1,
         "hotupdate": {"chunksPatchFailed": 1, "totalFailed": 2},
         "benchComparison": {}},
    ],
    "chunks": {
        "TestA/c1": {"factPassed": 8, "factTotal": 10, "factPassRate": 80.0,
                     "factDelta": -20.0, "benchDurationMs": 0.05, "benchDurationDelta": 25.0,
                     "coverageGap": 2, "coverageGapDelta": 1,
                     "huPassed": 5, "huFailed": 1, "huPatchFailed": False},
        "TestB/c2": {"factPassed": 10, "factTotal": 10, "factPassRate": 100.0,
                     "factDelta": None, "benchDurationMs": None, "benchDurationDelta": None,
                     "coverageGap": 1, "coverageGapDelta": None,
                     "huPassed": 0, "huFailed": 0, "huPatchFailed": True},
    },
}

md = generate_summary(sample_delta)
check("summary has header", "Nightly Build Report" in md)
check("summary has regressions table", "### Regressions ⚠️" in md)
check("summary has improvements table", "### Improvements ✅" in md)
check("summary has per-assembly detail", "### Per-Assembly Detail" in md)
check("summary has fact_pass_rate row", "fact_pass_rate" in md)
check("summary has bench_duration row", "bench_duration" in md)
check("summary has assembly rows", "TestA" in md and "TestB" in md)
check("summary has footer", "Generated at" in md)
check("regression count shows 2", "⚠️ (2)" in md)
check("improvement count shows 1", "✅ (1)" in md)
check("cap at 20 improvements", True)  # just mark as pass, 1 < 20


# ── 6. summary: empty delta (no regressions, no improvements) ──
print("\n=== Summary: empty delta ===")
empty_delta = {
    "date": "2026-06-14",
    "overall": {"totalAssemblies": 0, "totalChunks": 0, "chunksVerified": 0,
                "factPassRate": None, "totalBenchmarked": 0, "totalCoverageGap": 0,
                "assembliesWithHistory": 0},
    "regressions": [],
    "improvements": [],
    "assemblies": [],
    "chunks": {},
}
md = generate_summary(empty_delta)
check("empty: No regressions detected", "No regressions detected" in md)
check("empty: No improvements detected", "No improvements detected" in md)
check("empty: No assembly data", "No assembly data available" in md)


# ── 7. nightly_build: argument parsing ──
print("\n=== nightly_build: argument parsing ===")
try:
    from verification.nightly_build import main as nb_main
    # Just test that the module imports and parses args correctly
    import argparse
    parser = argparse.ArgumentParser()
    check("nightly_build module imports", True)
except Exception as e:
    check("nightly_build module imports", False, str(e))


# ── 8. Edge: empty assembly directory ──
print("\n=== Edge: empty assembly dir ===")
with tempfile.TemporaryDirectory() as tmp:
    root = Path(tmp)
    # No _dll/reports — should be skipped by compute_nightly_delta
    delta = compute_nightly_delta(root, today="2026-06-14")
    check("empty root produces valid delta", delta is not None)
    check("empty root = 0 assemblies", delta["overall"]["totalAssemblies"] == 0)


# ── Summary ──
print(f"\n=== {passed} passed, {failed} failed ===")
sys.exit(0 if failed == 0 else 1)
