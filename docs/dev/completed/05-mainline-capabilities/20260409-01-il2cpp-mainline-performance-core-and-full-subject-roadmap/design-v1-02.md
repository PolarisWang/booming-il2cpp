# IL2CPP Mainline 完整性定义与性能治理设计 v1.02

Date: 2026-04-09
Status: design-reframed-for-roadmap-v1-02

## 1. 设计目标

本轮设计不再默认把当前路线表述成“完整 C# -> C++ 总方案已经成立”，而是先修正它的定位：

- 当前路线首先是 `Phase A` 的 performance-first core mainline 设计。
- 它必须先回答“当前这条主线的 complete 到底是什么意思”。
- 它必须先冻结“C++ 性能最优”在工程上到底如何裁决，而不是继续停留在口号层。

因此，本轮设计的新增输出不是直接扩写更多 capability，而是为父 roadmap 增加一个前置 `Phase 0`。

## 2. 当前路线的定位修正

当前仓库中的这条 roadmap，应该被明确理解为：

- 一条面向 `generated native` 的主线核心路线。
- 一条为后续完整方案铺底的阶段性路线。
- 一条还没有覆盖 `.NET 10`、`macOS parity`、`Android/iOS`、完整高阶 `BCL` 语义的路线。

因此，当前路线不能再被表述成：

- 已经等价于完整 `C# -> C++ total solution`

更准确的表述应当是：

- 这是 total solution 之前必须先收口的 `Phase A mainline roadmap`

## 3. `Phase 0` 需要冻结的两类输出

### 3.1 `DefinitionOfComplete`

`Phase 0` 必须先把“complete”的定义写清楚，至少拆成三层：

- 语义完整度
  - 当前主线必须交付哪些语义：`dispatch`、`closed generics/layout`、`arrays/boxing`、`delegates/callbacks`、`exceptions`、必要的 `reflection/interop` 补位。
- 平台完整度
  - 当前主线以 `Windows reference/native host` 为收口基线。
  - `.NET 10`、`macOS parity`、`Android/iOS runtime subset` 不属于本轮完成定义，但必须被写成后续总路线的外层 follow-up。
- 工程完整度
  - 必须有 `MainlineFeaturePack`。
  - 必须保留现有最小 `probe subjects`。
  - 必须有统一 `native output / trace / perf` 验证入口、报告和 baseline gate。
  - `engine binding` 必须保留在整条 roadmap 最后收口。

### 3.2 `PerformanceGovernance`

`Phase 0` 还必须先把“C++ 性能最优”落成正式治理规则，而不是继续由局部讨论隐式决定：

- 首要目标
  - 优先最大化支持平台上 generated native 热路径的 runtime throughput 与 allocation 行为。
- 次级目标
  - 在不伤害热路径质量上限的前提下，控制 startup 成本。
- 可接受代价
  - 为了保留 specialization、devirtualization、layout-aware codegen 空间，可以接受受控的 code size、binary size、compile time 与实现复杂度成本。
- 裁决证据
  - `native perf baseline`
  - generated C++ inspection
  - runtime metrics / trace
  - correctness proof
- 明确否定
  - 不能仅凭 managed-only benchmark 就宣称某个 core 决策已经是“C++ 最优”。

## 4. 继承并硬化的既有设计

本轮不是推翻 `v1.01`，而是在其上补齐前置治理层。以下设计继续保留：

- `SemanticWorld` 继续保持六块 canonical output。
- `OptimizationFacts` 继续作为 `LinkedWorld -> CodeGen` 的 first-class contract。
- `LoweringFamily` 继续沿着“合法性过滤 + 性能裁决”演进。
- `NativeReferenceProofEmitter` 的长期方向仍然是退化为 lowering backend，而不是继续承担 sample-driven 识别职责。
- 继续保留 `HelloWorldObject`、`GenericEcho`、`ReflectionLite`、`PInvokeLite`。
- 继续新增独立 full subject `MainlineFeaturePack`。

同时，本轮把两个点从“倾向”升级为硬约束：

- `exceptions` 是 roadmap 内强制交付项，不再保留为 closure open question。
- `engine binding` 必须位于整个 roadmap 最后阶段，不得前置打乱 core 主线。

## 5. 新的设计结论

从现在开始，这条主线的正确理解应当是：

1. 当前并不是完整 `C# -> C++ total solution roadmap`。
2. 当前是 total solution 之前的 `performance-first core mainline roadmap`。
3. 在继续执行原 `Phase 1` 之前，必须先补上 `Phase 0`。
4. `Phase 0` 的职责是冻结：
   - `DefinitionOfComplete`
   - `PerformanceGovernance`
5. `Phase 0` 完成后，才能重新声称后续 child plan 的前提已经稳定。

## 6. 需要用户确认的点

本轮重规划后，仍有两个点需要你拍板：

1. 当前这轮“complete”的命名和边界，是否确认按 `Phase A Mainline Complete` 来定义，而不是把 `.NET 10` / parity / mobile 一起并入当前 done definition。
2. 性能治理的默认优先级，是否确认按下面的顺序冻结：
   - runtime throughput / allocation
   - startup
   - code size / binary size
   - compile time

如果你不同意这两个前提，下一步就不应该直接开 `Phase 0` child，而是继续在父任务层做新一轮 brainstorm。
