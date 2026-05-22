# GC 完成度评估：CRAG vs CoreCLR

> 评估日期：2026-05-22
> 范围：`src/native/runtime-core/gc/`（45 文件，17,116 行）+ `testing/src/native/runtime-core/gc/`（44 文件，~200+ 测试用例）

---

## 一、CRAG GC 架构总览

CRAG（Chaos Region-Aware GC）采用 region-based generational GC，核心组件：

| 组件 | 文件 | 行数 | 状态 |
|------|------|------|------|
| Region 框架 | gc_region.h/cpp | 415+1190 | 成熟 |
| Young Collector (Nursery) | gc_young_collector.h/cpp | 65+710 | 成熟 |
| Old Generation | gc_old_gen.h/cpp | 618+3333 | 成熟 |
| Gen1 (Survivor) | gc_gen1.h/cpp | 110+685 | 成熟 |
| BGC (Background) | gc_bgc.h/cpp | 508+1677 | 成熟 |
| LOH | gc_loh.h/cpp | 156+473 | 成熟 |
| POH | gc_region.h (POH region kind) | 嵌入 region | 成熟 |
| Card Table | gc_card_table.h/cpp | 301+211 | 成熟 |
| Scheduler (EMA) | gc_scheduler.h/cpp | 603+637 | 成熟 |
| Parallel Mark | gc_parallel_mark.h/cpp | 134+380 | 成熟 |
| Worker Pool | gc_worker_pool.h/cpp | 87+116 | 成熟 |
| Root Scanner | gc_root_scanner.h/cpp | 102+174 | 成熟 |
| Mark Bitmap | gc_mark_bitmap.h | 115 | 成熟 |
| GC API | gc_api.h/cpp | 162+526 | 成熟 |
| Events | gc_events.h/cpp | 199+256 | 成熟 |
| Stats | gc_stats.h/cpp | 290+156 | 成熟 |
| NUMA | gc_numa.h + 3 platform impls | 82+193+224+85 | 基础 |
| Heap Manager | gc_heap_manager.h/cpp | 97+61 | 基础 |
| Coordinator | gc_coordinator.h/cpp | 66+89 | 基础 |
| Layout Registry | gc_layout.h/cpp | 308+311 | 成熟 |
| Bump Cache | gc_bump_cache.h | 237 | 成熟 |
| Features | gc_features.h | 52 | 定义 |
| **合计** | | **17,116** | |

---

## 二、CoreCLR 功能对等表

### 2.1 分代结构

| CoreCLR 功能 | CRAG 状态 | 差异与备注 |
|---|---|---|
| Gen0 (Nursery) | ✅ 完成 | Cheney BFS copying, 4MB-256MB 自适应 |
| Gen1 (Survivor) | ✅ 完成 | 动态 promotion age [1,12], Gen1 collect→Old |
| Gen2 (Old) | ✅ 完成 | 3333 lines, 28 size classes (16B-32KB), mark-sweep |
| LOH (>=85KB) | ✅ 完成 | segment-based, 473 lines, sweep+可选 compaction |
| POH (Pinned) | ✅ 完成 | 作为 region kind 实现 |
| Frozen/ROH | ❌ 缺失 | 无 freeze 语义 region |
| Gen3 (Large Object) | ❌ 缺失 | LOH 与 Old 独立但无独立 Gen3 代数计数 |

### 2.2 收集类型

| CoreCLR | CRAG | 差异 |
|---|---|---|
| 0 (Batch/Blocking) | ✅ Young/Full GC | Young ~STW evacuate, Full ~STW mark-sweep |
| 1 (Background/BGC) | ✅ BGC | SATB concurrent mark-sweep, 1677 lines |
| 2 (Concurrent/Server) | ⚠️ 基础 | 仅 infrastructure: coordinator + heap manager |
| Blocking Gen2 | ✅ Full GC collect path | Full GC 调用 OldGen::Collect |
| Induced GC (GC.Collect) | ✅ chaos_gc_collect | 支持阻塞式+后台模式 |
| Compact/LOH compact | ⚠️ 部分 | LOH compact 存在但无 Gen2 compact |

