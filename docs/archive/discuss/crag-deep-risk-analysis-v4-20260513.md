# CRAG (Chaos Region-Aware GC) v4 深度风险分析报告

**分析时间**: 2026-05-13
**分析范围**: `src/native/runtime-core/gc/` + `domain_unloader.cpp` + `thread_state.cpp` + `gc_stress_test.cpp`
**分析维度**: 分配策略、GC 策略、内存泄漏、API 设计、线程安全、UAF/双重释放、性能瓶颈、测试覆盖

---

## v4 发现总览

| 等级 | 数量 | 编号 |
|------|------|------|
| HIGH | 6 | V4-H1 ~ V4-H6 |
| MEDIUM | 9 | V4-M1 ~ V4-M9 |
| LOW | 7 | V4-L1 ~ V4-L7 |

---

## HIGH 风险

### V4-H1: Safepoint 无进程级所有者 — 两个线程可同时持有 safepoint

- **文件/行号**: `thread_state.cpp` L175-L191（RequestGlobalSafepoint），L126-L127 注释提及 `s_safepoint_owner` 但未实现
- **问题描述**: `RequestGlobalSafepoint()` 仅依赖 `thread_local s_safepoint_depth` 做嵌套检测。由于是 TLS，两个不同线程各自看到 depth=0 并都认为自己是 safepoint 的唯一持有者。线程 A 翻转 generation 为奇数，线程 B 再次翻转（奇数 XOR 掩码 → 仍是奇数）。两个线程同时执行 GC 操作（扫描、标记、清理），而其他线程看到 generation 为奇数后在 SafepointPoll 中自旋——但此时每个 GC 线程看到的 generation 值可能不同，STW 模型崩溃。
- **影响**: 并发 GC 中根本性 STW 模型崩溃。GC 线程 A 在扫描 old-gen 页面时，GC 线程 B 可能在并发释放/标记同一页面。其他线程可能在 generation 翻转回偶数后恢复执行托管代码，而此时 GC 仍在进行中 → UAF + 堆损坏。域卸载场景（有自己的 Request/ReleaseGlobalSafepoint 配对）下嵌套调用会放大问题。
- **修复建议**: 添加进程级原子 `s_safepoint_owner` 字段（如注释所述），`RequestGlobalSafepoint` 通过 CAS 争抢所有权，失败则自旋等待。`ReleaseGlobalSafepoint` 必须清除。TLS depth 仅用于同一线程嵌套场景。

### V4-H2: Safepoint 无确认协议 — GC 在线程未全部停止时就推进

- **文件/行号**: `thread_state.cpp` L175-L191
- **问题描述**: `RequestGlobalSafepoint()` 将 generation 翻转为奇数后立即返回，不等待任何线程确认已到达 SafepointPoll。`GcScanAllThreadRoots` 的注释声称"所有托管线程要么在 SafepointPoll 自旋，要么已让出"，但协议层未强制执行。在深度 native 帧（如 P/Invoke、长时间 native 计算）中未及时轮询 safepoint 的线程，可以在 GC 已开始扫描其栈、标记、清理后继续修改托管堆。
- **影响**: 与 V4-H1 叠加时危害倍增。标记阶段之后对象的写入可导致不一致的标记状态和释放后内存回收。
- **修复建议**: 实现两阶段停止协议：(1) 设置奇数 generation，(2) 轮询所有活跃线程直到其 `last_seen_gen` 匹配当前奇数 generation。`ManagedThread` 已有 `last_seen_gen` 字段但从未被 GC 线程读取。

### V4-H3: `pinned_roots_` 在 Collect() 中无锁迭代，AddPinnedRoot 持锁写入 — 数据竞争/UAF

