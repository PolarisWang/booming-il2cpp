---
task_id: gc-iw-e1
title: NUMA 完善 + Card bundle + CPU 亲和性（C14/C16/C17）
task_type: plan
lifecycle_status: completed
phase: completed
parent_task_id: 20260523-crag-gc-industrialization-sequel
source_relation: roadmap-child
clearance_source: parent-roadmap
preflight_review: pass
auto_execution_decision: continue
created_at: 2026-05-23
updated_at: 2026-05-23
current_dir: docs/dev/completed/gc-iw-e1
---

# STATUS — gc-iw-e1：NUMA 完善 + Card bundle + CPU 亲和性（C14/C16/C17）

## 完成证据

- arch_review: ok
- authority_review: n/a
- verification:
  - canonicalCommand: ctest --test-dir build/testing -C Debug -R "test_gc_" -L unit --output-on-failure
  - formalObject: gc-region (PASS), gc-card-table-ext (PASS), gc-scheduler (PASS), gc-managed-api (PASS), gc-worker-pool (PASS), gc-coordinator (PASS), gc-heap-manager (PASS)
  - summaryPath: docs/dev/completed/gc-iw-e1/STATUS.md
- test_result: passed (7/8, 1 pre-existing failure: gc-sanity — POH region count + GcMemoryInfo, not caused by gc-iw-e1)
- wiki: n/a
- next: parent-roadmap -> gc-iw-e2 (写屏障假阳性消除 + OS 低内存通知)

## 改动文件

### C14 — NUMA 完善（GcNumaNodeOfAddress）

| 文件 | 改动 |
|------|------|
| `gc_heap_manager.h` | 新增 `GcGetHeapNumaNodeForAddress()` 声明 |
| `gc_heap_manager.cpp` | 实现 `GcGetHeapNumaNodeForAddress()` — 多阶段地址→NUMA 节点解析 |

实现了一个封装函数 `GcGetHeapNumaNodeForAddress(void* addr)`，包含四阶段解析：
1. Nursery 范围检查 → 返回当前线程 NUMA 节点
2. Old-gen 页表查找（O(log n) via FindPage）→ 返回页的 `numa_node` 字段
3. LOH 段检查 → 返回所属堆的 NUMA 节点
4. OS 级回退 → GcNumaNodeOfAddress()

### C16 — Card bundle 批量处理

| 文件 | 改动 |
|------|------|
| `gc_card_table.h` | CardSegment 新增 `dirty_bundle` 字段；DirtyCard 设置 bundle 标志；ScanDirtyCards/ScanDirtyCardsBatched 增加 bundle 快速跳过 |
| `gc_card_table.cpp` | ClearAllCards 清除 bundle 标志 |

添加 bundle 位（`std::atomic<uint8_t>`）到每个 CardSegment。DirtyCard 同时设置 card 字节和 bundle 位。扫描路径先检查 bundle（O(1) 干净段跳过），再回退到 SIMD 128 字节扫描。bundle 位是单调递增的（mutator 只置 1，GC 在 safepoint 清零），消除了竞态条件。

### C17 — CPU 亲和性

| 文件 | 改动 |
|------|------|
| `gc_bgc.cpp` | BgcThreadMain 启动时绑定到 NUMA 节点 0 |
| `gc_worker_pool.cpp` | WorkerLoop 启动时按 `worker_idx % node_count` 绑定 NUMA 节点 |

BGC 线程和 GcWorkerPool 工作线程现在在创建时绑定到特定的 NUMA 节点，确保内存访问局部性。
