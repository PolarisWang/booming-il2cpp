"""Cross-technology profile comparison — AOT vs .NET 8/10 GC and allocation.

Aligns profile data from two sources:
  - entry.exe --profile (chaos-aot): gcPauseNs, nurseryAllocBytes, fastPathRate
  - managed benchmark runner (net8-jit, net10-jit): totalAllocatedBytes, collectionCount0

Outputs per-method comparison by methodSubjectId, highlighting methods where
AOT's GC/allocation behavior differs significantly from .NET JIT.

Usage:
    from verification.analysis.profile_compare import compare_profile_data

    result = compare_profile_data(aot_profile, managed_records, metadata_methods)
    print_report(result)
"""

from __future__ import annotations

import json
from pathlib import Path
from typing import Any


def _build_managed_map(managed_records: list[dict]) -> dict[str, dict]:
    """Build {methodSubjectId: gcInfo} from managed benchmark records.

    Keeps only records with non-empty gcInfo and status='completed'.
    """
    result: dict[str, dict] = {}
    for rec in managed_records:
        if rec.get("status") != "completed":
            continue
        gc_info = rec.get("gcInfo")
        if not gc_info or not gc_info.get("totalAllocatedBytes", 0):
            continue
        msid = rec.get("methodSubjectId", "")
        if msid:
            result[msid] = gc_info
    return result


def _build_profile_map(profile_data: list[dict]) -> dict[str, dict]:
    """Build {methodSubjectId: profile} from AOT profile data.

    Falls back to index-based matching when methodSubjectId is not set.
    """
    result: dict[str, dict] = {}
    for entry in profile_data:
        msid = entry.get("methodSubjectId", "")
        if msid:
            result[msid] = entry
        else:
            # Store by methodIndex as fallback
            idx = entry.get("methodIndex", -1)
            if idx >= 0:
                result[f"_index_{idx}"] = entry
    return result


def _compute_alloc_ratio(aot_bytes: int, jit_bytes: int) -> float | None:
    """Compute AOT / JIT allocation ratio. >1.0 means AOT allocates more."""
    if jit_bytes <= 0:
        return None
    return round(aot_bytes / jit_bytes, 2)


def compare_profile_data(
    aot_profile_data: list[dict],
    managed_records: list[dict],
    metadata_methods: list[dict] | None = None,
    alloc_threshold: float = 2.0,
) -> dict[str, Any]:
    """Compare AOT profile vs managed gcInfo per-method.

    Args:
        aot_profile_data: profileData list from profile.json (chaos-aot).
        managed_records: Full benchmark-history.jsonl records (net8-jit, net10-jit).
        metadata_methods: subjects.metadata.json methods list for subjectId resolution.
        alloc_threshold: Alloc ratio threshold for flagging (default: 2.0x).

    Returns:
        Dict with per-method comparisons and aggregate summary.
    """
    managed_map = _build_managed_map(managed_records)
    profile_map = _build_profile_map(aot_profile_data)

    per_method: list[dict] = []
    high_alloc_methods: list[dict] = []
    total_high_alloc = 0
    total_compared = 0

    # Resolve by methodSubjectId from metadata
    if metadata_methods:
        for meta in metadata_methods:
            msid = meta.get("methodSubjectId", "")
            if not msid:
                continue
            profile = profile_map.get(msid)
            managed_gi = managed_map.get(msid)
            if not profile or not managed_gi:
                continue

            total_compared += 1
            aot_alloc = profile.get("nurseryAllocBytes", 0)
            jit_alloc = managed_gi.get("totalAllocatedBytes", 0)
            ratio = _compute_alloc_ratio(aot_alloc, jit_alloc)

            entry = {
                "methodSubjectId": msid,
                "aotGcPauseNs": profile.get("gcPauseNs", 0),
                "aotAllocBytes": aot_alloc,
                "aotFastPathRate": _fast_path_rate(profile),
                "jitAllocBytes": jit_alloc,
                "jitCollectionCount0": managed_gi.get("collectionCount0", 0),
                "aotVsJitAllocRatio": ratio,
                "aotVsJitGcPauseDelta": profile.get("gcPauseNs", 0) - (managed_gi.get("estimatedGcPauseNs", 0) or 0),
            }

            if ratio is not None and ratio > alloc_threshold:
                entry["_flag"] = "high_alloc"
                high_alloc_methods.append(entry)
                total_high_alloc += 1

            per_method.append(entry)

    return {
        "totalMethods": len(per_method),
        "totalCompared": total_compared,
        "highAllocCount": total_high_alloc,
        "allocThreshold": alloc_threshold,
        "perMethod": per_method,
        "highAllocMethods": sorted(
            high_alloc_methods,
            key=lambda m: m.get("aotVsJitAllocRatio", 0) or 0,
            reverse=True,
        )[:20],  # Top 20
    }


