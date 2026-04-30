#!/usr/bin/env python3
"""Chaos IL2CPP Wiki Consistency Checker

Mechanically verifies that the il2cpp architecture wiki (wiki/03-功能模块/06-il2cpp核心架构/)
is consistent with the actual codebase.

Phases:
  1 - Opcode coverage: bidirectional check between wiki translation tables and codegen switch
  2 - File path & function name existence (stub)
  3 - Bridge contract consistency (stub)
  4 - Architecture constraint verifiability (stub)

Usage:
  python build/check_wiki.py               # Run all phases
  python build/check_wiki.py --ci           # Exit 1 on any issue
  python build/check_wiki.py --phase 1      # Run only phase 1
  python build/check_wiki.py --phase 1 --ci
"""

import argparse
import json
import os
import re
import sys
from pathlib import Path

REPO_ROOT = Path(__file__).resolve().parent.parent

# ── Path constants ──────────────────────────────────────────────────────────

WIKI_ARCH_ROOT = REPO_ROOT / "wiki" / "03-功能模块" / "06-il2cpp核心架构"
TRANSLATION_TABLES_DIR = WIKI_ARCH_ROOT / "02-翻译路径参考"
CONSTRAINTS_DIR = WIKI_ARCH_ROOT / "03-架构约束"
CODGEN_METHOD_EMISSION = (
    REPO_ROOT
    / "src"
    / "managed"
    / "Chaos.IL2CPP.CodeGen"
    / "Emission"
    / "NativeAotLoweringPlanner.MethodEmission.cs"
)
CODGEN_EXCEPTION_EMISSION = (
    REPO_ROOT
    / "src"
    / "managed"
    / "Chaos.IL2CPP.CodeGen"
    / "Emission"
    / "NativeAotLoweringPlanner.ExceptionEmission.cs"
)
BRIDGE_CONTRACT = REPO_ROOT / "contracts" / "native" / "v0" / "codegen_bridge.h"
RUNTIME_ABI_CONTRACT = REPO_ROOT / "contracts" / "native" / "v0" / "runtime_abi.h"

# ── Opcode family config ─────────────────────────────────────────────────

# Wiki wildcard families map to their base name in codegen.
# E.g., wiki says "ldarg.*" means "all ldarg variants", codegen handles these
# with a single `case "ldarg":`.
FAMILY_BASE: dict[str, str] = {
    "ldc.i4.*": "ldc.i4",
    "ldarg.*": "ldarg",
    "ldloc.*": "ldloc",
    "stloc.*": "stloc",
    "conv.ovf.*": "conv.ovf",
    "ldelem.*": "ldelem",
    "stelem.*": "stelem",
    "ldind.*": "ldind",
    "stind.*": "stind",
    "add.ovf.*": "add.ovf",
    "sub.ovf.*": "sub.ovf",
    "mul.ovf.*": "mul.ovf",
}

# Backtick-quoted names that look like opcodes but are actually file paths
OPCODE_FALSE_POSITIVES: set[str] = {
    "test_dispatch_generic_constraint_native_aot.py",
}

# Known opcodes/scenarios mentioned in wiki that do NOT appear as case labels in codegen
KNOWN_EXCEPTIONS_WIKI_ONLY: set[str] = {
    "constrained.",       # prefix, handled by SkipConstrainedInstruction in loader
    "clt.un",             # IL opcode, not directly handled in native-aot codegen
    "conv.i",             # IL IntPtr conversion, not directly handled
    "ldarg.0",            # wiki lists ldarg.0 explicitly; codegen uses family "ldarg"
    "ldarg.1",            # same
    "ldarg.2",            # same
    "ldarg.3",            # same
    "ldarg.s",            # same
    "ldc.i4.0",           # wiki lists ldc.i4.* variants; codegen uses family "ldc.i4"
    "ldc.i4.1",           # same
    "ldc.i4.2",           # same
    "ldc.i4.3",           # same
    "ldc.i4.4",           # same
    "ldc.i4.5",           # same
    "ldc.i4.6",           # same
    "ldc.i4.7",           # same
    "ldc.i4.8",           # same
    "ldc.i4.m1",          # same
    "ldc.i4.s",           # same
    "ldloc.0",            # wiki lists ldloc.* variants; codegen uses family "ldloc"
    "ldloc.1",            # same
    "ldloc.2",            # same
    "ldloc.3",            # same
    "ldloc.s",            # same
    "stloc.0",            # wiki lists stloc.* variants; codegen uses family "stloc"
    "stloc.1",            # same
    "stloc.2",            # same
    "stloc.3",            # same
    "stloc.s",            # same
    "ldelem.i1",          # wiki lists ldelem.* family; codegen uses base "ldelem"
    "ldelem.u1",          # same
    "ldelem.i2",          # same
    "ldelem.u2",          # same
    "ldelem.i4",          # same
    "ldelem.u4",          # same
    "ldelem.i8",          # same
    "ldelem.u8",          # same
    "ldelem.r4",          # same
    "ldelem.r8",          # same
    "stelem.i1",          # wiki lists stelem.* family; codegen uses base "stelem"
    "stelem.u1",          # same
    "stelem.i2",          # same
    "stelem.u2",          # same
    "stelem.i4",          # same
    "stelem.u4",          # same
    "stelem.i8",          # same
    "stelem.u8",          # same
    "stelem.r4",          # same
    "stelem.r8",          # same
    "ldind.i1",           # wiki lists ldind.* family; codegen uses specific ldind.i4/stind.i4
    "ldind.u1",           # same
    "ldind.i2",           # same
    "ldind.u2",           # same
    "ldind.i4",           # wiki has ldind.* as family, codegen has ldind.i4 → add exception
    "ldind.u4",           # same
    "ldind.i8",           # same
    "ldind.u8",           # same
    "ldind.r4",           # same
    "ldind.r8",           # same
    "ldind.ref",          # same
    "stind.i1",           # wiki lists stind.* family; codegen uses specific stind.i4
    "stind.i2",           # same
    "stind.i4",           # same
    "stind.i8",           # same
    "stind.r4",           # same
    "stind.r8",           # same
    "stind.ref",          # same
    "conv.ovf.i2",        # wiki lists conv.ovf.* family; codegen uses specific i1/u1
    "conv.ovf.i4",        # same
    "conv.ovf.u2",        # same
    "conv.ovf.i8",        # same
    "conv.ovf.u4",        # same
    "conv.ovf.u8",        # same
    "add.ovf.*",          # wiki lists as family; codegen uses base "add.ovf"
    "sub.ovf.*",          # same
    "mul.ovf.*",          # same
    "conv.ovf",           # wiki family name maps here; codegen uses conv.ovf.i1/u1
}

