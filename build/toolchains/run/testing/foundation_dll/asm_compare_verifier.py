"""AsmCompare verifier — Stage 3.5 in the family verification pipeline.

Runs chaos-il2cpp asm-compare for all subject methods in a single batch call,
aggregates deterministic metrics into asm-compare-report.json.

Phase 1: Batch mode — pipeline runs once for all methods.
Phase 2: Build once.
P0:     Skip C++ generation when only metrics/analysis needed.
P1:     Parallel JIT capture.
P2:     Cache pipeline results keyed by DLL content hash.

Usage (via orchestrator):
    from asm_compare_verifier import verify_family_asm_compare
    result = verify_family_asm_compare("convert-char", "System.Private.CoreLib")
"""

from __future__ import annotations

import hashlib
import hashlib
import json
import os
import re
import subprocess
import time
from concurrent.futures import ThreadPoolExecutor, as_completed
from pathlib import Path
from typing import Any

_HERE = Path(__file__).resolve().parent
_REPO_ROOT = _HERE.parents[4]
_VERIFICATION_BASE = _REPO_ROOT / "testing" / "foundation-dll"
_CHAOS_CSPROJ = _REPO_ROOT / "src" / "managed" / "Chaos.IL2CPP.Driver" / "Chaos.IL2CPP.Driver.csproj"

_CACHE_DIR = _VERIFICATION_BASE / ".asm-compare-cache"

_DRIVER_BUILT = False


def verify_family_asm_compare(family_slug: str, assembly: str,
                              verification: str | Path | None = None) -> dict[str, Any]:
    """Run asm-compare for all subject methods in a single batch call.

    Phase 1: Batch mode — pipeline runs once for all methods.
    Phase 2: Build once.

    Args:
        family_slug: Family directory name (e.g. "convert-char").
        assembly: Assembly name (e.g. "System.Private.CoreLib").
        verification: Optional override for verification base directory.
            Used by the new (testing/) pipeline to point at testing/foundation-dll.

    Returns dict with keys: status, summary, details, reportPath.
    Writes asm-compare-report.json to the family directory.
    """
    v_base = Path(verification) if verification else _VERIFICATION_BASE
    report_path = v_base / assembly / family_slug / "asm-compare-report.json"

    # 1. Discover DLL
    dll_path = _discover_subjects_dll(family_slug, assembly, verification=v_base)
    if dll_path is None:
        return _error_result("Subjects DLL not found — run codegen stage first", report_path)

    # 2. Load contract methods
    mids = _load_contract_methods(family_slug, assembly, verification=v_base)
    if not mids:
        return _error_result("No methods in contract", report_path)

    # 3. Batch build (once per process lifetime)
    _ensure_driver_built()

    # 4. Check cache (P2): skip asm-compare if DLL unchanged
    dll_stat = dll_path.stat()
    cache_key = _compute_cache_key(dll_path)
    cached = _load_cached_result(cache_key, mids, dll_stat)
    if cached is not None:
        print(f"    [asm-compare cache HIT for {dll_path.name}]")
        method_results = cached
    else:
        # 5. Run batch asm-compare
        batch_result = _run_batch_asm_compare(dll_path, mids)
        method_results = batch_result.get("methodResults", [])
        _save_cached_result(cache_key, mids, dll_stat, method_results)
    ok_count = sum(1 for r in method_results if r.get("status") == "ok")
    fail_count = sum(1 for r in method_results if r.get("status") not in ("ok", "skipped"))

    jit_counts = []
    aot_counts = []
    for entry in method_results:
        m = entry.get("metrics", {})
        jc = m.get("jitInstructionCount") or 0
        ac = m.get("aotInstructionCount") or 0
        if jc > 0:
            jit_counts.append(jc)
        if ac > 0:
            aot_counts.append(ac)

    summary = _compute_summary(mids, jit_counts, aot_counts, ok_count, fail_count, method_results)

    report = {
        "schemaVersion": 2,
        "assemblyName": assembly,
        "familyId": f"family/{assembly}/{family_slug.replace('-', '/')}",
        "capturedAt": time.strftime("%Y-%m-%dT%H:%M:%S"),
        "batchMode": True,
        "summary": summary,
        "methodResults": method_results,
    }
    report_path.parent.mkdir(parents=True, exist_ok=True)
    with open(report_path, "w", encoding="utf-8") as f:
        json.dump(report, f, indent=2, ensure_ascii=False)

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


