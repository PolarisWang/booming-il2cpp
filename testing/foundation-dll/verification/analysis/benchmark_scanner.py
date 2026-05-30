"""benchmark_scanner — 全量扫描所有 family 的 benchmark 数据并输出优化优先级队列.

Usage:
    python -m verification.analysis.benchmark_scanner [options]

Options:
    --output FORMAT       json | table (default table)
    --sort-by FIELD       chaos-ns | vs-net8 | methods | freshness (default vs-net8)
    --min-methods N       只显示方法数 >= N 的 family (default 0)
    --include-missing     是否包含无数据的 family (default false)
    --top N               只显示前 N 个 (default 全部)
    --assembly NAME       目标 assembly (default System.Private.CoreLib)
    --json                等价于 --output json
"""

from __future__ import annotations

import argparse
import json
import sys
import time
from datetime import datetime, timezone
from pathlib import Path
from typing import Any, Dict, List, Optional, Tuple

# ── Constants ──────────────────────────────────────────────────────────

_FOUNDATION_DLL = Path(__file__).resolve().parents[2]
"""testing/foundation-dll/"""

_REPORT_REL = "multi-run/multi-run-report.json"

# Directories that should not be treated as family slugs
_NON_FAMILY_DIRS: set[str] = {
    "reports",
    "results",
    "config",
    "_contracts",
    "_core",
    "docs",
    "scripts",
    "testing",
    "verification",
    "__pycache__",
}

_SORT_KEY_MAP: dict[str, str] = {
    "chaos-ns": "chaos_aot_ns",
    "vs-net8": "slowdown_vs_net8",
    "methods": "method_count",
    "freshness": "data_timestamp",
}


# ── Data model helpers ─────────────────────────────────────────────────

def _find_summary(report: dict[str, Any], runtime: str) -> dict[str, Any] | None:
    """Find the summary entry for a given runtime name."""
    for s in report.get("summaries", []):
        if s.get("runtime") == runtime:
            return s
    return None


def _determine_data_quality(
    report: dict[str, Any] | None,
    chaos_summary: dict[str, Any] | None,
    net8_summary: dict[str, Any] | None,
) -> str:
    """Determine data quality label."""
    if report is None:
        return "missing"
    if chaos_summary is not None and net8_summary is not None:
        return "complete"
    return "partial"


def _safe_float(value: Any) -> float:
    """Safely convert to float, returning 0.0 on failure."""
    try:
        return float(value)
    except (TypeError, ValueError):
        return 0.0


def _extract_family_data(family_dir: Path) -> dict[str, Any] | None:
    """Extract benchmark data from a single family directory.

    Returns a dict with scanned metrics, or None if the directory
    is not a valid family (infra dir, etc.).
    """
    slug = family_dir.name

    # Skip non-family directories
    if slug in _NON_FAMILY_DIRS or slug.startswith("_"):
        return None

    report_path = family_dir / _REPORT_REL

    if not report_path.exists():
        return {
            "family": slug,
            "method_count": 0,
            "chaos_aot_ns": 0.0,
            "net8_jit_ns": 0.0,
            "speedup_vs_net8": 0.0,
            "slowdown_vs_net8": 0.0,
            "data_timestamp": "",
            "data_quality": "missing",
            "method_ok_count": 0,
            "stub_count": 0,
            "throws_count": 0,
            "unsupported_count": 0,
        }

    try:
        report = json.loads(report_path.read_text(encoding="utf-8"))
    except (OSError, json.JSONDecodeError):
        return {
            "family": slug,
            "method_count": 0,
            "chaos_aot_ns": 0.0,
            "net8_jit_ns": 0.0,
            "speedup_vs_net8": 0.0,
            "slowdown_vs_net8": 0.0,
            "data_timestamp": "",
            "data_quality": "missing",
            "method_ok_count": 0,
            "stub_count": 0,
            "throws_count": 0,
            "unsupported_count": 0,
        }

    chaos = _find_summary(report, "chaos-aot")
    net8 = _find_summary(report, "net8-jit")

    chaos_ns = _safe_float(chaos.get("geometric_mean_ns")) if chaos else 0.0
    net8_ns = _safe_float(net8.get("geometric_mean_ns")) if net8 else 0.0
    data_quality = _determine_data_quality(report, chaos, net8)

    # Compute speedup / slowdown
    speedup = (net8_ns / chaos_ns) if chaos_ns > 0 and net8_ns > 0 else 0.0
    slowdown = (chaos_ns / net8_ns) if chaos_ns > 0 and net8_ns > 0 else 0.0

    # Summaries from chaos-aot (preferred) or net8-jit for global counts
    ref_summary = chaos or net8
    method_count = ref_summary.get("method_count", 0) if ref_summary else 0
    ok_count = chaos.get("ok_count", 0) if chaos else 0
    stub_count = chaos.get("stub_count", 0) if chaos else 0
    throws_count = chaos.get("throws_count", 0) if chaos else 0
    unsupported_count = chaos.get("unsupported_count", 0) if chaos else 0

    timestamp = report.get("timestamp", "")

    return {
        "family": report.get("family", slug),
        "method_count": method_count,
        "chaos_aot_ns": round(chaos_ns, 2) if chaos_ns > 0 else 0.0,
        "net8_jit_ns": round(net8_ns, 2) if net8_ns > 0 else 0.0,
        "speedup_vs_net8": round(speedup, 2),
        "slowdown_vs_net8": round(slowdown, 2),
        "data_timestamp": timestamp,
        "data_quality": data_quality,
        "method_ok_count": ok_count,
        "stub_count": stub_count,
        "throws_count": throws_count,
        "unsupported_count": unsupported_count,
    }


