#!/usr/bin/env python3
"""EH iron-rule checker for staged changes.

Enforces the rules in .ai/skills/library/skills/dev-il2cpp-eh-expert/SKILL.md
that are statically detectable.  The rest (three-mode equivalence, negative
controls, method-table correctness) are runtime properties and are covered by
the eh-mode-gate workflow, not here.

WHY THIS EXISTS
---------------
The L2 wrapper defect that motivated the EH roadmap was a single hardcoded
C++ `try { } catch (const chaos_managed_exception&)` in the emitter.  It
compiled, it passed every existing test, and it was wrong on two of the three
delivery modes.  Nothing in the review path could see it, because the evidence
only exists in generated output, and generated output is not reviewed.

This checker looks at the *source* that produces that output.  It is
deliberately narrow: only patterns that are unambiguously wrong, so it never
forces a reviewer to justify a legitimate use.

DETECTED (rule numbers from the Expert SKILL)

  RULE-1  Hardcoded EH mechanism in emitter source
          Any C# string literal emitted into generated C++ that contains a
          bare `try {`, `catch (...)` or `throw ...` in the EH domain.
          Generated code must go through CHAOS_EH_* (see chaos/eh.h).

  RULE-3  Exception type not registered
          A `RaiseManagedException("X.Y.Z", ...)` call whose type name is
          absent from the codegen exception table.  Advisory only here:
          tools/scan_raised_exception_types.py --check is the authority.

WARN-ONLY BY DESIGN: this script never blocks on a heuristic.  Each finding
prints the rule, the file:line, and what to do.  Exit code is non-zero only
for RULE-1 violations, which have no legitimate form.
"""

from __future__ import annotations

import re
import subprocess
import sys
from pathlib import Path

REPO_ROOT = Path(__file__).resolve().parent.parent

# Files whose text becomes generated C++.  A bare try/catch here is rule-1.
EMITTER_PATH_MARKERS = (
    "Chaos.IL2CPP.Generator/",
)

# Emitter code writes generated C++ through several shapes, all of which must
# be covered — missing one means the check silently passes on a real violation:
#   builder.AppendLine("...try {")
#   builder.Append($"... catch (...)")
#   bodyLines.Add("    throw chaos_managed_exception{};")
#   sb.AppendLine(indent + "try {")
# So: any method call whose argument list contains a string literal carrying the
# offending construct.  Matching on the literal content (rather than the method
# name) is what makes this robust to new helper names.
_BARE_TRY = re.compile(r'''"[^"]*?\btry\s*\{''')
_BARE_CATCH = re.compile(r'''"[^"]*?\bcatch\s*\([^)]*chaos_managed_exception''')
_BARE_THROW = re.compile(r'''"[^"]*?\bthrow\s+chaos_managed_exception''')

# A literal can carry a C++ COMMENT rather than code — e.g. an explanatory
# comment emitted alongside the real statement.  Those are legitimate and must
# not be flagged; the first version reported one of them, which is exactly the
# false-positive class that makes a checker get bypassed.
_LITERAL_IS_COMMENT = re.compile(r'''"\s*(?:\\t|\\s|\s)*(?://|/\*)''')

# Comments legitimately mention these patterns when explaining history.
_COMMENT_LINE = re.compile(r"^\s*(//|/\*|\*)")

# A line must actually be feeding a C++ text buffer to count.  These are the
# append shapes the emitter uses; a plain string literal in, say, a log message
# or a path is not generated code.
_APPEND_SHAPES = (
    ".Append(", ".AppendLine(", ".Add(", ".AddRange(", "bodyLines.Add(", "return $\"",
)

# Extracts each individual "..." literal on a line (non-greedy, no escapes
# spanning quotes — sufficient for the emitter's single-line append calls).
_STRING_LITERALS = re.compile(r'"(?:[^"\\]|\\.)*"')

# C++ line comment inside an emitted literal: `// ...` to end of line.
_CPP_LINE_COMMENT = re.compile(r"//.*$")


