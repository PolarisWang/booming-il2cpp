#!/usr/bin/env python3
"""pre-push deep review runner.

Reads the outgoing diff on stdin, hands it to `claude -p` with the severity
rubric + project redlines, parses the resulting single-line JSON verdict, and
exits 1 (block push) when the verdict is "block" (≥1 L1/L2 issue).

Severity model (L1-L4) is defined in review_rubric.md and uses a distinct
vocabulary to avoid colliding with the repo's existing "priority P*" and
"coverage-tier P*" meanings.

Guard rails (from plan self-review):
  - `claude` missing on PATH  -> allow + warning (deterministic pre-commit gates
    still guard the objective P0/P1 cases). Never block on missing tooling.
  - claude present but non-JSON -> block (fail-closed): a bad parse must not let
    a severe issue slip through.
  - diff too large            -> 3-tier handling: full < small-ok; sample new/
    high-churn files; else fall back to `--stat` only.
  - RUBRIC / project rules injected so the model knows repo-specific redlines.
"""
from __future__ import annotations

import json
import shutil
import subprocess
import sys
import os

HOOK_DIR = os.path.dirname(os.path.abspath(__file__))
RUBRIC_PATH = os.path.join(HOOK_DIR, "review_rubric.md")
RULES_PATH = os.path.join(HOOK_DIR, "project_rules_short.txt")
DIFF_FULL_MAX = 200_000   # chars: full diff up to this
DIFF_SAMPLE_MAX = 400_000  # chars: sample new/high-churn diffs up to this
CLAUDE_TIMEOUT = 120


def parse_verdict(text: str) -> dict:
    """Parse the last non-empty line of the model output as a JSON verdict.

    Returns the parsed dict on success; on any failure returns a fail-closed
    verdict dict (severity L1, verdict block). Kept dependency-free and pure so
    it can be unit-tested in isolation.
    """
    if not text or not text.strip():
        return {
            "verdict": "block", "severity": "L1", "issue_count": 1,
            "issues": [{"file": "", "msg": "review_runner: empty model output -> block (fail-closed)"}],
        }
    for line in reversed(text.strip().splitlines()):
        line = line.strip()
        if not line:
            continue
        try:
            obj = json.loads(line)
            if isinstance(obj, dict) and "verdict" in obj:
                return obj
        except Exception:
            continue
    return {
        "verdict": "block", "severity": "L1", "issue_count": 1,
        "issues": [{"file": "", "msg": "review_runner: no JSON verdict found -> block (fail-closed). Raw tail: " + text.strip()[-300:]}],
    }


def claude_prompt(diff: str) -> str:
    rubric = open(RUBRIC_PATH, encoding="utf-8").read()
    try:
        rules = open(RULES_PATH, encoding="utf-8").read()
    except OSError:
        rules = "(project rules file missing)"
    return (
        "You are a senior reviewer for the Chaos IL2CPP monorepo (C++/C#/Python).\n"
        "Review the diff below against the severity rubric and the project redlines.\n\n"
        f"=== SEVERITY RUBRIC ===\n{rubric}\n\n"
        f"=== PROJECT REDLINES ===\n{rules}\n\n"
        "=== DIFF (JSON-escaped) ===\n" + json.dumps(diff) + "\n\n"
        "Emit exactly one line of JSON per the rubric. No commentary outside it."
    )


def review(diff: str) -> dict:
    if not shutil.which("claude"):
        return {
            "verdict": "allow", "severity": "L3", "issue_count": 1,
            "issues": [{"file": "", "msg": "claude CLI not on PATH; deep review skipped. "
                        "Deterministic pre-commit gates still apply."}],
        }
    prompt = claude_prompt(diff)
    try:
        # Pass the prompt via stdin, NOT as an argv element: on Windows a single
        # argv exceeding ~32KB raises WinError 206, and a meta-review --stat of a
        # huge push can easily blow past that. `claude -p` reads stdin when no
        # prompt argument is given.
        proc = subprocess.run(
            ["claude", "-p", "--output-format", "text"],
            input=prompt, capture_output=True, text=True,
            encoding="utf-8", errors="replace", cwd=".", timeout=CLAUDE_TIMEOUT,
        )
    except Exception as exc:  # subprocess-level failure (timeout, spawn error)
        return {
            "verdict": "block", "severity": "L1", "issue_count": 1,
            "issues": [{"file": "", "msg": f"review_runner: claude invocation failed: {exc}"}],
        }
    return parse_verdict(proc.stdout)


def sample_diff(diff_stat: str) -> str:
    """When the full diff exceeds the sampling budget, return just the --stat
    lines (churn overview) so a huge push is still reviewed at meta level."""
    return diff_stat


def main() -> int:
    diff = sys.stdin.read()
    if not diff or not diff.strip():
        print("[review] no diff on stdin, skipping")
        return 0

    full_len = len(diff)
    if full_len > DIFF_SAMPLE_MAX:
        stat = subprocess.run(["git", "diff", "--stat"], capture_output=True,
                              text=True, encoding="utf-8", errors="replace",
                              cwd=".").stdout
        print(f"[review] diff {full_len} chars > {DIFF_SAMPLE_MAX}; meta review (--stat)", file=sys.stderr)
        verdict = review(sample_diff(stat or diff[:DIFF_SAMPLE_MAX]))
    elif full_len > DIFF_FULL_MAX:
        # still large: hand only the stat + first N chars to stay in budget
        stat = subprocess.run(["git", "diff", "--stat"], capture_output=True,
                              text=True, encoding="utf-8", errors="replace",
                              cwd=".").stdout
        print(f"[review] diff {full_len} chars > {DIFF_FULL_MAX}; sampled review", file=sys.stderr)
        verdict = review(stat + "\n" + diff[:DIFF_FULL_MAX])
    else:
        verdict = review(diff)

    sev = verdict.get("severity", "none")
    count = verdict.get("issue_count", 0)
    if verdict.get("verdict") == "block":
        print(f"[review] BLOCK ({sev}) — {count} severe issue(s):")
        for i in verdict.get("issues", [])[:20]:
            print(f"  - {i.get('file','')} : {i.get('msg','')}")
        summary = verdict.get("summary")
        if summary:
            print(f"  summary: {summary}")
        return 1

    print(f"[review] OK ({sev}) — {count} issue(s)")
    for i in verdict.get("issues", [])[:10]:
        print(f"  - {i.get('file','')} L{i.get('severity','')} : {i.get('msg','')}")
    return 0


if __name__ == "__main__":
    sys.exit(main())
