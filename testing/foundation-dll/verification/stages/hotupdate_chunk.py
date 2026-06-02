"""Simplified hotupdate stage for chunk-based pipeline.

Runs entry.exe --hotupdate from the chunk's native directory and
parses per-method baseline/patched/revert JSON results.
"""

from __future__ import annotations

import json
import subprocess
import time

from verification.orchestration.context import ChunkContext, StageResult


def run_hotupdate_chunk(ctx: ChunkContext, stages: dict[str, StageResult]) -> StageResult:
    """HotUpdate stage: run chunk's entry.exe --hotupdate."""
    start = time.perf_counter()

    exe_path = ctx.entry_exe_path
    if not exe_path.exists():
        return StageResult(
            stage="hotupdate", status="skipped",
            summary=f"entry.exe not found, skipping hotupdate",
            duration_ms=int((time.perf_counter() - start) * 1000),
        )

    print(f"  [hotupdate] Running {exe_path} --hotupdate...")
    try:
        r = subprocess.run(
            [str(exe_path), "--hotupdate"],
            capture_output=True, text=True, timeout=120,
        )
    except subprocess.TimeoutExpired:
        return StageResult(
            stage="hotupdate", status="error",
            summary="hotupdate timed out after 120s",
            duration_ms=int((time.perf_counter() - start) * 1000),
        )

    stdout = r.stdout or ""
    stderr = r.stderr or ""

    # Parse JSON output
    hotupdate_data = {}
    try:
        json_start = stdout.find("{")
        json_end = stdout.rfind("}") + 1
        if json_start >= 0 and json_end > json_start:
            payload = stdout[json_start:json_end]
            hotupdate_data = json.loads(payload)
    except (json.JSONDecodeError, KeyError):
        pass

    passed = hotupdate_data.get("passedMethods", 0)
    failed = hotupdate_data.get("failedMethods", 0)
    all_semantic = hotupdate_data.get("allSemantic", False)
    all_revert = hotupdate_data.get("allRevert", False)

    ctx.results_dir.mkdir(parents=True, exist_ok=True)
    result_path = ctx.results_dir / "hotupdate.json"
    result_data = {
        "exitCode": r.returncode,
        "passed": passed,
        "failed": failed,
        "allSemantic": all_semantic,
        "allRevert": all_revert,
        "details": hotupdate_data,
        "stderr": stderr[:500] if stderr else "",
    }
    result_path.write_text(json.dumps(result_data, indent=2), encoding="utf-8")

    status = "passed" if failed == 0 and passed > 0 else "failed"
    if passed == 0 and r.returncode == 0:
        status = "skipped"

    duration_ms = int((time.perf_counter() - start) * 1000)
    print(f"  [hotupdate] {status}: {passed} passed, semantic={all_semantic}, revert={all_revert} ({duration_ms}ms)")

    return StageResult(
        stage="hotupdate", status=status,
        summary=f"{status}: {passed} passed, semantic_changed={all_semantic}",
        details=result_data,
        duration_ms=duration_ms,
    )
