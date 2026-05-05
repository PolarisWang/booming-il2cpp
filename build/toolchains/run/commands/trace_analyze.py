"""Trace analyzer: read trace JSONL and produce timing breakdowns.

Usage:
  run trace-analyze                         — Latest session summary
  run trace-analyze --session 0              — By session index
  run trace-analyze --stage batch-native-aot — Filter by stage
  run trace-analyze --json                   — Machine-readable JSON output

Output:
  - Total duration per stage/subsystem
  - Per-operation timing (avg, min, max, count)
  - Language breakdown (python/cs/cpp)
  - Slowest operations
  - Span tree depth statistics
"""

from __future__ import annotations

import json
import os
import sys
from collections import defaultdict
from pathlib import Path
from typing import Any

try:
    from ..core.result import CommandResult
except ImportError:
    root = Path(__file__).resolve().parents[1]
    sys.path.insert(0, str(root))
    from core.result import CommandResult


def _find_trace_sessions(repo_root: Path) -> list[dict[str, Any]]:
    trace_root = repo_root / "artifacts" / "logs" / "trace"
    if not trace_root.is_dir():
        return []
    sessions = []
    for entry in sorted(trace_root.iterdir(), reverse=True):
        trace_file = entry / "trace.jsonl"
        if trace_file.is_file():
            sessions.append({
                "session_id": entry.name,
                "trace_path": trace_file,
                "mtime": trace_file.stat().st_mtime,
            })
    return sessions


def _load_trace(path: Path) -> list[dict[str, Any]]:
    records = []
    with path.open("r", encoding="utf-8") as f:
        for line in f:
            stripped = line.strip()
            if not stripped:
                continue
            try:
                records.append(json.loads(stripped))
            except json.JSONDecodeError:
                pass
    return records


def _calculate_timing(records: list[dict[str, Any]]) -> dict[str, Any]:
    """Produce timing breakdown from a list of trace records."""

    # ── By stage ──
    stage_dur: dict[str, float] = defaultdict(float)
    stage_counts: dict[str, int] = defaultdict(int)

    # ── By operation ──
    op_stats: dict[str, dict] = defaultdict(lambda: {
        "count": 0, "total_ms": 0.0, "min_ms": float("inf"), "max_ms": 0.0
    })

    # ── By language ──
    lang_counts: dict[str, int] = defaultdict(int)

    # ── Span depth ──
    span_depths: list[int] = []
    spans: dict[str, int] = {}  # spanId → depth

    def _resolve_depth(span_id: str, recs: list, visited: set | None = None) -> int:
        if visited is None:
            visited = set()
        if span_id in visited:
            return 0
        visited.add(span_id)
        if span_id in spans:
            return spans[span_id]
        for r in recs:
            if r.get("spanId") == span_id:
                pid = r.get("parentSpanId", "")
                if pid:
                    d = _resolve_depth(pid, recs, visited) + 1
                    spans[span_id] = d
                    return d
        return 0

    for r in records:
        stage = r.get("s", "(none)")
        op = r.get("o", "?")
        lang = r.get("l", "?")
        dur = r.get("dur_ms")

        lang_counts[lang] += 1

        if dur is not None:
            stage_dur[stage] += float(dur)
            stage_counts[stage] += 1

            s = op_stats[op]
            s["count"] += 1
            s["total_ms"] += float(dur)
            s["min_ms"] = min(s["min_ms"], float(dur))
            s["max_ms"] = max(s["max_ms"], float(dur))

    # Span depth analysis
    for r in records:
        sid = r.get("spanId", "")
        if sid and sid not in spans:
            spans[sid] = _resolve_depth(sid, records)
    span_depths = list(spans.values())

    # Finalize op_stats: compute average
    op_summary = {}
    for op, s in sorted(op_stats.items()):
        op_summary[op] = {
            "count": s["count"],
            "total_ms": round(s["total_ms"], 1),
            "avg_ms": round(s["total_ms"] / s["count"], 1) if s["count"] else 0,
            "min_ms": round(s["min_ms"], 1) if s["min_ms"] != float("inf") else 0,
            "max_ms": round(s["max_ms"], 1),
        }

    # Top 10 slowest operations by total time
    sorted_ops = sorted(op_summary.items(), key=lambda x: x[1]["total_ms"], reverse=True)
    top_slowest = sorted_ops[:10]

    return {
        "total_records": len(records),
        "stage_breakdown": {
            stage: {
                "total_ms": round(stage_dur[stage], 1),
                "records": stage_counts[stage],
            }
            for stage in sorted(stage_dur.keys())
        },
        "language_breakdown": dict(lang_counts),
        "operation_stats": op_summary,
        "top_slowest_operations": [
            {"operation": op, **s} for op, s in top_slowest
        ],
        "span_depth": {
            "max": max(span_depths) if span_depths else 0,
            "avg": round(sum(span_depths) / len(span_depths), 1) if span_depths else 0,
            "total_spans": len(span_depths),
        },
    }


