"""Chunk-based verification pipeline — unified CLI for the new namespace-chunk pipeline.

Usage:
    cd testing/foundation-dll

    # Run a single chunk through build → fact → coverage-audit
    python -m verification.chunk_pipeline --chunk numerics

    # Run all chunks
    python -m verification.chunk_pipeline --all-chunks

    # Run specific stages on a chunk
    python -m verification.chunk_pipeline --chunk system --stages build,fact

    # Coverage audit only
    python -m verification.chunk_pipeline --chunk io --stages coverage-audit

    # Full pipeline on one chunk
    python -m verification.chunk_pipeline --chunk threading --stages build,fact,benchmark,coverage-audit
"""

from __future__ import annotations

import os
import re
import subprocess
import sys
import time
from datetime import datetime, timezone
from pathlib import Path

# Ensure foundation-dll/ is on sys.path
_HERE = Path(__file__).resolve().parent  # verification/
_FOUNDATION_DLL = _HERE.parent  # testing/foundation-dll/
if str(_FOUNDATION_DLL) not in sys.path:
    sys.path.insert(0, str(_FOUNDATION_DLL))

# Path to pipeline config YAML (kept as single source of truth)
_PIPELINE_CONFIG_PATH = _FOUNDATION_DLL / "config" / "pipeline-config.yaml"


def _load_pipeline_config() -> dict:
    """Load pipeline configuration from YAML.

    Uses a lightweight parser (no pyyaml dependency) since the config
    uses only simple key:value and nested key:value mappings.
    """
    config: dict = {}
    path = _PIPELINE_CONFIG_PATH
    if not path.exists():
        return config

    text = path.read_text(encoding="utf-8")
    current_section: str | None = None
    timeouts: dict[str, int] = {}

    for line in text.splitlines():
        stripped = line.strip()
        if not stripped or stripped.startswith("#"):
            continue

        # Top-level key: pipeline:
        m = re.match(r"^(\w[\w-]*):", stripped)
        if m and not stripped.startswith(" "):
            current_section = m.group(1)
            continue

        # Nested key under pipeline:
        if current_section == "pipeline":
            m = re.match(r"^  (\w[\w-]*):\s*(.*)", stripped)
            if m:
                key, val = m.group(1), m.group(2).strip()
                if val:
                    # Scalar value
                    if val.isdigit():
                        config[key] = int(val)
                    elif val.lower() in ("true", "false"):
                        config[key] = val.lower() == "true"
                    else:
                        config[key] = val
                else:
                    # Section header (e.g. timeouts:, defaultStages:)
                    current_subsection = key
            continue

        # timeouts subsection
        if current_section == "pipeline" and current_subsection == "timeouts":
            m = re.match(r"^    (\w[\w-]*):\s*(\d+)", stripped)
            if m:
                timeouts[m.group(1)] = int(m.group(2))

    if timeouts:
        config["timeouts"] = timeouts
    return config


_PIPELINE_CONFIG = _load_pipeline_config()


