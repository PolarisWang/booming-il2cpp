"""Nightly build entry point — run all assemblies through all stages.

Orchestrates:
  1. Discover all assemblies + chunks under testing/foundation-dll/
  2. Run build + fact + benchmark + hotupdate for all chunks (check config)
  3. [Optional] Rebuild with profile preset and collect profile data
  4. Aggregate per-assembly (serial)
  5. Reporting (cross-dll-dashboard, pipeline-runs)
  6. Nightly delta (cross-day comparison)
  7. Nightly summary (Markdown)

Usage:
    python -m verification.nightly_build [--max-workers 8] [--bench-workers auto]
    python -m verification.nightly_build --run-profile  # includes profile pass
"""

from __future__ import annotations

import json
import os
import shutil
import subprocess
import sys
import tempfile
import time
from concurrent.futures import ProcessPoolExecutor, as_completed, TimeoutError as FuturesTimeoutError
from datetime import datetime, timezone
from multiprocessing import Manager
from pathlib import Path
from typing import Any

# Ensure testing/foundation-dll is on sys.path for verification. imports
_HERE = Path(__file__).resolve().parent
_FOUNDATION_DLL = _HERE.parent                                # .../testing/foundation-dll/
_TESTING_DIR = _HERE.parent.parent                            # .../testing/
_REPO_ROOT = _HERE.parent.parent.parent                       # repo root
if str(_FOUNDATION_DLL) not in sys.path:
    sys.path.insert(0, str(_FOUNDATION_DLL))
if str(_TESTING_DIR) not in sys.path:
    sys.path.insert(0, str(_TESTING_DIR))

# SDK auto-build: ensure prebuilt native runtime libs are available
from _pipeline.tool_helpers import ensure_sdk
try:
    ensure_sdk(_REPO_ROOT)
    print("[nightly-build] SDK ready")
except RuntimeError as e:
    print(f"[nightly-build] WARNING: SDK not available: {e}")
    print("[nightly-build] Will attempt codegen inline (may fail if no prebuilt libs)")


from verification.orchestration.context import ChunkContext, StageResult
from verification.orchestration.discovery import discover_chunks
from verification.stages.hephaestus_cache import HephaestusCache, compute_input_hash
from verification.stages.build import run_build
from verification.stages.fact_chunk import run_fact_chunk
from verification.stages.profile import run_profile
from verification.stages.benchmark_chunk import run_benchmark_chunk
from verification.stages.managed_benchmark import run_managed_benchmark
from verification.stages.hotupdate_chunk import run_hotupdate_chunk
from verification.stages.benchmark_report import run_benchmark_report
from verification.stages.coverage_audit import run_coverage_audit
from verification.stages.aggregate import run_aggregate
from verification.stages.reporting import run_reporting


_DEFAULT_BUILD_WORKERS = 2


