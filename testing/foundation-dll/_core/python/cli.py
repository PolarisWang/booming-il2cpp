"""CLI entry point for foundation DLL family verification.

Usage:
    python -m testing.foundation_dll._core.python.cli convert-char
    python -m testing.foundation_dll._core.python.cli convert-char --strict
    python -m testing.foundation_dll._core.python.cli convert-char --skip benchmark hotupdate
"""

from __future__ import annotations

import argparse
import sys
from pathlib import Path

# Ensure _core is importable
_HERE = Path(__file__).resolve().parent
_CORE_PACKAGE = _HERE.parents[1]  # testing/foundation-dll/
if str(_CORE_PACKAGE) not in sys.path:
    sys.path.insert(0, str(_CORE_PACKAGE))

# Ensure foundation_dll adapter package is importable
_TESTING_SRC_PYTHON = _CORE_PACKAGE.parent / "src" / "python"  # testing/src/python/
if str(_TESTING_SRC_PYTHON) not in sys.path:
    sys.path.insert(0, str(_TESTING_SRC_PYTHON))

# Ensure old pipeline modules are importable via adapter
_OLD_PIPELINE = _HERE.parents[4] / "build" / "toolchains" / "run" / "testing" / "foundation_dll"
if str(_OLD_PIPELINE) not in sys.path:
    sys.path.insert(0, str(_OLD_PIPELINE))

from _core.python.models import FamilyContext
from _core.python.orchestrator import VerificationPipeline

_TESTING_ROOT = _CORE_PACKAGE.parent  # testing/


def _resolve_family_dir(slug: str, assembly: str) -> Path:
    """Resolve family directory under testing/foundation-dll/<assembly>/<slug>/."""
    return _TESTING_ROOT / "foundation-dll" / assembly / slug


def main() -> None:
    parser = argparse.ArgumentParser(description="Foundation DLL family verification")
    parser.add_argument("family_slug", help="Family slug (e.g., convert-char)")
    parser.add_argument("--assembly", default="System.Private.CoreLib",
                        help="Assembly name (default: System.Private.CoreLib)")
    parser.add_argument("--mode", choices=["standard", "strict"], default="standard",
                        help="Verification mode (default: standard)")
    parser.add_argument("--skip", nargs="*", default=[],
                        help="Stages to skip: preflight codegen jit_codegen fact fact_jit audit asm_compare microbench benchmark hotupdate")
    parser.add_argument("--verbose", "-v", action="store_true",
                        help="Verbose output")

    args = parser.parse_args()
    family_dir = _resolve_family_dir(args.family_slug, args.assembly)

    if not family_dir.exists():
        print(f"ERROR: Family directory not found: {family_dir}")
        print(f"  Has the family been imported yet?")
        sys.exit(1)

    ctx = FamilyContext(
        slug=args.family_slug,
        assembly=args.assembly,
        family_dir=family_dir,
        mode=args.mode,
        skip_stages=set(args.skip),
        verbose=args.verbose,
    )

    pipeline = VerificationPipeline(ctx)
    report = pipeline.run()

    print(report.to_json())

    if report.overall_status == "passed":
        sys.exit(0)
    else:
        sys.exit(1)


if __name__ == "__main__":
    main()
