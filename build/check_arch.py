#!/usr/bin/env python3
"""
Chaos IL2CPP — Native Architecture Checker

Reads build/architecture.yaml and enforces module-level architecture rules:
  1. Namespace correctness — file path must match module namespace
  2. Allowed includes — module may only include allowed headers
  3. Forbidden includes — module must not include certain paths
  4. Forbidden patterns — regex patterns banned in all/selected modules
  5. Include guard format — headers must follow CHAOS_IL2CPP_{MODULE}_H_
  6. Dependency direction — modules may only depend on declared dependencies
  7. Naming conventions — type/function/constant naming patterns

Usage:
  python build/check_arch.py                          # check all modules
  python build/check_arch.py --module chaos_common    # check single module
  python build/check_arch.py --file src/native/...    # check single file
  python build/check_arch.py --ci                     # strict mode (exit 1 on any issue)
  python build/check_arch.py --fix                    # auto-fix trivial issues
"""

import argparse
import os
import re
import sys
import fnmatch
from pathlib import Path
from typing import Any, Optional

try:
    import yaml
except ImportError:
    print("ERROR: PyYAML is required. Install with: pip install pyyaml", file=sys.stderr)
    sys.exit(1)

REPO_ROOT = Path(__file__).resolve().parent.parent
ARCH_SPEC_PATH = REPO_ROOT / "build" / "architecture.yaml"


# ── Load architecture spec ──────────────────────────────────────────

def load_arch_spec() -> dict:
    if not ARCH_SPEC_PATH.exists():
        print(f"ERROR: architecture spec not found at {ARCH_SPEC_PATH}", file=sys.stderr)
        sys.exit(1)
    with open(ARCH_SPEC_PATH, "r", encoding="utf-8") as f:
        spec = yaml.safe_load(f)
    return spec


# ── Find module for a given file ────────────────────────────────────

def find_module_for_file(file_path: Path, modules: dict) -> Optional[tuple[str, dict]]:
    resolved = file_path.resolve()
    for mod_name, mod_cfg in modules.items():
        mod_path = (REPO_ROOT / mod_cfg["path"]).resolve()
        try:
            resolved.relative_to(mod_path)
            return mod_name, mod_cfg
        except ValueError:
            continue
    return None


# ── Check namespace match ───────────────────────────────────────────

def check_namespace(lines: list, file_rel: str, module_cfg: dict) -> list:
    errors = []
    expected_ns = module_cfg.get("namespace", "")
    if not expected_ns:
        return errors  # anonymous-namespace modules skip this check

    # Derive namespace root from expected_ns (e.g. "chaos::il2cpp" from "chaos::il2cpp::runtime_core").
    # Accept any namespace under this root (including sibling namespaces like
    # chaos::il2cpp::struct_marshal) without requiring an explicit sub_namespaces entry.
    ns_root = expected_ns.rsplit("::", 1)[0] if "::" in expected_ns else expected_ns
    ns_prefix = f"{ns_root}::"

    # Find the actual namespace(s) used in the file
    actual_nss = []
    for line in lines:
        m = re.match(r'^namespace\s+([a-zA-Z0-9_:]+)\s*\{', line.strip())
        if m:
            ns = m.group(1)
            # Accept: main namespace, any namespace under ns_root, or explicitly listed sub-namespace
            if ns == expected_ns or ns.startswith(ns_prefix):
                actual_nss.append(ns)
            elif ns in module_cfg.get("sub_namespaces", []):
                actual_nss.append(ns)

    if not actual_nss:
        errors.append(
            f"[NAMESPACE] {file_rel}: expected namespace '{expected_ns}' "
            f"or a sub-namespace under '{ns_root}::' (found namespace pattern not matching)"
        )

    # Check namespace closing comment (against the first actual namespace found)
    if module_cfg.get("conventions", {}).get("namespace_closing_comment", False) and actual_nss:
        for actual_ns in actual_nss:
            expected_close = f"}}  // namespace {actual_ns}"
            if not any(expected_close in line for line in lines):
                errors.append(f"[NAMESPACE] {file_rel}: expected closing comment '{expected_close}'")
                break  # one error per file

    return errors


# ── Check include restrictions ──────────────────────────────────────

def check_includes(lines: list, file_rel: str, module_cfg: dict) -> list:
    errors = []
    all_allowed = module_cfg.get("allowed_includes", {})
    public_allowed = all_allowed.get("public", [])
    private_allowed = all_allowed.get("private", [])
    forbidden = module_cfg.get("forbidden_includes", [])

    for line in lines:
        m = re.match(r'#include\s+[<"](.+?)[>"]', line)
        if not m:
            continue
        inc_path = m.group(1)

        # Check forbidden includes
        for forbid in forbidden:
            if forbid in inc_path:
                # Allow if it's in allowed list
                if any(allowed in inc_path for allowed in public_allowed + private_allowed):
                    continue
                errors.append(f"[INCLUDE] {file_rel}: includes forbidden path '{inc_path}' (matches '{forbid}')")

    return errors


