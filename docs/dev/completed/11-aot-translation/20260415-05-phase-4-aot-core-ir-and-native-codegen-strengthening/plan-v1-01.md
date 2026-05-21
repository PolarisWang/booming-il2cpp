# Phase 4 AOT Core IR And Native Codegen Strengthening 实现计划

> 面向执行 Agent：必须使用 `dev:executing-plans`、`dev:test-driven-development` 和 `dev:systematic-debugging` 推进本计划。先补 RED 测试，再做最小实现。
> 目标：把 `typed-il -> AotCoreIr -> native` 变成正式主线，逐步替代当前 native-aot 的 ad-hoc entry-only 发射方式，并为对象模型、generic/sharing、exception、metadata closure 后续扩张建立稳定中层。

**设计文档**

- `docs/dev/in-progress/20260414-37-hybridclr-aligned-aot-hotupdate-development-plan/roadmap-v1-01.md`
- `docs/dev/in-progress/20260414-37-hybridclr-aligned-aot-hotupdate-development-plan/design-v1-01.md`
- `docs/dev/in-progress/20260414-37-hybridclr-aligned-aot-hotupdate-development-plan/review-v1-01.md`
- `docs/dev/completed/20260415-03-phase-2-typed-il-shared-contract-and-hybrid-dispatch-foundation/STATUS.md`
- `docs/dev/completed/20260415-04-phase-3-hotupdate-runtime-and-supplemental-metadata-foundation/STATUS.md`

## 预期改动面

- `src/managed/Chaos.IL2CPP.Contracts/ManagedClosureContracts.cs`
- `src/managed/Chaos.IL2CPP.CodeGen/CodeGenStage.cs`
- `src/managed/Chaos.IL2CPP.CodeGen/AotCoreIrLowering.cs`
- `src/managed/Chaos.IL2CPP.CodeGen/NativeAotEmitter.cs`
- 视实现需要补充的 codegen gap ledger / status docs
- `tests/unit/compatibility/test_phase4_aot_core_ir_contract.py`
- `tests/unit/compatibility/test_multi_assembly_entry_override.py`
- `tests/unit/compatibility/test_managed_closure_contract_bundle.py`
- `tests/unit/performance/test_native_aot_workload_entry_bundle.py`

## 执行步骤

- [x] Task 1: 先补 RED 测试，锁定 `AotCoreIr` contract、closure artifact 暴露面与 `NativeAotEmitter` 改为消费 `AotCoreIr`
  - `ManagedClosureArtifactNames.AotCoreIr`
  - `ManagedClosureResult.AotCoreIr`
  - `DriverEntry` 落盘 `aot-core-ir.json`
  - `NativeAotEmitter` 不再直接依赖 loader model
- [x] Task 2: 接入 direct-call metadata，并让 native-aot 从 entry-only 发射推进到 reachable static method emission
  - `AotCoreIrInstructionArtifact` 携带 `TargetSymbol / TargetParameterCount / TargetReturnType`
  - `AotCoreIrMethodArtifact` 携带 `NativeSymbol`
  - `NativeAotEmitter` 支持最小 static direct call、`ldarg`，并发出 reachable helper method 定义
- [x] Task 3: 把对象模型与 metadata closure 的最小 carrier 接入 `AotCoreIr`
  - 至少覆盖 type / field / method reference operand 的稳定载体
  - 避免 emitter 或后续 runtime 再回读 loader 临时结构
  - 先锁最小 contract，不一次性吞掉完整对象模型
- [x] Task 4: 冻结 generic / exception / metadata closure 的 codegen gap 边界
  - 明确 `AotCoreIr` 当前支持 / partial / unsupported 的具体面
  - 为 generic/sharing contract、exception lowering、metadata closure 留出正式字段或 gap 记录
  - 避免后续 Phase 5 重新回到临时 patch 路线
- [ ] Task 5: 跑完整验证并回写父 roadmap / 子任务状态，满足归档前置条件

## 验收

- `AotCoreIr` 已成为 native-aot 的正式输入，而不是额外挂件
- 当前 native-aot 关键路径至少满足：
  - 真实 closure 中能写出 `aot-core-ir.json`
  - `NativeAotEmitter` 基于 `AotCoreIr` 发出 entry method 与 reachable static direct-call methods
  - multi-assembly entry override 仍能打通到 native-aot
- Phase 4 剩余未覆盖能力必须被显式记录为 pending / partial / unsupported，而不是静默缺失

## 验证命令

- `python -m pytest tests/unit/compatibility/test_phase4_aot_core_ir_contract.py tests/unit/compatibility/test_multi_assembly_entry_override.py -q`
- `python -m pytest tests/unit/compatibility/test_managed_closure_contract_bundle.py tests/unit/performance/test_native_aot_workload_entry_bundle.py -q`
- `dotnet build src/managed/Chaos.IL2CPP.CodeGen/Chaos.IL2CPP.CodeGen.csproj -c Release -m:1`
- `dotnet build src/managed/Chaos.IL2CPP.Driver/Chaos.IL2CPP.Driver.csproj -c Release -m:1`
- `python -m pytest tests/unit/compatibility -q`
