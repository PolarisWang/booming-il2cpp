"""GC stress verification stage — runs GC stress tests after codegen.

Verifies that the GC runtime produces correct results under stress conditions,
including allocation-heavy workloads, background GC, and finalization stress.

This stage is optional (default skipped) because GC stress binaries must be
built separately and take significant time to run.
"""

from __future__ import annotations

import subprocess
import time
from pathlib import Path
from typing import Any

from _core.python.models import FamilyContext, StageResult

# Known GC stress test binaries (searched under the native build directory).
# Stage searches for any of these that exist.
_GC_STRESS_BINARIES = [
    "chaos_gc_stress_test.exe",
    "chaos_gc_sanity_test.exe",
    "chaos_gc_bgc_stress_test.exe",
    "chaos_gc_general_stress_test.exe",
]

# Known build output directories to search
_BUILD_DIR_CANDIDATES = [
    "build/native/tests/contracts/native/runtime-core/RelWithDebInfo",
    "build/native/tests/contracts/native/runtime-core/Debug",
    "build/native/tests/contracts/native/runtime-core/Release",
    "build/native/testing/src/native/runtime-core/gc/RelWithDebInfo",
    "build/native/testing/src/native/runtime-core/gc/Debug",
    "build/native/testing/src/native/runtime-core/gc/Release",
]


def run_gc_stress(ctx: FamilyContext, stages: dict[str, StageResult]) -> StageResult:
    """Stage 6.5: Run GC stress tests for memory correctness verification.

    Searches for built GC stress test binaries. If none found, the stage
    is skipped gracefully. If found, each binary is run with a short
    timeout and the results are aggregated.
    """
    start = time.perf_counter()

    repo_root = ctx.family_dir.parents[2]  # testing/ -> repo root
    discovered = _discover_binaries(repo_root)

    if not discovered:
        return StageResult(
            stage="gc_stress", status="skipped",
            summary="No GC stress test binaries found (build GC tests first)",
            duration_ms=int((time.perf_counter() - start) * 1000),
        )

    results: list[dict[str, Any]] = []
    all_passed = True

    for binary_name, binary_path in discovered:
        print(f"  [gc_stress] Running {binary_name}...")
        try:
            r = subprocess.run(
                [str(binary_path)],
                capture_output=True, text=True, timeout=60,
            )
            stdout = (r.stdout or "") + (r.stderr or "")
            passed = r.returncode == 0

            # Parse result summary
            passed_count = 0
            total_count = 0
            import re
            for line in stdout.splitlines():
                m = re.search(r'Passed:\s*(\d+)/(\d+)', line)
                if m:
                    passed_count, total_count = int(m.group(1)), int(m.group(2))
                m = re.search(r'(\d+)\s+scenarios?.*?(\d+)\s+passed', line, re.IGNORECASE)
                if m:
                    passed_count, total_count = int(m.group(1)), int(m.group(2))

            status = "passed" if passed else "failed"
            if not passed:
                all_passed = False

            results.append({
                "binary": binary_name,
                "status": status,
                "returncode": r.returncode,
                "passed": passed_count,
                "total": total_count,
                "stdout_preview": stdout[:300],
            })
            print(f"    -> {status} (rc={r.returncode})")

        except subprocess.TimeoutExpired:
            all_passed = False
            results.append({
                "binary": binary_name,
                "status": "failed",
                "error": "TIMEOUT (60s)",
            })
            print(f"    -> TIMEOUT (60s)")
        except FileNotFoundError:
            results.append({
                "binary": binary_name,
                "status": "skipped",
                "error": "binary vanished before execution",
            })
        except Exception as e:
            all_passed = False
            results.append({
                "binary": binary_name,
                "status": "error",
                "error": str(e),
            })

    passed_count = sum(1 for r in results if r.get("status") == "passed")
    total_count = len(results)
    status = "passed" if all_passed else "failed"

    return StageResult(
        stage="gc_stress", status=status,
        summary=f"{passed_count}/{total_count} GC stress tests passed",
        details={"results": results},
        duration_ms=int((time.perf_counter() - start) * 1000),
    )


def _discover_binaries(repo_root: Path) -> list[tuple[str, Path]]:
    """Discover GC test binaries under the build directory."""
    found: list[tuple[str, Path]] = []
    for binary_name in _GC_STRESS_BINARIES:
        for candidate in _BUILD_DIR_CANDIDATES:
            path = repo_root / candidate / binary_name
            if path.exists():
                found.append((binary_name, path))
                break
    return found
