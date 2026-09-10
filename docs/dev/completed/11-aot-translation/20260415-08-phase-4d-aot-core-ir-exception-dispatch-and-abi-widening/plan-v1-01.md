# Phase 4D AotCoreIr Exception Dispatch And Abi Widening 实现计划

> 面向执行 Agent：必须使用 `dev:executing-plans` 与 `dev:test-driven-development`；遇到失败时切到 `dev:systematic-debugging`。先做 RED 测试，再做最小实现，并持续回写 `STATUS.md` 与 `docs/dev/ACTIVE.md`。

**目标**

- 把 `AotCoreIr` 从 Phase 4C 的 generic / metadata contract，继续推进到 exception、dispatch 与更宽 ABI 的真实 native translation surface。

**架构边界**

- Native AOT 生成链继续固定为 `NativeAotLoweringPlanner + NativeAotEmitter + Templates/NativeAot*.scriban`。
- 先在 contract 与 lowering 层建立 typed carrier，再由 planner 产出 template model，最后让 emitter 只做 Scriban 渲染。
- `Chaos.TestFramework` 只保留在 `src/reference/Chaos.TestFramework`，不混入 `AotCoreIr` / planner / emitter。

**关键文档**

- `docs/dev/in-progress/20260414-37-hybridclr-aligned-aot-hotupdate-development-plan/roadmap-v1-02.md`
- `docs/dev/completed/20260415-08-phase-4d-aot-core-ir-exception-dispatch-and-abi-widening/design-v1-01.md`

**预期改动面**

- `src/managed/Chaos.IL2CPP.Contracts/ManagedClosureContracts.cs`
- `src/managed/Chaos.IL2CPP.CodeGen/AotCoreIrLowering.cs`
- `src/managed/Chaos.IL2CPP.CodeGen/NativeAotLoweringPlanner.cs`
- `tests/unit/compatibility/test_phase4d_*`
- `subjects/SolutionCorePack/source/FeatureSlices/CoreRuntimeFeatures/*`
- `docs/dev/completed/20260415-08-phase-4d-aot-core-ir-exception-dispatch-and-abi-widening/STATUS.md`
- `docs/dev/in-progress/20260414-37-hybridclr-aligned-aot-hotupdate-development-plan/STATUS.md`

## 执行步骤

- [x] Task 1：建立 Phase 4D RED 测试
  - 完成 dispatch widening 的 planner-level / native-aot-level RED。
  - 完成 exception lowering contract RED。
  - 完成 ABI widening retained proof 与 planner/native-aot RED。
- [x] Task 2：扩展 `ManagedClosureContracts`
  - 引入 `AotCoreIrExceptionRegionKind : byte` 与 `AotCoreIrExceptionRegionArtifact`。
  - 引入 `AotCoreIrAbiCarrierKind : byte`、`AotCoreIrAbiSlotArtifact`、`ReturnAbi`、`ParameterAbis`。
- [x] Task 3：扩展 `AotCoreIrLowering`
  - 把 EH region 真实 lowering 到 `aot-core-ir.json`。
  - 把 widened ABI carrier 真实 lowering 到 method return / parameter slots。
- [x] Task 4：扩展 `NativeAotLoweringPlanner`
  - dispatch widening 已闭环。
  - native-aot exception catch-only 已闭环。
  - ABI widening 已闭环，planner 现已消费 ABI slot 参数/返回值并生成 widened native signatures。
  - `finally` / `filter` / `fault` 留到后续独立切片。
- [x] Task 5：保持 `NativeAotEmitter` 与 Scriban 边界
  - 本轮没有把新逻辑塞回 emitter string patch。
  - Native AOT 生成链继续经由 `Templates/NativeAot*.scriban`。
- [x] Task 6：在 retained owner subject 上闭环 proof 并做回归
  - `DispatchProofEntry` 已在 native-aot 路径闭环。
  - `ExceptionCatchProofEntry` 已在 native-aot 路径闭环。
  - `AbiWideningProofEntry` 已在 native-aot 路径闭环。

## 阶段目标

- `AotCoreIr` 显式承载真实可执行的 dispatch / exception / ABI widening contract。
- Native AOT 生成链继续稳定建立在 `planner + emitter + Scriban templates` 之上。
- retained owner subject 至少有一条 dispatch proof、一条 exception proof 与一条 ABI widening proof 在新链路上闭环。

## 验收标准

- `AotCoreIr` 能显式表达 dispatch site / exception region / widened ABI carrier。
- `AotCoreIrLowering` 能把上述 typed carrier 从 managed world lowering 到 `aot-core-ir.json`。
- `NativeAotLoweringPlanner` 能基于新 carrier 产出稳定 template model。
- `NativeAotEmitter` 继续只通过 `.scriban` 渲染新增输出。
- retained proofs 在 managed / native 路径上持续闭环。

## 验证命令

- `python -m pytest tests/unit/compatibility/test_phase4d_abi_widening_aot_core_ir.py tests/unit/compatibility/test_phase4d_abi_widening_native_aot.py -q`
- `python -m pytest tests/unit/compatibility/test_mainline_capability_batch_a.py tests/unit/compatibility/test_mainline_capability_batch_b.py tests/unit/compatibility/test_phase4b_aot_core_ir_object_model.py tests/unit/compatibility/test_phase4c_aot_core_ir_generics.py tests/unit/compatibility/test_phase4c_generic_layout_aot_core_ir.py tests/unit/compatibility/test_phase4c_generic_layout_native_aot.py tests/unit/compatibility/test_phase4c_metadata_closure_contract.py tests/unit/compatibility/test_metadata_supplement_bridge_subjects.py tests/unit/compatibility/test_phase4d_dispatch_aot_core_ir.py tests/unit/compatibility/test_phase4d_dispatch_native_aot.py tests/unit/compatibility/test_phase4d_exception_aot_core_ir.py tests/unit/compatibility/test_phase4d_exception_native_aot.py tests/unit/compatibility/test_phase4d_abi_widening_aot_core_ir.py tests/unit/compatibility/test_phase4d_abi_widening_native_aot.py -q`
- `dotnet build src/managed/Chaos.IL2CPP.CodeGen/Chaos.IL2CPP.CodeGen.csproj -c Release -m:1`
- `dotnet build src/managed/Chaos.IL2CPP.Driver/Chaos.IL2CPP.Driver.csproj -c Release -m:1`

## 注意事项

- 不扩展新的 string protocol；新 contract 优先枚举化、紧凑化。
- 不要把 `Chaos.TestFramework` 逻辑混入 `AotCoreIr` / planner / emitter。
- dispatch、exception、ABI widening 必须拆开做 `RED -> GREEN`，避免大补丁混做导致根因不清。
