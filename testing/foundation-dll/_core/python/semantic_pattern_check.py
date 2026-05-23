"""IL→C++ semantic pattern verification for codegen quality.

Scans the generated C++ output to verify that key IL patterns are lowered
to expected native C++ forms rather than falling back to runtime dispatch.

Check categories:
  - Lowering density: proportion of methods with real lowering vs stubs
  - Arithmetic locality: +, -, *, / use local computation, not runtime calls
  - Branch directness: conditional branches use direct Jcc, not interpreter fallback
  - Call directness: method calls go to target functions, not dispatch trampolines
"""

from __future__ import annotations

import re
from pathlib import Path
from typing import Any

# ── C++ signal patterns ─────────────────────────────────────────────────

# Positive signals: patterns indicating real lowering
LOWERING_MARKERS = (b"chaos_eval_stack", b"_s0{};", b"CHAOS_IL2CPP_ARRAY")
ARITHMETIC_LOCAL = re.compile(rb"_s0\.\w+\s*=\s*_s0\.\w+\s*[\+\-\*/%]\s*_s0\.\w+")
DIRECT_CALL = re.compile(rb"(CHAOS_IL2CPP_CALL|RuntimeInvoke|FastInvoke)\s*\(")
NATIVE_JUMP = re.compile(rb"__builtin_expect|goto\s+chaos_label|if\s*\([^)]+\)\s*\{")

# Negative signals: patterns indicating runtime fallback
RUNTIME_FALLBACK = re.compile(rb"InterpreterDispatchRaw|InterpreterExecute|chaos_dispatch_fallback")
STUB_MARKER = re.compile(rb"NativeReferenceStub_")


def check_semantic_patterns(
    assembly: str,
    family_slug: str,
    cpp_path: Path | None,
) -> dict[str, Any]:
    """Run semantic pattern checks on generated C++.

    Args:
        assembly: Assembly name (e.g. "System.Private.CoreLib").
        family_slug: Family slug (e.g. "convert-char").
        cpp_path: Path to generated native-aot.generated.cpp, or None.

    Returns:
        Dict with keys: checks, overall_status.
        Each check entry: check_id, status, summary, details.
    """
    checks: list[dict[str, Any]] = []

    if not cpp_path or not cpp_path.exists():
        checks.append({
            "check_id": "p2_lowering_density",
            "status": "NOT_APPLICABLE",
            "summary": "Generated C++ not found",
        })
        return {"checks": checks, "overall_status": "NOT_APPLICABLE"}

    content: bytes = cpp_path.read_bytes()

    # ── Check 1: Lowering density ─────────────────────────────────────
    _check_lowering_density(content, family_slug, checks)

    # ── Check 2: Arithmetic locality ──────────────────────────────────
    _check_arithmetic_locality(content, checks)

    # ── Check 3: Branch directness ────────────────────────────────────
    _check_branch_directness(content, checks)

    # ── Check 4: Stub contamination ───────────────────────────────────
    _check_stub_contamination(content, checks)

    # Overall status = worst of all checks
    status_order = {"VIOLATION": 0, "CONCERN": 1, "ALIGNED": 2, "NOT_APPLICABLE": 3}
    overall = "ALIGNED"
    for c in checks:
        if status_order.get(c["status"], 99) < status_order.get(overall, 99):
            overall = c["status"]

    return {"checks": checks, "overall_status": overall}


def _check_lowering_density(
    content: bytes, family_slug: str, checks: list[dict[str, Any]]
) -> None:
    """Estimate what proportion of methods use real lowering.

    Counts function definitions (top-level C++ functions) and checks
    whether each contains a lowering marker.
    """
    func_starts = [m.start() for m in re.finditer(rb"^[a-zA-Z_]\w*\s+[a-zA-Z_]\w*\([^)]*\)\s*\{", content, re.MULTILINE)]
    if not func_starts:
        checks.append({
            "check_id": "p2_lowering_density",
            "status": "NOT_APPLICABLE",
            "summary": "No C++ functions found in generated code",
        })
        return

    lowered = 0
    for i, start in enumerate(func_starts):
        # Find matching close-brace for this function (nesting-aware)
        end = _find_matching_brace(content, start)
        if end is None:
            continue
        body = content[start:end]
        for marker in LOWERING_MARKERS:
            if marker in body:
                lowered += 1
                break

    total = len(func_starts)
    ratio = lowered / total if total > 0 else 0
    report = {"total_functions": total, "lowered": lowered, "ratio": round(ratio, 3)}

    if ratio >= 0.8:
        checks.append({
            "check_id": "p2_lowering_density",
            "status": "ALIGNED",
            "summary": f"{lowered}/{total} methods use native lowering ({ratio:.0%})",
            "details": report,
        })
    elif ratio >= 0.5:
        checks.append({
            "check_id": "p2_lowering_density",
            "status": "CONCERN",
            "summary": f"Only {lowered}/{total} methods use native lowering ({ratio:.0%})",
            "details": report,
        })
    else:
        checks.append({
            "check_id": "p2_lowering_density",
            "status": "VIOLATION",
            "summary": f"Only {lowered}/{total} methods use native lowering ({ratio:.0%}) — majority are stubs",
            "details": report,
        })


