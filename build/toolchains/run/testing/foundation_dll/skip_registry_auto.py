"""Skip Registry Auto-Generator.

Scans _METHOD_OVERRIDES in test_code_generator.py, extracts all "skip"
entries, and generates/validates skip-registry.json.

Usage:
    python skip_registry_auto.py --generate          # auto-generate registry from _METHOD_OVERRIDES
    python skip_registry_auto.py --validate          # validate existing registry
    python skip_registry_auto.py --report            # print summary report
    python skip_registry_auto.py --generate --output path/to/skip-registry.json
"""

from __future__ import annotations

import ast
import json
import sys
from datetime import date, datetime, timedelta
from pathlib import Path
from typing import Any

_HERE = Path(__file__).resolve().parent
_REPO_ROOT = _HERE.parents[4]
_VERIFICATION_BASE = _REPO_ROOT / "verification" / "foundation-dll"
_TEST_CODE_GENERATOR = _HERE / "test_code_generator.py"

DEFAULT_SKIP_REGISTRY_PATH = _VERIFICATION_BASE.parent / "projections" / "foundation-dll-audit" / "skip-registry.json"

try:
    from testing.trace import trace
except ImportError:
    def trace(*args, **kwargs):
        pass


# ── Parser ─────────────────────────────────────────────────────────────

def _parse_method_overrides() -> dict[tuple[str, str, int], str]:
    """Extract _METHOD_OVERRIDES dict from test_code_generator.py."""
    if not _TEST_CODE_GENERATOR.exists():
        print(f"ERROR: {_TEST_CODE_GENERATOR} not found")
        return {}

    source = _TEST_CODE_GENERATOR.read_text(encoding="utf-8")
    try:
        tree = ast.parse(source)
    except SyntaxError as e:
        print(f"ERROR: Failed to parse {_TEST_CODE_GENERATOR.name}: {e}")
        return {}

    for node in ast.walk(tree):
        # _METHOD_OVERRIDES uses type annotation: `_METHOD_OVERRIDES: dict[...] = {`
        # so it's stored as AnnAssign, not Assign
        if isinstance(node, ast.AnnAssign) and isinstance(node.target, ast.Name) and node.target.id == "_METHOD_OVERRIDES":
            try:
                raw = ast.literal_eval(node.value)
                if isinstance(raw, dict):
                    # Normalize keys: ast.literal_eval may return tuple with int or str
                    normalized = {}
                    for k, v in raw.items():
                        if isinstance(k, tuple) and len(k) == 3:
                            normalized[(str(k[0]), str(k[1]), int(k[2]))] = str(v) if v is not None else ""
                    return normalized
            except (ValueError, TypeError) as e:
                print(f"ERROR: Failed to literal_eval _METHOD_OVERRIDES: {e}")
                return {}
    return {}


# ── Skip classification ───────────────────────────────────────────────

SKIP_CATEGORIES = {
    "out_ref_param": ["out", "ref", "TryParse", "TryFormat"],
    "generic_unresolvable": ["GetSubArray", "Resize", "Empty", "ToArray"],
    "parse_method": ["Parse", "ToByte", "ToInt32", "ToSingle", "ToDouble", "ToBoolean", "ToDecimal"],
    "internal_intrinsic": ["Memmove", "BulkMoveWithWriteBarrier", "GetByte", "SetByte", "MemoryBarrier"],
    "thread_operation": ["Start", "Join", "Sleep", "Enter", "Exit", "Monitor", "Thread", "Interlocked"],
    "delegate_operation": ["Combine", "Remove", "Invoke", "CreateDelegate", "GetInvocationList"],
    "enum_operation": ["GetName", "GetNames", "GetValues", "IsDefined", "ToObject", "GetUnderlyingType"],
    "collection_method": ["Remove", "ContainsKey", "Contains", "Add", "Clear", "IndexOf", "BinarySearch"],
    "reflection": ["GetCustomAttribute", "GetCustomAttributes", "IsDefined", "GetType", "GetMethod", "GetField", "GetProperty", "Invoke", "InvokeMember"],
    "task_async": ["Wait", "Result", "Start", "Run", "ContinueWith", "FromResult", "WhenAll"],
}

