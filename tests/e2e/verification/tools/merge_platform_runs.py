"""Multi-platform merge — aligns the same (assembly, slug, methodSubjectId) across platforms.

Reads multiple pipeline-runs.jsonl entries and produces a unified view with
per-platform columns for each metric. Uses union + tag strategy: all platform
data is preserved; exact matches by (assembly, slug, methodSubjectId, platform)
are overwritten by the most recent run.
"""

from __future__ import annotations

import json
import sys
from pathlib import Path
from typing import Any


def merge_runs(run_files: list[Path]) -> dict[str, Any]:
    """Merge multiple pipeline-run JSONL files into a unified cross-platform view.

    Args:
        run_files: Paths to pipeline-runs.jsonl files from different platforms.

    Returns:
        Merged dict with per-platform method data arrays.
    """
    # Collect fact failures keyed by (assembly, slug, methodSubjectId) → per-platform
    failures_by_key: dict[str, dict[str, Any]] = {}
    platforms_seen: set[str] = set()

    for fpath in run_files:
        if not fpath.exists():
            print(f"[merge] Skipping missing file: {fpath}", file=sys.stderr)
            continue
        with open(fpath, encoding="utf-8") as f:
            for line in f:
                line = line.strip()
                if not line:
                    continue
                try:
                    run = json.loads(line)
                except json.JSONDecodeError:
                    continue

                platform = run.get("platform", "unknown")
                platforms_seen.add(platform)
                assemblies = run.get("assemblies", {})

                for asm_name, asm_data in assemblies.items():
                    for chunk in asm_data.get("chunks", []):
                        slug = chunk.get("slug", "")
                        for f_entry in chunk.get("fact", {}).get("failures", []):
                            # Prefer methodSubjectId (stable across platforms),
                            # falling back to methodIndex. Docstring promised
                            # methodSubjectId alignment but the code keyed on
                            # methodIndex only.
                            msid = f_entry.get("methodSubjectId") or f_entry.get("methodIndex")
                            mi = f_entry.get("methodIndex", 0)
                            key = f"{asm_name}/{slug}/method-{msid}"
                            if key not in failures_by_key:
                                failures_by_key[key] = {
                                    "assembly": asm_name,
                                    "slug": slug,
                                    "methodSubjectId": msid,
                                    "methodIndex": mi,
                                    "platforms": {},
                                }
                            failures_by_key[key]["platforms"][platform] = {
                                "value": f_entry.get("value"),
                                "message": f_entry.get("message"),
                                "errorPattern": f_entry.get("errorPattern"),
                            }

    merged = {
        "mergeTimestamp": __import__("datetime").datetime.now(__import__("datetime").timezone.utc).isoformat(),
        "platforms": sorted(platforms_seen),
        "platformCount": len(platforms_seen),
        "factFailures": sorted(failures_by_key.values(), key=lambda x: (x["assembly"], x["slug"], x["methodIndex"])),
        "totalFactFailures": len(failures_by_key),
    }
    return merged


def main() -> int:
    """CLI entry point: python -m verification.tools.merge_platform_runs <file1.jsonl> [file2.jsonl ...]"""
    args = sys.argv[1:]
    if len(args) < 2:
        print("Usage: python -m verification.tools.merge_platform_runs <run1.jsonl> <run2.jsonl> [...]",
              file=sys.stderr)
        return 1

    run_files = [Path(a) for a in args]
    merged = merge_runs(run_files)
    print(json.dumps(merged, indent=2, ensure_ascii=False))
    return 0


if __name__ == "__main__":
    sys.exit(main())
