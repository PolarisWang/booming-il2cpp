"""Regression report — aggregate GC health, perf comparison, and code size into a single report.

This module ties together gc_check, perf_compare, and code_size_tracker to produce
a unified optimization report for a chunk or assembly.

Usage:
    from verification.analysis.regression_report import generate_report, print_report

    report = generate_report("System.Private.CoreLib", "numerics")
    print_report(report)
"""

from __future__ import annotations

import json
from datetime import datetime, timezone
from pathlib import Path
from typing import Any

from verification.analysis.gc_check import check_profile, print_report as gc_print
from verification.analysis.perf_baseline import PerfBaseline
from verification.analysis.perf_compare import compare_profiles, print_comparison
from verification.analysis.code_size_tracker import CodeSizeTracker

_RESULTS_BASE = Path(__file__).resolve().parent.parent / "results" / "foundation-dll"


def generate_report(
    assembly: str,
    slug: str,
    foundation_dir: Path | None = None,
    thresholds: dict[str, float] | None = None,
) -> dict[str, Any]:
    """Generate a unified optimization report for a chunk.

    Reads the chunk's profile.json, compares against baseline (if available),
    runs GC health checks, collects code size data, and produces a single report.

    Args:
        assembly: Assembly name (e.g. "System.Private.CoreLib").
        slug: Chunk slug (e.g. "numerics").
        foundation_dir: Foundation-dll directory (default: auto-detect).
        thresholds: Custom GC check thresholds.

    Returns:
        Dict with keys: chunk, summary, gcHealth, perfComparison, codeSize, optimizationOpportunities.
    """
    if foundation_dir is None:
        # Auto-detect from this file's location
        foundation_dir = Path(__file__).resolve().parent.parent.parent

    chunk_dir = foundation_dir / "chunks" / slug
    results_dir = chunk_dir / "results"
    native_dir = chunk_dir / "native"

    report: dict[str, Any] = {
        "assembly": assembly,
        "slug": slug,
        "generatedAt": datetime.now(timezone.utc).isoformat(),
        "gcHealth": {"status": "no_data"},
        "perfComparison": {"status": "no_baseline"},
        "codeSize": {"status": "no_data"},
        "optimizationOpportunities": [],
    }

    # ── 1. Load profile data ──
    profile_path = results_dir / "profile.json"
    profile_data: list[dict] = []
    profile_summary: dict = {}
    if profile_path.exists():
        try:
            prof = json.loads(profile_path.read_text(encoding="utf-8"))
            profile_data = prof.get("profileData", [])
            profile_summary = prof.get("summary", {})
        except (json.JSONDecodeError, OSError):
            pass

    # ── 2. GC Health ──
    if profile_data:
        gc_health = check_profile(profile_data, thresholds)
        report["gcHealth"] = gc_health

        # Derive optimization opportunities from GC issues
        opps: list[dict] = []
        for issue in gc_health.get("issues", []):
            if issue["check"] == "GC-01":
                opps.append({
                    "category": "gc_pause",
                    "methodIndex": issue["methodIndex"],
                    "finding": f"Max GC pause {issue['actual']}ns exceeds threshold",
                    "suggestion": "Reduce allocation per method or increase nursery size",
                })
            elif issue["check"] == "GC-03":
                opps.append({
                    "category": "fast_path",
                    "methodIndex": issue["methodIndex"],
                    "finding": f"Fast path rate {issue['actual']:.1%} below threshold",
                    "suggestion": "Object size exceeds bump cache limits; consider object pooling",
                })
            elif issue["check"] == "GC-04":
                opps.append({
                    "category": "heap_fragmentation",
                    "methodIndex": issue["methodIndex"],
                    "finding": f"Heap delta exceeds allocation volume by {issue['actual']}x",
                    "suggestion": "Possible fragmentation; check large object allocations",
                })
            elif issue["check"] == "GC-05":
                opps.append({
                    "category": "alloc_hotspot",
                    "methodIndex": issue["methodIndex"],
                    "finding": f"High allocation count ({issue['actual']})",
                    "suggestion": "Consider caching or reducing per-call allocations",
                })
        report["optimizationOpportunities"] = opps

    # ── 3. Perf Comparison vs Baseline ──
    baseline_mgr = PerfBaseline()
    baseline = baseline_mgr.get(assembly, slug)
    if baseline and profile_summary:
        comparison = compare_profiles(baseline, profile_summary)
        report["perfComparison"] = comparison
    elif baseline:
        report["perfComparison"] = {"status": "no_current_data"}
    else:
        report["perfComparison"] = {"status": "no_baseline"}

    # ── 4. Code Size ──
    entry_exe = native_dir / "entry.exe"
    if entry_exe.exists():
        tracker = CodeSizeTracker()
        code_size_data = tracker.collect(entry_exe)
        report["codeSize"] = {
            "status": "ok",
            "sectionSizes": code_size_data.get("sectionSizes", {}),
            "functionCount": len(code_size_data.get("functionSizes", [])),
            "source": code_size_data.get("source", "none"),
        }
        # Compare code size with baseline
        if baseline:
            b_sections = baseline.get("sectionSizes", {})
            t_sections = code_size_data.get("sectionSizes", {})
            if b_sections.get("text", 0) > 0 and t_sections.get("text", 0) > 0:
                delta = t_sections["text"] - b_sections["text"]
                pct = round(delta / b_sections["text"] * 100, 2) if b_sections["text"] > 0 else 0
                report["codeSize"]["vsBaseline"] = {
                    "deltaBytes": delta,
                    "changePct": pct,
                }
                if pct > 10:
                    report["optimizationOpportunities"].append({
                        "category": "code_growth",
                        "methodIndex": -1,
                        "finding": f"Code size grew {pct}% vs baseline (+{delta} bytes)",
                        "suggestion": "Review recent codegen changes for bloat",
                    })

    # ── Summary ──
    gc_status = report["gcHealth"].get("status", "no_data")
    perf_status = report["perfComparison"].get("summary", {}).get("regressed", 0)
    code_status = "ok" if report["codeSize"].get("status") == "ok" else "no_data"

    opportunity_count = len(report["optimizationOpportunities"])
    if opportunity_count > 0 or perf_status > 0:
        overall = "needs_attention"
    elif gc_status == "healthy":
        overall = "healthy"
    else:
        overall = "unknown"

    report["summary"] = {
        "overallStatus": overall,
        "gcHealthStatus": gc_status,
        "perfRegressions": perf_status,
        "codeSizeStatus": code_status,
        "optimizationOpportunityCount": opportunity_count,
    }

    return report


