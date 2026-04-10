---
task_id: 20260409-10-total-solution-and-ios-hot-update-analysis
title: 完整 IL2CPP 总方案与 iOS 热更分析
task_type: roadmap
lifecycle_status: in_progress
phase: roadmap
created_at: 2026-04-09 23:20:00 +08:00
updated_at: 2026-04-11 10:00:00 +08:00
current_dir: docs/dev/in-progress/20260409-10-total-solution-and-ios-hot-update-analysis
parent_task_id:
source_task_id:
source_relation:
active: false
---

## 关键文档

- brainstorm: `docs/dev/in-progress/20260409-10-total-solution-and-ios-hot-update-analysis/brainstorm-v1-01.md`
- design: `docs/dev/in-progress/20260409-10-total-solution-and-ios-hot-update-analysis/design-v1-01.md`
- roadmap_or_plan: `docs/dev/in-progress/20260409-10-total-solution-and-ios-hot-update-analysis/roadmap-v1-01.md`

## 当前判断

- current_focus: 已把这项工作升级为 roadmap，当前重点是冻结“先 contract、再 ingestion/completeness、再 mobile/engine、最后 hot update runtime”的阶段顺序，并明确每阶段的验证 gate。
- why_now: 用户已经确认采用 `AOT 主线 + Interpreter 热更 + Metadata Supplement` 方案，下一步不能直接进入实现，必须先冻结跨阶段依赖与验证标准，避免大规模返工。
- done_definition: roadmap 的阶段、依赖、验证方式和首批 child task 已冻结，后续可以从 `shared-contract-freeze` 与 `full-project-ingestion-and-build-graph` 开始派生子任务。

## 最近摘要

- 2026-04-09 23:10:00 +08:00: 复查当前主线 roadmap，确认当前仓库仍定位为 `performance-first core mainline`，而非完整 `C# -> C++ total solution`。
- 2026-04-09 23:14:00 +08:00: 复查平台 gate，确认 iOS 当前仅冻结为 `compile/link/packaging`，尚未进入完整移动端 runtime 语义。
- 2026-04-09 23:18:00 +08:00: 补充外部约束，确认 iOS 热更应优先按 `AOT + Interpreter` 思路设计，并需要额外考虑 App Store 2.5.2 带来的发行边界。
- 2026-04-09 23:42:00 +08:00: 用户确认采用推荐架构，已新增 `roadmap-v1-01.md`，冻结 9 个阶段以及各阶段验证 gate。
- 2026-04-11 10:00:00 +08:00: 架构审核完成，修订 design 和 roadmap：更新基线（Driver CLI 已重构）；Phase 0 收窄 engine ABI scope + 新增 version policy + IR 决策前置；Phase 1 标注 CLI 基础可复用；Phase 2 新增 linker + debug baseline；Phase 5 移除对 Phase 4 依赖；design 新增 Linker 和调试章节。

## 下一步

- next_action: 优先创建两个 child task：`shared-contract-freeze` 与 `full-project-ingestion-and-build-graph`。前者冻结共享 ABI/identity/manifest，后者把输入层升级到真实 project graph。
- owner: codex
- trigger: 用户指定先启动哪个 child task，或要求继续细化其中一个阶段。

## 风险 / 阻塞

### risks

- 如果把“完整 IL2CPP”与“热更”混成一条执行链，主线的 generated native 热路径会被动态装载需求反向污染。
- 如果忽略 iOS 的发行边界，只讨论技术可行性，后期很可能在上架策略上返工。
- 如果在 project ingestion 与 engine ABI 未冻结前就先做热更实现，metadata 和 bridge 会高概率返工。

### blockers

- 当前没有实现阻塞，主要等待用户确认先启动哪个 child task。