def _ensure_driver_built() -> None:
    """Build Chaos.IL2CPP.Driver once per process lifetime (Phase 2)."""
    global _DRIVER_BUILT  # noqa: PLW0603
    if _DRIVER_BUILT:
        return
    _DRIVER_BUILT = True

    cfg = os.environ.get("CHAOS_BUILD_CONFIG", "Release")
    cmd = ["dotnet", "build", str(_CHAOS_CSPROJ), "-c", cfg, "--nologo", "-v", "quiet"]
    try:
        subprocess.run(cmd, check=True, capture_output=True, text=True, timeout=180)
    except subprocess.TimeoutExpired:
        print("    [driver build] timed out (180s)")
    except subprocess.CalledProcessError as e:
        print(f"    [driver build] warning: build failed ({e.stderr[:200]})")


def _load_contract_methods(family_slug: str, assembly: str,
                           verification: Path | None = None) -> list[str]:
    """Load method subject IDs from capability-family-contract.json."""
    v_base = verification if verification else _VERIFICATION_BASE
    family_dir = v_base / assembly / family_slug
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
    """Extract MethodName:ReturnType(Params) from full methodSubjectId.

    Must match AsmCompareHandler's methodMap index format:
        m.SubjectId.Substring(m.SubjectId.LastIndexOf("::") + 2)
    which gives everything after the last '::', e.g.:
        "ToChar:System.Char(System.Boolean)"  (includes return type)
        "Wait:System.Void()"
    """
    if "::" in full_id:
        return full_id.rsplit("::", 1)[-1]
    return full_id


def _discover_subjects_dll(family_slug: str, assembly: str,
                           verification: Path | None = None) -> Path | None:
    """Find the subjects DLL for asm-compare analysis."""
    import glob as _glob

    v_base = verification if verification else _VERIFICATION_BASE
    family_dir = v_base / assembly / family_slug
    candidates = [
        family_dir / "managed" / "subjects" / "build-output",
        family_dir / "managed" / "subjects" / "obj" / "Debug" / "net8.0",
        family_dir / "managed" / "subjects" / "obj" / "Debug" / "net10.0",
        family_dir / "managed" / "subjects" / "bin" / "Debug" / "net8.0",
        family_dir / "managed" / "subjects" / "bin" / "Debug" / "net10.0",
    ]
    for dll_dir in candidates:
        if not dll_dir.exists():
            continue
        dlls = list(dll_dir.glob("*Subjects.dll"))
        if dlls:
            return dlls[0]
        dlls = list(dll_dir.glob("*.dll"))
        if dlls:
            return dlls[0]
    return None


def _run_batch_asm_compare(dll_path: Path, mids: list[str]) -> dict[str, Any]:
    """Run asm-compare once for all methods (Phase 1: batch mode).

    Uses --methods with Subject_N names (for AOT pipeline lookup) and
    --method-subject-ids with full methodSubjectId from the contract (for JIT
    capture of the real target method rather than the Subjects DLL wrapper).
    """
    short_names = [f"Subject_{i}" for i in range(len(mids))]
    methods_arg = ",".join(short_names)
    subject_ids_arg = ",".join(mids)

    cfg = os.environ.get("CHAOS_BUILD_CONFIG", "Release")

    cmd = [
        "dotnet", "run", "--project", str(_CHAOS_CSPROJ), "--no-build",
        "-c", cfg, "--",
        "asm-compare",
        str(dll_path),
        "--methods", methods_arg,
        "--method-subject-ids", subject_ids_arg,
        "--format", "json",
        "--sections", "metrics,analysis",
    ]

    # Set longer timeout for batch mode (5 min + 30s per method)
    batch_timeout = max(300, 60 * len(mids)) + 30

    try:
        result = subprocess.run(
            cmd,
            capture_output=True,
            text=True,
            encoding="utf-8",
            errors="replace",
            timeout=batch_timeout,
            cwd=str(_REPO_ROOT),
        )
    except subprocess.TimeoutExpired:
        return {
            "methodResults": [
                {
                    "methodIndex": i,
                    "methodSubjectId": mid,
                    "shortName": _short_method_name(mid),
                    "status": "error",
                    "error": f"batch asm-compare timed out ({batch_timeout}s)",
                }
                for i, mid in enumerate(mids)
            ],
        }
    except OSError as e:
        return {
            "methodResults": [
                {
                    "methodIndex": i,
                    "methodSubjectId": mid,
                    "shortName": _short_method_name(mid),
                    "status": "error",
                    "error": str(e),
                }
                for i, mid in enumerate(mids)
            ],
        }

    stdout = result.stdout or ""
    stdout = stdout.strip()
    # Handle both JSON array (batch) and object (single) output
    arr_start = stdout.find("[")
    obj_start = stdout.find("{")
    if arr_start >= 0 and (obj_start < 0 or arr_start < obj_start):
        stdout = stdout[arr_start:]
    elif obj_start >= 0:
        stdout = stdout[obj_start:]

    try:
        data = json.loads(stdout)
    except (json.JSONDecodeError, ValueError) as e:
        return {
            "methodResults": [
                {
                    "methodIndex": i,
                    "methodSubjectId": mid,
                    "shortName": _short_method_name(mid),
                    "status": "error",
                    "error": f"JSON parse failed: {e}",
                    "stdout": (result.stdout or "")[:500],
                    "stderr": (result.stderr or "")[:500],
                }
                for i, mid in enumerate(mids)
            ],
        }

    # Normalize: ensure we handle both single result (dict) and batch (list)
    raw = data
    if isinstance(raw, dict):
        results_list = [raw]
    elif isinstance(raw, list):
        results_list = raw
    else:
        results_list = []

    method_results = []
    for i, mid in enumerate(mids):
        entry = results_list[i] if i < len(results_list) else {}
        normalized = _extract_metrics(entry, mid, _short_method_name(mid), i)
        method_results.append(normalized)

    return {"methodResults": method_results}


