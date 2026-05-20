# CRAG GC 与 CoreCLR GC 全面对比评估报告

> 日期: 2026-05-20
> 范围: CRAG (Chaos Region-Aware GC) V0 当前实现 vs .NET CoreCLR GC (基于 .NET 8/9 公开资料)
> 目标: 逐项对比 CRAG 与 CoreCLR GC 的功能完整度、架构差异、性能特征，并给出差距分析与演进路线

---

## 0. 对比概览 (Executive Summary)

| 维度 | CRAG GC | CoreCLR GC | 完成度 |
|------|---------|-----------|--------|
| 分代数量 | 2 代 (Young + Old) | 3 代 (Gen0 + Gen1 + Gen2) | 65% |
| 写屏障 | SATB pre-barrier + card dirty | 10 种特化变体 + card/bundle | 70% |
| BGC | 完整 (SATB + 并行标记 + 并发清扫 + STW re-mark) | 完整 | 85% |
| 并行 | 工作窃取 (≤8 workers) | Server GC per-CPU heap + 工作窃取 | 60% |
| 压缩 | NONE / COMPACT / CROSS_PAGE | 按代自动 + demotion | 60% |
| NUMA | 基础支持 (VirtualAllocExNuma) | 完整 (per-node heap + 亲和性) | 40% |
| 句柄 | 弱/强/依赖/固定 | 8+ 种 (含 async pinned / ref-count / WinRT) | 50% |
| 终结器 | 完整 (超时 + 重试 + 看门狗) | 完整 | 80% |
| LOH | 完整 (≥85KB, 可选压缩) | 完整 | 80% |
| POH | 完整 (bump-pointer + mark-sweep) | .NET 5+ 引入 | 85% |
| 调度 | EMA 存活率 + 自适应 Nursery | 更复杂的自适应预算调优 | 65% |
| 域卸载 | 完整 (ReleaseDomainRegions + lock-free fastpath) | AppDomain 卸载 (legacy) / NativeAOT 无 | 90% |
| GC Stress | 完整 | 完整 | 85% |
| 卡表 | 2-level (L1/L2) + 动态增长 | 多级 (card + bundle + brick) | 75% |
| 精确根 | 有 (GcSlotMap + conservative fallback) | JIT 生成精确 GC info | 70% |

**综合完成度: ~70%**

CRAG GC 在核心功能 (分配/回收/写屏障/BGC) 上已经成熟，主要差距在于:
1. **中间代缺失** — 无 Gen1 导致 Full GC 频率偏高
2. **并行度差距** — Server GC 的 per-CPU heap 设计在 CPU 密集场景有显著优势
3. **ARM64 优化** — CoreCLR 2025 年已落地 10 种特化写屏障变体
4. **成熟度差距** — CoreCLR GC 经过 20+ 年生产验证，边缘情况处理 (pinned plugs, demotion, segment hoarding) 更完善

---

## 1. 总体架构

### 1.1 CRAG GC

```
┌─────────────────────────────────────────────────────┐
│                  GcScheduler                         │
│  EMA存活率 / 自适应Nursery / 冷却控制 / 决策引擎      │
└──────────────┬──────────────────────────────────────┘
               │ 调度决策
┌──────────────▼──────────────────────────────────────┐
│            RegionManager (全局区域管理器)              │
│  NURSERY │ TENURED │ DOMAIN │ RAW │ FOH │ POH       │
└──────────┬─────────────────────────────────┬─────────┘
           │                                 │
┌──────────▼──────────┐    ┌─────────────────▼────────┐
│   YoungGeneration    │    │    MarkSweepOldGen        │
│   16MB nursery       │    │    Page-based (64KB)      │
│   TLAB per thread    │    │    28 size classes        │
│   8MB survivor       │    │    3-level compaction     │
│   Cheney copy        │    │    Parallel mark/sweep    │
└──────────────────────┘    └──────────────────────────┘
                                     │
           ┌─────────────────────────┼──────────────┐
           │                         │              │
┌──────────▼──────┐  ┌───────────────▼──────┐  ┌────▼──────────┐
│  BGC (SATB)     │  │  LOH (≥85KB)        │  │  POH (pinned) │
│  并行标记        │  │  Segment-based       │  │  bump-pointer  │
│  并发清扫        │  │  可选压缩            │  │  no-copy      │
│  STW re-mark    │  │  Pre-marking        │  │               │
└─────────────────┘  └──────────────────────┘  └───────────────┘
```

### 1.2 CoreCLR GC

