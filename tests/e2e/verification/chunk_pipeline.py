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

# ── Root cause fix: GBK-console crash on emoji (✅/❌/⚠️/…) prints ───────
# Force UTF-8 on stdout/stderr so emoji prints never crash on a GBK console.
# See _encoding.py for rationale (shared across all engine entry points).
from verification import _encoding as _console_encoding  # noqa: E402

# Ensure foundation-dll/ (family root) + the testing tree (_pipeline holder) are on
# sys.path. Roots are resolved through the central _path resolver so the engine is
# location-independent (env overrides CHAOS_FOUNDATION_DLL / CHAOS_TESTING_DIR; else
# marker-walk / parent inference).
import sys as _b_sys
from pathlib import Path as _b_Path
for _b_d in _b_Path(__file__).resolve().parents:
    if (_b_d / "_path.py").exists():
        if str(_b_d) not in _b_sys.path:
            _b_sys.path.insert(0, str(_b_d))
        break
from _path import foundation_root, testing_tree_root, build_root

_FOUNDATION_DLL = foundation_root()
_TESTING_DIR = testing_tree_root()
if str(_FOUNDATION_DLL) not in sys.path:
    sys.path.insert(0, str(_FOUNDATION_DLL))
if str(_TESTING_DIR) not in sys.path:
    sys.path.insert(0, str(_TESTING_DIR))

# SDK auto-build: ensure prebuilt native runtime libs are available.
# _REPO_ROOT is resolved robustly (tool_helpers walks up to the .git dir) rather
# than derived from _TESTING_DIR.parent — after the L6/L7 relocate, _TESTING_DIR may
# point at tests/e2e (not the repo-root child tests/), which would double-path the
# SDK/build_presets lookup.
from _pipeline.tool_helpers import ensure_sdk, _repo_root as _engine_repo_root
_REPO_ROOT = _engine_repo_root()
try:
    ensure_sdk(_REPO_ROOT)
    print("[chunk-pipeline] SDK ready")
except RuntimeError as e:
    print(f"[chunk-pipeline] WARNING: SDK not available: {e}")
    print("[chunk-pipeline] Will attempt codegen inline (may fail if no prebuilt libs)")

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
    current_subsection: str | None = None
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


def _toposort_stages(stage_names: list, stage_deps: dict) -> list:
    """Return stage_names reordered so every dependency precedes its dependent.

    DFS-based topological sort restricted to the requested stage set. Used so the
    pipeline no longer depends on the caller listing --stages in dependency order
    (e.g. "--stages reporting,aggregate" is automatically corrected). The DAG is
    acyclic by construction (STAGE_DEPS), so no cycle detection needed here.
    """
    ordered: list[str] = []
    visited: set[str] = set()
    requested = set(stage_names)

    def _visit(node: str) -> None:
        if node in visited or node not in requested:
            return
        visited.add(node)
        for dep in stage_deps.get(node, []):
            _visit(dep)
        ordered.append(node)

    for s in stage_names:
        _visit(s)
    return ordered


