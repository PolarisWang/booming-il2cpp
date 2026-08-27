#!/usr/bin/env python3
"""Profile analyzer — reads profile JSON reports and produces analytical summaries.

Usage:
    # Analyze a single profile report
    python scripts/profile/profile_analyzer.py profile_data/convert-char/profile-report.json

    # Compare two reports (baseline vs optimized)
    python scripts/profile/profile_analyzer.py profile_data/convert-char/baseline.json profile_data/convert-char/optimized.json --compare

    # Filter by noise threshold
    python scripts/profile/profile_analyzer.py profile-report.json --min-pct 0.1 --min-count 100
"""

from __future__ import annotations

import argparse
import json
import re
import sys
from pathlib import Path
from typing import Any

# Scope name clustering patterns
_CLUSTER_PATTERNS: list[tuple[str, str]] = [
    # Dispatch handlers (the interpreter dispatch loop)
    (r"^FastExecute$", "dispatch_loop"),
    (r"^RegisterExecute$", "dispatch_loop"),
    (r"^InterpreterEntryDirect", "dispatch_loop"),
    (r"^InterpreterDispatch", "dispatch_loop"),
    (r"^SetupFrame", "dispatch_loop"),
    # Call/CallVirt dispatch
    (r"^Handle_Call[^a-zA-Z]", "call_dispatch"),
    (r"^Handle_CallVirt", "callvirt_dispatch"),
    (r"^Handle_CallVirt_MicHit", "callvirt_mic_hit"),
    (r"^Handle_CallVirt_MicMiss", "callvirt_mic_miss"),
    (r"^Handle_CallVirtConstrained", "callvirt_constrained"),
    (r"^Handle_Calli", "calli_dispatch"),
    # Arithmetic
    (r"^Handle_Add$", "arithmetic"),
    (r"^Handle_Sub$", "arithmetic"),
    (r"^Handle_Mul$", "arithmetic"),
    (r"^Handle_Div$", "arithmetic"),
    (r"^Handle_Rem$", "arithmetic"),
    (r"^Handle_Neg$", "arithmetic"),
    (r"^Handle_And$", "bitwise"),
    (r"^Handle_Or$", "bitwise"),
    (r"^Handle_Xor$", "bitwise"),
    (r"^Handle_Not$", "bitwise"),
    (r"^Handle_Shl$", "bitwise"),
    (r"^Handle_Shr$", "bitwise"),
    # Load/Store
    (r"^Handle_LdLoc", "local_load"),
    (r"^Handle_StLoc", "local_store"),
    (r"^Handle_LdArg", "arg_load"),
    (r"^Handle_LdFld", "field_load"),
    (r"^Handle_StFld", "field_store"),
    (r"^Handle_LdElem", "element_load"),
    (r"^Handle_StElem", "element_store"),
    (r"^Handle_LdStr$", "string_load"),
    (r"^Handle_LdFtn$", "ftn_load"),
    # Constants
    (r"^Handle_LdcI4", "const_load"),
    (r"^Handle_LdcI8", "const_load"),
    (r"^Handle_LdcR4", "const_load"),
    (r"^Handle_LdcR8", "const_load"),
    (r"^Handle_LdNull$", "const_load"),
    # Branches
    (r"^Handle_Br[\s$]", "branch"),
    (r"^Handle_BrTrue", "branch_cond"),
    (r"^Handle_BrFalse", "branch_cond"),
    (r"^Handle_Beq$", "branch_cond"),
    (r"^Handle_Blt", "branch_cond"),
    (r"^Handle_Bgt", "branch_cond"),
    (r"^Handle_Ble", "branch_cond"),
    (r"^Handle_Bge", "branch_cond"),
    (r"^Handle_BneUn", "branch_cond"),
    (r"^Handle_Switch", "branch_switch"),
    # Conversions
    (r"^Handle_Conv_", "conversion"),
    (r"^Handle_ConvOvf", "conversion"),
    (r"^Handle_Unbox", "unbox"),
    (r"^Handle_Box$", "box"),
    # Object model
    (r"^Handle_NewObj", "new_object"),
    (r"^Handle_NewArr", "new_array"),
    (r"^Handle_CastClass", "type_check"),
    (r"^Handle_IsInst", "type_check"),
    (r"^Handle_InitObj", "init_object"),
    (r"^Handle_SizeOf", "sizeof"),
    (r"^Handle_LdLen$", "array_len"),
    # Exception handling
    (r"^Handle_Throw", "exception"),
    (r"^Handle_Rethrow", "exception"),
    (r"^Handle_Leave", "exception"),
    (r"^Handle_EndFinally", "exception"),
    (r"^Handle_EndFilter", "exception"),
    # Stack operations
    (r"^Handle_Pop$", "stack_op"),
    (r"^Handle_Dup$", "stack_op"),
    (r"^Handle_LdInd", "indirect_load"),
    (r"^Handle_StInd", "indirect_store"),
    # Memory block
    (r"^Handle_Cpblk$", "mem_block"),
    (r"^Handle_InitBlk$", "mem_block"),
    (r"^Handle_LocAlloc", "local_alloc"),
    # GC
    (r"^NurseryAllocate", "gc_alloc"),
    (r"^GcAllocate", "gc_alloc"),
    (r"^ObjectNew", "gc_alloc"),
    (r"^ArrayNew", "gc_alloc"),
    (r"^StringNew", "gc_alloc"),
    (r"^BoxValueObject", "gc_alloc"),
    (r"^GC_", "gc_collect"),
    (r"^OldGen::", "gc_collect"),
    (r"^Gen1", "gc_collect"),
    (r"^YoungCollector", "gc_collect"),
    (r"^ParallelMark", "gc_collect"),
    # JIT codegen
    (r"^Codegen::", "jit_codegen"),
    (r"^Generate$", "jit_codegen"),
    # VTable/Method resolution
    (r"^ResolveVirtualMethod", "vtable_resolve"),
    (r"^ResolveMethodTable", "method_resolve"),
    # Safepoint
    (r"^SafepointPoll", "safepoint"),
    (r"^GcScanAllThreadRoots", "gc_scan"),
    # Method invocation
    (r"^MethodInvoke", "method_invoke"),
    # Benchmark overhead
    (r"^BenchmarkLoop", "benchmark_framework"),
]


