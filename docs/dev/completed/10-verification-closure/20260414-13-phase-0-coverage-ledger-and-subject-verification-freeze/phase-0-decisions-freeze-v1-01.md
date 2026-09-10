# Phase 0 Decisions Freeze v1.01

Date: 2026-04-14 12:52:36 +08:00
Status: frozen

## 1. 目的

记录本阶段为了让后续 child tasks 能直接执行而冻结的默认工程判断。

## 2. 冻结决策

### 决策 A: capability 闭环的最小单位是 `capability_item`

- 不再按“散落语法点”管理主线。
- 语法点拆分可以继续做，但只是 proof 粒度，不是 owner 粒度。

### 决策 B: owner 以 retained subject 为唯一顶层单位

- 继续只保留：
  - `SolutionCorePack`
  - `HotUpdateHostPack`
  - `MixedExecutionFeaturePack`
- 新能力点只允许进入这三个 owner。

### 决策 C: benchmark 阈值默认按 `capability_family + metric` 治理

理由：

- 比按 subject 维度更细，能避免一个 subject 内不同类型能力共享错误阈值。
- 比按单 case 维度更稳，能避免阈值碎片化。

补充规则：

- 个别 benchmark case 如需例外，只允许在外层 reporting / benchmark policy 中声明。
- 不允许把阈值写进 subject source 或 attribute。

### 决策 D: “完整验证能力”采用统一 checklist，而不是口头解释

- 后续 child task 只要声称某个能力已经完成，就必须回到 checklist 核对。
- checklist 先放在 `docs/dev` 子任务目录，Phase 5/6 再决定是否抽到长期 architecture docs。

### 决策 E: reference/corelib/patch bundle 全部继续 repository-owned

- 不重新引入本机 SDK 路径。
- 不搞脚本动态重写 `.csproj`。
- 不用整包 SDK 镜像替代 foundation bundle。

### 决策 F: baseline 闭环的最小诊断产物只要求 proof / benchmark / support-state / archetype 映射

- trace、symbol、metadata supplement 摘要属于增强项。
- 它们不是 Phase 0 的前置完成条件。

## 3. 延后到后续阶段的问题

以下问题不在 Phase 0 定案，延后到后续 child task：

- Phase 1:
  - dashboard 需要显示哪些增强诊断件
  - 哪些 capability 需要强制对外暴露 trace/symbol
- Phase 5:
  - 哪些 archetype 进入默认 CI gate
- Phase 6:
  - mixed/hot-update release gates 下钻到哪个 host-platform 粒度
