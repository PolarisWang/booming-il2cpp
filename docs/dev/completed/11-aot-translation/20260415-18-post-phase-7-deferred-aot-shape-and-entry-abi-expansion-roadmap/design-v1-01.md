# Post Phase 7 Deferred AOT Shape And Entry ABI Expansion Design v1.01

Date: 2026-04-15 23:52:48 +08:00
Status: draft-approved-for-roadmap

## 1. 问题重述

上一轮 roadmap 已经把一组高价值 typed-IL surface 打通，但 deferred surface 仍然没有统一 contract。当前最核心的结构性问题有四个：

1. `byref` 仍然只是零散可用，没有被冻结成统一的 loader / IR / planner / emitter contract。
2. widened method shape 仍偏向最小方法签名，复杂 entry 形状、value-type receiver、wrapper/thunk 规则还没形成正式边界。
3. entry ABI normalization 仍存在 legacy fallback 和隐式约定，尚未形成单一路径。
4. `generic + byref + value-type` 混合路径没有被系统化验证，容易在某层支持、下一层失效。

## 2. 设计目标

- 把 deferred surface 机械整理成可执行的 capability family 与 child task。
- 保持单向链路：
  - `typed-il -> AotCoreIr -> NativeAotLoweringPlanner -> NativeAotEmitter + Scriban`
- 明确分层职责，禁止测试框架倒灌到 AOT core。
- 把新 support state 收口到 owner subject / proof / benchmark / gate，但不让这些证据定义底层 contract。

## 3. 设计边界

### 3.1 本轮负责

- 冻结 `byref / widened method shape / entry ABI / mixed dispatch / Scriban widening` 的台账。
- 为每一类 deferred surface 建立顺序 child task。
- 定义每阶段需要的验证与验收标准。

### 3.2 本轮不负责

- 重新设计 `Chaos.TestFramework` API。
- 重新组织 `subject` 目录或 benchmark dashboard 页面。
- 扩展 mobile host、engine binding、product release 流程。
- 把所有 C# feature 一次性补齐。

## 4. 分层职责

### 4.1 Loader / Metadata

负责：

- IL opcode decode；
- type / method / field signature shape decode；
- `byref`、pointer、value-type、generic instantiation 的 metadata 形状解析；
- 为 `AotCoreIr` 提供可消费的 typed 输入。

不负责：

- native ABI 决策；
- test framework 发现与执行。

### 4.2 AotCoreIr

负责：

- 把 typed-IL 输入组装为显式的 IR contract；
- 表达 `byref` carrier、method shape、entry shape、runtime helper 需求；
- 保持 contract 紧凑，不引入 string 协议。

不负责：

- 直接生成 native 文本；
- benchmark / proof 发现逻辑。

### 4.3 NativeAotLoweringPlanner

负责：

- 把 `AotCoreIr` 形状 legalize 到具体 lowering plan；
- 做 entry ABI normalization；
- 生成 wrapper / thunk / dispatch plan；
- 处理 `generic + byref + value-type` 混合路径。

不负责：

- 手工拼接 `.cpp`；
- subject 注册或测试元数据。

### 4.4 NativeAotEmitter + Scriban

负责：

- 读取 widened plan model；
- 组织 Scriban model；
- 通过 `Templates/NativeAot*.scriban` 输出 widened native shape。

硬约束：

- 本轮新增 shape 不能落回 emitter 内联字符串拼接；
- NativeAotEmitter 的 widening 也必须一起迁移/保持在 Scriban 路径。

### 4.5 Owner Subject / Evidence

负责：

- 使用现有 subject 工程表达 proof 与 benchmark 证据；
- 验证新支持面在 owner subject 中真实可见；
- 保持 `unsupported` 与 `missing evidence` 区分。

不负责：

- 定义 AOT core IR 的内部表示；
- 为某个测试样例特化 planner / emitter 分支。

## 5. 建议的 capability family

### 5.1 Byref Carrier Family

覆盖：

- `byref` 参数；
- `byref` 返回值；
- `ldloca` / `ldflda` / temporary address；
- `ref readonly` / mutable byref 的最小区分；
- byref 与 value copy 的 legalization 边界。

### 5.2 Widened Method Shape Family

覆盖：

- 大于当前最小 shape 的参数列表；
- instance/static entry 归一化；
- value-type receiver；
- wrapper / thunk 需要的签名镜像；
- 多参数、多 carrier 混合的方法形状。

### 5.3 Entry ABI Normalization Family

覆盖：

- managed entry 到 native entry 的统一参数映射；
- return carrier 归一化；
- byref / value-type / hidden receiver 的 ABI 规划；
- exported entry、invocation wrapper、dispatch wrapper 的共用 contract。

### 5.4 Generic Mixed Dispatch Family

覆盖：

- generic method / generic type 上的 byref 参数；
- value-type generic receiver；
- interface / virtual dispatch 与 byref carrier 混合；
- wrapper / thunk 与 generic sharing 的交叉边界。

### 5.5 Scriban Emitter Widening Family

覆盖：

- 新 widened plan model 的模板字段；
- wrapper / thunk / entry 代码形状；
- byref / value-type 辅助类型声明；
- 模板输出与 artifact-level tests 对齐。

## 6. 任务拆分原则

每个 child task 必须同时具备以下内容：

- 一份明确的 capability batch 清单；
- 一组底层测试：
  - loader / IR / planner / emitter unit tests；
- 一组证据测试：
  - owner proof；
  - 高价值路径 benchmark，或显式说明 benchmark deferred；
- 一份收口状态：
  - 对应 capability 从 `unsupported/partial` 移向 `supported`，或在 ledger 中保留原因。

## 7. 风险控制

- 先做 ledger freeze，再做实现，避免边界漂移。
- byref 与 entry ABI 先在 `AotCoreIr` 和 planner 定义 contract，再扩大 emitter / template。
- 所有新增 native 代码形状都先以 Scriban model 为目标，避免二次迁移成本。
- 测试框架只证明行为，不承载 AOT core 数据结构。

## 8. 推荐结论

先创建并执行 `Phase 0: Deferred Surface Freeze And Contract Ledger` child task，冻结以下内容：

- 具体 deferred surface 列表；
- 对应 capability family；
- owner subject / proof / benchmark 要求；
- `supported / partial / unsupported` 判定口径；
- 后续 child task 切片顺序。

完成这一步之后，再进入后续实现阶段，避免在 `byref` 与 entry ABI 上继续做隐式扩张。
