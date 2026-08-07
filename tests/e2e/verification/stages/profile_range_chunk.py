"""Profile-range stage — runs --profile-range across all safe AOT method ranges.

Collects per-method GC allocation data by dispatching real AOT methods
directly (not subject wrappers), with SEH crash protection to skip unsafe
methods.

Usage in pipeline:
    python -m verification.chunk_pipeline \\
        --assembly System.Collections \\
        --chunk global-ns \\
        --stages build,fact,profile-range
"""

from __future__ import annotations

import json
import subprocess
import time
from pathlib import Path
from typing import Any

from verification.orchestration.context import ChunkContext, StageResult

# Safe batch size for probing (smaller = lower crash risk)
_PROBE_BATCH = 10
# Method index ranges known to hang the process
_KNOWN_UNSAFE_RANGES: list[tuple[int, int]] = [
    (30, 35),
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
    """Read total AOT method count from manifest or generated cpp."""
    manifest_path = chunk_dir / "native" / "codegen" / "generated" / "native-aot.methods.json"
    if manifest_path.exists():
        try:
            manifest = json.loads(manifest_path.read_text(encoding="utf-8-sig"))
            return manifest.get("methodCount", 0)
        except (json.JSONDecodeError, OSError):
            pass
    cpp_path = chunk_dir / "native" / "codegen" / "generated" / "native-aot.generated.cpp"
    if cpp_path.exists():
        import re
        m = re.search(r"kAotMethodCount\s*=\s*(\d+)", cpp_path.read_text(encoding="utf-8", errors="replace"))
        if m:
            return int(m.group(1))
    return 0


def _is_unsafe(idx: int) -> bool:
    return any(s <= idx < e for s, e in _KNOWN_UNSAFE_RANGES)


def _probe(entry_exe: Path, start: int, end: int, timeout: int = 15) -> bool:
    if _is_unsafe(start):
        return False
    try:
        r = subprocess.run(
            [str(entry_exe), "--profile-range", str(start), str(end)],
            capture_output=True, text=True, timeout=timeout,
        )
        return bool(r.stdout.strip().startswith("{"))
    except (subprocess.TimeoutExpired, OSError):
        return False


def _scan_safe_ranges(entry_exe: Path, total: int) -> list[tuple[int, int]]:
    safe: list[tuple[int, int]] = []
    i = 0
    while i < total:
        if _is_unsafe(i):
            i += 1
            continue
        batch_end = min(i + _PROBE_BATCH, total)
        if _probe(entry_exe, i, batch_end):
            safe.append((i, batch_end))
            i = batch_end
        else:
            for j in range(i, batch_end):
                if not _probe(entry_exe, j, j + 1):
                    if j > i:
                        safe.append((i, j))
                    i = j + 1
                    break
            else:
                i = batch_end
    return safe


def run_profile_range(
    entry_exe: Path,
    native_dir: Path,
    native_config: str = "profile",
    verbose: bool = False,
) -> tuple[list[dict[str, Any]], str]:
    """Run --profile-range across all safe AOT ranges.

    Returns (profiles_list, status_message).
    """
    total = _get_method_count(native_dir)
    if total == 0:
        return [], "no_method_count"

    if verbose:
        print(f"  [profile-range] Scanning {total} AOT methods...")
    safe_ranges = _scan_safe_ranges(entry_exe, total)
    if not safe_ranges:
        return [], "no_safe_ranges"

    if verbose:
        safe_count = sum(e - s for s, e in safe_ranges)
        print(f"  [profile-range] {len(safe_ranges)} safe range(s), {safe_count}/{total} methods")

    method_map = _read_manifest(native_dir)
    all_profiles: list[dict[str, Any]] = []

    for start, end in safe_ranges:
        if verbose:
            print(f"  [profile-range] Dispatching [{start}, {end})...")
        try:
            r = subprocess.run(
                [str(entry_exe), "--profile-range", str(start), str(end)],
                capture_output=True, text=True, timeout=120,
            )
            raw = r.stdout.strip()
            js = raw.find("{")
            je = raw.rfind("}") + 1
            if js >= 0 and je > js:
                data = json.loads(raw[js:je])
                for p in data.get("profile", []):
                    midx = p.get("methodIndex", -1)
                    if midx >= 0 and method_map:
                        p["methodSubjectId"] = method_map.get(midx, f"method-{midx}")
                    all_profiles.append(p)
        except (subprocess.TimeoutExpired, OSError, json.JSONDecodeError) as e:
            if verbose:
                print(f"  [profile-range]   FAIL [{start}, {end}): {e}")

    return all_profiles, "ok"


def run_profile_range_chunk(ctx: ChunkContext, stages: dict[str, StageResult]) -> StageResult:
    """Profile-range stage: run --profile-range and write results."""
    start = time.perf_counter()

    exe_path = ctx.entry_exe_path
    if not exe_path.exists():
        return StageResult(
            stage="profile-range", status="skipped",
            summary=f"entry.exe not found: {exe_path}",
            duration_ms=int((time.perf_counter() - start) * 1000),
        )

    profiles, status = run_profile_range(
        exe_path, ctx.chunk_dir, ctx.native_config, verbose=ctx.verbose,
    )

    if status == "no_method_count":
        return StageResult(
            stage="profile-range", status="error",
            summary="Cannot determine AOT method count",
            duration_ms=int((time.perf_counter() - start) * 1000),
        )
    if status == "no_safe_ranges":
        return StageResult(
            stage="profile-range", status="error",
            summary="No safe method ranges found",
            duration_ms=int((time.perf_counter() - start) * 1000),
        )

    # Compute summary
    total_bytes = sum(p.get("nurseryAllocBytes", 0) for p in profiles)
    alloc_methods = sum(1 for p in profiles if p.get("nurseryAllocBytes", 0) > 0)
    total_gc_pause = sum(p.get("gcPauseNs", 0) for p in profiles)

    summary_data = {
        "nativeConfig": ctx.native_config,
        "methodCount": len(profiles),
        "methodsWithAlloc": alloc_methods,
        "totalNurseryAllocBytes": total_bytes,
        "totalGcPauseNs": total_gc_pause,
    }

    # Write results
    result_data = {
        **summary_data,
        "profileData": profiles,
    }
    ctx.results_dir.mkdir(parents=True, exist_ok=True)
    (ctx.results_dir / "profile-range.json").write_text(
        json.dumps(result_data, indent=2), encoding="utf-8")

    duration_ms = int((time.perf_counter() - start) * 1000)

    summary = (f"{len(profiles)} methods profiled, "
               f"{alloc_methods} with allocation, "
               f"{total_bytes}B total nursery alloc")
    if ctx.verbose:
        print(f"  [profile-range] {summary}")

    return StageResult(
        stage="profile-range", status="passed",
        summary=summary,
        details=summary_data,
        duration_ms=duration_ms,
    )
