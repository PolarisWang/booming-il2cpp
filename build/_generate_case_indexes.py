#!/usr/bin/env python3
"""
Generate missing method-*-case-index.json files for Phase 3 reflection families.

These families were created by _phase3_a_b_generator.py but scan_case_indexes()
was never called, leaving method-test-case-index.json, method-benchmark-case-index.json,
and method-hotupdate-case-index.json missing. This causes the dashboard to display
empty Tests/Benchmarks/HotUpdate sections.
"""

import sys
from pathlib import Path

REPO_ROOT = Path(__file__).resolve().parent.parent
sys.path.insert(0, str(REPO_ROOT / "build" / "toolchains" / "run" / "testing"))

from foundation_dll import case_index_scanner as case_index_scanner_module

FAMILIES = [
    "family/System.Private.CoreLib/reflection/type",
    "family/System.Private.CoreLib/reflection/assembly",
    "family/System.Private.CoreLib/reflection/member-complete",
    "family/System.Private.CoreLib/reflection/generics",
    "family/System.Private.CoreLib/reflection/parameters",
    "family/System.Private.CoreLib/reflection/module",
    "family/System.Private.CoreLib/reflection/field-property",
    "family/System.Private.CoreLib/attributes/custom-metadata",
]

if __name__ == "__main__":
    generated = []
    for family_id in FAMILIES:
        result = case_index_scanner_module.scan_case_indexes(
            REPO_ROOT,
            assembly_name="System.Private.CoreLib",
            family_id=family_id,
        )
        for artifact in result.get("artifacts", []):
            generated.append(artifact)

    if generated:
        print(f"Generated {len(generated)} case index files:")
        for path in sorted(generated):
            print(f"  - {path}")
    else:
        print("No artifacts returned; checking file existence...")
        for family_id in FAMILIES:
            parts = [p for p in str(family_id).split("/") if p]
            slug = "-".join(parts[2:])
            family_root = REPO_ROOT / "verification" / "foundation-dll" / "System.Private.CoreLib" / slug
            test_idx = family_root / "method-test-case-index.json"
            bench_idx = family_root / "method-benchmark-case-index.json"
            hot_idx = family_root / "method-hotupdate-case-index.json"
            test_ok = "OK" if test_idx.is_file() else "MISSING"
            bench_ok = "OK" if bench_idx.is_file() else "MISSING"
            hot_ok = "OK" if hot_idx.is_file() else "MISSING"
            print(f"  {family_id}: test={test_ok}, bench={bench_ok}, hot={hot_ok}")
