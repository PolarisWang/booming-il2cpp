"""nightly_triage — turn two nightly baselines into a graded regression report.

Reads the baseline store (see `baseline_store.py`) and answers three questions
per run:

  1. **What got worse?**  — fact / benchmark / hotupdate deltas against a rolling
     window of prior runs.
  2. **Is it real?**      — a delta is only "confirmed" when it exceeds the
     baseline's own noise band (median ± k·MAD).  This matters because measured
     benchmark CV is large: in a real CoreLib/text run only ~9.5% of methods had
     CV < 0.05, so a naive single-run comparison would be mostly false positives.
  3. **What should we do?** — when ≥ THRESHOLD_RATIO of the items in a domain
     regress, the domain is flagged for a design review ("方案重整") rather than
     a point fix.

Output is a graded markdown report:
  🔴 Confirmed regressions — exceeds both the % threshold and the noise band
  🟡 Watch                 — exceeds the % threshold but inside the noise band
  📋 Suggested actions     — what to dispatch, pending human confirmation
  ⏭️  Skipped               — chunks whose provenance makes them incomparable

The tool is read-only with respect to build artifacts: it never runs a nightly,
it only analyses snapshots already recorded by `aggregate_reports()`.
"""

from __future__ import annotations

import argparse
import statistics
import sys
from dataclasses import dataclass, field
from datetime import datetime, timezone
from pathlib import Path

# Bootstrap `verification` package import when run as a script.
_HERE = Path(__file__).resolve().parent          # verification/tools/
_VERIFY = _HERE.parent                            # verification/
_E2E = _VERIFY.parent                             # tests/e2e/
if str(_E2E) not in sys.path:
    sys.path.insert(0, str(_E2E))

from verification.tools.baseline_store import (  # noqa: E402
    Baseline, ChunkBaseline, is_comparable, load_recent, load_reference,
)

# ── thresholds ────────────────────────────────────────────────────────────────
# A domain whose regressed fraction reaches this is flagged for design review.
THRESHOLD_RATIO = 0.30

# Per-domain "worth looking at" thresholds.
FACT_REAL_DROP_PCT = 0.05      # realVerified/total down 5%+
FACT_SMOKE_RISE_PCT = 0.10     # smokeUnknown up 10%+
BENCH_SLOW_PCT = 0.30          # meanDurationMs up 30%+
HU_SEMANTIC_CHANGE_PCT = 0.50  # semanticChangedCount swings 50%+
CROSSTECH_DIFF_RATIO = 0.30    # >30% of matched records disagree

# MAD multiplier for the noise band.  3.0 ≈ 2σ for a normal distribution but is
# robust to the outliers that plague short benchmark runs.
MAD_K = 3.0

# Benchmarks faster than this are dominated by measurement noise — excluded
# from regression judgement entirely.
NOISE_FLOOR_MS = 0.05


# ── helpers ───────────────────────────────────────────────────────────────────

def _safe_ratio(num: float, den: float) -> float:
    return num / den if den else 0.0


def _pct_delta(cur: float, base: float) -> float:
    """Relative change; 0 when base is 0 (undefined)."""
    if base == 0:
        return 0.0
    return (cur - base) / abs(base)


def _noise_band(values: list[float]) -> tuple[float, float, float]:
    """Return (median, lower, upper) using a MAD-based band.

    Falls back to (median, median, median) when there is too little history or
    the samples are identical, which makes every delta 'significant' only if it
    also clears the percentage threshold.
    """
    if not values:
        return 0.0, 0.0, 0.0
    med = statistics.median(values)
    if len(values) < 2:
        return med, med, med
    deviations = [abs(v - med) for v in values]
    mad = statistics.median(deviations)
    if mad == 0:
        # Identical history — use a tiny epsilon so any real change registers,
        # but the percentage threshold still governs.
        return med, med, med
    spread = MAD_K * mad
    return med, med - spread, med + spread


def _beyond_band(value: float, band: tuple[float, float, float]) -> bool:
    _, lo, hi = band
    return value > hi or value < lo


# ── result model ──────────────────────────────────────────────────────────────

@dataclass
class Finding:
    domain: str            # fact | benchmark | hotupdate | crosstech
    chunk: str
    metric: str
    current: float
    baseline: float
    delta_pct: float
    confirmed: bool        # True → 🔴, False → 🟡
    note: str = ""

    def to_row(self) -> str:
        mark = "🔴" if self.confirmed else "🟡"
        return (f"| {mark} | {self.chunk} | {self.metric} | "
                f"{self.current:.4g} | {self.baseline:.4g} | "
                f"{self.delta_pct:+.1%} | {self.note} |")


