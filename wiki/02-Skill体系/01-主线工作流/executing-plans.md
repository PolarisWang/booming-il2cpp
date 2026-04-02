# executing-plans

> 项目中的顺序执行主线，负责执行计划、维护 `CURRENT.md`、归档 history 并触发项目知识沉淀。

## 基本信息

- 分层：主线工作流
- 项目职责：执行计划、维护 `docs/executions/CURRENT.md`、自动归档 execution history
- 实现路径：`.codex/skills/dev-executing-plans/SKILL.md`
- 状态：`active`

## 触发时机

- 已经有明确计划文档，且当前会话要顺序执行任务时
- 子 agent 不是标准路径，或当前选择直接在本会话执行时

## 在本项目中的作用

- 开始时确认任务总数
- 开始时创建或更新 `docs/executions/CURRENT.md`
- 每任务后更新 `CURRENT.md`
- 有长期知识时调用 `project-wiki-maintenance`
- 全部任务完成后自动归档为 `completed`

## 输入

- 设计文档
- 计划文档
- 当前执行上下文

## 输出

- `docs/executions/CURRENT.md`
- `docs/executions/history/execution-...-completed.md`
- 必要时的 wiki 更新

## 项目规则

- 用户不能手动选择 `completed`
- `completed` 只能在“任务全完成 + 验证通过 + wiki 更新完成”后自动产生
- 没有长期知识时，也要在 `CURRENT.md` 写明“本任务无 wiki 更新”

## 最近变更

- `2026-04-02`：接入 `CURRENT.md`、history archive 和 wiki 更新规则。
