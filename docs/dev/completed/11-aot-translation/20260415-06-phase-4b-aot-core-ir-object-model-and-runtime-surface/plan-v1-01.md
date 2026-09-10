# Phase 4B AotCoreIr Object Model And Runtime Surface 实现计划

> 面向执行 Agent：必须使用 `dev:executing-plans`、`dev:test-driven-development`；遇到失败时切到 `dev:systematic-debugging`。先补 RED 测试，再做最小实现，并持续回写 `STATUS.md` 与 `docs/dev/ACTIVE.md`。

**目标：** 把 `AotCoreIr` 从最小 `call + int32` 闭环推进到带对象模型与 runtime surface 的 Native AOT 中层，并在本轮内把 Native AOT 生成链重构为 `NativeAotLoweringPlanner + Scriban emitter` 双阶段。

**架构：** 先在 `ManagedClosureContracts` 与 `AotCoreIrLowering` 中补齐 object/runtime carrier，再把 Native AOT 输出从单体 `NativeAotEmitter` 重构为 planner 负责 template model、emitter 负责 Scriban 渲染的双阶段结构，最后在新生成链上继续关闭 array covariance 等剩余 4B 切片。

**技术栈：** C#/.NET 8、`Chaos.IL2CPP.Contracts`、`Chaos.IL2CPP.CodeGen`、Scriban、`pytest`、`SolutionCorePack`、`ChaosUnitTest`

**设计文档：**

- `docs/dev/in-progress/20260414-37-hybridclr-aligned-aot-hotupdate-development-plan/roadmap-v1-02.md`
- `docs/dev/in-progress/20260414-37-hybridclr-aligned-aot-hotupdate-development-plan/review-v1-02.md`
- `docs/dev/completed/20260415-05-phase-4-aot-core-ir-and-native-codegen-strengthening/aot-core-ir-codegen-gap-ledger-v1-01.md`
- `docs/dev/completed/20260415-05-phase-4-aot-core-ir-and-native-codegen-strengthening/STATUS.md`
- `docs/dev/in-progress/20260415-06-phase-4b-aot-core-ir-object-model-and-runtime-surface/brainstorm-v1-01.md`
- `docs/dev/in-progress/20260415-06-phase-4b-aot-core-ir-object-model-and-runtime-surface/design-v1-01.md`

**预期知识沉淀：** 暂留在 `docs/dev`；若 planner/emitter 边界稳定，再评估补入长期 architecture 文档。

---

## 预期改动面

- `src/managed/Chaos.IL2CPP.Contracts/ManagedClosureContracts.cs`
- `src/managed/Chaos.IL2CPP.CodeGen/AotCoreIrLowering.cs`
- `src/managed/Chaos.IL2CPP.CodeGen/NativeAotLoweringPlanner.cs`
- `src/managed/Chaos.IL2CPP.CodeGen/NativeAotEmitter.cs`
- 视需要新增 `NativeAotTemplateModels.cs` / `NativeAotTemplateRenderer.cs`
- `src/managed/Chaos.IL2CPP.CodeGen/Templates/NativeAot*.scriban`
- `tests/unit/compatibility/test_phase4b_aot_core_ir_object_model.py`
- `tests/unit/compatibility/test_phase4b_*_native_aot.py`
- `subjects/SolutionCorePack/source/FeatureSlices/CoreRuntimeFeatures/ObjectModelAndDispatch/*`
- `subjects/SolutionCorePack/source/FeatureSlices/CoreRuntimeFeatures/PrimitivesAndOps/*`
- `docs/dev/in-progress/20260415-06-phase-4b-aot-core-ir-object-model-and-runtime-surface/STATUS.md`
- `docs/dev/in-progress/20260414-37-hybridclr-aligned-aot-hotupdate-development-plan/STATUS.md`

## 执行步骤

- [x] Task 1：先补 RED tests，锁定 `AotCoreIr` object/runtime carrier 的 contract 边界
  - 为 `newobj / ldfld / stfld / ldsfld / stsfld / newarr / castclass / isinst / box / unbox` 建立 contract 断言
  - 明确哪些字段必须是枚举或紧凑数值，不继续扩 string 协议面
  - 验证 `aot-core-ir.json` 能稳定携带 type / field / runtime-service 级别的引用信息

- [x] Task 2：扩展 `ManagedClosureContracts`，补齐 object model / runtime-service schema
  - 引入最小枚举化 carrier，避免继续依赖裸字符串区分引用类别
  - 保持 schema 贴近后续 4C / 4D 可复用方向
  - 保持对旧 contract 的最小兼容迁移

