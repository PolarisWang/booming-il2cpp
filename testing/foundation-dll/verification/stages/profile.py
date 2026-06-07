"""Profile stage — runs entry.exe --profile for per-method GC/allocation/code-size profiling.

Flow:
  1. Verify entry.exe exists and was built with --preset profile
  2. Run entry.exe --profile
  3. Parse JSON profile output (GC pause, alloc volume, heap delta, fast path rate)
  4. Write results/profile.json
  5. Write results/profile-history.jsonl (dashboard-compatible format)
"""

from __future__ import annotations

import json
import os
import re
import subprocess
import time
from datetime import datetime, timezone
from pathlib import Path
from typing import Any

from verification.orchestration.context import ChunkContext, StageResult
from verification.analysis.code_size_tracker import CodeSizeTracker

_RESULTS_BASE = Path(__file__).resolve().parent.parent / "results" / "foundation-dll"


def _parse_profile_output(stdout: str) -> list[dict]:
    """Parse JSON profile output from entry.exe --profile.

    Expected format: {"profile": [{methodIndex, gcPauseNs, ...}, ...]}
    """
    try:
        json_start = stdout.find("{")
        json_end = stdout.rfind("}") + 1
        if json_start >= 0 and json_end > json_start:
            payload = stdout[json_start:json_end]
            parsed = json.loads(payload)
            return parsed.get("profile", [])
    except (json.JSONDecodeError, KeyError):
        pass
    return []


def _compute_summary(profile_data: list[dict]) -> dict[str, Any]:
    """Compute aggregate profile summary across all methods."""
    if not profile_data:
        return {}

    total_gc_pause_ns = sum(m.get("gcPauseNs", 0) for m in profile_data)
    max_gc_pause_ns = max(m.get("maxGcPauseNs", 0) for m in profile_data)
    total_nursery_bytes = sum(m.get("nurseryAllocBytes", 0) for m in profile_data)
    total_alloc_count = sum(m.get("allocCount", 0) for m in profile_data)
    total_fast_path = sum(m.get("fastPathCount", 0) for m in profile_data)
    total_slow_path = sum(m.get("slowPathCount", 0) for m in profile_data)
    total_path = total_fast_path + total_slow_path
    fast_path_rate = round(total_fast_path / total_path, 4) if total_path > 0 else 1.0

    # GC pause distribution
    methods_with_gc = sum(1 for m in profile_data if m.get("gcPauseCount", 0) > 0)
    methods_with_alloc = sum(1 for m in profile_data if m.get("allocCount", 0) > 0)

    # Code size (extracted from dumpbin output, not per-method here)
    code_text_bytes = 0
    code_data_bytes = 0

    return {
        "methodCount": len(profile_data),
        "totalGcPauseNs": total_gc_pause_ns,
        "maxGcPauseNs": max_gc_pause_ns,
        "totalNurseryAllocBytes": total_nursery_bytes,
        "totalAllocCount": total_alloc_count,
        "fastPathRate": fast_path_rate,
        "methodsWithGc": methods_with_gc,
        "methodsWithAlloc": methods_with_alloc,
        "codeTextBytes": code_text_bytes,
        "codeDataBytes": code_data_bytes,
    }


def _get_entry_count(exe_path: Path) -> int:
    """Read kSubjectEntryCount from generated native-aot.generated.cpp."""
    subject_file = exe_path.parent / "subjects" / "native-aot.generated.cpp"
    if not subject_file.exists():
        return 0
    content = subject_file.read_text(encoding="utf-8", errors="replace")
    m = re.search(r"kSubjectEntryCount\s*=\s*(\d+)", content)
    return int(m.group(1)) if m else 0


def _collect_section_sizes(entry_exe_path: Path) -> dict[str, int]:
    """Read PE section sizes via CodeSizeTracker (.map file + dumpbin fallback)."""
    tracker = CodeSizeTracker()
    result = tracker.collect(entry_exe_path)
    return result.get("sectionSizes", {"total": 0})


def _write_records_jsonl(
    profile_data: list[dict],
    summary: dict,
    ctx: ChunkContext,
    section_sizes: dict[str, int],
):
    """Append unified-format records to profile-history.jsonl."""
    records_path = ctx.results_dir / "profile-history.jsonl"
    now = datetime.now(timezone.utc).strftime("%Y-%m-%dT%H:%M:%SZ")

    record = {
        "runId": f"{ctx.assembly}-{ctx.slug}-profile-{int(time.time())}",
        "subject": f"{ctx.assembly}/{ctx.slug}",
        "mode": "native",
        "technology": "chaos-aot",
        "nativeConfig": ctx.native_config,
        "recordedAt": now,
        "summary": summary,
        "sectionSizes": section_sizes,
    }

    with open(records_path, "a", encoding="utf-8") as f:
        f.write(json.dumps(record, ensure_ascii=False, separators=(",", ":")) + "\n")
        for i, m in enumerate(profile_data):
            method_record = dict(record)
            method_record["runId"] = f"{record['runId']}-method-{i}"
            method_record["method"] = m
            f.write(json.dumps(method_record, ensure_ascii=False, separators=(",", ":")) + "\n")


