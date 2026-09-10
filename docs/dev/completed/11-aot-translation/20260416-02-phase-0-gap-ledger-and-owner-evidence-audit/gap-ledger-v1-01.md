# Phase 0 Gap Ledger v1.01

Date: 2026-04-16 09:05:23 +08:00
Status: completed

## 1. Ledger 口径

- `support_state`
  - `partial`: 已有一批实现与证据，但仍存在显式 unsupported 边界
  - `unsupported`: 目前还没有闭环实现路径
  - `evidence-only-gap`: 底层基本具备，但 owner proof / benchmark / gate 未补齐
- `owner_subject`
  - 纯 AOT family 默认固定到 `SolutionCorePack`
  - 仅在明确跨运行时语义时才允许其他 pack 参与

## 2. Gap Families

| family_id | family_name | current_support_state | stage_tags | key_code_anchors | existing_compatibility_anchors | child_phase | notes |
| --- | --- | --- | --- | --- | --- | --- | --- |
| `F01` | Loader Input And Metadata Decode | `partial` | `loader` | `LoaderStage.cs:206`, `LoaderStage.cs:748`, `LoaderStage.cs:778`, `LoaderStage.cs:938`, `LoaderStage.cs:1301`, `LoaderStage.cs:1448`, `LoaderStage.cs:1496`, `LoaderStage.cs:1515` | `test_phase7_loader_function_pointer_decode.py`, `test_phase7_loader_generic_method_reference_decode.py`, `test_loader_switch_opcode.py` | `20260416-03` | 已覆盖 function pointer、closed generic method reference、switch；剩余 entry handle、type/method/field/ldtoken handle 与 resolution scope decode 仍未冻结。 |
| `F02` | AotCoreIr Identity And Typed Contract | `partial` | `loader`, `aot-core-ir` | `AotCoreIrLowering.cs:59`, `AotCoreIrLowering.cs:65`, `AotCoreIrLowering.cs:76`, `AotCoreIrLowering.cs:503`, `AotCoreIrLowering.cs:516`, `AotCoreIrLowering.cs:665`, `AotCoreIrLowering.cs:677` | `test_phase4c_aot_core_ir_generics.py`, `test_phase4d_abi_widening_aot_core_ir.py`, `test_phase4d_dispatch_aot_core_ir.py`, `test_phase4d_exception_aot_core_ir.py` | `20260416-04` | 当前 IR 对 typed-il block/identity 对齐要求严格，但还没有把剩余 object/reference/value families 完整冻成统一 contract。 |
| `F03` | Object Model, Reference Semantics, And Low-Level Value Flow | `partial` | `loader`, `aot-core-ir`, `planner`, `emitter` | `LoaderStage.cs:778`, `NativeAotLoweringPlanner.cs:860`, `NativeAotLoweringPlanner.cs:2427` | `test_phase4b_aot_core_ir_object_model.py`, `test_phase4b_array_boxing_native_aot.py`, `test_phase4b_array_covariance_native_aot.py`, `test_phase4b_array_ops_native_aot.py`, `test_phase4b_boxed_struct_field_read_native_aot.py`, `test_phase4b_cast_type_check_native_aot.py`, `test_phase4b_object_field_native_aot.py`, `test_phase4b_object_return_native_aot.py` | `20260416-04` | 已有 object/array/cast/box 一批 proof；后续要把 address/byref/object helper 与更广的 value/reference 语义成体系收口。 |
| `F04` | Generic Layout, Sharing, And Dispatch Closure | `partial` | `loader`, `aot-core-ir`, `planner`, `emitter` | `LoaderStage.cs:1496`, `NativeAotLoweringPlanner.cs:338`, `NativeAotLoweringPlanner.cs:908`, `NativeAotLoweringPlanner.cs:964` | `test_phase4c_generic_layout_aot_core_ir.py`, `test_phase4c_generic_layout_native_aot.py`, `test_phase4_generic_dispatch_native_aot.py`, `test_phase4d_dispatch_aot_core_ir.py`, `test_phase4d_dispatch_native_aot.py` | `20260416-05` | generic closed refs、generic context、dispatch widening 已有基线，但 unresolved dispatch slot、dispatch kind 与更广 sharing 组合仍未收口。 |
| `F05` | Entry ABI, Carrier, And Native Workload Shape | `partial` | `aot-core-ir`, `planner`, `emitter`, `scriban` | `NativeAotLoweringPlanner.cs:546`, `NativeAotLoweringPlanner.cs:552`, `NativeAotLoweringPlanner.cs:2477`, `NativeAotLoweringPlanner.cs:2692`, `NativeAotLoweringPlanner.cs:2734`, `CodeGenStage.cs:392`, `CodeGenStage.cs:398`, `NativeAotEmitter.cs:85`, `NativeAotEmitter.cs:91` | `test_phase4d_abi_widening_aot_core_ir.py`, `test_phase4d_abi_widening_native_aot.py`, `test_scriban_vendor_build.py` | `20260416-05`, `20260416-06` | 当前仅支持 `generic-managed-entry + int(void)` 与有限 carrier；后续要先泛化 planner ABI，再扩大 emitter/workload shape。 |
| `F06` | Structured Exception And Control Flow Generalization | `partial` | `loader`, `aot-core-ir`, `planner`, `emitter` | `LoaderStage.cs:1448`, `NativeAotLoweringPlanner.cs:571`, `NativeAotLoweringPlanner.cs:2017` | `test_phase3_finally_filter_native_aot.py`, `test_phase4d_exception_aot_core_ir.py`, `test_phase4d_exception_native_aot.py`, `test_nested_exception_proof_subject.py` | `20260416-05` | catch/finally/filter 一批路径已闭环，但 planner 对更广 EH linear lowering 仍有限，benchmark 默认不要求。 |
| `F07` | Reflection, Metadata Closure, And Reference Bundle Validation | `partial` | `loader`, `aot-core-ir`, `planner`, `owner-evidence`, `gate` | `LoaderStage.cs:938`, `LoaderStage.cs:1496`, `LoaderStage.cs:1515` | `test_phase4c_metadata_closure_contract.py`, `test_phase5_owner_subject_coverage.py`, `test_solution_core_pack_subject.py` | `20260416-07`, `20260416-08` | 该 family 额外承接“仓库内 reference bundle / full-solution 验证”审计维度，避免把 reference-heavy 问题拆成零散 opcode。 |

## 3. Deferred 边界

### 3.1 不在本轮 Phase 0 处理

- 任何 loader / planner / emitter 代码实现
- 任何 `SolutionCorePack` proof/benchmark 代码新增
- 任何热更或混合执行语义扩张

### 3.2 已冻结的执行顺序

1. `F01`
2. `F02` / `F03`
3. `F04` / `F05` / `F06`
4. `F07`

## 4. 结论

- 本轮剩余 gap 已足够收敛到 7 个 capability family。
- 后续 child task 应沿 family 纵向推进，而不是按源文件逐条修 unsupported。