def main():
    import argparse

    parser = argparse.ArgumentParser(
        description="Chunk-based verification pipeline",
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog=__doc__,
    )
    parser.add_argument("--assembly", default="System.Private.CoreLib",
                        help="Assembly name (default: System.Private.CoreLib)")
    parser.add_argument("--chunk", default=None,
                        help="Chunk slug to run (e.g. 'numerics')")
    parser.add_argument("--all-chunks", action="store_true",
                        help="Run all chunks for the assembly")
    parser.add_argument("--stages", default="build,fact,hotupdate,coverage-audit",
                        help="Comma-separated stages to run (default: build,fact,hotupdate,coverage-audit)")
    parser.add_argument("--native-config", default="check",
                        choices=["check", "profile", "ship"],
                        help="Native build config (default: check)")
    parser.add_argument("--verbose", action="store_true",
                        help="Verbose output")
    parser.add_argument("--skip-probe", action="store_true",
                        help="Skip probe phase in AutoTestGenerator")

    # ── Hephaestus cache management subcommand ──
    subparsers = parser.add_subparsers(dest="hephaestus_cmd",
                                       help="Hephaestus cache management")
    hep_parser = subparsers.add_parser("hephaestus",
        help="Hephaestus Lib cache management commands")
    hep_sub = hep_parser.add_subparsers(dest="hep_action", required=True)

    hep_stats = hep_sub.add_parser("stats", help="Show cache statistics")
    hep_stats.add_argument("--assembly", default=None,
                           help="Filter by assembly name")

    hep_clear = hep_sub.add_parser("clear", help="Clear all cache entries")

    hep_inv = hep_sub.add_parser("invalidate",
                                 help="Invalidate entries for an assembly")
    hep_inv.add_argument("assembly", help="Assembly name to invalidate")

    hep_prune = hep_sub.add_parser("prune", help="Prune stale or excess entries")
    hep_prune.add_argument("--max-entries", type=int, default=50,
                           help="Maximum entries to keep")

    args = parser.parse_args()

    # ── Hephaestus cache management ──
    if args.hephaestus_cmd:
        from verification.stages.hephaestus_cache import HephaestusCache
        cache = HephaestusCache(_FOUNDATION_DLL, verbose=args.verbose)

        if args.hep_action == "stats":
            stats = cache.get_stats()
            print(f"Hephaestus Cache Statistics:")
            print(f"  Cache dir:      {stats['cache_dir']}")
            print(f"  Total entries:  {stats['total_entries']}")
            print(f"  Valid entries:  {stats['valid_entries']}")
            print(f"  Stale entries:  {stats['stale_entries']}")
            print(f"  Cache size:     {stats['total_size_bytes'] / (1024*1024):.1f} MB")
            print(f"  Max entries:    {stats['max_entries']}")
            if args.assembly:
                assembly_stats = [
                    e for e in cache._manifest.entries
                    if e.assembly == args.assembly
                ]
                print(f"  Assembly '{args.assembly}': {len(assembly_stats)} entries")
                for e in assembly_stats:
                    print(f"    {e.cache_key[:48]}... [{e.status}] {e.duration_ms}ms")
            return 0

        elif args.hep_action == "clear":
            import shutil
            cache_dir = cache._cache_dir
            if cache_dir.is_dir():
                shutil.rmtree(cache_dir)
                print(f"Cleared cache: {cache_dir}")
            else:
                print("Cache is already empty")
            return 0

        elif args.hep_action == "invalidate":
            count = cache.invalidate_assembly(args.assembly)
            print(f"Invalidated {count} entries for {args.assembly}")
            return 0

        elif args.hep_action == "prune":
            from verification.stages.hephaestus_cache import DEFAULT_MAX_ENTRIES
            cache._max_entries = args.max_entries
            cache._prune_old_entries()
            cache._save_manifest()
            print(f"Pruned cache to max {args.max_entries} entries")
            stats = cache.get_stats()
            print(f"  Valid entries: {stats['valid_entries']}")
            print(f"  Cache size:    {stats['total_size_bytes'] / (1024*1024):.1f} MB")
            return 0

    assembly = args.assembly
    foundation_dir = _FOUNDATION_DLL / assembly

    if not foundation_dir.is_dir():
        print(f"ERROR: Assembly directory not found: {foundation_dir}")
        return 1

    from verification.orchestration.context import ChunkContext
    from verification.orchestration.discovery import discover_chunks

    # Determine which chunks to run
    if args.all_chunks:
        chunks = discover_chunks(assembly, verbose=args.verbose, foundation_dir=foundation_dir)
        if not chunks:
            print(f"ERROR: No chunks found for {assembly}")
            return 1
    elif args.chunk:
        chunks = [args.chunk]
    else:
        parser.print_help()
        return 1

    # Map stage names to functions
    stage_functions = {
        "build": None,
        "fact": None,
        "profile": None,
        "benchmark": None,
        "managed_benchmark": None,
        "benchmark_report": None,
        "hotupdate": None,
        "coverage-audit": None,
        "aggregate": None,
    }

    stage_names = [s.strip() for s in args.stages.split(",")]
    for s in stage_names:
        if s not in stage_functions:
            print(f"ERROR: Unknown stage '{s}'. Valid: {', '.join(stage_functions.keys())}")
            return 1

    # Import stage functions
    from verification.stages.build import run_build
    from verification.stages.fact_chunk import run_fact_chunk
    from verification.stages.profile import run_profile
    from verification.stages.benchmark_chunk import run_benchmark_chunk
    from verification.stages.managed_benchmark import run_managed_benchmark
    from verification.stages.hotupdate_chunk import run_hotupdate_chunk
    from verification.stages.benchmark_report import run_benchmark_report
    from verification.stages.coverage_audit import run_coverage_audit
    from verification.stages.aggregate import run_aggregate

    runners = {
        "build": run_build,
        "fact": run_fact_chunk,
        "profile": run_profile,
        "benchmark": run_benchmark_chunk,
        "managed_benchmark": run_managed_benchmark,
        "hotupdate": run_hotupdate_chunk,
        "benchmark_report": run_benchmark_report,
        "coverage-audit": run_coverage_audit,
        "aggregate": run_aggregate,
    }

    overall_start = time.perf_counter()
    overall_status = "passed"

    # Use the config's stage timeout (look up benchmark first as most time-sensitive; fall back to any)
    timeouts = _PIPELINE_CONFIG.get("timeouts", {})
    stage_timeout_seconds = timeouts.get("benchmark",
                           timeouts.get("codegen_aot",
                           timeouts.get("fact_aot", 0)))
    chunk_mode = _PIPELINE_CONFIG.get("defaultMode", "standard")
    seq = 0

    for chunk_slug in chunks:
        chunk_dir = foundation_dir / "chunks" / chunk_slug
        # Read assembly dirs from pipeline-config.yaml for this chunk
        chunk_cfg = (_PIPELINE_CONFIG.get('chunks') or {}).get(chunk_slug, {})
        assembly_dirs_str = (chunk_cfg.get('assemblyDirs') or '').strip()
        assembly_dirs = []
        if assembly_dirs_str:
            for d in assembly_dirs_str.split(';'):
                d = d.strip()
                if not d:
                    continue
                resolved = Path(d)
                if not resolved.is_absolute():
                    resolved = foundation_dir / d
                if resolved.is_dir():
                    assembly_dirs.append(str(resolved))

        # ── Run identity / provenance metadata ──
        seq += 1
        run_id = f"fdn-{datetime.now(timezone.utc).strftime('%Y%m%d')}-{seq:03d}"
        platform_map = {"win32": "windows-x64", "linux": "linux-x64", "darwin": "macos-arm64"}
        platform = platform_map.get(sys.platform, sys.platform)
        device_id = os.environ.get("CHAOS_DEVICE_ID") or os.environ.get("HOSTNAME") or os.environ.get("COMPUTERNAME") or "unknown"
        device = {"id": device_id}
        try:
            import subprocess as _sp
            git_commit = _sp.run(["git", "rev-parse", "HEAD"], capture_output=True, text=True, timeout=5).stdout.strip()
            git_branch = _sp.run(["git", "rev-parse", "--abbrev-ref", "HEAD"], capture_output=True, text=True, timeout=5).stdout.strip()
        except Exception:
            git_commit = ""
            git_branch = ""

        ctx = ChunkContext(
            slug=chunk_slug,
            assembly=assembly,
            chunk_dir=chunk_dir,
            foundation_dir=foundation_dir,
            mode=chunk_mode,
            native_config=args.native_config,
            verbose=args.verbose,
            skip_probe=args.skip_probe,
            stage_timeout_seconds=stage_timeout_seconds,
            assembly_dirs=assembly_dirs,
            run_id=run_id,
            platform=platform,
            device=device,
            git_commit=git_commit,
            git_branch=git_branch,
        )

        print(f"\n{'='*60}")
        print(f"Chunk: {chunk_slug}  [{assembly}]")
        print(f"{'='*60}")

        stages_result: dict[str, dict] = {}
        for stage_name in stage_names:
            runner = runners[stage_name]
            print(f"\n  >>> Stage: {stage_name}")
            sr = runner(ctx, stages_result)
            stages_result[stage_name] = sr.to_dict()
            status_symbol = "+" if sr.status == "passed" else "-" if sr.status in ("failed", "error") else "?"
            print(f"  <<< {status_symbol} [{sr.status}] {sr.summary}")

            if sr.status in ("failed", "error"):
                overall_status = "failed"
                if sr.errors:
                    for err in sr.errors:
                        print(f"       Error: {err}")

        print(f"\n  Chunk '{chunk_slug}' summary: "
              f"{sum(1 for s in stages_result.values() if s.get('status') == 'passed')}/"
              f"{len(stage_names)} passed")

    total_duration = time.perf_counter() - overall_start
    print(f"\n{'='*60}")
    print(f"Pipeline complete: {overall_status}")
    print(f"Duration: {total_duration:.0f}s")
    print(f"{'='*60}")

    return 0 if overall_status == "passed" else 1


if __name__ == "__main__":
    sys.exit(main())
