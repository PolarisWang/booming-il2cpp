#!/usr/bin/env python3
"""Health Metrics Engine: computes per-skill health metrics from raw telemetry.

Subcommands:
  compute --all              Compute health snapshot for all skills
  compute --skill <name>     Compute health for one skill
  trend --skill <name>       Show health trend for a skill
  trend --all                Show trends for all skills
  report                     Generate health dashboard (stdout)

Metrics computed per skill:
  applied_rate         = reads_in_window / total_sessions_in_window
  completion_rate      = completed_sessions / total_sessions_using_skill
  tool_success_rate    = successful_tool_calls / total_tool_calls
  fallback_rate        = sessions_with_other_skill_after / total_sessions_using_skill
  quality_score_avg    = avg quality score from signals
"""

from __future__ import annotations

import argparse
import json
import subprocess
import sys
from collections import defaultdict
from datetime import datetime, timezone
from pathlib import Path
from typing import Any


# ── Threshold constants ──────────────────────────────────────────────────────
APPLIED_RATE_LOW = 0.4
COMPLETION_RATE_LOW = 0.6
TOOL_SUCCESS_LOW = 0.7
CONTEXT_SWITCH_HIGH = 0.5
QUALITY_HIGH = 0.8
FALLBACK_RATE_HIGH = 0.3

DEFAULT_WINDOW_DAYS = 30


# ── Helpers ──────────────────────────────────────────────────────────────────

def resolve_repo_root() -> Path | None:
    try:
        script_dir = Path(__file__).resolve().parent
        # Walk up to find skills/tooling/learning/ -> skills/ -> repo root
        for parent in [script_dir, script_dir.parent, script_dir.parent.parent]:
            git_root = subprocess.run(
                ["git", "-C", str(parent), "rev-parse", "--show-toplevel"],
                capture_output=True, text=True, timeout=10,
            )
            root = git_root.stdout.strip()
            if root:
                return Path(root).resolve()
        return None
    except Exception:
        return None


def load_jsonl(path: Path) -> list[dict]:
    if not path.exists() or path.stat().st_size == 0:
        return []
    records = []
    try:
        for line in path.read_text(encoding="utf-8").strip().splitlines():
            if line.strip():
                records.append(json.loads(line))
    except (json.JSONDecodeError, OSError):
        return []
    return records


def in_window(ts: str, window_days: int) -> bool:
    """Check if an ISO timestamp falls within the window."""
    try:
        dt = datetime.fromisoformat(ts)
        if dt.tzinfo is None:
            dt = dt.replace(tzinfo=timezone.utc)
        now = datetime.now(timezone.utc)
        delta = now - dt
        return delta.days < window_days
    except (ValueError, TypeError):
        return False


def slugify(name: str) -> str:
    return name.replace("\\", "/").split("/")[-1].replace("SKILL.md", "").strip("/")


def skill_name_from_path(path: str) -> str:
    """Extract skill name from a SKILL.md path like .../dev-brainstorm/SKILL.md"""
    normalized = path.replace("\\", "/")
    parts = normalized.split("/")
    for i, part in enumerate(parts):
        if part == "skills" and i + 2 < len(parts) and parts[i + 1] == "library" and parts[i + 2] == "skills":
            if i + 3 < len(parts):
                return parts[i + 3]
    # Fallback: try to find dev- prefix
    for part in parts:
        if part.startswith("dev-") or part.startswith("skill-"):
            return part
    return slugify(path)


# ── Health Computation ──────────────────────────────────────────────────────