def _fast_path_rate(profile: dict) -> float:
    """Compute fast path rate from profile entry."""
    fast = profile.get("fastPathCount", 0)
    slow = profile.get("slowPathCount", 0)
    total = fast + slow
    return round(fast / total, 4) if total > 0 else 1.0


def print_report(result: dict[str, Any]) -> None:
    """Print a human-readable profile comparison report."""
    print(f"{'='*60}")
    print(f"  Profile Comparison: AOT vs JIT")
    print(f"{'='*60}")
    print(f"  Methods compared: {result['totalCompared']}/{result['totalMethods']}")
    print(f"  High alloc methods (>{result['allocThreshold']}x): {result['highAllocCount']}")
    print()

    top = result.get("highAllocMethods", [])
    if top:
        print(f"  Top-{len(top)} high allocation methods:")
        for m in top:
            msid_short = m["methodSubjectId"][:80]
            ratio = m.get("aotVsJitAllocRatio", "?")
            aot_b = m.get("aotAllocBytes", 0)
            jit_b = m.get("jitAllocBytes", 0)
            gc_ns = m.get("aotGcPauseNs", 0)
            print(f"    🔴 {msid_short}")
            print(f"       Alloc: AOT={aot_b}B vs JIT={jit_b}B  ({ratio}x)")
            print(f"       GC pause: {gc_ns}ns  Fast path: {m.get('aotFastPathRate', 1.0)*100:.1f}%")
    else:
        print(f"  No high-alloc methods found within threshold.")
    print(f"{'='*60}")


# ── CLI entry point ──
def main() -> None:
    import argparse
    parser = argparse.ArgumentParser(
        description="Compare AOT profile vs managed GC data")
    parser.add_argument("--aot-profile", required=True,
                        help="Path to AOT profile.json")
    parser.add_argument("--managed-jsonl", required=True,
                        help="Path to benchmark-history.jsonl")
    parser.add_argument("--metadata", default=None,
                        help="Path to subjects.metadata.json (for subjectId resolution)")
    parser.add_argument("--threshold", type=float, default=2.0,
                        help="Alloc ratio threshold (default: 2.0)")
    parser.add_argument("--output", default=None,
                        help="Write comparison JSON to file")
    args = parser.parse_args()

    aot = json.loads(Path(args.aot_profile).read_text(encoding="utf-8"))
    aot_data = aot.get("profileData", [])

    managed_records = []
    with open(args.managed_jsonl, encoding="utf-8") as f:
        for line in f:
            line = line.strip()
            if line:
                try:
                    managed_records.append(json.loads(line))
                except json.JSONDecodeError:
                    continue

    metadata = None
    if args.metadata:
        meta = json.loads(Path(args.metadata).read_text(encoding="utf-8"))
        metadata = meta.get("methods", [])

    result = compare_profile_data(aot_data, managed_records, metadata, args.threshold)

    if args.output:
        Path(args.output).write_text(
            json.dumps(result, indent=2, ensure_ascii=False), encoding="utf-8")
        print(f"Report written to {args.output}")

    print_report(result)


if __name__ == "__main__":
    main()
