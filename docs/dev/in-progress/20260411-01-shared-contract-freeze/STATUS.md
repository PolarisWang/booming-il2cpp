---
task_id: 20260411-01-shared-contract-freeze
title: Shared Contract Freeze - 冻结 AOT 与热更共享 contract
task_type: plan
lifecycle_status: in_progress
phase: planning
created_at: 2026-04-11 14:00:00 +08:00
updated_at: 2026-04-11 14:00:00 +08:00
current_dir: docs/dev/in-progress/20260411-01-shared-contract-freeze
parent_task_id: 20260409-10-total-solution-and-ios-hot-update-analysis
source_task_id: 20260409-10-total-solution-and-ios-hot-update-analysis
source_relation: child_of_roadmap_phase_0
active: true
---

## 关键文档

- parent_roadmap: `docs/dev/in-progress/20260409-10-total-solution-and-ios-hot-update-analysis/roadmap-v1-01.md`
- phase_architecture: `docs/dev/in-progress/20260409-10-total-solution-and-ios-hot-update-analysis/phase-architecture-v1-01.md`
- plan: `docs/dev/in-progress/20260411-01-shared-contract-freeze/plan-v1-01.md`

## 当前判断

- current_focus: 编写并执行 Phase 0 计划，冻结 AOT 与热更共享 contract。
- why_now: 所有后续 Phase 都依赖统一的 identity/ABI/handle 模型。
- done_definition: 11 份 contract spec 文档 + schema + snapshot tests + 双边 mock 验证全部通过。

## 下一步

- next_action: 编写 plan-v1-01.md。
- owner: codex

## 风险 / 阻塞

### risks

- identity 格式如果不够灵活，后续 generic instantiation 会遇到编码问题。

### blockers

- 无。
