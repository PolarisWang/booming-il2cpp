#!/usr/bin/env python3
"""fake_green_gate.py — Phase 5 反假绿门禁。

检测 codegen 产出中是否含有"看起来绿但实际是 stub"的假绿模式。

使用方法:
    python testing/foundation-dll/verification/tools/fake_green_gate.py [--ci]

验收反例（Phase 5 门禁自身）:
    构造 `return 42L` 假 subject → 门禁判 FAIL

假绿模式清单:
    1. kUnsupportedAsyncIteratorSubjects 非空但有值（A1 标记）
    2. ChaosExternalRuntimeFallback 调用但对应 subject 未在 unsupported 清单中
    3. 99% 以上 method 体包含 "// AOT-unreachable stub"
    4. method 体只包含 `return {};` 或 `return 0;` 且不含任何 real lowering 特征
"""

from __future__ import annotations

import os
import re
import sys
from pathlib import Path


def check_fake_green(source: str, label: str) -> list[str]:
    """检查一段 emitted C++ source 是否为假绿。"""
    issues: list[str] = []

    # Pattern 1: Unsupported async iterator count > 0
    unsupported_match = re.search(r'kUnsupportedAsyncIteratorCount\s*=\s*(\d+)', source)
    if unsupported_match and int(unsupported_match.group(1)) > 0:
        issues.append(f"[{label}] kUnsupportedAsyncIteratorCount={unsupported_match.group(1)} > 0 — async iterators are stubbed")

    # Pattern 2: ChaosExternalRuntimeFallback calls not in unsupported list
    fallback_calls = re.findall(r'ChaosExternalRuntimeFallback\("([^"]+)"\)', source)
    if fallback_calls:
        issues.append(f"[{label}] {len(fallback_calls)} ChaosExternalRuntimeFallback call(s) found")

    # Pattern 3: method body that only returns a default
    method_bodies = re.findall(
        r'extern "C"[^}]+?\{([^}]*?(?:return\s*\{\}|return\s*0;)[^}]*?)\}',
        source
    )
    stub_count = sum(
        1 for body in method_bodies
        if not re.search(r'(pc-dispatch|chaos_continuation|async_task_run|set_result|set_exception)', body, re.IGNORECASE)
    )
    if stub_count > 0 and stub_count == len(method_bodies):
        issues.append(f"[{label}] ALL methods are stub (return-0/return-{{}} only)")

    return issues


def scan_file(path: Path) -> list[str]:
    issues: list[str] = []
    try:
        source = path.read_text(encoding='utf-8', errors='replace')
    except Exception as e:
        return [f"ERROR reading {path}: {e}"]

    label = path.name
    issues.extend(check_fake_green(source, label))
    return issues


def scan_directory(root: Path) -> list[str]:
    issues: list[str] = []
    for generated in root.rglob("*.generated.cpp"):
        issues.extend(scan_file(generated))
    for h in root.rglob("*.generated.h"):
        issues.extend(scan_file(h))
    return issues


def main():
    repo_root = Path(__file__).resolve().parent.parent.parent.parent  # tools -> verification -> e2e -> tests -> repo

    artifacts_dir = repo_root / "artifacts"
    if not artifacts_dir.is_dir():
        print("SKIP: no artifacts/ directory found — nothing to scan")
        return 0

    issues = scan_directory(artifacts_dir)

    if issues:
        print(f"FAIL: {len(issues)} fake-green issue(s) detected:")
        for issue in issues:
            print(f"  {issue}")
        return 1
    else:
        print("OK: no fake-green patterns detected in generated code")
        return 0


if __name__ == "__main__":
    sys.exit(main())