# ── Priority scoring ───────────────────────────────────────────────────

def _compute_priority_score(data: dict[str, Any]) -> float:
    """Compute priority score: chaos_aot_ns * slowdown * method_count."""
    return data["chaos_aot_ns"] * data["slowdown_vs_net8"] * data["method_count"]


def _format_reasoning(data: dict[str, Any], priority_score: float) -> str:
    """Generate a human-readable reasoning string for the priority entry."""
    parts = []
    if data["chaos_aot_ns"] > 0:
        parts.append(f"{data['chaos_aot_ns']}ns chaos-aot")
    if data["slowdown_vs_net8"] > 0:
        parts.append(f"{data['slowdown_vs_net8']}x slower than NET8")
    if data["method_count"] > 0:
        parts.append(f"{data['method_count']} methods")
    if data["data_quality"] != "complete":
        parts.append(f"data: {data['data_quality']}")
    return ", ".join(parts) if parts else "no data"


# ── Core scanning logic ────────────────────────────────────────────────

def scan_assembly(
    assembly: str = "System.Private.CoreLib",
    foundation_dll: Path | None = None,
) -> dict[str, Any]:
    """Scan all families in an assembly and return structured results.

    Args:
        assembly: Assembly name (e.g. "System.Private.CoreLib").
        foundation_dll: Path to the foundation-dll directory. Auto-detected.

    Returns:
        dict with keys: assembly, scanned_at, summary, families, priority_queue
    """
    fd = foundation_dll or _FOUNDATION_DLL
    assembly_dir = fd / assembly

    if not assembly_dir.is_dir():
        raise FileNotFoundError(
            f"Assembly directory not found: {assembly_dir}"
        )

    # Scan all subdirectories
    families: dict[str, dict[str, Any]] = {}
    for child in sorted(assembly_dir.iterdir()):
        if not child.is_dir():
            continue
        data = _extract_family_data(child)
        if data is None:
            continue
        families[data["family"]] = data

    # Compute summary stats
    with_data = [f for f in families.values() if f["data_quality"] != "missing"]
    total_methods = sum(f["method_count"] for f in with_data)

    summary = {
        "total_families": len(families),
        "with_data": len(with_data),
        "missing": len(families) - len(with_data),
        "total_methods_with_data": total_methods,
    }

    # Build priority queue
    priority_queue: list[dict[str, Any]] = []
    for data in with_data:
        score = _compute_priority_score(data)
        if score > 0:
            priority_queue.append({
                "family": data["family"],
                "priority_score": round(score, 2),
                "reasoning": _format_reasoning(data, score),
            })

    # Sort priority queue descending by score
    priority_queue.sort(key=lambda x: x["priority_score"], reverse=True)

    # Assign ranks
    for i, entry in enumerate(priority_queue):
        entry["rank"] = i + 1

    scanned_at = datetime.now(timezone.utc).strftime("%Y-%m-%dT%H:%M:%SZ")

    return {
        "assembly": assembly,
        "scanned_at": scanned_at,
        "summary": summary,
        "families": families,
        "priority_queue": priority_queue,
    }


# ── Sorting / filtering ────────────────────────────────────────────────

def _sort_key(data: dict[str, Any], field: str) -> Tuple:
    """Return a sort key tuple (primary desc, family name asc for tiebreak)."""
    # Map field alias to actual dict key
    actual = _SORT_KEY_MAP.get(field, field)

    if actual == "data_timestamp":
        # Freshness: older timestamps first (still scanning for optimization)
        # "" timestamps sort last
        ts = data.get(actual, "")
        return (0 if ts else 1, ts, data.get("family", ""))
    elif actual == "chaos_aot_ns":
        return (-data.get(actual, 0), data.get("family", ""))
    elif actual == "slowdown_vs_net8":
        return (-data.get(actual, 0), data.get("family", ""))
    elif actual == "method_count":
        return (-data.get(actual, 0), data.get("family", ""))
    else:
        return (-data.get(actual, 0), data.get("family", ""))