- **文件/行号**: `gc_old_gen.cpp` L829-L834（无保护迭代），L1047-L1052（mutex 保护的 push_back）
- **问题描述**: `Collect()` 通过 `for (auto& pr : pinned_roots_)` 迭代而不持有 `mutex_`（mutex 仅在线程池/页面列表操作期间持有）。数据竞争：线程 A（GC 线程，尚未进入 SafepointPoll 自旋）调用 `Collect()` 开始读取 `pinned_roots_`。线程 B（托管线程，在 SafepointPoll 自旋前）调用 `AddPinnedRoot`，获取 mutex，对 `pinned_roots_` 做 `push_back`。如果 `push_back` 触发 vector 重新分配，线程 A 的迭代器变为悬空 → 堆损坏。
- **影响**: 真正的数据竞争，可导致 UAF（释放的 vector 内存上读/写）、堆损坏（vector 内部指针损坏）、或撕裂读取导致错过 pinned root。随后 GC 可能释放仍有活跃 pinned root 的页面 → 托管代码视角的直接 UAF。
- **修复建议**: 将 `Collect()` 中的 `pinned_roots_` 迭代包装在 `std::lock_guard<std::mutex>` 中。或像 `DrainMarkStack` 那样在 mutex 保护下快照到本地 vector，释放 mutex 后迭代快照。

### V4-H4: Scheduler 分配计数器基本失效 — 主动 Full GC 触发接近死代码

- **文件/行号**: `gc_scheduler.cpp` L13-L15（RecordAllocation），L60-L82（DecideCollection），`gc_region.cpp` L86/L143/L185/L236（RecordAllocation 调用点）
- **问题描述**: `RecordAllocation(size)` 仅在 nursery slow path 中被调用，且记录的 `size` 是**当前单次分配的大小**（通常 32-2048 字节），而非自上次 GC 以来的累积分配量。这意味着 `alloc_since_last_gc_` 和 `alloc_since_last_full_gc_` 的值远低于实际分配量。`DecideCollection` 中的 FULL 触发条件 `alloc_full > heap_est * 4`（kFullTriggerMultiplier=4）几乎永远不会被满足——例如 64MB 堆需要 256MB 的 alloc_full，而每次 slow path 只加 32 字节，需要 800 万次 slow path 调用。但实际上 full GC 还通过 `IsFullGcRequested()`（外部请求）和 `OldGen::Allocate` 中的 oversized 路径（L296-L302）触发，所以不是完全 dead——只是基于分配量的主动触发失效。
- **影响**: Heap 在没有外部请求或 oversized 分配的情况下可以无界增长。Young GC 不受影响（因为 nursery-full 时通过"nursery 有存活对象"检查 L69-L73 主动触发）。Full GC 仅在以下情况触发：(a) 外部显式 `RequestFullGc`，(b) oversized (>32KB) 分配路径，(c) `IsFullGcRequested` 返回 true。正常 small/medium old-gen 分配路径永远不会触发 full GC。
- **修复建议**: 重构 `RecordAllocation` 使其跟踪实际的累积分配量。一种方案：在 `NurseryAllocate`（fast path inline）中使用线程局部计数器，slow path 中将累积值批量上报。或者：移除基于计数器的 full GC 触发，改用 heap 增长比例触发（如：old-gen page_count 翻倍时触发 full GC）。

### V4-H5: `RemoveNurseryRange` 原子存储顺序错误 — `IsNurseryPointer` 可对已释放 nursery 返回 false positive

- **文件/行号**: `gc_region.cpp` L527-L538（RemoveNurseryRange）
- **问题描述**: `RemoveNurseryRange` 按 `begin=0 → end=0` 的顺序写入。读者 `IsNurseryPointer` 按 `begin → end` 的顺序加载。当读者在 begin=0 已写入而 end 尚未写入的时间窗口中加载时，看到 `begin=0` 和 `end=old_end`，判断 `0 < old_end` 为 true → 对已释放 nursery 返回 true（false positive）。这会导致 GC 尝试从已释放的 nursery 中 scavenge 对象。
- **影响**: `IsInNursery` 对已释放内存返回 true → `GcScavengeObject` 读取陈旧/未定义数据 → 被提升的对象内容损坏。写入顺序应为 `end=0 → begin=0`（先让 range 失效，再清理 begin）。
- **修复建议**: 将 `RemoveNurseryRange` 中的存储顺序交换：先 `store(end, 0)`，后 `store(begin, 0)`。或在 begin 和 end 之间插入 atomic_thread_fence(release)。

