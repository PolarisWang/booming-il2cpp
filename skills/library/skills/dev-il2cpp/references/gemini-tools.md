# Gemini CLI 工具映射

技能使用 Claude Code 工具名称。当你在技能中遇到这些工具时，使用你的平台等价物：

| 技能中的引用 | Gemini CLI 等价物 |
|------------|-----------------|
| `Read`（文件读取） | `read_file` |
| `Write`（文件创建） | `write_file` |
| `Edit`（文件编辑） | `replace` |
| `Bash`（运行命令） | `run_shell_command` |
| `Grep`（搜索文件内容） | `grep_search` |
| `Glob`（按名称搜索文件） | `glob` |
| `TodoWrite`（任务跟踪） | `write_todos` |
| `Skill` 工具（调用技能） | `activate_skill` |
| `WebSearch` | `google_web_search` |
| `WebFetch` | `web_fetch` |
| `Task` 工具（派发子 Agent） | 无等价物——Gemini CLI 不支持子 Agent |

## 无子 Agent 支持

Gemini CLI 没有 Claude Code 的 `Task` 工具的等价物。依赖子 Agent 派发的技能（`subagent-driven-development`、`dispatching-parallel-agents`）将通过 `executing-plans` 回退到单会话执行。

## Gemini CLI 的额外工具

这些工具在 Gemini CLI 中可用，但在 Claude Code 中没有等价物：

| 工具 | 用途 |
|------|------|
| `list_directory` | 列出文件和子目录 |
| `save_memory` | 跨会话将事实持久化到 GEMINI.md |
| `ask_user` | 向用户请求结构化输入 |
| `tracker_create_task` | 丰富的任务管理（创建、更新、列表、可视化） |
| `enter_plan_mode` / `exit_plan_mode` | 在做更改之前切换到只读研究模式 |
