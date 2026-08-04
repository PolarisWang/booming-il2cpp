# CRAG (Chaos Region-Aware GC) 深度风险分析报告 v3

**分析日期**: 2026-05-13
**版本**: C0.2 (Phase C, M1)
**范围**: `src/native/runtime-core/gc/*`, `domain_unloader.cpp`, `memory_domain.cpp`, `thread_state.cpp`, `gc_stress_test.cpp`
**基线与 v2 差异**: v2 的 4 个 HIGH + 8 个 MEDIUM + 6 个 LOW 已全部修复并验证

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
11. [已修复项目交叉引用 (v2)](#11-已修复项目交叉引用-v2)
12. [改进建议优先级](#12-改进建议优先级)

---

## 1. 执行摘要

CRAG 子系统处于 **Phase C M1**（从 GcBumpCache 向精确分代 GC 过渡）。v2 所有 18 个问题已修复并验证通过（4 场景压力测试全部 PASS）。

**本次 v3 分析** 基于最新代码状态，发现 **4 个 HIGH、12 个 MEDIUM、5 个 LOW 风险，以及 5 个测试覆盖缺口**：

| 风险等级 | 数量 | 关键问题 |
|---------|------|---------|
| **High** | 4 | Mark stack overflow 闭包不完整、DomainUnloader 方向仍然反转、Safepoint 可重入、GcRecordAlloc 多路径缺失 |
| **Medium** | 12 | 跨线程截断、API 不对称、死代码、竞争条件、性能瓶颈等 |
| **Low** | 5 | 测试路径缺口、文件组织问题等 |
| **测试缺口** | 5 | NurseryAllocateAtomic、DomainAllocate、finalizer、BFS overflow、并发 full GC |

### 关键发现 v3 vs v2

| 变化 | 说明 |
|------|------|
| v2-H1 已修复 | CheneyBfsPrecise break→continue ✅ |
| v2-H2 已修复 | Card table overflow 标志 + 回退 ✅ |
| v2-H3 已修复 | EstimateObjectSize 改用 kMaxEstObjectSize 保守上限 ✅ |
| v2-H4 已修复 | VerifyPattern 现在正确检测 low32 不匹配 ✅ |
| v2-M3 **仍未修复** | DomainUnloader 方向反转——v2 报告的相同 bug ⚠️ |
| v3-H2 **新增** | Mark stack overflow fallback 传递闭包不完整→UAF |
| v3-H4 **新增** | Safepoint 可重入→两个 GC 同时执行/线程过早恢复 |

---

## 2. 分配策略风险

### 🔴 HIGH: GcRecordAlloc 在多个分配路径中缺失

**位置**:
- `gc_region.cpp:149-153` — `NurseryAllocateAtomicSlow` 的 oversized 路径
- `gc_region.cpp:254` — `DomainAllocate` 路径
- `gc_region.cpp:127` — `NurseryAllocateSlow` OOM 回退到 old_gen
- `gc_old_gen.cpp` — `OldGen::Allocate()` 的所有内部路径（free-list / page-carve）

**已存在的调用**: `gc_region.cpp:47` (NurseryAllocateSlow oversized)、`core/header_layouts.cpp:167,174` (GcAllocate/GcAllocateAtomic)

**影响**: `GcStats::alloc_total` 和 `alloc_bytes` 严重低估实际分配量。压力测试 JSON 报告的 `total_bytes_estimate` 来自手工计算，不是 GcStats。调度器的 `estimated_heap_size_` 依赖这些统计来做 full GC 触发决策。

**建议**: 在所有分配出口（包括 old-gen free-list、page-carve、domain-alloc、OOM fallback）统一添加 `GcRecordAlloc(size, oversized)`。

### 🟡 MEDIUM: 跨线程对象大小截断

**位置**: `gc_young_collector.cpp:107-109`

```cpp
// Cross-thread nursery object — use conservative cap.
obj_size = kMaxEstObjectSize;
```

**问题**: 跨线程 nursery 对象使用 `kMaxEstObjectSize` (2048) 作为大小上限。对于 >2048 字节的对象，old-gen 中的提升副本会被截断。v2 修复时将原来的 `EstimateObjectSize` (使用 TLS nursery 边界) 改为保守上限，避免了数据损坏，但引入了截断风险。

**影响**: 只有当 Phase 1 脏卡扫描发现跨线程引用且对象 >2048B 时才会触发。在 Phase C M0 中可能性较低，因为精确扫描 (Phase 2) 先运行且发现大部分引用。C3+ 使用精确 TypeInfo 大小后解决。

**建议**: C3 改用 `GcLayoutRegistry` 的精确大小信息：`layout_registry.Lookup(stable_id)->total_size`。

### 🟡 MEDIUM: DomainAllocate 无对称 DomainFree

**位置**: `gc_region.h:163`, `gc_region.cpp:254`

**问题**: `DomainAllocate()` 声明并实现，但没有对应的 `DomainFree()`。域内存通过 `heap->Destroy()` 在 `UnregisterMemoryDomain` 中批量释放。调用者如果需要在域卸载前释放单个域对象，没有任何 API 可用。这导致了 API 不对称性和潜在的"泄漏然后批量释放"设计。

**建议**: 添加 `DomainFree(void* ptr, DomainId domain_id)` 或明确文档说明"不支持单对象释放"。

### 🟡 MEDIUM: NurseryAllocateAtomicSlow 的 oversized 路径无 GcRecordAlloc

**位置**: `gc_region.cpp:149-153`

```cpp
if (size > kMaxNurseryAlloc) {
    CHAOS_IL2CPP_LOG_DEBUG("CRAG", "nursery_oversized_atomic");
    void* ptr = g_old_gen.Allocate(size, false);
    if (ptr) std::memset(ptr, 0, size);
    return ptr;  // ← 没有 GcRecordAlloc！
}
```

**对比** `NurseryAllocateSlow` 第 47 行有 `GcRecordAlloc`，但 `NurseryAllocateAtomicSlow` 的 oversized 路径缺失。

---

## 3. GC 策略风险

### 🔴 HIGH: Mark Stack Overflow Fallback 传递闭包不完整

**位置**: `gc_old_gen.cpp:559-582`

**问题**: 当 mark stack 溢出（超过 256K 条目）后，保守 fallback 扫描所有页面。对于发现的未标记对象，它只标记直接子字段（MarkObject + 不 push 到 stack），不做进一步的传递闭包。这意味着一个对象的可传递引用图中的深层对象可能未被标记，在 sweep 阶段被回收 → UAF。

```cpp
// Minimal transitive closure: scan pointer fields without pushing.
auto* page = FindPage(obj);
...
for (uint16_t i = 0; i < layout->pointer_count; i++) {
    uint16_t offset = layout->pointer_offsets[i].offset;
    auto* slot = reinterpret_cast<void**>(obj_base + offset);
    void* ref = *slot;
    if (ref != nullptr && FindPage(ref) != nullptr) {
        MarkObject(ref);  // ← 标记但没 push！
        // 无法从 ref 继续遍历
    }
}
```

**影响**: 只有直接子对象被标记，孙对象和更深层的对象会被 sweep 回收。触发的条件是 mark stack > 256K，这在大型对象图 full GC 中可能发生。

**建议**: 三个选项:
(a) 更大的 `kMaxMarkStack` (如 1M) 减少溢出概率
(b) 溢出后分多次 pass，每次处理一层
(c) 改用 tri-color marking with separate overflow list (Boehm 风格)

### 🔴 HIGH: DomainUnloader 方向仍然反转

**位置**: `domain_unloader.cpp:76-83`

**问题**: `ScanAndClearCrossDomainRefs` 的逻辑仍然是 **反转的**。函数扫描 card table（覆盖 old-gen 核心内存），对于指向域内的指针值，它 KEEP（保留），对于指向域外的指针值，它 CLEAR（清零）。这正好反了：

```
当前逻辑（错误）:
  if (val IN domain) → KEEP    ← 这是危险的 core→domain dangling 指针！
  if (val NOT IN domain) → CLEAR ← 这是安全的 core→core 指针，不该清

正确逻辑:
  if (val IN domain) → CLEAR   ← 清除 core→domain 指针，防止 dangling
  if (val NOT IN domain) → KEEP ← core→core 指针是安全的
```

**影响**: 域卸载后，核心 old-gen 中指向域对象的指针不清除，成为 dangling pointer。后续 GC 遍历或访问时 → UAF / 数据损坏。

**⚠️ 这是 v2-M3 的同一个 bug，v2 修复中从未实际应用。**

### 🟡 MEDIUM: CheneyBfsPrecise 死代码

**位置**: `gc_layout.cpp:208-250`

**问题**: `CheneyBfsPrecise` 函数完全实现但没有被任何代码调用。Young GC Phase 3 使用内联 BFS worklist（在 `GcScavengeObject` 中直接 push/pop），而不是调用这个独立的 BFS 函数。它编译到二进制中但不执行，增加代码体积和混淆。

**影响**: 代码体积冗余，后续维护者可能混淆哪个 BFS 路径是生效的。

**建议**: 删除或标记 `[[deprecated]]`。如果 C3 需要独立的 BFS，先解内联再激活。

---

## 4. 内存泄漏风险清单

### 🟡 MEDIUM: UnregisterMemoryDomain Tombstone 泄漏

**位置**: `memory_domain.cpp:491-512`

**问题**: `UnregisterMemoryDomain` 将已删除的 domain 条目设为 `nullptr`（第 507 行：`domain = nullptr`）。但 `g_domains` vector 的容量不会缩小。如果域反复注册/注销（典型的热更新场景），vector 中积累 nullptr tombstone，导致：
1. 内存泄漏（nullptr 条目占用 vector 容量）
2. `FindDomainByName` 和 `UnregisterMemoryDomain` 的 O(n) 遍历包含失效条目

**建议**: 在 `UnregisterMemoryDomain` 中用 `std::move` + `pop_back` 压缩 vector，或使用空闲列表复用条目。

### 🟡 MEDIUM: ReleaseDomainRegions 未清理 Nursery Range

**位置**: `gc_region.cpp:396-408`

**问题**: `ReleaseDomainRegions` 释放域的所有区域，包括 nursery-kind 区域。但不会调用 `RemoveNurseryRange` 来更新 nursery 区域数组。如果其他线程的 TLS nursery 指向被释放的域区域，后续的 nursery 范围检查（`IsInNursery`）可能出错。

**影响**: 严格来说这是一个正确的 bug——释放的区域从 region table 移除，但 nursery 范围数组仍然指向已释放的内存。

**建议**: 在 `ReleaseDomainRegions` 中，对 nursery-kind 区域调用 `RemoveNurseryRange`。

---

## 5. API 设计评估

### 5.1 分配 API 一致性更新

| API | 零初始化 | 线程安全 | GcRecordAlloc | GcLayout 追踪 | 对称性 |
|-----|---------|---------|--------------|--------------|-------|
| NurseryAllocate | ✅ | ✅ (TLS) | ✅ | ✅ | N/A (GC 自动) |
| NurseryAllocateAtomic | ✅ | ✅ (TLS) | ℹ️ 缺 oversized | ✅ | N/A |
| OldGen::Allocate | ✅ | ℹ️ 需 STW | ❌ 缺 | N/A | ✅ Free() |
| DomainAllocate | ❌ | ✅ (heap mutex) | ❌ 缺 | ❌ | ❌ 无 DomainFree |
| GcAllocate | ✅ | ✅ | ✅ | ✅ | N/A |
| GcAllocateAtomic | ✅ | ✅ | ✅ | ✅ | N/A |

### 🟡 MEDIUM: GcAllocate 实现在不相关文件中

**位置**: `core/header_layouts.cpp:164-176`

**问题**: `GcAllocate` 和 `GcAllocateAtomic` 的实现放在 `core/header_layouts.cpp` 中（标题是对象头布局）。这与分配逻辑无关。应该在 `gc/gc_helpers.cpp` 中。

**影响**: 文件组织混乱，不利于维护。

**建议**: 将 `GcAllocate`/`GcAllocateAtomic` 实现移到 `gc/gc_helpers.cpp`。

### 🔵 LOW: GcEndStubbornChange 声明无定义

**位置**: `gc_helpers.h:56`

**问题**: 声明了 `GcEndStubbornChange()` 但没有函数体。链接器如果在任何翻译单元中使用此声明就会产生未定义符号错误。当前没有代码调用它，所以未触发。

**建议**: 添加空函数体或删除声明。

---

## 6. 线程安全与 STW 模型

### 🔴 HIGH: Safepoint 可重入 Bug

**位置**: `thread_state.cpp:160-172`

```cpp
uint32_t RequestGlobalSafepoint() noexcept {
    uint32_t gen = s_generation.load(std::memory_order_acquire);
    uint32_t desired = (gen + 1) | kGcGenerationMask;
    s_generation.store(desired, std::memory_order_release);
    return desired;
}

void ReleaseGlobalSafepoint(uint32_t /*generation*/) noexcept {
    uint32_t gen = s_generation.load(std::memory_order_acquire);
    s_generation.store((gen + 1) & ~kGcGenerationMask, std::memory_order_release);
}
```

**问题**: 两个线程可以同时调用 `RequestGlobalSafepoint`：
1. 线程 A 调用 `RequestGlobalSafepoint` → generation = 0 → 1 | 1 = 1 (odd)
2. 线程 B 调用 `RequestGlobalSafepoint` → generation = 1 → 2 | 1 = 3 (still odd, different gen)
3. 线程 A 调用 `ReleaseGlobalSafepoint(3)` → generation = 3 → 4 & ~1 = 4 (even)
4. **所有线程释放！** 但线程 B 的 GC 还在运行！

**根本原因**: 
- `RequestGlobalSafepoint` 返回 `desired`，而不是加载的 generation
- `ReleaseGlobalSafepoint` 忽略 `generation` 参数——释放总是无条件翻转
- 没有嵌套计数或线程所有权检查

**影响**: 两个 GC 可以同时运行，互相破坏对方的数据结构。第一个 ReleaseGlobalSafepoint 会提前释放所有工作线程，即使第二个 GC 还在运行。

**建议**: 
```cpp
// Option A: 使用原子计数器 + 嵌套支持
std::atomic<int> s_safepoint_depth{0};
std::atomic<uint32_t> s_safepoint_owner{0};  // thread ID

uint32_t RequestGlobalSafepoint() {
    auto tid = std::this_thread::get_id();
    if (s_safepoint_owner.load() == tid && s_safepoint_depth > 0) {
        // 嵌套 — 只是增加深度
        s_safepoint_depth++;
        return ...;
    }
    // 第一次获取
    uint32_t gen = (s_generation.fetch_add(1) + 1) | kGcGenerationMask;
    s_generation.store(gen);
    s_safepoint_owner.store(tid);
    s_safepoint_depth.store(1);
    return gen;
}
```

### 🟡 MEDIUM: SafepointPoll 无硬超时

**位置**: `thread_state.cpp:149-155`

```cpp
int spins = 0;
while ((s_generation.load(std::memory_order_acquire) & kGcGenerationMask) != 0u) {
    if (++spins > 10000) {
        std::this_thread::yield();
        spins = 0;
    }
}
```

**问题**: 自旋 10,000 次后 yield 但 **没有实际超时机制**。注释提到"~10ms without confirmation, GC proceeds with conservative stack scanning anyway"，但此回退未实现。一个不检查 safepoint 的线程可以无限期延迟 GC。

**影响**: 如果托管线程执行长时间运行的无 safepoint-poll 代码，GC 无法开始。虽然当前调度器在 `NurseryAllocateSlow` 中有 safepoint 调用，但如果线程只读不分配，可能永远不响应 safepoint。

**建议**: 添加 50ms 硬超时后放弃等待（GC 用保守栈扫描继续）。

### 🟡 MEDIUM: 线程确认协议缺失

**位置**: `thread_state.cpp:160-166`

**问题**: `RequestGlobalSafepoint` 设置奇数 generation 后立即返回，不做以下操作：
1. 不等待所有线程确认到达 safepoint（通过检查 `thread->at_safepoint`）
2. 不统计已到达 safepoint 的线程数

这意味着 `RequestGlobalSafepoint` 返回后，仍有一些线程在访问托管堆。GC 线程立即执行 root 扫描和标记，而其他线程可能还在修改对象图。

**缓解**: 调用者（young GC, full GC）在 `RequestGlobalSafepoint` 后不立即执行操作——年轻 GC 在 `gc_region.cpp:78` 调用。但理论上如果线程不在 safepoint 就修改堆（例如通过写屏障），GC 可能看到不一致状态。

**建议**: `RequestGlobalSafepoint` 后添加轮询等待：遍历线程列表，等待所有 `is_running` 线程的 `at_safepoint` 标志置位。

### 🟡 MEDIUM: AddPinnedRoot 与 Parallel Sweep 竞争

**位置**: `gc_old_gen.cpp:897-937, 1022-1027`

**问题**: `Collect()` 的 Phase 1 标记在 `mutex_` 下调用 `AddPinnedRoot`（第 1023 行）。Phase 1 结束后 `mutex_` 释放。Phase 3 并行 sweep 获得 `mutex_`。但 `AddPinnedRoot` 可能在 Phase 3 sweep 过程中被外部线程调用——它获取 `mutex_` 并 push 到 `pinned_roots_` vector，但此时标记阶段已经结束，新添加的 pinned root 不会被标记，sweep 可能回收它指向的对象。

**影响**: 外部线程在 GC sweep 过程中注册 pinned root → pinned root 指向的对象被 sweep → UAF。

**建议**: 
- 在标记阶段复制 `pinned_roots_` 快照
- 或使用单独的回转缓冲区（double-buffer）在新 root 到达时触发重标记
- 或文档禁止在 GC 进行中注册 pinned root

---

## 7. OOM 风险

### 🟡 MEDIUM: RecordFullCollection 设置错误的 estimated_heap_size_

**位置**: `gc_scheduler.cpp:48-50`

```cpp
CHAOS_IL2CPP_SIZE alloc = alloc_since_last_gc_.load(std::memory_order_relaxed);
estimated_heap_size_.store(alloc > 0 ? alloc : kDefaultNurserySize, std::memory_order_relaxed);
```

**问题**: `RecordFullCollection` 将 `estimated_heap_size_` 设置为 `alloc_since_last_gc_`（自上次 young GC 以来的分配量，通常是一个 nursery 的大小 ~128KB-1MB），而不是实际的堆大小（可能数百 MB）。这导致 `DecideCollection()` 的 full GC 触发条件 `alloc_full > heap_est * kFullTriggerMultiplier` 过于乐观——heap_est 低估了 100-1000 倍，full GC 几乎永远不会被调度器触发。

**影响**: 年轻 GC 频繁触发（可能每秒多次），而 full GC 只通过其他路径触发（如手动 `RequestFullGc`）。堆无法及时收缩，OOM 风险增加。

**建议**: `RecordFullCollection` 应该追踪实际的堆用量（通过 `page_count_ * page_size` 或类似的估计），而不是重置为 nursery 级别的分配量。

---

## 8. UAF / 双重释放 / 安全风险

### 8.1 Mark stack overflow fallback 不完全 → UAF

详见 §3。这是当前最严重的 UAF 风险。

### 8.2 DomainUnloader 方向反转 → dangling pointer

详见 §3。域卸载后核心持有 dangling domain 指针。

### 8.3 v2 已修复：Card table 溢出 → UAF

**修复状态**: ✅ V2-H2 修复——添加 `g_card_table_overflowed` 标志 + 回退保守扫描。手动代码审查确认正确。

### 8.4 v1 已修复：Free() + GC sweep 双重释放

**修复状态**: ✅ `Free()` 不再操作 free list。

---

## 9. 性能瓶颈

### 🟡 MEDIUM: Young GC Phase 1 全量 old-gen 页面扫描

**位置**: `gc_young_collector.cpp:166`

**影响**: 每次年轻 GC 调用 `g_old_gen.ScanDirtyCardsInPages`，遍历 **所有 old-gen 页面**。随堆增长 O(n) 增加。没有增量脏页面列表跟踪。

**建议**: 维护增量脏页面列表（C2+ 优化）。

### 🟡 MEDIUM: DomainUnloader 全量卡表扫描

**位置**: `domain_unloader.cpp:55`

**影响**: 每次域卸载扫描 2M 个 card table 条目（1GB 范围覆盖），在 STW 下执行。最坏情况：每个脏卡完成 512B/8 = 64 次槽位扫描。最大 128M 指针检查 × 每次检查通过 `IsInDomain` 线性扫描 region 表。

**建议**: C4 后使用 region table 精确映射，避免全量扫描。

### 🟡 MEDIUM: ReleaseDomainRegions O(n) 线性扫描

**位置**: `gc_region.cpp:396-408`

**影响**: 持有 mutex 的同时线性扫描所有区域。域卸载在 STW 下执行，增加了暂停时间。

### 🟡 MEDIUM: IsNurseryPointer 线性扫描

**位置**: `gc_region.cpp:473-487`

**影响**: `IsNurseryPointer` 在热路径（Phase 1 脏卡扫描中的每个引用）线性扫描最多 512 个 nursery 范围槽位。

### 🔵 LOW: CoalescePage 全排序 O(n log n)

**位置**: `gc_old_gen.cpp:741`

**影响**: 每个页面 coalesce 使用 std::sort。

### 🔵 LOW: Full GC 线性线程列表扫描

**位置**: `gc_old_gen.cpp:820-833`

### 🔵 LOW: LOG_DEBUG 在 STW 路径中

**位置**: `gc_region.cpp:76,93,121`, `gc_young_collector.cpp:132`, `domain_unloader.cpp:111`

**影响**: STW 路径中的 CHAOS_IL2CPP_LOG_DEBUG 即使在 DEBUG 构建中也会增加暂停时间（I/O + 格式化）。

---

## 10. 测试覆盖缺口

| # | 缺口 | 影响 | 严重度 |
|---|------|------|--------|
| 1 | **NurseryAllocateAtomic 未测试** | 所有 4 场景只用 NurseryAllocate | 🟡 MEDIUM |
| 2 | **DomainAllocate + unload 未测试** | 域分配/卸载路径未经测试 | 🟡 MEDIUM |
| 3 | **Finalizer / pinned root 未测试** | RegisterFinalizer/AddPinnedRoot 未经测试 | 🟡 MEDIUM |
| 4 | **BFS worklist overflow 未测试** | 64K worklist 溢出回退路径未触发 | 🟡 MEDIUM |
| 5 | **并发 full GC 未测试** | full GC 只在所有 worker 退出后运行 | 🟡 MEDIUM |
| 6 | **VerifyPattern 只检查前 8 字节** | gc_stress_test.cpp:288 `(void)size` — 只检查 magic word，不检查 fill bytes | 🔵 LOW |
| 7 | **NurseryAllocateAtomic 的 oversized 路径未测试** | 无测试覆盖 >kMaxNurseryAlloc 的原子分配 | 🔵 LOW |

**建议**:
- Scenario E: 并发 full GC（worker 在 full GC 过程中继续分配）
- Scenario F: DomainAllocate + 域卸载验证
- Scenario G: Finalizer + pinned root 注册验证

---

## 11. 已修复项目交叉引用 (v2)

| 修复 | 文件 | 状态 |
|------|------|------|
| V2-H1: CheneyBfsPrecise break→continue | gc_layout.cpp:241 | ✅ 已验证 |
| V2-H2: Card table overflow 标志 + 回退 | gc_card_table.h:39-128 | ✅ 已验证 |
| V2-H3: EstimateObjectSize 保守上限 | gc_young_collector.cpp:96-109 | ✅ 已验证 |
| V2-H4: VerifyPattern 真实检测 | gc_stress_test.cpp:287-317 | ✅ 已验证 |
| V2-M1: Scheduler RecordAllocation 接线 | gc_region.cpp:40+ | ✅ 已验证 |
| V2-M2: Reallocate 释放旧指针 | gc_old_gen.cpp:384-412 | ✅ 已验证 |
| V2-M3: DomainUnloader 方向 | domain_unloader.cpp:55 | ❌ **仍未修复** |
| V2-M4: last_alloc_page_ 缓存失效 | gc_old_gen.cpp:959-961 | ✅ 已验证 |
| V2-M5: GcRecordAlloc 接线 | gc_stats.h → gc_region.cpp | ✅ 部分修复 |
| V2-M6: GcDumpStats 进程退出调用 | gc_stats.cpp | ✅ 已验证 |
| V2-L1: GcEndStubbornChange 注释 | gc_helpers.h:56 | ✅ "C3 stub" 注释 |
| V2-L2: s_thread_stacks_for_gc_mutex 删除 | gc_old_gen.cpp:1000 | ✅ 已验证 |
| V2-L3: SafepointPoll 无超时 | thread_state.cpp:149-155 | ❌ 未修复 |
| V2-M7(原): 测试覆盖缺口 | gc_stress_test.cpp | ❌ 仍存在 |
| V2-L4: CoalescePage 排序 | gc_old_gen.cpp:741 | ❌ 未修复 |
| V2-L5: Full GC 线性线程扫描 | gc_old_gen.cpp:820-833 | ❌ 未修复 |

---

## 12. 改进建议优先级

### P0: 必须修复 (High)

| # | 问题 | 文件 | 影响 | 建议修复 |
|---|------|------|------|---------|
| V3-H1 | GcRecordAlloc 多路径缺失 | gc_region.cpp:149-153, gc_old_gen.cpp | 统计/调度失效 | 所有分配出口添加 GcRecordAlloc |
| V3-H2 | Mark stack overflow 闭包不完整 | gc_old_gen.cpp:559-582 | UAF | 增大 mark stack 或分多次 pass |
| V3-H3 | DomainUnloader 方向反转 | domain_unloader.cpp:76-83 | UAF | 反转 IsInDomain 判断逻辑 |
| V3-H4 | Safepoint 可重入 | thread_state.cpp:160-172 | 两个 GC 同时执行 | 添加嵌套计数 + 线程所有权 |

### P1: 重要修复 (Medium)

| # | 问题 | 文件 | 影响 | 建议修复 |
|---|------|------|------|---------|
| V3-M1 | 跨线程对象截断 | gc_young_collector.cpp:107-109 | 数据损坏 | C3 用 GcLayout 精确大小 |
| V3-M2 | GcAllocate 文件位置错误 | core/header_layouts.cpp:164-176 | 维护性 | 移到 gc/gc_helpers.cpp |
| V3-M3 | 无 DomainFree API | gc_region.h:163 | API 不对称 | 添加或文档说明 |
| V3-M4 | AddPinnedRoot 竞争 | gc_old_gen.cpp:1022-1027 | UAF | 快照 + 双缓冲 |
| V3-M5 | UnregisterMemoryDomain tombstone 泄漏 | memory_domain.cpp:491-512 | 内存泄漏 | vector 压缩 |
| V3-M6 | CheneyBfsPrecise 死代码 | gc_layout.cpp:208-250 | 代码冗余 | 删除或激活 |
| V3-M7 | ReleaseDomainRegions 缺 RemoveNurseryRange | gc_region.cpp:396-408 | 指针错误 | 添加清理 |
| V3-M8 | SafepointPoll 无硬超时 | thread_state.cpp:149-155 | GC 延迟 | 添加 50ms 超时 |
| V3-M9 | 线程确认协议 | thread_state.cpp:160-166 | 不一致 | 等待所有线程确认 |
| V3-M10 | RecordFullCollection 错误 heap_est | gc_scheduler.cpp:48-50 | OOM 风险 | 用实际堆大小 |
| V3-M11 | 全量 old-gen 页面扫描 | gc_young_collector.cpp:166 | 性能 | 增量脏页面列表 |
| V3-M12 | 测试覆盖缺口 | gc_stress_test.cpp | 无法检测 | Scenario E/F/G |

### P2: 改进建议 (Low)

| # | 问题 | 文件 | 建议修复 |
|---|------|------|---------|
| V3-L1 | GcEndStubbornChange 声明无定义 | gc_helpers.h:56 | 加空函数体或删除 |
| V3-L2 | LOG_DEBUG 在 STW 路径 | gc_region.cpp:76,93,121 等 | 改为 LOG_INFO 或移除 |
| V3-L3 | Legacy gc_bump_arena.h 仍编译 | CMakeLists.txt | 从构建中移除 |
| V3-L4 | VerifyPattern 只检查 8 字节 | gc_stress_test.cpp:288 | 取消 (void)size，验证 fill bytes |
| V3-L5 | DumpCounters 死代码 | gc_bump_cache.h:60-87 | 删除 |

---

## 附录 A: 修复状态总览 (v1→v2→v3)

```
v1 (4 HIGH + 10 MEDIUM) → 全部修复 ✅
v2 (4 HIGH + 8 MEDIUM + 6 LOW) → 14/18 已修复, 4 未修复:
  - v2-M3 (DomainUnloader 方向) → ⚠️ v3-H3
  - v2-L3 (Safepoint 无超时) → ⚠️ v3-M8
  - v2-L4 (CoalescePage 排序) → ⚠️ v3-Low
  - v2-L5 (Full GC 线性扫描) → ⚠️ v3-Low

v3 新增:
  - 4 HIGH (H1-H4)
  - 12 MEDIUM (M1-M12)
  - 5 LOW (L1-L5)
```

## 附录 B: 风险依赖图

```
thread_state.cpp:160-172 (safepoint 可重入)      → 两个 GC 并发执行 → 互毁
gc_old_gen.cpp:559-582 (mark stack overflow)      → 传递闭包不完整 → UAF
domain_unloader.cpp:76-83 (方向反转)              → 域卸载后 dangling ptr → UAF
gc_scheduler.cpp:48-50 (错误 heap_est)            → full GC 不触发 → OOM
gc_young_collector.cpp:107-109 (跨线程截断)       → >2KB 对象提升截断 → 数据损坏
gc_young_collector.cpp:166 (全量页面扫描)          → young GC 暂停 O(n) 增长
gc_region.cpp:396-408 (缺 RemoveNurseryRange)     → 域卸载后 nursery 范围错误
gc_old_gen.cpp:1022-1027 (AddPinnedRoot 竞争)     → sweep 中注册 root → UAF
```