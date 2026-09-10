---
task_id: 20260413-27-phase-2-solution-core-pack-consolidation
title: Phase 2 SolutionCorePack Consolidation
task_type: plan
lifecycle_status: completed
phase: archived
created_at: 2026-04-13 09:33:43 +08:00
updated_at: 2026-04-13 11:07:49 +08:00
current_dir: docs/dev/completed/20260413-27-phase-2-solution-core-pack-consolidation
parent_task_id: 20260413-24-solution-core-pack-subject-consolidation
source_task_id: 20260413-24-solution-core-pack-subject-consolidation
source_relation: roadmap-child
active: false
---

## 关键文档

- parent_design: `docs/dev/in-progress/20260413-24-solution-core-pack-subject-consolidation/design-v1-01.md`
- parent_roadmap: `docs/dev/in-progress/20260413-24-solution-core-pack-subject-consolidation/roadmap-v1-01.md`
- plan: `docs/dev/completed/20260413-27-phase-2-solution-core-pack-consolidation/plan-v1-01.md`

## 当前判断

- current_focus: Phase 2 已完成；`SolutionCorePack` 已经从 skeleton 进入真实 retained subject，可同时承接 archetype solution slice、mainline proof 入口和 benchmark 入口。
- why_done: 之前 Phase 2 的关键缺口有两个，一是 solution 模式下 `source.entry` 位于非 primary assembly 时的真实执行链路，二是 `windows-native-check` proof 在 native build 阶段缺少完整运行时组装。现在这两条链路都已经打通并经过真实运行验证。
- done_definition: `SolutionCorePack` 已吸收三类 `Solution*` archetype；`MainlineFeaturePack/ProofEntry::Run()` 与 `PerformanceFeaturePack/ArithmeticBenchmarkEntry::RunWorkload()` 均可在同一个 solution-mode subject 下完成真实 proof / benchmark 闭环；相关 planner / registry / execution 回归通过。

## 最近摘要

- 2026-04-13 09:59:03 +08:00: 完成 `Task 1`，落地 `SolutionCorePack` 的 manifest / `.sln` / launcher skeleton，并通过新增的 subject / planner / registry 测试。
- 2026-04-13 11:07:49 +08:00: 完成 `Task 2` 到 `Task 5`，补齐 solution project output 级别的 multi-assembly 传递、C# / native AOT 多程序集入口解析、subject-owned native proof CMake 路由，并重新跑通 `windows-native-check` proof 与 `windows-native-perf` benchmark 的真实执行闭环。

## 下一步

- next_action: 返回父 roadmap，激活下一条子任务，开始 `HotUpdateHostPack` / `MixedExecutionFeaturePack` 的 retained-subject consolidation。
- owner: codex
- trigger: `SolutionCorePack` 已经具备作为 retained core pack 的真实执行基线，后续可以继续推进 Phase 3，而不需要回头补 `.sln` / multi-assembly / proof runtime 这些基础能力。

## 风险 / 阻塞

### risks

- Phase 3 需要避免把 hot-update 与 mixed-execution 重新吸成新的黑盒大包，仍然要坚持 retained subject 内部分 slice / project 的结构。
- `tests/` 与 retained subject smoke 的最终解耦仍然放在 roadmap 尾段，本轮没有提前处理。

### blockers

- 当前无外部 blocker。