# Known opcodes handled by codegen that are implicitly covered by wiki families
KNOWN_EXCEPTIONS_CODGEN_ONLY: set[str] = {
    "ldelem.ref",  # wiki covers via ldelem.*
    "stelem.ref",  # wiki covers via stelem.*
    "conv.ovf.i1", # wiki covers via conv.ovf.* family
    "conv.ovf.u1", # same
    "ldind.i4",    # wiki covers via ldind.* family
    "stind.i4",    # wiki covers via stind.* family
}

# ── Markdown parsing ────────────────────────────────────────────────────────

OPCODE_RE = re.compile(r"`([a-z][a-z0-9._*]+)`")
TABLE_ROW_OPCODE_RE = re.compile(r"\|\s*`([a-z][a-z0-9._*]+)`\s*\|")
GROUP_CELL_OPCODE_RE = re.compile(r"`([a-z][a-z0-9._*]+)`")


def extract_opcodes_from_table(md_text: str) -> set[str]:
    """Extract IL opcodes from wiki translation table markdown.

    Handles:
      - Standalone: | `ldnull` | ... |
      - Grouped:    | 算术 | `add` / `sub` / `mul` | ... |
    """
    opcodes: set[str] = set()
    lines = md_text.split("\n")
    in_table = False

    for line in lines:
        stripped = line.strip()

        # Detect table boundaries
        if stripped.startswith("|") and "---" in stripped:
            in_table = True
            continue
        if not stripped.startswith("|"):
            in_table = False
            continue
        if not in_table:
            # Also match post-table inline backtick opcodes
            for m in OPCODE_RE.finditer(stripped):
                opcodes.add(m.group(1))
            continue

        # Inside a table
        if in_table:
            # Try first-cell opcode first: | `ldnull` | ... |
            m = TABLE_ROW_OPCODE_RE.match(stripped)
            if m:
                opcodes.add(m.group(1))
            else:
                # Grouped row: | 类别 | `op1` / `op2` | ... |
                for m in GROUP_CELL_OPCODE_RE.finditer(stripped):
                    opcodes.add(m.group(1))

    return opcodes


def expand_wildcards(opcodes: set[str]) -> set[str]:
    """Expand wildcard opcodes like ldelem.* into their base form (e.g. ldelem).

    Wiki uses family names (ldelem.*) to describe groups. Codegen handles
    these with a single case label (case "ldelem":). So we map:
        ldelem.* → ldelem
    keeping non-wildcard entries as-is.
    """
    expanded: set[str] = set()
    for op in opcodes:
        if "*" in op:
            base = FAMILY_BASE.get(op)
            if base:
                expanded.add(base)
            else:
                # Unknown wildcard – keep raw name (may get filtered later)
                expanded.add(op)
        else:
            expanded.add(op)
    return expanded


# ── Codegen parsing ─────────────────────────────────────────────────────────

CODEGEN_CASE_RE = re.compile(r'case\s+"([a-z][a-z0-9._]+)"\s*:')


def extract_codegen_opcodes(cs_text: str) -> set[str]:
    """Extract opcode strings from codegen switch-case statements."""
    return set(CODEGEN_CASE_RE.findall(cs_text))


# ── Phase checks ────────────────────────────────────────────────────────────


def phase1_check_opcode_coverage() -> list[str]:
    """Bidirectional opcode coverage: wiki <-> codegen."""
    errors: list[str] = []

    # 1a. Collect wiki opcodes from all translation table files
    wiki_opcodes: set[str] = set()
    for table_file in sorted(TRANSLATION_TABLES_DIR.glob("*.md")):
        if "INDEX" in table_file.name:
            continue
        text = table_file.read_text(encoding="utf-8")
        raw = extract_opcodes_from_table(text)
        wiki_opcodes.update(expand_wildcards(raw))

    # 1b. Collect codegen opcodes from main switch (MethodEmission.cs)
    if not CODGEN_METHOD_EMISSION.is_file():
        errors.append(f"[INTERNAL] Codegen file not found: {CODGEN_METHOD_EMISSION}")
        return errors

    codegen_text = CODGEN_METHOD_EMISSION.read_text(encoding="utf-8")
    codegen_opcodes = extract_codegen_opcodes(codegen_text)

    # Also collect from ExceptionEmission.cs linear switch
    if CODGEN_EXCEPTION_EMISSION.is_file():
        exception_text = CODGEN_EXCEPTION_EMISSION.read_text(encoding="utf-8")
        codegen_opcodes.update(extract_codegen_opcodes(exception_text))

    # 1c. Cross-reference: wiki has but codegen doesn't
    wiki_only = wiki_opcodes - codegen_opcodes - KNOWN_EXCEPTIONS_WIKI_ONLY
    # Filter out entries that don't look like IL opcodes
    wiki_only = {o for o in wiki_only if re.match(r"^[a-z]", o) and "." in o}
    # Remove false positives (e.g., file paths in backticks)
    wiki_only -= OPCODE_FALSE_POSITIVES

    for opcode in sorted(wiki_only):
        errors.append(
            f"[OPCODE/WIKI_ONLY] '{opcode}' is documented in wiki translation "
            f"tables but has no matching 'case \"{opcode}\":' in "
            f"NativeAotLoweringPlanner.MethodEmission.cs"
        )

    # 1d. Cross-reference: codegen has but wiki doesn't
    codegen_only = codegen_opcodes - wiki_opcodes - KNOWN_EXCEPTIONS_CODGEN_ONLY
    codegen_only = {o for o in codegen_only if re.match(r"^[a-z]", o)}

    for opcode in sorted(codegen_only):
        errors.append(
            f"[OPCODE/CODEGEN_ONLY] '{opcode}' is handled in codegen "
            f"(NativeAotLoweringPlanner.MethodEmission.cs) but not documented "
            f"in wiki translation tables (02-翻译路径参考/)"
        )

    return errors


# ── Phase 2-4 stubs ─────────────────────────────────────────────────────────


# Paths that are intentionally generated at build time and won't exist in source
GENERATED_FILE_SUFFIXES: set[str] = {
    ".gen.h",
    ".gen.cpp",
}

# Wiki files to scan for Phase 2 (all .md under arch root)
WIKI_MD_FILES: list[Path] = []