- [ ] Task 3：扩展 `AotCoreIrLowering`，把 object/runtime reference 正式 lowering 到中层
  - 优先打通 `newobj / field / array / cast / box-unbox`
  - 当前轮补齐 array 相关 carrier，支撑 array covariance
  - 不在本轮引入完整 generic closure、EH closure、完整对象布局

- [ ] Task 4：新增 `NativeAotLoweringPlanner`
  - 从 `NativeAotEmitter` 中提炼 reachable method 收集、object model 归一、layout 整理、helper block 规划
  - 输出 `NativeAotTemplateModel`
  - planner 继续只消费 `AotCoreIr`，不回绑 loader / linked world

- [ ] Task 5：把 `NativeAotEmitter` 收窄为纯 Scriban 渲染器
  - 显式加载 `Templates/NativeAot*.scriban`
  - translation unit 顶层、object model、method 框架切到模板
  - 模板缺失或解析失败显式报错

- [ ] Task 6：在新生成链上继续推进 owner proof 与剩余 4B 语义
  - 优先完成 array covariance
  - proof 统一使用 `ChaosUnitTest` / `Assert`
  - 不引入新的临时 subject

- [ ] Task 7：回归验证并回写文档状态
  - 更新本子任务 `STATUS.md`
  - 回写父 roadmap `STATUS.md`
  - 如仍有明确缺口，补入 gap ledger 或本任务状态，不允许静默跳过

## 阶段目标

- `AotCoreIr` 不再只是 `call/int32` 中转壳，而是承接最小对象模型 carrier。
- Native AOT 生成链正式切换为 `planner + Scriban emitter` 双阶段。
- `SolutionCorePack` 至少有一条 object/array/boxing/cast proof 能通过 managed/native 验证，并继续扩展到 array covariance。

## 验收标准

- `AotCoreIrInstructionArtifact` 或等价 contract 能稳定表达 type reference、field reference、runtime-service kind，以及当前轮需要的 array carrier。
- `AotCoreIrLowering` 能从 managed/typed-il reference 产出上述 carrier，而不是把 emitter 重新绑回 loader model。
- `NativeAotLoweringPlanner` 已成为 Native AOT 输出链的语义规划入口。
- `NativeAotEmitter` 显式使用 `.scriban` 模板，不再直接拼整份 translation unit。
- 当前选定 proof 不再因为 `newobj / field / array / castclass / isinst / box / unbox` 直接报 unsupported。
- owner subject proof 使用 `ChaosUnitTest`/`Assert` 闭环，不依赖 `Console.WriteLine` 外判。
- 未纳入本轮的 generic/sharing、EH、dispatch widening、非 `int32` ABI 仍被显式记录。

## 验证命令

- `python -m pytest tests/unit/compatibility/test_phase4b_aot_core_ir_object_model.py -q`
- `python -m pytest tests/unit/compatibility/test_phase4b_object_field_native_aot.py tests/unit/compatibility/test_phase4b_array_boxing_native_aot.py tests/unit/compatibility/test_phase4b_array_ops_native_aot.py tests/unit/compatibility/test_phase4b_cast_type_check_native_aot.py tests/unit/compatibility/test_phase4b_boxed_struct_field_read_native_aot.py tests/unit/compatibility/test_phase4b_object_return_native_aot.py -q`
- `python -m pytest tests/unit/compatibility/test_phase4b_array_covariance_native_aot.py -q`
- `dotnet build src/managed/Chaos.IL2CPP.CodeGen/Chaos.IL2CPP.CodeGen.csproj -c Release -m:1`
- `dotnet build src/managed/Chaos.IL2CPP.Driver/Chaos.IL2CPP.Driver.csproj -c Release -m:1`

## 注意事项

- 不重新扩大 string 协议面；新增 identity / kind / dispatch 面优先枚举化、紧凑化。
- 不为 Phase 4B 引入新的临时 subject；proof 回收到既有 `SolutionCorePack`。
- 不把 ad-hoc emitter patch 当成功能完成；必须先走 contract，再走 lowering，再走 planner/emitter，再走 owner proof。
- 当前轮 `NativeAotEmitter` 的 Scriban 化与 planner 化是强制目标，不再后置。
- 不为了追求“一次做完”而吞下完整 GC / layout / ABI；本轮只关闭最小对象模型切片并完成生成链结构升级。