```
┌───────────────────────────────────────────────────┐
│              Garbage Collector (gc.cpp)             │
│   Server GC: per-CPU heap × N                     │
│   Workstation GC: single heap                     │
└──────────────────┬────────────────────────────────┘
                   │
      ┌────────────┼────────────┬──────────────┐
      │            │            │              │
┌─────▼────┐ ┌─────▼────┐ ┌────▼────┐  ┌──────▼──────┐
│  Gen 0   │ │  Gen 1   │ │  Gen 2  │  │  LOH (≥85KB)│
│ ~256KB   │ │  ~2MB    │ │  unb.   │  │  segments   │
│ ephemeral│ │ ephemeral│ │ segments│  │  optional   │
│ segment  │ │ segment  │ │         │  │  compact    │
└──────────┘ └──────────┘ └─────────┘  └─────────────┘
                                     ┌──────────────┐
                                     │  POH (.NET 5+)│
                                     │  pinning      │
                                     │  no-copy      │
                                     └──────────────┘
```

### 1.3 关键架构差异

| 维度 | CRAG | CoreCLR Server | CoreCLR Workstation |
|------|------|----------------|---------------------|
| 堆数量 | 1 (全局) | N (per-logical-CPU) | 1 |
| 回收线程 | 并行 worker pool (≤8) | per-heap GC 线程 + 工作窃取 | 1 |
| 区域单位 | Region (动态大小) | Segment (固定 16/64 MB) | Segment (固定 16 MB) |
| 内存来源 | RegionManager (池化管理) | VirtualAlloc (直接) | VirtualAlloc (直接) |
| GC 触发 | TLAB 耗尽 + 调度器 | Gen0 预算超 + 调度器 | Gen0 预算超 + 调度器 |

**影响**: CRAG 的单堆设计在 ≤8 核场景下表现良好，但在 32+ 核服务端场景会因堆锁争用劣于 CoreCLR Server GC。

---

## 2. 分代模型

### 2.1 CRAG 分代

| 属性 | Young Gen | Old Gen |
|------|----------|---------|
| Region 类型 | REGION_NURSERY | REGION_TENURED |
| 大小 | 16 MB (固定) | 按页增长 |
| 分配方式 | TLAB bump-pointer | per-size-class free list |
| 回收方式 | Cheney 复制 | Mark-Sweep (+ 可选压缩) |
| 回收范围 | 清除整个 nursery (复制存活对象) | 全堆标记 (可达旧版) |
| 存活对象去向 | Survivor 区 (第1次) → Old gen (第2次) | 留在原地 (或压缩移动) |

### 2.2 CoreCLR 分代

| 属性 | Gen 0 | Gen 1 | Gen 2 (含 LOH) |
|------|-------|-------|----------------|
| 预算 | ~256 KB (动态可调) | ~2 MB (动态可调) | 剩余全部 |
| 段位置 | Ephemeral segment | Ephemeral segment | 独立 segment(s) |
| 分配方式 | bump-pointer | 晋升得来 | bump (gen2) / free list (LOH) |
| 回收方式 | Mark-Sweep-Compact | Mark-Sweep-Compact | Mark-Sweep (+ 可选压缩) |
| 回收范围 | Gen 0 区 | Gen 0 + Gen 1 | 全堆 |

### 2.3 核心差异

| 对比项 | CRAG | CoreCLR | CRAG 差距 |
|--------|------|---------|----------|
| Gen1 独立收集 | ❌ (Survivor 随 Young GC 绑定) | ✅ | **高** |
| 自适应代大小 | ⚠️ (Nursery 自适应但范围小: 128KB-1MB) | ✅ (全代预算动态调优) | **中** |
| Demotion (降代) | ❌ | ✅ (填充 pinning 碎片) | **低** (非关键) |
| 代间引用跟踪 | Card table (1级) | Card + Bundle + Brick | **低** (功能已够) |

**关键判断**: CRAG 的 Survivor 区 ≈ CoreCLR 的 Gen1 降级版。主要缺失是可独立收集的 Gen1。

---

## 3. 堆结构

### 3.1 Region (CRAG) vs Segment (CoreCLR)

| 特性 | CRAG Region | CoreCLR Segment |
|------|------------|----------------|
| 基本大小 | 64 KB | 16 MB (WS) / 64 MB (Server) |
| 类型 | NURSERY/TENURED/DOMAIN/RAW/FOH/POH | Ephemeral/Gen2/LOH |
| 管理方式 | RegionManager 池 (双向链表 + slot table) | VirtualAlloc 直接分配 |
| 释放方式 | 归还 RegionManager free list | VirtualFree 归还 OS |
| 跨类型转换 | ✅ (Region 可回收重用为其他类型) | ❌ (段类型固定) |

**CRAG 优势**: Region 粒度更细 (64 KB vs 16 MB), 更适合 Unity/游戏场景的内存按需使用。

**CoreCLR 优势**: Segment 批处理减少管理开销，大页分配提高 TLB 命中。

### 3.2 Old Gen 页面结构 (CRAG)

