---
task_id: gc-iw-b3
title: CrossPageCompact 空转优化（C8）
task_type: plan
lifecycle_status: completed
phase: completed
parent_task_id: 20260523-crag-gc-industrialization-sequel
source_relation: roadmap-child
clearance_source: parent-roadmap
created_at: 2026-05-23
updated_at: 2026-05-23
current_dir: docs/dev/completed/gc-iw-b3
---

# STATUS — gc-iw-b3：CrossPageCompact 最小存活阈值优化（C8）

## 问题

`CrossPageCompact()` 以 `kCrossPageFragThreshold=0.30`（即 30% 以上空闲率）为阈值选择候选页面。但即使碎片率达标，若页面存活对象极少（如仅数个对象共几百字节），迁移开销（PlanPageEvacuation + GlobalRelocate + memcpy/memset）远超收益，导致 STW 时间浪费在空转上。

## 方案

在 `CrossPageCompact()` 的候选页选择阶段，增加每页最小存活字节阈值 `kMinCrossPageSurvivorBytes`。当页面存活对象总字节低于阈值时，跳过该页的 compaction，避免因碎片的微量收益付出全套迁移成本。

## 完成证据

- arch_review: ok（常量 + if-continue，无结构影响）
- authority_review: n/a（GC 引擎内部数值策略调整）
- verification:
  - canonicalCommand: cmake build chaos_runtime_core → gc_stress_test → gc_old_gen_test → foundation-dll pipeline
  - formalObject: gc_old_gen.h (kMinCrossPageSurvivorBytes), gc_old_gen.cpp (threshold check in CrossPageCompact)
  - unitTest: 6/6 (old_gen), 4/7 (sanity — 3 POH failures are pre-existing)
  - stressTest: 3/3 (gc_stress_test)
  - pipeline: 10/10 (garbage-collection family)
  - entryExe: Passed: 10/10
- test_result: passed
- wiki: n/a
- next: gc-iw-b4（retired_arrays_ 泄漏修复）
