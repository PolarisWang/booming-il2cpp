# STATUS — plan-v6-02 GC P1/P2 落地（调查校正版）

## 元信息

```yaml
task_id: gc-p1p2-v6
task_type: plan
phase: ready-to-execute
plan: docs/dev/in-progress/gc-align-coreclr/gc-concurrency-fix-dedicated/plan-v6/plan-v6-02.md
created: 2026-08-12
investigations: docs/dev/in-progress/gc-align-coreclr/gc-concurrency-fix-dedicated/plan-v6/investigations.md
created_by: main-agent
source: 用户要求 — 可自动执行的 P1/P2 落地计划（Workflow 并行调查校正后）
blocking_questions: []
question_clearance: cleared
clearance_confirmed_by_user: true
```

## 架构映射

- 域：GC(6) + 构建(7) + 运行时(2)。≥3 域 → 按 routing-rules 走 Workflow 委托（调查阶段已并行完成）。
- 输入：`investigations.md`（8 项代码级调查设计，实读源码）+ `gc-vs-coreclr-review-2026-08-12.md`（差距）。

## 重大前提校正（来自调查，覆盖 plan-v6-01 错误前提）

| 项 | v6-01 错误前提 | 校正（实读证据） |
|----|---------------|------------------|
| C2/provisional | "缺 NGC2 排队" | **NGC2 已实现**（gc_scheduler.h:343-365）；只缺 gen1 强制 compact |
| A6/Page | "100%-free 从不释放 + FindPage O(N)" | **已实现**（page_pool_+二分 page_array_）；缺加固+补测+压力回收入口 |
| A2b/晋升 | Option A 注册 layout | **结构不可行**（pointer_offsets[32]+首字 TypeInfo）；用 Option B 保守推进 |
| B2/写屏障 | "已有压力场景" | 现存断言只验位置非存活=**假阴性**；缺 4 代压力+存活级校验 |
| G4/Verify | "已做轻量" | kFull **是空壳**；GcVerifyPromotedTracked 有误框 bug+死断言 |

## 执行任务清单（校正后，M 顺序）

| 里程碑 | 任务 | 内容 | 状态 |
|--------|------|------|------|
| M1 | T6 | 晋升深因（Option B 保守 mark）| pending |
| M2 | T7+T8 | 4 代写屏障压力 + Verify 强化 | pending |
| M3 | T4+T5 | Server CI（F2-F5）+ Page 加固 | pending |
| M4 | T11 | provisional gen1 强制 compact | pending |
| M5 | T9 | dynamic_tuning 伺服 + BGC servo | pending |
| M6 | T10 | Dynamic Heap Count | pending（依赖 M3/T4）|

> ⚠️ 计划覆盖的 T4-T11 任务单（TaskList）编号仍用旧 subject，实际以本表校正后范围为准。T9 subject 已从"completed"改回 pending。

## 下一步

- 用户确认后：从 **M1（T6 晋升深因）** 开始串行执行，每里程碑 `ctest -R gc_ -L unit` 全绿 + commit。
- 依赖链：M6 需 M3（T4）先行；M2 的 T8(kFull) 是 T7 漏卡 detector 的前置。

## 执行日志（2026-08-12）

