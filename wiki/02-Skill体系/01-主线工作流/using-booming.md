# using-booming

> 项目中的全局 skill 入口，负责在任何行动前先决定应该走哪条技能链。

## 基本信息

- 分层：主线工作流
- 项目职责：统一 skill 分流，避免跳过调试、规划、测试治理和验证流程
- 实现路径：`.codex/skills/dev-using-booming/SKILL.md`
- 状态：`active`

## 触发时机

- 每次新会话开始
- 每次收到新的任务目标
- 每次准备进入分析、计划、实现、调试或验证前

## 在本项目中的特殊规则

- 只要任务涉及 `subjects/`、`tests/`、`Chaos.TestFramework`、collection file、runner、benchmark dashboard 或文件级 codegen，就必须先走 [`project-test-governance`](../04-%E8%B4%A8%E9%87%8F%E4%BF%9D%E9%9A%9C/project-test-governance.md)。
- bug、回归或数据异常必须优先走 `systematic-debugging`，不能直接跳到实现。
- 进入正式任务主线后，由计划/执行技能负责维护 `docs/dev/ACTIVE.md` 和任务状态。

## 常见分流

- 新功能、规范、架构设计：`brainstorming -> writing-plans`，复杂主线转 `roadmap`
- 继续执行现有计划：`executing-plans`
- bug / 回归 / benchmark 异常：`systematic-debugging -> project-test-governance -> test-driven-development`
- 测试主线、subject、runner、codegen 调整：`project-test-governance -> writing-plans` 或 `executing-plans`

## 最近变更

- `2026-04-17`：新增测试治理分流，要求 `subjects/test/Chaos.TestFramework/collection/runner/codegen` 相关任务先进入 `project-test-governance`。