@dataclass
class DomainVerdict:
    domain: str
    considered: int = 0
    regressed: int = 0
    ratio: float = 0.0
    needs_design_review: bool = False


@dataclass
class TriageResult:
    current: Baseline
    baseline_label: str
    comparable: bool
    comparability_reason: str
    commit_distance: int
    findings: list[Finding] = field(default_factory=list)
    verdicts: list[DomainVerdict] = field(default_factory=list)
    skipped: list[tuple[str, str]] = field(default_factory=list)
    prior_actions: list[tuple[str, bool, str]] = field(default_factory=list)

    def confirmed(self) -> list[Finding]:
        return [f for f in self.findings if f.confirmed]

    def watch(self) -> list[Finding]:
        return [f for f in self.findings if not f.confirmed]


# ── analysis ──────────────────────────────────────────────────────────────────

def _history_values(history: list[Baseline], chunk: str, domain: str,
                    metric: str) -> list[float]:
    vals: list[float] = []
    for b in history:
        cb = b.chunks.get(chunk)
        if not cb:
            continue
        section = getattr(cb, domain, None)
        if not section:
            continue
        v = section.get(metric)
        if isinstance(v, (int, float)):
            vals.append(float(v))
    return vals


def _analyze_fact(cur_cb: ChunkBaseline, hist: list[Baseline]) -> list[Finding]:
    out: list[Finding] = []
    cur_real = cur_cb.fact.get("realVerified")
    cur_total = cur_cb.fact.get("total") or 0
    if cur_real is None or cur_total == 0:
        return out
    cur_rate = cur_real / cur_total

    rates = [
        (_history_values(hist, cur_cb.key, "fact", "realVerified")[i]
         / (_history_values(hist, cur_cb.key, "fact", "total")[i] or 1))
        for i in range(len(_history_values(hist, cur_cb.key, "fact", "realVerified")))
    ] if hist else []
    base_rate = statistics.median(rates) if rates else cur_rate

    delta = _pct_delta(cur_rate, base_rate)
    if delta <= -FACT_REAL_DROP_PCT:
        band = _noise_band(rates)
        out.append(Finding(
            domain="fact", chunk=cur_cb.key, metric="realVerified/total",
            current=cur_rate, baseline=base_rate, delta_pct=delta,
            confirmed=_beyond_band(cur_rate, band),
            note="覆盖率下降 → 翻译管线/ATG",
        ))

    cur_smoke = cur_cb.fact.get("smokeUnknown", 0)
    smoke_hist = _history_values(hist, cur_cb.key, "fact", "smokeUnknown")
    base_smoke = statistics.median(smoke_hist) if smoke_hist else cur_smoke
    sdelta = _pct_delta(cur_smoke, base_smoke)
    if sdelta >= FACT_SMOKE_RISE_PCT:
        out.append(Finding(
            domain="fact", chunk=cur_cb.key, metric="smokeUnknown",
            current=cur_smoke, baseline=base_smoke, delta_pct=sdelta,
            confirmed=_beyond_band(cur_smoke, _noise_band(smoke_hist)),
            note="应断言未断言的方法增多",
        ))
    return out


def _analyze_benchmark(cur_cb: ChunkBaseline, hist: list[Baseline]) -> list[Finding]:
    out: list[Finding] = []
    section = cur_cb.benchmark or {}
    mean = section.get("meanDurationMs")
    if not isinstance(mean, (int, float)):
        return out
    if mean < NOISE_FLOOR_MS:
        return out  # below the noise floor — not judgeable
    hist_means = _history_values(hist, cur_cb.key, "benchmark", "meanDurationMs")
    hist_means = [v for v in hist_means if v >= NOISE_FLOOR_MS]
    if not hist_means:
        return out
    base = statistics.median(hist_means)
    delta = _pct_delta(mean, base)
    if delta >= BENCH_SLOW_PCT:
        out.append(Finding(
            domain="benchmark", chunk=cur_cb.key, metric="meanDurationMs",
            current=mean, baseline=base, delta_pct=delta,
            confirmed=_beyond_band(mean, _noise_band(hist_means)),
            note="性能退化 → 定位慢路径",
        ))
    return out


