# Phase 7 AOT Core IR Typed-IL Translation Expansion Design v1.01

Date: 2026-04-15 20:11:23 +08:00
Status: draft-approved-for-roadmap

## 1. 问题重述

当前仓库已经具备：

- Phase 0 冻结过的 capability taxonomy、stable identity、body availability、translation ledger、unsupported ledger。
- 一条能跑通最小可用闭环的 `typed-il -> AotCoreIr -> NativeAotLoweringPlanner -> NativeAotEmitter + Scriban` 路径。
- 基于 owner subject 的 proof / benchmark / dashboard / release evidence 基线。

但真实实现仍存在三个结构性问题：

1. Loader decode contract 仍不完整，尤其在 function pointer、metadata handle kind、部分 type/method/field signature shape 上仍有硬失败。
2. `AotCoreIr` 与 planner 当前仍偏向狭窄 carrier 集合，很多 widening 还停留在 `Int32` / `NativeInt`、catch-only exception shape、最小入口 ABI。
3. widening 的优先级尚未被正式冻结为下一轮 child tasks，继续直接改代码容易重新滑回“哪里报错补哪里”的 patchwork。

## 2. 设计目标

- 把下一轮 typed-IL translation 扩容明确拆成可执行 child tasks。
- 所有新支持点都沿用 Phase 0 的 capability / owner / body availability / unsupported reason 体系。
- 保持严格分层：
  - Loader 负责 metadata + IL decode；
  - `AotCoreIrLowering` 负责 IR contract 组装；
  - `NativeAotLoweringPlanner` 负责 lowering / ABI / legalization；
  - `NativeAotEmitter` 负责消费 planner model 并通过 Scriban 模板生成；
  - `Chaos.TestFramework` 与 subject 只承担声明和验证，不进入 AOT core pipeline。

## 3. 非目标

- 不在本轮把整个 `typed-il` / `AotCoreIr` opcode 表示完全改成 enum。
- 不在本轮重新设计 subject 目录或测试框架主架构。
- 不在本轮处理 mobile host、engine binding 或额外产品化问题。
- 不承诺一次性补齐所有 C# feature。

## 4. 核心设计

### 4.1 执行单位

本轮的最小执行单位不是“单个文件”也不是“单个 subject”，而是：

`capability batch x pipeline stage`

每个 batch 必须同时描述：

- capability family / item
- owner subject
- source anchor
- 受影响的 pipeline stage
- 目标 body availability
- proof required / benchmark required
- 当前 support state 与 reason code

这意味着 child task 不会只写成“补几个 opcode”，而会写成例如：

- “有符号/无符号比较与 conversion widening”
- “structured exception control flow widening”
- “byref / value-type / ABI carrier widening”

### 4.2 分层职责

#### Loader

负责：

- opcode decode
- metadata handle resolution
- type / method / field reference shape decode
- generic context 与 signature shape 的可消费表示

不负责：

- AOT 专用 lowering 决策
- test framework / subject metadata

#### AotCoreIrLowering

负责：

- 把 typed-il 与 linked world 组装为 `AotCoreIrArtifact`
- 解析 target reference、runtime service kind、ABI slot
- 把 managed exception region 转换为 AOT Core IR 侧的 typed contract

不负责：

- native C++ 输出格式
- benchmark / proof 发现逻辑

#### NativeAotLoweringPlanner

负责：

- opcode legalization
- ABI carrier mapping
- runtime helper / object model / dispatch 方案选择
- native invocation / entry / return / byref 规划

不负责：

- 直接拼出最终 `.cpp`
- 测试声明或 reporting 逻辑

#### NativeAotEmitter + Scriban

负责：

- 读取 lowering plan / `AotCoreIrArtifact`
- 构造模板模型
- 统一走 `Templates/NativeAot*.scriban` 输出代码

要求：

- 新增 widening 不允许再用新的大段内联 C++ 拼接路径替代模板。
- 允许 emitter 做模板模型装配，但真正的 native 结构输出继续由 Scriban 模板承载。

