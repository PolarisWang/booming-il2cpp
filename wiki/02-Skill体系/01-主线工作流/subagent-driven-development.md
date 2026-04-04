# subagent-driven-development

> 项目中的子 agent 执行主线，负责在并行子上下文里完成任务，并同步维护任务目录状态。

## 基本信息

- 分层：主线工作流
- 项目职责：在子 agent 工作流下执行计划，同时维护 `STATUS.md`、`ACTIVE.md`、索引与 wiki 更新
- 实现路径：`.codex/skills/dev-subagent-driven-development/SKILL.md`
- 状态：`active`

## 在本项目中的作用

- 开始时确认任务总数
- 创建或更新当前任务目录中的 `STATUS.md`
- 创建或更新 `docs/dev/ACTIVE.md`
- 每任务后更新 `STATUS.md`、`ACTIVE.md`、索引与进度文档
- 对长期知识调用 `project-wiki-maintenance`
- 完成时将任务目录移动到 `docs/dev/completed/`

## 项目规则

- 子 agent 负责任务实现与审查，不负责绕过活动任务守卫
- 活动任务守卫仍然是任何新响应前的前置条件
- 规格合规性审查必须先于代码质量审查

## 最近变更

- `2026-04-04`：子 agent 执行主线从 `CURRENT.md` 迁移到任务目录状态模型。
