"""Unit tests for asm_compare_verifier.py.

Tests method name parsing, metrics extraction, error handling, and summary computation.
"""

from __future__ import annotations

import json
import sys
import time
from pathlib import Path
from typing import Any

# Ensure the module is importable
_HERE = Path(__file__).resolve().parent
sys.path.insert(0, str(_HERE))

from asm_compare_verifier import (
    _short_method_name,
    _extract_metrics,
    _compute_summary,
    _stat_range,
    _error_result,
)


# ── _short_method_name ──────────────────────────────────────────────

def test_short_method_name_full_subject_id():
    """Full subject ID with assembly prefix."""
    result = _short_method_name(
        "System.Private.CoreLib/ConvertCharSubjects/ConvertCharSubjects::Subject_0:System.Void()"
    )
    assert result == "ConvertCharSubjects::Subject_0", f"got {result}"


def test_short_method_name_convert():
    """Convert.ToChar with assembly prefix."""
    result = _short_method_name(
        "System.Private.CoreLib/System.Convert::ToChar:System.Char(System.Object)"
    )
    assert result == "System.Convert::ToChar", f"got {result}"


def test_short_method_name_no_assembly():
    """Subject ID without assembly prefix."""
    result = _short_method_name("ConvertCharSubjects::Subject_0:System.Void()")
    assert result == "ConvertCharSubjects::Subject_0", f"got {result}"


def test_short_method_name_double_colon_only():
    """Input with only :: and no type info."""
    result = _short_method_name("Foo::Bar")
    assert result == "Foo::Bar", f"got {result}"


def test_short_method_name_no_match():
    """Input without :: returns empty string."""
    result = _short_method_name("JustAMethodName")
    assert result == "", f"got {result}"


def test_short_method_name_empty():
    """Empty input returns empty string."""
    result = _short_method_name("")
    assert result == "", f"got {result}"


def test_short_method_name_generic():
    """Subject ID with generic type parameters."""
    result = _short_method_name(
        "System.Private.CoreLib/System.Collections.Generic.List`1::Add:System.Void(!0)"
    )
    assert "List" in result and "::Add" in result, f"got {result}"


# ── _extract_metrics ────────────────────────────────────────────────

def _make_asm_compare_data(
    jit_ok: bool = True,
    aot_ok: bool = True,
    jit_instructions: int = 42,
    aot_instructions: int = 156,
    jit_size: int = 168,
    has_analysis: bool = True,
) -> dict:
    """Helper to build a mock asm-compare JSON output dict."""
    data: dict[str, Any] = {
        "tool": "asm-compare",
        "method": "ConvertCharSubjects::Subject_0",
        "assembly": "/path/to/Subjects.dll",
        "capturedAt": "2026-05-15T12:00:00Z",
        "jit": {
            "status": "ok" if jit_ok else "failed",
            "error": None if jit_ok else "JIT capture failed",
            "address": "0x7ffc12345678",
            "size": jit_size if jit_ok else 0,
            "instructions": jit_instructions if jit_ok else 0,
        },
        "aot": {
            "status": "ok" if aot_ok else "not_found",
            "symbol": "ConvertCharSubjects_Subject_0" if aot_ok else "",
            "methodId": "12345" if aot_ok else "",
            "subjectId": "ConvertCharSubjects::Subject_0" if aot_ok else "",
            "instructions": aot_instructions if aot_ok else 0,
            "locals": 2 if aot_ok else 0,
            "params": 1 if aot_ok else 0,
            "returnType": "System.Void" if aot_ok else "",
            "isStatic": True if aot_ok else False,
        } if aot_ok else None,
        "metrics": {
            "jitInstructionCount": jit_instructions if jit_ok else 0,
            "aotInstructionCount": aot_instructions if aot_ok else 0,
            "ratio": round(aot_instructions / jit_instructions, 2) if (jit_ok and jit_instructions > 0) else 0,
            "jitCodeSize": jit_size if jit_ok else 0,
            "aotApproxCodeSize": (aot_instructions if aot_ok else 0) * 8,
        },
    }

    if has_analysis:
        data["analysis"] = [
            {"severity": "info", "category": "EXTERNAL_RUNTIME_CALL",
             "message": "Call to 'System.Console::WriteLine' uses ExternalRuntime dispatch"},
            {"severity": "info", "category": "VIRTUAL_DISPATCH",
             "message": "1 virtual/interface dispatch call(s)"},
        ]

    return data


def test_extract_metrics_basic():
    """Basic metrics extraction with JIT and AOT data."""
    data = _make_asm_compare_data()
    entry = _extract_metrics(data, "ConvertCharSubjects::Subject_0", "ConvertCharSubjects::Subject_0")

    assert entry["methodSubjectId"] == "ConvertCharSubjects::Subject_0"
    assert entry["shortName"] == "ConvertCharSubjects::Subject_0"
    assert entry["metrics"]["jitInstructionCount"] == 42
    assert entry["metrics"]["aotInstructionCount"] == 156
    assert entry["metrics"]["ratio"] == round(156 / 42, 2)
    assert entry["metrics"]["jitCodeSize"] == 168
    assert entry["jit"]["status"] == "ok"
    assert entry["aot"]["status"] == "ok"
    assert entry["aot"]["symbol"] == "ConvertCharSubjects_Subject_0"


