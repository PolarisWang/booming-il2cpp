# executing-plans

> 项目中的顺序执行主线，负责执行计划、维护 `STATUS.md`、`ACTIVE.md`、索引和目录终态，并触发项目知识沉淀。

## 基本信息

- 分层：主线工作流
- 项目职责：执行计划、维护任务目录状态、在完成时把任务目录移动到 `completed/`
- 实现路径：`.codex/skills/dev-executing-plans/SKILL.md`
- 状态：`active`

## 触发时机

- 已经有明确计划文档，且当前会话要顺序执行任务时
- 子 agent 不是标准路径，或当前选择直接在本会话执行时

## 在本项目中的作用

- 开始时确认任务总数
- 创建或更新当前任务目录中的 `STATUS.md`
- 创建或更新 `docs/dev/ACTIVE.md`
- 每任务后更新 `STATUS.md`、`ACTIVE.md`、`notes/progress-*.md` 与索引
- 有长期知识时调用 `project-wiki-maintenance`
- 全部任务完成后将目录移动到 `docs/dev/completed/`

## 项目规则

- 用户不能手动选择 `completed`
- `completed` 只能在“任务全完成 + 验证通过 + wiki 更新完成”后自动产生
- 没有长期知识时，也要在 `STATUS.md` 或最新 `notes/progress-*.md` 写明“本任务无 wiki 更新”

## 最近变更

- `2026-04-04`：执行主线从 `CURRENT.md + history` 迁移到 `STATUS.md + ACTIVE.md + 目录终态`。
