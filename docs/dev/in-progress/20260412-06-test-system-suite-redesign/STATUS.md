---
task_id: 20260412-06-test-system-suite-redesign
title: 测试体系 Solution-First Subject 重设计与能力覆盖模型
task_type: roadmap
lifecycle_status: in_progress
phase: roadmap
created_at: 2026-04-12 15:22:24 +08:00
updated_at: 2026-04-12 17:30:00 +08:00
current_dir: docs/dev/in-progress/20260412-06-test-system-suite-redesign
parent_task_id:
source_task_id: 20260412-05-phase-b-generic-native-aot-benchmark-backend
source_relation: paused-current-task-for-broader-redesign
active: true
---

## 关键文档

- brainstorm: `docs/dev/in-progress/20260412-06-test-system-suite-redesign/brainstorm-v1-01.md`
- design: `docs/dev/in-progress/20260412-06-test-system-suite-redesign/design-v1-03.md`
- roadmap_or_plan: `docs/dev/in-progress/20260412-06-test-system-suite-redesign/roadmap-v1-03.md`
- superseded_design: `docs/dev/in-progress/20260412-06-test-system-suite-redesign/design-v1-02.md`
- superseded_roadmap: `docs/dev/in-progress/20260412-06-test-system-suite-redesign/roadmap-v1-02.md`

## 当前判断

- current_focus: v1.03 已切换到 `solution-first + engineering-core + attribute-declared hybrid` 基线。
- why_now: 用户已经确认 `ChaosUnitTest` / `ChaosBenchmark` 命名、少 string 多枚举、工程级验证优先。
- done_definition: 设计与 roadmap 已升级到 v1.03；`scenario/workload` 已从默认物理目录降为逻辑分类；subject manifest 已被定义为薄 orchestration manifest。

## 最近摘要

- 2026-04-12 15:22:24 +08:00: 用户暂停 `20260412-05-phase-b-generic-native-aot-benchmark-backend`，转向更高层的测试体系重设计。
- 2026-04-12 16:10:00 +08:00: 已产出 v1.02，完成 solution-first 顶层 `subject` 基线替换。
- 2026-04-12 17:30:00 +08:00: 已产出 `design-v1-03.md` 与 `roadmap-v1-03.md`，纳入 `ChaosUnitTest` / `ChaosBenchmark`、紧凑枚举、工程级验证优先三条新增约束。

## 下一步

- next_action: 创建 `phase-0-hybrid-model-freeze-and-migration-inventory` child plan。
- trigger: 用户确认以 v1.03 作为执行基线，或直接要求开始 Phase 0。

## 风险 / 阻塞

### risks

- 如果把 convert/codegen/native executable 误归到 `ChaosBenchmark`，工程级主链会再次失真。
- 如果 per-subject manifest 继续内联大段 `executionPipelines` / `environmentMatrices`，subject 目录复杂度会回弹。
- 如果 `tests/` taxonomy 不先清理，新机制测试会继续写进 `test_phase*` / `test_stage*` 路径。

### blockers

- 当前无外部 blocker；v1.03 基线已就绪，可进入 Phase 0 child plan。
