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

import sys
import time
from pathlib import Path

# Ensure foundation-dll/ is on sys.path
_HERE = Path(__file__).resolve().parent  # verification/
_FOUNDATION_DLL = _HERE.parent  # testing/foundation-dll/
if str(_FOUNDATION_DLL) not in sys.path:
    sys.path.insert(0, str(_FOUNDATION_DLL))


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
    parser.add_argument("--stages", default="build,fact,coverage-audit",
                        help="Comma-separated stages to run (default: build,fact,coverage-audit)")
    parser.add_argument("--native-config", default="check",
                        choices=["check", "profile", "ship"],
                        help="Native build config (default: check)")
    parser.add_argument("--verbose", action="store_true",
                        help="Verbose output")
    parser.add_argument("--skip-probe", action="store_true",
                        help="Skip probe phase in AutoTestGenerator")

    args = parser.parse_args()
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
        "benchmark": None,
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
    from verification.stages.benchmark_chunk import run_benchmark_chunk
    from verification.stages.hotupdate_chunk import run_hotupdate_chunk
    from verification.stages.coverage_audit import run_coverage_audit
    from verification.stages.aggregate import run_aggregate

    runners = {
        "build": run_build,
        "fact": run_fact_chunk,
        "benchmark": run_benchmark_chunk,
        "hotupdate": run_hotupdate_chunk,
        "coverage-audit": run_coverage_audit,
        "aggregate": run_aggregate,
    }

    overall_start = time.perf_counter()
    overall_status = "passed"

    for chunk_slug in chunks:
        chunk_dir = foundation_dir / "chunks" / chunk_slug
        ctx = ChunkContext(
            slug=chunk_slug,
            assembly=assembly,
            chunk_dir=chunk_dir,
            foundation_dir=foundation_dir,
            native_config=args.native_config,
            verbose=args.verbose,
            skip_probe=args.skip_probe,
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