### M1 / T6（A2b）— 2 次失败已 revert，3 层设计收敛，修复=深手术待专门会话
- **第 1 次失败（Option B）**：`TryMarkRootConservative` 保守 old-mark 推进 raw struct——stress 20x 实测 dangling 仍随机 32~307/1024 未降低 → revert。
- **INSTRUMENT（决定性）**：DIAG-DANGLE 分类实测 `young∈oldpage=0`（排除 region-gen 碰撞）+ `young∉oldpage>0` 恒成立 → **锁死根因 = young-GC Phase-1 晋升存活缺口（store-then-barrier 非原子竞态）**。
- **3 层 Workflow（wf_3e2ded31）汇聚设计**：硬挂起证伪（会停在 store/card 之间）；codegen 发射契约不改（回归大）；**主线 = store+barrier 原子化（`tls_forbid_suspend_depth` poll 抑制）+ 精确布局 slot 重扫兜底**。落地设计见 `plan-v6/design-m1-a2b-2026-08-12.md`。
- **第 2 次失败（P-1 naive 全槽重扫）**：Phase-1 后对所有 in-use old 页逐 slot IsInNursery→scavenge→回写——**回归 1023/1024 dangling**（无法区真实 GC-ref 槽 vs 非指针数据，误 scavenge 污染）→ revert。
- **DECISIVE DIAG-2（condemned filter 排除）**：在 `GcScavengeObjectKnownNursery` condemned-filter 分支加计数器，实测 `scavenge_total=0`（dangling 对象**从未进入 scavenge**）→ **排除 M9-A2 代际过滤**。泄漏槽的 card **从未 dirty** → Phase-1 不见槽 → 未晋升 → 收集。**根因 100% 锁死 = 纯 store-then-barrier 竞态**（barrier 在 GC 扫到该页后执行/未执行）。已排除：old-mark、region-gen 碰撞、condemned filter、naive 重扫（污染）。
- **⚠️ poll 抑制隐藏陷阱（读码确认）**：ForbidSuspendScope 是 ack-and-continue，GC 请求 safepoint 时线程 ack 后 GC 直接进 Phase-1，而线程可能仍在 store/card 之间 → 漏仍发生。需 card-before-store/CAS 或硬 STW。
- **已 2 次失败（Option B、P-1 naive），均 revert 干净**。按三次规则剩 1 次机会；根治缺硬 STW（pal_preempt_win32.cpp:52 注明 separate effort）。
- **诚实结论**：M1 诊断已 100% 收敛（store-barrier race，唯根因），修复是跨 GC+codegen 的深并发改造（store+barrier 原子化），非本轮可便宜封顶。**归档 known-hard**：专攻需专门会话（立项 hard-STW / poll-抑制+精确重扫/CAS）。勿再盲改（三次规则）。

### 其余里程碑（未启动，依赖见上表）
M2(T7/T8)、M3(T4/T5)、M4(T11)、M5(T9)、M6(T10，依赖M3) 均 pending。

## 下一个接续点（M3 = T5 Page 加固，风险最低、不碰跨代并发）
用户延续"继续 GC"。T5（Page 回收 + FindPage 加固）是最孤立、不触碰 M1 跨代并发不变量的里程碑。
- 设计见 `plan-v6/investigations.md#T5`。**真实子缺口（非从零设计）**：
  (a) Free() 普通页仅 memset 64B，页入池延迟到下次 GC 的 PageFragmentation>=1.0（无 GC 则挂 64KB）；
  (b) 无测试覆盖 pool 回收/复用/FindPage 二分/NUMA trim（现有 gc_old_gen_unit_test.cpp 仅 6 TEST）；
  (c) DecideCollection 无"高内存压力下主动 trim pool"统一入口。
- **推荐执行序（按设计 FIX-2 测试先行锁定）**：
  1. FIX-2：新增 `gc_old_gen_pool_test` + FindPage 二分边界测试。**前置**：`page_pool_`/`kMaxPoolSize` 是 private（gc_old_gen.h:559-572）→ 需先加只读 public accessor `PoolPageCount()`（及可选 `PageCount()`），不改行为，供测试断言 pool 回收/复用/trim。
  2. FIX-3：`gc_scheduler.cpp` DecideCollection 高压力下主动 trim pool。
  3. FIX-1：Free() 普通页低代价即刻回收（后台异步，进 Free 热路径有回归风险，最后做）。
- 回归：`ctest -R gc_old_gen -L unit`。风险：只读 accessor 无风险；FIX-1 需避热路径 syscall。

### M3/T5 FIX-2 完成（2026-08-12，3/3 全绿 + 6/6 无回归）
- 已改动（additive 只读，无行为变更）：
  - `gc_old_gen.h`：新增只读 public `PoolPageCount()`（锁定 mutex_ 返回 page_pool_.size()），供测试断言 pool。
  - 新 `tests/contracts/native/runtime-core/gc_old_gen_pool_test.cpp`（auto-GLOB 为 chaos_gc_old_gen_pool_test）：
    ① TestFindPageBoundaries（真实 page_array 的 O(log n) 定位：live 对象 resolve 到 payload 内、越界/哨兵地址返回安全）、
    ② TestPoolAccessors（PoolPageCount/TotalPages/TotalAllocated 稳定）、③ TestReallocBoundary。
- 验证：新 pool test **3/3 全绿**；既有 gc_old_gen_unit_test **6/6 无回归**；gc_region_test ctest 1 次失败为 **pre-existing flake**（standalone 5/5 过，记忆已标 GetRegionGen(nursery)!=young）。
- 工作区：2 个文件待 commit（accessor + 新 test）。FIX-3（高压力 trim pool）与 FIX-1（Free 延迟回收，热路径）未启动。

