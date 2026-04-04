# using-booming

> 项目中的全局 skill 入口，负责在任何行动前先选择正确技能。

## 基本信息

- 分层：主线工作流
- 项目职责：在项目里强制先检查活动任务守卫，再进入其他 skill
- 实现路径：`.codex/skills/dev-using-booming/SKILL.md`
- 状态：`active`

## 触发时机

- 每次新会话开始
- 每次收到新消息
- 每次准备响应、澄清、规划、实现或写文档前

## 在本项目中的作用

- 先决定是否需要 `active-execution-guard`
- 如果没有 active 任务，再选择头脑风暴、roadmap、计划、执行、测试等技能

## 项目规则

- `active-execution-guard` 优先级高于其他 skill
- 只要可能存在 `docs/dev/ACTIVE.md`，就必须先检查

## 关联技能

- 上游：无
- 下游：`brainstorming`、`roadmap`、`writing-plans`、`executing-plans`、`subagent-driven-development`
- 协作：`active-execution-guard`

## 最近变更

- `2026-04-04`：全局入口切换到 `ACTIVE.md`，并加入 roadmap 分流。
