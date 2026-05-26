"""Fast managed benchmark runner — generate, build once, run directly.

Usage:
    python -m verification.entry_points.fast_managed_bench [--parallel N]
"""
from __future__ import annotations

import json
import subprocess
import sys
import time
from concurrent.futures import ThreadPoolExecutor, as_completed
from pathlib import Path

from verification._path import _HERE as _VERIFICATION_ROOT
from verification.stages.benchmark import (
    _generate_managed_harness,
    _load_method_subject_ids,
)
from verification.analysis.perf_store import save_managed_benchmark_records
from verification.orchestration.context import FamilyContext

_TESTING_ROOT = _VERIFICATION_ROOT.parent
ASSEMBLY = "System.Private.CoreLib"
TECH_TFMS = [("net10-jit", "net10.0"), ("net8-jit", "net8.0")]
ITERATIONS = 100000

SKIP_SLUGS = {
    "aggregation", "filtering", "immutable-array", "immutable-list", "reports",
    "interface-dispatch,pinvoke-dllimport,primitive-numeric-conversions-core",
}


def discover_families() -> list[str]:
    families_dir = _TESTING_ROOT / ASSEMBLY
    slugs = sorted([
        d.name for d in families_dir.iterdir()
        if d.is_dir() and (
            (d / "capability-family-contract.json").exists() or
            (d / "contract.json").exists()
        )
    ])
    return [s for s in slugs if s not in SKIP_SLUGS]


def run_one_family(slug: str) -> dict:
    """Generate harness, build and run for each TFM, save results."""
    result = {"slug": slug, "status": "ok"}
    family_dir = _TESTING_ROOT / ASSEMBLY / slug
    ctx = FamilyContext(slug=slug, assembly=ASSEMBLY, family_dir=family_dir)

    mids = _load_method_subject_ids(ctx)
    if not mids:
        result["status"] = "skipped"
        result["reason"] = "no methods"
        return result

    harness_dir = family_dir / "managed_test" / "benchmarks"
    harness_dir.mkdir(parents=True, exist_ok=True)

    print(f"[{slug}] Generating harness ({len(mids)} methods)...")
    if not _generate_managed_harness(harness_dir, slug, ASSEMBLY, mids, ITERATIONS):
        result["status"] = "failed"
        result["reason"] = "harness generation failed"
        return result

    csproj = harness_dir / "ManagedBenchmarkHarness.csproj"
    tech_results = {}

    for tech_key, tfm in TECH_TFMS:
        dll_path = harness_dir / "bin" / "Release" / tfm / "ManagedBenchmarkHarness.dll"

        print(f"[{slug}] Building ({tfm})...")
        build = subprocess.run(
            ["dotnet", "build", str(csproj), "-f", tfm,
             "--configuration", "Release", "--nologo", "-v", "q"],
            capture_output=True, text=True, timeout=120,
        )
        if build.returncode != 0:
            tech_results[tech_key] = {"status": "failed", "reason": f"build: {build.stderr[:200]}"}
            print(f"[{slug}] {tech_key} BUILD FAILED: {build.stderr[:200]}")
            continue

        print(f"[{slug}] Running benchmark ({tech_key})...")
        try:
            run = subprocess.run(
                ["dotnet", "exec", str(dll_path)],
                capture_output=True, text=True, timeout=120,
            )
        except subprocess.TimeoutExpired:
            tech_results[tech_key] = {"status": "failed", "reason": "timeout (120s)"}
            print(f"[{slug}] {tech_key} TIMEOUT")
            continue
        if run.returncode != 0:
            tech_results[tech_key] = {"status": "failed", "reason": f"exit={run.returncode}: {run.stderr[:200]}"}
            print(f"[{slug}] {tech_key} RUN FAILED: {run.stderr[:200]}")
            continue

        try:
            data = json.loads(run.stdout)
        except json.JSONDecodeError as e:
            tech_results[tech_key] = {"status": "failed", "reason": f"json: {e}"}
            print(f"[{slug}] {tech_key} JSON ERROR: {e}")
            continue

        method_results = data.get("results", [])
        for mr in method_results:
            ms = mr.get("elapsedMilliseconds", 0)
            mr["opsPerSecond"] = (ITERATIONS / (ms / 1000.0)) if ms > 0 else 0.0
            mr["status"] = "completed" if (mr.get("isBodyReal", False) or mr.get("opsPerSecond", 0) > 0) else "error"

        save_managed_benchmark_records(ctx, tech_key, method_results)

        ok = sum(1 for mr in method_results if mr.get("status") == "completed")
        tech_results[tech_key] = {"ok": ok, "total": len(method_results)}
        print(f"[{slug}] {tech_key}: {ok}/{len(method_results)} ok")

    if not tech_results:
        result["status"] = "failed"
        result["reason"] = "all techs failed"
        return result

    total_ok = sum(t.get("ok", 0) for t in tech_results.values())
    total_all = sum(t.get("total", 0) for t in tech_results.values())
    result["ok"] = total_ok
    result["total"] = total_all
    result["techs"] = {k: v for k, v in tech_results.items() if "ok" in v}
    return result


def main():
    import argparse
    parser = argparse.ArgumentParser()
    parser.add_argument("--parallel", type=int, default=1, help="Parallel workers")
    parser.add_argument("--family", default=None)
    args = parser.parse_args()

    if args.family:
        slugs = [args.family]
    else:
        slugs = discover_families()

    print(f"Running {len(slugs)} families, parallel={args.parallel}")

    results = []
    total_ok = 0
    total_all = 0
    failed = []
    start = time.perf_counter()
    tech_agg: dict[str, dict] = {}

    if args.parallel > 1:
        with ThreadPoolExecutor(max_workers=args.parallel) as ex:
            futs = {ex.submit(run_one_family, s): s for s in slugs}
            for fut in as_completed(futs):
                slug = futs[fut]
                try:
                    r = fut.result()
                except Exception as e:
                    r = {"slug": slug, "status": "failed", "reason": str(e)}
                results.append(r)
                if r["status"] == "ok":
                    total_ok += r.get("ok", 0)
                    total_all += r.get("total", 0)
                    for tk, tr in r.get("techs", {}).items():
                        agg = tech_agg.setdefault(tk, {"ok": 0, "total": 0})
                        agg["ok"] += tr["ok"]
                        agg["total"] += tr["total"]
                else:
                    failed.append(r)
                    print(f"[{slug}] FAILED: {r.get('reason', '?')}")
    else:
        for slug in slugs:
            r = run_one_family(slug)
            results.append(r)
            if r["status"] == "ok":
                total_ok += r.get("ok", 0)
                total_all += r.get("total", 0)
                for tk, tr in r.get("techs", {}).items():
                    agg = tech_agg.setdefault(tk, {"ok": 0, "total": 0})
                    agg["ok"] += tr["ok"]
                    agg["total"] += tr["total"]
            else:
                failed.append(r)

    elapsed = time.perf_counter() - start
    print(f"\n{'='*50}")
    print(f"Done: {len(results)} families in {elapsed:.0f}s")
    for tk, agg in sorted(tech_agg.items()):
        print(f"  {tk}: {agg['ok']}/{agg['total']} ok")
    if failed:
        print(f"Failed: {len(failed)} families:")
        for f in failed:
            print(f"  {f['slug']}: {f.get('reason', '?')}")
    print(f"{'='*50}")


if __name__ == "__main__":
    main()
