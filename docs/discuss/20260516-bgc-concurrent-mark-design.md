# BGC: 并发标记设计 (Background GC / Concurrent Mark)

> 2026-05-16
> CRAG GC Phase C — Concurrent Mark for old-generation collection

---

## 1. 问题陈述

当前 MarkSweepOldGen::CollectFull() 是 STW (Stop-The-World) 完整暂停：
- 请求全局 safepoint → 所有线程在安全点阻塞
- 单线程/并行 mark + sweep（仍持有 safepoint）
- 释放 safepoint → 所有线程恢复

对延迟敏感的 workload（游戏、UI 60fps 渲染），STW 暂停应低于 ~8ms。大堆（数百 MB）的 mark 耗时可能达到 50-200ms，必须并发化。

**目标**：将 mark 阶段从 STW 移至后台线程并发执行，STW 仅保留短暂的 re-mark 阶段（~1-5ms）。

---

## 2. 总体架构

```
┌──────────────────────────────────────────────────────────────┐
│  BGC 三阶段流程                                               │
│                                                              │
│  ┌──────────┐    ┌──────────────┐    ┌──────────┐           │
│  │ Concurrent │    │  STW         │    │ Concurrent│          │
│  │ Mark       │───→│  Re-Mark     │───→│ Sweep     │          │
│  │ (BGC 线程)  │    │ (所有线程)    │    │ (BGC 线程) │          │
│  └──────────┘    └──────────────┘    └──────────┘           │
│       │                 │                    │               │
│       │ tri-color       │ drain              │ sweep pages   │
│       │ mark stack      │ remaining          │ → free lists  │
│       │ + write barrier │ marked objects     │               │
│  ┌────┴────┐      ┌────┴────┐        ┌────┴────┐          │
│  │ Grey    │      │ Drain   │        │ Sweep   │          │
│  │ Mark    │      │ Final   │        │ Pages   │          │
│  │ Stack   │      │ Stack   │        │         │          │
│  └─────────┘      └─────────┘        └─────────┘          │
└──────────────────────────────────────────────────────────────┘
```

### 关键设计决策

1. **Incremental update (SATB) vs. Snapshot-at-the-beginning**：选择 SATB (Snapshot-AT-the-Beginning) 写屏障。理由：SATB 在并发标记期间**不需要**标记已标记对象的变更字段（增量更新需要），并发标记开始时的快照确保所有存活对象都被标记。CMS 风格的增量更新需要 pre-write barrier——SATB 也是 pre-write barrier，但语义更简单（记录快照中的引用）。

2. **Pre-write barrier (SATB)**：在引用被覆盖前，记录旧的引用值。BGC 标记开始后，任何对象引用的修改都要将旧值记录到 SATB 队列中，确保快照完整性。

3. **Post-write barrier (card marking)**：与现有 card table 复用。并发标记期间，mutator 对新分配的跨代引用标记 card。Re-mark 阶段扫描所有被标记的 card。

---

## 3. 数据结构

### 3.1 SATB 队列

每个线程（或每个 mutator）一个 SATB 队列，存储并发标记开始后**被覆盖的旧引用**。

```cpp
struct SatbEntry {
    void* obj;   // 被修改的对象
    void* slot;  // 具体字段地址
};

struct ThreadSatbQueue {
    SatbEntry* buffer;
    CHAOS_IL2CPP_SIZE index;
    CHAOS_IL2CPP_SIZE capacity;
    ThreadSatbQueue* next;  // global list link
};
```

BGC 线程定期遍历所有线程的 SATB 队列，处理每个条目：
- 读取 `slot` 中的当前值（已经是新值，但旧值已被覆盖——实际存储旧值）
- 等等，SATB 应该是**在覆盖前记录旧值**，所以在写入屏障中，我们记录的是即将被覆盖的旧值

重新设计：

```cpp
// Pre-write barrier (在 mutator 覆盖 slot 前调用)
// 记录 `slot` 中当前存储的旧引用
void SatbPreWriteBarrier(void** slot, void* new_value) {
    if (!g_bgc_is_marking) return;  // 不在并发标记期，跳过
    void* old_value = *slot;
    if (old_value == nullptr) return;
    if (!g_old_gen.IsInOldGen(old_value)) return;  // 只关心 old-gen 引用
    
    ThreadSatbQueue* q = GetCurrentThreadSatbQueue();
    if (q->index >= q->capacity) {
        SatbQueueFlush(q);  // 溢出时刷新到全局队列
    }
    q->buffer[q->index++] = {old_value, slot};
}
```

### 3.2 BGC 线程状态

```cpp
enum class BgcPhase : uint8_t {
    IDLE,           // 无并发标记
    CONCURRENT_MARK,  // 正在并发标记
    STW_REMARK,     // 暂停 re-mark
    CONCURRENT_SWEEP, // 并发 sweep
    FINISHED        // 完成
};

class BgcController {
public:
    std::atomic<BgcPhase> phase_{BgcPhase::IDLE};
    
    // BGC 线程句柄
    std::thread bgc_thread_;
    std::atomic<bool> bgc_running_{false};
    
    // 全局 SATB 队列（BGC 线程从各 mutator 队列收集）
    std::vector<SatbEntry> global_satb_queue_;
    std::mutex satb_mutex_;
    
    // 标记栈（复用 MarkSweepOldGen::mark_stack_ 中的 tri-color 标记）
    // 通过 g_old_gen 访问
    
    // 统计
    std::atomic<CHAOS_IL2CPP_SIZE> marked_count_{0};
    std::atomic<CHAOS_IL2CPP_SIZE> satb_drained_{0};
};
```

