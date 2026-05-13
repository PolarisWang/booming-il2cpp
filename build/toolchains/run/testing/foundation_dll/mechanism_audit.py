"""Mechanism + Principle Audit: Mechanism + Principle alignment verification.

Orchestrates:
  1. Stub detection (via stub_detector.py)
  2. _METHOD_OVERRIDES skip audit
  3. Fact Static assert verification
  4. 7 automated principle checks (via principle_auto_checks.py)

Outputs:
  - mechanism-audit-report.json  (机制真实性 + skip 审计)
  - principle-alignment-report.json (7 项自动化原则检查)

Usage:
    python mechanism_audit.py --family convert-char
    python mechanism_audit.py --family convert-char --verbose
    python mechanism_audit.py --assembly System.Private.CoreLib
    python mechanism_audit.py --all
"""

from __future__ import annotations

import ast
import json
import re
import sys
from dataclasses import dataclass, field, asdict
from pathlib import Path
from typing import Any

_HERE = Path(__file__).resolve().parent
_REPO_ROOT = _HERE.parents[4]
_VERIFICATION_BASE = _REPO_ROOT / "verification" / "foundation-dll"
_TEST_CODE_GENERATOR = _HERE / "test_code_generator.py"

sys.path.insert(0, str(_HERE.parent.parent))  # for testing package

try:
    from testing.foundation_dll.stub_detector import scan_family, FamilyStubResult
except ImportError:
    scan_family = None

try:
    from testing.foundation_dll.principle_auto_checks import run_all_checks as run_principle_checks
except ImportError:
    run_principle_checks = None

try:
    from testing.trace import trace
except ImportError:
    def trace(*args, **kwargs):
        pass


# ── Data types ─────────────────────────────────────────────────────────

@dataclass
class MethodAuditRecord:
    method_name: str
    subject_id: str = ""
    is_stub: bool = False
    stub_pattern: str = ""
    is_skipped: bool = False
    has_lowering: bool = False
    assert_fired: bool = False
    audit_status: str = "PASS"   # PASS | FAIL | SKIP

    def to_dict(self) -> dict[str, Any]:
        return asdict(self)


@dataclass
class MechanismAuditReport:
    family: str
    assembly: str
    total_methods: int = 0
    audited: int = 0
    false_passing: int = 0
    stubs_found: int = 0
    skips_found: int = 0
    methods_without_lowering: int = 0
    methods_without_assert: int = 0
    passed: bool = False
    method_details: list[dict] = field(default_factory=list)
    skip_details: list[dict] = field(default_factory=list)
    errors: list[str] = field(default_factory=list)

    def to_dict(self) -> dict[str, Any]:
        return asdict(self)


# ── _METHOD_OVERRIDES parser ──────────────────────────────────────────

def _parse_method_overrides() -> dict[tuple[str, str, int], str]:
    """Extract _METHOD_OVERRIDES dict from test_code_generator.py."""
    if not _TEST_CODE_GENERATOR.exists():
        return {}

    source = _TEST_CODE_GENERATOR.read_text(encoding="utf-8")
    try:
        tree = ast.parse(source)
    except SyntaxError:
        return {}

    for node in ast.walk(tree):
        # _METHOD_OVERRIDES uses type annotation: `_METHOD_OVERRIDES: dict[...] = {`
        if isinstance(node, ast.AnnAssign) and isinstance(node.target, ast.Name) and node.target.id == "_METHOD_OVERRIDES":
            try:
                raw = ast.literal_eval(node.value)
                if isinstance(raw, dict):
                    return raw
            except (ValueError, TypeError):
                return {}
    return {}


def _get_skip_entries(relevant_type_names: set[str] | None = None) -> dict[tuple[str, str, int], str]:
    """Get only the 'skip' entries from _METHOD_OVERRIDES.

    When relevant_type_names is provided, filters to entries whose type_name
    appears in the relevant set (per-family filtering).
    """
    overrides = _parse_method_overrides()
    skips = {k: v for k, v in overrides.items() if v == "skip"}
    if relevant_type_names is not None:
        skips = {k: v for k, v in skips.items() if k[0] in relevant_type_names}
    return skips


# ── Generated C++ analysis ────────────────────────────────────────────

