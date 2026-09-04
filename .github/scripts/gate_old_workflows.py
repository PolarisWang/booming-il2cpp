#!/usr/bin/env python3
"""Phase 3: decommission gate — switch superseded monolith workflows to manual-only.

Strategy: replace each old workflow's top-level `on:` (auto-trigger) block with a
manual-only `workflow_dispatch` trigger.  Revertible, YAML-safe (no per-job surgery),
and keeps the files runnable by hand if ever needed.  Run AFTER the new Pipeline has
been validated green on real GitHub.

Run: python .github/scripts/gate_old_workflows.py
"""
import os

WORKFLOWS = [
    '.github/workflows/ci.yml', '.github/workflows/foundation-dll-regression.yml',
    '.github/workflows/codegen-regression.yml', '.github/workflows/aot-regression.yml',
    '.github/workflows/hybrid-regression.yml', '.github/workflows/jit-baseline.yml',
    '.github/workflows/gc-ci.yml', '.github/workflows/gc-linux-x64-ci.yml',
    '.github/workflows/gc-stress-nightly.yml', '.github/workflows/native-nightly.yml',
    '.github/workflows/clang-tidy.yml', '.github/workflows/ubsan-ci.yml',
    '.github/workflows/managed-csharp-style.yml', '.github/workflows/hygiene-gate.yml',
    '.github/workflows/secret-scan.yml', '.github/workflows/codeql.yml',
    '.github/workflows/docs.yml', '.github/workflows/arm64-jit-ci.yml',
]


def gate(path: str) -> bool:
    with open(path, encoding='utf-8') as fh:
        lines = fh.readlines()

    # Locate `on:` (line whose strip == 'on:') and the next top-level block start.
    on_idx = None
    next_tl = len(lines)
    for i, line in enumerate(lines):
        s = line.strip()
        if on_idx is None:
            if s == 'on:' or s.startswith('on: '):
                on_idx = i
            continue
        if s and not s.startswith('#') and not line.startswith(' '):
            next_tl = i
            break

    if on_idx is None:
        print(f'  {os.path.basename(path)}: no on: found, skip')
        return False

    head = lines[:on_idx]
    dispatch = [
        'on:\n',
        '  workflow_dispatch:\n',
        '    # [DEGATED] superseded by Pipeline profile (pipeline.yml). Manual only.\n',
        '    inputs:\n',
        '      reason:\n',
        '        description: "Manual override (was auto-triggered)"\n',
        '        default: "degated-superseded-by-pipeline"\n',
        '        required: false\n',
        '        type: string\n',
    ]
    new_lines = head + dispatch + lines[next_tl:]
    with open(path, 'w', encoding='utf-8') as fh:
        fh.writelines(new_lines)
    return True


if __name__ == '__main__':
    changed = 0
    for wf in WORKFLOWS:
        if not os.path.exists(wf):
            print(f'  {os.path.basename(wf)}: MISSING')
            continue
        if gate(wf):
            print(f'  {os.path.basename(wf)}: degated')
            changed += 1
        else:
            print(f'  {os.path.basename(wf)}: no change')
    print(f'\nDegated {changed}/{len(WORKFLOWS)}')
