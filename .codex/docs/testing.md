# 测试 Booming 技能

本文档描述如何测试 Booming 技能，特别是像 `subagent-driven-development` 这样的复杂技能的集成测试。

## 概述

测试涉及子 Agent、工作流和复杂交互的技能，需要以无头模式运行实际的 Claude Code 会话，并通过会话记录来验证其行为。

## 测试结构

```
tests/
├── claude-code/
│   ├── test-helpers.sh                    # 共享测试工具
│   ├── test-subagent-driven-development-integration.sh
│   ├── analyze-token-usage.py             # Token 分析工具
│   └── run-skill-tests.sh                 # 测试运行器（如果存在）
```

## 运行测试

### 集成测试

集成测试以实际技能执行真实的 Claude Code 会话：

```bash
# 运行 subagent-driven-development 集成测试
cd tests/claude-code
./test-subagent-driven-development-integration.sh
```

**注意：** 集成测试可能需要 10-30 分钟，因为它们执行带有多个子 Agent 的真实实现计划。

### 要求

- 必须从 **booming 插件目录**运行（不是从临时目录）
- Claude Code 必须已安装且作为 `claude` 命令可用
- 必须启用本地开发市场：在 `~/.claude/settings.json` 中设置 `"booming@booming-dev": true`

## 集成测试：subagent-driven-development

### 它测试什么

集成测试验证 `subagent-driven-development` 技能是否正确：

1. **计划加载**：在开始时读取计划一次
2. **完整任务文本**：向子 Agent 提供完整的任务描述（不让他们读取文件）
3. **自审**：确保子 Agent 在报告之前进行自审
4. **审查顺序**：在代码质量审查之前运行规格合规性审查
5. **审查循环**：在发现问题时使用审查循环
6. **独立验证**：规格审查者独立读取代码，不信任实现者的报告

### 工作原理

1. **设置**：创建一个带有最小实现计划的临时 Node.js 项目
2. **执行**：以无头模式运行带有技能的 Claude Code
3. **验证**：解析会话记录（`.jsonl` 文件）来验证：
   - 调用了 Skill 工具
   - 派发了子 Agent（Task 工具）
   - 使用了 TodoWrite 进行跟踪
   - 创建了实现文件
   - 测试通过
   - Git 提交显示正确的工作流
4. **Token 分析**：按子 Agent 显示 Token 使用情况细分

### 测试输出

```
========================================
 集成测试：subagent-driven-development
========================================

测试项目：/tmp/tmp.xyz123

=== 验证测试 ===

测试 1：已调用 Skill 工具...
  [通过] subagent-driven-development 技能已调用

测试 2：已派发子 Agent...
  [通过] 已派发 7 个子 Agent

测试 3：任务跟踪...
  [通过] TodoWrite 使用了 5 次

测试 6：实现验证...
  [通过] src/math.js 已创建
  [通过] add 函数存在
  [通过] multiply 函数存在
  [通过] test/math.test.js 已创建
  [通过] 测试通过

测试 7：Git 提交历史...
  [通过] 创建了多个提交（共 3 个）

测试 8：未添加额外功能...
  [通过] 未添加额外功能

=========================================
 Token 使用情况分析
=========================================

使用情况细分：
----------------------------------------------------------------------------------------------------
Agent           描述                                 消息数    输入      输出      缓存      成本
----------------------------------------------------------------------------------------------------
main            主会话（协调器）                       34       27      3,996  1,213,703  $4.09
3380c209        实现任务 1：创建 Add 函数               1        2        787     24,989  $0.09
...

总计：
  估计成本：$4.67

========================================
 测试摘要
========================================

状态：通过
```

## Token 分析工具

### 用法

分析任何 Claude Code 会话的 Token 使用情况：

```bash
python3 tests/claude-code/analyze-token-usage.py ~/.claude/projects/<project-dir>/<session-id>.jsonl
```

### 查找会话文件

会话记录存储在 `~/.claude/projects/` 中，工作目录路径被编码：

```bash
# /Users/jesse/Documents/GitHub/booming/booming 的示例
SESSION_DIR="$HOME/.claude/projects/-Users-jesse-Documents-GitHub-booming-booming"

# 查找最近的会话
ls -lt "$SESSION_DIR"/*.jsonl | head -5
```

### 显示内容

- **主会话使用情况**：协调器（你或主 Claude 实例）的 Token 使用情况
- **每子 Agent 细分**：每个 Task 调用，包括：
  - Agent ID
  - 描述（从提示中提取）
  - 消息数
  - 输入/输出 Token
  - 缓存使用情况
  - 估计成本