def _collect_wiki_md_files() -> list[Path]:
    """Collect all .md files under the architecture wiki root.

    Uses os.walk to handle Chinese path encoding on Windows (rglob mangles CJK).
    Includes INDEX.md files (they contain file path references for Phase 2).
    """
    if WIKI_MD_FILES:
        return WIKI_MD_FILES
    files: list[Path] = []
    for root_str, dirs, fnames in os.walk(str(WIKI_ARCH_ROOT)):
        root = Path(root_str)
        for f in fnames:
            if f.endswith(".md"):
                files.append(root / f)
    WIKI_MD_FILES.extend(files)
    return WIKI_MD_FILES


# Pattern for markdown links: [text](path)
FILE_PATH_LINK_RE = re.compile(r"\[([^\]]+)\]\(([^)]+)\)")

# URL-encoded characters in Chinese wiki paths (e.g., %E7%BF%BB%E8%AF%91)
# These need to be decoded before filesystem access
import urllib.parse


KNOWN_SOURCE_PREFIXES: tuple[str, ...] = (
    "build/",
    "src/",
    "contracts/",
    "wiki/",
    "docs/",
)


def resolve_wiki_path(wiki_file: Path, raw_path: str) -> Path | None:
    """Resolve a path from wiki markdown to an absolute repo path.

    Handles:
      - Relative paths: `./foo.md`, `foo/bar.md`
      - Repo-root paths: `contracts/native/...`
      - Wildcard paths: `src/foo/*.h`, `foo.*`
      - URL-encoded Chinese paths: `%E7%BF%BB%E8%AF%91` → `翻译`
    Returns None if the path can't be resolved.
    """
    path_str = raw_path.strip()

    # URL-decode: wiki uses %XX encoding for Chinese characters
    path_str = urllib.parse.unquote(path_str)

    # Skip generated files (don't exist until build time)
    if any(path_str.endswith(suffix) for suffix in GENERATED_FILE_SUFFIXES):
        return None

    # Skip paths with wildcards (can't check existence directly)
    if "*" in path_str:
        return None

    # If it starts with ./ or is a relative path (no leading known prefix)
    if path_str.startswith("./"):
        # Relative to wiki file's parent directory
        resolved = (wiki_file.parent / path_str[2:]).resolve()
        return resolved
    elif not any(path_str.startswith(p) for p in KNOWN_SOURCE_PREFIXES):
        # Relative path (e.g., `04-历史决策/01-管线五层职责决策.md`)
        resolved = (wiki_file.parent / path_str).resolve()
        # Only return if it would resolve within the repo
        repo_str = str(REPO_ROOT).replace("\\", "/")
        resolved_str = str(resolved).replace("\\", "/")
        if resolved_str.startswith(repo_str):
            return resolved
        return None
    else:
        # Repo-root relative (starts with build/, src/, etc.)
        resolved = (REPO_ROOT / path_str).resolve()
        return resolved


def phase2_check_file_paths(verbose: bool = False) -> list[str]:
    """Check that file paths referenced in wiki exist in the repo."""
    errors: list[str] = []
    md_files = _collect_wiki_md_files()

    checked_count = 0
    found_count = 0
    missing_count = 0

    for md_file in md_files:
        text = md_file.read_text(encoding="utf-8")
        rel_path = md_file.relative_to(REPO_ROOT)

        # 2a. Extract paths from markdown links: [text](path)
        link_count = 0
        for match in FILE_PATH_LINK_RE.finditer(text):
            link_count += 1
            link_text = match.group(1)
            raw_path = match.group(2).strip()

            # Skip external URLs and anchor-only links
            if raw_path.startswith("http://") or raw_path.startswith("https://"):
                continue
            if raw_path.startswith("#"):
                continue

            resolved = resolve_wiki_path(md_file, raw_path)
            if resolved is None:
                continue

            checked_count += 1
            if resolved.exists():
                found_count += 1
            else:
                # Double-check with url-decoded path
                raw_decoded = urllib.parse.unquote(raw_path)
                if raw_decoded != raw_path:
                    alt_resolved = resolve_wiki_path(md_file, raw_decoded)
                    if alt_resolved and alt_resolved.exists():
                        found_count += 1
                        continue
                missing_count += 1
                errors.append(
                    f"[PATH/MISSING] '{raw_path}' (from '{link_text}' in {rel_path}) "
                    f"→ expected at {resolved} but file not found"
                )

    if verbose:
        print(f"[PHASE 2] checked={checked_count} found={found_count} missing={missing_count}", file=sys.stderr)

    return errors


# ── Phase 3-4 stubs ─────────────────────────────────────────────────────────


def phase3_check_bridge_contracts() -> list[str]:
    """Check bridge contract name consistency. (Stub)"""
    # TODO: extract function names from wiki, cross-ref codegen_bridge.h / runtime_abi.h
    return []


# ── Phase 4: MetadataClosure audit ─────────────────────────────────────

