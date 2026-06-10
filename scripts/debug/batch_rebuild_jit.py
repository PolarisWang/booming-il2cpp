#!/usr/bin/env python3
"""Batch rebuild entry-jit.exe with real verification dispatch for all families.

Note: The compound family name is "interface-dispatch,pinvoke-dllimport,primitive-numeric-conversions-core"
but the orchestrator may use a different slug. This script works directly with the
testing directory structure.

Usage:
    python batch_rebuild_jit.py
"""

from __future__ import annotations

import os
import sys
import shutil
import subprocess
import textwrap
from pathlib import Path

# Add the new pipeline directory to sys.path
_PIPELINE_DIR = Path(__file__).resolve().parent / "testing" / "foundation-dll" / "verification" / "stages"
sys.path.insert(0, str(_PIPELINE_DIR))

from pipeline_native_aot_runner import build_entry_executable, ensure_cmake_lists_file  # type: ignore[import-unverified]

_VERIFICATION_BASE = Path("testing/foundation-dll/System.Private.CoreLib")


def get_all_families() -> list[str]:
    """Return all family slugs sorted."""
    families = []
    for d in sorted(_VERIFICATION_BASE.iterdir()):
        if d.is_dir() and (d / "codegen").exists():
            families.append(d.name)
    return families


def rebuild_jit_family(family_slug: str) -> bool:
    """Rebuild entry-jit.exe for one family, preserving real verification dispatch."""
    family_dir = _VERIFICATION_BASE / family_slug
    native_dir = family_dir / "native"
    dispatch_cpp = native_dir / "verification_dispatch.generated.cpp"
    jit_exe = native_dir / "entry-jit.exe"

    # Save the real verification dispatch content
    saved_dispatch = None
    if dispatch_cpp.exists():
        saved_dispatch = dispatch_cpp.read_bytes()

    # Use the build_entry_executable with is_jit=True
    # This cleans native/, regenerates CMakeLists, and rebuilds
    print(f"\n  [{family_slug}] Rebuilding entry-jit.exe...")
    ok = build_entry_executable(
        family_slug,
        verification=_VERIFICATION_BASE,
        output_name="entry-jit.exe",
        is_jit=True,
    )

    if not ok:
        print(f"  [{family_slug}] FAILED: build_entry_executable returned False")
        return False

    # Restore the real verification dispatch if we saved it
    if saved_dispatch is not None:
        dispatch_cpp.write_bytes(saved_dispatch)
        print(f"  [{family_slug}] restored verification_dispatch.generated.cpp")

        # Rebuild just the dispatch file and re-link
        build_dir = native_dir / "build"
        if build_dir.exists():
            # Need to re-run cmake configure since CMakeLists may have changed
            result = subprocess.run(
                ["cmake", "--build", ".", "--config", "RelWithDebInfo", "--target", "entry"],
                cwd=str(build_dir),
                capture_output=True, text=True, timeout=300,
            )
            if result.returncode != 0:
                print(f"  [{family_slug}] FAILED: cmake --build (dispatch relink)")
                print(f"    stderr: {result.stderr[-500:]}")
                return False
            print(f"  [{family_slug}] dispatch re-linked into entry-jit.exe")
        else:
            print(f"  [{family_slug}] WARNING: no build directory, skip dispatch relink")
    else:
        print(f"  [{family_slug}] WARNING: no verification dispatch to restore")

    # Verify entry-jit.exe exists
    if jit_exe.exists():
        size_kb = jit_exe.stat().st_size / 1024
        print(f"  [{family_slug}] entry-jit.exe rebuilt: {size_kb:.0f} KB")
        return True
    else:
        print(f"  [{family_slug}] FAILED: entry-jit.exe not found after rebuild")
        return False


def main():
    families = get_all_families()
    print(f"Found {len(families)} families")

    passed = 0
    failed = 0

    for idx, family_slug in enumerate(families, 1):
        print(f"\n[{idx}/{len(families)}] {family_slug}")
        ok = rebuild_jit_family(family_slug)
        if ok:
            passed += 1
        else:
            failed += 1

    print(f"\n{'='*60}")
    print(f"Summary: passed={passed}, failed={failed}, total={len(families)}")

    if failed > 0:
        sys.exit(1)


if __name__ == "__main__":
    main()