def _render_timing_report(stats: dict[str, Any], session_id: str) -> str:
    lines = []
    lines.append(f"Trace Analysis: {session_id}")
    lines.append(f"Total records: {stats['total_records']}")
    lines.append("")

    # Language breakdown
    lines.append("-- Language Distribution --")
    for lang, count in sorted(stats["language_breakdown"].items()):
        pct = count / stats["total_records"] * 100 if stats["total_records"] else 0
        lines.append(f"  {lang:8s}  {count:4d} records ({pct:.0f}%)")
    lines.append("")

    # Stage timing
    if stats["stage_breakdown"]:
        lines.append("-- Stage Timing --")
        total = sum(s["total_ms"] for s in stats["stage_breakdown"].values())
        for stage, s in sorted(stats["stage_breakdown"].items(), key=lambda x: -x[1]["total_ms"]):
            pct = s["total_ms"] / total * 100 if total else 0
            bar = "#" * int(pct / 5) + "." * (20 - int(pct / 5))
            lines.append(f"  {stage:30s} {bar} {s['total_ms']:8.1f}ms ({pct:.0f}%)")
        lines.append(f"  {'TOTAL':30s} {'':20s} {total:8.1f}ms")
        lines.append("")

    # Top slowest operations
    if stats["top_slowest_operations"]:
        lines.append("-- Slowest Operations --")
        lines.append(f"  {'Operation':40s} {'Count':>6s} {'Total':>8s} {'Avg':>8s} {'Max':>8s}")
        lines.append(f"  {'-'*40}  {'-'*5}  {'-'*7}  {'-'*7}  {'-'*7}")
        for op in stats["top_slowest_operations"]:
            lines.append(
                f"  {op['operation']:40s} {op['count']:5d}  "
                f"{op['total_ms']:7.1f} {op['avg_ms']:7.1f} {op['max_ms']:7.1f}"
            )
        lines.append("")

    # Span depth
    sd = stats["span_depth"]
    if sd["total_spans"] > 0:
        lines.append("-- Span Hierarchy --")
        lines.append(f"  Max depth: {sd['max']}  Avg depth: {sd['avg']}  Total spans: {sd['total_spans']}")
        lines.append("")

    return "\n".join(lines)


def handle(
    repo_root: Path,
    host_platform: str,
    command_text: str,
    options: dict[str, Any] | None = None,
) -> CommandResult:
    opts = options or {}

    sessions = _find_trace_sessions(repo_root)
    if not sessions:
        return CommandResult.success(
            command=command_text,
            host_platform=host_platform,
            target="trace-analyze",
            payload={"records": 0},
            text="No trace sessions found in artifacts/logs/trace/.\n",
        )

    # Session selection
    session_idx = opts.get("session")
    if session_idx is not None:
        try:
            session = sessions[int(session_idx)]
        except (ValueError, IndexError):
            return CommandResult.failure(
                command=command_text,
                host_platform=host_platform,
                target="trace-analyze",
                errors=[f"invalid session index: {session_idx}"],
            )
    else:
        session = sessions[0]  # latest

    records = _load_trace(session["trace_path"])
    if not records:
        return CommandResult.success(
            command=command_text,
            host_platform=host_platform,
            target="trace-analyze",
            payload={"records": 0},
            text=f"Session {session['session_id']}: 0 records.\n",
        )

    # Filter
    stage_filter = opts.get("stage")
    if stage_filter:
        records = [r for r in records if stage_filter in r.get("s", "")]
    op_filter = opts.get("op")
    if op_filter:
        records = [r for r in records if op_filter in r.get("o", "")]
    lang_filter = opts.get("lang")
    if lang_filter:
        records = [r for r in records if r.get("l") == lang_filter]

    stats = _calculate_timing(records)

    if opts.get("json"):
        text = json.dumps(stats, indent=2, ensure_ascii=False)
    else:
        text = _render_timing_report(stats, session["session_id"])

    return CommandResult.success(
        command=command_text,
        host_platform=host_platform,
        target="trace-analyze",
        payload=stats,
        text=text,
    )
