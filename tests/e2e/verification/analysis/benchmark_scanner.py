"""Benchmark scanner — Phase 0 of optimization campaign.

Scans all chunks across all assemblies for benchmark data and generates
a priority-ordered optimization candidate report.

Usage:
    python -m verification.analysis.benchmark_scanner --output scan-report.json
    python -m verification.analysis.benchmark_scanner --priority-sort --output priority.json
"""

from __future__ import annotations

import json
import time
from pathlib import Path

import sys as _b_sys
from pathlib import Path as _b_Path
for _b_d in _b_Path(__file__).resolve().parents:
    if (_b_d / "_path.py").exists():
        if str(_b_d) not in _b_sys.path:
            _b_sys.path.insert(0, str(_b_d))
        break
from _path import foundation_root

_FOUNDATION_DLL = foundation_root()


def _find_all_chunks() -> list[tuple[str, str]]:
    """Find all (assembly, chunk_slug) pairs in foundation-dll."""
    chunks = []
    for assembly_dir in sorted(_FOUNDATION_DLL.iterdir()):
        if not assembly_dir.is_dir() or assembly_dir.name.startswith("."):
            continue
        chunks_dir = assembly_dir / "chunks"
        if not chunks_dir.is_dir():
            continue
        for chunk_dir in sorted(chunks_dir.iterdir()):
            if chunk_dir.is_dir():
                chunks.append((assembly_dir.name, chunk_dir.name))
    return chunks


def _load_benchmark(assembly: str, chunk: str) -> dict | None:
    """Load benchmark.json for a chunk, or None."""
    path = _FOUNDATION_DLL / assembly / "chunks" / chunk / "results" / "benchmark.json"
    if path.exists():
        try:
            return json.loads(path.read_text(encoding="utf-8"))
        except Exception:
            return None
    return None


def _load_fact(assembly: str, chunk: str) -> dict | None:
    """Load fact.json for a chunk, or None."""
    path = _FOUNDATION_DLL / assembly / "chunks" / chunk / "results" / "fact.json"
    if path.exists():
        try:
            return json.loads(path.read_text(encoding="utf-8"))
        except Exception:
            return None
    return None


def _load_comparison(assembly: str, chunk: str) -> dict | None:
    """Load comparison.json for a chunk, or None."""
    path = _FOUNDATION_DLL / assembly / "chunks" / chunk / "results" / "comparison.json"
    if path.exists():
        try:
            return json.loads(path.read_text(encoding="utf-8"))
        except Exception:
            return None
    return None


def scan_all(output_path: str | None = None) -> list[dict]:
    """Scan all chunks and return benchmark data."""
    chunks = _find_all_chunks()
    results = []

    for assembly, chunk in chunks:
        bm = _load_benchmark(assembly, chunk)
        fact = _load_fact(assembly, chunk)
        comp = _load_comparison(assembly, chunk)

        entry = {
            "assembly": assembly,
            "chunk": chunk,
            "has_benchmark": bm is not None,
            "has_fact": fact is not None,
            "has_comparison": comp is not None,
        }

        if bm:
            s = bm.get("summary", {})
            pms = bm.get("perMethodStats", [])
            entry["bench_methods"] = bm.get("methodCount", 0)
            entry["bench_total_ms"] = s.get("elapsedMilliseconds", 0) or 0
            entry["bench_mean_ms"] = s.get("meanDurationMs", 0) or 0
            entry["bench_max_ms"] = s.get("maxDurationMs", 0) or 0
            entry["bench_alloc"] = s.get("totalAllocatedBytes", 0) or 0
            # Top 5 slowest
            slowest = sorted(pms, key=lambda x: x.get("meanDurationMs", 0), reverse=True)[:5]
            entry["top5_slowest_ms"] = [round(p.get("meanDurationMs", 0), 3) for p in slowest]

        if fact:
            entry["fact_passed"] = fact.get("passed", 0)
            entry["fact_total"] = fact.get("total", 0)

        if comp:
            methods = comp.get("methods", [])
            net8_count = sum(1 for m in methods if m.get("net8Ms"))
            chaos_count = sum(1 for m in methods if m.get("chaosAotMs"))
            entry["comparison_methods"] = len(methods)
            entry["comparison_net8"] = net8_count
            entry["comparison_chaos"] = chaos_count

        results.append(entry)

    if output_path:
        path = Path(output_path)
        path.parent.mkdir(parents=True, exist_ok=True)
        path.write_text(json.dumps(results, indent=2, ensure_ascii=False), encoding="utf-8")
        print(f"  [scanner] Wrote {len(results)} entries to {output_path}")

    return results


def priority_sort(results: list[dict]) -> list[dict]:
    """Sort by priority_score = bench_total_ms * bench_methods."""
    for r in results:
        r["priority_score"] = r.get("bench_total_ms", 0) * max(r.get("bench_methods", 0), 1)
    return sorted(results, key=lambda x: x.get("priority_score", 0), reverse=True)


def main():
    import argparse
    parser = argparse.ArgumentParser(description="Benchmark scanner")
    parser.add_argument("--output", default="benchmark-scan-report.json")
    parser.add_argument("--priority-sort", action="store_true")
    args = parser.parse_args()

    results = scan_all(args.output)
    if args.priority_sort:
        results = priority_sort(results)
        sorted_path = args.output.replace(".json", "-priority.json")
        Path(sorted_path).write_text(json.dumps(results, indent=2, ensure_ascii=False), encoding="utf-8")
        print(f"  [scanner] Priority-sorted: {sorted_path}")
        print()
        print("  Top 10 candidates:")
        for r in results[:10]:
            print(f"    [{r['chunk']:<20s}] score={r.get('priority_score',0):.0f}  bench={r.get('bench_methods',0)} methods  tot={r.get('bench_total_ms',0):.1f}ms")


if __name__ == "__main__":
    main()