def _detect_bench_workers() -> int:
    """Auto-detect benchmark concurrency based on available CPU cores.

    Formula: max(1, min(4, (cpu_count - 2) // 2))
    - Reserves 2 cores for OS + other build stages
    - Each benchmark worker needs ~2 cores (cache/memory sensitive)
    - Capped at 4 workers max to avoid excessive contention
    Falls back to 1 (serial) if detection fails.
    """
    try:
        cpus = os.cpu_count() or 4
        return max(1, min(4, (cpus - 2) // 2))
    except Exception:
        return 1


_DEFAULT_BENCH_WORKERS = _detect_bench_workers()


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


def _restore_profile_cache_for(ctx: ChunkContext) -> StageResult:
    """Restore profile-tier entry.exe from hephaestus cache to native/.

    Uses temp dir + copy to avoid ETXTBUSY. Returns "passed" on success,
    "skipped" if no cache entry exists.  Interface matches the stage
    pipeline (ctx, stages) but stages is unused.

    On cache miss, falls back to a full profile-tier build so that
    --run-profile works on first run (no pre-seeded cache needed).
    """
    subjects_dll = ctx.subjects_dll_path
    metadata_path = ctx.subjects_metadata_path
    if not subjects_dll.exists() or not metadata_path.exists():
        return StageResult(stage="build", status="skipped",
                           summary="subjects DLL/metadata not found")

    input_hash = compute_input_hash(subjects_dll, metadata_path, ctx.assembly)
    cache_obj = HephaestusCache(ctx.foundation_dir, verbose=ctx.verbose)
    cache_key = cache_obj.compute_key(input_hash, ctx.assembly, ctx.slug)
    cache_key = f"{cache_key}/profile"

    if not cache_obj.is_cache_hit(cache_key):
        print(f"  [nightly] [{ctx.assembly}/{ctx.slug}] profile cache MISS — "
              f"falling back to full profile build")
        return run_build(ctx, {})

    exe = ctx.entry_exe_path
    if exe.exists():
        exe.unlink()
        if ctx.verbose:
            print(f"  [nightly] [{ctx.assembly}/{ctx.slug}] removed old entry.exe")

    with tempfile.TemporaryDirectory(prefix="profile-restore-") as tmp:
        tmp_path = Path(tmp)
        if not cache_obj.restore_to(cache_key, tmp_path):
            return StageResult(stage="build", status="error",
                               summary="profile cache restore failed")
        tmp_exe = tmp_path / "entry.exe"
        if not tmp_exe.exists():
            return StageResult(stage="build", status="error",
                               summary="entry.exe missing from profile cache")
        shutil.copy2(tmp_exe, exe)
        exe_size = exe.stat().st_size
        if ctx.verbose:
            print(f"  [nightly] [{ctx.assembly}/{ctx.slug}] restored profile entry.exe ({exe_size} bytes)")

    return StageResult(stage="build", status="passed",
                       summary=f"profile entry.exe restored ({exe_size} bytes)")


def _run_chunk_stages(
    assembly: str,
    slug: str,
    foundation_dir: Path,
    pipeline_config: dict,
    bench_semaphore: Any,
    native_config: str = "check",
    verbose: bool = False,
    stage_timeout: int = 0,
    profile_pass: bool = False,
    stages_filter: list[str] | None = None,
) -> tuple[str, dict[str, StageResult]]:
    """Run all stages for a single chunk returning (assembly/slug, {stage: result}).

    Normal pass (profile_pass=False): build → fact → profile → benchmark → hotupdate
    Profile pass (profile_pass=True): build → profile only (profile-tier rebuild)
    When stages_filter is set, only those stages run (e.g. ["build","fact"]).

    Build failure stops further stages for this chunk.
    Benchmark concurrency is controlled via bench_semaphore (default: 2).
    """
    try:
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

        # Profile pass: restore entry.exe from profile cache via temp dir
        # to avoid ETXTBUSY from overwriting a still-mapped executable.
        if profile_pass:
            exe = ctx.entry_exe_path
            if exe.exists():
                exe.unlink()
                if verbose:
                    print(f"  [nightly] [{assembly}/{slug}] removed {exe} for profile restore")

        stages: dict[str, StageResult] = {}
        if profile_pass:
            # Profile pass: restore profile cache + profile stage only
            stage_order = [
                ("build", lambda ctx, _stages: _restore_profile_cache_for(ctx)),
                ("profile", run_profile),
            ]
        elif stages_filter:
            stage_map = {
                "build": run_build,
                "fact": run_fact_chunk,
                "profile": run_profile,
                "benchmark": run_benchmark_chunk,
                "managed_benchmark": run_managed_benchmark,
                "hotupdate": run_hotupdate_chunk,
                "benchmark_report": run_benchmark_report,
                "coverage_audit": run_coverage_audit,
            }
            stage_order = [(s, stage_map[s]) for s in stages_filter if s in stage_map]
        else:
            stage_order = [
                ("build", run_build),
                ("fact", run_fact_chunk),
                ("profile", run_profile),
                ("benchmark", run_benchmark_chunk),
                ("managed_benchmark", run_managed_benchmark),
                ("hotupdate", run_hotupdate_chunk),
                ("benchmark_report", run_benchmark_report),
                ("coverage_audit", run_coverage_audit),
            ]

        for stage_name, stage_fn in stage_order:
            if verbose:
                print(f"\n  [nightly] [{assembly}/{slug}] {stage_name}...")
            try:
                if stage_name in ("benchmark", "managed_benchmark"):
                    if verbose:
                        print(f"  [nightly] [{assembly}/{slug}] waiting for benchmark semaphore...")
                    with bench_semaphore:
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
                    stage=stage_name, status="skipped",
                    summary=f"nightly exception: {e}",
                )
                if stage_name == "build":
                    break  # can't proceed without build

    except Exception as _e:
        if verbose:
            print(f"  [nightly] [{assembly}/{slug}] init EXCEPTION: {_e}")
        _s = {"build": StageResult(stage="build", status="skipped", summary=f"init: {_e}")}
        return f"{assembly}/{slug}", _s
    return f"{assembly}/{slug}", stages


def _collect_chunk_results(
    futures: list,
    total: int,
    verbose: bool = False,
    phase_label: str = "chunks",
) -> dict[str, dict[str, StageResult]]:
    """Collect futures results, handling errors gracefully. Shows ETA."""
    results: dict[str, dict[str, StageResult]] = {}
    completed = 0
    crashed = 0
    start_time = time.monotonic()
    last_print = 0.0
    for future in as_completed(futures, timeout=max(600, total * 120)):
        completed += 1
        try:
            key, stages = future.result()
            results[key] = stages
            build_st = stages.get("build", StageResult(stage="build", status="?")).status
        except SystemExit:
            # Worker process called sys.exit() — likely a subprocess crash (OOM, segfault)
            crashed += 1
            key = f"<crashed worker #{completed}>"
            results[key] = {"build": StageResult(stage="build", status="failed",
                             summary="worker process crashed (OOM/segfault)")}
            build_st = "crashed"
        except FuturesTimeoutError:
            completed -= 1  # timeout doesn't consume a slot
            crashed += 1
            key = f"<timeout worker #{completed+1}>"
            results[key] = {"build": StageResult(stage="build", status="skipped",
                             summary="worker timeout (stage > max wait)")}
            build_st = "timeout"
        except BaseException as e:
            key = f"<future #{completed}>"
            results[key] = {"build": StageResult(stage="build", status="skipped",
                             summary=f"future exception: {e}")}
            build_st = "exception"

        now = time.monotonic()
        elapsed = now - start_time
        pct = completed / total * 100
        rate = completed / max(elapsed, 0.1)
        eta = (total - completed) / max(rate, 0.1)
        eta_str = f"{eta:.0f}s" if eta < 3600 else f"{eta/60:.0f}m"
        if verbose or completed == total or (now - last_print >= 5.0):
            print(f"  [{phase_label}] [{completed}/{total} {pct:.0f}%] "
                  f"{key.split('/')[-1] if '/' in key else key}: "
                  f"build={build_st}, elapsed={elapsed:.0f}s, ETA={eta_str}")
            last_print = now
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
                        help=f"Max parallel benchmark workers (default: auto={_DEFAULT_BENCH_WORKERS}, formula: max(1, min(4, (cpu_count-2)//2)))")
    parser.add_argument("--native-config", default="check",
                        choices=["check", "profile", "ship"],
                        help="Native build config (default: check)")
    parser.add_argument("--stage-timeout", type=int, default=600,
                        help="Per-stage timeout in seconds (default: no timeout)")
    parser.add_argument("--verbose", "-v", action="store_true",
                        help="Verbose output")
    parser.add_argument("--no-profile", action="store_true",
                        help="Skip profile pass (profile is ON by default)")
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

    # ── Step 2: Phase A — Build + Fact only ──
    print(f"\n{'='*60}")
    print(f"  Phase A: Build + Fact ({len(all_chunks)} chunks, {args.max_workers} workers)")
    print(f"{'='*60}")
    sys.stdout.flush()

    bench_semaphore = Manager().Semaphore(args.bench_workers or 1)
    all_results: dict[str, dict[str, StageResult]] = {}

    phase_a_futures = []
    with ProcessPoolExecutor(max_workers=args.max_workers) as executor:
        for asm, slug, fdir in all_chunks:
            f = executor.submit(
                _run_chunk_stages,
                assembly=asm, slug=slug, foundation_dir=fdir,
                pipeline_config=pipeline_config,
                bench_semaphore=bench_semaphore,
                native_config=args.native_config,
                verbose=args.verbose,
                stage_timeout=args.stage_timeout,
                stages_filter=["build", "fact"],
            )
            phase_a_futures.append(f)

        all_results = _collect_chunk_results(phase_a_futures, len(all_chunks),
                                             verbose=args.verbose, phase_label="A:build+fact")

    phase_a_passed = sum(
        1 for r in all_results.values()
        if r.get("build", StageResult(stage="build", status="?")).status == "passed"
    )
    print(f"\n  Phase A complete: {phase_a_passed}/{len(all_chunks)} build passed")

    # ── Step 3: Phase B — Benchmark + Coverage (only for passed chunks) ──
    passed_chunks = [
        (asm, slug, fdir) for asm, slug, fdir in all_chunks
        if all_results.get(f"{asm}/{slug}", {}).get("build",
            StageResult(stage="build", status="?")).status == "passed"
    ]

    if passed_chunks:
        print(f"\n{'='*60}")
        print(f"  Phase B: Benchmark + Coverage ({len(passed_chunks)} chunks)")
        print(f"{'='*60}")

        phase_b_stages = [
            "benchmark", "managed_benchmark", "hotupdate",
            "benchmark_report", "coverage_audit",
        ]
        phase_b_futures = []
        with ProcessPoolExecutor(max_workers=args.max_workers) as executor:
            for asm, slug, fdir in passed_chunks:
                f = executor.submit(
                    _run_chunk_stages,
                    assembly=asm, slug=slug, foundation_dir=fdir,
                    pipeline_config=pipeline_config,
                    bench_semaphore=bench_semaphore,
                    native_config=args.native_config,
                    verbose=args.verbose,
                    stage_timeout=args.stage_timeout,
                    stages_filter=phase_b_stages,
                )
                phase_b_futures.append(f)

            phase_b_results = _collect_chunk_results(
                phase_b_futures, len(passed_chunks),
                verbose=args.verbose, phase_label="B:bench+coverage",
            )
            for key, stages in phase_b_results.items():
                if key in all_results:
                    all_results[key].update(stages)
                else:
                    all_results[key] = stages
    else:
        print("\n  Phase B skipped: no chunks passed build in Phase A")

    # Count successes/failures
    build_ok = sum(
        1 for r in all_results.values()
        if r.get("build", StageResult(stage="build", status="?")).status in ("passed", "skipped")
    )
    print(f"\n  Build: {build_ok}/{len(all_results)} passed")

    # ── Phase 2 (optional): Profile pass with profile-tier build ──
    if not args.no_profile:
        print(f"\n{'='*60}")
        print(f"  Phase 2: Profile pass (native_config=profile)...")
        print(f"{'='*60}")
        # Profile pass: rebuild entry.exe with PROFILE tier, then collect profile data.
        # Uses lower concurrency since rebuild is expensive.
        profile_workers = max(1, args.max_workers // 2)
        print(f"  Profile workers: {profile_workers}")
        profile_futures = []
        with ProcessPoolExecutor(max_workers=profile_workers) as executor:
            for asm, slug, fdir in all_chunks:
                future = executor.submit(
                    _run_chunk_stages,
                    assembly=asm, slug=slug, foundation_dir=fdir,
                    pipeline_config=pipeline_config,
                    bench_semaphore=bench_semaphore,
                    native_config="profile",
                    verbose=args.verbose,
                    stage_timeout=args.stage_timeout,
                    profile_pass=True,
                )
                profile_futures.append(future)

        profile_results = _collect_chunk_results(profile_futures, len(all_chunks), verbose=args.verbose)

        profile_ok = sum(
            1 for r in profile_results.values()
            if r.get("profile", StageResult(stage="profile", status="?")).status == "passed"
        )
        print(f"\n  Profile: {profile_ok}/{len(profile_results)} chunks profiled")

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
        print(f"  WARN: {failed_builds} chunk(s) failed")
    print(f"{'='*60}\n")

    return 1 if failed_builds > 0 else 0


if __name__ == "__main__":
    raise SystemExit(main())