### 2.3 GC 算法

| CoreCLR | CRAG | 差异 |
|---|---|---|
| Mark-Sweep | ✅ OldGen/BGC | Parallel mark 380 lines, bit array + work-stealing |
| Plan/Steady (copying) | ✅ Young Cheney BFS | forwarding pointer (bit0 tagged) |
| SATB pre-write barrier | ✅ BGC | per-thread 1024-entry buffer |
| Card table post-write | ✅ L1/L2 card table | 512 bytes/card, 2-level, ~6 instructions |
| Mark-Stack work-stealing | ✅ | ParallelMark work-stealing |
| Pin handling | ✅ POH region | Pinned objects allocated in separate region |
| Demotion | ❌ 缺失 | 无 demotion 路径 |
| Segment deletion/merging | ⚠️ | Region manager 支持 return-to-free, 但无 segment compaction |

### 2.4 内存管理

| CoreCLR | CRAG | 差异 |
|---|---|---|
| VirtualAlloc segments | ⚠️ 部分 | Region 使用 GC_MALLOC, 非 VirtualAlloc |
| Large pages | ⚠️ 基础 | 有 flag 和 NUMA 查询, 非默认 |
| Segment list | ✅ Region list | GcRegionManager 持有 free/active region 列表 |
| SOH/LOH segment split | ✅ Region kinds | NURSERY/TENURED/LOH 不同 region kind |
| OOM handling | ⚠️ | old_gen 有 fallback paths, 但缺乏系统级 OOM 事件 |
| Memory pressure | ✅ | AddMemoryPressure/RemoveMemoryPressure 在 gc_api |

### 2.5 线程交互

| CoreCLR | CRAG | 差异 |
|---|---|---|
| Cooperative safepoint | ✅ gc_transition.h | Polling-based (chaos_gc_safepoint_poll) |
| Thread hijacking | ❌ 缺失 | CoreCLR 可 suspend 线程执行, CRAG 只有 cooperative poll |
| GcJoin (GC join) | ✅ BGC worker pool | GcWorkerPool::RunWorkers |
| Reentrant GC guard | ✅ gc_coordinator | compare_exchange_strong 防止重入 |
| GC mode (coop/preempt) | ⚠️ | 有 GcTransition, 但缺乏全局 preemptive mode 切换 |
| GcThread static data | ✅ | GcThreadStatic 在 gc_api |

### 2.6 Handle 类型

| CoreCLR | CRAG | 差异 |
|---|---|---|
| Strong Handle | ✅ HNDTYPE_DEFAULT | gc_handle_test verified |
| Weak (short/long) | ✅ HNDTYPE_WEAK | gc_handle_test verified |
| Pinned Handle | ✅ HNDTYPE_PINNED | gc_poh_test verified |
| Dependent Handle | ✅ HNDTYPE_DEPENDENT | gc_handle_test verified |
| Weak WinRT | ❌ 缺失 | WinRT-specific, N/A for il2cpp |
| RefCounted Handle | ❌ 缺失 | COM interop, N/A (for now) |
| Async pinned handle | ❌ | Not yet needed |
| Handle table | ✅ GcHandleTable | 在 gc_api.cpp |

### 2.7 高级功能

