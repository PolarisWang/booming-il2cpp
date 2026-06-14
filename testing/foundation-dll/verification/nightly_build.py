"""Nightly build entry point — run all assemblies through all stages.

Orchestrates:
  1. Discover all assemblies + chunks under testing/foundation-dll/
  2. Run build + fact for all chunks in parallel (high concurrency)
  3. Run benchmark for all chunks with limited concurrency (benchmark isolation)
  4. Aggregate per-assembly (serial)
  5. Reporting (cross-dll-dashboard, pipeline-runs)
  6. Nightly delta (cross-day comparison)
  7. Nightly summary (Markdown)

Usage:
    python -m verification.nightly_build [--max-workers 8] [--bench-workers 1]
"""

from __future__ import annotations

import json
import os
import subprocess
import sys
import time
from concurrent.futures import ProcessPoolExecutor, as_completed
from datetime import datetime, timezone
from multiprocessing import Lock
from pathlib import Path
from typing import Any

# Ensure testing/foundation-dll is on sys.path for verification. imports
_HERE = Path(__file__).resolve().parent
_FOUNDATION_DLL = _HERE  # testing/foundation-dll/
if str(_FOUNDATION_DLL) not in sys.path:
    sys.path.insert(0, str(_FOUNDATION_DLL))

from verification.orchestration.context import ChunkContext, StageResult
from verification.orchestration.discovery import discover_chunks
from verification.stages.build import run_build
from verification.stages.fact_chunk import run_fact_chunk
from verification.stages.benchmark_chunk import run_benchmark_chunk
from verification.stages.hotupdate_chunk import run_hotupdate_chunk
from verification.stages.aggregate import run_aggregate
from verification.stages.reporting import run_reporting


_DEFAULT_BUILD_WORKERS = 4
_DEFAULT_BENCH_WORKERS = 1


def _discover_assemblies(foundation_dll: Path) -> list[str]:
    """Discover assembly directories that have chunk configurations."""
    assemblies: list[str] = []
    for entry in sorted(foundation_dll.iterdir()):
        if not entry.is_dir():
            continue
        name = entry.name
        if name.startswith("_") or name.startswith("."):
            continue
        # Must have _dll/namespace-partition.json (chunk config)
        partition = entry / "_dll" / "namespace-partition.json"
        if partition.exists():
            assemblies.append(name)
    return assemblies


def _load_pipeline_config(foundation_dll: Path) -> dict:
    """Load pipeline-config.yaml for chunk-specific settings."""
    try:
        import yaml
    except ImportError:
        return {}
    config_path = foundation_dll / "config" / "pipeline-config.yaml"
    if config_path.exists():
        try:
            with open(config_path, encoding="utf-8") as f:
                return yaml.safe_load(f) or {}
        except Exception:
            return {}
    return {}


def _run_chunk_stages(
    assembly: str,
    slug: str,
    foundation_dir: Path,
    pipeline_config: dict,
    bench_lock: Any,
    native_config: str = "check",
    verbose: bool = False,
    stage_timeout: int = 0,
) -> tuple[str, dict[str, StageResult]]:
    """Run all stages for a single chunk returning (assembly/slug, {stage: result}).

    Stages: build → fact → benchmark → hotupdate.
    Build failure stops further stages for this chunk.
    Benchmark is serialized via bench_lock to prevent CPU interference.
    """
    chunk_dir = foundation_dir / "chunks" / slug

    # Resolve assembly dirs from pipeline-config.yaml
    chunk_cfg = (pipeline_config.get("chunks") or {}).get(slug, {})
    assembly_dirs_str = (chunk_cfg.get("assemblyDirs") or "").strip()
    assembly_dirs = [d.strip() for d in assembly_dirs_str.split(",") if d.strip()] if assembly_dirs_str else []

    # Get git info for provenance
    try:
        git_commit = subprocess.run(
            ["git", "rev-parse", "HEAD"], capture_output=True, text=True, timeout=10
        ).stdout.strip()
        git_branch = subprocess.run(
            ["git", "rev-parse", "--abbrev-ref", "HEAD"], capture_output=True, text=True, timeout=10
        ).stdout.strip()
    except Exception:
        git_commit = ""
        git_branch = ""

    run_id = f"fdn-{datetime.now(timezone.utc).strftime('%Y%m%d')}-nightly"

    ctx = ChunkContext(
        slug=slug,
        assembly=assembly,
        chunk_dir=chunk_dir,
        foundation_dir=foundation_dir,
        mode="standard",
        native_config=native_config,
        verbose=verbose,
        stage_timeout_seconds=stage_timeout,
        assembly_dirs=assembly_dirs,
        run_id=run_id,
        git_commit=git_commit,
        git_branch=git_branch,
        platform=sys.platform,
    )

    stages: dict[str, StageResult] = {}
    stage_order = [
        ("build", run_build),
        ("fact", run_fact_chunk),
        ("benchmark", run_benchmark_chunk),
        ("hotupdate", run_hotupdate_chunk),
    ]

    for stage_name, stage_fn in stage_order:
        if verbose:
            print(f"\n  [nightly] [{assembly}/{slug}] {stage_name}...")
        try:
            if stage_name == "benchmark":
                # Benchmark must be serialized — acquire lock
                if verbose:
                    print(f"  [nightly] [{assembly}/{slug}] waiting for benchmark lock...")
                with bench_lock:
                    result = stage_fn(ctx, stages)
            else:
                result = stage_fn(ctx, stages)

            stages[stage_name] = result

            # If build fails, skip remaining stages for this chunk
            if stage_name == "build" and result.status in ("failed", "error"):
                if verbose:
                    print(f"  [nightly] [{assembly}/{slug}] build failed, skipping fact/benchmark/hotupdate")
                break

        except Exception as e:
            if verbose:
                print(f"  [nightly] [{assembly}/{slug}] {stage_name} EXCEPTION: {e}")
            stages[stage_name] = StageResult(
                stage=stage_name, status="error",
                summary=f"nightly exception: {e}",
            )
            if stage_name == "build":
                break  # can't proceed without build

    return f"{assembly}/{slug}", stages


