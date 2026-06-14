"""Nightly delta — cross-day comparison for foundation-dll verification.

Reads today's latest results + yesterday's history, computes per-chunk deltas
for fact pass rate, benchmark performance, coverage, and hotupdate.

Usage:
    python -m verification.nightly_delta --assembly System.Collections \\
        --date 2026-06-14 --history-dir _dll/reports/history \\
        --latest-dir _dll/reports/latest --output nightly-delta.json

    # Batch mode: scan all assemblies under testing/foundation-dll/
    python -m verification.nightly_delta --all --output nightly-delta.json
"""

from __future__ import annotations

import json
import os
import re
import sys
from datetime import date, datetime, timedelta, timezone
from pathlib import Path
from typing import Any


# ── Data loading ──────────────────────────────────────────────────────────


def _load_json(path: Path) -> dict[str, Any] | None:
    """Load a JSON file, returning None on any failure."""
    try:
        return json.loads(path.read_text(encoding="utf-8"))
    except (OSError, json.JSONDecodeError):
        return None


def _find_last_history(dll_reports: Path, prefix: str, before_date: str) -> Path | None:
    """Find the most recent history file before a given date.

    Scans dll_reports/history/ for files matching {prefix}-YYYY-MM-DD.json
    where the date < before_date. Returns the closest (latest) one.
    """
    history_dir = dll_reports / "history"
    if not history_dir.is_dir():
        return None

    candidates: list[tuple[str, Path]] = []
    pattern = re.compile(rf"^{re.escape(prefix)}-(\d{{4}}-\d{{2}}-\d{{2}})\.json$")
    for f in history_dir.iterdir():
        m = pattern.match(f.name)
        if m and m.group(1) < before_date:
            candidates.append((m.group(1), f))

    if not candidates:
        return None
    candidates.sort(key=lambda x: x[0], reverse=True)
    return candidates[0][1]


# ── Per-assembly delta ─────────────────────────────────────────────────────


def _chunk_key(assembly: str, slug: str) -> str:
    return f"{assembly}/{slug}"


