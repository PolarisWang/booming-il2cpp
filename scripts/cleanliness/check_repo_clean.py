#!/usr/bin/env python3
"""Repository-root cleanliness guard.

Checks that the repo root stays clean of build artifacts, debug logs, and
stray scratch files — the "working directory clean" monitor.

Used in two places (A+C double protection):
  A. pre-commit  guard  -> hard gate on commit that would add junk
  C. Claude hook        -> soft realtime nudge when junk is produced mid-session

Run modes:
  --hard   exit 1 on any violation (used by pre-commit / CI)
  --soft   print warnings, always exit 0 (used by the Claude PostToolUse hook)
  (default) --soft

A violation is triggerered when a NEW untracked non-ignored file appears at the
repo root that isn't a legit root file/dir. Tracked files are never flagged
(this guard only surfaces new junk, so it is zero-risk to existing source).

Allowlist is auto-derived: any root entry already tracked in git is trusted.
Deviation is only allowed for the explicit self-maintaining exceptions below.
"""

import json
import re
import subprocess
import sys
from pathlib import Path

REPO_ROOT = Path(__file__).resolve().parent.parent.parent
REGISTRY = REPO_ROOT / "scripts" / "cleanliness" / "generated-registry.json"

# Root entries that are legitimately NOT tracked but present by design.
# Keep in sync with the root CMakeLists / docs layout. Anything tracked is
# automatically trusted.
#
# NOTE: deliberately does NOT whitelist the deleted historical root-CMake build
# dirs (abi/ bootstrap/ common/ runtime-core/ support/ engine-bridge/ fuzz/
# hot-update/ codegen/) — if a rogue root-CMake re-run ever recreates one, the
# guard must flag it.
ROOT_EXCEPTIONS = {
    # top-level source dirs
    ".ai", ".claude", ".claude_local",
    "artifacts",            # CI/pipeline cache (gitignored by design)
    "contracts", "src", "tests", "docs", "wiki",
    "third_party", "scripts", "tools", "cmake", "build",
    "optimization-campaign", "results", "schemas",
}

# Build/scratch file *types* that must never appear loose at repo root,
# even if not yet matching a gitignore rule (defense in depth).
ROOT_BANNED_EXT = {
    ".log", ".obj", ".o", ".a", ".lib", ".pdb", ".ilk", ".exe", ".so",
    ".vcxproj", ".sln", ".stackdump",
}
ROOT_BANNED_NAMES = {
    "CMakeCache.txt", "cmake_install.cmake", "Makefile", "CTestTestfile.cmake",
    "compile_commands.json", "ALL_BUILD.vcxproj", "ZERO_CHECK.vcxproj",
    "INSTALL.vcxproj", "RUN_TESTS.vcxproj",
}


def git(*args):
    out = subprocess.run(
        ["git"] + list(args), cwd=REPO_ROOT,
        capture_output=True, text=True,
        encoding="utf-8", errors="replace",
    )
    return out.stdout.strip()


def derive_trusted_roots():
    """Everything git already tracks at root is trusted."""
    tracked = git("ls-files")
    roots = {line.split("/")[0] for line in tracked.splitlines() if line}
    return roots | ROOT_EXCEPTIONS


def load_generated_registry():
    """Load regenerable-file globs from generated-registry.json (empty if absent)."""
    if not REGISTRY.exists():
        return []
    try:
        data = json.loads(REGISTRY.read_text(encoding="utf-8"))
    except (OSError, ValueError):
        return []
    return [e.get("glob", "") for e in data.get("entries", [])]


_REGEX_CACHE = {}


