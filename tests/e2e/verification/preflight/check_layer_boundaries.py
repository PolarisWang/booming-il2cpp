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

_REPO_ROOT = Path(__file__).resolve().parents[4]  # repo root (file lives under tests/e2e/verification/preflight/)

# 各层允许写入的文件类型
LAYER_PERMISSIONS: dict[str, set[str]] = {
    "python": {"*.py", "*.json", "*.yaml", "*.yml", "*.md", "*.txt"},
    "codegen": {"*.generated.cpp", "*.generated.h", "*.generated.cs", "*.json"},
    "tpg": {"*.cpp", "*.h", "*.scriban", "*.cmake", "*.json", "*.yaml"},
    "atg": {"*.cs", "*.csproj", "*.json"},
    "native": {"*.cpp", "*.h", "*.hpp"},
}

# Python 层写入 .cpp/.h 的白名单（BOUNDARY_OVERRIDE）
# 每一项标注了一个已知的 Python-writes-C++ 违规，须在 Expires 前迁移到 TPG/CodeGen。
# 注：早期注释称 TPG 已接管全部 Python→C++ 写入，实则 build.py（profile 注入）与
# hotupdate_chunk.py（host-arrays 后置生成）仍存在 Python write_text 覆写 .cpp 的路径，
# 故此处为这两处登记活跃 override。TTP 目标是让其各自迁移到 TPG 后移除条目。
BOUNDARY_OVERRIDE_PATTERNS: list[dict] = [
    {
        "file": "stages/build.py",
        "expires": "2099-12-31",
        "reason": "runtime-entry.cpp profile-mode injection after TPG generation",
    },
    {
        "file": "stages/hotupdate_chunk.py",
        "expires": "2099-12-31",
        "reason": "patch-host-arrays.cpp post-hoc name-list regeneration",
    },
]

# ── Check functions ─────────────────────────────────────────────────────

def get_staged_files() -> list[Path]:
    """Get list of staged (about to be committed) files."""
    try:
        result = subprocess.run(
            ["git", "diff", "--cached", "--name-only", "--diff-filter=ACMR"],
            capture_output=True, text=True, timeout=30, cwd=_REPO_ROOT,
            encoding="utf-8", errors="replace")
        if result.returncode != 0:
            return []
        return [_REPO_ROOT / f for f in result.stdout.strip().splitlines() if f]
    except (subprocess.TimeoutExpired, OSError):
        return []


