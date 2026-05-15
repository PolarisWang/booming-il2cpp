"""AsmCompare verifier — Stage 3.5 in the family verification pipeline.

Runs chaos-il2cpp asm-compare for each subject method with --format json --sections metrics,
aggregates deterministic metrics into asm-compare-report.json.

Usage (via orchestrator):
    from asm_compare_verifier import verify_family_asm_compare
    result = verify_family_asm_compare("convert-char", "System.Private.CoreLib")
"""

from __future__ import annotations

import json
import re
import subprocess
import time
from pathlib import Path
from typing import Any

_HERE = Path(__file__).resolve().parent
_REPO_ROOT = _HERE.parents[4]
_VERIFICATION_BASE = _REPO_ROOT / "verification" / "foundation-dll"
_CHAOS_CSPROJ = _REPO_ROOT / "src" / "managed" / "Chaos.IL2CPP.Driver" / "Chaos.IL2CPP.Driver.csproj"


def verify_family_asm_compare(family_slug: str, assembly: str) -> dict[str, Any]:
    """Run asm-compare for all subject methods and return aggregated result.

    Returns dict with keys: status, summary, details, reportPath.
    Writes asm-compare-report.json to the family directory.
    """
    report_path = _VERIFICATION_BASE / assembly / family_slug / "asm-compare-report.json"

    # 1. Discover DLL
    dll_path = _discover_subjects_dll(family_slug, assembly)
    if dll_path is None:
        return _error_result("Subjects DLL not found — run codegen stage first", report_path)

    # 2. Load contract methods
    mids = _load_contract_methods(family_slug, assembly)
    if not mids:
        return _error_result("No methods in contract", report_path)

    # 3. Run asm-compare for each method
    method_results: list[dict[str, Any]] = []
    ok_count = 0
    fail_count = 0
    jit_counts: list[int] = []
    aot_counts: list[int] = []

    for idx, mid in enumerate(mids):
        # The subjects DLL uses Subject_N naming, not the contract's methodSubjectId.
        # E.g. contract has "System.Convert::ToChar:System.Char(System.Boolean)" but
        # the subjects DLL has ConvertCharSubjects.Subject_0().
        subject_method_name = f"Subject_{idx}"
        entry = _run_single_asm_compare(dll_path, subject_method_name, mid, idx)
        method_results.append(entry)

        if entry["status"] == "ok":
            ok_count += 1
            m = entry.get("metrics", {})
            jc = m.get("jitInstructionCount", 0)
            ac = m.get("aotInstructionCount", 0)
            if jc > 0:
                jit_counts.append(jc)
            if ac > 0:
                aot_counts.append(ac)
        else:
            fail_count += 1

    # 4. Summary statistics
    summary = _compute_summary(mids, jit_counts, aot_counts, ok_count, fail_count, method_results)

    report = {
        "schemaVersion": 1,
        "assemblyName": assembly,
        "familyId": f"family/{assembly}/{family_slug.replace('-', '/')}",
        "capturedAt": time.strftime("%Y-%m-%dT%H:%M:%S"),
        "summary": summary,
        "methodResults": method_results,
    }
    report_path.parent.mkdir(parents=True, exist_ok=True)
    with open(report_path, "w", encoding="utf-8") as f:
        json.dump(report, f, indent=2, ensure_ascii=False)

    # Determine status: passed if >= 80% methods OK
    ok_pct = (ok_count / len(mids) * 100) if mids else 0
    status = "passed" if ok_pct >= 80 else "failed"

    return {
        "status": status,
        "summary": (
            f"asm-compare: {ok_count}/{len(mids)} methods OK, "
            f"avg JIT={summary['jitInstructionCount']['avg']} instr, "
            f"avg AOT IR={summary['aotInstructionCount']['avg']} instr, "
            f"expansion={summary.get('overallIrExpansionRatio', 'N/A')}x"
        ),
        "details": summary,
        "reportPath": str(report_path),
    }


# ── Internal helpers ────────────────────────────────────────────────


def _load_contract_methods(family_slug: str, assembly: str) -> list[str]:
    """Load method subject IDs from capability-family-contract.json."""
    family_dir = _VERIFICATION_BASE / assembly / family_slug
    contract_path = family_dir / "capability-family-contract.json"
    if not contract_path.exists():
        return []
    try:
        with open(contract_path, encoding="utf-8") as f:
            contract = json.load(f)
        mids = contract.get("methodSubjectIds", [])
        if not mids:
            mids = [m["methodSubjectId"] for m in contract.get("methodContracts", []) if m.get("methodSubjectId")]
        return mids
    except (OSError, ValueError, KeyError):
        return []


def _short_method_name(full_id: str) -> str:
    """Extract TypeName::MethodName from a full methodSubjectId.

    Input:  System.Private.CoreLib/ConvertCharSubjects/ConvertCharSubjects::Subject_0:System.Void()
    Output: ConvertCharSubjects::Subject_0

    Input:  System.Private.CoreLib/System.Convert::ToChar:System.Char(System.Object)
    Output: System.Convert::ToChar
    """
    m = re.match(r'(?:[^/]+/)?([^:]+)::([^:]+)(?::|$)', full_id)
    if m:
        type_part = m.group(1)
        method = m.group(2)
        type_name = type_part.split("/")[-1]
        return f"{type_name}::{method}"
    if "::" in full_id:
        return full_id.split("::")[-1]
    return ""


def _discover_subjects_dll(family_slug: str, assembly: str) -> Path | None:
    """Find the subjects DLL in the family managed build output."""
    family_dir = _VERIFICATION_BASE / assembly / family_slug
    dll_dir = family_dir / "managed" / "subjects" / "build-output"
    if not dll_dir.exists():
        return None
    dlls = list(dll_dir.glob("*Subjects.dll"))
    if dlls:
        return dlls[0]
    dlls = list(dll_dir.glob("*.dll"))
    return dlls[0] if dlls else None


