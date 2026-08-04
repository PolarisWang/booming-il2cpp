---
task_id: gc-iw-b2
title: Parallel mark lock-free deque 改造（C7）
task_type: plan
lifecycle_status: completed
phase: completed
parent_task_id: 20260523-crag-gc-industrialization-sequel
source_relation: roadmap-child
clearance_source: parent-roadmap
created_at: 2026-05-23
updated_at: 2026-05-23
current_dir: docs/dev/completed/gc-iw-b2
---

# STATUS — gc-iw-b2：Parallel mark lock-free deque 改造（C7）

## 问题

当前 `gc_parallel_mark.cpp` 中每个 worker 使用 `std::vector<MarkChunk> deque` + `std::mutex steal_mutex` 保护。所有 deque 操作（push/pop/steal）都通过 mutex 序列化，导致：

1. `StealChunk` 锁 victim mutex — 高频偷取时产生严重锁争用
2. `PushChunk`/`PopChunk` 也需要锁（尽管是 single-producer），因为 steal 可能并发访问

## 方案

用 Chase-Lev lock-free work-stealing deque 替换 `std::vector + std::mutex`。Chase-Lev 是经典 lock-free deque 算法，CoreCLR GC、TBB、Java ForkJoinPool 都使用该算法。

关键特性：
- Owner (local worker) push/pop：无锁，只操作 `bottom` 指针
- Thief (remote worker) steal：只操作 `top` 指针
- 冲突时通过 CAS 协调
- 数组通过 power-of-2 掩码实现循环缓冲

## 范围

- `src/native/runtime-core/gc/gc_parallel_mark.h` — 新增 LockFreeDeque 类，替换 MarkWorkerState 中的 deque + steal_mutex
- `src/native/runtime-core/gc/gc_parallel_mark.cpp` — 更新 PushChunk/PopChunk/StealChunk 为 lock-free 操作，移除所有 steal_mutex 使用

## 完成证据

- arch_review: ok
- authority_review: ok
- verification:
  - canonicalCommand: native build (chaos_runtime_core.lib) + gc_parallel_mark_test + gc_stress_test + foundation-dll garbage-collection pipeline
  - formalObject: gc_parallel_mark.h / gc_parallel_mark.cpp 中的 LockFreeDeque 实现
  - unitTest: 6/6 (PushPopLocalLifo, StealFromIdle, StealFromBusy, StealAll, InitDestroy, FlushPending)
  - stressTest: 3/3 (Light_4Threads, 100Threads_MixedSizes, Gen1Pressure)
  - pipeline: 10/10 (garbage-collection family, fact mode)
- test_result: passed
- wiki: n/a
- next: gc-iw-b3（CrossPageCompact 空转优化）
