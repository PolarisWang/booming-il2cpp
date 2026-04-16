# Post Phase 7 Deferred AOT Shape And Entry ABI Expansion Brainstorm v1.01

Date: 2026-04-15 23:52:48 +08:00
Status: approved-for-roadmap

## 1. 背景

`20260415-11` 已经完成了上一轮 typed-IL translation expansion，项目级验证恢复到：

- `python -m pytest -q`
- 结果：`713 passed, 40 skipped`

但当前 closure 更接近“已经把一批高价值 surface 闭环”，而不是“全部 AOT 形状已经完成”。从上一轮实现与台账回看，真正还没系统化收口的点集中在：

- `byref` 形状：
  - 参数、返回值、局部、field address、temporary address、byref-to-value 的合法化；
- widened method shape：
  - 大于当前最小 method shape 的参数列表、wrapper/thunk、instance/static 归一化、value-type receiver；
- entry ABI normalization：
  - managed entry、native exported entry、invocation wrapper、generic/shared wrapper 的参数与返回约定；
- `generic + byref + value-type` 混合路径：
  - planner 能否稳定消费，emitter 和 Scriban 模板能否真实落地，而不是停在 `NotSupportedException` 或 legacy fallback。

## 2. 这轮 follow-up 要解决什么

这轮 roadmap 的目标不是继续泛化测试系统，也不是重开一个大而全的 capability 项目，而是把上一轮明确 deferred 的 AOT surface 独立收口：

- 让 typed-IL 中的 `byref / widened method shape / entry ABI` 有正式 contract；
- 保持 `AotCoreIr -> NativeAotLoweringPlanner -> NativeAotEmitter + Scriban` 单向链路，不回退到手工字符串拼接；
- 只在 owner subject / proof / benchmark / gate 层补证据，不让测试框架主导 AOT core 结构；
- 为后续 hot-update / hybrid CLR 风格扩展，先把 AOT 侧的形状表达打牢。

## 3. 约束

- `Chaos.TestFramework` 代码不能混入 AOT Core IR、planner、emitter。
- 新增 native codegen 逻辑必须继续走 `Scriban`，不能在 emitter 里恢复大段手工 `C++` 拼接。
- 不继续扩大 string 协议；需要身份或分类时，优先复用现有 compact identity / enum 路线。
- 这轮不负责 mobile host、engine binding、dashboard UI 重构，也不重开上一轮已经完成的 benchmark/gate 大面。

## 4. 讨论过的推进方案

### 方案 A：按 pipeline 分层推进

顺序：

1. 先把 loader / metadata 全部 widen；
2. 再做 `AotCoreIr`；
3. 再做 planner；
4. 最后做 emitter / owner evidence。

优点：

- 代码边界清晰；
- 分工上最容易理解。

缺点：

- 最容易出现上层变绿、下层仍然不可消费的假闭环；
- `byref` 和 entry ABI 这种跨层语义，很难只在单层定义完成。

### 方案 B：按 capability batch 纵向推进

顺序：

1. 先冻结 deferred surface 台账；
2. 每个 batch 横穿 loader -> IR -> planner -> emitter -> evidence；
3. 每个 batch 收口后再进入下一批。

优点：

- 最接近真实交付路径；
- 可以明确每一批是否真的具备 owner proof / benchmark / gate evidence；
- 不会因为单层成功而误判整体支持。

缺点：

- 每一批都会触发多层修改；
- 如果 batch 边界切得不好，单个 child task 会变得过大。

### 方案 C：按 subject 缺口反推底层

顺序：

1. 先从 proof / benchmark 缺数据项出发；
2. 看到哪个 subject 跑不过，再反推 pipeline 缺口。

优点：

- 用户感知收益最直接；
- 很适合补 owner evidence。

缺点：

- 极易退化为 case-by-case patch；
- 最容易把 `Chaos.TestFramework` 路径和底层 AOT 设计缠在一起。

## 5. 结论

采用方案 B。

执行原则：

- 先写新一轮 deferred surface ledger，只聚焦：
  - `byref carrier family`
  - `widened method shape family`
  - `entry ABI normalization family`
  - `generic/value-type/byref mixed dispatch family`
  - `Scriban emitter/template widening family`
- 再把这些 family 拆成顺序 child task；
- 每个 child task 都必须回答：
  - loader 是否能 decode；
  - `AotCoreIr` 是否能表达；
  - planner 是否能 legalize；
  - emitter + Scriban 是否能输出；
  - owner proof / benchmark / gate 是否能给出正确证据。

## 6. 为什么必须用 roadmap

这个 follow-up 明显不适合直接写成单个稳定 plan，因为它同时满足：

- 需要 2 个以上顺序阶段；
- 需要多个相互依赖但相对独立的 child task；
- 会跨多次会话持续推进；
- 在正式实现前，必须先冻结 phase 边界、退出标准和 child task mapping。

因此，这次输出应该是新的 roadmap，而不是直接进入单个 plan。
