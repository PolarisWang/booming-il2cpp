---
task_id: 20260414-07-phase-2-solution-core-pack-mechanical-split
title: Phase 2 SolutionCorePack Mechanical Split
task_type: plan
lifecycle_status: completed
phase: completed
created_at: 2026-04-14 10:19:44 +08:00
updated_at: 2026-04-14 11:04:15 +08:00
current_dir: docs/dev/completed/20260414-07-phase-2-solution-core-pack-mechanical-split
parent_task_id: 20260414-04-il2cpp-hotupdate-capability-expansion-roadmap
source_task_id: 20260414-04-il2cpp-hotupdate-capability-expansion-roadmap
source_relation: roadmap-child
active: false
---

## 关键文档

- parent_design: `docs/dev/in-progress/20260414-04-il2cpp-hotupdate-capability-expansion-roadmap/design-v1-01.md`
- parent_roadmap: `docs/dev/in-progress/20260414-04-il2cpp-hotupdate-capability-expansion-roadmap/roadmap-v1-01.md`
- phase_0_freeze: `docs/dev/completed/20260414-05-phase-0-capability-taxonomy-and-inventory-freeze/capability-taxonomy-freeze-v1-01.md`
- phase_1_completed: `docs/dev/completed/20260414-06-phase-1-chaos-test-framework-metadata-expansion/STATUS.md`
- plan: `docs/dev/completed/20260414-07-phase-2-solution-core-pack-mechanical-split/plan-v1-01.md`

## 当前判断

- current_focus: Phase 2 已完成；`SolutionCorePack` 已成为 capability taxonomy 下的核心 retained subject，并完成 `FeatureSlices/Benchmarks/Archetypes` 机械拆分、入口对齐与 metadata 落地。
- why_now: `SolutionCorePack` 的真实结构已经稳定，下一步应该把同一套 capability/archetype 口径扩展到 `HotUpdateHostPack`。
- success_definition:
  - `SolutionCorePack` 的 manifest / solution / launcher / slice 结构与当前 taxonomy 对齐
  - proof / benchmark 条目能够声明 capability family / capability item
  - 关键 compatibility / planner / registry / benchmark-source 测试通过

## 最近摘要

- 2026-04-14 10:19:44 +08:00: 创建 Phase 2 child task，准备先跑 `SolutionCorePack` 相关 RED，再收口真实 subject 结构与 metadata。
- 2026-04-14 11:04:15 +08:00: 完成 `SolutionCorePack` 命名去兼容层、launcher/manifest 对齐、compiled catalog 的 solution sibling output 发现修复，并重跑目标回归通过。

## 下一步

- next_action: 返回父 roadmap，创建并执行 `20260414-08-phase-3-hotupdate-host-pack-full-project-workflow`。
- owner: codex
- trigger: Phase 2 目标回归通过，真实 `SolutionCorePack` 结构已经稳定

## 风险 / 阻塞

### risks

- `SolutionCorePack` 目录内已经存在较大范围重构，如果直接覆盖旧路径引用，可能踩到用户尚未完成的并行调整。
- benchmark source 与 planner 仍可能残留旧的 `MainlineFeaturePack` / `PerformanceFeaturePack` 命名耦合，需要边跑边清理。

### blockers

- 当前无外部 blocker。

## wiki

- 本子任务先不更新 `wiki/`。
- 稳定后的 `SolutionCorePack` 结构与 capability 表达方式由父 roadmap Phase 5/6 统一沉淀到 `wiki/03-功能模块/06-il2cpp核心架构/05-架构参考`。
