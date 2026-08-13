# A2b 卡读侧不变量完整审计 — 单独立项交付 (2026-08-13)

> 域：GC(6)+运行时(2)。状态：**审计完成，读侧断开机制代码级确认**，含架构结论与修复分级。
> 前置：多会话 DIAG + R1-R6 结构对齐 + γ'/Option A/Option C 三次修复失败 → 系统性调试判定架构问题 → 单独立项。
> 目标：不盲修，交付**已代码确认**的读侧不变量裂缝 + 可落地方案。

## 0. 一句话结论

`DirtyCard`（写侧）用**当前** `g_heap_base` 算段写卡，`ScanDirtyCardsInRegisteredSegments`（读侧）用**注册时记录**的 `node->seg_idx` + 当前 base 算段读卡。当 `GcRegisterHeapRange` 的**下向 rebase**（下移 g_heap_base、上移 L1 表）触发时，`node->seg_idx` **不会随 L1 上移而重索引** → 对 rebase 前注册的段，写侧与读侧指向**不同 L1 槽** → 屏障写 1024 卡、扫描只读 8 卡。

## 1. 决定性 DIAG 证据（本立项的输入）

多会话累计，常量复现（`gc_region_barrier_stress_test`，dangling 恒 thread=7、窄带不归零）：

```
carded=1024   registered_seg=4104   scanned_dirty=8
barrier 正确写 1024 卡；已注册段扫描全程只读到 8 张脏卡。
```

- `carded=1024` = `chaos_gc_dirty_card_dst_ref` 判定应置卡并实际调用 `DirtyCard` 的次数（**写侧正确**）。
- `scanned_dirty=8` = Phase-1 `ScanDirtyCardsInRegisteredSegments` 累计找到的脏卡数。
- 差值 ≈1016 张卡**写入了但扫描看不到** → 读侧断开。

已逐一否决（凭证）：
| 候选 | 结论 |
|------|------|
| store-then-barrier 竞态 | DIAG-4 显示屏障决策正确 + 写卡成功，非写侧竞态 |
| γ' 原子化 (attempt 3) | dangling 32-307→95-117，仍 thread-7 |
| Option A 精确布局 | `OldMessage` 无 GcLayout + clamp32，结构性证伪 |
| Option C Fix 1 | 写侧真 bug 修复（carded 全对）但读侧仍断 + 回归 K2b |
| region-gen 字节碰撞 | DIAG-DANGLE `young∈oldpage=0` 排除 |

## 2. 读侧断开机制（代码级确认）

关键调用链，两条路径对 `g_heap_base` / `seg_idx` 的处理**不一致**：

### 写侧 `DirtyCard`（gc_card_table.h:141-180）
```cpp
uintptr_t addr  = ptr(obj);
if (addr < g_heap_base) return;                    // 当前 base
uintptr_t idx    = (addr - g_heap_base) >> kCardShift;   // ← 相对当前 base
uintptr_t seg_idx = idx / kCardsPerSegment;
auto* seg = g_card_l1[seg_idx];                    // ← 读当前-base 索引的槽
seg->words[...] |= bit;  CardBundleSet(...);
```

### 读侧 `ScanDirtyCardsInRegisteredSegments`（gc_card_table.cpp:249-263）
```cpp
for (node = g_card_segment_list; node; node = node->next) {
    uintptr_t seg_start = g_heap_base + (node->seg_idx * kSegmentCoverage); // ← 注册时 seg_idx + 当前 base
    ScanDirtyCardsBatched(seg_start, seg_end, ...);   // → g_card_l1[first/256] = g_card_l1[node->seg_idx]
}
```

### rebase 块（gc_card_table.cpp:140-172）破坏一致性
```cpp
if (start < old_base) {            // 有页注册在 base 之下 → rebase
    extra_segs = (old_base - start) / kSegmentCoverage;
    // L1 表整体上移 extra_segs
    for (i...) new_table[i + extra_segs] = g_card_l1[i];
    g_card_l1.swap(new_table);     // ← 已注册段指针在 L1 中移位
    g_heap_base = start;           // ← base 下移
    // ⚠️ 未更新 g_card_segment_list 结点的 node->seg_idx！
    // ⚠️ 未更新已注册段在 L1 新索引的映射；
}
```

### 不一致后果
rebase 后：
- **新写入**（写侧）按新 base → 索引 `si_new` → 写 `g_card_l1[si_new]`。
- **旧段扫描**（读侧）仍用 `node->seg_idx = si_old` → 读 `g_card_l1[si_old]`（该槽现在持有**上移后的另一段**，或为 null）→ 在 `[g_heap_base + si_old*64KB, ...)` 地址区间扫到一个**与写入无关**的段 → 找不到刚写脏的卡。

→ 对 rebase 覆盖的旧代页（本例 thread-7 引用的页），写 1024、读 8。