def compute_skill_health(
    skill_name: str,
    usage_records: list[dict],
    tool_records: list[dict],
    session_records: list[dict],
    signal_records: list[dict],
    window_days: int,
) -> dict[str, Any]:
    """Compute health metrics for a single skill."""
    # Filter records in window and for this skill
    usage = [r for r in usage_records
             if in_window(r.get("timestamp", ""), window_days)
             and skill_name_from_path(r.get("skill_path", "")) == skill_name]
    tools = [r for r in tool_records
             if in_window(r.get("timestamp", ""), window_days)
             and skill_name_from_path(r.get("skill_path", "") or "") == skill_name]
    sessions = [r for r in session_records
                if in_window(r.get("timestamp", ""), window_days)
                and skill_name_from_path(r.get("skill_path", "") or "") == skill_name]
    signals = [r for r in signal_records
               if in_window(r.get("timestamp", ""), window_days)
               and slugify(r.get("domain_hint", "")) == skill_name.replace("dev-", "")[:20]]

    total_usage = len(usage)
    total_tools = len(tools)
    total_sessions = len(sessions)
    total_signals = len(signals)

    # applied_rate: reads relative to total sessions using this skill
    applied_rate = min(1.0, total_usage / max(total_sessions, 1))

    # completion_rate
    completed = sum(1 for s in sessions if s.get("completed", False))
    completion_rate = completed / max(total_sessions, 1)

    # tool_success_rate
    successful = sum(1 for t in tools if t.get("success", True))
    tool_success_rate = successful / max(total_tools, 1) if total_tools > 0 else 1.0

    # fallback_rate: sessions where user switched to another skill afterward
    fallback_rate = 0.0
    if total_sessions > 0:
        # Heuristic: if a session record exists but completion is False
        # AND there's no recent commit, treat as fallback
        fallbacks = sum(1 for s in sessions
                        if not s.get("completed", False)
                        and not s.get("signals", {}).get("has_recent_commit", False))
        fallback_rate = fallbacks / total_sessions

    # quality_score_avg
    quality_scores = [s.get("quality_score", 0.0) for s in signals if s.get("quality_score") is not None]
    quality_avg = sum(quality_scores) / max(len(quality_scores), 1)

    # context_switching_ratio (proxy: usage spread across multiple days)
    unique_days = len(set(
        r.get("timestamp", "")[:10] for r in usage
    ))
    context_switching_ratio = min(1.0, unique_days / max(window_days, 1) * 10)

    return {
        "skill": skill_name,
        "window_days": window_days,
        "metrics": {
            "applied_rate": round(applied_rate, 4),
            "completion_rate": round(completion_rate, 4),
            "tool_success_rate": round(tool_success_rate, 4),
            "fallback_rate": round(fallback_rate, 4),
            "quality_score_avg": round(quality_avg, 4),
            "context_switching_ratio": round(context_switching_ratio, 4),
        },
        "raw_counts": {
            "usage_reads": total_usage,
            "tool_calls": total_tools,
            "sessions": total_sessions,
            "quality_signals": total_signals,
            "completed_sessions": completed,
            "successful_tool_calls": successful,
        },
        "timestamp": datetime.now(timezone.utc).isoformat(),
    }


def compute_all_health(
    repo_root: Path,
    window_days: int = DEFAULT_WINDOW_DAYS,
    skill_filter: str | None = None,
) -> dict[str, Any]:
    """Load all telemetry and compute health for every skill."""
    telemetry_dir = repo_root / "skills" / "lifecycle" / "telemetry"
    signals_dir = repo_root / "skills" / "lifecycle" / "learning" / "signals"

    usage_records = load_jsonl(telemetry_dir / "usage.jsonl")
    tool_records = load_jsonl(telemetry_dir / "tool_outcomes.jsonl")
    session_records = load_jsonl(telemetry_dir / "session_outcomes.jsonl")

    signal_records = []
    if signals_dir.exists():
        for f in sorted(signals_dir.glob("*.jsonl")):
            signal_records.extend(load_jsonl(f))

    # Collect all skill names from telemetry + library
    skills_in_telemetry: set[str] = set()
    for r in usage_records:
        name = skill_name_from_path(r.get("skill_path", ""))
        if name:
            skills_in_telemetry.add(name)
    for r in tool_records:
        name = skill_name_from_path(r.get("skill_path", "") or "")
        if name:
            skills_in_telemetry.add(name)

    # Also scan library for all known skills
    library_dir = repo_root / "skills" / "library" / "skills"
    all_skill_names = set(skills_in_telemetry)
    if library_dir.exists():
        for d in library_dir.iterdir():
            if d.is_dir():
                all_skill_names.add(d.name)

    # Compute per-skill health
    results = {}
    for name in sorted(all_skill_names):
        if skill_filter and name != skill_filter:
            continue
        health = compute_skill_health(
            name, usage_records, tool_records,
            session_records, signal_records, window_days,
        )
        results[name] = health

    snapshot = {
        "timestamp": datetime.now(timezone.utc).isoformat(),
        "window_days": window_days,
        "total_skills": len(results),
        "skills": results,
    }
    return snapshot


# ── Trend Computation ───────────────────────────────────────────────────────