def test_extract_metrics_no_jit():
    """Metrics extraction when JIT capture failed."""
    data = _make_asm_compare_data(jit_ok=False)
    entry = _extract_metrics(data, "mid", "short")

    assert entry["jit"]["status"] == "failed"
    assert entry["metrics"]["jitInstructionCount"] == 0
    assert entry["metrics"]["ratio"] == 0


def test_extract_metrics_no_aot():
    """Metrics extraction when AOT method not found."""
    data = _make_asm_compare_data(aot_ok=False)
    entry = _extract_metrics(data, "mid", "short")

    assert entry["aot"]["status"] == "not_found"
    assert entry["aot"]["symbol"] == ""
    assert entry["metrics"]["aotInstructionCount"] == 0


def test_extract_metrics_analysis():
    """Analysis findings are extracted correctly."""
    data = _make_asm_compare_data(has_analysis=True)
    entry = _extract_metrics(data, "mid", "short")

    assert len(entry["analysis"]) == 2
    assert entry["analysis"][0]["category"] == "EXTERNAL_RUNTIME_CALL"
    assert entry["analysis"][1]["category"] == "VIRTUAL_DISPATCH"


def test_extract_metrics_no_analysis():
    """No analysis findings when analysis key is missing."""
    data = _make_asm_compare_data(has_analysis=False)
    entry = _extract_metrics(data, "mid", "short")

    assert entry["analysis"] == []


def test_extract_metrics_zero_jit_instructions():
    """Zero JIT instructions should not cause division errors."""
    data = _make_asm_compare_data(jit_instructions=0, aot_instructions=100)
    entry = _extract_metrics(data, "mid", "short")

    assert entry["metrics"]["ratio"] == 0
    assert entry["metrics"]["jitInstructionCount"] == 0


# ── _compute_summary ────────────────────────────────────────────────

def test_compute_summary_basic():
    """Basic summary computation with mixed results."""
    mids = ["m1", "m2", "m3"]
    jit_counts = [10, 20, 30]
    aot_counts = [50, 100, 150]

    method_results = [
        {"analysis": [{"category": "EXTERNAL_RUNTIME_CALL"}]},
        {"analysis": [{"category": "VIRTUAL_DISPATCH"}]},
        {"analysis": [{"category": "BOXING_OVERHEAD"}]},
    ]

    summary = _compute_summary(mids, jit_counts, aot_counts, 3, 0, method_results)

    assert summary["totalMethods"] == 3
    assert summary["okCount"] == 3
    assert summary["failedCount"] == 0
    assert summary["jitInstructionCount"]["min"] == 10
    assert summary["jitInstructionCount"]["max"] == 30
    assert summary["jitInstructionCount"]["avg"] == 20.0
    assert summary["jitInstructionCount"]["total"] == 60
    assert summary["aotInstructionCount"]["min"] == 50
    assert summary["aotInstructionCount"]["max"] == 150
    assert summary["overallIrExpansionRatio"] == round(300 / 60, 2)
    assert summary["totalExternalRuntimeCalls"] == 1
    assert summary["totalVirtualDispatches"] == 1
    assert summary["totalBoxingOps"] == 1


def test_compute_summary_empty():
    """Summary with no data."""
    summary = _compute_summary([], [], [], 0, 0, [])

    assert summary["totalMethods"] == 0
    assert summary["okCount"] == 0
    assert summary["failedCount"] == 0
    assert summary["overallIrExpansionRatio"] == 0


def test_compute_summary_no_jit():
    """Summary with no JIT data (all failed)."""
    mids = ["m1", "m2"]
    summary = _compute_summary(mids, [], [], 0, 2, [{}, {}])

    assert summary["totalMethods"] == 2
    assert summary["okCount"] == 0
    assert summary["failedCount"] == 2
    assert summary["overallIrExpansionRatio"] == 0


# ── _stat_range ─────────────────────────────────────────────────────

def test_stat_range_basic():
    """Basic stat range computation."""
    result = _stat_range([10, 20, 30, 40])
    assert result["min"] == 10
    assert result["max"] == 40
    assert result["avg"] == 25.0
    assert result["total"] == 100


def test_stat_range_single():
    """Stat range with single value."""
    result = _stat_range([42])
    assert result["min"] == 42
    assert result["max"] == 42
    assert result["avg"] == 42.0
    assert result["total"] == 42


def test_stat_range_empty():
    """Stat range with empty list."""
    result = _stat_range([])
    assert result["min"] == 0
    assert result["max"] == 0
    assert result["avg"] == 0
    assert result["total"] == 0


# ── _error_result ───────────────────────────────────────────────────

def test_error_result():
    """Error result helper returns correct structure."""
    result = _error_result("Something went wrong", Path("/tmp/report.json"))

    assert result["status"] == "skipped"
    assert result["summary"] == "Something went wrong"
    assert result["details"] == {}
    assert result["reportPath"].endswith("report.json")