TRACKING_ISSUES = {
    "out_ref_param": "https://github.com/chaos-il2cpp/chaos/issues/out-ref-param-generation",
    "generic_unresolvable": "https://github.com/chaos-il2cpp/chaos/issues/generic-resolver",
    "parse_method": "https://github.com/chaos-il2cpp/chaos/issues/parse-input-generation",
    "internal_intrinsic": "https://github.com/chaos-il2cpp/chaos/issues/intrinsic-codegen",
    "thread_operation": "https://github.com/chaos-il2cpp/chaos/issues/thread-sync-stubs",
    "delegate_operation": "https://github.com/chaos-il2cpp/chaos/issues/delegate-stubs",
    "enum_operation": "https://github.com/chaos-il2cpp/chaos/issues/enum-reflection",
    "collection_method": "https://github.com/chaos-il2cpp/chaos/issues/collection-entry-gen",
    "reflection": "https://github.com/chaos-il2cpp/chaos/issues/reflection-stubs",
    "task_async": "https://github.com/chaos-il2cpp/chaos/issues/task-async-stubs",
}


def _classify_skip(type_name: str, method_name: str) -> str:
    """Classify a skip entry into a category for the registry."""
    for category, patterns in SKIP_CATEGORIES.items():
        for p in patterns:
            if p.lower() in method_name.lower() or p.lower() in type_name.lower():
                return category
    return "other"


def _default_review_date() -> str:
    """Default review date: 3 months from now."""
    return (date.today() + timedelta(days=90)).isoformat()


def generate_registry() -> dict[str, Any]:
    """Generate skip-registry.json from _METHOD_OVERRIDES."""
    overrides = _parse_method_overrides()
    skip_entries = {k: v for k, v in overrides.items() if v == "skip"}

    exceptions = []
    for (type_name, method_name, param_count), _ in sorted(skip_entries.items()):
        category = _classify_skip(type_name, method_name)
        subject_id = f"System.Private.CoreLib/{type_name}::{method_name}(...)"
        tracking = TRACKING_ISSUES.get(category, "https://github.com/chaos-il2cpp/chaos/issues/new")
        elimination_plan = f"Phase 3/4: {category.replace('_', ' ')} — see {tracking}"

        exceptions.append({
            "methodSubjectId": subject_id,
            "typeName": type_name,
            "methodName": method_name,
            "paramCount": param_count,
            "category": category,
            "skipReason": f"_METHOD_OVERRIDES: {category} — auto-generated entry",
            "eliminationPlan": elimination_plan,
            "trackingTicket": tracking,
            "reviewDate": _default_review_date(),
            "autoGenerated": True,
        })

    registry = {
        "generatedAt": datetime.now().isoformat(),
        "source": f"_METHOD_OVERRIDES in {_TEST_CODE_GENERATOR.name}",
        "totalSkipEntries": len(skip_entries),
        "totalNonSkipOverrides": len(overrides) - len(skip_entries),
        "categories": {
            cat: sum(1 for e in exceptions if e["category"] == cat)
            for cat in sorted(set(e["category"] for e in exceptions))
        },
        "exceptions": exceptions,
    }

    trace("skip_registry.generate", stage="audit", total=len(exceptions))
    return registry


