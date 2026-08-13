# Chaos 跨代写屏障/卡表/压缩重定位 — CoreCLR 对照重构清单

> 日期：2026-08-12｜方法：3 维度并行对照 CoreCLR (`D:\OpenSource\dotnet\runtime\src\coreclr\gc`) vs Chaos (`src/native/runtime-core/gc`)｜状态：分析（未改码）
> 目的：结构性对齐 CoreCLR，找 Chaos 需要重构的差异。证据均为两仓库 文件:行号。

---

## 0. 一句话
Chaos 的 GC 架构在设计上对标 CoreCLR，但**有 3 组结构性偏差**导致"写屏障/卡扫描/压缩重定位"三者间存在正确性裂缝。这些裂缝就是 `gc_region_barrier_stress` dangling 及我多轮诊断（`wrote=1024` vs `dirty_cards=0`）背后的架构级成因，非单个 bug。

---

## 1. 卡表架构：扁平绝对索引 vs L1/L2+base 相对【最深层偏差，P0】

### CoreCLR（正确基准）
- 卡表是**扁平 uint32 数组**，索引用**绝对地址**：`gcard_of(obj) = obj / card_size`（gcpriv.h:6792-6795）。写屏障与扫描**用同一套绝对索引 → 结构上天然耦合**。
- 表基址靠"重译指针"（translate_card_table，gc.cpp:2677）承载，扩展时 StompWriteBarrier 重写，代码里**无 `-heap_base`**。
- **扫描集合 = committed heap_segment 链**（card_marking_enumerator，gc.cpp:4096-4127）→ 写屏障写的卡**一定在扫描集合内**。

### Chaos（现状）
- **L1/L2 两级指针表 + base 相对索引**：`idx=(addr-g_heap_base)>>9`，`seg_idx=idx/128`，`card_idx=idx%128`（gc_card_table.h:155-161）。
- 写屏障：`seg==nullptr` 时**静默丢弃写入**（gc_card_table.h:162-171）。
- **扫描集合 = 分配器链**（old-gen `page_list_` / LOH `segment_list_`），ScanDirtyCards 从页范围推卡索引（gc_old_gen.h:249-253）。

### 差距（为什么会有 `wrote=1024` vs `dirty_cards=0`）
> 写屏障的"L1 段是否非 null"（注册状态）与扫描的"页是否在 page_list_"（分配状态）是**两套独立状态**。若某页在 page_list_ 但 L1 段 null（注册遗漏/反注册不同步/基址错位），写屏障静默丢卡，扫描仍按页走 → 跨代边丢失。**CoreCLR 无此裂缝（同一扁平表 + committed 段作扫描集合，写读必闭合）。**

### 重构方向（P0，根治写读断开）
1. **扫描集合改为"已注册段"（g_card_segment_list），而非分配器链** —— 与 ClearAllCards 已用的跟踪链统一（gc_card_table.cpp:50-56,222-233）。
2. **GcRegisterHeapRange 与 FreePage/释放成对** —— 旧代 FreePage 只 VirtualFree 不反注册（gc_old_gen.cpp:381），gen1 换区只注册新的（gc_region.cpp:675-679）= 段泄漏 + 已释放页卡永不回收。CoreCLR 表随 committed 区间统一扩展/收缩（card_table.cpp:565-617）。
3. （可选）**消除可变 g_heap_base 间接层**，或至少让 GcRegisterHeapRange 不再运行期搬移 L1 + 改 base（gc_card_table.cpp:134-166），改固定索引或 stomp 重译。

---

## 2. 压缩重定位：统一根集 vs 逐路径手写子集【P0，根定址裂缝】

### CoreCLR
- `relocate_phase`（relocate_compact.cpp:1427）压缩后**无条件重扫全部根**：`GCScan::GcScanRoots(Relocate)` 走 EE 扫线程栈+static/global+dynamic+finalizer（gcscan.cpp:151）；`GCScan::GcScanHandles(Relocate)` 走 update strong/pinned/dependent/weak-interior（objecthandle.cpp:1597/1757/1340/1306）。
- **同一 promote_func 回调在 mark/relocate 两阶段复用**（sc.promotion==FALSE 走改址分支）→ **无逐类漏根可能**。

### Chaos
- **3 条搬迁路径各自手写根子集，覆盖不一致且都漏 static root**：
  | 根/对象 | CrossPageCompact | ParallelCompactPages | Demotion |
  |--------|-----------------|----------------------|----------|
  | old-gen 页内槽 | ✅ GlobalRelocate:2089 | ✅ Phase4:1936 | ✅ demotion:158 |
  | POH | ✅ :2129 | ✅ :1980 | ❌ |
  | 线程栈 | ✅ RelocateRoots:2156 | **❌ 完全没扫** | ✅ :212 |
  | GCHandle | ❌ | ❌ | ✅ :233→GcRelocateHandles |
  | **static root** | **❌ 漏** | **❌ 漏** | **❌ 漏** |

