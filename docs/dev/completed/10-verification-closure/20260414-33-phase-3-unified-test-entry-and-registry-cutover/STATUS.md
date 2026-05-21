---
task_id: 20260414-33-phase-3-unified-test-entry-and-registry-cutover
title: Phase 3 Unified Test Entry And Registry Cutover
task_type: plan
lifecycle_status: completed
phase: completed
created_at: 2026-04-14 17:32:13 +08:00
updated_at: 2026-04-14 18:37:13 +08:00
current_dir: docs/dev/completed/20260414-33-phase-3-unified-test-entry-and-registry-cutover
parent_task_id: 20260414-20-subject-generated-test-workspace-roadmap
source_task_id: 20260414-20-subject-generated-test-workspace-roadmap
source_relation: roadmap-child
active: false
---

## 关键文档

- parent_design: `docs/dev/completed/20260414-20-subject-generated-test-workspace-roadmap/design-v1-01.md`
- parent_roadmap: `docs/dev/completed/20260414-20-subject-generated-test-workspace-roadmap/roadmap-v1-01.md`
- phase_0_contract_freeze: `docs/dev/completed/20260414-30-phase-0-generated-workspace-contract-and-legacy-inventory/contract-freeze-v1-01.md`
- phase_2_completion: `docs/dev/completed/20260414-32-phase-2-workspace-v2-and-native-test-project-generation/STATUS.md`
- plan: `docs/dev/completed/20260414-33-phase-3-unified-test-entry-and-registry-cutover/plan-v1-01.md`

## 当前结论

- current_focus: Phase 3 已完成，`run test` / registry / benchmark catalog 已统一收口到 workspace manifest v2 与 generated host 主链。
- why_now: Phase 2 固化 workspace v2 之后，本阶段完成了 unified test entry 切换，并把 Phase 4 benchmark closure 与 Phase 5 legacy cleanup 所需的主链收口一并打通。
- success_definition:
  - `run test subject --id subject/<id>`、`run test declared-unit-test --id declared-unit-test/<stableId>`、`run test declared-benchmark --id declared-benchmark/<stableId>` 均优先消费 workspace manifest v2。
  - benchmark command 与 dashboard 直接消费 workspace declared catalog，并透传 `entryIndex`。
  - registry-object compatibility alias、legacy run shim 与 dead legacy dispatch code 已删除。

## 最近摘要

- 2026-04-14 17:32:13 +08:00: Phase 2 完成并归档；创建 Phase 3 child task，准备切换 unified test entry 与 registry 主链。
- 2026-04-14 18:37:13 +08:00: 完成 workspace-driven test entry cutover、benchmark catalog/dashboard closure、legacy registry alias 删除，并修复 loader 缺失 `conv.i` / `conv.u` 导致的 native-aot workload bundle 失败；全量 `python -m pytest` 通过。

## 下一步

- next_action: 无。本 child task 已完成；父 roadmap 将一并归档 Phase 4/5 的内联收口结果。
- owner: codex
- trigger: `python -m pytest` 全量通过，且 `docs/dev` 状态已回写。

## 风险 / 阻塞

### risks

- 当前无额外阻塞；后续如需继续 generic 化 `tests/unit/planning/test_subject_planner.py` 与 retained-subject 规划逻辑，需要另起 follow-up。

### blockers

- 无。

## 验证

- `python -m pytest tests/tooling/run/test_subject_command.py`
- `python -m pytest tests/tooling/run/test_unified_test_command.py`
- `python -m pytest tests/unit/performance/test_benchmark_command.py tests/unit/performance/test_benchmark_comparison.py tests/unit/performance/test_benchmark_dashboard_generator.py tests/unit/performance/test_benchmark_subject_sources.py tests/unit/run/test_benchmark_command.py tests/unit/run/test_benchmark_comparison.py tests/unit/run/test_benchmark_subject_sources.py`
- `python -m pytest tests/unit/run/test_repo_layout.py tests/integration/run tests/tooling/run/test_subject_command.py tests/tooling/run/test_unified_test_command.py tests/unit/performance/test_benchmark_command.py tests/unit/performance/test_benchmark_dashboard_generator.py`
- `python -m pytest tests/unit/performance/test_native_aot_workload_entry_bundle.py -q`
- `python -m pytest`

## wiki

- 本子任务只更新 `docs/dev`，未单独更新 `wiki/`。
