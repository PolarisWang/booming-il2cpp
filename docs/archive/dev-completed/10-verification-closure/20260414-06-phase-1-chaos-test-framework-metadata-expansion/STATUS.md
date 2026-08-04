---
task_id: 20260414-06-phase-1-chaos-test-framework-metadata-expansion
title: Phase 1 Chaos TestFramework Metadata Expansion
task_type: plan
lifecycle_status: completed
phase: completed
created_at: 2026-04-14 10:28:00 +08:00
updated_at: 2026-04-14 10:19:44 +08:00
current_dir: docs/dev/completed/20260414-06-phase-1-chaos-test-framework-metadata-expansion
parent_task_id: 20260414-04-il2cpp-hotupdate-capability-expansion-roadmap
source_task_id: 20260414-04-il2cpp-hotupdate-capability-expansion-roadmap
source_relation: roadmap-child
active: false
---

## 关键文档

- parent_design: `docs/dev/in-progress/20260414-04-il2cpp-hotupdate-capability-expansion-roadmap/design-v1-01.md`
- parent_roadmap: `docs/dev/in-progress/20260414-04-il2cpp-hotupdate-capability-expansion-roadmap/roadmap-v1-01.md`
- phase_0_freeze: `docs/dev/completed/20260414-05-phase-0-capability-taxonomy-and-inventory-freeze/capability-taxonomy-freeze-v1-01.md`
- plan: `docs/dev/completed/20260414-06-phase-1-chaos-test-framework-metadata-expansion/plan-v1-01.md`

## 当前判断

- current_focus: Phase 1 已完成，`Chaos.TestFramework`、declaration discovery、compiled catalog 已具备 capability family / capability item / archetype / hot-update capability 元数据闭环。
- why_now: Phase 2 可以直接在真实 `SolutionCorePack` proof/benchmark/archetype 上消费这套稳定 metadata，而不需要继续回头补 framework 合同。
- success_definition:
  - `Chaos.TestFramework` 暴露紧凑枚举与 attribute 字段
  - declaration discovery 能读出新 metadata
  - compiled catalog 能输出新 metadata
  - fixture 与回归测试覆盖新字段

## 最近摘要

- 2026-04-14 10:28:00 +08:00: 创建 Phase 1 child task，准备按 TDD 扩展 framework metadata。
- 2026-04-14 10:19:44 +08:00: 完成 capability family / capability item 枚举与 attribute 字段扩展，接通 declaration discovery / Python catalog / label helper，并通过 `test_chaos_test_framework_contracts.py` 与 `test_declared_metadata_discovery.py`。

## 下一步

- next_action: 返回父 roadmap，创建并执行 `20260414-07-phase-2-solution-core-pack-mechanical-split`。
- owner: codex
- trigger: Phase 1 目标测试通过，framework metadata 合同已稳定

## 风险 / 阻塞

### risks

- Phase 2 开始后，如果 `SolutionCorePack` 的真实 proof / benchmark 条目没有同步补 capability metadata，planner / dashboard 仍然无法稳定消费这套新合同。
- 后续 `HotUpdateHostPack` / `MixedExecutionFeaturePack` 也必须沿用同一套数值口径，避免再次引入字符串侧协议漂移。

### blockers

- 当前无外部 blocker。

## wiki

- 本子任务先不更新 `wiki/`。
- 稳定后的公开 contract 由父 roadmap Phase 5/6 统一沉淀到 `docs/architecture`。