def compute_trend(repo_root: Path, skill_name: str | None = None) -> list[dict]:
    """Read historical health snapshots and return trend data."""
    health_dir = repo_root / "skills" / "lifecycle" / "telemetry" / "health"
    if not health_dir.exists():
        return []

    snapshots = sorted(health_dir.glob("health-snapshot-*.json"))
    trends = []
    for snap_path in snapshots:
        try:
            snap = json.loads(snap_path.read_text(encoding="utf-8"))
            ts = snap.get("timestamp", snap_path.name)
            skills_data = snap.get("skills", {})
            if skill_name:
                if skill_name in skills_data:
                    entry = skills_data[skill_name]
                    entry["_snapshot_date"] = ts
                    trends.append(entry)
            else:
                # Return aggregate: avg of key metrics across all skills
                metrics_list = [s.get("metrics", {}) for s in skills_data.values()]
                if not metrics_list:
                    continue
                avg_metrics = {}
                for key in ("applied_rate", "completion_rate", "tool_success_rate", "fallback_rate"):
                    vals = [m.get(key, 0.0) for m in metrics_list if m.get(key) is not None]
                    avg_metrics[key] = round(sum(vals) / max(len(vals), 1), 4)
                trends.append({
                    "_snapshot_date": ts,
                    "metrics_avg": avg_metrics,
                    "total_skills": len(skills_data),
                })
        except (json.JSONDecodeError, OSError):
            continue

    return trends


# ── Report Generation ───────────────────────────────────────────────────────

def generate_report(snapshot: dict) -> str:
    """Generate a human-readable health dashboard."""
    lines = ["# Skill Health Dashboard", ""]
    lines.append(f"Snapshot: {snapshot['timestamp']}")
    lines.append(f"Window: {snapshot['window_days']} days")
    lines.append(f"Total skills tracked: {snapshot['total_skills']}")
    lines.append("")

    skills = snapshot.get("skills", {})
    # Sort by health: skills needing attention first
    def health_score(item):
        m = item[1].get("metrics", {})
        return m.get("completion_rate", 1.0) + m.get("tool_success_rate", 1.0)

    sorted_skills = sorted(skills.items(), key=health_score)

    lines.append("## Per-Skill Metrics")
    lines.append("")
    header = f"{'Skill':<35} {'Applied':>8} {'Complete':>9} {'ToolOK':>7} {'Fallback':>9} {'Quality':>8} {'Switch':>8}"
    lines.append(header)
    lines.append("-" * len(header))

    for name, data in sorted_skills:
        m = data.get("metrics", {})
        lines.append(
            f"{name:<35} "
            f"{m.get('applied_rate', 0):>8.2f} "
            f"{m.get('completion_rate', 0):>9.2f} "
            f"{m.get('tool_success_rate', 0):>7.2f} "
            f"{m.get('fallback_rate', 0):>9.2f} "
            f"{m.get('quality_score_avg', 0):>8.2f} "
            f"{m.get('context_switching_ratio', 0):>8.2f}"
        )

    lines.append("")
    lines.append("## Skills Needing Attention")
    lines.append("")

    needs_fix = []
    needs_retire = []
    needs_derive = []
    for name, data in sorted_skills:
        m = data.get("metrics", {})
        raw = data.get("raw_counts", {})
        flags = []

        if m.get("completion_rate", 1.0) < COMPLETION_RATE_LOW and raw.get("sessions", 0) >= 3:
            flags.append("FIX")
        if m.get("applied_rate", 1.0) < APPLIED_RATE_LOW and raw.get("sessions", 0) >= 5:
            flags.append("LOW-USE")
        if m.get("context_switching_ratio", 0) > CONTEXT_SWITCH_HIGH and raw.get("sessions", 0) >= 10:
            flags.append("DERIVE")
        if m.get("fallback_rate", 0) > FALLBACK_RATE_HIGH and raw.get("sessions", 0) >= 3:
            flags.append("HIGH-FALLBACK")
        if m.get("tool_success_rate", 1.0) < TOOL_SUCCESS_LOW and raw.get("tool_calls", 0) >= 5:
            flags.append("LOW-TOOL")

        if flags:
            lines.append(f"- **{name}**: {', '.join(flags)}")
            needs_fix.append(name)

    if needs_fix:
        lines.append("")
        lines.append("### FIX Candidates (completion_rate < 0.6)")
        for name in needs_fix:
            m = skills[name].get("metrics", {})
            lines.append(f"- {name}: completion={m.get('completion_rate', 0):.2f}, sessions={skills[name].get('raw_counts', {}).get('sessions', 0)}")

    lines.append("")
    lines.append("---")
    lines.append(f"Generated by health_engine.py at {datetime.now(timezone.utc).isoformat()}")

    return "\n".join(lines)


# ── CLI ──────────────────────────────────────────────────────────────────────

