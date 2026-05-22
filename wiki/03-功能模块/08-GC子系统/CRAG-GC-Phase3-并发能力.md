# CRAG-GC-Phase3 — 高级并发能力

> 对应路线图 Phase 3，子任务 gc-p4-01/gc-p4-02/gc-p4-03 一次性完成。
> 编写日期: 2026-05-22

## 1. Server GC 模式 (G-4, gc-p4-01)

### 设计决策

Young generation (nursery + Gen1 survivor) 在 Server GC 模式下**保持共享**，不按 heap 分割。只有 OldGen、LOH、Scheduler 是 per-NUMA-node 的。这一决策避免了 per-heap root scanning 和 card table 分区的复杂性，同时仍能实现并行 old-gen GC。

### 架构

```
WKS 模式 (CHAOS_IL2CPP_GC_SERVER=0):
  G_YoungGen() → g_young_gen (全局)
  G_OldGen()   → g_old_gen   (全局)
  G_Loh()      → g_loh       (全局)
  G_Scheduler()→ g_gc_scheduler (全局)

Server 模式 (CHAOS_IL2CPP_GC_SERVER=1):
  G_YoungGen() → g_young_gen           (共享)
  G_OldGen()   → tls_current_heap->old_gen (per-heap)
  G_Loh()      → tls_current_heap->loh     (per-heap)
  G_Scheduler()→ tls_current_heap->scheduler (per-heap)
  G_Gen1State()→ g_gen1_state (共享)
```

### 核心组件

| 组件 | 职责 |
|------|------|
| `GcHeapManager` | 管理 per-NUMA-node heap 数组，提供线程→heap 映射 |
| `GcCoordinator` | 多 heap GC 编排：获取全局 safepoint → Phase 1 共享 young GC → Phase 2 并行 old-gen GC |
| `GcHeapContext` | 每个 heap 的上下文包（old gen, LOH, scheduler） |
| `GcWorkerPool` | 用于并行执行 per-heap GC 的 worker 线程池 |

### 执行流程 (ExecuteMultiHeapGc)

```
1. 获取全局 safepoint
2. Phase 1: 共享 young GC + Gen1 collection（在共享 garden 上执行一次）
3. Phase 2: per-heap old-gen full GC（GcWorkerPool 并行执行）
4. 恢复线程 heap 绑定
5. 释放 safepoint
```

### 线程绑定

- `RegisterThread()` → `SetThreadHeap()` 根据 NUMA 节点绑定线程
- `UnregisterThread()` → `ClearThreadHeap()` 清除绑定
- `HeapForCurrentThread()` → 查询当前 NUMA 节点

## 2. 并发根扫描 (G-25, gc-p4-02)

### 问题

BGC 并发标记期间，mutator 线程可能覆盖根 slot（静态字段、GCHandle 目标等）。如果某个根 slot 持有某对象的唯一引用，覆盖后该对象可能被过早回收。

### 解决方案

Per-thread ring buffer 跟踪根 slot 修改，在 STW re-mark 阶段重新标记。

### 组件

| 组件 | 职责 |
|------|------|
| `ManagedThread::root_change_buffer_[64]` | 64-entry ring buffer |
| `ManagedThread::root_change_head_/tail_` | 生产者/消费者位置 |
| `BgcRecordRootChange(slot, old_value)` | 根 slot 写屏障中调用 |
| `BgcDrainRootChangeBuffer(mt, mark_fn)` | STW re-mark 阶段 drain |

### 数据流

```
Mutator: 覆盖 root slot
  → BgcRecordRootChange(slot, old_value)
  → 写入 ring buffer (CAS head)

STW Re-Mark:
  → EnumerateThreads → BgcDrainRootChangeBuffer
  → 对每个 entry: old_value 在 old-gen 且未标记 → BgcTryMark + PushToBgcMarkDeque
```

### 写入屏障插入点

根 slot 写屏障（静态字段赋值、GCHandle 目标修改）需要额外调用 `BgcRecordRootChange`。当前实现提供基础设施，具体屏障插入由后续集成完成。

## 3. NUMA 感知空闲页回收 (G-23, gc-p4-03)

### 问题

旧 gen 空闲页池（`page_pool_`）不感知 NUMA 节点。AllocatePage 从池中弹出页时，可能从远程 NUMA 节点分配，增加跨节点访问延迟。

### 解决方案

池条目（`PoolEntry`）增加 `numa_node` 字段，AllocatePage 优先匹配当前 NUMA 节点。

### 改动

| 文件 | 改动 |
|------|------|
| `gc_old_gen.h` | `PoolEntry` 增加 `int8_t numa_node` |
| `gc_old_gen.cpp` | `AllocatePage` 池搜索优先匹配当前 NUMA 节点 |
| `gc_old_gen.cpp` | 回收页时保留 `numa_node` |
| `gc_old_gen.cpp` | BgcSweep 裁剪池时按节点公平保留 |

### 限流

`kMaxPoolSize = 16` 限制池大小。当超出时，优先释放远程 NUMA 节点的空闲页。

## 4. 文件改动清单

### gc-p4-01 (Server GC 模式)

| 文件 | 改动 |
|------|------|
| `CMakeLists.txt` | 添加 Server GC 源文件 + `CHAOS_IL2CPP_GC_SERVER` option |
| `gc_features.h` | 添加 CHAOS_IL2CPP_GC_SERVER 文档 |
| `gc_heap.h` | 访问器函数 + 共享 young gen + Server GC 模式 per-heap 路由 |
| `gc_heap_manager.h/cpp` | GcHeapManager 实现 + per-heap old_gen.Init() |
| `gc_coordinator.h/cpp` | multi-heap GC 编排 |
| `gc_region.cpp` | chaos_gc_collect 路由到 GcCoordinator |
| `thread_state.cpp` | RegisterThread/UnregisterThread 加入 heap 绑定 |

### gc-p4-02 (并发根扫描)

| 文件 | 改动 |
|------|------|
| `thread_state.h` | ManagedThread 添加 root change ring buffer |
| `gc_root_change.h` | BgcRecordRootChange + BgcDrainRootChangeBuffer |
| `gc_bgc.h` | PushToBgcMarkDeque 公开方法 |
| `gc_bgc.cpp` | StwRemark 集成 root change buffer drain |

### gc-p4-03 (NUMA 感知空闲页)

| 文件 | 改动 |
|------|------|
| `gc_old_gen.h` | PoolEntry 增加 numa_node |
| `gc_old_gen.cpp` | AllocatePage NUMA 优先 + 回收保留 numa_node |

### Accessor 迁移

~17 个文件中的 200+ 处 `g_young_gen`/`g_old_gen`/`g_gc_scheduler`/`g_loh` 替换为 `G_YoungGen()`/`G_OldGen()`/`G_Scheduler()`/`G_Loh()`，实现 WKS/Server 双模式切换。

## 5. 构建与测试

| 构建 | 结果 |
|------|------|
| MSBuild chaos_runtime_core (RelWithDebInfo) | ✅ PASS |
| 现有 GC 测试 (5 个套件) | ✅ 无回归（仅保留 3 个预先存在的失败） |

## 6. 已知限制

- Server GC 模式默认编译关闭（`CHAOS_IL2CPP_GC_SERVER=OFF`），需要手动打开
- 多 NUMA 节点环境需要额外的 OOM 测试
- 并发根扫描的接入点（root slot 写屏障）需后续集成到静态字段和 GCHandle 写入路径
- 预存的 `synchronization.cpp` 和 `entry_direct.cpp` 编译错误需要独立修复
