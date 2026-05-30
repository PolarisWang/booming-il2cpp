"""Benchmark stage runner — native AOT + JIT performance.

Runs entry.exe --benchmark and entry-jit.exe --benchmark for all native methods.
"""

from __future__ import annotations

import json
import re
import shutil
import subprocess
import time
from pathlib import Path
from typing import Any

from verification.orchestration.context import FamilyContext, StageResult


# ── PROFILE data parsing ──────────────────────────────────────────────────

_PROFILE_LINE_RE = re.compile(
    r"^PROFILE\|(?P<name>[^|]+)\|"
    r"avg=(?P<avg_cycles>[0-9.]+)\|"
    r"avg_ns=(?P<avg_ns>[0-9.]+)\|"
    r"min=(?P<min>[0-9]+)\|"
    r"max=(?P<max>[0-9]+)\|"
    r"count=(?P<count>[0-9]+)\|"
    r"total_ns=(?P<total_ns>[0-9.]+)"
)
_CALIBRATION_RE = re.compile(
    r"^PROFILE\|CALIBRATION\|ns_per_cycle=(?P<ns_per_cycle>[0-9.]+)"
)


def _parse_profile_data(stderr_text: str) -> dict[str, Any]:
    """Parse PROFILE_DUMP() output from stderr into structured data."""
    scopes: dict[str, dict[str, Any]] = {}
    ns_per_cycle: float | None = None
    for line in stderr_text.splitlines():
        line = line.strip()
        m = _CALIBRATION_RE.match(line)
        if m:
            ns_per_cycle = float(m.group("ns_per_cycle"))
            continue
        m = _PROFILE_LINE_RE.match(line)
        if m:
            name = m.group("name")
            scopes[name] = {
                "avg_cycles": float(m.group("avg_cycles")),
                "avg_ns": float(m.group("avg_ns")),
                "min_cycles": int(m.group("min")),
                "max_cycles": int(m.group("max")),
                "count": int(m.group("count")),
                "total_ns": float(m.group("total_ns")),
            }
    return {
        "ns_per_cycle": ns_per_cycle,
        "scopes": scopes,
        "has_profile_data": len(scopes) > 0,
    }


# ── Native benchmark runners ────────────────────────────────────────────

def _load_method_count(ctx: FamilyContext) -> int:
    """Load method count from contract.json."""
    contract_path = ctx.contract_path
    if not contract_path.exists():
        return 0
    try:
        import json as _json
        c = _json.loads(contract_path.read_text(encoding="utf-8"))
        mids = c.get("methodSubjectIds", [])
        if mids:
            return len(mids)
        return c.get("methodSubjectCount", 0)
    except Exception:
        return 0


def _run_single_benchmark(
    exe_path: Path, method_index: int, iterations: int = 100000,
    collect_profile: bool = False,
) -> dict[str, Any] | None:
    """Run entry.exe --benchmark N and parse JSON timing output.

    When collect_profile=True, also captures stderr and parses PROFILE| lines
    from PROFILE_DUMP() output (requires PROFILE-tier build of entry.exe).
    """
    try:
        r = subprocess.run(
            [str(exe_path), "--benchmark", str(method_index), str(iterations)],
            capture_output=True, text=True, timeout=120,
        )
    except subprocess.TimeoutExpired:
        return {"methodIndex": method_index, "error": "timed out"}
    except OSError as e:
        return {"methodIndex": method_index, "error": str(e)}

    output = (r.stdout or "").strip()
    result = None
    for line in output.splitlines():
        line = line.strip()
        if line.startswith("{"):
            try:
                result = json.loads(line)
                result["methodIndex"] = method_index
            except json.JSONDecodeError:
                pass

    if result is None:
        result = {"methodIndex": method_index, "error": f"no JSON output: {output[:200]}", "exitCode": r.returncode}

    if collect_profile:
        stderr_text = r.stderr or ""
        profile = _parse_profile_data(stderr_text)
        if profile["has_profile_data"]:
            result["profile"] = profile

    return result