def staged_files() -> list[str]:
    try:
        out = subprocess.run(
            ["git", "diff", "--cached", "--name-only", "--diff-filter=ACMR"],
            cwd=REPO_ROOT, capture_output=True, text=True, check=True,
        ).stdout
    except (subprocess.CalledProcessError, FileNotFoundError):
        return []
    return [line for line in out.splitlines() if line.strip()]


def check_rule1(path: str) -> list[str]:
    """Hardcoded EH mechanism in emitter source (blocking)."""
    if not any(marker in path.replace("\\", "/") for marker in EMITTER_PATH_MARKERS):
        return []
    if not path.endswith(".cs"):
        return []

    full = REPO_ROOT / path
    if not full.is_file():
        return []

    findings = []
    for n, line in enumerate(full.read_text(encoding="utf-8", errors="replace").splitlines(), 1):
        if _COMMENT_LINE.match(line):
            continue
        # Only lines that feed a C++ text buffer can emit generated code.
        if not any(shape in line for shape in _APPEND_SHAPES):
            continue
        # Inspect the emitted text with C++ comments removed.  A literal may
        # legitimately MENTION the construct inside a C++ comment it writes out
        # (e.g. "// a bare throw chaos_managed_exception only works in ..."),
        # which is not the same as emitting the construct.  Stripping comments
        # first is more robust than locating "//" per literal, because
        # interpolated literals (${indent}...) are not delimited the way a
        # plain string is.
        emitted = _CPP_LINE_COMMENT.sub("", line)
        for label, rx in (("try {", _BARE_TRY),
                          ("catch (chaos_managed_exception", _BARE_CATCH),
                          ("throw chaos_managed_exception", _BARE_THROW)):
            if rx.search(emitted):
                findings.append(
                    f"{path}:{n}: EH RULE-1 -- emitter writes a bare C++ `{label}` into generated code. "
                    f"Offending line: {line.strip()[:140]} "
                    "Generated code must use the CHAOS_EH_* macros (src/native/common/chaos/eh.h); "
                    "a hardcoded C++ construct only works under CPP_THROW and silently fails "
                    "under WIN32_SEH / SETJMP. See dev-il2cpp-eh-expert iron rule 1."
                )
                break
    return findings


def check_rule3(path: str) -> list[str]:
    """RaiseManagedException type-name registration (advisory)."""
    if not path.endswith((".cpp", ".h")):
        return []
    full = REPO_ROOT / path
    if not full.is_file():
        return []

    text = full.read_text(encoding="utf-8", errors="replace")
    names = set(re.findall(r'RaiseManagedException\(\s*"([^"]+)"', text))
    if not names:
        return []
    # Locate the codegen-side registration list.  If it cannot be found the
    # check degrades to informational rather than guessing.
    table = REPO_ROOT / "src/managed/Chaos.IL2CPP.Generator/Emission/NativeAotLoweringPlanner.ModuleRegistration.Dispatch.cs"
    if not table.is_file():
        return []
    table_text = table.read_text(encoding="utf-8", errors="replace")
    missing = sorted(n for n in names if f'"{n}"' not in table_text)
    if not missing:
        return []
    return [
        f"{path}: EH RULE-3 (advisory) — {len(missing)} raised exception type(s) not found in the "
        f"codegen exception table: {', '.join(missing[:5])}"
        + (" …" if len(missing) > 5 else "")
        + "\n      Unregistered types resolve to 0 and raise a NULL exception object that no\n"
          "      catch clause can match.  Run: python tools/scan_raised_exception_types.py --check"
    ]


def main() -> int:
    files = staged_files()
    if not files:
        return 0

    blocking: list[str] = []
    advisory: list[str] = []

    for path in files:
        blocking.extend(check_rule1(path))
        advisory.extend(check_rule3(path))

    for msg in advisory:
        print(f"WARNING: {msg}")
    if advisory:
        print()

    if blocking:
        print("ERROR: EH iron-rule violations in staged generated-code emitters.\n")
        for msg in blocking:
            print(msg)
            print()
        print("These have no legitimate form. Fix them, or 'git commit --no-verify' to bypass.")
        return 1

    if advisory:
        print("(EH rule-3 findings above are advisory and did not block the commit.)")
    return 0


if __name__ == "__main__":
    sys.exit(main())
