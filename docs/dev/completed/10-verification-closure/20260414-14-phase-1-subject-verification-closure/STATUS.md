---
task_id: 20260414-14-phase-1-subject-verification-closure
title: Phase 1 Subject Verification Closure
task_type: plan
lifecycle_status: in-progress
phase: completed
created_at: 2026-04-14 13:02:35 +08:00
updated_at: 2026-04-14 13:10:19 +08:00
current_dir: docs/dev/completed/20260414-14-phase-1-subject-verification-closure
parent_task_id: 20260414-12-il2cpp-core-capability-development-roadmap
source_task_id: 20260414-12-il2cpp-core-capability-development-roadmap
source_relation: roadmap-child
active: false
---

## 关键文档

- parent_design: `docs/dev/in-progress/20260414-12-il2cpp-core-capability-development-roadmap/design-v1-01.md`
- parent_roadmap: `docs/dev/in-progress/20260414-12-il2cpp-core-capability-development-roadmap/roadmap-v1-01.md`
- phase_0_status: `docs/dev/completed/20260414-13-phase-0-coverage-ledger-and-subject-verification-freeze/STATUS.md`
- phase_0_checklist: `docs/dev/completed/20260414-13-phase-0-coverage-ledger-and-subject-verification-freeze/subject-verification-checklist-v1-01.md`
- plan: `docs/dev/completed/20260414-14-phase-1-subject-verification-closure/plan-v1-01.md`

## 当前结论

- current_focus: 收口 retained subjects 的 verification / benchmark dashboard / support-state 外层闭环，确保后续 capability batch 可以稳定接入。
- why_now: Phase 0 已冻结 capability ledger、complete verification checklist 与 reference bundle policy；Phase 1 现在可以直接针对真实闭环缺口做最小修复。
- success_definition:
  - retained subjects 新增 proof 或 benchmark 时，不再需要为外层兼容层补历史 alias；
  - benchmark/dashboard 只展示当前声明的 canonical benchmark case，不再混入历史失效 case；
  - support-state、subject summary、case matrix 和文档输出对同一套 declared metadata 保持一致。

## 最近摘要

- 2026-04-14 13:02:35 +08:00: 创建 Phase 1 child task，准备对 retained subject verification closure 做最小闭环修复。
- 2026-04-14 13:02:35 +08:00: 已确认当前主要阻塞点是 benchmark dashboard generator 会把历史 records 中已失效的 benchmark case 混入当前 dashboard 输出。
- 2026-04-14 13:10:19 +08:00: 已通过 TDD 为 dashboard generator 补充“过滤历史失效 case”和“旧 identity 归并到 declared case”两类用例，并完成实现修复。
- 2026-04-14 13:10:19 +08:00: 已刷新 `docs/benchmark` 产物；`SolutionCorePack.json` 不再残留 `PerformanceFeaturePack/*` case，overview 显示 3 个 retained subjects 全部 fully recorded 且 missingModeCount 为 0。

## 下一步

- next_action: 返回父 roadmap，将 `20260414-14` 标记为 completed，并切入 `20260414-15-phase-2-il2cpp-core-semantics-batch-a`。
- owner: codex
- trigger: Phase 1 child task 已激活。

## 风险 / 阻塞

### risks

- 如果直接清理 `records.jsonl` 而不修 generator，旧 case 会在下次数据回流时再次污染 dashboard。
- 如果过滤规则过强，可能误删当前 declared case 但暂时缺记录的占位条目，造成 case coverage 统计不完整。

### blockers

- 当前无外部 blocker。

## wiki

- 本子任务当前只修复 `docs/benchmark` 生成闭环与其测试，不单独更新 `wiki/`。
