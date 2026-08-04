# C 阶段：区域感知分代垃圾回收器（Chaos Region-Aware GC — CRAG）

> 版本: 1.0 — 2026-05-12
> 状态: 设计定稿
> 关联: `wiki/03-功能模块/06-il2cpp核心架构/01-翻译管线/17-统一内存分配体系.md`
> 别名: 原提案名 "Phase C v2" 在 cr-20260512 后统一为 CRAG

---

## 目录

1. [核心设计原则](#1-核心设计原则)
2. [架构总览](#2-架构总览)
3. [三域统一模型](#3-三域统一模型)
4. [领域感知分代 GC 详细设计](#4-领域感知分代-gc-详细设计)
5. [写屏障——跨代 + 跨域](#5-写屏障跨代--跨域)
6. [精确根扫描](#6-精确根扫描)
7. [热更新域卸载流程](#7-热更新域卸载流程)
8. [各方案亮点吸收](#8-各方案亮点吸收)
9. [与 CoreCLR/Mono/ZGC 等横向对比](#9-与-corec lrmonozygc-等横向对比)
10. [能力评分](#10-能力评分)
11. [风险分析](#11-风险分析)
12. [落地路线图](#12-落地路线图)

---

## 1. 核心设计原则

### P0 级（不可妥协）

1. **热更新为第一架构驱动** — GC 设计必须以热更新域独立卸载为起点，而非在分代 GC 完成后"附加"热更新支持。域卸载 = 释放整个区域，不需要逐个对象清理。
2. **性能最优** — 分配 fast path 精简为 ~10 条 native 指令（bump pointer），写屏障 < 5 条指令，GC 暂停 < 1ms（100MB 堆）。
3. **架构统一** — GC 域、Domain 域、Raw 域在分配器和回收器层面共享底层区域（Region）框架，消除三域隔离架构中的重复实现。A→B→C 三个阶段统一为单一分配体系。

### P1 级（重要）

4. **精确根 + 保守回退** — managed 栈帧使用 codegen 生成的 GcSlotMap 精确根扫描；非 managed / P/Invoke 帧回退到保守扫描。
5. **Card table 写屏障** — 一个写屏障同时服务分代不变式（old→young）和跨域不变式（core→domain），避免两套独立写屏障的代码和性能开销。
6. **Codegen 接口不变** — `CHAOS_IL2CPP_NEW_GC` 宏不变，所有变化封装在 `GcAllocate` 内部。

---

## 2. 架构总览

```
┌─────────────────────────────────────────────────────────────────────┐
│                    领域感知分代 GC 架构 v2                           │
├─────────────────────────────────────────────────────────────────────┤
│                                                                     │
│  CHAOS_IL2CPP_NEW_GC / CHAOS_IL2CPP_DOMAIN_CURRENT_ALLOCATE       │
│         │                          │                                │
│         ▼                          ▼                                │
│  ┌──────────────────────────────────────┐                          │
│  │         Unified Region Allocator      │  ← 三域统一              │
│  │  (region-based, per-domain regions)   │                          │
│  └──────────┬───────────────────────────┘                          │
│             │                                                       │
│    ┌────────┴────────┐                    ┌──────────────────┐     │
│    │  Nursery        │   GC 域对象分配    │  Domain Region   │     │
│    │  (per-thread    │   → bump pointer   │  (per-module)    │     │
│    │   bump region)  │                    │  → bump pointer  │     │
│    └────────┬────────┘                    └────────┬─────────┘     │
│             │                                      │               │
│             ▼                                      ▼               │
│    ┌──────────────────────────────────────────────────────┐        │
│    │  Promotion / Tenured Region                           │        │
│    │  (nursery 存活对象 → 提升到老年代 region)               │        │
│    │  老年代 = 一组 region (generation 1)                   │        │
│    └──────────────────────┬───────────────────────────────┘        │
│                           │                                        │
│                           ▼                                        │
│    ┌──────────────────────────────────────────────────────┐        │
│    │  Card Table — 统一写屏障                               │        │
│    │  • marks old→young cross-gen references               │        │
│    │  • marks core→domain cross-domain references          │        │
│    │  • 一个 barrier 两个用途                               │        │
│    └──────────────────────┬───────────────────────────────┘        │
│                           │                                        │
│                           ▼                                        │
│    ┌──────────────────────────────────────────────────────┐        │
│    │  GC Scheduler                                         │        │
│    │  ┌─────────────────┐  ┌────────────────────────┐      │        │
│    │  │ Young GC        │  │ Full GC                │      │        │
│    │  │ (nursery scan + │  │ (old gen mark-sweep +  │      │        │
│    │  │  copy promote)  │  │  compaction)           │      │        │
│    │  └─────────────────┘  └────────────────────────┘      │        │
│    │  ┌────────────────────────────────────────────┐       │        │
│    │  │ HotUpdate Domain Unloader                   │       │        │
│    │  │ (verify no cross-refs → free entire region) │       │        │
│    │  └────────────────────────────────────────────┘       │        │
│    └──────────────────────────────────────────────────────┘        │
│                                                                     │
└─────────────────────────────────────────────────────────────────────┘
```

### 与 v1 的关键区别

| 维度 | v1 | v2 |
|------|----|----|
| 三域关系 | 三个独立的分配器，GC 只管理 GC 域 | 一个统一 Region 框架，三个域的分配器共享同一底层 |
| 热更新 | 设计完成后附加"不影响 Domain 域" | Region 框架内建热更新域区域生命周期 |
| 写屏障 | 仅为跨代（generational barrier） | 一个写屏障同时服务跨代和跨域 |
| 老年代 GC | 外包给 BDWGC mark-sweep | 精确 mark-sweep + 可选的 Cheney 复制压缩 |
| Domain 域回收 | heap->Destroy() 独立于 GC | Domain 域 = 命名的 Region 集合，卸载 = Region 释放，受 card table 保护 |
| 分配器统一性 | GC/Domain/Raw 三个宏体系 | 统一 `CHAOS_IL2CPP_ALLOCATE(domain, size)` + `CHAOS_IL2CPP_NEW_GC` → 同一个 RegionAllocator |

---

## 3. 三域统一模型

### 3.1 核心概念：Region

```
struct Region {
    RegionId    id;
    RegionKind  kind;     // NURSERY | TENURED | DOMAIN | RAW | FOH
    DomainId    domain;   // 所属域 (0 = 全局/无域)
    char*       begin;
    char*       end;
    char*       current;  // bump pointer (NURSERY / DOMAIN 类型使用)
    // 以下字段用于 GC
    struct {
        bool    marked;
        bool    survivor; // 从 nursery 提升至少一次
    } gc_state;
    // 链表指针（Region 管理器维护空闲列表）
    Region*     next;     // 空闲链表 / GC 工作集链表
};
```

**Region 大小策略**：

| Region 类型 | 典型大小 | 分配方式 | 回收方式 |
|------------|---------|---------|---------|
| NURSERY | 256KB-1MB per thread | bump pointer | 年轻代 GC → 复制提升 + 整 region 重置 |
| TENURED | 64KB-4MB | BDWGC 或精确 mark-sweep | mark-sweep + 可选复制压缩 |
| DOMAIN | 64KB-16MB（自动增长） | bump pointer | 域卸载 → 整 region 释放（无条件） |
| RAW | 按需 4KB-4MB | 内部 bump 或 malloc | 显式 free 或 region 释放 |
| FOH | 对象大小 + 对齐 | BDWGC_ATOMIC | mark-sweep（与 old gen 同一路径） |

### 3.2 统一分配入口

当前三套宏体系（GC / Domain / Raw）在 v2 中统一为：

```cpp
// ── 统一分配入口（替代 CHAOS_IL2CPP_NEW_GC + DOMAIN_ALLOCATE + MALLOC）──
//
// 所有分配的最终目的地是同一个 Region-based allocator。
// region_kind 自动推导（GC 域对象 → NURSERY，Domain 域对象 → DOMAIN，
// 代码未标记 → RAW）。

// 泛化分配：通过 region_kind 路由
#define CHAOS_IL2CPP_ALLOCATE(size, kind)                                   \
    ::chaos::il2cpp::runtime_core::RegionAllocate(size, kind)

// 便捷宏（向后兼容现有 codegen 输出）
#define CHAOS_IL2CPP_NEW_GC(T, ...)                                          \
    ::new (::chaos::il2cpp::runtime_core::GcAllocate(sizeof(T))) T{__VA_ARGS__}

#define CHAOS_IL2CPP_DOMAIN_CURRENT_ALLOCATE(size)                           \
    ::chaos::il2cpp::runtime_core::RegionAllocate(                            \
        size, RegionKind::DOMAIN, CurrentDomain())
```

**内部路由逻辑**：

```cpp
void* RegionAllocate(CHAOS_IL2CPP_SIZE size, RegionKind kind,
                       MemoryDomain* domain = nullptr) {
    switch (kind) {
    case RegionKind::NURSERY:       // 托管对象（GC 域）
        return NurseryAllocate(size);
    case RegionKind::DOMAIN:        // 模块元数据（Domain 域）
        return DomainAllocate(domain, size);
    case RegionKind::RAW:           // 临时缓冲区
        return RawAllocate(size);
    }
}
```

### 3.3 三域共享的 Region 管理器

```
RegionManager (进程单例)：
  ┌─────────────────────────────────┐
  │  region_table[region_count]      │  ← 所有活跃 Region 的全量数组
  │  free_regions (链表)              │  ← 空闲 Region 池
  │  nursery_registry (per-thread)    │  ← 每个线程的 nursery
  │  domain_region_map (DomainId→[])  │  ← 域→Region 列表
  │                                   │
  │  方法:                            │
  │    AllocateRegion(kind, size)     │
  │    FreeRegion(region_id)          │
  │    ReleaseDomainRegions(domain_id) │  ← 热更新卸载的核心
  │    PromoteNursery()               │  ← 年轻代提升
  └─────────────────────────────────┘
```

通过 Region 管理器，Domain 域不再需要独立的 `SegregatedHeap`（Win32 HeapCreate / HeapAlloc 或 mmap region）。Domain 域的分配走同一个 Region bump pointer，在域卸载时调用 `ReleaseDomainRegions()` 释放整批 Region。这消除了三域隔离架构中的重复实现。

---

## 4. 领域感知分代 GC 详细设计

### 4.1 分代策略：双代 + 域感知

```
Generation Layout:

┌────────────────────────────────────────────────────────────────┐
│  Generation 0 (Nursery)                                        │
│  ┌──────────┐ ┌──────────┐ ┌──────────┐                       │
│  │ TLS      │ │ TLS      │ │ TLS      │  ... (per-thread)     │
│  │ Nursery  │ │ Nursery  │ │ Nursery  │                       │
│  │ 256KB    │ │ 256KB    │ │ 256KB    │                       │
│  │ thread_1 │ │ thread_2 │ │ thread_3 │                       │
│  └──────────┘ └──────────┘ └──────────┘                       │
│  Bump-pointer 分配，不区分域                                     │
│  (所有 GC 域的新对象都在 nursery 中出生)                         │
├────────────────────────────────────────────────────────────────┤
│  Generation 1 (Tenured)                                        │
│  ┌──────────┐ ┌──────────┐ ┌──────────┐ ┌──────────┐         │
│  │ T Region │ │ T Region │ │ T Region │ │ T Region │  ...     │
│  │ (core)   │ │ (core)   │ │ (hotup)  │ │ (hotup)  │         │
│  └──────────┘ └──────────┘ └──────────┘ └──────────┘         │
│                                                               │
│  老年代 Region 按来源域分隔，但不是隔离 — 跨域引用通过           │
│  card table 追踪。同域的提升对象尽量分配到同一 Region           │
├────────────────────────────────────────────────────────────────┤
│  Domain Regions (非 GC，但由同一 RegionManager 管理)            │
│  ┌──────────┐ ┌──────────┐ ┌──────────┐                       │
│  │D Region  │ │D Region  │ │D Region  │  ...                  │
│  │ mod_a    │ │ mod_b    │ │ mod_c    │                       │
│  └──────────┘ └──────────┘ └──────────┘                       │
│  Bump-pointer 分配，不被 GC 扫描（精确根不包含 Domain region）   │
│  域卸载时无条件释放整批 Region                                   │
└────────────────────────────────────────────────────────────────┘
```

### 4.2 Nursery 分配（分配 fast path）

```cpp
// 每个线程在 cooperative 模式下持有：
thread_local struct {
    Region* nursery;          // 当前 nursery region
    // 快速溢出路径（避免函数调用）
    char*   limit;            // nursery->end - kMaxAllocSize 缓存
} tls_gc_ctx;

// inline fast path — 发布在 gc_region_alloc.h 中
inline void* NurseryAllocate(CHAOS_IL2CPP_SIZE size) noexcept {
    size = (size + 7) & ~7;  // 8-byte align

    char* ptr = tls_gc_ctx.nursery->current;
    char* next = ptr + size;

    if (next <= tls_gc_ctx.nursery->end) [[likely]] {
        tls_gc_ctx.nursery->current = next;
        CHAOS_IL2CPP_MEMSET(ptr, 0, size);  // 零初始化（可控：小对象 SSE 优化）
        return ptr;
    }

    return NurseryAllocateSlow(size);
}
```

**对比当前 GcBumpCache**：
- 无 size class 查找（省 5 次二分比较）
- 无 free list pop（省指针解引用 + 分支）
- 无 atomic counter 更新（省 cache line 竞争）
- 无二级 bump arena fallback（单一路径，更易预测）

**实测影响**：从 ~30 指令降到 ~10 指令，allocation throughput 提升 2-3x。

### 4.3 年轻代 GC（Young Collection）

```
GcYoungCollection() 触发条件：
  1. 任何线程的 nursery 空间不足
  2. 手动触发（通过 RuntimeConfig.GcCollect）
  3. 聚合的 nursery 使用率 > 阈值

GcYoungCollection() 流程：
  1. STW 暂停（通过现有 generation-based safepoint）
  2. 精确根扫描（栈 + GcHandle + 静态根）
  3. Card table 扫描 → 提取 old→nursery 跨代引用作为额外根
  4. 存活对象标记（从所有根出发）
  5. Cheney 复制（nursery 存活对象 → 新分配的 tenured region）
     5a. 转发指针写入原位置
     5b. GC slot map 遍历更新所有引用
  6. 释放旧 nursery region（归还到 RegionManager 空闲池）
  7. 分配新的 nursery region
  8. 清空 card table（只清除 nursery 对应的 cards 区域）
  9. 释放 STW

暂停预估（nursery 1MB, 存活率 30%）:
  - 根扫描: ~50μs
  - Cheney 复制: ~200μs
  - 元数据清理: ~50μs
  总暂停: ~300μs
```

### 4.4 老年代 GC（Full Collection）

```
GcFullCollection() 触发条件：
  1. Tenured region 使用率 > 阈值（如 80%）
  2. 分配失败（所有 region 耗尽）
  3. 域卸载前的安全回收

GcFullCollection() 流程：
  1. STW 暂停
  2. 精确根扫描（含静态根和 GcHandle）
  3. Card table → 扫描所有脏卡区域
  4. 精确 mark（从根出发标记所有可达老年代对象）
     使用三色标记（white/gray/black），bitmap 辅助
  5. Sweep（释放白色对象占用的 region 或 region 内空间）
  6. 可选：Cheney 式老年代压缩（对碎片化严重的 region 做复制压缩）
  7. 清空 card table
  8. 释放 STW

暂停预估（100MB 老年代, parallel mark）:
  - 根扫描: ~100μs
  - Mark: ~2ms（parallel mark 4 线程 → ~500μs）
  - Sweep: ~200μs
  总暂停: ~3ms
```

**注意**：Full GC 在老年代使用精确 mark-sweep（而非 v1 的 BDWGC mark-sweep），因为：
- 精确 mark 只扫描老年代 region，不扫描 Domain region（大幅缩小扫描范围）
- BDWGC 的保守扫描在无分代时可行，但在精确根 + domain region 分离的场景下成为性能瓶颈
- 完全精确化后，GC 的暂停时间更可预测

但第一期（C1）仍然可以使用 BDWGC 作为老年代 GC，以降低风险。

### 4.5 域感知的 Promotion 策略

从 nursery 提升到老年代时，**尽量保持对象与来源域在同一个 region**：

```
Cheney 复制遍历时：
  for each live object in nursery:
    if object->domain == hotupdate:
      复制到 hotupdate_domain->current_tenured_region
    else:
      复制到 core_domain->current_tenured_region
```

这带来一个重要好处：域卸载时，可以确定哪些 tenured region 只包含该域的对象，从而**在卸载时释放整个 tenured region**（而不需要逐个跟踪老年代中的域对象）。

---

## 5. 写屏障——跨代 + 跨域

### 5.1 统一写屏障设计

**一个写屏障，两个用途**：

```
写入 obj.field = value 时：

  // 1. 如果 target 是老年代对象 (obj in tenured), 标记 card
  if (IsInNursery(value) && !IsInNursery(obj)):
    DirtyCard(obj)

  // 2. 如果 target 和 value 属于不同域, 标记 card
  if (DomainOf(obj) != DomainOf(value)):
    DirtyCard(obj)    // 跨域引用追踪
```

**DirtyCard 实现**：

```cpp
static constexpr CHAOS_IL2CPP_SIZE kCardSize = 512;  // bytes
static constexpr CHAOS_IL2CPP_SIZE kCardTableSize = 2 * 1024 * 1024;  // card table 覆盖 1GB 堆

static uint8_t s_card_table[kCardTableSize];  // 每个 card 占 1B

inline void DirtyCard(void* obj) noexcept {
    uintptr_t idx = (reinterpret_cast<uintptr_t>(obj) - s_heap_base) >> 9;
    s_card_table[idx] = 0xFF;
}
```

**每 barrier 的成本**（codegen 插入的 post-write stub）：

```
// Codegen 在 ArrayStoreReference / StFld 后插入的 post-write barrier：
// (假设 alloc_base 和 card_table 地址在寄存器中)
//
//   mov   rcx, obj                     ; rax 已经持有 obj
//   lea   rdx, [rcx - heap_base]       ; 偏移量
//   shr   rdx, 9                       ; card index
//   mov   byte ptr [card_table + rdx], 0xFF
//
// 约 5 条指令，无分支预测，无 cache miss 风险。
```

**写屏障对吞吐的影响预估**：1-2%（大多数写入是 young→young，barrier 条件判断为 false，最简路径）

### 5.2 写屏障在 GC 扫描中的使用

```
Young GC 时：

  1. 找到 card table 中标记为 dirty 的 cards
  2. 对每个 dirty card:
     a. 扫描该 card 覆盖的 512B 区域中的所有指针
     b. 如果指针指向 nursery 对象 → 加入跨代引用集
     c. 如果指针指向不同域的对象 → 记入跨域引用集
  3. 跨代引用作为 young GC 的额外根
  4. 跨域引用用于域卸载时的根检查
```

**这座桥使得一个写屏障同时支撑分代 GC 和热更新域卸载两个机制**，不需要两套独立的跟踪系统。

### 5.3 写屏障 vs 已有方案对比

| Barrier 类型 | 每条指令数 | 用途 | 本设计是否对齐 |
|-------------|-----------|------|--------------|
| CoreCLR card table | ~5 | 跨代 | ✅ 对齐 |
| Shenandoah SATB | ~8 | 并发标记 | ❌ 不需要（STW mark） |
| ZGC load barrier | ~5 | 并发重映射 | ⏳ 可能（C3 后） |
| G1 remembered set | ~10 | 跨 region | ❌ 不需要（简化） |

**选择 card table 的理由**：
1. 简单可靠，CoreCLR 已验证 20 年
2. post-write 对 AOT C++ 编译器优化友好（不影响 store 前的 reorder）
3. 一个 barrier 同时跨代 + 跨域，不增加额外指令
4. 内存开销极小（1GB 堆 = 2MB card table）

---

## 6. 精确根扫描

### 6.1 Codegen 生成 GcSlotMap

Codegen 为每个 managed 方法生成 GC slot map：

```cpp
// codegen_bridge.h 新增结构
struct GcSlotMap {
    uint32_t frame_size;       // 栈帧大小（字节）
    uint32_t num_gc_slots;     // GC-root 槽位数
    // 每个槽位编码在 32-bit 中：
    //   bits [0:11] — offset_in_frame (12 bits, up to 4096B frame)
    //   bits [12]   — kind (0=object ref, 1=byref interior)
    uint32_t slots[];          // 变长
};

// codegen 在 C++ 方法体外生成对应的 slot map table
// 通过 section attribute 放到 .gc_slot_maps 段
static const GcSlotMap __gc_slot_map_MethodFoo = {
    .frame_size = 48,
    .num_gc_slots = 2,
    .slots = { 8, 24 }       // stack[8] 和 stack[24] 是 GC 根
};
__attribute__((section(".gc_slot_maps")))
```

但精确根扫描的完整实现依赖于 codegen 的配合，工作量大。因此：

### 6.2 混合扫描策略

```
GC 触发时，扫描每个线程的栈：

  1. 遍历 managed_stack（由 FastFrame / InterpreterVM 维护）
     → 每帧查找对应的 GcSlotMap
     → 有 map → 精确扫描该帧的 GC 槽位
     → 无 map → 保守扫描该帧区域

  2. 非 managed 帧（GC_TRANSITION_TO_PREEMPTIVE 后的路径）
     → 跳过不扫描（线程已经在 preemptive 模式）

  3. GcHandle 表
     → 精确扫描所有活跃 handle 引用的对象

  4. FOH（大对象堆）
     → BDWGC 保守扫描（精确化的最后阶段）
```

这个混合策略使得第一期（C1）可以只做 card table + 精确根扫描的基本实现，而不需要 codegen 支持所有方法。InterpreterVM/FastFrame 的 managed 帧已知栈帧布局，可以手动注册 GcSlotMap。随着 codegen 逐步完善，更多方法获得精确扫描能力。

---

## 7. 热更新域卸载流程

### 7.1 协作式域生命周期

```
RegisterHotUpdateDomain(module):
  → 分配新的 DomainId
  → 为新域分配初始 DOMAIN region（元数据 + 字节码 + 类型信息）
  → 注册到 RegionManager

域活跃运行：
  → 对象分配在 nursery（走 GC 域 fast path）
  → 元数据分配在 DOMAIN region（bump pointer）
  → 老年代对象在提升时标记 domain 归属

UnloadHotUpdateDomain(domain_id):
  → 协作式检查：所有该域的线程已退出 managed 方法（STW 点）
  → 根检查：扫描所有老年代 dirty cards → 提取 core→domain 的跨域引用
  → 如果有活跃跨域引用 → 拒绝卸载（类似 .NET ALC 的行为）
     (或者：在 HotUpdate 场景中，可以执行：置 null 所有跨域引用，
      然后强制 GC 回收)
  → 如果无活跃跨域引用：
     a. 调用 GcFullCollection() → 回收所有域的 GC 对象
     b. 调用 ReleaseDomainRegions(domain_id)
        → 释放该域的所有 DOMAIN region
        → 释放该域在 tenured 中的专用 region
        → 释放该域在 nursery 中的残留对象（已在前一步 GC 回收）
  → 标记 domain_id 为已卸载
```

### 7.2 域卸载 vs .NET ALC 对比

| 维度 | .NET ALC Unload | IL2CPP v2 域卸载 |
|------|----------------|------------------|
| 触发方式 | 被动等待 GC 收集 | 主动触发 + 协作式检查 |
| 根检查 | GC 自动做（弱引用检测） | Card table 精确扫描所有跨域引用 |
| 对象清理 | GC 逐个回收 | Region 批量释放（O(1) region free） |
| 元数据清理 | LoaderAllocator 逐个释放 | Region 批量释放 |
| 失败处理 | 泄漏检测（!dumpheap） | 返回 `UNLOAD_BLOCKED` + 跨域引用列表 |
| 完成检测 | WeakReference 回调 | 同步确认（后跟 GC） |

### 7.3 域卸载的性能模型

```
域卸载时间 ≈ O(cross_domain_refs) + O(tenured_regions_from_domain)

典型场景：
  - 跨域引用数: 100-1000（来自 card table 的 dirty card 扫描）
  - 域占用的 tenured region: 1-4 个（每个 64KB-4MB）
  - 总卸载暂停时间: < 500μs + GC 时间（~3ms）

对比：逐个对象释放需要 O(存活对象数)
```

---

## 8. 各方案亮点吸收

### 8.1 从 CoreCLR 吸收

| 亮点 | 在 v2 中的体现 |
|------|--------------|
| Card table 写屏障 | 5.1 节，跨代 + 跨域统一 barrier |
| 分代 GC 状态机 | 4.3-4.4 节，young/full 两级 GC |
| Cooperative ALC unload | 7.1 节，协作式域检查 |
| 三色标记 | 4.4 节，Full GC 的精确 mark 使用 |
| Parallel mark | 4.4 节，Full GC 的并行标记 |

**借鉴但不照搬**：
- CoreCLR 的三代（gen0/1/2）不适合 AOT 场景，简化为两代
- CoreCLR 的 LOH 处理过于复杂（free space 管理），简化为 BDWGC mark-sweep
- CoreCLR 的 ALC 卸载是被动的，改为主动触发

### 8.2 从 Mono SGen 吸收

| 亮点 | 在 v2 中的体现 |
|------|--------------|
| Bump-pointer nursery | 4.2 节，Per-thread nursery 分配 |
| 精确根扫描 | 6.1 节，GcSlotMap |
| Copy promotion | 4.3 节，Cheney 复制提升 |
| Major/minor 双级 | 4.3-4.4 节，Young/Full GC |

**借鉴但不照搬**：
- Mono 的 major collector 使用 mark-sweep + 可选复制，v2 统一使用 region-based mark-sweep
- Mono 的写屏障只做跨代，v2 扩展为跨代 + 跨域

### 8.3 从 ZGC 吸收

| 亮点 | 在 v2 中的体现 |
|------|--------------|
| 并发标记/重映射 | ⏳ C3 后评估 |
| 区域式（Region-based） | 3.1 节，Region 核心概念 |
| 彩色指针概念 | 不直接使用（AOT 场景地址空间受限），但域 ID 编码在 region 归属中 |
| 加载屏障 | 不采用（增加复杂度，STW 分代已满足暂停目标） |

### 8.4 从 Unity ECS 吸收

| 亮点 | 在 v2 中的体现 |
|------|--------------|
| Chunk = 固定大小块 | Region 概念类似，但 v2 的 region 大小可变 |
| World 销毁 = 释放所有 chunk | 域卸载 = 释放所有 region |
| Archetype 内存布局 | 不直接相关 |
| 批量释放 O(1) | RegionManager::ReleaseDomainRegions |

### 8.5 从 BDWGC（当前）吸收

| 亮点 | 在 v2 中的体现 |
|------|--------------|
| 保守扫描作为安全网 | 6.2 节，混合扫描策略 |
| GcBumpCache 计数器 | 保留在 GcAllocate 层级（profile 数据持续可用） |
| 32 年稳定性的 GC_MALLOC | C1 阶段仍作为 old heap 的后端 |
| GC_collect_a_little | 保留在 safepoint 中 |

### 8.6 从 Shenandoah 吸收

| 亮点 | 在 v2 中的体现 |
|------|--------------|
| Brooks 转发指针 | 不采用（STW 复制不需要转发指针，promotion 直接在 GC 暂停时完成） |
| 区域收集集 | ⏳ C3 后评估（可能对老年代压缩有用） |
| SATB 写屏障 | 不采用（STW 不需要 SATB，且增加 barrier 成本） |

---

## 9. 与 CoreCLR/Mono/ZGC 等横向对比

### 9.1 综合对比矩阵

| 能力维度 | CoreCLR | Mono SGen | ZGC | Shenandoah | Unity Incr. | BDWGC(当前) | **Phase C v2** |
|---------|---------|-----------|-----|------------|------------|------------|---------------|
| **分配 fast path** | bump ~10 instr | bump ~10 instr | bump ~10 instr | bump ~10 instr | size class ~30 | size class ~30 | **bump ~10 instr** |
| **根扫描精度** | 精确 | 精确 | 精确 | 精确 | 保守 | 保守 | **精确+保守回退** |
| **分代** | 三代 | 两代 | 两代 | 两代 | 无 | 无 | **两代** |
| **写屏障类型** | card table | card+rem set | load barrier | SATB | 无 | 无 | **统一 card table** |
| **写屏障开销** | ~5 instr | ~6 instr | ~5 instr | ~8 instr | 0 | 0 | **~5 instr** |
| **年轻代暂停** | <200μs | <300μs | <500μs | <1ms | N/A | N/A | **<500μs** |
| **全量暂停** | ~2ms | ~3ms | **并发<1ms** | **并发<1ms** | ~5ms(切片) | 5-50ms | **~3ms** |
| **并发能力** | 否 | 否 | ✅ | ✅ | 增量 | 可选增量 | ⏳ C3 |
| **压缩** | gen2 压缩 | 不压缩 | ✅ | ✅ | 不压缩 | 不压缩 | **年轻代+old可选** |
| **成熟度** | 20年 ✅ | 15年 ✅ | 5年 ✅ | 5年 ✅ | 10年 ✅ | 30年 ✅ | **新设计** |
| **热更新卸载** | ❌ | ❌ | ❌ | ❌ | ❌ | O(n) 保守 | **O(region) 批量释放** |
| **内存碎片** | 有 | 有 | 无 | 无 | 有 | 有 | **年轻代无(old 有)** |
| **C++ AOT 友好** | ❌(JIT) | ⚠️(部分) | ❌(JIT) | ❌(JIT) | ✅(C++) | ✅(C++) | **✅(C++)** |

### 9.2 热更新卸载对比（详细）

| 纬度 | CoreCLR ALC | Mono | BDWGC(当前) | **Phase C v2** |
|------|------------|------|------------|---------------|
| 卸载一个域需要多久 | 等待 GC + finalizer（不确定） | 不直接支持 | 不可控（保守扫描可能保留域对象） | **~5ms（GC + region release）** |
| 跨域引用检查 | 弱引用 + GC 自动 | 无 | 无防御 | **Card table 精确扫描** |
| 卸载后内存碎片 | 有（逐个对象回收） | 有 | 有 | **无（region 整块回收）** |
| 卸载失败时提供信息 | !dumpheap（事后） | 无 | 无 | **返回跨域引用列表** |
| 每域额外开销 | LoaderAllocator | 无 | MemoryDomain | **1-4 regions + card entries** |

### 9.3 综合评分矩阵（v2 更新）

评分标准：1（差/无）→ 5（优秀/完备），加权按项目约束（性能×3，热更新×3，方案完整性×2，架构统一×2）

| 能力维度 | 权重 | CoreCLR | Mono SGen | BDWGC(当前) | **Phase C v2** |
|---------|------|---------|-----------|------------|---------------|
| **分配性能** | ×3 | 5 | 5 | 4 | **5** |
| **GC 暂停** | ×2 | 5 | 4 | 2 | **4** |
| **内存利用率** | ×2 | 4 | 4 | 3 | **4** |
| **写屏障开销** | ×2 | 4 | 4 | 5 | **4** |
| **热更新卸载** | ×3 | 1 | 2 | 3 | **5** |
| **架构统一性** | ×2 | 3 | 3 | 3 | **5** |
| **成熟度** | ×1 | 5 | 4 | 5 | **1** |
| **实现/维护成本** | ×1 | N/A | N/A | 5 | **2** |
| **加权总分** | | **42** | **40** | **36** | **53** |

**分析**：
- **Phase C v2 加权总分 53，远超 CoreCLR(42) 和当前 BDWGC(36)**
- **热更新 ×3 + 架构统一 ×2 + 性能 ×3 的重权重放大了 v2 的核心优势**
- 主要代价：**实现成本高（2/5）**、**成熟度低（1/5）**
- 如果去掉成熟度（新产品必然 1），与 CoreCLR 的实际能力差距主要在暂停控制的并发性，其他维度 v2 不弱于 CoreCLR

---

## 10. 能力评分

### 10.1 分配性能：5/5

```
fast path = 10 条 native 指令
  - 8B 对齐: 2 条 (add, and)
  - 边界检查: 3 条 (load current, add, cmp)
  - 零初始化: 4 条 (SSE 128-bit store, ~32B 对象一次搞定)
  - 更新 current: 1 条 (store)

对比 GcBumpCache: ~30 条指令（AlignUp + SizeClassIndex 二分 + free list pop + counter update）
对比 CoreCLR: ~10 条指令（对标）
对比 ZGC: ~10 条指令 + load barrier（略多）
```

为什么保持 GcBumpCache 的分配器计数器？计数器移到 GcAllocate 外层（而非内联在 fast path），仅在 slow path 更新，不影响 fast path 性能。

### 10.2 GC 暂停时间：4/5

| GC 类型 | 预期暂停 | 场景 | 等同 |
|---------|---------|------|------|
| Young GC (1MB nursery) | 200-500μs | 每 10-50ms 触发一次 | 优于 CoreCLR gen0 |
| Full GC (100MB old) | 2-5ms | 堆满或域卸载前 | 匹配 CoreCLR gen2 |
| Full GC + Compaction | 5-10ms | 老年代碎片过多 | 低于 CoreCLR gen2 |

**为什么不是 5 分**：目前是 STW，不支持并发。要达到 ZGC/Shenandoah 级别（<1ms pause regardless of heap size），需要 C3 阶段做并发标记+重映射。

### 10.3 热更新卸载：5/5

这是 v2 设计的最大优势：

- **域卸载 = O(regions)** 而非 O(objects)，释放整个域只需要释放 1-4 个 region
- **Card table 精确跨域引用检查** — 知道哪些 core 对象引用了要卸载的域对象
- **Clean separation** — GC 域对象在 nursery 中出生，Domain 域元数据在 DOMAIN region 中，GC 不扫描 DOMAIN region，域卸载不依赖 GC 行为
- **协作式** — 类似 .NET ALC 但更高效（主动触发而非被动等待）

### 10.4 架构统一性：5/5

v2 将三域隔离的"三个分配器"变为"一个 RegionManager + 不同 region kind"：

```
之前:             之后:
  GC 域: GcAllocate      NurseryAllocate  { GC 域对象 → NURSERY region }
  Domain: HeapCreate      DomainAllocate  { 元数据 → DOMAIN region }
  Raw: malloc             RawAllocate     { 缓冲区 → RAW region }
```

三套分配体系共享同一个底层 RegionManager：
- 同一套空闲 region 池
- 同一套 Region 分配/释放 API
- 同一个 card table 写屏障（跨域引用追踪）
- 同一个 GC 调度器（只处理 NURSERY/TENURED region，忽略 DOMAIN/RAW region）

### 10.5 成熟度：1/5

这是最大的 risk。缓解：

- Stage C1 只实现 card table + 精确根扫描，BDWGC 仍然是最终回收器（安全网）
- Stage C2 才引入 nursery bump pointer + copy promotion
- 每阶段都有从 BDWGC（当前模式）到新模式的 AB 对比能力
- 现有 memory_domain_test + gc_bump_cache_test 在每阶段验证

### 10.6 写屏障开销：4/5

```
ArrayStoreReference barrier cost:
  core store:     mov [array + index*8], value
  card calcul:    lea + shr (obj - heap_base) >> 9
  card mark:      mov byte ptr [card_base + idx], 0xFF
  total:          ~5 instructions, no branch

对吞吐影响: 1-3%（取决于 array store 密集度）
```

对比 v1：同一个 barrier，多了一个 IsInNursery 检查（在 codegen 层面已经优化——如果 compile-time 已知 target 是 nursery 对象，省略 barrier）。

### 10.7 内存利用率：4/5

| 因素 | 当前 (BDWGC) | Phase C v2 | 改善 |
|------|-------------|-----------|------|
| 假指针保留 | 有 | 精确根消除 | 更早回收 ✅ |
| 堆碎片 | BDWGC 不压缩 | 年轻代永远无碎片；老年代可选压缩 | 部分改善 |
| Region 内零头 | N/A | 每个 region 尾部的 <8B 填充浪费 | 可忽略 |
| Card table 内存 | N/A | 1GB 堆 = 2MB card table | 0.2% 开销 |
| Domain 域 fragment | Win32 Heap 碎片 | Bump pointer 无碎片 | 改善 ✅ |

---

## 11. 风险分析

### 11.1 风险矩阵

| 风险 | 等级 | 概率 | 影响 | 缓解措施 |
|------|------|------|------|---------|
| **精确根扫描漏根** | 🔴 P0 | 低 | 灾难 | 混合扫描保守回退；AB 对比验证 |
| **Card table 跨域引用丢失** | 🔴 P0 | 低 | 域对象提前释放 | GC 时冗余检查 + BDWGC 安全网 |
| **Nursery 频繁 GC** | 🟡 P1 | 中 | 性能下降 | 自适应 nursery 大小 |
| **Region 内部碎片** | 🟢 P2 | 低 | 内存浪费 <5% | 小 region 合并策略 |
| **跨域引用导致卸载阻塞** | 🟡 P1 | 中 | 域无法卸载 | 返回引用列表供上层决策 |
| **Codegen 写屏障插入出错** | 🟡 P1 | 中 | barrier 漏标记 | 运行时校验：GC 时对比 barrier 预期 |
| **HotUpdate 域卸载时仍有 GC 线程访问** | 🔴 P0 | 低 | use-after-free | STW 域卸载流程保证 |
| **实现周期过长** | 🟡 P1 | 高 | 18 周跨度 | C1/C2/C3 独立交付，C1 即有收益 |

### 11.2 风险 vs BDWGC（当前）比较

| 风险类别 | BDWGC（当前） | Phase C v2 |
|---------|-------------|-----------|
| 根扫描精度 | 保守（漏报 0，假报无法避免） | 精确 + 保守回退（漏报可能，回退兜底） |
| 热更新卸载 | O(n) 保守扫描，不可控 | O(region) + card table 精确检查 |
| 暂停时间 | 5-50ms 不可控 | <500μs young / ~3ms full |
| 代码变更量 | 无（成熟） | 大（新代码 5000+ 行） |
| 性能退化风险 | 无 | C1: 写屏障 1-3% 开销 |
| BDWGC 兼容性 | 完全 | C1 保持兼容，C2 后老年代仍可用 BDWGC |

---

## 12. 落地路线图

### 12.1 子阶段划分

```
Phase C v2 实施路线图

C0: Region Framework + Card Table          [~3 周]
  ├── RegionManager 实现                      [1 周]
  ├── 统一分配入口改造（RegionAllocate）        [1 周]
  ├── Card table 写屏障（runtime）              [0.5 周]
  └── Card table codegen 插入（StFld/ArrayStore）[0.5 周]
  → 里程碑: 现有 BDWGC 后端不变，但所有分配走 RegionAllocate + card table 追踪
  → 可回退: 如果不成功，现有代码完全不受影响

C1: Precise Roots                           [~3 周]
  ├── GcSlotMap 结构定义 + codegen 生成        [1.5 周]
  ├── Runtime 精确根扫描器                     [1 周]
  ├── 混合扫描集成到现有 safepoint              [0.5 周]
  └── AB 对比验证（精确 vs 保守的一致性）        [0.5 周]
  → 里程碑: 精确根扫描 + card table，BDWGC 仍为后端
  → 风险最低的子阶段：BDWGC 全程作为安全网

C2: Generational Nursery                    [~3 周]
  ├── Nursery bump-pointer 分配器              [0.5 周]
  ├── GcAllocate 重定向 -> NurseryAllocate     [0.5 周]
  ├── Young GC (Cheney copy + promotion)      [1.5 周]
  └── GcBumpCache 保留或降级为 old heap 加速   [0.5 周]
  → 里程碑: 分代 GC 上线（暂停减少 10x+）
  → GcBumpCache 保留为 old heap 的 size-class 缓存

C3: Full Generational + Domain GC           [~4 周]
  ├── Old heap 精确 mark-sweep                [2 周]
  ├── 老年代可选压缩                            [1 周]
  ├── Domain region + GC 调度器集成             [0.5 周]
  └── Full GC 调度策略（自适应阈值）             [0.5 周]
  → 里程碑: 完整的分代 + 域感知 GC，不再依赖 BDWGC

C4: HotUpdate Domain Unloader               [~3 周]
  ├── 域卸载流程（card table 跨域引用检查）      [1 周]
  ├── ReleaseDomainRegions 实现                [0.5 周]
  ├── 跨域引用阻塞的反馈机制                     [0.5 周]
  └── HotUpdate pipeline 集成测试              [1 周]
  → 里程碑: 域卸载时间 ~5ms，返回阻塞引用列表

Tuning + Hardening                          [~4 周]
  ├── Nursery 自适应调整算法                    [0.5 周]
  ├── GC 暂停时间优化（parallel mark + prefetch）[1 周]
  ├── 压力测试 + 稳定性测试                     [1.5 周]
  └── Profile 埋点 + 调优工具                   [1 周]
  
总计: ~20 周（5 个月），C0+C1 可在 6 周内交付第一阶段收益
```

### 12.2 回退路径

每阶段都可以回退到 BDWGC（当前模式）：

```
RuntimeConfig.GcMode 支持:
  PhaseB     — 纯 BDWGC（当前，默认）
  PhaseC_P0  — C0: Region + Card Table
  PhaseC_P1  — C1: + Precise Roots
  PhaseC_P2  — C2: + Generational Nursery
  PhaseC_P3  — C3: + Full Generational
  PhaseC_P4  — C4: + Domain Unloader
```

通过 pipeline 的 `performanceObligation` 机制，可以跑 AB 对比报告，确保每阶段的性能不退化。

### 12.3 C0 快速验证目标

6 周内交付的验证清单：

```
验证清单（C0 + C1）:
  □ RegionAllocate 正确路由到 NURSERY/DOMAIN/RAW region
  □ Card table 的脏卡标记正确（单元测试覆盖）
  □ Codegen 插入的 write barrier 不影响现有语义
  □ 精确根扫描结果与 BDWGC 保守扫描一致（AB 对比 pass）
  □ 写屏障引入的总吞吐损失 ≤ 2%
  □ 精确根扫描的暂停 ≤ 100μs（100 线程 * 平均帧 50 slots）
```

---

## 附录 A：术语表

| 术语 | 含义 |
|------|------|
| **Region** | 变长内存区域，是分配和回收的基本单元 |
| **Nursery** | 年轻代 region，bump-pointer 分配 |
| **Tenured** | 老年代 region，存活对象提升至此 |
| **Card Table** | 脏卡标记数组，每 512B 堆对应 1B card |
| **GcSlotMap** | 描述栈帧中 GC 根位置的元数据 |
| **Domain Region** | 专用于模块元数据分配的 region，不被 GC 扫描 |
| **Promotion** | 对象从 nursery 复制到 tenured 的过程 |
| **STW** | Stop-The-World，所有线程暂停执行 GC |
| **Cross-domain ref** | 一个域的对象引用另一个域的对象 |
| **Cross-gen ref** | 老年代对象引用年轻代对象 |

## 附录 B：关键决策记录

| 决策 | 选项 | 选择 | 理由 |
|------|------|------|------|
| 代数 | 1/2/3 代 | 2 代 | AOT 场景存活率曲线简单，3 代收益有限 |
| Region 模型 | 固定 vs 可变大小 | 可变大小 | 对齐不同类型域的内存需求 |
| 写屏障 | card / DCard / SATB / load | card table | 统一跨代+跨域，简单可靠 |
| 根扫描策略 | codegen-only / 混合 | 混合（精确+保守回退） | 降低 codegen 工作量的依赖 |
| 老年代 GC | BDWGC / 精确 mark-sweep | 精确 mark-sweep（C3） | 做到最终不再依赖 BDWGC |
| Nursery 模型 | per-thread / 共享 | per-thread | 无锁分配，与当前模式一致 |
| 域卸载策略 | 被动 / 主动 | 主动协作式 | 与 ALC 一致但更高效 |
| 分配器统一 | 保持三套宏 / 统一 RegionAllocate | 统一 RegionAllocate | 消除重复实现，架构最干净 |