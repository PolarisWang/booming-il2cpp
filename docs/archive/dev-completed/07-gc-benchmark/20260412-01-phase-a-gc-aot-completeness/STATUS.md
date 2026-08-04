---
task_id: 20260412-01-phase-a-gc-aot-completeness
title: Phase A — GC 集成 + AOT 运行时关键补完
task_type: plan
lifecycle_status: completed
phase: archived
created_at: 2026-04-12 08:00:00 +08:00
updated_at: 2026-04-12 12:00:00 +08:00
current_dir: docs/dev/completed/20260412-01-phase-a-gc-aot-completeness
parent_task_id: "20260411-10-post-phase7-development-roadmap"
source_task_id: "20260411-10-post-phase7-development-roadmap"
source_relation: child_of_roadmap_phase_A
active: false
---

## 关键文档

- parent_roadmap: docs/dev/in-progress/20260411-10-post-phase7-development-roadmap/roadmap-v1-01.md
- plan: docs/dev/in-progress/20260412-01-phase-a-gc-aot-completeness/plan-v1-01.md
- latest_progress: docs/dev/in-progress/20260412-01-phase-a-gc-aot-completeness/notes/progress-v1-01.md

## 当前判断

- current_focus: BDWGC 接入 (A.1-A.6) 为第一批，vtable/EH/泛型/委托/opcode (A.7-A.11) 为第二批
- why_now: 父 roadmap 已激活，Phase A 是所有后续 Phase 的前置条件
- done_definition: MainlineFeaturePack 全量 capability 在 Windows x64 通过，连续分配 100 万对象无 crash

## 最近摘要

- 2026-04-12 08:00:00 +08:00: 创建 Phase A 执行计划

## 下一步

- next_action: 执行 Batch 1 — A.1 BDWGC 构建集成
- trigger: 立即

## 风险

### risks
- BDWGC win32_threads.c 可能需要额外 WIN32 宏配置
- ObjectHeader 扩展 vtable 指针后 size 变化，需更新所有 sizeof 引用
- A.11 opcode 扩展工作量大，建议分迭代

### blockers
- 无
