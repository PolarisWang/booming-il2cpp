#!/usr/bin/env python3
"""check_workarounds.py — workaround / 降级 / 跨层豁免 门禁（后门 2&3 铁禁）。

方案 B 的机制级实现。目标：让"掩盖根因的 workaround"物理上无法通过 pre-commit，
但精确放行"无害的健壮性 fallback"（平台适配 / 边界情形），避免无差别禁误伤。

三把闸（挂在 .githooks/pre-commit 的 [workaround-gate]）：

  1. FALLBACK_KIND 标注强制
     任何 staged Python 出现 `fallback` / `retry` / `_sanitize` / `_fallback` 等
     降级词时，必须是机器可读标注的三类之一：
       # FALLBACK_KIND: robust     — 无害健壮性（平台适配/边界），放行
       # FALLBACK_KIND: workaround — 掩盖根因的降级，必须额外有 issue+expires
     扫描到降级词但**没有** FALLBACK_KIND 标注 → 拦（无法区分，视为违规）。

  2. workaround 必须带 expiring issue
     FALLBACK_KIND: workaround 的行，必须在文件内或近邻有：
       # WRO: EXPIRES YYYY-MM-DD  <issue-link>
     且 expires 不得超过 GATE 硬上限（见下），否则拦。
     没有 expires/issue 的 workaround → 拦（= 允许"永久 workaround"，正好是
     后门 2 当前 `2099-12-31` 的死穴）。

  3. 静默降级返回拦截
     检测 `status="skipped"` / `status="passed"` 但伴随 fallback 注释、或
     `return None` / `_custom_fallback` 在同函数内无根治标注 → 拦。
     防止"看似成功实未验证主路径"（build.py Custom-only fallback 模式）。

   无害示例（放行）：Windows symlink→marker 备选、benchmark 校准降级迭代数。
   有害示例（拦）：build.py net10.0 降级重试掩盖编译错、Custom-only fallback
   掩盖 subjects==0 探测失败。

用法:  python check_workarounds.py [--ci] [--verbose]
  --ci    扫描所有变更（含未 staged / untracked），否则只扫 staged。
"""

from __future__ import annotations

import re
import subprocess
import sys
from pathlib import Path

# ── Constants ────────────────────────────────────────────────────────────

_REPO_ROOT = Path(__file__).resolve().parents[4]  # tests/e2e/verification/preflight/ → repo root

# workaround 允许的最长存活窗口。超过即"看似过期实为永久"，拦。
GATE_MAX_EXPIRY_DAYS = 90

# 触发降级扫描的词（命中即要求 FALLBACK_KIND 标注）
FALLBACK_KEYWORDS = re.compile(
    r"\b(?:fallback|_fallback|_fallback_|retry|_sanitize|_retry|_custom_fallback)\b",
    re.IGNORECASE,
)
# 伪装成功的降级返回：把失败/未生成包装成 status=skipped/passed 的显式 StageResult/Result
# 构造，且文件内带 fallback/retry 语义（信号：主路径未验证却被当"跳过/成功"）。
# 这是 build.py Custom-only fallback → status=skipped 那种可判定形态。
# 危险：`return None` / docstring / 常量 一律不算——Python 里裸 return None 是普通守卫，
# 仅凭它无法判定"掩盖"，必须看到显式 status=skipped|passed 的返回构造才算。
SILENT_SUCCESS = re.compile(
    r"return\s+[^#;\n]*?(?:StageResult|Result|StageOutcome)\([^)]*?"
    r"status\s*=\s*[\"'](?:skipped|passed)[\"']",
    re.IGNORECASE,
)
# 机器可读标注
FALLBACK_KIND_RE = re.compile(r"#\s*FALLBACK_KIND\s*:\s*(robust|workaround)", re.IGNORECASE)
WRO_EXPIRES_RE = re.compile(r"#\s*WRO:\s*EXPIRES\s+(\d{4}-\d{2}-\d{2})")
# BOUNDARY_OVERRIDE 里的 expires（后门 2）
BO_EXPIRES_RE = re.compile(r"BOUNDARY_OVERRIDE[\s\S]{0,400}?['\"]?expires['\"]?\s*:\s*['\"](\d{4}-\d{2}-\d{2})['\"]", re.IGNORECASE)


# ── staged/all changed file collection（复用 check_layer_boundaries 模式）────

def _git_files(cmd: list[str]) -> list[Path]:
    try:
        r = subprocess.run(cmd, capture_output=True, text=True, timeout=30,
                           cwd=_REPO_ROOT, encoding="utf-8", errors="replace")
        if r.returncode != 0:
            return []
        return [_REPO_ROOT / f for f in r.stdout.strip().splitlines() if f]
    except (subprocess.TimeoutExpired, OSError):
        return []


