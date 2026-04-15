# AotCoreIr Codegen Gap Ledger v1.01

Date: 2026-04-15 05:02:07 +08:00
Status: in-progress

## 1. 目标

把 Phase 4 当前已经稳定的 `typed-il -> AotCoreIr -> native` 支撑面、仍处于 partial 的中间层 contract，以及明确未进入本轮的 gap 全部显式记录下来，避免后续 Phase 5/6 再回到“看到一个 feature 再 patch 一刀”的路线。

## 2. 当前已稳定支撑面

- `managed closure`
  - `ManagedClosureArtifactNames.AotCoreIr`
  - `ManagedClosureResult.AotCoreIr`
  - `DriverEntry` 落盘 `aot-core-ir.json`
- `method-level contract`
  - `AotCoreIrMethodArtifact.NativeSymbol`
  - `Identity / Signature / ReturnType / ParameterCount / LocalCount / ExceptionRegionCount`
- `instruction-level contract`
  - `Op / Operand / IlOffset / ResultType / Callee`
  - `Reference`
  - `TargetSymbol / TargetParameterCount / TargetReturnType`
  - `DispatchKindCode`
- `native-aot emitter`
  - 以 `AotCoreIr` 为正式输入
  - entry method wrapper + reachable static managed method emission
  - 最小 static direct call
  - `ldc.i4 / ldarg / ldloc / stloc / add / sub / mul / div / rem / br / blt / ret`

## 3. Partial / In-Progress

- `reference-bearing instructions`
  - `ManagedInstructionReference` 已带入 `AotCoreIrInstructionArtifact`
  - 当前只冻结 carrier，不代表 native emitter 已消费全部 `type / field / method` reference surface
- `metadata closure`
  - call target、method symbol、instruction reference 已进入中层
  - token-level metadata closure、preserve / reflection / runtime metadata stitching 仍未由 `AotCoreIr` 统一消费
- `object model`
  - 当前只完成“reference carrier 进中层”
  - `newobj / box / unbox / castclass / isinst / newarr / ldfld / stfld / ldsfld / stsfld` 等仍未进入 native-aot 正式 lowering

## 4. Unsupported / Deferred

- `generic / sharing`
  - generic context carrier
  - codegen closure for generic instantiations
  - runtime sharing hooks beyond current typed identity baseline
- `exceptions`
  - `ExceptionRegionCount` 已存在
  - 真正的 EH lowering、landing pad、finally/filter/rethrow 仍未进入本轮
- `dispatch`
  - virtual / interface / bridge thunk native emission
  - imported / external runtime call 的统一 native lowering
- `ABI / type system`
  - 非 `System.Int32` 参数与返回
  - value type object model
  - reference type layout / GC / write barrier
  - array / string / delegate / reflection runtime service

## 5. 阶段判断

- Phase 4 当前已经完成的不是“所有 AOT feature coverage”，而是：
  - 把 `AotCoreIr` 真正立为 native-aot 中层
  - 把 method symbol、direct call、instruction reference carrier 收口到正式 contract
  - 把 reachable static helper emission 拉通，避免 native-aot 继续停在 entry-only demo 路线
- 后续 Phase 5 应基于本台账按 capability / translation surface 批次推进，不再允许直接跳过 `AotCoreIr` 去 patch emitter。

## 6. 下一步

- 优先确认本台账作为 Phase 4 的当前真相源。
- 如果继续留在 Phase 4，再扩的也只能是“小切片补 contract + 补最小 lowering + 补验证”。
- 一旦决定切到 Phase 5，必须直接以本台账为输入做 capability batch 规划。