### V4-H6: `GcRecordAlloc` 在 `NurseryAllocateAtomicSlow` oversized 路径中重复计数

- **文件/行号**: `gc_region.cpp` L151-L154
- **问题描述**: `NurseryAllocateAtomicSlow` 的 oversized 路径调用 `g_old_gen.Allocate(size, false)`（内部已调用 `GcRecordAlloc(size, true)`），然后又在 L153 显式调用 `GcRecordAlloc(size, true)`。导致 `alloc_bytes` 和 `alloc_count` 重复计算。对比 `NurseryAllocateSlow` 的 oversized 路径（L44-L48），后者正确处理了（无显式 GcRecordAlloc）。
- **影响**: 统计信息严重不准确（计数翻倍）。虽然在统计正确性之外无功能性影响，但会掩盖真正的内存压力信号。
- **修复建议**: 移除 L153 的 `GcRecordAlloc(size, true)`，与 L44-L48 的风格保持一致。同理，OOM fallback 路径 L126-L128 和 L219-L221 也有相同问题——`g_old_gen.Allocate` 已内部调用 `GcRecordAlloc`，外部不应重复调用。

---

## MEDIUM 风险

### V4-M1: Full-GC reclaim 路径缺失 `RecordAllocation`

- **文件/行号**: `gc_region.cpp` L107-L109, L202-L204
- **问题描述**: `NurseryAllocateSlow` 和 `NurseryAllocateAtomicSlow` 中 full-GC 后重试 bump 的路径（如果 full GC 释放了足够空间）不调用 `g_gc_scheduler.RecordAllocation(size)`。对比 young-GC reclaim 路径（L86/L185）正确调用了。此处缺失会导致 scheduler 的分配计数器漏记。
- **影响**: `alloc_since_last_gc_` 和 `alloc_since_last_full_gc_` 漏记。虽然 V4-H4 已指出计数器整体偏小，但此处缺失进一步加剧了计数不准。
- **修复建议**: 在 L109 和 L204 的 `return ptr` 前添加 `g_gc_scheduler.RecordAllocation(size)`。

### V4-M2: `SafepointPoll` 硬超时破坏 STW 不变量

- **文件/行号**: `thread_state.cpp` L157-L170
- **问题描述**: 硬超时（10M 自旋 ≈10ms）允许线程在 generation 仍为奇数时退出自旋（L168 `break`）。退出后将 `at_safepoint` 置 false 并返回调用者继续执行托管代码。此时 generation 仍为奇数，GC 线程认为所有线程已暂停。
- **影响**: 即使有了正确的 `s_safepoint_owner`（修复 V4-H1 后），此超时也为 GC 进行中的并发托管代码执行打开了窗口。`GcScanAllThreadRoots` 既不读取也不检查 `at_safepoint`。
- **修复建议**: 不要默默地退出自旋。超时应设置 per-thread"超时"标志，GC 线程在 root 扫描前检查并 respect 该标志。或使超时可配置，生产环境默认禁用（仅作为防御死锁的逃生出口）。

### V4-M3: 跨线程 nursery root 扫描在线程退出期间有 TOCTOU 竞争

- **文件/行号**: `gc_old_gen.cpp` L842-L864，`thread_state.cpp` L77-L92
- **问题描述**: `Collect()` 在 safepoint 保护下枚举所有 `ManagedThread` 条目，读取 `nursery_ctx` 并扫描 nursery。但在 `EnumerateThreads` 和 lambda 之间的间隙，线程可调用 `UnregisterThread`（内部调用 `TeardownTlsNursery` 将 `nursery_ctx` 置 nullptr）。lambda 检查了 nullptr，但在赋值给本地 `nursery*` 后、访问 `nursery->begin` 前未重新检查。如果 `TeardownTlsNursery` 在该间隙释放了 `Region` 的内存，则 `ScanRangeForRoots` 读取变为 UAF。
- **影响**: 在严重争用下（大量线程在 safepoint 采集窗口恰好退出），概率低但后果严重。
- **修复建议**: 在回调中获取本地 nursery 指针后添加屏障读-检查-使用循环。或在区域管理器 API 中添加"pin region"调用，使区域在 GC 扫描期间保持活跃。