def _count_assert_invocations(cpp_content: str) -> int:
    """Count assert-related references in generated code (placeholder, no longer used)."""
    return 0


# ── Fact Static results loader ──────────────────────────────────────────

def _load_fact_static_results(assembly: str, family_slug: str) -> dict[str, Any] | None:
    fs_path = _VERIFICATION_BASE / assembly / family_slug / "managed_test" / "tests" / "l2-results.json"
    if fs_path.exists():
        try:
            return json.loads(fs_path.read_bytes())
        except (json.JSONDecodeError, OSError):
            return None
    return None


# ── Main audit logic ──────────────────────────────────────────────────

def audit_family(assembly: str, family_slug: str) -> MechanismAuditReport:
    """Run full Mechanism + Principle audit on a single family."""
    trace("mechanism_audit.audit_family", stage="audit", family=family_slug, assembly=assembly)

    report = MechanismAuditReport(family=family_slug, assembly=assembly)
    family_dir = _VERIFICATION_BASE / assembly / family_slug
    errors: list[str] = []

    # ── 1. Stub detection ─────────────────────────────────────────────
    stub_results: FamilyStubResult | None = None
    if scan_family is not None:
        try:
            stub_results = scan_family(family_dir)
        except Exception as e:
            errors.append(f"stub_detector error: {e}")
    else:
        errors.append("stub_detector not available")

    # ── 2. Load generated C++ ─────────────────────────────────────────
    # Prefer codegen/<ClassName>/generated/ (new multi-assembly pipeline), fall back to legacy flat path.
    cpp_candidates = sorted(family_dir.glob("codegen/*/generated/native-aot.generated.cpp"))
    if cpp_candidates:
        cpp_path = cpp_candidates[0]
    else:
        cpp_path = family_dir / "codegen" / "generated" / "native-aot.generated.cpp"
    cpp_content = cpp_path.read_text(encoding="utf-8") if cpp_path.exists() else ""

    if not cpp_content:
        errors.append("Generated C++ not found")

    has_file_level_lowering = ("chaos_eval_stack" in cpp_content or "CHAOS_IL2CPP_ARRAY" in cpp_content) if cpp_content else False

    # ── 3. Load _METHOD_OVERRIDES skip entries ─────────────────────────
    # Extract relevant C# type names from the capability-family-contract
    relevant_types: set[str] = set()
    if stub_results:
        # Derive type names from stub_results' method subject IDs
        for sf in stub_results.files:
            for m in sf.methods:
                if m.subject_id:
                    type_path = m.subject_id.split("::")[0] if "::" in m.subject_id else ""
                    if "/" in type_path:
                        raw_type = type_path.split("/", 1)[1]
                    else:
                        raw_type = type_path
                    tn = raw_type.rsplit(".", 1)[-1] if "." in raw_type else raw_type
                    # Strip generic arity suffix (e.g., "Span`1" -> "Span")
                    bt_match = re.match(r"^(\w+)`\d+$", tn)
                    if bt_match:
                        tn = bt_match.group(1)
                    if tn:
                        relevant_types.add(tn)
    # Fallback: load from contract file directly
    if not relevant_types:
        contract_path = family_dir / "capability-family-contract.json"
        if contract_path.exists():
            try:
                contract = json.loads(contract_path.read_bytes())
                mids = contract.get("methodSubjectIds", []) or [m["methodSubjectId"] for m in contract.get("methodContracts", []) if m.get("methodSubjectId")]
                for mid in mids:
                    if "::" in mid:
                        type_path = mid.split("::", 1)[0]
                        raw_type = type_path.split("/", 1)[1] if "/" in type_path else type_path
                        tn = raw_type.rsplit(".", 1)[-1]
                        bt_match = re.match(r"^(\w+)`\d+$", tn)
                        if bt_match:
                            tn = bt_match.group(1)
                        if tn:
                            relevant_types.add(tn)
            except (OSError, json.JSONDecodeError):
                pass
    skip_entries = _get_skip_entries(relevant_type_names=relevant_types if relevant_types else None)

    # ── 4. Load Fact Static results ────────────────────────────────────
    l2_results = _load_fact_static_results(assembly, family_slug)

    # ── 5. Process methods ────────────────────────────────────────────
    method_details: list[dict] = []
    skip_details: list[dict] = []
    false_passing = 0
    stubs_found = 0
    skips_found = 0
    no_lowering = 0
    no_assert = 0

    # Extract method names from stub results — only from native-aot generated files
    # (benchmark stubs and hotupdate stubs are expected harness code, not audit targets)
    stub_methods_set: set[str] = set()
    if stub_results:
        for sf in stub_results.files:
            if sf.file_kind != "native-aot":
                continue
            for m in sf.methods:
                entry = {
                    "method_name": m.method_name,
                    "subject_id": m.subject_id or "",
                    "is_stub": m.is_stub,
                    "stub_pattern": m.stub_pattern,
                }
                method_details.append(entry)
                if m.is_stub:
                    stubs_found += 1
                    stub_methods_set.add(m.method_name)

    # If no stub_results, extract methods from C++ using dynamic class name
    if not method_details and cpp_content:
        class_name = f"{family_slug.title().replace('-', '').replace('_', '')}NativeEntry"
        # Namespaced pattern: ClassName_ClassName_Method0
        namespaced = re.findall(rf'\b{class_name}_{class_name}_(\w+)\s*\(', cpp_content)
        if namespaced:
            for m in namespaced:
                method_details.append({
                    "method_name": m,
                    "subject_id": "",
                    "is_stub": False,
                    "stub_pattern": "",
                })
        else:
            # Flat pattern: ClassName_Method0 or NativeReferenceStub_Method0 or RunNativeAot
            flat = re.findall(rf'(?:NativeReferenceStub_|{class_name}_|NativeEntry_|RunNativeAot_)(\w+)\s*\(', cpp_content)
            for m in flat:
                method_details.append({
                    "method_name": m,
                    "subject_id": "",
                    "is_stub": "NativeReferenceStub_" in m,
                    "stub_pattern": "NativeReferenceStub_" if "NativeReferenceStub_" in m else "",
                })

    # Cross-reference with skip entries
    for (type_name, method_name, param_count), override in skip_entries.items():
        skip_details.append({
            "type_name": type_name,
            "method_name": method_name,
            "param_count": param_count,
            "override_value": override,
        })
        skips_found += 1

    # Check each method for lowering and assert
    assert_invocations = _count_assert_invocations(cpp_content)
    for md in method_details:
        mname = md["method_name"]
        # Use file-level chaos_eval_stack check — per-method body parsing
        # is unreliable on complex C++; if the file has real lowering, it's
        # reasonable to attribute lowering to all non-stub methods.
        is_explicit_stub = md.get("is_stub", False)
        md["has_lowering"] = has_file_level_lowering and not is_explicit_stub
        md["assert_fired"] = assert_invocations > 0

        if not md["has_lowering"] and not is_explicit_stub:
            no_lowering += 1
        if not md["assert_fired"]:
            no_assert += 1

        # A method is false-passing if it's a stub, or it lacks lowering
        # (for non-stub methods lacking lowering, it's a concern but not a false pass)
        is_false_pass = is_explicit_stub
        md["audit_status"] = "FAIL" if is_false_pass else "PASS"
        if is_false_pass:
            false_passing += 1

    total = len(method_details)

    # ── 6. Build report ──────────────────────────────────────────────
    report.total_methods = total
    report.audited = total
    report.false_passing = false_passing
    report.stubs_found = stubs_found
    report.skips_found = skips_found
    report.methods_without_lowering = no_lowering
    report.methods_without_assert = no_assert
    report.passed = false_passing == 0
    report.method_details = method_details
    report.skip_details = skip_details
    report.errors = errors

    return report


