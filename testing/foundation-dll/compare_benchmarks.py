"""Compare native AOT vs .NET 8 JIT / .NET 10 JIT benchmark performance.

Reads existing data:
  - chunks/{slug}/results/benchmark.json     → native AOT (chaos-aot)
  - results/.../{slug}/perf/benchmark-history.jsonl → managed (net8-jit, net10-jit)

Produces a full comparison table with per-chunk breakdown.
"""
from __future__ import annotations

import json
import math
import os
from pathlib import Path

CHUNKS_DIR = Path("D:/agent/booming-il2cpp/testing/foundation-dll/System.Private.CoreLib/chunks")
PERF_DIR = Path("D:/agent/booming-il2cpp/testing/foundation-dll/verification/results/foundation-dll/System.Private.CoreLib")


def _geometric_mean(values: list[float]) -> float:
    if not values:
        return 0.0
    log_sum = sum(math.log(v) for v in values if v > 0)
    return math.exp(log_sum / len(values)) if log_sum else 0.0


def load_native_aot(slug: str) -> dict | None:
    """Load native AOT benchmark data from benchmark.json."""
    path = CHUNKS_DIR / slug / "results" / "benchmark.json"
    if not path.exists():
        return None
    try:
        data = json.loads(path.read_text(encoding="utf-8"))
    except (json.JSONDecodeError, OSError):
        return None

    results = data.get("results", [])
    method_count = data.get("methodCount", 0)
    summary = data.get("summary", {})
    technology = data.get("technology", "chaos-aot")

    if not results or method_count == 0:
        return None

    ops_values = [
        r.get("opsPerSecond", 0) for r in results
        if isinstance(r.get("opsPerSecond"), (int, float)) and r["opsPerSecond"] > 0
    ]
    elapsed_values = [
        r.get("elapsedMilliseconds", 0) for r in results
        if isinstance(r.get("elapsedMilliseconds"), (int, float)) and r["elapsedMilliseconds"] > 0
    ]

    if not ops_values:
        return None

    return {
        "method_count": method_count,
        "ok_count": len(ops_values),
        "technology": technology,
        "gm_ops": _geometric_mean(ops_values),
        "mean_ops": summary.get("meanOpsPerSecond", sum(ops_values) / len(ops_values)),
        "min_ops": min(ops_values),
        "max_ops": max(ops_values),
        "mean_elapsed_ms": sum(elapsed_values) / len(elapsed_values) if elapsed_values else 0,
        "total_elapsed_ms": sum(elapsed_values),
    }


def load_managed(slug: str, technology: str) -> dict | None:
    """Load managed benchmark data for a specific technology (net8-jit or net10-jit)."""
    path = PERF_DIR / slug / "perf" / "benchmark-history.jsonl"
    if not path.exists():
        return None

    records = []
    try:
        with open(path, "r", encoding="utf-8") as f:
            for line in f:
                line = line.strip()
                if line:
                    records.append(json.loads(line))
    except (OSError, json.JSONDecodeError):
        return None

    # Filter by technology
    tech_records = [r for r in records if r.get("technology") == technology and r.get("status") == "completed"]
    if not tech_records:
        return None

    ops_values = []
    elapsed_values = []
    for r in tech_records:
        metrics = r.get("metrics", {})
        ops = metrics.get("opsPerSecond", 0)
        elapsed = metrics.get("elapsedMilliseconds", 0)
        if isinstance(ops, (int, float)) and ops > 0:
            ops_values.append(ops)
        if isinstance(elapsed, (int, float)) and elapsed > 0:
            elapsed_values.append(elapsed)

    if not ops_values:
        return None

    return {
        "method_count": len(tech_records),
        "ok_count": len(ops_values),
        "gm_ops": _geometric_mean(ops_values),
        "mean_ops": sum(ops_values) / len(ops_values),
        "mean_elapsed_ms": sum(elapsed_values) / len(elapsed_values) if elapsed_values else 0,
    }


def _format_ratio(native_gm: float, managed_gm: float, native_label: str) -> str:
    """Format ratio with indicator for which is faster."""
    if managed_gm <= 0:
        return "—"
    ratio = native_gm / managed_gm
    if ratio >= 1.0:
        return f"{ratio:.2f}x"
    else:
        return f"{ratio:.2f}x"