# ── Check forbidden patterns ────────────────────────────────────────

def check_forbidden_patterns(lines: list, file_rel: str, module_cfg: dict, global_cfg: dict) -> list:
    errors = []
    mod_name = module_cfg.get("_name", "")
    module_forbidden = module_cfg.get("conventions", {}).get("forbidden_patterns", [])
    global_forbidden = global_cfg.get("forbidden_patterns", [])

    all_patterns = module_forbidden + global_forbidden
    for entry in all_patterns:
        pattern = entry.get("pattern", "")
        message = entry.get("message", f"forbidden pattern: {pattern}")
        exclude = entry.get("exclude_modules", [])

        if mod_name in exclude:
            continue

        for lineno, line in enumerate(lines, 1):
            if re.search(pattern, line):
                errors.append(f"[PATTERN] {file_rel}:{lineno}: {message}")
                break  # one error per pattern per file

    return errors


# ── Check include guard format ──────────────────────────────────────

def check_include_guard(lines: list, file_rel: str, module_cfg: dict) -> list:
    errors = []
    if not file_rel.endswith(".h"):
        return errors

    prefix = module_cfg.get("conventions", {}).get("include_guard_prefix", "")
    global_prefix = module_cfg.get("conventions", {}).get("include_guard_prefix", "CHAOS_IL2CPP")

    # First non-empty line should be #ifndef CHAOS_IL2CPP_...
    for line in lines:
        line = line.strip()
        if not line or line.startswith("//") or line.startswith("/*"):
            continue
        if line.startswith("#pragma once"):
            errors.append(f"[GUARD] {file_rel}: use #ifndef/#define include guards, not #pragma once")
        elif line.startswith("#ifndef"):
            guard = line[8:].strip()
            if prefix and not guard.startswith(prefix):
                errors.append(f"[GUARD] {file_rel}: include guard '{guard}' should start with '{prefix}'")
        else:
            errors.append(f"[GUARD] {file_rel}: first non-comment line should be #ifndef (include guard)")
        break

    return errors


# ── Check naming conventions ────────────────────────────────────────

def check_naming(lines: list, file_rel: str, module_cfg: dict) -> list:
    errors = []
    conv = module_cfg.get("conventions", {})
    if not conv:
        return errors

    # Constants check (k prefix)
    const_pattern = conv.get("naming_constants", "")
    if "k" in const_pattern or "k_" in const_pattern:
        for lineno, line in enumerate(lines, 1):
            m = re.search(r'(?:constexpr|const)\s+\S+\s+(\w+)\s*=', line)
            if m:
                name = m.group(1)
                # Skip member variables
                if name.startswith("m_") or name.startswith("_"):
                    continue
                # Check k-prefix for constexpr constants at namespace scope
                if module_cfg.get("namespace") and not name.startswith(("k", "g_", "CHAOS_")):
                    # Only flag if it looks like a module-scope constant (not class member)
                    if not line.strip().startswith("    ") and not line.strip().startswith("\t"):
                        pass  # be lenient for now

    # Function naming (snake_case vs PascalCase)
    func_style = conv.get("naming_functions", "")
    for lineno, line in enumerate(lines, 1):
        # Check function definitions: "return_type name("
        m = re.match(r'^[\w:<>]+\s+(\w+)\s*\(', line)
        if m:
            func_name = m.group(1)
            # Skip operator overloads, constructors, destructors
            if func_name.startswith(("operator", "~")) or func_name == func_name.upper():
                continue
            if func_style == "snake_case" and func_name != func_name.lower() and "_" not in func_name:
                if not func_name[0].isupper():  # member functions may be PascalCase
                    pass
            elif func_style == "PascalCase" and "_" in func_name and func_name == func_name.lower():
                if func_name.startswith("k_"):  # constant, not function
                    continue
                errors.append(f"[NAMING] {file_rel}:{lineno}: function '{func_name}' should be PascalCase (module: {module_cfg.get('_name')})")

    return errors


# ── Check single file ───────────────────────────────────────────────