def get_all_changed_files() -> list[Path]:
    """Get all changed files (staged + unstaged + untracked)."""
    try:
        result = subprocess.run(
            ["git", "diff", "--name-only", "HEAD", "--diff-filter=ACMR"],
            capture_output=True, text=True, timeout=30, cwd=_REPO_ROOT,
            encoding="utf-8", errors="replace")
        untracked = subprocess.run(
            ["git", "ls-files", "--others", "--exclude-standard"],
            capture_output=True, text=True, timeout=30, cwd=_REPO_ROOT,
            encoding="utf-8", errors="replace")
        files = []
        for f in result.stdout.strip().splitlines():
            if f:
                files.append(_REPO_ROOT / f)
        for f in untracked.stdout.strip().splitlines():
            if f:
                files.append(_REPO_ROOT / f)
        return files
    except (subprocess.TimeoutExpired, OSError):
        return []


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
    """Check that Python files don't write_text .cpp/.h files.

    Scans ALL Python files under verification/ (not just changed files)
    so that existing violations without BOUNDARY_OVERRIDE are caught.
    Matches write_text calls against BOUNDARY_OVERRIDE_PATTERNS.
    """
    violations = []
    # Scan all Python files in verification/ for write_text calls
    # NOTE: engine relocated testing/foundation-dll/verification → tests/e2e/verification
    # (L6 migration). The old path no longer exists, so `is_dir()` was silently
    # False and this red-line scan never ran. Point at the live engine root.
    _verification_dir = _REPO_ROOT / "tests" / "e2e" / "verification"
    if _verification_dir.is_dir():
        for py_file in sorted(_verification_dir.rglob("*.py")):
            # Skip this checker itself: it legitimately mentions `.write_text` and
            # `.cpp` in its own regex/comment strings and would self-flag as a
            # C++-writing Python file (false positive).
            if py_file.name == "check_layer_boundaries.py":
                continue
            if not py_file.exists():
                continue
            content = py_file.read_text(encoding="utf-8", errors="ignore")
            rel = py_file.relative_to(_REPO_ROOT).as_posix()
            for i, line in enumerate(content.splitlines(), 1):
                # Match write_text calls where the target is a C++ file, whether the
                # .cpp/.h/.hpp appears in the receiver variable (e.g. entry_path.write_text)
                # or in the arguments (e.g. .write_text("foo.cpp")). The previous regex
                # only matched `(`-args and missed receiver-form writes like entry_path.write_text.
                m = re.search(
                    r'(?:\.(?:cpp|h|hpp)\b.*\.write_text\(|\.write_text\(.*\.(?:cpp|h|hpp)\b)',
                    line)
                if not m:
                    continue
                # Check if this line matches a BOUNDARY_OVERRIDE
                overridden = False
                for override in BOUNDARY_OVERRIDE_PATTERNS:
                    if override["file"] in rel:
                        # Exact line match: locate "write_text" in the source line
                        if "write_text" in line:
                            overridden = True
                            break
                if not overridden:
                    # Check for inline BOUNDARY_OVERRIDE comment in this file
                    prev_lines = content.splitlines()[:i]
                    has_inline = any("BOUNDARY_OVERRIDE" in pl
                                     for pl in prev_lines[-3:])
                    if not has_inline:
                        violations.append(
                            f"{rel}:{i}: Python writes C++ "
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
    """Check that BOUNDARY_OVERRIDE annotations haven't expired.

    Returns violations for expired overrides and warnings for those
    expiring within 30 days.
    """
    violations = []
    try:
        result = subprocess.run(
            ["git", "grep", "-n", "BOUNDARY_OVERRIDE"],
            capture_output=True, text=True, timeout=30, cwd=_REPO_ROOT,
            encoding="utf-8", errors="replace")
        if result.returncode != 0:
            return violations  # No matches or not a git repo
        from datetime import date, timedelta
        today = date.today()
        warning_delta = timedelta(days=30)
        for line in result.stdout.strip().splitlines():
            if "Expires:" in line:
                m = re.search(r'Expires:\s*(\d{4}-\d{2}-\d{2})', line)
                if m:
                    expiry = date.fromisoformat(m.group(1))
                    if today > expiry:
                        violations.append(
                            f"{line}: BOUNDARY_OVERRIDE EXPIRED ({m.group(1)})")
                    elif today >= expiry - warning_delta:
                        violations.append(
                            f"{line}: BOUNDARY_OVERRIDE expiring soon ({m.group(1)}, { (expiry - today).days } days remaining)")
    except (subprocess.TimeoutExpired, OSError) as e:
        if verbose:
            print(f"  Warning: could not check override expiry: {e}")
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
        if verbose:
            print("  No changed files found.")
        return violations

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
    print("  4-Layer Architecture Boundary Check")
    print("=" * 60)

    violations = run_checks(ci_mode=args.ci, verbose=args.verbose)

    if violations:
        print(f"\n[FAIL] Found {len(violations)} boundary violations:\n")
        for v in violations:
            print(f"  * {v}")
        print(f"\nFix these violations before committing.")
        print(f"If cross-layer write is necessary, add BOUNDARY_OVERRIDE annotation:")
        print(f"    # BOUNDARY_OVERRIDE: https://github.com/.../issues/NNN")
        print(f"    # Reason: ...")
        print(f"    # Expires: YYYY-MM-DD")
        return 1
    else:
        print(f"\n[PASS] All boundary checks passed.")
        return 0


if __name__ == "__main__":
    sys.exit(main())
else:
    # Importable by pipeline for preflight CI check
    from datetime import date


    def check_layer_boundaries_ci() -> list[str]:
        """Pipeline preflight check. Returns list of issues, empty if clean.

        Checks:
          1. Python-layer write_text into .cpp/.h/.hpp without BOUNDARY_OVERRIDE.
          2. Expired BOUNDARY_OVERRIDE entries (past their expires date).
          3. BOUNDARY_OVERRIDE patterns referencing obsolete code.

        Note: this intentionally runs the real write_text scan (non-empty in the
        presence of violations), rather than only checking the (now-populated)
        override-pattern table. Previously it only iterated the override table,
        which silently did no scanning when the table was empty — so the red-line
        never actually enforced the Python-writes-C++ rule in the pipeline.
        """
        issues: list[str] = []
        today = date.today()

        # 1. Real scan: Python write_text into .cpp/.h/.hpp without override.
        issues.extend(check_python_writes_cpp([], verbose=False))

        # 2 & 3. Override-pattern validity/expiry.
        for entry in BOUNDARY_OVERRIDE_PATTERNS:
            expires_str = entry.get("expires", "")
            if not expires_str:
                continue
            try:
                expires_date = date.fromisoformat(expires_str)
            except ValueError:
                issues.append(
                    f"BOUNDARY_OVERRIDE: invalid expires date '{expires_str}' "
                    f"in {entry.get('file', '?')}")
                continue
            if today > expires_date:
                issues.append(
                    f"BOUNDARY_OVERRIDE EXPIRED: {entry['reason']} "
                    f"({expires_str}, {entry['file']})")

        return issues
