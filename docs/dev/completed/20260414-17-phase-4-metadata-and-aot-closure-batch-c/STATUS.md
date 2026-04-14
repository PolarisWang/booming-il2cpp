---
task_id: 20260414-17-phase-4-metadata-and-aot-closure-batch-c
title: Phase 4 Metadata And AOT Closure Batch C
task_type: plan
lifecycle_status: completed
phase: completed
created_at: 2026-04-14 14:07:36 +08:00
updated_at: 2026-04-14 14:07:36 +08:00
current_dir: docs/dev/completed/20260414-17-phase-4-metadata-and-aot-closure-batch-c
parent_task_id: 20260414-12-il2cpp-core-capability-development-roadmap
source_task_id: 20260414-12-il2cpp-core-capability-development-roadmap
source_relation: roadmap-child
active: false
---

## 关键文档

- parent_design: `docs/dev/completed/20260414-12-il2cpp-core-capability-development-roadmap/design-v1-01.md`
- parent_roadmap: `docs/dev/completed/20260414-12-il2cpp-core-capability-development-roadmap/roadmap-v1-01.md`
- phase_0_coverage_ledger: `docs/dev/completed/20260414-13-phase-0-coverage-ledger-and-subject-verification-freeze/coverage-ledger-freeze-v1-01.md`
- phase_3_status: `docs/dev/completed/20260414-16-phase-3-low-level-and-interop-batch-b/STATUS.md`

## 当前结论

- current_focus: 关闭 metadata / linker / generic closure 的主体缺口，同时保持 archetype 级 AOT 证据不被错误地下沉成新的 declared proof。
- why_now: Phase 3 已建立细粒度 interop / low-level 切片，Phase 4 可以在同一个 `SolutionCorePack` retained subject 内补齐 metadata / AOT owner。
- success_definition:
  - `CustomAttributeLookup`、`MemberMetadataLookup`、`LinkerPreserveContract`、`RequiredInstantiationClosure`、`GenericSharingBoundary` 全部进入 declared proof / benchmark。
  - 既有 `MetadataClosure` 与 `LinkerStripping` proof 保持稳定。
  - `AotClosureValidation` 继续以 `CoreLibReferenceSolution` archetype 级证据承接，保持与 coverage ledger 的 proof shape 一致。

## 最近摘要

- 2026-04-14 14:07:36 +08:00: 在 `ReflectionAndMetadata` 下新增 `CustomAttributeLookupProofEntry`、`MemberMetadataLookupProofEntry`、`LinkerPreserveContractProofEntry`，在 `GenericsAndCollections` 下新增 `RequiredInstantiationClosureProofEntry`、`GenericSharingBoundaryProofEntry`。
- 2026-04-14 14:07:36 +08:00: 新增 `MemberMetadataLookupBenchmarkEntry`、`RequiredInstantiationClosureBenchmarkEntry`、`GenericSharingBoundaryBenchmarkEntry`，并通过 compiled catalog 锁定 alias / family / item / requires / archetype。
- 2026-04-14 14:07:36 +08:00: 明确保留 `AotClosureValidation -> CoreLibReferenceSolution` 的 archetype 级证据，不额外制造新的 subject/proof 包装层。

## 下一步

- next_action: 返回父 roadmap，切入 `20260414-18-phase-5-full-solution-and-productization-closure`。
- owner: codex
- trigger: Phase 4 已完成。

## 风险 / 阻塞

### risks

- 若把 `AotClosureValidation` 机械地下沉为新的 declared proof，会重新引入“capability 名称存在，但证据 shape 与实际 archetype 不一致”的伪闭环。

### blockers

- 当前无外部 blocker。

## wiki

- 本阶段继续仅在 `docs/dev` 沉淀执行痕迹。
