---
task_id: gc-p2-04
title: Finalizer OOM 保证：预留内存/专用堆（G-12）
task_type: plan
lifecycle_status: completed
phase: completed
created_at: 2026-05-22 19:40:00 +08:00
updated_at: 2026-05-22 19:40:00 +08:00
current_dir: docs/dev/in-progress/gc-p2-04-finalizer-oom
parent_task_id: 20260522-gc-industrialization
source_relation: roadmap-child
clearance_source: parent-roadmap
blocking_questions: []
question_clearance: cleared
clearance_confirmed_by_user: true
---

# STATUS — gc-p2-04: Finalizer OOM 保证

## 目标

为 GC 添加紧急内存预留机制：当 Finalizer 线程需要分配托管内存但 GC 堆已满时，使用预分配的紧急保留内存，防止 Finalizer 死锁。

## 设计方案

### 问题
Finalizer 线程运行 C# 对象的 finalizer 方法，这些方法可能分配新对象。当 GC 堆已满（OOM）且无法通过 GC 回收足够内存时，Finalizer 线程的 `NurseryAllocate` 返回 nullptr，导致：
1. Finalizer 因分配失败无法推进
2. 内存无法释放（因为 Finalizer 未运行完）
3. 系统死锁

### 方案
在 `MarkSweepOldGen` 中添加紧急保留内存池（64KB），Finalizer 线程可在 OOM 时从中分配。

### 实现步骤
| # | 文件 | 改动 |
|---|------|------|
| 1 | `gc_old_gen.h` | 添加紧急保留字段和方法 |
| 2 | `gc_old_gen.cpp` | 实现 InitEmergencyReserve、AllocateFromEmergencyReserve、ReplenishEmergencyReserve |
| 3 | `gc_bgc.h` | 添加 IsFinalizerThread 和 finalizer_thread_id_ |
| 4 | `gc_bgc.cpp` | FinalizerThreadMain 中记录 thread id；BGC 循环结束后补充保留区 |
| 5 | `gc_region.cpp` | NurseryAllocateSlow 中 g_old_gen.Allocate 失败后尝试紧急保留区 |
| 6 | `gc_api.cpp` | Collect 路径中补充保留区 |

## 结果

- **Emergency Reserve**: 64KB bump-pointer pool with CAS thread safety, zeroed on allocation
  - `AllocateFromEmergencyReserve`: pointer-aligned, WARN log on activation
  - `ReplenishEmergencyReserve`: resets + zeroes at end of each GC cycle
  - `InitEmergencyReserve`: allocated via GcNumaVirtualAlloc, non-fatal WARN on failure
- **Finalizer Thread Detection**: `BgcController::IsFinalizerThread()` via `std::thread::id` comparison
- **Integration Points**:
  - `NurseryAllocateSlow` Phase 4: fallback after `g_old_gen.Allocate` fails
  - BGC main loop: replenish before `cycle_complete_`
  - `ForceComplete()`: replenish before setting phase to IDLE
  - `Collect()`: replenish at end
- **Tests**: 12/12 finalizer tests pass (8 original + 4 emergency reserve tests)

## 测试验证

```
chaos_gc_finalizer_test: 12 tests, 0 failures
All GC tests (63 total across 9 targets): PASS
```
- [x] 紧急保留区在 Init 中正确分配
- [x] Finalizer 线程 OOM 时能用保留区分配
- [x] GC 循环结束后保留区被补充
- [x] 全部 GC 测试 PASS（63 tests across 9 targets, 0 failures）
