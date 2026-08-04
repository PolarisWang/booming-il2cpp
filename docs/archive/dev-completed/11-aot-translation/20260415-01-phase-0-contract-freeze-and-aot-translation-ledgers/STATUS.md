---
task_id: 20260415-01-phase-0-contract-freeze-and-aot-translation-ledgers
title: Phase 0 Contract Freeze And AOT Translation Ledgers
task_type: plan
lifecycle_status: completed
phase: completed
created_at: 2026-04-15 00:11:06 +08:00
updated_at: 2026-04-15 00:29:10 +08:00
current_dir: docs/dev/completed/20260415-01-phase-0-contract-freeze-and-aot-translation-ledgers
parent_task_id: 20260414-37-hybridclr-aligned-aot-hotupdate-development-plan
source_task_id: 20260414-37-hybridclr-aligned-aot-hotupdate-development-plan
source_relation: roadmap-child
active: false
---

## 关键文档

- parent_status: `docs/dev/in-progress/20260414-37-hybridclr-aligned-aot-hotupdate-development-plan/STATUS.md`
- parent_roadmap: `docs/dev/in-progress/20260414-37-hybridclr-aligned-aot-hotupdate-development-plan/roadmap-v1-01.md`
- parent_review: `docs/dev/in-progress/20260414-37-hybridclr-aligned-aot-hotupdate-development-plan/review-v1-01.md`
- capability_matrix: `wiki/03-功能模块/06-il2cpp核心架构/05-架构参考/il2cpp-hotupdate-capability-matrix.md`
- reference_policy: `wiki/03-功能模块/06-il2cpp核心架构/05-架构参考/reference-assembly-and-corelib-policy.md`
- archetype_matrix: `wiki/03-功能模块/06-il2cpp核心架构/05-架构参考/solution-archetype-matrix.md`
- unsupported_policy: `wiki/03-功能模块/06-il2cpp核心架构/05-架构参考/unsupported-feature-policy.md`
- plan: `docs/dev/completed/20260415-01-phase-0-contract-freeze-and-aot-translation-ledgers/plan-v1-01.md`
- reference_bundle_freeze: `docs/dev/completed/20260415-01-phase-0-contract-freeze-and-aot-translation-ledgers/reference-bundle-freeze-v1-01.md`
- capability_taxonomy_freeze: `docs/dev/completed/20260415-01-phase-0-contract-freeze-and-aot-translation-ledgers/capability-taxonomy-freeze-v1-01.md`
- stable_identity_freeze: `docs/dev/completed/20260415-01-phase-0-contract-freeze-and-aot-translation-ledgers/stable-identity-and-body-availability-v1-01.md`
- translation_surface_ledger: `docs/dev/completed/20260415-01-phase-0-contract-freeze-and-aot-translation-ledgers/aot-translation-surface-ledger-v1-01.md`
- unsupported_surface_ledger: `docs/dev/completed/20260415-01-phase-0-contract-freeze-and-aot-translation-ledgers/aot-unsupported-surface-ledger-v1-01.md`

## 当前判断

- current_focus: 把母 roadmap 已冻结的 Q0-Q8 变成可执行的 Phase 0 台账，避免后续 Phase 1-6 继续边做边改契约。
- why_now: 当前仓库已经有 retained subjects、reference bundle、`Chaos.TestFramework` 枚举和 attribute 基线，适合先把“真实存在的事实”冻结成统一 ledger。
- success_definition:
  - `reference bundle`、两级 `capability taxonomy`、stable numeric identity 和 `body availability` 口径冻结。
  - `AOT translation surface` 以统一维度映射到 owner subject / archetype / proof / benchmark。
  - `partial` / `unsupported` / deferred surface 进入单独台账，并带稳定 reason code。

## 最近摘要

- 2026-04-15 00:11:06 +08:00: 创建 Phase 0 child task，开始整理 reference bundle、capability taxonomy、stable identity 与 AOT 双台账。
- 2026-04-15 00:11:06 +08:00: 已复核当前 retained subjects、`assets/reference-bundles/dotnet-foundation`、`src/reference/Chaos.TestFramework` 与既有 architecture 文档，确认本阶段有真实输入可冻结。
- 2026-04-15 00:29:10 +08:00: 已完成 5 份 freeze 文档，并完成一次路径、owner、reason code 与 bundle 边界自检；本子任务收口完成，返回父 roadmap 切入 Phase 1。

## 下一步

- next_action: 返回父 roadmap，创建并执行 `Phase 1: Subject Source Reshape And Test Metadata Cutover` child task。
- owner: codex
- trigger: Phase 0 已完成，无需额外触发条件。

## 风险 / 阻塞

### risks

- 如果 Phase 0 不把 capability family/item 与 owner subject 的关系冻结，Phase 1 继续拆 slice 时仍会出现“有文件、无 owner”或“有 owner、无 stable id”的漂移。
- 如果 `body availability` 继续以字符串散落在 contracts / manifests / reporting 中，Phase 2-4 会很难把 AOT、interpreter、bridge/thunk 对齐到同一条 dispatch 线。
- 如果 unsupported surface 只写在讨论里、不写进台账，Phase 5-6 的 benchmark、dashboard 和 release gate 仍会出现“看起来支持、实际上缺证据”的假闭环。

### blockers

- 当前无外部 blocker。

## wiki

- 本子任务先沉淀在 `docs/dev`；如 Phase 2-6 继续沿用这些口径，再择机同步回 `wiki/03-功能模块/06-il2cpp核心架构/05-架构参考` 或 `wiki/`。