def _filter_and_sort(
    scan_result: dict[str, Any],
    sort_by: str = "vs-net8",
    min_methods: int = 0,
    include_missing: bool = False,
    top_n: int = 0,
) -> list[dict[str, Any]]:
    """Apply filters and sorting to scanned families.

    Returns an ordered list of family data dicts.
    """
    families = list(scan_result["families"].values())

    # Filter by data quality
    if not include_missing:
        families = [f for f in families if f["data_quality"] != "missing"]

    # Filter by min methods (only for families with data)
    if min_methods > 0:
        families = [
            f for f in families
            if f["data_quality"] == "missing" or f["method_count"] >= min_methods
        ]

    # Sort
    families.sort(key=lambda f: _sort_key(f, sort_by))

    # Apply top N (only meaningful when sorting by non-freshness fields)
    if top_n > 0 and sort_by != "freshness":
        families = families[:top_n]

    return families


# ── Output formatters ──────────────────────────────────────────────────

def _format_table(
    scan_result: dict[str, Any],
    families_ordered: list[dict[str, Any]],
    sort_by: str,
) -> str:
    """Format results as a human-readable table."""
    s = scan_result["summary"]
    lines: list[str] = []

    heading = f"Benchmark Inventory for {scan_result['assembly']}"
    lines.append(heading)
    lines.append("=" * len(heading))
    lines.append(
        f"Total families: {s['total_families']} | "
        f"With data: {s['with_data']} | "
        f"Missing: {s['missing']} | "
        f"Total methods with data: {s['total_methods_with_data']}"
    )
    lines.append("")

    # Build priority index for rank lookup
    rank_map: dict[str, int] = {}
    for entry in scan_result["priority_queue"]:
        rank_map[entry["family"]] = entry["rank"]

    header = f"{'Rank':<6} {'Family':<35} {'Methods':<8} {'ChaosAOT':<11} {'NET8':<11} {'Speedup':<9} {'DataQuality':<13}"
    lines.append(header)
    lines.append("-" * len(header))

    for data in families_ordered:
        rank = rank_map.get(data["family"], "-")
        if isinstance(rank, int) and rank > 0:
            rank_str = str(rank)
        else:
            rank_str = "-"

        chaos_str = f"{data['chaos_aot_ns']:.2f}ns" if data["chaos_aot_ns"] > 0 else "-"
        net8_str = f"{data['net8_jit_ns']:.2f}ns" if data["net8_jit_ns"] > 0 else "-"
        speedup_str = f"{data['speedup_vs_net8']:.2f}x" if data["speedup_vs_net8"] > 0 else "-"

        line = (
            f"{rank_str:<6} {data['family']:<35} {data['method_count']:<8} "
            f"{chaos_str:<11} {net8_str:<11} {speedup_str:<9} "
            f"{data['data_quality']:<13}"
        )
        lines.append(line)

    return "\n".join(lines)


def _format_json(scan_result: dict[str, Any]) -> str:
    """Format results as JSON."""
    return json.dumps(scan_result, indent=2, ensure_ascii=False)


# ── CLI entry point ────────────────────────────────────────────────────

def _parse_args(argv: list[str] | None = None) -> argparse.Namespace:
    parser = argparse.ArgumentParser(
        description="Benchmark scanner — scan family benchmark data and output "
                    "optimization priority queue.",
    )
    parser.add_argument(
        "--output",
        choices=["json", "table"],
        default="table",
        help="Output format (default: table)",
    )
    parser.add_argument(
        "--json",
        action="store_true",
        help="Equivalent to --output json",
    )
    parser.add_argument(
        "--sort-by",
        choices=list(_SORT_KEY_MAP.keys()),
        default="vs-net8",
        help="Sort field (default: vs-net8)",
    )
    parser.add_argument(
        "--min-methods",
        type=int,
        default=0,
        help="Minimum method count to include (default: 0)",
    )
    parser.add_argument(
        "--include-missing",
        action="store_true",
        default=False,
        help="Include families without benchmark data (default: false)",
    )
    parser.add_argument(
        "--top",
        type=int,
        default=0,
        help="Show only top N families (default: all)",
    )
    parser.add_argument(
        "--assembly",
        default="System.Private.CoreLib",
        help="Target assembly (default: System.Private.CoreLib)",
    )
    return parser.parse_args(argv)


def main(argv: list[str] | None = None) -> int:
    """CLI entry point. Returns exit code."""
    args = _parse_args(argv)

    # Resolve output format (--json overrides --output)
    output_format = "json" if args.json else args.output

    try:
        result = scan_assembly(args.assembly)
    except FileNotFoundError as e:
        print(f"Error: {e}", file=sys.stderr)
        return 1

    families_ordered = _filter_and_sort(
        result,
        sort_by=args.sort_by,
        min_methods=args.min_methods,
        include_missing=args.include_missing,
        top_n=args.top,
    )

    # Build output result (reorder families dict)
    output_result = result.copy()
    output_result["families"] = {
        f["family"]: f for f in families_ordered
    }

    if output_format == "json":
        print(_format_json(output_result))
    else:
        print(_format_table(result, families_ordered, args.sort_by))

    return 0


if __name__ == "__main__":
    sys.exit(main())
