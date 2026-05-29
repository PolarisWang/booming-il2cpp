"""Unified CLI entry point for foundation-dll verification pipeline.

Usage:
    cd testing/foundation-dll

    # Single family (default mode)
    python -m verification --slug convert-char
    python -m verification --slug value-typebox --mode strict
    python -m verification --slug convert-char --skip benchmark,hotupdate

    # Batch all families
    python -m verification --batch
    python -m verification --batch --mode strict --resume convert-char

    # CI smoke test (4 families, quick 6-stage)
    python -m verification --ci
    python -m verification --ci --families convert-char,boxing-unboxing-casts

    # Managed-only benchmark
    python -m verification --managed-bench
    python -m verification --managed-bench --parallel 8
"""

from __future__ import annotations

import argparse
import sys
from pathlib import Path


def _setup_path() -> None:
    """Ensure testing/foundation-dll/ is on sys.path so 'verification' is importable."""
    pkg_root = Path(__file__).resolve().parent.parent  # testing/foundation-dll/
    if str(pkg_root) not in sys.path:
        sys.path.insert(0, str(pkg_root))


def _build_parser() -> argparse.ArgumentParser:
    parser = argparse.ArgumentParser(
        description="Foundation-dll verification pipeline — single entry point",
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog=__doc__,
    )

    # ── Mode flags (mutually exclusive) ──────────────────────────────
    mode = parser.add_argument_group("mode (mutually exclusive, default=family)")
    mode.add_argument("--batch", action="store_true", help="Run batch on all families")
    mode.add_argument("--ci", action="store_true", help="CI smoke test (4 families, quick)")
    mode.add_argument("--managed-bench", action="store_true",
                      help="Managed-only benchmark (skip native stages)")

    # ── Global flags ─────────────────────────────────────────────────
    global_ = parser.add_argument_group("global flags")
    global_.add_argument("--slug", default=None,
                         help="Family slug (required in single-family mode)")
    global_.add_argument("--assembly", default="System.Private.CoreLib",
                         help="Assembly name (default: System.Private.CoreLib)")
    global_.add_argument("--mode", choices=["standard", "strict"], default="standard",
                         help="Verification mode (default: standard)")
    global_.add_argument("--native-config", choices=["check", "profile", "ship"],
                         default="check",
                         help="Native build config (default: check)")
    global_.add_argument("--skip-stages", "--skip", default=None,
                         help="Comma-separated stages to skip: "
                              "preflight,codegen,jit_codegen,fact,fact_jit,audit,"
                              "asm_compare,microbench,benchmark,hotupdate")
    global_.add_argument("--output", "-o", default=None,
                         help="Report output path")

    # ── Mode-specific flags ──────────────────────────────────────────
    spec = parser.add_argument_group("mode-specific flags")
    spec.add_argument("--resume", default=None,
                      help="[batch] Resume from this slug")
    spec.add_argument("--families", default=None,
                      help="[ci] Comma-separated family slugs")
    spec.add_argument("--parallel", type=int, default=None,
                      help="[managed-bench] Number of parallel workers")

    return parser


def main() -> None:
    _setup_path()

    # Now verification package is on sys.path
    parser = _build_parser()
    args = parser.parse_args()

    skip_stages = set()
    if args.skip_stages:
        skip_stages = set(s.strip() for s in args.skip_stages.split(","))

    # Dispatch by mode
    if args.batch:
        _run_batch(args, skip_stages)
    elif args.ci:
        _run_ci(args, skip_stages)
    elif args.managed_bench:
        _run_managed_bench(args)
    else:
        _run_single(args, skip_stages)