#### Subjects / Chaos.TestFramework

负责：

- 用 `ChaosUnitTest` / `ChaosBenchmark` 声明 capability 证据
- 用 `Assert` 做 proof 判定
- 给 benchmark / dashboard / release gate 提供真实 evidence

不负责：

- 承担任何 AOT Core IR 逻辑
- 成为底层 pipeline 的专用分支条件

## 5. 建议的批次模型

### 批次 A：Loader 与 metadata decode widening

聚焦：

- function pointer signature shape
- 额外 metadata handle kind / method specification / field reference 解析收口
- pointer / byref / array / generic instantiation 相关 type shape 的 decode 补齐

完成后应保证：

- 相关 capability 不再在 loader 阶段直接失败。
- `typed-il` 至少能拿到完整、可消费的 reference / signature 信息。

### 批次 B：Scalar / conversion / comparison / carrier widening

聚焦：

- 有符号与无符号比较族
- `conv.*` / overflow conversion 家族继续扩展
- `bool` / `char` / `byte` / `short` / `uint` / `long` / `ulong` / `nint` / `nuint` 等 carrier widening

完成后应保证：

- planner 不再默认把大部分值都压缩成 `Int32` 语义。
- `AotCoreIr` 与 ABI slot 能准确描述 widened carriers。

### 批次 C：Byref / value-type / dispatch / metadata runtime-service widening

聚焦：

- byref / value-type 传参与返回
- `ldtoken`、runtime handle、metadata carrier
- delegate / function pointer / generic dispatch 相关的 typed contract 继续闭环

完成后应保证：

- planner 能消费更复杂的 target reference / ABI carrier。
- owner subject 中与 metadata / dispatch 相关的 proof 能走到 Native AOT。

### 批次 D：Structured control flow 与异常形状 widening

聚焦：

- `switch`
- `leave`
- finally / fault / filter region shape
- 非 catch-only 的 EH lowering 与 codegen

完成后应保证：

- `AotCoreIrLowering` 与 planner 对 EH 的处理不再只允许最小 catch-only shape。
- 与 exception / control-flow 对齐的 proof 能真实闭环。

### 批次 E：NativeAOT 入口、ABI、模板与工程证据收口

聚焦：

- widened entry / invocation ABI
- planner model 与 Scriban 模板扩展
- owner subject proof / benchmark / unsupported report / release evidence 对新支持面收口

完成后应保证：

- 新 capability batch 不只在单元测试里成立，也能体现在 subject evidence 与工程 gate 里。

## 6. 测试与验收模型

每个 child task 都必须同时补至少两层验证：

### 底层验证

- Loader / AOT Core IR / planner / emitter 的 unit tests
- 必要时补 integration tests，验证 artifact 串联

### 证据验证

- owner subject proof
- 高价值路径 benchmark
- 若暂不要求 benchmark，必须在 capability state 或 roadmap 中显式说明

### 阶段验收

每个阶段退出前必须回答：

- 这批 capability 的 loader decode 是否稳定？
- `AotCoreIr` contract 是否已经覆盖到对应 carrier / control-flow / metadata shape？
- planner / emitter / Scriban 是否真正消费了这些信息？
- subject 证据是否已能展示“supported”，而不是仍停留在 `missing evidence`？

## 7. 风险控制

- 对 widening 做 capability-first batching，避免散点式 patch。
- 对新增 native 生成面继续强制 `planner -> emitter -> Scriban templates`，避免回退到老式 emitter 拼接。
- 对测试框架继续严格隔离，确保 `Chaos.TestFramework` 只在 subject / reference 层演化。
- 对 unsupported / missing evidence 持续区分，避免 dashboard 再次把“未生成数据”误显示为“Unsupported”。

## 8. 推荐结论

下一步不直接改实现，而是先执行 Phase 0 child task：

- 冻结“当前真实未支持的 typed-IL translation gap”
- 明确 batch 边界
- 把每一批 batch 映射到可执行 child task

这样后续每一轮执行都能对齐到同一份台账，不会重新发散。
