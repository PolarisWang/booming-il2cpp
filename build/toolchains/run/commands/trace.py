"""Trace viewer: read and filter cross-language trace JSONL."""

from __future__ import annotations

import json
import os
import sys
from pathlib import Path
from typing import Any

try:
    from ..core.result import CommandResult
except ImportError:
    root = Path(__file__).resolve().parents[1]
    sys.path.insert(0, str(root))
    from core.result import CommandResult


ANSI_BOLD = "\x1b[1m"
ANSI_DIM = "\x1b[2m"
ANSI_CYAN = "\x1b[36m"
ANSI_GREEN = "\x1b[32m"
ANSI_YELLOW = "\x1b[33m"
ANSI_RED = "\x1b[31m"
ANSI_RESET = "\x1b[0m"
ANSI_BLUE = "\x1b[34m"
ANSI_MAGENTA = "\x1b[35m"


def _stage_color(stage: str) -> str:
    palette = {
        "derive": ANSI_GREEN,
        "generate": ANSI_CYAN,
        "codegen": ANSI_YELLOW,
        "audit": ANSI_BLUE,
        "kernel": ANSI_MAGENTA,
        "test": ANSI_GREEN,
        "benchmark": ANSI_YELLOW,
    }
    return palette.get(stage.split(":")[0], ANSI_DIM)


def _language_tag(lang: str) -> str:
    tags = {"python": "py", "cs": "cs", "cpp": "cpp"}
    return tags.get(lang, lang)


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


def _load_trace(path: Path, max_records: int = 0) -> list[dict[str, Any]]:
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
            if max_records and len(records) >= max_records:
                break
    return records


def _build_span_tree(records: list[dict[str, Any]]) -> list[dict[str, Any]]:
    """Build a tree from flat trace records using spanId/parentSpanId."""
    spans: dict[str, dict] = {}
    roots: list[dict] = []

    for r in records:
        span_id = r.get("spanId", "")
        if span_id:
            if span_id not in spans:
                spans[span_id] = {"records": [], "depth": 0}
            spans[span_id]["records"].append(r)
            spans[span_id]["span_id"] = span_id

    # Assign depth by following parentSpanId chain
    def resolve_depth(span_id: str, visited: set | None = None) -> int:
        if visited is None:
            visited = set()
        if span_id in visited:
            return 0
        visited.add(span_id)
        span = spans.get(span_id)
        if not span:
            return 0
        if span["depth"] > 0:
            return span["depth"]
        # Find parent from any record
        for rec in span["records"]:
            pid = rec.get("parentSpanId", "")
            if pid:
                span["depth"] = resolve_depth(pid, visited) + 1
                return span["depth"]
        return 0

    for sid in spans:
        spans[sid]["depth"] = resolve_depth(sid)

    return list(spans.values())