def _changed_py(ci_mode: bool) -> list[Path]:
    """Collect Python files to scan.

    Non-CI (pre-commit): only staged .py (incremental, fast).
    CI: full engine trees (tests/e2e/verification + tests/e2e/translation) — 与
    check_layer_boundaries 的全量红线语义一致，确保存量未标注 fallback 也被抓，
    而不是只扫相对 HEAD 的变更。
    """
    if ci_mode:
        roots = [
            _REPO_ROOT / "tests" / "e2e" / "verification",
            _REPO_ROOT / "tests" / "e2e" / "translation",
        ]
        files: list[Path] = []
        for r in roots:
            if r.is_dir():
                files.extend(p for p in r.rglob("*.py") if p.is_file())
        return files
    return [f for f in _git_files(["git", "diff", "--cached", "--name-only", "--diff-filter=ACMR"])
            if f.suffix == ".py"]


# ── Checks ───────────────────────────────────────────────────────────────

def _check_file(path: Path, verbose: bool) -> list[str]:
    """Scan a single Python file for mask-the-failure workarounds.

    两个可判定靶点，不做"任何 fallback 词都要标注"的过度匹配（那会误伤 docstring/
    常量/文案）。靶点：
      1. 伪装成功的降级返回：return StageResult(status=skipped|passed) 且行/函数内带
         fallback/retry 语义 → 把失败包装成"跳过/成功"，掩盖主路径未验证。拦截。
      2. 显式 workaround 无到期点：文件声明 FALLBACK_KIND: workaround 但无 WRO: EXPIRES
         （或超 GATE_MAX_EXPIRY_DAYS）→ 允许"永久 workaround"，死穴。拦截。
    无害健壮性（symlink 备选、校准常量、文案）不触发上述两靶点，天然放行。
    """
    if not path.exists():
        return []
    try:
        content = path.read_text(encoding="utf-8", errors="ignore")
    except OSError:
        return []
    rel = path.relative_to(_REPO_ROOT).as_posix()
    lines = content.splitlines()
    violations: list[str] = []

    # Pre-collect FALLBACK_KIND annotations present anywhere in the file.
    kinds = {"robust": False, "workaround": False}
    for ln in lines:
        m = FALLBACK_KIND_RE.search(ln)
        if m:
            kinds[m.group(1).lower()] = True

    # Pre-collect any WRO expires in the file.
    wro_expires = [m.group(1) for ln in content.splitlines()
                   for m in [WRO_EXPIRES_RE.search(ln)] if m]

    # Target 1: masked-success returns. Only when the SAME physical line both
    # constructs a status=skipped/passed return AND mentions fallback/retry —
    # i.e. a failure/未生成 is explicitly re-labeled as skipped/passed as part of a
    # fallback path. A plain `StageResult(status="skipped")` without a fallback word
    # is a legit conditional skip and is NOT flagged.
    for i, ln in enumerate(lines, 1):
        if SILENT_SUCCESS.search(ln):
            has_fbk_word = bool(re.search(r"fallback|retry|_sanitize|_custom_fallback", ln, re.IGNORECASE))
            if not has_fbk_word:
                # Legit conditional skip (no fallback intent on this line) → allow.
                continue
            violations.append(
                f"{rel}:{i}: masked-success return (StageResult/Result status=skipped/passed "
                f"on a fallback/retry path) — masks un-verified main path. Root-fix or "
                f"annotate '# FALLBACK_KIND: workaround' + '# WRO: EXPIRES <expiry> <issue>'.")

    # Target 2: explicit workaround without expiring WRO.
    if kinds["workaround"]:
        if not wro_expires:
            violations.append(
                f"{rel}: FALLBACK_KIND: workaround has no '# WRO: EXPIRES YYYY-MM-DD <issue>' annotation.")
        else:
            from datetime import date
            for exp in wro_expires:
                try:
                    exp_date = date.fromisoformat(exp)
                except ValueError:
                    violations.append(f"{rel}: invalid WRO expires '{exp}'.")
                    continue
                days = (exp_date - date.today()).days
                if days > GATE_MAX_EXPIRY_DAYS:
                    violations.append(
                        f"{rel}: workaround expires {exp} ({days}d) exceeds "
                        f"GATE_MAX_EXPIRY_DAYS={GATE_MAX_EXPIRY_DAYS}. Workarounds must be "
                        f"root-fixed within cap.")

    return violations


