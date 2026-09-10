"""Profile range runner — runs --profile-range across all safe AOT method ranges.

Detects safe ranges by probing small batches, then dispatches all safe methods
with the full ProfileEmitJson infrastructure to capture GC allocation data.

Usage:
    python profile_range_runner.py --assembly System.Collections.NonGeneric --chunk global-ns
"""

from __future__ import annotations

import json
import subprocess
import sys
import time
from pathlib import Path
from typing import Any

# Safe batch size for probing (smaller = less crash risk)
_PROBE_BATCH = 10
# Ranges known to hang the process (index ranges that are never safe)
_KNOWN_UNSAFE_RANGES: list[tuple[int, int]] = [
    (30, 35),  # causes process hang
]


def _read_manifest(chunk_dir: Path) -> dict[int, str] | None:
    """Read native-aot.methods.json and build {methodIndex -> subjectId} map."""
    manifest_path = chunk_dir / "native" / "codegen" / "generated" / "native-aot.methods.json"
    if not manifest_path.exists():
        return None
    try:
        manifest = json.loads(manifest_path.read_text(encoding="utf-8-sig"))
        methods = manifest.get("methods", [])
        return {m["index"]: m.get("subjectId", f"method-{m['index']}")
                for m in methods if "index" in m}
    except (json.JSONDecodeError, KeyError, IndexError, OSError):
        return None


def _get_method_count(chunk_dir: Path) -> int:
    """Read total AOT method count from manifest or native-aot.generated.cpp."""
    manifest_path = chunk_dir / "native" / "codegen" / "generated" / "native-aot.methods.json"
    if manifest_path.exists():
        try:
            manifest = json.loads(manifest_path.read_text(encoding="utf-8-sig"))
            return manifest.get("methodCount", 0)
        except (json.JSONDecodeError, OSError):
            pass
    # Fallback: grep the generated cpp for kAotMethodCount
    cpp_path = chunk_dir / "native" / "codegen" / "generated" / "native-aot.generated.cpp"
    if cpp_path.exists():
        import re
        m = re.search(r"kAotMethodCount\s*=\s*(\d+)", cpp_path.read_text(encoding="utf-8", errors="replace"))
        if m:
            return int(m.group(1))
    return 0


def _is_in_unsafe_range(idx: int) -> bool:
    for start, end in _KNOWN_UNSAFE_RANGES:
        if start <= idx < end:
            return True
    return False


def _probe_range(entry_exe: Path, start: int, end: int, timeout: int = 15) -> bool:
    """Probe if a range [start, end) is safe to dispatch."""
    if _is_in_unsafe_range(start):
        return False
    try:
        r = subprocess.run(
            [str(entry_exe), "--profile-range", str(start), str(end)],
            capture_output=True, text=True, timeout=timeout,
        )
        return bool(r.stdout.strip().startswith("{"))
    except (subprocess.TimeoutExpired, OSError):
        return False


def _scan_safe_ranges(entry_exe: Path, total_count: int) -> list[tuple[int, int]]:
    """Scan all AOT methods and return list of safe (start, end) ranges."""
    safe_ranges: list[tuple[int, int]] = []
    i = 0
    while i < total_count:
        # Skip known unsafe
        if _is_in_unsafe_range(i):
            i += 1
            continue
        # Probe a batch
        batch_end = min(i + _PROBE_BATCH, total_count)
        if _probe_range(entry_exe, i, batch_end):
            safe_ranges.append((i, batch_end))
            i = batch_end
        else:
            # Narrow down to find the exact bad method
            found_bad = False
            for j in range(i, batch_end):
                if not _probe_range(entry_exe, j, j + 1):
                    print(f"  [profile-range] SKIP bad index {j}")
                    found_bad = True
                    if j > i:
                        safe_ranges.append((i, j))
                    i = j + 1
                    break
            if not found_bad:
                i = batch_end
    return safe_ranges


