---
task_id: 20260425-05-workflow-progress-and-completion-rules-freeze
title: Workflow Progress And Completion Rules Freeze
task_type: plan
lifecycle_status: completed
phase: phase-1
created_at: 2026-04-25 23:30:00 +08:00
updated_at: 2026-04-25 23:30:00 +08:00
current_dir: docs/dev/in-progress/20260425-05-workflow-progress-and-completion-rules-freeze
parent_task_id: 20260425-03-foundation-dll-capability-closure-reporting-roadmap
parent_task_phase: phase-1
parent_task_relation: child
clearance_source: parent-roadmap
blocking_questions: []
question_clearance: cleared
active: false
---

# 20260425-05 Workflow Progress And Completion Rules Freeze

## 目标

冻结 workflow denominator、DLL completed 规则和 waiver authority。

## 退出条件

- [x] family 级 `requiredGates` 口径固定
- [x] `completion-certification` 被固定为 DLL 级 gate
- [x] waiver / exclusion / platform-blocked 的 authority 和最小字段固定

## 交付物

1. `workflow-gate-contract-v1-01.md` — workflow gate 合约
2. `dll-completed-contract-v1-01.md` — DLL completed 判定规则
3. `waiver-contract-v1-01.md` — waiver / exclusion / platform-blocked 合约

## 依赖

- Phase 0 已完成的 3 个设计文档
- `verification/catalog/programs/foundation-dll-translation-audit.program.json`
- `docs/dev/in-progress/20260419-01-foundation-dll-translation-audit-roadmap/design-v1-01.md` (§10 状态模型)

## 当前状态

- 三个设计文档均已完成草稿（draft 状态）：
  - `workflow-gate-contract-v1-01.md` — 定义 gate 层级（family 级 6 个 + DLL 级 1 个）、两层状态模型（policyState + executionState）、Workflow Progress 计算公式（family 级加权平均）
  - `dll-completed-contract-v1-01.md` — 定义 DLL completed 的 6 条判定规则（family 闭包全覆盖、无 in-progress family、required gates 通过、completion-certification 通过、waiver authority 有效、evidence 存在）、DLL 状态转换图、blocker 机制
  - `waiver-contract-v1-01.md` — 定义三种豁免类型（waiver/exclusion/platform-blocked）的最小字段、authority 要求（不允许 task STATUS.md）、定期限 90 天/续期最多 3 次的约束、角色权限矩阵

## 下一步

- 已完成 Phase 1，进入 Phase 2：`20260425-06-capability-family-ledger-and-report-contract-cutover`
- `next_action`: 开始起草 Phase 2 设计文档

## 验证

- `workflow-gate-contract-v1-01.md` 经用户 review 确认
- `dll-completed-contract-v1-01.md` 经用户 review 确认
- `waiver-contract-v1-01.md` 经用户 review 确认
