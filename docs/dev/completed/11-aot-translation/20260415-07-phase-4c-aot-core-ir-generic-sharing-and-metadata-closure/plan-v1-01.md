# Phase 4C AotCoreIr Generic Sharing And Metadata Closure 实现计划

> 面向执行 Agent：必须使用 `dev:executing-plans` 与 `dev:test-driven-development`；遇到失败时切到 `dev:systematic-debugging`。先做 RED 测试，再做最小实现，并持续回写 `STATUS.md` 与 `docs/dev/ACTIVE.md`。

**目标：** 把 `AotCoreIr` 从 Phase 4B 的 object/runtime surface 继续推进到正式承接 generic context、instantiation closure、sharing contract 与 metadata closure stitching 的中层 contract，并在 retained owner subject 上闭环至少一条 generic proof。

**架构：**

- 先在 `ManagedClosureContracts` 与 `AotCoreIrLowering` 中建立 generic carrier。
- 再让 `NativeAotLoweringPlanner` 消费这些 typed carrier，输出稳定 template model。
- `NativeAotEmitter` 继续只做 Scriban 渲染，不重新吸收 lowering/collection 逻辑。
- metadata closure 的采集与 stitching 需要有正式 contract，但不能把 `Chaos.TestFramework` 或测试框架私有逻辑混入 `AotCoreIr`。

**技术栈：** C#/.NET 8、`Chaos.IL2CPP.Contracts`、`Chaos.IL2CPP.CodeGen`、Scriban、`pytest`、`SolutionCorePack`、`ChaosUnitTest`

**设计文档：**

- `docs/dev/in-progress/20260414-37-hybridclr-aligned-aot-hotupdate-development-plan/roadmap-v1-02.md`
- `docs/dev/completed/20260415-06-phase-4b-aot-core-ir-object-model-and-runtime-surface/STATUS.md`
- `docs/dev/completed/20260415-05-phase-4-aot-core-ir-and-native-codegen-strengthening/aot-core-ir-codegen-gap-ledger-v1-01.md`
- `docs/dev/completed/20260415-07-phase-4c-aot-core-ir-generic-sharing-and-metadata-closure/brainstorm-v1-01.md`
- `docs/dev/completed/20260415-07-phase-4c-aot-core-ir-generic-sharing-and-metadata-closure/design-v1-01.md`

**预期知识沉淀：** 暂留在 `docs/dev`；待 generic sharing / metadata closure contract 稳定后，再评估是否补入长期 architecture 文档。

---

## 预期改动面

- `src/managed/Chaos.IL2CPP.Contracts/ManagedClosureContracts.cs`
- `src/managed/Chaos.IL2CPP.CodeGen/AotCoreIrLowering.cs`
- `src/managed/Chaos.IL2CPP.CodeGen/NativeAotLoweringPlanner.cs`
- `src/managed/Chaos.IL2CPP.CodeGen/NativeAotEmitter.cs`
- `src/managed/Chaos.IL2CPP.CodeGen/Templates/NativeAot*.scriban`
- `tests/unit/compatibility/test_phase4c_*`
- `subjects/SolutionCorePack/source/FeatureSlices/CoreRuntimeFeatures/GenericsAndCollections/*`
- `docs/dev/completed/20260415-07-phase-4c-aot-core-ir-generic-sharing-and-metadata-closure/STATUS.md`
- `docs/dev/in-progress/20260414-37-hybridclr-aligned-aot-hotupdate-development-plan/STATUS.md`

## 执行步骤

- [x] Task 1：先做 RED 测试，冻结 Phase 4C 的 typed contract 边界
  - 为 generic context / instantiation / sharing / metadata closure 建立 contract 断言
  - 明确哪些 carrier 必须是枚举或紧凑整数，避免引入新的 string protocol
  - 断言 `aot-core-ir.json` 与相关 artifact 能稳定表达 generic owner / type argument / method argument / closure role
- [x] Task 2：扩展 `ManagedClosureContracts`，补齐 Phase 4C 需要的 generic carrier
  - 引入最小 generic context / instantiation / sharing kind schema
  - 引入 metadata closure stitching 所需的 typed reference carrier
  - 不混入测试框架或 ad-hoc runtime patch 字段
