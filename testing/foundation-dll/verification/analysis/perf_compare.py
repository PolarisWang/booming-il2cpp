"""Horizontal comparison engine — cross-technology, cross-configuration benchmark analysis."""

from __future__ import annotations

import json
import math
from dataclasses import dataclass
from pathlib import Path
from typing import Any

from verification.orchestration.context import FamilyContext
from verification.analysis.perf_store import (
    BenchmarkQuery,
    query_benchmarks,
    query_latest_benchmarks,
    query_microbench,
)


def _store_dir(ctx: FamilyContext) -> Path:
    return ctx.results_dir / "perf"


def _load_jsonl(path: Path) -> list[dict[str, Any]]:
    if not path.exists():
        return []
    records = []
    with open(path, "r", encoding="utf-8") as f:
        for line in f:
            line = line.strip()
            if line:
                try:
                    records.append(json.loads(line))
                except json.JSONDecodeError:
                    pass
    return records


def _load_method_subject_ids(ctx: FamilyContext) -> list[str]:
    cp = ctx.contract_path
    if not cp.exists():
        return []
    try:
        c = json.loads(cp.read_text(encoding="utf-8"))
        mids = c.get("methodSubjectIds", [])
        if not mids:
            mids = [m["methodSubjectId"] for m in c.get("methodContracts", []) if m.get("methodSubjectId")]
        return mids
    except Exception:
        return []


TECHNOLOGY_NAMES: dict[str, str] = {
    "net8-jit": ".NET 8 JIT",
    "net10-jit": ".NET 10 JIT",
    "chaos-aot": "IL2CPP AOT",
    "chaos-jit": "IL2CPP JIT",
    "chaos-hu-aot": "HotUpdate AOT",
    "chaos-hu-jit": "HotUpdate JIT",
}

DEFAULT_TECHNOLOGIES = [
    "chaos-aot",
    "chaos-jit",
    "chaos-hu-aot",
    "chaos-hu-jit",
]


@dataclass
class MethodComparisonRow:
    method_index: int
    method_subject_id: str
    label: str
    values: dict[str, float]
    statuses: dict[str, str]


def _short_label(mid: str) -> str:
    if "::" in mid:
        return mid.rsplit("::", 1)[-1]
    return mid


def compare_across_technologies(
    ctx: FamilyContext,
    technologies: list[str] | None = None,
) -> list[MethodComparisonRow]:
    techs = technologies or DEFAULT_TECHNOLOGIES
    mids = _load_method_subject_ids(ctx)
    if not mids:
        return []

    tech_data: dict[str, list[dict[str, Any]]] = {}
    for tech in techs:
        latest = query_latest_benchmarks(ctx, tech)
        if latest:
            tech_data[tech] = latest

    rows: list[MethodComparisonRow] = []
    for i, mid in enumerate(mids):
        values: dict[str, float] = {}
        statuses: dict[str, str] = {}
        for tech in techs:
            records = tech_data.get(tech, [])
            rec = next((r for r in records if r.get("methodIndex") == i), None)
            if rec:
                ops = rec.get("metrics", {}).get("opsPerSecond", 0)
                values[tech] = ops
                statuses[tech] = rec.get("status", "completed")
            else:
                values[tech] = 0.0
                statuses[tech] = "missing"

        rows.append(MethodComparisonRow(
            method_index=i,
            method_subject_id=mid,
            label=_short_label(mid),
            values=values,
            statuses=statuses,
        ))

    return rows


def print_comparison_table(
    rows: list[MethodComparisonRow],
    technologies: list[str] | None = None,
    top_n: int = 0,
) -> None:
    if not rows:
        print("No comparison data available.")
        return

    techs = technologies or list(rows[0].values.keys())
    if not techs:
        return

    header = f"{'Method':45s}" + "".join(f"{TECHNOLOGY_NAMES.get(t, t):>16s}" for t in techs)
    print(header)
    print("-" * len(header))

    for row in rows:
        if top_n > 0 and row.method_index >= top_n:
            break
        label = row.label[:44]
        cells = [f"{row.values.get(t, 0):>15.0f}" if row.statuses.get(t) == "completed"
                 else f"{'N/A':>15s}"
                 for t in techs]
        print(f"{label:45s}" + "".join(cells))


def compute_ratios(
    rows: list[MethodComparisonRow],
    numerator_tech: str = "chaos-aot",
    denominator_tech: str = "chaos-jit",
) -> list[dict[str, Any]]:
    ratios = []
    for row in rows:
        num = row.values.get(numerator_tech, 0)
        den = row.values.get(denominator_tech, 0)
        if num > 0 and den > 0:
            ratios.append({
                "method_index": row.method_index,
                "label": row.label,
                "numerator_ops": num,
                "denominator_ops": den,
                "ratio": num / den,
            })
    return ratios


def geometric_mean(values: list[float]) -> float:
    if not values:
        return 0.0
    log_sum = sum(math.log(v) for v in values if v > 0)
    return math.exp(log_sum / len(values)) if log_sum else 0.0


@dataclass
class RegressionAlert:
    method_index: int
    method_subject_id: str
    technology: str
    metric: str
    previous_value: float
    current_value: float
    delta_pct: float
    severity: str