def run_profile(ctx: ChunkContext, stages: dict[str, StageResult]) -> StageResult:
    """Profile stage: run entry.exe --profile and collect GC/allocation/code-size data."""
    start = time.perf_counter()

    exe_path = ctx.entry_exe_path
    if not exe_path.exists():
        return StageResult(
            stage="profile", status="skipped",
            summary=f"entry.exe not found: {exe_path}",
            duration_ms=int((time.perf_counter() - start) * 1000),
        )

    # Only run in profile build config
    if ctx.native_config != "profile":
        print(f"  [profile] WARNING: native_config is '{ctx.native_config}', not 'profile'. "
              f"Profile data may be incomplete. Use --native-config profile for full data.")

    entry_count = _get_entry_count(exe_path)
    if entry_count == 0:
        return StageResult(
            stage="profile", status="skipped",
            summary=f"No subject entries found in {exe_path}",
            duration_ms=int((time.perf_counter() - start) * 1000),
        )

    print(f"  [profile] Running {exe_path} --profile ({entry_count} entries)...")
    env = os.environ.copy()
    env["CHAOS_IL2CPP_LOG_LEVEL"] = "0"  # suppress debug logs

    try:
        r = subprocess.run(
            [str(exe_path), "--profile"],
            capture_output=True, timeout=600, env=env,
        )
    except subprocess.TimeoutExpired:
        return StageResult(
            stage="profile", status="error",
            summary="profile timed out after 600s",
            duration_ms=int((time.perf_counter() - start) * 1000),
        )

    stdout = r.stdout.decode("utf-8", errors="replace") if r.stdout else ""

    # Parse profile output
    profile_data = _parse_profile_output(stdout)
    if not profile_data:
        return StageResult(
            stage="profile", status="error",
            summary="No profile data parsed from entry.exe output",
            duration_ms=int((time.perf_counter() - start) * 1000),
        )

    # Compute summary
    summary = _compute_summary(profile_data)

    # Collect section sizes (code size tracking)
    section_sizes = _collect_section_sizes(exe_path)
    summary["codeTextBytes"] = section_sizes.get("text", 0)
    summary["codeDataBytes"] = section_sizes.get("data", 0)

    # Build result data
    result_data: dict[str, Any] = {
        "exitCode": r.returncode,
        "nativeConfig": ctx.native_config,
        "entryCount": entry_count,
        "profileData": profile_data,
        "summary": summary,
        "sectionSizes": section_sizes,
    }

    # Write results
    ctx.results_dir.mkdir(parents=True, exist_ok=True)
    result_path = ctx.results_dir / "profile.json"
    result_path.write_text(json.dumps(result_data, indent=2), encoding="utf-8")

    # Write JSONL records
    _write_records_jsonl(profile_data, summary, ctx, section_sizes)

    # Print summary
    duration_ms = int((time.perf_counter() - start) * 1000)
    print(f"  [profile] {len(profile_data)} methods profiled")
    print(f"  [profile]   GC pause:  total={summary.get('totalGcPauseNs', 0)}ns, "
          f"max={summary.get('maxGcPauseNs', 0)}ns, "
          f"methods_with_gc={summary.get('methodsWithGc', 0)}")
    print(f"  [profile]   Alloc:     nursery={summary.get('totalNurseryAllocBytes', 0)}B, "
          f"count={summary.get('totalAllocCount', 0)}")
    print(f"  [profile]   Fast path: {summary.get('fastPathRate', 1.0)*100:.1f}% "
          f"(fast={profile_data[0].get('fastPathCount', 0) if profile_data else 0}, "
          f"slow={profile_data[0].get('slowPathCount', 0) if profile_data else 0})")
    print(f"  [profile]   Code size: text={section_sizes.get('text', 0)}B, "
          f"data={section_sizes.get('data', 0)}B")
    print(f"  [profile] Done ({duration_ms}ms)")

    return StageResult(
        stage="profile", status="passed",
        summary=f"{len(profile_data)} methods profiled, "
                f"GC pause max={summary.get('maxGcPauseNs', 0)}ns, "
                f"fast path rate={summary.get('fastPathRate', 1.0)*100:.1f}%",
        details=result_data,
        duration_ms=duration_ms,
    )
