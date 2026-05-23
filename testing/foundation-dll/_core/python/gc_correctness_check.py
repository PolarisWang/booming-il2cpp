"""GC correctness principle checks — slot map integrity + write barrier verification.

Checks integrated into the audit stage to verify that codegen output
includes correct GC metadata and write barrier usage.

Checks:
  - p2_gc_slot_map: GC slot map entries exist with proper slot counts
  - p2_write_barrier: Managed reference stores use CHAOS_IL2CPP_WRITE_BARRIER
"""

from __future__ import annotations

import re
from pathlib import Path
from typing import Any


def check_gc_correctness(
    assembly: str,
    family_slug: str,
    cpp_path: Path | None,
) -> dict[str, Any]:
    """Run GC correctness checks on generated C++.

    Args:
        assembly: Assembly name.
        family_slug: Family slug.
        cpp_path: Path to generated native-aot.generated.cpp, or None.

    Returns:
        Dict with keys: checks, overall_status.
    """
    checks: list[dict[str, Any]] = []

    if not cpp_path or not cpp_path.exists():
        checks.append({
            "check_id": "p2_gc_slot_map",
            "status": "NOT_APPLICABLE",
            "summary": "Generated C++ not found",
        })
        checks.append({
            "check_id": "p2_write_barrier",
            "status": "NOT_APPLICABLE",
            "summary": "Generated C++ not found",
        })
        return {"checks": checks, "overall_status": "NOT_APPLICABLE"}

    content: bytes = cpp_path.read_bytes()

    _check_gc_slot_map(content, checks)
    _check_write_barrier(content, checks)

    # Overall status = worst of all checks
    status_order = {"VIOLATION": 0, "CONCERN": 1, "ALIGNED": 2, "NOT_APPLICABLE": 3}
    overall = "ALIGNED"
    for c in checks:
        if status_order.get(c["status"], 99) < status_order.get(overall, 99):
            overall = c["status"]

    return {"checks": checks, "overall_status": overall}


def _check_gc_slot_map(content: bytes, checks: list[dict[str, Any]]) -> None:
    """Verify GC slot map entries exist with valid slot counts.

    Parses the kChaosGcSlotMapsSection structure to count entries
    and verify slot counts.
    """
    # Find the GC slot maps section
    section_start = content.find(b"kChaosGcSlotMapsSection")
    if section_start == -1:
        checks.append({
            "check_id": "p2_gc_slot_map",
            "status": "CONCERN",
            "summary": "No kChaosGcSlotMapsSection found — GC slot maps not emitted",
        })
        return

    # Count slot map entries (each entry has num_gc_slots)
    num_gc_pattern = re.compile(rb"num_gc_slots\s*=\s*(\d+)")
    slot_counts = [int(m.group(1)) for m in num_gc_pattern.finditer(content)]

    if not slot_counts:
        checks.append({
            "check_id": "p2_gc_slot_map",
            "status": "VIOLATION",
            "summary": "GC slot maps section exists but no slot entries found",
        })
        return

    total_entries = len(slot_counts)
    entries_with_slots = sum(1 for s in slot_counts if s > 0)
    zero_slot_entries = total_entries - entries_with_slots

    if entries_with_slots == 0:
        checks.append({
            "check_id": "p2_gc_slot_map",
            "status": "CONCERN",
            "summary": f"All {total_entries} GC slot map entries have zero slots — no GC tracking",
            "details": {
                "total_entries": total_entries,
                "entries_with_slots": 0,
                "zero_slot_entries": zero_slot_entries,
            },
        })
    elif zero_slot_entries > total_entries * 0.5:
        checks.append({
            "check_id": "p2_gc_slot_map",
            "status": "CONCERN",
            "summary": f"{zero_slot_entries}/{total_entries} entries have zero GC slots (>50%)",
            "details": {
                "total_entries": total_entries,
                "entries_with_slots": entries_with_slots,
                "zero_slot_entries": zero_slot_entries,
            },
        })
    else:
        checks.append({
            "check_id": "p2_gc_slot_map",
            "status": "ALIGNED",
            "summary": f"{entries_with_slots}/{total_entries} entries have GC slot maps (avg {sum(slot_counts)/total_entries:.1f} slots/entry)",
            "details": {
                "total_entries": total_entries,
                "entries_with_slots": entries_with_slots,
                "zero_slot_entries": zero_slot_entries,
                "avg_slots_per_entry": round(sum(slot_counts) / total_entries, 1),
            },
        })


def _check_write_barrier(content: bytes, checks: list[dict[str, Any]]) -> None:
    """Verify that managed reference stores use CHAOS_IL2CPP_WRITE_BARRIER.

    Heuristic: search for CHAOS_IL2CPP_WRITE_BARRIER usage patterns.
    Also look for bare pointer stores that might be missing write barriers.
    """
    # Count WRITE_BARRIER usage
    barrier_uses = len(re.findall(rb"CHAOS_IL2CPP_WRITE_BARRIER", content))

    # Count potential unsafe stores: .field = value pattern on pointer/reference
    # This is a heuristic — not all matches are managed ref stores.
    unsafe_store_pattern = re.compile(rb"\.\w+\s*=\s*_s\d+\.\w+")
    potential_stores = len(unsafe_store_pattern.findall(content))

    if barrier_uses > 0:
        checks.append({
            "check_id": "p2_write_barrier",
            "status": "ALIGNED",
            "summary": f"Found {barrier_uses} CHAOS_IL2CPP_WRITE_BARRIER usages",
            "details": {
                "barrier_uses": barrier_uses,
                "potential_stores": potential_stores,
            },
        })
    elif potential_stores == 0:
        checks.append({
            "check_id": "p2_write_barrier",
            "status": "NOT_APPLICABLE",
            "summary": "No managed reference field stores detected — write barriers not applicable",
            "details": {
                "barrier_uses": 0,
                "potential_stores": 0,
            },
        })
    else:
        checks.append({
            "check_id": "p2_write_barrier",
            "status": "CONCERN",
            "summary": f"No write barriers found but {potential_stores} potential field stores detected",
            "details": {
                "barrier_uses": 0,
                "potential_stores": potential_stores,
            },
        })
