#!/usr/bin/env python3
"""Classify nightly regressions into severity tiers and emit an actionable report.

Why
---
nightly_delta.py already computes per-chunk deltas, and nightly_summary.py
renders them.  What was missing is a *decision*: which regressions must block,
which should just be seen.  Without that, the nightly issue either fires on
every blip or is ignored entirely.

Severity model
--------------
  hard   the metric crossed a threshold that means the product regressed.
         These are the reason to file/keep an issue open.
  soft   movement in the wrong direction, below the hard threshold.  Reported
         for trend-watching, never blocks.
  info   improvements and neutral movement.

Thresholds are per-metric and explicit (see DEFAULT_THRESHOLDS).  A single
hard regression promotes the whole report to overall=hard.

Usage
-----
    python degenerate_report.py --delta nightly-delta.json \
        --output nightly-degradation.json [--markdown nightly-degradation.md]
"""
from __future__ import annotations

import argparse
import json
import sys
from pathlib import Path

SCHEMA_VERSION = 1

# metric -> (hard_threshold, soft_threshold), in the metric's own units.
# A regression is `delta` moving in the bad direction past the threshold.
# bad direction: pass_rate/coverage are "higher is better"; duration is the
# opposite, so its deltas are negated before comparison (see _is_regression).
DEFAULT_THRESHOLDS: dict[str, dict[str, float]] = {
    "fact_pass_rate":   {"hard": 5.0,  "soft": 0.5},   # percentage points
    "bench_duration":   {"hard": 20.0, "soft": 5.0},   # percent slower
    "coverage_gap":     {"hard": 1.0,  "soft": 0.1},   # chunk count
}

HIGHER_IS_BETTER = {"fact_pass_rate"}   # everything else: higher delta = worse


def _classify(metric: str, delta: float | None) -> str | None:
    """Return 'hard' / 'soft' / None for a regression delta."""
    if delta is None:
        return None
    t = DEFAULT_THRESHOLDS.get(metric)
    if t is None:
        return None

    worse = -delta if metric in HIGHER_IS_BETTER else delta
    if worse >= t["hard"]:
        return "hard"
    if worse >= t["soft"]:
        return "soft"
    return None


def classify_report(delta: dict, thresholds: dict | None = None) -> dict:
    """Attach severity to each regression in a nightly-delta document."""
    global DEFAULT_THRESHOLDS
    if thresholds:
        DEFAULT_THRESHOLDS = thresholds

    regressions = delta.get("regressions") or []
    improvements = delta.get("improvements") or []
    new_failures = delta.get("newFailures") or delta.get("new_failures") or []

    hard, soft = [], []
    for r in regressions:
        sev = _classify(r.get("metric"), r.get("delta"))
        if sev is None:
            continue
        entry = dict(r)
        entry["severity"] = sev
        (hard if sev == "hard" else soft).append(entry)

    # New failures are always hard — a chunk that previously had results and
    # now does not is a regression regardless of any metric threshold.
    for nf in new_failures:
        entry = dict(nf)
        entry["severity"] = "hard"
        hard.append(entry)

    overall = "hard" if hard else ("soft" if soft else "ok")
    return {
        "schemaVersion": SCHEMA_VERSION,
        "overall": overall,
        "thresholds": DEFAULT_THRESHOLDS,
        "counts": {
            "hard": len(hard),
            "soft": len(soft),
            "improvements": len(improvements),
            "newFailures": len(new_failures),
        },
        "hard": sorted(hard, key=lambda x: (x.get("assembly", ""), x.get("slug", ""))),
        "soft": sorted(soft, key=lambda x: (x.get("assembly", ""), x.get("slug", ""))),
        "improvements": improvements,
    }


def render_markdown(rep: dict) -> str:
    icon = {"hard": "🔴", "soft": "🟡", "ok": "🟢"}[rep["overall"]]
    c = rep["counts"]
    out = [
        "## Nightly 退化报告",
        "",
        f"**总体判定：{icon} {rep['overall'].upper()}** —— "
        f"hard {c['hard']} · soft {c['soft']} · "
        f"改善 {c['improvements']} · 新增失败 {c['newFailures']}",
        "",
    ]

    if rep["hard"]:
        out += ["### 🔴 Hard（需处理）", "",
                "| DLL | Chunk | 指标 | 前 | 后 | Δ |", "|-----|-------|------|----|----|---|"]
        for r in rep["hard"]:
            out.append(
                f"| {r.get('assembly','—')} | {r.get('slug','—')} | "
                f"{r.get('metric','—')} | {r.get('before','—')} | "
                f"{r.get('after','—')} | {r.get('delta','—')} |")
        out.append("")

    if rep["soft"]:
        out += [f"### 🟡 Soft（观察，{len(rep['soft'])} 项）", ""]
        for r in rep["soft"][:20]:
            out.append(f"- `{r.get('assembly','—')}` / `{r.get('slug','—')}` "
                       f"{r.get('metric')} Δ{r.get('delta')}")
        if len(rep["soft"]) > 20:
            out.append(f"- …另有 {len(rep['soft']) - 20} 项")
        out.append("")

    if rep["overall"] == "ok":
        out += ["无退化。", ""]

    th = rep["thresholds"]
    out += [
        "---",
        "",
        "<sub>阈值：" + " · ".join(
            f"{m} hard≥{v['hard']}/soft≥{v['soft']}" for m, v in th.items()
        ) + "</sub>",
    ]
    return "\n".join(out) + "\n"


def main(argv: list[str] | None = None) -> int:
    ap = argparse.ArgumentParser(description=__doc__,
                                 formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("--delta", required=True, help="nightly-delta.json path")
    ap.add_argument("--output", required=True, help="Classification JSON output")
    ap.add_argument("--markdown", help="Optional Markdown report output")
    ap.add_argument("--fail-on-hard", action="store_true",
                    help="Exit 1 when overall=hard (for use as a CI gate)")
    args = ap.parse_args(argv)

    try:
        delta = json.loads(Path(args.delta).read_text(encoding="utf-8"))
    except (OSError, json.JSONDecodeError) as e:
        print(f"ERROR: cannot read delta {args.delta}: {e}", file=sys.stderr)
        return 2

    rep = classify_report(delta)

    out = Path(args.output)
    out.parent.mkdir(parents=True, exist_ok=True)
    out.write_text(json.dumps(rep, indent=2, ensure_ascii=False) + "\n",
                   encoding="utf-8")
    if args.markdown:
        Path(args.markdown).write_text(render_markdown(rep), encoding="utf-8")

    print(f"degradation-report -> {out}")
    print(f"  overall  {rep['overall']}")
    print(f"  hard={rep['counts']['hard']} soft={rep['counts']['soft']} "
          f"improvements={rep['counts']['improvements']}")

    if args.fail_on_hard and rep["overall"] == "hard":
        return 1
    return 0


if __name__ == "__main__":
    sys.exit(main())