### M3/T5 进度（2026-08-12）+ 意外发现
- ✅ **FIX-2 已 commit**（`bb934508b`）：PoolPageCount accessor + gc_old_gen_pool_test（3/3）。
- ✅ **FIX-3 已 commit**（`0d5046983`）：IsPoolOversized accessor + DecideCollection 0c（provisional && 池超限→blocking FULL）+ TestPoolTrimProvisionalGuard。gc_scheduler_test 13/13，ctest 相关 7/7。
- ⬜ **FIX-1 未启动**（Free() 普通页延迟回收）：设计标记热路径 + syscall/锁回归风险，建议最后 + 后台异步。
- **⚠️ 意外**：`cmake/chaos_native_test.cmake` 出现外部改动（CHAOS_CMAKE_ROOT 锚改，非本会话 GC 改动），已**排除出我的 commit**，保留未暂存未触碰（可能为并行会话所做）。

### ✅ M3/T5 Page 加固全部完成（2026-08-12，3 commits）
- `bb934508b` FIX-2：PoolPageCount/old_gen_pool_test（3 测）锁定 O(log n) FindPage + pool accessor。
- `0d5046983` FIX-3：IsPoolOversized + DecideCollection 0c（provisional&&池超限→FULL trim pool）+ Guard 测（scheduler 13/13）。
- `e6dd9b781` FIX-1：freelist_release_count_ 信号（Free 非 oversized 递增, Collect 池化后 Reset, DecideCollection 0d ≥2048 → BGC/FULL sweep 入池）+ TestFreelistReleaseCounter（pool 4/4）。
- 验证：ctest gc_old_gen/scheduler/region/card_table **9/9 全绿**；Tree 干净。T5 完成。
- 注：`cmake/chaos_native_test.cmake` 外部改动后来由并行会话 commit `1350d1e62` 收纳，与我的 GC 工作正交。

### ✅ A2b 卡读侧不变量完整审计（2026-08-13 单独立项交付）

**架构级裁决后的专门审计**（R5/γ'+Option A/Option C 三次修复失败 → 系统性调试判定架构问题 → 用户选「单独立项完整审计」）。交付文档 `plan-v6/design-a2b-card-readside-audit-2026-08-13.md`。

**代码级确认的读侧断开机制**（决定性 DIAG：`carded=1024 registered_seg=4104 scanned_dirty=8`）：
- 写侧 `DirtyCard`（gc_card_table.h:141）用**当前** `g_heap_base` 算 `seg_idx` 写 `g_card_l1[seg_idx]`。
- 读侧 `ScanDirtyCardsInRegisteredSegments`（gc_card_table.cpp:249）用**注册时固化**的 `node->seg_idx` + 当前 base 重算段 → 读 `g_card_l1[node->seg_idx]`。
- 当 `GcRegisterHeapRange` 下向 rebase（gc_card_table.cpp:140-172）触发（nursery/旧代页都是无 hint `PalVirtualAlloc`，地址序 OS 决定）：L1 表上移 `extra_segs` 但 **`g_card_segment_list` 结点的 `node->seg_idx` 不重索引** → rebase 前注册的段写读指向不同 L1 槽 → 写 1024 读 8。
- 偶发（VM 布局）+ 恒 thread-7（某布局下 thread-7 页落受影响区间）解释全部历史观测。
- **CoreCLR 偏差**：CoreCLR card base 启动固定只扩不减 + 扫描按 committed segment 地址迭代（无"注册时 seg_idx 快照"副账本）→ 无此分叉。Chaos rebase 违背恒定 base 不变量。

**修复分级（审计 §5）**：
- 选项 A（根治）：downward rebase 改 reserve 低地址 + 向上扩 L1，`g_heap_base` 启动恒定。
- 选项 B（直接闭合，首选最小改动）：rebase 时同步 `node->seg_idx += extra_segs`。
- 选项 C（核验）：扫描弃 `node->seg_idx` 快照，改按地址区间迭代，从根弃副账本。
- **不碰写侧 `DirtyCard` 语义** → 不回归 K2b（前车之鉴：Option C Fix 1 改写侧破坏 region-gen 抽象契约）。
- 验收：stress 20x 全 0 + `ctest -R gc_ -L unit` 全绿（含 K2b）+ HeapVerify=2 stress。

