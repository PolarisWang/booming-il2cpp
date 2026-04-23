# writing-plans

> 项目中的计划编写器，负责把设计拆成可执行任务，并为执行阶段准备足够上下文。

## 基本信息

- 分层：主线工作流
- 项目职责：输出可执行计划，并冻结 design path、知识沉淀落点、obligation intake 与固定收尾约束
- 实现路径：`.codex/skills/dev-writing-plans/SKILL.md`
- 状态：`active`

## 在本项目中的作用

- 让执行阶段能稳定维护任务目录中的 `STATUS.md`、`ACTIVE.md` 与进度文档
- 让每个任务的知识沉淀目标尽量明确
- 让任务总数可被执行阶段稳定确认
- 让任务在完成后进入“审视架构合理性 -> 测试通过 -> 归档 completed -> 合并&提交”固定收尾链路
- 如果计划对应 roadmap-child 或其它独立子任务，必须在计划里写明：终态时要物理移动目录并回写父任务，不能只在正文里写 `completed`

## AOT / IL2CPP / Test Governance Intake

命中测试治理主线的计划，必须显式冻结：

- `capabilityFamily`
- `capabilityItem`
- `ownerSubjectId`
- `proofRequired`
- `benchmarkRequired`
- `hotupdateImpact`
- `formalVerificationObjects`
- `requiredGates`

约束：

- 非适用字段也必须显式写值，不能省略
- `formalVerificationObjects` 必须写正式对象或 declared entry，而不是“后续跑测试”
- `requiredGates` 默认至少覆盖 `collector -> registry -> workspace`

## 最近变更

- `2026-04-18`：新增 AOT / IL2CPP / test governance intake 字段，要求 plan 先冻结 obligation，再进入执行。
- `2026-04-22`：补充 roadmap-child 终态约束，要求计划显式声明物理归档与父任务回写。
