========================================================================
## C2 (P2 provisional 完整形态) [P2（provisional 完整形态，属于降延迟/正确性收口；非 P1 阻塞项）。注意：任务声称的两项缺口中 NGC2 已实现（P1·已闭合），真正待补仅 gen1 强制 compact。若上游要求快速收口，可先只落 gen1 force compact 的 fire-once 最小实现，old-gen DecideCompactMode 强压（#⑤）降为可选增强单独立项。] provisional 完整形态：gen1 强制 compact（缺失）+ NGC2 排队（已实现，需复核）
[evidence] - NGC2 已实现：gc_scheduler.h:343-345（SetProvisionalMode 置 ngc2_queued_）、gc_scheduler.h:348-365（IsNgc2Queued/QueueNgc2/ClearNgc2）、gc_scheduler.cpp:315-321（DecideCollection 消费→FULL+清除）、gc_scheduler_test.cpp:157-183（TestNgc2Queue）
- force-blocking：gc_scheduler.cpp:306-309（provisional→prefer_bgc=false）、gc_api.cpp:125-129（OOM 进 provisional）、gc_old_gen.cpp:3272-3278（provisional→cap=0）
- gen1 compact 机制：gc_gen1.cpp:192/516-528/531-534/545-547/625-628/653-661（partial-retention compact + gen1_prev_compact_end）、gc_young_gen.h:74/102（promotion_age_threshold_）
- 【缺口证据】gen1 收集触发仅看 threshold/occupancy：gc_young_collector.cpp:723-737（should_collect_gen1 || near_full），无任何 InProvisionalMode 分支
- old-gen compact 纯启发式不经 provisional：gc_old_gen.cpp:1597-1654（DecideCompactMode NONE 早退 1630-1637）、gc_old_gen.cpp:2816/3360（两调用点 state）、gc_old_gen.h:428/470
- 消费点：gc_coordinator.cpp:59-64（server 单判 gen1）、gc_young_collector.cpp:745（WKS 调 GcGen1Collection）
- plan 陈旧标注：plan-v6-01.md:126-133、STATUS.md:131（claim 缺 NGC2，与实际不符）；docs/assessments/gc-vs-coreclr-review-2026-08-12.md:120、roadmap-v2-01.md:48/80/91/139（C2 定位与依赖）
[deps] - 直接依赖：已完成的 D1（OOM 降级链/force-blocking，gc_api.cpp+gc_scheduler）与 M4/M3B 的 NGC2 queue（已落地，当前就绪）
- 原 roadmap 标注依赖 M5（BGC 分相 gen1-compact 时序）与 M3B（server 堆调整）：经核实，gen1 force compact 消费点在 coordinator(Server) 与 young_collector(WKS) 都有，实现本身不阻塞于 M5 分相完成；但 BGC GEN1_GEN2 并发 mark 竞争需 M5 的 IsGen1MarkingActive 门控已可用（现在就有）——所以**不依赖 M3B 新增实现**，M5 提供的是正确性门控而非前置阻塞
- 本轮 C2 仅剩 gen1 强制 compact 一个子项，NGC2 部分依赖已满足无需等待
- 前置交付物：gc_scheduler NGC2 queue 已合并（本仓现况），gc_gen1 partial-retention compact 管线上一里程碑已存在
[risks] ① gen1 强制 compact 每触发即全量压缩「老对象」，尾部对象搬迁成本上升，长 pause 风险——必须在 fire-once + 仅 provisional 激活下收窄影响面；建议 gen1 force 只在 provisional_mode_ 且 gen1 有可用 compact 预算（gen1_capacity 不足时用既有 promote-all 兜底，gc_gen1.cpp:546-548 已有 MALLOC 失败 fallback）时执行。
② `GcGen1Collection(bool)` 签名变更波及所有调用点（gc_coordinator.cpp:63、gc_young_collector.cpp:745），需全量编译验证，避免漏改老调用默认参数 TRUE 误行为。默认 false 保持兼容。
③ provisional 存续期（api.cpp 未退出前）若每次 gen1 都强压，可能与 BGC GEN1_GEN2 scope（gc_young_collector.cpp:741）的并发 mark 竞争——force compact 必须同样受 `IsGen1MarkingActive()` 门控，否则破坏 BGC 两快照。
④ load/consume 标志需用一致的内存序（acquire/release），否则跨线程 gen1 收集可能读到陈旧 force 值或漏清。
⑤ 依赖修正：任务/STATUS(P2 task #11) claim "缺 NGC2" 与实际代码不符（NGC2 已实现+测试），exec 收口时应把这条从 C2 里剔除，仅剩 gen1 强制 compact，避免重复实现/重复测试。
### gap_analysis
逐条核对任务前提与实际代码，结论如下。

【现状盘点——五条线索均已实读】

① force-blocking（已有，完整）：
- `gc_scheduler.h:615` `std::atomic<bool> provisional_mode_{false}`
- `gc_scheduler.h:341-346` `SetProvisionalMode(bool)` 存标志；**进入时同时置 `ngc2_queued_=true`（line 343-345）**
- `gc_scheduler.cpp:306-309` `DecideCollection()` 内 `provisional` 强制 `prefer_bgc=false`（禁 BGC 延迟）
- `gc_api.cpp:106/118/129` OOM 分配失败/恢复时 toggle provisional；`gc_old_gen.cpp:3272-3278` provisional 下 `effective_cap=0`（禁 old-gen 池扩容）

② NGC2（mandated gen2 collection）排队【已实现 ≠ 任务 premise】：
- `gc_scheduler.h:348-365` 完整 API：`IsNgc2Queued()/QueueNgc2()/ClearNgc2()`
- `gc_scheduler.cpp:315-321` `DecideCollection()` 顶部消费：`ngc2_queued_` 真 → `SetLastTriggerReason(PROVISIONAL/EXPLICIT_REQUEST)` → `return GcCollectionKind::FULL`，随即清除（fire-once）
- 附带测试：`tests/contracts/native/runtime-core/gc_scheduler_test.cpp:157-183` `TestNgc2Queue()`（含「SetProvisionalMode(true) 排 NGC2 → next decide 强制 FULL」断言，line 174-179）
- 结论：任务 claim "缺 NGC2 排队" 相对代码已**陈旧**。plan-v6-01.md:131、STATUS.md:131 的 "缺" 与实际不符；roadmap 依赖 M3B 的前提也无必要（NGC2 是 WKS 调度层独立实现，不依赖 server 多堆）

③ gen1 强制 compact（【真正缺口】）：
- gen1 有 partial-retention mark-compact：`gc_gen1.cpp:192-674`，核心是 `gen1_prev_compact_end`（老对象 promote、新对象 compact，见 gc_gen1.cpp:531-534, 625-628, 653-657）
- 触发完全由 `promotion_age_threshold_`（`gc_young_gen.h:102` 默认 2；`gc_young_collector.cpp:723-737` 决定是否调 `GcGen1Collection`）与 BGC GEN1_GEN2 scope（`gc_young_collector.cpp:741`）控制
- **没有任何分支读 `InProvisionalMode()` 去影响 gen1 是否 compact / 是否 promote-all**。provisional 只影响调度层 FULL/BGC 选择与旧代池 cap，不带动 gen1 压缩
- old-gen 的 `DecideCompactMode()`（`gc_old_gen.cpp:1597-1654`）纯碎片启发式（`total_live==0/kMinCompactBytes/candidate_pages==0` → NONE，line 1630-1637），同样不感知 provisional

④ CoreCLR 对齐差距：collect.cpp:1638 语义 = "高记忆+高碎片 → gen1 强制 compact + NGC2 排队、而非全 GC"。本仓 NGC2 有了，gen1 强制 compact 分支缺。

结论 gap：C2 只剩「provisional 触发 → gen1 强制 compact」这一个语义分支没落地；NGC2 premise 应标记为已完成并由 exec 复核再收口。
### fix_design
【修改点】只补「gen1 强制 compact」，NGC2 已齐不动。

核心：新增一个 scheduler 可查的 `ForceGen1Compact` 原子标志，在 provisional 进入时置位；`GcGen1Collection` 前读取该标志，为真时跳过 promote-all 语义、走 partial-retention compact，并在消费后清除（fire-once，对齐 NGC2 做法）。

① gc_scheduler.h：
- 新增成员（紧挨 provisional_mode_，line 615 附近）：
  `std::atomic<bool> force_gen1_compact_{false};`
  及 `bool ForceGen1Compact() const noexcept`（acquire 读）
- 扩 `SetProvisionalMode(bool on)`（line 341-346）：`on` 时额外 `force_gen1_compact_.store(true)`（说明：进入 provisional 意味着后续 gen1 收集强制 compact 降延迟）

② gc_scheduler.h 或 gc_scheduler.cpp：新增 `void ConsumeForceGen1Compact() noexcept { force_gen1_compact_.store(false, release); }`（读后清除，防每轮重复压缩）

③ 触发消费点——两处采集 gen1 汇聚入口，均需在调用 `GcGen1Collection()` 前判标志：
- `gc_coordinator.cpp:59-64`（Server 路径 Phase1 gen1 收集）
- `gc_young_collector.cpp:737-745`（WKS young 收集内 gen1 分支）：在 `should_collect_gen1` 逻辑后加
  ```cpp
  bool force_c = G_Scheduler().ForceGen1Compact();
  if (force_c) { G_Scheduler().ConsumeForceGen1Compact(); /* fall-through 强制收集+compact */ }
  ```
  并把 force 态透传给 `GcGen1Collection()` 控制 promote-all vs compact。

④ gc_gen1.cpp 签名扩展：`GcGen1Collection(bool force_compact=false)`（头 `gc_gen1.h` 同步）。在 Phase 4（gc_gen1.cpp:516-528）加分支：`force_compact` 时把 `boundary=gen1_prev_compact_end` 语义改成「全量 compact（所有存活对象 compact 回 gen1，不 promote）」，复用既有 compact_buf 管线（line 545-671），仅改「老对象也 compact 而非 promote」的落点。缺省 false 保持现状。

⑤ old-gen 侧可选：若要让 provisional 全 GC 也强制压实（对齐 collect.cpp 的 gen1 强制），`MarkSweepOldGen::DecideCompactMode()`（gc_old_gen.cpp:1597）开头加 `if (G_Scheduler().InProvisionalMode())` 强行越过 line 1630-1637 的 NONE 早退，返回 `CROSS_PAGE`。此为可选增强，可单独立项。

设计要点：显式 fire-once 标志 + 消费清除，语义与既有 NGC2(ngc2_queued_) 完全一致，避免 provisional 存续期每次 gen1 都强制 compact。不依赖 M3B。经此，provisional 完整形态 = force-blocking(✅) + NGC2(✅) + gen1 强制 compact(补)。
### tests
新增/扩展测试（挂 `gc_provisional_test`）：

① TestProvAllCompact（核心，黑盒）：
- 前置：填充 gen1 使有若干"老"（cross 过 gen1_prev_compact_end）+ "新"存活对象
- `SetProvisionalMode(true)` → 触发 gen1 收集
- 断言：(a) `GcGen1Collection` 强收集被调（gen1_bump 被处理/重置）；(b) "老"存活对象未 promote 到 gen2，而是 compact 回 gen1（检查 gen1_prev_compact_end 更新 + 对象仍驻 gen1 区）；(c) 消费后 `ConsumeForceGen1Compact` 已清除、二次收集不再强压
- 对照：provisional OFF 同场景走原 threshold 语义

② TestProvAstable（fire-once）：
- provisional 保持 on、连续若干 gen1 收集：第一次因 force 标志压 compct，之后无反复强压（检查 force_gen1_compact_ 一次消费即清）

③ 若实现 #⑤ 额外断言：provisional 下 `DecideCompactMode()` 返回 CROSS_PAGE 而非 NONE，即使碎片低于阈值（读旧代 total_live / candidate_pages）。单独测试。

测试置于 `tests/contracts/native/runtime-core/` 下的 gc_scheduler_test.cpp（补 TestProvisionalForceBlocking 邻近）或新建 `gc_provisional_test.cpp`，复用 GC_CHECK/TestNgc2Queue 既有风格（gc_scheduler_test.cpp:152-183, 200-222）。均带 baseline 对照防止误报。
========================================================================
## #5 A6 [None] Page 回收(pool) + FindPage 索引：现状已落地，本轮为缺口复核+加固设计
[evidence] gc_old_gen.h L295-308 (page_array_ 声明"O(log n)"), L536-547 (page_array_/retired_arrays_ 成员), L555-572 (page_pool_/PoolEntry/kMaxPoolSize=16); gc_old_gen.cpp L395-441 RebuildPageArray(排序), L443-523 FindPage(二分+重试), L207-309 AllocatePage(pool 取页+PalVirtualCommit+recordPageCountGrowth), L388-391 FreePage(VirtualFree), L256/273-274/303 pool 复用, L886-904 Free 仅 oversized 设 in_use=false, L907-916 普通页 Free 仅 memset 64B, L1293-1456 SweepPage(仅 oversized L1332 设 in_use=false), L1545-1560 PageFragmentation, L2715-2732 Collect 100%-free 普通页入池+page_count_--(L2733), L3223-3258 BgcSweep Phase4b should_pool>=1.0f 摘除, L3269-3328 pool NUMA trim+provisional effective_cap=0, L3390-3407 BgcCompact PalVirtualDecommit 补入池; gc_scheduler.h L500 kMaxPageGrowthThreshold=16, L557 page_count_growth_; gc_scheduler.cpp L287-345 DecideCollection(page_count_growth>=16 触发 + provisional prefer_bgc=false), L144/164 page_count_growth_ reset; 测试 tests/contracts/native/runtime-core/gc_old_gen_unit_test.cpp 仅 6 TEST(L30/54/74/90/110/122)。
[deps] 依赖 A6 里程碑；池/索引主实现已在 main(dev) 由先前 GC plan-v5(M11/M3A/M2) 落地，需 BGC/Server GC 多堆(M3A-2/3)与 dynamic_tuning 伺服(E1/H3) 上下文配合验证 NUMA/压力路径。
[risks] 1) 复用页 dangling：pool 页回收后页内残留 stale 指针(Free 的普通页 L916 只 memset 64B)。旧 mark_bitmap 已被 PalVirtualDecommit 零化、头部字段由 PoolEntry 恢复(L282-287)+重新 carve_free_lists(L298)，内存复用安全；但若 trim 用 MEM_RELEASE 释放 VA(L3322)而 AllocatePage 仍用 PalVirtualCommit 唤醒，地址可能被 OS 收回后 recommit 到不同数据——需保证 pool 内页在 trim 时彻底从所有引用(含 retired_arrays_ 内旧快照指针)摘除，当前依赖 BgcSweep/Collect 的 deferred_sync 机制，并发窗口极小但存在理论 UAF。find_old_gen_unit_test 的 FindObjectContaining(L99/L1223)对池内残留指针需处理。