### 3.3 并发标记标志

全局原子标志，控制写屏障的开关：

```cpp
extern std::atomic<bool> g_bgc_is_marking;  // true = 写屏障生效
```

在 CONCURRENT_MARK 阶段开始时设为 true，re-mark 完成后设为 false。

---

## 4. 写屏障实现

### 4.1 Pre-write barrier (SATB)

```cpp
// 内联于 header 中，作为所有 managed 引用的写入前调用
// codegen 在每次 stfld/stobj 前 EmitSatbPreWriteBarrier
inline void SatbPreWriteBarrier(void** slot) {
    if (!g_bgc_is_marking.load(std::memory_order_relaxed)) [[likely]]
        return;
    
    void* old_val = *slot;
    if (old_val == nullptr) [[likely]]
        return;
    
    // 只记录 old-gen 中的引用
    if (!RegionManager::Instance().IsInOldGen(old_val) && 
        !g_old_gen.IsInOldGen(old_val)) [[likely]]
        return;
    
    // 记录到当前线程的 SATB 队列
    auto* q = GetCurrentThreadSatbQueue();
    auto idx = q->index.fetch_add(1, std::memory_order_relaxed);
    if (idx < q->capacity) {
        q->buffer[idx] = old_val;
    } else {
        // 队列满，刷新到全局队列
        SatbQueueFlush(q);
    }
}
```

### 4.2 Post-write barrier (Card marking)

复用现有的 card table。在标记阶段，mutator 的新写入会标记对应的 card：

```cpp
// 现有的 card_dirty 逻辑保持不动：
// GcCardMarkDirty(slot_addr) 在 stfld 后调用
```

但在 BGC 的 concurrent mark 阶段，re-mark 阶段需要扫描所有 dirty cards 来发现并发期间的新分配引用。

---

## 5. 三阶段流程

### 5.1 Phase 1: Concurrent Mark

**触发条件**：GC 调度器决定需要 full GC，且适合并发执行（非紧急内存压力）。

```
1. 请求 STW safepoint（非常短暂）
2. 设置 root set：
   - GC roots（静态变量、GCHandle、线程栈）→ 标记为 grey
   - 将 grey 对象推入 mark_stack_
3. 设置 g_bgc_is_marking = true
4. 释放 safepoint
5. BGC 线程启动：
   while (mark_stack_ 非空 || satb 队列有未处理条目) {
     a. 从 mark_stack_ pop 一个 grey 对象
     b. GcLayout 扫描其所有引用字段
     c. 对每个引用：
        - 若未标记：标记 → 推入 mark_stack_ 作为新 grey
     d. 当前对象变为 black
     e. 定期检查（每 64 个对象）：
        - 遍历所有线程 SATB 队列，将条目推入 mark_stack_
        - 统计 marked_count_
   }
6. mark_stack_ 为空且 SATB 队列全部排空 → 并发标记完成
```

**BGC 线程调度**：以低优先级运行，每次处理 1024 个对象后 yield，避免 CPU 争抢。

### 5.2 Phase 2: STW Re-Mark

**目的**：处理并发标记期间 mutator 修改导致的遗漏对象。

```
1. 请求 STW safepoint
2. 处理所有 SATB 队列的剩余条目
3. 扫描 dirty cards → 发现跨代新引用
4. Drain mark_stack_ 直到空
5. 设置 g_bgc_is_marking = false
6. 释放 safepoint
```

Re-mark 的暂停时间 = O(SATB 条目数 + dirty card 数)，通常 < 5ms。

### 5.3 Phase 3: Concurrent Sweep

```
1. BGC 线程遍历所有 old-gen 页面
2. 对每个页面：
   a. 扫描 bitmap，将未标记块合并到 free list
   b. 跳过 small page（空闲率高 → 留给下一次 full GC 做 compact）
3. 完成后设置 phase = FINISHED
4. 调度器通知等待 GC 的线程
```

并发 sweep 可以与 mutator 并发运行。Sweep 期间分配可能使用空闲的已回收块。

---

## 6. GC 布局扫描集成

现有 `GcLayout` 类型描述（gc_layout.h）已支持精确扫描。并发标记直接复用：

```cpp
// BGC 线程中调用
void BgcMarkObject(void* obj) {
    if (MarkObject(obj)) {  // MarkObject 返回 false 表示已标记
        mark_stack_.push_back(obj);
    }
}

void BgcProcessGreyObject(void* obj) {
    GcLayout* layout = GetLayout(obj);
    if (!layout || layout->IsPointerFree()) return;
    
    void* ptr = obj + sizeof(ObjectHeader); // 第一个字段
    for (int i = 0; i < layout->FieldCount(); i++) {
        if (layout->IsGcRef(i)) {
            void* ref = *static_cast<void**>(ptr);
            if (ref && g_old_gen.IsInOldGen(ref)) {
                BgcMarkObject(ref);
            }
        }
        ptr += layout->FieldSize(i);
    }
}
```

