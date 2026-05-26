#!/usr/bin/env python3
"""Check emission source files/scriban templates for bare `new` that should be CHAOS_IL2CPP_NEW_GC.

Scans:
  - src/managed/Chaos.IL2CPP.Generator/Emission/*.cs
  - src/managed/Chaos.IL2CPP.Generator/Templates/*.scriban

Returns non-zero exit code if any bare new is found.
Intended for CI / pre-commit use.

Usage:
    python build/toolchains/run/testing/foundation_dll/check_emission_bare_new.py
    python check_emission_bare_new.py --verbose
    python check_emission_bare_new.py --fix-scriban  # dry-run fix for scriban templates
"""

from __future__ import annotations

import re
import sys
from pathlib import Path


_HERE = Path(__file__).resolve().parent
_REPO_ROOT = _HERE.parents[4]

_EMISSION_DIR = _REPO_ROOT / "src" / "managed" / "Chaos.IL2CPP.Generator" / "Emission"
_TEMPLATES_DIR = _REPO_ROOT / "src" / "managed" / "Chaos.IL2CPP.Generator" / "Templates"

# Patterns of bare `new` in emission C# files (strings emitted as C++ code).
# The C# emission generates C++ via AppendLiteral/AppendLine:
#   AppendLiteral("    auto* chaos_xxx = new Type{};")
# We match the emitted C++ `new` inside the C# string literal.
_CS_BARE_NEW_RE = re.compile(
    r'(?:Append(?:Literal|Line)|\.Append)\('
    r'[^)]*"'
    r'[^"]*\bnew\s+'
    r'(?!CHAOS_IL2CPP_NEW_GC)'
    r'([A-Za-z_]\w*(?:::)?\w*)'
    r'\s*[\{(]'
)

# Patterns of bare `new` in scriban templates:
#   auto* chaos_object = new {{ type }}{};
#   auto* chaos_object = new {{ ctor.native_type_symbol }}{};
_TEMPLATE_BARE_NEW_RE = re.compile(
    r'\bnew\s+'
    r'(\{\{\s*[^}]+\s*\}\})'
    r'\s*\{\}'
)

# ── Whitelist: emitted C++ `new` that is LEGAL ──────────────────────────
# These appear inside C# emission string literals but are Raw domain or
# C# infrastructure, NOT managed object allocations that need CHAOS_IL2CPP_NEW_GC.
_LEGAL_CS_NEW_PATTERNS = [
    r'new char\[',           # temp UTF-8 buffer (Raw domain)
    r'new CHAOS_IL2CPP_',     # already correct macro
    r'new StringBuilder',     # C# infrastructure
    r'new AppendInterpolatedStringHandler',  # C# infrastructure
    r'new HashSet<',          # C# infrastructure
    r'new List<',             # C# infrastructure
    r'new Dictionary<',       # C# infrastructure
    r'new Func<',             # C# infrastructure
    r'new ReadOnlySpan<',     # C# infrastructure
    r'new Span<',             # C# infrastructure
    r'new Action<',           # C# infrastructure
    r'new _003C',             # C# compiler-generated
    r'new CHAOS_IL2CPP_UINT', # Raw domain POD array
    r'new CHAOS_IL2CPP_INT',  # Raw domain POD array
    r'new CHAOS_IL2CPP_SIZE', # Raw domain
    r'new TypeInfoHot',       # Raw domain (module metadata)
    r'new char\*',           # pointer array
    r'new void\*',           # pointer array
]

_LEGAL_CS_NEW_RES = [re.compile(p) for p in _LEGAL_CS_NEW_PATTERNS]


def is_legal_cs_new(line: str) -> bool:
    """Check if a bare-new match is on a whitelisted pattern."""
    for legal_re in _LEGAL_CS_NEW_RES:
        if legal_re.search(line):
            return True
    return False


def check_emission_files(verbose: bool = False) -> list[dict]:
    """Scan emission .cs files for bare new violations.

    Returns list of {file, line_no, line_text, type_name} dicts.
    """
    violations: list[dict] = []
    if not _EMISSION_DIR.exists():
        print(f"Emission directory not found: {_EMISSION_DIR}", file=sys.stderr)
        return violations

    for cs_file in sorted(_EMISSION_DIR.rglob("*.cs")):
        rel = cs_file.relative_to(_REPO_ROOT)
        lines = cs_file.read_text(encoding="utf-8").splitlines()
        for lineno, line in enumerate(lines, 1):
            m = _CS_BARE_NEW_RE.search(line)
            if not m:
                continue
            if is_legal_cs_new(line):
                continue
            violations.append({
                "file": str(rel),
                "line_no": lineno,
                "line": line.strip(),
                "type": m.group(1),
            })
            if verbose:
                print(f"  {rel}:{lineno}  {line.strip()}")

    return violations


def check_scriban_templates(verbose: bool = False) -> list[dict]:
    """Scan scriban templates for bare new violations.

    Returns list of {file, line_no, line_text, template_var} dicts.
    """
    violations: list[dict] = []
    if not _TEMPLATES_DIR.exists():
        print(f"Templates directory not found: {_TEMPLATES_DIR}", file=sys.stderr)
        return violations

    for tpl_file in sorted(_TEMPLATES_DIR.rglob("*.scriban")):
        rel = tpl_file.relative_to(_REPO_ROOT)
        lines = tpl_file.read_text(encoding="utf-8").splitlines()
        for lineno, line in enumerate(lines, 1):
            m = _TEMPLATE_BARE_NEW_RE.search(line)
            if not m:
                continue
            violations.append({
                "file": str(rel),
                "line_no": lineno,
                "line": line.strip(),
                "template_var": m.group(1),
            })
            if verbose:
                print(f"  {rel}:{lineno}  {line.strip()}")

    return violations


def main() -> int:
    verbose = "-v" in sys.argv or "--verbose" in sys.argv

    cs_violations = check_emission_files(verbose)
    tpl_violations = check_scriban_templates(verbose)

    all_violations = cs_violations + tpl_violations

    if not all_violations:
        print("[PASS] No bare `new` found in emission sources or templates.")
        return 0

    print(f"[FAIL] {len(all_violations)} bare `new` violation(s) found:", file=sys.stderr)
    for v in all_violations:
        loc = f"{v['file']}:{v['line_no']}"
        print(f"  {loc}  {v['line']}", file=sys.stderr)

    print(file=sys.stderr)
    print("Each violation is an emitted C++ `new Type{}` that should be", file=sys.stderr)
    print("`CHAOS_IL2CPP_NEW_GC(Type)` so the allocation goes through the", file=sys.stderr)
    print("GC heap instead of malloc.", file=sys.stderr)
    return 1


if __name__ == "__main__":
    raise SystemExit(main())