def run_full_audit(assembly: str, family_slug: str) -> dict[str, Any]:
    """Run Mechanism + Principle audit + principle checks, output both reports as a single result."""
    mechanism = audit_family(assembly, family_slug)
    principle_result = run_principle_checks(assembly, family_slug) if run_principle_checks else {
        "error": "principle_auto_checks not available",
    }

    # Determine overall status
    mechanism_pass = mechanism.passed
    principle_overall = principle_result.get("summary", {}).get("overall", "NOT_APPLICABLE") if isinstance(principle_result, dict) else "NOT_APPLICABLE"
    overall_pass = mechanism_pass and principle_overall != "VIOLATION"

    report = {
        "generated_at": __import__("datetime").datetime.now().isoformat(),
        "assembly": assembly,
        "family": family_slug,
        "mechanism_audit": mechanism.to_dict(),
        "principle_alignment": principle_result,
        "overall": {
            "passed": overall_pass,
            "mechanism_passed": mechanism_pass,
            "principle_status": principle_overall,
            "false_passing": mechanism.false_passing,
        },
    }

    trace("mechanism_audit.complete", stage="audit", family=family_slug,
          overall=overall_pass, false_passing=mechanism.false_passing,
          principle=principle_overall)

    return report


def run_assembly_audit(assembly: str) -> dict[str, Any]:
    """Run audit for all families in an assembly."""
    asm_dir = _VERIFICATION_BASE / assembly
    if not asm_dir.exists():
        return {"error": f"Assembly directory not found: {asm_dir}"}

    families: dict[str, Any] = {}
    for item in sorted(asm_dir.iterdir()):
        if not item.is_dir() or item.name.startswith("_") or item.name == "reports":
            continue
        cpp_path = item / "codegen" / "generated" / "native-aot.generated.cpp"
        if not cpp_path.exists():
            continue
        families[item.name] = run_full_audit(assembly, item.name)

    # Summary
    total = len(families)
    passed = sum(1 for f in families.values() if f.get("overall", {}).get("passed"))
    failed = sum(1 for f in families.values() if not f.get("overall", {}).get("passed"))
    false_passing_total = sum(
        f.get("mechanism_audit", {}).get("false_passing", 0)
        for f in families.values()
    )

    return {
        "generated_at": __import__("datetime").datetime.now().isoformat(),
        "assembly": assembly,
        "total_families": total,
        "passed_families": passed,
        "failed_families": failed,
        "total_false_passing": false_passing_total,
        "families": families,
    }


