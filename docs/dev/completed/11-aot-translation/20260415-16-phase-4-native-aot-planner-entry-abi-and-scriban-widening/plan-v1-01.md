# Phase 4 Native AOT Planner Entry ABI And Scriban Widening 实现计划

> 面向执行 Agent：继续采用 `RED -> GREEN -> REFACTOR`，并同步维护 `STATUS.md`、`docs/dev/ACTIVE.md` 与 `notes/progress-*.md`。

## 目标

- 用真实 generic dispatch owner proof 锁定 Phase 4 RED
- 扩大 planner / emitter 对 generic virtual/interface dispatch 的消费能力
- 保持生成出口仍然是 `NativeAotEmitter + Scriban`

## 设计文档

- `docs/dev/completed/20260415-16-phase-4-native-aot-planner-entry-abi-and-scriban-widening/design-v1-01.md`

## 涉及文件与职责

- `subjects/SolutionCorePack/source/FeatureSlices/CoreRuntimeFeatures/ObjectModelAndDispatch/*.cs`
- `src/managed/Chaos.IL2CPP.CodeGen/NativeAotLoweringPlanner.cs`
- `src/managed/Chaos.IL2CPP.CodeGen/NativeAotEmitter.cs`
- `src/managed/Chaos.IL2CPP.CodeGen/Templates/NativeAot*.scriban`
- `tests/unit/compatibility/` 下新增或扩展的 Phase 4 real-driver tests

## 执行步骤

- [x] Task 1: 用 generic virtual/interface dispatch owner proof 锁 RED
  - 审计现有 `DispatchProof` / `InterfaceDispatchProof`
  - 补真实 generic dispatch compatibility tests
- [x] Task 2: 扩大 planner 的 generic dispatch lowering
  - 收口 dispatch slot / target 解析
  - 保持不扩 public workload ABI
- [x] Task 3: 验证 emitter / Scriban 路径仍然闭环
  - 生成产物级断言
  - 如需扩 model，继续经 `NativeAotEmitter + Scriban`
- [x] Task 4: 跑验证并决定是否切入 Phase 5
  - targeted suites
  - 完整 `tests/unit/compatibility -q`
  - 更新父 roadmap / index / notes

## 本轮验证

- `python -m pytest tests/unit/compatibility/test_phase4_generic_dispatch_native_aot.py -q` -> `3 passed`
- `python -m pytest tests/unit/compatibility/test_phase4_generic_dispatch_native_aot.py tests/unit/compatibility/test_phase4d_dispatch_native_aot.py tests/unit/compatibility/test_phase4d_dispatch_aot_core_ir.py tests/unit/compatibility/test_interface_dispatch_aot_runtime_completeness.py -q` -> `10 passed`
- `python -m pytest tests/unit/compatibility -q` -> `242 passed`