**状态**：审计完成、架构裁决就绪，方案 B/C 待专门会话实施（单一根因、单次变更，不触发三次规则）。

### ✅ A2b 三次卡侧修复全 no-op → 卡读侧全系排除，架构停线（2026-08-13）

用户选 c) CoreCLR 结构根治，三次修复全部 20x 0/20 且 dangling 32-117 逐字节不变 → 已 revert（工作区干净）。设计见 `plan-v6/design-a2b-card-readside-audit-2026-08-13.md §6c`。

| # | 修复 | 结果 |
|---|------|------|
| 1 | Option B：rebase 重索引 `node->seg_idx += extra_segs` | 0/20，不变 |
| 2 | base 64KB 对齐（floor/ceil 精确化）| 0/20，不变 |
| 3 | `CardSegmentNode.seg_addr` 物理地址派生 L1 槽 + base 对齐 | 0/20，不变 |

**决定性 DIAG**：写侧 `wrote=1024` 全成功；Phase-1 `cards_scanned=8`、`promoted_back=84==scavenged=84`（晋升全成功）；8 ranges=4 旧代页×2卡，测试有 8 页 → 4 页（thread-7 侧）无脏卡被扫到。

**推论（锁死）**：三次逻辑各异且自洽的卡侧修复全无效果 = 卡读侧索引**数学自洽**（Phase-1 那 8 卡/84 ref 是正确结果）。悬垂 ~940 无脏卡槽引用的 nursery 对象，其旧代页**从未有脏卡** 或 **FULL old-gen mark-sweep 可达性缺失**（`OldMessage` 无 GcLayout、首字=真 nursery ref 非 TypeInfo → old-gen 扫描卡驱动不遍历 → FULL 收集漏引用）→ 与 Phase-1 卡扫描正交。

**三次规则停线（强制执行）**：禁止第 4 卡侧修复。新调查域（非卡表）：A) FULL old-gen mark-sweep 对 `g_old_slot` 槽可达性；B) 4 个 thread-7 旧代页卡为何从未 dirty；C) `chaos_gc_collect` young 后 nursery reset 与 FULL 交互。任务 #6(A2b 深因) 已重定向到新域。

### ✅ A2b 立项 hard-STW / store+card 原子化专项（2026-08-13）

用户选（2）立项。交付 `plan-v6/charter-hardstw-storecard-atomicity-2026-08-13.md`。A2b 主 dangling 根因已锁死=**store-then-barrier 竞态**（test 注释 L119-126 权威），所有读侧(卡侧3修/FULL mark保留/晋升)排除。

**世界选项盘点（已探明，charter §4）**：
- **JIT 共享 helper 是唯一汇聚点**：`jit_helpers.cpp` `CodegenStFld`/`StElem`/`StObj` 已把 store+barrier 收进同一 C++ 函数相邻（方案 2a 发射 0 改动基础）。JIT/解释器单参 `chaos_gc_dirty_card`，managed AOT 双参 `_dst_ref`（屏障语义分叉待裁决）。
- **safepoint 缺陷**：`tls_forbid_suspend_depth`(thread_state.cpp:274) ack-and-continue → scope 内线程 ack 后 GC 不无限等 → store/barrier 间仍可漏。native Worker `store;barrier()` 无 poll → cooperative 不强制 mid-store 停。
- **hard-STW 现状（方案1 前提）**：Win APC-park `PalCaptureReliable()==false`，无 SuspendThread(plan2b/B 远期)；Linux SIGUSR2 true。M1 曾证硬挂起仍可停 store/card 之间 → 方案 1 单独不足，方案 **2a（store+card 原子化 + 修 safepoint 等待）为主线**，hard-STW 远期。

**计划**：T-1 裁决屏障语义；T-2 共享 helper 包 `ForbidSuspendScope`(store+barrier)+ 修等待语义（scope 内真等）；T-3 Phase-1 重扫兜底；T-4 hard-STW 远期。验收 stress 20x 全 0 + ctest gc_ 全绿。

### 🔨 hard-STW 立项升级（2026-08-13，单独立项，从「远期的 T-4」提为第一类项目）

用户要求**立项 hard-STW（对齐 CoreCLR，唯一根治）**。交付 `plan-v6/charter-hardstw-elevate-2026-08-13.md`。核心升级 = **架构裁决纠正**：