def _run_single(args: argparse.Namespace, skip_stages: set[str]) -> None:
    """Single-family mode (default)."""
    if not args.slug:
        print("ERROR: --slug is required in single-family mode")
        print("  Use --batch for batch mode, --ci for CI smoke, --managed-bench for managed-only")
        sys.exit(1)

    from verification.orchestration.context import FamilyContext
    from verification.orchestration.engine import VerificationPipeline
    from verification.analysis.perf_store import auto_save_perf_data

    from verification.entry_points.cli import _resolve_family_dir

    family_dir = _resolve_family_dir(args.slug, args.assembly)
    if not family_dir.exists():
        print(f"ERROR: Family directory not found: {family_dir}")
        sys.exit(1)

    ctx = FamilyContext(
        slug=args.slug,
        assembly=args.assembly,
        family_dir=family_dir,
        mode=args.mode,
        skip_stages=skip_stages,
        native_config=args.native_config,
    )

    pipeline = VerificationPipeline(ctx)
    report = pipeline.run()
    auto_save_perf_data(ctx, report.stages)

    report_path = ctx.results_dir / "unified-verification-report.json"
    report_path.parent.mkdir(parents=True, exist_ok=True)
    report_path.write_text(report.to_json(), encoding="utf-8")

    if args.output:
        Path(args.output).write_text(report.to_json(), encoding="utf-8")

    print(report.to_json())
    sys.exit(0 if report.overall_status == "passed" else 1)


def _run_batch(args: argparse.Namespace, skip_stages: set[str]) -> None:
    """Batch all families."""
    from verification.entry_points.batch import discover_families, main as batch_main

    if args.slug:
        # Single family via --batch flag (convenience)
        from verification.orchestration.context import FamilyContext
        from verification.orchestration.engine import VerificationPipeline
        from verification.analysis.perf_store import auto_save_perf_data

        from verification.entry_points.batch import run_family

        result = run_family(
            args.slug, args.assembly, skip_stages,
            native_config=args.native_config, mode=args.mode,
        )

        if args.output:
            import json
            Path(args.output).write_text(
                json.dumps(result, indent=2, ensure_ascii=False), encoding="utf-8")

        sys.exit(0 if result["status"] == "passed" else 1)
    else:
        # Full batch run
        # Reconstruct sys.argv for batch.main() to consume
        import shlex
        batch_argv = [sys.argv[0]]
        if args.assembly != "System.Private.CoreLib":
            batch_argv += ["--assembly", args.assembly]
        if args.mode != "standard":
            batch_argv += ["--mode", args.mode]
        if args.native_config != "check":
            batch_argv += ["--native-config", args.native_config]
        if args.skip_stages:
            batch_argv += ["--skip-stages", args.skip_stages]
        if args.resume:
            batch_argv += ["--resume", args.resume]
        if args.output:
            batch_argv += ["--output", args.output]

        old_argv = sys.argv
        sys.argv = batch_argv
        try:
            batch_main()
        finally:
            sys.argv = old_argv


def _run_ci(args: argparse.Namespace, skip_stages: set[str]) -> None:
    """CI smoke test — 4 families, quick 6-stage."""
    from verification.entry_points.ci_smoke import main as ci_main

    import shlex
    ci_argv = [sys.argv[0]]
    if args.assembly != "System.Private.CoreLib":
        ci_argv += ["--assembly", args.assembly]
    if args.families:
        ci_argv += ["--families", args.families]
    if args.skip_stages:
        ci_argv += ["--skip-stages", args.skip_stages]
    if args.output:
        ci_argv += ["--output", args.output]

    old_argv = sys.argv
    sys.argv = ci_argv
    try:
        ci_main()
    finally:
        sys.argv = old_argv


def _run_managed_bench(args: argparse.Namespace) -> None:
    """Managed-only benchmark."""
    from verification.entry_points.fast_managed_bench import main as bench_main

    import shlex
    bench_argv = [sys.argv[0]]
    if args.assembly != "System.Private.CoreLib":
        bench_argv += ["--assembly", args.assembly]
    if args.output:
        bench_argv += ["--output", args.output]
    if args.parallel is not None:
        bench_argv += ["--parallel", str(args.parallel)]

    old_argv = sys.argv
    sys.argv = bench_argv
    try:
        bench_main()
    finally:
        sys.argv = old_argv


if __name__ == "__main__":
    main()