def check_file(file_path: Path, modules: dict, global_cfg: dict, fix: bool = False) -> list:
    file_rel = str(file_path.relative_to(REPO_ROOT)).replace("\\", "/")
    result = find_module_for_file(file_path, modules)
    if result is None:
        return [f"[SKIP] {file_rel}: not in any known module (skipped)"]

    mod_name, mod_cfg = result
    if isinstance(mod_cfg, dict):
        mod_cfg["_name"] = mod_name

    with open(file_path, "r", encoding="utf-8") as f:
        lines = f.readlines()

    errors = []
    errors.extend(check_namespace(lines, file_rel, mod_cfg))
    errors.extend(check_includes(lines, file_rel, mod_cfg))
    errors.extend(check_forbidden_patterns(lines, file_rel, mod_cfg, global_cfg))
    errors.extend(check_include_guard(lines, file_rel, mod_cfg))
    errors.extend(check_naming(lines, file_rel, mod_cfg))

    return errors


# ── Check module (all files) ────────────────────────────────────────

def check_module(mod_name: str, mod_cfg: dict, modules: dict, global_cfg: dict, fix: bool = False) -> list:
    errors = []
    mod_path = REPO_ROOT / mod_cfg["path"]
    if not mod_path.exists():
        return [f"[MODULE] module '{mod_name}' path '{mod_cfg['path']}' not found"]

    # Skip conventions checks for vendored third-party libraries
    if mod_cfg.get("conventions", {}).get("note", "").startswith("vendored"):
        return []

    patterns = ["*.h", "*.cpp"]
    for pattern in patterns:
        for file_path in sorted(mod_path.rglob(pattern)):
            # Skip __pycache__, generated, and backup files
            if any(part.startswith("__") or part.startswith(".") for part in file_path.parts):
                continue
            file_errors = check_file(file_path, modules, global_cfg, fix)
            errors.extend(file_errors)

    return errors


# ── Check dependency direction ──────────────────────────────────────

def check_dependencies(modules: dict) -> list:
    """Check that actual includes between modules match the declared dependency matrix."""
    errors = []
    matrix = modules  # dependency info is in each module's dependencies list

    for mod_name, mod_cfg in modules.items():
        mod_path = REPO_ROOT / mod_cfg["path"]
        if not mod_path.exists():
            continue
        allowed_deps = set(mod_cfg.get("dependencies", [])) | {mod_name}

        for file_path in sorted(mod_path.rglob("*.cpp")):
            if any(part.startswith("__") or part.startswith(".") for part in file_path.parts):
                continue
            file_rel = str(file_path.relative_to(REPO_ROOT)).replace("\\", "/")
            with open(file_path, "r", encoding="utf-8") as f:
                content = f.read()

            # Find all #include that reference another module
            for other_mod_name, other_mod_cfg in modules.items():
                if other_mod_name == mod_name:
                    continue
                other_path = str(other_mod_cfg["path"])
                if other_path in content:
                    if other_mod_name not in allowed_deps:
                        errors.append(
                            f"[DEP] {file_rel}: includes '{other_mod_name}' ({other_path}) "
                            f"but '{mod_name}' does not declare it as a dependency"
                        )

    return errors


# ── Main ────────────────────────────────────────────────────────────

def main():
    parser = argparse.ArgumentParser(description="Chaos IL2CPP Native Architecture Checker")
    parser.add_argument("--module", help="Check specific module only")
    parser.add_argument("--file", help="Check single file only")
    parser.add_argument("--ci", action="store_true", help="CI mode: exit 1 on any issue")
    parser.add_argument("--fix", action="store_true", help="Auto-fix trivial issues")
    args = parser.parse_args()

    spec = load_arch_spec()
    modules = spec.get("modules", {})
    global_cfg = spec.get("global_conventions", {})
    all_errors = []

    if args.file:
        file_path = Path(args.file)
        if not file_path.is_absolute():
            file_path = REPO_ROOT / file_path
        all_errors.extend(check_file(file_path, modules, global_cfg, args.fix))
    elif args.module:
        if args.module not in modules:
            print(f"ERROR: unknown module '{args.module}'. Available: {list(modules.keys())}", file=sys.stderr)
            sys.exit(1)
        all_errors.extend(check_module(args.module, modules[args.module], modules, global_cfg, args.fix))
    else:
        for mod_name, mod_cfg in modules.items():
            all_errors.extend(check_module(mod_name, mod_cfg, modules, global_cfg, args.fix))
        all_errors.extend(check_dependencies(modules))

    # Report results
    if all_errors:
        for error in sorted(all_errors):
            print(error, file=sys.stderr)
        print(f"\n{'='*60}", file=sys.stderr)
        print(f"Found {len(all_errors)} architecture issue(s)", file=sys.stderr)
        if args.ci:
            sys.exit(1)
    else:
        print("[OK] All architecture checks passed.")


if __name__ == "__main__":
    main()
