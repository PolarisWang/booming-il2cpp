# subagent-driven-development

> 项目中的子 agent 执行主线，负责在并行子上下文里完成任务，并同步维护当前执行上下文。

## 基本信息

- 分层：主线工作流
- 项目职责：在子 agent 工作流下执行计划，同时维护 `CURRENT.md` 和 wiki 更新
- 实现路径：`.codex/skills/booming-code-subagent-driven-development/SKILL.md`
- 状态：`active`

## 在本项目中的作用

- 开始时确认任务总数
- 创建或更新 `docs/executions/CURRENT.md`
- 每任务后更新 `CURRENT.md`
- 对长期知识调用 `project-wiki-maintenance`
- 完成时自动归档 `completed`

## 项目规则

- 子 agent 负责任务实现与审查，不负责绕过活动计划守卫
- 活动计划守卫仍然是任何新响应前的前置条件

## 最近变更

- `2026-04-02`：补充 `CURRENT.md` 与 wiki 协同规则。