def compute_assembly_delta(
    assembly: str,
    foundation_dir: Path,
    today: str,
) -> dict[str, Any]:
    """Compute delta for a single assembly between today and last history."""
    dll_reports = foundation_dir / "_dll" / "reports"
    latest_dir = dll_reports / "latest"
    history_dir = dll_reports / "history"

    # ── Load today's data ──
    today_dash = _load_json(latest_dir / "dashboard.json") or {}
    today_fact = _load_json(latest_dir / "fact-summary.json") or {}
    today_coverage = _load_json(latest_dir / "coverage-audit.json") or {}
    today_summary = today_dash.get("summary", {})

    today_chunks: dict[str, dict] = {}
    for cs in today_fact.get("chunkSummaries", []):
        slug = cs.get("slug", "?")
        today_chunks[slug] = cs

    # ── Find previous history ──
    prev_path = _find_last_history(dll_reports, "benchmark", today)
    prev_data = _load_json(prev_path) if prev_path else None
    prev_chunks: dict[str, dict] = {}
    prev_date: str | None = None
    if prev_data:
        prev_date = prev_data.get("date")
        for cs in prev_data.get("chunks", []):
            prev_chunks[cs.get("slug", "?")] = cs

    # ── Per-chunk delta ──
    per_chunk: dict[str, dict[str, Any]] = {}
    all_slugs = set(today_chunks.keys()) | set(prev_chunks.keys())

    for slug in sorted(all_slugs):
        key = _chunk_key(assembly, slug)
        tc = today_chunks.get(slug)
        pc = prev_chunks.get(slug)
        delta: dict[str, Any] = {"slug": slug}

        # Status
        if tc and pc:
            delta["status"] = "compared"
        elif tc and not pc:
            delta["status"] = "new"
        elif not tc and pc:
            continue  # chunk was removed, skip
        else:
            continue

        # ── Fact delta ──
        tf = tc.get("fact", {}) if tc else {}
        pf = pc.get("fact", {}) if pc else {}
        t_passed = tf.get("passed", 0)
        t_total = tf.get("total", 0)
        p_passed = pf.get("passed", 0)
        p_total = pf.get("total", 0)

        t_rate = (t_passed / t_total * 100) if t_total > 0 else None
        p_rate = (p_passed / p_total * 100) if p_total > 0 else None
        delta["factPassed"] = t_passed
        delta["factTotal"] = t_total
        delta["factPassRate"] = round(t_rate, 1) if t_rate is not None else None
        if delta["status"] == "compared" and t_rate is not None and p_rate is not None:
            delta["factDelta"] = round(t_rate - p_rate, 1)
        else:
            delta["factDelta"] = None

        # Coverage gap: metaTotal - total
        t_gap = max(0, (tf.get("metaTotal", 0) or 0) - t_total)
        p_gap = max(0, (pf.get("metaTotal", 0) or 0) - p_total)
        delta["coverageGap"] = t_gap
        if delta["status"] == "compared":
            delta["coverageGapDelta"] = t_gap - p_gap
        else:
            delta["coverageGapDelta"] = None

        # ── Benchmark delta ──
        tb = tc.get("benchmark", {}) if tc else {}
        pb = pc.get("benchmark", {}) if pc else {}
        t_dur = tb.get("meanDurationMs")
        p_dur = pb.get("meanDurationMs")
        t_ops = tb.get("meanOpsPerSecond")
        p_ops = pb.get("meanOpsPerSecond")

        delta["benchMethodCount"] = tb.get("methodCount", 0)
        delta["benchDurationMs"] = t_dur
        delta["benchOpsPerSec"] = t_ops
        if delta["status"] == "compared" and t_dur and p_dur and p_dur > 0:
            delta["benchDurationDelta"] = round((t_dur - p_dur) / p_dur * 100, 1)
        else:
            delta["benchDurationDelta"] = None
        if delta["status"] == "compared" and t_ops and p_ops and p_ops > 0:
            delta["benchOpsDelta"] = round((t_ops - p_ops) / p_ops * 100, 1)
        else:
            delta["benchOpsDelta"] = None

        # ── Hotupdate delta ──
        th = tc.get("hotupdate", {}) if tc else {}
        ph = pc.get("hotupdate", {}) if pc else {}
        delta["huPassed"] = th.get("passed", 0)
        delta["huFailed"] = th.get("failed", 0)
        delta["huPatchFailed"] = th.get("patchFailed", False)

        per_chunk[key] = delta

    # ── Assembly-level aggregate ──
    fact_passed = sum(c.get("factPassed", 0) for c in per_chunk.values())
    fact_total = sum(c.get("factTotal", 0) for c in per_chunk.values())
    fact_rate = (fact_passed / fact_total * 100) if fact_total > 0 else None

    prev_fact_passed = sum(
        pc.get("fact", {}).get("passed", 0) for pc in prev_chunks.values()
    )
    prev_fact_total = sum(
        pc.get("fact", {}).get("total", 0) for pc in prev_chunks.values()
    )
    prev_fact_rate = (prev_fact_passed / prev_fact_total * 100) if prev_fact_total > 0 else None

    aggregate = {
        "assembly": assembly,
        "totalChunks": today_summary.get("chunks", 0),
        "chunksVerified": today_summary.get("chunksVerified", 0),
        "chunksWithMetaMismatch": today_summary.get("chunksWithMetaMismatch", 0),
        "chunksWithValueWarnings": today_summary.get("chunksWithValueWarnings", 0),
        "factPassRate": round(fact_rate, 1) if fact_rate is not None else None,
        "totalBenchmarked": today_summary.get("totalBenchmarkedMethods", 0),
        "benchComparison": today_summary.get("benchmarkComparison", {}),
        "hotupdate": today_summary.get("hotupdate", {}),
        "totalCoverageGap": sum(c.get("coverageGap", 0) for c in per_chunk.values()),
        "hasPrevious": prev_path is not None,
        "previousDate": prev_date,
    }
    if prev_fact_rate is not None and fact_rate is not None:
        aggregate["factPassRateDelta"] = round(fact_rate - prev_fact_rate, 1)
    else:
        aggregate["factPassRateDelta"] = None

    return {"assembly": assembly, "aggregate": aggregate, "chunks": per_chunk}


