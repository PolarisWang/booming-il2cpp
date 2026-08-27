#!/usr/bin/env python3
"""Completion-audit check — surface likely-incomplete external-facing functions.

The highest-value semantic-drift signal in an AI-agent repo is the "return-0 loan":
a function does real-looking work (compiles, looks implemented) but ends by
returning a bare literal default (e.g. `return 0;`) instead of the computed result —
the managed side then silently gets 0. Crypto hashes are a concrete instance.

This check is DELIBERATELY conservative and ADVISORY:
  - It only flags functions whose body is non-trivial (real computation) AND whose
    LAST statement is a bare literal return (`return 0;`, `return 0 }`, `return {};`,
    `return nullptr;`) — a "does work then throws the result away" shape.
  - It does NOT flag guard-clause `if (ptr==nullptr) return 0;` (those are correct).
  - Output is a WARN candidate list for human/AI review; NOT a hard gate (a function
    may legitimately return 0 on its contract).

Usage (advisory, exit 0 unless a self-consistent FAIL is found; --fail-fast -> exit 1):
  python check_completion_audit.py           # list suspected-incomplete (WARN)
  python check_completion_audit.py --ci      # same, exit 1 if any found
  Orchestrated by chaos_hygiene.py (hygiene-registry.json "completion-audit").
"""

import re
import sys
from pathlib import Path

REPO_ROOT = Path(__file__).resolve().parent.parent.parent

# Directories scanned for exported runtime-stub files.
STUB_DIRS = [
    REPO_ROOT / "src/native/runtime-core/runtime_stubs",
]

# A final lone literal-return that signals "threw away the real result".
_HOLLOW_RETURN = re.compile(
    r"^\s*return\s*(?:0|0\s*;|-?\d+\s*;|nullptr\s*;|\{\}\s*;|false\s*;|true\s*;)\s*$",
    re.MULTILINE,
)


def _functions(file: Path):
    """Yield (name, body) for each top-level function definition in a .cpp file.
    Crude but sufficient: matches `rettype name(params) { ... }` at brace depth 0."""
    txt = file.read_text(encoding="utf-8", errors="replace")
    # strip line comments only at statement level matters little; parse braces
    depth = 0
    buf = []
    current = None
    sig_re = re.compile(r"^([A-Za-z_][\w:<>,*&\s]*\s+([A-Za-z_]\w*)\s*\([^;{}]*\))\s*\{", re.MULTILINE)
    i = 0
    n = len(txt)
    # simplistic: find fns by scanning for '{' after a signature-looking prefix
    for m in sig_re.finditer(txt):
        start = m.end()  # position right after '{'
        depth = 1
        j = start
        while depth > 0 and j < n:
            c = txt[j]
            if c == '{':
                depth += 1
            elif c == '}':
                depth -= 1
            j += 1
        body = txt[start:j - 1]
        yield m.group(2), body
    return


def _is_hollow(decl_name: str, body: str) -> bool:
    """True if the function body is substantial AND its last statement is a bare
    literal return — the 'does work then discards the result' hole shape."""
    # ignore pure getters/short bodies: < some threshold lines = probably intentional
    body_lines = body.splitlines()
    code = [ln for ln in body_lines if ln.strip() and not ln.strip().startswith(('//', '/*', '*'))]
    if len(code) < 6:  # too short to "do real work and discard" — not a convincing hole
        return False
    # last non-comment statement is a bare literal return
    tail = [ln.strip() for ln in reversed(code) if ln.strip()]
    if not tail:
        return False
    return bool(_HOLLOW_RETURN.match(tail[0]))


def main() -> int:
    flagged = []
    for d in STUB_DIRS:
        if not d.exists():
            continue
        for f in sorted(d.glob("*.cpp")):
            for name, body in _functions(f):
                if _is_hollow(name, body):
                    flagged.append((f.name, name))

    if flagged:
        print("[WARN] suspected-incomplete functions (real body then bare default return):")
        for fn, name in flagged:
            print(f"  {fn}: {name}")
        print("  Review each: if it returns a sentinel instead of the computed result, the")
        print("  managed side silently gets a default — complete it or mark the stub explicitly.")
        if "--ci" in sys.argv:
            print("[FAIL] completion-audit found suspected-incomplete functions (--ci)")
            return 1
        return 0
    print("[PASS] no suspected-incomplete functions found")
    return 0


if __name__ == "__main__":
    sys.exit(main())
