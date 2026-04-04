---
task_id: 20260329-01-il2cpp-like-cs-to-cpp
title: IL2CPP-Like C# to C++ 父 Roadmap
task_type: roadmap
lifecycle_status: hanging
phase: roadmap
created_at: 2026-03-29 23:37:57 +0800
updated_at: 2026-04-04 14:20:39 +0800
current_dir: docs/dev/hanging/20260329-01-il2cpp-like-cs-to-cpp
parent_task_id:
source_task_id:
source_relation:
active: false
---

## 关键文档
- brainstorm: `docs/dev/hanging/20260329-01-il2cpp-like-cs-to-cpp/brainstorm-v1-01.md`
- design: `docs/dev/hanging/20260329-01-il2cpp-like-cs-to-cpp/design-v1-01.md`
- roadmap_or_plan: `docs/dev/hanging/20260329-01-il2cpp-like-cs-to-cpp/roadmap-v1-01.md`
- latest_progress: `docs/dev/hanging/20260329-01-il2cpp-like-cs-to-cpp/notes/progress-v1-02.md`

## 当前判断
- current_focus: 顶层设计已经完成，并已进入父 roadmap 状态。
- why_now: `Roadmap 0` 已执行完成，后续 roadmap 尚未启动，父任务当前应保持挂起而不是完成。
- done_definition: 父 roadmap 文档稳定，且后续子任务可从本目录继续派生。

## 最近摘要
- 2026-04-04 14:20:39 +0800: 已补父 roadmap 的子任务映射状态表；`Roadmap 1` 与 `Roadmap 3` 现为 `ready`，其余保持 `planned`。
- 2026-04-04 14:09:00 +0800: 顶层设计目录已与父 roadmap 目录合并，转为挂起中的父任务。
- 2026-04-04 14:09:00 +0800: `Roadmap 0` 已完成，`Roadmap 1` 到 `Roadmap 6` 仍未开始。

## 下一步
- next_action: 未来恢复时，优先从 `Roadmap 1` 或 `Roadmap 3` 派生新子任务。
- owner: codex
- trigger: 需要继续推进下一个 roadmap 子任务时恢复。

## 风险/阻塞
### risks
- 旧文档正文保留历史文件名和 `docs/booming/...` 路径，当前作为兼容信息保留。

### blockers
- 无

## 关联任务
### parent
- 无

### source
- 无

### children
- `20260331-01-roadmap-0-contract-freeze-and-skeleton`：completed
- `20260404-03-roadmap-1-assembly-and-typed-il-ir`：ready
- `roadmap-2-aot-closure-and-manifest`：planned
- `20260404-04-roadmap-3-runtime-core-v1`：ready
- `roadmap-4-codegen-bridge-and-e2e`：planned
- `roadmap-5-mvp-and-golden-projects`：planned
- `roadmap-6-cross-platform-and-phase-2-3`：planned
