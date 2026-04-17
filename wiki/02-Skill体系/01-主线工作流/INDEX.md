# 01-主线工作流 INDEX

> 收录项目主线流程 skill 的项目化解释。

## 子类别

| 类别 | 说明 | 索引 |
|------|------|------|
| `-` | 当前无下级目录 | `-` |

## 重要文档

| 文档 | 主题 | 说明 |
|------|------|------|
| [`using-booming.md`](./using-booming.md) | 全局入口 | 说明 skill 优先级和活动任务守卫优先级 |
| [`roadmap.md`](./roadmap.md) | 路线图规划 | 说明 roadmap 父任务、阶段与子任务派生 |
| [`writing-plans.md`](./writing-plans.md) | 计划编写 | 说明计划如何绑定设计与 wiki 落点 |
| [`executing-plans.md`](./executing-plans.md) | 计划执行 | 说明 `STATUS.md`、`ACTIVE.md` 与目录终态 |
| [`subagent-driven-development.md`](./subagent-driven-development.md) | 子 agent 执行 | 说明子 agent 主线如何维护任务目录状态 |

## 本层规则

- 主线工作流页只解释“项目里怎么用”，不复制原始 `SKILL.md`。
- 涉及 `ACTIVE.md` 的主线必须和执行守卫保持一致。
- 命中 AOT / IL2CPP / test governance 的主线任务，先走 `project-test-governance`，再进入 plan 或 execute。
- `completed` 前的正式验证对象选择由质量保障层的 `verification-before-completion` 统一负责。

## 最近更新

- `2026-04-04`：加入 roadmap 主线，并将执行上下文说明切换到 `docs/dev`。
- `2026-04-18`：补充 AOT obligation-driven 计划字段与 formal verification gate 的主线映射。
