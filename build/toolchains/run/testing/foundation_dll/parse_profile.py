#!/usr/bin/env python3
"""Parse PROFILE| pipe-delimited stderr data into a structured hot-spot report.

Usage:
    python parse_profile.py <profile_data.txt> [--csv hotspot_report.csv] [--md hotspot_report.md]

Input format (from profile.h ProfileDump):
    PROFILE|CALIBRATION|ns_per_cycle=0.227284
    PROFILE|ScopeName|avg=32780|avg_ns=7450|min=836|max=56188|count=3|total_ns=22351

Output:
    - CSV: aggregated, sorted by total_ns descending
    - MD: formatted report with sections
"""

import csv
import re
import sys
from collections import OrderedDict
from pathlib import Path


PROFILE_RE = re.compile(
    r"^PROFILE\|(?P<scope>[^|]+)\|"
    r"avg=(?P<avg>[^|]+)\|avg_ns=(?P<avg_ns>[^|]+)\|"
    r"min=(?P<min>[^|]+)\|max=(?P<max>[^|]+)\|"
    r"count=(?P<count>[^|]+)\|total_ns=(?P<total_ns>[^|]+)"
)

CALIB_RE = re.compile(r"^PROFILE\|CALIBRATION\|ns_per_cycle=(?P<ns_per_cycle>[^\s]+)")


def parse_file(path: str) -> tuple[list[dict], float]:
    """Parse PROFILE lines, return (records, ns_per_cycle)."""
    records = []
    ns_per_cycle = 0.0
    with open(path, "r", encoding="utf-8") as f:
        for line in f:
            line = line.strip()
            m = CALIB_RE.match(line)
            if m:
                ns_per_cycle = float(m.group("ns_per_cycle"))
                continue
            m = PROFILE_RE.match(line)
            if m:
                d = m.groupdict()
                d["avg"] = float(d["avg"])
                d["avg_ns"] = float(d["avg_ns"])
                d["min"] = int(d["min"])
                d["max"] = int(d["max"])
                d["count"] = int(d["count"])
                d["total_ns"] = float(d["total_ns"])
                records.append(d)
    return records, ns_per_cycle


def classify_scope(scope: str) -> str:
    """Classify scope into a category for section grouping."""
    if scope == "FastExecute":
        return "A-FastExecuteLoop"
    if scope.startswith("InterpreterEntryDirect"):
        return "B-EntryDispatch"
    if scope.startswith("Handle_"):
        return "C-Handlers"
    if scope in ("GcAllocate", "GcAllocateAtomic", "ObjectNew", "ArrayNew",
                  "StringNewUtf8", "BoxValueObject", "GcAllocateImpl"):
        return "D-Allocation"
    if scope.startswith("Gc") or scope.startswith("GC_"):
        return "E-GC"
    if scope in ("InterpreterDispatch", "InterpreterDispatchRaw"):
        return "F-Dispatch"
    if scope in ("ResolveVirtualMethodPointer", "ResolveMethodTable"):
        return "G-VTable"
    if "MicHit" in scope or "MicMiss" in scope:
        return "H-MIC"
    return "Z-Other"