def _collect_chunk_results(
    futures: list,
    total: int,
    verbose: bool = False,
) -> dict[str, dict[str, StageResult]]:
    """Collect futures results, handling errors gracefully."""
    results: dict[str, dict[str, StageResult]] = {}
    completed = 0
    for future in as_completed(futures):
        completed += 1
        try:
            key, stages = future.result()
            results[key] = stages
            status = stages.get("build", StageResult(stage="build", status="?")).status
            if verbose:
                print(f"  [nightly] [{completed}/{total}] {key}: build={status}, "
                      f"fact={stages.get('fact', StageResult(stage='fact', status='?')).status}")
        except Exception as e:
            key = f"<future #{completed}>"
            results[key] = {"build": StageResult(stage="build", status="error", summary=f"future exception: {e}")}
            if verbose:
                print(f"  [nightly] [{completed}/{total}] {key}: future EXCEPTION: {e}")
    return results


def main() -> int:
    import argparse

    parser = argparse.ArgumentParser(
        description="Nightly build entry point — run all assemblies through all stages",
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog=__doc__,
    )
    parser.add_argument("--foundation-dll", default=None,
                        help="Path to testing/foundation-dll/ (default: auto-detect)")
    parser.add_argument("--assembly", default=None,
                        help="Run a single assembly only (default: all)")
    parser.add_argument("--max-workers", type=int, default=_DEFAULT_BUILD_WORKERS,
                        help=f"Max parallel build/fact workers (default: {_DEFAULT_BUILD_WORKERS})")
    parser.add_argument("--bench-workers", type=int, default=_DEFAULT_BENCH_WORKERS,
                        help=f"Max parallel benchmark workers (default: {_DEFAULT_BENCH_WORKERS})")
    parser.add_argument("--native-config", default="check",
                        choices=["check", "profile", "ship"],
                        help="Native build config (default: check)")
    parser.add_argument("--stage-timeout", type=int, default=0,
                        help="Per-stage timeout in seconds (default: no timeout)")
    parser.add_argument("--verbose", "-v", action="store_true",
                        help="Verbose output")
    parser.add_argument("--skip-nightly-report", action="store_true",
                        help="Skip delta and summary generation at the end")
    parser.add_argument("--output-dir", default=None,
                        help="Output directory for nightly reports (default: results/nightly/)")
    args = parser.parse_args()

    overall_start = time.perf_counter()
    foundation_dll = Path(args.foundation_dll) if args.foundation_dll else _FOUNDATION_DLL
    output_dir = Path(args.output_dir) if args.output_dir else foundation_dll.parent / "results" / "nightly"

    # ── Step 1: Discover assemblies + chunks ──
    print(f"\n{'='*60}")
    print(f"  Nightly Build — {datetime.now(timezone.utc).strftime('%Y-%m-%d %H:%M:%S UTC')}")
    print(f"{'='*60}")

    assemblies = [args.assembly] if args.assembly else _discover_assemblies(foundation_dll)
    if not assemblies:
        print("ERROR: No assemblies found")
        return 1
    print(f"\n  Discovered {len(assemblies)} assemblies")

    pipeline_config = _load_pipeline_config(foundation_dll)

    all_chunks: list[tuple[str, str, Path]] = []  # (assembly, slug, foundation_dir)
    for asm in assemblies:
        chunks = discover_chunks(asm, foundation_dir=foundation_dll / asm)
        if not chunks:
            print(f"  WARNING: No chunks for {asm}")
        for slug in chunks:
            all_chunks.append((asm, slug, foundation_dll / asm))

    if not all_chunks:
        print("ERROR: No chunks found across all assemblies")
        return 1
    print(f"  Total chunks: {len(all_chunks)}")

    # ── Step 2: Run build + fact + benchmark + hotupdate in parallel ──
    print(f"\n  Phase 1-2: Running chunks (build={args.max_workers} workers, "
          f"bench={args.bench_workers} workers)...")

    bench_lock = Lock()
    all_results: dict[str, dict[str, StageResult]] = {}
    futures = []

    with ProcessPoolExecutor(max_workers=args.max_workers) as executor:
        for asm, slug, fdir in all_chunks:
            future = executor.submit(
                _run_chunk_stages,
                assembly=asm,
                slug=slug,
                foundation_dir=fdir,
                pipeline_config=pipeline_config,
                bench_lock=bench_lock,
                native_config=args.native_config,
                verbose=args.verbose,
                stage_timeout=args.stage_timeout,
            )
            futures.append(future)

        all_results = _collect_chunk_results(futures, len(all_chunks), verbose=args.verbose)

    # Count successes/failures
    build_ok = sum(
        1 for r in all_results.values()
        if r.get("build", StageResult(stage="build", status="?")).status in ("passed",)
    )
    print(f"\n  Build: {build_ok}/{len(all_results)} passed")

    # ── Step 3: Aggregate per-assembly ──
    print(f"\n  Phase 3: Aggregating per-assembly...")
    for asm in assemblies:
        fdir = foundation_dll / asm
        # Create a temporary aggregate context
        agg_ctx = ChunkContext(
            slug="__aggregate__",
            assembly=asm,
            chunk_dir=fdir / "chunks" / "__aggregate__",
            foundation_dir=fdir,
            mode="standard",
            native_config=args.native_config,
            verbose=args.verbose,
        )
        try:
            agg_result = run_aggregate(agg_ctx, {})
            print(f"    {asm}: aggregate {agg_result.status}")
        except Exception as e:
            print(f"    {asm}: aggregate EXCEPTION: {e}")

    # ── Step 4: Reporting ──
    print(f"\n  Phase 4: Reporting...")
    for asm in assemblies:
        fdir = foundation_dll / asm
        report_ctx = ChunkContext(
            slug="__report__",
            assembly=asm,
            chunk_dir=fdir / "chunks" / "__report__",
            foundation_dir=fdir,
            mode="standard",
            native_config=args.native_config,
            verbose=args.verbose,
        )
        try:
            report_result = run_reporting(report_ctx, {})
            print(f"    {asm}: reporting {report_result.status}")
        except Exception as e:
            print(f"    {asm}: reporting EXCEPTION: {e}")

    # ── Step 5: Nightly delta + summary ──
    if not args.skip_nightly_report:
        print(f"\n  Phase 5: Nightly report...")
        output_dir.mkdir(parents=True, exist_ok=True)

        # Run delta
        try:
            from verification.nightly_delta import compute_nightly_delta

            today = datetime.now(timezone.utc).strftime("%Y-%m-%d")
            delta = compute_nightly_delta(foundation_dll, today=today)
            delta_path = output_dir / "nightly-delta.json"
            delta_path.write_text(json.dumps(delta, indent=2, ensure_ascii=False), encoding="utf-8")
            print(f"    Delta: {delta_path}")
            print(f"      {delta['overall']['totalAssemblies']} assemblies, "
                  f"{len(delta['regressions'])} regressions, "
                  f"{len(delta['improvements'])} improvements")

            # Run summary
            from verification.nightly_summary import generate_summary

            summary = generate_summary(delta)
            summary_path = output_dir / "nightly-summary.md"
            summary_path.write_text(summary, encoding="utf-8")
            print(f"    Summary: {summary_path}")

        except Exception as e:
            print(f"    Nightly report EXCEPTION: {e}")

    # ── Summary ──
    total_duration = time.perf_counter() - overall_start
    print(f"\n{'='*60}")
    print(f"  Nightly build complete ({total_duration:.0f}s)")
    failed_builds = len(all_results) - build_ok
    if failed_builds > 0:
        print(f"  ⚠️  {failed_builds} chunk(s) failed")
    print(f"{'='*60}\n")

    return 1 if failed_builds > 0 else 0


if __name__ == "__main__":
    raise SystemExit(main())