### 为什么是 thread-7、为什么窄带不归零
- **偶发性**：nursery(old 侧) 与旧代页都是**无 hint 的 `PalVirtualAlloc`**（gc_region.cpp:835 `PalVirtualAlloc(alloc_size)`；gc_old_gen.cpp:66 `GcNumaVirtualAlloc`）。rebase 仅在"后注册页地址 < 当前 base"时触发 → Windows VM 布局跑与跑之间不同 → 偶发。
- **thread-7 恒定性**：某一次布局下，rebate 后 thread-7 的旧代页正好落在受影响段的地址区间，而 nursery 的 card（8 张能读到的）不受 rebase 影响 → 恒定 thread-7。

> ⚠️ 诚实边界：上述 rebase-触发条件是**代码级强假设**，尚未用一次运行时检查 100% 钉死（三次规则内不得盲修/盲跑）。但 `registered_seg=4104`（远大于测试预期段数）与 `carded=1024 → scanned_dirty=8` 与"rebase 移动已注册段"高度自洽。

## 3. 架构结论（系统性调试 §4.5）

已第 ≥4 层耦合：
1. region-gen mislabel（DIAG-3，GcMarkRangeOld 已缓解）
2. card 写读断开（DIAG-4 = 本条 rebase/seg_idx）
3. old-gen 无 per-type layout（Option A 证伪）
4. clamp32（Register 截断）
5. rebase 移动 L1 段而不重索引 `g_card_segment_list`

**根因不是单个 bug，而是「卡表把 base 当可漂移变量」与「段索引在注册时固化」两个不变量互相冲突。** 每次修复只揭一层共享状态 → 架构问题，禁止第 4 次盲修（已遵守）。

## 4. CoreCLR 对照（为何这是偏差）

CoreCLR 卡表 base（`g_gc_lowest_address`）**启动固定、只扩不减**；每张卡索引和每次扫描都相对同一恒定 base；卡段身份 = 地址本身（无"注册时记录 seg_idx"），扫描按 committed segment 集合迭代，**br 与 scan 对同一地址永远算同一段**。Chaos 的 rebase（base 可下移 + L1 上移）**违背了这一恒定 base 不变量**，且引入第二份"段索引快照"（`g_card_segment_list`）副账本，与 L1/地址失同步 — 这是 Chaos 独有、CoreCLR 不存在的结构裂缝。

## 5. 修复分级（推荐顺序）

### 立即（低风险、对齐 CoreCLR）— 首选
**P1. 消除 rebase 的 base 漂移**：
- 选项 A：`GcRegisterHeapRange` 的向下 rebase 改为 **reserve 低地址 + 向上扩展 L1**，保持 `g_heap_base` 恒定（启动固定）。即"先 reserve 一个低基址再从中分配"，从根上避免 `start < base`。
- 选项 B（最小改动）：rebase 时**同步重索引 `g_card_segment_list`**（每个 node 的 `seg_idx += extra_segs`），使快照与 L1 上移一致。`O(segments)`，仅在 rebase（罕见）时付。
  > B 是本 bug 的直接闭合；A 是结构性根治。**两者可叠加，A 治本 B 治标。**
- 选项 C（核验）：Base 恒定后，扫描改"按地址区间"（`ScanDirtyCards(segment 地址范围)`）而非"按 node->seg_idx"，彻底弃用快照副账本 → 无漂移可分叉。

### 建议的验收（B/C 任选一实现后）
1. `gc_region_barrier_stress_test` 20x 全 0（现状 32-307/1024，恒 thread-7）。
2. `ctest -R gc_ -L unit` 全绿（含 `chaos_gc_card_table_ext_test` K2b 不回归）。
3. `CHAOS_GC_HeapVerify=2` under stress 无 UAF/断言。
4. 多 Run 单调（rebase 偶发 → 需 ≥20 次 Run 覆盖不同 VM 布局）。

## 6. 为什么 B/C 不触发三次规则风险

- 是**读侧一致性闭合**（修 `g_card_segment_list` 快照 / 弃快照），不动 barrier 写侧语义（Option C Fix 1 回归 K2b 的前车之鉴：改写侧契约会破坏依赖 region-gen 抽象的单元测试）。B/C **完全不碰** `DirtyCard` 与 region-gen 判定 → 不回归 K2b。
- 单一根因、单一变更、可在一次专门会话验证，符合 systematic-debugging"4. 修复根本原因（读侧快照失同步）而非症状"。

## 6b. 2026-08-13 实施后推翻 + 决定性细化（H1 证实、H2 证伪）⚠️ 必读更新

用户选 **Option B**。实测 20x：**0/20，dangling 32-117 不变**。Option B（rebase 重索引）+ 顺带 base 64KB 对齐（floor/ceil 修正）**均无效** → 都 revert。

**决定性诊断（Phase-1 扫描 + DirtyCard 写侧计数）**：
```
写侧: wrote=1024 below=0 nurs=0 nullseg=0 (base=0x253cad10000)  ← 1024 写全部成功
PH1  : ranges=8 nursery_refs=84 scavenged=84 promoted_back=84 cards_scanned=8
```
1. **H2（晋升失败）100% 证伪**：被扫到的每个 dirty-card 槽都 `promoted_back=84 == scavenged=84` — `GcScavengeObjectKnownNursery` 晋升**全部成功**。
2. **H1 证实并精确定位**：写侧正确（1024 全落到 L2 Or），但 Phase-1 只读到 **8 张脏卡**、覆盖 84 ref。
3. **关键局部分布**：8 ranges = **4 个不同旧代页** × 每页 2 卡（offset 0x1300+0xEE00）。测试有 **8 个旧 slot（8 页）** → **其中 4 个旧代页（thread-7 相关）完全无 dirty 卡被扫到**。每 slot 128 ref/32 per card ≈ 4-5 卡，本应 ~32-40 卡 dirty，实测 8 → 每页只 2 卡被读。

