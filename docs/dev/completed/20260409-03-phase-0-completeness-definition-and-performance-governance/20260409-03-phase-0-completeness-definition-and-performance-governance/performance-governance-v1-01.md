# Phase A Performance Governance v1.01

Date: 2026-04-09
Status: frozen-for-phase-1-unblock

## 1. 文档目的

本文件定义当前主线中，“generated native performance-first”到底如何落成正式治理规则。

本文件生效后，后续 child task 不能再只靠口头表述“性能优先”，而必须按这里的目标函数、证据链和裁决顺序行事。

## 2. 总原则

在 correctness 成立、required semantics 不被破坏的前提下：

- **一切 core 层架构决策优先服务于 generated native 热路径的 C++ 性能上限。**

这里的“性能上限”不是泛指所有指标平均最优，而是有明确优先级的。

## 3. 目标函数优先级

默认优先级冻结为：

1. generated native 热路径的 runtime throughput
2. generated native 热路径的 allocation 行为
3. startup
4. code size / binary size
5. compile time

等价表达是：

- 为了保留 specialization、devirtualization、layout-aware codegen 空间，可以接受受控的 code size、binary size 与 compile time 成本
- 但不能以牺牲 required semantics 为代价

## 4. 允许与不允许的牺牲

### 4.1 允许的牺牲

在受控范围内，允许为了 native 质量上限而牺牲：

- code size
- binary size
- compile time
- 实现复杂度的适度上升

### 4.2 不允许的牺牲

不允许为了看起来更快而牺牲：

- correctness
- 已定义为 required 的 `exceptions` 语义
- `Phase A Mainline Complete` 所要求的 capability 交付边界

## 5. 裁决顺序

当多个 lowering/path 都合法时，必须按下面顺序裁决：

1. legality filter
2. performance arbitration
3. deterministic tie-breaker

这意味着：

- 静态优先级顺序不再是主裁决器
- deterministic order 只在性能裁决无法区分时才允许介入

## 6. 证据链

能够支持“这个决策更优”的证据，必须优先来自：

1. native perf baseline
2. generated C++ inspection
3. runtime metrics / trace
4. correctness proof
5. managed benchmark 仅作为辅助信息

明确禁止：

- 仅凭 managed-only benchmark 就宣称某个 core 决策已经是 “C++ 最优”

## 7. 面向后续阶段的默认拍板

在后续 `Phase 1` 到 `Phase 6` 中，默认拍板规则如下：

- 当某个方案更能保留 closed-world specialization 空间时，默认优先
- 当某个方案更能保留 devirtualization 空间时，默认优先
- 当某个方案更能保留 layout-aware codegen 空间时，默认优先
- 但如果它破坏 required `exceptions` 或其他 required semantics，则不允许采用

## 8. 对验证主线的约束

本治理规则要求后续主线至少具备：

- `windows-native-profile` 作为正式 perf 收口矩阵
- `MainlineFeaturePack` 作为 capability/perf 主宿主
- perf report 与 baseline compare 进入正式 report surface

这意味着：

- perf 不能继续主要停留在 `GenericEcho` 的 managed perf 模式
- `MainlineFeaturePack` 不是可选增强，而是 native perf 主线的正式宿主

## 9. 冻结结论

从现在开始，当前主线里“C++ 性能最优”的默认工程含义是：

- 优先最大化 generated native 热路径 throughput / allocation 表现
- 以 native evidence 为主证据
- 在 legality 之后用 performance arbitration 做主裁决
- 在 required semantics 成立前提下，允许付出受控 code size / compile time 成本
