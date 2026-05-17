# AOT Core IR Gap Audit And Next Expansion Design v1.01

Date: 2026-04-16 08:54:25 +08:00
Status: draft-approved-for-roadmap

## 1. 问题重述

虽然上一轮 roadmap 已经完成了 `typed-il` 的一批高价值 widening，但仓库中的真实代码边界表明，AOT core 仍未完成“可持续扩展”的状态。当前真正的问题不是单个 proof 是否能跑通，而是：

1. typed-IL 输入面仍未完全冻结，`LoaderStage` 还存在多类未支持的 opcode / handle / reference shape。
2. `AotCoreIr` 目前更像是围绕已支持能力构建的 contract，还没有针对剩余 capability family 建立统一扩展路线。
3. `NativeAotLoweringPlanner` 仍把 native AOT 约束在最小入口 ABI、有限 dispatch、有限 EH 与有限 carrier。
4. `NativeAotEmitter` 虽然已经走 Scriban，但只接受 `generic-managed-entry + int(void)` 这一类 workload。
5. 工程级验证如果没有继续固定 owner subject，就会再次出现底层 widening 与上层 evidence 脱节。

## 2. 设计目标

- 对剩余 AOT core 缺口做一次 capability-first 审计，而不是继续按零散错误推进。
- 把 roadmap 主线固定为：
  - `LoaderStage`
  - `AotCoreIr`
  - `NativeAotLoweringPlanner`
  - `NativeAotEmitter + Scriban`
  - `SolutionCorePack` owner evidence
- 保持测试框架与 AOT core 分层：
  - `Chaos.TestFramework` 只承载 proof / benchmark 发现与断言
  - 不承载 `AotCoreIr` 内部语义
- 继续压住 string 协议面，不通过新的字符串约定扩大调度面。

## 3. 硬边界与代码证据

### 3.1 Loader 输入面仍有缺口

代码锚点：`src/managed/Chaos.IL2CPP.Loader/LoaderStage.cs`

已审计到的显式边界包括：

- `unsupported managed entry point handle kind`
- `unsupported IL opcode in loader`
- `unsupported type reference opcode`
- `unsupported ldtoken handle kind in loader`
- `unsupported method reference handle kind`
- `unsupported field reference handle kind`
- `unsupported type handle kind`
- `unsupported resolution scope kind`

含义：

- 这不是单点 bug，而是 typed-IL 输入面仍未完成 capability ledger 冻结。

### 3.2 Planner 仍受最小 native AOT 假设约束

代码锚点：`src/managed/Chaos.IL2CPP.CodeGen/NativeAotLoweringPlanner.cs`

已审计到的显式边界包括：

- entry 必须 `parameterless`
- entry 必须返回 `System.Int32`
- `native-aot method ... does not support current exception region shape`
- 多处 `native-aot lowering does not support opcode`
- `does not support callvirt dispatch kind`
- `does not support ABI return carrier`
- `does not support ABI parameter carrier`
- `does not support ABI argument carrier`

含义：

- planner 还没有形成可复用的通用 entry ABI、dispatch family 与 widened carrier contract。

### 3.3 Emitter 仍未走到通用 workload ABI

代码锚点：

- `src/managed/Chaos.IL2CPP.CodeGen/CodeGenStage.cs`
- `src/managed/Chaos.IL2CPP.CodeGen/NativeAotEmitter.cs`

已审计到的显式边界包括：

- `PlanKind = "generic-managed-entry"`
- `WorkloadAbi = "int(void)"`
- emitter 仅接受上述组合

含义：

- 虽然 emitter 已迁到 Scriban，但其 plan model 仍然是最小 workload ABI 模式，而不是通用 native entry/output 层。

### 3.4 Owner evidence 应继续收敛到 SolutionCorePack

代码锚点：

- `subjects/SolutionCorePack/source/`
- `subjects/HotUpdateHostPack/source/`
- `subjects/MixedExecutionFeaturePack/source/`

审计结果：

- `SolutionCorePack` 已覆盖绝大多数纯 AOT 能力 proof / benchmark：async、generics、reflection、interop、unsafe、threading、runtime helpers、metadata。
- `HotUpdateHostPack` 与 `MixedExecutionFeaturePack` 主要承载热更与跨运行时语义，不适合作为纯 AOT capability backlog 的默认 owner。

设计结论：

- 新增纯 AOT core 证据默认进入 `SolutionCorePack`。
- 只有能力本身要求热更或混合执行语义时，才允许落到另外两个 pack。

## 4. capability family 划分

### 4.1 Family A：Loader / Typed-IL Input Closure

负责：

- 剩余 IL opcode decode
- entry point / method / field / type / `ldtoken` handle 解析
- resolution scope 与 metadata identity 完整性

验收重点：

- 新增 capability 不再在 loader 阶段硬失败。

### 4.2 Family B：AotCoreIr Semantic Surface

负责：

- 新增 typed contract 的 IR 表达
- object model、address-taken semantics、runtime helper、剩余 target reference 与 method shape widening

验收重点：

- 新增能力不再需要用 planner/emitter 临时推断 IR 缺失语义。

### 4.3 Family C：Planner / Entry ABI / Dispatch / EH

负责：

- native entry ABI 泛化
- wrapper / thunk / dispatch 规划
- 剩余 opcode legalization
- 更广的 EH shape
- 更广的 ABI carrier

验收重点：

- planner 成为 widened shape 的单一事实来源，不再依赖 fallback ABI。

### 4.4 Family D：Emitter + Scriban Shape Expansion

负责：

- 扩大 plan kind / workload ABI
- 扩大 Scriban model
- 输出 widened entry / wrapper / helper / object model 代码形状

验收重点：

- 所有新 shape 都通过 Scriban 模板落地，不回退到字符串拼接。

### 4.5 Family E：Owner Evidence And Gates

负责：

- `SolutionCorePack` proof / benchmark / dashboard / gate
- `supported / missing evidence / unsupported` 口径对齐

验收重点：

- 底层支持状态与工程级 evidence 保持同步，不再出现“底层已支持但 owner evidence 缺失”的漂移。

## 5. 设计约束

### 5.1 不混入测试框架语义

- `Chaos.TestFramework` 不允许承载 `AotCoreIr` 数据结构、planner 规则或 emitter 细节。
- proof / benchmark 只表达行为与证据。

### 5.2 不重开 string 协议面

- 不用新的 string tag 去驱动 subject 内部调度。
- capability / runtime feature / category 继续优先用枚举和强类型 API。

### 5.3 Scriban 继续作为生成唯一主线

- 任何新的 native shape widening 都必须同步落入 `NativeAotEmitter + Scriban`。
- 不允许把“这一轮先手拼 C++、下轮再迁 Scriban”作为路径。

## 6. 推荐执行策略

采用“capability family 纵向闭环”策略：

1. 先冻结 gap ledger 与 owner evidence。
2. 再按 family 逐批执行 `loader -> IR -> planner -> emitter + Scriban -> owner evidence`。
3. 每一批都要求：
   - 明确 code anchors
   - 明确 owner subject
   - 明确 proof / benchmark / gate
   - 明确 deferred 边界

## 7. 推荐结论

立即创建新的 follow-up roadmap，并从 `Phase 0: Gap Ledger And Owner Evidence Audit` 开始执行。这个阶段的目标不是改代码，而是把剩余主线一次性切清：

- 哪些是真实 capability family
- 哪些属于 loader / IR / planner / emitter / evidence
- 哪些应该进入 `SolutionCorePack`
- 哪些继续 deferred