def _write_provenance(ctx, stages_result: dict, stage_names: list | None = None) -> None:
    """Persist per-chunk build/run provenance to <chunk>/results/provenance.json.

    The build StageResult carries details.fastpath / details.hephaestus
    (cache_hit|miss) / details.cacheKey. These are the only trustworthy
    cache-vs-fresh markers produced at runtime, but chunk_pipeline dropped them
    after the build stage. Aggregate re-reads this file to expose them in
    fact-summary.json / dashboard.json, so a harness can tell fresh builds from
    reused (possibly stale) cache/fastpath intermediates.

    Called immediately after the build stage (not after all stages) so aggregate
    (a later stage in the same invocation) sees a populated file.
    """
    try:
        import json
        ctx.results_dir.mkdir(parents=True, exist_ok=True)
        build_details = (stages_result.get("build") or {}).get("details", {})

        provenance = {
            "slug": ctx.slug,
            "assembly": ctx.assembly,
            "runId": ctx.run_id,
            "platform": ctx.platform,
            "deviceId": (ctx.device or {}).get("id", ""),
            "gitCommit": ctx.git_commit,
            "gitBranch": ctx.git_branch,
            "nativeConfig": ctx.native_config,
            "mode": ctx.mode,
            "stagesRun": list(stage_names) if stage_names else None,
            "build": {
                "status": (stages_result.get("build") or {}).get("status"),
                "fastpath": build_details.get("fastpath", False),
                "cacheProvenance": build_details.get("hephaestus"),
                "cacheKey": build_details.get("cacheKey"),
                "cachedAt": build_details.get("cachedAt"),
            },
        }
        (ctx.results_dir / "provenance.json").write_text(
            json.dumps(provenance, indent=2), encoding="utf-8")
    except Exception as e:  # non-fatal: provenance is an enhancement
        print(f"  [provenance] WARNING: could not persist provenance: {e}")


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
    parser.add_argument("--stages", default=None,
                        help="Comma-separated stages to run (default: build,fact,hotupdate,coverage-audit; or build,fact,coverage-audit with --smoke)")
    parser.add_argument("--smoke", action="store_true",
                        help="Smoke mode: run only fast verification stages (build,fact,coverage-audit) "
                             "and, with --all-chunks, only the first --smoke-chunks chunks.")
    parser.add_argument("--mode", default=None,
                        choices=["standard", "strict"],
                        help="Verification mode: standard (default) or strict (coverage fails if >5%% methods missing)")
    parser.add_argument("--native-config", default="check",
                        choices=["check", "profile", "ship"],
                        help="Native build config (default: check)")
    parser.add_argument("--verbose", action="store_true",
                        help="Verbose output")
    parser.add_argument("--skip-probe", action="store_true",
                        help="Skip probe phase in AutoTestGenerator")
    parser.add_argument("--smoke-chunks", type=int, default=1,
                        help="In smoke mode with --all-chunks, limit to this many chunks (default 1)")

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

    # ── Smoke mode: default to fast stages and a limited chunk set ──
    if args.smoke:
        if args.stages is None:
            args.stages = "build,fact,coverage-audit"
        print("[chunk-pipeline] SMOKE mode: stages=[" + args.stages + "]")

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
        if args.smoke and len(chunks) > args.smoke_chunks:
            print(f"[chunk-pipeline] SMOKE: limiting {len(chunks)} chunks to first {args.smoke_chunks}")
            chunks = chunks[:args.smoke_chunks]
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
        "reporting": None,
    }

    if args.stages is None:
        args.stages = "build,fact,hotupdate,coverage-audit"
    stage_names = [s.strip() for s in args.stages.split(",")]
    for s in stage_names:
        if s not in stage_functions:
            print(f"ERROR: Unknown stage '{s}'. Valid: {', '.join(stage_functions.keys())}")
            return 1

    # ── Stage dependency DAG validation ──
    STAGE_DEPS: dict[str, list[str]] = {
        "fact":              ["build"],
        "benchmark":         ["build", "fact"],
        # P1-A: managed_benchmark appends net8/net10 to the SAME perf store that
        # benchmark_chunk writes with "w". Must run AFTER benchmark or its "w"
        # would erase the AOT+JIT records. Enforcing the dep fixes the ordering.
        "managed_benchmark": ["build", "benchmark"],
        "benchmark_report":  ["benchmark", "managed_benchmark"],
        "hotupdate":         ["build", "fact"],
        "coverage-audit":    ["build", "fact"],
        "profile":           ["build"],
        # P1-C: aggregate reads comparison.json (benchmark_report), hotupdate.json,
        # benchmark.json — all produced AFTER build/fact/coverage-audit. Enforce the
        # real producers so a single-assembly CLI run can't read stale/empty inputs.
        "aggregate":         ["build", "fact", "coverage-audit", "benchmark",
                              "hotupdate", "benchmark_report"],
        "reporting":         ["aggregate"],
    }
    stage_set = set(stage_names)
    missing_deps = False
    for s in stage_names:
        for dep in STAGE_DEPS.get(s, []):
            if dep not in stage_set:
                print(f"ERROR: Stage '{s}' requires '{dep}' but it's not in --stages")
                missing_deps = True
    if missing_deps:
        print(f"  Hint: typical pipeline is --stages build,fact,coverage-audit")
        return 1

    # Topologically reorder stages so dependencies run before dependents,
    # regardless of the order the caller listed them. This removes the earlier
    # implicit requirement that --stages be listed in dependency order.
    reordered = _toposort_stages(stage_names, STAGE_DEPS)
    if reordered != stage_names:
        print(f"[chunk-pipeline] Reordered stages by dependency: "
              f"{', '.join(reordered)}")
    stage_names = reordered

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
    from verification.stages.reporting import run_reporting

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
        "reporting": run_reporting,
    }

    overall_start = time.perf_counter()
    overall_status = "passed"
    total_chunks = len(chunks)

    # ── Preflight: layer boundary check (hard gate) ───────────────
    try:
        from verification.preflight.check_layer_boundaries import check_layer_boundaries_ci
        boundary_issues = check_layer_boundaries_ci()
        if boundary_issues:
            print(f"  [preflight] ❌  {len(boundary_issues)} layer boundary issue(s):")
            for issue in boundary_issues:
                print(f"      {issue}")
            overall_status = "failed"  # boundary violations are a hard gate
        else:
            print(f"  [preflight] ✅ layer boundary check passed")
    except ImportError:
        # preflight should always be importable from the relocated engine; if it
        # genuinely cannot load, surface it as a gate failure rather than silently
        # skipping (silent-skip previously let violations go unflagged).
        print(f"  [preflight] ⚠️  check_layer_boundaries not importable — treating as gate FAILURE")
        overall_status = "failed"

    # Use the config's stage timeout (look up benchmark first as most time-sensitive; fall back to any)
    timeouts = _PIPELINE_CONFIG.get("timeouts", {})
    stage_timeout_seconds = timeouts.get("benchmark",
                           timeouts.get("codegen_aot",
                           timeouts.get("fact_aot", 0)))
    chunk_mode = _PIPELINE_CONFIG.get("defaultMode", "standard")
    if args.mode:
        chunk_mode = args.mode
    seq = 0

    for chunk_slug in chunks:
        # Variant A: chunk_dir IS the build-output root (discardable); the
        # version-controlled source root is derived from foundation_dir/chunks.
        build_dir = build_root() / assembly / "chunks" / chunk_slug
        chunk_dir = build_dir
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

        # ── Progress / ETA ──
        elapsed = time.perf_counter() - overall_start
        pct = (seq - 1) / total_chunks * 100 if total_chunks > 0 else 0
        if seq > 1 and elapsed > 5:
            eta_seconds = (elapsed / (seq - 1)) * (total_chunks - seq + 1)
            eta_str = f", ETA {eta_seconds:.0f}s"
        else:
            eta_str = ""
        print(f"\n{'='*60}")
        print(f"[{seq}/{total_chunks} ({pct:.0f}%){eta_str}] Chunk: {chunk_slug}  [{assembly}]")
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
            elif sr.status == "warning" and overall_status != "failed":
                # Strict non-silent warnings (e.g. chaos-aot produced no data)
                # downgrade the run to "warning" (still exit 0 / non-fatal) but
                # are surfaced in the final status — never silently swallowed.
                overall_status = "warning"
                print(f"       [warning] {sr.summary}")

            # Persist per-chunk provenance right after build so the aggregate
            # stage (which runs later in the SAME invocation and re-reads the
            # chunk results dir) sees a populated provenance.json. Writing it
            # only after ALL stages ran meant aggregate read results before the
            # file existed (cacheProvenance/cachedAt came back null).
            if stage_name == "build":
                _write_provenance(ctx, stages_result)

        print(f"\n  Chunk '{chunk_slug}' summary: "
              f"{sum(1 for s in stages_result.values() if s.get('status') == 'passed')}/"
              f"{len(stage_names)} passed")

    total_duration = time.perf_counter() - overall_start
    print(f"\n{'='*60}")
    print(f"Pipeline complete: {overall_status}")
    print(f"Duration: {total_duration:.0f}s")
    print(f"{'='*60}")

    # Exit semantics: 0=passed, 2=warning (non-fatal, harness can detect a
    # strict non-silent warning without failing the run), 1=failed.
    return _exit_code_for_status(overall_status)


def _exit_code_for_status(overall_status: str) -> int:
    """Map an aggregated pipeline status to a process exit code.

    0 = passed, 1 = failed/error, 2 = warning (non-fatal but surfaced so a
    harness can detect a strict non-silent warning without failing the run).
    """
    if overall_status == "passed":
        return 0
    if overall_status == "warning":
        print("[chunk-pipeline] WARNING: pipeline completed with warnings (exit 2)")
        return 2
    return 1


if __name__ == "__main__":
    sys.exit(main())
