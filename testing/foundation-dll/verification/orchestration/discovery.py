"""Chunk discovery — read namespace-partition.json for chunk slugs.

Usage:
    from verification.orchestration.discovery import discover_chunks
"""

from __future__ import annotations

import json
from pathlib import Path


def discover_chunks(assembly: str = "System.Private.CoreLib",
                    verbose: bool = False,
                    foundation_dir: Path | None = None) -> list[str]:
    """Read namespace-partition.json and return sorted chunk slugs."""
    if foundation_dir is None:
        from verification._path import _HERE as _VERIFICATION_ROOT
        foundation_dir = _VERIFICATION_ROOT.parent / assembly

    partition_path = foundation_dir / "_dll" / "namespace-partition.json"
    if not partition_path.exists():
        print(f"WARNING: namespace-partition.json not found: {partition_path}")
        return []

    try:
        partition = json.loads(partition_path.read_text(encoding="utf-8"))
    except (json.JSONDecodeError, OSError) as e:
        print(f"WARNING: Failed to read {partition_path}: {e}")
        return []

    chunks = partition.get("chunks", [])
    slugs = [chunk["slug"] for chunk in chunks if "slug" in chunk]

    if verbose:
        print(f"  [discover] {len(slugs)} chunks from {assembly}")
        for slug in slugs:
            print(f"    - {slug}")

    return slugs