| CoreCLR | CRAG | 差异 |
|---|---|---|
| Finalization | ⚠️ 部分 | `chaos_gc_register_finalizable` 是 stub！old_gen 有 ProcessFinalizer 逻辑但 finalizer 查找未接 |
| WeakTable (ConditionalWeakTable) | ❌ 缺失 | 无对应实现 |
| GCNotification | ⚠️ | gc_events 有 callback 注册但无 GCNotification API |
| GCRoot registration | ✅ | gc_static_roots.h/cpp |
| GC.GetGeneration | ⚠️ | 可通过 region kind 推断但无直接 API |
| GC.CollectionCount | ✅ | gc_api 有 counter |
| GCMemoryInfo | ⚠️ | gc_sanity_test 有验证但 API 不完全 |
| Object pinning | ✅ POH | POH region + pin handle |
| Deploy (ephemeral promotion) | ✅ YoungGen | Self-healing through BFS pointer update |
| Write-watch (Eager/LOB) | ❌ 缺失 | CoreCLR 用于 dirty tracking 优化 |
| GC stress mode | ✅ CHAOS_IL2CPP_GC_STRESS | gc_stress.h/cpp, 8 lines |
| Hard/Soft limits | ❌ 缺失 | CoreCLR 的 GCHeapHardLimit |
| DATAS (Dynamic Adaptation) | ⚠️ | Scheduler 有 EMA 但无 CoreCLR-level DATAS |

### 2.8 诊断

| CoreCLR | CRAG | 差异 |
|---|---|---|
| ETW events | ⚠️ gc_events.h/cpp | Events 框架存在但未接 ETW |
| GC stats snapshot | ✅ gc_stats.h/cpp | GcGetSnapshot() 返回完整统计数据 |
| GC count / pause timing | ✅ | young/gen1/full pause ns total 全部统计 |
| Memory load tracking | ⚠️ | 有部分监控但无 CoreCLR 级 memory load 检测 |
| stress_log | ✅ | GC stress |

---

## 三、已识别的关键缺口

### P0 — 功能不完整（影响正确性）

| # | 缺口 | 位置 | 影响 |
|---|------|------|------|
| 1 | **终结器未接** | `gc_api.cpp:452` `chaos_gc_register_finalizable` 是 stub | 所有带 finalizer 的对象不会正确回收。`finalizer_test` 通过仅因测试自包含 mock |
| 2 | **kMaxPromoteObjects=256K 静默溢出** | `gc_young_collector.cpp` | 大 nursery 中 promotion 超限静默跳过 |
| 3 | **Gen1 budget trigger 死代码** | `gc_gen1.cpp` | Budget trigger 逻辑被 promotion_age_threshold 替代但代码未清理 |

### P1 — 功能不完整（影响性能/可靠性）

| # | 缺口 | 位置 | 影响 |
|---|------|------|------|
| 4 | **SATB buffer overflow 未测试** | bgc 使用 SATB | 多线程并发分配时可能丢失引用变更 |
| 5 | **BGC + LOH 并发未测试** | gc_bgc.cpp + gc_loh.cpp | 后台 GC 中 LOH 分配/释放无测试 |
| 6 | **Gen1 compaction 测试未覆盖** | gc_gen1_test.cpp | 只有普通 collect, 无碎片化后 compaction 验证 |
| 7 | **BGC 并发 mark-new-roots (G-25) 未测试** | gc_bgc_root_scan_test.cpp | 仅有 2 tests, 高并发场景未覆盖 |

### P2 — 缺失功能（产品级必需）

| # | 缺口 | 预计工作量 |
|---|------|-----------|
| 8 | Thread hijacking（线程悬挂） | ~2 周 |
| 9 | FH (Finalizer) region kind（而非内存池扫描） | ~1 周 |
| 10 | Server GC 默认启用 + 集成测试 | ~2 周 |
| 11 | Demotion 路径（Gen1→Nursery） | ~1 周 |
| 12 | OOM 事件 + 处理 | ~1 周 |
| 13 | GCNotification API | ~3 天 |
| 14 | GCMemoryInfo 完整实现 | ~1 周 |
| 15 | DATAS / 自适应策略增强 | ~2 周 |
| 16 | 硬件 write barrier（ARM64 FEAT_MTE） | ~估值 |

### P3 — 优化/工具