# Reflection helper detection functions in ObjectModelUtilities.cs
# These are the subject-ID matching functions that gate codegen emission
REFLECTION_HELPER_DETECTION_FUNCS: dict[str, str] = {
    "IsTypeReflectionHelperSubjectId":
        "Type.GetTypeFromHandle, get_TypeHandle, GetField(string), "
        "GetMethod(string), GetGenericArguments, get_DeclaringType, "
        "GetMethod(string,BindingFlags), get_Assembly, "
        "GetGenericTypeDefinition, GetConstructors(BindingFlags), "
        "get_Name, get_FullName, get_Namespace, get_AssemblyQualifiedName, "
        "get_IsValueType, get_IsEnum, get_IsAbstract, get_IsSealed, "
        "get_IsInterface, get_IsNested, get_IsPublic, get_IsNotPublic, "
        "get_IsNestedPublic, get_IsNestedPrivate, get_IsArray, get_IsByRef, "
        "get_IsPointer, get_IsGenericType, get_IsConstructedGenericType, "
        "get_IsGenericTypeDefinition, ContainsGenericParameters, "
        "get_GenericParameterPosition, GetGenericParameterConstraints, "
        "MakeGenericType, MakeArrayType, MakeByRefType, MakePointerType, "
        "get_BaseType, get_UnderlyingSystemType, get_ReflectedType, "
        "GetMethods, GetFields, GetNestedTypes, GetNestedType(string), "
        "GetInterfaces, GetElementType, GetMembers",
    "IsAssemblyReflectionHelperSubjectId":
        "Type.get_Assembly, Assembly.GetType(string), "
        "Type.GetType(string), Assembly.GetName, AssemblyName.get_Name, "
        "GetExecutingAssembly, GetCallingAssembly, GetEntryAssembly, "
        "GetTypes, GetExportedTypes, GetForwardedTypes, GetModules, "
        "GetModule(string), GetManifestResourceStream(string), "
        "GetManifestResourceNames, get_FullName, get_Location, "
        "get_ImageRuntimeVersion, get_EntryPoint, "
        "AssemblyName.get_Version, AssemblyName.get_CultureInfo, "
        "Assembly.GetType(string,bool)",
    "IsReflectionMemberHelperSubjectId":
        "Type.GetMethod(string), Type.GetMethod(string,BindingFlags), "
        "MethodBase.GetParameters, MemberInfo.get_Name, "
        "ParameterInfo.get_Name, MethodBase.get_MethodHandle, "
        "MethodBase.Invoke(object,object[]), "
        "FieldInfo.get_IsStatic, FieldInfo.get_IsInitOnly, "
        "FieldInfo.get_IsLiteral, FieldInfo.get_FieldHandle, "
        "MethodBase.get_IsPublic, MethodBase.get_IsStatic, "
        "MethodBase.get_IsVirtual, MethodBase.get_CallingConvention, "
        "MethodInfo.GetBaseDefinition, ConstructorInfo.get_CallingConvention, "
        "ParameterInfo.get_ParameterType, ParameterInfo.get_DefaultValue, "
        "ParameterInfo.get_RawDefaultValue, ParameterInfo.get_HasDefaultValue, "
        "ParameterInfo.get_Attributes, ParameterInfo.get_Position, "
        "ParameterInfo.GetRequiredCustomModifiers, "
        "ParameterInfo.GetOptionalCustomModifiers",
    "IsActivationHelperSubjectId":
        "Activator.CreateInstance(Type), "
        "Activator.CreateInstance(Type,object[]), "
        "Activator.CreateInstance<T>(), "
        "Activator.CreateInstance(Type,BindingFlags,Binder,object[],CultureInfo), "
        "Array.CreateInstance(Type,int), "
        "Array.CreateInstance(Type,int,int), "
        "Array.CreateInstance(Type,int[]), "
        "ConstructorInfo.Invoke(object[])",
}

