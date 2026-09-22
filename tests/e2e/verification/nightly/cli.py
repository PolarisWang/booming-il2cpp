"""CLI entry point — replacement for `verification.nightly_runner.main`.

Usage:
    python -m verification.nightly.cli --max-workers 4 --native-config check
    python -m verification.nightly.cli --assembly System.Linq --max-workers 2
    python -m verification.nightly.cli --resume <run_id>
"""

from __future__ import annotations

import signal
import sys
import time
from pathlib import Path

from .config import NightlyConfig
from .worklist import discover_worklist
from .run import NightlyResult, run_phases, build_resume_worklist
from .aggregate import aggregate_reports


def _install_interrupt_handler() -> None:
    """Turn an interrupt into an orderly shutdown instead of a hard kill.

    Windows delivers STATUS_CONTROL_C_EXIT (0xC000013A, printed as -1073741510)
    when Jenkins aborts/times out the build; on POSIX the equivalent is SIGINT.
    Both killed the process outright, so the results of every chunk that had
    already finished were lost and the run looked like "produced nothing"
    (builds #288/#289/#290).

    Raising KeyboardInterrupt instead lets the exception unwind through
    run_phases(), where each worker subprocess is reaped by its existing
    finally/cleanup path, and lets main() still reach aggregate_reports() so the
    partial payload is published.  The Jenkinsfile already treats a publish with
    few/no passing chunks as a failure, so this loses no signal — it only keeps
    the data needed to explain the failure.
    """
    def _handler(signum, frame):  # noqa: ARG001 — signature fixed by signal API
        raise KeyboardInterrupt(f"interrupted by signal {signum}")

    for name in ("SIGINT", "SIGTERM", "SIGBREAK"):
        sig = getattr(signal, name, None)
        if sig is None:
            continue
        try:
            signal.signal(sig, _handler)
        except (ValueError, OSError):
            # Not settable in this context (e.g. non-main thread) — keep going.
            pass


def main() -> int:
    _install_interrupt_handler()
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
    try:
        result = run_phases(config, work)
    except KeyboardInterrupt:
        # Interrupted (Ctrl+C on POSIX, STATUS_CONTROL_C_EXIT on Windows when
        # Jenkins aborts).  Do not re-raise: fall through to aggregation so the
        # chunks that DID finish are still written out and published.  Exiting
        # 0 here would be wrong — report failure.
        print("\n=== INTERRUPTED (Ctrl+C / STATUS_CONTROL_C_EXIT) ===", flush=True)
        print("=== publishing partial results for completed chunks ===", flush=True)
        result = NightlyResult()
        result.end_wall = time.time()
        result.interrupted = True

    # Aggregate
    summary = aggregate_reports(config, result)
    print(f"\n{'='*64}")
    print(f"  {summary.summary_text()}")
    print(f"  Duration: {result.end_wall - result.start_wall:.0f}s")
    print(f"{'='*64}\n")

    if result.interrupted:
        return 1
    return 1 if result.failed_count > 0 else 0


if __name__ == "__main__":
    raise SystemExit(main())