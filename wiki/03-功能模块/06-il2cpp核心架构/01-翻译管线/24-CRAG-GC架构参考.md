# CRAG GC 架构参考

> CRAG (Chaos Region-Aware GC) — 分代式、Region 感知、精确扫描的垃圾回收器。
> 代码位于 `src/native/runtime-core/gc/`，核心模块约 15 个文件。

## 架构总览

```
┌──────────────────────────────────────────────────────────────┐
│                     CRAG GC 架构                              │
├──────────────────────────────────────────────────────────────┤
│                                                              │
│  线程 1 (TLS Nursery)    线程 2 (TLS Nursery)    ...         │
│  ┌──────────────────┐   ┌──────────────────┐                │
│  │ Bump pointer     │   │ Bump pointer     │                │
│  │ 256KB region     │   │ 256KB region     │                │
│  └────────┬─────────┘   └────────┬─────────┘                │
│           │                      │                          │
│           ▼                      ▼                          │
│  ┌────────────────────────────────────────────────────┐     │
│  │         Young Collection (STW safepoint)            │     │
│  │  Phase 1: Scan dirty cards (old→nursery refs)      │     │
│  │  Phase 2: Precise nursery scan (GcLayout)          │     │
│  │  Phase 3: Cheney BFS transitive closure            │     │
│  │  Phase 4: Sweep nursery + clear card table         │     │
│  └──────────────────────┬─────────────────────────────┘     │
│                         │                                    │
│                         ▼                                    │
│  ┌────────────────────────────────────────────────────┐     │
│  │              Old Generation                         │     │
│  │  28 size classes × 64KB pages, mark bitmap         │     │
│  │  Parallel mark (work-stealing deques)              │     │
│  │  Optional compaction (page-internal, 30% threshold)│     │
│  └──────────────────────┬─────────────────────────────┘     │
│                         │                                    │
│                         ▼                                    │
│  ┌────────────────────────────────────────────────────┐     │
│  │              Large Object Heap                      │     │
│  │  Objects >85KB, VirtualAlloc segments              │     │
│  │  Mark-sweep, no compaction                         │     │
│  └────────────────────────────────────────────────────┘     │
│                                                              │
│  ┌────────────────────────────────────────────────────┐     │
│  │              Card Table (two-level)                 │     │
│  │  L1: 64K segment pointers (512KB)                  │     │
│  │  L2: 128 cards/segment × 512B = 64KB coverage      │     │
│  │  Total: 4GB heap, no mutex on write barrier        │     │
│  └────────────────────────────────────────────────────┘     │
└──────────────────────────────────────────────────────────────┘
```

## 核心设计决策

### 分代策略 (Generational)

| 代 | 区域 | 分配方式 | 回收算法 | 触发条件 |
|----|------|---------|---------|---------|
| Young (Nursery) | 每线程 256KB region | Bump pointer | Cheney 复制 + 精确 GcLayout 扫描 | 分配预算耗尽 (EMA 阈值) |
| Old (Tenured) | 28 size class × 64KB pages | Size-class freelist | Mark-sweep (并行) + 可选 compact | 分配失败 / 显式 GC.Collect |
| LOH | VirtualAlloc segments | 直接分配 | Mark-sweep (不压缩) | >85KB 对象 |

### Region 框架

所有内存区域由 `RegionManager` 统一管理：

| Region 类型 | 用途 | 大小 | 回收方式 |
|------------|------|------|---------|
| NURSERY | 每线程年轻代 | 256KB (可调) | Young GC 后释放或重用 |
| TENURED | 老年代 page | 1MB | Mark-sweep + 可选 compact |
| DOMAIN | 模块元数据 | 64KB | 域卸载时批量释放 |
| RAW | 临时分配 | 按需 | malloc/free |

### 精确扫描 (Precise Scanning)

- **GcLayout 注册表**：进程级 hash table (4096 槽开放寻址)，keyed by TypeInfo stable_id
- **Young GC Phase 2**：逐对象读取 TypeInfo* → 查 layout → 只扫描 layout 声明的 pointer offset
- **Full GC DrainMarkStack**：相同机制，消除 conservative 误报
- **TypeInfo 地址范围验证**：`IsValidTypeInfoPointer()` 用固定数组 (64 槽) + release fence 保证无锁安全读

### 写屏障 (Write Barrier)

两层卡表 (CoreCLR 对齐)：

```
地址:     obj_addr
         │
         ├─ (obj_addr - heap_base) >> 9  → card index
         │
         ├─ idx / 128  → L1 segment pointer (64K entries)
         └─ idx % 128  → L2 card byte (128 per segment)
```

- **DirtyCard**：~6 条 native 指令 (sub + shr + div + and + load + store)，无 mutex
- **ClearAllCards**：O(allocated_segments) 而非 O(64K)，通过 tracked segment list
- **ScanDirtyCards**：模板化范围扫描，用于 young GC Phase 1

