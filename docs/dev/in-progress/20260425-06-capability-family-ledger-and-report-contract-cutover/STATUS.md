---
task_id: 20260425-06-capability-family-ledger-and-report-contract-cutover
title: Capability-Family Ledger And Projection Contract Cutover
task_type: plan
lifecycle_status: completed
phase: phase-2
created_at: 2026-04-26 00:00:00 +08:00
updated_at: 2026-04-26 00:00:00 +08:00
current_dir: docs/dev/in-progress/20260425-06-capability-family-ledger-and-report-contract-cutover
parent_task_id: 20260425-03-foundation-dll-capability-closure-reporting-roadmap
parent_task_phase: phase-2
parent_task_relation: child
clearance_source: parent-roadmap
blocking_questions: []
question_clearance: cleared
active: false
---

# 20260425-06 Capability-Family Ledger And Projection Contract Cutover

## 目标

把新的 capability-family 数据模型接入 formal projection / docs projection contract。

## 退出条件

- [x] program / DLL / capability family / source refs / evidence / waivers 的正式字段固定
- [x] projection contract 与 report consumer 一致
- [x] `program.json` 不再出现 dashboard 需要但 payload 不提供的空字段

## 交付物

1. `projection-schema-v1-01.md` — updated projection schema
2. `generator-contract-v1-01.md` — updated generator contract
3. `projection-payload-examples-v1-01.md` — updated JSON payload examples

## 依赖

- Phase 0 完成的 3 个文档（ledger schema、authority contract、granularity examples）
- Phase 1 完成的 3 个文档（workflow gate、DLL completed、waiver）
- `verification/catalog/programs/foundation-dll-translation-audit.program.json`
- `docs/dev/in-progress/20260419-01-foundation-dll-translation-audit-roadmap/design-v1-01.md`

## 当前状态

- 三个设计文档均已完成草稿（draft 状态）：
  - `projection-schema-v1-01.md` — 三级 projection schema（program/DLL/family），CapabilityClosure / WorkflowProgress 字段定义，schemaVersion 从 1→2 升级路径，向后兼容策略
  - `generator-contract-v1-01.md` — 输入合约（新增 capability-family-ledger.json 核心输入）、处理合约（数据流、核心步骤、状态推导逻辑）、输出合约（文件清单、位置、约束）、ledger 不存在时的降级行为
  - `projection-payload-examples-v1-01.md` — program.json / dll-matrix.json / dlls/<assembly>.json 完整 payload 示例，覆盖 CoreLib 已闭包 family、未开始 DLL 和 high-risk DLL 三种状态

## 下一步

- 已完成 Phase 2，进入 Phase 3：`20260425-07-dll-report-pages-source-links-and-artifact-split`
- `next_action`: 开始起草 Phase 3 设计文档

## 验证

- `projection-schema-v1-01.md` 经用户 review 确认
- `generator-contract-v1-01.md` 经用户 review 确认
- `projection-payload-examples-v1-01.md` 经用户 review 确认