# Map from codegen-detected subject ID patterns (short form without signature) to capability family IDs.
# Matching is prefix-based: ledger subject IDs starting with the key map to the given family.
REFLECTION_SUBJECTID_TO_FAMILY: dict[str, str] = {
    # ── reflection/type (methods that query Type properties/members) ──
    "System.Private.CoreLib/System.Type::GetTypeFromHandle": "reflection/type",
    "System.Private.CoreLib/System.Type::GetType": "reflection/type",
    "System.Private.CoreLib/System.Type::get_Name": "reflection/type",
    "System.Private.CoreLib/System.Type::get_FullName": "reflection/type",
    "System.Private.CoreLib/System.Type::get_Namespace": "reflection/type",
    "System.Private.CoreLib/System.Type::get_AssemblyQualifiedName": "reflection/type",
    "System.Private.CoreLib/System.Type::get_TypeHandle": "reflection/type",
    "System.Private.CoreLib/System.Type::get_IsValueType": "reflection/type",
    "System.Private.CoreLib/System.Type::get_IsEnum": "reflection/type",
    "System.Private.CoreLib/System.Type::get_IsAbstract": "reflection/type",
    "System.Private.CoreLib/System.Type::get_IsSealed": "reflection/type",
    "System.Private.CoreLib/System.Type::get_IsInterface": "reflection/type",
    "System.Private.CoreLib/System.Type::get_IsArray": "reflection/type",
    "System.Private.CoreLib/System.Type::get_IsByRef": "reflection/type",
    "System.Private.CoreLib/System.Type::get_IsPointer": "reflection/type",
    "System.Private.CoreLib/System.Type::get_IsGenericType": "reflection/type",
    "System.Private.CoreLib/System.Type::get_IsConstructedGenericType": "reflection/type",
    "System.Private.CoreLib/System.Type::get_IsNested": "reflection/type",
    "System.Private.CoreLib/System.Type::get_IsPublic": "reflection/type",
    "System.Private.CoreLib/System.Type::get_IsNotPublic": "reflection/type",
    "System.Private.CoreLib/System.Type::get_IsNestedPublic": "reflection/type",
    "System.Private.CoreLib/System.Type::get_IsNestedPrivate": "reflection/type",
    "System.Private.CoreLib/System.Type::get_BaseType": "reflection/type",
    "System.Private.CoreLib/System.Type::get_DeclaringType": "reflection/type",
    "System.Private.CoreLib/System.Type::get_ReflectedType": "reflection/type",
    "System.Private.CoreLib/System.Type::get_UnderlyingSystemType": "reflection/type",
    "System.Private.CoreLib/System.Type::GetMethods": "reflection/type",
    "System.Private.CoreLib/System.Type::GetFields": "reflection/type",
    "System.Private.CoreLib/System.Type::GetProperties": "reflection/type",
    "System.Private.CoreLib/System.Type::GetEvents": "reflection/type",
    "System.Private.CoreLib/System.Type::GetConstructors": "reflection/type",
    "System.Private.CoreLib/System.Type::GetMembers": "reflection/type",
    "System.Private.CoreLib/System.Type::GetInterfaces": "reflection/type",
    "System.Private.CoreLib/System.Type::GetElementType": "reflection/type",
    "System.Private.CoreLib/System.Type::GetNestedTypes": "reflection/type",
    "System.Private.CoreLib/System.Type::GetNestedType": "reflection/type",
    # Methods with BindingFlags overloads are mapped to reflection/binding below.
    # Only simple overloads (no BindingFlags) are tracked under reflection/type in the ledger.
    # ── reflection/generics ──
    "System.Private.CoreLib/System.Type::GetGenericTypeDefinition": "reflection/generics",
    "System.Private.CoreLib/System.Type::GetGenericArguments": "reflection/generics",
    "System.Private.CoreLib/System.Type::MakeGenericType": "reflection/generics",
    "System.Private.CoreLib/System.Type::GetGenericParameterConstraints": "reflection/generics",
    "System.Private.CoreLib/System.Type::get_GenericParameterPosition": "reflection/generics",
    "System.Private.CoreLib/System.Type::ContainsGenericParameters": "reflection/generics",
    "System.Private.CoreLib/System.Type::get_IsGenericTypeDefinition": "reflection/generics",
    "System.Private.CoreLib/System.Type::MakeArrayType": "reflection/generics",
    "System.Private.CoreLib/System.Type::MakeByRefType": "reflection/generics",
    "System.Private.CoreLib/System.Type::MakePointerType": "reflection/generics",
    # ── reflection/assembly ──
    "System.Private.CoreLib/System.Reflection.Assembly::GetExecutingAssembly": "reflection/assembly",
    "System.Private.CoreLib/System.Reflection.Assembly::GetCallingAssembly": "reflection/assembly",
    "System.Private.CoreLib/System.Reflection.Assembly::GetEntryAssembly": "reflection/assembly",
    "System.Private.CoreLib/System.Reflection.Assembly::GetName": "reflection/assembly",
    "System.Private.CoreLib/System.Reflection.Assembly::get_FullName": "reflection/assembly",
    "System.Private.CoreLib/System.Reflection.Assembly::get_Location": "reflection/assembly",
    "System.Private.CoreLib/System.Reflection.Assembly::get_ImageRuntimeVersion": "reflection/assembly",
    "System.Private.CoreLib/System.Reflection.Assembly::GetTypes": "reflection/assembly",
    "System.Private.CoreLib/System.Reflection.Assembly::GetExportedTypes": "reflection/assembly",
    "System.Private.CoreLib/System.Reflection.Assembly::GetForwardedTypes": "reflection/assembly",
    "System.Private.CoreLib/System.Reflection.Assembly::GetType": "reflection/assembly",
    "System.Private.CoreLib/System.Reflection.Assembly::GetModule": "reflection/assembly",
    "System.Private.CoreLib/System.Reflection.Assembly::GetModules": "reflection/assembly",
    "System.Private.CoreLib/System.Reflection.Assembly::GetManifestResourceStream": "reflection/assembly",
    "System.Private.CoreLib/System.Reflection.Assembly::GetManifestResourceNames": "reflection/assembly",
    "System.Private.CoreLib/System.Reflection.Assembly::get_EntryPoint": "reflection/assembly",
    "System.Private.CoreLib/System.Reflection.AssemblyName::get_Name": "reflection/assembly",
    "System.Private.CoreLib/System.Reflection.AssemblyName::get_Version": "reflection/assembly",
    "System.Private.CoreLib/System.Reflection.AssemblyName::get_CultureInfo": "reflection/assembly",
    "System.Private.CoreLib/System.Type::get_Assembly": "reflection/assembly",
    # ── reflection/member-complete ──
    "System.Private.CoreLib/System.Reflection.MemberInfo::get_Name": "reflection/member-complete",
    "System.Private.CoreLib/System.Reflection.MemberInfo::get_MemberType": "reflection/member-complete",
    "System.Private.CoreLib/System.Reflection.MemberInfo::get_DeclaringType": "reflection/member-complete",
    "System.Private.CoreLib/System.Reflection.MemberInfo::get_ReflectedType": "reflection/member-complete",
    "System.Private.CoreLib/System.Reflection.MemberInfo::get_MetadataToken": "reflection/member-complete",
    "System.Private.CoreLib/System.Reflection.MethodInfo::get_ReturnType": "reflection/member-complete",
    "System.Private.CoreLib/System.Reflection.MethodInfo::GetBaseDefinition": "reflection/member-complete",
    "System.Private.CoreLib/System.Reflection.MethodBase::get_MethodHandle": "reflection/member-complete",
    "System.Private.CoreLib/System.Reflection.MethodBase::get_CallingConvention": "reflection/member-complete",
    "System.Private.CoreLib/System.Reflection.MethodBase::get_IsPublic": "reflection/member-complete",
    "System.Private.CoreLib/System.Reflection.MethodBase::get_IsStatic": "reflection/member-complete",
    "System.Private.CoreLib/System.Reflection.MethodBase::get_IsVirtual": "reflection/member-complete",
    "System.Private.CoreLib/System.Reflection.MethodInfo::get_ReturnType": "reflection/member-complete",
    "System.Private.CoreLib/System.Reflection.FieldInfo::get_FieldType": "reflection/member-complete",
    "System.Private.CoreLib/System.Reflection.FieldInfo::get_IsStatic": "reflection/member-complete",
    "System.Private.CoreLib/System.Reflection.FieldInfo::get_IsInitOnly": "reflection/member-complete",
    "System.Private.CoreLib/System.Reflection.FieldInfo::get_IsLiteral": "reflection/member-complete",
    "System.Private.CoreLib/System.Reflection.FieldInfo::get_FieldHandle": "reflection/member-complete",
    "System.Private.CoreLib/System.Reflection.PropertyInfo::get_PropertyType": "reflection/member-complete",
    "System.Private.CoreLib/System.Reflection.PropertyInfo::get_CanRead": "reflection/member-complete",
    "System.Private.CoreLib/System.Reflection.PropertyInfo::get_CanWrite": "reflection/member-complete",
    "System.Private.CoreLib/System.Reflection.PropertyInfo::GetIndexParameters": "reflection/member-complete",
    "System.Private.CoreLib/System.Reflection.ConstructorInfo::get_CallingConvention": "reflection/member-complete",
    # ── reflection/parameters ──
    "System.Private.CoreLib/System.Reflection.ParameterInfo::get_Name": "reflection/parameters",
    "System.Private.CoreLib/System.Reflection.ParameterInfo::get_ParameterType": "reflection/parameters",
    "System.Private.CoreLib/System.Reflection.ParameterInfo::get_DefaultValue": "reflection/parameters",
    "System.Private.CoreLib/System.Reflection.ParameterInfo::get_RawDefaultValue": "reflection/parameters",
    "System.Private.CoreLib/System.Reflection.ParameterInfo::get_HasDefaultValue": "reflection/parameters",
    "System.Private.CoreLib/System.Reflection.ParameterInfo::get_Attributes": "reflection/parameters",
    "System.Private.CoreLib/System.Reflection.ParameterInfo::get_Position": "reflection/parameters",
    "System.Private.CoreLib/System.Reflection.ParameterInfo::GetRequiredCustomModifiers": "reflection/parameters",
    "System.Private.CoreLib/System.Reflection.ParameterInfo::GetOptionalCustomModifiers": "reflection/parameters",
    "System.Private.CoreLib/System.Reflection.MethodBase::GetParameters": "reflection/parameters",
    # ── reflection/module ──
    "System.Private.CoreLib/System.Reflection.Module::get_FullyQualifiedName": "reflection/module",
    "System.Private.CoreLib/System.Reflection.Module::get_Name": "reflection/module",
    "System.Private.CoreLib/System.Reflection.Module::get_Assembly": "reflection/module",
    "System.Private.CoreLib/System.Reflection.Module::GetType": "reflection/module",
    "System.Private.CoreLib/System.Reflection.Module::GetTypes": "reflection/module",
    "System.Private.CoreLib/System.Reflection.Module::GetCustomAttributes": "reflection/module",
    # ── reflection/binding (methods with BindingFlags/Invoke overloads) ──
    "System.Private.CoreLib/System.Type::GetMethod": "reflection/binding",
    "System.Private.CoreLib/System.Type::GetField": "reflection/binding",
    "System.Private.CoreLib/System.Type::GetProperty": "reflection/binding",
    "System.Private.CoreLib/System.Type::GetEvent": "reflection/binding",
    "System.Private.CoreLib/System.Reflection.MethodBase::Invoke": "reflection/binding",
    # ── reflection/activation ──
    "System.Private.CoreLib/System.Activator::CreateInstance": "reflection/activation",
    "System.Private.CoreLib/System.Array::CreateInstance": "reflection/activation",
    "System.Private.CoreLib/System.Reflection.ConstructorInfo::Invoke": "reflection/activation",
    # ── reflection/member-basics ──
    "System.Private.CoreLib/System.Reflection.FieldInfo::GetValue": "reflection/member-basics",
    "System.Private.CoreLib/System.Reflection.FieldInfo::SetValue": "reflection/member-basics",
    "System.Private.CoreLib/System.Reflection.PropertyInfo::GetValue": "reflection/member-basics",
    "System.Private.CoreLib/System.Reflection.PropertyInfo::SetValue": "reflection/member-basics",
    # ── attributes/custom-metadata ──
    "System.Private.CoreLib/System.Attribute::GetCustomAttribute": "attributes/custom-metadata",
    "System.Private.CoreLib/System.Attribute::GetCustomAttributes": "attributes/custom-metadata",
    "System.Private.CoreLib/System.Attribute::IsDefined": "attributes/custom-metadata",
    "System.Private.CoreLib/System.Attribute::get_TypeId": "attributes/custom-metadata",
}


