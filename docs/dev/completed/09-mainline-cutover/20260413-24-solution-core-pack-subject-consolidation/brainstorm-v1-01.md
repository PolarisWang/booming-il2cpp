# Subject Solution Core-Pack 收敛 Brainstorm v1.01

Date: 2026-04-13 08:43:27 +08:00
Status: approved

## 1. 用户目标

用户这次的新目标有三件事：

- 清理 `subjects/`
- 把现有 subject 尽可能合并到一个 solution 模式下，并保持测试目标清晰
- 统一入口同步调整，并最终验证所有 solution 模式 subject 的 `proof` 和 `benchmark` 流程

用户随后澄清了两点：

- “验证四年的 solution 的测试工程” 实际指的是 “验证所有 solution 模式的 subject”
- 在候选方案中，最终选择 **方案 A：极限大合并**

## 2. 当前仓库事实

基于本轮现场检查，当前仓库的关键事实如下：

- `subjects/` 仍包含大量顶层 legacy root，不少仍是 `*Proof`、`Bench*`、`*Lite` 这类按单点能力拆出来的 subject。
- 外层统一入口 `run test ...` 已经存在，registry / selector / report / artifact 主链也已经统一。
- `MainlineFeaturePack`、`PerformanceFeaturePack`、`HotUpdateHostPack`、`MixedExecutionFeaturePack` 已经部分具备 pack 化基础。
- `ChaosUnitTest` / `ChaosBenchmark` 的 attribute 模型已经局部落地，但还没有成为所有 retained subject 的唯一声明主线。
- 目前 `subjects/` 下没有显式 `.sln` 文件；所谓 “solution 模式” 在仓库现状中更接近 “`source/` 下若干 project 的 solution-like 布局”。

## 3. 候选方案

### 方案 A：极限大合并

- 把大部分 legacy proof / benchmark / interop / engine / archetype subject 并入一个大的 `SolutionCorePack`
- 只保留极少数真正因为 executable plan 差异而无法合并的 subject
- 统一入口在 retained subject 内部也同步收敛

优点：

- `subjects/` 最干净
- 新增测试点的默认动作最简单
- 用户目标与仓库形态高度一致

问题：

- mega subject 的组织和命名边界必须提前冻结
- 迁移与验证会跨多个阶段，不能一次性硬切

### 方案 B：分域 canonical packs

- 保留多个清晰分域的 canonical subject
- 以 `MainlineFeaturePack`、`PerformanceFeaturePack`、`HotUpdateHostPack`、`MixedExecutionFeaturePack`、`InteropHostPack` 等方式长期共存

优点：

- 边界更稳
- 单个 subject 复杂度更低

问题：

- 和用户希望 “尽可能合并” 的目标不完全一致

### 方案 C：入口统一优先，物理合并延后

- 先统一入口和 registry 语义
- 暂时不大动 `subjects/` 物理布局

优点：

- 风险最低

问题：

- 不能真正把 `subjects/` 清干净
- 后面仍要补做一次大迁移

## 4. 决策

用户已明确选择 **方案 A**。

因此本任务后续不再围绕 “多个功能域长期并列共存” 做设计，而是采用如下基线：

- retained subject 只保留极少数真正有独立 executable plan 的 solution-mode subject
- 默认把现有 `Solution*`、`Mainline*`、`Performance*`、大部分 `*Proof`、大部分 `Bench*` 并入单一核心 subject
- 统一入口不再按历史 subject shell 风格增长，而是围绕 retained subject 的统一 launcher / discovery / selector 收敛

## 5. 由此冻结的设计方向

本次 roadmap 将以以下方向为前提：

- canonical retained subject 首选：
  - `SolutionCorePack`
  - `HotUpdateHostPack`
  - `MixedExecutionFeaturePack`
- 每个 retained subject 都升级为显式 `.sln` 驱动的 solution-mode subject
- 每个 retained subject 都必须有：
  - 至少一条 `proof` 主路径
  - 至少一条 `benchmark` 主路径
- legacy 顶层 root 迁移后只保留过渡 alias / redirect，再逐步删物理目录
- `subject.manifest.json` 继续走薄 orchestrator 方向
- 源码内部测试声明继续使用：
  - `ChaosUnitTest`
  - `ChaosBenchmark`

## 6. 为什么进入 roadmap 而不是直接写 plan

本任务必须进入 `roadmap`，原因是它同时包含：

- `subjects/` 大规模物理收敛
- retained subject 的显式 `.sln` 升级
- subject 内部统一入口改造
- registry / selector 的 subject 选择策略调整
- 对所有 retained solution-mode subject 的 `proof` / `benchmark` 验证闭环

这不是一个单次稳定可执行的 plan，而是明显需要分阶段推进的结构性重整任务。
