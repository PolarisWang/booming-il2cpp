#!/usr/bin/env python3
"""build/check_arch.py — Native C++ architecture compliance checker.

Usage:
    python build/check_arch.py --module chaos_runtime_core
    python build/check_arch.py --file src/native/runtime-core/foo.cpp
    python build/check_arch.py --ci          # all modules, CI exit code
    python build/check_arch.py --ci --fix    # CI mode + auto-fix

Reads build/architecture.yaml for module definitions and rules.
Exits 0 on success, 1 on any architecture issue (--ci mode).
"""

import argparse
import os
import re
import sys
from pathlib import Path

PROJECT_ROOT = Path(__file__).resolve().parent.parent
YAML_PATH = PROJECT_ROOT / "build" / "architecture.yaml"
NATIVE_SRC = PROJECT_ROOT / "src" / "native"


# ── Load architecture YAML (simple parser, no PyYAML dependency) ──────────

def load_arch_yaml(path):
    """Minimal YAML parser for build/architecture.yaml. Returns a dict."""
    with open(path, encoding="utf-8") as f:
        text = f.read()
    return _parse_yaml(text)


def _parse_yaml(text):
    """Parse a restricted subset of YAML (no nested sequences, simple scalars)."""
    result = {}
    modules = []
    current_module = None
    namespace_aliases = {}
    rules = {}
    in_modules = False
    in_aliases = False
    in_rules = False
    for line in text.split("\n"):
        stripped = line.strip()
        if not stripped or stripped.startswith("#"):
            continue
        if stripped == "modules:":
            in_modules, in_aliases, in_rules = True, False, False
            continue
        if stripped == "namespace_aliases:":
            in_modules, in_aliases, in_rules = False, True, False
            continue
        if stripped == "rules:":
            in_modules, in_aliases, in_rules = False, False, True
            continue
        if in_modules and stripped.startswith("- name:"):
            if current_module:
                modules.append(current_module)
            current_module = {"name": stripped.split(":", 1)[1].strip()}
            continue
        if in_modules and current_module and ":" in stripped and not stripped.startswith("-"):
            key, val = stripped.split(":", 1)
            current_module[key.strip()] = val.strip()
            continue
        if in_aliases and ":" in stripped:
            key, val = stripped.split(":", 1)
            val = val.strip().strip("[]").replace(" ", "").split(",")
            namespace_aliases[key.strip()] = val
            continue
        if in_rules and ":" in stripped:
            key, val = stripped.split(":", 1)
            val = val.strip()
            if val.lower() == "true":
                val = True
            elif val.lower() == "false":
                val = False
            rules[key.strip()] = val
            continue
    if current_module:
        modules.append(current_module)
    result["modules"] = modules
    result["namespace_aliases"] = namespace_aliases
    result["rules"] = rules
    return result


# ── Module detection: map a file path to its module name ──────────────────

def module_for_file(rel_path, modules):
    """Return (module_name, module_info) for a relative native source path."""
    for m in modules:
        prefix = m["path"].replace("\\", "/")
        if rel_path.startswith(prefix):
            # Check it's really within this module's dir
            remainder = rel_path[len(prefix):]
            if remainder.startswith("/") or remainder == "":
                return m["name"], m
    return None, None


def allowed_inherits(module_name, modules):
    """Return set of module names this module may include from."""
    for m in modules:
        if m["name"] == module_name:
            inherits = m.get("inherits_from", [])
            return set(inherits) | {module_name}
    return {module_name}


def expected_namespace(module_name, modules, aliases):
    """Return the expected C++ namespace for a module."""
    for m in modules:
        if m["name"] == module_name:
            ns = m.get("namespace", "")
            return ns
    # Check aliases
    for alias_ns, mods in aliases.items():
        if module_name in mods:
            return alias_ns
    return ""


# ── Checks ────────────────────────────────────────────────────────────────

class ArchIssue:
    def __init__(self, filepath, line, severity, rule, message):
        self.filepath = filepath
        self.line = line
        self.severity = severity  # "error" or "warning"
        self.rule = rule
        self.message = message

    def __str__(self):
        loc = f"{self.filepath}:{self.line}" if self.line else self.filepath
        return f"[{self.severity}] {loc}: {self.rule} — {self.message}"


