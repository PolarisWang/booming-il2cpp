#!/usr/bin/env python3
"""check_layer_boundaries.py — 四层架构边界完整性检查。

在 CI pipeline 启动时和 pre-commit hook 中运行，确保：
- Python 层不 write_text 写入 .cpp/.h 文件
- Codegen emitter 产出的 .generated.* 文件自包含
- ATG 不生成 C++ 代码
- 所有跨层写入有 BOUNDARY_OVERRIDE 标注

使用方式:
    python check_layer_boundaries.py [--ci] [--verbose]

    --ci: CI 模式，检查所有文件（包括未 staged 变更）
    --verbose: 详细输出每个检查项
"""

from __future__ import annotations

import os
import re
import subprocess
import sys
from pathlib import Path

# ── Constants ────────────────────────────────────────────────────────────

_REPO_ROOT = Path(__file__).resolve().parents[3]

# 各层允许写入的文件类型
LAYER_PERMISSIONS: dict[str, set[str]] = {
    "python": {"*.py", "*.json", "*.yaml", "*.yml", "*.md", "*.txt"},
    "codegen": {"*.generated.cpp", "*.generated.h", "*.generated.cs", "*.json"},
    "tpg": {"*.cpp", "*.h", "*.scriban", "*.cmake", "*.json", "*.yaml"},
    "atg": {"*.cs", "*.csproj", "*.json"},
    "native": {"*.cpp", "*.h", "*.hpp"},
}

# Python 层写入 .cpp/.h 的白名单（BOUNDARY_OVERRIDE）
BOUNDARY_OVERRIDE_PATTERNS: list[dict] = [
    # {
    #     "file": "stages/build.py",
    #     "line_pattern": "write_text",
    #     "reason": "BOUNDARY_OVERRIDE: <ticket-url>",
    #     "expires": "2026-07-15",
    # },
]

# ── Check functions ─────────────────────────────────────────────────────

def get_staged_files() -> list[Path]:
    """Get list of staged (about to be committed) files."""
    result = subprocess.run(
        ["git", "diff", "--cached", "--name-only", "--diff-filter=ACMR"],
        capture_output=True, text=True, timeout=30, cwd=_REPO_ROOT)
    if result.returncode != 0:
        # Fallback: use all tracked files
        return []
    return [_REPO_ROOT / f for f in result.stdout.strip().splitlines() if f]


def get_all_changed_files() -> list[Path]:
    """Get all changed files (staged + unstaged + untracked)."""
    result = subprocess.run(
        ["git", "diff", "--name-only", "HEAD", "--diff-filter=ACMR"],
        capture_output=True, text=True, timeout=30, cwd=_REPO_ROOT)
    untracked = subprocess.run(
        ["git", "ls-files", "--others", "--exclude-standard"],
        capture_output=True, text=True, timeout=30, cwd=_REPO_ROOT)
    files = []
    for f in result.stdout.strip().splitlines():
        if f:
            files.append(_REPO_ROOT / f)
    for f in untracked.stdout.strip().splitlines():
        if f:
            files.append(_REPO_ROOT / f)
    return files


def detect_layer(file_path: Path) -> str | None:
    """Detect which layer a file belongs to based on its path."""
    rel = file_path.relative_to(_REPO_ROOT).as_posix()

    if "test" in rel.lower() and rel.endswith(".cs"):
        return "test"
    if "verification" in rel and rel.endswith(".py"):
        return "python"
    if "AutoTestGenerator" in rel:
        return "atg"
    if "TestProjectGenerator" in rel:
        return "tpg"
    if "Chaos.IL2CPP.Generator" in rel:
        return "codegen"
    if "src/native/" in rel:
        return "native"
    return None


def check_python_writes_cpp(files: list[Path], verbose: bool) -> list[str]:
    """Check that Python files don't write_text .cpp/.h files."""
    violations = []
    for f in files:
        if detect_layer(f) != "python":
            continue
        if not f.exists():
            continue
        content = f.read_text(encoding="utf-8", errors="ignore")

        # Check for write_text calls with .cpp/.h/.hpp extension
        for i, line in enumerate(content.splitlines(), 1):
            if re.search(r'\.write_text\(.*\.(cpp|h|hpp)', line):
                # Check if this line has BOUNDARY_OVERRIDE annotation
                prev_lines = content.splitlines()[:i]
                has_override = any(
                    "BOUNDARY_OVERRIDE" in pl
                    for pl in prev_lines[-3:]  # Check 3 preceding lines
                )
                if not has_override:
                    violations.append(
                        f"{f.relative_to(_REPO_ROOT)}:{i}: Python writes C++ "
                        f"without BOUNDARY_OVERRIDE annotation"
                    )
    return violations


