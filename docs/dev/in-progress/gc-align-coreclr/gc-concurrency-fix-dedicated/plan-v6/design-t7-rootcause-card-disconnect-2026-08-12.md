# T7 写屏障压力 — 根因定位 + CoreCLR 对照 + 解决方案（2026-08-12）

> 现状：young-GC Phase-1 old-gen 页 dirty-card 扫描 **找不到 barrier 设的卡**（精确证据，见下）。
> 这不是 store-barrier 竞态、不是 region-gen、不是晋升逻辑——是 **card 表写/读对 old-gen 页断开**。

## 1. 决定性证据链（多轮 instrument，全部 revert 干净）

| DIAG | 数据 | 含义 |
|------|------|------|
| barrier 决策分型 | `carded=1024  refge_dst_skip=0  dst_young=1024` | 屏障判定正确：ref 侧永远年轻，dst 见 young(0) |
| GcMarkRangeOld 对照 | 仍 `dst_gen_young=1024`（+补标也不变） | dst 的 4MB 字节被 nursery 覆盖（但补标无效） |
| Phase-1 扫描 | **`dirty_cards=0  nursery_refs_seen=0`** | **Phase-1 累计找到 0 张脏卡、0 个 nursery ref** |
| DirtyCard 写 | **`wrote=1024  nullseg_drop=0`** | **barrier 成功写了 1024 张卡（segment 都在，无丢弃）** |

**矛盾 = 根因**：
> 屏障**成功写入**了 1024 张卡（L2 segment 在，卡字节设置），但 young-GC Phase-1 的 `ScanDirtyCardsInPagesBatched` **读到 0 张脏卡**。
> ⇒ **barrier 写入的卡，与 Phase-1 扫描的卡，是同一对象但不同视图/不同 index/不同范围。**

## 2. 这个 bug 的性质（架构级）
Chaos 的两级 card 表（L1 64K segment 指针 + L2 128×512B card）是**写读共享**的单一结构，理论上写和读算同一 `(addr - g_heap_base) >> kCardShift`。出现 disconnect 只能是：
1. **`g_heap_base` 在"barrier 写入"和"Phase-1 扫描"之间变了** → 两次算的 idx 不同 → 扫描看不到 barrier 写的卡。old-gen 页（VirtualAlloc，独立 pool）先分配，若后续更高地址页把 base 扩了，或 base 重定基，就漂移。
2. **L2 segment 分配/覆盖范围与 barrier 写入的 segment 不同**。
3. 卡被**并发清除**（clear-card 与 barrier 写入 race），但 `dirty_cards=0` 是 8 周期累计仍 0，更像确定性 disconnect 而非偶发 race。

## 3. CoreCLR 对照（为什么它没有此 bug）
CoreCLR 的 card 表是**单一、启动即固定 base**（`g_gc_lowest/highest_address`，gc.h:155-156），**无运行时 base 漂移**；且 card 段在 committed 时分配、永不复用回收，GC 扫描按"committed segment"迭代（`card_table.cpp` / `make_card_table`），**barrier 写的 segment 一定在扫描集合里**。写读同源、同 base、同 segment 集——**无 disconnect 空间**。

Chaos 的 cardinal 差异：
- `g_heap_base` **可被 GcRegisterHeapRange 改变**（gc_card_table.cpp:134 向下扩 base / :123 向上生长），而 barrier 与 scan 若在不同 base 时刻计算，就错位。
- old-gen 页（独立 VirtualAlloc）与 Region 分配交错，base 变化更频繁。

## 4. 解决方案（对齐 CoreCLR）
**核心 = 让 barrier(写) 与 Phase-1 扫描(读) 对 old-gen 页共享同一、稳定的 card 视图。** 三选：

### 方案 P1（首选，直击）：card index 用"对象绝对地址"，不用漂移的 `g_heap_base`
- 改 `DirtyCard` 与 `ScanDirtyCards` 的 index 计算：**统一用 `g_heap_base` 的固定句点**（首地址，永不再变），或把 base 改为`启动即固定`（CoreCLR 做法），彻底消除漂移。
- 最稳：**`g_heap_base` 一旦 `GcSetHeapBase` 设定就不再被 `GcRegisterHeapRange` 改**；扩覆盖只扩 L1 表，不改 base。→ 写读同 base，disconnect 消失。

### 方案 P2：扫描按"已注册 segment"而非"页 payload 范围"
- `ScanDirtyCardsInPagesBatched` 改为遍历 `g_card_segment_list`（已分配的 L2 segment），对 barrier 写过的 segment 必扫。CoreCLR 就是这样（committed segment 迭代）。

### 方案 P3（兜底/诊断）：Phase-1 前 dump 单张 demo 卡
- 定位阶段：选一张 barrier 确定的卡，dump 其 segment index + card byte，与 Phase-1 扫到的对比，确认是 base 漂移还是 segment 集差异。

## 5. 下一步（诚实）
这一点(line-level)还需**一次聚焦确认**（P3 dump demo 卡，区分 base 漂移 vs segment 集差异），然后 P1（固定 base）或 P2（committed-segment 扫描）落地。两者都是**小而精准**的修复，不是 γ'/region-gen 那些大方向。工作区干净（所有实验已 revert）。
