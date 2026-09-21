#!/usr/bin/env python3
"""Scan native runtime stubs for exception types they raise.

Why this exists
---------------
`BuildExceptionTypeTable()` in the Generator emits `kChaosExceptionTypes[]`,
which `ResolveTypeByName()` consults before falling back to a reflection-image
scan.  The consumer is `RaiseManagedException(type_full_name, ...)` — and the
names it is handed come from *hand-written* C++ stubs under
`src/native/runtime-core/runtime_stubs/`.

That directory is not reachable from the Generator at codegen time, so the set
of raised types is carried in C# as data (`FileOnlyExceptionTypeNames` in
`NativeAotLoweringPlanner.ModuleRegistration.Dispatch.cs`).  This script
regenerates it and — more importantly — acts as the **negative control**: if a
stub starts raising a type that is not in the C# list, `ResolveTypeByName`
returns 0 for it, `RaiseManagedException` raises a NULL object, and the typed
assertion fails against correct native behaviour.  That failure mode is silent,
so the check has to be explicit.

Usage
-----
    python tools/scan_raised_exception_types.py            # print the C# list
    python tools/scan_raised_exception_types.py --check     # exit 1 on drift

Exit code 0 = in sync, 1 = drift (or --check found a missing type).
"""

from __future__ import annotations

import argparse
import re
import sys
from pathlib import Path

REPO_ROOT = Path(__file__).resolve().parent.parent
STUB_GLOB_DIR = REPO_ROOT / "src" / "native" / "runtime-core"
DISPATCH_CS = (
    REPO_ROOT
    / "src"
    / "managed"
    / "Chaos.IL2CPP.Generator"
    / "Emission"
    / "NativeAotLoweringPlanner.ModuleRegistration.Dispatch.cs"
)

# `RaiseManagedException(` followed by a string literal; the call is often split
# across lines, so allow whitespace but not a closing paren before the literal.
RAISE_RE = re.compile(r'RaiseManagedException\s*\(\s*"([^"]+)"')


def collect_raised_types() -> dict[str, list[str]]:
    """Map exception type name -> sorted list of files that raise it."""
    found: dict[str, list[str]] = {}
    for path in STUB_GLOB_DIR.rglob("*.cpp"):
        try:
            text = path.read_text(encoding="utf-8", errors="replace")
        except OSError:
            continue
        for match in RAISE_RE.finditer(text):
            name = match.group(1)
            # Only exception-shaped names; guards against a stub passing a
            # message string in the type position.
            if "Exception" not in name:
                continue
            rel = path.relative_to(REPO_ROOT).as_posix()
            found.setdefault(name, []).append(rel)
    return {k: sorted(set(v)) for k, v in found.items()}


def parse_csharp_list() -> set[str]:
    """Extract the current FileOnlyExceptionTypeNames entries from the .cs."""
    text = DISPATCH_CS.read_text(encoding="utf-8")
    # Anchor on the declaration, not the doc-comment mention above it.
    decl = "string[] FileOnlyExceptionTypeNames"
    start = text.find(decl)
    if start < 0:
        raise SystemExit(f"ERROR: FileOnlyExceptionTypeNames not found in {DISPATCH_CS}")
    open_brace = text.find("{", start)
    close_brace = text.find("};", open_brace)
    if open_brace < 0 or close_brace < 0:
        raise SystemExit("ERROR: could not find the body of FileOnlyExceptionTypeNames")
    body = text[open_brace:close_brace]
    return set(re.findall(r'"([^"]+)"', body))


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument(
        "--check",
        action="store_true",
        help="exit 1 if the C# list is out of sync with the native stubs",
    )
    args = parser.parse_args()

    raised = collect_raised_types()
    declared = parse_csharp_list()

    names = sorted(raised)
    if not args.check:
        print("// ── regenerate FileOnlyExceptionTypeNames from this list ──")
        print("private static readonly string[] FileOnlyExceptionTypeNames =")
        print("{")
        for name in names:
            print(f'    "{name}",')
        print("};")
        print()
        print(f"# {len(names)} distinct raised exception types")
        for name in names:
            print(f"#   {name}")
            for f in raised[name]:
                print(f"#       {f}")
        return 0

    missing = sorted(set(raised) - declared)
    stale = sorted(declared - set(raised))

    if not missing and not stale:
        print(f"OK: {len(declared)} raised exception types, C# list in sync.")
        return 0

    print("DRIFT between native stubs and FileOnlyExceptionTypeNames:", file=sys.stderr)
    for name in missing:
        print(f"  MISSING from C# list (raised but not declared): {name}", file=sys.stderr)
        for f in raised[name]:
            print(f"      {f}", file=sys.stderr)
        print(
            "      -> ResolveTypeByName returns 0 for this type; the raised\n"
            "         exception object will be NULL and typed catch will fail.",
            file=sys.stderr,
        )
    for name in stale:
        print(f"  STALE in C# list (declared but never raised): {name}", file=sys.stderr)
    return 1


if __name__ == "__main__":
    sys.exit(main())