def build_parser() -> argparse.ArgumentParser:
    parser = argparse.ArgumentParser(description="Skill Health Metrics Engine")
    sub = parser.add_subparsers(dest="command", required=True)

    # compute
    p_compute = sub.add_parser("compute", help="Compute health snapshot")
    p_compute.add_argument("--all", action="store_true", help="Compute for all skills")
    p_compute.add_argument("--skill", type=str, default=None, help="Skill name to compute")
    p_compute.add_argument("--window", type=int, default=DEFAULT_WINDOW_DAYS, help="Window in days")
    p_compute.add_argument("--output", type=str, default=None, help="Output file (default: health/ dir)")

    # trend
    p_trend = sub.add_parser("trend", help="Show health trend")
    p_trend.add_argument("--skill", type=str, default=None, help="Skill name for trend")
    p_trend.add_argument("--all", action="store_true", help="Show aggregate trend")

    # report
    p_report = sub.add_parser("report", help="Generate health dashboard")
    p_report.add_argument("--window", type=int, default=DEFAULT_WINDOW_DAYS, help="Window in days")

    # check (one-shot: compute + report + propose)
    p_check = sub.add_parser("check", help="One-shot health check: compute + report + propose")
    p_check.add_argument("--window", type=int, default=DEFAULT_WINDOW_DAYS, help="Window in days")

    return parser


def main() -> int:
    parser = build_parser()
    args = parser.parse_args()

    repo_root = resolve_repo_root()
    if not repo_root:
        print("Error: not in a git repository", file=sys.stderr)
        return 1

    if args.command == "compute":
        if not args.all and not args.skill:
            print("Error: specify --all or --skill", file=sys.stderr)
            return 1
        snapshot = compute_all_health(repo_root, args.window, args.skill)

        # Write to health directory
        health_dir = repo_root / "skills" / "lifecycle" / "telemetry" / "health"
        health_dir.mkdir(parents=True, exist_ok=True)
        date_str = datetime.now(timezone.utc).strftime("%Y%m%d")
        out_path = args.output or (health_dir / f"health-snapshot-{date_str}.json")
        if isinstance(out_path, str):
            out_path = Path(out_path)

        out_path.write_text(
            json.dumps(snapshot, indent=2, ensure_ascii=False),
            encoding="utf-8",
        )
        print(f"Health snapshot written: {out_path}")
        if args.skill:
            s = snapshot["skills"].get(args.skill, {})
            print(json.dumps(s, indent=2, ensure_ascii=False))

    elif args.command == "trend":
        trends = compute_trend(repo_root, args.skill if not args.all else None)
        if not trends:
            print("No trend data available (no health snapshots found)")
            return 0
        for t in trends[-10:]:  # Last 10
            print(f"[{t.get('_snapshot_date', '?')[:19]}] ", end="")
            if "metrics" in t:
                m = t["metrics"]
                print(f"applied={m.get('applied_rate', '?'):.2f} complete={m.get('completion_rate', '?'):.2f} tool_ok={m.get('tool_success_rate', '?'):.2f}")
            elif "metrics_avg" in t:
                m = t["metrics_avg"]
                print(f"(avg) applied={m.get('applied_rate', '?'):.2f} complete={m.get('completion_rate', '?'):.2f} skills={t.get('total_skills', '?')}")
            else:
                print(json.dumps(t, ensure_ascii=False)[:120])

    elif args.command == "report":
        snapshot = compute_all_health(repo_root, args.window)
        report = generate_report(snapshot)
        print(report)

    elif args.command == "check":
        # Step 1: compute
        snapshot = compute_all_health(repo_root, args.window)
        health_dir = repo_root / "skills" / "lifecycle" / "telemetry" / "health"
        health_dir.mkdir(parents=True, exist_ok=True)
        date_str = datetime.now(timezone.utc).strftime("%Y%m%d")
        out_path = health_dir / f"health-snapshot-{date_str}.json"
        out_path.write_text(json.dumps(snapshot, indent=2, ensure_ascii=False), encoding="utf-8")
        print(f"[check] Health snapshot written: {out_path}")

        # Step 2: report
        print("\n" + "=" * 70)
        report = generate_report(snapshot)
        print(report)

        # Step 3: propose
        print("=" * 70)
        evolve_script = repo_root / "skills" / "tooling" / "learning" / "evolve.py"
        if evolve_script.exists():
            print("[check] Checking evolution proposals...\n")
            result = subprocess.run(
                [sys.executable, str(evolve_script), "propose"],
                cwd=str(repo_root),
                capture_output=True, text=True, timeout=60,
            )
            if result.stdout:
                print(result.stdout)
            if result.stderr:
                print(result.stderr, file=sys.stderr)
        else:
            print("[check] evolve.py not found, skipping proposal check")

        print("[check] Health check complete.")

    return 0


if __name__ == "__main__":
    raise SystemExit(main())