def _extract_metrics(
    data: dict, method_subject_id: str, short_name: str, idx: int
) -> dict[str, Any]:
    """Extract relevant metrics from batch JSON entry."""
    if data.get("status") == "not_found":
        return {
            "methodIndex": idx,
            "methodSubjectId": method_subject_id,
            "shortName": short_name,
            "status": "error",
            "error": "Method not found in IL2CPP pipeline output",
        }

    if "error" in data:
        return {
            "methodIndex": idx,
            "methodSubjectId": method_subject_id,
            "shortName": short_name,
            "status": "error",
            "error": data.get("error", "unknown"),
        }

    metrics = data.get("metrics", {})
    jit_info = data.get("jit") or {}
    aot_info = data.get("aot") or {}

    jit_status: str = jit_info.get("status", "unknown") if jit_info else "not_found"
    aot_status: str = aot_info.get("status", "unknown") if aot_info else "not_found"

    entry: dict[str, Any] = {
        "methodIndex": idx,
        "methodSubjectId": method_subject_id,
        "shortName": short_name,
        "status": "ok" if data.get("status") == "ok" else "warning",
        "metrics": {
            "jitInstructionCount": metrics.get("jitInstructionCount") or 0,
            "aotInstructionCount": metrics.get("aotInstructionCount") or 0,
            "ratio": metrics.get("ratio") or 0,
            "jitCodeSize": metrics.get("jitCodeSize") or 0,
        },
        "jit": {
            "status": jit_status,
            "size": jit_info.get("size", 0) if jit_info else 0,
            "instructions": jit_info.get("instructions", 0) if jit_info else 0,
        },
        "aot": {
            "status": aot_status,
            "symbol": aot_info.get("symbol", "") if aot_info else "",
            "instructions": aot_info.get("instructions", 0) if aot_info else 0,
        },
        "analysis": [],
        "instructionCategories": {},
    }

    # Parse analysis findings
    analysis = data.get("analysis")
    if isinstance(analysis, list):
        cat_counts: dict[str, int] = {}
        for finding in analysis:
            entry["analysis"].append({
                "severity": finding.get("severity", "info"),
                "category": finding.get("category", ""),
                "message": finding.get("message", ""),
            })
            cat = finding.get("category", "other")
            cat_counts[cat] = cat_counts.get(cat, 0) + 1
        entry["instructionCategories"] = dict(sorted(cat_counts.items()))

    # IR trace breakdown
    ir_trace = data.get("irTrace") or data.get("aot", {}).get("irTrace")
    if ir_trace and isinstance(ir_trace, list):
        ir_steps: dict[str, int] = {}
        for step in ir_trace:
            op = step.get("op", "unknown")
            ir_steps[op] = ir_steps.get(op, 0) + 1
        entry["irTraceSummary"] = dict(sorted(ir_steps.items(), key=lambda x: -x[1])[:20])

    # JIT instruction type breakdown
    jit_disasm = data.get("jitDisasm") or (jit_info or {}).get("disasm")
    if jit_disasm and isinstance(jit_disasm, list):
        jit_types: dict[str, int] = {}
        for inst in jit_disasm:
            op = inst.get("op", "unknown").split(" ")[0] if isinstance(inst.get("op"), str) else "unknown"
            jit_types[op] = jit_types.get(op, 0) + 1
        entry["jitInstructionMix"] = dict(sorted(jit_types.items(), key=lambda x: -x[1])[:15])

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
    all_categories: dict[str, int] = {}

    for entry in method_results:
        for finding in entry.get("analysis", []):
            cat = finding.get("category", "")
            if cat == "EXTERNAL_RUNTIME_CALL":
                external_calls += 1
            elif cat == "VIRTUAL_DISPATCH":
                virtual_dispatches += 1
            elif cat == "BOXING_OVERHEAD":
                boxing_ops += 1
        for cat, count in entry.get("instructionCategories", {}).items():
            all_categories[cat] = all_categories.get(cat, 0) + count

    top_categories = dict(sorted(all_categories.items(), key=lambda x: -x[1])[:10])

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
        "topInstructionCategories": top_categories,
    }

    if jit_counts and aot_counts:
        summary["overallIrExpansionRatio"] = round(sum(aot_counts) / sum(jit_counts), 2)
    else:
        summary["overallIrExpansionRatio"] = 0

    ratios = []
    for entry in method_results:
        r = entry.get("metrics", {}).get("ratio") or 0
        if r > 0:
            ratios.append(r)
    if ratios:
        summary["averageExpansionRatio"] = round(sum(ratios) / len(ratios), 2)
        summary["maxExpansionRatio"] = round(max(ratios), 2)
        summary["minExpansionRatio"] = round(min(ratios), 2)
    else:
        summary["averageExpansionRatio"] = 0

    return summary