def cluster_scope_name(name: str) -> str:
    """Map a PROFILE_SCOPE name to a cluster category."""
    for pattern, cluster in _CLUSTER_PATTERNS:
        if re.match(pattern, name):
            return cluster
    return "other"


def load_report(path: Path) -> dict[str, Any]:
    with open(path, encoding="utf-8") as f:
        return json.load(f)


def filter_scopes(
    scopes: list[dict[str, Any]],
    *,
    min_pct: float = 0.1,
    min_count: int = 100,
    min_avg_cycles: float = 100.0,
) -> list[dict[str, Any]]:
    """Filter out noisy scopes below thresholds."""
    filtered = []
    for s in scopes:
        if s["percent_of_total"] < min_pct:
            continue
        if s["total_count"] < min_count:
            continue
        if s.get("avg_cycles", s["avg_ns"] / 0.35) < min_avg_cycles:
            continue
        filtered.append(s)
    return filtered


def compute_cluster_summary(
    scopes: list[dict[str, Any]],
) -> list[dict[str, Any]]:
    """Aggregate scopes into clusters and compute cluster-level stats."""
    clusters: dict[str, dict[str, float]] = {}
    for s in scopes:
        cluster = cluster_scope_name(s["scope_name"])
        if cluster not in clusters:
            clusters[cluster] = {
                "total_ns": 0.0,
                "total_count": 0,
                "num_scopes": 0,
            }
        c = clusters[cluster]
        c["total_ns"] += s["total_ns"]
        c["total_count"] += s["total_count"]
        c["num_scopes"] += 1

    total_ns = sum(c["total_ns"] for c in clusters.values())
    cluster_list = []
    for name, data in clusters.items():
        cluster_list.append({
            "cluster": name,
            "total_ns": round(data["total_ns"], 1),
            "total_count": data["total_count"],
            "num_scopes": data["num_scopes"],
            "percent_of_total": (
                round(data["total_ns"] / total_ns * 100, 2) if total_ns > 0 else 0.0
            ),
        })
    cluster_list.sort(key=lambda c: -c["total_ns"])
    return cluster_list


