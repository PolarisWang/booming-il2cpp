"""Perf baseline management — establish, store, query, and update profile baselines.

A baseline is a snapshot of profile metrics (GC pause, alloc volume, code size, etc.)
for a specific chunk at a specific point in time. Baselines are stored as JSON files
under results/foundation-dll/{assembly}/{slug}/baseline.json.

Usage:
    from verification.analysis.perf_baseline import PerfBaseline
    bl = PerfBaseline()
    bl.establish("numerics", profile_data)
    baseline = bl.get("numerics")
"""

from __future__ import annotations

import json
import time
from datetime import datetime, timezone
from pathlib import Path
from typing import Any

from verification._path import results_base

_RESULTS_BASE = results_base()


class PerfBaseline:
    """Manage profile baselines for foundation-dll chunks."""

    def __init__(self, results_base: Path | None = None):
        self._results_base = results_base or _RESULTS_BASE

    def _baseline_path(self, assembly: str, slug: str) -> Path:
        return self._results_base / assembly / slug / "perf" / "baseline.json"

    def establish(self, assembly: str, slug: str, profile_data: list[dict],
                  section_sizes: dict[str, int] | None = None,
                  metadata: dict[str, Any] | None = None) -> Path:
        """Create a new baseline from profile data.

        Args:
            assembly: Assembly name (e.g. "System.Private.CoreLib")
            slug: Chunk slug (e.g. "numerics")
            profile_data: Per-method profile data from profile.json
            section_sizes: PE section sizes (code size tracking)
            metadata: Optional metadata (git commit, build config, etc.)

        Returns:
            Path to the written baseline file.
        """
        if not profile_data:
            raise ValueError("Cannot establish baseline from empty profile data")

        # Compute baseline metrics
        gc_pause_values = [m.get("gcPauseNs", 0) for m in profile_data]
        max_gc_pause_values = [m.get("maxGcPauseNs", 0) for m in profile_data]
        nursery_bytes_values = [m.get("nurseryAllocBytes", 0) for m in profile_data]
        alloc_count_values = [m.get("allocCount", 0) for m in profile_data]
        fast_path_values = [m.get("fastPathCount", 0) for m in profile_data]
        slow_path_values = [m.get("slowPathCount", 0) for m in profile_data]

        total_fast = sum(fast_path_values)
        total_slow = sum(slow_path_values)
        total_path = total_fast + total_slow
        # 0.0 (not 1.0) when there is no path data — never fabricate a perfect
        # fast path from an empty sample.
        fast_path_rate = round(total_fast / total_path, 4) if total_path > 0 else 0.0

        baseline = {
            "assembly": assembly,
            "slug": slug,
            "establishedAt": datetime.now(timezone.utc).isoformat(),
            "methodCount": len(profile_data),
            "metrics": {
                "totalGcPauseNs": sum(gc_pause_values),
                "meanGcPauseNs": round(sum(gc_pause_values) / len(gc_pause_values), 2),
                "maxGcPauseNs": max(max_gc_pause_values),
                "totalNurseryAllocBytes": sum(nursery_bytes_values),
                "meanNurseryAllocBytes": round(sum(nursery_bytes_values) / len(nursery_bytes_values), 2),
                "totalAllocCount": sum(alloc_count_values),
                "meanAllocCount": round(sum(alloc_count_values) / len(alloc_count_values), 2),
                "fastPathRate": fast_path_rate,
            },
            "sectionSizes": section_sizes or {},
            "metadata": metadata or {},
        }

        path = self._baseline_path(assembly, slug)
        path.parent.mkdir(parents=True, exist_ok=True)
        path.write_text(json.dumps(baseline, indent=2), encoding="utf-8")
        return path

    def get(self, assembly: str, slug: str) -> dict[str, Any] | None:
        """Retrieve the baseline for a chunk, or None if not established."""
        path = self._baseline_path(assembly, slug)
        if not path.exists():
            return None
        try:
            return json.loads(path.read_text(encoding="utf-8"))
        except (json.JSONDecodeError, OSError):
            return None

    def list_baselines(self, assembly: str | None = None) -> list[dict[str, str]]:
        """List all established baselines.

        Args:
            assembly: If set, only list baselines for this assembly.

        Returns:
            List of {"assembly": ..., "slug": ..., "establishedAt": ...}
        """
        baselines: list[dict[str, str]] = []
        if assembly:
            search_root = self._results_base / assembly
        else:
            search_root = self._results_base

        if not search_root.exists():
            return baselines

        for baseline_file in search_root.rglob("perf/baseline.json"):
            try:
                data = json.loads(baseline_file.read_text(encoding="utf-8"))
                baselines.append({
                    "assembly": data.get("assembly", baseline_file.parent.parent.parent.name),
                    "slug": data.get("slug", baseline_file.parent.parent.name),
                    "establishedAt": data.get("establishedAt", ""),
                })
            except (json.JSONDecodeError, OSError):
                continue

        return baselines

    def delete(self, assembly: str, slug: str) -> bool:
        """Delete the baseline for a chunk. Returns True if deleted."""
        path = self._baseline_path(assembly, slug)
        if path.exists():
            path.unlink()
            return True
        return False

    def update(self, assembly: str, slug: str, profile_data: list[dict],
               section_sizes: dict[str, int] | None = None) -> Path | None:
        """Replace an existing baseline with new data. Returns None if no baseline exists."""
        existing = self.get(assembly, slug)
        metadata = existing.get("metadata", {}) if existing else {}
        metadata["previousEstablishedAt"] = existing.get("establishedAt", "") if existing else ""
        metadata["updatedAt"] = datetime.now(timezone.utc).isoformat()
        return self.establish(assembly, slug, profile_data, section_sizes, metadata)