2) 压缩时引用(dangling during compact)：即使能池化，若某 100%-free 页在 BgcCompact 的 GlobalRelocate 仍被其他页对象 slot 引用(内部指针指向该页已 free 内存)，memmove 后 relocation 读旧地址会踩池化后新数据。当前设计以「仅 100%-free 才池化」规避——但普通页 Free 只 memset 64B，若对象有跨页内部指针，需确认该页在池化前无 cross-page 活引用(RelocatePage 的 pages 快照是冻结的，池化发生在 sweep 后 relocation 已完)。

3) FIX-1 若做成同步即刻回收，会在 Free() 热路径引入 syscall/锁竞争，性能回归——必须后台异步 + mutex_ 保护不可重入。

4) 既有 page_pool_/FindPage 已是「已落地代码」，误改易破坏 NUMA/并发一致性；改前须以测试锁定现状再动。

建议按依赖顺序：FIX-2 测试先行锁定 → FIX-1/FIX-3 加固 → 全量 unit 回归。
### gap_analysis
任务描述的三项现状与源码实际不符——核心机制已全部在 main(dev) 实现并落地：

【关键偏差1：FindPage 已 O(log n)——非 O(N)线性】gc_old_gen.h L295-308/536-547 已声明 page_array_(atomic<PageArray*>) 索引；gc_old_gen.cpp L395-441 RebuildPageArray() 在每次 page_list_ 变更后于 mutex_ 内重建「按地址升序的 OldGenPage** 数组」；L443-523 FindPage() 对快照做二分(binary search)，线性扫描仅作 init/空数组兜底(L450-477/469)。老数组永不释放(retired_arrays_ L547)，无 TOCTOU。header 注释(L295)已注明「O(log n)」。

【关键偏差2：100%-free 普通页已回收进 pool——非从未释放】gc_old_gen.h L555-572 已定义 page_pool_+PoolEntry+kMaxPoolSize=16；gc_old_gen.cpp L388-391 FreePage()(VirtualFree)；Collect Phase4(L2715-2732)：对 100%-free 普通页(PageFragmentation>=1.0f 且非 oversized)摘出 page_list_→push 进 page_pool_，page_count_--；BgcSweep Phase4b(L3223-3258)同样处理但先 defer 到 deferred_pool_pages_，BgcCompact(L3401-3407)STW 下 PalVirtualDecommit 后入池；AllocatePage(L256-309)优先从 pool 取页并 PalVirtualCommit 复用；pool 超限按 NUMA trim 回 OS(L3269-3328)。page_count_ 在回收处递减(L2733/3245)，仅在真正增长处递增(L303/368)——非单调。

【真实剩余缺口（本轮值得做）】：
(a) Free() 只对 oversized 页 L886-904 设 in_use=false 走 deferred_free；普通页 L907-916 仅 memset(ptr,0,64)，其 page 何时入池完全依赖「下一次 collect 的 PageFragmentation>=1.0」——若此后无 GC，整页 64KB 内存一直挂着（SweepPage 已把块放回 free_lists，可复用但虚拟页未释放回池）。这是「普通页释放」的真正延迟点。
(b) 无测试覆盖 pool 回收/复用/FindPage 二分/NUMA trim；现有 gc_old_gen_unit_test.cpp 仅 6 个 TEST(L30/54/74/90/110/122)，未见 pool/索引断言。
(c) NotifyGC 接入点：DecideCollection 用 page_count_growth_(>=16,L335)与 alloc_full 触发齐全，但「内存压力下应主动 trim pool/清空 vector」尚无统一入口（仅 BgcSweep/Sweep 顺带）。
结论：任务应重定义为「复核既有实现 + 补缺口」，而非从零设计。
### fix_design
因已有实现，改为「对齐缺口清单的加固型修复」：

【FIX-1 补 Free() 普通页延迟回收(真正缺口)】gc_old_gen.cpp Free() L907-916 分支：memset 后检测该页是否已全空。放一个轻量判断：SweepPage 语义保证非 oversized 页 empty 判定应看 mark bitmap 全 0 或该页所有 free_lists 覆盖整 payload。更稳妥方案=不在此处动，额外加一个「低代价即刻回收开关」：在 Free() 后调用新增 G_Scheduler().RecordFreelistRelease()（或直接触发 RecordPageCountGrowth 的反向回收信号），使下次 DecideCollection 优先走一次 BgcSweep 让 Phase4b 回收该页。推荐后台异步，不在分配热路径同步跑。

【FIX-2 校验 FindPage 二分正确性(已有代码加固)】无需重写；新增断言/测试锁定：RandomAddr=页[0].start-8 必须返回 nullptr(下界外)、page[last].end+8 nullptr(上界外)、页间空洞(nullptr)、页中心、页首/页尾边界各返回正确页；并验证 RebuildPageArray 在 pool 回收 + AllocatePage 复用后 count/顺序一致(不引用已 VirtualFree 页)。

【FIX-3 统一 pool 触发阈值接入 DecideCollection】gc_scheduler.cpp DecideCollection L287 增加分支：若 page_pool_ 大小>kMaxPoolSize 且 InProvisionalMode()（高内存压力），在返回 NONE 前先触发一次 FULL(or FULL_BGC) 走 Sweep 清空 pool(现已有 provisional 时 effective_cap=0 L3278 的机制)；或新增 G_Scheduler().RequestPoolTrim() hint。保证「高压力下 pool 不悬空长期占用物理内存」。

【池语义对齐 CoreCLR region_free_list.cpp】：既有 PoolEntry 内存策略(MEM_DECOMMIT 保留 VA + COMMIT 复用 + NUMA 匹配取页)已对齐，无需改。只需确保 trim 时(L3322)用 PalVirtualFree(MEM_RELEASE) 释放 VA，与 AllocatePage 的 PalVirtualCommit 对称；deferred_pool_pages_ 由 BgcCompact 统一 commit，避免 dangling。
### tests
1) 新增 gc_old_gen_pool_test：①分配多页→全部 Free→进 pool；②断言 page_pool_.size 增加且 page_count_ 递减；③再次 Allocate→复用 pool 页(断言返回地址∈pool 且无新 VirtualAlloc Syscall 计数增长)；④超限 trim(page_pool_>16 后断言≤16 且 NUMA 各节点≥kMinPagesPerPoolNode=4)。

2) 新增 FindPage 二分边界测试(插桩 page_array_ 固定 5 页)：验证关键/边界地址归属与越界返回 nullptr；页间空洞(刻意不连续地址)正确；并发下 RebuildPageArray 后旧快照仍可安全读(retired_arrays_ 不释放)——现有 FindPage 已自带一次重试(L501)，断言重试覆盖并发新增页场景。

3) gc_old_gen_free_reclaim 测试（对应 FIX-1 缺口）：分配→Free 单个普通页→触发一次 BgcSweep→断言该 100%-free 普通页被移入 deferred_pool / pool，page_count_ 减 1，且不出现 dangling(find 后无 use-after-free)。

4) 回归：tests/runner/test_driver.py --layer unit 现有 6 个 TEST 全绿 + 既有 jit/gc 测试不回归。

全部测试放 tests/contracts/native/runtime-core/(与 gc_old_gen_unit_test.cpp 同目录)。
========================================================================
## A2 / P3-M3B (任务 #10) [P2（缩放能力/性能域，非正确性主线）。CoreCLR dynamic_heap_count 对齐项，任务单 #10 (A2)/plan-v5 P3-M3B。] Dynamic Heap Count：运行期增减 Server GC 堆数（依赖 M3A Server 多堆先落地）
[evidence] 固定堆数定义：gc_heap_manager.cpp:19-48 (Initialize/num_heaps/GcNumaNodeCount/heap_count_/make_unique)；members：gc_heap_manager.h:45(HeapCount只读)、:62-66(ForEachHeap)、:72-73(heap_count_+heaps_)。Thread映射：gc_heap_manager.cpp:50-60(HeapForCurrentThread 越界回退)；attach/detach：thread_state.cpp:122(SetThreadHeap)/:147(ClearThreadHeap 域卸载)。coordinator join：gc_coordinator.cpp:43-89(ExecuteMultiHeapGc, L47 n_heaps定格, L68-80 RunWorkers按n_heaps, L83 SetThreadHeap)。Server 门控：gc_features.h:38-44(CHAOS_IL2CPP_GC_SERVER=(=0))；per-heap 路由：gc_heap.h:31-41(GcHeapContext)、:67-86(Server accessor)，:36 young_gen 声明未用。动态线程先例：gc_worker_pool.h:26-46(idle 60s/SetMaxWorkers/kMaxWorkers=64)。负载信号：gc_scheduler.h:469(kEmaAlpha)、:117/446(RecordAllocation/TotalAllocatedSinceLastGC)。config 键模式：gc_config.h:37-65(CHAOS_GC_CONFIGURATION_KEYS)、gc_config.cpp:104+(setter ABI)。NUMA 探测：gc_numa_win.cpp:21-49/51-53。hint 公式：gc_heap_manager.cpp:38-40。计划依赖：plan-v5-01.md:81(M3B"依赖 M3A")、:44-48(M3A)、plan-v6-01.md:16-25(P1-A M3A-2/3)。
[deps] 强依赖 **M3A = Server GC 多堆集成**（plan-v5-01.md:44-48；plan-v6-01.md:16-25 P1-A）先行。必须已存在的 M3A 落地面：a) `GcHeapManager::Initialize` 多堆 array 真实可用（`gc_region.cpp:566` M3A-1 已接）；b) `ExecuteMultiHeapGc` 多堆协调跑通（`gc_coordinator.cpp:43-89`）；c) CI 新增 `-DCHAOS_IL2CPP_GC_SERVER=ON` 矩阵（plan-v6-01.md:24）；d) `G_*()` accessor 的 per-heap 路由（`gc_heap.h:75-84`）在 Server 下验证（plan-v6-01.md:23 标注 g_gc_scheduler 裸全局留待 server CI 处理）。依赖符号（本设计消费）：`GcHeapManager::Instance()/HeapCount()/GetHeap()/ForEachHeap`、`GcNumaNodeCount/CurrentNode`、`G_OldGen().Collect`、`GcWorkerPool::RunWorkers`、`threading::RequestGlobalSafepoint/ReleaseGlobalSafepoint`、`GcScheduler` EMA 负载字段。未接 M3A 前本任务仅设计可交付，代码实现必须挂起。
[risks] 1. per-heap hint 冲突：grow 若不复用既有堆 hint 公式（`gc_heap_manager.cpp:38-40`），新堆可能与已驻留堆 VA 冲突 — 必须抽 `HeapHintForIndex` 幂等生成，禁止偏移重排。
2. shrink 回收与 TLAB 在飞：safepoint 冻结 managed 线程但 TLS nursery 仍持 bump 指针，removed 堆的 TLAB 需先归还 region 再删堆（隐患同 `thread_state.cpp:134 TeardownTlsNursery` 时序）。
3. `GcHeapContext.young_gen` 潜伏不一致(声而不用)：动态堆若误改 per-heap young 会产生假正，须始终走共享 `g_young_gen`。
4. 负载信号源单一（只有 allocation EMA，无真实 memory-load 采样）→ 误判 grow/shrink 抖动；需 EMA+冷却+MinGcInterval 双闸。
5. 增减与 coordinator 的 `mgr.HeapCount()` 定格（L47）竞态：若 L47 保留 int 快照而增减在 Phase1 后发生，Phase2 worker 越界/缺堆 — 必须改 atomic acquire 读。
6. Server 默认 OFF：不接 M3A（Server ON + CI 全绿）就实现，无法在现有默认构建验证 — design 强依赖 M3A，实现前 M3A 必须落。
7. 原子化 `heap_count_`/`heaps_` swap 增加每分配路径一次 acquire 读开销（`SetThreadHeap` 热路径）— 可接受（每线程 attach 一次），但需基准确认无回归。
8. WKS 模式（Server OFF）所有动态逻辑必须 `#if CHAOS_IL2CPP_GC_SERVER` 完全排除（镜像 `gc_coordinator.cpp:32/41` 现状），否则污染 WKS 全局。
### gap_analysis
【现状-实读】GC 堆数完全固定，无任何运行期增减路径。