### 差距
- `GcScanStaticRoots`（gc_static_roots.cpp:70）**只在 mark 阶段（gc_old_gen.cpp:2561）调用一次**，任何 relocation 路径都不调 → static/ALC 字段指向被压缩搬走的 old-gen 对象 → relocation 后 dangling。
- `GcRelocateHandles`（engine_lifecycle.cpp:643）只在 LOH（gc_old_gen.cpp:2812）和 demotion 被调，压缩 old-gen 搬迁不调 → 被搬对象的 strong/pinned/dependent handle 是旧地址。

### 重构方向（P0，对应我此前一次未成功的修复，但收敛为单入口）
- 抽一个 **`RelocateAllRoots(old→new map)` 统一函数**，固定依次：① 页内槽 ② POH ③ 线程栈+自栈 ④ `GcScanStaticRoots`（补上）⑤ `GcRelocateHandles`（补上）。三条路径（CROSS_PAGE/COMPACT/demotion）+ 未来路径都走它。**从根上消除逐类漏根。**

---

## 3. 写屏障安全模型 + gen1 语义【P1，两处不一致】

### 3a. gen1 屏障跳卡 vs Phase2b 依赖 gen1 卡（自相矛盾）
- **CoreCLR**：region 屏障只跳 gen0（asm:216），**gen1→nursery 仍置卡**（asm:243 isOldToYoung）。
- **Chaos**：`chaos_gc_dirty_card_dst_ref` 对 `dst_gen <= kRegionGenGen1`（含 gen1）**直接 return 不置卡**（gc_card_table.cpp:104）；但 young-GC 的 **Phase2b 又依赖 gen1 脏卡找 gen1→nursery**（young_collector.cpp:597-630）。→ **两套逻辑自相矛盾：经 region 屏障建立的 gen1→nursery 引用丢失。**
- 重构：统一两套语义——要么 gen1 也置卡（对齐 CoreCLR），要么 young-GC 把 gen1 当 survivor 逐对象整体扫（不靠卡）。选一保证 gen1→nursery 可见。

### 3b. ack-and-continue vs 硬 STW（安全模型）
- **CoreCLR**：写屏障 store-then-card 安全性依赖 **硬挂起 STW**（SuspendEE 冻结所有 mutator 后才扫卡/清卡）→ 无"store 已落、card 未落、GC 已清"窗口；屏障叶子无 poll，安全点由 JIT 在方法边界插。
- **Chaos**：ack-and-continue 协作式 safepoint → 协程/OS 调度可在 store→card 之间打断，若 GC 恰在此窗口清卡则旧→young 边丢失。
- 重构：年轻收集务必用**硬挂起**（win SuspendThread）冻结 mutator 后再卡扫描/清卡，或至少在卡激活/清理段加防护。风险：平台差异（Windows APC 无法捕 JIT safepoint reg，见记忆 jit-phase2-register-capture）——但**卡表安全不依赖寄存器捕获**。

---

## 4. 已对齐（无需改，诚实标注）
- 旧代（gen2+）卡在 young-GC 中保留供 BGC STW re-mark：Chaos（young_collector.cpp:859-862, 只清 nursery/gen1）与 CoreCLR 保留 condemned+1 旧代卡一致 ✅
- 卡表 L1/L2 + card-bundle 2MB 稀疏上标 + 原子 Or 防并发丢位：已对齐 CoreCLR（gc_card_table.h:102-114 vs card_table.cpp:19-52）✅
- store-then-card 存储顺序方向一致 ✅
- 卡粒度（512B vs CoreCLR 256B+bit-per-word）为性能密度差异，非正确性裂缝，P2。

---

## 5. 优先级排序
| 优先级 | 重构项 | 对应 dangling 成因 | 成本 |
|--------|--------|-------------------|------|
| **P0** | R1 扫描集合=已注册段（消除写读断开） | `wrote=1024` vs `dirty_cards=0` 根源 | 中 |
| **P0** | R2 统一 RelocateAllRoots（static+handle 纳入） | static root 压缩后未重定址 | 中 |
| **P0** | R3 gen1 屏障语义统一 | gen1→nursery 跨代丢失 | 低 |
| **P1** | R4 GcRegisterHeapRange/FreePage 成对（段生命周期） | 段泄漏 + 已释放页卡残留 | 中 |
| **P1** | R5 硬挂起安全模型 | store→card 窗口 | 高（平台） |
| **P2** | R6 卡粒度/密度 (512→256B bit-per-word) | 性能 | 中 |

---

## 6. 建议下一步
R1（扫描集合=已注册段）是**最直接对应 `dirty_cards=0` 的根**，且改动相对集中（卡表扫描端 + 段注册成对）。我建议**先做 R1**：让扫描遍历 `g_card_segment_list`，写读闭合，重跑 stress 观察。

工作区当前干净（未改码）。要我实施 **R1** 吗？