def run_profile_range(
    entry_exe: Path,
    chunk_dir: Path,
    native_config: str = "profile",
) -> list[dict[str, Any]]:
    """Run --profile-range across all safe AOT ranges and return profile data."""
    total_count = _get_method_count(chunk_dir)
    if total_count == 0:
        print(f"  [profile-range] Cannot determine method count")
        return []

    # Scan safe ranges
    print(f"  [profile-range] Scanning {total_count} AOT methods for safe ranges...")
    safe_ranges = _scan_safe_ranges(entry_exe, total_count)
    print(f"  [profile-range] Found {len(safe_ranges)} safe range(s)")

    # Load method index -> subjectId map
    method_map = _read_manifest(chunk_dir)

    # Run each safe range
    all_profiles: list[dict[str, Any]] = []
    for start, end in safe_ranges:
        print(f"  [profile-range] Profiling [{start}, {end})...")
        try:
            r = subprocess.run(
                [str(entry_exe), "--profile-range", str(start), str(end)],
                capture_output=True, text=True, timeout=120,
            )
            raw = r.stdout.strip()
            json_start = raw.find("{")
            json_end = raw.rfind("}") + 1
            if json_start >= 0 and json_end > json_start:
                data = json.loads(raw[json_start:json_end])
                for p in data.get("profile", []):
                    midx = p.get("methodIndex", -1)
                    if midx >= 0 and method_map:
                        p["methodSubjectId"] = method_map.get(midx, f"method-{midx}")
                    all_profiles.append(p)
        except (subprocess.TimeoutExpired, OSError, json.JSONDecodeError) as e:
            print(f"  [profile-range]   FAIL [{start}, {end}): {e}")

    return all_profiles


def main():
    import argparse
    parser = argparse.ArgumentParser(description="Profile range runner")
    parser.add_argument("--assembly", default="System.Collections.NonGeneric")
    parser.add_argument("--chunk", default="global-ns")
    parser.add_argument("--native-config", default="profile")
    args = parser.parse_args()

    foundation_dir = Path(args.assembly)
    chunk_dir = foundation_dir / "chunks" / args.chunk
    entry_exe = chunk_dir / "native" / "entry.exe"

    if not entry_exe.exists():
        print(f"ERROR: entry.exe not found: {entry_exe}")
        return 1

    profiles = run_profile_range(entry_exe, chunk_dir, args.native_config)

    if not profiles:
        print("No profile data collected")
        return 1

    # Write enriched profile data
    output = {
        "nativeConfig": args.native_config,
        "methodCount": len(profiles),
        "profileData": profiles,
        "summary": {
            "methodCount": len(profiles),
            "totalGcPauseNs": sum(p.get("gcPauseNs", 0) for p in profiles),
            "totalNurseryAllocBytes": sum(p.get("nurseryAllocBytes", 0) for p in profiles),
            "totalAllocCount": sum(p.get("allocCount", 0) for p in profiles),
            "methodsWithAlloc": sum(1 for p in profiles if p.get("nurseryAllocBytes", 0) > 0),
        },
    }

    result_path = chunk_dir / "results" / "profile-range.json"
    result_path.parent.mkdir(parents=True, exist_ok=True)
    result_path.write_text(json.dumps(output, indent=2), encoding="utf-8")

    alloc_methods = [p for p in profiles if p.get("nurseryAllocBytes", 0) > 0]
    print(f"\n  [profile-range] {len(profiles)} methods profiled")
    print(f"  [profile-range] {len(alloc_methods)} methods with allocation")
    total_bytes = sum(p.get("nurseryAllocBytes", 0) for p in alloc_methods)
    print(f"  [profile-range] Total nursery alloc: {total_bytes} bytes")
    for p in sorted(alloc_methods, key=lambda x: -x["nurseryAllocBytes"])[:10]:
        sid = p.get("methodSubjectId", f"method-{p['methodIndex']}")
        short = sid.split("/")[-1] if "/" in sid else sid
        print(f"    [{p['methodIndex']:3d}] {short[:70]}")
        print(f"          nursery={p['nurseryAllocBytes']}B fastPath={p['fastPathCount']} allocCount={p['allocCount']}")

    return 0


if __name__ == "__main__":
    sys.exit(main())