### V4-M4: `ScanAndClearCrossDomainRefs` 扫描整个卡表 — O(1GB) STW 暂停

- **文件/行号**: `domain_unloader.cpp` L55-L89
- **问题描述**: 实现通过 `kCardTableEntries`（2M 条目，覆盖 1GB 堆）线性扫描，为每个卡片执行 `IsInDomain`（mutex 保护下扫描 region 表）。对于 1GB 堆，2M 次迭代 × 每次 region 表扫描。本身不是正确性错误，但在 safepoint（"world stopped"）期间创建了 O(1GB) 暂停。
- **影响**: 1GB 堆域卸载可暂停所有线程数秒，延迟关键路径（热更新加载、故障转移）。
- **修复建议**: 添加 region→card 反向映射，限制为仅扫描被销毁域 region 覆盖的卡片。`ReleaseDomainRegions` 已遍历 region 表——可用 per-region 的 card boundary 只扫描相关卡片。

### V4-M5: 卡表溢出标志使 >1GB 堆时所有卡片变脏 — 保守回退

- **文件/行号**: `gc_card_table.h` L59-L63（DirtyCard 设置溢出），L92-L95（ClearAllCards 重置），L112-L119（ScanDirtyCards 回退）
- **问题描述**: 堆增长超过 1GB（`kCardTableEntries * kCardSize = 2M * 512B`）时，`DirtyCard` 设置 `g_card_table_overflowed = true`。一旦设置，`ScanDirtyCards` 将所有范围内卡片视为脏，失去跨代引用过滤。堆 >1GB 后每个 young GC 扫描每个 old-gen 页面上的每个 slot。
- **影响**: 正确性保持（保守扫描捕获所有引用），但性能影响巨大。堆 >1GB 的工作负载中 GC 时间可能主导暂停时间。
- **修复建议**: 在 `AllocatePage` 期间动态调整卡表大小。或记录超 1GB 的分配并扩展卡表。

### V4-M6: 压力测试 Scenario C 从不调用 `VerifyPattern`

- **文件/行号**: `gc_stress_test.cpp` L543-L548
- **问题描述**: Scenario C（激进 young GC 压力）调用 `WritePattern` 写入填充字节，但从不调用 `VerifyPattern` 验证。这意味着 Scenario C 无法检测：(a) 错误复制的转发内容（old-gen 副本中损坏的填充字节），(b) 重叠转发，(c) 因分配错误导致的垃圾数据。
- **影响**: 100 线程 × 256 次分配在激进 young GC 压力下的数据损坏不会报告为失败。
- **修复建议**: 向 Scenario C 添加 `VerifyPattern` 调用——至少每 16 次分配一次，类似于 Scenario A/B 的 `kVerifyStep` 模式。

### V4-M7: 压力测试中存在双重 `TeardownTlsNursery` 调用

- **文件/行号**: `gc_stress_test.cpp` L252-L255（显式 `TeardownTlsNursery()`），`thread_state.cpp` L83-L85（`UnregisterThread` 内部隐式调用）
- **问题描述**: `UnregisterWorker()` 先调用 `TeardownTlsNursery()` 再调用 `UnregisterThread()`，而 `UnregisterThread()` 内部也调用 `TeardownTlsNursery()`。当前安全只因第二次调用是 no-op（已将 `tls_nursery_ctx.nursery` 置 nullptr）。但对 `TeardownTlsNursery` 未来行为的变化会破坏测试。
- **修复建议**: 从 `UnregisterWorker` 移除显式 `TeardownTlsNursery()`，让 `UnregisterThread()` 的自动清理处理 nursery 释放。