def _analyze_hotupdate(cur_cb: ChunkBaseline, hist: list[Baseline]) -> list[Finding]:
    out: list[Finding] = []
    section = cur_cb.hotupdate or {}
    if not section:
        return out
    if section.get("patchFailed"):
        out.append(Finding(
            domain="hotupdate", chunk=cur_cb.key, metric="patchFailed",
            current=1.0, baseline=0.0, delta_pct=1.0, confirmed=True,
            note="补丁机制失败 → hotupdate 管线修复",
        ))
    if section.get("crash"):
        out.append(Finding(
            domain="hotupdate", chunk=cur_cb.key, metric="crash",
            current=1.0, baseline=0.0, delta_pct=1.0, confirmed=True,
            note="hotupdate 崩溃",
        ))
    cur_sem = section.get("semanticChangedCount", 0)
    hist_sem = _history_values(hist, cur_cb.key, "hotupdate", "semanticChangedCount")
    if hist_sem:
        base = statistics.median(hist_sem)
        delta = _pct_delta(cur_sem, base)
        if abs(delta) >= HU_SEMANTIC_CHANGE_PCT:
            out.append(Finding(
                domain="hotupdate", chunk=cur_cb.key,
                metric="semanticChangedCount",
                current=cur_sem, baseline=base, delta_pct=delta,
                confirmed=_beyond_band(cur_sem, _noise_band(hist_sem)),
                note="语义变化检测异常",
            ))
    return out


def _analyze_crosstech(cur_cb: ChunkBaseline, hist: list[Baseline]) -> list[Finding]:
    out: list[Finding] = []
    section = cur_cb.crosstech or {}
    diff = section.get("diffCount")
    aot = section.get("aotPassed") or 0
    jit = section.get("jitPassed")
    if diff is None or jit is None or not aot:
        return out
    ratio = _safe_ratio(diff, aot)
    if ratio >= CROSSTECH_DIFF_RATIO:
        out.append(Finding(
            domain="crosstech", chunk=cur_cb.key, metric="aot/jit diff",
            current=diff, baseline=0.0, delta_pct=ratio, confirmed=True,
            note=f"{diff}/{aot} AOT/JIT 结果不一致 → 翻译一致性修复",
        ))
    return out


def analyze(report_dir: Path, against: str = "latest", window: int = 5,
            verbose: bool = False) -> TriageResult | None:
    """Analyse the newest baseline against the preceding window."""
    recent = load_recent(report_dir, n=window + 1)
    if len(recent) < 2:
        return None
    current = recent[-1]
    history = recent[:-1]

    comparable = True
    reason = "ok"
    for h in history:
        ok, r = is_comparable(current, h)
        if not ok:
            comparable = False
            reason = r
            break

    commit_distance = _commit_distance(current, history)
    result = TriageResult(
        current=current,
        baseline_label=f"{len(history)} 轮滚动窗口",
        comparable=comparable,
        comparability_reason=reason,
        commit_distance=commit_distance,
    )

    if not comparable:
        return result

    domains = {"fact": [], "benchmark": [], "hotupdate": [], "crosstech": []}
    for key, cb in current.chunks.items():
        # Skip chunks with no history at all — nothing to compare against.
        if not any(key in h.chunks for h in history):
            continue
        domains["fact"].extend(_analyze_fact(cb, history))
        domains["benchmark"].extend(_analyze_benchmark(cb, history))
        domains["hotupdate"].extend(_analyze_hotupdate(cb, history))
        domains["crosstech"].extend(_analyze_crosstech(cb, history))

    for d, findings in domains.items():
        considered = sum(1 for k in current.chunks if k in
                         [kk for h in history for kk in h.chunks])
        verdict = DomainVerdict(
            domain=d,
            considered=considered,
            regressed=len(findings),
            ratio=_safe_ratio(len(findings), considered),
        )
        verdict.needs_design_review = verdict.ratio >= THRESHOLD_RATIO
        result.verdicts.append(verdict)
        result.findings.extend(findings)

    return result


def _commit_distance(cur: Baseline, history: list[Baseline]) -> int:
    """Number of runs between the oldest history entry and current."""
    if not history:
        return 0
    return len(history)


# ── rendering ─────────────────────────────────────────────────────────────────

_ACTION_HINT = {
    "fact": "翻译管线功能开发（提升 fact 率）",
    "benchmark": "性能优化 / 方案重整",
    "hotupdate": "hotupdate 管线修复",
    "crosstech": "翻译一致性修复",
}