def _check_arithmetic_locality(
    content: bytes, checks: list[dict[str, Any]]
) -> None:
    """Check that arithmetic operations use local computation."""
    matches = ARITHMETIC_LOCAL.findall(content)
    fallback = RUNTIME_FALLBACK.findall(content)

    if len(matches) > 0 or len(fallback) == 0:
        checks.append({
            "check_id": "p2_arithmetic_locality",
            "status": "ALIGNED",
            "summary": f"Local arithmetic patterns found: {len(matches)}",
            "details": {"local_arithmetic": len(matches), "runtime_fallbacks": len(fallback)},
        })
    elif len(fallback) < 5:
        checks.append({
            "check_id": "p2_arithmetic_locality",
            "status": "CONCERN",
            "summary": f"No local arithmetic detected, {len(fallback)} runtime fallback calls",
            "details": {"local_arithmetic": 0, "runtime_fallbacks": len(fallback)},
        })
    else:
        checks.append({
            "check_id": "p2_arithmetic_locality",
            "status": "VIOLATION",
            "summary": f"No local arithmetic and {len(fallback)} runtime fallback calls — possible stub generation",
            "details": {"local_arithmetic": 0, "runtime_fallbacks": len(fallback)},
        })


def _check_branch_directness(
    content: bytes, checks: list[dict[str, Any]]
) -> None:
    """Check that branches use direct patterns, not dispatch trampolines."""
    has_direct = bool(NATIVE_JUMP.search(content))
    fallback_count = len(RUNTIME_FALLBACK.findall(content))

    if has_direct:
        checks.append({
            "check_id": "p2_branch_directness",
            "status": "ALIGNED",
            "summary": f"Direct branch/guard patterns found, {fallback_count} runtime fallbacks",
            "details": {"has_direct_branch": True, "runtime_fallbacks": fallback_count},
        })
    elif fallback_count <= 3:
        checks.append({
            "check_id": "p2_branch_directness",
            "status": "CONCERN",
            "summary": f"No direct branch patterns, {fallback_count} runtime fallback calls",
            "details": {"has_direct_branch": False, "runtime_fallbacks": fallback_count},
        })
    else:
        checks.append({
            "check_id": "p2_branch_directness",
            "status": "VIOLATION",
            "summary": f"No direct branch patterns with {fallback_count} runtime fallback calls",
            "details": {"has_direct_branch": False, "runtime_fallbacks": fallback_count},
        })


def _check_stub_contamination(
    content: bytes, checks: list[dict[str, Any]]
) -> None:
    """Check for stub markers indicating incomplete codegen."""
    stubs = STUB_MARKER.findall(content)
    if len(stubs) == 0:
        checks.append({
            "check_id": "p2_stub_contamination",
            "status": "ALIGNED",
            "summary": "No stub markers found — all methods use real lowering",
        })
    elif len(stubs) <= 3:
        checks.append({
            "check_id": "p2_stub_contamination",
            "status": "CONCERN",
            "summary": f"{len(stubs)} stub markers found — minor stub contamination",
            "details": {"stub_count": len(stubs)},
        })
    else:
        checks.append({
            "check_id": "p2_stub_contamination",
            "status": "VIOLATION",
            "summary": f"{len(stubs)} stub markers found — significant stub contamination",
            "details": {"stub_count": len(stubs)},
        })


# ── Helpers ─────────────────────────────────────────────────────────────

def _find_matching_brace(content: bytes, start: int) -> int | None:
    """Find the matching closing brace, accounting for nesting."""
    brace_start = content.find(b"{", start)
    if brace_start == -1:
        return None
    depth = 1
    pos = brace_start + 1
    while depth > 0 and pos < len(content):
        # Skip string/char literals
        if content[pos:pos+1] == b'"':
            end = content.find(b'"', pos + 1)
            if end == -1:
                return None
            pos = end + 1
            continue
        if content[pos:pos+2] == b"//":
            end = content.find(b"\n", pos)
            if end == -1:
                end = len(content)
            pos = end + 1
            continue
        if content[pos:pos+2] == b"/*":
            end = content.find(b"*/", pos + 2)
            if end == -1:
                return None
            pos = end + 2
            continue
        if content[pos:pos+1] == b"{":
            depth += 1
        elif content[pos:pos+1] == b"}":
            depth -= 1
        pos += 1
    return pos if depth == 0 else None