def run_checks(filepath, modules, rules, aliases, fix=False):
    """Run all applicable checks on a single file. Returns (issues, fix_count)."""
    issues = []
    fix_count = 0

    if not os.path.isfile(filepath):
        return issues, fix_count

    try:
        with open(filepath, encoding="utf-8", errors="replace") as f:
            lines = f.readlines()
    except Exception as e:
        issues.append(ArchIssue(filepath, 0, "error", "read-error", str(e)))
        return issues, fix_count

    rel_path = os.path.relpath(filepath, PROJECT_ROOT).replace("\\", "/")
    module_name, _ = module_for_file(rel_path, modules)
    is_header = filepath.endswith(".h")
    is_cpp = filepath.endswith(".cpp")
    basename = os.path.basename(filepath)
    bare_name = os.path.splitext(basename)[0]
    header_name = bare_name + ".h"

    # ── Check 1: ban #pragma once ─────────────────────────────────────
    if rules.get("ban_pragma_once", True) and is_header:
        for i, line in enumerate(lines, 1):
            if re.match(r"#pragma\s+once", line):
                if fix:
                    guard = _generate_guard(rel_path, module_name, modules)
                    if guard:
                        lines[i - 1] = f"#ifndef {guard}\n"
                        lines.insert(i, f"#define {guard}\n")
                        fix_count += 1
                    else:
                        issues.append(ArchIssue(
                            rel_path, i, "warning", "pragma-once",
                            "Cannot auto-fix #pragma once: unknown module"))
                else:
                    issues.append(ArchIssue(
                        rel_path, i, "error", "pragma-once",
                        "Use #ifndef/#define guard instead of #pragma once"))
                break  # only report once

    # ── Check 2: include guard format ─────────────────────────────────
    if is_header:
        has_ifndef = False
        for i, line in enumerate(lines, 1):
            m = re.match(r"#ifndef\s+(CHAOS_IL2CPP_\w+_H_)", line)
            if m:
                has_ifndef = True
                guard = m.group(1)
                expected = _generate_guard(rel_path, module_name, modules)
                if expected and guard != expected:
                    if fix:
                        # Replace the guard name in #ifndef, #define, #endif
                        for j in range(i - 1, len(lines)):
                            lines[j] = lines[j].replace(guard, expected)
                        fix_count += 1
                    else:
                        issues.append(ArchIssue(
                            rel_path, i, "warning", "guard-format",
                            f"Include guard '{guard}' should be '{expected}'"))
                break
        if not has_ifndef:
            issues.append(ArchIssue(
                rel_path, 1, "warning", "guard-missing",
                "Header file missing include guard"))

    # ── Check 3: no using namespace in headers ────────────────────────
    if rules.get("ban_using_namespace_in_headers", True) and is_header:
        for i, line in enumerate(lines, 1):
            stripped = line.strip()
            # Skip comments and string literals
            if stripped.startswith("//") or stripped.startswith("/*"):
                continue
            m = re.match(r"using\s+namespace\s+\w", stripped)
            if m:
                issues.append(ArchIssue(
                    rel_path, i, "error", "using-namespace",
                    "No 'using namespace' in header files"))

    # ── Check 4: no relative includes ──────────────────────────────────
    if rules.get("ban_relative_include", True):
        for i, line in enumerate(lines, 1):
            if re.match(r'#include\s+"\.\./', line):
                issues.append(ArchIssue(
                    rel_path, i, "error", "relative-include",
                    "Use absolute-style includes (<module/header.h>) instead of '../'"))

    # ── Check 5: no #include "chaos/..." quote style ──────────────────
    if rules.get("ban_quoted_chaos_include", True):
        for i, line in enumerate(lines, 1):
            if re.search(r'#include\s+"chaos/', line):
                if fix:
                    m = re.search(r'#include\s+"(chaos/[^"]+)"', line)
                    if m:
                        lines[i - 1] = f"#include <{m.group(1)}>\n"
                        fix_count += 1
                else:
                    issues.append(ArchIssue(
                        rel_path, i, "error", "quoted-chaos-include",
                        "Use <chaos/header.h> instead of \"chaos/header.h\""))

    # ── Check 6: no NULL macro ────────────────────────────────────────
    if rules.get("ban_null_macro", True):
        for i, line in enumerate(lines, 1):
            stripped = line.strip()
            if stripped.startswith("//") or stripped.startswith("/*"):
                continue
            if re.search(r'\bNULL\b', stripped):
                issues.append(ArchIssue(
                    rel_path, i, "warning", "null-macro",
                    "Use 'nullptr' instead of 'NULL'"))

    # ── Check 7: first include in .cpp must be matching .h ────────────
    if rules.get("enforce_first_include", True) and is_cpp:
        found_header = False
        for i, line in enumerate(lines, 1):
            stripped = line.strip()
            if stripped.startswith("//") or stripped.startswith("/*"):
                continue
            if stripped == "":
                continue
            # Check if first non-comment include is the matching header
            m = re.search(r'#include\s+[<"]' + re.escape(bare_name) + r'\.h[>"]', stripped)
            if m:
                found_header = True
                break
            if re.match(r'#include\s+[<"]', stripped):
                # First include is something else
                issues.append(ArchIssue(
                    rel_path, i, "warning", "first-include",
                    f"First non-comment include should be \"{header_name}\""))
                break
        # If we never found any include at all, that's fine (small files)

    # ── Check 8: namespace matches module (simple heuristic) ──────────
    if module_name and is_header:
        expected_ns = expected_namespace(module_name, modules, aliases)
        if expected_ns and expected_ns != "chaos::il2cpp::common":
            # Only check headers that have a namespace block
            for i, line in enumerate(lines, 1):
                m = re.search(r'namespace\s+([a-z_:]+)\s*\{', line)
                if m:
                    found = m.group(1).rstrip("::")
                    # Extract expected short suffix (e.g., runtime_core from chaos::il2cpp::runtime_core)
                    expected_suffix = expected_ns.split("::")[-1]
                    found_suffix = found.split("::")[-1]
                    if found_suffix != expected_suffix:
                        # Check if it's a known alias
                        is_alias = False
                        for alias_ns in aliases:
                            if found == alias_ns or found.endswith("::" + alias_ns.split("::")[-1]):
                                is_alias = True
                                break
                        if not is_alias:
                            issues.append(ArchIssue(
                                rel_path, i, "warning", "namespace",
                                f"Expected namespace '{expected_ns}', found '{found}'"))
                    break

    if fix and fix_count > 0:
        with open(filepath, "w", encoding="utf-8") as f:
            f.writelines(lines)

    return issues, fix_count