def render_markdown(result: TriageResult | None) -> str:
    if result is None:
        return ("# Nightly Triage\n\n"
                "> 基线不足（需要至少 2 轮已记录的 baseline）。\n"
                "> 先跑一轮 nightly 建立基线，下一轮起产出 triage。\n")

    cur = result.current
    lines: list[str] = []
    ts = cur.timestamp[:10] if cur.timestamp else datetime.now(
        timezone.utc).strftime("%Y-%m-%d")
    lines.append(f"# Nightly Triage — {ts}")
    commit = (cur.provenance.get("gitCommit") or "?")[:8]
    lines.append(
        f"> 运行: `{cur.run_id}` @ `{commit}` | 基线: {result.baseline_label} | "
        f"可比性: {'✅ ' + result.comparability_reason if result.comparable else '❌ ' + result.comparability_reason}"
    )
    lines.append("")

    if not result.comparable:
        lines.append("## ⏭️ Skipped")
        lines.append("")
        lines.append(f"本轮与历史基线不可比（{result.comparability_reason}）—— "
                     f"跨设备/平台/配置的对比会产生假回归，已跳过全部判定。")
        lines.append("")
        return "\n".join(lines)

    # Confirmed
    confirmed = result.confirmed()
    lines.append(f"## 🔴 Confirmed Regressions ({len(confirmed)})")
    lines.append("")
    if confirmed:
        lines.append("| | Chunk | 指标 | 当前 | 基线 | Δ% | 说明 |")
        lines.append("|---|-------|------|------|------|----|------|")
        lines += [f.to_row() for f in confirmed]
    else:
        lines.append("_无确认退化_")
    lines.append("")

    # Watch
    watch = result.watch()
    lines.append(f"## 🟡 Watch ({len(watch)})")
    lines.append("")
    if watch:
        lines.append("| | Chunk | 指标 | 当前 | 基线 | Δ% | 说明 |")
        lines.append("|---|-------|------|------|------|----|------|")
        lines += [f.to_row() for f in watch]
    else:
        lines.append("_无观察项_")
    lines.append("")

    # Suggested actions (pending human confirmation)
    lines.append("## 📋 Suggested Actions (待确认)")
    lines.append("")
    by_domain: dict[str, list[Finding]] = {}
    for f in confirmed + watch:
        by_domain.setdefault(f.domain, []).append(f)
    if by_domain:
        for d, fs in sorted(by_domain.items()):
            design = next((v for v in result.verdicts
                           if v.domain == d and v.needs_design_review), None)
            tag = "【方案重整】" if design else "【点修】"
            lines.append(f"- [ ] {tag} **{d}** — {len(fs)} 项 — {_ACTION_HINT.get(d, '')}")
            for f in fs[:3]:
                lines.append(f"      · {f.chunk} / {f.metric} ({f.delta_pct:+.1%})")
            if len(fs) > 3:
                lines.append(f"      · … 另有 {len(fs) - 3} 项")
    else:
        lines.append("_本轮回退项为空，无需派发_")
    lines.append("")

    # Domain verdicts
    lines.append("## 📊 域裁决")
    lines.append("")
    lines.append("| 域 | 对比项 | 退化物 | 比例 | 结论 |")
    lines.append("|----|-------|-------|------|------|")
    for v in result.verdicts:
        verdict = "🔴 方案重整" if v.needs_design_review else (
            "🟡 点修" if v.regressed else "✅ 正常")
        lines.append(f"| {v.domain} | {v.considered} | {v.regressed} | "
                     f"{v.ratio:.0%} | {verdict} |")
    lines.append("")
    lines.append(f"> 方案重整阈值: 域内退化比例 ≥ {THRESHOLD_RATIO:.0%}；"
                 f"噪音带: median ± {MAD_K}·MAD；benchmark 噪音地板: "
                 f"{NOISE_FLOOR_MS}ms")
    lines.append("")
    return "\n".join(lines)


# ── CLI ───────────────────────────────────────────────────────────────────────

def main() -> int:
    try:
        sys.stdout.reconfigure(encoding="utf-8", errors="replace")
    except Exception:
        pass

    ap = argparse.ArgumentParser(description="Nightly regression triage")
    ap.add_argument("--report-dir", default="nightly-build-report",
                    help="nightly report root (holds baseline/ and triage/)")
    ap.add_argument("--against", default="latest",
                    help="baseline selector (only 'latest' supported today)")
    ap.add_argument("--window", type=int, default=5,
                    help="rolling window size for the noise band (default 5)")
    ap.add_argument("--stdout", action="store_true",
                    help="print report instead of writing to triage/")
    args = ap.parse_args()

    report_dir = Path(args.report_dir)
    result = analyze(report_dir, against=args.against, window=args.window)
    report = render_markdown(result)

    if args.stdout:
        print(report)
        return 0

    out_dir = report_dir / "triage"
    out_dir.mkdir(parents=True, exist_ok=True)
    stamp = datetime.now(timezone.utc).strftime("%Y%m%d")
    out_path = out_dir / f"triage-{stamp}.md"
    out_path.write_text(report, encoding="utf-8")
    print(report)
    print(f"\n(written to {out_path})")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