(1) 固定堆数定义：`gc_heap_manager.cpp:19-48` `GcHeapManager::Initialize(int num_heaps=0)`。num_heaps<=0 时经 `GcNumaNodeCount()` 自动探测(NUMA 节点数，Windows `gc_numa_win.cpp:34-35` 以 GetNumaHighestNodeNumber 得 highest+1)，否则用 num_heaps=n 直接设为 `heap_count_`（L28）。然后一次性 `make_unique<GcHeapContext[]>`(heap_count_) 数组（L29）。`heap_count_` 改完即定，后续再无写。

(2) 增减要改的成员：`gc_heap_manager.h:72-73` `int heap_count_{0}; std::unique_ptr<GcHeapContext[]> heaps_;`。`ForEachHeap`（h:62-66）按 heap_count_ 遍历；`HeapCount()`（h:45）只读；`GetHeap(int)`（h:48-50）直接 `heaps_[id]` 无界判。`GcHeapContext` 每堆载荷见 `gc_heap.h:31-41`（old_gen/loh/scheduler/gen1_state + 未用的 young_gen）。

(3) Thread→heap 映射消费 heap_count_：`gc_heap_manager.cpp:50-60` `HeapForCurrentThread()` 取 `GcNumaCurrentNode()`，越界(`node<0||node>=heap_count_`)回退 0。`SetThreadHeap`（h:81-84）在 `RegisterThread` 时调用（`thread_state.cpp:122`）；`ClearThreadHeap`（h:89-92）在 `UnregisterThread`/域卸载时调用（`thread_state.cpp:147`）——这是 per-heap 一致性在卸载侧的唯一挂钩。

(4) 多堆 GC join：`gc_coordinator.cpp:43-89` `ExecuteMultiHeapGc()`。L47 `int n_heaps = mgr.HeapCount()` 在进入 safepoint 后**一次性读取**；Phase1 共享 young+gen1 各跑一次（L54-65）；Phase2 按 `n_heaps<=1` 串行/否则 `GcWorkerPool::RunWorkers(n_heaps,[&]...GetHeap(worker_idx)...)`（L68-80）做 per-heap old-gen 收。L83 `SetThreadHeap()` 恢复本线程绑定。

(5) Server 模式默认 OFF：`gc_features.h:38-44` `CHAOS_IL2CPP_GC_SERVER=(=0)`，WKS 走全局 `g_old_gen` 等。Server ON 时 `gc_heap.h:75-84` 的 `G_OldGen/G_Loh/G_Scheduler` 经 `tls_current_heap` 路由到 per-heap；young+gen1 仍共享 `g_young_gen`（h:74）。注意：`GcHeapContext.young_gen` 成员（h:36）声明但全库无 `.young_gen` 实读——Server 实际用共享 g_young_gen，是动态堆设计必须先厘清的潜伏不一致。

【缺口】(a) heap 数初始化后锁死；(b) heaps_ 是 fixed-size `unique_ptr[]`，无 grow/shrink 原语；(c) coordinator 在 safepoint 后按 `mgr.HeapCount()` 定格 n_heaps，增减需在 GC 帧内重新协商；(d) 每堆 old_gen 拥有独立 NUMA hint `0x200000000 + i*0x40000000`（`gc_heap_manager.cpp:38-40`），增减堆要复算 hint 防 VA 冲突与既有堆冲突；(e) 无负载信号源接入点——scheduler 的 EMA/trigger 数据是唯一现成"负载"代理。
### fix_design
【设计】新增 `src/native/runtime-core/gc/gc_dynamic_heap_count.cpp/.h`，对齐 CoreCLR DYNAMIC_HEAP_COUNT 语义（运行期增减堆数），但强依赖 **M3A（Server 多堆 ON 且 CI 矩阵跑通）** 先落地。改法分 5 步：

(1) GcHeapManager 增 grow/shrink 原语（改 `gc_heap_manager.h/.cpp`）：
- 新增 `bool ResizeHeaps(int target_heaps) noexcept;` — 在 safepoint 下、`heap_count_==0` 或变更前禁止分配（gate）：目标只增不破坏既有堆。grow 侧：`GcHeapContext[]` 换新数组拷贝、新堆 `old_gen.Init(新hint)`（hint 生成器抽成 `HeapHintForIndex(i)` 复用 `gc_heap_manager.cpp:38-40` 公式，保证既有堆 hint 不变——这是不回归的关键）；shrink 侧：先对要删除的 id 做 old_gen/Loh 占位回收（域卸载语义，见 (3)），再收缩数组。`heap_count_` 用 `std::atomic<int>` 存储，`HeapCount()` 改 acquire 读。
- 新增 `int HeapForCurrentThread()` 保留越界回退 0（已如此），shrink 后仍在堆内的线程经 safepoint 重新 `SetThreadHeap()` 重绑（`gc_coordinator.cpp:83` 已有恢复点）。
- 新增 `int MaxHeaps()`（= NUMA 节点数探测值 `GcNumaNodeCount()`，每次迭代可重探测支持热插拔）与 `int MinHeaps()`（下限 1，WKS 恒 1）。

(2) 动态增减决策接入点（新增决策器，消费 scheduler 负载）：
- 新增 `GcDynamicHeapCount::MaybeResize()` 挂在 coordinator `ExecuteMultiHeapGc` Phase1 之前（`gc_coordinator.cpp:49` 获取 safepoint 后、Phase1 young+gen1 之前）。
- 信号源：跨堆聚合 `G_*_Scheduler().RecordAllocation/TotalAllocatedSinceLastGC/estimated_heap_size/flush 频率`（`gc_scheduler.h:117/137/446/568`）。负载判据：当"全局存活字节 / 每堆估计大小"持续超阈值（EMA 平滑，alpha=1/16 同 `gc_scheduler.h:469` kEmaAlpha）→ grow；当多堆长期空转（alloc 速率低 + 存活低）→ shrink。调参用 `gc_config.h` CHAOS_GC_CONFIGURATION_KEYS 加三键：`DynamicHeapCount(=0 关)`, `HeapGrowLoadThresholdFP`(*1000), `HeapShrinkLoadThresholdFP`，并补 `chaos_gc_config_set_dynamic_heap_count` 程序化 ABI（镜像 `gc_config.cpp:104+` 的 setter 模式）。grow 增量=1 堆/次、shrink=回收完全空闲堆，均带冷却（复用 `MinGcIntervalMs` 节奏，`gc_scheduler.h:592 kMinGcIntervalNs`）。

(3) GC join / 域卸载 per-heap 一致性：
- **增减只在 server-GC safepoint 内生效**——`ExecuteMultiHeapGc` L50 `RequestGlobalSafepoint()` 已冻结全部 managed 线程，`tls_current_heap` 不再被读改，此时操作 `heaps_` 数组安全。增减完统一 `SetThreadHeap()`（coordinator L83 已做，shrunk 出去的线程 id 越界→回退 heap 0）。
- **域卸载**：`ClearThreadHeap()`（`thread_state.cpp:147`）只清 TLS 指针，不解绑旧域在多个 heap 的 residual。动态堆场景要为 shrink 的堆补 `DomainDestroyAll`/per-heap domain 记录回收（对每个被移除 heap 遍历其 domain 表 O(region) 批量释放——对齐 plan-v5-01.md:48"域卸载 O(region)"判据）。新增 `GcHeapManager::ReclaimHeapDomains(int heap_id)` 在 shrink 前调用。
- **join 一致性**：Phase2 的 `RunWorkers(n_heaps,...)` L74 必须用**本次增减后的实际 HeapCount()**（atomic acquire 读，不再 L47 定格一次），worker idx 越界直接跳过（`if(worker_idx < n_heaps)` L75 已具此格）。

(4) 新测试 `gc_dynamic_heap_count_test.cpp`（放 `tests/contracts/native/runtime-core/`，用 `gc_test_macros.h` GC_TEST/GC_CHECK 范式，CMake 需 `CHAOS_IL2CPP_GC_SERVER=ON` 预设）：
- 挂在 Server 预设矩阵（plan-v6-01.md:24 CI 新增 ON 预设）下跑。测：a) `Initialize(n)` 后 HeapCount()==n；b) `ResizeHeaps(n+1)` 后旧堆 hint 不变、新堆可分配、`ForEachHeap` 遍历到新堆；c) `ResizeHeaps(n-1)` 后 removed 堆域回收、越界线程回退 heap0；d) `MaybeResize` 在人为高存活(灌 alloc)+高 threshold 配置下 grow 触发、alloc 停后 shrink 触发；e) 空转负载不触发（调参下限）。

(5) 依赖符号清单（必须 M3A 先行）见 Dependencies。
### tests
新文件 `tests/contracts/native/runtime-core/gc_dynamic_heap_count_test.cpp`，宏：`GC_TEST`/`GC_CHECK`（`gc_test_macros.h:28/48`），需 `-DCHAOS_IL2CPP_GC_SERVER=ON` 构建预设（接 plan-v6-01.md:24 CI 矩阵）。断言语义：
1. `GC_CHECK(GcHeapManager::Instance().HeapCount()==N)` 初始堆数正确；
2. `ResizeHeaps(N+1)` → `HeapCount()==N+1` 且 `GetHeap(N).old_gen` 可分配、既有堆 heap_hint 不变（hint 幂等）；
3. `ResizeHeaps(N-1)` → 被移除堆的 domain 已回收（`ReclaimHeapDomains` 返回释放 region 数>0）、映射越界线程 `HeapForCurrentThread()` 回退 0；
4. 高存活负载配置下 `MaybeResize` grow 触发（HeapCount 增）、alloc 停 + 冷却后 shrink 触发（回落到 MinHeaps）；
5. 空转低存活时 `MaybeResize` 保持堆数不变（负样本）。
验证入口：`ctest --test-dir artifacts/presets/windows-x64-reference -C Debug -R "gc_dynamic_heap_count" -L unit --output-on-failure`（plan-v5-01.md:110 同款命令）。
========================================================================
## B2 / task#7 [P1（世代写屏障正确性护栏——漏卡=潜在 UAF，属正确性，非性能）] 世代 _dst_ref 写屏障多线程多代压力验证 + kFull/poison 漏卡捕捉设计
[evidence] - 现有 K2b: gc_region_barrier_stress_test.cpp 行77-152（单 Old→Nursery 方向、位置校验行127-148、chaoas_gc_collect 全链行105-110、原根因注释行119-126）。
- 屏障逻辑: gc_card_table.h:95-116（_dst_ref 三短路）/gc_card_table.cpp:95-117；DirtyCard header:138-173；CardBundleSet header:102-114（原子 Or）。
- 世代表: gc_region.h:107-112 GetRegionGen、116-121 SetRegionGen、138 GcMarkRangeOld；kRegionGenYoung=0/Gen1=1/Old=2 行85-93。
- kFull 缺失: gc_diagnostics.cpp:66-71（"future referenced-allocation walk"）、gc_diagnostics.h:12-16/30。
- 对象存活谓词: gc_old_gen.cpp:625-635 IsMarked、gc_old_gen.h:328、gc_loh.h:84 IsMarked、gc_region.h:495 IsNurseryPointer。
- 漏卡_UAF 机制: gc_young_collector.cpp:475-506 Phase1 dirty-slot scan、行534-537 raw-nursery FastSkip、行810-811 bump 重置。
- 0xFF 释放/毒化: gc_old_gen.cpp:2363 注释、行589 DiagCountOxFFBytes、行1021-1034 sentinel 跳过。
- 收集分代: gc_api.cpp:208-278（gen0/1/全链路由）。
- CMake 自动 GLOB: tests/contracts/native/runtime-core/CMakeLists.txt:16-18。
[deps] - 复用 B2 现有基础设施：chaos_gc_dirty_card_dst_ref、GcScanAllThreadRoots、ScanDirtyCardsInPagesBatched、GcScavengeObjectKnownNursery、G_OldGen/Loh/YoungGen。
- 需一并改：gc_diagnostics.cpp/h 的 kFull referenced-allocation walk + 新增 GcVerifyCrossGenEdges()（本任务范围）。
- 依赖 GcConfig().HeapVerify knob 可注入（前置已 merge，M11）。
- 与 backend 侧：P1 前端（codegen/解释器/JIT 三层 _dst_ref 接线）已存在（A2 前提），压力验证是其验收端。
[risks] 1. race/灰区：store 与 _dst_ref 两操作非原子（stress_test:119-126 已承认）。并发 worker 写 slot 时，若 GC safepoint 落在 store 与 barrier 之间，即便 barrier 正确也可能当期漏——必须在 safepoint（RequestGlobalSafepoint+ReleaseGlobalSafepoint）内做 GC 且 worker 在 cooperative mode 下才可将窗口闭合；否则会出现"假漏卡"误报。建议参照现有测试把写 slot 与 barrier 逻辑上当作一个 atomic 段（或接受小概率误报并仅在强窗口关闭后断言）。
2. 误报：IsMarked 依赖 mark bitmap 存活期。若断言时机晚于 bitmap 清空/复用，会把已回收却未 0xFF 覆盖的 old 对象误判为存在漏卡。须在 chaos_gc_collect 返回后立即（next mutation 前）调用 GcVerifyCrossGenEdges。
3. 代归属判定歧义：nursery 重置后"地址仍 in-range"（gap_analysis A-③）——纯 IsNurseryPointer 会误判 old/已亡地址为存活。必须配合 GetRegionGen==young 与 gen1_bump 边界，避免误报。
4. verify_heap kFull 遍历放到生产 GC 边界可能增加 pause 开销；建议只读压力测试启用（skip|gc 测试专属激活），不进默认产线路径。
5. raw（无 TypeInfo）载荷的保守 slot 扫描会误把非指针字节当引用造成误报；载荷 slot 布局需固定 +4 间隔且只放真指针，或载荷用有效 TypeInfo（Nursery 对象带 m_typeInfo 头）以减少保守误扫——这是与既有 stress_test 冲突的设计取舍（现有 raw 载荷用保守路径）。
6. LOH/Gen1 分配在纯 contracts 测试环境是否已初始化（G_Loh().Init()/G_OldGen().Init 需确认入口，如 InitYoungGeneration 是否顺带 init LOH）——未实读 init 链条，推测 stress_test 能跑到 old-gen 分配故已可行；若缺需在测试前置 init。
### gap_analysis
现状与缺口（全部实读）：

