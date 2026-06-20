"""Orchestrator — parallel execution engine for nightly builds.

Extracted from `nightly_build.py` to provide a reusable, configurable
execution engine with logging injection and structured results.
"""
from __future__ import annotations

import json
import os
import subprocess
import sys
import time
from concurrent.futures import ProcessPoolExecutor, as_completed
from datetime import datetime, timezone
from multiprocessing import Manager
from pathlib import Path
from typing import Any

# Ensure testing/ and testing/foundation-dll/ are on sys.path so
# _pipeline.tool_helpers and verification.* imports resolve correctly.
_HERE = Path(__file__).resolve().parent                           # verification/nightly_runner/
_FOUNDATION_DLL = _HERE.parent.parent                              # testing/foundation-dll/
_TESTING_DIR = _FOUNDATION_DLL.parent                              # testing/
_REPO_ROOT = _TESTING_DIR.parent                                   # repo root
if str(_FOUNDATION_DLL) not in sys.path:
    sys.path.insert(0, str(_FOUNDATION_DLL))
if str(_TESTING_DIR) not in sys.path:
    sys.path.insert(0, str(_TESTING_DIR))

from _pipeline.tool_helpers import ensure_sdk

from verification.nightly_runner.config import NightlyConfig
from verification.nightly_runner.log_manager import ChunkLogManager
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


# ── Data types ──────────────────────────────────────────────────────────

class ChunkSpec:
    """Specification for a single chunk to be processed by a worker."""
    __slots__ = ("assembly", "slug", "foundation_dir", "bench_semaphore",
                 "pipeline_config")

    def __init__(self, assembly: str, slug: str, foundation_dir: Path,
                 bench_semaphore: Any, pipeline_config: dict):
        self.assembly = assembly
        self.slug = slug
        self.foundation_dir = foundation_dir
        self.bench_semaphore = bench_semaphore
        self.pipeline_config = pipeline_config


class NightlyResult:
    """Structured result from a nightly build run."""

    def __init__(self):
        self.started_at: float = time.perf_counter()
        self.started_wallclock: float = time.time()
        self.ended_at: float = 0.0
        self.ended_wallclock: float = 0.0
        self.chunk_results: dict[str, dict[str, StageResult]] = {}
        self.chunk_durations: dict[str, float] = {}
        self.chunk_count: int = 0
        self.assembly_count: int = 0

    @property
    def build_passed(self) -> int:
        return sum(
            1 for r in self.chunk_results.values()
            if r.get("build", StageResult(stage="build", status="?")).status
               in ("passed", "skipped")
        )

    @property
    def build_failed(self) -> int:
        return self.chunk_count - self.build_passed

    def summary(self) -> str:
        dur = self.ended_at - self.started_at
        return (f"{self.build_passed}/{self.chunk_count} chunks passed "
                f"({self.assembly_count} assemblies, {dur:.0f}s)")


# ── Helpers (extracted from nightly_build.py) ───────────────────────────

def discover_assemblies(foundation_dir: Path) -> list[str]:
    """Discover assembly directories that have chunk configurations."""
    assemblies: list[str] = []
    for entry in sorted(foundation_dir.iterdir()):
        if not entry.is_dir():
            continue
        name = entry.name
        if name.startswith("_") or name.startswith("."):
            continue
        partition = entry / "_dll" / "namespace-partition.json"
        if partition.exists():
            assemblies.append(name)
    return assemblies


def load_pipeline_config(foundation_dir: Path) -> dict:
    """Load pipeline-config.yaml for chunk-specific settings."""
    try:
        import yaml
    except ImportError:
        return {}
    config_path = foundation_dir / "config" / "pipeline-config.yaml"
    if config_path.exists():
        try:
            with open(config_path, encoding="utf-8") as f:
                return yaml.safe_load(f) or {}
        except Exception:
            return {}
    return {}


