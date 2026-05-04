#!/usr/bin/env python3
"""
Fix broken PascalCase names caused by prefix overlap in the batch rename.
Also handle remaining namespace/naming issues.

Usage: python build/fix_broken_renames.py
"""

import re
from pathlib import Path

REPO_ROOT = Path(__file__).resolve().parent.parent

# ── Fix broken PascalCase → correct PascalCase ──
# These were incorrectly produced because a shorter function name
# (e.g. chaos_reflection_get_assembly) was replaced before a longer
# one (e.g. chaos_reflection_get_assembly_location).
# Sort by length descending so longer names are replaced first.
FIXUP_LIST = sorted([
    ("ChaosReflectionGetAssembly_full_name", "ChaosReflectionGetAssemblyFullName"),
    ("ChaosReflectionGetAssembly_location", "ChaosReflectionGetAssemblyLocation"),
    ("ChaosReflectionGetAssembly_qualified_name", "ChaosReflectionGetAssemblyQualifiedName"),
    ("ChaosReflectionGetAssembly_name_value", "ChaosReflectionGetAssemblyNameValue"),
    ("ChaosReflectionGetAssembly_name", "ChaosReflectionGetAssemblyName"),
    ("ChaosReflectionGetConstructors_default", "ChaosReflectionGetConstructorsDefault"),
    ("ChaosReflectionGetFields_bindingflags", "ChaosReflectionGetFieldsBindingflags"),
    ("ChaosReflectionGetIsGenericType_def", "ChaosReflectionGetIsGenericTypeDef"),
    ("ChaosReflectionGetMethod_handle", "ChaosReflectionGetMethodHandle"),
    ("ChaosReflectionGetMethods_bindingflags", "ChaosReflectionGetMethodsBindingflags"),
    ("ChaosReflectionGetModuleName_only", "ChaosReflectionGetModuleNameOnly"),
], key=lambda x: len(x[0]), reverse=True)

FILES = [
    "src/native/runtime-core/generated_code_compat.h",
    "src/native/runtime-core/runtime_stubs.cpp",
    "src/native/runtime-core/exception_api.cpp",
    "src/native/runtime-core/reflection_api.cpp",
    "src/native/runtime-core/reflection_api.h",
]

def fixup_file(rel_path):
    path = REPO_ROOT / rel_path
    if not path.exists():
        print(f"  SKIP: {rel_path}")
        return 0

    with open(path, encoding="utf-8") as f:
        content = f.read()

    original = content
    count = 0
    for wrong, correct in FIXUP_LIST:
        if wrong in content:
            content = content.replace(wrong, correct)
            count += 1

    if content != original:
        with open(path, "w", encoding="utf-8") as f:
            f.write(content)
        print(f"  FIXED: {rel_path} ({count} fixes)")
    else:
        print(f"  OK: {rel_path}")
    return count

def main():
    total = 0
    for rel_path in FILES:
        total += fixup_file(rel_path)
    print(f"\nTotal fixes: {total}")

if __name__ == "__main__":
    main()