【A. 现存 K2b 覆盖 —— gc_region_barrier_stress_test.cpp】
- 唯一测试 RunCrossGenRefStoreStress（行77-152）：kThreads=8 线程各自把 NurseryAllocate(128) 的 raw 对象存入共享 old-gen OldMessage.nursery_slot[128]，经 chaos_gc_dirty_card_dst_ref(g_old_slot[id], nursery_obj)（行67）写卡；主线程循环 kGcCycles=8 次 RequestGlobalSafepoint + chaos_gc_collect()（行105-110，full 链=young+gen1+old）。
- 覆盖：仅有 **Old(gen2)→Nursery(gen0)** 单一跨代方向。
- 缺口①（代）: 未覆盖 Gen1(gen1)→Nursery、Gen1→Old、Old→Gen1、Old→LOH、LOH→Nursery/Gen1、Gen1→Gen1 生存。golden 测试只有一列。
- 缺口②（代共存）: 未在同一对象图里同时出现 Nursery/Gen1/Old/LOH 四代交叉。
- 缺口③（关键）: 断言(行127-148)只做 **位置校验** —— IsNurseryPointer(行135) 或 IsInOldGen(行136) —— 位置合法≠存活。一个被 young-GC 收集的 nursery 对象地址在 nursery 重置后仍"位于 nursery 范围"，或释放的 old-gen 对象所在 page 仍 IsInOldGen；因此该测试可能把"已亡但未复用"的地址误判为 valid，**对漏卡导致的 UAF 是假阴性**。
- 缺口④: 无确定性漏卡注入/激发，仅靠 8×128 并发 + 8 GC 概率命中 store-then-barrier 窗口（行119-126 注释承认非原子窗口）。

【B. 触达漏卡的真实机制 —— gc_young_collector.cpp】
- Phase1 扫描 dirty cards（行475-506）：旧代 page/LOH segment 的 ScanDirtyCardsInPagesBatched，对每格 IsInNursery 判 nursery 引用并 GcScavengeObjectKnownNursery。漏卡=该 slot 所在 card 未 dirty→Phase1 跳过→nursery 对象未晋升→Phase4 重置 bump（行810-811）→引用悬空。这是 UAF 源（需被压力测试放大）。
- Phase2（行548-591）只扫 nursery 有效 TypeInfo 对象；无 TypeInfo 触发 FastSkip（行534-537 直接 goto phase3）——所以 stress 的 raw 对象只靠 Phase1 dirty-card 漏/不漏决定生死。**_dst_ref 漏卡→对象立死**，正是要捕捉的目标。

【C. verify_heap kFull/poison 接线现状 —— gc_diagnostics.cpp】
- GcVerifyHeap()（行66-71）：kOff 直接 return；非 off 只调 GcVerifyRegionToGenerationMap()；**kFull 的"referenced-allocation walk"注释明确标 future（行69-71）**——即 kFull 现在=kCrucial，无对象级引用完整遍历。这是"用 verify_heap kFull 捕捉漏卡"的最直接缺口。
- 对象存活判定原语已存在且可复用：MarkSweepOldGen::IsMarked(obj)（行625-635，读 mark bitmap）+ LargeObjectHeap::IsMarked + RegionManager::IsNurseryPointer + IsInGen1/g_young_gen.bump。因此参考完整性遍历可以基于这些既有谓词实现，无需改核心 GC。

【D. 释放/poison 事实】
- 释放的 old-gen 内存以 0xFF 填充（gc_old_gen.cpp:2363 注释 + DiagCountOxFFBytes 行589 统计 0xFF）；free block 首字为 sentinel TypeInfo（MarkObject 行1021-1034 跳过）。这为"毒化捕捉"提供锚点：一个已 freed 的 old-gen 目标，其首字必是 sentinel 或 0xFF，IsMarked=false。

结论：需新增一份"多线程+多代+引用完整性断言（存活级）+ verify_heap kFull 接线"的压力测试；kFull 参考遍历必须从"分区"升到"存活"。
### fix_design
精确改法（全部基于实读代码；不改任何已有文件语义）：

【1】新测试文件 gc_region_multigen_stress_test.cpp，放 tests/contracts/native/runtime-core/（CMakeLists.txt 行16 chaos_native_glob_add_tests(chaos "gc_*.cpp" ...) 已自动 GLOB，无需手改 CMake；RESOURCE_LOCK bgc 由 LOCK_SOURCES 无需改）。

【2】构造多代对象图（复用既有 API，避免新引入分配域）：
- 代锚点：用 G_OldGen().Allocate(size, /*scanning=*/true) 造 Old；g_loh 用 kLohThreshold 之上尺寸（gc_loh.h:36 默认 85KB）造 LOH（G_Loh().Allocate）；Gen1 需经 promotion 产生——先 NurseryAllocate 并 live，让年轻 GC 晋升，再用 G_YoungGen().gen1_region/gen1_bump 确认落入 Gen1；Nursery 用 NurseryAllocate。
- 四方向交叉引用载荷结构（每个都含 N 个指针 slot，指向其下代对象）：
  Old→Nursery / Old→Gen1 / Old→LOH；Gen1→Nursery/LOH；LOH→Nursery/Gen1。每个写用 chaos_gc_dirty_card_dst_ref(dst, ref) 记录。
- 祖先存活根：把每代"根对象"注册进 static root（GcRegisterStaticRootRange，复用 stress_test:98 模式），保证它们在前一 full GC 存活，使"悬空"只可能是**漏卡**造成，而非故意回收。

【3】引用完整性断言 —— 用存活级校验替代现有 K2b 位置校验（stress_test:127-148 的弱点）：
- 每一代 GC 周期末，遍历每条 cross-gen 引用 (src_slot→ref)，断言 ref 存活：
  ref 落在 nursery → RegionManager::IsNurseryPointer(ref)（gc_region.h:495）且必须仍指向未重置释放的对象——注意 nursery 重置后地址仍 in-range，需辅以 gen 判断：GetRegionGen(ref)==kRegionGenYoung（gc_region.h:107）。
  ref 在 old → G_OldGen().IsMarked(ref)（gc_old_gen.h:328 / .cpp:625）返回 true（full-GC 刚跑完，mark bitmap 为当前轮结果）；
  ref 在 LOH → G_Loh().IsMarked(ref)（gc_loh.h:84）；
  ref 在 Gen1 → 落在 G_YoungGen().gen1_region..gen1_bump 区间且其首字非 0xFF/sentinel。
- 复标记法（推荐，避免依赖 bitmap 生命周期）：GC 后统一用 G_OldGen().IsMarked + G_Loh().IsMarked 遍历，任一跨代 ref 的 target IsMarked=false = **漏卡/漏根**，直接 FAIL。若担心 bitmap 被后续操作清掉，则在 chaos_gc_collect 返回后、任何新分配前立即做。

【4】verify_heap kFull + poison 接线（核心新增）：
- 在 gc_diagnostics.cpp:66 GcVerifyHeap() 的 kFull 分支（行69 处"future"占位）实现 referenced-allocation walk：
  遍历 G_OldGen().PageList()（gc_old_gen.h:377）每 page 的 live slots（MarkObject 的 layout->pointer_count + pointer_offsets 精确/layout 缺失时退化整页 slot 扫描，mirror gc_old_gen.cpp:1071-1091 的保守 fallback），对每个非空 slot 值判断是否指向 nursery/Gen1（GetRegionGen<=kRegionGenGen1）且 target 已释放。释放的判定=old 目标 IsMarked=false 或 0xFF 字段（DiagCountOxFFBytes，gc_old_gen.h:312）或 free-block sentinel。
- 在 gc_diagnostics.h:30 加薄封装：GcVerifyCrossGenEdges()（逻辑分离，便于测试直接调）——GcVerifyHeap 内部 kFull 路径调用它。
- 毒化位：遍历 old-gen page 时读第一个字，若==free sentinel 或整块 0xFF 且仍被某 live slot 引用→报 GcVerify 校验失败（与 CheckRegionGen 相同的 LOG_ERROR 渠道，gc_diagnostics.cpp:30-34；测试 harness 以 FAIL 标记识别）。
- 测试内激活：set GcConfig().HeapVerify 或直接调 GcVerifyHeap()/新增的 GcVerifyCrossGenEdges()，在每次 chaos_gc_collect 之后调用。

【5】多线程模型（复用 stress_test worker + RequestGlobalSafepoint 协调）：
- kThreads 个 worker 写各自 old/loh 载荷的跨代 slot（并发分配 Nursery+写 _dst_ref），coordinator 在 8-16 个完整收集周期内分别以 gen=0（young only）、gen=1（young+gen1，gc_api.cpp:235-265）、gen=2/全链（chaos_gc_collect）三种方式触发，覆盖不同 condemned 集。
- 每轮 GC 后用 GcVerifyCrossGenEdges() 扫（safepoint 外，单线程）。

【6】确定性漏卡激发（提升假阴性覆盖率）：
- 可选加开一列"no-barrier 对照组"：写 slot 但不调 _dst_ref，断言 GcVerifyCrossGenEdges 必然失败——证明测试本身能捕捉漏卡（self-check of the detector）。对照组用独立 heap/标记跳过，避免污染主断言。
### tests
文件：/d/agent/chaos-il2cpp/tests/contracts/native/runtime-core/gc_region_multigen_stress_test.cpp（gc_*.cpp 前缀→CMake GLOB 自动注册 target chaos_gc_region_multigen_stress_test）。

断言语义：
1. TEST("MultiGenCrossRefIntegrity")：四代对象图建立后，逐条断言每条 cross-gen 引用 target 存活级为真（nursery: IsNurseryPointer+region_gen==young；old: G_OldGen().IsMarked；LOH: G_Loh().IsMarked；Gen1: 在 gen1_region..gen1_bump）。FAIL=dangling>0 → 漏卡。
2. TEST("YoungGcPreservesOldToNursery")：gen=0 收集多轮后，Old→Nursery 引用 target 仍存活（barrier 不漏 young 引用）。
3. TEST("Gen1AndLohEdges")：Gen1→Nursery 与 LOH→Nursery/Gen1 引用在 multiple GC 后仍存活。
4. TEST("VerifyFullCatchesMissedCard")：对照组（跳过 _dst_ref 写 slot）→ 调 GcVerifyCrossGenEdges() 必须返回失败，证明 detector 有效（self-check）。
5. 复用 gc_test_macros GC_CHECK/GC_FAIL（stress_test 一致的模式）。

依赖：GcVerifyCrossGenEdges() 的 kFull 实现（gc_diagnostics.cpp）属于本任务一并落；heap verify 级别注入用 GcConfig().HeapVerify（可用 GcConfig() 现有 knob，gc_config.h）。
========================================================================
## 9 (E1/H3/E2) [P2(方案归属 dynamic_tuning 伺服)。P1 优先用于本机 gen0 触发阈值闭环(DT-E1 空间域),BGC FL servo(H3)与 ML 后置;不触碰 P1 Server-GC 多堆/M3A 等任务(#4/#5/#6/#7)范围。] dynamic_tuning 伺服 + BGC 双伺服(FL/ML) 代码级修复设计
[evidence] 旋钮机制: gc_config.h:37-65(CHAOS_GC_CONFIGURATION_KEYS/INT_CONFIG),77-78(字段宏展开),86-92；gc_config.cpp:56-72(Initialize env 覆盖)；gc_region.cpp:575-577(latch 热路径)。hot-path 读取点: gc_scheduler.cpp:38(CooldownAllocations),63(MinGcIntervalMs),195-216(Gen1 促龄),380(FullTriggerMultiplierFP),421(YoungTriggerMultiplierFP),438(HighPressureTriggerMultiplierFP),554-557/602-605(Min/Max N/G);gc_bgc.cpp:949(MarkSliceBudgetUs)。frag 采集: gc_young_collector.cpp:908 SetOldGenFragmentation(OverallFragmentation());gc_old_gen.cpp:1562-1595(OverallFragmentation 位图扫描 (payload-live)/payload);gc_scheduler.h:452-462,465(old_gen_fragmentation_fp_)。surv 采集: gc_scheduler.cpp:78-92(young EMA),167-221(gen1 EMA+促龄+ns/byte)。gen0 触发阈值注入点: gc_scheduler.cpp:420-431(scaled_young_multiplier,现仅压力驱动)。nursery 固定: gc_region.cpp:602-615(AllocateRegion),gc_young_collector.cpp:819-823(Phase C 延迟注记);gen1 动态 resize 762-774。free-list 结构未采: gc_old_gen.h:77(free_lists[28]),564-572/pool;memory_load 非闭环: gc_api.cpp:424-439(GetPlatformMemoryStatus)。BGC servo 接入点: gc_bgc.cpp:843(外层 while 顶),949(切片预算读),585/612(cycle_complete 前)。测试惯例: tests/unit/runtime-native/runtime-core/gc/gc_scheduler_test.cpp + CMakeLists chaos_native_add_test + gc_scheduler.cpp 直链。
[deps] 依赖 M11 已落库 24 旋钮(gc_config.h INT_CONFIG 宏 + GcConfigImpl 构造期默认 + env 覆盖 + gc_region.cpp:575 latch);依赖现有 EMA 存活率(gc_scheduler.h:508/528/465 old_gen_fragmentation_fp_)与 BGC 事件驱动(bgc_cv_);依赖 GetPlatformMemoryStatus(gc_api.cpp:27)取 memory_load。本方案为 E1(空间域 dynamic_tuning)+H3(FL servo)+E2(BGC 双伺服)整体设计,落地顺序:先 1)旋钮增密 2)free-list 采集 3)gen0 预算注入 4)FL BGC,ML 严格后置。
[risks] 1. 伺服振荡→GC 过频: FL 比例增益 BgcFlMarkGainFP 过大或钳位缺失,mark_slice/gen0 预算在每轮正反馈下振荡 → GC 频次飙升、延迟高企。缓解: 严格节流(ServoMinIntervalMs 250ms)+ 输出双向限幅 + TestServoDecay 回归闸。
2. 偶发 GC 延迟: 预算收缩太快会缩短 young GC 触发阈值 → 分配压力下 safepoint 更频繁。缓解: Gen0BudgetFloorFp 兜底 + frag 权重 FragWeightNurseryFP 默认仅 0.8。
3. 撕裂读: Servo(后台/BGC 线程)写预算, DecideCollection(任意 mutator 线程)读 → 必须 atomic<float> 位图(BitsToDouble 同款,见 gc_scheduler.h:15-24/508)防数据竞争;禁止裸 float。
4. free-list rate 采集 O(n): 复刻 OverallFragmentation 全页遍历,高频调用抬 GC 尾部开销。缓解: 只在 BgcSweep 收尾采集(低频),不进 young GC 每轮。
5. BGC FL 服务器耦合: 改 slice_budget 读取点(gc_bgc.cpp:949)若 Servo 未初始化默认返 MarkSliceBudgetUs,防 null。
6. 违规风险: gc_config.h:30-32 死旋钮禁令 —— 新增 25 旋钮必须逐一接读取点,否则 hook 拦截 (pre-commit/pre-push LLM gate)。
7. 回归护栏: 全部新旋钮默认值=当前行为(frag 折扣权重=0.8s 接近现 0.75-1.0,Gen0Budget=1.0),确保 P1 基线(现有 11 个 jit/gc 全绿)不被伺服默认态破坏。
### gap_analysis
【现状-实读】M11 已有 24 旋钮机制(见下 evidence),但存在 4 个未闭环缺口:

