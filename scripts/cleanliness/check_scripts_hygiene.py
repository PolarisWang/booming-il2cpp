#!/usr/bin/env python3
"""Scripts-hygiene check — surface agent-generated one-shot scratch in scripts/.

The root-clean guard whitelists scripts/ entirely, so agent one-off temp scripts
(tmp_*, build helpers, WCT deadlock spies, .exe artifacts) can land in scripts/ and
be committed with nothing catching them. This check scans tracked scripts/ for
recognizable one-shot/temp/diagnostic patterns and reports them as WARN so they are
routed to scripts/debug/ or gitignored rather than silently becoming permanent
'production' source.

DELIBERATELY advisory (WARN, never blocks): 'one-shot vs maintained tool' is a
semantic judgment; naming only gives a strong hint. Agent may confirm/ignore.

Usage:
  check_scripts_hygiene.py           # WARN list (exit 0)
  check_scripts_hygiene.py --ci      # exit 1 if any flagged (CI hard-enforce)
  Orchestrated by chaos_hygiene.py (hygiene-registry.json "scripts-hygiene").
"""

import subprocess
import sys
from pathlib import Path

REPO_ROOT = Path(__file__).resolve().parent.parent.parent

# Build artifacts that should never be tracked (must be gitignored instead).
_ARTIFACT_EXT = (".exe", ".obj", ".o", ".a", ".lib", ".dll", ".pdb")


def _tracked_scripts() -> list[str]:
    out = subprocess.run(
        ["git", "ls-files", "scripts/"], cwd=REPO_ROOT,
        capture_output=True, text=True, encoding="utf-8", errors="replace",
    ).stdout
    return [p for p in out.splitlines() if p]


def main() -> int:
    scripts = _tracked_scripts()
    flagged_files = []

    for rel in scripts:
        base = Path(rel).name
        parts = rel.replace("\\", "/").split("/")
        in_debug = len(parts) >= 2 and parts[1] == "debug"

        # (a) Build artifacts — must be gitignored, never tracked (anywhere).
        if base.endswith(_ARTIFACT_EXT):
            flagged_files.append((rel, "build artifact should be gitignored, not tracked"))

        # (b) one-shot tmp_* scratch — must be gitignored, not tracked (even in debug/).
        elif base.startswith("tmp_"):
            flagged_files.append((rel, "one-shot tmp_* scratch — should be gitignored, not committed"))

        # (c) loose scratch-prefixed file at scripts/ ROOT (not under a domain subdir)
        #     — belongs in scripts/debug/ (or gitignored).
        elif len(parts) == 2 and base.startswith(("_", "tmp_", "wct_")):  # scripts/<file>
            flagged_files.append((rel, "loose root-level scripts/ scratch — route to scripts/debug/ or gitignore"))

        # (d) inside scripts/debug/: acceptable home for diagnostic tooling (NOT flagged).
        #     Spies/_build helpers that reference a real diagnostic live here by design.
        #     (tmp_* already caught in (b).)

    if flagged_files:
        print("[WARN] agent one-shot/artifact files in scripts/:")
        seen = set()
        for rel, why in flagged_files:
            if rel in seen:
                continue
            seen.add(rel)
            print(f"  {rel}: {why}")
        print("  Hint: maintained debug tooling belongs in scripts/debug/ (kept); one-shot tmp_*")
        print("  and build artifacts (.exe/.obj) must be gitignored, not committed.")
        print("  Advisory — confirm each is intentional before silencing.")
        if "--ci" in sys.argv:
            print("[FAIL] scripts-hygiene found artifact/one-shot scratch (--ci)")
            return 1
        return 0
    print("[PASS] scripts/ is free of one-shot scratch and build artifacts")
    return 0


if __name__ == "__main__":
    sys.exit(main())
