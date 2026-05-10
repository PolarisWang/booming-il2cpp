"""Patch generated vcxproj files to add compiler warning suppression.

CMake's VS generator hardcodes /EHc for extern "C" functions which causes
C4297 warnings when our generated code intentionally throws through extern "C"
frames. This script adds /wd4297 to suppress those warnings.

Usage:
    python _patch_vcxproj.py <build-dir>
"""

from __future__ import annotations

import sys
import re
from pathlib import Path


def patch_vcxproj(build_dir: Path) -> bool:
    """Patch all .vcxproj files in build_dir to suppress C4297."""
    patched_any = False
    for vcxproj in build_dir.glob("*.vcxproj"):
        content = vcxproj.read_text(encoding="utf-8")

        # Skip if already patched
        if "/wd4297" in content:
            continue

        # Find all ClCompile sections and add /wd4297 to AdditionalOptions
        # Pattern: <ClCompile> ... </ClCompile>
        clcompile_pattern = re.compile(
            r'(<ClCompile>.*?)(</ClCompile>)', re.DOTALL
        )

        def add_wd4297(m: re.Match) -> str:
            inner = m.group(1)
            close = m.group(2)
            # Check if there's already an AdditionalOptions
            if "<AdditionalOptions>" in inner:
                # Append to existing
                inner = inner.replace(
                    "</AdditionalOptions>",
                    " /wd4297</AdditionalOptions>"
                )
            else:
                # Add new AdditionalOptions element before closing
                inner += "      <AdditionalOptions>/wd4297 %(AdditionalOptions)</AdditionalOptions>\n"
            return inner + close

        new_content = clcompile_pattern.sub(add_wd4297, content)
        if new_content != content:
            vcxproj.write_text(new_content, encoding="utf-8")
            print(f"  [patch_vcxproj] patched {vcxproj.name} (+/wd4297)")
            patched_any = True
        else:
            print(f"  [patch_vcxproj] no ClCompile section found in {vcxproj.name}")

    return patched_any


def main() -> None:
    if len(sys.argv) < 2:
        print("Usage: python _patch_vcxproj.py <build-dir>")
        sys.exit(1)

    build_dir = Path(sys.argv[1])
    if not build_dir.exists():
        print(f"Build dir not found: {build_dir}")
        sys.exit(1)

    success = patch_vcxproj(build_dir)
    sys.exit(0 if success else 1)


if __name__ == "__main__":
    main()