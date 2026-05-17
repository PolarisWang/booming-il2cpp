# AOT Core IR Gap Audit And Next Expansion Brainstorm v1.01

Date: 2026-04-16 08:54:25 +08:00
Status: approved-for-roadmap

## 1. 背景

上一轮 roadmap 已经收口了 `byref`、widened method shape、entry ABI normalization 与一批 mixed dispatch / Scriban widening，但这不等于 AOT core 全量完成。新的主线需要回答的问题是：

- 剩余 `AotCoreIr` / planner / emitter 缺口到底还有哪些真实能力族？
- 这些缺口该如何分批，而不是继续按零散 `NotSupportedException` 补洞？
- 工程级验证应该由哪个 subject 承担 owner 责任？

## 2. 真实审计证据

### 2.1 Loader 仍有显式边界

来自 `src/managed/Chaos.IL2CPP.Loader/LoaderStage.cs` 的审计结果显示，当前 loader 仍有多个显式不支持点：

- `unsupported managed entry point handle kind`
- `unsupported IL opcode in loader`
- `unsupported type reference opcode`
- `unsupported ldtoken handle kind in loader`
- `unsupported method reference handle kind`
- `unsupported field reference handle kind`
- `unsupported type handle kind`
- `unsupported resolution scope kind`

这说明 typed-IL 输入面还没有完全冻结。

### 2.2 Planner 仍把 native AOT 路径限定在最小集合

来自 `src/managed/Chaos.IL2CPP.CodeGen/NativeAotLoweringPlanner.cs` 的审计结果显示，当前 planner 仍明确限制：

- entry 必须 `parameterless` 且返回 `System.Int32`
- 仅支持当前有限的 exception region shape
- 有一组 `does not support opcode`
- 有一组 `does not support callvirt dispatch kind`
- ABI return / parameter / argument carrier 仍有明确 unsupported 分支

### 2.3 Emitter 仍不是通用 entry ABI

来自 `src/managed/Chaos.IL2CPP.CodeGen/NativeAotEmitter.cs` 与 `CodeGenStage.cs` 的审计结果显示：

- `PlanKind` 仍固定为 `generic-managed-entry`
- `WorkloadAbi` 仍固定为 `int(void)`

这意味着 emitter 还没有成长为通用 workload / entry ABI 输出层。

### 2.4 Owner subject 需要继续收敛

现有 subject 扫描表明：

- `subjects/SolutionCorePack` 已经覆盖 async、generics、reflection、interop、unsafe、threading、runtime helpers 等纯 AOT 核心能力 proof/benchmark。
- `subjects/HotUpdateHostPack` 面向热更。
- `subjects/MixedExecutionFeaturePack` 面向混合执行。

因此纯 AOT core 主线不应再扩散到多个 subject，而应继续以 `SolutionCorePack` 为 owner。

## 3. 方案比较

### 方案 A：按 opcode / 异常消息建立 backlog

- 做法：直接把 `LoaderStage`、`NativeAotLoweringPlanner`、`NativeAotEmitter` 的 `unsupported` 分支逐条抄成待办。
- 优点：初始整理快。
- 问题：会把同一个 capability family 切碎到不同文件和不同阶段，后续仍然是补丁式推进。

### 方案 B：按 subject / proof 缺口反推底层 backlog

- 做法：先看 `SolutionCorePack` 哪些 proof/benchmark 缺，再从失败反推 loader / IR / planner / emitter 要改什么。
- 优点：工程级目标直观。
- 问题：容易为了个别 proof 通过，在底层塞入临时特判；也会把 AOT core 与测试证据边界混在一起。

### 方案 C：按 capability family 纵向闭环推进

- 做法：先冻结 capability-first gap ledger，再按 `loader -> AotCoreIr -> planner -> emitter + Scriban -> owner evidence` 切成阶段 child task。
- 优点：
  - 能同时约束底层 contract 和 owner evidence。
  - 能把 `SolutionCorePack` 作为纯 AOT owner subject 固定下来。
  - 最符合此前两轮 roadmap 的执行方式。
- 问题：前期文档整理成本略高。

## 4. 结论

- 采用方案 C。
- 当前任务应进入新的 roadmap，而不是直接写单次 plan。
- 新 roadmap 的第一个 child task 必须是 `Phase 0: Gap Ledger And Owner Evidence Audit`，先冻结：
  - capability family
  - stage impact
  - owner subject
  - proof / benchmark / gate 证据要求
  - deferred 原因

## 5. 推荐阶段方向

- Phase 0: gap ledger + owner evidence audit
- Phase 1: loader / typed-IL 输入面 closure
- Phase 2: `AotCoreIr` 语义面 widening
- Phase 3: planner / entry ABI / dispatch / EH generalization
- Phase 4: `NativeAotEmitter + Scriban` shape widening
- Phase 5: `SolutionCorePack` owner proof / benchmark 收口
- Phase 6: engineering gate 与 full-solution 验证

## 6. 分流判断

本任务需要：

- 多阶段推进
- 多个 child task
- 跨多次会话持续推进

因此必须进入 `roadmap`，不适合直接进入单次 `plan`。
