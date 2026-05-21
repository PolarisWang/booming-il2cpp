---
task_id: 20260415-20-phase-1-byref-and-method-shape-contract-expansion
title: Phase 1 Byref And Method Shape Contract Expansion
task_type: plan
lifecycle_status: completed
phase: completed
created_at: 2026-04-16 00:31:55 +08:00
updated_at: 2026-04-16 00:31:55 +08:00
current_dir: docs/dev/completed/20260415-20-phase-1-byref-and-method-shape-contract-expansion
parent_task_id: 20260415-18-post-phase-7-deferred-aot-shape-and-entry-abi-expansion-roadmap
source_task_id: 20260415-18-post-phase-7-deferred-aot-shape-and-entry-abi-expansion-roadmap
source_relation: roadmap-child
design_doc: docs/dev/completed/20260415-20-phase-1-byref-and-method-shape-contract-expansion/design-v1-01.md
plan_doc: docs/dev/completed/20260415-20-phase-1-byref-and-method-shape-contract-expansion/plan-v1-01.md
total_tasks: 4
current_task: completed
active: false
---

## 关键文档

- parent_status: `docs/dev/completed/20260415-18-post-phase-7-deferred-aot-shape-and-entry-abi-expansion-roadmap/STATUS.md`
- parent_roadmap: `docs/dev/completed/20260415-18-post-phase-7-deferred-aot-shape-and-entry-abi-expansion-roadmap/roadmap-v1-01.md`
- design: `docs/dev/completed/20260415-20-phase-1-byref-and-method-shape-contract-expansion/design-v1-01.md`
- plan: `docs/dev/completed/20260415-20-phase-1-byref-and-method-shape-contract-expansion/plan-v1-01.md`

## 当前结论

- widened method shape 所需的 metadata decode 和 `AotCoreIr` 输入合同已经闭环。
- loader 侧 function pointer、generic method reference、ABI widening 所需的输入面已经被归并到统一 contract，而不是留在临时特例里。
- planner / emitter 后续 widening 已经建立在 Phase 1 的显式 shape 上。

## 最近摘要

- 2026-04-16 00:31:55 +08:00: 以当前分支实现为准归档 Phase 1，确认 loader / metadata / `AotCoreIr` widening 已完成。

## 下一步

- next_action: 无。本 child task 已完成，由父 roadmap 继续收口 Phase 2-5。
- owner: codex
- trigger: 无。

## 风险 / 阻塞

### risks

- 如果后续新增更复杂的 byref 组合面，仍需先在 loader / IR 层扩 contract，再进入 planner / emitter。

### blockers

- 当前无外部 blocker。

## 验证

- `python -m pytest tests/unit/compatibility/test_phase7_loader_function_pointer_decode.py tests/unit/compatibility/test_phase7_loader_generic_method_reference_decode.py tests/unit/compatibility/test_phase4d_abi_widening_aot_core_ir.py -q` -> 已包含于父任务 `274 passed` 总体验证。

## wiki

- 暂不单独写入 wiki。