### 并行标记 (Parallel Mark)

| 参数 | 值 |
|------|-----|
| 触发阈值 | page_count >= 64 (4MB+) |
| Worker 数 | min(pages/32 + 1, hw_concurrency, 8) |
| 数据结构 | Chunked work-stealing deque |
| Chunk 粒度 | (page_idx, bitmap_word_mask) |
| 终止检测 | atomic active_workers 递减 |
| PRNG | XorShift32 (thread-local, 替代 rand()) |

### 压缩 (Compaction)

- **范围**：Page 内 (不跨 page，简化 relocation delta)
- **触发**：任一 size class fragmentation_ratio > 30%
- **三阶段**：Plan → Relocate → Compact
- **Plan**：bitmap 扫描 → 计算每个 marked object 的新地址
- **Relocate**：binary search 更新所有 pointer slot
- **Compact**：memmove 对象到新位置，重建 free list

### 域集成 (Domain Integration)

- 每个热更新模块有独立 MemoryDomain
- 域卸载时：safepoint → 扫描跨域引用 → 释放所有 domain regions
- 跨域引用通过 card table 扫描检测
- 域内存不参与 GC 复制 (无 forwarding pointer)

## 文件清单

| 文件 | 职责 | 关键函数/类型 |
|------|------|-------------|
| `gc_region.h/cpp` | Region 框架 + NurseryAllocate | `RegionManager`, `NurseryAllocate`, `DomainAllocate` |
| `gc_old_gen.h/cpp` | 老年代 (size-class + mark-sweep + compact) | `OldGen`, `Collect`, `PlanPageCompaction` |
| `gc_young_collector.h/cpp` | 年轻代收集 | `GcYoungCollection`, `GcScavengeObject`, Cheney BFS |
| `gc_card_table.h/cpp` | 两层卡表 | `DirtyCard`, `IsDirty`, `ClearAllCards`, `ScanDirtyCards` |
| `gc_loh.h/cpp` | Large Object Heap | `LargeObjectHeap`, `MarkObject`, `Sweep` |
| `gc_parallel_mark.h/cpp` | 并行标记框架 | `ParallelMarkState`, `StealChunk`, `DrainMarkStackParallel` |
| `gc_layout.h/cpp` | GC 布局注册表 | `GcLayoutRegistry`, `ScanObjectPointers` |
| `gc_scheduler.h/cpp` | GC 调度器 (EMA) | `GcScheduler`, `DecideCollection`, `RecommendedNurserySize` |
| `gc_stats.h/cpp` | GC 统计 + 诊断 | `GcStats`, `GcGetSnapshot`, pause histogram, event ring buffer |
| `gc_events.h/cpp` | GC 事件回调 | `GcFireEvent`, `GcRegisterEventCallback` |
| `gc_handle.h/cpp` | GCHandle 管理 | `GcProcessWeakHandlesAfterYoungGC` |

## 性能特征

### 暂停时间 (实测, Debug 构建, 100 线程并发)

| GC 类型 | 平均暂停 | 典型范围 | 说明 |
|---------|---------|---------|------|
| Young GC | ~350µs | 250-600µs | 256KB nursery, 0 promoted |
| Full GC (mark only) | ~20µs | 5-85µs | 小堆 (<100 pages) |
| Full GC (parallel mark) | ~130ms | 50-200ms | 100MB+ 堆, 8 workers |
| Full GC (compact) | ~180ms | 100-300ms | 含 relocate + compact |
| LOH Sweep | ~1ms | 0.5-5ms | 取决于 segment 数量 |

### 内存开销

| 组件 | 开销 | 说明 |
|------|------|------|
| Card table L1 | 512KB | 固定 64K × 8 bytes |
| Card table L2 | 每 64KB heap 128 bytes | 按需分配 |
| Mark bitmap | 每 page 1KB | 64KB page → 1024 bytes bitmap |
| GcLayout table | ~32KB | 4096 槽 × 8 bytes |
| Nursery per thread | 256KB | 可调 (scheduler 自适应) |

## 与 CoreCLR/Mono/Unity IL2CPP 对比

| 能力 | CRAG GC | CoreCLR | Mono GC | Unity IL2CPP (BDWGC) |
|------|---------|---------|---------|---------------------|
| 分代 | 2 代 (young + old) | 3 代 (gen0/1/2) | 2 代 (nursery + old) | 无 (保守式) |
| 并行标记 | 是 (work-stealing) | 是 (BGC 并发) | 是 | 无 |
| 并发标记 | 否 (计划中) | 是 (BGC) | 否 | 无 |
| 压缩 | 是 (page 内) | 是 (gen0/1) | 是 | 无 |
| 写屏障 | 两层卡表 | 卡表 + card word | 写屏障 | 无 |
| 精确扫描 | 是 (GcLayout) | 是 | 是 | 否 (保守) |
| LOH | 是 (85KB 阈值) | 是 (85KB) | 是 | N/A |
| 热更新支持 | 原生 (MemoryDomain) | 无 | 无 | 无 |
| 暂停时间 (100MB) | ~130ms | ~50ms (BGC) | ~200ms | ~500ms+ |
| 内存开销 | 中等 | 高 | 中等 | 低 |

