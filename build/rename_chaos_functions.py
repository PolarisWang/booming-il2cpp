#!/usr/bin/env python3
"""
Batch rename chaos_* extern "C" functions to PascalCase across
all C++ native files and C# emitter files.

Usage: python build/rename_chaos_functions.py
"""

import re
import os
from pathlib import Path

REPO_ROOT = Path(__file__).resolve().parent.parent

# ── Auto-generate rename map from function list ──
# Capture all unique chaos_* function names from the native files.
FUNCTION_NAMES = set()

def collect_functions():
    """Scan native files for chaos_* function definitions/declarations."""
    native_files = [
        REPO_ROOT / "src/native/runtime-core/generated_code_compat.h",
        REPO_ROOT / "src/native/runtime-core/runtime_stubs.cpp",
        REPO_ROOT / "src/native/runtime-core/exception_api.cpp",
        REPO_ROOT / "src/native/runtime-core/reflection_api.cpp",
        REPO_ROOT / "src/native/runtime-core/reflection_api.h",
    ]
    for path in native_files:
        if not path.exists():
            continue
        with open(path, encoding="utf-8") as f:
            for line in f:
                m = re.match(r'^[\w:<>]+\s+(chaos_\w+)\s*\(', line)
                if m:
                    FUNCTION_NAMES.add(m.group(1))

collect_functions()

def to_pascal_case(name):
    """Convert chaos_snake_case_name → ChaosSnakeCaseName"""
    return "".join(w.capitalize() for w in name.split("_"))

RENAME_MAP = {name: to_pascal_case(name) for name in sorted(FUNCTION_NAMES)}

print(f"Total unique functions to rename: {len(RENAME_MAP)}")
for old, new in RENAME_MAP.items():
    print(f"  {old} -> {new}")

# ── Files to process ──
NATIVE_FILES = [
    "src/native/runtime-core/generated_code_compat.h",
    "src/native/runtime-core/runtime_stubs.cpp",
    "src/native/runtime-core/exception_api.cpp",
    "src/native/runtime-core/reflection_api.cpp",
    "src/native/runtime-core/reflection_api.h",
]

CSHARP_FILES = [
    "src/managed/Chaos.IL2CPP.CodeGen/Emission/NativeAotLoweringPlanner.MethodEmission.cs",
    "src/managed/Chaos.IL2CPP.CodeGen/Emission/NativeAotLoweringPlanner.ExceptionEmission.cs",
    "src/managed/Chaos.IL2CPP.CodeGen/NativeAotLoweringPlanner.cs",
    "src/managed/Chaos.IL2CPP.CodeGen/RuntimeSupport/NativeAotLoweringPlanner.ExternalRuntimeHelpers.InvocationAbi.cs",
    "src/managed/Chaos.IL2CPP.CodeGen/Emission/NativeAotLoweringPlanner.ObjectModelUtilities.cs",
    "src/managed/Chaos.IL2CPP.CodeGen/Emission/NativeAotLoweringPlanner.ReflectionObjectEmission.cs",
    "src/managed/Chaos.IL2CPP.CodeGen/RuntimeSupport/RuntimeHelperShapeRegistry.cs",
    "src/managed/Chaos.IL2CPP.CodeGen/RuntimeSupport/NativeAotLoweringPlanner.ExternalRuntimeHelpers.cs",
    "src/managed/Chaos.IL2CPP.CodeGen/Validation/NativeCodegenValidator.cs",
]

def process_file(rel_path, dry_run=False):
    """Apply renames to a single file. Returns count of replacements."""
    path = REPO_ROOT / rel_path
    if not path.exists():
        print(f"  SKIP (not found): {rel_path}")
        return 0

    with open(path, encoding="utf-8") as f:
        content = f.read()

    original = content
    count = 0
    for old_name, new_name in RENAME_MAP.items():
        # Only replace whole-word occurrences to avoid partial matches
        replaced = content.replace(old_name, new_name)
        if replaced != content:
            count += (len(replaced) - len(content)) // len(new_name) if new_name != old_name else 0
            count = content.count(old_name)
            content = replaced

    if content != original:
        if dry_run:
            print(f"  WOULD REPLACE: {rel_path} ({count} changes)")
        else:
            with open(path, "w", encoding="utf-8") as f:
                f.write(content)
            print(f"  UPDATED: {rel_path} ({count} changes)")
        return count
    else:
        print(f"  NO CHANGE: {rel_path}")
        return 0

def main():
    total = 0
    print("\n=== Processing native C++ files ===")
    for rel_path in NATIVE_FILES:
        total += process_file(rel_path)

    print("\n=== Processing C# emitter files ===")
    for rel_path in CSHARP_FILES:
        total += process_file(rel_path)

    print(f"\nTotal replacements: {total}")

if __name__ == "__main__":
    main()
