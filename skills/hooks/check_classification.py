"""PreToolUse hook: 强制要求输出分类声明后才能使用非只读工具。

流程：
1. 用户发消息后，标记文件 `.claude/.classified` 不存在
2. Agent 必须先输出分类声明，并用 Bash 写入标记文件:
     echo "本轮任务涉及 运行时(1) + 构建(7) ，fix 操作，第 1 轮" > .claude/.classified
3. 后续非只读工具调用通过 hook 验证文件内容
4. Agent 在响应结束时用 Bash 删除标记文件
5. 下一条用户消息 → 标记文件不存在 → 必须重新分类

Bash 被豁免（可以创建/删除标记文件），但 Agent 应自觉先分类再用 Bash。
"""

import os
import re
import sys
from pathlib import Path

# 查找项目根目录下的 .claude
_here = Path(__file__).resolve().parent  # skills/hooks/
_claude_dir = _here.parent.parent / ".claude"
if not _claude_dir.exists():
    _claude_dir = Path.cwd() / ".claude"

flag_file = _claude_dir / ".classified"

tool_name = os.environ.get("CLAUDE_TOOL_NAME", sys.argv[1] if len(sys.argv) > 1 else "unknown")

# 只读工具和 Bash 可免检（Bash 用于管理标记文件）
ALLOWED_TOOLS = {"Read", "Grep", "Glob", "Bash"}
if tool_name in ALLOWED_TOOLS:
    sys.exit(0)

# 如果标记文件不存在，拒绝工具调用
if not flag_file.exists():
    print(file=sys.stderr)
    print("  ⚠️  分类声明未输出！", file=sys.stderr)
    print("  CLAUDE.md 第〇条规则要求：回复第一行必须是分类声明", file=sys.stderr)
    print("  格式：本轮任务涉及 {域1(编号)} + {域2(编号)} ... ，{action} 操作，第 N 轮", file=sys.stderr)
    print(file=sys.stderr)
    print("  示例: 本轮任务涉及 运行时(1) + 构建(7) ，fix 操作，第 1 轮", file=sys.stderr)
    print(file=sys.stderr)
    sys.exit(1)

# ── 内容验证 ────────────────────────────────────────────────────
# 读取分类声明并验证格式
classification = flag_file.read_text(encoding="utf-8", errors="replace").strip()
if not classification:
    print("  ⚠️  分类文件为空！请写入分类声明后再使用工具。", file=sys.stderr)
    sys.exit(1)

# 验证格式: "本轮任务涉及 ... ，{action} 操作，第 N 轮"
pattern = r'本轮任务涉及\s+(.+?)\s*，\s*(\w+)\s*操作，\s*第\s*(\d+)\s*轮'
m = re.match(pattern, classification)
if not m:
    print(f"  ⚠️  分类声明格式错误: '{classification}'", file=sys.stderr)
    print("  正确格式: 本轮任务涉及 运行时(1) + 构建(7) ，fix 操作，第 1 轮", file=sys.stderr)
    sys.exit(1)

domain_part = m.group(1)
action = m.group(2)
round_str = m.group(3)

# 验证 action
VALID_ACTIONS = {"read", "fix", "build", "verify", "plan"}
if action not in VALID_ACTIONS:
    print(f"  ⚠️  无效的 action '{action}'，允许: {', '.join(VALID_ACTIONS)}", file=sys.stderr)
    sys.exit(1)

# 验证 domain 编号（格式: "域1(编号1) + 域2(编号2)"）
domain_nums = re.findall(r'\((\d+)\)', domain_part)
for dn in domain_nums:
    num = int(dn)
    if num < 1 or num > 8:
        print(f"  ⚠️  无效的域编号 {num}，域编号必须在 1-8 之间", file=sys.stderr)
        sys.exit(1)

# 验证 round
round_num = int(round_str)
if round_num < 1:
    print(f"  ⚠️  无效的轮次 {round_num}，轮次必须是正整数", file=sys.stderr)
    sys.exit(1)

# ── Sub-agent guard ─────────────────────────────────────────────
# 检测是否通过 Skill 工具调用进入了子 Expert，防止递归循环
subagent_file = _claude_dir / ".subagent"
if tool_name == "Skill" and subagent_file.exists():
    subagent = subagent_file.read_text(encoding="utf-8", errors="replace").strip()
    print(f"  ⚠️ 已在子 Agent '{subagent}' 中，不允许通过 Skill 工具重新路由。",
          file=sys.stderr)
    print(f"  如需其他 Expert，请标记 remaining 后由 Dispatcher 分配。", file=sys.stderr)
    sys.exit(1)

sys.exit(0)
