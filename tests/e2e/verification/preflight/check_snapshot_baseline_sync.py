#!/usr/bin/env python3
"""check_snapshot_baseline_sync.py — 快照基线同步性检查。

Pre-commit hook 组件。防止「registry 改了但基线没重生成」长期累积。

背景
----
快照基线是 codegen 输出的逐字节副本。每次 RuntimeHelperShapeRegistry /
NativeAotEmitter 演进后必须重生成基线，否则：

  * 基线静默陈旧，测试红但与本次改动无关（归因困难）；
  * 更糟的是两套套件时代已实证的形态 —— 基线落后几个季度仍无人察觉，
    直到有人偶然跑一次才暴露。

历史实例：runtime_helper_shapes.h 基线停在 670 条 shape，而 registry 已产出
2554 条；两套重复套件里一份 1670 行、另一份 6211 行，长期并存且互相矛盾。

本检查**不重生成基线**（那需要跑 dotnet build + emitter，代价高且要求
Windows host）。它只做一件事：确认**基线里记录的形状枚举**与
**registry 源码里注册的形状**数量级一致 —— 足以捕捉「落后几个数量级」
这类真正的漂移，而不会因一两个 shape 的增删误报。

判据与已知局限
--------------
阈值取「基线 shape 条目数 >= registry 注册点数」这种单调判据在删减时
会误报，故改为**比例判据**：基线条目数不得低于 registry 注册点数的 60%。
这个下界不在数学上严格（注册点与 enum 条目并非一对一：GenericShapeDescriptor
走 manifest 而非 ShapeId enum），但它能可靠地捕捉数量级级别的陈旧
（670 vs 2554 会被抓住），而对正常的增删不敏感。

返回    0 = 通过
        1 = 基线显著陈旧
"""

from __future__ import annotations

import re
import sys
from pathlib import Path

_REPO_ROOT = Path(__file__).resolve().parents[4]

_SNAPSHOT_SUITE = _REPO_ROOT / "tests" / "unit" / "managed" / "snapshot"
_REGISTRY_DIR = _REPO_ROOT / "src" / "managed" / "Chaos.IL2CPP.Generator" / "RuntimeSupport"

# A registry entry point looks like registry.Register(...) / registry.RegisterGeneric(...).
_REGISTRATION_RE = re.compile(r"\bregistry\.Register(?:Generic)?\s*\(")
# Shape enum entries look like  SHAPE_FOO_BAR = 0x1234u,
_SHAPE_ENTRY_RE = re.compile(r"^\s*SHAPE_[A-Z0-9_]+\s*=", re.MULTILINE)

# Baseline may lag this fraction before it is called stale.  See module docstring.
_MIN_RATIO = 0.60

issues: list[str] = []


def _count_registrations() -> int:
    total = 0
    if not _REGISTRY_DIR.is_dir():
        return 0
    for path in _REGISTRY_DIR.glob("*.cs"):
        try:
            total += len(_REGISTRATION_RE.findall(path.read_text(encoding="utf-8")))
        except OSError:
            continue
    return total


def _baseline_shape_counts() -> tuple[int, Path | None]:
    """Largest shape-enum entry count across all fixtures, and its path."""
    best = 0
    best_path: Path | None = None
    if not _SNAPSHOT_SUITE.is_dir():
        return 0, None
    for path in _SNAPSHOT_SUITE.glob("Baselines/*/runtime_helper_shapes.h"):
        try:
            n = len(_SHAPE_ENTRY_RE.findall(path.read_text(encoding="utf-8", errors="replace")))
        except OSError:
            continue
        if n > best:
            best, best_path = n, path
    return best, best_path


def main() -> int:
    if not _SNAPSHOT_SUITE.is_dir():
        # Suite absent (renamed or removed) — not this check's business.
        print("OK: snapshot suite not found; baseline sync check skipped")
        return 0

    registrations = _count_registrations()
    baseline_shapes, baseline_path = _baseline_shape_counts()

    if registrations == 0:
        print("OK: no shape registrations found; skipping")
        return 0

    if baseline_path is None:
        issues.append(
            "未找到任何 runtime_helper_shapes.h 基线。\n"
            "  若快照套件仍在，基线可能被误删。"
        )
    else:
        ratio = baseline_shapes / registrations
        if ratio < _MIN_RATIO:
            issues.append(
                f"快照基线显著陈旧：基线 shape 条目 {baseline_shapes} 条，"
                f"registry 注册点 {registrations} 个（比例 {ratio:.0%} < {_MIN_RATIO:.0%}）。\n"
                f"  基线: {baseline_path.relative_to(_REPO_ROOT)}\n"
                f"  这通常意味着 registry 已演进但没人跑 SNAPSHOT_UPDATE=1。\n"
                f"  修复: dotnet build tests/unit/managed/snapshot/FixtureAssembly/ && "
                f"SNAPSHOT_UPDATE=1 dotnet test tests/unit/managed/snapshot/"
            )

    if issues:
        for issue in issues:
            print(f"FAIL: {issue}")
        return 1

    print(
        f"OK: 快照基线同步（{baseline_shapes} shape 条目 vs "
        f"{registrations} registry 注册点）"
    )
    return 0


if __name__ == "__main__":
    sys.exit(main())
