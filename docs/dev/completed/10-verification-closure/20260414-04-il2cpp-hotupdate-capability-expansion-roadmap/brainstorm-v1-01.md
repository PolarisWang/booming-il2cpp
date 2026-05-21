# IL2CPP 与热更能力覆盖扩展 Brainstorm v1.01

Date: 2026-04-14 09:47:42 +08:00

## 1. 用户目标重述

本轮新任务不是继续做“收口验证”，而是基于已经完成的 3 个 retained subjects，继续把 IL2CPP 与热更的能力覆盖扩展开来：

- 把历史语法点机械拆成单文件 proof/benchmark 条目。
- 按能力族尽量合并在同一个共享项目里，而不是继续新增很多测试工程。
- subject 内部不再依赖 string 协议持续扩大调度面。
- 完整 solution 与完整热更 workflow 都要成为 canonical subject 内容。
- 最终新增测试目标时，优先是往现有 retained subject 里补 capability/archetype，而不是再建顶层 subject。

## 2. 当前基线

- 上一条 roadmap 已完成，顶层 `subjects/` 只剩：
  - `SolutionCorePack`
  - `HotUpdateHostPack`
  - `MixedExecutionFeaturePack`
- `Chaos.TestFramework` 已经在 `src/reference/Chaos.TestFramework`，proof 也已走 `Assert`。
- registry / planner / benchmark dashboard / proof / benchmark / archetype 真实链路已经跑通。

这意味着新问题不再是“系统能不能工作”，而是“能力覆盖怎么扩，且不破坏已经收口的结构”。

## 3. 关键判断

### 3.1 不应该再扩顶层 subject

顶层 subject 已经按 executable plan 划分完毕。继续新增 `LanguageFeaturePack` 之类 subject，只会把已经收口的结构重新打散。

### 3.2 应该把语法点重写为能力族下的单文件条目

历史 proof 最大的问题不是“文件太多”，而是：

- 语义边界不稳定。
- 同一类能力散落在多个旧 subject / phase 命名中。
- discovery / dashboard 看到的是旧命名，而不是能力模型。

所以更合理的方向是：

- 单文件条目化。
- 能力族归并。
- 少量共享项目承载。

### 3.3 需要一个新的 roadmap，而不是直接开改

这次不是单个实现任务，而是会持续跨多个阶段：

- capability taxonomy freeze
- framework metadata 扩展
- retained subjects 结构迁移
- pipeline / dashboard / docs 跟进
- 最终全量验证

它明显属于 roadmap，而不是一次性的 plan。

## 4. 方案对比

### 方案 A: 继续沿旧 proof 名称逐个迁移

优点：

- 短期改动小。

缺点：

- 会把旧 phase / subject 命名继续带进新体系。
- 不能解决能力矩阵和 archetype 矩阵的表达问题。

结论：

- 不推荐。

### 方案 B: 以 retained subjects 为边界，按能力族和 archetype 双维度重建内部结构

优点：

- 保持顶层结构稳定。
- 新增测试目标时有稳定归属。
- 能直接服务 planner / dashboard / docs。

缺点：

- 前期必须先做 taxonomy freeze。

结论：

- 推荐。

### 方案 C: 重新拆出更多顶层测试工程或顶层 subject

优点：

- 单项目体量看起来更小。

缺点：

- 与现有 retained-subject 冻结方向相冲突。
- 会让统一入口、registry、dashboard 再次变复杂。

结论：

- 不推荐。

## 5. 推荐方向

采用方案 B，并先做一个新的父 roadmap：

- 顶层保留 3 个 retained subjects。
- `SolutionCorePack` 承担绝大多数 IL2CPP 语义能力与 solution archetype。
- `HotUpdateHostPack` 承担完整热更 workflow。
- `MixedExecutionFeaturePack` 承担 mixed/interpreter 特有链路。
- 所有新的 proof/benchmark 声明优先继续走 `Chaos.TestFramework`。

## 6. 下一步建议

下一步不是直接写实现，而是先落一条新的 roadmap，并把第一个 child task 固定为：

- `Phase 0: capability taxonomy and inventory freeze`

这一步需要先回答：

- 哪些二级能力项是本轮 canonical vocabulary。
- 哪些 archetype 是 first-class contract。
- 哪些历史语法点只需要 proof，哪些需要 benchmark。
- 哪些内容归 `SolutionCorePack`，哪些必须留在 `HotUpdateHostPack` 或 `MixedExecutionFeaturePack`。
