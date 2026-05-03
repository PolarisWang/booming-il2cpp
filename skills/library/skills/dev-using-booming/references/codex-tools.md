# Codex 工具映射

技能使用 Claude Code 工具名称。当你在技能中遇到这些工具时，使用你的平台等价物：

| 技能中的引用 | Codex 等价物 |
|------------|-------------|
| `Task` 工具（派发子 Agent） | `spawn_agent` |
| 多个 `Task` 调用（并行） | 多个 `spawn_agent` 调用 |
| Task 返回结果 | `wait` |
| Task 自动完成 | `close_agent` 释放槽位 |
| `TodoWrite`（任务跟踪） | `update_plan` |
| `Skill` 工具（调用技能） | 技能本地加载——直接遵循指令 |
| `Read`、`Write`、`Edit`（文件） | 使用你的原生文件工具 |
| `Bash`（运行命令） | 使用你的原生 shell 工具 |

## 子 Agent 派发需要 collab

添加到你的 Codex 配置（`~/.codex/config.toml`）：

```toml
[features]
collab = true
```

这为 `dispatching-parallel-agents` 和 `subagent-driven-development` 等技能启用 `spawn_agent`、`wait` 和 `close_agent`。