def compute_nightly_delta(
    foundation_dll_root: Path,
    today: str | None = None,
) -> dict[str, Any]:
    """Compute nightly delta across all assemblies in foundation-dll/.

    Scans testing/foundation-dll/ for assembly directories (those with
    _dll/reports/latest/), computes deltas for each, collects regressions
    and improvements.
    """
    if today is None:
        today = date.today().isoformat()

    assemblies: list[dict] = []
    all_chunks: dict[str, dict] = {}
    regressions: list[dict] = []
    improvements: list[dict] = []
    new_failures: list[dict] = []

    for entry in sorted(foundation_dll_root.iterdir()):
        if not entry.is_dir():
            continue
        assembly = entry.name
        # Check if this is a foundation-dll assembly (has _dll/reports/latest)
        if not (entry / "_dll" / "reports" / "latest").is_dir():
            continue

        result = compute_assembly_delta(
            assembly=assembly,
            foundation_dir=entry,
            today=today,
        )
        assemblies.append(result)

        # Collect per-chunk data and detect regressions/improvements
        for key, cd in result["chunks"].items():
            all_chunks[key] = cd

            if cd.get("status") != "compared":
                continue

            # Fact regression: pass rate dropped > 5%
            fd = cd.get("factDelta")
            if fd is not None and fd < -5:
                regressions.append({
                    "assembly": assembly,
                    "slug": cd["slug"],
                    "metric": "fact_pass_rate",
                    "before": round(cd["factPassRate"] - fd, 1) if cd.get("factPassRate") is not None else None,
                    "after": cd.get("factPassRate"),
                    "delta": fd,
                })

            # Fact improvement: pass rate increased > 5%
            if fd is not None and fd > 5:
                improvements.append({
                    "assembly": assembly,
                    "slug": cd["slug"],
                    "metric": "fact_pass_rate",
                    "before": round(cd["factPassRate"] - fd, 1) if cd.get("factPassRate") is not None else None,
                    "after": cd.get("factPassRate"),
                    "delta": fd,
                })

            # Benchmark regression: duration increased > 10%
            bd = cd.get("benchDurationDelta")
            if bd is not None and bd > 10:
                regressions.append({
                    "assembly": assembly,
                    "slug": cd["slug"],
                    "metric": "bench_duration",
                    "before": round(cd["benchDurationMs"] / (1 + bd / 100), 4) if cd.get("benchDurationMs") else None,
                    "after": cd.get("benchDurationMs"),
                    "delta": bd,
                })

            # Benchmark improvement: duration decreased > 10%
            if bd is not None and bd < -10:
                improvements.append({
                    "assembly": assembly,
                    "slug": cd["slug"],
                    "metric": "bench_duration",
                    "before": round(cd["benchDurationMs"] / (1 + bd / 100), 4) if cd.get("benchDurationMs") else None,
                    "after": cd.get("benchDurationMs"),
                    "delta": bd,
                })

            # Coverage gap widened
            cgd = cd.get("coverageGapDelta")
            if cgd is not None and cgd > 0:
                regressions.append({
                    "assembly": assembly,
                    "slug": cd["slug"],
                    "metric": "coverage_gap",
                    "before": cd["coverageGap"] - cgd,
                    "after": cd["coverageGap"],
                    "delta": cgd,
                })

            # Coverage gap narrowed
            if cgd is not None and cgd < 0:
                improvements.append({
                    "assembly": assembly,
                    "slug": cd["slug"],
                    "metric": "coverage_gap",
                    "before": cd["coverageGap"] - cgd,
                    "after": cd["coverageGap"],
                    "delta": cgd,
                })

    # ── Overall summary ──
    total_chunks = sum(a["aggregate"]["totalChunks"] for a in assemblies)
    total_verified = sum(a["aggregate"]["chunksVerified"] for a in assemblies)
    total_benchmarked = sum(a["aggregate"]["totalBenchmarked"] for a in assemblies)
    total_coverage_gap = sum(a["aggregate"]["totalCoverageGap"] for a in assemblies)

    all_fact_passed = sum(
        c.get("factPassed", 0) for c in all_chunks.values()
    )
    all_fact_total = sum(
        c.get("factTotal", 0) for c in all_chunks.values()
    )
    overall_rate = (all_fact_passed / all_fact_total * 100) if all_fact_total > 0 else None

    return {
        "date": today,
        "generatedAt": datetime.now(timezone.utc).isoformat(),
        "overall": {
            "totalAssemblies": len(assemblies),
            "totalChunks": total_chunks,
            "chunksVerified": total_verified,
            "factPassRate": round(overall_rate, 1) if overall_rate is not None else None,
            "totalBenchmarked": total_benchmarked,
            "totalCoverageGap": total_coverage_gap,
            "assembliesWithHistory": sum(1 for a in assemblies if a["aggregate"]["hasPrevious"]),
        },
        "regressions": regressions,
        "improvements": improvements,
        "assemblies": [a["aggregate"] for a in assemblies],
        "chunks": all_chunks,
    }


# ── CLI ──


def main() -> int:
    import argparse

    parser = argparse.ArgumentParser(description="Compute nightly delta across all assemblies")
    parser.add_argument("--foundation-dll", default=None,
                        help="Path to testing/foundation-dll/ (default: auto-detect)")
    parser.add_argument("--date", default=None,
                        help="Today's date YYYY-MM-DD (default: today)")
    parser.add_argument("--output", default="nightly-delta.json",
                        help="Output path for delta JSON (default: nightly-delta.json)")
    args = parser.parse_args()

    if args.foundation_dll:
        root = Path(args.foundation_dll)
    else:
        root = Path(__file__).resolve().parent.parent

    delta = compute_nightly_delta(root, today=args.date)
    output = Path(args.output)
    output.parent.mkdir(parents=True, exist_ok=True)
    output.write_text(json.dumps(delta, indent=2, ensure_ascii=False), encoding="utf-8")
    print(f"  [nightly-delta] Written to {output}")
    print(f"  [nightly-delta] {delta['overall']['totalAssemblies']} assemblies, "
          f"{delta['overall']['totalChunks']} chunks, "
          f"{len(delta['regressions'])} regressions, "
          f"{len(delta['improvements'])} improvements")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
