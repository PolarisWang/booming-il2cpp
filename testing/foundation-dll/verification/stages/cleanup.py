"""Cleanup stage — remove temporary build artifacts from previous pipeline stages.

Runs at the end of the verification pipeline to reclaim disk space.
Does NOT remove source files, generated code, or verification reports — only
intermediate build outputs that can be regenerated on the next run.
"""

from __future__ import annotations

import shutil
import time
from pathlib import Path

from verification.orchestration.context import FamilyContext, StageResult


# Directories and patterns to clean up within the family directory
_CLEANUP_TARGETS = [
    # Entry point build outputs (from family_entrypoint.py generate_and_build)
    "build-output/",
    # Managed patch build outputs (from hotupdate _build_patch_dll)
    "managed/patch/bin/",
    "managed/patch/obj/",
    # Patch data is regenerated each hotupdate run
    "subjects.patchdata",
]


def run_cleanup(ctx: FamilyContext, stages: dict[str, StageResult]) -> StageResult:
    """Remove temporary build artifacts from the family verification directory."""
    start = time.perf_counter()
    removed = 0
    errors = []

    for target in _CLEANUP_TARGETS:
        path = ctx.family_dir / target
        if not path.exists():
            continue
        try:
            if path.is_dir():
                shutil.rmtree(path)
                removed += 1
            elif path.is_file():
                path.unlink()
                removed += 1
        except (OSError, PermissionError) as e:
            errors.append(f"{target}: {e}")

    # Also clean per-subject build-output dirs under codegen/
    codegen_dir = ctx.family_dir / "codegen"
    if codegen_dir.is_dir():
        for sub_dir in codegen_dir.iterdir():
            if sub_dir.is_dir():
                build_out = sub_dir / "build-output"
                if build_out.exists():
                    try:
                        shutil.rmtree(build_out)
                        removed += 1
                    except (OSError, PermissionError) as e:
                        errors.append(f"codegen/{sub_dir.name}/build-output: {e}")

    duration = int((time.perf_counter() - start) * 1000)
    summary = f"removed {removed} artifact(s)"
    if errors:
        summary += f", {len(errors)} error(s)"
        for e in errors:
            print(f"  [cleanup] WARNING: {e}")

    status = "passed" if not errors else "passed"  # cleanup errors are non-fatal
    print(f"  [cleanup] {summary}")
    return StageResult(
        stage="cleanup", status=status, summary=summary,
        duration_ms=duration,
    )