def print_scope_table(
    scopes: list[dict[str, Any]], title: str, limit: int = 20
) -> None:
    """Print a formatted table of scopes."""
    print(f"\n{'=' * 80}")
    print(f"  {title}")
    print(f"{'=' * 80}")
    print(
        f"  {'Scope':40s} {'Total(ns)':>12s} {'Avg(ns)':>8s} {'Count':>8s} {'%':>6s} {'Cluster':>16s}"
    )
    print(f"  {'-' * 40} {'-' * 12} {'-' * 8} {'-' * 8} {'-' * 6} {'-' * 16}")
    for s in scopes[:limit]:
        cluster = cluster_scope_name(s["scope_name"])
        print(
            f"  {s['scope_name']:40s} "
            f"{s['total_ns']:>12.1f} "
            f"{s['avg_ns']:>8.1f} "
            f"{s['total_count']:>8d} "
            f"{s['percent_of_total']:>5.2f}% "
            f"{cluster:>16s}"
        )


def print_cluster_table(clusters: list[dict[str, Any]], limit: int = 15) -> None:
    """Print a formatted table of scope clusters."""
    print(f"\n{'─' * 60}")
    print(f"  Cluster Summary")
    print(f"{'─' * 60}")
    print(
        f"  {'Cluster':20s} {'Total(ns)':>12s} {'Count':>8s} {'Scopes':>6s} {'%':>6s}"
    )
    print(f"  {'-' * 20} {'-' * 12} {'-' * 8} {'-' * 6} {'-' * 6}")
    for c in clusters[:limit]:
        print(
            f"  {c['cluster']:20s} "
            f"{c['total_ns']:>12.1f} "
            f"{c['total_count']:>8d} "
            f"{c['num_scopes']:>6d} "
            f"{c['percent_of_total']:>5.2f}%"
        )


def print_comparison(
    baseline: dict[str, Any], optimized: dict[str, Any], limit: int = 15
) -> None:
    """Compare two profile reports (baseline vs optimized)."""
    b_scopes = {s["scope_name"]: s for s in baseline.get("scopes_by_total_ns", [])}
    o_scopes = {s["scope_name"]: s for s in optimized.get("scopes_by_total_ns", [])}

    all_names = set(b_scopes.keys()) | set(o_scopes.keys())

    print(f"\n{'=' * 90}")
    print(f"  Comparison: baseline vs optimized")
    print(f"{'=' * 90}")
    print(f"  Baseline benchmark ns: {baseline.get('total_benchmark_ns', 0):.1f}")
    print(f"  Optimized benchmark ns: {optimized.get('total_benchmark_ns', 0):.1f}")

    changes = []
    for name in all_names:
        b = b_scopes.get(name)
        o = o_scopes.get(name)
        if b and o:
            delta_pct = (o["total_ns"] - b["total_ns"]) / b["total_ns"] * 100
            changes.append({
                "scope_name": name,
                "baseline_ns": b["total_ns"],
                "optimized_ns": o["total_ns"],
                "delta_pct": round(delta_pct, 2),
            })
        elif b and not o:
            changes.append({
                "scope_name": name,
                "baseline_ns": b["total_ns"],
                "optimized_ns": 0,
                "delta_pct": -100.0,
            })

    changes.sort(key=lambda c: -abs(c["delta_pct"]))

    print(f"\n  Scope changes (sorted by |delta|):")
    print(
        f"  {'Scope':40s} {'Baseline(ns)':>12s} {'Opt(ns)':>12s} {'Delta%':>8s}"
    )
    print(f"  {'-' * 40} {'-' * 12} {'-' * 12} {'-' * 8}")
    for c in changes[:limit]:
        marker = "▲" if c["delta_pct"] > 0 else "▼"
        print(
            f"  {c['scope_name']:40s} "
            f"{c['baseline_ns']:>12.1f} "
            f"{c['optimized_ns']:>12.1f} "
            f"{marker}{abs(c['delta_pct']):>7.2f}%"
        )

    improved = sum(1 for c in changes if c["delta_pct"] < -1)
    regressed = sum(1 for c in changes if c["delta_pct"] > 1)
    unchanged = len(changes) - improved - regressed
    print(f"\n  Summary: {improved} improved, {regressed} regressed, {unchanged} unchanged")