def validate_registry(registry_path: Path | str | None = None) -> dict[str, Any]:
    """Validate an existing skip registry against current _METHOD_OVERRIDES."""
    path = Path(registry_path) if registry_path else DEFAULT_SKIP_REGISTRY_PATH
    if not path.exists():
        return {"error": f"Skip registry not found: {path}", "valid": False}

    try:
        registry = json.loads(path.read_bytes())
    except (json.JSONDecodeError, OSError) as e:
        return {"error": f"Cannot read registry: {e}", "valid": False}

    current_skips = set(k for k, v in _parse_method_overrides().items() if v == "skip")
    registered = set()
    for entry in registry.get("exceptions", []):
        registered.add((entry.get("typeName", ""), entry.get("methodName", ""), entry.get("paramCount", 0)))

    unregistered = current_skips - registered
    stale = registered - current_skips
    expired = []
    today = date.today()
    for entry in registry.get("exceptions", []):
        rd = entry.get("reviewDate", "")
        if rd:
            try:
                if date.fromisoformat(rd) < today:
                    expired.append(entry["methodSubjectId"])
            except ValueError:
                pass

    valid = len(unregistered) == 0 and len(expired) == 0

    return {
        "valid": valid,
        "totalInRegistry": len(registered),
        "totalInCode": len(current_skips),
        "unregisteredSkips": sorted(unregistered),
        "staleRegistryEntries": sorted(stale),
        "expiredEntries": expired,
        "expiredCount": len(expired),
        "unregisteredCount": len(unregistered),
    }


def report() -> dict[str, Any]:
    """Generate a human-readable summary report."""
    overrides = _parse_method_overrides()
    total = len(overrides)
    skip_count = sum(1 for v in overrides.values() if v == "skip")
    custom_count = total - skip_count

    categories = {}
    for (type_name, method_name, _), val in overrides.items():
        if val == "skip":
            cat = _classify_skip(type_name, method_name)
            categories[cat] = categories.get(cat, 0) + 1

    return {
        "total_overrides": total,
        "skip_entries": skip_count,
        "custom_entries": custom_count,
        "categories": categories,
    }


def main() -> None:
    import argparse
    parser = argparse.ArgumentParser(description="Skip Registry Auto-Generator")
    parser.add_argument("--generate", action="store_true", help="Generate skip-registry.json")
    parser.add_argument("--validate", action="store_true", help="Validate existing registry")
    parser.add_argument("--report", action="store_true", help="Summary report")
    parser.add_argument("-o", "--output", type=Path, help="Output path for generated registry")

    args = parser.parse_args()

    if args.generate:
        registry = generate_registry()
        output_path = args.output or DEFAULT_SKIP_REGISTRY_PATH
        output_path.parent.mkdir(parents=True, exist_ok=True)
        output_path.write_text(json.dumps(registry, indent=2, ensure_ascii=False), encoding="utf-8")
        print(f"Skip registry written to: {output_path}")
        print(f"  Total skip entries: {registry['totalSkipEntries']}")
        print(f"  Non-skip overrides: {registry['totalNonSkipOverrides']}")
        print(f"  Categories:")
        for cat, count in sorted(registry["categories"].items()):
            print(f"    {cat}: {count}")

    if args.validate:
        result = validate_registry(args.output)
        if result.get("valid"):
            print("Skip registry VALID")
        else:
            print("Skip registry INVALID:")
            if result.get("unregisteredCount", 0) > 0:
                print(f"  {result['unregisteredCount']} unregistered skips:")
                for s in result.get("unregisteredSkips", []):
                    print(f"    - {s}")
            if result.get("expiredCount", 0) > 0:
                print(f"  {result['expiredCount']} expired entries")

    if args.report:
        r = report()
        print("=== _METHOD_OVERRIDES Summary ===")
        print(f"  Total overrides: {r['total_overrides']}")
        print(f"  Skip entries:    {r['skip_entries']}")
        print(f"  Custom entries:   {r['custom_entries']}")
        print(f"  Categories:")
        for cat, count in sorted(r["categories"].items()):
            print(f"    {cat}: {count}")

    if not any([args.generate, args.validate, args.report]):
        parser.print_help()


if __name__ == "__main__":
    main()
