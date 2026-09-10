---
task_id: gc-iw-b5
title: Finalizer 线程池替代 std::async（C10）
task_type: plan
lifecycle_status: completed
phase: completed
parent_task_id: 20260523-crag-gc-industrialization-sequel
source_relation: roadmap-child
clearance_source: parent-roadmap
created_at: 2026-05-23
updated_at: 2026-05-23
current_dir: docs/dev/completed/gc-iw-b5
---

# STATUS — gc-iw-b5：Finalizer 线程池替代 std::async（C10）

## 问题

BGC 原先使用专用 std::thread 做 finalizer 线程。该线程与 BGC 主线程之间通过 condition_variable 同步。该设计在 BGC 停止/启动周期中存在竞态——finalizer 线程在 BGC 停止后可能继续处理已发布的工作，导致 thread leak。

## 方案

移除专用 finalizer 线程，改为在 BGC 线程上内联执行 finalization（`ProcessFinalizersInline()`）。当 BGC 周期完成时，BGC 线程直接处理 `bgc_dead_finalizables_` 和 `bgc_dead_weak_handles_` 而非发布到独立线程。

### 改动文件

| 文件 | 改动 |
|------|------|
| `gc_bgc.h` | 移除 finalizer_thread_/finalizer_cv_/finalizer_mutex_；添加 ProcessFinalizersInline()、IsBgcThread()、bgc_thread_id_ |
| `gc_bgc.cpp` | 替换 PublishFinalizationWork()+FinalizerThreadMain() 为 ProcessFinalizersInline() |
| `gc_region.cpp` | IsFinalizerThread() → IsBgcThread() (BGC 线程 OOM guarantee) |
| `type_registry.cpp` | 修复 const std::mutex 编译错误 |

### 验证

| 场景 | 结果 |
|------|------|
| test_gc_bgc_root_scan（4 测试） | 4/4 PASS |
| test_gc_old_gen（6 测试） | 6/6 PASS |
| test_gc_bgc_unit（8 测试） | 8/8 PASS |
| test_gc_stress（3 测试） | 3/3 PASS |

## 完成证据

- arch_review: ok（移除整个 finalizer 线程状态机，架构更干净）
- authority_review: n/a
- verification:
  - unitTest: 18/18 (root_scan + old_gen + bgc_unit)
  - stressTest: 3/3 (gc_stress)
- test_result: passed
- wiki: n/a
- next: gc-iw-c1（LatencyMode 差异化行为实现）
