"""Cross-session benchmark history tracker.

Records per-session snapshots of multi-run benchmark reports into
JSONL history files, enabling trend analysis and cross-session comparison.

CLI:
    python -m verification.analysis.benchmark_history record <slug> \
        --assembly System.Private.CoreLib

    python -m verification.analysis.benchmark_history trend <slug> \
        --assembly System.Private.CoreLib [--output table|json]

    python -m verification.analysis.benchmark_history compare <slug> \
        --from <session-id> --to <session-id> --assembly System.Private.CoreLib
"""

import argparse
import json
import math
import subprocess
import sys
import uuid
from datetime import datetime, timezone
from pathlib import Path
from typing import Any, Dict, List, Optional

# ── path helpers ──────────────────────────────────────────────────────────


def _get_foundation_root() -> Path:
    """Return absolute path to foundation-dll root directory."""
    return Path(__file__).resolve().parent.parent.parent


def _get_family_dir(assembly: str, slug: str) -> Path:
    """Return the family directory under foundation-dll root."""
    return _get_foundation_root() / assembly / slug


def _get_history_path(assembly: str, slug: str) -> Path:
    """Return path to the benchmark-history.jsonl file for this family."""
    return _get_family_dir(assembly, slug) / "perf" / "benchmark-history.jsonl"


def _get_commit_hash() -> str:
    """Get current git commit hash from the repository root."""
    try:
        result = subprocess.run(
            ["git", "rev-parse", "HEAD"],
            capture_output=True,
            text=True,
            cwd=_get_foundation_root(),
        )
        return result.stdout.strip()
    except Exception:
        return "unknown"


def _load_history(history_path: Path) -> List[Dict[str, Any]]:
    """Load all records from a history JSONL file."""
    records: List[Dict[str, Any]] = []
    if not history_path.exists():
        return records
    with open(history_path, encoding="utf-8") as f:
        for line in f:
            line = line.strip()
            if line:
                records.append(json.loads(line))
    return records


def _extract_method_datum(m: dict, report: dict) -> dict:
    """Extract per-method data from a single method entry in the report."""
    samples = m.get("samples", {})

    def _ns(key: str) -> Optional[float]:
        entry = samples.get(key)
        if entry is None:
            return None
        return entry.get("mean_ns")

    chaos_aot_ns = _ns("chaos-aot")
    chaos_jit_ns = _ns("chaos-jit")
    net8_jit_ns = _ns("net8-jit")
    net10_jit_ns = _ns("net10-jit")
    mono_ns = _ns("mono")

    # Determine overall per-method status
    status = "ok"
    for runtime_key in report.get("config", {}).get("requested_runtimes", []):
        s = samples.get(runtime_key, {})
        if s.get("status") and s["status"] != "ok":
            status = s["status"]
            break

    # Compute aot_vs_net8_ratio
    aot_vs_net8_ratio: Optional[float] = None
    if chaos_aot_ns is not None and net8_jit_ns is not None and net8_jit_ns > 0:
        aot_vs_net8_ratio = round(chaos_aot_ns / net8_jit_ns, 4)

    return {
        "subject_id": m.get("subject_id", ""),
        "chaos_aot_ns": chaos_aot_ns,
        "chaos_jit_ns": chaos_jit_ns,
        "net8_jit_ns": net8_jit_ns,
        "net10_jit_ns": net10_jit_ns,
        "mono_ns": mono_ns,
        "aot_vs_net8_ratio": aot_vs_net8_ratio,
        "status": status,
    }


# ── record ────────────────────────────────────────────────────────────────