- [x] Task 3：扩展 `AotCoreIrLowering`，把 generic-related references 正式 lowering 到中层
  - 优先覆盖 generic type instantiation、generic method instantiation、shared body identity
  - 明确 metadata closure 由哪些 carrier 进入 `AotCoreIr`
  - 本轮不尝试一次性承接 exception / ABI widening / 全部泛型优化
- [x] Task 4：扩展 `NativeAotLoweringPlanner`
  - 让 planner 基于新的 generic carrier 规划 template model
  - 继续坚持 planner 消费 `AotCoreIr`，不回绑 loader/world 模型
  - 用审计注释把 `method.GenericContext` / `targetReference.GenericContext` 渲染进 native-aot 输出
- [x] Task 5：扩展 `NativeAotEmitter` 与 Scriban 模板
  - 所有新增 Native AOT 输出继续走 `Templates/NativeAot*.scriban`
  - 不允许回退为 emitter 内部直接拼整段 C++
  - 若模板模型不足，优先扩展 planner model，而不是在 emitter 里写条件拼接
- [x] Task 6：在 retained owner subject 上闭环 generic proof，并做回归
  - 优先使用 `SolutionCorePack` 中已保留的 generic feature slice
  - proof 继续统一使用 `ChaosUnitTest` / `Assert`
  - 更新 `STATUS.md`、父 roadmap、索引；如仍有缺口，显式保留在 gap/status 中

## 阶段目标

- `AotCoreIr` 不再只覆盖非泛型 object/runtime surface，而是开始承接正式 generic contract。
- Native AOT 生成链继续稳定建立在 `planner + emitter + Scriban templates` 之上，并由 planner 消费 `genericContext`。
- retained owner subject 至少有一条 generic proof 能在新链路上闭环。

## 验收标准

- `ManagedClosureContracts` 或等价 contract 能稳定表达 generic context、instantiation、sharing kind、metadata closure role。
- `AotCoreIrLowering` 能把上述 typed carrier 从 managed/typed-il world lowering 到 `aot-core-ir.json`。
- `NativeAotLoweringPlanner` 能基于 Phase 4C carrier 产出稳定 template model。
- `NativeAotEmitter` 继续只通过 `.scriban` 渲染新增输出，不退回字符串拼接协议。
- 至少一条 retained owner subject generic proof 在 managed/native 路径上闭环。
- 未纳入本轮的 exception / dispatch widening / ABI widening 仍被显式记录，而不是被静默吞掉。

## 验证命令

- `python -m pytest tests/unit/compatibility/test_phase4c_aot_core_ir_generics.py -q`
- `python -m pytest tests/unit/compatibility/test_phase4c_metadata_closure_contract.py -q`
- `python -m pytest tests/unit/compatibility/test_phase4c_generic_layout_aot_core_ir.py tests/unit/compatibility/test_phase4c_generic_layout_native_aot.py tests/unit/compatibility/test_mainline_capability_batch_a.py -q`
- `python -m pytest tests/unit/compatibility/test_phase4b_aot_core_ir_object_model.py tests/unit/compatibility/test_phase4c_aot_core_ir_generics.py tests/unit/compatibility/test_phase4c_generic_layout_aot_core_ir.py tests/unit/compatibility/test_phase4c_generic_layout_native_aot.py tests/unit/compatibility/test_phase4c_metadata_closure_contract.py tests/unit/compatibility/test_metadata_supplement_bridge_subjects.py tests/unit/compatibility/test_mainline_capability_batch_a.py -q`
- `dotnet build src/managed/Chaos.IL2CPP.CodeGen/Chaos.IL2CPP.CodeGen.csproj -c Release -m:1`
- `dotnet build src/managed/Chaos.IL2CPP.Driver/Chaos.IL2CPP.Driver.csproj -c Release -m:1`

## 注意事项

- 不扩张 string protocol；新增 identity / kind / closure role 优先枚举化、紧凑化。
- `Chaos.TestFramework` 继续只作为真实 managed dependency 存在于 `src/reference/Chaos.TestFramework`，不得混入 `AotCoreIr`、planner、emitter。
- 任何 Native AOT 代码生成改动都必须沿用 `NativeAotLoweringPlanner + NativeAotEmitter + Templates/NativeAot*.scriban`。
- 不为了追求“一次做完”而吞入 exception、dispatch、ABI widening；这些属于 Phase 4D。
