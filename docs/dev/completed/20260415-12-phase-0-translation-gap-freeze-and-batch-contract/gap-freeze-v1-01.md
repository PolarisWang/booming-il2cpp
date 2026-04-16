# Phase 7 Translation Gap Freeze v1.01

Date: 2026-04-15 20:32:24 +08:00
Status: draft

## 1. 目的

把当前 `typed-il -> AotCoreIr -> NativeAotLoweringPlanner -> NativeAotEmitter + Scriban` 的真实缺口冻结成后续 Phase 1-5 的执行台账。

本台账延续以下 Phase 0 冻结物：

- capability taxonomy
- stable identity
- body availability
- unsupported reason code

本台账不重定义这些基础契约，只增加：

- gap group
- stage tags
- child-task mapping
- 优先级 / deferred 理由

## 2. 分批原则

1. 先按 capability-first 分批，再标注源码层 stage tag。
2. 先处理真正阻塞 typed-il / AOT Core IR 闭环的缺口，不先做展示层或报告层工作。
3. 不把 `Bridge` / `ExternalRuntime` dispatch、structured EH、entry ABI widening 混成同一批。
4. 任何缺口如果已经能在底层测试体现，但还没有 owner subject 证据，归 Phase 5。

## 3. 当前 gap inventory

| gap group | stage tags | 代码锚点 | 真实缺口 | 受影响 capability / owner | 对应 child task | 优先级 | deferred / 说明 | 现有测试锚点 |
| --- | --- | --- | --- | --- | --- | --- | --- | --- |
| Loader.EntryAndHandleKinds | `loader` `metadata` | `LoaderStage.cs:206,938,1456,1475,1625` | managed entry handle kind、`ldtoken` handle kind、method/field handle kind、method specification target kind 仍有硬失败 | `SolutionCorePack` 下的 reflection / metadata / dispatch / generic instantiation | `20260415-13` | P0 | Phase 1 必须先收口，否则 typed-il 输入面不稳定 | `test_phase4_aot_core_ir_contract.py`, `test_mainline_capability_batch_b.py` |
| Loader.OpcodeDecodeCoverage | `loader` | `LoaderStage.cs:748,778,795,1022,1117,1408` | loader 仍保留 opcode/type/method/field/exception region fallback failure；现有支持面与 interpreter 侧 opcode 覆盖不一致 | `SolutionCorePack` exceptions/control-flow/object-model | `20260415-13` | P0 | 先补 decode，不在本阶段要求 native emission | `test_loader_switch_opcode.py`, `test_mainline_capability_batch_a.py`, `test_mainline_capability_batch_b.py` |
| Loader.FunctionPointerAndSignatureShape | `loader` `metadata` | `TypeProviders.cs:30,141` | function pointer signature 仍直接拒绝；后续 delegate / metadata / interop widening 会被前置阻断 | `SolutionCorePack` delegate / reflection / interop | `20260415-13` | P0 | Phase 1 先完成 decode + identity；native 发射延后到 Phase 4 | `test_mainline_capability_batch_b.py` |
| CoreIr.ExceptionContractBoundary | `aot-core-ir` | `AotCoreIrLowering.cs:161` | AOT Core IR 虽已定义 catch/finally/fault/filter 枚举，但实际 contract 仍偏保守；后续 EH widening 需要单独冻结 | `SolutionCorePack` exception family | `20260415-15` | P1 | 不是 typed-il 输入阻塞，但会阻塞 structured EH 闭环 | `test_phase4d_exception_aot_core_ir.py`, `test_nested_exception_proof_subject.py` |
| Planner.DispatchKindsAndTargetMetadata | `planner` `dispatch` | `NativeAotLoweringPlanner.cs:250,703,759,1732,1743,1754` | unresolved dispatch slot、`callvirt` 仅支持 `None/Direct/Virtual`、缺 target symbol / parameter metadata / target reference metadata 时硬失败 | `SolutionCorePack` dispatch / `MixedExecutionFeaturePack` bridge dispatch | `20260415-16` | P1 | `Bridge` / `ExternalRuntime` 需要和 planner/emitter contract 一起做，不能只在 loader 层补 | `test_phase4d_dispatch_native_aot.py`, `test_phase4d_dispatch_aot_core_ir.py` |
| Planner.EntryAbiAndLegacyFallback | `planner` `abi` | `NativeAotLoweringPlanner.cs:366,372,1838,1866` | entry 仍固定为 `int(void)` 且要求 `Int32` return；legacy call target ABI 仍偏最小化 | `SolutionCorePack` benchmark/native entry、工程级 workload | `20260415-16` | P1 | Phase 4 统一处理 entry ABI widening 与 emitter/template 消费 | `test_native_aot_workload_entry_bundle.py` |
| Planner.ScalarOpcodeCoverage | `planner` `abi` | `NativeAotLoweringPlanner.cs:655` | planner opcode switch 仍存在统一 fallback；现阶段 numeric/branch/conversion 仍偏 `Int32` 最小集 | `SolutionCorePack` arithmetic / compare / conversion | `20260415-14` | P0 | Phase 2 的主批次 | `test_phase4_aot_core_ir_contract.py`, `test_mainline_capability_batch_a.py` |
| Planner.ObjectModelReferenceKindGuards | `planner` `object-model` | `NativeAotLoweringPlanner.cs:905,927,983,1013,1064,1114,1157,1286,1322,1347,1382,1416,1454` | object model / array / field / boxing / value-type 相关 lowering 仍依赖严格的 target reference kind / metadata 形状；后续 byref/value-type widening 会继续撞墙 | `SolutionCorePack` object-model / generics / metadata | `20260415-14` | P1 | 先随 carrier widening 一起做，不单独切一阶段 | `test_phase4b_*` |
| Planner.StructuredEhAndControlFlow | `planner` `eh` `control-flow` | `NativeAotLoweringPlanner.cs:390,1664-1680` | method 仍要求 catch-only EH shape；try/handler 内部出现 `br/blt/bne.un/leave/ret` 视为不支持 | `SolutionCorePack` exceptions/control-flow | `20260415-15` | P0 | Phase 3 的主批次 | `test_phase4d_exception_native_aot.py`, `test_nested_exception_proof_subject.py` |
| Planner.AbiCarrierBoundary | `planner` `abi` | `NativeAotLoweringPlanner.cs:1762-1989` | ABI carrier 仍只稳定支持 `Void` / `Int32` / `NativeInt` / `ValueTypeByValue`；parameter/return mapping、argument formatting、value-type metadata 仍偏窄 | `SolutionCorePack` conversion / interop / value-type / low-level | `20260415-14` | P0 | Phase 2 的主批次；是否引入新 carrier 取决于 child task 设计 | `test_phase4d_abi_widening_aot_core_ir.py`, `test_phase4d_abi_widening_native_aot.py` |
| Emitter.PlanKindAndWorkloadAbiBoundary | `emitter` `templates` | `NativeAotEmitter.cs:87-94` | emitter 当前只接受 `generic-managed-entry` 和 `int(void)` workload ABI；widened planner model 还没有新的 emitter/template contract | `SolutionCorePack` benchmark / engineering archetype | `20260415-16` | P1 | 必须与 planner widening 同步推进 | `test_native_aot_workload_entry_bundle.py` |
| Emitter.TemplateExpansionSurface | `emitter` `templates` | `NativeAotEmitter.cs`, `Templates/NativeAot*.scriban` | 当前模板面只有 translation unit/object model/method 三段，后续 widened entry ABI / helper / dispatch / EH 需要显式设计模板模型 | `SolutionCorePack` native AOT codegen | `20260415-16` | P1 | 坚持 Scriban，不回退到大段字符串拼接 | `test_phase4*_native_aot.py` |
| OwnerEvidenceAndGateClosure | `subject` `benchmark` `reporting` | owner subjects + dashboard/reporting pipeline | 新增底层能力如果没有 proof / benchmark / gate 收口，会再次出现底层支持但外层缺证据 | `SolutionCorePack`, `HotUpdateHostPack`, `MixedExecutionFeaturePack` | `20260415-17` | P2 | 只有当前四个底层阶段完成后才推进 | `test_benchmark_dashboard_generator.py`, `test_subject_reporting.py`, `test_hot_update_productization_gates.py` |