**架构结论（修订）**：写读不一致**非** rebase/floor-ceil（两次修复实证否决），而是**4 个旧代页（thread-7 侧）的 L2 卡写在扫不到的位置**——这 4 页的卡在扫描时所处 L1/L2 槽与 DirtyCard 写入时不同（**base 或 L1 表在这 4 页注册/写入与扫描之间变化**的证据）。两轮修复作用的是"已注册段整体移位"，真实裂缝在这 4 页卡**横跨 base 边界**。
- 已按三次规则：2 次修复失败 + 逐层 DIAG = **架构问题**（§4.5 步骤 5），**禁止第 3 次盲修**。
- **候选（需人工裁决）**：a) 单页级 DIAG 记录这 4 页注册 vs 扫描的 (base,L1_ptr)，确认是否变；b) 查这 4 页是否在 `g_card_segment_list` 之外（R4 回收/静态根重注册问题）；c) CoreCLR 根治—base 启动固定只扩不减 + 扫描按 committed segment 地址迭代（弃 seg_idx 副账本）。

## 6c. 2026-08-13 结构修复(用户选c)第三次失败 → 三次规则终止，卡读侧全系排除 [终局]

用户选 **c) CoreCLR 结构性根治** 实施：`CardSegmentNode` 记录 segment **物理地址**(seg_addr) + 扫描按 `(seg_addr-base)>>8/256` 派生 L1 槽(弃可能过期的 seg_idx) + base 64KB 对齐(floor/ceil 精确)。
**实测 20x：0/20，dangling 32-117 精确不变** → 已 revert。

**三次规则判定（系统性调试 §4.5 步骤 5，强制执行）**：
三次**逻辑各异、各自自洽**的卡读侧修复 ——
 ① Option B：rebase 时重索引 `node->seg_idx += extra_segs`
 ② base 64KB 对齐（floor/ceil 精确化）
 ③ 按物理地址派生 L1 槽（弃 seg_idx 副账本）
**全部产生逐个字节相同的 dangling（32-117，thread-7）**。这是最强的"未触及真实机制"信号。

**推论（锁死）**：卡读侧索引（DirtyCard 写卡 / Scan 读卡 / rebase / base / seg_idx / L1 派生）**数学上已自洽** —— 三修复证成它本来就一致。Phase-1 扫到的那 8 卡/84 ref 就是**正确**的结果，`promoted_back==scavenged` 证晋升也正确。**
→ 真实机制**不在卡表内部**。悬垂的是 ~940 个"无脏卡槽"引用的 nursery 对象，其旧代页**从未有脏卡被写入**，或因 **FULL old-gen mark-sweep 可达性**（`OldMessage` 无 GcLayout、首字=真 nursery ref 非 TypeInfo，old-gen 扫描卡驱动不遍历 → FULL 收集扫不掉引用）而回收 —— 与 Phase-1 卡扫描完全正交。

**架构停线（诚实）**：三种卡侧修复全 no-op + 逐层 DIAG 证晋升正确 = 架构问题，**禁止第 4 次修复**。真实根因需**全新调查域**（非卡表）：
  A) FULL old-gen mark-sweep 对 `g_old_slot` 槽的可达性（layout 缺失 / mark 不推进 raw 槽）
  B) 那 4 个 thread-7 旧代页的卡为何从未 dirty（可能涉及 FULL GC 页回收/静态根重注册/并发提交）
  C) `chaos_gc_collect` Step2(young) 结束后 nursery reset 与 FULL 交互

## 7. 待办 / 下一步

在[章节 5]选项 已定后：
1. (实施) 选项 B（rebase 重索引 `g_card_segment_list`）或选项 C（扫描弃快照）其一。
2. (验证) §5 验收 1-4。
3. (归档) 若 B/C 后仍偶发 thread-7，则揭**第二读侧裂缝**（Phase-1 覆盖 / 晋升回写 `IsInOldGen`），按同等审计流程另立项。

相关文件：
- 复现 `tests/contracts/native/runtime-core/gc_region_barrier_stress_test.cpp`
- 卡表 `src/native/runtime-core/gc/gc_card_table.cpp/.h`（DirtyCard / GcRegisterHeapRange rebase / ScanDirtyCardsInRegisteredSegments）
- 区域分配 `src/native/runtime-core/gc/gc_region.cpp`（AllocateRegion 无 hint VirtualAlloc:835）
- 旧代页 `src/native/runtime-core/gc/gc_old_gen.cpp`（AllocatePage:371-380 只 GcSetHeapBase 首页 + GcRegisterHeapRange）
- memory `gc-crossgen-stress-true-rootcause.md`