def _render_trace_tree(records: list[dict[str, Any]], *, show_source: bool = False) -> str:
    """Render trace records as a span-indented tree."""
    lines = []

    # Build span tree
    spans = _build_span_tree(records)

    # If we have spans, render by span tree; otherwise fall back to flat
    if spans:
        # Sort spans by first record's timestamp, then by depth
        def _first_ts(s: dict) -> str:
            return s["records"][0].get("t", "") if s["records"] else ""
        spans.sort(key=lambda s: (_first_ts(s), s["depth"]))

        for span in spans:
            depth = span["depth"]
            prefix = "  " * depth

            for i, r in enumerate(span["records"]):
                op = r.get("o", "?")
                stage = r.get("s", "")
                lang = r.get("l", "?")
                timestamp = r.get("t", "")[11:19]
                phase = r.get("phase", "")
                span_id = r.get("spanId", "")

                stage_color = _stage_color(stage)
                lang_tag = _language_tag(lang)
                time_str = f"{ANSI_DIM}{timestamp}{ANSI_RESET}"
                stage_str = f"{stage_color}{stage}{ANSI_RESET}" if stage else ""
                lang_str = f"{ANSI_DIM}[{lang_tag}]{ANSI_RESET}"

                # Phase indicator
                if phase == "enter":
                    phase_char = "▶"
                elif phase == "exit":
                    phase_char = "✓"
                    if r.get("dur_ms"):
                        phase_char = f"✓ {r['dur_ms']}ms"
                elif phase == "exception":
                    phase_char = "✗"
                else:
                    phase_char = "·"

                # Show spanId in dim for enter records
                span_info = f" {ANSI_DIM}[{span_id}]{ANSI_RESET}" if phase == "enter" and span_id else ""

                line = f"{prefix}{time_str} {lang_str} {phase_char}{span_info} "
                if stage:
                    line += f"{stage_str}:"
                line += f"{ANSI_BOLD}{op}{ANSI_RESET}"

                # Key-value pairs
                skip_keys = {"t", "l", "s", "o", "f", "phase", "dur_ms",
                             "exception", "message", "spanId", "parentSpanId"}
                pairs = []
                for k, v in r.items():
                    if k in skip_keys:
                        continue
                    if isinstance(v, str):
                        pairs.append(f"{k}={v}")
                    else:
                        pairs.append(f"{k}={v}")
                if pairs:
                    line += f" {ANSI_DIM}{'  '.join(pairs)}{ANSI_RESET}"

                # Exception detail
                if phase == "exception":
                    exc = r.get("exception", "")
                    msg = r.get("message", "")
                    line += f"\n{prefix}  {ANSI_RED}✗ {exc}: {msg}{ANSI_RESET}"

                # Source file
                if show_source and r.get("f"):
                    source = r["f"]
                    parts = source.replace("\\", "/").split("/")
                    short_source = "/".join(parts[-3:]) if len(parts) > 3 else source
                    line += f"\n{prefix}  {ANSI_DIM}└─ {short_source}{ANSI_RESET}"

                lines.append(line)
    else:
        # Flat rendering fallback (no span IDs)
        for r in records:
            op = r.get("o", "?")
            stage = r.get("s", "")
            lang = r.get("l", "?")
            timestamp = r.get("t", "")[11:19]
            phase = r.get("phase", "")

            stage_color = _stage_color(stage)
            lang_tag = _language_tag(lang)
            time_str = f"{ANSI_DIM}{timestamp}{ANSI_RESET}"
            stage_str = f"{stage_color}{stage}{ANSI_RESET}" if stage else ""
            lang_str = f"{ANSI_DIM}[{lang_tag}]{ANSI_RESET}"

            if phase == "enter":
                phase_char = "▶"
            elif phase == "exit":
                phase_char = "✓"
                if r.get("dur_ms"):
                    phase_char = f"✓ {r['dur_ms']}ms"
            elif phase == "exception":
                phase_char = "✗"
            else:
                phase_char = "·"

            line = f"{time_str} {lang_str} {phase_char} "
            if stage:
                line += f"{stage_str}:"
            line += f"{ANSI_BOLD}{op}{ANSI_RESET}"

            skip_keys = {"t", "l", "s", "o", "f", "phase", "dur_ms",
                         "exception", "message", "spanId", "parentSpanId"}
            pairs = []
            for k, v in r.items():
                if k in skip_keys:
                    continue
                if isinstance(v, str):
                    pairs.append(f"{k}={v}")
                else:
                    pairs.append(f"{k}={v}")
            if pairs:
                line += f" {ANSI_DIM}{'  '.join(pairs)}{ANSI_RESET}"

            if phase == "exception":
                exc = r.get("exception", "")
                msg = r.get("message", "")
                line += f"\n  {ANSI_RED}✗ {exc}: {msg}{ANSI_RESET}"

            lines.append(line)

    return "\n".join(lines)


def _render_session_list(sessions: list[dict[str, Any]]) -> str:
    lines = [f"Trace sessions ({len(sessions)}):", ""]
    for i, s in enumerate(sessions):
        from datetime import datetime
        dt = datetime.fromtimestamp(s["mtime"]).strftime("%Y-%m-%d %H:%M:%S")
        lines.append(f"  [{i}] {s['session_id']}  ({dt})")
    lines.append("")
    lines.append("Use: run trace [--session N] [--stage STAGE] [--op OP] [--family FAMILY]")
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
            target="trace",
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
                target="trace",
                errors=[f"invalid session index: {session_idx}"],
            )
    else:
        session = sessions[0]  # latest

    # Load records
    max_records = int(opts.get("max_records", 500) or 500)
    all_records = _load_trace(session["trace_path"], max_records=max_records)
    if not all_records:
        return CommandResult.success(
            command=command_text,
            host_platform=host_platform,
            target="trace",
            payload={"records": 0},
            text=f"Session {session['session_id']}: 0 records.\n",
        )

    # Filter
    filtered = all_records
    stage_filter = opts.get("stage")
    if stage_filter:
        filtered = [r for r in filtered if stage_filter in r.get("s", "")]
    op_filter = opts.get("op")
    if op_filter:
        filtered = [r for r in filtered if op_filter in r.get("o", "")]
    family_filter = opts.get("family")
    if family_filter:
        filtered = [r for r in filtered if family_filter in str(r.values())]
    lang_filter = opts.get("lang")
    if lang_filter:
        filtered = [r for r in filtered if r.get("l") == lang_filter]
    trace_id_filter = opts.get("trace_id")
    if trace_id_filter:
        filtered = [r for r in filtered if trace_id_filter in r.get("traceId", "")]

    # Output
    if opts.get("json"):
        text = "\n".join(json.dumps(r, ensure_ascii=False) for r in filtered) + "\n"
    else:
        show_source = opts.get("source", False)
        tree = _render_trace_tree(filtered, show_source=show_source)
        trace_id_filter = opts.get("trace_id")
        summary = (
            f"Trace: {session['session_id']}  "
            f"({len(filtered)}/{len(all_records)} records"
            f"{f', stage={stage_filter}' if stage_filter else ''}"
            f"{f', op={op_filter}' if op_filter else ''}"
            f"{f', traceId={trace_id_filter}' if trace_id_filter else ''}"
            f")\n"
        )
        text = summary + "\n" + tree + "\n"

    return CommandResult.success(
        command=command_text,
        host_platform=host_platform,
        target="trace",
        payload={
            "session_id": session["session_id"],
            "total_records": len(all_records),
            "filtered_records": len(filtered),
            "records": filtered,
        },
        text=text,
    )
