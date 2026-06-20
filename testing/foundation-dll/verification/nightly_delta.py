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
    # Benchmark history (for performance comparison)
    prev_bench_path = _find_last_history(dll_reports, "benchmark", today)
    prev_bench_data = _load_json(prev_bench_path) if prev_bench_path else None
    prev_chunks: dict[str, dict] = {}
    prev_bench_date: str | None = None
    if prev_bench_data:
        prev_bench_date = prev_bench_data.get("date")
        for cs in prev_bench_data.get("chunks", []):
            prev_chunks[cs.get("slug", "?")] = cs

    # Fact history (for fact pass rate trend)
    prev_fact_path = _find_last_history(dll_reports, "fact", today)
    prev_fact_data = _load_json(prev_fact_path) if prev_fact_path else None
    prev_fact_chunks: dict[str, dict] = {}
    prev_fact_date: str | None = None
    if prev_fact_data:
        prev_fact_date = prev_fact_data.get("date")
        for cs in prev_fact_data.get("chunks", []):
            prev_fact_chunks[cs.get("slug", "?")] = cs

    # Coverage history (for coverage gap trend)
    prev_cov_path = _find_last_history(dll_reports, "coverage", today)
    prev_cov_data = _load_json(prev_cov_path) if prev_cov_path else None
    prev_cov_chunks: dict[str, dict] = {}
    prev_cov_date: str | None = None
    if prev_cov_data:
        prev_cov_date = prev_cov_data.get("date")
        for cs in prev_cov_data.get("chunks", []):
            prev_cov_chunks[cs.get("slug", "?")] = cs

    # ── Per-chunk delta ──
    per_chunk: dict[str, dict[str, Any]] = {}
    all_slugs = set(today_chunks.keys()) | set(prev_chunks.keys()) | set(prev_fact_chunks.keys()) | set(prev_cov_chunks.keys())

    for slug in sorted(all_slugs):
        key = _chunk_key(assembly, slug)
        tc = today_chunks.get(slug)
        pc = prev_chunks.get(slug)
        pf = prev_fact_chunks.get(slug)
        pcov = prev_cov_chunks.get(slug)
        delta: dict[str, Any] = {"slug": slug}

        # Status — use benchmark history as primary, fall back to fact
        if tc and (pc or pf or pcov):
            delta["status"] = "compared"
        elif tc and not (pc or pf or pcov):
            delta["status"] = "new"
        elif not tc and (pc or pf):
            continue  # chunk was removed, skip
        else:
            continue

        # Skip chunks that were never built
        fact = tc.get("fact", {}) if tc else {}
        t_total = fact.get("total", 0)
        if t_total == 0:
            continue

        # ── Fact delta ──
        tf = tc.get("fact", {}) if tc else {}
        pf_dict = pf.get("fact", {}) if pf else {}
        t_passed = tf.get("passed", 0)
        t_total = tf.get("total", 0)
        p_passed = pf_dict.get("passed", 0)
        p_total = pf_dict.get("total", 0)

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
        t_meta = tf.get("metaTotal") if tf.get("metaTotal") is not None else None
        t_gap = max(0, t_meta - t_total) if t_meta is not None and t_total > 0 else None
        p_meta = pf_dict.get("metaTotal") if pf_dict.get("metaTotal") is not None else None
        p_gap = max(0, p_meta - p_total) if p_meta is not None and p_total > 0 else None
        delta["coverageGap"] = t_gap
        t_gap_pct = round(t_gap / t_meta * 100, 1) if t_gap is not None and t_meta and t_meta > 0 else None
        p_gap_pct = round(p_gap / p_meta * 100, 1) if p_gap is not None and p_meta and p_meta > 0 else None
        delta["coverageGapPct"] = t_gap_pct
        if delta["status"] == "compared" and t_gap is not None and p_gap is not None:
            delta["coverageGapDelta"] = t_gap - p_gap
            if t_gap_pct is not None and p_gap_pct is not None:
                delta["coverageGapPctDelta"] = round(t_gap_pct - p_gap_pct, 1)
            else:
                delta["coverageGapPctDelta"] = None
        else:
            delta["coverageGapDelta"] = None
            delta["coverageGapPctDelta"] = None

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

        # ── Memory delta (allocated bytes) ──
        t_alloc = tb.get("totalAllocatedBytes")
        p_alloc = pb.get("totalAllocatedBytes")
        delta["benchAllocatedBytes"] = t_alloc
        if delta["status"] == "compared" and t_alloc and p_alloc and p_alloc > 0:
            delta["benchAllocDelta"] = round((t_alloc - p_alloc) / p_alloc * 100, 1)
        else:
            delta["benchAllocDelta"] = None

        # ── Profile delta (code size, optional) ──
        tp = tc.get("profile", {}) if tc else {}
        pp = pc.get("profile", {}) if pc else {}
        delta["profileMethodCount"] = tp.get("methodCount", 0)
        delta["profileTotalSize"] = tp.get("totalSize", 0)
        delta["profileSource"] = tp.get("source", "")
        t_code = tp.get("totalSize")
        p_code = pp.get("totalSize")
        if delta["status"] == "compared" and t_code and p_code and p_code > 0:
            delta["profileSizeDelta"] = round((t_code - p_code) / p_code * 100, 1)
        else:
            delta["profileSizeDelta"] = None

        # ── Hotupdate delta ──
        th = tc.get("hotupdate", {}) if tc else {}
        ph = pc.get("hotupdate", {}) if pc else {}
        delta["huPassed"] = th.get("passed", 0)
        delta["huFailed"] = th.get("failed", 0)
        delta["huPatchFailed"] = th.get("patchFailed", False)
        delta["huRevertRegression"] = th.get("revertRegressionCount", 0)
        delta["huHasBaselineBenchmark"] = len(th.get("details", {}).get("baselineBenchmark", [])) > 0 if th else False

        # ── Build status ──
        tb = tc.get("build", {}) if tc else {}
        delta["buildStatus"] = tb.get("status", "not_run")

        # ── Managed benchmark ──
        tm = tc.get("managedBenchmark", {}) if tc else {}
        delta["mbPassed"] = tm.get("passed", 0)
        delta["mbTotal"] = tm.get("total", 0)

        # ── Benchmark report (cross-technology comparison) ──
        tr = tc.get("benchmarkReport", {}) if tc else {}
        delta["comparisonMethods"] = tr.get("totalMethods", 0)
        delta["comparisonWithNet8"] = tr.get("methodsWithNet8", 0)

        per_chunk[key] = delta

    # ── Assembly-level aggregate ──
    fact_passed = sum(c.get("factPassed", 0) for c in per_chunk.values())
    fact_total = sum(c.get("factTotal", 0) for c in per_chunk.values())
    fact_rate = (fact_passed / fact_total * 100) if fact_total > 0 else None

    prev_fact_passed = sum(
        fc.get("fact", {}).get("passed", 0) for fc in
        (prev_fact_chunks if prev_fact_chunks else prev_chunks).values()
    )
    prev_fact_total = sum(
        fc.get("fact", {}).get("total", 0) for fc in
        (prev_fact_chunks if prev_fact_chunks else prev_chunks).values()
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
        "totalCoverageGap": sum(c.get("coverageGap") or 0 for c in per_chunk.values()),
        "coverageGapPct": round(
            sum(c.get("coverageGap") or 0 for c in per_chunk.values())
            / max(1, sum(c.get("factTotal", 0) for c in per_chunk.values()) + sum(c.get("coverageGap") or 0 for c in per_chunk.values()))
            * 100, 1
        ) if sum(c.get("factTotal", 0) for c in per_chunk.values()) > 0 else None,
        "totalAllocatedBytes": sum(c.get("benchAllocatedBytes") or 0 for c in per_chunk.values()),
        "hasPrevious": prev_fact_path is not None or prev_bench_path is not None,
        "previousDate": prev_fact_date or prev_bench_date,
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

            # Memory allocation regression: allocatedBytes increased > 15%
            mad = cd.get("benchAllocDelta")
            if mad is not None and mad > 15:
                regressions.append({
                    "assembly": assembly,
                    "slug": cd["slug"],
                    "metric": "memory_alloc",
                    "before": round(cd["benchAllocatedBytes"] / (1 + mad / 100), 1) if cd.get("benchAllocatedBytes") else None,
                    "after": cd.get("benchAllocatedBytes"),
                    "delta": mad,
                })
            if mad is not None and mad < -15:
                improvements.append({
                    "assembly": assembly,
                    "slug": cd["slug"],
                    "metric": "memory_alloc",
                    "before": round(cd["benchAllocatedBytes"] / (1 + mad / 100), 1) if cd.get("benchAllocatedBytes") else None,
                    "after": cd.get("benchAllocatedBytes"),
                    "delta": mad,
                })

            # Coverage gap pct regression: gap percentage increased > 5pp
            cgpd = cd.get("coverageGapPctDelta")
            if cgpd is not None and cgpd > 5:
                regressions.append({
                    "assembly": assembly,
                    "slug": cd["slug"],
                    "metric": "coverage_gap_pct",
                    "before": round((cd.get("coverageGapPct") or 0) - cgpd, 1),
                    "after": cd.get("coverageGapPct"),
                    "delta": cgpd,
                })
            if cgpd is not None and cgpd < -5:
                improvements.append({
                    "assembly": assembly,
                    "slug": cd["slug"],
                    "metric": "coverage_gap_pct",
                    "before": round((cd.get("coverageGapPct") or 0) - cgpd, 1),
                    "after": cd.get("coverageGapPct"),
                    "delta": cgpd,
                })

    # ── Overall summary ──
    total_chunks = sum(a["aggregate"]["totalChunks"] for a in assemblies)
    total_verified = sum(a["aggregate"]["chunksVerified"] for a in assemblies)
    total_benchmarked = sum(a["aggregate"]["totalBenchmarked"] for a in assemblies)
    total_coverage_gap = sum(a["aggregate"]["totalCoverageGap"] for a in assemblies)
    total_allocated = sum(a["aggregate"].get("totalAllocatedBytes", 0) for a in assemblies)

    all_fact_passed = sum(
        c.get("factPassed", 0) for c in all_chunks.values()
    )
    all_fact_total = sum(
        c.get("factTotal", 0) for c in all_chunks.values()
    )
    overall_rate = (all_fact_passed / all_fact_total * 100) if all_fact_total > 0 else None

    # ── Compute regression grade ──
    # Use chunk-level aggregate approach: grade based on worst per-chunk delta.
    # Thresholds match regression_grading.py (tuned for real benchmark variance).
    bench_grades = []
    for ck, cd in all_chunks.items():
        bd = cd.get("benchDurationDelta")
        ad = cd.get("benchAllocDelta")
        if (bd is not None and bd > 50) or (ad is not None and ad > 30):
            bench_grades.append("hard")
        elif (bd is not None and bd > 25) or (ad is not None and ad > 30):
            bench_grades.append("soft")
        else:
            bench_grades.append("none")

    grade_order = {"none": 0, "soft": 1, "hard": 2}
    regression_grade = max(bench_grades, key=lambda g: grade_order.get(g, 0)) if bench_grades else "none"
    regressed_count = sum(1 for g in bench_grades if g == "hard")
    degraded_count = sum(1 for g in bench_grades if g == "soft")

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
            "totalCoverageGapPct": round(
                total_coverage_gap / max(1, all_fact_total + total_coverage_gap) * 100, 1
            ) if all_fact_total > 0 else None,
            "totalAllocatedBytes": total_allocated,
            "assembliesWithHistory": sum(1 for a in assemblies if a["aggregate"]["hasPrevious"]),
            "regressionGrade": regression_grade,
            "regressedChunks": regressed_count,
            "degradedChunks": degraded_count,
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