### V4-M8: `survival_rate_` 非原子 double 的数据竞争

- **文件/行号**: `gc_scheduler.cpp` L27（RecordYoungCollection 写入），`gc_scheduler.cpp` L95（RecommendedNurserySize 读取）
- **问题描述**: `survival_rate_` 是非原子 double，在 `RecordYoungCollection`（safepoint 下写入）和 `RecommendedNurserySize`（任意线程/任意时间读取）之间存在数据竞争。读取 torn value 可导致 nursery size 计算失控。
- **影响**: 技术上 UB（C++ 对非原子变量的并发读写），实际效果是偶发的 nursery size 异常值。
- **修复建议**: 将 `survival_rate_` 改为 `std::atomic<double>` 或 `std::atomic<uint64_t>`（bitcast double↔uint64_t）。

### V4-M9: BFS worklist overflow multi-pass 可能残留未标记对象

- **文件/行号**: `gc_young_collector.cpp` L348-L395
- **问题描述**: Cheney BFS worklist 溢出时，后备执行 O(2 × nursery_size) 保守转发。所有未转发 nursery 字被保守转发到 old-gen，然后对所有转发对象做修复传递。如果 nursery 满（256KB/8=32K 对象），最坏 O(64KB) 复制 + O(32K) 指针检查。
- **影响**: 正确性保持（`IsForwarded` 检查防重）。性能影响在 STW 中对于 256KB nursery 应 <1ms。但若 overflow 在生产场景中变频繁则需关注。
- **修复建议**: 若 BFS overflow 变频繁，增加 `kCheneyWorklistSize`（当前 64K）或实现动态扩容。

---

## LOW 风险

### V4-L1: `ReleaseDomainRegions` 批量释放后不修剪空闲列表

- **文件/行号**: `gc_region.cpp` L402-L421
- **影响**: 域卸载后空闲列表可超过 `kFreeListTrimThreshold=16`，驻留内存不释放。热更新场景累积。
- **修复建议**: `ReleaseDomainRegions` 末尾添加 trim 调用。

### V4-L2: `UnregisterThread` 泄漏 `ManagedThread` 对象（有意但需记录）

- **文件/行号**: `thread_state.cpp` L87-L89
- **影响**: 每个线程 ~120 字节。10 万线程 → ~12MB。长时间运行高线程池场景需关注。
- **状态**: 代码注释已说明，按设计接受。

### V4-L3: old-gen fallback 路径冗余 `std::memset`

- **文件/行号**: `gc_region.cpp` L127, L153, L220
- **影响**: `g_old_gen.Allocate` 内部已 zero 内存，外部再 memset 浪费带宽。最坏 ~5-10% 性能影响。
- **修复建议**: 移除 OOM fallback 路径的显式 `std::memset`。

### V4-L4: Yong GC BFS overflow fallback 复杂度

- **文件/行号**: `gc_young_collector.cpp` L350-L395
- **影响**: 功能正确但 O(2×nursery_size)。对于 256KB nursery 应 <1ms。
- **修复建议**: 暂不处理。若 overflow 变频繁则增大 `kCheneyWorklistSize`。

### V4-L5: 并行 sweep spin wait 无超时 — 线程死锁风险

- **文件/行号**: `gc_old_gen.cpp` L955-L957
- **影响**: 若 worker 线程意外退出，主线程永久自旋 → hang。
- **修复建议**: 添加超时 + fallback 顺序 sweep。

### V4-L6: 多 pass mark stack overflow（4 pass 后仍可能残留）

- **文件/行号**: `gc_old_gen.cpp` L537-L603
- **影响**: 4 pass 后若 `more` 仍 true 则退出循环，可能残留未标记对象 → 可达对象被错误释放（UAF）。pathologically rare。
- **修复建议**: 第 4 pass 后若仍有对象，降级为保守全量标记（标记所有指针指向的 old-gen 页面）。

