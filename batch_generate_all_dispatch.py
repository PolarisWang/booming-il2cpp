#!/usr/bin/env python3
"""Batch-generate verification_dispatch.generated.cpp for ALL families.

Reads native-aot.methods.json from each family's codegen output and generates
a real verification_dispatch.generated.cpp.  Also regenerates the CMakeLists.txt
for any family that uses flat layout (missing chaos_generated_module.cpp glob).

Usage:
    python batch_generate_all_dispatch.py
"""

from __future__ import annotations

import os
import sys
import subprocess
from pathlib import Path

# Add the verification tooling directory to sys.path
_PIPELINE_DIR = Path(__file__).resolve().parent / "testing" / "foundation-dll" / "verification" / "tooling"
sys.path.insert(0, str(_PIPELINE_DIR))

from verification_dispatch_generator import generate_verification_dispatch

_VERIFICATION_BASE = Path("testing/foundation-dll/System.Private.CoreLib")


def get_all_families() -> list[str]:
    """Return all family slugs sorted."""
    families = []
    for d in sorted(_VERIFICATION_BASE.iterdir()):
        if d.is_dir() and (d / "codegen").exists():
            families.append(d.name)
    return families


def find_manifest(family_dir: Path) -> Path | None:
    """Find native-aot.methods.json in a family's codegen output."""
    codegen_dir = family_dir / "codegen"
    for d in codegen_dir.iterdir():
        if d.is_dir() and d.name.endswith("Subjects"):
            candidate = d / "generated" / "native-aot.methods.json"
            if candidate.exists():
                return candidate
        if d.name == "generated":
            # Also check flat codegen/generated/
            candidate = d / "native-aot.methods.json"
            if candidate.exists():
                return candidate
    # Also check flat layout at codegen/
    candidate = codegen_dir / "native-aot.methods.json"
    if candidate.exists():
        return candidate
    return None


def ensure_cmakelists(family_dir: Path) -> bool:
    """Fix CMakeLists.txt if it's missing the flat layout chaos_generated_module.cpp glob."""
    cmake_path = family_dir / "native" / "CMakeLists.txt"
    if not cmake_path.exists():
        return False

    content = cmake_path.read_text(encoding="utf-8")

    # Check if flat layout chaos_generated_module.cpp glob is missing
    # The correct glob should include *Subjects/generated/chaos_generated_module.cpp
    has_flat_module = (
        '"${CHAOS_CODEGEN_DIR}/*Subjects/generated/chaos_generated_module.cpp"' in content
        or '"${CMAKE_CURRENT_SOURCE_DIR}/*Subjects/generated/chaos_generated_module.cpp"' in content
    )

    if has_flat_module:
        return False  # Already has it

    # Add flat layout glob after the existing chaos_generated_module.cpp lines
    # First, check if we even need it — does the family USE flat layout?
    # Flat layout means native-aot.generated.cpp is in *Subjects/generated/ directly
    # AND there's no *Subjects/generated/generated/ directory
    subjects_dirs = list((family_dir / "native").glob("*Subjects"))
    needs_flat = False
    for sd in subjects_dirs:
        nested_dir = sd / "generated" / "generated"
        if not nested_dir.exists():
            needs_flat = True
            break

    if not needs_flat:
        return False  # Uses paged/nested layout, existing glob is fine

    # Add flat layout glob lines
    # Replace the lines containing *Subjects/generated/generated/chaos_generated_module.cpp
    # with both flat and nested versions
    for pattern_key in [
        "CHAOS_CODEGEN_DIR}/*Subjects/generated/generated/chaos_generated_module.cpp",
        "CMAKE_CURRENT_SOURCE_DIR}/*Subjects/generated/generated/chaos_generated_module.cpp",
    ]:
        old_line = f'    "${{{pattern_key}"\n'
        # Insert flat layout version right before the nested version
        flat_key = pattern_key.replace("/generated/chaos_generated_module.cpp", "/chaos_generated_module.cpp")
        flat_line = f'    "${{{flat_key}"\n'
        content = content.replace(old_line, flat_line + old_line)

    cmake_path.write_text(content, encoding="utf-8")
    print(f"  [cmake] fixed flat layout glob: {cmake_path}")
    return True


def get_entry_executable(family_dir: Path) -> Path | None:
    """Find the entry executable (entry-jit.exe preferred, then entry.exe)."""
    native_dir = family_dir / "native"
    for name in ["entry-jit.exe", "entry.exe"]:
        p = native_dir / name
        if p.exists():
            return p
    return None


def batch_generate_dispatch(families: list[str]) -> tuple[int, int]:
    """Generate verification_dispatch.generated.cpp for all families."""
    generated = 0
    skipped = 0
    fixed_cmake = 0

    for family_slug in families:
        family_dir = _VERIFICATION_BASE / family_slug
        dispatch_output = family_dir / "native" / "verification_dispatch.generated.cpp"

        # Find manifest
        manifest_path = find_manifest(family_dir)
        if manifest_path is None:
            print(f"  [skip] {family_slug}: manifest not found")
            skipped += 1
            continue

        # Generate dispatch
        try:
            generate_verification_dispatch(str(manifest_path), str(dispatch_output))
            generated += 1
        except Exception as e:
            print(f"  [ERROR] {family_slug}: {e}")
            skipped += 1

        # Fix CMakeLists if needed
        if ensure_cmakelists(family_dir):
            fixed_cmake += 1

    return generated, skipped, fixed_cmake


def main():
    families = get_all_families()
    print(f"Found {len(families)} families\n")

    generated, skipped, fixed_cmake = batch_generate_dispatch(families)

    print(f"\nSummary: generated={generated}, skipped={skipped}, cmake_fixed={fixed_cmake}, total={len(families)}")


if __name__ == "__main__":
    main()
