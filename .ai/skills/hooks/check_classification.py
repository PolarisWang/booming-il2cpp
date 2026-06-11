"""PreToolUse hook: 验证分类声明格式 (简化版)

Skill 工具在非标准 API 环境下无法加载子 Expert，
因此 loaded_expert 强制验证已被移除。

流程:
1. 用户发消息 → Agent 输出分类声明 + "→ 加载 dev-xxx-expert"
2. echo "..." > .claude/.classified (hook 验证格式)
3. 后续 Edit/Write/Bash 域文件 → 仅提醒分类声明要求，不阻止
"""

import os
import re
import subprocess
import sys
from pathlib import Path


_RESOLVED_REPO_ROOT: Path | None = None


def _get_repo_root() -> Path | None:
    global _RESOLVED_REPO_ROOT
    if _RESOLVED_REPO_ROOT is not None:
        return _RESOLVED_REPO_ROOT
    # R9: try multiple fallback strategies
    strategies = [
        lambda: Path(__file__).resolve().parent,   # script dir
        lambda: Path.cwd(),                          # cwd
        None,
    ]
    for get_dir in strategies:
        if get_dir is None:
            break
        try:
            d = get_dir()
            output = subprocess.run(
                ["git", "-C", str(d), "rev-parse", "--show-toplevel"],
                capture_output=True, text=True, timeout=5,
            )
            root = output.stdout.strip()
            if root:
                _RESOLVED_REPO_ROOT = Path(root).resolve()
                return _RESOLVED_REPO_ROOT
        except Exception:
            continue
    return None


# R9: 多级 claude_dir 候选
_claude_candidates: list[Path] = [
    Path(__file__).resolve().parent.parent.parent / ".claude",
]
repo_root = _get_repo_root()
if repo_root:
    _claude_candidates.append(repo_root / ".claude")
_claude_candidates.append(Path.cwd() / ".claude")

_claude_dir = _claude_candidates[0]
for c in _claude_candidates:
    if c.exists():
        _claude_dir = c
        break

flag_file = _claude_dir / ".classified"

# R1: 多源工具名解析，不依赖单一 env var
tool_name = os.environ.get("CLAUDE_TOOL_NAME", "")
if not tool_name:
    for i, arg in enumerate(sys.argv):
        if arg == "--tool" and i + 1 < len(sys.argv):
            tool_name = sys.argv[i + 1]
            break
# R1: 如果仍无法获取工具名（非标准 API 未传递 env var），直接放行
# 避免对 Read/Grep/Glob 产生误报
if not tool_name:
    sys.exit(0)


# ═══════════════════════════════════════════════════════════════════
# 只读工具豁免
# ═══════════════════════════════════════════════════════════════════
ALLOWED_TOOLS = {"Read", "Grep", "Glob"}
if tool_name in ALLOWED_TOOLS:
    sys.exit(0)


# ═══════════════════════════════════════════════════════════════════
# Bash 处理 — 三档过滤
# ═══════════════════════════════════════════════════════════════════
if tool_name == "Bash":
    cmd = " ".join(sys.argv[1:]) if len(sys.argv) > 1 else ""

    # 第一档: 管理 Bash — 只操作 .claude/ 标记文件，不检查
    MANAGEMENT_PATTERNS = [
        r'echo\s+.*>.*\.claude/',
        r'echo\s+.*>.*\.loaded_expert',
        r'echo\s+.*>.*\.classified',
        r'rm\s+-f.*\.claude/',
        r'touch\s+.*\.claude/',
    ]
    for pat in MANAGEMENT_PATTERNS:
        if re.search(pat, cmd):
            sys.exit(0)

    # 第二档: 只读 Bash — 不影响源文件，不检查
    READONLY_BASH_WORDS = {
        "ls", "pwd", "which", "type", "date", "printf",
        "cat", "head", "tail", "wc", "sort", "uniq", "cut",
        "file", "du", "df", "ps", "env", "which",
        "git status", "git log", "git diff", "git show", "git branch",
        "git ls-files", "git ls-tree",
    }
    first_word = cmd.strip().split()[0] if cmd.strip() else ""
    if first_word in READONLY_BASH_WORDS:
        sys.exit(0)

    # 第三档: 域操作 Bash — 仅提醒分类要求（不强制 loaded_expert，因 Skill 工具不可用）
    # 无域路径匹配 → 放行
    sys.exit(0)


# ═══════════════════════════════════════════════════════════════════
# 分类声明 + Expert 声明验证（Edit/Write/Skill/Workflow）
# ═══════════════════════════════════════════════════════════════════
if not flag_file.exists():
    print(file=sys.stderr)
    print("  ⚠️  分类声明未输出！", file=sys.stderr)
    print(file=sys.stderr)
    sys.exit(1)

classification = flag_file.read_text(encoding="utf-8", errors="replace").strip()
if not classification:
    print("  ⚠️  分类文件为空！", file=sys.stderr)
    sys.exit(1)

# R6: 兼容中英文逗号、->/→ 箭头、灵活空格
pattern = (
    r'本轮任务涉及\s+(.+?)\s*[，,]\s*(\w+)\s*操作\s*[，,]\s*'
    r'第\s*(\d+)\s*轮\s*(?:→|->|=>|—>)?\s*加载\s+(dev-\S+)'
)
m = re.match(pattern, classification)
if not m:
    # R3: 格式错误 → 静默清理残留 .classified（非阻塞不打扰用户）
    flag_file.unlink(missing_ok=True)
    print(file=sys.stderr)
    print(f"  ⚠️  分类声明缺少 Expert 加载声明", file=sys.stderr)
    print("  格式: 本轮任务涉及 CodeGen(4) ，fix 操作，第 1 轮 → 加载 dev-il2cpp-codegen-expert", file=sys.stderr)
    sys.exit(1)

domain_part = m.group(1)
action = m.group(2)
round_str = m.group(3)
declared_expert = m.group(4)

VALID_ACTIONS = {"read", "fix", "build", "verify", "plan"}
if action not in VALID_ACTIONS:
    # R3: 无效 action → 静默清理并退出（避免跨会话残留）
    flag_file.unlink(missing_ok=True)
    print(f"  ⚠️  无效 action '{action}'", file=sys.stderr)
    sys.exit(1)

domain_nums = re.findall(r'\((\d+)\)', domain_part)
for dn in domain_nums:
    num = int(dn)
    if num < 1 or num > 8:
        print(f"  ⚠️  无效域编号 {num}", file=sys.stderr)
        sys.exit(1)

round_num = int(round_str)
if round_num < 1:
    print(f"  ⚠️  无效轮次 {round_num}", file=sys.stderr)
    sys.exit(1)

sys.exit(0)