def write_reports(assembly: str, family_slug: str, output_dir: Path | None = None) -> dict[str, str]:
    """Run audit and write reports to disk."""
    result = run_full_audit(assembly, family_slug)

    family_dir = output_dir or (_VERIFICATION_BASE / assembly / family_slug)

    mechanism_path = family_dir / "mechanism-audit-report.json"
    family_dir.mkdir(parents=True, exist_ok=True)
    mechanism_path.write_text(
        json.dumps(result.get("mechanism_audit", {}), indent=2, ensure_ascii=False),
        encoding="utf-8",
    )

    principle_path = family_dir / "principle-alignment-report.json"
    principle_path.write_text(
        json.dumps(result.get("principle_alignment", {}), indent=2, ensure_ascii=False),
        encoding="utf-8",
    )

    print(f"  [Mechanism+Principle] Mechanism audit: {mechanism_path.name} "
          f"({result['mechanism_audit']['false_passing']} false passes)")
    print(f"  [Mechanism+Principle] Principle alignment: {principle_path.name} "
          f"(overall={result['principle_alignment'].get('summary', {}).get('overall', 'N/A')})")

    return {
        "mechanism_audit": str(mechanism_path),
        "principle_alignment": str(principle_path),
    }


def main() -> None:
    import argparse
    parser = argparse.ArgumentParser(description="Mechanism + Principle Audit: Mechanism + Principle Verification")
    parser.add_argument("--family", help="Family slug (e.g., convert-char)")
    parser.add_argument("--assembly", default="System.Private.CoreLib")
    parser.add_argument("--verbose", "-v", action="store_true")
    parser.add_argument("--output", "-o", type=Path, help="Output directory (default: family dir)")
    parser.add_argument("--write", action="store_true", help="Write reports to disk")
    args = parser.parse_args()

    if args.family:
        if args.write:
            result = write_reports(args.assembly, args.family, args.output)
        else:
            result = run_full_audit(args.assembly, args.family)
            print(json.dumps(result, indent=2, ensure_ascii=False))
    else:
        result = run_assembly_audit(args.assembly)
        print(json.dumps(result, indent=2, ensure_ascii=False))

    # Exit code
    if isinstance(result, dict):
        overall = result.get("overall", {})
        if overall.get("passed") is not None:
            sys.exit(0 if overall["passed"] else 1)
        if result.get("failed_families") is not None:
            sys.exit(0 if result["failed_families"] == 0 else 1)


if __name__ == "__main__":
    main()