def print_report(report: dict[str, Any]) -> None:
    """Print a human-readable optimization report."""
    summary = report.get("summary", {})
    print(f"{'='*60}")
    print(f"  Optimization Report: {report['assembly']}/{report['slug']}")
    print(f"{'='*60}")
    print(f"  Overall: {summary.get('overallStatus', 'unknown')}")
    print()

    # GC Health
    gc = report.get("gcHealth", {})
    print(f"  GC Health: {gc.get('status', 'N/A')}")
    if gc.get("issues"):
        gc_print(gc)
    print()

    # Perf Comparison
    perf = report.get("perfComparison", {})
    if perf.get("summary"):
        print(f"  Perf Comparison vs Baseline:")
        print_comparison(perf)
    else:
        print(f"  Perf Comparison: {perf.get('status', 'N/A')}")
    print()

    # Code Size
    cs = report.get("codeSize", {})
    if cs.get("sectionSizes"):
        sec = cs["sectionSizes"]
        print(f"  Code Size: {cs.get('source', 'N/A')}")
        print(f"    .text: {sec.get('text', 0)} bytes")
        print(f"    .data: {sec.get('data', 0)} bytes")
        print(f"    Total: {sec.get('total', 0)} bytes")
        if cs.get("vsBaseline"):
            vs = cs["vsBaseline"]
            arrow = "🔴" if vs["changePct"] > 0 else "🟢"
            print(f"    {arrow} vs Baseline: {vs['deltaBytes']:+d} bytes ({vs['changePct']:+.2f}%)")
    print()

    # Optimization Opportunities
    opps = report.get("optimizationOpportunities", [])
    if opps:
        print(f"  Optimization Opportunities ({len(opps)}):")
        for opp in opps:
            print(f"    [{opp['category']}] {opp['finding']}")
            print(f"      Suggestion: {opp['suggestion']}")
    else:
        print(f"  No optimization opportunities identified.")
    print(f"{'='*60}")


def generate_assembly_report(assembly: str) -> dict[str, Any]:
    """Generate a combined report for all chunks in an assembly."""
    foundation_dir = Path(__file__).resolve().parent.parent.parent
    chunks_dir = foundation_dir / "chunks"

    chunk_reports: list[dict] = []
    if chunks_dir.exists():
        for slug in sorted(d.name for d in chunks_dir.iterdir() if d.is_dir()):
            cr = generate_report(assembly, slug)
            chunk_reports.append(cr)

    return {
        "assembly": assembly,
        "generatedAt": datetime.now(timezone.utc).isoformat(),
        "chunkCount": len(chunk_reports),
        "chunks": chunk_reports,
        "summary": {
            "totalOptimizationOpportunities": sum(
                len(c.get("optimizationOpportunities", [])) for c in chunk_reports
            ),
            "chunksWithIssues": sum(
                1 for c in chunk_reports
                if c.get("gcHealth", {}).get("status") in ("unhealthy", "degraded", "warning")
            ),
            "chunksWithRegressions": sum(
                1 for c in chunk_reports
                if c.get("perfComparison", {}).get("summary", {}).get("regressed", 0) > 0
            ),
        },
    }


# ── CLI entry point ──
def main() -> None:
    import argparse
    parser = argparse.ArgumentParser(description="Generate optimization report")
    parser.add_argument("assembly", help="Assembly name (e.g. System.Private.CoreLib)")
    parser.add_argument("--slug", default=None, help="Chunk slug (omit for all chunks)")
    parser.add_argument("--json", action="store_true", help="Output as JSON")
    parser.add_argument("--output", default=None, help="Write report to file")

    args = parser.parse_args()

    if args.slug:
        report = generate_report(args.assembly, args.slug)
    else:
        report = generate_assembly_report(args.assembly)

    if args.json or args.output:
        output = json.dumps(report, indent=2, ensure_ascii=False)
        if args.output:
            Path(args.output).write_text(output, encoding="utf-8")
            print(f"Report written to {args.output}")
        else:
            print(output)
    else:
        if args.slug:
            print_report(report)
        else:
            print(f"Assembly Report: {args.assembly}")
            print(f"  Chunks: {report['chunkCount']}")
            print(f"  Opportunities: {report['summary']['totalOptimizationOpportunities']}")
            print(f"  Chunks with issues: {report['summary']['chunksWithIssues']}")
            print(f"  Chunks with regressions: {report['summary']['chunksWithRegressions']}")
            for cr in report.get("chunks", []):
                opps = len(cr.get("optimizationOpportunities", []))
                gc_s = cr.get("gcHealth", {}).get("status", "?")
                print(f"    {cr['slug']}: GC={gc_s}, opps={opps}")


if __name__ == "__main__":
    main()
