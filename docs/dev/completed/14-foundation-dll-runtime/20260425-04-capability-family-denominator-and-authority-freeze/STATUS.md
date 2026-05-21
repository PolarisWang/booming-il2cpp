---
task_id: 20260425-04-capability-family-denominator-and-authority-freeze
title: Capability Family Denominator And Authority Freeze
task_type: plan
lifecycle_status: completed
phase: phase-0
created_at: 2026-04-25 23:00:00 +08:00
updated_at: 2026-04-27 02:20:00 +08:00
current_dir: docs/dev/completed/20260425-04-capability-family-denominator-and-authority-freeze
parent_task_id: 20260425-03-foundation-dll-capability-closure-reporting-roadmap
parent_task_phase: phase-0
parent_task_relation: child
clearance_source: parent-roadmap
blocking_questions: []
question_clearance: cleared
active: false
---

# 20260425-04 Capability Family Denominator And Authority Freeze

## 目标

冻结 foundation DLL capability closure 的正式 denominator authority 和 family 粒度。

## 退出条件

- [x] `capability-family ledger` 的 authority 关系被文档化
- [x] capability family 的主粒度固定为语义 family
- [x] implementation family 和 source grouping 的从属关系固定

## 交付物

1. `capability-family-ledger-schema-v1-01.md` — ledger schema / design 文档
2. `denominator-authority-contract-v1-01.md` — denominator authority contract
3. `family-granularity-examples-v1-01.md` — family 粒度示例

## 依赖

- `verification/catalog/programs/foundation-dll-translation-audit.program.json`
- 当前 audit input / surface / semantic / nativeization artifacts
- `docs/dev/in-progress/20260419-01-foundation-dll-translation-audit-roadmap/roadmap-v1-01.md`
- `docs/dev/in-progress/20260419-01-foundation-dll-translation-audit-roadmap/design-v1-01.md`

## 当前状态

- 三个设计文档均已完成草稿（draft 状态）：
  - `capability-family-ledger-schema-v1-01.md` — 定义 ledger schema、family 层次结构、authority 分层模型、family ID 命名规范、闭包判定规则
  - `denominator-authority-contract-v1-01.md` — 定义分母 = capability family 数，原始 authority（4 个 artifact）vs 报告 authority（ledger 快照），更新流程、冲突裁决规则、authority 生命周期
  - `family-granularity-examples-v1-01.md` — 5 个示例（Convert.ToChar、Convert.ToString、Buffer、Enum 拆分、System.Text.Json 拆分），4 个负面示例，粒度检查清单

## 下一步

- 已完成 Phase 0，进入 Phase 1：`20260425-05-workflow-progress-and-completion-rules-freeze`
- `next_action`: 开始起草 Phase 1 设计文档

## 验证

- `capability-family-ledger-schema-v1-01.md` 经用户 review 确认
- `denominator-authority-contract-v1-01.md` 经用户 review 确认
- `family-granularity-examples-v1-01.md` 经用户 review 确认