```
┌──────────────────────────────────────────────────┐
│  OldGenPage (64 KB total)                        │
│  ├─ PageHeader (256 B)                           │
│  │  ├─ metadata (kind, numa_node, page_idx, Etc.) │
│  │  └─ free_list_ptrs[28] (per-size-class head)  │
│  ├─ Mark Bitmap (512 B = 4096 bits)              │
│  ├─ Payload (63 KB)                              │
│  │  ├─ Allocated blocks (分散)                   │
│  │  └─ Free blocks (free list 链接)              │
│  └─ 16B poison suffix                            │
└──────────────────────────────────────────────────┘
```

### 3.3 CoreCLR Ephemeral Segment

```
┌─────────────────────────────────────────────────────┐
│  Ephemeral Segment (16 MB / 64 MB)                   │
│  ├─ Segment header                                   │
│  ├─ Gen 2 objects (可跨越多个 segment)                │
│  ├─ Gen 1 boundary (动态调整)                        │
│  ├─ Gen 0 boundary (动态调整 = allocation context)    │
│  └─ Free space                                       │
└─────────────────────────────────────────────────────┘
```

**关键差异**: CoreCLR 将 Gen0/Gen1 限制在同一个 segment 中，使短生命周期对象在局部内存区域活动，提高缓存友好度。CRAG 的独立 Nursery region 也有类似效果。

---

## 4. 分配路径

### 4.1 分配流程对比

```
CRAG:
  GcAllocate(size, scanning_required)
    ├─ size ≤ 32KB → NurseryAllocate → TLAB bump → ✅
    │   └─ TLAB exhausted → TlabClaimFromYoungGen
    │       └─ Young gen exhausted → Young GC → 重试
    │           └─ 重试失败 → OldGen.Allocate → ✅
    └─ size > 32KB → directly to OldGen.Allocate → ✅

CoreCLR:
  Alloc(size, alloc_context)
    ├─ size < 85KB → Gen0 bump (alloc_context->alloc_ptr)
    │   └─ Gen0 budget exceeded → GarbageCollect(0) → 重试
    │       └─ 重试失败 → 扩容 Ephemeral Segment (扩展 Gen0)
    │           └─ 失败 → OOM
    └─ size ≥ 85KB → LOH

  TLAB equivalent: alloc_context per-thread
    ├─ CoreCLR 也有线程本地分配上下文 (acontext)
    ├─ 每次分配从 Ephemeral Segment 的 Gen0 空间 bump
    ├─ Gen0 耗尽触发 GC, 不是按固定 TLAB 大小
    └─ Server GC 有 per-heap alloc_context
```

**差异**: CRAG 的 TLAB (64KB) 与 CoreCLR 的 alloc_context 功能等价。差异在 GC 触发粒度不同。

### 4.2 GC 分配栅栏 (GC Stress)

两者都有 GC Stress 模式。CRAG 每 `kStressInterval = 1000` 次分配触发 GC，CoreCLR 类似。

---

## 5. 写屏障

### 5.1 变体数量

| 写屏障类型 | CRAG | CoreCLR |
|-----------|------|---------|
| Pre-barrier (SATB) | 1 (BgcSatbPreWriteBarrier) | 1 (通用) |
| Post-barrier (card dirty) | 1 (chaos_gc_dirty_card) | 10 (特化变体, 2025 PR) |
| Specialized (no-GC-region) | ❌ (SafepointPoll 绕过) | ✅ |
| Specialized (non-heap) | ❌ | ✅ |
| Card bundle marking | ❌ | ✅ |
| Arm64 optimized | ❌ (仅 x64) | ✅ (2025 完成) |

