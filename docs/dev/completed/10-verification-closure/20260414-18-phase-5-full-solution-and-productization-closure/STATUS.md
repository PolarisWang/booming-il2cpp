---
task_id: 20260414-18-phase-5-full-solution-and-productization-closure
title: Phase 5 Full-Solution And Productization Closure
task_type: plan
lifecycle_status: completed
phase: completed
created_at: 2026-04-14 14:07:36 +08:00
updated_at: 2026-04-14 14:07:36 +08:00
current_dir: docs/dev/completed/20260414-18-phase-5-full-solution-and-productization-closure
parent_task_id: 20260414-12-il2cpp-core-capability-development-roadmap
source_task_id: 20260414-12-il2cpp-core-capability-development-roadmap
source_relation: roadmap-child
active: false
---

## 关键文档

- parent_design: `docs/dev/completed/20260414-12-il2cpp-core-capability-development-roadmap/design-v1-01.md`
- parent_roadmap: `docs/dev/completed/20260414-12-il2cpp-core-capability-development-roadmap/roadmap-v1-01.md`
- phase_4_status: `docs/dev/completed/20260414-17-phase-4-metadata-and-aot-closure-batch-c/STATUS.md`

## 当前结论

- current_focus: 验证 `SolutionCorePack` 与 `HotUpdateHostPack` 的 canonical solution archetype 已经形成稳定的工程级 gate，而不是停留在单文件 proof。
- why_now: Phase 2-4 已把 capability owner 和 declared catalog 收口；Phase 5 负责确认完整 solution graph、本地 reference bundle、launcher generic entry 和 archetype matrix 这些工程级闭环全部成立。
- success_definition:
  - `SimpleLibrarySolution`、`MultiProjectSolution`、`PackageReferenceSolution`、`ReferenceAssemblySolution`、`CoreLibReferenceSolution`、`MixedReferenceClosureSolution` 全部稳定保留在 `SolutionCorePack`。
  - `FullProjectHotUpdateSolution` 保留在 `HotUpdateHostPack`，并通过 archetype matrix 与 perf matrix 接入统一入口。
  - launcher generic source-entry 选择与 subject manifest schema 保持稳定。

## 最近摘要

- 2026-04-14 14:07:36 +08:00: 复核 `subjects/SolutionCorePack/source/Archetypes/**` 与 `subjects/HotUpdateHostPack/source/Archetypes/FullProjectHotUpdateSolution/**`，确认工程级样例仍是 retained subject 的核心证据载体。
- 2026-04-14 14:07:36 +08:00: 通过 `tests/unit/compatibility/test_solution_core_pack_subject.py`、`tests/unit/compatibility/test_mainline_feature_pack_onboarding.py`、`tests/unit/compatibility/test_subject_manifest_schema.py` 与 `tests/tooling/run/test_subject_command.py`，确认 full-solution manifest、launcher、pipeline、profile、schema 没有回退。
- 2026-04-14 14:07:36 +08:00: Phase 5 未新增新 subject；完整 solution 证据继续集中在 `SolutionCorePack` / `HotUpdateHostPack` 的 archetype 子树下。

## 下一步

- next_action: 返回父 roadmap，切入 `20260414-19-phase-6-mixed-execution-hotupdate-and-release-gates`。
- owner: codex
- trigger: Phase 5 已完成。

## 风险 / 阻塞

### risks

- 若后续再把 solution archetype 拆成新的顶层 subject，会破坏当前“新增测试目标只在 retained subject 内扩 capability / archetype 证据”的主设计。

### blockers

- 当前无外部 blocker。

## wiki

- 本阶段继续不单独更新 `wiki/`。