(1) 旋钮机制分两种接入形态,已实读证实:
  - 建构造期默认: gc_config.h:77-78 宏展开为普通字段 `CHAOS_IL2CPP_SIZE NAME=(DEFAULT);`,`GcConfigImpl` 值初始化(读前恒返回默认,无 0 假值)。
  - 热路径 latch(init 一次性复制到 inline 全局): gc_region.cpp:575-577 latch `kMaxTlabAlloc/kLohThreshold/kMaxParallelMarkWorkers`(供每次分配读 machine load)。
  - GC-decision 慢路径直接读: gc_scheduler.cpp:38/63/195-216/380/421/438/554-557/602-605 直接 `GcConfig().X`;gc_bgc.cpp:949 读 `MarkSliceBudgetUs`。
  - 硬约束: gc_config.h:30-32「每个 key 必须被真实运行时路径消费(或 latch)」—— 加旋钮必须同步接读取点,否则违反规则。

(2) frag/surv「只记录不闭环」缺口(核心):
  - frag 采集点实读: gc_young_collector.cpp:908 `G_Scheduler().SetOldGenFragmentation(G_OldGen().OverallFragmentation())`;底层 `OverallFragmentation()`(gc_old_gen.cpp:1562-1595)是 O(n) 位图扫描,算 `(payload-live)/payload`(=「未用比例」含空页,非严格 free-list 空洞)。
  - frag 目前唯一消费点: gc_scheduler.cpp:543-548 在 RecommendedNurserySize 里按 `frag>0.30/0.50` 打 0.75/0.5 折扣 → 只影响 nursery 大小,**不喂 gen0 触发阈值**。
  - surv(EMA)已实读: gc_scheduler.cpp:78-92(young EMA)、167-221(gen1 EMA+促龄);消费于 RecommendedNurserySize(505-527)+ promotion_age(127-135/186-220),**不喂 gen0 触发阈值**。
  - gen0 触发阈值注入点已定位: DecideCollection 的 `scaled_young_multiplier`(gc_scheduler.cpp:420-431),现仅由压力(memory pressure_ratio 与 FullTriggerMultiplierFP)线性缩放,悬空于 frag/surv。
  - memory load: 仅在 gc_api.cpp:424-429 用 GetPlatformMemoryStatus 临时算(high_memory_load_threshold=total_phys/2 等),**非闭环**——dynamic_tuning 的「memory load→gen 预算」没有伺服回路。

(3) 共享 nursery REGION 尺寸 init 后固定: gc_region.cpp:602-615 按 DefaultNurserySize/DefaultGen1Size 一次性 AllocateRegion;gc_young_collector.cpp:819-823 注释明确「动态 nursery resize 推迟到 Phase C;gen1 为独立定区」。故 RecommendedNurserySize 只影响触发阈值(经 last_nursery_used_)、不适配共享 region 尺寸。Gen1 已有动态 resize(762-774)。

(4) free-list rate 完全未采: OldGenPage.free_lists[28](gc_old_gen.h:77)、page_pool_(571) 存在,但无「自由表率/碎块密度」采集器。此为 CoreCLR dynamic_tuning 的碎片密度项,缺口真实。
### fix_design
【精确改法 — 新增 src/native/runtime-core/gc/gc_dynamic_tuning.h/.cpp(仿 gc_diagnostics 单例),并新增 gc_bgc_servo.h/.cpp】

== 1) 旋钮 24→~50,新增旋钮与读取点(全部进 CHAOS_GC_CONFIGURATION_KEYS,gc_config.h:37) ==
对拍 CoreCLR dynamic_tuning.cpp 语义(自由表率/碎片密度/memory load→gen 预算),新增:
  - FragTargetFP(默认 300=0.30): 目标老代碎片;读于伺服 Update 每轮。
  - SurvTargetFP(默认 300=0.30): 目标 young 存活率;读于伺服。
  - FragWeightNurseryFP(默认 800): frag→预算折扣;读于 gc_scheduler.cpp:543 处替换硬编码 0.30/0.50。
  - Gen0BudgetFloor/Gen0BudgetCeilFp(默认 500/3000): 伺服输出的 gen0 预算倍率钳位;读于 DecideCollection:420 处。
  - FlRateTargetFP(默认 500): 目标 free-list 率;读于伺服。
  - ServoMinIntervalMs(默认 250): 伺服节流间隔;读于伺服主循环。
  - ServoSteeringP(默认 10,存为 fp*1000=0.01): FL 反馈线性比例增益;读于伺服。
  - BgcFlMarkGainFP(默认 50)=0.05: BGC FL servo 标记切片调整增益;读于 gc_bgc.cpp:949 附近。
  - BgcFlMaxSliceUs/BgcFlMinSliceUs(默认 8000/250): FL 输出切片钳位;读于 servo。
每加一个都同步在 gc_config.cpp:77 启动 LOG 补位(保持 24→~50 全可观测)。latch 型不加(全是 decision-time 慢路径读)。

== 2) 伺服状态结构 + 采集点 ==
新增 `namespace chaos::il2cpp::runtime_core` 下 `struct GcServoState`(单一结构,全部 std::atomic,放 gc_dynamic_tuning.h):
  - `atomic<float> free_list_rate` : 老代自由表率(新增采集器)。
  - `atomic<float> frag_density`   : 复用 G_Scheduler().OldGenFragmentation()(读点 gc_scheduler.h:459-462)。
  - `atomic<float> memory_load`    : 新闭环保留,采集于 gc_api.cpp:429 的 GetPlatformMemoryStatus 处,顺手 memcpy 进 state。
  - `atomic<double> gen0_budget_fp`(fp*1000 输出倍率)。
  - `atomic<uint64_t> last_update_ns` : 伺服节流。
单例 `GcServo& GcDynamicTuning()`(仿 GcConfig g_config 静态,见 gc_config.cpp:16-18)。
采集点(各一层封口):
  - free-list rate 采集: 在 MarkSweepOldGen 新增 `float FreeListRate() const`(同 OverallFragmentation O(n) 位点,gc_old_gen.cpp:1562 同法遍历 free_lists 链长和),在 gc_old_gen.cpp BgcSweep 收尾(~3272 区域)与 young_collector:908 同处喂给 Servo。
  - frag: 直接复用 SetOldGenFragmentation 已 store 的 old_gen_fragmentation_fp_(gc_scheduler.h:456)。
  - memory load: gc_api.cpp:427-429 处同时喂 Servo。

== 3) 反馈闭环:指标→gen0/nursery/gen1 预算/触发阈值(对齐 dynamic_tuning 语义) ==
核心注入点 = DecideCollection 的 `scaled_young_multiplier`(gc_scheduler.cpp:422-427),现改为三步:
  a) 关停硬编码压力缩放,改为 `gen0_multiplier = Gen0BudgetFp/1000`(从 Servo 读)。
  b) 把 frag 并入: `gen0_multiplier *= (1.0 - FragWeightNurseryFP/1000 * frag)` (对拍 dynamic_tuning「碎片密度高→gen0 预算降→更早 YOUNG」)。
  c) `scaled_young_multiplier = clamp(gen0_multiplier, Gen0BudgetFloorFp/1000, Gen0BudgetCeilFp/1000)` 替换 425-427 行。
  - RecommendedNurserySize(496) 的硬编码 frag 折扣(543-548)改为读 FragWeightNurseryFP,删除散落常量。
  - Gen1: 把 RecommendedGen1Size(574) 输出乘 `(1 + (1-avg_gen1_survival)*Gen1SurvScaleFp)` 由 gen1 EMA(gc_scheduler.h:528)驱动,喂给 762-774 的 ResizeGen1Region。此为「surv→gen1 预算」闭环,现只做阈值不做预算。
  - 闭环自洽: Servo 读 frag/surv/fl_rate/memory_load → 产 Gen0BudgetFp → DecideCollection 消费;输出节点每轮钳位+节流,防止振荡。

== 4) BGC 双伺服(FL 先落地,ML 后置) —— FL 伺服状态与更新 ==
新增 `BgcServo`(gc_bgc_servo.h): 状态 = `{ atomic<int> mark_slice_us, atomic<uint32_t> cycle_count, atomic<uint64_t> last_cycle_ns, atomic<float> last_cycle_cost_ns }`。
FL 更新(通用反馈线性 y = P*(setpoint−y)):
  ```
  FL_BGC(stuck_measure):  // cycle 内并发标记吞吐 = bytes_marked / elapsed
     if now-last_cycle_ns < ServoMinIntervalMs: return
     error = BgcFlTargetThroughputFp/1000 − throughput
     adj = BgcFlMarkGainFP * error            // 限幅 ±30%
     mark_slice_us = clamp(mark_slice_us ± adj, BgcFlMinSliceUs, BgcFlMaxSliceUs)
  ```
  触发时机: 在 BgcThreadMain 外层 while 顶部(gc_bgc.cpp:843 处,phase-IDLE 每次醒来)、及 CONCURRENT_MARK 切片循环 gc_bgc.cpp:949 读 `mark_slice_us` 替代 `GcConfig().MarkSliceBudgetUs`(用 Servo 产出值)。
  采集度量: 记录并发标记刚完成的 bytes_marked/循环耗时,写在 cycle_complete_=true(gc_bgc.cpp:585)前。
ML 后置: 预留 `BgcServo::UpdateMl(...)` 壳(空实现),依赖数据积累(fl_rate/frag/memory_load 时间序列)后,再演化成轻量回归/阈值分类,当前不接。

== 5) 新增 25 旋钮清单(24→~50)与读取点 == 见「1」表,均 decision-time 读、无新增 latch。

== 6) 测试 gc_dynamic_tuning_test ==
新文件 tests/unit/runtime-native/runtime-core/gc/gc_dynamic_tuning_test.cpp + gc_scheduler.cpp 链路 + gc_bgc_servo.cpp 链路(仿 CMakeLists test_gc_scheduler 加 chaos_native_add_test(test_gc_dynamic_tuning, gc_dynamic_tuning_test.cpp, 补 gc_scheduler.cpp/gc_bgc_servo.cpp))。断言:
  1) Servo 无输入时 Gen0BudgetFp 恒 1.0(不改变现有触发) — 回归安全闸。
  2) 喂 frag=0.6 → gen0_multiplier 单调下降且 clamp 在 [Floor,Ceil]。
  3) 喂 free_list_rate 上升 → fl_rate_ 指标更新且下次 YOUNG 触发提前(alloc 阈值变小)。
  4) FL_BGC: 构造吞吐低场景 → mark_slice_us 增加;高→减;输出 clamp 在 [Min,Max]。
  5) 节流: 连续调 2 次 100ms 内只更新一次(ServoMinIntervalMs 生效)。
  6) decay: 停止输入 5 轮后预算回归默认(无死锁/无振荡)。

== 7) 风险 == 见 risks。
### tests
gc_dynamic_tuning_test.cpp(GoogleTest,仿 gc_scheduler_test.cpp):
 - TestServoNoOpDefaultBudget: 空输入时 Gen0BudgetFp==1.0,DecideCollection 行为回归不变(安全闸)。
 - TestFragShrinksGen0Budget: SetOldGenFragmentation(0.6) 后 gen0_multiplier<1.0 且 ≥Gen0BudgetFloorFp。
 - TestFreeListRateFeedsTrigger: 注入 free_list_rate 上升,RecordAllocation 达更小阈值即触发 YOUNG。
 - TestFlBgcAdjustsSlice_LowThroughput: 构造低吞吐 → mark_slice_us 上升且 clamp≥BgcFlMinSliceUs。
 - TestFlBgcAdjustsSlice_HighThroughput: 高吞吐 → mark_slice_us 下降。
 - TestServoThrottleInterval: 两次 Update 距 < ServoMinIntervalMs 只算一次。
 - TestServoClamp: Gen0Budget 与 mark_slice 均钳位不越界。
 - TestMlStub: UpdateMl 空调用不崩溃(占位)。
