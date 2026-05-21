# Phase 1 Loader And Metadata Decode Expansion 实现计划

> 面向执行 Agent：按 `RED -> GREEN -> REFACTOR` 推进，并保持 `STATUS.md`、`docs/dev/ACTIVE.md` 与 `notes/progress-*.md` 对齐。

## 目标

- 关闭 `20260415-12` 冻结台账里属于 loader / metadata decode 的可达缺口。
- 让真实 `calli/function pointer` 与 closed generic method-spec 路径稳定进入 `typed-il` / `aot-core-ir`。

## 设计文档

- `docs/dev/completed/20260415-13-phase-1-loader-and-metadata-decode-expansion/design-v1-01.md`

## 涉及文件与职责

- `src/managed/Chaos.IL2CPP.Loader/LoaderStage.cs`
- `src/managed/Chaos.IL2CPP.Loader/Metadata/TypeProviders.cs`
- `src/managed/Chaos.IL2CPP.Contracts/ManagedClosureContracts.cs`
- `src/managed/Chaos.IL2CPP.CodeGen/CodeGenStage.cs`
- `src/managed/Chaos.IL2CPP.CodeGen/AotCoreIrLowering.cs`
- `tests/unit/compatibility/test_phase7_loader_function_pointer_decode.py`
- `tests/unit/compatibility/test_phase7_loader_generic_method_reference_decode.py`

## 执行步骤

- [x] Task 1: 锁定 Batch A 真实入口与失败形态
  - proof: `CoreRuntimeFeatures/FunctionPointerProofEntry::Run()`
  - benchmark: `CoreRuntimeBenchmarks/FunctionPointerBenchmarkEntry::RunWorkload()`
  - 新增 RED 测试并确认 `calli` 缺失稳定 signature contract
- [x] Task 2: 补齐 function pointer signature / `calli` contract
  - loader 解码 `StandaloneSignatureHandle`
  - `ManagedCallSiteSignature` 贯穿 `managed -> typed-il -> aot-core-ir`
  - `TypeProviders.GetFunctionPointerType(...)` 不再直接拒绝
- [x] Task 3: 收口 method-spec / handle-kind 可达缺口
  - 新增真实 `GenericInstantiationProofEntry::Run()` RED 测试
  - `TypedIlInstructionArtifact` 现已保留 `reference`
  - method/field `ldtoken` handle kind 明确记为“当前 C# 前端不可达，deferred”
- [x] Task 4: 跑通 Phase 1 验证并切入 Phase 2
  - targeted suites 全绿
  - `tests/unit/compatibility -q` 全绿
  - 准备父 roadmap 切换到 `20260415-14`

## 本轮验证

- `python -m pytest tests/unit/compatibility/test_phase7_loader_function_pointer_decode.py -q`
- `python -m pytest tests/unit/compatibility/test_phase7_loader_generic_method_reference_decode.py -q`
- `python -m pytest tests/unit/compatibility/test_phase4_aot_core_ir_contract.py -q`
- `python -m pytest tests/unit/compatibility/test_mainline_capability_batch_b.py -q`
- `python -m pytest tests/unit/compatibility/test_managed_closure_contract_bundle.py -q`
- `python -m pytest tests/unit/compatibility/test_phase4c_metadata_closure_contract.py -q`
- `python -m pytest tests/unit/compatibility -q`

## 完成结论

- 本阶段已完成，可归档到 `docs/dev/completed/20260415-13-phase-1-loader-and-metadata-decode-expansion/`。