def _generate_guard(rel_path, module_name, modules):
    """Generate expected include guard for a file."""
    if module_name:
        for m in modules:
            if m["name"] == module_name:
                prefix = m.get("guard_prefix", "CHAOS_IL2CPP")
                base = os.path.basename(rel_path)
                name_part = base.replace(".", "_").upper()
                return f"{prefix}_{name_part}"
    # Fallback: use path-based generation
    parts = rel_path.replace("src/native/", "").replace("/", "_").replace(".", "_").upper()
    return f"CHAOS_IL2CPP_{parts}"


# ── Main ──────────────────────────────────────────────────────────────────

def main():
    parser = argparse.ArgumentParser(
        description="Chaos IL2CPP native C++ architecture compliance checker")
    group = parser.add_mutually_exclusive_group()
    group.add_argument("--module", metavar="NAME",
                       help="Check all .cpp/.h files in a specific module")
    group.add_argument("--file", metavar="PATH",
                       help="Check a single file (relative or absolute path)")
    group.add_argument("--ci", action="store_true",
                       help="Check all native modules; exit 1 on any issue")
    parser.add_argument("--fix", action="store_true",
                        help="Auto-fix issues where possible")
    args = parser.parse_args()

    if not YAML_PATH.exists():
        print(f"[check_arch] ERROR: {YAML_PATH} not found", file=sys.stderr)
        sys.exit(1)

    arch = load_arch_yaml(YAML_PATH)
    modules = arch.get("modules", [])
    rules = arch.get("rules", {})
    aliases = arch.get("namespace_aliases", {})

    # Collect files to check
    files_to_check = []

    if args.module:
        found = False
        for m in modules:
            if m["name"] == args.module:
                mod_dir = PROJECT_ROOT / m["path"]
                if mod_dir.exists():
                    files_to_check.extend(
                        sorted(str(p) for p in mod_dir.rglob("*") if p.suffix in (".cpp", ".h")))
                found = True
                break
        if not found:
            print(f"[check_arch] ERROR: Unknown module '{args.module}'", file=sys.stderr)
            sys.exit(1)

    elif args.file:
        fp = Path(args.file)
        if not fp.is_absolute():
            fp = PROJECT_ROOT / fp
        if fp.exists() and fp.suffix in (".cpp", ".h"):
            files_to_check.append(str(fp))
        else:
            print(f"[check_arch] ERROR: File not found or not C++: {fp}", file=sys.stderr)
            sys.exit(1)

    elif args.ci:
        for m in modules:
            mod_dir = PROJECT_ROOT / m["path"]
            if mod_dir.exists():
                files_to_check.extend(
                    sorted(str(p) for p in mod_dir.rglob("*") if p.suffix in (".cpp", ".h")))

    else:
        parser.print_help()
        sys.exit(0)

    # Run checks
    all_issues = []
    total_fixes = 0
    for f in files_to_check:
        rel = os.path.relpath(f, PROJECT_ROOT).replace("\\", "/")
        issues, fixed = run_checks(f, modules, rules, aliases, fix=args.fix)
        for iss in issues:
            print(str(iss))
        all_issues.extend(issues)
        total_fixes += fixed

    errors = [i for i in all_issues if i.severity == "error"]
    warnings = [i for i in all_issues if i.severity == "warning"]

    summary = (
        f"\n[check_arch] {len(errors)} error(s), {len(warnings)} warning(s)"
        f" across {len(files_to_check)} file(s)"
    )
    if total_fixes:
        summary += f" [{total_fixes} auto-fix(es) applied]"
    print(summary)

    if errors:
        sys.exit(1)
    sys.exit(0)


if __name__ == "__main__":
    main()