def _run_single_asm_compare(dll_path: Path, short_name: str, full_id: str, idx: int) -> dict[str, Any]:
    """Run asm-compare for one method and return the result entry."""
    cmd = [
        "dotnet", "run", "--project", str(_CHAOS_CSPROJ), "--",
        "asm-compare",
        str(dll_path),
        "--method", short_name,
        "--format", "json",
        "--sections", "metrics",
    ]

    try:
        result = subprocess.run(
            cmd,
            capture_output=True,
            text=True,
            encoding="utf-8",
            errors="replace",
            timeout=120,
            cwd=str(_REPO_ROOT),
        )
    except subprocess.TimeoutExpired:
        return {
            "methodIndex": idx,
            "methodSubjectId": full_id,
            "shortName": short_name,
            "status": "error",
            "error": "asm-compare timed out (120s)",
        }
    except OSError as e:
        return {
            "methodIndex": idx,
            "methodSubjectId": full_id,
            "shortName": short_name,
            "status": "error",
            "error": str(e),
        }

    # Attempt JSON parse regardless of return code
    stdout = result.stdout or ""
    # Strip leading non-JSON garbage (dotnet run progress output mixed into stdout)
    json_start = stdout.find("{")
    if json_start > 0:
        stdout = stdout[json_start:]
    try:
        data = json.loads(stdout)
    except (json.JSONDecodeError, ValueError) as e:
        return {
            "methodIndex": idx,
            "methodSubjectId": full_id,
            "shortName": short_name,
            "status": "error",
            "error": f"JSON parse failed: {e}",
            "stdout": (result.stdout or "")[:500],
            "stderr": (result.stderr or "")[:500],
        }

    entry = _extract_metrics(data, full_id, short_name)

    if result.returncode != 0:
        entry["status"] = "warning"
        entry["returnCode"] = result.returncode
        entry["stderr"] = (result.stderr or "")[:500]
    else:
        entry["status"] = "ok"

    entry["methodIndex"] = idx
    return entry


def _extract_metrics(data: dict, method_subject_id: str, short_name: str) -> dict[str, Any]:
    """Extract relevant metrics from asm-compare JSON output."""
    metrics = data.get("metrics", {})
    jit_info = data.get("jit") or {}

    aot_raw = data.get("aot")
    if aot_raw:
        aot_info = aot_raw
        aot_status: str = aot_info.get("status", "unknown")
    else:
        aot_info = {}
        aot_status = "not_found"

    entry: dict[str, Any] = {
        "methodSubjectId": method_subject_id,
        "shortName": short_name,
        "metrics": {
            "jitInstructionCount": metrics.get("jitInstructionCount", 0),
            "aotInstructionCount": metrics.get("aotInstructionCount", 0),
            "ratio": metrics.get("ratio", 0),
            "jitCodeSize": metrics.get("jitCodeSize", 0),
        },
        "jit": {
            "status": jit_info.get("status", "unknown"),
            "size": jit_info.get("size", 0),
            "instructions": jit_info.get("instructions", 0),
        },
        "aot": {
            "status": aot_status,
            "symbol": aot_info.get("symbol", ""),
            "instructions": aot_info.get("instructions", 0),
        },
        "analysis": [],
    }

    # Parse analysis findings
    analysis = data.get("analysis")
    if isinstance(analysis, list):
        for finding in analysis:
            entry["analysis"].append({
                "severity": finding.get("severity", "info"),
                "category": finding.get("category", ""),
                "message": finding.get("message", ""),
            })

    return entry


def _compute_summary(
    mids: list[str],
    jit_counts: list[int],
    aot_counts: list[int],
    ok_count: int,
    fail_count: int,
    method_results: list[dict[str, Any]],
) -> dict[str, Any]:
    """Compute aggregate summary statistics."""
    external_calls = 0
    virtual_dispatches = 0
    boxing_ops = 0

    for entry in method_results:
        for finding in entry.get("analysis", []):
            cat = finding.get("category", "")
            if cat == "EXTERNAL_RUNTIME_CALL":
                external_calls += 1
            elif cat == "VIRTUAL_DISPATCH":
                virtual_dispatches += 1
            elif cat == "BOXING_OVERHEAD":
                boxing_ops += 1

    summary: dict[str, Any] = {
        "totalMethods": len(mids),
        "okCount": ok_count,
        "failedCount": fail_count,
        "skippedCount": len(mids) - ok_count - fail_count,
        "jitInstructionCount": _stat_range(jit_counts),
        "aotInstructionCount": _stat_range(aot_counts),
        "totalExternalRuntimeCalls": external_calls,
        "totalVirtualDispatches": virtual_dispatches,
        "totalBoxingOps": boxing_ops,
    }

    if jit_counts and aot_counts:
        summary["overallIrExpansionRatio"] = round(sum(aot_counts) / sum(jit_counts), 2)
    else:
        summary["overallIrExpansionRatio"] = 0

    return summary


def _stat_range(values: list[int]) -> dict[str, float]:
    """Compute min/max/avg/total from a list of values."""
    if not values:
        return {"min": 0, "max": 0, "avg": 0, "total": 0}
    return {
        "min": min(values),
        "max": max(values),
        "avg": round(sum(values) / len(values), 1),
        "total": sum(values),
    }


def _error_result(message: str, report_path: Path) -> dict[str, Any]:
    """Return a skipped/failed result when prerequisites are missing."""
    return {
        "status": "skipped",
        "summary": message,
        "details": {},
        "reportPath": str(report_path),
    }