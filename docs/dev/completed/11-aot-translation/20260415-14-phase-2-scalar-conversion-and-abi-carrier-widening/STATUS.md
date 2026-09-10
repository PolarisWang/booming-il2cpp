---
task_id: 20260415-14-phase-2-scalar-conversion-and-abi-carrier-widening
title: Phase 2 Scalar Conversion And ABI Carrier Widening
task_type: plan
lifecycle_status: completed
phase: completed
created_at: 2026-04-15 21:23:30 +08:00
updated_at: 2026-04-15 22:04:44 +08:00
current_dir: docs/dev/completed/20260415-14-phase-2-scalar-conversion-and-abi-carrier-widening
parent_task_id: 20260415-11-phase-7-aot-core-ir-typed-il-translation-expansion-roadmap
source_task_id: 20260415-11-phase-7-aot-core-ir-typed-il-translation-expansion-roadmap
source_relation: roadmap-child
design_doc: docs/dev/completed/20260415-14-phase-2-scalar-conversion-and-abi-carrier-widening/design-v1-01.md
plan_doc: docs/dev/completed/20260415-14-phase-2-scalar-conversion-and-abi-carrier-widening/plan-v1-01.md
total_tasks: 4
current_task: completed
active: false
---

## 关键文档

- parent_status: `docs/dev/in-progress/20260415-11-phase-7-aot-core-ir-typed-il-translation-expansion-roadmap/STATUS.md`
- parent_roadmap: `docs/dev/in-progress/20260415-11-phase-7-aot-core-ir-typed-il-translation-expansion-roadmap/roadmap-v1-01.md`
- phase_1_status: `docs/dev/completed/20260415-13-phase-1-loader-and-metadata-decode-expansion/STATUS.md`
- brainstorm: `docs/dev/completed/20260415-14-phase-2-scalar-conversion-and-abi-carrier-widening/brainstorm-v1-01.md`
- design: `docs/dev/completed/20260415-14-phase-2-scalar-conversion-and-abi-carrier-widening/design-v1-01.md`
- plan: `docs/dev/completed/20260415-14-phase-2-scalar-conversion-and-abi-carrier-widening/plan-v1-01.md`

## 当前结论

- Task 1-2 已完成：`Int8 / UInt8 / Int16 / UInt16` carrier、`ResolveAbiSlot(...)` 与 native AOT consume 路径已经通过 `IntegerCarrierProofEntry` / `IntegerCarrierBenchmarkEntry` 形成真实闭环。
- Task 3 已完成：`Float32 / Float64` carrier、`ldc.r4 / ldc.r8`、`conv.r4 / conv.r8` 以及 float/double ABI 参数、返回路径已经通过 `FloatingPointCarrierBenchmarkEntry` 形成真实 native AOT 闭环。
- Task 4 已完成：ABI widening、小整数 widening、浮点 widening 与 capability batch 套件保持绿色；完整 `tests/unit/compatibility -q` 结果为 `236 passed`。
- 本 child task 的收口边界明确为 scalar carrier / conversion / minimal native ABI consumption widening；structured EH/control-flow 转入 `20260415-15-phase-3-structured-control-flow-and-exception-expansion`。

## 最近摘要

- 2026-04-15 21:48:08 +08:00: 完成 `test_phase2_small_integer_carrier_native_aot.py` 与相关 ABI widening 回归，integer-first widening 稳定转绿。
- 2026-04-15 22:04:44 +08:00: 新增 `test_phase2_floating_point_carrier_native_aot.py` 与 `FloatingPointCarrierBenchmark.cs`，补齐 `Float32/Float64` carrier 与 `conv.r4/r8` native lowering。
- 2026-04-15 22:04:44 +08:00: 运行 `python -m pytest tests/unit/compatibility -q`，结果 `236 passed`；Phase 2 进入 completed。

## 下一步

- next_action: 无。本 child task 已完成，父 roadmap 切入 `20260415-15-phase-3-structured-control-flow-and-exception-expansion`。
- owner: codex
- trigger: 无。

## 风险 / 阻塞

### risks

- Phase 2 只收口 float/double carrier / literal / conversion / ABI 路径；若 retained owner subject 后续引入 float/double 真正算术路径，需要单独开新的 widening 切片。
- 当前 phase 结束后，最接近 native AOT 主线的新 blocker 已转为 structured EH/control-flow；如果为了 `FinallyAndFilter` 先扩 dispatch，会把 Phase 3 与 Phase 4 的边界重新打散。

### blockers

- 当前无外部 blocker。

## 验证

- `python -m pytest tests/unit/compatibility/test_phase2_small_integer_carrier_native_aot.py -q`
- `python -m pytest tests/unit/compatibility/test_phase2_floating_point_carrier_native_aot.py -q`
- `python -m pytest tests/unit/compatibility/test_phase4d_abi_widening_aot_core_ir.py tests/unit/compatibility/test_phase4d_abi_widening_native_aot.py -q`
- `python -m pytest tests/unit/compatibility/test_mainline_capability_batch_a.py tests/unit/compatibility/test_mainline_capability_batch_b.py -q`
- `python -m pytest tests/unit/compatibility/test_phase2_small_integer_carrier_native_aot.py tests/unit/compatibility/test_phase2_floating_point_carrier_native_aot.py tests/unit/compatibility/test_phase4d_abi_widening_aot_core_ir.py tests/unit/compatibility/test_phase4d_abi_widening_native_aot.py tests/unit/compatibility/test_mainline_capability_batch_a.py tests/unit/compatibility/test_mainline_capability_batch_b.py -q` -> `15 passed`
- `python -m pytest tests/unit/compatibility -q` -> `236 passed`

## wiki

- 本阶段仍不单独沉淀长期 wiki；关键边界已经收口到 Phase 2 文档与父 roadmap，后续若浮点算术 widening 再次成为主线，再抽取到 architecture/wiki。
