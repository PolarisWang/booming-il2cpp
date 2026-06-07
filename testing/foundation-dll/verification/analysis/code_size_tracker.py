"""Code size tracker — per-function code size via .map file analysis.

Reads linker .map files (MSVC link.exe /MAP format) to extract per-function
.text section sizes. Falls back to dumpbin /HEADERS section-level when
.map is unavailable.

Usage:
    from verification.analysis.code_size_tracker import CodeSizeTracker

    tracker = CodeSizeTracker()
    sizes = tracker.collect("path/to/entry.exe")
"""

from __future__ import annotations

import json
import re
import subprocess
from pathlib import Path
from typing import Any


class CodeSizeTracker:
    """Track per-function and per-section code sizes from native binaries."""

    def collect(self, entry_exe_path: Path) -> dict[str, Any]:
        """Collect code size data from an entry.exe binary.

        Strategy:
          1. Try to find and parse the .map file (per-function detail)
          2. Fall back to dumpbin /HEADERS (section-level)

        Returns:
            Dict with keys: sectionSizes, functionSizes (if .map available), totalSize.
        """
        result: dict[str, Any] = {
            "sectionSizes": {"text": 0, "data": 0, "pdata": 0, "reloc": 0, "total": 0},
            "functionSizes": [],
            "source": "none",
        }

        if not entry_exe_path.exists():
            return result

        # Total file size
        result["sectionSizes"]["total"] = entry_exe_path.stat().st_size

        # Step 1: Try .map file
        map_path = entry_exe_path.with_suffix(".map")
        if map_path.exists():
            functions = self._parse_map_file(map_path)
            if functions:
                result["functionSizes"] = functions
                result["source"] = "map"

                # Derive section sizes from function data
                text_total = sum(f.get("textSize", 0) for f in functions)
                if text_total > 0:
                    result["sectionSizes"]["text"] = text_total

        # Step 2: dumpbin /HEADERS for section info
        section_sizes = self._dumpbin_sections(entry_exe_path)
        if section_sizes.get("text", 0) > 0:
            # Use dumpbin as the authoritative section sizes (more accurate)
            result["sectionSizes"].update(section_sizes)
            if result["source"] == "none":
                result["source"] = "dumpbin"

        return result

    def _parse_map_file(self, map_path: Path) -> list[dict]:
        """Parse MSVC linker .map file to extract per-function code sizes.

        The .map file has sections like:
          Address         Publics by Value              Rva+Base       Lib:Object

        0001:00000000       ?func1@@YAHXZ                00400000     f    foo.obj
        0001:00000080       ?func2@@YAXH@Z               00400080     f    bar.obj

        We extract segment:offset pairs from the "Publics by Value" section.
        """
        functions: list[dict] = []
        in_preferred = False

        try:
            text = map_path.read_text(encoding="utf-8", errors="replace")
        except OSError:
            return []

        for line in text.splitlines():
            # Detect start of "Publics by Value" section
            if "Publics by Value" in line:
                in_preferred = True
                continue
            if in_preferred and line.strip() == "":
                in_preferred = False
                continue
            if not in_preferred:
                continue

            # Parse line like:  0001:00000080       ?func@@YAHXZ   00400080     f    foo.obj
            m = re.match(
                r"\s*[0-9a-fA-F]+:([0-9a-fA-F]+)\s+"
                r"(\?\S+|\w+)\s+"
                r"([0-9a-fA-F]+)\s+"
                r"\S+\s+"
                r"(\S+)",
                line,
            )
            if not m:
                continue

            offset_hex = m.group(1)
            symbol = m.group(2)
            # Size is computed from the NEXT symbol's offset minus this one's offset
            # We can't easily compute per-function size from .map without sorting,
            # so we store raw offset for offline analysis
            try:
                offset = int(offset_hex, 16)
            except ValueError:
                continue

            functions.append({
                "symbol": symbol,
                "offset": offset,
                "textSize": 0,  # placeholder — computed in post-process
            })

        # Compute per-function sizes from offsets
        if len(functions) >= 2:
            # Sort by offset
            functions.sort(key=lambda f: f["offset"])
            for i in range(len(functions) - 1):
                functions[i]["textSize"] = functions[i + 1]["offset"] - functions[i]["offset"]
            # Last function — no next symbol to compute from
            if functions:
                functions[-1]["textSize"] = 0  # unknown without next symbol

        return functions

    def _dumpbin_sections(self, exe_path: Path) -> dict[str, int]:
        """Get section sizes from dumpbin /HEADERS output."""
        sizes: dict[str, int] = {}
        try:
            result = subprocess.run(
                ["dumpbin", "/HEADERS", str(exe_path)],
                capture_output=True, text=True, timeout=30,
            )
            if result.returncode == 0:
                for line in result.stdout.splitlines():
                    m = re.match(r"\s+\.(\w+)\s+size\s*=\s*0x([0-9a-fA-F]+)", line.strip())
                    if m:
                        section = m.group(1).lower()
                        size = int(m.group(2), 16)
                        sizes[section] = size
        except (FileNotFoundError, subprocess.TimeoutExpired, OSError):
            pass
        return sizes


# ── CLI entry point ──
def main() -> None:
    import argparse
    parser = argparse.ArgumentParser(description="Track code size from native binary")
    parser.add_argument("exe", help="Path to entry.exe")
    parser.add_argument("--json", action="store_true", help="Output as JSON")

    args = parser.parse_args()
    tracker = CodeSizeTracker()
    result = tracker.collect(Path(args.exe))

    if args.json:
        print(json.dumps(result, indent=2))
    else:
        sections = result["sectionSizes"]
        print(f"Code Size Report for {args.exe}")
        print(f"  Source: {result['source']}")
        print(f"  Total:  {sections.get('total', 0)} bytes")
        print(f"  .text:  {sections.get('text', 0)} bytes")
        print(f"  .data:  {sections.get('data', 0)} bytes")
        print(f"  .pdata: {sections.get('pdata', 0)} bytes")
        print(f"  .reloc: {sections.get('reloc', 0)} bytes")
        print(f"  Functions: {len(result.get('functionSizes', []))}")


if __name__ == "__main__":
    main()
