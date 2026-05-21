# Loader/IR-First Canonical Generic Hard Cut Design v1.01

Date: 2026-04-21 23:02:19 +08:00
Status: drafted

## 1. 架构目标

把当前 generic 主路径从：

- `closed clone/materialization`
- `subject-id string matching`
- `per-instantiation native body`

迁移到：

- `canonical generic definition`
- `instantiation demand graph`
- `shared generic AOT IR`
- `runtime generic context`
- `thin instantiation stub`

## 2. 组件与职责

### Contracts / Naming

落点：

- `src/managed/Chaos.IL2CPP.Contracts/ManagedClosureModels.cs`
- `src/managed/Chaos.IL2CPP.Contracts/ManagedNaming.cs`
- `src/managed/Chaos.IL2CPP.Contracts/TypedIlAndAotCoreIrContracts.cs`

职责：

- 定义 canonical generic identity
- 定义 `GenericInstantiationKey`
- 定义 `GenericSpecializationKind`
- 定义 `RuntimeGenericContextArtifact`
- 定义 `SharedGenericBodyId` / `InstantiationStubId`

### Loader

落点：

- `src/managed/Chaos.IL2CPP.Loader/LoaderStage.GenericMaterialization.cs`
- `src/managed/Chaos.IL2CPP.Loader/LoaderStage.CrossAssemblyInstantiation.cs`

职责：

- 从复制闭合实例改成构建 instantiation demand graph
- 收集跨程序集 generic instantiation 需求
- 输出 open definition、canonical instantiation、specialization policy

### AOT Core IR

职责：

- 显式表达 `open definition`
- 显式表达 `shared body`
- 显式表达 `runtime generic context`
- 显式表达 `thin stub`

### Planner / Emitter / Runtime

落点：

- `src/managed/Chaos.IL2CPP.CodeGen/NativeAotLoweringPlanner.cs`
- `src/managed/Chaos.IL2CPP.CodeGen/Planning/NativeAotLoweringPlanner.InvocationPlanning.cs`

职责：

- 从字符串前缀驱动改成消费 shared-generic IR
- 基于 specialization policy 生成 shared body 与 thin stub
- 让 runtime helper 消费 `RuntimeGenericContextArtifact`

### Verification

职责：

- 建立 generic matrix
- 建立 AOT IR shape gates
- 建立 native/hotupdate proof 和 size/memory benchmark

## 3. 数据流

1. managed closure 阶段发现 open generic definitions 与 instantiation demand
2. loader 产出 canonical generic identities 与 instantiation graph
3. AOT IR 为每个可共享 family 产出：
   - shared body
   - runtime context
   - instantiation stub
4. planner/emitter 只消费 shared-generic IR，不再直接把闭合实例视为完整 body
5. runtime 通过 context/stub 执行 shared body

## 4. 错误处理与边界

- 遇到 canonical identity 歧义时必须 fail-fast，不允许 silent fallback。
- 遇到 specialization policy 未覆盖的形态时必须显式报 `not supported`，不允许偷偷回到旧 clone 路径。
- reflection / hotupdate 边界必须依赖正式 proof 判定，不接受仅靠 generated cpp review 推断正确性。

## 5. 测试与验证

必须同时覆盖：

- `tests/unit/**` 上的 contracts / parser / IR shape
- owner subject managed/native proof
- hotupdate generic boundary proof
- generated cpp size、symbol count、peak memory benchmark

验证顺序固定为：

`contracts -> loader/IR gates -> owner native proof -> hotupdate proof -> benchmark`

## 6. 切流原则

- 不保留长期 dual-path。
- legacy generic clone/materialization 只允许在最终 cutover commit 前临时存在。
- 一旦 phase 5 开始接 runtime kernel，新增 generic family 需求默认冻结，不与架构切流并行扩张。