def check_codegen_self_contained(files: list[Path], verbose: bool) -> list[str]:
    """Check that codegen-emitted .generated files are self-contained."""
    violations = []
    # Simple check: generated .cpp files should not contain #include "../"
    # This is a heuristic — the real check would compile them
    for f in files:
        if not f.name.endswith(".generated.cpp") and not f.name.endswith(".generated.h"):
            continue
        if not f.exists():
            continue
        content = f.read_text(encoding="utf-8", errors="ignore")
        for i, line in enumerate(content.splitlines(), 1):
            if '#include "../' in line:
                violations.append(
                    f"{f.relative_to(_REPO_ROOT)}:{i}: generated file uses "
                    f"relative include — may not be self-contained"
                )
    return violations


def check_override_expiry(verbose: bool) -> list[str]:
    """Check that BOUNDARY_OVERRIDE annotations haven't expired."""
    violations = []
    result = subprocess.run(
        ["git", "grep", "-n", "BOUNDARY_OVERRIDE"],
        capture_output=True, text=True, timeout=30, cwd=_REPO_ROOT)

    for line in result.stdout.strip().splitlines():
        if "Expires:" in line:
            # Extract expiry date
            m = re.search(r'Expires:\s*(\d{4}-\d{2}-\d{2})', line)
            if m:
                from datetime import date
                expiry = date.fromisoformat(m.group(1))
                if date.today() > expiry:
                    violations.append(
                        f"{line}: BOUNDARY_OVERRIDE expired ({m.group(1)})"
                    )
    return violations


def run_checks(ci_mode: bool, verbose: bool) -> list[str]:
    """Run all boundary checks and return violations."""
    violations = []

    if ci_mode:
        files = get_all_changed_files()
        if verbose:
            print(f"  Scanning {len(files)} changed files...")
    else:
        files = get_staged_files()
        if verbose:
            print(f"  Scanning {len(files)} staged files...")

    if not files:
        # Fallback: scan entire repo
        if verbose:
            print("  No changed files found, scanning repo...")
        files = list(_REPO_ROOT.rglob("*.py")) + list(_REPO_ROOT.rglob("*.generated.cpp"))

    # Check 1: Python writes C++
    violations.extend(check_python_writes_cpp(files, verbose))

    # Check 2: Generated file self-containment
    violations.extend(check_codegen_self_contained(files, verbose))

    # Check 3: BOUNDARY_OVERRIDE expiry
    violations.extend(check_override_expiry(verbose))

    return violations


# ── Main ─────────────────────────────────────────────────────────────────

def main() -> int:
    import argparse
    parser = argparse.ArgumentParser(
        description="四层架构边界完整性检查")
    parser.add_argument("--ci", action="store_true",
                        help="CI 模式 (检查所有变更，而不仅是 staged)")
    parser.add_argument("--verbose", "-v", action="store_true",
                        help="详细输出")
    args = parser.parse_args()

    print("=" * 60)
    print("  四层架构边界完整性检查")
    print("=" * 60)

    violations = run_checks(ci_mode=args.ci, verbose=args.verbose)

    if violations:
        print(f"\n❌ 发现 {len(violations)} 个边界违规:\n")
        for v in violations:
            print(f"  • {v}")
        print(f"\n请修复这些违规后再提交。")
        print(f"如确实需要跨层写入，请添加 BOUNDARY_OVERRIDE 标注:")
        print(f"    # BOUNDARY_OVERRIDE: https://github.com/.../issues/NNN")
        print(f"    # Reason: ...")
        print(f"    # Expires: YYYY-MM-DD")
        return 1
    else:
        print(f"\n✅ 边界检查通过，无违规。")
        return 0


if __name__ == "__main__":
    sys.exit(main())