def _glob_to_regex(glob):
    """Translate a git-style glob (with ** recursion) to an anchored regex.

    NOTE: 'fnmatch' cannot express recursive '**' (it treats ** == * and * does
    not span '/'), so a hand-rolled converter is required for registry globs
    like 'tests/e2e/translation/**/managed/subjects/subjects.metadata.json'
    to match at any nesting depth. Cached per pattern.
    """
    if glob in _REGEX_CACHE:
        return _REGEX_CACHE[glob]
    i, n, out = 0, len(glob), []
    while i < n:
        c = glob[i]
        if c == "*":
            if i + 1 < n and glob[i + 1] == "*":
                # '**' then '/': matches zero-or-more complete dir segments
                i += 2
                if i < n and glob[i] == "/":
                    out.append("(?:[^/]+/)*")
                    i += 1
                else:  # bare '**' at end -> anything
                    out.append(".*")
                continue
            out.append("[^/]*")  # '*' within a segment, does not cross '/'
            i += 1
        elif c == "?":
            out.append("[^/]")
            i += 1
        elif c in ".(){}\\^$+[]-|":
            out.append("\\" + c)
            i += 1
        else:
            out.append(c)
            i += 1
    rx = re.compile("^" + "".join(out) + "$")
    _REGEX_CACHE[glob] = rx
    return rx


def _matches_any(path: str, globs) -> bool:
    return any(_glob_to_regex(g).match(path.replace("\\", "/")) for g in globs)


def main():
    mode = "soft"
    if "--hard" in sys.argv:
        mode = "hard"
    elif "--soft" in sys.argv:
        mode = "soft"

    trusted = derive_trusted_roots()

    # Untracked + not-ignored entries (git status --porcelain lists every
    # untracked *file*, including those nested inside a brand-new directory).
    status = git("status", "--porcelain", "--untracked-files=all", "--", ".")
    violations = []
    for line in status.splitlines():
        if not line.startswith("??"):
            continue  # tracked-but-modified is fine
        path = line[3:]
        top = path.split("/")[0]
        if top in trusted:
            continue  # inside a known source root — not root-level junk
        # `top` is a NEW untracked root-level entry (loose file OR a whole new
        # directory tree). Flag regardless of nesting depth so a rogue build
        # that drops debug_output/ at root gets caught.
        if "/" in path:
            violations.append(
                f"new untracked root-level directory: {top}/ (first file: {path}) — "
                f"not a whitelisted root; re-route to artifacts/ or a source dir"
            )
        else:
            suffix = Path(top).suffix.lower()
            if suffix in ROOT_BANNED_EXT or top in ROOT_BANNED_NAMES:
                violations.append(f"banned build-artifact type at root: {path}")
            else:
                violations.append(
                    f"new untracked file at repo root: {path} — "
                    f"add to .gitignore, commit it under a proper dir, or delete it"
                )

    # Informational: tracked regenerable manifests that are modified (expected
    # churn from the registry, not audit). Soft mode reports them so devs see
    # "this churns on purpose — commit deliberately"; hard mode still passes
    # (they are committed/expected, not junk). Deleted regenerable manifests ARE
    # flagged (a tracked generated file should not be silently dropped).
    regen = load_generated_registry()
    if regen:
        tracked_lines = git("status", "--porcelain", "--", ".")
        regen_modified, regen_deleted = [], []
        for line in tracked_lines.splitlines():
            if len(line) < 4 or line.startswith("??"):
                continue  # skip untracked (handled as root-junk above)
            path = line[3:]
            # status is "XY path"; X=staged, Y=unstaged; M/D in either means
            # modified/deleted. ` M` (working-tree) and `M ` (staged) both.
            x, y = line[0], line[1]
            if not _matches_any(path, regen):
                continue
            if "D" in (x + y):
                regen_deleted.append(path)
            if "M" in (x + y) or "R" in (x + y):
                regen_modified.append(path)
        if regen_deleted:
            for p in regen_deleted:
                violations.append(
                    f"regenerable manifest DELETED but registry says it should stay committed: {p}"
                )
        if regen_modified and mode != "hard":
            print("  [repo-clean] informational: regenerable manifests changed (expected churn, commit deliberately):")
            for p in regen_modified[:8]:
                print(f"    {p}")
            if len(regen_modified) > 8:
                print(f"    ... +{len(regen_modified)-8} more")

    if not violations:
        print("=== [repo-clean] repo root is clean ===")
        return 0
    for v in violations:
        print("  [repo-clean] " + v)
    print(f"=== [repo-clean] {len(violations)} repo-root cleanliness issue(s) ===")
    print("  Hint: keep the repo root for source only. Move docs under docs/,")
    print("  add scratch to .gitignore, or commit it in a real module dir.")
    if mode == "hard":
        return 1
    return 0


if __name__ == "__main__":
    sys.exit(main())
