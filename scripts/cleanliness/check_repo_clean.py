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

import subprocess
import sys
from pathlib import Path

REPO_ROOT = Path(__file__).resolve().parent.parent.parent

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
