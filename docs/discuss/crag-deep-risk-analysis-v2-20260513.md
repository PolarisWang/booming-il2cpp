# CRAG (Chaos Region-Aware GC) 深度风险分析报告 v2

**分析日期**: 2026-05-13
**版本**: C0.1 (Phase C, M0)
**范围**: `src/native/runtime-core/gc/*`, `domain_unloader.cpp`, `memory_domain.cpp`, `gc_stress_test.cpp`

---

## 目录

1. [执行摘要](#1-执行摘要)
2. [分配策略风险](#2-分配策略风险)
3. [GC 策略风险](#3-gc-策略风险)
4. [内存泄漏风险清单](#4-内存泄漏风险清单)
5. [API 设计评估](#5-api-设计评估)
6. [线程安全与 STW 模型](#6-线程安全与-stw-模型)
7. [OOM 风险](#7-oom-风险)
8. [UAF / 双重释放 / 安全风险](#8-uaf--双重释放--安全风险)
9. [性能瓶颈](#9-性能瓶颈)
10. [测试覆盖缺口](#10-测试覆盖缺口)
11. [已修复项目交叉引用 (v1)](#11-已修复项目交叉引用-v1)
12. [改进建议优先级](#12-改进建议优先级)

---

## 1. 执行摘要

CRAG 子系统处于 **Phase C M0**（中间阶段），从 GcBumpCache 向精确分代 GC 过渡。

**v1 分析**（此前报告，已全部修复）覆盖了 4 个 HIGH、10 个 MEDIUM 风险，包括 Free()+GC sweep 双重释放、无界 mark stack OOM、SweepPage size-class 丢弃等。

**本次 v2 分析** 基于最新代码状态，发现 **4 个 HIGH、8 个 MEDIUM、6 个 LOW 风险，以及 7 个测试覆盖缺口**：

| 风险等级 | 数量 | 关键问题 |
|---------|------|---------|
| **High** | 4 | CheneyBfs 提前终止、卡表 >1GB 堆静默失败、TLS nursery 跨线程误算、VerifyPattern 无效 |
| **Medium** | 8 | Scheduler 未接线、Reallocate 泄漏、DomainUnloader 方向错误、缓存失效等 |
| **Low** | 6 | 死代码、空操作 API、性能瓶颈等 |
| **测试缺口** | 7 | 未测试 NurseryAllocateAtomic、DomainAllocate、finalizer、溢出路径等 |

---

## 2. 分配策略风险

### 🔴 HIGH: EstimateObjectSize 使用调用线程 TLS nursery

**位置**: `gc_young_collector.cpp:96`
```cpp
CHAOS_IL2CPP_SIZE obj_size = EstimateObjectSize(obj, tls_nursery_ctx.nursery);
```

**问题**: `GcScavengeObject` 在此调用 `EstimateObjectSize(obj, tls_nursery_ctx.nursery)`，使用 **调用线程** 的 TLS nursery 边界来估计对象大小。但对象可能来自 **另一个线程** 的 nursery（通过 Phase 1 脏卡扫描发现）。不同线程可能有不同大小的 nursery，导致估计错误 → 复制截断 → old-gen 数据损坏。

**影响**: 跨线程 nursery 引用被转发时，如果源 nursery 大于 GC 线程的 nursery，则复制的对象会被截断，old-gen 中的后复制副本包含截断数据。

**建议**: 将对象地址作为参数传递，通过 `RegionManager::Instance().IsNurseryPointer` 查找实际的 nursery 区域，或用全局 nursery 范围数组来准确查找对象所在的 nursery 边界。

### 🟡 MEDIUM: Scheduler RecordAllocation 未接线

**位置**: `gc_region.cpp:40-141`, `gc_scheduler.h:53`

**问题**: `NurseryAllocateSlow` 和 `NurseryAllocateAtomicSlow` 在生产路径中均未调用 `g_gc_scheduler.RecordAllocation(size)`。只有压力测试在分发前手动调用它。这意味着 `DecideCollection()` 检查 `alloc_since_last_gc_` 时，该值始终接近 0，从而基于分配量的 YOUNG GC 触发条件失效。

**缓解**: `NurseryAllocateSlow` 有物理满的备用触发（`nursery->current > nursery->begin`），因此 YOUNG GC 仍然会触发，但调度器的基于预算的决策("nursery 用了 X 倍分配量才触发")不工作。

**建议**: 在 `NurseryAllocateSlow` 和 `NurseryAllocateAtomicSlow` 的 bump 分配前插入 `g_gc_scheduler.RecordAllocation(size)`。

### 🟡 MEDIUM: GcRecordAlloc 未接线

**位置**: `gc_stats.h:76`

**问题**: `GcRecordAlloc` 声明用于分配计数，但生产分配路径中未调用。`GcStats::alloc_total` 和 `alloc_bytes` 始终为 0。压力测试的 JSON 报告中 `total_bytes_estimate` 来自手工计算，不是来自 GcStats。

**建议**: 接入 `NurseryAllocateSlow`、`NurseryAllocateAtomicSlow`、`OldGen::Allocate` 和 `DomainAllocate`。

### 🟡 MEDIUM: DomainAllocate 路径未对接 RegionManager

**位置**: `gc_region.cpp:248-259`

**问题**: 注释说 "In later M-stones, this will route through RegionManager"。当前委托给 `domain->heap->Allocate(size)`，回退到 `CHAOS_IL2CPP_MALLOC(size)`。回退路径分配的内存不被 RegionManager 追踪，域卸载时可能泄漏。

**建议**: C4 对接 RegionManager，使 DomainAllocate 使用域区域而非 SegregatedHeap。

---

## 3. GC 策略风险

### 🔴 HIGH: CheneyBfsPrecise break 导致闭包丢失

**位置**: `gc_layout.cpp:237-241`

**问题**: 当 `CheneyBfsPrecise` 在遍历提升对象区域时遇到没有注册 GcLayout 的对象，代码执行 `break` 而非 `continue`——提前终止整个遍历。这意味着该提升区域中 **所有后续对象** 都跳过指针扫描，它们的 nursery 引用未被转发，产生 dangling pointer。

```cpp
if (layout == nullptr) {
    CHAOS_IL2CPP_LOG_DEBUG("CRAG", "cheney_bfs_no_layout");
    break;  // ← BUG: should be 'continue'
}
```

**影响**: 在 production 中若有类型没有预先注册 GC 布局，且该类型后跟更多包含指针的类型，则这些后续类型的指针未被转发 → nursery sweep 后 dangling pointer → UAF。

**建议**: `break` 改为 `continue`，跳过当前对象继续遍历后续区域。同时添加一个单元测试验证无 layout 对象后有正常对象的场景。

### 🔴 HIGH: Stress Test VerifyPattern 从不失败

**位置**: `gc_stress_test.cpp:287-312`

**问题**: `VerifyPattern` 函数中所有模式不匹配都被静默接受：

- 第 293 行: 如果 bit 0 置位（转发指针）→ `return true` ✅
- 第 304-306 行: 如果 `high32 != 0xBAD0DEAD`（过时 nursery 数据）→ `return true` ✅
- 第 310-312 行: 如果 high32 匹配但 low32 不匹配（其他线程的模式）→ `return true` ✅

**所有路径都返回 true。该函数没有实际检测能力。** 压力测试报告 "0 pattern failures" 是虚假的。

**影响**: 内存损坏、数据损坏在压力测试中不会被发现。

**建议**: 重构 `VerifyPattern`:
- 对已转发的对象（bit 0 置位）跳过验证（返回 true），但确认其转发地址指向 old-gen
- 对 `high32 != 0xBAD0DEAD` 的情况返回 false（说明 nursery 回收后数据被覆盖，但写模式预期应保持到验证时）
- 对 low32 不匹配的情况返回 false

### 🔴 HIGH: Card table 硬限制 1GB 堆

**位置**: `gc_card_table.h:31,46-52`

**问题**: `g_card_table[2 * 1024 * 1024]` 固定 2M 条目，覆盖 1GB 地址空间（每 card 512 字节）。当堆增长超过 1GB 时：
- `DirtyCard()` 第 49 行: `if (idx < kCardTableEntries)` 静默失败 —— 写屏障不生效
- 跨代引用不被记录 → young GC 遗漏 old→nursery 指针 → UAF

```cpp
inline void DirtyCard(const void* obj) noexcept {
    uintptr_t idx = (addr - g_heap_base) >> kCardShift;
    if (idx < kCardTableEntries) [[likely]] {
        g_card_table[idx] = 0xFF;
    }  // 静默失败！
}
```

**影响**: 生产环境中若托管堆超过 1GB，写屏障完全失效，young GC 后 old-gen 持有 dangling nursery 指针。

**建议**: 三种方案选一：
(a) 使用可增长的 card table（mmap 大地址空间预留 + 按需提交）
(b) 添加 `s_card_table_overflowed` 标志，溢出时 young GC 回退到保守全堆扫描
(c) 在接近 1GB 时触发 aggressive full GC 防止堆继续增长

### 🟡 MEDIUM: DomainUnloader 方向错误

**位置**: `domain_unloader.cpp:55-85`

**问题**: `ScanAndClearCrossDomainRefs` 扫描脏卡表，对于指向域外的指针值将其清零。这清除的是 **域→核心** 的指针。但域卸载期间真正危险的是 **核心→域** 的指针——这些指针在域内存释放后变成 dangling pointer。

当前逻辑：
- `val` 在域内部 → 保持（域内引用，正常）
- `val` 在域外部 → 清零（这是域→核心引用）

但域正在被释放，域→核心引用无关紧要。核心→域的引用没有被任何代码清除。

**影响**: 域卸载后核心内存中指向域对象的指针成为 dangling pointer → UAF。

**建议**: 增加第二阶段扫描核心内存（old-gen pages、线程栈、全局数据）中指向被卸载域的引用并清除。

### 🟡 MEDIUM: Young GC promoted_bytes 始终为 0

**所有压力测试报告**: `young_promoted_bytes = 0`。压力测试不创建实际的托管对象图，`WritePattern` 写入的 pattern 使 `IsValidTypeInfoPointer` 失败，因此 Phase 2 精确扫描找不到有效对象。Cheney 复制路径和晋升路径在实际对象图中未经测试。

### 🟡 MEDIUM: PromoteNursery 是桩函数

**位置**: `gc_region.cpp:412-418`

```cpp
CHAOS_IL2CPP_SIZE RegionManager::PromoteNursery(Region* nursery) {
    (void)nursery;
    return 0;  // stub
}
```

**影响**: 晋升不通过 RegionManager 跟踪。区域级别的晋升管理不可用。

---

## 4. 内存泄漏风险清单

### 🟡 MEDIUM: Reallocate 泄漏旧指针

**位置**: `gc_old_gen.cpp:384-407`

**问题**: `Reallocate()` 始终 alloc-new + copy，从不释放旧指针。注释第 403 行明确说 "For C3, we do NOT free the old pointer (let GC sweep handle it)"。但每次调用 `Reallocate` 都泄漏旧块，直到下一次 full GC 的 sweep 阶段才回收。对于频繁调整大小的对象，内存占用显著膨胀。

**建议**: 在 `Reallocate` 中分配新块后调用 `Free(ptr)`。当前 `Free()` 对非 oversized 块只做 memset(0,64)，依赖 GC sweep 回收，这是安全的。

### 🟡 MEDIUM: last_alloc_page_ 缓存从未失效

**位置**: `gc_old_gen.cpp:244, 249-250`

**问题**: `last_alloc_page_[sc_idx]` 缓存每个 size-class 最后使用的页面。当页面被停用（`in_use=false` 且从 `page_list_` 中移除后 VirtualFree），缓存仍指向该页面。虽然 `TryAllocateFromFreeLists` 检查 `in_use`，不会返回错误块，但会触发不必要的 O(page_list) 遍历。

**建议**: 在 `FreePage()` 或 `Collect()` Phase 4 中扫描 `last_alloc_page_[]` 并清除指向释放页面的条目。

---

## 5. API 设计评估

### 5.1 分配 API 一致性

| API | 零初始化 | 线程安全 | GC 追踪 | 生产就绪 |
|-----|---------|---------|---------|---------|
| NurseryAllocate | ✅ (memset) | ✅ (TLS) | ℹ️ 缺 RecordAllocation | ❌ 缺统计 |
| NurseryAllocateAtomic | ✅ (memset) | ✅ (TLS) | ℹ️ 同 NurseryAllocate | ❌ 缺统计 |
| OldGen::Allocate | ℹ️ 部分 (free-list 有，page carve 无) | ℹ️ 依赖调用者 STW | ✅ | ✅ |
| DomainAllocate | ❌ | ✅ (heap mutex) | ❌ | ❌ 缺 RegionManager |
| RawAllocate | ❌ | ✅ (malloc) | ❌ | ✅ |

### 🟡 MEDIUM: GcDumpStats 从未被调用

**位置**: `gc_stats.cpp:10, gc_stats.h:86`

**问题**: `GcDumpStats()` 有完整实现但 **没有任何生产调用者**。压力测试使用自己的 `SnapshotGcStats()` 而不是调用它。进程退出时永不 dump stats。

**建议**: 在进程退出处理器或 GC 析构中接入 `GcDumpStats()`。

### 🔵 LOW: GcEndStubbornChange 为空操作

**位置**: `gc_helpers.h:56`

**问题**: 声明为增量 GC 写屏障函数，注释说 "No-op when bdwgc is not compiled with MANUAL_VDB"。当前无函数体。在 C3+ 增量 GC 之前不会造成问题，但命名可能误导。

**建议**: 添加注释标注 "C3 stub"。

### 🔵 LOW: s_thread_stacks_for_gc_mutex 死代码

**位置**: `gc_old_gen.cpp:1000`

**问题**: 声明了 `static std::mutex s_thread_stacks_for_gc_mutex`，但从未被任何代码引用。自重构为 thread_local `tls_thread_stack` 后的遗留物。

**建议**: 删除此声明。

---

## 6. 线程安全与 STW 模型

### 6.1 GcScanAllThreadRoots 文件静态变量

**位置**: `thread_state.cpp:174-221`
**风险**: 🔵 LOW (v1 延续)

`s_callback` 和 `s_user_data` 是文件作用域静态变量。在 STW 下 GC 线程独占执行，当前安全但脆弱。因 MSVC 不支持 capture lambda→函数指针转换，无法简单改为参数传递。

### 6.2 SafepointPoll 无超时机制

**位置**: `thread_state.cpp:149-155`

**问题**: 自旋等待 10,000 次后调用 `std::this_thread::yield()`，但 **没有实际超时机制**。注释提到 "~10ms without confirmation, GC proceeds with conservative stack scanning anyway (bdwgc fallback)"，但此超时未实现。一个陷入托管代码而不检查 safepoint 的线程可以无限期延迟 GC。

**建议**: 添加实际超时（如 50ms 后放弃等待，GC 用保守栈扫描继续）。

---

## 7. OOM 风险

v2 无新增 HIGH OOM 风险。此前无界 mark stack OOM 已在 v1 修复（`kMaxMarkStack = 256K` 界限 + overflow fallback）。

---

## 8. UAF / 双重释放 / 安全风险

### 8.1 v1 已修复：Free() + GC sweep 双重释放

**修复状态**: ✅ `Free()` 对非 oversized 不再操作 free list，只 memset 破坏 TypeInfo*，完全依赖 GC sweep 回收。

### 8.2 v1 已修复：Finalizer 对存活对象执行

**修复状态**: ✅ `RunFinalizers` 通过 mark bitmap 检查存活状态，仅对 unreachable 执行 finalizer。

### 8.3 Card table 1GB 边界导致的 UAF（v2 新发现）

详见 [§3 GC 策略风险 — Card table 硬限制 1GB 堆](#-high-card-table-硬限制-1gb-堆)。

---

## 9. 性能瓶颈

### 9.1 Young GC Phase 1 脏卡全量扫描

**位置**: `gc_young_collector.cpp:152`
**影响**: 🟡 MEDIUM

每次 young GC 调用 `g_old_gen.ScanDirtyCardsInPages`，遍历 **所有 old-gen 页面**。随 `page_count` O(n) 增长。没有递增的脏页面列表跟踪。

### 9.2 CoalescePage 全量排序 O(n log n)

**位置**: `gc_old_gen.cpp:741`
**影响**: 🔵 LOW

每次 `CoalescePage` 对页面内所有空闲块按地址排序后线性合并。碎片化页面有数百个块时开销显著。

### 9.3 Full GC 线性扫描线程列表

**位置**: `gc_old_gen.cpp:820-833`
**影响**: 🔵 LOW

每次 full GC 调用 `EnumerateThreads`（O(threads) 线性遍历）。100 线程可接受，1000+ 线程显著。

### 9.4 DomainUnloader 全量卡表扫描

**位置**: `domain_unloader.cpp:55`
**影响**: 🔵 LOW

每次域卸载扫描 2M 个 card table 条目（1GB 范围），O(2M) 操作。在 STW 下执行，影响暂停时间。

---

## 10. 测试覆盖缺口

本次分析发现 **7 个测试覆盖缺口**：

| # | 缺口 | 影响 | 严重度 |
|---|------|------|--------|
| 1 | **VerifyPattern 从不失败** | 测试无检测能力，虚假通过 | 🔴 HIGH |
| 2 | **未测试 NurseryAllocateAtomic** | 所有 4 场景只用 NurseryAllocate | 🟡 MEDIUM |
| 3 | **未测试 DomainAllocate** | 域区域分配/释放路径未经测试 | 🟡 MEDIUM |
| 4 | **未测试 finalizer/pinned root** | `RegisterFinalizer`/`AddPinnedRoot` 未经测试 | 🟡 MEDIUM |
| 5 | **未测试 BFS overflow path** | worklist 64K 溢出回退路径未触发 | 🟡 MEDIUM |
| 6 | **未测试 oversized old-gen** | >32KB 直接 old-gen 分配路径未经测试 | 🟡 MEDIUM |
| 7 | **未测试 concurrent full GC** | full GC 只在所有 worker 退出后运行 | 🟡 MEDIUM |

**建议**: 添加 Scenario E (concurrent full GC)、Scenario F (DomainAllocate + unload)、Scenario G (finalizer verification)。

---

## 11. 已修复项目交叉引用 (v1)

以下问题在 v1 分析报告中已发现并修复：

| 修复 | 文件 | 状态 |
|------|------|------|
| Free() + GC sweep 双重释放 | `gc_old_gen.cpp:344-382` | ✅ |
| RunFinalizers 仅对 unreachable 对象执行 | `gc_old_gen.cpp:1069-1115` | ✅ |
| SweepPage size-class 丢弃块 | `gc_old_gen.cpp:585-702` | ✅ |
| 无界 mark stack OOM | `gc_old_gen.cpp:441-579` | ✅ |
| CoalescePage 同样丢弃 | `gc_old_gen.cpp:704-782` | ✅ |
| parallel sweep 过度订阅 | `gc_old_gen.cpp:896` | ✅ |
| CoalescePage 全局 mutex | `gc_old_gen.cpp:704-710` | ✅ |
| MemoryDomain 结构体泄漏 | `memory_domain.cpp:491-512` | ✅ |
| BFS worklist overflow dangling ptr | `gc_young_collector.cpp:313-361` | ✅ |
| domain_unloader refs_found 始终 0 | `domain_unloader.cpp:24-90` | ✅ |
| CollectIncremental → CollectFull | `gc_old_gen.h/.cpp` | ✅ |

---

## 12. 改进建议优先级

### P0: 必须修复 (High)

| # | 问题 | 文件 | 建议修复 |
|---|------|------|---------|
| V2-H1 | CheneyBfsPrecise break→continue | `gc_layout.cpp:241` | break 改为 continue |
| V2-H2 | Card table >1GB 静默失败 | `gc_card_table.h:31,49` | 添加 overflow 标志 + 回退保守扫描 |
| V2-H3 | EstimateObjectSize TLS 错误 | `gc_young_collector.cpp:96` | 通过 RegionManager 查找准确的 nursery |
| V2-H4 | VerifyPattern 从不失败 | `gc_stress_test.cpp:287-312` | 重构为真实检测，检测 stale/corrupted data |

### P1: 重要修复 (Medium)

| # | 问题 | 文件 | 建议修复 |
|---|------|------|---------|
| V2-M1 | Scheduler RecordAllocation 未接线 | `gc_region.cpp:40+` | 慢路径加入 RecordAllocation 调用 |
| V2-M2 | Reallocate 泄漏旧指针 | `gc_old_gen.cpp:384-407` | 分配新块后调用 Free(ptr) |
| V2-M3 | DomainUnloader 方向错误 | `domain_unloader.cpp:55-85` | 增加核心→域引用扫描阶段 |
| V2-M4 | last_alloc_page_ 缓存失效 | `gc_old_gen.cpp:244` | 页面回收时清除缓存的页指针 |
| V2-M5 | GcRecordAlloc 未接线 | `gc_stats.h:76` | 接入所有分配路径 |
| V2-M6 | GcDumpStats 未被调用 | `gc_stats.cpp:10` | 进程退出时调用 |
| V2-M7 | 测试覆盖缺口 (7 项) | `gc_stress_test.cpp` | 添加 Scenario E/F/G |
| V1-M8 | Young GC 脏卡全量扫描 | `gc_young_collector.cpp:152` | 递增脏页面列表 |

### P2: 改进建议 (Low)

| # | 问题 | 文件 | 建议修复 |
|---|------|------|---------|
| V2-L1 | GcEndStubbornChange 空操作 | `gc_helpers.h:56` | 加 C3 stub 注释 |
| V2-L2 | s_thread_stacks_for_gc_mutex 死代码 | `gc_old_gen.cpp:1000` | 删除 |
| V2-L3 | SafepointPoll 无超时 | `thread_state.cpp:149-155` | 添加实际超时机制 |
| V2-L4 | CoalescePage 全量排序 O(n log n) | `gc_old_gen.cpp:741` | 基数排序或哈希表合并 |
| V2-L5 | Full GC 线性线程扫描 | `gc_old_gen.cpp:820-833` | 按需/分根扫描 (C3+) |

---

## 附录 A: 文件间风险依赖图

```
gc_layout.cpp:241 (break→continue)          → young GC transitive closure 丢失
gc_young_collector.cpp:96 (TLS nursery 错误)  → cross-thread 晋升数据损坏
gc_card_table.h:31,49 (1GB cap)              → write barrier 失效 → UAF
domain_unloader.cpp:55-85 (方向错误)          → domain unload 后核心 dangling ptr
gc_region.cpp:40+ (缺 RecordAllocation)      → scheduler 触发条件失效
gc_old_gen.cpp:384-407 (Reallocate 泄漏)     → 内存占用膨胀
gc_old_gen.cpp:244 (cache 失效)             → 不必要的 page_list 遍历
```

## 附录 B: 跨线程 nursery 转发数据流分析

```
线程 A (触发 GC)          线程 B (有对象在 nursery)
    │                           │
    │   GcYoungCollection()      │
    │     Phase 1: 脏卡扫描      │
    │       ┌────────────────────┤
    │       │ 发现 B 的 obj      │
    │       │ GcScavengeObject() │
    │       │   EstimateObjectSize(obj, tls_nursery_ctx.nursery)
    │       │                                 ↑ 线程 A 的 nursery!
    │       │   如果 A.nursery < B.nursery    │
    │       │   → 复制截断                     │
    │       │   → old-gen 中数据损坏           │
    │       └─────────────────────────────────┤
    │     Phase 3: BFS → 使用损坏的副本       │
    │                                         │
    │   nursery sweep → B 的 nursery 重置      │
    │   old-gen 中损坏的副本 → 数据损坏         │
```

**风险缓解条件**: 此问题仅在线程有不同大小的 nursery 时触发。在 `kMinNurserySize=128KB` = `kDefaultNurserySize` 时所有线程 nursery 大小相同，不会触发。但调度器的自适应 `RecommendedNurserySize()`（范围 128KB-1MB）可能在不同线程上产生不同大小。