## 4. Child-task batching 结论

### `20260415-13-phase-1-loader-and-metadata-decode-expansion`

- 接收：
  - `Loader.EntryAndHandleKinds`
  - `Loader.OpcodeDecodeCoverage`
  - `Loader.FunctionPointerAndSignatureShape`
- 退出条件：
  - 目标 capability 不再卡在 loader / metadata decode
  - typed-il 输入面能稳定带出需要的 reference / signature / handle metadata

### `20260415-14-phase-2-scalar-conversion-and-abi-carrier-widening`

- 接收：
  - `Planner.ScalarOpcodeCoverage`
  - `Planner.AbiCarrierBoundary`
  - `Planner.ObjectModelReferenceKindGuards`
- 退出条件：
  - widened scalar / conversion / compare / ABI carrier 在 `AotCoreIr` 与 planner 里一致存在

### `20260415-15-phase-3-structured-control-flow-and-exception-expansion`

- 接收：
  - `CoreIr.ExceptionContractBoundary`
  - `Planner.StructuredEhAndControlFlow`
- 退出条件：
  - 不再受限于 catch-only EH shape
  - `switch` / `leave` / finally / fault / filter 的目标批次闭环

### `20260415-16-phase-4-native-aot-planner-entry-abi-and-scriban-widening`

- 接收：
  - `Planner.DispatchKindsAndTargetMetadata`
  - `Planner.EntryAbiAndLegacyFallback`
  - `Emitter.PlanKindAndWorkloadAbiBoundary`
  - `Emitter.TemplateExpansionSurface`
- 退出条件：
  - widened planner model 能被 emitter + Scriban 真正消费
  - dispatch / entry ABI / workload ABI 边界不再依赖最小 fallback

### `20260415-17-phase-5-owner-subject-benchmark-and-gate-closure`

- 接收：
  - `OwnerEvidenceAndGateClosure`
- 退出条件：
  - 新支持面有 owner subject proof / benchmark / gate evidence

## 5. Deferred 说明

- `HybridDispatchKind.Bridge` / `ExternalRuntime` 不在 Phase 1 解决。
  - 原因：它们不是 typed-il decode 问题，而是 planner / emitter / runtime boundary 问题。
- widened entry ABI 不在 Phase 2 解决。
  - 原因：单独只做 carrier widening 会导致 emitter/template 再次失配。
- owner subject / benchmark / gate 不在底层阶段混入处理。
  - 原因：必须先让底层 contract 稳定，再统一做 evidence 收口。

## 6. 结论

本台账已经足以支撑父 roadmap 继续推进：

- Phase 0 可视为完成
- Phase 1 下一步直接进入 loader / metadata decode expansion
