---
task_id: 20260414-08-phase-3-hotupdate-host-pack-full-project-workflow
title: Phase 3 HotUpdateHostPack Full-Project Workflow
task_type: plan
lifecycle_status: completed
phase: completed
created_at: 2026-04-14 11:04:15 +08:00
updated_at: 2026-04-14 11:16:39 +08:00
current_dir: docs/dev/completed/20260414-08-phase-3-hotupdate-host-pack-full-project-workflow
parent_task_id: 20260414-04-il2cpp-hotupdate-capability-expansion-roadmap
source_task_id: 20260414-04-il2cpp-hotupdate-capability-expansion-roadmap
source_relation: roadmap-child
active: false
---

## 关键文档

- parent_design: `docs/dev/in-progress/20260414-04-il2cpp-hotupdate-capability-expansion-roadmap/design-v1-01.md`
- parent_roadmap: `docs/dev/in-progress/20260414-04-il2cpp-hotupdate-capability-expansion-roadmap/roadmap-v1-01.md`
- phase_1_completed: `docs/dev/completed/20260414-06-phase-1-chaos-test-framework-metadata-expansion/STATUS.md`
- phase_2_completed: `docs/dev/completed/20260414-07-phase-2-solution-core-pack-mechanical-split/STATUS.md`
- plan: `docs/dev/completed/20260414-08-phase-3-hotupdate-host-pack-full-project-workflow/plan-v1-01.md`

## 当前判断

- current_focus: Phase 3 已完成；`HotUpdateHostPack` 已收口为完整热更 workflow 的 canonical retained subject，并补齐 `FullProjectHotUpdateSolution`、proof/benchmark capability metadata 与 metadata supplement proof。
- why_now: 热更主链路的 capability/archetype 口径已经稳定，下一步应把同一套方法应用到 `MixedExecutionFeaturePack`。
- success_definition:
  - `HotUpdateHostPack` proof / benchmark 条目都带上 capability family / capability item / archetype / hot-update capability
  - `FullProjectHotUpdateSolution` 与 subject manifest / solution / host workflow 一致
  - 关键 declared metadata / compatibility / perf source 回归通过

## 最近摘要

- 2026-04-14 11:04:15 +08:00: 创建 Phase 3 child task，准备先对 HotUpdate declared metadata discovery 做 RED，再补 proof/benchmark metadata 与缺失条目。
- 2026-04-14 11:16:39 +08:00: 完成 HotUpdateHostPack capability metadata 补齐，新增 `MetadataSupplementProofEntry`，并通过 declared metadata / hot-update compatibility / perf source 回归。

## 下一步

- next_action: 返回父 roadmap，创建并执行 `20260414-09-phase-4-mixed-execution-capability-closure`。
- owner: codex
- trigger: Phase 3 目标回归通过，热更 workflow 与 metadata 口径已稳定

## 风险 / 阻塞

### risks

- `HotUpdateHostPack` 当前虽然已有 `Host/Proofs`、`Host/Benchmarks` 与 `FullProjectHotUpdateSolution` 结构，但 declared metadata 仍可能停留在旧的 hot-update-only 口径，导致父 roadmap 后续 dashboard / docs 失真。
- `MetadataSupplement` 在 taxonomy 中是明确能力项，如果本阶段没有显式 proof 或 benchmark 条目承载它，后续 capability coverage 会留下空洞。

### blockers

- 当前无外部 blocker。

## wiki

- 本子任务先不更新 `wiki/`。
- 稳定后的热更 canonical workflow 由父 roadmap Phase 5/6 统一沉淀到 `wiki/03-功能模块/06-il2cpp核心架构/05-架构参考`。