# ── Chunk execution ─────────────────────────────────────────────────────

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
) -> tuple[str, dict[str, StageResult]]:
    """Run all stages for a single chunk.

    Normal pass: build → fact → profile → benchmark → managed_benchmark →
                 hotupdate → benchmark_report → coverage_audit
    Profile pass: build → profile only

    Returns (f"{assembly}/{slug}", {stage: result}).
    """
    try:
        chunk_dir = foundation_dir / "chunks" / slug

        # Resolve assembly dirs from pipeline-config.yaml
        chunk_cfg = (pipeline_config.get("chunks") or {}).get(slug, {})
        assembly_dirs_str = (chunk_cfg.get("assemblyDirs") or "").strip()
        assembly_dirs = (
            [d.strip() for d in assembly_dirs_str.split(",") if d.strip()]
            if assembly_dirs_str else []
        )

        # Git info for provenance
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

        if profile_pass:
            stage_order = [
                ("build", lambda ctx_, _: _restore_profile_cache(ctx_, ctx.chunk_dir)),
                ("profile", run_profile),
            ]
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
                    with bench_semaphore:
                        result = stage_fn(ctx, stages)
                else:
                    result = stage_fn(ctx, stages)

                stages[stage_name] = result

                if stage_name == "build" and result.status in ("failed", "error"):
                    if verbose:
                        print(f"  [nightly] [{assembly}/{slug}] build failed, skipping")
                    break

            except Exception as e:
                if verbose:
                    print(f"  [nightly] [{assembly}/{slug}] {stage_name} EXCEPTION: {e}")
                stages[stage_name] = StageResult(
                    stage=stage_name, status="skipped",
                    summary=f"nightly exception: {e}",
                )
                if stage_name == "build":
                    break

    except Exception as e:
        if verbose:
            print(f"  [nightly] [{assembly}/{slug}] init EXCEPTION: {e}")
        stages = {"build": StageResult(stage="build", status="skipped", summary=f"init: {e}")}
        return f"{assembly}/{slug}", stages

    return f"{assembly}/{slug}", stages


def _restore_profile_cache(ctx: ChunkContext, chunk_dir: Path) -> StageResult:
    """Restore profile-tier entry.exe from hephaestus cache."""
    import shutil
    import tempfile

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
        print(f"  [nightly] profile cache MISS — falling back to full profile build")
        return run_build(ctx, {})

    exe = ctx.entry_exe_path
    if exe.exists():
        exe.unlink()

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

    return StageResult(stage="build", status="passed",
                       summary=f"profile entry.exe restored ({exe_size} bytes)")


# ── Profile pass ────────────────────────────────────────────────────────

