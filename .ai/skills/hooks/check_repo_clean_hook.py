#!/usr/bin/env python3
"""PostToolUse hook: realtime nudge when the repo root gets dirty (方案 C).

Wraps scripts/cleanliness/check_repo_clean.py in --soft mode and surfaces a
short nudge to the user the FIRST time a new root-level cleanliness violation
appears, then stays silent until the situation changes again (dedup via a state
file). Never fails the hook and never blocks — this is advisory only. The
hard gate lives in the pre-commit hook (方案 A).

The nudge is emitted to the hook's stdout, which the harness surfaces in the
conversation.
"""

import hashlib
import json
import subprocess
import sys
from pathlib import Path


def _repo_root() -> Path | None:
    p = Path(__file__).resolve()
    for _ in range(6):  # .ai/skills/hooks/... -> repo root
        p = p.parent
        if (p / ".git").exists():
            return p
    return None


def _violations(repo_root: Path) -> list[str]:
    """Run the shared checker (soft) and capture its violation lines."""
    checker = repo_root / "scripts" / "cleanliness" / "check_repo_clean.py"
    if not checker.exists():
        return []
    try:
        proc = subprocess.run(
            [sys.executable, str(checker), "--soft"],
            cwd=repo_root, capture_output=True, text=True, timeout=20,
        )
    except (subprocess.SubprocessError, OSError):
        return []
    lines = []
    for ln in proc.stdout.splitlines():
        s = ln.strip()
        if s.startswith("[repo-clean] ") and not s.startswith("[repo-clean] repo"):
            lines.append(s)
    return lines


def main() -> int:
    repo_root = _repo_root()
    if not repo_root:
        return 0
    viols = _violations(repo_root)
    signature = hashlib.sha256("\n".join(viols).encode()).hexdigest()

    state = repo_root / ".claude" / ".repo-clean-state.json"
    state.parent.mkdir(parents=True, exist_ok=True)
    try:
        last = json.loads(state.read_text(encoding="utf-8")) if state.exists() else {}
        last_sig = last.get("signature")
    except (OSError, ValueError):
        last_sig = None

    if not viols:
        # clean — forget prior state so the next dirtiness re-warns
        try:
            state.write_text(json.dumps({"signature": "", "count": 0}, ensure_ascii=False), encoding="utf-8")
        except OSError:
            pass
        return 0

    if last_sig == signature:
        return 0  # unchanged since last time — stay silent

    try:
        state.write_text(json.dumps({"signature": signature, "count": len(viols)}, ensure_ascii=False), encoding="utf-8")
    except OSError:
        pass

    print("\n[cleanliness] Repo root gained new junk — keep the root to source only:")
    for v in viols[:5]:
        print(f"  {v}")
    if len(viols) > 5:
        print(f"  ... and {len(viols) - 5} more")
    print("[cleanliness] Fix: move to docs/, add to .gitignore, or commit in a real module dir. "
          "(pre-commit will hard-fail if left at root.)")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
