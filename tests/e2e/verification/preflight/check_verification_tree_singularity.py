#!/usr/bin/env python3
"""check_verification_tree_singularity.py — 验证引擎树唯一性检查。

Pre-commit hook 和 CI pipeline 的组件。
确保验证引擎（verification tree）只有一份权威副本，
阻止陈旧的孪生树静默复活。

权威路径: tests/e2e/verification/
禁止路径: testing/foundation-dll/verification/（在 b4ae15122 中删除）

返回值：
  0 = 唯一性通过
  1 = 有陈旧孪生树或其他问题
"""

from __future__ import annotations

import sys
from pathlib import Path

_REPO_ROOT = Path(__file__).resolve().parents[4]

CANONICAL = _REPO_ROOT / "tests" / "e2e" / "verification"
STALE_TWIN = _REPO_ROOT / "testing" / "foundation-dll" / "verification"

issues: list[str] = []

if not CANONICAL.is_dir():
    issues.append(f"权威验证引擎路径不存在: {CANONICAL}")

if STALE_TWIN.is_dir():
    issues.append(
        f"发现陈旧孪生验证树: {STALE_TWIN}\n"
        f"  它在 b4ae15122 中被从 git 删除但可能被并行 agent 的 clean checkout 复活。\n"
        f"  删除它并用 'git commit --no-verify' 跳过本检查。"
    )

if issues:
    for issue in issues:
        print(f"FAIL: {issue}")
    sys.exit(1)
else:
    print("OK: 验证引擎树唯一性确认 — 仅 tests/e2e/verification/ 存在")
    sys.exit(0)