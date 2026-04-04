# active-execution-guard

> 项目中的全局守卫，负责在任何问答前检查并处理 active 任务。

## 基本信息

- 分层：执行守卫
- 项目职责：检查 `docs/dev/ACTIVE.md`，并阻止未处理的 active 任务被绕过
- 实现路径：`.codex/skills/dev-active-execution-guard/SKILL.md`
- 状态：`active`

## 触发时机

- 任何回答前
- 任何澄清前
- 任何规划、实现、测试、调试、写文档前

## 在本项目中的作用

- `ACTIVE.md` 存在时，先要求用户处理当前任务
- 用户只能选择 `继续`、`挂起` 或 `放弃`
- 不允许用户手动选择 `completed`

## 项目规则

- `ACTIVE.md` 不完整也不能绕过
- `挂起` 与 `放弃` 处理完成后，才允许继续用户本次请求
- 只有执行技能可以自动产生 `completed`

## 最近变更

- `2026-04-04`：从 `CURRENT.md` 守卫迁移到 `ACTIVE.md` 守卫。
