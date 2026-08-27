# YoungCollectionEmpty 真测试重构 — 交接后续执行

> 日期：2026-08-27 | 作者：GC expert（本会话）| 关联：`gc-flakiness-hang-pauseforyounggc-deadlock.md`
> 目的：把既有 FAIL 的 `YoungCollectionEmpty`（`test_gc_young_collector`，SEH 0xC0000005 + r2 后 IsDirty(p) 未清）
> 从"3 轮试错已停"推进到"有承接者的真测试重构专项"。所有证据已在本会话实测固化，接续者勿再重做诊断。

## 一、最终定论（实测证据，已确认非生产 GC 缺陷）

用临时 probe（`GcRegisterHeapRange` 正确锚定 base 后 dirty `p` 的卡 → `ClearCardRange(nursery->begin, end)` → 查 `IsDirty(p)`）实测输出：

```
[diag] g_heap_base=0x171EB4E0000 p=0x171F5510048 nursery=[0x171ED2F0048,0x171F12F0048) size=64MB inNursery=1
[diag] after Clear isDirty=1   ← ClearCardRange 未清 p 的卡
```

**两个独立事实：**
1. `p` 数值上高于 `nursery->end`（0x171F5510048 > 0x171F12F0048，~5MB 越界），但 `IsInNursery(p)=1`。
   - `IsInNursery` 走 `RegionManager::Instance().IsNurseryPointer(ptr)`（gc_young_collector.cpp:68-74）——含**多/全线程 active nursery 区**。
   - 测试捕获的 `g_young_gen.region`（test 开头）是**特定一个** nursery 区，**不是** `NurseryAllocate(32)` 实际落点的区。
   - → `ClearCardRange(捕获区)` 覆盖不到 p，故卡未清。**生产单一共享 nursery 无此跨区漂移，ClearCardRange 正确。**
2. 原始 crash：测试用 `GcSetHeapBase(nursery->begin)` 原始覆盖 `g_heap_base`，违反文档化不变式
   （gc_old_gen.cpp:384-394：base 必须是最低注册地址，`GcRegisterHeapRange` 唯一 owner）。覆盖后
   `ScanDirtyCardsInRegisteredSegments` 用新 base 重算旧 old-gen 段地址→偏离 ~2.3GB→扫未映射内存→0xC0000005。

**结论：`YoungCollectionEmpty` 是测试对"单一 nursery（捕获区==分配区）"假设在多区累计测试态的误设，非生产 GC 缺陷，与方案1 无关。**

## 二、为什么是测试重构（而非生产修复）

- 生产：单个共享 nursery（G_YoungGen shared young gen），`NurseryAllocate` 落点和 `g_young_gen.region` 一致；
  `DirtyCard` 对 nursery 写直接 skip（gc_card_table.h:161-163），nursery 卡从不经写屏障，`ClearCardRange`
  只管 old-gen 卡。nursery 对象走精确扫描（Phase-2），不依赖脏卡。
- 测试：多 TEST_F 累计全局态（GcUnitTestBase::SetUp 每次 warmup `g_old_gen.Allocate` + 前测试污染），
  RegionManager 累积 active nursery 区，测试捕获的 region 与分配区可能不一致。**这是测试环境全局态问题。**

## 三、重构方向（候选，需 RegionManager API 探查后定）

> 接续者先读 `RegionManager::IsNurseryPointer` + 多区管理 API，再选。

1. **定区用 RegionManager 权威**：用 `IsInNursery(p)` 判定 p 所在区，或查 `RegionManager` 提供"当前/分配区"
   的 API，dirty `p` 的卡 + `ClearCardRange(该区)`，不再假定捕获区==分配区。
   - 需找：RegionManager 是否暴露"含某地址的 active nursery 区 [begin,end)"查询。若有 → 最优。
2. **约束单区布局**：测试前 `GcSetHeapBase` 更正 + 仅保留一个 active nursery（shutdown/回收多余区），
   恢复"单一 nursery"前提。较重，需理解区生命周期。
3. **弱化断言**（最小改动，非推荐为根治）：去掉 `dirty_cards_scanned==0` + 改 `IsDirty(p)` 的区范围
   用 `IsInNursery(p)` 同源。保守但保留单区假设的脆弱。

## 四、本会话已验证的关键事实（勿重做）

- 原始 `GcSetHeapBase(nursery->begin)`（test line ~182）→ crash 0xC0000005 at
  `GcYoungCollection`→`ScanDirtyCardsInRegisteredSegments`→`phase1_scan_cb`（lambda_2::operator+0x58），
  读地址 0x1ff1ebf0f48（≈ base+2.3GB，未映射）。3 轮尝试（原文 / GcRegisterHeapRange / 生产锚定+去断言）
  均已实测，结论见 §一/§二。
- `dirty_cards_scanned==0`（L198）在 base 锚定到真实最低（old-gen 低于 nursery）时必然非0（r1 见残留脏卡），
  是布局脆弱断言，非不变式。
- `ClearAllCards()` 会清掉 BGC re-mark 依赖的 old-gen 卡态，破坏后续 `ConservativeSweepSelfRefs` 等 → 禁用。
- 相关测试 `CollectionWithDirtyCard`（PASSED）用 `calloc old_block` + `GcSetHeapBase(base_aligned=nursery 对齐)`
  —— raw block 未注册为 managed segment，故不触发段地址爆炸；可作为"可控基址"参考但不适用 nursery 区漂移。

## 五、验收（接续者做）

1. 重构后 `test_gc_young_collector` 全绿（含 YoungCollectionEmpty，不再 crash 不再误断言）。
2. **不回归**：`test_gc_bgc_root_scan` 4/4（本轮已修的 RootChangeBufferWraparound 不能回退）、
   `test_gc_parallel_mark` 6/6、`test_gc_worker_pool` 7/7、test_gc_bgc_unit/smoke、chaos_gc_bgc_smoke。
3. 提交根因/修复策略/回归检查三规则 message，单独 commit。
4. 若走方案1/2 需新增 RegionManager 测试覆盖多区行为，勿只改单测。

## 六、与既有修复线边界

- `RootChangeBufferWraparound`（已修并 push `3d11cca10`）：`BgcRecordRootChange` 移除 producer tail 前进，
  tail 归 STW drainer 独占。**已合入 main**，接续者在此之上工作。
- 本专项只动 `gc_young_collector_test.cpp`（+ 必要 RegionManager API 探查/测试），不碰并行 GC 工作线改动
  （`gc_old_gen.cpp`/`parse_convert.cpp` 等未提交项属并发轨道）。