### V4-L7: 测试覆盖缺口

- **文件/行号**: `gc_stress_test.cpp` 全局
- **缺失场景**:
  1. **域卸载**: 无创建域→分配域内存→卸载域的场景。`UnloadDomain` 从未被调用。
  2. **并发 AddPinnedRoot**: 无线程在 GC 期间调用 `AddPinnedRoot` 的测试。
  3. **跨线程指针**: 无线程写入另一线程 nursery 的测试（反映跨代引用）。
  4. **超大对象 (>32KB)**: 所有分配低于 `kMaxNurseryAlloc`。`g_old_gen.Allocate` 的大分配路径未经压力执行。
  5. **终结器**: 无注册终结器的场景。`RunFinalizers` 代码路径未经真实工作负载执行。
  6. **嵌套 safepoint**: 无从 GC 回调中调用 `RequestGlobalSafepoint` 的场景。
  7. **域取消注册的 compress-and-pop-back 路径**: 无线程在域取消注册后使用 `FindDomainById`。
- **修复建议**: 为每个缺失场景添加特定测试场景（场景 E/F/G）。

---

## v1 → v2 → v3 → v4 修复状态总览

| 版本 | 发现 | HIGH | MEDIUM | LOW | 修复 |
|------|------|------|--------|-----|------|
| v1 | 4 HIGH + 10 MEDIUM | 4 | 10 | 0 | 全部修复 |
| v2 | 4 HIGH + 8 MEDIUM + 6 LOW | 4 | 8 | 6 | 全部修复 |
| v3 | 4 HIGH + 12 MEDIUM + 5 LOW | 4 | 12 | 5 | 全部修复，stress test 4/4 PASS |
| v4 | **6 HIGH + 9 MEDIUM + 7 LOW** | **6** | **9** | **7** | 待修复 |

---

## 风险依赖图

```
V4-H1 (safepoint_owner) ──→ V4-H2 (确认协议)
       │                          │
       └──→ V4-M2 (超时破坏STW) ──┘
                                       
V4-H3 (pinned_roots_ data race)  ← 独立
                                       
V4-H4 (scheduler counter) ──→ V4-M1 (漏记RecordAllocation)
       │
       └──→ V4-M8 (survival_rate_ data race)
                                       
V4-H5 (RemoveNurseryRange store order)  ← 独立

V4-H6 (GcRecordAlloc double count)  ← 独立
       │
       └──→ V4-L3 (冗余memset)
                                       
V4-M4 (domain unloader O(1GB))  ← 独立
V4-M5 (卡表 >1GB overflow)  ← 独立
V4-M6 + V4-M7 + V4-L7 (测试覆盖)  ← 独立
```

---

## 最优修复顺序

**Phase 1（正确性 — 必须全部修复）**：
1. **V4-H1** + **V4-H2**: safepoint 所有者 + 确认协议（STW 根本性修复）
2. **V4-H3**: pinned_roots_ 互斥锁保护（数据竞争 → UAF）
3. **V4-H5**: RemoveNurseryRange 原子存储顺序（false positive → 数据损坏）
4. **V4-H6**: GcRecordAlloc 重复计数修复

**Phase 2（GC 有效性）**：
5. **V4-H4**: Scheduler 计数器重构（主动 full GC 触发）
6. **V4-M1**: Full-GC reclaim 路径补 RecordAllocation
7. **V4-M8**: survival_rate_ 原子化

**Phase 3（性能 + 测试）**：
8. **V4-M2**: Safepoint 超时安全化
9. **V4-M3**: TOCTOU nursery 竞争修复
10. **V4-M4**: 域卸载卡表扫描优化
11. **V4-M5**: 卡表动态扩展
12. **V4-M6~M7 + V4-L7**: 测试覆盖增强

---

*报告结束。v4 共 6 HIGH + 9 MEDIUM + 7 LOW 风险项，Phase 1（正确性项）推荐优先修复。*