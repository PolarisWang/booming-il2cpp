# IL2CPP Mainline 完整方案与性能治理复盘 Brainstorm v1.02

Date: 2026-04-09
Status: needs-roadmap-replan

## 1. 复盘目标

本轮不是继续执行 `Phase 1`，而是先回答一个更高优先级的问题：

- 当前 `roadmap v1.01`，是否已经等价于“完整的 C# -> C++ 完整方案”？
- 当前所有关键决策，是否已经真正收敛成“C++ 性能最优”的完整治理体系？

如果答案不是“是”，就必须先开新一轮 brainstorm，把疑问点重新收口，再决定是否继续执行原 `Phase 1` child plan。

## 2. 直接结论

### 2.1 对“完整的 C# -> C++ 完整方案”的判断

**答案：不是。**

当前 `roadmap v1.01` 只是：

- 一个面向 `Phase A` 的 performance-first core mainline 路线图

它还不是：

- 对“完整 C# -> C++ 方案”做过完成定义的总路线图

原因很直接：

1. roadmap 当前明确排除了 `.NET 10`、`macOS` parity、Android、iOS。
2. roadmap 当前仍把完整异常模型和高阶 BCL 语义放在“当前非目标”之外。
3. `MainlineFeaturePack` 还只是 contract/planning surface，不是已经存在的完整 end-to-end 宿主。
4. 当前实现仍明显 sample-driven，离“完整方案已成型”有很大距离。

### 2.2 对“是否已经完全按 C++ 性能最优决策”的判断

**答案：方向上是，但治理上还没有完全闭合。**

已经成立的部分：

- generated native performance-first 已被提升为 governing rule。
- `OptimizationFacts`、`legality filter + performance arbitration` 已进入主线决策。

还没闭合的部分：

1. 还没有正式的 performance objective function。
2. 还没有冻结 code size / compile time / runtime throughput 之间的裁决规则。
3. 还没有真正的 native perf baseline gate 去证明“最优决策”。
4. 还没有说明 `exceptions`、generics specialization、devirtualization 在冲突时怎样做性能裁决。

换句话说：

- **“性能优先”已经是路线原则**
- **但还不是已冻结的完整性能治理系统**

## 3. 当前 roadmap 的主要缺口

## 3.1 缺少 “Definition Of Complete”

现在的 roadmap 没有先回答：

- 什么叫“完整的 C# -> C++ 方案”

至少还缺三层完成定义：

- 语义完整度
  - 例如 exceptions、delegates、arrays、boxing、generics、reflection、interop 到什么深度才算“完整”
- 平台完整度
  - 是只要求 Windows reference，还是包含 desktop parity，还是包含 Android/iOS runtime subset
- 工程完整度
  - 是否要求统一 subject、统一 report、统一 perf gate、统一 engine binding

如果不先定义“完整”，后面 roadmap 再长，也无法回答“我们是不是已经实现了完整方案”。

## 3.2 缺少 “Performance Governance”

现在的 roadmap 说了“性能优先”，但没有冻结：

- 优先优化什么
  - throughput、latency、allocation、code size、binary size、startup 还是 traceability
- 允许牺牲什么
  - code size、编译时长、实现复杂度、调试可读性
- 在什么证据下做裁决
  - benchmark baseline、generated code inspection、runtime metrics、platform gates

这意味着当前路线虽然方向正确，但还不足以支撑“每一个分歧都按 C++ 性能最优收口”。

## 3.3 当前 roadmap 其实是 “Core Mainline Roadmap”，不是 “Total Solution Roadmap”

当前 Phase 1-7 的实质更接近：

- 先把 IL2CPP core mainline 做成 performance-first
- 再补完整 subject
- 最后收口 engine binding

它缺少总路线图里常见的两件事：

- 总体完成定义
- 当前这条 core roadmap 与未来 `.NET 10` / parity / mobile / engine 的关系说明

## 3.4 `exceptions` 的位置还不够硬

虽然用户已经明确说 `exceptions` 必须做，
但 roadmap 里它仍然被放在 `Phase 6`，且完成定义还写成 open question。

这在治理上仍然不够硬：

- 它应该被明确写成“必做项”
- 而不是“可能在 closure 再决定”

## 4. 可选重规划路径

### 方案 A：不改 roadmap，只口头说明它不是完整方案

优点：

- 最快。

缺点：

- 文档继续会误导后续执行。
- 无法在未来回答“完整方案是否达成”。

判断：

- 不推荐。

### 方案 B：给当前 roadmap 增加一个前置 `Phase 0`

新增一个前置阶段，专门冻结两件事：

- `DefinitionOfComplete`
- `PerformanceGovernance`

然后原来的 `Phase 1-7` 顺延执行。

优点：

- 最小改动就能把核心缺口补上。
- 不推翻现有 performance-first core 路线。
- 最适合当前仓库状态。

缺点：

- 需要暂停当前 child plan。

判断：

- **推荐**

### 方案 C：直接把 roadmap 扩成“全量总路线图”

做法：

- 现在就把 `.NET 10`、desktop parity、mobile、engine binding、完整 BCL 语义全写进一条总 roadmap。

优点：

- 看起来更像“完整方案”。

缺点：

- 当前实现基础还不够稳。
- 会把讨论重新拉散。

判断：

- 不推荐在当前时点采用。

## 5. 推荐重规划结论

推荐采用 **方案 B**：

- 暂停 `20260409-02-phase-1-optimization-facts-and-native-perf-contracts` 的执行。
- 回到父 roadmap。
- 新增前置 `Phase 0`，冻结：
  - 完整方案定义
  - 性能治理规则
- 明确 `exceptions` 是强制交付项，不再保留为 closure open question。
- 然后再恢复原 `Phase 1` child plan。

## 6. 建议新增的 `Phase 0`

`Phase 0: Completeness Definition And Performance Governance`

建议它至少回答以下问题：

1. 什么叫“完整的 C# -> C++”
2. 当前这条 roadmap 属于总路线图中的哪一段
3. 哪些语义是必须交付项
4. 哪些平台属于当前路线，哪些属于后续路线
5. “C++ 性能最优”具体按什么指标裁决
6. 当 runtime throughput、code size、compile time 冲突时怎么选

## 7. 这轮 brainstorm 的结论

当前文档状态下，不能把 `roadmap v1.01` 描述成：

- 已经覆盖完整 C# -> C++ 全方案

也不能把当前决策描述成：

- 已经完全闭合为“所有分歧都按 C++ 性能最优裁决”的最终体系

可以更准确地说：

- 这是一个方向正确、但仍需补 `Phase 0` 的 performance-first core mainline roadmap。
