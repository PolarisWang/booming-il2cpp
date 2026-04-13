---
task_id: 20260413-28-phase-3-hotupdate-and-mixed-consolidation
title: Phase 3 HotUpdate And Mixed Consolidation
task_type: plan
lifecycle_status: completed
phase: archived
created_at: 2026-04-13 11:18:24 +08:00
updated_at: 2026-04-13 12:14:06 +08:00
current_dir: docs/dev/completed/20260413-28-phase-3-hotupdate-and-mixed-consolidation
parent_task_id: 20260413-24-solution-core-pack-subject-consolidation
source_task_id: 20260413-24-solution-core-pack-subject-consolidation
source_relation: roadmap-child
active: false
---

## 关键文档

- parent_design: `docs/dev/in-progress/20260413-24-solution-core-pack-subject-consolidation/design-v1-01.md`
- parent_roadmap: `docs/dev/in-progress/20260413-24-solution-core-pack-subject-consolidation/roadmap-v1-01.md`
- plan: `docs/dev/completed/20260413-28-phase-3-hotupdate-and-mixed-consolidation/plan-v1-01.md`

## 当前判断

- current_focus: Phase 3 已完成；`HotUpdateHostPack` 与 `MixedExecutionFeaturePack` 都已经从 retained solution skeleton 收口到真实可执行的 retained subject。
- why_done: 本阶段的关键缺口不在“再做一个大而全的 backend”，而在让两个 retained subject 真正跑通 proof / benchmark 闭环，并把之前仅停留在 skeleton 或声明层的能力补成真实运行链路。
- done_definition: `HotUpdateHostPack` 已补齐 `SharedContractProof` 与最小 benchmark 闭环；`MixedExecutionFeaturePack` 已在保持 retained solution 结构的前提下完成 managed / interpreter / native benchmark 闭环；相关 planner / manifest / declared catalog / loader regression 全部通过。

## 最近摘要

- 2026-04-13 11:18:24 +08:00: 激活 Phase 3 子任务，开始盘点 retained hot-update / mixed-execution 的真实缺口，准备先补测试再补 retained pack 能力。
- 2026-04-13 12:14:06 +08:00: 完成 `HotUpdateHostPack` 与 `MixedExecutionFeaturePack` 的真实闭环收口；补齐 loader 对 `switch`、`constrained`、token-based `ldelem/stelem`、`unbox/unbox.any` 的支持，新增 native-only benchmark entry，并跑通两个 retained subject 的 proof / benchmark 验证。

## 下一步

- next_action: 返回父 roadmap，激活 `20260413-29-phase-4-unified-entry-and-registry-cutover`，开始统一 retained subject 的入口模型、registry、selector 与 CLI 默认路径。
- owner: codex
- trigger: `SolutionCorePack`、`HotUpdateHostPack`、`MixedExecutionFeaturePack` 均已具备真实执行证据，可以进入统一入口与外层路由的 Phase 4。

## 风险 / 阻塞

### risks

- Phase 4 之前仍允许 `source.entry` / matrix `workloadEntry` 存在 subject 内部差异，但这些差异现在已经收敛到 retained subject 内部，不再是 legacy root 级别的结构问题。
- Generic native AOT benchmark backend 的通用扩展没有在本阶段展开；`MixedExecutionFeaturePack` 采用了 matrix-level native-only benchmark entry 作为当前收口方式，后续若要泛化需在挂起任务中重新规划。
- `tests/` 目录里残留的 subject-heavy 机制测试仍然留在后续 cleanup phase 处理，本阶段没有提前切入。

### blockers

- 当前无外部 blocker。
