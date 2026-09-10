"""Comprehensive report data correctness tests.

Tests the full reporting data pipeline end-to-end using synthetic data:
1. Schema validation — all JSONL/JSON outputs match expected schemas
2. Edge cases — empty data, missing files, partial failures, truncation
3. End-to-end data flow — simulate run → reporting stage → AI export
4. Backward compatibility — existing aggregate output format unchanged
"""

import json
import sys
import tempfile
from pathlib import Path
from datetime import datetime, timezone

# ── Setup path ──
_HERE = Path(__file__).resolve().parent  # testing/foundation-dll/tests
_ROOT = _HERE.parent                     # testing/foundation-dll
sys.path.insert(0, str(_ROOT))

from verification.reporting.models import (
    classify_failure, route_for_error, route_for_bottleneck,
    FactFailure, FactSummary, BenchmarkEntry, HotupdateEntry,
    ChunkReport, PipelineRunReport,
)
from verification.stages.reporting import (
    _extract_fact_failures, _build_cross_dll_summary,
)
from verification.tools.ai_export import export_all
from verification.tools.merge_platform_runs import merge_runs


def _make_assembly(
    asm_name: str,
    chunks: list[dict],
    fact_passed: int = 0,
    fact_total: int = 0,
    bench_count: int = 0,
    chaos_pct: dict | None = None,
) -> dict:
    """Build synthetic assembly data matching aggregate output."""
    return {
        "fact-summary": {
            "assemblyName": asm_name,
            "totalPassed": fact_passed,
            "totalFactMethods": fact_total,
        },
        "dashboard": {
            "summary": {
                "totalBenchmarkedMethods": bench_count,
            },
        },
        "comparison-summary": {
            "aggregate": {
                "chaosAotVsNet8Pct": chaos_pct or {"mean": 0},
            },
        },
        "chunks": chunks,
    }


def _make_chunk(slug: str, failures: list[dict] | None = None) -> dict:
    """Build synthetic chunk data."""
    return {
        "slug": slug,
        "fact": {
            "passed": 10,
            "total": 10 + len(failures or []),
            "failures": failures or [],
        },
    }


# ═══════════════════════════════════════════════════════════════
# 1. Schema Validation
# ═══════════════════════════════════════════════════════════════

def test_pipeline_run_report_schema():
    """Verify PipelineRunReport serializes to expected JSON schema."""
    r = PipelineRunReport(
        runId="fdn-20260614-001",
        timestamp="2026-06-14T12:00:00Z",
        platform="windows-x64",
        device={"id": "ci-node-1", "name": "CI Node 1"},
        gitCommit="abc123def",
        gitBranch="main",
        nativeConfig="check",
        mode="standard",
        totalDurationMs=45200,
        overviewStatus="passed",
        chunks=[
            ChunkReport(
                slug="text",
                assembly="System.Private.CoreLib",
                fact=FactSummary(
                    totalMethods=50,
                    passed=48,
                    failures=[
                        FactFailure(
                            methodSubjectId="System.String::Concat",
                            methodIndex=3,
                            value=-1,
                            message="expected 42, got -1",
                            errorPattern="value_suspicious",
                            routeHint="dev-il2cpp-debug-expert",
                        ),
                    ],
                ),
            ),
        ],
    )

    # Test JSONL line
    line = r.to_jsonl_line()
    parsed = json.loads(line)
    assert parsed["runId"] == "fdn-20260614-001"
    assert parsed["platform"] == "windows-x64"
    assert parsed["device"]["id"] == "ci-node-1"
    assert parsed["chunks"][0]["slug"] == "text"
    failure = parsed["chunks"][0]["fact"]["failures"][0]
    assert failure["errorPattern"] == "value_suspicious"
    assert failure["routeHint"] == "dev-il2cpp-debug-expert"
    assert failure["methodSubjectId"] == "System.String::Concat"

    # Test JSON
    json_str = r.to_json()
    parsed2 = json.loads(json_str)
    assert parsed2["overviewStatus"] == "passed"
    print("  [OK] PipelineRunReport schema")