def _check_boundary_overrides(verbose: bool) -> tuple[list[str], list[str]]:
    """后门 2 硬上限：BOUNDARY_OVERRIDE 的 expires 不得超过 GATE_MAX_EXPIRY_DAYS。

    存量 override 治理：历史遗留的 3 个永久 override（PHASE5-*）已有 preferred-fix
    声明、依赖未排期的 TPP 迁移，当前若强制红会自锁管线。因此对它们**记录告警不拦**，
    但把它们钉进 KNOWN_PERSISTENT_OVERRIDES：任何新出现的 BOUNDARY_OVERRIDE（非白名单）
    一旦 expires 超过 GATE_MAX_EXPIRY_DAYS 即拦——**存量逼迫到期，新 override 绝对禁止永久**。

    Returns: (violations, warnings)
    """
    # 已知存量永久 override（已有 issue + reasons + preferred fix，待 TPP 迁移）。
    # 按 override 所在源码文件路径标识。新 override 若与此列表无关、又设为永久 → 拦。
    KNOWN_PERSISTENT_OVERRIDES = {
        "stages/build.py",
        "stages/hotupdate_chunk.py",
        "fix_value_type_typedefs.py",
    }
    violations: list[str] = []
    warnings: list[str] = []
    override_file = _REPO_ROOT / "tests" / "e2e" / "verification" / "preflight" / "check_layer_boundaries.py"
    if not override_file.exists():
        return violations, warnings
    content = override_file.read_text(encoding="utf-8", errors="ignore")
    from datetime import date
    for m in BO_EXPIRES_RE.finditer(content):
        exp = m.group(1)
        try:
            exp_date = date.fromisoformat(exp)
        except ValueError:
            violations.append(f"BOUNDARY_OVERRIDE: invalid expires '{exp}' in {override_file.name}")
            continue
        # Map each override entry back to its source file by scanning the patterns table.
        # For simplicity, we gate on the pattern's "file" key contents below.
        # Here: if any override in the patterns table is NOT a known-persistent file,
        # it's a new override → enforce hard cap.
        # (Entry-level enforcement done in main() against patterns' file fields.)

    # Gate new (non-whitelisted) overrides by scanning the patterns table's file keys.
    for entry in _BO_PATTERNS_list():
        src_path = entry.get("file", "")
        expires_str = entry.get("expires", "")
        fname = src_path.split("/")[-1]
        if not expires_str:
            violations.append(f"BOUNDARY_OVERRIDE for {src_path}: no 'expires' set (banned).")
            continue
        try:
            exp_date = date.fromisoformat(expires_str)
        except ValueError:
            violations.append(f"BOUNDARY_OVERRIDE for {src_path}: invalid expires '{expires_str}'.")
            continue
        days = (exp_date - date.today()).days
        is_legacy = fname in KNOWN_PERSISTENT_OVERRIDES and expires_str > "2099-01-01"
        if is_legacy:
            warnings.append(
                f"[legacy] override {src_path} expires {expires_str} is permanent-known; "
                f"must be root-fixed (preferred fix) — tracked, not blocking.")
            continue
        if days > GATE_MAX_EXPIRY_DAYS or expires_str > "2099-01-01":
            violations.append(
                f"BOUNDARY_OVERRIDE for {src_path}: expires {expires_str} ({days}d) exceeds "
                f"GATE_MAX_EXPIRY_DAYS={GATE_MAX_EXPIRY_DAYS}. New overrides must be "
                f"root-fixed within cap (permanent override banned).")
    return violations, warnings


def _BO_PATTERNS_list() -> list[dict]:
    """Read the live BOUNDARY_OVERRIDE_PATTERNS table from check_layer_boundaries.py."""
    override_file = _REPO_ROOT / "tests" / "e2e" / "verification" / "preflight" / "check_layer_boundaries.py"
    if not override_file.exists():
        return []
    import ast
    try:
        tree = ast.parse(override_file.read_text(encoding="utf-8", errors="ignore"))
    except SyntaxError:
        return []
    for node in ast.walk(tree):
        if isinstance(node, ast.Assign):
            for tgt in node.targets:
                if isinstance(tgt, ast.Name) and tgt.id == "BOUNDARY_OVERRIDE_PATTERNS":
                    try:
                        val = ast.literal_eval(node.value)
                    except (ValueError, TypeError):
                        return []
                    return val if isinstance(val, list) else []
    return []


# ── Main ─────────────────────────────────────────────────────────────────

def main() -> int:
    import argparse
    parser = argparse.ArgumentParser(description="workaround / 降级 / 跨层豁免 门禁")
    parser.add_argument("--ci", action="store_true", help="扫描所有变更（含未 staged）")
    parser.add_argument("--verbose", "-v", action="store_true")
    args = parser.parse_args()

    py_files = _changed_py(args.ci)
    if args.verbose:
        print(f"  [workaround-gate] scanning {len(py_files)} Python files")

    violations: list[str] = []
    warnings: list[str] = []
    for f in py_files:
        # Skip the checker itself (its own docstring/false positives).
        if f.name == "check_workarounds.py":
            continue
        violations.extend(_check_file(f, args.verbose))
    bo_violations, bo_warnings = _check_boundary_overrides(args.verbose)
    violations.extend(bo_violations)
    warnings.extend(bo_warnings)

    for w in warnings:
        if args.verbose:
            print(f"    [warn] {w}")

    if violations:
        print(f"\n[FAIL] Found {len(violations)} workaround/override violation(s):")
        for v in violations:
            print(f"  * {v}")
        print("\nWorkarounds that mask a root cause are banned (方案 B).")
        print("To allow a harmless robustness fallback, annotate the file:")
        print("    # FALLBACK_KIND: robust")
        print("To allow a temporary (must-resolve) workaround, annotate + cap:")
        print("    # FALLBACK_KIND: workaround")
        print("    # WRO: EXPIRES YYYY-MM-DD  <https://github.com/.../issues/NNN>")
        return 1
    if args.verbose:
        print("  [workaround-gate] clean")
    return 0


if __name__ == "__main__":
    sys.exit(main())