附 gc_scheduler_test(RECOMMENDED) 现有断言补: 前置 Servo 状态复位(RESET_FOR_TEST 已提供如 RecordGcCompleted)。
========================================================================
## G4 [P2（验证正确性/调试可达性；不动 GC 主链路语义）] Verify 深度强化：分级 HeapVerify + poison + 晋升 tracked 一致性(P1-A3)
[evidence] gc_diagnostics.cpp:15-20=kOff/kCrucial/kFull 分级; :66-71=GcVerifyHeap 空 kFull(:69-70 future walk); :73-88=GcVerifyPromotedTracked 现断言(只验 IsInOldGen+OLD, 误框gen1)
gc_diagnostics.h:12-16=HeapVerifyLevel 枚举; gc_config.h:46=HeapVerify 0/1/2
gc_young_collector.h:31-54=YoungCollectionResult(bfs_worklist, bfs_worklist_count, condemned_gen_num)
gc_young_collector.cpp:290-308=晋升分流(gen0→Gen1, gen1存活留Gen1), IsInGen1语义; :136-143=IsInGen1Local; :226-241/:336-351= worklist enqueue; :747-748=gen1_result只并计数不入worklist; :925-933=出口(EtwFireGcYoungEnd/bgc resume/return)
GcVerifyPromotedTracked 唯一调用=gc_region.cpp:1095(遗留 PromoteNursery, :1086)
GcVerifyHeap 仅=gc_region.cpp:1414/1458(非Server的 chaos_gc_collect)
GcYoungCollection 主路径调用=gc_region.cpp:250/266/411/1423, gc_api.cpp:243/273, gc_coordinator.cpp:57 —— 全无 GcVerifyPromotedTracked
IsInOldGen=old_gen.cpp:525 二元搜 page_array_; GetRegionGen/SetRegionGen/GcMarkRangeOld=gc_region.h:85-133(kRegionGenShift=22, 1字节/4MB)
页面布局=old_gen.h:38-48(64KB页, size class 16..32768最小16); old_gen.cpp:210-254(carve_free_lists满切到最小16B, 现有 bitmap 16B 0xCD poison:214-218)
测试=tests/contracts/native/runtime-core/gc_diagnostics_test.cpp(现4用例, TestPromotedTracked只测空worklist:65-74)<span style="word-break:break-all">
[deps] P1-A2b（young-GC 晋升回写深因，task #6）；P1-B2（世代写屏障压力验证，task #7）——屏障/晋升路径稳定后本验证才无干扰；GcGen1Collection 的晋升产物覆盖为可选项（是否要全量）。IsInGen1 语义决定 #2 分支正确性，先锁定。
[risks] 1) gen1 晋升产物不在 bfs_worklist（gc_young_collector.cpp:747-748 只并入计数），#2 对 gen1 的“合法分支”只覆盖来自 worklist 的地址；真实 gen1-only 晋升仍不被 GcVerifyPromotedTracked 覆盖——若预期“全覆盖”需另通 gen1 页，否则存在声明/实际偏差，文档须明确。
2) #2 重写不改断言强度但改“无条件 IsInOldGen 通过才不报错”的语义：若实现误判 gen1 地址合法性，会让本应报错的“越轨晋升进 gen1”变静默。IsInGen1Local 是 local static（gc_young_collector.cpp:136），跨 TU 调用需提为公开/或内联重复，注意符号可见性。
3) poison 若走“每对象 prefix”会与 type-info first-word（gc_young_collector.cpp:97 读首字）冲突，本设计回避；但页尾 tail_poison 需要保证不被后续 size-class 分发踩到（最小 16B 对齐），插入点必须在 carve_free_lists 之后且改写 page->payload_size 大小关系。
4) kFull 做整页 marked-object walk 是 O(对象数) 冷路径，大堆（LOH 大段）下 pause 侧写可能显著；依赖 kFull 仅在调试/CI（CHAOS_GC_HeapVerify=2）启用，严禁进默认（默认 0 已是最优，gc_config.h:46）。
5) Server GC 接线（GcCoordinator）补 Exit 校验需确认 BGC 并发窗口不会让“正在晋升的中间态”被误报——STW 语义保证 young GC 内无并发，但 Resume 后立即查可能不可靠，验证点应放在 STW 内。
### gap_analysis
【现状盘点 - 全部实读】

1. 分级已存在但 kFull 是空壳：
   - gc_config.h:46 定义 HeapVerify INT 0/1/2；gc_diagnostics.cpp:15-20 映射 0→kOff, 1→kCrucial, ≥2→kFull。
   - gc_diagnostics.cpp:66-71 `GcVerifyHeap()` 只调 `GcVerifyRegionToGenerationMap()`（kCrucial 那档）；kFull 分支注释明确写 "future referenced-allocation walk"（:69-70）——**目前完全没实现**。即 kFull(2) 与 kCrucial(1) 现在行为等价，分级无梯度。

2. 接入点断裂（关键缺口）：
   - `GcVerifyPromotedTracked` 只有在 gc_region.cpp:1095（`RegionManager::PromoteNursery`，遗留路径，:1086）被调一次。主收集路径 `GcYoungCollection` 调用处 —— gc_region.cpp:250/266/411/1423、gc_api.cpp:243/273、gc_coordinator.cpp:57 —— **无一调用** GcVerifyPromotedTracked。P1-A3 断言在真实 young-GC 流程里是死的。
   - `GcVerifyHeap` 只在 `chaos_gc_collect()` 的**非** `CHAOS_IL2CPP_GC_SERVER` 分支（gc_region.cpp:1414/1458）Entry/Exit；Server GC（`GcCoordinator`，:1411）以及 allocation-triggered young GC（gc_region.cpp:250/266）完全不验证。

3. 晋升断言有个**现有误框**：gc_diagnostics.cpp:73-88 断言每个 bfs_worklist 元素 `IsInOldGen(p)` 且 region-gen==OLD。但晋升目标不总是 old-gen：gc_young_collector.cpp:290-308 `GcScavengeObjectKnownNursery` 按源 gen 分流，gen0 对象 TryAllocateInGen1()（:304）到 gen1（region-gen=Gen1(1)），gen1 存活且 threshold>1 也留 gen1（:296）。IsInOldGen（gc_old_gen.cpp:525，二元搜 page_array_）对 gen1 地址返回 false，region-gen 也≠OLD。即当前断言在“晋升到 gen1 的普通路径”会**误报失败**。

4. poison：已有的是**位图 poison**（gc_old_gen.cpp:214-218，bitmap 后 16 字节 0xCD）用于防溢出写——但**无对象级 / 页面级 poison 校验入口**，kFull 也未消费它。页面尾部无保留 poison 槽：carve_free_lists 把整块 payload 切到最小 16B size class（gc_old_gen.cpp:223-254 + gc_old_gen.h:42-46），尾部仅剩 <16B 缝隙。
### fix_design
【精确改法】

1) kFull 分级补强 —— gc_diagnostics.cpp `GcVerifyHeap()`：
   - 把现有 GcVerifyRegionToGenerationMap 保留为 kCrucial；kFull 新增“referenced-allocation 抽查 walk”：遍历 G_OldGen().GetPageArray()（gc_old_gen.h:300-308），对每个 in_use 页面，用 GcMarkBitmap.TestSlot 筛出 marked 对象首址，校验：(a) region-gen==OLD；(b) IsInOldGen 命中；(c) first-word 是合法 TypeInfo（GcLayoutRegistry::IsValidTypeInfoPointer）。O(n objects) 冷路径，仅 kFull。
   - kFull 还需消费既有位图 poison：对每页 bitmap 后 16 字节（kBitmapPoison=16，gc_old_gen.cpp:216）校验仍为 0xCD，异常即 LOG_ERROR——把已有 poison 从“写后无人读”变成“每 kFull 校验”。

2) 晋升产物 IsInOldGen 断言 —— 重写 gc_diagnostics.cpp `GcVerifyPromotedTracked`（先修误框再做一致性）：
   - 按目标地址分类校验（不能用无条件 IsInOldGen）：对每个 bfs_worklist[i] 用 GetRegionGen 取 gen；
     * gen==OLD  → 断言 IsInOldGen(p)==true（否则即“晋升进未跟踪 old 页”，P1-A3 要抓的回归）；
     * gen==Gen1 (1) → 断言 IsInGen1 语义：地址落在 G_YoungGen().gen1_region[begin, gen1_bump)（gc_young_collector.cpp:136-143 的 IsInGen1Local）；
     * 其它/gen0 → 报错（晋升产物不该落 gen0/nursery）。
   - 现有 gen2 分支的 IsInOldGen+region-gen 校验保留，但新增“gen1 合法分支”，消除误报。

3) 挂 GcYoungCollection 出口：#6 精确改法。
   - 在 GcYoungCollection 末尾（gc_young_collector.cpp:931 return 之前的 `return result;` 处，此时 result 已含 BFS worklist 全量）插 `if (GcGetHeapVerifyLevel() >= HeapVerifyLevel::kFull) GcVerifyPromotedTracked(result);`。应插在 :926 GcEtwFireGcYoungEnd / :929 bgc resume 附近、:933 return 前，语义上“所有 promotion 完成且 region-gen 已定型”。
   - 需要 `#include "gc_diagnostics.h"` 到 gc_young_collector.cpp 顶部（:16 现不含）。
   - 注意 gen1 聚合：Phase4 GcGen1Collection 把 gen1_result 并入 result.objects_promoted（:747-748）但**不并入 bfs_worklist**——gen1 晋升产物不在 worklist，校验范围需在文档注明；若要求覆盖则需单独通行 gen1 页（或另开 GcVerifyPromotedTrackedForGen1）。建议本期先只验 worklist，gen1 单列后续。

4) poison marker（对象/页面尾部）—— 对齐 CoreCLR 的 debug 分配头/填充，不动热路径：
   - 页面级：在 `OldGenPage` 追加一行 `uint8_t tail_poison[16]`（gc_old_gen.h:63），`carve_free_lists` 之后（gc_old_gen.cpp:254）写 0xCD；全页回收/Free 前校验≠0xCD 则报错（抓越界写）。最小 size class=16 与尾槽对齐，不侵入已切分区域。
   - 也可复用现有 kBitmapPoison（16B 0xCD）作为首个已在位的“页面 guard”，kFull 只做读校验即可零新增分配。
   - **建议**：本期以“读校验既有位图 poison + 页尾单行 tail_poison”两层为主，不做每对象 0xCD prefix（改 Allocate 布局侵入大、且与 type-info first-word 冲突）。

5) 测试 & CHAOS_GC_HeapVerify=2 stress：见 tests 栏。

6) 接线缺口一并修：Server/主路径也补 GcVerifyHeap —— 在 GcCoordinator 的 GC 完成点（gc_coordinator.cpp:57 附近 GcYoungCollection 返回后）与 chaos_gc_collect Server 出口加 Exit 校验；allocation-triggered young GC（gc_region.cpp:250/266）已在其内部经 #3 出口校验覆盖，无需重复。
### tests
tests/contracts/native/runtime-core/gc_diagnostics_test.cpp 增测：

1) 误框回归测试（必须第一个保证本地跑得通）：构造一个伪 YoungCollectionResult，worklist 放一个 gen1 地址（GcMarkRangeOld gen1 语义或 SetRegionGen 到 Gen1），断言 GcVerifyPromotedTracked 不报错（验证 #2 的 gen1 合法分支）；再放一个“region-gen=OLD 但不在 page_array_”的假地址，断言其触发 LOG_ERROR。二者证明断言能区分合法/越轨。

2) kFull 分级：setenv("CHAOS_GC_HeapVerify","2") 后 cfg.Initialize()，断言 GcGetHeapVerifyLevel()==kFull 且 GcVerifyHeap() 对干净/未初始化堆不崩（扩展现有 TestVerifyLevel/TestUninitializedVerify，gc_config 已支持 env 覆盖，gc_config.cpp:62-72）。

3) poison 校验：复用已有 kBitmapPoison（gc_old_gen.cpp:216），手动翻转其中 1 字节后断言 kFull verify 报错；恢复后断言命中 0xCD 静默。