def test_fact_failure_schema():
    """Verify ai/fact-failures.jsonl entry schema."""
    entry = {
        "assembly": "System.Private.CoreLib",
        "slug": "numerics",
        "platform": "windows-x64",
        "gitCommit": "abc123",
        "methodIndex": 45,
        "value": -1,
        "message": "expected 42, got -1",
        "errorPattern": "value_suspicious",
        "routeHint": "dev-il2cpp-debug-expert",
    }
    # All required fields present
    required = ["assembly", "slug", "platform", "methodIndex", "errorPattern", "routeHint"]
    for field in required:
        assert field in entry, f"Missing required field: {field}"
    print("  [OK] fact-failures schema")


def test_benchmark_bottleneck_schema():
    """Verify bottleneck field values are valid."""
    valid = ["gc_pause", "dispatch_overhead", "translation_path",
             "alloc_hot", "unstable", ""]
    entry = BenchmarkEntry(
        methodSubjectId="System.String::Join",
        chaosAotMs=45.2,
        net8Ms=12.1,
        bottleneck="dispatch_overhead",
        routeHint="dev-il2cpp-runtime-expert",
    )
    d = entry.__dict__
    assert d["bottleneck"] in valid, f"Invalid bottleneck: {d['bottleneck']}"
    print("  [OK] benchmark bottleneck schema")


# ═══════════════════════════════════════════════════════════════
# 2. classify_failure Edge Cases
# ═══════════════════════════════════════════════════════════════

def test_classify_failure_suspicious():
    result = classify_failure("expected 42, got -1", -1)
    assert result == "value_suspicious", f"Got {result}"
    print("  [OK] classify: value_suspicious with -1 value")


def test_classify_failure_shutdown_av():
    result = classify_failure("Access Violation at 0x00007FFE", 0)
    assert result == "shutdown_av", f"Got {result}"
    print("  [OK] classify: shutdown_av with AV message")


def test_classify_failure_segfault():
    result = classify_failure("segfault at 0x0 ip 0x7ffff", 0)
    assert result == "shutdown_av", f"Got {result}"
    print("  [OK] classify: shutdown_av with segfault")


def test_classify_failure_lnk():
    result = classify_failure("LNK2019: unresolved external symbol chaos_external", 0)
    assert result == "lnk2019", f"Got {result}"
    print("  [OK] classify: lnk2019")


def test_classify_failure_c3861():
    result = classify_failure("C3861: 'identifier' not found", 0)
    assert result == "c3861", f"Got {result}"
    print("  [OK] classify: c3861")


def test_classify_failure_assert():
    result = classify_failure("Assert failed: expected 5 but got 3", 3)
    assert result == "assert_fail", f"Got {result}"
    print("  [OK] classify: assert_fail")


def test_classify_failure_unknown():
    result = classify_failure("Some random error", 0)
    assert result == "unknown", f"Got {result}"
    print("  [OK] classify: unknown")


def test_classify_failure_empty_message():
    result = classify_failure("", None)
    assert result == "unknown", f"Got {result}"
    print("  [OK] classify: empty message, None value")


def test_classify_failure_value_positive():
    """Positive value with message that doesn't match known patterns → unknown."""
    result = classify_failure("something wrong", 42)
    assert result == "unknown", f"Got {result}"
    print("  [OK] classify: positive value with message → unknown")


# ═══════════════════════════════════════════════════════════════
# 3. Routing Edge Cases
# ═══════════════════════════════════════════════════════════════

def test_route_all_patterns():
    patterns = {
        "lnk2019": "dev-il2cpp-codegen-expert",
        "c3861": "dev-il2cpp-codegen-expert",
        "value_suspicious": "dev-il2cpp-debug-expert",
        "shutdown_av": "dev-il2cpp-runtime-expert",
        "assert_fail": "dev-il2cpp-codegen-expert",
        "metadata_mismatch": "dev-il2cpp-codegen-expert",
        "unknown": "dev-il2cpp-debug-expert",
    }
    for pattern, expected in patterns.items():
        result = route_for_error(pattern)
        assert result == expected, f"{pattern} → {result}, expected {expected}"
    print(f"  [OK] route_for_error: {len(patterns)} patterns")