---

## 7. 与现有 Full GC 的集成

### 7.1 调度决策

在 `gc_scheduler.h` 中新增：

```cpp
enum class GcCollectionKind : uint8_t {
    NONE,
    YOUNG,
    FULL,       // STW full GC（紧急内存压力）
    FULL_BGC    // Background concurrent GC（低延迟模式）
};
```

- **紧急内存压力**（Allocation failure、OOM）：走 FULL_STW
- **正常 full GC 触发**（碎片率 30%+、分配速率 EMA 提示）：走 FULL_BGC

### 7.2 并发标记期间的分配

Mutator 在 BGC 并发标记期间继续分配：
- Young 分配继续使用 nursery bump（不变）
- Old-gen 分配需要：
  - 如果分配触发新的 full GC，等待 BGC 完成
  - 分配过程中标记 card（已通过 post-write barrier 完成）
  - 大对象直接分配 oversized page（mark bitmap 初始化为全 marked）

### 7.3 并发安全标记堆

`MarkObject` 必须是并发安全的。对 mark bitmap 的测试与设置需使用原子操作：

```cpp
// 原子版本（BGC 并发使用）
bool MarkObjectConcurrent(void* obj) {
    Page* page = FindPage(obj);
    CHAOS_IL2CPP_SIZE offset = static_cast<char*>(obj) - page->Payload();
    CHAOS_IL2CPP_SIZE bit_idx = offset / kMinObjectSize;
    CHAOS_IL2CPP_SIZE byte_idx = bit_idx / 8;
    uint8_t mask = static_cast<uint8_t>(1 << (bit_idx % 8));
    
    uint8_t old = __atomic_fetch_or(page->MarkBitmap() + byte_idx, mask, __ATOMIC_RELEASE);
    return (old & mask) == 0; // true = 首次标记
}
```

---

## 8. 实现路线

### P1: 基础设施（~200 行）

| 任务 | 文件 | 描述 |
|------|------|------|
| BgcController 类框架 | `gc_bgc.h/.cpp` | 阶段枚举、线程管理 |
| SATB 队列 + 写屏障 | `gc_bgc.h/.cpp` | 线程本地 SATB 队列 |
| g_bgc_is_marking 标志 | `gc_bgc.h` | 原子标志，全局可见 |
| Codegen SATB emit | `NativeAotLoweringPlanner*.cs` | 在 stfld/stobj 前插入 SatbPreWriteBarrier 调用 |

### P2: Concurrent Mark（~400 行）

| 任务 | 文件 | 描述 |
|------|------|------|
| BGC 线程主循环 | `gc_bgc.cpp` | 从 mark_stack_ 消费 grey 对象 |
| SATB 队列处理 | `gc_bgc.cpp` | 定期 drain 所有线程 SATB |
| 并发安全 MarkObject | `gc_bgc.cpp` | 原子 bitmap 测试-设置 |
| 初始 root 标记 | `gc_bgc.cpp` | STW 设置 root set |

### P3: Re-Mark + Sweep（~300 行）

| 任务 | 文件 | 描述 |
|------|------|------|
| STW re-mark 暂停 | `gc_bgc.cpp` | SATB drain + dirty card scan |
| 并发 sweep | `gc_bgc.cpp` | 复用现有 SweepPage |
| 调度器集成 | `gc_scheduler.cpp` | FULL_BGC 触发决策 |

### P4: 验证 + 调优

| 任务 | 描述 |
|------|------|
| GC stress test BGC mode | 在 CHAOS_GC_STRESS_MODE 下启用 BGC |
| 暂停时间分析 | PROFILE 构建 + RDTSC 测量 STW 持续时间 |
| SATB 队列大小调优 | 避免过多写屏障 STW 停顿 |

---

## 9. 风险与考量

1. **SATB 队列溢出**：内存压力下的高频率写入可能导致 SATB 队列溢出。对策：每个条目仅是 `void*`（8 字节），默认队列大小 4KB→512 条目。若溢出则阻塞线程直到 BGC 线程 drain。

2. **BGC 线程 CPU 争抢**：BGC 线程以空闲优先级运行（`SCHED_IDLE` on Linux, `THREAD_PRIORITY_LOWEST` on Windows），在单核 CPU 上也应最小化对 mutator 的影响。

3. **碎片化**：并发标记不处理碎片。碎片率 > 50% 的 full GC 应降级为 STW + compact（现有逻辑不变）。

4. **与现有并行 mark 的冲突**：BGC 线程使用单线程 mark（无 work-stealing），因为并发标记期间线程数不固定。STW re-mark 可用并行 mark（现有 DrainMarkStackParallel）。

5. **Finalizer 处理**：BGC 标记完成后，不可达对象的 finalizer 在 STW re-mark 后处理（现有 RunFinalizers 逻辑不变）。