def generate_report(records: list[dict], ns_per_cycle: float, csv_path: str | None = None, md_path: str | None = None):
    """Aggregate, sort, and output report."""

    if not records:
        print("No PROFILE records found.")
        return

    # Sort by total_ns descending
    records.sort(key=lambda r: r["total_ns"], reverse=True)

    # CSV output
    if csv_path:
        fieldnames = ["scope", "avg", "avg_ns", "min", "max", "count", "total_ns"]
        with open(csv_path, "w", newline="", encoding="utf-8") as f:
            writer = csv.DictWriter(f, fieldnames=fieldnames)
            writer.writeheader()
            for r in records:
                writer.writerow({k: r[k] for k in fieldnames})
        print(f"CSV written to {csv_path}")

    # MD report
    if md_path:
        total_ns_all = sum(r["total_ns"] for r in records)

        # Group by category
        categories: dict[str, list[dict]] = OrderedDict()
        for r in records:
            cat = classify_scope(r["scope"])
            categories.setdefault(cat, []).append(r)

        lines = [
            "# Hot-Spot Profile Report",
            "",
            f"- **Calibration**: {ns_per_cycle:.4f} ns/cycle",
            f"- **Total recorded time**: {total_ns_all:.0f} ns ({total_ns_all / 1e6:.2f} ms)",
            f"- **Unique scopes**: {len(records)}",
            "",
            "---",
            "",
        ]

        # Section A: FastExecute main loop
        lines += [
            "## A. FastExecute Main Loop",
            "",
            "| Scope | Avg (ns) | Count | Total (ns) | % of Total |",
            "|-------|----------|-------|------------|------------|",
        ]
        for r in records:
            if r["scope"] == "FastExecute":
                pct = (r["total_ns"] / total_ns_all * 100) if total_ns_all > 0 else 0
                lines.append(f"| {r['scope']} | {r['avg_ns']:.0f} | {r['count']} | {r['total_ns']:.0f} | {pct:.1f}% |")
        if not any(r["scope"] == "FastExecute" for r in records):
            lines.append("| *(no FastExecute data)* |")
        lines += ["", "---", ""]

        # Section B: Entry dispatch paths
        lines += [
            "## B. Entry Dispatch Path Distribution",
            "",
            "| Path | Avg (ns) | Count | Total (ns) | % of Total |",
            "|------|----------|-------|------------|------------|",
        ]
        dispatch_prefixes = ("InterpreterEntryDirect",)
        for r in records:
            if r["scope"].startswith(dispatch_prefixes):
                pct = (r["total_ns"] / total_ns_all * 100) if total_ns_all > 0 else 0
                lines.append(f"| {r['scope']} | {r['avg_ns']:.0f} | {r['count']} | {r['total_ns']:.0f} | {pct:.1f}% |")
        lines += ["", "---", ""]

        # Section C: Top-10 Handlers
        lines += [
            "## C. Top-10 Handler Hot-Spots",
            "",
            "| Handler | Avg (ns) | Count | Total (ns) | % of Total |",
            "|---------|----------|-------|------------|------------|",
        ]
        handler_records = [r for r in records if r["scope"].startswith("Handle_")]
        for r in handler_records[:10]:
            pct = (r["total_ns"] / total_ns_all * 100) if total_ns_all > 0 else 0
            lines.append(f"| {r['scope']} | {r['avg_ns']:.0f} | {r['count']} | {r['total_ns']:.0f} | {pct:.1f}% |")
        if not handler_records:
            lines.append("| *(no handler data)* |")
        lines += ["", "---", ""]

        # Section D: Allocation pipeline
        lines += [
            "## D. Allocation Pipeline",
            "",
            "| Scope | Avg (ns) | Count | Total (ns) | % of Total |",
            "|-------|----------|-------|------------|------------|",
        ]
        alloc_scopes = ("GcAllocate", "GcAllocateAtomic", "ObjectNew", "ArrayNew",
                        "StringNewUtf8", "BoxValueObject", "GcAllocateImpl")
        for r in records:
            if r["scope"] in alloc_scopes:
                pct = (r["total_ns"] / total_ns_all * 100) if total_ns_all > 0 else 0
                lines.append(f"| {r['scope']} | {r['avg_ns']:.0f} | {r['count']} | {r['total_ns']:.0f} | {pct:.1f}% |")
        lines += ["", "---", ""]

        # Section E: MIC hit/miss
        lines += [
            "## E. MIC (Method Inline Cache) Hit/Miss",
            "",
            "| Scope | Avg (ns) | Count | Total (ns) | % of Total |",
            "|-------|----------|-------|------------|------------|",
        ]
        mic_records = [r for r in records if "MicHit" in r["scope"] or "MicMiss" in r["scope"]]
        for r in mic_records:
            pct = (r["total_ns"] / total_ns_all * 100) if total_ns_all > 0 else 0
            lines.append(f"| {r['scope']} | {r['avg_ns']:.0f} | {r['count']} | {r['total_ns']:.0f} | {pct:.1f}% |")
        # Calculate hit ratio
        hit_count = next((r["count"] for r in mic_records if "MicHit" in r["scope"]), 0)
        miss_count = next((r["count"] for r in mic_records if "MicMiss" in r["scope"]), 0)
        total_mic = hit_count + miss_count
        if total_mic > 0:
            hit_rate = hit_count / total_mic * 100
            lines.append(f"| **MIC Hit Rate** | | **{hit_count}/{total_mic} ({hit_rate:.1f}%)** | | |")
        if not mic_records:
            lines.append("| *(no MIC data)* |")
        lines += ["", "---", ""]

        # Section F: Full sorted table
        lines += [
            "## F. All Scopes (sorted by total_ns)",
            "",
            "| Scope | Avg (ns) | Count | Total (ns) | % |",
            "|-------|----------|-------|------------|----|",
        ]
        for r in records[:30]:  # top 30
            pct = (r["total_ns"] / total_ns_all * 100) if total_ns_all > 0 else 0
            lines.append(f"| {r['scope']} | {r['avg_ns']:.0f} | {r['count']} | {r['total_ns']:.0f} | {pct:.1f}% |")
        if len(records) > 30:
            lines.append(f"| *... and {len(records) - 30} more scopes* |")

        with open(md_path, "w", encoding="utf-8") as f:
            f.write("\n".join(lines) + "\n")
        print(f"MD report written to {md_path}")

    # Stdout summary
    print(f"\nParsed {len(records)} scopes, {ns_per_cycle:.4f} ns/cycle")
    print(f"{'Top 5 by total_ns':-^60}")
    print(f"{'Scope':<40} {'Avg(ns)':>10} {'Count':>8} {'Total(ns)':>12}")
    print("-" * 72)
    for r in records[:5]:
        print(f"{r['scope']:<40} {r['avg_ns']:>10.0f} {r['count']:>8} {r['total_ns']:>12.0f}")


def main():
    if len(sys.argv) < 2:
        print(__doc__)
        sys.exit(1)

    input_path = sys.argv[1]
    csv_path = None
    md_path = None

    for i, arg in enumerate(sys.argv[2:], start=2):
        if arg == "--csv" and i + 1 < len(sys.argv):
            csv_path = sys.argv[i + 1]
        elif arg == "--md" and i + 1 < len(sys.argv):
            md_path = sys.argv[i + 1]

    if not Path(input_path).exists():
        print(f"Error: file not found: {input_path}")
        sys.exit(1)

    records, ns_per_cycle = parse_file(input_path)
    generate_report(records, ns_per_cycle, csv_path, md_path)


if __name__ == "__main__":
    main()