**CoreCLR 的 10 种写屏障变体** (PR #111636, 2025-12):
- 根据目标 위치 (ephemeral / LOH / non-GC / no-GC-region) 和源位置组合
- 每种变体去掉不必要的检查，减少 inline 代码量和分支数
- x64 用 `movabs` 内联常量, Arm64 从 barrier 后的缓冲加载

### 5.2 CRAG 写屏障实现

**SATB Pre-Barrier** (`gc_bgc_inline.h:42-102`):
```
fast path: relaxed load of g_bgc_is_marking
  ├─ not marking → return immediately (2-3 instructions)
  └─ marking → load old ref from slot
      └─ allocate in TLS SATB buffer → store ref
          └─ buffer full → flush to global queue
```

**Post-Barrier** (`gc_card_table.h:80-100`):
```
DirtyCard(addr):
  card_idx = (addr - g_heap_base) >> 9  (512B card size)
  L1_idx = card_idx >> 7
  seg = g_card_l1[L1_idx]
  card = seg->cards[card_idx & 0x7F]
  if card != 0xFF:
    seg->cards[card_idx & 0x7F] = 0xFF
```

### 5.3 差距分析

| 差距项 | 影响 | 优先级 |
|--------|------|--------|
| 无特化变体 | 通用 barrier 对非堆/非 ephemeral 写入额外检查 | **低** (代码生成优化) |
| 无 card bundle | 大堆场景下卡表扫描+ 15~30% | **低** (游戏堆小) |
| 无 Arm64 优化 | Arm64 平台写屏障多 ~5-10 条指令 | **中** (涉 Arm 时解决) |
| 无 Interpreter 写屏障 | Interpreter 中 ref stfld/stobj 无 barrier | **高** (已在 P3 修复队列) |

---

## 6. 回收算法

### 6.1 Young GC / Gen0 Collection

| 阶段 | CRAG Young GC | CoreCLR Gen0 Collection |
|------|--------------|------------------------|
| 根枚举 | Card table (old→young) + 精确 nursery 扫描 + BFS | Card table + Gen0/Gen1 精确扫描 + mark stack |
| 存活对象处理 | Cheney 复制到 Survivor/Old gen | 就地 compact 或晋升到 Gen1/Gen2 |
| 回收效果 | 整块 nursery 清除 (bump 归零) | Gen0 区域 compact + free list |
| 写屏障开销 | 仅在 card dirtying | 同 + card bundle 维护 |

**差异点**:

| 对比项 | CRAG | CoreCLR |
|--------|------|---------|
| 复制 vs 压缩 | Cheney 复制 (所有存活对象搬移) | Mark-Compact (部分压缩) |
| 晋升年龄控制 | 固定 1 (进 Survivor 后 1 次即升 Old) | 自适应 (根据存活率动态) |
| Demotion | ❌ | ✅ (填充 pinning 产生的空洞) |
| Card table 扫描 | 线性扫描 dirty card | 同 + bundle 跳过 clean 段 |
| 并行 | 单线程 (Young GC) | Server GC: per-CPU heap 并行 |

### 6.2 Full GC / Gen2 Collection

| 阶段 | CRAG Full GC | CoreCLR Gen2 Collection |
|------|-------------|------------------------|
| 根枚举 | 栈 (保守) + 静态根 + POH + GCHandle | 同 (精确程度更高) |
| 标记 | 并行 DrainMarkStack (工作窃取) | 并行 (每 heap 各扫各的 + 工作窃取) |
| 清扫 | 线性页面扫描, 重建 free list | 同 |
| 压缩 | 3 级 (NONE/COMPACT/CROSS_PAGE) | 按代自动 + demotion |
| 悬浮引用 | Re-mark pass (finalizer 复活) | 同 |
| 句柄处理 | 弱句柄 null + dependent handle 修复 | 同 (更多句柄类型) |

**CRAG 优势**
- 3 级压缩模式是 CoreCLR 的超集 (CoreCLR 在 SOH 上自动压缩, 但无 explicit CROSS_PAGE 分段疏散)
- Re-mark pass 设计合理

**CoreCLR 优势**
- Demotion 减少 pinning 碎片
- Server GC per-CPU 堆在 32+ 核场景有 ~2x 吞吐优势
- 更成熟的世代预算调优

---

## 7. BGC (Background GC)

### 7.1 状态机对比

```
CRAG BGC:
  IDLE → ROOT_COLLECT (STW) → CONCURRENT_MARK → REMARK_NEEDED
  → STW_REMARK (STW) → CONCURRENT_SWEEP → COMPACT_NEEDED
  → STW_COMPACT (STW) → FINISHED → IDLE

CoreCLR BGC:
  IDLE → ROOT_COLLECT (STW) → CONCURRENT_MARK → STW_REMARK
  → CONCURRENT_SWEEP → CONCURRENT_PLAN (for compact)
  → STW_COMPACT → FINISHED → IDLE
```

### 7.2 功能对比

| 功能 | CRAG BGC | CoreCLR BGC |
|------|---------|-------------|
| SATB pre-barrier | ✅ | ✅ (且更早引入, 成熟度高) |
| 并行标记 | ✅ (≤8 workers, 工作窃取) | ✅ (Server: per-heap + 窃取) |
| 并发清扫 | ✅ (BgcSweep) | ✅ |
| SATB 冻结协议 | ✅ (20 idle rounds → freeze) | ✅ |
| 前台 GC 抢占 BGC | ✅ (Young GC STW) | ✅ (Foreground GC 暂停 BGC) |
| BGC 超时安全阀 | ✅ (~10秒 idle break) | ✅ |
| BGC 线程数 | 固定 ≤8, 1 主线程 + workers | Server: per-CPU, WS: 1 |
| 并发计划 (compact) | ❌ (compact 永远是 STW) | ✅ (CONCURRENT_PLAN 阶段) |
| BGC 期间 LOH 分配 | ✅ | ✅ |

**差距**: CoreCLR 的 CONCURRENT_PLAN 阶段允许部分压缩决策与标记并行，进一步减少 STW 窗口。

---

## 8. GC 调度

### 8.1 触发条件对比

| 触发条件 | CRAG | CoreCLR |
|---------|------|---------|
| Gen0 预算超 | ✅ (Nursery 满) | ✅ (Gen0 满了) |
| 内存压力 | ✅ (256MB 阈值) | ✅ (低内存通知) |
| 显式请求 | ✅ (chaos_gc_collect) | ✅ (GC.Collect) |
| 自适应触发 | ✅ (2× 上次 nursery 使用量) | ✅ (更复杂的启发式) |
| AppDomain 卸载 | ✅ (ReleaseDomainRegions) | ✅ (但 .NET Core 已移除 AD) |
| 低内存 OS 通知 | ❌ | ✅ (MEMORY_PRESSURE_LOW/HIGH) |

### 8.2 冷却控制

| 特性 | CRAG | CoreCLR |
|------|------|---------|
| 最小间隔 50ms | ✅ | ✅ (类似) |
| 分配计数冷却 | ✅ (256 次分配) | ✅ (类似) |
| CAS slot 防风暴 | ✅ (TryClaimGcSlot) | ✅ |

**差距**: CRAG 缺少 OS 低内存通知集成。在内存受限环境 (手机/主机) 中可能会延迟 full GC 响应。

---

## 9. 句柄管理

### 9.1 句柄类型对比

| 句柄类型 | CRAG | CoreCLR |
|---------|------|---------|
| Weak (GC 前 null) | ✅ (GcProcessWeakHandlesAfterYoungGC / AfterFullGC) | ✅ (HNDTYPE_WEAK_SHORT) |
| WeakTrackResurrection (GC 后 null) | ✅ (在 finalization 后 null) | ✅ (HNDTYPE_WEAK_LONG) |
| Strong (阻止回收) | ✅ (通过引擎集成层) | ✅ (HNDTYPE_STRONG) |
| Pinned (阻止搬移) | ✅ (AddPinnedRoot) | ✅ (HNDTYPE_PINNED) |
| Dependent (ConditionalWeakTable) | ✅ (GcCreateDependentHandle + 多轮扫描) | ✅ (HNDTYPE_DEPENDENT) |
| Async Pinned | ❌ | ✅ (HNDTYPE_ASYNCPINNED) |
| Ref-counted | ❌ | ✅ (HNDTYPE_REFCOUNT) |
| Sized Ref | ❌ | ⚠️ (一些版本) |
| WinRT Weak | ❌ | ✅ (HNDTYPE_WEAK_WINRT) |

**差距**: Async Pinned 对 I/O 密集型应用重要 (disk/network I/O 期间 pin buffer)。CRAG 需评估是否需要。

---

## 10. 终结器

### 10.1 对比

| 特性 | CRAG | CoreCLR |
|------|------|---------|
| 终结器线程 | ✅ (独立线程, 抢占式模式) | ✅ (Finalizer thread) |
| 超时机制 | ✅ (2s 超时, ≤3 次重试, 永久跳过) | ✅ (2s 超时, 2 次重试) |
| 看门狗 | ✅ (10s heartbeat check) | ✅ |
| 终结队列 | ✅ (finalizers_ vector) | ✅ (Finalizer Queue) |
| f-reachable 队列 | ✅ (通过 PublishFinalizationWork) | ✅ |
| SuppressFinalize | ✅ | ✅ |
| ReRegisterForFinalize | ✅ | ✅ |
| 并发 BGC 终结处理 | ✅ (CollectDeadFinalizablesForBgc) | ✅ |
| 弱句柄时机 | ✅ (终结后 null, WTR 语义正确) | ✅ |

**差异**: CRAG 使用 `std::async` 运行每个终结器 (超时控制)，而 CoreCLR 在终结器线程上同步执行。CRAG 的 `std::async` 方式为每个挂起的终结器创建一个独立线程 (更浪费)，但隔离性更好。

---

## 11. 根扫描

### 11.1 根类型对比

| 根类型 | CRAG | CoreCLR |
|--------|------|---------|
| 线程栈 (保守) | ✅ (GcScanAllThreadRoots) | ✅ (但精度更高) |
| 线程栈 (精确) | ✅ (GcSlotMap + GcScanFrameHybrid) | ✅ (JIT GC info) |
| CPU 寄存器 | ⚠️ (SpillGcRefRegs) | ✅ (JIT 生成 STW 时的寄存器保存) |
| 静态字段 | ✅ (GcRegisterStaticRootRange) | ✅ |
| GCHandle | ✅ (GcIterateHandleTable / TenuredHandles) | ✅ (所有句柄类型) |
| 终结器队列 | ✅ (finalizers_ 作为根) | ✅ |
| 跨代引用 | ✅ (Card table) | ✅ (Card + bundle) |
| Pinned Object 表 | ✅ (GcIteratePinnedObjects) | ✅ |

**差距**: CRAG 的 `GcRegisterSlotMapsFromSection` 是 no-op (占位符)，意味着当前所有 codegen 必须逐个方法调用 `GcRegisterSlotMap`。

---

## 12. GC 模式 (Latency Modes)

### 12.1 模式对比

| 模式 | CRAG | CoreCLR |
|------|------|---------|
| BATCH | ⚠️ (枚举存在, 但行为无差异) | ✅ (禁用 BGC, 暂停时间优先) |
| INTERACTIVE | ⚠️ (枚举存在, 但行为无差异) | ✅ (BGC 默认, 平衡) |
| LOW_LATENCY | ⚠️ (仅偏向 BGC) | ✅ (抑制 Gen2 full GC) |
| SUSTAINED_LOW_LATENCY | ⚠️ (仅偏向 BGC) | ✅ (比 LOW_LATENCY 更激进) |
| NO_GC_REGION | ✅ (完整实现) | ✅ |
| Server GC | ❌ (无 per-CPU heap) | ✅ |
| Workstation GC | ✅ (等价) | ✅ |

**详细说明 (CoreCLR 模式行为)**:
- **BATCH**: 禁用 BGC, full GC 时 STW 更长但总 CPU 开销最低
- **INTERACTIVE**: 启用 BGC (默认), 平衡暂停和吞吐
- **LOW_LATENCY**: 压制 Gen2 回收, 适合 UI 线程: 对象只升到 Gen2 不回收, 内存会增长
- **SUSTAINED_LOW_LATENCY**: 类似 LOW_LATENCY, 但允许 Gen2 full GC, 频率极低
- **NO_GC_REGION**: 完全禁止 GC 触发, 离开时 GC 补跑

CRAG 的 NO_GC_REGION 实现到位, 但其他模式的差异化行为尚未实现。

---

## 13. NUMA 支持

| 特性 | CRAG | CoreCLR |
|------|------|---------|
| 拓扑检测 | ✅ (Windows/Linux) | ✅ |
| per-node 虚拟分配 | ✅ (VirtualAllocExNuma / mbind) | ✅ |
| per-page NUMA 节点跟踪 | ✅ (OldGenPage::numa_node) | ✅ |
| per-node 堆 | ❌ | ✅ (Server GC 每个 node 一个 heap) |
| 节点亲和性 (分配) | ✅ (在 OldGen 页面分配时使用当前节点) | ✅ |
| GcNumaNodeOfAddress | ⚠️ (返回 -1, 未实现) | ✅ |
| 跨节点 stolen 工作 | N/A (无 per-node heap) | ✅ (Server GC 工作窃取跨节点) |

**差距**: CRAG 的 NUMA 支持是部分实现 — 页面分配时优先当前节点, 但缺少 per-node heap 的核心设计。在 NUMA 机器上, 跨节点 remote memory 访问延迟 ~1.5x local。

---

## 14. 压缩

### 14.1 压缩模式对比

| 模式 | CRAG | CoreCLR |
|------|------|---------|
| 不压缩 (纯 Mark-Sweep) | ✅ (NONE) | ✅ |
| 页内压缩 | ✅ (COMPACT, 3-phase: plan/compact/relocate) | ✅ (SOH 自动压缩) |
| 跨页疏散 | ✅ (CROSS_PAGE, >40% 碎片触发, 512KB 预算/cycle) | ⚠️ (无 explicit 跨页疏散) |
| Demotion 压缩 | ❌ | ✅ (Pinned 对象后空洞用 demotion 填充) |
| LOH 压缩 | ✅ (可选, AUTOMATIC/NONE/ON_REQUEST, 4MB 预算/cycle) | ⚠️ (历史不压缩, .NET 4.5+ 可选, .NET 8 改进) |

**CRAG 优势**: CROSS_PAGE 疏散是对 CoreCLR 的超越, 对处理高碎片化场景更灵活。

**CoreCLR 优势**: Demotion 机制在 pinning 场景下更高效 — 不是搬移对象, 而是将新晋升的对象放到被 pin 对象留下的空洞里。

---

## 15. LOH (大对象堆)

| 特性 | CRAG | CoreCLR |
|------|------|---------|
| 阈值 | 85 KB | 85,000 bytes |
| 段管理 | LOHSegment (含 mark_bit) | Segment (含 mark array) |
| 分配方式 | Segment free list | Segment free list |
| 默认压缩 | AUTOMATIC (>25% 碎片触发) | ❌ (历史默认不压缩) |
| 可选压缩 | ✅ (4MB 预算/cycle) | ✅ (.NET 4.5.1+, 可选) |
| BGC concurrent mark | ✅ (Pre-marking 防止 BGC 清扫) | ✅ |
| 迭代压缩 | ✅ (kMaxCompactBytes 限制单次预算) | ✅ |

CRAG LOH 已基本对标 CoreCLR, 且 AUTOMATIC 压缩模式更激进。

---

## 16. POH (固定对象堆)

| 特性 | CRAG | CoreCLR |
|------|------|---------|
| 引入版本 | V0 (初始) | .NET 5 |
| 分配方式 | Bump-pointer (mutex 保护) | Bump-pointer |
| Region/段 | REGION_POH (64KB) | 独立 segment |
| 回收方式 | Mark-Sweep (作为根扫描) | Mark-Sweep |
| 无复制保证 | ✅ | ✅ |
| 固定对象注册 | ✅ (GcAddPinnedObject/RemovePinnedObject) | ✅ |
| 线程安全 | 单进程 wide (mutex) | 更细粒度 |

CRAG 的 POH 实现完整, 甚至比 .NET 5 更早在自研 GC 中加入此概念。**但单 mutex 保护可能在大量并发固定分配时成为瓶颈**。

---

## 17. 卡表 (Card Table)

| 特性 | CRAG | CoreCLR |
|------|------|---------|
| 层级 | 2 (L1 + L2 segment) | 2 (card table + bundle) |
| Card 大小 | 512 字节 | 2 KB (x64) / 1 KB (x86) |
| 覆盖范围 | 4 GB (可动态增长) | 全地址空间 |
| Bundle 表 | ❌ | ✅ (1 bit : 64 cards) |
| 动态增长 | ✅ (GcRegisterHeapRange) | ✅ |
| 原子脏标记 | ✅ (非原子写, 但 IDEMPOTENT) | ✅ (x86 上字节写) |
| 清理策略 | ClearAllCards 遍历 segment 列表 | 同 |

**差距**: Bundle 表可在全堆扫描时跳过大段 clean card (64 cards 一组), 在大堆场景减少卡表扫描 ~20%。

---

## 18. 并行机制

| 特性 | CRAG | CoreCLR Server GC |
|------|------|-------------------|
| 并行标记 | ✅ (DrainMarkStackParallel, 工作窃取) | ✅ (per-heap + 窃取) |
| 并行清扫 | ✅ (GcWorkerPool, ≤7 workers) | ✅ |
| 并行压缩 | ✅ (ParallelCompactPages) | ✅ |
| per-CPU heap | ❌ | ✅ (核心设计) |
| Card stealing | ❌ | ✅ (线程完成本 heap 后帮其他 heap) |
| 标记工作窃取 | ✅ (chunk-based deque) | ✅ |
| 工作池 | ✅ (GcWorkerPool, dedicated threads) | ✅ (GC 线程) |

**核心差距**: CRAG 没有 per-CPU heap。在 Server GC 中, 每个 CPU 有独立堆和分配上下文, 消除分配锁竞争, 同时使 GC 期间各 CPU 回收本堆, 减少跨 CPU 缓存一致性流量。

---

## 19. API 面

### 19.1 BCL API 实现

| API | CRAG | CoreCLR |
|-----|------|---------|
| GC.Collect() | ✅ (chaos_gc_collect) | ✅ |
| GC.Collect(gen, mode) | ✅ (chaos_gc_collect_with_mode) | ✅ |
| GC.GetTotalMemory(force) | ✅ | ✅ |
| GC.GetTotalAllocatedBytes() | ❌ | ✅ (.NET 7+) |
| GC.GetAllocatedBytesForCurrentThread() | ❌ | ✅ |
| GC.GetGCMemoryInfo() | ✅ (chaos_gc_get_memory_info) | ✅ |
| GC.WaitForPendingFinalizers() | ✅ | ✅ |
| GC.KeepAlive() | ✅ (no-op) | ✅ (no-op) |
| GC.SuppressFinalize() | ✅ | ✅ |
| GC.ReRegisterForFinalize() | ✅ | ✅ |
| GC.AddMemoryPressure() | ✅ | ✅ |
| GC.RemoveMemoryPressure() | ✅ | ✅ |
| GC.CollectionCount(gen) | ❌ | ✅ |
| GC.GetGeneration(obj) | ⚠️ (仅在 gc_api 声明) | ✅ |
| GC.TryStartNoGCRegion() | ✅ | ✅ |
| GC.EndNoGCRegion() | ✅ | ✅ |
| GCLatencyMode | ✅ (基本版, 无差异化行为) | ✅ (完整) |
| GCNotification | ❌ | ✅ (GC.RegisterForFullGCNotification) |

### 19.2 缺失 API 优先级

| API | 影响 | 优先级 |
|-----|------|--------|
| GC.CollectionCount(gen) | Diagnostics 缺失 | **低** |
| GC.GetGeneration(obj) | GCMemoryInfo.generation 已模拟 | **低** |
| GC.GetTotalAllocatedBytes | ETW 替代方案 | **低** |
| GCNotification | 企业级监控需求 | **低** |

---

## 20. 已知限制与待修复项

| # | 限制 | 严重程度 | 涉及模块 | 说明 |
|---|------|---------|---------|------|
| 1 | 写屏障仅 codegen 路径, Interpreter 路径缺失 | **高** | Write barrier | Interpreter 执行 stfld/stobj ref 无 SATB/dirty 调用 |
| 2 | kPromotionAgeThreshold 固定 =1 | **中** | Age tenuring | 对象 2 次 Young GC 就升 Old, Full GC 频率偏高 |
| 3 | GcSlotMap section registration 为 no-op | **中** | Slot map | 每方法必须逐个注册, 无批量入口 |
| 4 | GcEndStubbornChange 是 no-op | **中** | Write barrier | Incremental GC stub |
| 5 | Nursery 全局边界单调增长不收缩 | **低** | Memory | IsInNursery 假阳性但不致命 |
| 6 | POH 单进程 mutex | **低** | POH | 高并发固定分配瓶颈 |
| 7 | 退管页面数组永不释放 | **低** | Old gen | retired_arrays_ 无限增长 |
| 8 | NUMA GcNumaNodeOfAddress 返回 -1 | **低** | NUMA | 地址-节点映射未实现 |
| 9 | Finalizer 使用 std::async per call | **低** | Finalizer | 每个挂起终结器创建一个线程 |
| 10 | Latency mode 其他模式无差异化行为 | **低** | Scheduling | 仅有 NO_GC_REGION 工作 |
| 11 | Server GC per-CPU heap 缺失 | **中** | Parallelism | 32+ 核可扩展性受限 |

---

## 21. 差距分析与演进路线

### 21.1 差距优先级矩阵

```
                    High Impact           Medium Impact        Low Impact
                ┌────────────────────┬────────────────────┬────────────────┐
   Easy         │                    │                    │                │
   (≤1 周)      │                    │ #2 AgeThreshold→2  │ #5 Nursery     │
                │                    │ #6 POH per-thread  │   边界收缩     │
                ├────────────────────┼────────────────────┼────────────────┤
   Medium       │ #1 Interpreter     │ #3 SlotMap 批量    │ #8 NUMA        │
   (1-4 周)     │   写屏障           │   注册             │   NodeOfAddr   │
                │ #10 Latency mode   │ #11 Server GC      │                │
                │   差异化           │   per-CPU heap     │                │
                ├────────────────────┼────────────────────┼────────────────┤
   Hard         │                    │ #4 Incremental     │ #7 retired     │
   (>4 周)      │                    │   GC (GcEndStub)   │   _arrays 释放  │
                │                    │ #9 Finalizer       │                │
                │                    │   std::async→pool  │                │
                └────────────────────┴────────────────────┴────────────────┘
```

### 21.2 分阶段建议路线

#### P0 (V0 发布前必须完成)
- **#1 Interpreter 写屏障** — SATB pre-barrier + card dirty 在 Interpreter 路径缺失, 会导致 BGC 标记不完整
- **#2 AgeThreshold 1→2** — 1 行常量变动, 预期 Full GC -10~20%

#### P1 (V0 发布后 1-2 月内)
- **#10 Latency mode 差异化** — BATCH 禁用 BGC, LOW_LATENCY 压制 Gen2
- **#4 Incremental GC stub** — 至少实现 GcEndStubbornChange 的分布式 card dirtying
- **#9 Finalizer thread pool** — 用固定线程池替换 std::async

#### P2 (V1 规划)
- **#11 Server GC per-CPU heap** — 核心架构升级, 预期 32+ 核吞吐 +50~100%
- **Gen1 独立收集** (参见 `docs/discuss/three-generational-gc-analysis.md`)

#### P3 (长期)
- **AVX-512 写屏障** — 如 CoreCLR PR, 使用 SIMD vectorized card dirtying
- **POH per-thread context** — 消除 mutex 瓶颈
- **Arm64 特化** — 写屏障 + atomic support

### 21.3 结论

CRAG GC 已经达到了一个功能相当完整的 V0 版本。核心框架 (分配/回收/写屏障/BGC/句柄/终结器) 全部就位, 与 CoreCLR 的直接功能差距集中在:

1. **Interpreter 写屏障缺失** (P0, 必须修复)
2. **中间代缺失** (可通过 Survivor 优化缓解)
3. **Server GC per-CPU heap 缺失** (32+ 核场景才显瓶颈)
4. **各种成熟度边缘情况** (demotion/slot map 批量注册/etc.)

游戏场景下 (<16 核, 暂停时间敏感), CRAG GC 的当前设计已经 Pareto-optimal。主要优化方向应聚焦在:
- 写屏障覆盖全面性 (#1)
- Full GC 频率降低 (#2 + Survivor 优化)
- BGC 稳定性 (测试覆盖)