def _parse_reflection_helper_detection_code(cs_path: Path) -> dict[str, set[str]]:
    """Parse C# reflection helper detection functions to extract subject IDs.

    Reads ObjectModelUtilities.cs and extracts the subject ID patterns
    from IsTypeReflectionHelperSubjectId, IsAssemblyReflectionHelperSubjectId,
    and IsReflectionMemberHelperSubjectId.
    """
    if not cs_path.is_file():
        return {}

    text = cs_path.read_text(encoding="utf-8")
    results: dict[str, set[str]] = {}

    func_re = re.compile(
        r'(IsTypeReflectionHelperSubjectId|IsAssemblyReflectionHelperSubjectId'
        r'|IsReflectionMemberHelperSubjectId|IsActivationHelperSubjectId)\s*\(string\s+subjectId\)'
    )
    match_call = re.compile(r'MatchesMethodSubject\('
                            r'\s*subjectId\s*,'
                            r'\s*"([^"]+)"\s*,'
                            r'\s*"([^"]+)"'
                            r'(.*?)\)')
    starts_with_re = re.compile(r'subjectId\s*!=\s*null\s*&&\s*subjectId\.StartsWith\(\s*"([^"]+)"')

    current_func: str | None = None
    for line in text.split("\n"):
        fm = func_re.search(line)
        if fm:
            current_func = fm.group(1)
            results[current_func] = set()
            # Parse MatchesMethodSubject calls on the same line
            for mm in match_call.finditer(line):
                declaring_type = mm.group(1)
                method_name = mm.group(2)
                subject_id = f"{declaring_type}::{method_name}"
                results[current_func].add(subject_id)
            # Parse StartsWith calls on the same line
            for sw in starts_with_re.finditer(line):
                results[current_func].add(sw.group(1).rstrip("::"))
            continue

        if current_func:
            for mm in match_call.finditer(line):
                declaring_type = mm.group(1)
                method_name = mm.group(2)
                subject_id = f"{declaring_type}::{method_name}"
                results[current_func].add(subject_id)
            for sw in starts_with_re.finditer(line):
                results[current_func].add(sw.group(1).rstrip("::"))

    return results


def _parse_ledger_reflection_families(ledger_path: Path) -> dict[str, dict]:
    """Parse ledger JSON to extract all reflection/* families."""
    if not ledger_path.is_file():
        return {}

    text = ledger_path.read_text(encoding="utf-8")
    data = json.loads(text)

    reflection_families: dict[str, dict] = {}
    for dll_entry in data.get("dlls", []):
        for fam in dll_entry.get("families", []):
            fam_id = fam.get("familyId", "")
            if "/reflection/" in fam_id or "/attributes/" in fam_id:
                reflection_families[fam_id] = fam

    return reflection_families