| # | 缺口 | 说明 |
|---|------|------|
| 17 | 硬/软内存限制（GCHeapHardLimit） | 容器化部署需要 |
| 18 | ETW/PerfView 集成 | 接入 Windows ETW 提供 GC events |
| 19 | Large pages 默认启用 | 减少 TLB miss |
| 20 | VirtualAlloc backing（而非 GC_MALLOC） | 更好的 memory pressure 控制 |
| 21 | Write-watch / eager dirty tracking | 减少 card table 写屏障开销 |

---

## 四、测试覆盖评估

### 4.1 现有测试资产（35 targets, ~200+ test cases）

| Batch | 覆盖 | 评估 |
|-------|------|------|
| A: 单元测试 | bit utils, bump cache, layout | 良好 |
| B: 标准测试 | region, card table, events, stats, scheduler, memory domain | 良好 |
| C: 功能测试 | root scanner, handles, POH, LOH, TLAB | 良好 |
| D: GC 单元测试 | young collector, old gen, Gen1, finalizer, full GC, managed API, safepoint, parallel mark, mark bitmap, sanity, domain unload | 良好 |
| E: Stress | multi-threaded stress scenarios | 基础 |
| F: Benchmarks | Gen1 pause, GC throughput | 基础 |
| G: 迁移 stress | worker pool, mini stress, monitor stress | 良好 |
| H: 中等 stress | finalizer stress, LOH stress, oversized stress | 良好 |
| I: BGC | BGC smoke, root scan, unit, stress | 良好 |
| J: 大型 stress | general stress, delegate stress (timeout=300s) | 良好 |

### 4.2 测试缺口

| 缺口 | 严重性 | 说明 |
|------|--------|------|
| BGC+LOH 并发无测试 | 中 | 需要 concurrent allocation during BGC cycle |
| Gen1 compaction 碎片化测试 | 中 | Gen1 budget trigger 无边界测试 |
| SATB buffer overflow | 中 | 1024-entry buffer 多线程满负载 |
| BGC 并发 mark-new-roots | 中 | G-25 scenario |
| OOM 测试 | 低 | out-of-memory recovery |
| Multi-heap Server GC | 低 | CHAOS_IL2CPP_GC_SERVER=1 时无集成测试 |
| NUMA 亲和性测试 | 低 | gc_numa_win.cpp 有实现但无多 socket 测试 |
| 长时间运行（hours） | 低 | stress 测试仅 300s timeout |

---

## 五、与 CoreCLR 代码量对比

| 维度 | CRAG GC | CoreCLR GC (gcpriv) | 比例 |
|------|---------|---------------------|------|
| 核心 GC | ~17,116 lines | ~35,000 lines (est.) | ~49% |
| GC 测试 | ~12,000 lines | ~25,000 lines (est.) | ~48% |
| 测试 targets | 35 | ~100+ (CoreCLR 测试套件) | ~35% |
| 测试文件 | 44 | ~80+ | ~55% |

CRAG GC 在核心功能覆盖上达到 CoreCLR ~70%，但在边缘情况、诊断、可配置性和测试深度上仍有显著差距。

---

## 六、推荐优先级

### 短期（1-2 周）
1. **Finalizer 接线** — P0, 当前 stub 状态导致最终化完全不工作
2. **Gen1 budget trigger 清理 + compaction 测试** — P1, 清理死代码增强 Gen1 可靠性
3. **BGC+LOH 并发测试 + SATB overflow 测试** — P1, 多线程并发正确性

### 中期（3-6 周）
4. Thread hijacking — P2, 替代性 suspend 机制
5. OOM 事件 + GCNotification — P2
6. Server GC 集成测试 — P2
7. GCMemoryInfo 完整 API — P2

### 长期（6-12 周）
8. Demotion 路径 — P2
9. Hard/Soft limits + VirtualAlloc — P3
10. ETW events — P3
11. Large pages 默认 — P3