### CRAG GC 优势

1. **热更新深度集成**：MemoryDomain + 域卸载器 + 跨域引用扫描，这是 CoreCLR/Mono/Unity IL2CPP 都不具备的能力
2. **Region 统一框架**：Nursery/Old/Domain/Raw 四种 region 统一管理，简化分配路径
3. **精确扫描**：GcLayout 注册表 + TypeInfo 地址验证，消除 conservative 误报
4. **两层卡表**：CoreCLR 对齐设计，写屏障无 mutex，4GB 堆覆盖

### 已知差距

1. **无并发标记 (BGC)**：Full GC 完全 STW，大堆 (>500MB) 暂停可能超过 500ms
2. **无三代分代**：CoreCLR 的 gen0/gen1 快速回收 + gen2 完整回收模式更高效
3. **Region 框架 O(R) 扫描**：FreeRegion 和 IsInDomain 线性扫描 region 表，大 region 数时性能下降
4. **FindPage O(N)**：OldGen 的 page 查找是线性扫描，大堆时影响性能

## 关键数据流

### 分配路径

```
GcAllocate(size)
  │
  ├─ size > 85KB? → LOH::Allocate(size)
  │
  └─ size ≤ 85KB?
       │
       ├─ size ≤ 32KB? → NurseryAllocate(size)
       │    ├─ bump fit? → return ptr (~10 insn)
       │    └─ bump fail? → NurseryAllocateSlow(size)
       │         ├─ scheduler triggers young GC? → GcYoungCollection
       │         ├─ full GC requested? → OldGen::Collect
       │         └─ get fresh nursery → return ptr
       │
       └─ size > 32KB? → OldGen::Allocate(size, scanning)
            ├─ size class fit? → freelist alloc
            └─ oversized? → dedicated page
```

### Young GC 路径

```
GcYoungCollection(nursery)
  │
  ├─ Phase 1: ScanDirtyCardsInPages (old→nursery refs)
  │    └─ for each dirty card: scan slots → IsInNursery → GcScavengeObject
  │
  ├─ Phase 2: Precise nursery scan (GcLayout)
  │    └─ walk nursery bump range → TypeInfo* → layout → scan pointer offsets
  │
  ├─ Phase 3: Cheney BFS (transitive closure)
  │    └─ drain worklist → ScanObjectPointers → GcScavengeObject
  │
  ├─ Phase 3b: GcProcessWeakHandlesAfterYoungGC
  │
  └─ Phase 4: Sweep nursery + ClearAllCards
```

### Full GC 路径

```
OldGen::Collect(domain_id, reason)
  │
  ├─ Mark phase:
  │    ├─ ScanGCHandleRoots (strong handles)
  │    ├─ ScanPinnedRoots
  │    ├─ ScanThreadRoots (stacks + TLS nurseries)
  │    ├─ DrainMarkStack (parallel if pages ≥ 64)
  │    └─ HandleReMarkPass (finalizer resurrection)
  │
  ├─ Sweep phase:
  │    ├─ SweepPages (free unmarked pages)
  │    ├─ LOH::Sweep (free unmarked segments, trim to 4)
  │    └─ Compact if fragmentation > 30%
  │
  └─ Post-GC:
       ├─ GcProcessWeakHandlesAfterFullGC
       ├─ GcLayoutEpochReclaim (RCU)
       └─ RecordFullCollection
```

## 配置参数

| 参数 | 默认值 | 说明 |
|------|--------|------|
| Nursery 大小 | 256KB | 每线程，scheduler 自适应 |
| Old gen page 大小 | 64KB | 28 size classes |
| LOH 阈值 | 85KB | 匹配 CoreCLR |
| Card size | 512B | 匹配 CoreCLR |
| 并行标记阈值 | 64 pages | 4MB+ 启用并行 |
| 最大并行 workers | 8 | 受 hw_concurrency 限制 |
| 压缩碎片阈值 | 30% | 任一 size class 触发 |
| BFS worklist 初始 | 64K entries | 动态增长 (×2 realloc) |
| LOH 最大 free segments | 4 | 超额释放回 OS |
| GcLayout 表大小 | 4096 槽 | 开放寻址，动态增长 |
| 卡表 L1 大小 | 64K entries | 固定，4GB 覆盖 |
| Nursery range slots | 512 | 锁-free 并发读 |
| Free list trim | 16 | 超过则释放尾部 |
| 事件 ring buffer | 64 条 | 最后 64 次 GC 事件 |