stress 验证步骤（CHAOS_GC_HeapVerify=2）：
- 起 stress：`CHAOS_GC_HeapVerify=2` 环境变量下跑 gc_general_stress_test / gc_soak_test / gc_region_barrier_stress_test（tests/unit/runtime-native/runtime-core/gc/ 下），确认 OLD-gen 晋升均通过（无 P1-A3 log）。
- 回归对比：同 stress 在 HeapVerify=0 跑一遍，确认指标（objects_promoted/pause）与 =2 一致（验证无性能级差异、验证明开关不影响语义）。
- 负向注入：故意把某 promoted 对象 page 从 page_array_ 摘除（或 SetRegionGen 改 OLD→young），观察 GcVerifyPromotedTracked 必报 LOG_ERROR——证明断言真实可触发。
========================================================================
## 6 [P1(正确性, 深 GC 手术)。属"晋升对象被后续全GC回收"的 use-after-free 类, 触发即悬垂, 应 P1 处理。方案B为 P1 保正确最小改法(改 Collect 静态根回调+mark 推进), 方案A 精确化不构成 P1 拦路(受硬约束判死, 属 P2 协议重构)。] young-GC 晋升回写深因(A2b): 全GC旧代mark未推进存活raw-struct(OldMessage)引用 → 方案B修Collect保守推进, 方案A受布局硬约束判死
[evidence] 读实证据:
● 测试 OldMessage 定义: tests/contracts/native/runtime-core/gc_region_barrier_stress_test.cpp:51-53 `struct OldMessage{ void* nursery_slot[kObjsPerThread=128]; }`; 分配 L87-92 `G_OldGen().Allocate(sizeof(OldMessage)+16, /*scanning=*/true)` + `std::memset(old,0,...)`; 注册静态根 L98 `GcRegisterStaticRootRange(g_old_slot,sizeof(g_old_slot),0)`; DIAG 后验 L127-146。
● chaos_gc_collect 跑 FULL 旧代 mark-sweep: gc_region.cpp:1409-1461 (L1421 年轻GC, L1451 `G_OldGen().Collect(nullptr,nullptr)`)。
● MarkObject 要求首字合法 TypeInfo: gc_old_gen.cpp:1024-1041 (`IsValidTypeInfoPointer→ReadStableId→Lookup; 否则 return false`)。
● TryMarkRoot 先 IsValidManagedObject 再 MarkObject: gc_old_gen.cpp:2922-2942 (L2935)。ScanRangeForRoots 同: L2944-2970 (L2963)。
● IsValidManagedObject = 首字指向已注册 TypeInfo 范围: gc_layout.h:304-313 L308 读首字 L312 IsValidTypeInfoPointer。
● SweepPage 回收未 mark slot: gc_old_gen.cpp:1364-1392 (free_run 把未标记连续区并入 free list)。
● 布局数组定长32: gc_layout.h:41 `kGcLayoutMaxInlinePointers=32`, L65 `GcPointerOffset pointer_offsets[32]`; gc_layout.cpp:158-159 将 pointer_count clamp 到 32。无 chained overflow 实装(L40 注释提及"chained overflow"但无对应存储/扫描实现)。
● 写屏障正确卡: gc_card_table.cpp:95-117 (dst_gen=old(2), ref_gen=young(0) → DirtyCard)。
● 年轻GC Phase-1 基于值扫描+回写(非布局): gc_young_collector.cpp:467-506 (L478-488 逐8字节取 val, IsInNursery→GcScavengeObjectKnownNursery→*ptr_slot=tenured)。OOM/timed_out 时 GcScavengeObjectKnownNursery 返回 nullptr(L268-270, L310), Phase-1 仅 `if(tenured!=nullptr)` 才回写(L483/L500), 否则旧址残留。
● 区域世代标签: gc_region.h:85-93 (Young=0, Gen1=1, Old=2); IsNurseryPointer 用 [begin,end) 全区间(gc_region.cpp:866-871)。

诚实标注: 我不能执行 GC; 212 个 g_DIAG-DANGLE 实测归零与否未验证(任务为只读设计)。"DIAG 读到旧代 free block 中的残留 nursery 指针"为推断(past 读到的代码强支持, 未实测); "OldMessage 被 SweepPage 回收"为推演自 5+6; "年轻Phase-1写回漏执行"在 OOM/timed_out 分支为读实(代码存在), 是否为 212 的主因未实测。
[deps] 依赖 P1-A2b 已有静态根注册(gc_static_roots.cpp, 已合入); 依赖 GcLayoutRegistry 现有 Register/RegisterTypeInfoRange/ScanObjectPointers(gc_layout.cpp/h); 与 B2(世代写屏障压力)共享测试文件 gc_region_barrier_stress_test.cpp; 与 G4(Verify 强化)共享 IsValidManagedObject 语义。若采用方案B(全GC用布局扫存活旧对象引用), 不依赖注册布局, 可先行落地。
[risks] 1) 保守扫扩大 mark 范围 → 可能把 free block / 非对象槽误 mark(现有 sentinel+IsSentinelStableId 防护仅覆盖有 TypeInfo 对象, gc_old_gen.cpp:1034); 需保守分支仍做 FindPage 范围守卫, 避免扫越旧页。
2) DrainMarkStack 无效TypeInfo分支扩展对 nursery 引用推进时, 依赖 FULL GC 已先扫各 TLS nursery 存活集(Collect L2477-2516), 时序耦合需验证; 若扫顺序不当可能漏/重复推。
3) 方案B 只对 mark-reach 的旧对象推进, 若某存活旧对象本身未被任何根触及(仅内部引用它), 保守推进仍救不了——与现存的根扫描完整性耦合, 属既有根系缺口, 非本修复唯一目标。
4) 并行 mark(DrainMarkStackParallel, gc_old_gen.cpp:1159+)同样需同步改; 其 per-slot IsValidTypeInfo 检查(L1212)会跳过无 TypeInfo 对象, 需并行版也补保守推进。
5) 改变存活判定语义会影响 finalizer/weak-handle Phase(3/3b/3c)对 IsMarked 的依赖(gc_old_gen.cpp:2646-2674), 需回归。
6) 方案A 若被误选, 因 layout 定长32+首字 TypeInfo 硬约束会改出不可编译/不可发现的对象, 除非先做协议重构(超出本任务)。
诚实标注: 上述风险基于读码推演, 未经执行验证。
### gap_analysis
现状: FULL-GC mark 以"对象首字是合法 TypeInfo"为存活判定(IsValidManagedObject→MarkObject 链), raw无类型 struct(OldMessage void*[128])无法被识别/标记。缺口=存活旧对象的引用树不会被 mark 推进: 静态根注册(gc_static_roots.cpp:81-83)把旧对象地址交给 TryMarkRoot, 但 TryMarkRoot 先 IsValidManagedObject(false) 即拒, 对象整树不被 mark, SweepPage 回收 → DIAG-DANGLE。年轻GC Phase-1(值驱动+卡驱动, gc_young_collector.cpp:467-506)对 dirty card 内的 OldMessage 槽能正确晋升回写, 但 OOM/timed_out 时 tenured==nullptr 分支不回写(L483/500)是次级缺陷。方案A受"首字即TypeInfo+pointer_offsets 定长32"双约束(gc_layout.h:41,65;gc_layout.cpp:158-159)结构判死; 方案B在现有 DrainMarkStack 无效TypeInfo分支已有保守扫雏形(L1071-1090), 扩展方向可行。diag 语义: IsNurseryPointer 用 [begin,end) 全区间(gc_region.cpp:866-871), 故 region_gen=0∧IsNursery=0∧IsOoG=0 指向已被回收的 nursery/旧对象 free block 内的残留指针, 与"旧代 mark 缺口→回收 OldMessage"吻合。
### fix_design
精确确认(1): 读码判定根因=旧代 mark 未跟进存活 raw-struct(OldMessage)计数占主导, 而非 Phase-1 tenured==nullptr 分支。理由(读实): �ич MarkObject/TryMarkRoot/ScanRangeForRoots 全部要求对象首字是合法 TypeInfo(gc_old_gen.cpp:1024-1037, 2935, 2963), 而 OldMessage 首字是 nursery 指针且被 memset 为 0(gc_region_barrier_stress_test.cpp:51-53,90); IsValidManagedObject(gc_layout.h:304)必然 false → OldMessage 在 FULL GC(chaos_gc_collect→Collect)中永不 mark → SweepPage(gc_old_gen.cpp:1364)回收 → g_old_slot 悬垂, 其 nursery_slot 残留指针即 DIAG region_gen=0∧IsNursery=0∧IsOoG=0(gc_region.h:85; IsNurseryPointer 全区间)。Phase-1 回写漏执行仅在 OOM/timed_out 分支(gc_young_collector.cpp:483/500), 测试堆小不是主因。故首选修复=方案B(全GC按布局扫存活旧对象引用), 方案A受"布局定长32+TypeInfo必须在首字"双重硬约束被阻断(见下), 无法单独落地。

方案A(测试用 GcLayoutRegistry 注册 OldMessage layout 扫 pointer offsets 0..127): 读实判定【结构不可行】。两处硬阻塞: (a) kGcLayoutMaxInlinePointers=32(gc_layout.h:41), pointer_offsets 定长[32](L65), Register clamp pointer_count 到32(gc_layout.cpp:158-159), 128 槽无法装下; (b) 更根本: 当前扫描协议要求对象首字=TypeInfo 指针(IsValidManagedObject,gc_layout.h:304; MarkObject,gc_old_gen.cpp:1024; DrainMarkStack,L1067-1071), 而 OldMessage 偏移0就是真实数据(first nursery ref), 不存在放 TypeInfo 的头部空间, memset 到0后就无法靠布局被发现。结论: 方案A需先改"首字即 TypeInfo"协议(把 layout 锚点从首字挪到对象头槽/页内索引)才可能, 属大手术, 超出本测试任务。

方案B(MarkSweepOldGen::Collect 用布局扫存活旧对象引用): 推荐落地。核心是让 FULL GC mark 对"无 TypeInfo 但已知 layout(或保守扫)的存活旧对象"跟进其引用边。具体精确改法:
  1) gc_old_gen.cpp `Collect` 新加 Phase 1.5(静态根之后, 即 L2564 之后、Phase2 之前): 对 register 的静态/裸根指向的对象做 `ScanRangeForRoots` 语义的"容器扫描", 而非仅 TryMarkRoot(它要求 IsValidManagedObject)。拆出内联核 `TryMarkRootConservative`: 越过 IsValidManagedObject 检查, 若 val 在旧页 FindPage!=nullptr 且未 mark 则标记+入栈(保留 MarkObject 的 TypeInfo/sentinel 防护对无布局对象放宽到保守扫)。扫描方向=存活旧对象引用边 → 推进到其 nursery/gen1 引用对象(这些对象由 FULL GC 的 TLS nursery 根扫描已按范围推进, 见 Collect L2477-2516)。
  2) 对满足 TryMarkRoot 且首次 mark 的对象, 在 DrainMarkStack/DrainMarkStackParallel 的 TypeInfo 无效分支(现 L1071-1090 已保守扫 whole-object)保持现有保守扫; 对"无 TypeInfo 的旧对象"不可用扫描, MarkObject 仍 false 会跳过——需在该 fallback 分支把"未 mark 引用到 nursery 的对象"也记为存活(参考 CoreCLR 从存活对象引用推进语义: 存活对象经 4MB 世代+card 已可识别, 非仅首字)。
  3) 最小改法(推荐, 防过度): 仅把静态根回调从 `TryMarkRoot` 换成 `TryMarkRootConservative`(gc_old_gen.cpp:2555-2560), 使 g_old_slot 的 OldMessage 被 mark; 再在 DrainMarkStack 无效 TypeInfo 分支(已保守扫 whole-object, L1081-1089)把对 nursery 的引用 FindPage==nullptr 也推进标记其 gen0 目标(借 FULL GC 已扫的 TLS nursery 存活集)。定位在旧对象引用推进, 对齐 CoreCLR mark-from-live-objects。

