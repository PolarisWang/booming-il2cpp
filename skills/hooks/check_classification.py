"""PreToolUse hook: 强制要求输出分类声明后才能使用非只读工具。

流程：
1. 用户发消息后，标记文件 `.claude/.classified` 不存在
2. Agent 必须先输出 `[c:...]` 并用 Bash 创建标记文件
3. 后续非只读工具调用被允许（标记文件存在）
4. Agent 在响应结束时用 Bash 删除标记文件
5. 下一条用户消息 → 标记文件不存在 → 必须重新分类

Bash 被豁免（可以创建/删除标记文件），但 Agent 应自觉先分类再用 Bash。
"""

import os
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
    print("  CLAUDE.md 第〇条规则要求：回复第一行必须是 [c:域数字|action|r轮次]", file=sys.stderr)
    print(file=sys.stderr)
    print("  示例: [c:1,2|fix|r1]  = 运行时+GC域，修复操作，第一轮", file=sys.stderr)
    print(file=sys.stderr)
    sys.exit(1)

sys.exit(0)