- **总计**：整体 Token 使用情况和成本估计

### 理解输出

- **高缓存读取**：好——意味着提示缓存在起作用
- **主会话上的高输入 Token**：预期——协调器有完整上下文
- **每子 Agent 类似成本**：预期——每个获得类似任务复杂度
- **每任务成本**：典型范围是每个子 Agent $0.05-$0.15，取决于任务

## 故障排除

### 技能未加载

**问题**：运行无头测试时找不到技能

**解决方案**：
1. 确保你从 booming 目录运行：`cd /path/to/booming && tests/...`
2. 检查 `~/.claude/settings.json` 的 `enabledPlugins` 中是否有 `"booming@booming-dev": true`
3. 验证技能存在于 `skills/` 目录

### 权限错误

**问题**：Claude 被阻止写入文件或访问目录

**解决方案**：
1. 使用 `--permission-mode bypassPermissions` 标志
2. 使用 `--add-dir /path/to/temp/dir` 授予对测试目录的访问权限
3. 检查测试目录的文件权限

### 测试超时

**问题**：测试花费太长时间并超时

**解决方案**：
1. 增加超时：`timeout 1800 claude ...`（30 分钟）
2. 检查技能逻辑中的无限循环
3. 审查子 Agent 任务复杂度

### 未找到会话文件

**问题**：测试运行后找不到会话记录

**解决方案**：
1. 检查 `~/.claude/projects/` 中的正确项目目录
2. 使用 `find ~/.claude/projects -name "*.jsonl" -mmin -60` 查找最近的会话
3. 验证测试实际上已运行（检查测试输出中的错误）

## 编写新集成测试

### 模板

```bash
#!/usr/bin/env bash
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
source "$SCRIPT_DIR/test-helpers.sh"

# 创建测试项目
TEST_PROJECT=$(create_test_project)
trap "cleanup_test_project $TEST_PROJECT" EXIT

# 设置测试文件...
cd "$TEST_PROJECT"

# 用技能运行 Claude
PROMPT="你的测试提示在这里"
cd "$SCRIPT_DIR/../.." && timeout 1800 claude -p "$PROMPT" \
  --allowed-tools=all \
  --add-dir "$TEST_PROJECT" \
  --permission-mode bypassPermissions \
  2>&1 | tee output.txt

# 查找并分析会话
WORKING_DIR_ESCAPED=$(echo "$SCRIPT_DIR/../.." | sed 's/\\//-/g' | sed 's/^-//')
SESSION_DIR="$HOME/.claude/projects/$WORKING_DIR_ESCAPED"
SESSION_FILE=$(find "$SESSION_DIR" -name "*.jsonl" -type f -mmin -60 | sort -r | head -1)

# 通过解析会话记录验证行为
if grep -q '"name":"Skill".*"skill":"your-skill-name"' "$SESSION_FILE"; then
    echo "[通过] 技能已调用"
fi

# 显示 Token 分析
python3 "$SCRIPT_DIR/analyze-token-usage.py" "$SESSION_FILE"
```

### 最佳实践

1. **始终清理**：使用 trap 清理临时目录
2. **解析记录**：不要 grep 用户面向的输出——解析 `.jsonl` 会话文件
3. **授予权限**：使用 `--permission-mode bypassPermissions` 和 `--add-dir`
4. **从插件目录运行**：技能只有在从 booming 目录运行时才加载
5. **显示 Token 使用情况**：始终包含 Token 分析以了解成本
6. **测试真实行为**：验证实际创建的文件、通过的测试、做出的提交

## 会话记录格式

会话记录是 JSONL（JSON Lines）文件，每行是一个 JSON 对象，代表一条消息或工具结果。

### 关键字段

```json
{
  "type": "assistant",
  "message": {
    "content": [...],
    "usage": {
      "input_tokens": 27,
      "output_tokens": 3996,
      "cache_read_input_tokens": 1213703
    }
  }
}
```

### 工具结果

```json
{
  "type": "user",
  "toolUseResult": {
    "agentId": "3380c209",
    "usage": {
      "input_tokens": 2,
      "output_tokens": 787,
      "cache_read_input_tokens": 24989
    },
    "prompt": "你正在实现任务 1...",
    "content": [{"type": "text", "text": "..."}]
  }
}
```

`agentId` 字段链接到子 Agent 会话，`usage` 字段包含该特定子 Agent 调用的 Token 使用情况。
