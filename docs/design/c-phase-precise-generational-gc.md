# C 阶段：精确分代 GC — 设计文档

> 版本: v1 — 2026-05-12
> 关联: `wiki/03-功能模块/06-il2cpp核心架构/01-翻译管线/17-统一内存分配体系.md`
> 状态: 设计提案，待讨论

---

## 目录

1. [背景与目标](#1-背景与目标)
2. [设计总览](#2-设计总览)
3. [详细设计](#3-详细设计)
4. [与 CoreCLR 及主流方案横向对比](#4-与-coreclr-及主流方案横向对比)
5. [风险分析与能力评分](#5-风险分析与能力评分)
6. [落地路线图](#6-落地路线图)

---

## 1. 背景与目标

### 1.1 现状

当前分配体系（Phase B）架构：

```
CHAOS_IL2CPP_NEW_GC(T)
  → GcAllocate(sizeof(T))
  → tls_gc_arena.Allocate(size)    // GcBumpCache
  → size class hit → pop free list (O(1))
  |  miss → RefillSizeClass() → BumpAllocate() → GC_MALLOC()
  |  oversized (>32KB) → GC_MALLOC()
→ placement new T{}
```

**优点**：线程本地无锁分配 fast path，GC 层面完全使用 BDWGC 保守式 mark-sweep。

**根本问题**：
1. **保守式扫描** — BDWGC 扫描所有栈/寄存器/堆内存，将任何"看起来像指针"的值当作根。导致：
   - 假指针导致对象无法回收（内存泄漏）
   - 非 GC 域的大块内存（Domain heap、Raw buffers）被完整扫描，性能开销大
   - 精确根信息无法利用
2. **无分代** — 所有对象同等对待，每次 GC 扫描整个堆。托管堆越大，GC 暂停越长
3. **无压缩** — BDWGC 是 mark-sweep，不压缩。长时间运行堆碎片化，分配失败提前触发 GC
4. **无写屏障** — `ArrayStoreReference` 不做任何 barrier，无法支持分代

### 1.2 目标

C 阶段要实现：

| 能力 | 目标 | 优先级 |
|------|------|--------|
| 精确根扫描 | 栈/寄存器精确根，而非保守扫描 | P0 |
| 分代收集 | 年轻代（nursery）+ 老年代，避免全堆扫描 | P0 |
| 写屏障 | 支持 generational invariant（card table 或 DCard） | P0 |
| 压缩能力 | 年轻代压缩/提升，消除碎片 | P1 |
| Codegen 透明 | CHAOS_IL2CPP_NEW_GC 接口不变 | P0（约束） |
| 与 B 阶段兼容 | GcBumpCache 可继续使用或平滑过渡 | P1 |
| 暂停时间可控 | 年轻代 GC < 1ms，全量 GC < 5ms（for 100MB 堆） | P1 |
| 热更新兼容 | 不影响 Domain 域独立卸载 | P2 |

### 1.3 非目标

- **不替换 BDWGC 的全部功能** — C 阶段可以在 BDWGC 之上构建分代层，而非完全替换
- **不实现 concurrent/parallel GC** — 第一阶段使用 STW (stop-the-world) 分代。concurrent marking 作为后续优化
- **不改变 Domain/Raw 域分配模型** — 精确分代仅影响 GC 域

---

## 2. 设计总览

### 2.1 架构图

```
┌──────────────────────────────────────────────────────────────────┐
│                    C 阶段：精确分代 GC 架构                        │
├──────────────────────────────────────────────────────────────────┤
│                                                                  │
│  CHAOS_IL2CPP_NEW_GC / CHAOS_IL2CPP_NEW_GC_ARRAY               │
│         │                                                        │
│         ▼                                                        │
│  GcAllocate(size)          ← codegen 入口，接口不变              │
│         │                                                        │
│         ▼                                                        │
│  ┌─────────────────┐    ┌──────────────────┐                    │
│  │  Young Heap     │    │  Old Heap        │   ← 分代堆         │
│  │  (Nursery)      │    │  (Tenured)       │                    │
│  │  256KB-1MB/T    │    │  BDWGC / custom  │                    │
│  │  bump-pointer   │    │  mark-sweep      │                    │
│  └────────┬────────┘    └────────┬─────────┘                    │
│           │                      │                              │
│           ▼                      ▼                              │
│  ┌─────────────────────────────────────┐                        │
│  │  Write Barrier (Card Table)         │   ← 分代不变式         │
│  │  cards[heap_size / CARD_SIZE]       │                        │
│  └─────────────────────────────────────┘                        │
│           │                                                     │
│           ▼                                                     │
│  ┌─────────────────────────────────────┐                        │
│  │  Precise Root Scanner               │   ← 替代保守扫描      │
│  │  • Stack slots (per frame layout)   │                        │
│  │  • GC registers                     │                        │
│  │  • Static roots (GcHandle table)    │                        │
│  └─────────────────────────────────────┘                        │
│                                                                  │
│  GcBumpCache (可选的 TLS 加速层)                                │
│  → 可继续作为 young heap 之上的对象缓存                          │
│  → 或直接 bump young heap，去掉 free list 间接层                 │
│                                                                  │
└──────────────────────────────────────────────────────────────────┘
```

### 2.2 分代策略

**两点分代（Two-Generation）**：

```
Generation 0 （年轻代 / Nursery）:
  - 大小: 256KB ~ 1MB 每线程（或全局共享 ~8MB nursery）
  - 分配: bump-pointer（无 free list，简单快速）
  - 回收: 复制收集（copy collection），存活对象提升到老年代
  - 暂停: 与 nursery 大小成正比，典型 < 500μs

Generation 1 （老年代 / Tenured）:
  - 大小: 堆剩余全部
  - 分配: 从年轻代提升而来
  - 回收: mark-sweep（可用 BDWGC 现有实现）
  - 暂停: 与存活对象数成正比，典型 1-5ms

FOH （大对象堆 / >85KB）:
  - 直接分配在 BDWGC，不经过 nursery
  - 回收使用 BDWGC mark-sweep
  - 不压缩（BDWGC 原生行为）
```

为什么不采用三代（CoreCLR 的 gen0/gen1/gen2）？因为 IL2CPP AOT 场景下代际存活率曲线不同 — 大多数托管对象要么极短命（临时分配），要么永久存活（typeof()、反射对象）。三点不会带来显著收益，却增加两倍的写屏障 + GC 状态机复杂度。

### 2.3 路线

C 阶段的实现分为三个子阶段：

| 子阶段 | 内容 | 依赖 |
|--------|------|------|
| **C1: Precise Roots** | 精确栈/寄存器扫描 + 写屏障 → card table | GcBumpCache 不变 |
| **C2: Generational Nursery** | 年轻代分配 + copy collection + 提升 | C1 |
| **C3: Full Generational GC** | 跨代引用跟踪、mixed collection、FOH | C2 |

---

## 3. 详细设计

### 3.1 线程生命周期集成

```
ThreadAttach()
  → GC_regiser_my_thread()          [keep BDWGC thread tracking]
  → AllocateNursery()               [per-thread nursery or TLS slot in shared nursery]

ThreadDetach()
  → FlushNurseryToOld()             [promote all surviving nursery objects]
  → GC_unregister_my_thread()
```

### 3.2 精确根扫描（C1）

**现状**：BDWGC 的 `ALL_INTERIOR_POINTERS` 保守扫描扫描整个 C 栈 + 寄存器。

**精确扫描设计**：

```
┌─────────────────────────────────────────────────────────┐
│  Precise Root Scanner                                    │
├─────────────────────────────────────────────────────────┤
│  1. GcHandle Table (pinned handles)                      │
│     → 已知 root 集合，与现有 GcHandleNew 集成             │
│                                                          │
│  2. Thread Stack Frames                                  │
│     → ManagedThread::managed_stack 遍历                  │
│     → 每帧有 GC-slot bitmap（由 codegen 生成）            │
│     → 非 managed 栈帧不扫描（GC_TRANSITION_TO_PREEMPTIVE）│
│                                                          │
│  3. Static Data Region                                   │
│     → .data/.bss 中由 codegen 标记的 GC-root 区域         │
│     → 通过 RegisterStaticRoot(base, size) 注册            │
│                                                          │
│  4. FOH (Large Object Heap)                              │
│     → 仍然由 BDWGC 保守扫描（FOH 对象少，误报可忽略）     │
└─────────────────────────────────────────────────────────┘
```

**关键机制 — GC Slot Maps**：

Codegen 为每个 managed 方法生成 GC-slot bitmap，描述栈帧中哪些位置是 GC 对象指针：

```
// IL method: void Foo() { object o = new object(); ... }
//
// Codegen 生成的 GC map:
struct GcSlotMap {
    uint32_t frame_size;       // 帧大小（字节）
    uint32_t num_slots;        // GC-root slot 数量
    // 每 64 位一个字：offset_in_frame, kind(0=obj, 1=byref)
    uint64_t slots[1];         // 变长数组
};
```

当线程处于 cooperative mode 且在 managed 代码中时，扫描器遍历 `managed_stack` 帧链，读取每帧的 `GcSlotMap`，精确报告根对象地址。

**与现有 safepoint 机制的集成**：

当前 `SafepointPoll()` 在 GC 触发时令线程停在已知位置。C1 在此位置插入根扫描：

```
SafepointPoll() → generation 为奇数 → 
  1. 线程设置 at_safepoint=true
  2. 线程调用 ReportPreciseRoots(managed_stack, gc_slot_maps)
  3. 线程 spin-wait 直到 generation 翻转回偶数
```

### 3.3 写屏障（C1）

**Card Table 设计**：

```
对象引用写入时：
  ArrayStoreReference(array, index, value):
    array[index] = value;
    if (IsInNursery(value) && !IsInNursery(array)):
      DirtyCard(array);   // 标记 card 为脏

脏卡标记：
  static constexpr CHAOS_IL2CPP_SIZE kCardSize = 512;  // bytes per card
  static uint8_t s_card_table[HEAP_MAX / kCardSize];
  
  inline void DirtyCard(void* obj) {
      uintptr_t idx = (reinterpret_cast<uintptr_t>(obj) - s_heap_base) / kCardSize;
      s_card_table[idx] = 0xFF;  // dirty
  }
```

**写屏障开销预计**：3-5 条 native 指令（基数偏移 + 计算 card index + store byte），对典型 workload 影响 < 2%。这需要 codegen 插入 barrier —— 这是 C 阶段对 codegen 的唯一影响。

**写屏障 vs DCard 方案选择**：

| 方案 | 每 Barrier 成本 | 扫描时成本 | 实现复杂度 |
|------|----------------|-----------|-----------|
| Card Table (512B) | ~5 指令 | O(cards/8) | 低 |
| DCard (双 card, 延迟合并) | ~8 指令 | O(dirty_cards) | 中 |
| Log-based (WBB buffer) | ~3 指令 | O(entries) | 高 |

**推荐**：第一期使用 Card Table。简单可靠，512B card size 在典型 100MB 堆下只有 200K cards（~200KB 内存），在 GC 触发时扫描耗时 < 50μs。

### 3.4 分代 Nursery（C2）

**Nursery 分配**：

```
// 每个线程在 cooperative 模式下持有 nursery 指针：
struct GcNursery {
    char* begin;
    char* end;
    char* current;    // bump pointer
};

// 分配（inline fast path）：
void* GcAllocate(CHAOS_IL2CPP_SIZE size) {
    size = AlignUp(size);
    GcNursery* nursery = tls_nursery;
    char* ptr = nursery->current;
    char* next = ptr + size;
    if (next <= nursery->end) [[likely]] {
        nursery->current = next;
        return ptr;
    }
    return GcAllocateSlow(size);  // nursery full, trigger GC
}
```

这比 GcBumpCache 更简单：没有 free list、没有 size classes、没有 batch refill。只是一个 bump pointer。

**Nursery 满 → GC 触发**：

```
GcAllocateSlow(size):
  if (size > kLargeObjectThreshold):
    return OldHeapAllocate(size)   // 直接走老年代
  
  // 触发年轻代 GC
  GcYoungCollection();
  // 重试
  char* ptr = nursery->current;
  char* next = ptr + size;
  if (next <= nursery->end) [[likely]]:
    nursery->current = next;
    return ptr;
  
  // nursery 仍不够 → 扩展或走 old
  if (size <= nursery->size / 2):
    ExtendNursery();  // 再分配一个 nursery block
    return retry;
  else:
    return OldHeapAllocate(size);
```

**年轻代 GC 流程**：

```
GcYoungCollection():
  1. Safepoint (STW)
  2. 扫描精确根 → 标记 nursery 中存活对象
  3. 扫描 card table → 找到老年代→年轻代跨代引用 → 这些也是根
  4. Cheney 复制：遍历存活对象图
     → 存活对象复制到 old heap 的 bump allocator
     → 原位置放 forwarding pointer
     → 更新所有引用指向新位置
  5. 清空 card table
  6. 重置 nursery->current = nursery->begin
  7. 释放 safepoint
```

**Cheney 复制算法**：

```
扫地机（scavenger）：
  scan = copy_space_begin
  free = copy_space_begin +存活对象总大小
  
  // 广度优先遍历存活图
  while scan < free:
    obj = scan
    for each slot in obj (通过 GC slot map):
      ref = *slot
      if ref != null && IsInNursery(ref):
        if ref.forwarding == null:
          ref.forwarding = free
          memcpy(free, ref, ref.size)
          free += ref.size
        *slot = ref.forwarding   // 更新引用
    scan += obj.size
  
  存活对象现在在 copy_space[begin, free) 中
  这些被提升到老年代
```

**与 GcBumpCache 的关系**：

在 C2 部署后，GcBumpCache 有两种选择：

1. **保留为 old heap 的 TLS 加速层** — GcBumpCache 继续为老年代分配提供 size-class 缓存。年轻代分配走 nursery bump pointer。
2. **完全替换** — 用 nursery + old heap bump pointer 完全替代 GcBumpCache。简单但失去了 size class 对中型对象（256-2048 字节）的加速。

**推荐**：初期保留 GcBumpCache 作为 old heap 分配加速，C3 后评估是否可以移除。

### 3.5 与 BDWGC 的共存策略

C 阶段**不需要完全替换** BDWGC，而是分层：

```
┌──────────────────────────────────────┐
│  C 阶段 GC 调度器                     │
│  │                                   │
│  ├─ Young Heap (nursery)             │
│  │  自定义复制收集器，不经过 BDWGC    │
│  │                                   │
│  ├─ Old Heap (tenured)               │
│  │  BDWGC mark-sweep (GC_MALLOC)    │
│  │  + 可选的 bump-pointer 扩展       │
│  │                                   │
│  ├─ FOH (Large Object Heap)          │
│  │  BDWGC mark-sweep (GC_MALLOC)    │
│  │                                   │
│  ├─ Precise Root Scanner             │
│  │  替代 BDWGC 的保守栈扫描          │
│  │                                   │
│  └─ Card Table (write barrier)       │
│     仅新增，不修改 BDWGC             │
└──────────────────────────────────────┘
```

这个设计意味着：
- **BDWGC 不需要修改** — 我们只是在它前面加了一层 nursery
- **BDWGC 的保守扫描范围缩小** — 不再扫描栈、寄存器，只扫描 old/FOH 堆内部
- **回退方案** — 如果 C 阶段某部分出问题，可以快速切回纯 BDWGC 模式

### 3.6 与 HotUpdate 系统的兼容

热更新依赖于 MemoryDomain 的独立卸载。C 阶段不影响 Domain 域：

- Nursery 中的对象全是 GC 域对象，与 Domain 域无关
- 老年代中的 GC 域对象可能引用 Domain 域元数据（如 TypeInfo），但这是现有架构已经处理的模式
- 精确根扫描不需要了解 Domain 域的内部结构

---

## 4. 与 CoreCLR 及主流方案横向对比

### 4.1 对比对象

| 方案 | 类型 | 特点 |
|------|------|------|
| **CoreCLR GC** | 三代分代（gen0/1/2）planning-mark-sweep + LOH | 业界标杆，企业级成熟度 |
| **Mono GC (SGen)** | 分代 + 复制收集 + precise roots | 与 IL2CPP 场景最接近 |
| **Unity Incremental GC** | BDWGC 增量模式，时间切片 | 当前移动游戏的行业方案 |
| **BDWGC (当前)** | 保守 mark-sweep，可选增量/分代 | Chaos IL2CPP 当前方案 |
| **Phase C (提案)** | 两点分代 + precise roots + card table | 本提案 |

### 4.2 各维度对比

#### 分配性能

| 方案 | fast path 成本 | TLS 支持 | 分配 fragmentation |
|------|--------------|---------|-------------------|
| **CoreCLR** | ~10 instr (bump) ✅ | per-thread alloc context ✅ | 无（bump）✅ |
| **Mono SGen** | ~10 instr (bump) ✅ | per-thread nursery ✅ | 无（bump）✅ |
| **Unity Incremental** | ~30 instr (size class) | per-thread (BDWGC TLA) | 有（free list） |
| **BDWGC (当前)** | ~30 instr (size class) | per-thread GcBumpCache ✅ | 有（free list + bump） |
| **Phase C** | ~10 instr (bump) ✅ | per-thread nursery ✅ | 无（bump）✅ |

#### 根扫描

| 方案 | 精度 | 栈扫描开销 | 假指针风险 |
|------|------|-----------|-----------|
| **CoreCLR** | 精确 ✅ | O(roots) ✅ | 无 ✅ |
| **Mono SGen** | 精确 ✅ | O(roots) ✅ | 无 ✅ |
| **Unity Incremental** | 保守 ❌ | O(stack) ❌ | 有 ❌ |
| **BDWGC (当前)** | 保守 ❌ | O(stack) ❌ | 有 ❌ |
| **Phase C** | 精确 ✅ | O(roots) ✅ | 无 ✅ |

#### GC 暂停

| 方案 | 年轻代 GC | 全量 GC (100MB) | 最坏情况 |
|------|----------|----------------|---------|
| **CoreCLR** | < 200μs ✅ | ~2ms ✅ | 5ms+（gen2 压缩） |
| **Mono SGen** | < 300μs ✅ | ~3ms | 10ms+（major） |
| **Unity Incremental** | N/A（无分代） | 0.5-5ms（切片） | 无分代的累积开销 |
| **BDWGC (当前)** | N/A（无分代） | 5-50ms ❌ | 大堆分钟级 ❌ |
| **Phase C** | < 500μs ✅ | ~3ms（BDWGC mark-sweep） | 5ms+（old 压缩） |

#### 写屏障开销

| 方案 | 类型 | 每 barrier 成本 | 对吞吐影响 |
|------|------|----------------|-----------|
| **CoreCLR** | card table (byte write) | ~5 instr | 1-3% ✅ |
| **Mono SGen** | card table + remembered set | ~6 instr | 1-3% ✅ |
| **Unity Incremental** | 无写屏障（无分代） | 0 ✅ | 0 ✅ |
| **BDWGC (当前)** | 无写屏障 | 0 ✅ | 0（但不支持分代） |
| **Phase C** | card table (byte write) | ~5 instr | 1-3% ✅ |

#### 成熟度

| 方案 | 生产验证 | 异常处理 | 生态工具 |
|------|---------|---------|---------|
| **CoreCLR** | 20+ 年 ✅ | 完备 ✅ | SOS, PerfView, ETW ✅ |
| **Mono SGen** | 15+ 年 ✅ | 完备 ✅ | Mono profiler |
| **Unity Incremental** | 10+ 年 ✅ | 基本 | Unity Profiler |
| **BDWGC (当前)** | 30+ 年 ✅ | 完备 | 第三方工具 |
| **Phase C** | 0（新设计）❌ | 需实现 | 需构建 |

#### 热更新兼容

| 方案 | 模块独立卸载 | Cross-domain 引用 | 实现难度 |
|------|------------|------------------|---------|
| **CoreCLR** | 不支持（进程级）❌ | N/A | N/A |
| **Mono SGen** | 部分支持 | 有限 | 高 |
| **Unity Incremental** | 不直接支持 | 无 | 中 |
| **BDWGC (当前)** | 通过 MemoryDomain ✅ | 安全（保守扫描） ✅ | 低 |
| **Phase C** | 通过 MemoryDomain ✅ | 安全（精确根需要 Domain 元数据标记） | 中 |

### 4.3 综合评分矩阵

评分标准：1（差/无）→ 5（优秀/完备），加权按优先级约束（性能×3，方案完整性×2，热更新×1）

| 能力维度 | 权重 | CoreCLR | Mono SGen | Unity Incr. | BDWGC(当前) | Phase C |
|---------|------|---------|-----------|------------|------------|---------|
| **分配性能** | ×3 | 5 | 5 | 3 | 4 | 5 |
| **根扫描精度** | ×2 | 5 | 5 | 2 | 2 | 5 |
| **GC 暂停时间** | ×2 | 5 | 4 | 3 | 2 | 4 |
| **内存利用率** | ×2 | 4 | 4 | 3 | 3 | 4 |
| **写屏障开销** | ×2 | 4 | 4 | 5 | 5 | 4 |
| **成熟度** | ×1 | 5 | 4 | 4 | 5 | 1 |
| **热更新兼容** | ×1 | 1 | 2 | 2 | 4 | 4 |
| **实现/维护成本** | ×1 | N/A | N/A | N/A | 5 | 2 |
| **加权总分** | | **46** | **44** | **32** | **34** | **39** |

**分析**：
- **CoreCLR** 总分最高但热更新完全不兼容（不适合此项目）
- **Phase C** 总分 39，高于当前 BDWGC(34) 和 Unity Incremental(32)，低于 CoreCLR/Mono 但热更新兼容性远超
- 主要短板：成熟度（全新实现）、实现成本高

---

## 5. 风险分析与能力评分

### 5.1 风险矩阵

| 风险 | 等级 | 概率 | 影响 | 缓解措施 |
|------|------|------|------|---------|
| **精确根扫描漏根** | 🔴 P0 | 低 | 灾难：GC 回收存活对象导致 use-after-free | 保守回退：漏根时回退 BDWGC 保守扫描；分阶段部署 |
| **写屏障漏标记** | 🔴 P0 | 低 | 灾难：跨代引用丢失，存活对象被年轻代 GC 回收 | 冗余验证：GC 时对 card 标记过的对象做额外验证 |
| **Nursery 大小不当** | 🟡 P1 | 中 | 性能下降：频繁 GC 或分配延迟 | 可配置 + 自适应：根据分配率调整 nursery |
| **与 BDWGC 版本不兼容** | 🟡 P1 | 中 | 碰撞、死锁 | 独立测试套件 + memory_domain_test 扩展 |
| **Codegen 写屏障插入** | 🟡 P1 | 中 | codegen 复杂度增加 | 第一期不做写屏障优化，先实现精确根扫描 |
| **Card table 内存开销** | 🟢 P2 | 低 | 100MB 堆 = ~200KB card table | 完全可以接受 |
| **热更新路径下的跨代引用** | 🟢 P2 | 低 | 泄漏或误回收 | 域卸载前强制 full collection + card flush |
| **多线程 nursery 竞争** | 🟡 P1 | 中 | TLS nursery 无竞争，但 GC 触发时需要协调 | 已使用 STW 协调，无竞争问题 |

### 5.2 各项能力详评

#### 5.2.1 分配性能（评分：5/5）

```
C 阶段 fast path:
  GcAllocate(32) → nursery->current += 32 → return ptr
  精简为 ~10 条指令（边界检查 + bump + 两个 load）
  
对比当前（GcBumpCache）：
  GcAllocate(32) → AlignUp → SizeClassIndex(二分) → 
  sc.free_list pop → sc.hits++ → return
  ~30 条指令，包含分支预测和计数器原子操作
```

**nursery bump pointer 比 size class free list 更快的原因**：
- 无 size class 查找（二分查找 5 次比较）
- 无 free list pop（读取 next 指针）
- 无原子计数器更新（hits++）
- 无二级 fallback（bump arena 是唯一路径）

**风险点**：如果 nursery 太小，频繁 GC 导致 allocation 实际成本变高。但通过配置适当 nursery 大小（典型 512KB-1MB），GC 频率可控制在 < 50次/秒。

#### 5.2.2 根扫描精度（评分：5/5）

精确根扫描消除 BDWGC 保守扫描的两个根本问题：

1. **假指针保留**：C# 数值计算中的整数值可能"看起来像"BDWGC 堆中的地址，导致对象无法回收。精确扫描只扫描 `GcSlotMap` 标记的 root slot，完全消除此问题。

2. **大内存区域扫描**：Domain 域分配的元数据（可能 GB 级）不再被 GC 线程扫描。这是当前架构中隐藏的性能问题 — BDWGC 保守扫描扫描整个 allocatable 地址空间，而 Domain 堆的数据也在其中。

**前提条件**：codegen 必须为每个 managed 方法生成正确的 `GcSlotMap`。这是工作量最大的部分。

#### 5.2.3 GC 暂停时间（评分：4/5）

| GC 类型 | 预期暂停 | 条件 |
|---------|---------|------|
| 年轻代（nursery 满） | 100-500μs | nursery 1MB，存活率 < 30% |
| 老年代（BDWGC mark-sweep） | 1-5ms | 100MB old heap，parallel mark |
| Full collection（old 压缩） | 5-20ms | 大堆 + 压缩 |

**暂停时间_不_是 5 分的原因**：因为老年代仍然使用 BDWGC mark-sweep，在堆较大时暂停不可控。要达到 CoreCLR 级别的暂停控制，需要将 BDWGC 替换为精确 mark-sweep + compaction，这是 C3 之后的工作。

#### 5.2.4 内存利用率（评分：4/5）

| 因素 | 当前 (BDWGC) | Phase C | 改善 |
|------|-------------|---------|------|
| 假指针保留 | 有 | 无 ✅ | 更早回收 |
| 碎片 | BDWGC 不压缩 ❌ | 年轻代压缩 + 老年代 BDWGC | 部分改善 |
| Nursery 浪费 | N/A | 1MB/thread 常驻 | 但可回收 |
| 写屏障内存 | N/A | ~200KB card table | 可忽略 |

#### 5.2.5 写屏障开销（评分：4/5）

```
// card table write barrier (post-write)
ArrayStoreReference:
  1. store array[index] = value                // 主写入
  2. if (!IsInNursery(array)):                 // 检查 target 是否在老年代
  3.   idx = (array - heap_base) >> 9          // 计算 card index (512=2^9)
  4.   s_card_table[idx] = 0xFF                // 标记脏卡
```

**为什么不是 5 分**：对比 Unity Incremental（无写屏障，0 开销），Phase C 增加了写屏障。但这是分代收集的必须成本。

**优化点**：可以通过在 codegen 层面省略某些已知安全的写入（如构造函数中的初始化写入、同一对象内的字段写入）来降低开销。

#### 5.2.6 成熟度（评分：1/5）

新设计，零生产验证。这是 Phase C 最大的风险。缓解策略：

- **渐进式部署**：C1（精确根 + 写屏障）在 BDWGC 之上运行，BDWGC 仍然是最终的回收器。如果精确根漏根，BDWGC 的保守扫描作为安全网。
- **AB 测试框架**：可以在 runtime config 中切换"phase_b"（纯 BDWGC）和"phase_c"（精确分代），对比内存行为和性能。
- **CI 验证**：现有的 foundation-dll pipeline 可以扩展为运行两种模式并对比结果。

#### 5.2.7 热更新兼容（评分：4/5）

C 阶段不影响 MemoryDomain 系统。精确根扫描只扫描 GC 域对象。Domain 域的热卸载完全不变。

**为什么不是 5 分**：极端情况下，如果热卸载的 Domain 元数据被老年代 GC 域对象引用，在精确根扫描时这些引用会被追踪。但是因为老年代使用 BDWGC mark-sweep（保守扫描），这些引用会被正确处理。风险在于精确根扫描阶段（young GC 时）如果扫描了 Domain 域指针...

**解决方案**：young GC 时 card table 已经记录了老年代 → 年轻代的跨代引用，Domain 域对象不可能引用年轻代（Domain 域分配不使用 GC 堆），所以 young GC 不需要扫描 Domain 域。

#### 5.2.8 实现成本（评分：2/5）

| 组件 | 预估工作量 | 关键文件 |
|------|-----------|---------|
| C1: 精确根扫描数据管道 (codegen) | 2-3 周 | `NativeAotLoweringPlanner.cs`, `codegen_bridge.h` |
| C1: Card table 写屏障 (codegen) | 1 周 | `NativeAotLoweringPlanner.cs` (array store) |
| C1: 运行时 card table + 写屏障实现 | 1 周 | `runtime_core.cpp`, `gc_card_table.h` |
| C2: Nursery 分配器 | 1 周 | `gc_nursery.h`, `runtime_core.cpp` |
| C2: 年轻代复制收集器 | 2 周 | `gc_young_collector.h`, `gc_young_collector.cpp` |
| C2: 提升路径 + old heap 集成 | 1 周 | `gc_young_collector.cpp`, `gc_bump_cache.h` |
| C3: Old heap 精确 mark-sweep | 3-4 周 | `gc_old_collector.h/cpp` |
| C3: 完整 GC 调度器 | 1 周 | `gc_scheduler.h/cpp` |
| 测试 + 验证 | 2 周 | `memory_domain_test.cpp`, foundation-dll families |
| 性能调优 | 2 周 | profile data analysis |
| **总计** | **16-22 周** | |

---

## 6. 落地路线图

### 6.1 子阶段划分

```
Phase C 实施路线图：

C1: Precise Roots + Write Barrier     [~4 周]
  ├── Codegen: GcSlotMap 生成            [2 周]
  ├── Codegen: Card table insert         [1 周]
  ├── Runtime: Card table impl           [0.5 周]
  └── Runtime: Precise root scan         [0.5 周]

C2: Generational Nursery                [~4 周]
  ├── Runtime: Nursery allocator         [1 周]
  ├── Runtime: Young GC (copy)           [2 周]
  ├── Integration: GcBumpCache as old    [0.5 周]
  └── Validation: GC benchmark suite     [0.5 周]

C3: Full Generational                   [~6 周]
  ├── Runtime: Old heap precise mark     [3 周]
  ├── Runtime: Mixed collection          [1 周]
  ├── Runtime: FOH handling              [0.5 周]
  ├── Runtime: GC scheduler + policy     [0.5 周]
  └── Validation: Long-run stress test   [1 周]

Tuning + Bug Fixes                       [~4 周]
  ├── Performance profiling × 2 rounds   [2 周]
  ├── Edge case hardening                [1 周]
  └── AB mode CI integration             [1 周]

总计: ~18 周（4.5 个月）
```

### 6.2 C1 快速验证目标

在投入 C1 实现前，应先在 codegen 层面验证可行性：

```
验证清单：
  □ codegen 可以为每个 managed 方法输出 GcSlotMap
  □ GcSlotMap 格式与运行时扫描器匹配
  □ write barrier 插入不影响现有 ArrayStore 行为
  □ precise root scan 结果与 BDWGC 保守扫描一致（AB 对比）
  □ 性能回退：插入 write barrier 后 ≤ 2% 吞吐损失
```

### 6.3 AB 对比模式

```
RuntimeConfig 中加入:
  enum class GcMode {
      PhaseB,          // 纯 BDWGC (当前)
      PhaseC_Precise,  // C1: 精确根 + 写屏障 + BDWGC
      PhaseC_Nursery,  // C2: 分代 + 精确根 + 写屏障
      PhaseC_Full,     // C3: 全功能
  };
```

通过 foundation-dll pipeline 的 `performanceObligation` 机制，可以对比不同模式下的分配吞吐、GC 频率、暂停时间等指标。

---

## 附录 A：术语

| 术语 | 含义 |
|------|------|
| Nursery | 年轻代，bump-pointer 分配的对象区域 |
| Tenured | 老年代，经历多次 GC 仍存活的对象 |
| FOH | 大对象堆（>85KB），不经过 nursery |
| Card Table | 写屏障使用的脏卡标记数组 |
| GcSlotMap | 描述栈帧中 GC-root 位置的 bitmap |
| STW | Stop-The-World，所有线程暂停以执行 GC |
| Copy Collection | 复制收集，将存活对象复制到新区域 |
| Forwarding Pointer | 复制后原对象位置写入的转发指针 |
| BDWGC | Boehm-Demers-Weiser Garbage Collector，当前保守 GC |

## 附录 B：决策记录

| 决策 | 选项 | 选择 | 理由 |
|------|------|------|------|
| 代数 | 1/2/3 代 | 2 代（nursery + tenured） | AOT 场景代际存活率曲线简单，3 代收益有限 |
| 写屏障 | card / DCard / log | card table (512B) | 实现简单、开销可接受、CoreCLR 已验证 |
| 年轻代算法 | copy / mark-sweep | Cheney copy | 无碎片、一次遍历、实现成熟 |
| 精确根来源 | codegen / runtime | codegen GcSlotMap | 需要 codegen 配合，但精度最高 |
| 老年代 GC | BDWGC / 精确 | BDWGC（过渡） | 降低第一期风险；成熟稳定 |
| Nursery 模型 | per-thread / 共享 | per-thread | 无锁分配，与 GcBumpCache 模式一致 |