def _stat_range(values: list[int]) -> dict[str, float]:
    if not values:
        return {"min": 0, "max": 0, "avg": 0, "total": 0}
    return {
        "min": min(values),
        "max": max(values),
        "avg": round(sum(values) / len(values), 1),
        "total": sum(values),
    }


def _error_result(message: str, report_path: Path) -> dict[str, Any]:
    return {
        "status": "skipped",
        "summary": message,
        "details": {},
        "reportPath": str(report_path),
    }


# ── P2: Pipeline result cache ──────────────────────────────────────


def _compute_cache_key(dll_path: Path) -> str:
    """SHA256 of DLL content for cache key."""
    h = hashlib.sha256()
    with open(dll_path, "rb") as f:
        while True:
            chunk = f.read(65536)
            if not chunk:
                break
            h.update(chunk)
    return h.hexdigest()[:32]


def _mids_hash(mids: list[str]) -> str:
    """Hash method list to detect contract changes."""
    h = hashlib.sha256()
    for mid in mids:
        h.update(mid.encode())
    return h.hexdigest()[:16]


def _load_cached_result(
    cache_key: str, mids: list[str], dll_stat: os.stat_result
) -> list[dict[str, Any]] | None:
    """Load cached methodResults if cache is valid.

    Validates:
      - DLL mtime and size match (DLL unchanged)
      - Method list matches (contract unchanged)
    """
    cache_dir = _CACHE_DIR / cache_key
    meta_path = cache_dir / "metadata.json"
    data_path = cache_dir / "method_results.json"
    if not meta_path.exists() or not data_path.exists():
        return None

    try:
        with open(meta_path, encoding="utf-8") as f:
            meta = json.load(f)
    except (OSError, ValueError):
        return None

    # Validate DLL freshness
    if meta.get("dll_mtime") != dll_stat.st_mtime_ns:
        return None
    if meta.get("dll_size") != dll_stat.st_size:
        return None

    # Validate method list
    if meta.get("mids_hash") != _mids_hash(mids):
        return None

    try:
        with open(data_path, encoding="utf-8") as f:
            return json.load(f)
    except (OSError, ValueError):
        return None


def _save_cached_result(
    cache_key: str,
    mids: list[str],
    dll_stat: os.stat_result,
    method_results: list[dict[str, Any]],
) -> None:
    """Save methodResults to cache."""
    cache_dir = _CACHE_DIR / cache_key
    cache_dir.mkdir(parents=True, exist_ok=True)

    meta = {
        "dll_mtime": dll_stat.st_mtime_ns,
        "dll_size": dll_stat.st_size,
        "mids_hash": _mids_hash(mids),
        "cached_at": time.strftime("%Y-%m-%dT%H:%M:%S"),
        "method_count": len(method_results),
    }
    meta_path = cache_dir / "metadata.json"
    data_path = cache_dir / "method_results.json"

    with open(meta_path, "w", encoding="utf-8") as f:
        json.dump(meta, f)

    with open(data_path, "w", encoding="utf-8") as f:
        json.dump(method_results, f, indent=2, ensure_ascii=False)