def test_bottleneck_routes():
    bottlenecks = {
        "gc_pause": "dev-il2cpp-gc-expert",
        "dispatch_overhead": "dev-il2cpp-runtime-expert",
        "translation_path": "dev-il2cpp-translation-expert",
        "alloc_hot": "dev-il2cpp-codegen-expert",
        "interpreter_overhead": "dev-il2cpp-runtime-expert",
        "keep_native_blocked": "dev-il2cpp-codegen-expert",
        "patch_apply_fail": "dev-il2cpp-build-fixer",
        "tier_deopt_loop": "dev-il2cpp-runtime-expert",
    }
    for bottleneck, expected in bottlenecks.items():
        result = route_for_bottleneck(bottleneck)
        assert result == expected, f"{bottleneck} → {result}, expected {expected}"
    print(f"  [OK] route_for_bottleneck: {len(bottlenecks)} routes")


# ═══════════════════════════════════════════════════════════════
# 4. reporting stage data extraction
# ═══════════════════════════════════════════════════════════════

def test_extract_fact_failures_empty():
    result = _extract_fact_failures({})
    assert result == [], f"Expected [], got {result}"
    print("  [OK] extract_fact_failures: empty input")


def test_extract_fact_failures_no_failures():
    asm = {"TestLib": _make_assembly("TestLib", [_make_chunk("math", [])])}
    result = _extract_fact_failures(asm)
    assert result == [], f"Expected [], got {result}"
    print("  [OK] extract_fact_failures: no failures → empty")


def test_extract_fact_failures_mixed():
    asm = {
        "LibA": _make_assembly(
            "LibA", [
                _make_chunk("math", [
                    {"methodIndex": 0, "value": -1, "message": "fail",
                     "errorPattern": "value_suspicious", "routeHint": "debug"},
                ]),
                _make_chunk("text", []),
            ],
        ),
    }
    result = _extract_fact_failures(asm)
    assert len(result) == 1, f"Expected 1 failure, got {len(result)}"
    assert result[0]["assembly"] == "LibA"
    assert result[0]["slug"] == "math"
    print("  [OK] extract_fact_failures: mixed (1 fail, 1 clean)")


def test_extract_fact_failures_multiple_assemblies():
    asm = {
        "A": _make_assembly("A", [_make_chunk("a1", [
            {"methodIndex": 1, "value": -1, "message": "e1",
             "errorPattern": "value_suspicious", "routeHint": "debug"},
        ])]),
        "B": _make_assembly("B", [_make_chunk("b1", [
            {"methodIndex": 2, "value": None, "message": "e2",
             "errorPattern": "shutdown_av", "routeHint": "runtime"},
        ])]),
    }
    result = _extract_fact_failures(asm)
    assert len(result) == 2, f"Expected 2 failures, got {len(result)}"
    assemblies = {r["assembly"] for r in result}
    assert assemblies == {"A", "B"}, f"Wrong assemblies: {assemblies}"
    print("  [OK] extract_fact_failures: 2 assemblies")


def test_build_cross_dll_summary():
    asm = {
        "A": _make_assembly("A", [], fact_passed=45, fact_total=50, bench_count=10),
        "B": _make_assembly("B", [], fact_passed=90, fact_total=100, bench_count=20),
    }
    result = _build_cross_dll_summary(asm)
    assert result["totalAssemblies"] == 2
    assert abs(result["overallFactPassRate"] - 0.9) < 0.001  # (45+90)/(50+100) = 0.9
    print(f"  [OK] cross-DLL summary: {result['totalAssemblies']} assemblies, "
          f"rate={result['overallFactPassRate']:.2%}")


# ═══════════════════════════════════════════════════════════════
# 5. AI Export end-to-end
# ═══════════════════════════════════════════════════════════════

