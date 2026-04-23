# subagent-driven-development

> 项目中的子 agent 执行主线，负责在并行子上下文里完成任务，并同步维护任务目录状态。

## 基本信息

- 分层：主线工作流
- 项目职责：在子 agent 工作流下执行计划，同时维护 `STATUS.md`、`ACTIVE.md`、索引与 wiki 更新，并走统一收尾链路
- 实现路径：`.codex/skills/dev-subagent-driven-development/SKILL.md`
- 状态：`active`

## 在本项目中的作用

- 开始时确认任务总数
- 创建或更新当前任务目录中的 `STATUS.md`
- 创建或更新 `docs/dev/ACTIVE.md`
- 每任务后更新 `STATUS.md`、`ACTIVE.md`、索引与进度文档
- 对长期知识调用 `project-wiki-maintenance`
- completion 前与顺序执行主线使用同一套 formal verification gate
- 需要新的验证通过记录 / 新的数据时，在 formal object 与项目测试通过后继续执行 `run verify verification-v1 --json`，刷新 `verification-v1` 正式产物

## 项目规则

- 子 agent 负责任务实现与审查，不负责绕过活动任务守卫
- 规格合规性审查必须先于代码质量审查
- 命中 obligation 字段时，必须先消费 `verification-before-completion`
- `completed` 前至少确认 `requiredGates`、managed proof、native proof、按需 hotupdate proof / benchmark、以及全部 `formalVerificationObjects`
- 如本轮要求新的 verification 数据，`completed` 前还必须确认 `docs/testing-inventory/verification/{latest,master,reports}` 已刷新；命中 codegen 主线时再确认 `subjects/*/verification/codegen-stubs/*`
- 测试阶段如果出现 `dotnet` 编译崩溃，必须先查根因并修复
- 开始新的复杂任务、切换任务或恢复任务前，必须先执行与顺序主线等价的 `ACTIVE.md` 守卫
- `docs/dev/in-progress/` 只允许承载 `lifecycle_status: in-progress` 的任务；终态任务必须同步更新 `STATUS.md`、`current_dir`、索引和物理目录
- 带有 `parent_task_id` 或 `source_relation: roadmap-child*` 的 child task 完成后，必须先归档 child task，再回写父任务摘要与 child mapping

## 最近变更

- `2026-04-18`：与顺序执行主线对齐，统一接入 obligation-driven formal verification gate。
- `2026-04-22`：补齐与 `executing-plans` 等价的 `ACTIVE.md` 守卫和 roadmap-child 归档规则。
- `2026-04-23`：与顺序执行主线保持一致，新增 verification-v1 数据刷新要求。
