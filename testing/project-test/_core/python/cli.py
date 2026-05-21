"""CLI entry point for project verification.

Usage:
    python -m testing.project_test._core.python.cli runtime-self-test
    python -m testing.project_test._core.python.cli runtime-self-test --strict
    python -m testing.project_test._core.python.cli runtime-self-test --skip benchmark
"""

from __future__ import annotations

import argparse
import sys
from pathlib import Path

# Ensure project-test _core is importable
_HERE = Path(__file__).resolve().parent
_CORE_PACKAGE = _HERE.parents[1]  # testing/project-test/
if str(_CORE_PACKAGE) not in sys.path:
    sys.path.insert(0, str(_CORE_PACKAGE))

# Ensure testing/ is importable for _pipeline shared components
_TESTING_ROOT = _CORE_PACKAGE.parent  # testing/
if str(_TESTING_ROOT) not in sys.path:
    sys.path.insert(0, str(_TESTING_ROOT))

# NOTE: When full codegen integration is needed, add foundation-dll paths
# with sys.path.append() to avoid shadowing project-test's _core package:
#   sys.path.append(str(_TESTING_ROOT / "foundation-dll"))

from _core.python.models import ProjectContext
from _core.python.orchestrator import ProjectPipeline


def resolve_project_dir(slug: str) -> Path:
    """Resolve project directory under testing/project-test/<slug>/."""
    return _TESTING_ROOT / "project-test" / slug


def main() -> None:
    parser = argparse.ArgumentParser(description="Complete project verification")
    parser.add_argument("project_slug", help="Project slug (e.g., runtime-self-test)")
    parser.add_argument("--mode", choices=["standard", "strict"], default="standard",
                        help="Verification mode (default: standard)")
    parser.add_argument("--skip", nargs="*", default=[],
                        help="Stages to skip")
    parser.add_argument("--verbose", "-v", action="store_true",
                        help="Verbose output")

    args = parser.parse_args()
    project_dir = resolve_project_dir(args.project_slug)

    if not project_dir.exists():
        print(f"ERROR: Project directory not found: {project_dir}")
        sys.exit(1)

    ctx = ProjectContext(
        slug=args.project_slug,
        root_dir=project_dir,
        mode=args.mode,
        skip_stages=set(args.skip),
        verbose=args.verbose,
    )

    pipeline = ProjectPipeline(ctx)
    report = pipeline.run()

    print(report.to_json())

    if report.overall_status == "passed":
        sys.exit(0)
    else:
        sys.exit(1)


if __name__ == "__main__":
    main()