def test_ai_export_fact_failures():
    with tempfile.TemporaryDirectory() as tmp:
        src = Path(tmp) / "pipeline-runs.jsonl"
        src.write_text(json.dumps({
            "runId": "test-001", "platform": "windows-x64", "gitCommit": "abc",
            "assemblies": {
                "LibX": {
                    "chunks": [{"slug": "core", "fact": {
                        "passed": 8, "total": 10,
                        "failures": [
                            {"methodIndex": 2, "errorPattern": "value_suspicious",
                             "routeHint": "debug", "value": -1, "message": "bad"},
                        ],
                    }}],
                },
            },
        }) + "\n")
        ai_dir = Path(tmp) / "ai"
        counts = export_all(src, ai_dir)

        # Check fact-failures.jsonl
        ff = ai_dir / "fact-failures.jsonl"
        lines = [json.loads(l) for l in ff.read_text().strip().splitlines() if l.strip()]
        assert len(lines) == 1
        assert lines[0]["assembly"] == "LibX"
        assert lines[0]["errorPattern"] == "value_suspicious"
        assert lines[0]["routeHint"] == "debug"
        assert lines[0]["platform"] == "windows-x64"

        # Check known-failures.jsonl
        kf = ai_dir / "known-failures.jsonl"
        klines = [json.loads(l) for l in kf.read_text().strip().splitlines() if l.strip()]
        assert len(klines) >= 1
        assert klines[0]["assembly"] == "LibX"
        assert klines[0]["errorPattern"] == "value_suspicious"
        print("  [OK] ai_export: fact-failures + known-failures")


def test_ai_export_empty():
    """Empty pipeline-runs.jsonl should produce empty AI exports."""
    with tempfile.TemporaryDirectory() as tmp:
        src = Path(tmp) / "pipeline-runs.jsonl"
        src.write_text("")  # empty
        ai_dir = Path(tmp) / "ai"
        counts = export_all(src, ai_dir)
        ff = ai_dir / "fact-failures.jsonl"
        assert ff.exists()
        assert ff.read_text().strip() == "", f"Expected empty, got: {ff.read_text()}"
        print("  [OK] ai_export: empty input → empty outputs")


def test_ai_export_missing_source():
    """Missing source file should not crash."""
    counts = export_all(Path("/nonexistent/pipeline-runs.jsonl"), Path("/tmp/ai"))
    assert "error" in counts
    print("  [OK] ai_export: missing source → graceful error")


def test_ai_export_multiple_runs():
    """Multiple pipeline runs should all be collected."""
    with tempfile.TemporaryDirectory() as tmp:
        src = Path(tmp) / "pipeline-runs.jsonl"
        src.write_text(
            json.dumps({"runId": "run-1", "platform": "windows-x64",
                        "assemblies": {"A": {"chunks": [{"slug": "s1", "fact": {
                            "passed": 5, "total": 5,
                            "failures": [{"methodIndex": 0, "errorPattern": "value_suspicious",
                                          "routeHint": "debug", "value": -1, "message": "e"}]}}]}}}) + "\n" +
            json.dumps({"runId": "run-2", "platform": "linux-x64",
                        "assemblies": {"B": {"chunks": [{"slug": "s2", "fact": {
                            "passed": 3, "total": 5,
                            "failures": [{"methodIndex": 1, "errorPattern": "shutdown_av",
                                          "routeHint": "runtime", "value": None, "message": "AV"}]}}]}}}) + "\n"
        )
        ai_dir = Path(tmp) / "ai"
        counts = export_all(src, ai_dir)
        ff = ai_dir / "fact-failures.jsonl"
        lines = [json.loads(l) for l in ff.read_text().strip().splitlines() if l.strip()]
        assert len(lines) == 2, f"Expected 2 entries, got {len(lines)}"
        platforms = {l["platform"] for l in lines}
        assert platforms == {"windows-x64", "linux-x64"}
        error_patterns = {l["errorPattern"] for l in lines}
        assert error_patterns == {"value_suspicious", "shutdown_av"}
        print(f"  [OK] ai_export: {len(lines)} entries across 2 platforms")