def _run_benchmark_all(
    exe_path: Path, method_indices: list[int], iterations: int = 100000,
    collect_profile: bool = False,
) -> list[dict[str, Any]] | None:
    """Run entry.exe --benchmark-all to benchmark multiple methods in one call.

    Returns a list of per-method result dicts, or None if --benchmark-all
    is not supported by the entry.exe (falls through to per-method mode).
    """
    try:
        r = subprocess.run(
            [str(exe_path), "--benchmark-all", str(iterations)],
            capture_output=True, text=True, timeout=300,
        )
    except (subprocess.TimeoutExpired, OSError):
        return None

    output = (r.stdout or "").strip()
    try:
        data = json.loads(output)
    except json.JSONDecodeError:
        return None

    raw_results = data.get("benchmarkAll")
    if not isinstance(raw_results, list):
        return None

    # Build a lookup from methodIndex -> result
    by_index: dict[int, dict] = {}
    for entry in raw_results:
        mi = entry.get("methodIndex")
        if mi is not None:
            by_index[mi] = entry

    # Return results in the requested order
    ordered = []
    for mi in method_indices:
        entry = by_index.get(mi)
        if entry is None:
            ordered.append({"methodIndex": mi, "error": "missing from batch output"})
        else:
            ordered.append(entry)

            # Attach PROFILE data if collected (stderr)
            if collect_profile and r.stderr:
                profile = _parse_profile_data(r.stderr)
                if profile["has_profile_data"]:
                    entry["profile"] = profile

    return ordered


def _run_all_benchmarks(
    ctx: FamilyContext, exe_path: Path, label: str,
    collect_profile: bool = False,
) -> dict[str, Any]:
    """Run --benchmark for all methods under a given EXE.

    R5: Before timing, runs --fact-json to verify all methods pass (value gate).
    Methods that fail the value gate are excluded from benchmark timing.
    """
    method_count = _load_method_count(ctx)
    if method_count == 0:
        return {"status": "skipped", "summary": "no methods in contract"}

    # R5: Value gate — pre-verify all methods before benchmarking
    value_gate_failures: set[int] = set()
    try:
        r = subprocess.run(
            [str(exe_path), "--fact-json"],
            capture_output=True, text=True, timeout=120,
        )
        # Check exit code zero AND valid JSON output
        fact_data = None
        if r.returncode == 0:
            # Robust JSON extraction — find first complete JSON object in output
            output = (r.stdout or "").strip()
            json_start = output.find("{")
            if json_start >= 0:
                depth = 0
                json_end = -1
                for i in range(json_start, len(output)):
                    if output[i] == '{':
                        depth += 1
                    elif output[i] == '}':
                        depth -= 1
                        if depth == 0:
                            json_end = i + 1
                            break
                if json_end > json_start:
                    try:
                        fact_data = json.loads(output[json_start:json_end])
                    except json.JSONDecodeError:
                        pass

        if fact_data:
            for nr in fact_data.get("factResults", []):
                si = nr.get("si", -1)
                if si >= 0 and not nr.get("passed", True):
                    value_gate_failures.add(si)
                    print(f"    [benchmark/{label}] value gate FAIL: si={si}")
        elif r.returncode != 0:
            print(f"    [benchmark/{label}] WARNING: --fact-json returned exit={r.returncode}, skipping value gate")
        else:
            print(f"    [benchmark/{label}] WARNING: could not parse --fact-json output, skipping value gate")
    except Exception as e:
        print(f"    [benchmark/{label}] WARNING: value gate error: {e}, skipping")

    if value_gate_failures:
        print(f"    [benchmark/{label}] value gate: {len(value_gate_failures)} methods excluded from benchmark")

    # ── Try --benchmark-all (single invocation) first, fall back to per-method ──
    results: list[dict[str, Any]] = []
    total_ops = 0.0
    ok_count = 0
    fail_count = 0
    used_batch_mode = False

    # Methods excluded by value gate
    for i in value_gate_failures:
        results.append({
            "methodIndex": i,
            "error": "value_gate_failed",
            "opsPerSecond": 0,
        })
        fail_count += 1

    remaining = [i for i in range(method_count) if i not in value_gate_failures]
    if remaining:
        batch_result = _run_benchmark_all(exe_path, remaining, collect_profile=collect_profile)
        if batch_result is not None:
            used_batch_mode = True
            results.extend(batch_result)
            for r in batch_result:
                if r and "error" not in r:
                    total_ops += r.get("opsPerSecond", 0)
                    ok_count += 1
                else:
                    fail_count += 1
        else:
            # Fall back to per-method subprocess
            for i in remaining:
                result = _run_single_benchmark(exe_path, i, collect_profile=collect_profile)
                results.append(result)
                if result and "error" not in result:
                    total_ops += result.get("opsPerSecond", 0)
                    ok_count += 1
                else:
                    fail_count += 1

    mode = "batch" if used_batch_mode else "per-method"

    avg_ops = total_ops / ok_count if ok_count > 0 else 0.0
    print(f"  [benchmark/{label}] {ok_count}/{method_count} OK ({mode}), avg {avg_ops:.0f} ops/s"
          f" ({len(value_gate_failures)} value-gate skipped)")

    # Aggregate PROFILE data across methods (when collected)
    profile_summary = None
    if collect_profile:
        aggregated: dict[str, dict[str, float]] = {}
        for r in results:
            profile = r.get("profile") if r else None
            if not profile:
                continue
            for name, data in profile.get("scopes", {}).items():
                if name not in aggregated:
                    aggregated[name] = {"total_ns": 0.0, "total_count": 0, "min_cycles": float("inf"), "max_cycles": 0}
                agg = aggregated[name]
                agg["total_ns"] += data["total_ns"]
                agg["total_count"] += data["count"]
                if data["min_cycles"] < agg["min_cycles"]:
                    agg["min_cycles"] = data["min_cycles"]
                if data["max_cycles"] > agg["max_cycles"]:
                    agg["max_cycles"] = data["max_cycles"]

        if aggregated:
            total_profile_ns = sum(a["total_ns"] for a in aggregated.values())
            scope_list = []
            for name, agg in aggregated.items():
                avg_ns = agg["total_ns"] / agg["total_count"] if agg["total_count"] > 0 else 0.0
                pct = (agg["total_ns"] / total_profile_ns * 100) if total_profile_ns > 0 else 0.0
                scope_list.append({
                    "scope_name": name,
                    "total_ns": round(agg["total_ns"], 1),
                    "total_count": agg["total_count"],
                    "avg_ns": round(avg_ns, 1),
                    "min_cycles": agg["min_cycles"],
                    "max_cycles": agg["max_cycles"],
                    "percent_of_profile": round(pct, 2),
                })
            scope_list.sort(key=lambda s: -s["total_ns"])
            profile_summary = {
                "total_profile_ns": round(total_profile_ns, 1),
                "num_scopes": len(scope_list),
                "scopes": scope_list,
            }
            print(f"  [benchmark/{label}] PROFILE: {len(scope_list)} scope(s) collected, "
                  f"top: {scope_list[0]['scope_name']}={scope_list[0]['total_ns']:.0f}ns"
                  if scope_list else "")

    return {
        "status": "passed" if ok_count > 0 else "failed",
        "label": label,
        "results": results,
        "okCount": ok_count,
        "failCount": fail_count,
        "totalMethods": method_count,
        "averageOpsPerSecond": avg_ops,
        "valueGateFailures": list(value_gate_failures),
        "profile": profile_summary,
    }


