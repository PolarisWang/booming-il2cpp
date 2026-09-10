#!/usr/bin/env python3
"""Generated-file drift guard (hermetic — never writes into the repo).

Ensures committed generated artifacts (C++ headers, C# enums) are UP TO DATE with
their generators: a dev who edits schemas/ir_opcodes.yaml must have re-run
generate_ir_opcodes.py, and nobody hand-edited a generated file.

This is the enforce-half of "generative scripts don't pollute":
  - every registered generator must support an output-redirect flag (--out-dir);
  - the guard runs it into a TEMP sandbox and diffs the produced bytes against the
    committed file (semantic: ignores the auto-gen banner line);
  - it never modifies the repo itself, so running it can never dirty the tree.

Used as:
  Manual: python scripts/cleanliness/check_generated_up_to_date.py      # warn-only (exit 0 on drift)
  Hard gate: python ... --fail-fast                                     # exit 1 on drift
  CI/pre-commit: orchestrated by chaos_hygiene.py (hygiene-registry.json "generated-drift").
    * CI (--ci) enforces: drift -> the orchestrator flags the module as WARN/non-PASS
      and --ci blocks on any non-PASS -> real drift fails CI.
    * pre-commit (--gate) is ADVISORY: gate mode blocks only FAIL (not WARN), so a
      pre-existing drift does not brick unrelated commits; CI is where enforced.
"""

import json
import subprocess
import sys
import tempfile
from pathlib import Path

REPO_ROOT = Path(__file__).resolve().parent.parent.parent

GENERATORS = [
    {
        "name": "ir_opcodes",
        # --out-dir makes this fully hermetic (sandbox capture).
        "cmd": lambda sandbox: [
            sys.executable,
            str(REPO_ROOT / "scripts/codegen/generate_ir_opcodes.py"),
            "--out-dir", str(sandbox),
        ],
        "rel_outputs": [
            "ir_opcodes.h",
            "ir_opcode_names.h",
            "InstructionOpCode.cs",
        ],
        "committed": [
            "src/native/interpreter/generated/ir_opcodes.h",
            "src/native/interpreter/generated/ir_opcode_names.h",
            "src/managed/Chaos.IL2CPP.Contracts/generated/InstructionOpCode.cs",
        ],
    },
]

# Auto-gen banner line(s) to strip for semantic compare.
_BANNER = ("// Auto-generated from schemas",)


def _normalize(text: str) -> str:
    """Semantic compare string. Collapses runs of whitespace to single space per
    line and strips, so cosmetic spacing differences (`SimdFma = 110 ` vs
    `SimdFma = 110`, `}  // namespace` vs `} // namespace`) don't false-positive —
    the guard catches real content drift (different opcodes/values/order/names),
    not whitespace no-op regeneration.  Also strips the auto-gen banner line(s)
    (review #2): the banner is generator-version noise, not content — if a future
    generator embeds a timestamp/path/hash there, the committed and fresh copies
    would differ and false-report DRIFT on an unchanged artifact.  Stripping it
    makes the compare robust to that."""
    lines = text.splitlines()
    # Drop leading lines that exactly match a known auto-gen banner (e.g. the
    # "// Auto-generated from schemas" header).  Only exact matches on the banner
    # prefix are stripped, so a semantically-important line is never lost.
    while lines and any(lines[0].startswith(prefix) for prefix in _BANNER):
        lines.pop(0)
    normal = [" ".join(line.split()) for line in lines]
    return "\n".join(normal).strip()


def _committed_text(rel: str) -> str:
    try:
        return (REPO_ROOT / rel).read_text(encoding="utf-8", errors="replace")
    except OSError:
        return ""


def main() -> int:
    failures = []
    with tempfile.TemporaryDirectory() as td:
        sandbox = Path(td)
        for gen in GENERATORS:
            name = gen["name"]
            try:
                proc = subprocess.run(
                    gen["cmd"](sandbox),
                    capture_output=True, text=True,
                    encoding="utf-8", errors="replace",
                    timeout=120,
                )
            except (subprocess.SubprocessError, OSError) as e:
                failures.append(f"{name}: generator spawn failed: {e}")
                continue
            if proc.returncode:
                failures.append(f"{name}: generator rc={proc.returncode}: {(proc.stdout or proc.stderr)[:200]}")
                continue
            for rel, committed_rel in zip(gen["rel_outputs"], gen["committed"]):
                fresh = _normalize((sandbox / rel).read_text(encoding="utf-8", errors="replace"))
                cur = _normalize(_committed_text(committed_rel))
                if fresh != cur:
                    failures.append(
                        f"{name}: DRIFT in {committed_rel} — committed file differs from a fresh "
                        f"regeneration. Re-run: python scripts/codegen/generate_ir_opcodes.py"
                    )

    if failures:
        print("=== [generated-drift] generated files are STALE vs their generators ===")
        for f in failures:
            print("  " + f)
        print("  Fix: re-run the generator (e.g. python scripts/codegen/generate_ir_opcodes.py) and commit,")
        print("  or remove the stale generated artifact if it is intentionally version-pinned.")
        if "--fail-fast" in sys.argv:
            # Explicit [FAIL] + non-zero exit: the orchestrator FAILs CI on real
            # drift via an unambiguous status (review #1) — not a fragile text
            # heuristic.  CI passes --fail-fast (chaos_hygiene._dispatch).
            print("[FAIL] generated files are STALE vs their generators")
            return 1
        # pre-commit: warn, don't hard-block (a pre-existing drift shouldn't brick
        # unrelated commits). CI/--fail-fast enforces the real gate.
        print("[WARN] generated files are STALE vs their generators (advisory)")
        print("  [warn-only] pre-existing drift — pre-commit did NOT block. CI --fail-fast enforces.")
        return 0
    print("[PASS] all registered generated files are up to date")
    print("=== [generated-drift] all registered generated files are up to date (hermetic check, no writes) ===")
    return 0


if __name__ == "__main__":
    sys.exit(main())
