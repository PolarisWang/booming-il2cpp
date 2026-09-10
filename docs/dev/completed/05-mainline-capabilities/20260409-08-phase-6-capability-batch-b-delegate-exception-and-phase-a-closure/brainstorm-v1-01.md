# Phase 6 Capability Batch B Brainstorm v1.01

Date: 2026-04-09

## 1. 背景

`Phase 5` 已经把 `dispatch`、`closed generics/layout`、`arrays/boxing` 接入完整 `MainlineFeaturePack` 主线。接下来的 `Phase 6` 不是继续堆更多例子，而是补齐 `Phase A` 闭环前最后一批强制能力：

- `delegates/callbacks`
- `exceptions`
- 必要的 `reflection/interop` 补位

同时仍然必须满足这些硬约束：

1. 当前主线仍然不是完整 `C# -> C++ total solution`
2. 一切取舍继续服从 generated native 工程的 `C++` 性能优先
3. `engine binding` 必须继续留在 roadmap 最后阶段
4. `contract` / `subject` 边界清理仍然是单独 follow-up，不插回当前主任务

## 2. 备选方案

### 方案 A：先做 delegates，exceptions 再拆到后续

优点：

- 实现表面更小
- 能更快开始编码

缺点：

- 直接违反 roadmap 里“`exceptions` 是强制交付项”的约束
- `Phase A closure` 仍然不成立
- 后续还要再拆一次 design / plan / verification 主线

结论：

- 不采用

### 方案 B：把 delegates / exceptions / reflection / interop / engine binding 一次并入

优点：

- 看起来像“一步到位”

缺点：

- 过早把 `engine binding` 卷入 core mainline
- 设计边界会再次失真
- 很难继续证明 generated native `C++` 性能优先

结论：

- 不采用

### 方案 C：`Phase 6` 只做 Batch B + Phase A closure，继续复用完整 subject 主线

优点：

- 与 roadmap 边界一致
- 仍能保持一个 `MainlineFeaturePack`
- 能把 `reflection/interop` 从“已有 lite 参考能力”提升为“完整 subject 主线可验证能力”
- 不提前污染 `engine binding`

缺点：

- 需要在一个阶段内同时处理 `delegates` 与 `exceptions`
- 对 runtime / codegen 边界要求更高

结论：

- 推荐采用

## 3. 推荐方向

推荐沿用 `Phase 5` 的结构原则：

- 一个完整 `MainlineFeaturePack`
- 多个 capability proof slice
- correctness 分矩阵切开
- perf 继续保留统一入口

在 `Phase 6` 里，具体能力边界建议如下：

### 3.1 Delegates / Callbacks

- 先覆盖 closed static delegate
- 再覆盖 closed instance delegate
- 支持 delegate invoke 与必要的 callback relay
- 对 exact target 场景优先 lower 成 direct call
- 仅对无法静态裁决的场景保留 runtime helper fallback

不做：

- multicast delegates
- open instance delegates 全量矩阵
- expression trees
- 把 delegate callback 扩展成通用 engine callback ABI

### 3.2 Exceptions

- 先覆盖 `throw / catch / finally`
- 目标是在 generated native 里形成真实受控语义，不再只是 stub
- 设计上优先考虑“热路径无额外显式状态传播负担”的方案

不做：

- exception filters
- async exceptions
- 完整 CLR EH 兼容
- 跨 `engine binding` 边界的最终异常策略

### 3.3 Reflection / Interop 补位

- 只做 `Phase A closure` 所需的最小补位
- 优先复用已有 `reflection.closed-type-query.minimal` 与 `interop.pinvoke-direct-call.minimal` 经验
- 目标是把这些能力接回 `MainlineFeaturePack`，而不是维持平行 lite subject 主线

## 4. 当前建议

下一步应进入 `design-v1-01.md`，冻结下面这些关键设计：

1. `delegates/callbacks` 的 exact-target vs runtime fallback 裁决边界
2. `exceptions` 的 cold-path 语义策略
3. `reflection/interop` 补位的最小闭环范围
4. `Phase A closure` 到底以哪些 focused matrices 与 perf evidence 为完成证据
