---
task_id: 20260408-01-unified-project-generation-entrypoint
title: 统一入口工程生成与可调试工作区
task_type: roadmap
lifecycle_status: in_progress
phase: roadmap
created_at: 2026-04-08 11:25:00 +0800
updated_at: 2026-04-08 11:42:00 +0800
current_dir: docs/dev/in-progress/20260408-01-unified-project-generation-entrypoint
parent_task_id:
source_task_id:
source_relation:
active: true
---

## 关键文档
- brainstorm: `docs/dev/in-progress/20260408-01-unified-project-generation-entrypoint/brainstorm-v1-01.md`
- design: `docs/dev/in-progress/20260408-01-unified-project-generation-entrypoint/design-v1-01.md`
- roadmap_or_plan: `docs/dev/in-progress/20260408-01-unified-project-generation-entrypoint/roadmap-v1-01.md`
- latest_progress: 无

## 当前判断
- current_focus: 冻结 `generate project subject` 的 roadmap，明确 `subject-exec` 手动刷新与后续 phase 边界。
- why_now: 用户已确认先做 subject 的 `generate project`，并同意 `subject-exec` 采用手动刷新。
- done_definition: 基于 roadmap 派生 Phase 1 child plan，只覆盖 `generate project subject` 设计与实现入口。

## 最近摘要
- 2026-04-08 11:25:00 +0800: 已完成统一入口、subject/native/core 现状扫描，确认当前仓库缺少正式的 project/workspace 生成能力。
- 2026-04-08 11:25:00 +0800: 已形成方案对比，推荐“引用仓库真源的工作区生成器”而不是源码快照导出。
- 2026-04-08 11:25:00 +0800: 已判断本任务适合进入 roadmap，而不是直接写单阶段 plan。
- 2026-04-08 11:42:00 +0800: 用户确认 `subject-exec` 采用手动刷新，并要求统一入口下一步优先加入 `generate project subject`。
- 2026-04-08 11:42:00 +0800: 已生成父 roadmap，阶段顺序固定为 subject generation -> subject build -> core/target expansion。

## 下一步
- next_action: 派生 Phase 1 child plan，只定义 `generate project subject` 的命令面、输出布局和 `subject-exec` 手动刷新行为。
- owner: codex
- trigger: 用户要求继续推进设计并进入 Phase 1 child plan。

## 风险/阻塞
### risks
- 如果用户期望的是“完全独立可搬运工程副本”，当前推荐方案需要改成导出型工作区，复杂度会显著上升。
- subject native 调试工程与 canonical run-scoped artifact 的边界必须写清，否则后续容易把 `artifacts/projects/**` 误当正式证据。
- Windows 一等公民与 cross-host 一等公民是两种不同范围；如果要首版同时覆盖 VS/macOS/Xcode/Linux IDE，任务会进一步扩张。

### blockers
- 无