# ── Integration-style: verify_family_asm_compare (mocked) ───────────

def test_verify_family_asm_compare_no_dll(monkeypatch):
    """When DLL is not found, returns skipped result."""
    import asm_compare_verifier as acv

    def mock_discover(*args, **kwargs):
        return None

    monkeypatch.setattr(acv, "_discover_subjects_dll", mock_discover)

    result = acv.verify_family_asm_compare("convert-char", "System.Private.CoreLib")
    assert result["status"] == "skipped"
    assert "DLL not found" in result["summary"]


def test_verify_family_asm_compare_no_contract(monkeypatch):
    """When contract has no methods, returns skipped result."""
    import asm_compare_verifier as acv

    def mock_discover(*args, **kwargs):
        return Path("/fake/Subjects.dll")

    def mock_contract(*args, **kwargs):
        return []

    monkeypatch.setattr(acv, "_discover_subjects_dll", mock_discover)
    monkeypatch.setattr(acv, "_load_contract_methods", mock_contract)

    result = acv.verify_family_asm_compare("convert-char", "System.Private.CoreLib")
    assert result["status"] == "skipped"
    assert "No methods" in result["summary"]


def test_verify_family_asm_compare_all_ok(monkeypatch, tmp_path):
    """All methods succeed — status should be passed."""
    import asm_compare_verifier as acv

    def mock_discover(*args, **kwargs):
        return Path("/fake/Subjects.dll")

    def mock_contract(*args, **kwargs):
        return [
            "System.Private.CoreLib/ConvertCharSubjects::Subject_0:System.Void()",
            "System.Private.CoreLib/ConvertCharSubjects::Subject_1:System.Void()",
        ]

    call_count = 0

    def mock_run_single(*args, **kwargs):
        nonlocal call_count
        call_count += 1
        return {
            "methodIndex": call_count - 1,
            "methodSubjectId": f"mid_{call_count - 1}",
            "shortName": f"Subject_{call_count - 1}",
            "status": "ok",
            "metrics": {
                "jitInstructionCount": 20 + call_count * 10,
                "aotInstructionCount": 100 + call_count * 50,
                "ratio": 5.0,
                "jitCodeSize": 80 + call_count * 40,
            },
            "jit": {"status": "ok", "size": 80, "instructions": 20},
            "aot": {"status": "ok", "symbol": "sym", "instructions": 100},
            "analysis": [],
        }

    monkeypatch.setattr(acv, "_discover_subjects_dll", mock_discover)
    monkeypatch.setattr(acv, "_load_contract_methods", mock_contract)
    monkeypatch.setattr(acv, "_run_single_asm_compare", mock_run_single)

    result = acv.verify_family_asm_compare("convert-char", "System.Private.CoreLib")
    assert result["status"] == "passed"
    assert result["details"]["okCount"] == 2
    assert result["details"]["totalMethods"] == 2
    assert "2/2 methods OK" in result["summary"]


def test_verify_family_asm_compare_partial_fail(monkeypatch, tmp_path):
    """Some methods fail — status depends on pass rate."""
    import asm_compare_verifier as acv

    def mock_discover(*args, **kwargs):
        return Path("/fake/Subjects.dll")

    def mock_contract(*args, **kwargs):
        return [
            "ConvertCharSubjects::Subject_0:System.Void()",
            "ConvertCharSubjects::Subject_1:System.Void()",
            "ConvertCharSubjects::Subject_2:System.Void()",
            "ConvertCharSubjects::Subject_3:System.Void()",
            "ConvertCharSubjects::Subject_4:System.Void()",
        ]

    call_count = [0]

    def mock_run_single(*args, **kwargs):
        call_count[0] += 1
        if call_count[0] <= 3:
            return {
                "methodIndex": call_count[0] - 1,
                "methodSubjectId": f"mid_{call_count[0] - 1}",
                "shortName": f"Subject_{call_count[0] - 1}",
                "status": "ok",
                "metrics": {"jitInstructionCount": 10, "aotInstructionCount": 50, "ratio": 5.0, "jitCodeSize": 40},
                "jit": {"status": "ok", "size": 40, "instructions": 10},
                "aot": {"status": "ok", "symbol": "sym", "instructions": 50},
                "analysis": [],
            }
        else:
            return {
                "methodIndex": call_count[0] - 1,
                "methodSubjectId": f"mid_{call_count[0] - 1}",
                "shortName": f"Subject_{call_count[0] - 1}",
                "status": "error",
                "error": "timed out",
            }

    monkeypatch.setattr(acv, "_discover_subjects_dll", mock_discover)
    monkeypatch.setattr(acv, "_load_contract_methods", mock_contract)
    monkeypatch.setattr(acv, "_run_single_asm_compare", mock_run_single)

    result = acv.verify_family_asm_compare("convert-char", "System.Private.CoreLib")
    # 3/5 = 60% < 80% → failed
    assert result["status"] == "failed"
    assert result["details"]["okCount"] == 3
    assert result["details"]["failedCount"] == 2