def record(slug: str, assembly: str, session_id: Optional[str] = None) -> str:
    """Record a snapshot from multi-run-report.json into the history file.

    Returns the assigned session_id.
    """
    family_dir = _get_family_dir(assembly, slug)
    report_path = family_dir / "multi-run" / "multi-run-report.json"

    if not report_path.exists():
        print(f"ERROR: multi-run report not found at {report_path}", file=sys.stderr)
        sys.exit(1)

    with open(report_path, encoding="utf-8") as f:
        report = json.load(f)

    sid = session_id or str(uuid.uuid4())[:8]

    commit_hash = _get_commit_hash()
    timestamp = datetime.now(timezone.utc).strftime("%Y-%m-%dT%H:%M:%SZ")

    # Build per-method data
    methods_data = [_extract_method_datum(m, report) for m in report.get("methods", [])]

    # Build summaries dict keyed by runtime
    summaries_dict: Dict[str, dict] = {}
    for s in report.get("summaries", []):
        runtime = s.get("runtime", "")
        summaries_dict[runtime] = {
            "geometric_mean_ns": s.get("geometric_mean_ns"),
            "min_ns": s.get("min_ns"),
            "max_ns": s.get("max_ns"),
            "median_ns": s.get("median_ns"),
            "ok_count": s.get("ok_count"),
            "total_methods": s.get("method_count"),
        }

    record_data = {
        "timestamp": timestamp,
        "commit_hash": commit_hash,
        "slug": slug,
        "family": report.get("family", slug),
        "assembly": assembly,
        "session_id": sid,
        "duration_ms": report.get("duration_ms"),
        "config": report.get("config", {}),
        "methods": methods_data,
        "summaries": summaries_dict,
    }

    # Append to history file
    history_path = _get_history_path(assembly, slug)
    history_path.parent.mkdir(parents=True, exist_ok=True)

    with open(history_path, "a", encoding="utf-8") as f:
        f.write(json.dumps(record_data, ensure_ascii=False) + "\n")

    total_ok = sum(1 for m in methods_data if m["status"] == "ok")
    print(f"Recorded session {sid} for {assembly}/{slug}")
    print(f"  Timestamp: {timestamp}")
    print(f"  Commit:    {commit_hash[:12]}")
    print(f"  Methods:   {len(methods_data)} total, {total_ok} ok")
    print(f"  History:   {history_path}")

    return sid


# ── trend ─────────────────────────────────────────────────────────────────


def trend(slug: str, assembly: str, output: str = "table", limit: Optional[int] = None):
    """Show geometric mean trend over time."""
    history_path = _get_history_path(assembly, slug)
    records = _load_history(history_path)

    if not records:
        print(f"No history records found at {history_path}", file=sys.stderr)
        sys.exit(1)

    if limit:
        records = records[-limit:]

    if output == "json":
        print(json.dumps(records, indent=2, ensure_ascii=False))
        return

    _print_trend_table(assembly, slug, records)


def _sorted_runtimes(records: List[Dict[str, Any]]) -> List[str]:
    """Collect runtime keys across records in a stable preferred order."""
    all_runtimes: set = set()
    for rec in records:
        all_runtimes.update(rec.get("summaries", {}).keys())
    preferred = ["chaos-aot", "chaos-jit", "net8-jit", "net10-jit", "mono"]
    ordered = [r for r in preferred if r in all_runtimes]
    ordered += sorted(r for r in all_runtimes if r not in preferred)
    return ordered


def _format_geo_ns(value: Optional[float], width: int = 14) -> str:
    """Format a geometric mean ns value for table display."""
    if value is not None:
        return f"{value:>{width}.1f}"
    return f"{'N/A':>{width}}"


def _print_trend_table(assembly: str, slug: str, records: List[Dict[str, Any]]):
    """Print a human-readable trend table."""
    runtimes = _sorted_runtimes(records)

    # Build column headers
    cols = [
        ("Session", 12),
        ("Date", 20),
        ("Commit", 14),
    ]
    for r in runtimes:
        cols.append((r, 18))
    cols.append(("AOT/n8", 10))
    cols.append(("OK/Total", 10))

    def _render_row(values: List[str]) -> str:
        return "  ".join(f"{v:{w}}" for v, (_, w) in zip(values, cols))

    header = _render_row([c for c, _ in cols])
    sep = "-" * max(len(header), 80)

    print(f"\nBenchmark Trend: {assembly}/{slug}")
    print(f"  Records: {len(records)}")
    print(sep)
    print(header)
    print(sep)

    for rec in records:
        s = rec.get("summaries", {})
        sid = (rec.get("session_id") or "?")[:12]
        ts = (rec.get("timestamp") or "?")[:19]
        commit = (rec.get("commit_hash") or "?")[:12]

        cells = [sid, ts, commit]

        for r in runtimes:
            gm = s.get(r, {}).get("geometric_mean_ns")
            cells.append(_format_geo_ns(gm, 0))

        # Global AOT/n8 ratio
        aot = s.get("chaos-aot", {}).get("geometric_mean_ns")
        n8 = s.get("net8-jit", {}).get("geometric_mean_ns")
        if aot and n8 and n8 > 0:
            cells.append(f"{aot/n8:>10.1f}")
        else:
            cells.append(f"{'N/A':>10}")

        # OK count across first available runtime
        first_runtime = runtimes[0] if runtimes else ""
        first_s = s.get(first_runtime, {})
        ok_c = first_s.get("ok_count")
        total_c = first_s.get("total_methods")
        if ok_c is not None and total_c is not None:
            cells.append(f"{ok_c:>3}/{total_c:<5}")
        else:
            cells.append(f"{'N/A':>10}")

        print(_render_row(cells))

    print(sep)


