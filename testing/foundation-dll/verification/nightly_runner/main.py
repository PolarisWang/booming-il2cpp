"""Main entry point — Jenkins-friendly CLI for running nightly builds.

Usage (Jenkins shell step):
    cd $WORKSPACE
    python -m verification.nightly_runner.main \\
        --report-dir /var/lib/jenkins/workspace/nightly-build-report \\
        --max-workers 8 \\
        --native-config check

Environment variables auto-detected:
    JOB_NAME           → jenkins_job
    BUILD_ID           → jenkins_build_id
    BUILD_URL          → jenkins_build_url
    JENKINS_TRIGGER    → jenkins_trigger
"""
from __future__ import annotations

import sys
from pathlib import Path

# Ensure testing/foundation-dll is on sys.path
_HERE = Path(__file__).resolve().parent
from verification._path import foundation_root
_FOUNDATION_DLL = foundation_root()
if str(_FOUNDATION_DLL) not in sys.path:
    sys.path.insert(0, str(_FOUNDATION_DLL))

from verification.nightly_runner.config import NightlyConfig
from verification.nightly_runner.orchestrator import NightlyOrchestrator
from verification.nightly_runner.report_collector import ReportCollector


def main() -> int:
    import argparse

    parser = argparse.ArgumentParser(
        description="Nightly Runner — full pipeline execution with unified reporting",
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog=__doc__,
    )
    parser.add_argument("--report-dir", default="nightly-build-report",
                        help="Root directory for all reports (default: nightly-build-report)")
    parser.add_argument("--foundation-dll", default=None,
                        help="Path to testing/foundation-dll/ (default: auto-detect)")
    parser.add_argument("--assembly", default=None,
                        help="Run a single assembly only (default: all)")
    parser.add_argument("--max-workers", type=int, default=4,
                        help="Max parallel build/fact workers (default: 4)")
    parser.add_argument("--bench-workers", type=int, default=None,
                        help="Max parallel benchmark workers (default: auto)")
    parser.add_argument("--native-config", default="check",
                        choices=["check", "profile", "ship"],
                        help="Native build config (default: check)")
    parser.add_argument("--stage-timeout", type=int, default=0,
                        help="Per-stage timeout in seconds (default: no timeout)")
    parser.add_argument("--no-profile", action="store_true",
                        help="Skip profile pass (profile is ON by default)")
    parser.add_argument("--verbose", "-v", action="store_true",
                        help="Verbose/Debug output")
    parser.add_argument("--no-logs", action="store_true",
                        help="Disable per-worker log capture")
    parser.add_argument("--skip-nightly-report", action="store_true",
                        help="Skip delta and summary generation at the end")
    args = parser.parse_args()

    # Build config
    env_meta = NightlyConfig.from_env()
    config = NightlyConfig(
        report_dir=Path(args.report_dir).resolve(),
        foundation_dir=Path(args.foundation_dll).resolve() if args.foundation_dll else None,
        assemblies=[args.assembly] if args.assembly else None,
        max_workers=args.max_workers,
        bench_workers=args.bench_workers,
        native_config=args.native_config,
        run_profile=not args.no_profile,
        stage_timeout=args.stage_timeout,
        capture_logs=not args.no_logs,
        log_level="DEBUG" if args.verbose else "INFO",
        keep_console_output=True,
        skip_nightly_report=args.skip_nightly_report,
        **env_meta,
    )

    # Phase 1: Execute
    print(f"  Report directory: {config.report_dir}")
    print(f"  Assemblies: {'all' if config.assemblies is None else config.assemblies}")
    print(f"  Stages: {', '.join(config.stages)}")
    print(f"  Workers: {config.max_workers} (bench: {config.bench_workers})")
    print(f"  Native config: {config.native_config}")
    print(f"  Profile pass: {'yes' if config.run_profile else 'no'}")
    print(f"  Log capture: {'yes' if config.capture_logs else 'no'}")

    orchestrator = NightlyOrchestrator(config)
    result = orchestrator.run()

    # Phase 2: Reports
    collector = ReportCollector(config, result)
    report_path = collector.collect()

    print(f"\n{'='*60}")
    print(f"  Nightly build complete: {result.summary()}")
    print(f"  Report: {report_path}")
    print(f"{'='*60}\n")

    return 1 if result.build_failed > 0 else 0


if __name__ == "__main__":
    raise SystemExit(main())
