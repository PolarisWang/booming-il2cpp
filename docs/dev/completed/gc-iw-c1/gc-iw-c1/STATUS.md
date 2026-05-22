---
task_id: gc-iw-c1
title: LatencyMode 差异化行为实现（C11）
task_type: plan
lifecycle_status: in-progress
phase: writing-plans
parent_task_id: 20260523-crag-gc-industrialization-sequel
source_relation: roadmap-child
clearance_source: parent-roadmap
created_at: 2026-05-23
updated_at: 2026-05-23
current_dir: docs/dev/in-progress/gc-iw-c1
---

# STATUS — gc-iw-c1：LatencyMode 差异化行为实现（C11）

## 设计摘要

父 roadmap 已确认 C11 边界：LatencyMode 差异化行为实现（Batch/LowLatency/SustainedLowLatency）。当前 `GcLatencyMode` 枚举已存在但 `SetLatencyMode()`/`GetLatencyMode()` 仅做原子存储，`DecideCollection()` 只对 LOW_LATENCY/SUSTAINED_LOW_LATENCY 偏好 BGC 而非 STW FULL。BATCH 模式与 INTERACTIVE 无区别。

### preflight self-review

- 范围检查：在父 roadmap 已批准的 C11 边界内（gc_scheduler.cpp, gc_api.cpp）
- 依赖检查：Phase B 全部完成，GC 引擎已硬化
- authority 检查：改 gc_scheduler 调度决策 + gc_api 模式切换，不改 GC 核心算法
- preflight_review: pass
- auto_execution_decision: continue
