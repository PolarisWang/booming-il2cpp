# active-execution-guard

> 项目中的全局守卫，负责在任何问答前检查并处理活动计划。

## 基本信息

- 分层：执行守卫
- 项目职责：检查 `docs/executions/CURRENT.md`，并阻止未处理的活动计划被绕过
- 实现路径：`.codex/skills/booming-code-active-execution-guard/SKILL.md`
- 状态：`active`

## 触发时机

- 任何回答前
- 任何澄清前
- 任何规划、实现、测试、调试、写文档前

## 在本项目中的作用

- `CURRENT.md` 存在时，先要求用户处理当前计划
- 用户只能选择 `继续` 或 `放弃`
- 不允许用户手动选择 `completed`

## 项目规则

- `CURRENT.md` 不完整也不能绕过
- 只有执行技能可以自动产生 `completed` history

## 最近变更

- `2026-04-02`：新增全局活动计划守卫。