def _run_native_benchmarks(
    ctx: FamilyContext,
    aot_exe: Path,
    jit_exe: Path,
    collect_profile: bool,
) -> dict[str, Any]:
    """Run native AOT and JIT benchmarks (sequential within this call)."""
    benchmarks: dict[str, Any] = {}

    if aot_exe.exists():
        aot_result = _run_all_benchmarks(ctx, aot_exe, "native-aot", collect_profile=collect_profile)
        benchmarks["native-aot"] = aot_result
    else:
        benchmarks["native-aot"] = {"status": "skipped", "summary": "entry.exe not found"}

    if jit_exe.exists():
        jit_result = _run_all_benchmarks(ctx, jit_exe, "native-jit")
        benchmarks["native-jit"] = jit_result
    else:
        benchmarks["native-jit"] = {"status": "skipped", "summary": "entry-jit.exe not found"}

    return benchmarks


def run_benchmark(ctx: FamilyContext, stages: dict[str, StageResult]) -> StageResult:
    """Stage 8: native benchmark (native-aot + native-jit)."""
    start = time.perf_counter()

    method_count = _load_method_count(ctx)
    if method_count == 0:
        return StageResult(
            stage="benchmark", status="skipped",
            summary="no methods in contract",
            duration_ms=int((time.perf_counter() - start) * 1000),
        )

    aot_exe = ctx.entry_exe_path
    jit_exe = ctx.entry_jit_exe_path
    collect_profile = ctx.native_config == "profile"

    benchmarks = _run_native_benchmarks(ctx, aot_exe, jit_exe, collect_profile)

    total_ok = sum(
        1 for b in benchmarks.values()
        if isinstance(b, dict) and b.get("status") == "passed"
    )

    status = "passed" if total_ok > 0 else "failed"

    return StageResult(
        stage="benchmark", status=status,
        summary=f"{status}: native-aot="
                f"{benchmarks.get('native-aot', {}).get('okCount', 0)}/"
                f"{benchmarks.get('native-aot', {}).get('totalMethods', 0)}, "
                f"native-jit="
                f"{benchmarks.get('native-jit', {}).get('okCount', 0)}/"
                f"{benchmarks.get('native-jit', {}).get('totalMethods', 0)}",
        details=benchmarks,
        duration_ms=int((time.perf_counter() - start) * 1000),
    )
