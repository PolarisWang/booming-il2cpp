"""CLI entry point — replacement for `verification.nightly_runner.main`.

Usage:
    python -m verification.nightly.cli --max-workers 4 --native-config check
    python -m verification.nightly.cli --assembly System.Linq --max-workers 2
    python -m verification.nightly.cli --resume <run_id>
"""

from __future__ import annotations

import sys
from pathlib import Path

from .config import NightlyConfig
from .worklist import discover_worklist
from .run import NightlyResult, run_phases, build_resume_worklist
from .aggregate import aggregate_reports


def main() -> int:
    import argparse

    # ⚡ Fix GBK stdout encoding (UnicodeEncodeError on ⚠ etc.)
    try:
        sys.stdout.reconfigure(encoding="utf-8", errors="replace")
    except Exception:
        pass

    parser = argparse.ArgumentParser(
        description="Nightly Build — subprocess-per-chunk engine (Route 3)",
        formatter_class=argparse.RawDescriptionHelpFormatter,
    )
    parser.add_argument("--max-workers", type=int, default=4,
                        help="Max parallel chunk workers (default: 4)")
    parser.add_argument("--native-config", default="check",
                        choices=["check", "profile", "ship"],
                        help="Native build config (default: check)")
    parser.add_argument("--assembly", default=None, action="append",
                        help="Run only this assembly (can be repeated)")
    parser.add_argument("--resume", default=None,
                        help="Resume from a prior run_id (skip already-passed chunks)")
    parser.add_argument("--no-profile", action="store_true",
                        help="Skip profile pass")
    parser.add_argument("--verbose", "-v", action="store_true",
                        help="Verbose output")
    parser.add_argument("--foundation-dll", default=None,
                        help="Path to foundation directory (default: auto-detect)")

    # Lazy parse to allow --help before config resolution
    args = parser.parse_args()

    # Build config
    env_meta = NightlyConfig.from_env()
    config = NightlyConfig(
        max_workers=args.max_workers,
        native_config=args.native_config,
        assembly_filter=args.assembly,
        resume_run_id=args.resume,
        profile_pass=not args.no_profile,
        verbose=args.verbose,
        **env_meta,
    )
    if args.foundation_dll:
        config.foundation_dir = Path(args.foundation_dll).resolve()

    # Print banner
    print(f"  Report directory: {config.report_dir}")
    print(f"  Run ID:           {config.run_id}")
    print(f"  Foundation:       {config.foundation_dir}")
    print(f"  Workers:          {config.max_workers}")
    print(f"  Config:           {config.native_config}")
    print(f"  Profile:          {'yes' if config.profile_pass else 'no'}")
    print(f"  Resume:           {config.resume_run_id or 'no'}")
    print(f"  Assemblies:       {'all' if config.assembly_filter is None else config.assembly_filter}")

    # Discover worklist
    all_work = discover_worklist(config)
    if not all_work:
        print("ERROR: No work items found. Check foundation dir.", flush=True)
        return 1

    print(f"  Discovered {len(all_work)} chunks across "
          f"{len(set(w.assembly for w in all_work))} assemblies", flush=True)

    # Resume filter
    if config.resume_run_id:
        config.report_dir = Path(config.report_dir).resolve()
        # Read prior state from the same report_dir:
        # resume_run_id is the ID of a prior run whose state is under
        # <report_dir>/run-state/<resume_run_id>/
        work = build_resume_worklist(config, all_work)
    else:
        work = all_work

    if not work:
        print("All chunks already passed. Nothing to do.", flush=True)
        return 0

    # Execute
    result = run_phases(config, work)

    # Aggregate
    summary = aggregate_reports(config, result)
    print(f"\n{'='*64}")
    print(f"  {summary.summary_text()}")
    print(f"  Duration: {result.end_wall - result.start_wall:.0f}s")
    print(f"{'='*64}\n")

    return 1 if result.failed_count > 0 else 0


if __name__ == "__main__":
    raise SystemExit(main())