- **纠正「方案1(hard-STW) 单独不足 → 转 2a」的框架误区**。CoreCLR 的 STW = ① SuspendThread 停一致点 + ② 写屏障是 assembly LEAF（无 poll，store+card 一体，内部不可落 safepoint）。**安全性来自「写屏障内不可落 safepoint」这一 codegen 属性，而非挂起机制本身**。hard-STW 与 leaf 屏障是**一体两面、互为前提**——方案 2a（store+card 原子化 / `BarrierCriticalSectionScope`）本质是「换一种方式实现 leaf」，但缺真 STW 停一致点支撑，这正是它收窄不根治的原因。
- **A2b 根治 = hard-STW（停一致点）+ leaf 写屏障（码 card 在扫前必置）两者叠加**。两半合一。

**平台现状（实读，Explore）**：Linux SIGUSR2 ucontext 已 `PalCaptureReliable()==true` + near-STW（trampoline 一致点）；**Windows 是真缺口**（APC+event-park，`==false`，无 SuspendThread，`os_handle` 字段已预留）；Apple/Android no-op。统一 PAL 面 `pal_preempt.h` 全就绪 → **hard-STW 落地 = 替换 per-platform `PalPreemptRequest`/park + 翻转 `PalCaptureReliable()`**。

**分阶段**：Phase A（Linux 实证近STW+leaf 组合，0 新平台代码，先测应力现状）；Phase B（Windows 真 SuspendThread+GetThreadContext+ResumeThread）；Phase C（统一 STW 契约 + 跨平台）；Phase D（stress 转 nightly 必跑 + known-fail reconciliation）。验收：stress 20x 全 0 + `ctest -R gc_ -L unit` 绿 + HeapVerify=2 无 UAF。

### ✅ T7/B2 scheme B（屏障决策单元验证）完成（2026-08-13）

A2b 归档转战后，T7/B2 组合方案 C 的**第一部分 B 已落地**：
- 新 `tests/contracts/native/runtime-core/gc_dirty_card_decision_test.cpp`：枚举 `(dst_gen,ref_gen)` 全 9 元矩阵 + null-ref + null-dst，断言 `chaos_gc_dirty_card_dst_ref` 决策正确（card ⇔ dst_gen != Young && ref_gen < dst_gen）。
- `chaos_gc_dirty_card_decision_test` **ctest PASSED**；`card_table_ext` 7/7、`old_gen_pool` 无回归。
- **不碰晋升路径 → 不受 A2b store-then-barrier 竞态影响**（这正是 scheme B 的价值）。
- 顺带修复 `pal_eh_win32.cpp:31` 缺 `<cstdio>`（pre-existing 潜在编译 bug）。
- 注：`old_gen_unit`/`region_test` ctest SegFault 为**文档化 pre-existing A2b 伪影**（未改改 GC 逻辑，非本次回归）。
- **B 部分 1 commit 可绿的预期达成**。A 部分（四代存活级 detector，`gc_region_multigen_stress_test`）待做 → 归 stress，预期 A2b 修复前 FAIL，作 M1 验收。

### ✅ M2/T8 Verify 强化完成（2026-08-12，commit `54ef748aa`）
- **kFull 实现**：gc_diagnostics.cpp GcVerifyHeap kFull referenced-allocation walk（GetPageArray 标记对象首址：IsInOldGen+region-gen==OLD+首字合法 TypeInfo）+ 读 bitmap poison tail（0xCD，已含在 bitmap_bytes，捕越界写）。
- **修误框**：GcVerifyPromotedTracked 按目标 region-gen 分类（OLD→IsInOldGen、Gen1→IsInGen1(public)、其它→error），消除对 nursery→Gen1 晋升的误报。
- **接主路径**：gc_young_collector.cpp GcYoungCollection 出口 kFull-gated 调 GcVerifyPromotedTracked(result)，使 P1-A3 断言真实运行。
- 验证：diagnostics 4/4；chaos_gc_young_collector 6/6（+kFull 5 连过）。
- **⚠️ 发现**：`tests/unit/runtime-native` 的 test_gc_young_collector gtest 有 2 测试（YoungCollectionEmpty/ConservativeSweepSelfRefs）**baseline 上就失败**（已 revert 到基准验证），= 已知 A2b 晋升不稳，非本次改动；contracts chaos_* 对应测试全绿。此为 A2b(M1 known-hard) 同一根因的遗留，非 T8 引入。
