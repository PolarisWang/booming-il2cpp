---
task_id: 20260416-04-phase-2-aot-core-ir-semantic-surface-expansion
title: Phase 2 AotCoreIr Semantic Surface Expansion
task_type: plan
lifecycle_status: completed
phase: completed
created_at: 2026-04-16 09:58:34 +08:00
updated_at: 2026-04-16 10:36:50 +08:00
current_dir: docs/dev/completed/20260416-04-phase-2-aot-core-ir-semantic-surface-expansion
parent_task_id: 20260416-01-aot-core-ir-gap-audit-and-next-expansion-roadmap
source_task_id: 20260416-01-aot-core-ir-gap-audit-and-next-expansion-roadmap
source_relation: roadmap-child-phase-2
design_doc: docs/dev/completed/20260416-04-phase-2-aot-core-ir-semantic-surface-expansion/design-v1-02.md
plan_doc: docs/dev/completed/20260416-04-phase-2-aot-core-ir-semantic-surface-expansion/plan-v1-02.md
total_tasks: 6
current_task: completed
active: false
---

## 关键文档

- parent_status: `docs/dev/abandoned/20260416-01-aot-core-ir-gap-audit-and-next-expansion-roadmap/STATUS.md`
- parent_roadmap: `docs/dev/abandoned/20260416-01-aot-core-ir-gap-audit-and-next-expansion-roadmap/roadmap-v1-01.md`
- design: `docs/dev/completed/20260416-04-phase-2-aot-core-ir-semantic-surface-expansion/design-v1-02.md`
- plan: `docs/dev/completed/20260416-04-phase-2-aot-core-ir-semantic-surface-expansion/plan-v1-02.md`
- progress_note: `docs/dev/completed/20260416-04-phase-2-aot-core-ir-semantic-surface-expansion/notes/progress-20260416-1036.md`

## 当前结论

- `Phase 2` 已完成三批真实语义收口：
  - `ldtoken` 的 `type / field / method targetReference`
  - `System.Int64` 的独立 ABI carrier
  - `System.UInt64` 的独立 ABI carrier
- `AotCoreIrAbiCarrierKind` 已从 `small integer + float` 扩展到覆盖 `Int64 / UInt64`，不再把真实 64-bit integer surface 压缩成 `NativeInt`
- `NativeAotLoweringPlanner` 已同步具备 `Int64 / UInt64` 的 bit-preserving ABI 往返 helper：
  - `chaos_store_int64 / chaos_load_int64`
  - `chaos_store_uint64 / chaos_load_uint64`
- `SolutionCorePack` owner evidence 已新增并收口：
  - `IntegerCarrierProof / IntegerCarrierBenchmark` 的 `RoundTripInt64`
  - `NativeCallInteropBenchmark` 的 `RoundTripUInt64(GetTickCount64())`
- `NativeCallInteropBenchmark` 的 workload 已收敛为 native-friendly 主路径，不再把 reflection fallback 带入 native closure

## 最近摘要

- 2026-04-16 09:58:34 +08:00: 创建 `20260416-04` 子任务，正式切入 `Phase 2`
- 2026-04-16 10:13:21 +08:00: 完成 batch A `ldtoken targetReference` 收口；局部回归结果为 `19 passed`
- 2026-04-16 10:36:50 +08:00: 完成 batch B/C `Int64 + UInt64` ABI carrier widening；扩展 owner benchmark/proof，并将相邻 compatibility 回归压到 `30 passed`

## 下一步

- next_action: 无。本 child task 已完成；父 roadmap 继续推进 `20260416-05-phase-3-native-aot-planner-and-entry-abi-generalization`
- owner: codex
- trigger: 用户要求继续执行主 roadmap

## 风险 / 阻塞

### risks

- 虽然 `Phase 2` 已完成 64-bit carrier 语义收口，但 planner 层仍未覆盖纯 `UInt64` literal / bitwise 这类更宽 opcode legalization 面
- 后续 `Phase 3` 与 `Phase 4` 仍需继续保持 `IR -> planner -> emitter/Scriban` 分层收口，避免重新退化成 subject 侧绕路

### blockers

- 当前无外部 blocker

## 验证

- `python -m pytest tests/unit/compatibility/test_phase2_int64_carrier_native_aot.py -q`
  - 结果：`2 passed`
- `python -m pytest tests/unit/compatibility/test_phase2_uint64_interop_carrier_native_aot.py -q`
  - 结果：`2 passed`
- `python -m pytest tests/unit/compatibility/test_phase2_int64_carrier_native_aot.py tests/unit/compatibility/test_phase2_uint64_interop_carrier_native_aot.py tests/unit/compatibility/test_phase2_small_integer_carrier_native_aot.py tests/unit/compatibility/test_phase2_floating_point_carrier_native_aot.py tests/unit/compatibility/test_phase2_aot_core_ir_ldtoken_target_reference.py tests/unit/compatibility/test_phase1_loader_typed_reference_decode.py tests/unit/compatibility/test_phase1_loader_vararg_decode.py tests/unit/compatibility/test_phase4_aot_core_ir_contract.py tests/unit/compatibility/test_mainline_capability_batch_a.py tests/unit/compatibility/test_mainline_capability_batch_b.py tests/unit/compatibility/test_phase7_loader_function_pointer_decode.py tests/unit/compatibility/test_phase7_loader_generic_method_reference_decode.py tests/unit/compatibility/test_loader_switch_opcode.py -q`
  - 结果：`30 passed`

## wiki

- 暂不写入 wiki；等 `Phase 3` / `Phase 4` 完成 planner/emitter widening 模式后再统一沉淀