def phase4_check_arch_constraints() -> list[str]:
    """MetadataClosure audit: cross-reference codegen reflection detection vs ledger."""
    errors: list[str] = []

    # Paths
    utilities_path = (
        REPO_ROOT
        / "src" / "managed" / "Chaos.IL2CPP.CodeGen"
        / "Emission" / "NativeAotLoweringPlanner.ObjectModelUtilities.cs"
    )
    ledger_path = (
        REPO_ROOT
        / "verification" / "projections" / "foundation-dll-audit"
        / "capability-family-ledger.json"
    )
    emission_path = (
        REPO_ROOT
        / "src" / "managed" / "Chaos.IL2CPP.CodeGen"
        / "Emission" / "NativeAotLoweringPlanner.ReflectionObjectEmission.cs"
    )

    # 4a: Parse codegen reflection helper detection functions
    helper_subjects = _parse_reflection_helper_detection_code(utilities_path)
    if not helper_subjects:
        errors.append("[METADATA/INTERNAL] Could not parse reflection helper detection code")
        return errors

    detected_subjects: set[str] = set()
    for func_name, subjects in helper_subjects.items():
        detected_subjects.update(subjects)

    # 4b: Parse ledger reflection families
    ledger_families = _parse_ledger_reflection_families(ledger_path)
    if not ledger_families:
        errors.append("[METADATA/INTERNAL] Could not parse ledger reflection families")
        return errors

    ledger_method_subjects: set[str] = set()
    ledger_family_by_method: dict[str, str] = {}
    for fam_id, fam in ledger_families.items():
        for ms in fam.get("methodSubjectIds", []):
            ledger_method_subjects.add(ms)
            ledger_family_by_method[ms] = fam_id

    # 4c: Cross-reference: codegen detects but ledger doesn't track
    def is_covered_by_ledger(detected_subject: str, ledger_entries: set[str]) -> bool:
        for le in ledger_entries:
            if le.startswith(detected_subject):
                return True
        return False

    for subject in sorted(detected_subjects):
        family_id = REFLECTION_SUBJECTID_TO_FAMILY.get(subject, "unknown")
        found_in_ledger = is_covered_by_ledger(subject, ledger_method_subjects)
        if not found_in_ledger:
            errors.append(
                f"[METADATA/CODEGEN_ONLY] '{subject}' is detected by codegen "
                f"({family_id}) but not tracked in any reflection/* ledger family. "
                f"Add a methodSubjectIds entry."
            )

    # 4d: Also check: ledger methods that are NOT detectable by codegen at all
    def is_covered_by_ledger(detected_subject: str, ledger_entries: set[str]) -> bool:
        for le in ledger_entries:
            if le.startswith(detected_subject):
                return True
        return False

    ledger_only_count = 0
    for ls in sorted(ledger_method_subjects):
        # Extract short form: remove the signature suffix after the method name.
        # "System.Private.CoreLib/System.Type::get_Name:System.String()" ->
        # "System.Private.CoreLib/System.Type::get_Name"
        short_form = re.sub(r":(?![:/])[^:]*$", "", ls) if "::" in ls else ls
        # More robust: split on ::, take first two parts + method name
        if "::" in ls:
            # Find the :: separator, then find the next : before the signature
            parts = ls.split("::")
            if len(parts) >= 2:
                method_part = parts[1].split(":")[0] if ":" in parts[1] else parts[1]
                short_form = f"{parts[0]}::{method_part}" if method_part else parts[0]
            else:
                short_form = ls
        if not is_covered_by_ledger(short_form, detected_subjects):
            ledger_only_count += 1
    if ledger_only_count > 0:
        print(f"[PHASE 4] {ledger_only_count} ledger methods have no codegen detection", file=sys.stderr)

    # 4d: Check for ledger gate status
    all_pending = True
    for fam_id, fam in ledger_families.items():
        gates = fam.get("verificationGates", {})
        # Only check pre-registered (non-orphan-confirmed) families
        if fam.get("synthesisStatus") == "taxonomy-registered":
            for gate, status in gates.items():
                if status != "pending":
                    all_pending = False
        # Check that families with gates="passed" have method entries
        elif fam.get("synthesisStatus") == "orphan-confirmed":
            method_count = fam.get("methodCount", 0)
            actual_methods = len(fam.get("methodSubjectIds", []))
            if method_count != actual_methods or actual_methods == 0:
                errors.append(
                    f"[METADATA/LEDGER_COUNT] Family '{fam_id}' claims "
                    f"{method_count} methods but has {actual_methods} methodSubjectIds"
                )

    # 4e: Check codegen emission coverage matches ledger expectations
    emission_text = emission_path.read_text(encoding="utf-8") if emission_path.is_file() else ""

    helper_functions_emitted = {
        # String helpers
        "chaos_reflection_get_string_utf8": "string UTF-8 extraction",
        "chaos_reflection_create_string_utf8_copy": "string UTF-8 creation",
        "chaos_reflection_concat_string_pair_values": "string concatenation",
        "chaos_reflection_create_string_literal": "string literal creation",
        "chaos_reflection_create_reference_array": "reference-type array creation",
        # Type reflection
        "chaos_reflection_get_type_name_value_from_handle": "type name lookup",
        "chaos_reflection_get_type_metadata_token_from_handle": "type metadata token",
        "chaos_reflection_get_type_attributes_from_handle": "type attributes",
        "chaos_reflection_get_type_kind_flags_from_handle": "type kind flags (ValueType/Enum)",
        "chaos_reflection_get_type_generic_flags_from_handle": "type generic flags (IsGenericType/etc)",
        "chaos_reflection_get_type_full_name_from_handle": "type FullName",
        "chaos_reflection_get_type_namespace_from_handle": "type Namespace",
        "chaos_reflection_get_type_qualified_name_from_handle": "type AssemblyQualifiedName",
        "chaos_reflection_create_type_value": "Type object creation",
        "chaos_reflection_get_type_from_handle": "Type.GetTypeFromHandle",
        "chaos_reflection_get_type_handle": "Type.get_TypeHandle",
        # Method reflection
        "chaos_reflection_get_method_name_value_from_handle": "method name lookup",
        "chaos_reflection_get_method_metadata_token_from_handle": "method metadata token",
        "chaos_reflection_resolve_method_handle": "method handle by name",
        "chaos_reflection_get_method": "Type.GetMethod dispatch",
        "chaos_reflection_get_method_handle": "MethodBase.get_MethodHandle",
        "chaos_reflection_create_closed_generic_method_handle": "MakeGenericMethod handle",
        "chaos_reflection_make_generic_method": "MakeGenericMethod",
        "chaos_reflection_invoke_method": "MethodBase.Invoke",
        "chaos_reflection_get_methods": "Type.GetMethods",
        "chaos_reflection_get_fields": "Type.GetFields",
        "chaos_reflection_get_nested_types": "Type.GetNestedTypes",
        "chaos_reflection_get_interfaces": "Type.GetInterfaces",
        "chaos_reflection_get_members": "Type.GetMembers",
        # Member support
        "chaos_reflection_get_member_name": "member name dispatch",
        "chaos_reflection_get_declaring_type": "declaring type dispatch",
        "chaos_reflection_get_metadata_token": "metadata token dispatch",
        # Generics
        "chaos_reflection_get_generic_type_definition": "generic type definition",
        "chaos_reflection_get_generic_arguments": "generic arguments",
        "chaos_reflection_make_generic_type": "MakeGenericType",
        # Fields
        "chaos_reflection_get_field": "field lookup",
        "chaos_reflection_get_field_attributes_from_token": "field attributes (IsStatic/InitOnly/Literal)",
        # Parameters
        "chaos_reflection_get_parameter_name": "parameter name",
        # Constructors
        "chaos_reflection_get_constructors": "constructor enumeration",
        # Assembly
        "chaos_reflection_get_assembly": "assembly lookup",
        "chaos_reflection_get_executing_assembly": "Assembly.GetExecutingAssembly",
        "chaos_reflection_get_assembly_types": "Assembly.GetTypes",
        "chaos_reflection_get_assembly_name": "Assembly.GetName",
        "chaos_reflection_get_assembly_name_value": "AssemblyName.get_Name",
        "chaos_reflection_resolve_type_handle": "type handle resolution",
        "chaos_reflection_get_type_from_assembly": "Assembly.GetType",
        "chaos_reflection_get_type_by_name": "Type.GetType(string)",
        # Attributes
        "chaos_reflection_get_custom_attribute": "Attribute.GetCustomAttribute",
        # Exception metadata
        "chaos_reflection_get_exception_message": "Exception.Message",
        "chaos_reflection_get_exception_param_name": "ArgumentException.ParamName",
        "chaos_reflection_set_exception_metadata": "Exception metadata capture",
    }

    for func_name, description in helper_functions_emitted.items():
        if func_name not in emission_text:
            errors.append(
                f"[METADATA/EMISSION_MISSING] '{func_name}' ({description}) "
                f"is not emitted in ReflectionObjectEmission.cs"
            )

    # 4f: Per-family gap analysis
    matched_in_ledger = set()
    for ds in detected_subjects:
        for ls in ledger_method_subjects:
            if ls.startswith(ds):
                matched_in_ledger.add(ds)
                break

    print(f"[PHASE 4] Per-family summary:", file=sys.stderr)
    for short_id in sorted(set(REFLECTION_SUBJECTID_TO_FAMILY.values())):
        fam_detected = {s for s in detected_subjects
                        if REFLECTION_SUBJECTID_TO_FAMILY.get(s) == short_id}
        fam_matched = fam_detected & matched_in_ledger
        d_count = len(fam_detected)
        m_count = len(fam_matched)
        uncovered = d_count - m_count
        status = "OK" if uncovered == 0 else f"GAP ({uncovered} subjects not in ledger)"
        ledger_fam_id = f"family/System.Private.CoreLib/{short_id}"
        l_count = len(ledger_families.get(ledger_fam_id, {}).get("methodSubjectIds", []))
        print(f"  [{status}] {short_id}: codegen detects {d_count} subjects, "
              f"ledger tracks {l_count} methods", file=sys.stderr)
        if uncovered > 0:
            for ds in sorted(fam_detected - fam_matched):
                errors.append(
                    f"[METADATA/CODEGEN_ONLY] In family '{short_id}': "
                    f"'{ds}' detected by codegen but not tracked in ledger"
                )

    # Also check for completely missing families
    all_detected_families = set(REFLECTION_SUBJECTID_TO_FAMILY.values())
    for fam_id_raw, fam in ledger_families.items():
        # Extract short ID like "reflection/type" from "family/System.Private.CoreLib/reflection/type"
        # or "attributes/custom-metadata" from "family/System.Private.CoreLib/attributes/custom-metadata"
        parts = fam_id_raw.split("/")
        matched_id = None
        if len(parts) >= 4:
            # e.g. ['family', 'System.Private.CoreLib', 'reflection', 'type']
            if parts[2] in ("reflection", "attributes"):
                matched_id = f"{parts[2]}/{parts[3]}"
        if matched_id is None:
            matched_id = parts[-1]  # fallback
        if matched_id not in all_detected_families:
            print(f"  [NOCODEGEN] {matched_id}: ledger has methods but no codegen detection mapping", file=sys.stderr)

    activation_detected = {s for s in detected_subjects
                           if REFLECTION_SUBJECTID_TO_FAMILY.get(s) == "reflection/activation"}
    if activation_detected:
        print(f"  [NOTE] reflection/activation: {len(activation_detected)} codegen subjects "
              f"detected (via IsActivationHelperSubjectId)", file=sys.stderr)
    module_detected = {s for s in detected_subjects
                       if REFLECTION_SUBJECTID_TO_FAMILY.get(s) == "reflection/module"}
    if not module_detected:
        print(f"  [NOCODEGEN] reflection/module: no dedicated codegen detection (covered by assembly helpers)", file=sys.stderr)

    # 4g: Report summary counts
    total_detected = len(detected_subjects)
    total_ledger_methods = len(ledger_method_subjects)
    total_families = len(ledger_families)

    print(f"[PHASE 4] Codegen detects {total_detected} reflection helper subject IDs", file=sys.stderr)
    print(f"[PHASE 4] Ledger tracks {total_ledger_methods} methods across {total_families} reflection/attributes families", file=sys.stderr)

    return errors


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(
        description="Chaos IL2CPP Wiki Consistency Checker",
        epilog="Phases: 1=opcode-coverage, 2=file-path-existence, "
        "3=bridge-contracts, 4=metadata-closure-audit",
    )
    parser.add_argument(
        "--ci",
        action="store_true",
        help="CI mode: exit 1 on any issue",
    )
    parser.add_argument(
        "--phase",
        choices=["1", "2", "3", "4", "all"],
        default="all",
        help="Run specific check phase (default: all)",
    )
    parser.add_argument(
        "--verbose",
        action="store_true",
        help="Show detailed progress output",
    )
    return parser.parse_args()


def main() -> None:
    args = parse_args()
    all_errors: list[str] = []

    phase_map = {
        "1": ("Opcode coverage", phase1_check_opcode_coverage),
        "2": ("File path existence", phase2_check_file_paths),
        "3": ("Bridge contract consistency", phase3_check_bridge_contracts),
        "4": ("Architecture constraint verifiability", phase4_check_arch_constraints),
    }

    phases_to_run = (
        list(phase_map.keys()) if args.phase == "all" else [args.phase]
    )

    for phase_key in phases_to_run:
        name, check_fn = phase_map[phase_key]
        if args.verbose:
            print(f"[PHASE {phase_key}] {name}...", file=sys.stderr)
        all_errors.extend(check_fn(verbose=args.verbose) if phase_key == "2" else check_fn())

    if all_errors:
        for error in sorted(all_errors):
            print(error, file=sys.stderr)
        print(f"\n{'=' * 60}", file=sys.stderr)
        print(f"Found {len(all_errors)} wiki consistency issue(s)", file=sys.stderr)
        if args.ci:
            sys.exit(1)
    else:
        print("[OK] All wiki checks passed.")


if __name__ == "__main__":
    main()