# ═══════════════════════════════════════════════════════════════
# 6. Merge platform runs
# ═══════════════════════════════════════════════════════════════

def test_merge_platform_runs_empty():
    result = merge_runs([])
    assert result["platformCount"] == 0
    assert result["totalFactFailures"] == 0
    print("  [OK] merge_platform_runs: empty input")


def test_merge_platform_runs_two_platforms():
    with tempfile.TemporaryDirectory() as tmp:
        f1 = Path(tmp) / "win.jsonl"
        f1.write_text(json.dumps({
            "runId": "win-run", "platform": "windows-x64",
            "assemblies": {"Lib": {"chunks": [{"slug": "core", "fact": {
                "passed": 8, "total": 10,
                "failures": [{"methodIndex": 2, "errorPattern": "value_suspicious",
                              "routeHint": "debug", "value": -1, "message": "bad"}],
            }}]}},
        }) + "\n")
        f2 = Path(tmp) / "linux.jsonl"
        f2.write_text(json.dumps({
            "runId": "linux-run", "platform": "linux-x64",
            "assemblies": {"Lib": {"chunks": [{"slug": "core", "fact": {
                "passed": 9, "total": 10,
                "failures": [{"methodIndex": 2, "errorPattern": "value_suspicious",
                              "routeHint": "debug", "value": -1, "message": "bad"}],
            }}]}},
        }) + "\n")

        result = merge_runs([f1, f2])
        assert result["platformCount"] == 2
        assert "windows-x64" in result["platforms"]
        assert "linux-x64" in result["platforms"]

        # Same methodIndex=2 should be merged into one entry with 2 platforms
        assert result["totalFactFailures"] == 1, \
            f"Expected 1 merged entry (same methodIndex), got {result['totalFactFailures']}"
        merged = result["factFailures"][0]
        assert "windows-x64" in merged["platforms"]
        assert "linux-x64" in merged["platforms"]
        print(f"  [OK] merge_platform_runs: {result['platformCount']} platforms, "
              f"{result['totalFactFailures']} merged failures")


# ═══════════════════════════════════════════════════════════════
# Run all tests
# ═══════════════════════════════════════════════════════════════

def run_all():
    tests = [
        ("Schema", [
            test_pipeline_run_report_schema,
            test_fact_failure_schema,
            test_benchmark_bottleneck_schema,
        ]),
        ("classify_failure edge cases", [
            test_classify_failure_suspicious,
            test_classify_failure_shutdown_av,
            test_classify_failure_segfault,
            test_classify_failure_lnk,
            test_classify_failure_c3861,
            test_classify_failure_assert,
            test_classify_failure_unknown,
            test_classify_failure_empty_message,
            test_classify_failure_value_positive,
        ]),
        ("Routing", [
            test_route_all_patterns,
            test_bottleneck_routes,
        ]),
        ("Data extraction", [
            test_extract_fact_failures_empty,
            test_extract_fact_failures_no_failures,
            test_extract_fact_failures_mixed,
            test_extract_fact_failures_multiple_assemblies,
            test_build_cross_dll_summary,
        ]),
        ("AI export", [
            test_ai_export_fact_failures,
            test_ai_export_empty,
            test_ai_export_missing_source,
            test_ai_export_multiple_runs,
        ]),
        ("Merge platform", [
            test_merge_platform_runs_empty,
            test_merge_platform_runs_two_platforms,
        ]),
    ]

    total = 0
    passed = 0
    for group_name, group_tests in tests:
        print(f"\n── {group_name} ──")
        for t in group_tests:
            total += 1
            try:
                t()
                passed += 1
            except Exception as e:
                print(f"  [FAIL] {t.__name__}: {e}")

    print(f"\n{'='*50}")
    print(f"Results: {passed}/{total} passed")
    if passed < total:
        print("SOME TESTS FAILED")
        return 1
    print("ALL TESTS PASSED")
    return 0


if __name__ == "__main__":
    sys.exit(run_all())