def generate_markdown_report(
    report: dict[str, Any],
    scopes: list[dict[str, Any]],
    clusters: list[dict[str, Any]],
) -> str:
    """Generate a markdown summary report."""
    lines = [
        f"# Profile Report: {report.get('family', 'unknown')}",
        f"",
        f"- Captured at: {report.get('captured_at', 'N/A')}",
        f"- Methods: {report.get('num_methods', 0)}",
        f"- Iterations: {report.get('iterations', 0)}",
        f"- Total benchmark time: {report.get('total_benchmark_ns', 0) / 1_000_000:.1f} ms",
        f"- ns/cycle: {report.get('ns_per_cycle', 'N/A')}",
        f"",
        f"## Cluster Summary",
        f"",
        f"| Cluster | Total (ns) | Count | Scopes | % of Total |",
        f"|---------|-----------|-------|--------|-----------|",
    ]
    for c in clusters[:10]:
        lines.append(
            f"| {c['cluster']} | {c['total_ns']:.1f} | {c['total_count']} | "
            f"{c['num_scopes']} | {c['percent_of_total']:.2f}% |"
        )

    lines.extend([
        "",
        f"## Top Scopes by Total CPU Time",
        "",
        f"| Scope | Total (ns) | Avg (ns) | Count | % | Cluster |",
        f"|-------|-----------|---------|-------|---|---------|",
    ])
    for s in scopes[:20]:
        cluster = cluster_scope_name(s["scope_name"])
        lines.append(
            f"| {s['scope_name']} | {s['total_ns']:.1f} | {s['avg_ns']:.1f} | "
            f"{s['total_count']} | {s['percent_of_total']:.2f}% | {cluster} |"
        )

    return "\n".join(lines)


def main() -> None:
    parser = argparse.ArgumentParser(description="Profile data analyzer")
    parser.add_argument("reports", nargs="+", type=Path, help="Profile report JSON files")
    parser.add_argument("--compare", action="store_true", help="Compare two reports")
    parser.add_argument("--min-pct", type=float, default=0.1, help="Min %% of total to include")
    parser.add_argument("--min-count", type=int, default=100, help="Min call count")
    parser.add_argument(
        "--min-avg-cycles", type=float, default=100.0, help="Min avg cycles to include"
    )
    parser.add_argument("--markdown", type=Path, default=None, help="Output markdown report path")
    parser.add_argument("--limit", type=int, default=20, help="Max scopes to display")
    args = parser.parse_args()

    if args.compare:
        if len(args.reports) != 2:
            print("Error: --compare requires exactly 2 report files", file=sys.stderr)
            sys.exit(1)
        baseline = load_report(args.reports[0])
        optimized = load_report(args.reports[1])
        print_comparison(baseline, optimized, args.limit)
        return

    for report_path in args.reports:
        report = load_report(report_path)
        scopes = report.get("scopes_by_total_ns", [])
        filtered = filter_scopes(
            scopes,
            min_pct=args.min_pct,
            min_count=args.min_count,
            min_avg_cycles=args.min_avg_cycles,
        )

        print(f"\nReport: {report_path}")
        print(f"  Family: {report.get('family', 'N/A')}")
        print(f"  Methods: {report.get('num_methods', 0)}")
        print(f"  Iterations: {report.get('iterations', 0)}")
        print(f"  Total benchmark: {report.get('total_benchmark_ns', 0) / 1_000_000:.1f} ms")
        print(f"  Scopes total: {len(scopes)}, after filter: {len(filtered)}")

        clusters = compute_cluster_summary(filtered)
        print_cluster_table(clusters, args.limit)

        print_scope_table(filtered, "Top Scopes by Total CPU Time", args.limit)

        # Also show by avg_ns for latency insights
        by_avg = sorted(filtered, key=lambda s: -s["avg_ns"])
        print_scope_table(by_avg, "Top Scopes by Average Latency", 10)

        # Markdown output
        if args.markdown:
            md = generate_markdown_report(report, filtered, clusters)
            args.markdown.parent.mkdir(parents=True, exist_ok=True)
            args.markdown.write_text(md, encoding="utf-8")
            print(f"\nMarkdown report written to {args.markdown}")


if __name__ == "__main__":
    main()
