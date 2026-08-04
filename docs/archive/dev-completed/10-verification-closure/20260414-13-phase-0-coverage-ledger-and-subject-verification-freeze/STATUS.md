---
task_id: 20260414-13-phase-0-coverage-ledger-and-subject-verification-freeze
title: Phase 0 Coverage Ledger And Subject Verification Freeze
task_type: plan
lifecycle_status: completed
phase: completed
created_at: 2026-04-14 12:52:36 +08:00
updated_at: 2026-04-14 12:56:46 +08:00
current_dir: docs/dev/completed/20260414-13-phase-0-coverage-ledger-and-subject-verification-freeze
parent_task_id: 20260414-12-il2cpp-core-capability-development-roadmap
source_task_id: 20260414-12-il2cpp-core-capability-development-roadmap
source_relation: roadmap-child
active: false
---

## 关键文档

- parent_design: `docs/dev/in-progress/20260414-12-il2cpp-core-capability-development-roadmap/design-v1-01.md`
- parent_roadmap: `docs/dev/in-progress/20260414-12-il2cpp-core-capability-development-roadmap/roadmap-v1-01.md`
- capability_matrix: `wiki/03-功能模块/06-il2cpp核心架构/05-架构参考/il2cpp-hotupdate-capability-matrix.md`
- reference_policy: `wiki/03-功能模块/06-il2cpp核心架构/05-架构参考/reference-assembly-and-corelib-policy.md`
- plan: `docs/dev/completed/20260414-13-phase-0-coverage-ledger-and-subject-verification-freeze/plan-v1-01.md`
- coverage_ledger_freeze: `docs/dev/completed/20260414-13-phase-0-coverage-ledger-and-subject-verification-freeze/coverage-ledger-freeze-v1-01.md`
- subject_verification_checklist: `docs/dev/completed/20260414-13-phase-0-coverage-ledger-and-subject-verification-freeze/subject-verification-checklist-v1-01.md`
- reference_bundle_policy_freeze: `docs/dev/completed/20260414-13-phase-0-coverage-ledger-and-subject-verification-freeze/reference-bundle-policy-freeze-v1-01.md`
- phase_0_decisions_freeze: `docs/dev/completed/20260414-13-phase-0-coverage-ledger-and-subject-verification-freeze/phase-0-decisions-freeze-v1-01.md`

## 当前判断

- current_focus: Phase 0 已完成，capability owner 台账、完整验证 checklist 与 reference bundle 口径已经冻结，可作为后续所有 child task 的硬边界。
- why_now: Phase 1 以及之后的能力补齐不再需要重复讨论 owner、bundle 与闭环标准。
- success_definition:
  - coverage ledger 能覆盖母 roadmap 中所有待补 capability batch。
  - retained subject 的“完整验证能力”有统一 checklist。
  - reference/corelib/patch 的 repository-owned bundle 边界、刷新规则与 archetype 消费方式固定。

## 最近摘要

- 2026-04-14 12:52:36 +08:00: 创建 Phase 0 child task，开始产出 coverage ledger、verification checklist、reference bundle 与 decisions freeze 文档。
- 2026-04-14 12:56:46 +08:00: 完成 Phase 0 自检；确认 capability item、owner archetype、reference bundle 路径与现有 architecture docs / retained subject 真实结构一致。

## 下一步

- next_action: 返回父 roadmap，创建并执行 `20260414-14-phase-1-subject-verification-closure`。
- owner: codex
- trigger: Phase 0 freeze 文档完成并通过自检。

## 风险 / 阻塞

### risks

- 如果 capability item 命名与现有 architecture matrix 不一致，后续 child task 会再次出现重复命名和错误 owner。
- 如果“完整验证能力”定义不够硬，Phase 1 之后仍会出现 proof 有了但 dashboard/reporting/docs 不闭环的情况。

### blockers

- 当前无外部 blocker。

## wiki

- 本子任务只冻结 `docs/dev` 与 `wiki/03-功能模块/06-il2cpp核心架构/05-架构参考` 的衔接边界，暂不改 `wiki/`。