def _run_profile_pass(
    all_chunks: list[tuple[str, str, Path]],
    pipeline_config: dict,
    bench_semaphore: Any,
    config: NightlyConfig,
) -> dict[str, dict[str, StageResult]]:
    """Run profile pass: restore from cache or rebuild with profile tier."""
    print(f"\n{'='*60}")
    print(f"  Profile pass (native_config=profile)...")
    print(f"{'='*60}")

    profile_workers = max(1, config.max_workers // 2)
    profile_futures = []
    with ProcessPoolExecutor(max_workers=profile_workers) as executor:
        for asm, slug, fdir in all_chunks:
            future = executor.submit(
                _run_chunk_stages,
                assembly=asm, slug=slug, foundation_dir=fdir,
                pipeline_config=pipeline_config,
                bench_semaphore=bench_semaphore,
                native_config="profile",
                verbose=config.log_level == "DEBUG",
                stage_timeout=config.stage_timeout,
                profile_pass=True,
            )
            profile_futures.append(future)

    return _collect_chunk_results(profile_futures, len(all_chunks),
                                  verbose=config.log_level == "DEBUG")


# ── Result collection ───────────────────────────────────────────────────

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
            results[key] = {
                "build": StageResult(stage="build", status="skipped",
                                     summary=f"future exception: {e}")}
            if verbose:
                print(f"  [nightly] [{completed}/{total}] {key}: future EXCEPTION: {e}")
    return results


# ── Main orchestrator ───────────────────────────────────────────────────

class NightlyOrchestrator:
    """Parallel execution orchestrator for nightly builds.

    Usage:
        config = NightlyConfig(...)
        orch = NightlyOrchestrator(config)
        result = orch.run()
    """

    def __init__(self, config: NightlyConfig):
        self.config = config
        self._result = NightlyResult()

        # Ensure SDK is available
        try:
            ensure_sdk(self.config.repo_root)
            print("[nightly-runner] SDK ready")
        except RuntimeError as e:
            print(f"[nightly-runner] WARNING: SDK not available: {e}")

    def run(self) -> NightlyResult:
        """Execute the full nightly build pipeline."""
        overall_start = time.perf_counter()
        result = self._result
        config = self.config
        foundation_dir = config.foundation_dir

        # ── Step 1: Discover assemblies + chunks ──
        print(f"\n{'='*60}")
        print(f"  Nightly Runner — {datetime.now(timezone.utc).strftime('%Y-%m-%d %H:%M:%S UTC')}")
        print(f"{'='*60}")

        assemblies = config.assemblies or discover_assemblies(foundation_dir)
        if not assemblies:
            print("ERROR: No assemblies found")
            return result
        result.assembly_count = len(assemblies)
        print(f"\n  Discovered {len(assemblies)} assemblies")

        pipeline_config = load_pipeline_config(foundation_dir)

        all_chunks: list[tuple[str, str, Path]] = []
        for asm in assemblies:
            chunks = discover_chunks(asm, foundation_dir=foundation_dir / asm)
            if not chunks:
                print(f"  WARNING: No chunks for {asm}")
            for slug in chunks:
                all_chunks.append((asm, slug, foundation_dir / asm))

        if not all_chunks:
            print("ERROR: No chunks found across all assemblies")
            return result
        result.chunk_count = len(all_chunks)
        print(f"  Total chunks: {len(all_chunks)}")

        # ── Step 2: Run pipeline stages ──
        print(f"\n  Phase 1: Running chunks "
              f"(workers={config.max_workers}, bench_concurrency={config.bench_workers})...")

        bench_semaphore = Manager().Semaphore(config.bench_workers or 1)

        with ProcessPoolExecutor(max_workers=config.max_workers) as executor:
            futures = []
            for asm, slug, fdir in all_chunks:
                future = executor.submit(
                    self._run_single_chunk,
                    assembly=asm,
                    slug=slug,
                    foundation_dir=fdir,
                    pipeline_config=pipeline_config,
                    bench_semaphore=bench_semaphore,
                    capture_logs=config.capture_logs,
                    keep_console=config.keep_console_output,
                    report_dir=config.report_dir,
                    native_config=config.native_config,
                    verbose=config.log_level == "DEBUG",
                    stage_timeout=config.stage_timeout,
                )
                futures.append(future)

            result.chunk_results = _collect_chunk_results(
                futures, len(all_chunks),
                verbose=config.log_level == "DEBUG",
            )

        # Print build summary
        print(f"\n  Build: {result.build_passed}/{result.chunk_count} passed")

        # ── Profile pass (optional) ──
        if config.run_profile:
            _run_profile_pass(all_chunks, pipeline_config, bench_semaphore, config)

        # ── Step 3: Aggregate per-assembly ──
        print(f"\n  Phase 3: Aggregating per-assembly...")
        for asm in assemblies:
            fdir = foundation_dir / asm
            agg_ctx = ChunkContext(
                slug="__aggregate__",
                assembly=asm,
                chunk_dir=fdir / "chunks" / "__aggregate__",
                foundation_dir=fdir,
                mode="standard",
                native_config=config.native_config,
                verbose=config.log_level == "DEBUG",
            )
            try:
                agg_result = run_aggregate(agg_ctx, {})
                print(f"    {asm}: aggregate {agg_result.status}")
            except Exception as e:
                print(f"    {asm}: aggregate EXCEPTION: {e}")

        # ── Step 4: Reporting ──
        print(f"\n  Phase 4: Reporting...")
        for asm in assemblies:
            fdir = foundation_dir / asm
            report_ctx = ChunkContext(
                slug="__report__",
                assembly=asm,
                chunk_dir=fdir / "chunks" / "__report__",
                foundation_dir=fdir,
                mode="standard",
                native_config=config.native_config,
                verbose=config.log_level == "DEBUG",
            )
            try:
                report_result = run_reporting(report_ctx, {})
                print(f"    {asm}: reporting {report_result.status}")
            except Exception as e:
                print(f"    {asm}: reporting EXCEPTION: {e}")

        result.ended_at = time.perf_counter()
        result.ended_wallclock = time.time()
        print(f"\n{'='*60}")
        print(f"  Nightly run complete: {result.summary()}")
        if result.build_failed > 0:
            print(f"  WARN: {result.build_failed} chunk(s) failed")
        print(f"{'='*60}\n")

        return result

    @staticmethod
    def _run_single_chunk(
        assembly: str,
        slug: str,
        foundation_dir: Path,
        pipeline_config: dict,
        bench_semaphore: Any,
        *,
        capture_logs: bool = True,
        keep_console: bool = True,
        report_dir: Path | None = None,
        native_config: str = "check",
        verbose: bool = False,
        stage_timeout: int = 0,
    ) -> tuple[str, dict[str, StageResult]]:
        """Run chunk with log capture, meant for ProcessPoolExecutor workers.

        All config values are passed as explicit keyword arguments (not via
        module-level globals, which don't survive pickling across processes).
        """
        if capture_logs and report_dir:
            log_mgr = ChunkLogManager(
                assembly=assembly,
                slug=slug,
                report_dir=report_dir,
                keep_console=keep_console,
            )
        else:
            log_mgr = _NullContextManager()

        with log_mgr:
            result = _run_chunk_stages(
                assembly=assembly,
                slug=slug,
                foundation_dir=foundation_dir,
                pipeline_config=pipeline_config,
                bench_semaphore=bench_semaphore,
                native_config=native_config,
                verbose=verbose,
                stage_timeout=stage_timeout,
            )
        return result


class _NullContextManager:
    """No-op context manager when log capture is disabled."""
    def __enter__(self):
        return self

    def __exit__(self, *args):
        pass
