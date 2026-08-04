# CRAG (Chaos Region-Aware GC) 深度风险分析报告

**分析日期**: 2026-05-13
**版本**: C0.1 (Phase C, M0)
**范围**: `src/native/runtime-core/gc/`, `thread_state`, `memory_domain`, `domain_unloader`

---

## 目录

1. [执行摘要](#1-执行摘要)
2. [分配策略分析](#2-分配策略分析)
3. [GC 策略分析](#3-gc-策略分析)
4. [内存泄漏风险清单](#4-内存泄漏风险清单)
5. [API 设计评估](#5-api-设计评估)
6. [线程安全与 STW 模型](#6-线程安全与-stw-模型)
7. [OOM 风险](#7-oom-风险)
8. [性能瓶颈](#8-性能瓶颈)
9. [UAF / 双重释放 / 安全风险](#9-uaf--双重释放--安全风险)
10. [改进建议优先级](#10-改进建议优先级)

---

## 1. 执行摘要

CRAG 子系统处于 **Phase C M0**（中间阶段），从 BumpCache 向精确分代 GC 过渡。整体架构方向正确，但存在 **4 个高风险、8 个中风险** 问题需要解决。

**修复状态 (2026-05-13)**: 全部 4 个 HIGH 和 6 个 MEDIUM 风险已修复 ✅ 详见各章节标记。

---

## 2. 分配策略分析

### 2.1 分配路径拓扑

```
GcAllocate(size)                           → codegen 入口 (gc_helpers.h)
  → NurseryAllocate(size)                  → inline bump, fast path (gc_region.h:109)
       → NurseryAllocateSlow(size)          → overflow / GC trigger (gc_region.cpp:40)
            → g_old_gen.Allocate()           → oversized >32KB
            → GcYoungCollection()            → scheduler triggered
            → g_old_gen.Collect()            → full GC requested
            → RegionManager::AllocateNurseryOfSize() → new nursery

GcAllocateAtomic(size)                     → codegen 入口 (gc_helpers.h)
  → NurseryAllocateAtomic(size)            → inline bump, fast path (gc_region.h:135)
       → 同上, 但 scanning_required=false

DomainAllocate(domain_id, size)            → 模块元数据 (gc_region.cpp:248)
  → domain->heap->Allocate(size)           → SegregatedHeap
  → 回退: CHAOS_IL2CPP_MALLOC(size)

RawAllocate(size) / RawFree(ptr)           → 临时分配 (gc_region.h:172)
  → CHAOS_IL2CPP_MALLOC / FREE
```

### 2.2 分配域分离

| 域 | 分配函数 | 释放机制 | 生命周期 |
|----|---------|----------|---------|
| GC (托管) | NurseryAllocate → OldGen::Allocate | GC 回收 | 由 GC 决定 |
| Domain (模块) | DomainAllocate → SegregatedHeap::Destroy | 批量 Destroy | 模块卸载时 |
| Raw (临时) | RawAllocate → malloc/free | 显式 Free | 调用者管理 |

### 2.3 风险: 跨域混用的检测机制

**发现**: `DomainFreeTagged` (memory_domain.cpp:298) 在分配头部嵌入了 `IDomainHeap*` 指针，bit 0 置 1 作为标签。释放时验证标签并路由到正确的 heap。这是一道深度防御，但：

- **问题**: `RawFree` 直接调用 `CHAOS_IL2CPP_FREE`，没有标签检查。如果 GC 域指针被误传给 `RawFree`，会直接 `std::free`，造成 UAF。
- **问题**: GC 域 (NurseryAllocate 出来的指针) 没有标签机制。如果被传给 `DomainFreeTagged`，标签检查会 fail 然后回退到 `std::free` —— 同样 UAF。
- **建议**: 统一所有分配域使用标签 header，或者在调试构建下加类型断言。

### 2.4 风险: oversized (>32KB) 分配路径

**发现**: 超过 `kMaxNurseryAlloc=32KB` 的分配直接绕过 nursery 进入 `g_old_gen.Allocate()`。这在 `NurseryAllocateSlow` (gc_region.cpp:42) 中发生。OldGen 的 oversized 分配走 `AllocatePage()` 为一个对象分配整个 64KB 页面。

- **问题**: 这个路径绕过了 scheduler 的年轻 GC 检查。Oversized 页面直接在 old-gen 分配，但如果这些对象很快死亡，只有 full GC 才能回收它们。这意味着 oversize 分配可能在 young GC 密集触发时产生 old-gen 碎片。
- **缓解**: OldGen::Allocate 中 oversized 路径 (gc_old_gen.cpp:296-306) 在分配前检查 `DecideCollection()`，但只检查 FULL GC。它不检查 YOUNG GC。所以 oversize 分配不会触发 young GC。

### 2.5 风险: DomainAllocate 退化回退

**发现**: `DomainAllocate` (gc_region.cpp:248) 优先通过 `domain->heap->Allocate(size)` 分配。如果 `domain` 为 nullptr 或 `heap` 为 nullptr，回退到 `CHAOS_IL2CPP_MALLOC`。

- **问题**: 回退路径分配的内存**没有被 Domain 追踪**。模块卸载时 `ReleaseDomainRegions` 找不到这些 malloc 内存，导致泄漏。同时 `DomainFreeTagged` 也无法正确处理。
- **严重度**: 中 (仅发生在异常路径)。

---

## 3. GC 策略分析

### 3.1 收集器模式

| 特性 | 年轻代 (Young) | 老年代 (Full) |
|-----|--------------|--------------|
| 算法 | Cheney 复制 (copy) | Mark-Sweep (non-compacting) |
| 触发 | NurseryAllocateSlow 中 scheduler 决策 | IsFullGcRequested 标志 / Oversized 分配 |
| STW | 是 (RequestGlobalSafepoint) | 是 |
| 根扫描 | 精确 (GcLayout) + dirty cards | 保守 (线程栈) + 精确 (nursery safe area) |
| 晋升 | GcScavengeObject → OldGen::Allocate | N/A |
| 清除 | nursery->current = nursery->begin | 并行 CTZ sweep + Coalesce |

### 3.2 Scheduler 自适应策略

```
触发条件:
  YOUNG: alloc_since_last_gc > last_nursery_used * kYoungTriggerMultiplier (1.0)
  FULL:  alloc_since_last_full_gc > estimated_heap_size * kFullTriggerMultiplier (4.0)

Nursery 大小:
  target = survival_rate * 2 * last_nursery_used
  clamped: [128KB, 1MB]

EMA survival rate:
  alpha = 1/16, 初始化 = 0.5
```

**发现**: 从上次报告的 1.5x 降到了 1.0x，已经纠正了 young GC 不触发的问题。

### 3.3 风险: Young GC promoted_bytes 始终为 0

**观察所有报告**: `young_promoted_bytes = 0` 在所有 4 个场景中。这意味着没有对象被晋升到 old-gen。

**根因**: 压力测试不创建托管对象图 —— `WritePattern` 写入的 pattern (`0xBAD0DEADull << 32 | ...`) 使 `IsValidTypeInfoPointer` 失败 (因为 0xBAD0DEAD 不在 TypeInfo 地址范围内)。因此 Phase 2 精确扫描找不到任何有效对象。Phase 1 (dirty cards) 也不产生晋升，因为测试中没有跨代引用。

**隐含问题**: Young GC 在不产生晋升时实际效果 === nursery reset。所有分配数据被丢弃。Cheney 复制路径 (GcScavengeObject) 从未被调用。这意味着 **young GC 的晋升路径在生产压力下未经测试**。

**严重度**: 中 (测试覆盖缺口，不是代码缺陷)。

### 3.4 风险: Young GC Phase 1 脏卡扫描延迟

**发现**: `GcYoungCollection` (gc_young_collector.cpp:152) 扫描 **所有 old-gen 页面** 的脏卡，不是只扫描当前 nursery 上下文的范围。随着 old-gen 页面数量增长，脏卡扫描时间线性增加。

- 场景 D 结束后 active_regions=334，但 old-gen 页面为 0 (因为无晋升)。生产环境中 old-gen 页面可能上千。
- **建议**: Phase 1 应只扫描自上次年轻 GC 以来变脏的卡片。当前实现每次扫描全部 old-gen。

### 3.5 风险: Full GC 在 has_roots==false 时跳过扫描

**修复后验证**: Collect() (gc_old_gen.cpp:701) 已经修改为无条件执行所有阶段。之前 `!has_roots` 跳过 DrainMarkStack 和终结器的 bug 已修复。

### 3.6 风险: CollectIncremental 是伪增量

**发现**: `CollectIncremental()` (gc_old_gen.cpp:892) 直接调用 `Collect(nullptr, nullptr)` —— 每次都是完整 STW 标记-清除。

- **问题**: 命名故意误导。"Incremental" 是 C3 路线图承诺，当前是桩。
- **建议**: 要么重命名 (CollectFull)，要么加 `#warning C3 stub` 编译警告。

**修复状态**: ✅ 已重命名为 `CollectFull`，注释说明 C3 将替换为增量切片。

---

## 4. 内存泄漏风险清单

### 🔴 HIGH: SweepPage 未匹配 size-class 丢弃块

**位置**: `gc_old_gen.cpp:569-573`
**描述**: CTZ 位图扫描产生连续未标记运行。如果 `run_bytes` 不符合任何 size-class (28 个 size-class 包括 16,24,32,40,48,56,64,80,96,112,128,160,192,224,256,320,384,512,1024,2048,3072,4096,6144,8192,12288,16384,24576,32768)，该块被静默丢弃。

**示例**: 72 字节运行 (9 × 8 字节槽) 没有匹配的 size-class，泄漏。112 字节有，120 没有。64 有，72 没有。

**影响**: 每次 GC 循环泄漏少量内存。长时间运行堆会持续膨胀。

**修复**: 使用 split allocation: 将运行块按最大匹配 size-class 拆分为多个小块的级联。

**修复状态**: ✅ `SweepPage` free_run 使用迭代 size-class 拆分 (`remaining → sc_idx → carve → advance`)，所有字节都被分配到合适的 size-class，零丢弃。

### 🔴 HIGH: CoalescePage 合并后同样丢弃

**位置**: `gc_old_gen.cpp:686-693`
**描述**: `CoalescePage` 在合并相邻空闲块后，再次做 `SizeClassIndex(b.size)` 检查。合并后的块也可能不匹配 size-class。

**影响**: 同上。Coalesce 反而可能**加剧**泄漏。

**修复状态**: ✅ `CoalescePage` 合并后使用与 `SweepPage` 相同的 size-class 拆分逻辑 (`cursor + sc_size + advance`)，零丢弃。

### 🟡 MEDIUM: Free() 在 `found==false` 时泄漏

**位置**: `gc_old_gen.cpp:400-404`
**描述**: 如果反向布局步行找不到匹配的 size-class 偏移，块被 `memset(ptr, 0, 64)` 但不放回空闲链表。

**代码注释**: gc_old_gen.cpp:427 直接承认 "Free() without size tracking is not reliable"。

**影响**: 每次不成功的 Free 泄漏一个块。

**修复状态**: ✅ `Free()` 已重构：非 oversized 路径不再操作 free list（只 zero 64 字节破坏 TypeInfo*），完全依赖 GC sweep 回收。不再有 `found==false` 泄漏。

### 🟡 MEDIUM: ManagedThread 条目泄漏

**位置**: `thread_state.cpp:89`
**描述**: `RegisterThread` 用 `new ManagedThread()` 分配，但注释说 "entries are intentionally leaked"。条目在 `is_running=false` 下逻辑删除。

**影响**: 每次模块热更新 (创建新线程处理) 增加 ≈ 80 字节泄漏。不严重但不理想。

**修复状态**: ⏸ 保持原样。进程生命周期的 runtime 元数据，泄漏影响极小。C5+ 可回收。

### 🟡 MEDIUM: MemoryDomain 结构体泄漏

**位置**: `memory_domain.cpp:491-510`
**描述**: `UnregisterMemoryDomain` 调用 `heap->Destroy()` 和 `delete domain->heap`，但**不 delete `MemoryDomain*` 本身**。大约 48 字节/次泄漏。

**修复状态**: ✅ 已添加 `delete domain; domain = nullptr;`。

### 🟡 MEDIUM: VTable registry 进程生命周期泄漏

**位置**: `vtable_registry.cpp:183`
**描述**: `BuildRuntimeVTable` 分配的 vtable 数组 (malloc) 永久驻留在 `flat_vtables` 映射中。模块卸载时不清除。

**注释**: 有意的进程生命周期分配，但如果模块热更新重新加载，旧的 vtable 数组不会被释放。

**修复状态**: ⏸ 保持原样。C5 域卸载设施完备后统一清理。

### 🔵 LOW: domain_unloader `cross_domain_refs_found` 永远是 0

**位置**: `domain_unloader.cpp:24`
**描述**: `ScanAndClearCrossDomainRefs` 声明了 `refs_found` 变量但从未递增。输出的 `cross_domain_refs_found` 始终为 0。`cross_domain_refs_cleared` 正确。

**影响**: 统计信息不正确，不影响正确性。

**修复状态**: ✅ 已修复。`refs_found` 在跨域引用找到时递增，通过输出参数正确返回。`cleared` 为返回值。

---

## 5. API 设计评估

### 5.1 分配 API 一致性

| API | 是否内联 | 是否零初始化 | 是否线程安全 | 失败行为 |
|-----|---------|------------|------------|---------|
| NurseryAllocate | 是 (region.h:109) | 是 (memset) | 是 (TLS) | 返回 nullptr / fallback |
| NurseryAllocateAtomic | 是 (region.h:135) | 是 (memset) | 是 (TLS) | 同上 |
| GcAllocate | 是 (helpers.h:15) | 委托 NurseryAllocate | 是 | 同上 |
| DomainAllocate | 否 (region.cpp:248) | 否 (heap->Allocate) | 是 (heap mutex) | 返回 nullptr |
| RawAllocate | 是 (region.h:172) | 否 | 是 (malloc 线程安全) | 返回 nullptr |
| OldGen::Allocate | 否 (old_gen.cpp:243) | 否 (直接返回 payload) | 否 (依赖调用者) | 返回 nullptr |

**不一致**: OldGen::Allocate 不零初始化，而 NurseryAllocate 零初始化。调用者需要知道这个区别。

### 5.2 释放 API 对称性

| 分配 API | 对应释放 | 对称性 |
|---------|---------|--------|
| NurseryAllocate | GC 隐式回收 | 不对称 (调用者不负责释放) |
| OldGen::Allocate | OldGen::Free / GC sweep | 双路径 (显式或 GC) |
| DomainAllocate | DomainAllocateTagged / heap->Destroy | 双路径 |
| RawAllocate | RawFree | 对称 |

**问题**: `OldGen::Free` 和 GC sweep 是两个独立的回收路径。一个对象的块如果被 Free 放回空闲链表，然后再被 GC sweep 扫描到，会发生什么？GC sweep 扫描的是 mark bitmap，不是空闲链表。Free 不会清除 bitmap 位。所以 sweep 时该块可能被当作"未标记"重新回收 —— **双重回收**。

**等一等**: 不对。GC mark 阶段标记的是**存活对象**，不是空闲块。Free 放回空闲链表的块对应的内存，在 mark bitmap 中可能之前已经标记了 (因为块被使用后分配出去，标记位对应那个地址)。但是 mark bitmap 在 GC 开始时没有被清除。等等 —— `Collect()` 怎么重置 mark bitmap？

**查找**: `gc_old_gen.cpp` 中的 `Collect()` 在标记阶段调用 `MarkObject` 来设置位图中的位。但在标记阶段开始前，位图是否被清除？

看 `SweepPage()` (gc_old_gen.cpp:588): `std::memset(bm, 0, bitmap_bytes)` —— 位图在**清除**阶段被清除。

再看 `Collect()`: 在清除阶段，`Collect` 调用 `SweepPage`，它在结束时 memset 位图。然后下一轮 Collect 开始时，位图是干净的。**所以不会双重回收。**

但是：如果在两次 GC 之间调用了 `Free()`，空闲链表中有一个块，而该块对应的位图位是 0 (因为上一轮 sweep 清除了)。下一轮 Collect 开始标记时，没有根指向这个块 (因为它在空闲链表，不在活跃对象图中)，所以它不会被标记。然后 SweepPage 在位图中看到它未标记，又把它加回空闲链表 —— **导致空闲链表中的块重复出现，造成链表循环或损坏**。

**严重度**: 🔴 HIGH。这是一个真实的设计缺陷。

**验证**: 看 `Free()` (gc_old_gen.cpp:344-405):
- `Free()` 确实把块放回 `page->free_lists[sc]`。
- 它**不设置 mark bitmap**。
- 下一轮 `Collect()` 开始 → 标记阶段 → 没有根 → 未标记
- 清除阶段 → `SweepPage` → 看到未标记 → `free_run` → 再加到空闲链表
- 结果: 同一块在空闲链表中出现两次

这可能导致：
1. 空闲链表循环 (如果 next 指针被覆盖)
2. 两个不同的父块指向同一个空闲块
3. 分配返回已经被分配出去的内存 (UAF)

**这是区域管理器 free_list 实现的问题的旧生代版本。** 区别：RegionManager 的 free_list 释放后重置 `r->id = kRegionIdInvalid`，且重新分配时重置 `r->current = r->begin`。但 OldGen 的 free_list 是裸指针链表，没有版本号或 ID 跟踪。

**修复**: `Free()` 应该在把块放回空闲链表的同时设置 mark bitmap 位。或者放弃显式 `Free()`，只依赖 GC sweep。

### 5.3 RAII / 智能指针使用

**发现**: 整个 GC 子系统使用原始指针 (`Region*`, `OldGenPage*`, `OldGenFreeBlock*`)。没有 `unique_ptr` 或 `shared_ptr`。

**评价**: 在运行时/GC 上下文中这是可接受的 —— GC 子系统需要精确控制内存布局和生命周期，智能指针的开销和语义干扰不适合。但这也意味着所有生命周期管理是手动的。

### 5.4 全局单例耦合

```
g_old_gen (MarkSweepOldGen)         ← gc_region, gc_young_collector, gc_stress_test 直接调用
g_gc_scheduler (GcScheduler)        ← gc_region, gc_old_gen 直接调用
g_gc_stats (GcStats)                ← gc_young_collector, gc_old_gen 直接调用
g_card_table (uint8_t[2M])           ← domain_unloader 直接引用
g_heap_base (uintptr_t)             ← domain_unloader 直接引用
tls_nursery_ctx (NurseryContext)     ← gc_young_collector 直接引用
```

**问题**: 全局变量被文件直接引用，没有依赖注入或接口抽象。这使得单元测试困难，未来架构重构 (如多堆支持) 不可能。

---

## 6. 线程安全与 STW 模型

### 6.1 STW 模型整体评价

CRAG 使用全局 safepoint 协议 (`RequestGlobalSafepoint` / `ReleaseGlobalSafepoint`)。在 GC 期间，所有托管线程暂停在 `SafepointPoll`。STW 模型正确性依赖于：

1. 所有托管线程在合理时间内到达 safepoint
2. 没有线程在 STW 期间操作 GC 状态
3. 非托管线程 (如本地线程) 不参与

**发现**: 这些假设在当前场景下是合理的，但有竞争风险 (见下文)。

### 6.2 FindPage 无锁遍历

**位置**: `gc_old_gen.cpp:196-209`
**描述**: `FindPage` 遍历 `page_list_` 时**不持有 `mutex_`**。在 `Collect()` 期间 STW 下是安全的，但在 `Free()` 中调用时，如果另一个线程同时修改 `page_list_`，会读取失效指针。

**缓解**: `Free()` 是用户代码调用的。在正常执行中，用户线程不会并发执行 `Collect()`，因为 `Collect()` 需要请求 safepoint 暂停所有线程。但如果 `Free()` 在某个线程中调用，而另一个线程正在 nursery allocate slow path 中触发 full GC 并进入 `Collect()`，那么：

1. Thread A: 调用 `Free(ptr)` → `FindPage(ptr)` → 读取 `page->next`
2. Thread B: NurseryAllocateSlow → `IsFullGcRequested()` → `RequestGlobalSafepoint()` → 等待 Thread A
3. Thread A 到达 SafepointPoll 前可能正在 Free() 中修改 free_list

**但**: Thread A 最终会到达 SafepointPoll (例如通过下一个 NurseryAllocate 或显式调用)。在此期间，Thread B 等待。所以 Thread A 的 Free() 在没有持锁的情况下修改了 page_list_，而 Thread B 即将读取它。**这是不安全的**。

**严重度**: 🟡 MEDIUM。在实践中不太可能触发，但理论上存在竞争。

### 6.3 并行 Sweep mutex 争用

**位置**: `gc_old_gen.cpp:816`
**描述**: 并行 sweep 中每个 worker 线程在 `CoalescePage` 中争用同一个 `mutex_`。
- 28 页, 8 线程 → 8 线程串行执行 CoalescePage
- `CoalescePage` 不需要 `mutex_` (它只操作单页的内部 free list，别的线程不会访问该页)

**建议**: 让 `CoalescePage` 不使用 `mutex_`(当前是 `lock_guard` 在函数最外层)，或者分拆锁。

**修复状态**: ✅ `CoalescePage` 已移除全局 `mutex_`（注释说明 STW 下每个 worker 处理独享页面）。

### 6.4 GcScanAllThreadRoots 使用文件静态变量

**位置**: `thread_state.cpp:174-221`
**描述**: `s_callback` 和 `s_user_data` 是文件作用域静态变量。如果两个线程同时调用 `GcScanAllThreadRoots`，文件静态变量会被覆盖。

**缓解**: 在 STW 模型下，GC 线程独占执行。`GcScanAllThreadRoots` 只在 `Collect()` 期间被调用。当前安全，但脆弱。

**建议**: 将 callback/user_data 作为参数传递，而不是文件静态变量。

---

## 7. OOM 风险

### 🔴 HIGH: 无界 Mark Stack

**位置**: `gc_old_gen.cpp:464-526`
**描述**: `mark_stack_` (类型 `std::vector<void*>`) 在 DrainMarkStack 期间无限增长。在深度对象图中，vector 可能增长到 GB 级别。

**场景**: 一个深度为 1M、扇出为 2 的对象链，标记栈可能包含百万级条目。

**影响**: `std::vector::push_back` 抛出 `std::bad_alloc` → 未捕获 → 进程崩溃。

**修复**: 加大小限制 `kMaxMarkStack = 256K` (2MB)，溢出时 fallback 到保守逐页重扫描。

**修复状态**: ✅ 已添加 `kMaxMarkStack` 界限 + `overflowed` 标志，溢出时 fallback 到 `DrainMarkStackConservative`（无新 push，逐页保守扫描）。

### 🟡 MEDIUM: CoalescePage vector 分配失败

**位置**: `gc_old_gen.cpp:649,671`
**描述**: `CoalescePage` 在堆上分配 `std::vector<BlockAddr>`。如果内存不足，`push_back` 或 `reserve` 抛出异常。

**影响**: 异常传播到并行 sweep worker，未捕获，进程崩溃。

### 🟡 MEDIUM: AllocatePage 返回 nullptr

**位置**: `gc_old_gen.cpp:123`
**描述**: VirtualAlloc/mmap 失败返回 nullptr。`AllocatePage` 检查此条件并返回 nullptr。但 `Allocate()` 不处理 nullptr (gc_old_gen.cpp:308-309) —— 它直接返回 `page->Payload()`。

**查看代码**: 
```cpp
OldGenPage* page = AllocatePage(size, scanning_required);
if (!page) return nullptr;  // 已修复
return page->Payload();
```
实际上 `Allocate()` 确实检查了 nullptr。在 `TryAllocateFromFreeLists` 回退到 `AllocatePage` 时检查。**OOM 传播正确。**

### 🟡 MEDIUM: BFS worklist 溢出丢失晋升

**位置**: `gc_young_collector.cpp:272-344`
**描述**: BFS worklist 固定 64K 条目 (512KB)。溢出时走 fallback —— 保守扫描所有未转发 nursery 对象。

**问题**: overflow fallback 分配 old-gen 内存但不更新 worklist 中的引用。如果 promoted 对象在 overflow 后还有 nursery 引用，这些引用会被遗漏。

**缓解**: overflow fallback 中分配的 promoted 对象不会再被 BFS 扫描其指针字段。这意味着这些对象持有的 nursery 引用不会被转发。Sweep 阶段会丢弃这些引用指向的对象，**造成 dangling pointer**。

**严重度**: 🟡 MEDIUM

**修复状态**: ✅ overflow fallback 后增加 fixup pass：遍历全部已转发 nursery 对象，对每个 tenured copy 执行 `ScanObjectPointers`，修复所有未转发的 nursery→nursery 指针。

---

## 8. 性能瓶颈

### 8.1 Young GC 脏卡全量扫描

**位置**: `gc_young_collector.cpp:152`
**每次年轻 GC 扫描所有 old-gen 页面的脏卡**。O(n) 随 old-gen 页面数增长。跳过无需扫描的干净页面依赖脏卡标记——但即使在脏卡标记正确的情况下也要遍历全部页面链表。

### 8.2 CoalescePage 全量排序 O(n log n)

**位置**: `gc_old_gen.cpp:671`
**对页面内所有空闲块按地址排序**，然后线性扫描合并。对于高度碎片化的页面 (可能有数百个空闲块)，排序开销显著。

### 8.3 Full GC 线性扫描线程列表

**位置**: `gc_old_gen.cpp:728-751`
**每次 full GC 线性遍历所有注册线程**。随线程数 O(n) 增长。100 线程下可接受，1000 线程下显著。

### 8.4 Full GC 线性扫描卡表

**无处使用**: `domain_unloader.cpp:54` 扫描 `kCardTableEntries` (= 2M) 个条目。每次域卸载扫 2M 个卡片。2M × 少量内存访问 ≈ 2-16MB 读取。不算大但在 STW 下执行。

### 8.5 Wild oversubscription in parallel sweep

**位置**: `gc_old_gen.cpp:808`
```cpp
int num_threads = std::thread::hardware_concurrency();
```
128 核机器上生成 127 个 worker 线程去清除 28 页。127 线程 vs 28 页 = 严重过度订阅。

**修复**: `min(hardware_concurrency(), pages.size())`。

**修复状态**: ✅ 已修复：`max_workers = std::min(hardware_concurrency(), total_pages)` + 确保至少 2。

---

## 9. UAF / 双重释放 / 安全风险

### 9.1 🔴 HIGH: Free() + GC sweep 双重释放

**已在 5.2 分析**: `OldGen::Free()` 将块放回空闲链表但不设置 bitmap。下一轮 GC sweep 会再次回收同一块，导致空闲链表损坏。

**修复**: Free() 需要设置 bitmap 位，或者废弃 Free() 只依赖 GC。

**修复状态**: ✅ 非 oversized 的 `Free()` 不再操作 free list，只 `memset(ptr, 0, 64)` 破坏 TypeInfo*，完全依赖 GC sweep 回收。Oversized 路径正确 unlink + VirtualFree。

### 9.2 Finalizer 重复执行

**位置**: `gc_old_gen.cpp:979-994`
**每轮 GC 执行所有注册的 finalizer**，无论对象是否存活。不是只执行 unreachable 对象的 finalizer。

**问题**: 存活对象的 finalizer 也会被执行。如果 finalizer 有副作用 (如释放外部资源)，则存活对象会意外释放资源。

**修复**: RunFinalizers 应该检查对象是否存活 (通过 mark bitmap)。当前实现是错的。

**严重度**: 🔴 HIGH

**修复状态**: ✅ `RunFinalizers` 已重构：通过 mark bitmap 检查对象存活状态，仅对 unreachable 对象执行 finalizer；存活对象重新注册到 finalizer 列表。

### 9.3 Reallocate 使用整页作为 old_size

**位置**: `gc_old_gen.cpp:416`
```cpp
CHAOS_IL2CPP_SIZE old_size = new_size;
```
`Reallocate` 不知道原始对象大小，使用 `min(remaining, new_size)` 近似。可能复制超过实际对象大小的内容，暴露相邻对象的数据。

**严重度**: 🟡 MEDIUM (信息泄漏)

### 9.4 Oversized 页面释放后 FindPage

**位置**: `gc_old_gen.cpp:196-209`
`FindPage` 遍历无锁 list。如果 oversized 页面刚被 Free() 并 VirtualFree，但另一个 Free() 的 FindPage 正持有指向它的指针，则产生 UAF。

**缓解**: STW 模型保护。两阶段 Free() 不并发。

---

## 10. 改进建议优先级

### P0: 必须修复 (High)

| # | 问题 | 文件 | 建议修复 | 状态 |
|---|------|------|---------|------|
| 1 | Free()+GC sweep 双重释放 | gc_old_gen.cpp | Free() 设置 bitmap 位；或废弃 Free() | ✅ |
| 2 | Finalizer 对存活对象执行 | gc_old_gen.cpp | RunFinalizers 检查 mark bitmap | ✅ |
| 3 | SweepPage 未匹配 size-class 泄漏 | gc_old_gen.cpp | 改用 round-up + 拆分，或 bitmap-based free list | ✅ |
| 4 | 无界 mark stack OOM | gc_old_gen.cpp | 加大小限制 + overflow fallback | ✅ |

### P1: 重要修复 (Medium)

| # | 问题 | 文件 | 建议修复 | 状态 |
|---|------|------|---------|------|
| 5 | CoalescePage 同样丢弃块 | gc_old_gen.cpp | 同 #3 | ✅ |
| 6 | Free() found==false 泄漏 | gc_old_gen.cpp | 改进反向布局算法 | ✅ (废弃 Free free-list) |
| 7 | FindPage 无锁遍历 | gc_old_gen.cpp | Free 中加 mutex 保护 | ✅ |
| 8 | Young GC 脏卡全量扫描 | gc_young_collector.cpp | 记录上次扫描位置 | ⏸ |
| 9 | Parallel sweep oversubscription | gc_old_gen.cpp | 限制 threads ≤ pages.size() | ✅ |
| 10 | BFS worklist overflow dangling ptr | gc_young_collector.cpp | overflow 后 retry 或全量 conservative | ✅ |

### P2: 改进建议 (Low)

| # | 问题 | 文件 | 建议修复 | 状态 |
|---|------|------|---------|------|
| 11 | GcScanAllThreadRoots 文件静态变量 | thread_state.cpp | 改为参数传递 | ⏸ (MSVC lambda 限制) |
| 12 | CoalescePage 使用全局 mutex | gc_old_gen.cpp | 移除不必要的 mutex 锁 | ✅ |
| 13 | CollectIncremental 伪增量 | gc_old_gen.cpp | 重命名或加 warning | ✅ |
| 14 | ManagedThread 条目泄漏 | thread_state.cpp | 延迟删除到 free list | ⏸ (C5+) |
| 15 | MemoryDomain 结构体泄漏 | memory_domain.cpp | delete domain 指针 | ✅ |
| 16 | cross_domain_refs_found 为 0 | domain_unloader.cpp | 修改变量名或计数逻辑 | ✅ |
| 17 | Young GC promoted 路径未经测试 | stress test | 添加托管对象图压力测试 | ⏸ |

---

## 附录 A: 文件间依赖图

```
gc_region.h/.cpp ──────→ gc_scheduler.h/.cpp   (g_gc_scheduler)
                    ──→ gc_old_gen.h/.cpp       (g_old_gen)
                    ──→ gc_young_collector.h/.cpp
                    ──→ gc_layout.h/.cpp
                    ──→ thread_state.h/.cpp      (safepoint)
                    ──→ memory_domain.h/.cpp

gc_young_collector.cpp ─→ gc_region.h         (IsInNursery)
                      ──→ gc_old_gen.h         (Allocate, ScanDirtyCardsInPages)
                      ──→ gc_card_table.h
                      ──→ gc_layout.h
                      ──→ gc_stats.h
                      ──→ gc_scheduler.h
                      ──→ thread_state.h       (tls_nursery_ctx)

gc_old_gen.h/.cpp ─────→ gc_card_table.h
                    ──→ gc_bit_utils.h
                    ──→ gc_scheduler.h
                    ──→ gc_stats.h
                    ──→ thread_state.h         (EnumerateThreads, GcScanAllThreadRoots)

gc_layout.h/.cpp ──────→ gc_card_table.h     (write barrier)

domain_unloader.cpp ───→ gc_region.h          (RegionManager)
                    ──→ gc_card_table.h       (g_card_table, g_heap_base)
                    ──→ memory_domain.h
                    ──→ thread_state.h        (safepoint)
```

**关键发现**: `gc_young_collector.cpp` 直接引用 `tls_nursery_ctx` (gc_young_collector.cpp:96)。这个是线程局部变量，但在 `GcYoungCollection` 中读取调用线程的 nursery —— 不是被收集线程的 nursery。如果线程 A 的 nursery 满了触发 young GC，但 `GcYoungCollection` 读取 `tls_nursery_ctx` (线程 A 的 nursery)，这是正确的。但如果另一个线程 B 的 nursery 也满了，线程 B 的参数不会被读取，这个正确吗？

**实际上**: `GcYoungCollection` 接受 `Region* nursery` 参数，所以收集的是正确的 nursery。但 `EstimateObjectSize` (gc_young_collector.cpp:96) 读取 `tls_nursery_ctx.nursery` 来估计对象大小。在 STW 下，这读取的是**调用 GC 线程的 TLS**，不是被收集线程的 TLS。如果线程 A 触发 GC 并收集自己的 nursery，这是对的。但如果线程 A 触发 GC 而线程 B 的 nursery 也需要收集，那么 `EstimateObjectSize` 会使用线程 A 的 nursery 边界来估计线程 B 的 nursery 对象大小 —— **可能不正确**。

**但在 Phase 2 (精确扫描) 路径中**，EstimateObjectSize 只在 layout==nullptr 或 first_word==nullptr 时作为 fallback 使用。在精确布局路径中使用 `layout->instance_size`。所以这是 **低风险**。

---

## 附录 B: 代码行级风险清单

| 行号 | 文件 | 风险 |
|-----|------|------|
| 42-46 | gc_region.cpp | Oversized 直接进 old-gen，绕过了 young GC |
| 86-96 | gc_region.cpp | Full GC 前没有 `RecordFullCollection`? 有 |
| 96 | gc_young_collector.cpp | EstimateObjectSize 使用调用线程的 TLS nursery 边界 |
| 196-209 | gc_old_gen.cpp | FindPage 无锁遍历 page_list_ |
| 296-306 | gc_old_gen.cpp | Oversized 分配只检查 FULL GC，不检查 YOUNG |
| 344-405 | gc_old_gen.cpp | Free() 不设置 bitmap → 双重释放风险 |
| 400-404 | gc_old_gen.cpp | found==false 泄漏 |
| 416 | gc_old_gen.cpp | Reallocate 不知道原始大小 |
| 460 | gc_old_gen.cpp | marked_count_ 用 relaxed ordering |
| 464-526 | gc_old_gen.cpp | 无界 mark stack |
| 538-552 | gc_old_gen.cpp | Oversized 页面位图 byte 扫描 (可优化) |
| 569-573 | gc_old_gen.cpp | SweepPage 未匹配 size-class 丢弃 |
| 635 | gc_old_gen.cpp | CoalescePage 持有全局 mutex |
| 686-693 | gc_old_gen.cpp | CoalescePage 同样丢弃合并块 |
| 778-783 | gc_old_gen.cpp | Finalizer 在 sweep 前执行 (顺序正确，但逻辑错误) |
| 808 | gc_old_gen.cpp | 过度订阅 (hardware_concurrency) |
| 892-896 | gc_old_gen.cpp | CollectIncremental 是伪增量 |
| 907 | gc_old_gen.cpp | tls_thread_stack 是全局变量 |
| 78 | gc_region.h | tls_nursery_ctx.limit 设为 end - 32KB |
| 89 | gc_scheduler.cpp | RecordFullCollection 不清零 alloc_since_last_gc_ |
| 82 | thread_state.cpp | static thread_local ManagedThread 在 safepoint 下不确保可见 |
| 82 | thread_state.h | GcScanAllThreadRoots 文件静态变量 |
| 89 | thread_state.cpp | ManagedThread 条目有意图泄漏 |
| 493 | memory_domain.cpp | MemoryDomain 结构体泄漏 |
| 112 | domain_unloader.cpp | cross_domain_refs_found 始终为 0 |