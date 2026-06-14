#!/usr/bin/env python3
"""Batch hotupdate + coverage-audit runner for foundation-dll families.

Usage:
    cd testing/foundation-dll
    python verification/stages/batch_hotupdate_audit.py
"""

from __future__ import annotations

import json
import os
import subprocess
import sys
import time
from pathlib import Path

_FOUNDATION_DLL = Path(__file__).resolve().parents[2]

# Families with entry.exe (from previous build+fact)
HAS_EXE_FAMILIES = [
    "System.Collections",
    "System.Collections.Immutable",
    "System.ComponentModel.TypeConverter",
    "System.Data.Common",
    "System.Diagnostics.DiagnosticSource",
    "System.Formats.Asn1",
    "System.IO.Compression.Brotli",
    "System.IO.Compression.ZipFile",
    "System.IO.Pipelines",
    "System.Linq",
    "System.Linq.Expressions",
    "System.Net.Http",
    "System.Net.ServerSentEvents",
    "System.Net.Sockets",
    "System.ObjectModel",
    "System.Private.CoreLib",
    "System.Private.Xml",
    "System.Reflection.Metadata",
    "System.Runtime.InteropServices",
    "System.Runtime.Serialization.Formatters",
    "System.Security.Claims",
    "System.Security.Cryptography",
    "System.Security.Principal.Windows",
    "System.Text.Json",
    "System.Threading.Tasks.Parallel",
]

# Families that already passed hotupdate (with proper patch data flow)
ALREADY_PASSED_HOTUPDATE = {
    "System.Collections", "System.Collections.Immutable",
    "System.ComponentModel.TypeConverter",
    "System.Data.Common", "System.Diagnostics.DiagnosticSource",
    "System.Formats.Asn1", "System.IO.Compression.Brotli",
    "System.IO.Pipelines", "System.Linq", "System.Linq.Expressions",
    "System.Net.Http", "System.Net.ServerSentEvents", "System.Net.Sockets",
    "System.Private.Xml",
    "System.Reflection.Metadata", "System.Runtime.Serialization.Formatters",
    "System.Security.Cryptography",
    "System.Security.Principal.Windows", "System.Text.Json",
    "System.Threading.Tasks.Parallel",
}


def run_stage(family: str, chunk_slug: str, stage: str) -> dict:
    """Run a single stage for a family chunk via chunk_pipeline."""
    base_dir = _FOUNDATION_DLL / family
    if not base_dir.is_dir():
        return {"family": family, "chunk": chunk_slug, "stage": stage, "status": "error", "detail": "family dir not found"}

    cmd = [
        sys.executable, "-m", "verification.chunk_pipeline",
        "--assembly", family,
        "--chunk", chunk_slug,
        "--stages", stage,
        "--skip-probe",
    ]
    print(f"  [{family}/{chunk_slug}] Running {stage}...")
    start = time.perf_counter()
    result = subprocess.run(cmd, capture_output=True, text=True, encoding='utf-8', errors='replace', timeout=1800, cwd=_FOUNDATION_DLL)
    elapsed = time.perf_counter() - start

    # Print output
    for line in result.stdout.splitlines():
        print(f"    {line}")
    if result.returncode != 0:
        for line in result.stderr.splitlines()[-5:]:
            print(f"    ERR: {line}")

    # Try to read the stage result from the chunk's results directory
    result_path = base_dir / "chunks" / chunk_slug / "results" / f"{stage.replace('-', '_')}.json"
    status = "unknown"
    detail = f"exit={result.returncode}, elapsed={elapsed:.0f}s"
    if result_path.exists():
        try:
            data = json.loads(result_path.read_text(encoding="utf-8"))
            status = data.get("status", "unknown")
            summary = data.get("summary", "")
            detail = f"{status}: {summary} ({elapsed:.0f}s)"
        except (json.JSONDecodeError, OSError):
            pass

    return {"family": family, "chunk": chunk_slug, "stage": stage, "status": status, "detail": detail}


def find_chunks(family: str) -> list[str]:
    """Find all chunk slugs for a family."""
    chunks_dir = _FOUNDATION_DLL / family / "chunks"
    if not chunks_dir.is_dir():
        return []
    return sorted(d.name for d in chunks_dir.iterdir() if d.is_dir())


def has_subjects_metadata(family: str, chunk_slug: str) -> bool:
    """Check if the chunk has subjects metadata."""
    meta = _FOUNDATION_DLL / family / "chunks" / chunk_slug / "managed" / "subjects" / "subjects.metadata.json"
    return meta.exists()


def main():
    start_all = time.perf_counter()

    # ── Phase 1: Hotupdate for families that need it ──
    hotupdate_results: list[dict] = []
    for family in HAS_EXE_FAMILIES:
        if family in ALREADY_PASSED_HOTUPDATE:
            print(f"\n  SKIP hotupdate [{family}]: already passed in previous run")
            continue

        chunks = find_chunks(family)
        if not chunks:
            print(f"\n  SKIP hotupdate [{family}]: no chunks")
            continue

        # Run hotupdate for the first chunk (most families have 1 chunk)
        chunk = chunks[0]
        if not has_subjects_metadata(family, chunk):
            print(f"\n  SKIP hotupdate [{family}/{chunk}]: no subjects metadata")
            continue

        print(f"\n{'='*60}")
        print(f"  HOTUPDATE: {family} / {chunk}")
        print(f"{'='*60}")
        result = run_stage(family, chunk, "hotupdate")
        hotupdate_results.append(result)
        print(f"  => {result['detail']}")

    # ── Phase 2: Summary ──
    total_elapsed = time.perf_counter() - start_all
    print(f"\n\n{'='*60}")
    print(f"BATCH COMPLETE ({total_elapsed:.0f}s)")
    print(f"{'='*60}")

    print(f"\n--- Hotupdate Results ---")
    for r in hotupdate_results:
        print(f"  {r['family']:40s} {r['status']:20s} {r['detail']}")

    # Save to JSON
    output = {
        "hotupdate": hotupdate_results,
        "elapsed_s": total_elapsed,
    }
    output_path = _FOUNDATION_DLL / "_batch_hotupdate_audit_results.json"
    output_path.write_text(json.dumps(output, indent=2), encoding="utf-8")
    print(f"\nResults saved to {output_path}")

    return 0


if __name__ == "__main__":
    sys.exit(main())
