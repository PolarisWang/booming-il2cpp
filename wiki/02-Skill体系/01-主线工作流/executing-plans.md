# executing-plans

> 项目中的顺序执行主线，负责执行计划、维护 `STATUS.md`、`ACTIVE.md`、索引和目录终态，并触发项目知识沉淀。

## 基本信息

- 分层：主线工作流
- 项目职责：执行计划、维护任务目录状态，并在完成时按“架构审视 -> formal verification + 测试 -> completed 归档 -> 合并&提交”固定顺序收尾
- 实现路径：`.codex/skills/dev-executing-plans/SKILL.md`
- 状态：`active`

## 在本项目中的作用

- 开始时确认任务总数
- 创建或更新当前任务目录中的 `STATUS.md`
- 创建或更新 `docs/dev/ACTIVE.md`
- 每任务后更新 `STATUS.md`、`ACTIVE.md`、`notes/progress-*.md` 与索引
- 有长期知识时调用 `project-wiki-maintenance`
- obligation-driven 任务在 `completed` 前必须先消费 [`verification-before-completion`](../04-%E8%B4%A8%E9%87%8F%E4%BF%9D%E9%9A%9C/verification-before-completion.md)
- 需要新的验证通过记录 / 新的数据时，在 formal object 与项目测试通过后继续执行 `run test inventory --json`，刷新 `verification-v1` 正式产物

## 项目规则

- 用户不能手动选择 `completed`
- `completed` 只能在“任务全完成 + formal verification gate 通过 + 架构合理性审视通过 + 受影响测试通过 + wiki 更新完成”后自动产生
- 命中 `ownerSubjectId`、`proofRequired`、`benchmarkRequired`、`hotupdateImpact`、`formalVerificationObjects`、`requiredGates` 等字段时，视为 obligation-driven
- obligation-driven 任务至少要确认 `requiredGates`、managed proof、native proof、按需 hotupdate proof / benchmark、以及全部 `formalVerificationObjects`
- 如本轮要求新的 verification 数据，`completed` 前还必须确认 `docs/testing-inventory/verification/{latest,master,reports}` 已刷新；命中 codegen 主线时再确认 `subjects/*/verification/codegen-stubs/*`
- 测试阶段如果出现 `dotnet` 编译崩溃，必须先查根因并修复，不能绕过
- `docs/dev/in-progress/` 只允许承载 `lifecycle_status: in-progress` 的任务；终态任务必须同步更新 `STATUS.md`、`current_dir`、索引和物理目录
- 带有 `parent_task_id` 或 `source_relation: roadmap-child*` 的 child task 完成后，必须先归档 child task，再回写父任务摘要与 child mapping

## 最近变更

- `2026-04-18`：把 AOT obligation-driven formal verification gate 接入 `completed` 判定。
- `2026-04-22`：补充 `in-progress` 目录一致性与 roadmap-child 归档规则，禁止“正文已完成但目录仍留在 in-progress”。
- `2026-04-23`：补充 verification-v1 数据刷新要求；需要新验证记录时，formal object 与项目测试通过后还必须刷新 `latest/master/reports` 正式产物。