def detect_regression(
    ctx: FamilyContext,
    technology: str = "chaos-aot",
    threshold_pct: float = 10.0,
) -> list[RegressionAlert]:
    path = _store_dir(ctx) / "benchmark-history.jsonl"
    records = _load_jsonl(path)
    records = [r for r in records if r.get("technology") == technology]
    if not records:
        return []

    from itertools import groupby
    records.sort(key=lambda r: r.get("timestamp", ""))
    groups: list[tuple[str, list[dict[str, Any]]]] = []
    for ts, grp in groupby(records, key=lambda r: r.get("timestamp", "")):
        groups.append((ts, list(grp)))

    if len(groups) < 2:
        return []

    prev_ts, prev_records = groups[-2]
    cur_ts, cur_records = groups[-1]

    prev_map: dict[int, dict[str, Any]] = {
        r.get("methodIndex", 0): r for r in prev_records
    }
    cur_map: dict[int, dict[str, Any]] = {
        r.get("methodIndex", 0): r for r in cur_records
    }

    alerts: list[RegressionAlert] = []
    all_indices = set(prev_map.keys()) | set(cur_map.keys())
    for idx in sorted(all_indices):
        prev = prev_map.get(idx)
        cur = cur_map.get(idx)
        if not prev or not cur:
            continue
        prev_ops = prev.get("metrics", {}).get("opsPerSecond", 0)
        cur_ops = cur.get("metrics", {}).get("opsPerSecond", 0)
        if prev_ops <= 0 or cur_ops <= 0:
            continue

        delta_pct = (cur_ops - prev_ops) / prev_ops * 100
        if abs(delta_pct) < threshold_pct:
            severity = "unchanged"
        elif delta_pct > 0:
            severity = "improvement"
        else:
            severity = "regression"

        alerts.append(RegressionAlert(
            method_index=idx,
            method_subject_id=prev.get("methodSubjectId", ""),
            technology=technology,
            metric="opsPerSecond",
            previous_value=prev_ops,
            current_value=cur_ops,
            delta_pct=round(delta_pct, 1),
            severity=severity,
        ))

    return alerts


def print_regression_table(alerts: list[RegressionAlert]) -> None:
    if not alerts:
        print("No regression data.")
        return

    regressions = [a for a in alerts if a.severity == "regression"]
    improvements = [a for a in alerts if a.severity == "improvement"]
    unchanged = [a for a in alerts if a.severity == "unchanged"]

    print(f"Regression alerts: {len(regressions)} regressions, "
          f"{len(improvements)} improvements, {len(unchanged)} unchanged")

    if regressions:
        print(f"\n{'REGRESSIONS':=^60}")
        header = f"{'Method':45s} {'Before':>12s} {'After':>12s} {'Delta':>8s}"
        print(header)
        print("-" * len(header))
        for a in sorted(regressions, key=lambda x: x.delta_pct):
            label = _short_label(a.method_subject_id)[:44]
            print(f"{label:45s} {a.previous_value:>12.0f} {a.current_value:>12.0f} {a.delta_pct:>7.1f}%")

    if improvements:
        print(f"\n{'IMPROVEMENTS':=^60}")
        for a in sorted(improvements, key=lambda x: -x.delta_pct)[:10]:
            label = _short_label(a.method_subject_id)[:44]
            print(f"{label:45s} {a.previous_value:>12.0f} {a.current_value:>12.0f} +{a.delta_pct:>6.1f}%")


@dataclass
class TechnologySummary:
    technology: str
    display_name: str
    method_count: int
    ok_count: int
    geometric_mean_ops: float
    min_ops: float
    max_ops: float
    total_ops: float


def summarize_technologies(
    ctx: FamilyContext,
    technologies: list[str] | None = None,
) -> list[TechnologySummary]:
    techs = technologies or DEFAULT_TECHNOLOGIES
    summaries: list[TechnologySummary] = []

    for tech in techs:
        latest = query_latest_benchmarks(ctx, tech)
        if not latest:
            summaries.append(TechnologySummary(
                technology=tech,
                display_name=TECHNOLOGY_NAMES.get(tech, tech),
                method_count=0,
                ok_count=0,
                geometric_mean_ops=0.0,
                min_ops=0.0,
                max_ops=0.0,
                total_ops=0.0,
            ))
            continue

        ops_list = [
            r.get("metrics", {}).get("opsPerSecond", 0)
            for r in latest if r.get("status") == "completed" and r.get("metrics", {}).get("opsPerSecond", 0) > 0
        ]
        summaries.append(TechnologySummary(
            technology=tech,
            display_name=TECHNOLOGY_NAMES.get(tech, tech),
            method_count=len(latest),
            ok_count=len(ops_list),
            geometric_mean_ops=round(geometric_mean(ops_list), 0),
            min_ops=min(ops_list) if ops_list else 0.0,
            max_ops=max(ops_list) if ops_list else 0.0,
            total_ops=sum(ops_list),
        ))

    return summaries


def print_summary_table(summaries: list[TechnologySummary]) -> None:
    if not summaries:
        print("No data.")
        return

    header = f"{'Technology':20s} {'Methods':>8s} {'OK':>6s} {'GeoMean ops/s':>16s} {'Min':>12s} {'Max':>12s}"
    print(header)
    print("-" * len(header))
    for s in summaries:
        name = s.display_name[:19]
        print(f"{name:20s} {s.method_count:>8d} {s.ok_count:>6d} "
              f"{s.geometric_mean_ops:>16.0f} {s.min_ops:>12.0f} {s.max_ops:>12.0f}")