# ── compare ───────────────────────────────────────────────────────────────


def compare(
    slug: str,
    assembly: str,
    from_id: str,
    to_id: str,
    output: str = "table",
):
    """Compare two sessions from the history."""
    history_path = _get_history_path(assembly, slug)
    records = _load_history(history_path)

    if not records:
        print(f"No history records found at {history_path}", file=sys.stderr)
        sys.exit(1)

    from_rec: Optional[dict] = None
    to_rec: Optional[dict] = None
    for rec in records:
        sid = rec.get("session_id", "")
        if sid == from_id:
            from_rec = rec
        if sid == to_id:
            to_rec = rec

    if not from_rec:
        print(f"ERROR: session '{from_id}' not found in history", file=sys.stderr)
        sys.exit(1)
    if not to_rec:
        print(f"ERROR: session '{to_id}' not found in history", file=sys.stderr)
        sys.exit(1)

    # Build method lookup
    from_methods = {m["subject_id"]: m for m in from_rec.get("methods", [])}
    to_methods = {m["subject_id"]: m for m in to_rec.get("methods", [])}

    # Compare per-method
    changes: List[dict] = []
    for subj_id in sorted(set(from_methods) | set(to_methods)):
        fm = from_methods.get(subj_id)
        tm = to_methods.get(subj_id)

        if not fm or not tm:
            changes.append(
                {
                    "subject_id": subj_id,
                    "change": "removed" if not fm else "added",
                }
            )
            continue

        chaos_aot_delta = _delta(fm.get("chaos_aot_ns"), tm.get("chaos_aot_ns"))
        chaos_jit_delta = _delta(fm.get("chaos_jit_ns"), tm.get("chaos_jit_ns"))
        net8_delta = _delta(fm.get("net8_jit_ns"), tm.get("net8_jit_ns"))
        aot_ratio_delta = _delta(fm.get("aot_vs_net8_ratio"), tm.get("aot_vs_net8_ratio"))

        changes.append(
            {
                "subject_id": subj_id,
                "change": "changed",
                "chaos_aot_delta_ns": chaos_aot_delta,
                "chaos_jit_delta_ns": chaos_jit_delta,
                "net8_jit_delta_ns": net8_delta,
                "aot_vs_net8_ratio_delta": aot_ratio_delta,
            }
        )

    if output == "json":
        _print_compare_json(from_id, to_id, from_rec, to_rec, changes)
        return

    _print_compare_table(from_id, to_id, from_rec, to_rec, changes)


def _delta(a: Optional[float], b: Optional[float]) -> Optional[float]:
    """Return b - a if both are not None, otherwise None."""
    if a is not None and b is not None:
        return b - a
    return None


def _print_compare_json(
    from_id: str,
    to_id: str,
    from_rec: dict,
    to_rec: dict,
    changes: List[dict],
):
    """Output comparison in JSON format."""
    result = {
        "from_session": {
            "session_id": from_id,
            "timestamp": from_rec.get("timestamp"),
            "commit": from_rec.get("commit_hash"),
        },
        "to_session": {
            "session_id": to_id,
            "timestamp": to_rec.get("timestamp"),
            "commit": to_rec.get("commit_hash"),
        },
        "summaries": {
            "from": from_rec.get("summaries"),
            "to": to_rec.get("summaries"),
        },
        "changes": [c for c in changes if c["change"] == "changed"],
    }
    print(json.dumps(result, indent=2, ensure_ascii=False))