选 A/B/组合的推荐理由: 选方案B为主。CoreCLR 语义=mark 是从根+存活对象堆引用推进, 只要旧对象存活其引用槽(无论有无 GC-info)都构成边; 现实装以"IsValidManagedObject"为存活前提, 让无 TypeInfo 的 raw-struct 整树失联, 是结构性缺口。方案A被"首字TypeInfo+定长32"硬约束判死, 不应作为主选; 若日后要做精确, 需先引入"对象头旁挂 layout 锚"(非首字)协议(属独立里程碑), 届时方案A复为可选项。组合=先在方案B落地保正确(CoreCLR 保守推进), 方案A的精确化为远期。
### tests
回归判据(DIAG-DANGLE 归零): 复用 tests/contracts/native/runtime-core/gc_region_barrier_stress_test.cpp 后验(L127-146)的 `dangling==0` GC_CHECK(L146/148) + 新增断言:
1) 主判据: `GC_CHECK(dangling == 0, "cross-gen barrier: 0 dangling references after GC")` 在 20x 循环(fix 后把 kGcCycles=8 扩到 20 或外层重跑)下归零; 现行为 212/thread7 不合格。
2) mark 完整性判别: 新增 `GC_CHECK(G_OldGen().IsMarked(g_old_slot[t]) || <存活判定>, "...")` 语义=Full-GC(chaos_gc_collect 退出, gc_region.cpp:1458 GcVerifyHeap 可加钩子)后 OldMessage 仍存活, 证明方案B保守 mark 生效。
3) 引用边推进判别: post-Full-GC 后 `for i: ref=g_old_slot[t]->nursery_slot[i]` 若 ref 是 nursery 地址须 IsNurseryPointer(ref)==true(证明其 gen0 目标被 mark 存活), 防"OldMessage 存活但其 nursery 目标被回收"的同根次类。
4) DIAG-DANGLE 判断函数放测试堆内(honest: 现有 DIAG 在 stress 测试后验, 不在 GC 源码), 输出 `dangling` 计数; 判据=0 且 20x 稳定。
新增/断言文件: tests/contracts/native/runtime-core/gc_region_barrier_stress_test.cpp(扩 cycle + 增断言); 如需单元级精确识别 old-gen-mark vs young-write-back, 在 tests/unit/runtime-native/runtime-core/gc/gc_young_collector_test.cpp 走 K2c 类 snapshot 判布局扫。
========================================================================
## 4 [P1] Server GC 多堆集成 (M3A-2/3) 代码级修复设计
[evidence] src/native/runtime-core/CMakeLists.txt:239(OFF),260-261(at_server def = SERVER=1);gc_features.h:38-44(默认 OFF);gc_coordinator.cpp:23-39(RequestGlobalGc WKS 空跑/SERVER 转 ExecuteMultiHeapGc)、43-89(ExecuteMultiHeapGc 无 finalizer/record/verify)、68-79(每堆 Collect 并行)、83(SetThreadHeap 恢复);gc_coordinator.h:55-61;gc_heap_manager.cpp:19-48(Initialize 建堆+old_gen.Init)、50-60(HeapForCurrentThread);gc_heap.h:60-84(G_* 两套,SERVER 74-84 per-heap,tls_current_heap);gc_api.cpp:235-277(gen==1/gen==0 分支直连单堆,绕过 coordinator)、208-234(gen>=2 走 chaos_gc_collect);gc_region.cpp:1409-1466(chaos_gc_collect SERVER→RequestGlobalGc + WKS epilogue: RunFinalizers/GcVerifyHeap)、1411(RequestGlobalGc 唯一外部触发)、566(M3A-1 Initialize);gc_old_gen.cpp:2398-2512(Collect Phase1 根扫描 G_OldGen().ScanRangeForRoots 共享 young/gen1 → 每堆只标自己位图 = 跨堆漏标)、2402(SigNalFullGcApproach per-heap 重复)、2428(StopConcurrentMark);gc_young_gen.h:160-173(FlushTlsAllocCounter 裸 extern g_gc_scheduler);gc_worker_pool.h:62(RunWorkers);CMakePresets.json:10-143(debug/asan/windows-x64-reference 等,fragments);scripts/ci_test.py:51-75(PRESETS 各项,无 server);.github/workflows/gc-ci.yml:42-79(单 config Debug + windows-x64-reference + -R chaos_gc_ -LE);tests/unit/runtime-native/runtime-core/gc/gc_coordinator_test.cpp:30-65(仅单例/幂等,WKS),gc_heap_manager_test.cpp:64-107(SERVER 测试在 #if CHAOS_IL2CPP_GC_SERVER 内,默认构建不进),gc/CMakeLists.txt:501-508(test_gc_coordinator/test_gc_heap_manager 注册)。
[deps] 依赖:M3A-1(GcHeapManager::Initialize 接线,已就绪);GcWorkerPool::RunWorkers(GcWorkerPool 存在,gc_worker_pool.h:62,SERVER 并行宿主);tls_current_heap 机制(gc_heap.h:51)。被阻塞:动态堆数(P2 A2)、provisional 完整形态(P2 C2)在 SERVER 下需本修复的堆簿记。BGC/Server 混用(bgc controller 每堆,epilogue F3 需处理。
[risks] 1) 【最高】F2 跨堆根标记若改法引入 per-root O(n_heaps) FindPage 查表,标记阶段变慢;但正确性优先,先用正确实现再优化为直接 per-root 归属索引。
2) F3 epilogue 在 coordinator 每堆 Repeat 调 RunFinalizers,G_Scheduler 通知/位图重标记若与 BGC 重叠可能竞态(Collect 内已 StopConcurrentMark:gc_old_gen.cpp:2428,epilogue 复用同一保护)。
3) SERVER=1 构建若启用 GcNumaInit,CI 无 NUMA 硬件的 windows-latest 上 num_heaps 可能=1(自动回退 gc_heap_manager.cpp:25),则多堆代码路径测不到——CI 需用显式 heap_count 注入(Initialize(2/4))或 mock GcNumaCurrentNode 固定多堆。
4) F4 include 循环(gc_heap.h 已 include gc_young_gen.h:21,反向 include 需防环)→ 建议把 FlushTlsAllocCounter 移到 cpp 或弱化为 G_Scheduler 前向申明+extern,避免 header 环。
5) 打开 SERVER 会改变 memory 布局(gc_heap_manager.cpp:38-40 每堆独立 heap_hint),既有 WKS 测试/基准可能因默认不打开而不受影响;但新 SERVER target 需独立于 reference preset,否则 windows-x64-reference 二进制被重构。
6) 依赖:M3A-1 已就绪(Initialize 已接);F2 依赖 GcHeapManager::ForEachHeap/GcNumaCurrentNode 稳定性;CI 依赖 CMakePresets 新 preset + gc-ci 矩阵。阻塞上游:动态堆数(Dynamic Heap Count, P2 任务A2)与本修复正交。
### gap_analysis
现状实读:CHAOS_IL2CPP_GC_SERVER=OFF 默认(gc_features.h:38-44);CMake option 已在 src/native/runtime-core/CMakeLists.txt:239-OFF,260-261 做 target_compile_definitions(chaos_runtime_core PRIVATE CHAOS_IL2CPP_GC_SERVER=1);M3A-1 已接 gc_region.cpp:566 GcHeapManager::Instance().Initialize();G_OldGen()/G_Scheduler()/G_Loh() 在 SERVER 下经 tls_current_heap 路由(gc_heap.h:74-84)。

六处缺口(实读证据):
1) 【入口旁路】SERVER 下三处收集入口绕过 coordinator,直接走当前线程单堆。chaos_gc_collect 已正确路由到 RequestGlobalGc(gc_region.cpp:1408-1411 SERVER 分支);但 chaos_gc_collect_with_mode 的 gen==1(gc_api.cpp:235-266)与 gen==0(gc_api.cpp:267-277)分支直接调 GcYoungCollection/GcGen1Collection/G_OldGen().RunFinalizers(),在 SERVER 下只作用于共享 young/gen1 + 调用线程的堆,未触发全部堆,gc_active_ 门(resp)也绕开。
2) 【并行根扫描正确性(最严重)】ExecuteMultiHeapGc gc_coordinator.cpp:74-79 让每个 worker 在各自堆上跑 G_OldGen().Collect;而 Collect 的 Phase1 根扫描 gc_old_gen.cpp:2486-2512 通过 G_OldGen().ScanRangeForRoots(2014:2497,2508)(SERVER 下解析为 worker 自己 tls_current_heap->old_gen)扫描【共享 young/gen1】并只标到自己堆的位图。共享 young 中指向【堆1】的旧对象,当【堆0/堆i】的 worker 先扫到该 nursery 时,漏标堆1对象 → 堆1对象可能被过早回收。根对象与堆归属无分发/无跨堆标记,是确定性 UAF 根因。
3) 【epilogue 缺失】ExecuteMultiHeapGc(gc_coordinator.cpp:43-89)整段无 RunFinalizers/RecordFullCollection/RecordYoungCollection/RecordGen1Collection/GcVerifyHeap/GcEtwFireGcEnd(全文件 grep 无命中)。WKS 路径这些收尾都有(gc_region.cpp:1458-1466)。finalizer 在 SERVER 下完全不跑;调度器 EMA/堆估计从不更新 → 下次触发失真。
4) 【accessor 裸全局】gc_young_gen.h:160-173 FlushTlsAllocCounter 用 extern GcScheduler g_gc_scheduler(裸全局)而非 G_Scheduler()(SERVER 下 per-heap),导致 TLS 分配计数写进全局 scheduler,per-heap scheduler 的 alloc_since_last_gc_ 为 0 → per-heap 触发决策永不成立。需一并改走 G_Scheduler()。
5) 【通知重复】Collect 内部 G_Scheduler().SignalFullGcApproach()/Complete(gc_old_gen.cpp:2402)在 SERVER 下每堆各发一次,full-GC notification 被 N 堆重复通知。
6) 【trigger 链】GcAllocate/GC.Collect 的触发主链经 chaos_gc_collect→coordinator 是对的;但 nursery 耗尽 young-GC 触发(GcYoungCollection)在每个线程 TLS 上,gen1 共享故 OK,不属于缺口。调度决策 DecideCollection 返回的 FULL_FULL_FULL 最终都汇入 chaos_gc_collect,故 mainline 通了。

CI:gc-ci.yml 目前仅单配置 Debug + windows-x64-reference preset,gc-coordinator/gc-heap-manager 测试只是 WKS 单堆(tests/unit/runtime-native/runtime-core/gc/gc_coordinator_test.cpp, gc_heap_manager_test.cpp,后者 SERVER 测试块在 #if CHAOS_IL2CPP_GC_SERVER 内=当前构建永远不进)。无任何 SERVER=1 构建矩阵。
### fix_design
按缺口逐项给精确改法:

【F1 入口旁路(必做,证书:gc_api.cpp:235-277)】
chaos_gc_collect_with_mode 的 gen<0||gen>=2 已走 chaos_gc_collect(正确路由)。把 gen==1 与 gen==0 两分支在 SERVER 下改称为 GcCoordinator::Instance().RequestGlobalGc();(gen1/young 共享 + coordinator 内部已含 young/gen1 两相),WKS 保持原逻辑。即给 gc_api.cpp:236 和 267 两处 ifndef SERVER 包裹,或最简:两分支整体改成 `#if CHAOS_IL2CPP_GC_SERVER RequestGlobalGc(); return; #endif` 前置守卫。

【F2 并行根扫描(核心,必做,gc_coordinator.cpp + gc_old_gen.cpp)】
不能让每堆各自扫共享 young/gen1 并只标自己位图。改法:coordinator Phase2 之前,先做一次【根分发 mark】——在单一(主/caller)线程、safepoint 下,遍历共享 young+gen1+g_all_heap 线程栈,逐 root 调用 MarkObject,但 MarkObject 需先定位 root 指向哪个堆的 old-gen(G_HeapManager(): IsInOldGen/FindPage 判归属)再标到该堆。更省事方案(bounded):ScanRangeForRoots 时不绑定 tls,而是先枚举所有堆,对每个 root 用各堆 old_gen.IsOldInGen 判定所属堆并标入对应堆的 mark_stack。落地点:在 gc_coordinator.cpp:74 循环前插入一次根分发,把跨堆标记做完,再让各 worker 只做本堆的 DrainMarkStackParallel+Sweep。或把 MarkSweepOldGen 增加 per-root 归属回调,min. 把 coordinator 传的 root_callback 改为跨堆分发而非 nullptr。
【F3 epilogue(gc_coordinator.cpp:86 前)】
ExecuteMultiHeapGc 在 SetThreadHeap() 恢复前(83-84)后补:遍历所有堆 `mgr.ForEachHeap(... G_SchedulerRecordFullCollection/heap 估计 ...)`,对每堆 G_OldGen().RunFinalizers(),调用一次 GcEtwFireGcFullEnd/GcEnd,并对汇聚的 pause/total 调一次根链 RecordFullCollection。建议把 WKS 路径 gc_region.cpp:1458-1466 的收尾逻辑提取成 GcFinishFullGc() 复用。
【F4 gc_young_gen.h:160-173】
FlushTlsAllocCounter 的 `extern GcScheduler g_gc_scheduler; g_gc_scheduler.RecordAllocation(...)` 改为 G_Scheduler().RecordAllocation(...)(gc_heap.h:63/81 提供 SERVER per-heap 路由)。注意 gc_young_gen.h 未 include gc_heap.h,需补 include 或用前向;因 inline 内调 accessor,补 `#include "gc_heap.h"` 即可。
【F5 通知去重(gc_old_gen.cpp:2402 附近)】
SIGNALFullGcApproach/Complete 只在 heap_id==0 或 coordinator 栈内触发一次,避免 N 堆重复。minimal:在 coordinator Phase2 前后各调一次 SignalFullGcApproach/Complete,并从 Collect 内去除SERVER下的重复调用(ifdef)。
【F6 其它 SERVER 接线】
- coordinator 的 n_heaps<=1 分支(gc_coordinator.cpp:68-72)与 set tls 在集合后无需改;但单堆时也应走 F2 根分发(mark 判定同一堆,退化为现状)。
- ExecuteMultiHeapGc 内 G_Scheduler 相关(每堆 Collect 内部)已由 F3 覆盖。
顺序:F2(正确性根因,最高)→F3/F4/F1(F5 收尾)→ CI/测试。
### tests
多堆集成 gtest,路径:tests/unit/runtime-native/runtime-core/gc/gc_coordinator_test.cpp(扩)+ 新增 tests/unit/runtime-native/runtime-core/gc/gc_server_multiheap_test.cpp(建议),在 gc/CMakeLists.txt 用 chaos_native_add_test(test_gc_server_multiheap ...)注册,并在该 CMake target 加 target_compile_definitions(CHAOS_IL2CPP_GC_SERVER=1)(仿 gc_heap_manager_test.cpp:64 的 #if 块,但需独立 SERVER 构建 target 才真进 SERVER 分支)。

测什么(断言语义):
1) F2 根分发正确性:2 堆,先在堆0分配对象 O0、把指针写入共享 nursery(模拟跨堆引用),在堆1分配 O1;trigger RequestGlobalGc;断言 O0 与 O1 均存活(gc_old_gen.IsInOldGen+IsMarked),且无堆被空扫漏标 → 核心防 UAF。
2) 堆间引用可达:堆0对象持有堆1对象引用,GC 后两者均保留(验证跨堆 mark)。
3) per-heap 独立回收:仅堆1制造垃圾,GC 后堆1 live 减少/堆0 live 不变。
4) F3 epilogue:GC 后 G_Scheduler().TotalAllocatedSinceLastGC()/RecordFullCollection 状态更新、finalizer 被调(注册 finalizer 回调查 flag)。
5) F4:设 TLS 分配计数>0,调用 FlushTlsAllocCounter,断言 per-heap G_Scheduler()(GetHeap(i)) 的分配计数累加而非全局。
6) RequestGlobalGc 幂等/重入(gc_active_ 门):并发多次调用不重入、IsGcActive 归一。
7) heap_count>1 下 ExecuteMultiHeapGc worker 路径不加锁死(RunWorkers 收敛)。

注意:现有 gc_coordinator_test.cpp:42-47 的 RequestGlobalGcDoesNotCrash 在 SERVER 构建下会真正跑多堆,需保证 F2 修完才绿。CI 侧(见第3项)用独立 SERVER preset 跑,不可混进默认 WKS 测试(否则 #if 块为空)。