def main():
    all_chunks = sorted(os.listdir(CHUNKS_DIR))
    native_label = "chaos-aot"

    # Collect for overall summary
    all_native_gms: list[float] = []
    all_net8_gms: list[float] = []
    all_net10_gms: list[float] = []

    # Count stats
    total_native_methods = 0
    total_net8_methods = 0
    total_net10_methods = 0

    # Header
    print(f"{'Chunk':25s} | {'chaos-aot':>20s} | {'NET 8 JIT':>20s} | {'NET10 JIT':>20s} | {'AOT/NET8':>10s} | {'AOT/NET10':>10s} | {'Methods':>8s}")
    print("-" * 128)

    for slug in all_chunks:
        if not (CHUNKS_DIR / slug).is_dir():
            continue

        native = load_native_aot(slug)
        net8 = load_managed(slug, "net8-jit")
        net10 = load_managed(slug, "net10-jit")

        native_str = f"{native['gm_ops']:>.0f}" if native else "—"
        net8_str = f"{net8['gm_ops']:>.0f}" if net8 else "—"
        net10_str = f"{net10['gm_ops']:>.0f}" if net10 else "—"

        aot_vs_net8_str = _format_ratio(native["gm_ops"], net8["gm_ops"], native_label) if native and net8 and native["gm_ops"] > 0 and net8["gm_ops"] > 0 else "—"
        aot_vs_net10_str = _format_ratio(native["gm_ops"], net10["gm_ops"], native_label) if native and net10 and native["gm_ops"] > 0 and net10["gm_ops"] > 0 else "—"

        method_count = native["method_count"] if native else (net8["method_count"] if net8 else (net10["method_count"] if net10 else 0))
        methods_str = str(method_count) if method_count else "—"

        print(f"{slug:25s} | {native_str:>20s} | {net8_str:>20s} | {net10_str:>20s} | {aot_vs_net8_str:>10s} | {aot_vs_net10_str:>10s} | {methods_str:>8s}")

        if native:
            all_native_gms.append(native["gm_ops"])
            total_native_methods += native["method_count"]
        if net8:
            all_net8_gms.append(net8["gm_ops"])
            total_net8_methods += net8["method_count"]
        if net10:
            all_net10_gms.append(net10["gm_ops"])
            total_net10_methods += net10["method_count"]

    # Summary row
    print("-" * 128)
    overall_native = f"{_geometric_mean(all_native_gms):>.0f}" if all_native_gms else "—"
    overall_net8 = f"{_geometric_mean(all_net8_gms):>.0f}" if all_net8_gms else "—"
    overall_net10 = f"{_geometric_mean(all_net10_gms):>.0f}" if all_net10_gms else "—"

    if all_native_gms and all_net8_gms:
        overall_r8 = _geometric_mean(all_native_gms) / _geometric_mean(all_net8_gms)
        overall_r8_str = f"{overall_r8:.2f}x"
    else:
        overall_r8_str = "—"

    if all_native_gms and all_net10_gms:
        overall_r10 = _geometric_mean(all_native_gms) / _geometric_mean(all_net10_gms)
        overall_r10_str = f"{overall_r10:.2f}x"
    else:
        overall_r10_str = "—"

    print(f"{'** Overall **':25s} | {overall_native:>20s} | {overall_net8:>20s} | {overall_net10:>20s} | {overall_r8_str:>10s} | {overall_r10_str:>10s} | {total_native_methods:>8d}")

    # Notes
    print()
    print(f"  Coverage: {len(all_native_gms)}/22 chunks have native AOT data ({total_native_methods} methods)")
    print(f"            {len(all_net8_gms)}/22 chunks have NET 8 JIT data ({total_net8_methods} methods)")
    print(f"            {len(all_net10_gms)}/22 chunks have NET10 JIT data ({total_net10_methods} methods)")
    print()
    print("  chaos-aot: il2cpp AOT codegen (entry.exe --benchmark-all)")
    print("  NET 8 JIT: dotnet run --framework net8.0 (managed benchmark)")
    print("  NET10 JIT: dotnet run --framework net10.0 (managed benchmark)")
    print("  Ratio >1.0: AOT (chaos-aot) is faster")
    print("  Ratio <1.0: Managed JIT is faster")


if __name__ == "__main__":
    main()