def _print_compare_table(
    from_id: str,
    to_id: str,
    from_rec: dict,
    to_rec: dict,
    changes: List[dict],
):
    """Print a human-readable comparison table."""
    from_ts = (from_rec.get("timestamp") or "?")[:19]
    to_ts = (to_rec.get("timestamp") or "?")[:19]
    from_commit = (from_rec.get("commit_hash") or "?")[:12]
    to_commit = (to_rec.get("commit_hash") or "?")[:12]

    print(
        f"\nBenchmark Comparison: {from_rec.get('assembly', '?')}"
        f"/{from_rec.get('slug', '?')}"
    )
    print(f"  From: {from_id}  ({from_ts}, commit {from_commit})")
    print(f"  To:   {to_id}  ({to_ts}, commit {to_commit})")

    # Summary-level comparison
    all_runtimes = set(from_rec.get("summaries", {}).keys()) | set(
        to_rec.get("summaries", {}).keys()
    )
    preferred = ["chaos-aot", "chaos-jit", "net8-jit", "net10-jit", "mono"]
    runtimes = [r for r in preferred if r in all_runtimes]

    print(f"\n{'Runtime':<20} {'From (ns)':<16} {'To (ns)':<16} {'Delta':<16}")
    print("-" * 68)

    for r in runtimes:
        fs = from_rec.get("summaries", {}).get(r, {})
        ts = to_rec.get("summaries", {}).get(r, {})
        f_gm = fs.get("geometric_mean_ns")
        t_gm = ts.get("geometric_mean_ns")

        if f_gm is not None and t_gm is not None:
            delta = t_gm - f_gm
            pct = (delta / f_gm) * 100 if f_gm > 0 else 0.0
            direction = "UP" if delta > 0 else ("DOWN" if delta < 0 else "SAME")
            print(
                f"{r:<20} {f_gm:>10.1f} ns     "
                f"{t_gm:>10.1f} ns     "
                f"{delta:>+9.1f} ns  {direction} ({pct:+.1f}%)"
            )
        else:
            print(
                f"{r:<20} {str(f_gm or 'N/A'):>16} {str(t_gm or 'N/A'):>16} {'N/A':>16}"
            )

    # Per-method changes (show top deltas)
    notable = sorted(
        [c for c in changes if c["change"] == "changed"],
        key=lambda c: abs(c.get("chaos_aot_delta_ns") or 0),
        reverse=True,
    )

    print(f"\nPer-Method Changes (sorted by |AOT delta|):")
    print(f"{'Subject':<60} {'AOT Δns':<16} {'JIT Δns':<16} {'n8 Δns':<16}")
    print("-" * 108)

    for c in notable:
        subj = c["subject_id"].split("::")[-1][:58]
        aot_delta = c.get("chaos_aot_delta_ns")
        jit_delta = c.get("chaos_jit_delta_ns")
        n8_delta = c.get("net8_jit_delta_ns")

        aot_str = f"{aot_delta:+.1f}" if aot_delta is not None else "N/A"
        jit_str = f"{jit_delta:+.1f}" if jit_delta is not None else "N/A"
        n8_str = f"{n8_delta:+.1f}" if n8_delta is not None else "N/A"
        print(f"{subj:<60} {aot_str:>16} {jit_str:>16} {n8_str:>16}")

    print()


# ── CLI ───────────────────────────────────────────────────────────────────


def main():
    parser = argparse.ArgumentParser(description="Cross-session benchmark history tracker")
    sub = parser.add_subparsers(dest="command", required=True)

    # record
    p_record = sub.add_parser("record", help="Record a benchmark snapshot")
    p_record.add_argument("slug", help="Family slug (e.g., enum-parsing)")
    p_record.add_argument(
        "--assembly", default="System.Private.CoreLib", help="Assembly name"
    )
    p_record.add_argument(
        "--session-id", help="Optional session ID (auto-generated if omitted)"
    )

    # trend
    p_trend = sub.add_parser("trend", help="Show benchmark trend over time")
    p_trend.add_argument("slug", help="Family slug")
    p_trend.add_argument("--assembly", default="System.Private.CoreLib")
    p_trend.add_argument(
        "--output", choices=["table", "json"], default="table"
    )
    p_trend.add_argument("--limit", type=int, help="Limit to last N records")

    # compare
    p_compare = sub.add_parser("compare", help="Compare two sessions")
    p_compare.add_argument("slug", help="Family slug")
    p_compare.add_argument(
        "--from", required=True, dest="from_id", help="Source session ID"
    )
    p_compare.add_argument(
        "--to", required=True, dest="to_id", help="Target session ID"
    )
    p_compare.add_argument("--assembly", default="System.Private.CoreLib")
    p_compare.add_argument(
        "--output", choices=["table", "json"], default="table"
    )

    args = parser.parse_args()

    if args.command == "record":
        record(args.slug, args.assembly, args.session_id)
    elif args.command == "trend":
        trend(args.slug, args.assembly, args.output, args.limit)
    elif args.command == "compare":
        compare(args.slug, args.assembly, args.from_id, args.to_id, args.output)


if __name__ == "__main__":
    main()
