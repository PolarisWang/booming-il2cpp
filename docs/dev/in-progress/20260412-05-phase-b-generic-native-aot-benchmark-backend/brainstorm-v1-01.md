# Phase B Generic Native AOT Benchmark Backend Brainstorm

Date: 2026-04-12 11:42:02 +08:00
Status: concluded

## 任务意图

当前 `run benchmark --subject BenchArithmetic --mode native --record` 没有产出 dashboard 数据，
根因已经明确不是 benchmark 记录层、dashboard 聚合层，或者 benchmark source 里的 `Stopwatch` / `args`
之类写法，而是 native benchmark 仍然复用了 `generated-native-proof` 路线。

真实阻塞点是 `NativeReferenceLoweringPlanner.SelectLoweringFamily` 无法把
`BenchArithmetic/Program::Main()` 归类到现有 proof family，最终报错：

`CodeGen lowering selection could not classify 'BenchArithmetic/Program::Main()' from semantic structure + capability bundle`

这说明当前问题不是“某个 benchmark 样例还不够 minimal”，而是 backend 选路本身就错了。

## 必须守住的边界

- 性能计时、采样、对比逻辑必须放在 generated code 之外，由外部 perf worker / harness 承担。
- benchmark source 从 C# 层开始就不应携带 perf scaffolding；workload source 应保持纯 workload。
- 不能继续通过给 `NativeReferenceLoweringPlanner` 增加 `Bench*Minimal` / `Bench*Proof` family 特判来打补丁。
- proof backend 与 generic native AOT backend 必须拆线，benchmark native 不能再绑定 proof family 分类器。

## 备选方案

### 方案 A：继续沿 proof family 路线补 benchmark 特判

- 做法：为 `BenchArithmetic` 继续新增 lowering family、模板或 capability 组合特判，直到 `Main()` 能被 proof emitter 接住。
- 优点：
  - 改动局部，短期可能最快看到一个 native 可执行物。
- 缺点：
  - 会把 benchmark 伪装成 proof subject，继续污染 `NativeReferenceLoweringPlanner`。
  - 每新增一个 `Bench*` 都会重复扩展 family，无法形成通用 AOT backend。
  - 与用户已经明确拒绝的方向冲突。

### 方案 B：保留 proof backend，不拆线，只给现有链路补一个 generic fallback

- 做法：继续让 benchmark 走 `generated-native-proof`，但在 planner / emitter 某处增加 fallback，尽量绕过 family 分类失败。
- 优点：
  - 比纯 proof family 特判稍微通用一点。
- 缺点：
  - proof artifact、generic AOT artifact、runtime perf entry 仍然缠在一起。
  - 即便跑通 `BenchArithmetic`，后续 `BenchAllocation` / `BenchDispatch` 仍然会复发边界不清的问题。
  - 无法从结构上回答“benchmark native 到底消费什么 contract”。

### 方案 C：拆分 `generated-native-proof` 与 `generated-native-aot`，并把 perf harness 保持在外部

- 做法：为 benchmark native 建立独立的 `generated-native-aot` 路线，冻结新的 artifact contract，
  在 manifest 中引入 `workloadEntry`，让外部 native perf worker 调 workload，而不是把计时代码塞进 generated code。
- 优点：
  - 与用户约束一致，边界清晰。
  - 可以把 proof backend 继续保留给 capability/proof subject，把 benchmark/native 迁到通用 AOT backend。
  - `BenchArithmetic` 打通后可以复制到其他 `Bench*`，不再要求 proof family 扩张。
- 缺点：
  - 不是一个小修，需要 roadmap 级拆解。
  - 需要先冻结 contract，再做 MVP，不适合直接跳到编码。

## 当前推荐

- 采用 **方案 C**。

## 额外判断

这个任务不适合直接写成单一实现 plan，而应先形成新的子 roadmap：

- 至少天然分成 backend 拆线、benchmark entry contract、BenchArithmetic MVP、后续 rollout 四个阶段。
- 这些阶段共享同一目标，但依赖、风险和验收点不同。
- 如果现在直接写一个大 plan，很容易又回到“proof family 修修补补先跑一个”的错误路径。

因此下一步应当是：

1. 创建新的子任务目录，承载 generic native AOT benchmark backend 的 roadmap。
2. 明确 `generated-native-proof` 与 `generated-native-aot` 的分工和 artifact contract。
3. 先把 `BenchArithmetic` 作为 MVP 打通，再决定其他 `Bench*` 的 rollout 顺序。
