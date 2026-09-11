#!/usr/bin/env python3
"""check_verification_tree_singular.py — 验证引擎单一权威树守卫。

在 CI pipeline 启动时和 pre-commit hook 中运行，确保：

- `testing/foundation-dll/verification/` **不存在**（已废弃的僵尸树）
- 权威树 `tests/e2e/verification/` 存在且可导入

背景（2026-09-11）：
    323e8c279 (L6) 用 `git mv` 把验证引擎从 `testing/foundation-dll/verification/`
    搬到 `tests/e2e/verification/`。旧路径未被删除干净，且**随后被并行 agent 的
    clean-checkout 复活**（见 [[parallel-agent-clean-checkout-clobber]]），
    形成一棵 72 文件的僵尸树。

    两者都是 git-tracked、都不是 symlink，只是同名前缀 —— 极易误判成同一棵树。
    按路径调用 `python testing/foundation-dll/verification/chunk_pipeline.py`
    会**静默跑到 Oct-10 的旧引擎上**，导致修复"改了却没生效"、
    整轮结论建立在错误的引擎上（这次真实发生过）。

    CI 走的是权威树：`.github/workflows/foundation-dll.yml:144`。
    skill 文档里的路径引用也已在同一次清理中改为权威树。

使用方式:
    python check_verification_tree_singular.py [--ci] [--verbose]

    --ci: CI 模式，检查所有文件（包括未 staged 变更）
    --verbose: 详细输出每个检查项
"""

from __future__ import annotations

import subprocess
import sys
from pathlib import Path

# ── Constants ────────────────────────────────────────────────────────────

_REPO_ROOT = Path(__file__).resolve().parents[4]
# repo root: file lives under tests/e2e/verification/preflight/

# 已废弃的僵尸树路径（相对 repo root）
_ZOMBIE_TREE = "testing/foundation-dll/verification"
# 权威树路径（相对 repo root）
_CANONICAL_TREE = "tests/e2e/verification"

_EXIT_OK = 0
_EXIT_VIOLATION = 1


def _tracked_files_under(prefix: str) -> list[str]:
    """返回 git 跟踪的、位于 prefix 下的文件列表。"""
    try:
        out = subprocess.run(
            ["git", "ls-files", "--", prefix],
            cwd=str(_REPO_ROOT),
            capture_output=True,
            text=True,
            check=False,
        )
    except OSError:
        return []
    if out.returncode != 0:
        return []
    return [line for line in out.stdout.splitlines() if line.strip()]


def check_verification_tree_singular(verbose: bool = False) -> list[str]:
    """返回违规描述列表；空列表表示通过。"""
    issues: list[str] = []

    # 1. 僵尸树必须不存在（既不在 git index，也不在文件系统）
    zombie_abs = _REPO_ROOT / _ZOMBIE_TREE
    tracked = _tracked_files_under(_ZOMBIE_TREE)
    if tracked:
        issues.append(
            f"僵尸验证树被 git 跟踪: {_ZOMBIE_TREE}/ ({len(tracked)} 文件) — "
            f"权威树是 {_CANONICAL_TREE}/，旧路径已在 323e8c279 搬迁，"
            f"请 `git rm -r {_ZOMBIE_TREE}/` 而不是在此路径修改"
        )
    elif zombie_abs.exists() and any(zombie_abs.rglob("*")):
        # 未跟踪但在磁盘上：通常是 clean-checkout 复活或 __pycache__ 残留。
        # 残留的 .pyc 同样危险 —— 没有 .py 也能从 __pycache__ 里加载旧字节码。
        leftovers = [
            p for p in zombie_abs.rglob("*") if p.is_file()
        ]
        if leftovers:
            issues.append(
                f"僵尸验证树残留在磁盘: {_ZOMBIE_TREE}/ ({len(leftovers)} 文件，未跟踪) — "
                f"删除它：`rm -rf {_ZOMBIE_TREE}/`"
            )

    # 2. 权威树必须存在且是 Python 包
    canonical_abs = _REPO_ROOT / _CANONICAL_TREE
    if not (canonical_abs / "__init__.py").is_file():
        issues.append(
            f"权威验证树缺失或不完整: {_CANONICAL_TREE}/__init__.py 不存在 — "
            f"验证引擎应位于此处（CI: .github/workflows/foundation-dll.yml）"
        )

    if verbose:
        print(f"  [tree] zombie={_ZOMBIE_TREE} tracked={len(tracked)}")
        print(f"  [tree] canonical={_CANONICAL_TREE} exists={canonical_abs.exists()}")

    return issues


def check_verification_tree_singular_ci(verbose: bool = False) -> list[str]:
    """CI 入口别名，与 check_layer_boundaries_ci 对称。"""
    return check_verification_tree_singular(verbose=verbose)


def main(argv: list[str] | None = None) -> int:
    args = list(sys.argv[1:] if argv is None else argv)
    verbose = "--verbose" in args
    ci_mode = "--ci" in args

    print("[preflight] verification tree singularity check")
    issues = check_verification_tree_singular(verbose=verbose)

    if issues:
        print(f"[preflight] {len(issues)} verification tree issue(s):")
        for issue in issues:
            print(f"  - {issue}")
        print(
            f"[preflight] hint: 跑管线一律用\n"
            f"    export PYTHONPATH=\"$PWD/tests/e2e:$PWD/tests/e2e/verification\"\n"
            f"    python -m verification.chunk_pipeline --assembly <asm> --chunk <slug>"
        )
        return _EXIT_VIOLATION

    mode = "ci" if ci_mode else "local"
    print(f"[preflight] verification tree check passed ({mode})")
    return _EXIT_OK


if __name__ == "__main__":
    raise SystemExit(main())
