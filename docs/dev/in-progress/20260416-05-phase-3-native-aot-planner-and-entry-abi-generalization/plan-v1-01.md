# Phase 3 NativeAot Planner And Entry ABI Generalization 实现计划 v1.01

Date: 2026-04-16 10:36:50 +08:00
Status: in-progress

## 计划清单

- [x] 在 `SolutionCorePack` owner benchmark / proof 中构造真实 `ldc.i8 + and/or/xor` surface
- [x] 先写 RED compatibility 测试，冻结 `ldc.i8 + and/or/xor` failing surface
- [x] 最小扩展 `NativeAotLoweringPlanner` 对 `ldc.i8` 与 bitwise opcode 的 legalization
- [x] 在 `SolutionCorePack` owner benchmark 中构造真实 `shl / shr / shr.un` surface
- [x] 先写 RED compatibility 测试，冻结 `shl / shr / shr.un` failing surface
- [x] 最小扩展 `NativeAotLoweringPlanner` 对 `shl / shr / shr.un` 的 legalization
- [x] 用现有 proof 冻结 `not` surface，并补齐对应 planner legalization
- [x] 用现有 proof 冻结 branch comparison surface，并补齐 `beq / bgt / ble / bge / bge.un`
- [x] 为 branch owner proof 回补 `LoaderStage` 的 `Bge_un / Bge_un_s` decode 缺口
- [x] 运行局部与相邻 compatibility 回归
- [x] 继续审计下一批 owner-driven blocker
- [x] 回写父 roadmap / 子任务状态与进展记录

## 本轮新增收口

- 新增 `tests/unit/compatibility/test_phase4_generic_constraint_dispatch_native_aot.py`
- 修复 generic placeholder instantiation 在 loader 中被错误折叠回 open definition 的问题
- 为 planner 增补 `ldarga` 的最小 lowering，并在调用点标准化 inbound native-int 参数
- 为 linker 增补 interface dispatch implementation retention，使 reachable slot 能带出 concrete implementation
- 纯化 `GenericConstraintProof`，移除字符串插值噪音
- 将聚合 `ProofEntry::Run()` 的 owner audit 推进到 async external-runtime helper family

## 涉及文件

- `src/managed/Chaos.IL2CPP.Loader/Metadata/TypeProviders.cs`
- `src/managed/Chaos.IL2CPP.CodeGen/NativeAotLoweringPlanner.cs`
- `src/managed/Chaos.IL2CPP.Linker/LinkerStage.cs`
- `subjects/SolutionCorePack/source/FeatureSlices/CoreRuntimeFeatures/GenericsAndCollections/GenericConstraintProof.cs`
- `tests/unit/compatibility/test_phase4_generic_constraint_dispatch_native_aot.py`

## 预期验证

- focused compatibility 测试能够冻结并验证：
  - `ldc.i8 + and/or/xor`
  - `shl / shr / shr.un`
  - `not`
  - `beq / bgt / ble / bge / bge.un`
  - generic constraint interface dispatch closure
- 相邻 compatibility 基线保持绿色
- `ProofEntry::Run()` 的 aggregate owner audit 能继续暴露下一层真实 blocker，而不是停留在 generic dispatch 噪音
