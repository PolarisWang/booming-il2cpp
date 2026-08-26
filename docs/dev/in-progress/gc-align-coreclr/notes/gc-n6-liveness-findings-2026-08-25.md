# GC-N6 调查记录 — 内容存活校验暴露的运行时缺陷

> 日期：2026-08-25 | 状态：3 个发现（2 已修复提交 `ef0012d49`+`200c7dd88` / 1 已实现待验证 / 内容校验测试重载中）
> 关联：GC-N6（世代写屏障压力确定性化）、A2b 跨代引用链

---

## 一、背景

GC-N6 目标：把 `chaos_gc_region_barrier_stress_test` 的断言从"位置合法"（region-gen 分类）升级为"存活"（内容校验）——位置合法 ≠ 存活，一个被回收再循环的 nursery 地址仍分类为 managed 区域。

原型实现（本轮编写，测试本体已回滚保持快 gate 绿）：每个 nursery 对象 offset 0 写注册 TypeInfo（精确尺寸）、offset +8 写唯一 magic；GC 周期后校验每个 old-gen 槽位目标对象的 magic。**内容校验原型成功发现 3 个运行时缺陷**——"位置校验"版测试全部掩盖。

## 五、内容校验升级决定性复现跨代 UAF（2026-08-26，A2b 现形）

重载内容校验原型（nursery 对象 typed：TypeInfo@0 + self-pointer@8 + 确定性 magic@16；老 old-gen slot 存 ref），
多线程 coordinator + 8 worker，**4/4 稳定复现 `magic-miss=142/1024`，`region-invalid=0`**（连跑一致）。

- **`region-invalid=0`**：所有 dangling slot 的 ref 地址仍被 region-gen 分类为 managed——**位置校验版全绿**。
- **`magic-miss=142`**：地址上对象已是**他物**（目标被回收、地址被复用）→ **真实跨代 UAF**，位置校验掩盖。
- **per-thread 分布 `[15,64,15,6,15,6,15,6]`**（奇偶不对称、thread1=64）：**非均匀、时机依赖** → 指向
  **A2b store-then-barrier 竞态**：coordinated GC safepoint 落在 worker 的 slot-store 与 card-barrier 之间 →
  Phase-1 旧页干净卡 → 跨代边丢 → 目标被回收 → slot 过期。

**结论**：内容校验升级成功把"A2b 竞态导致的跨代 UAF"从"位置校验掩盖"变成**确定性可复现（4/4）**——这正是
GC-N6 的价值主张，也为 A2b 提供一个可靠 reproducer（此前 A2b 调查"复现 4/5 被 -LE 排除"）。**该 UAF 是
真实、尚存的生产性正确性 bug**（与 GC-N7 残余同源——丢跨代引用→堆破坏）。

**处置**：内容校验测试暴露真 bug → **不可进快 gate**（会红）；保留为 A2b 专项的确定性 reproducer 依据。
位置校验版保持快 gate 绿。

### A2b 机制 gap 定位（2026-08-26 续，为何 142 仍发生）

`barrier_inflight` + coordinator drain 已存在（thread_state.cpp:506-523）：collect 阶段 EnumerateThreads 计
`suspend_ack!=epoch 或 barrier_inflight!=0` 的线程，`s_remaining==0 → break`。**这是点态快照 → TOCTOU 窗**：
coordinator `break` 之后、young-GC Phase-1 开始之前，一个 worker 可能**新开**一个 `BarrierCriticalSectionScope`
（inflight 0→1，forbid_suspend.h:109 无条件置位，不查 safepoint 是否已请求）→ 该 store 的 card 不在 drain 已扫
视野内 → Phase-1 旧页干净卡 → 目标回收 → UAF。per-worker ~1 GC-cycle 的 store 量 = 与 142/1024 吻合。

**修复方向（γ' 阶段1 refined）**：让"safepoint 请求后不得新开 barrier 临界区"（在 `BarrierCriticalSectionScope`
内层查 `suspend_seq!=0` 则先落 barrier_inflight 再 poll，或用 request→Phase-1 全窗 drain）。高风险 safepoint
并发改动，需分阶段、先证明窗、bounded-wait + APC 兜底防死锁。

### A2b 机制 gap 定位 — 实证推翻（2026-08-26 续，γ' 阶段1 已实现并否定）

按 `design-a2b-t1-barrier-entry-gate-2026-08-26.md` **实现并测试了 γ' 阶段1**（barrier 入口 gate
`SafepointRequested()→SafepointPoll()` + coordinator drain 连续两轮 0 才 break），在 content-check probe 上跑
**20× magic-miss 全部 142**（对照 reverted 基线仍 142）。关键诊断：

- **`gate fires = 0`**（RAII-scope + chaos_barrier_enter 双计数均为 0）：整个 8-GC-cycle run 中，无任何 worker 在
  barrier 入口观察到 `suspend_seq != 0`。—— **该复现里根本没有发生"store→card 与活跃 safepoint 重叠"**，
  更不存在 TOCTOU 窗。**142 不是 A2b store-then-barrier 竞态**。
- **物理结构（每 miss 逐条抓）**：`gen=1`（gen1/survivor）+ `nursery=64 / gen1=78 / old=0`；content 为
  `foreign_BAD0DEAD=64`（目标被回收、地址被**另一对象复用**）+ `zero=42`（目标指向清零块）等。magic 从
  offset 8 移到 32 该结构不变 → **稳定、确定的丢边**，与写屏障时机无关。

**结论（诚实推翻 design 前提）**：142 是 **young-GC 晋升可达性 bug**（从 static-root 引用的 old-gen 槽持有的
nursery 对象，未被正确晋升/标记，即便卡已脏且 barrier 不与 safepoint 重叠）——非写屏障 TOCTOU。γ' 阶段1 修复
对 142 是**零收益**（会额外每次 store 加一次 acquire load），已整体回滚不提交（不 overclaim）。**真根因转
promotion-reachability 专项**（可能收敛 GC-N6 发现3 typed-walk / demotion 交互）。

### H2 假设（gen1→gen2 引用重定位缺口）实测否定 + 决定性隔离实验（2026-08-26 续）

按 H2 假设**并实现了 gen1→gen2 relocation 修复**（`gc_gen1.cpp` 所有晋升路径收集 `{old→new}` + `RelocateGen1References`
镜像 `DemotionRelocate`，覆盖 old-gen 页/static-root/thread-root/gen1 内部 + recard），实测：**272 rewrites 确实触发，
但 magic-miss 仍是 142**（且隔离实验跑出 `gen1_test` 回归 — "high-threshold demote 保持 region-gen 1" 断言失败）。**该修复也回滚**。

**决定性隔离实验（把根因从"并发竞态"与"标记 bug"分开）**：workers 跑完后**不加并发 GC**，仅 join 后**一次 full collect** →
**`magic-miss=0`（1024/1024 全活，content 全对）**。对照并发 8-cycle → 142。

⇒ **顺序（无并发 GC）标记/晋升完全正确**（full-GC mark 能到 old-gen interior → nursery 对象，全部驻留）。
⇒ **142 只在"GC 与 workers 并发执行"时出现，是纯并发 promotion/relocation 竞态**。
⇒ 且该竞态**不是** barrier 入口 TOCTOU（γ' gate=0）、**不是** gen1→gen2 引用重定位缺口（relocation 触发但无效）。

### 决定性 cycle-scan + mode 判别 — 重定位是纯多代 GC 缺陷，非并发（2026-08-26 终）

升级 probe 为 `mode` 参数化（`probe_a2b_content_check.exe <cycles> <mode>`），测出**最终判别**：

| mode / cycles | magic-miss | 含义 |
|------|-----------|------|
| `0 1`（并发 1 次 GC） | **0** | 单次并发 GC 不丢 |
| `0 2`（并发 2 次） | **142** | 第 2 次 GC 现形，饱和 |
| `0 4/8` | 142 | 不累积 |
| `1`（join 后 1 次 full collect，全顺序） | **0** | 单次全收集保全部 |
| **`2`（worker 全部写完 → 再跑 GC cycles，无 store 竞态）** | **142** | **决定性：非并发竞态** |

**`mode=2`（=0 关键）**：等所有 worker 把 1024 个 slot 全部写完后，**再**跑多次 GC，全程无 store-vs-GC 交错
→ 仍是 142。**彻底排除"worker 与 GC 并发写竞态"**。

⇒ **142 是纯多代 GC 缺陷**：当跨**多次 `chaos_gc_collect()` 调用**分割晋升时（而非一次全收集原子处理），
**早期调用产生的 gen1 存活对象，在后续调用的 gen1 collection 中丢失 cross-gen 边**。mode=0 饱和尚在 cycle=2
（首次有意义的 gen1 collection）、mode=1 单次全收集正确处理、mode=2 多调用丢 —— 三证据完全吻合。

**重定向修复并部分奏效（142→85）**：对该确定性 mode=2 重上 `RelocateGen1References` → **magic-miss 142→85**，
`gen1_relocate: 85 pairs, 84 rewrites`。说明 gen1 collection 缺"搬迁后重写外部引用"确实丢了 ~57 条边，修复
已救回；但 **85 残留**（rewritten 后目标地址 content 仍错 / 对象在旧-gen2 副本已损）+ 引 `gen1_test` demote 回归
（"high-threshold demote 保持 region-gen 1"）。**修复不完整 + 有回归 → 再次回滚不提交。**

**交接**：确定性 reproducer = `probe_a2b_content_check.exe 2 2`（1 秒内、无线程非确定、mode2）。真根因 =
**跨多次 `chaos_gc_collect()` 调用时 gen1 collection 丢失 cross-gen 边**；修复方向 = gen1 collection 搬迁后重写
外部引用（Probe 已证 142→85 有效），需解决 85 残留（rewritten 目标 content 仍错）+ demote 回归再提交。

**剩余根因精确定位**：并发 young-GC 晋升竞态（worker 的槽 store 跨多 GC-cycle 与晋升/回收交错，丢 cross-gen 边）。
γ'（barrier 入口）+ gen1-relocation 均未覆盖它。已恢复两处修复，保持工作树干净（无 overclaim）。下一专项需
**顺序序依赖调试**（cdb 抓跨 cycle 的 worker store ↔ 晋升交错点），非本 session 可盲修。

### 🔴 决定性翻案（2026-08-26 交接后首个 session）— 142 是 harness barrier-usage bug 非 GC 缺陷

重建当前 source 的 `probe 2 2` 基线 = **142**（3 连跑确定）。逐位诊断（新增 per-store `IsDirty` + `GetRegionGen`）：

```
original (dirty base card):  card_set=6..15/128 per thread, card_dirty=84/1024, young GC promoted=84
       all 1024 slot values: ref_young=1024 (GetRegionGen<old), ref_old=0  → barrier 条件不跳过
```

**根因 = probe 调屏障用错 `dst`**：`chaos_gc_dirty_card_dst_ref(g_old_slot[id], obj)` 传 **OldMessage 基址**，
只 dirty 基址所在 256B card（覆盖 slot[0..31]）。slot[32..127]（offset 256..1023，位于基址 card 之外）的 card
**从未被设置** → young GC Phase-1 扫不到 → 其 nursery 对象不晋升 → nursery reset 后悬垂 → 142。

**对照实验 A（改用正确 `dst = &g_old_slot[id]->nursery_slot[i]`）**：`card_set=128/128, card_dirty=1024/1024`,
young GC **promoted=1025（全部）**。⇒ **修正 barrier 用法后,young-GC 晋升无丢失。** 真实生产 codegen 每个
`stfld obj.field = ref` 均传**精确字段地址**,不会只有基址 card——probe 与 A2b 真实测试 (gc_region_barrier_
stress_test:73) 同为此基址简写,均是该潜在 harness bug。

**结论（诚实推翻前 session 的"真根因=gen1 collection 丢边"）**：142 主要是 **harness barrier `dst` 用错** 造成的
young-GC cross-gen 边丢失,并非"纯多代 GC 缺陷 / gen1 relocation 缺口"。前 session 的 `RelocateGen1References`
142→85 是在**错误层**修（修的是 reloc 缺口,但真 bug 是 card 设错）。~57 条"救回"实为对该 harness bug 的部分遮蔽。

**真正确修复**（task）：
1. **修 harness**：probe 与 `gc_region_barrier_stress_test` 的 barrier 调用改传**精确字段地址**
   `&slot[i]`（贴合生产语义）。这应让 content-check 测试 magic-miss → 0。
2. **验证**：修正后 `probe 2 2` 应 ≈0。若仍有残留,那才是真 GC 缺陷（再追）。
3. **独立真实问题**：Experiment A（全晋升到 gen1）后在 Debug 下 old-gen full collect 挂于
   `cross_page_compact pages=1 objects=20`（GcWorkerPool::RunWorkers 并行 copy/sweep 相）。该 hang 与
   N6 根因无关,属 parallel-GC 线的 worker_pool/gc_old_gen 交互,需单独看（且 gc_worker_pool.cpp 有并行线
   未提交改动）。

### 🔴 决定性确认（mode3）：修正 card 后 young grooming 全对,但还有独立的 content-wiring 缺陷

新增 probe `mode 3`（正确 slot-address carding + 只跑 young collection,含内部 gen1 Phase-4,跳过会挂的 full
old-gen;**修正 probe harness：不重复显式调 GcGen1Collection**,否则 gen1 双跑到异地）：

```
pre-cycle0 : nursery=1024 | content_ok=1024 content_bad=0   ← mutator 100% 正确
post-cycle0: nursery=0 gen1=1024 | content_ok=1024 content_bad=0   ← young→gen1 promote 100% 正确
post-cycle1: nursery=0 gen1=0 old=1024 | content_ok=17 content_bad=1007   ← cycle-1 gen1 collection corrupt
```

**🔴 精确定位（决定）**：corrupt 只发生在 **cycle-1 的 GcGen1Collection 把 gen1 survivor promote 到 old-gen**。
cycle-0 young→gen1 promote 100% 正确。根因 = gen1 收集**搬迁（compaction/move）后不 relocation 外部引用**。
card bug 修好后这个 gen1-relocation 缺口现形（前 session 的 `RelocateGen1References` 正是为它、但被 card bug 遮蔽）。

对象在 gen1 里地址 128B 间隔（128B 分配正确），但 slot(t,i) 指向 post-compaction 前地址 → cycle-1 Phase-3b 判 IsInGen1
失败 → 当 dead 回收 → 槽读回收后垃圾 = iter+16 错位内容。17 存活 = 恰在当前 gen1_bump 内。miss 打印 ref gen=1 与
DIAG gen1=0 矛盾 = 4MB chunk tag 陈旧（discover-1 类）。

## 二、发现 1（✅ 已修复提交 `ef0012d49`）：世代屏障 4MB region-gen chunk 碰撞 → 漏卡 → UAF

### 证据链（诊断输出，10/10 复现）

```
dangle[t=1 i=0] ref=0x1892F0B04C0 gen=0 actual=0xe7943218 card_dirty=0
dangle[t=2 i=0] ref=0x1892F150048 gen=0 actual=0x33160049 card_dirty=0
slot_region=0x18933160048
nursery=[0x1892F150048, 0x18933150048)  heap_base=0x189257A0000
```

- `card_dirty=0`：old-gen 槽位卡**从未被设置**（young GC 只清 nursery+gen1 卡，old-gen 卡持久）。
- `slot_region=0x18933160048` 与 nursery 尾部 `0x18933150048` 落在**同一 4MB chunk**。

### 根因

nursery 初始化 `SetRegionGen(nursery_begin, young)` 按 4MB 粒度标记 chunk；nursery 尾部所在 chunk 被标记 young。当 old-gen 页分配落到该 chunk 时，其上的 old-gen 对象被 `GetRegionGen` 误判为 young → `chaos_gc_dirty_card_dst_ref` 的 `dst_gen == young` 短路**跳过设卡** → Phase-1 扫不到槽 → 被引用 nursery 对象回收 → **跨代 UAF**。

### 修复（✅ 已落地）

- 目标侧：chunk 标签 → 精确 `RegionManager::IsNurseryPointer(dst)`。
- 引用侧：`ref_gen >= dst_gen` → `ref_gen >= kRegionGenOld`（保守多卡无害）。
- 验证：barrier stress（位置校验）5/5、card_table_ext 5/5、region 18/18 全绿。
- 遗留：codegen 侧 `_dst_ref` 发射若同用 chunk 标签需同样修复（未验证）。

## 三、发现 2（🟡 已实现 `gc_demotion.cpp` Phase 2.5，验证被发现 3 的挂起阻断）：demotion 不重设卡 → gen1 内跨代边不可见

### 证据链（布局版测试 8/10，槽值偏移模式）

```
dangle[t=4 i=0] ref=0x214B5170048 gen=0 actual=0x31415986 expected=0x314159a6
dangle[t=4 i=1] ref=0x214B51700C8 gen=0 actual=0x31415987 expected=0x314159a7
nursery_begin=0x214B5160048
（首个 collect: demotion: 8 objects, 2176 bytes relocated to Gen1 —— OldMessages 被 demote 到 gen1）
```

- 槽值持有**过期 nursery 地址**（内容已是对面对象的 magic = 地址被复用）→ 对象被回收而槽仍引用。
- 根因：full GC 把持有 old→nursery 内部引用的对象（OldMessage）**demote 到 gen1**，但新 gen1 地址**从未被写屏障设卡** → young GC Phase-2b（gen1 脏卡扫描）看不到 → nursery 对象被回收 → 槽过期。

### 修复（🟡 已实现未验证）

`DemotionRelocate` 新增 **Phase 2.5**：对每个 demoted entry 的 `new_addr` 调 `DirtyCard`（锁自由，gen1 已注册卡表），使 Phase-2b 能重扫被搬移对象内的跨代引用。语义严格更保守。

**状态**：已实现（含 `#include "gc_card_table.h"`），**验证被发现 3 的 young-GC 无限循环挂起阻断**——布局版测试当前无法完成一轮，需先解决发现 3。

## 四、发现 3（🟡 部分处理：补了一处一致 guard `200c7dd88`；若真根（Phase-3 BFS）仍 OPEN）：typed young-GC 无限循环挂起

> 2026-08-26 诚实更正：`200c7dd88` 修的是 **Phase-2 walk 缺 instance_size==0 guard**（对齐 PreciseObjectSize/gc_gen1，属正确的健壮性提升），
> 但原始挂起用的是 **128B 合法布局（instance_size=128≠0）**，故该 guard **未必是发现3的真正根因**。
> 真正根因更可能是原 finding 建议的 **Phase-3 BFS worklist_count 无限增长**（`GcScavengeObjectKnownNursery` 无限打日志
> 发生在 Phase 0/1/2b/3 promotion 路径，非 Phase-2 walk）。**尚未定位确认。**

### 症状

布局版测试（注册 TypeInfo 的 128B nursery 对象 + 屏障修复 + demotion 修复）**首个 young GC 即挂起**：`scavenge_object_known_nursery` 无限打日志（45s+ 不间断），无 `young trace` 完成行。位置校验版（raw 对象）无此问题。复现率：本会话 3/3 触发（含仅屏障修复、屏障+demotion 修复两种组合）。

### 已做（健壮性，非确证根因 —— `200c7dd88`）

`gc_young_collector.cpp` Phase-2 精确遍历：`obj_size = layout->instance_size`，只对 `layout==nullptr` 回退；`instance_size==0` 时
`scan_ptr+=0` 可死循环。已加 `instance_size==0 → EstimateObjectSize` 回退（对齐 PreciseObjectSize 与 gc_gen1 各 walk）。
验证：old_gen 6/6、region 18/18、card_table 5/5 全绿；gen1 基线一致。**此为正确健壮性 guard，但非发现3确证根因。**

### 已验证：当前提交代码下 discover-3 不再复现（2026-08-26）

重建 discover-3 触发：**多线程 coordinator + 8 worker 各持 old-gen OldMessage + 128B typed nursery 对象
（TypeInfo@0 + magic@8，coordinator 端打字，Phase-2 精确扫描）**，连跑 **8/8 全完成，无挂起**（临时 repro，已删）。
同时单线程自引用 typed 128B young-GC 亦不挂。

**结论**：typed young-GC + 并发坐标路径在当前代码下健壮。discover-3 挂起**很可能已被既有提交消除**
——最可能 `200c7dd88`（`instance_size==0 → scan_ptr 必前进`）或协作修复（屏障 `ef0012d49`、demotion `4fd172906`）。
属间接证据（repro 与原内容校验原型有措辞差异：打字位置、demotion 交互），非 100% 断言，但强正面。

### 待定位（发现3 真根）

**Phase-3 BFS 无限增长**：`GcScavengeObjectKnownNursery` 对 128B typed 对象无限打日志，符合 BFS worklist 循环追加
（`result->bfs_worklist[bfs_worklist_count++] = target;` 若对象图含自环/交错引用且 `IsInNursery` 判因被误判为
"未晋升需再 BFS"，可无限增长）。待：重载内容校验测试 → cdb 抓栈确认 `bfs_worklist_count` 是否单调增 → 定位循环追加条件。

### 已知事实

- 挂起发生在**第一个** young GC（usage=524288，nursery 全量），demotion 尚未参与（full GC 的 young 阶段先行）。
- 位置校验版（raw 对象、Phase-2 跳过）**不挂** → 与"Phase-2 精确遍历 typed 对象"路径强相关。
- 嫌疑路径（未定位）：Phase-2 nursery walk 对 typed 对象的 obj_size 推进、Phase-3 BFS 对 promoted typed 对象的 worklist 追加、或 Phase-1/2b 脏卡扫描与 typed 内容的交互。

### 建议

专项会话（`dev-systematic-debugging`）：cdb 抓栈确认循环点；重点查 Phase-2 walk 的 `scan_ptr` 推进与 Phase-3 BFS 的 `bfs_worklist_count` 增长。**修复前布局版测试不可启用**；位置校验版保持快 gate 绿。

## 五、本轮处置

| 项 | 处置 |
|----|------|
| `gc_region_barrier_stress_test.cpp` | **回滚至 HEAD**（位置校验版）——快 gate 保持绿；内容校验原型见本记录 |
| `chaos_gc_dirty_card_dst_ref` | ✅ 保留修复（发现 1，`ef0012d49`），三组测试全绿 |
| `gc_demotion.cpp` Phase 2.5 | 🟡 保留修复（发现 2），验证待发现 3 解决后补 |
| GC-N6 状态 | 测试强化本体待发现 3 修复后重新启用；发现 3 升级独立专项 |

## 六、验证基线（本轮）

- `chaos_gc_region_barrier_stress_test`（位置校验）：5/5 exit 0
- `test_gc_card_table_ext`：5/5 PASSED
- `test_gc_region`：18/18 PASSED

## 七、pre-existing 修复（2026-08-26 续 session：逐条修）

在完成 gen1-relocation 主修后，逐条修了 4 个 pre-existing GC 问题（3 个已 commit，1 个待专项）：

1. **dirty_card_decision 3-failure**（commit 1b9bf80c9）：测试的决策矩阵与 finding-1 修正后的 barrier 语义不符。
   修正测试：young-dst 用**真实 nursery 区**（IsNurseryPointer=true→no card），mature-dst 用 synthetic
   （card iff ref_gen<old，含保守 gen1→gen1 card）。barrier 本身正确（physical-nursery dst gate）。

2. **region_test GetRegionGen(nursery)!=young 偶发**（commit 1b9bf80c9）：region-gen 表 4MB 粒度，nursery/gen1/
   tenured 64KB 区共享 chunk，last-writer-wins 把 nursery 的 young 标签冲成 GEN1/OLD（ASLR 依赖 ~50% 闪）。根修：
   `GetRegionGen` 物理优先——`GcGetRegionGenPhysical` 先查 IsNurseryPointer / Gen1 range，物理 young 权威覆盖
   表字节，避免 barrier ref-gen / scavenge condemned-filter 因冲坏标签丢边→UAF。

3. **old-gen full-collect CrossPageCompact hang**（commit 7f56e8150）：`GcWorkerPool::Initialize` readiness
   deadlock——用全局累积 `ready_count_`，`target=ready.load()+spawned` 在**复用 exited slot** 时双重计数（旧线程
   bump 残留）→ ready 永不达标 → Initialize 死锁 → Phase-4 copy RunWorkers 卡死 → probe mode2 全 GC 挂。
   修：**Initialize 前 reset ready_count_=0**，target=spawned（每次新线程 entry bump 恰好 +1）。配合 entry-time
   bump（原 first-park 对 fast-path worker 失效）。

**遗留（pre-existing，待专项）**：
- mode2 cycle1 `GcVerifyHeap` 崩（demotion 1596 对象 relocate 到 Gen1 后 GcVerify 走 stale/0xFF ref）→ 真实
  demote 破坏性 bug（A2b memory 的 gc_demotion TypeInfo-less fallback 担忧现形，因 hang 修好后可达）。task#10。
- gen1_test teardown SEGFAULT（atoexit GcDumpStats/全局析构与后台线程 race，cdb 下不复现）。


## 八、随修 #10：mode2 cycle1 CrossPageCompact 崩（demotion 移动性 + CoreCLR 对照）

**崩溃点**（marker 定位）：probe mode2 cycle1 的 full collect 到 `CrossPageCompact`（`pages=1 objects=20` 日志后、
`cross_page_compact_done` 前）崩，`READ_ADDRESS=0xffffffffffffffff`（读 `-1`/0xFF 引用）。DemotionRelocate 各 phase
与 DecideCompactMode 都完成，崩在 GlobalRelocate/compaction 读到一个 stale/0xFF 引用。

**根因类别**：**Chaos 物理移动式 demotion**（`gc_demotion.cpp` gen2→gen1 `memcpy`）+ **手动 blanket lower_bound 修复引用**
（`DemotionRelocate` / `CrossPageCompact::GlobalRelocate`），极易漏/误读引用 → 移动后旧地址 0xFF/回收 → 下次访问读 `-1`。

**CoreCLR 对照**（`D:\OpenSource\dotnet\runtime\src\coreclr\gc\`，USE_REGIONS 版，由 subagent 调研）：
- **CoreCLR demotion 不物理移动 gen2 对象**。demoted 对象留原地址，只 `set_region_plan_gen_num` 降代 + **保持 card 不清理**
  （`card_table.cpp:1081` 跳过 `clear_gen1_cards` 当 `settings.demotion`），gen1 重扫即可。`relocate_compact.cpp:620-663`
  `check_demotion_helper` 只 `set_card`，不改子地址 → **无 "移动后修全部引用" 的脆弱性问题**。
- 任何被移对象（真 compact/evacuation）统一走 `relocate_address`（`relocate_compact.cpp:482-578`，brick+tree，不遗漏），
  且所有引用（stack/root/handle/interior/跨代）都过同一函数。
- verify_heap 遇 0xFF 引用会 `FATAL_GC_ERROR`（`vm/object.cpp` Validate→SanityCheck）；stale ref 只有在引到已释放再
  0xFF/复用内存时才崩——即 **被移动对象的引用没被更新** 这一不变量被打破。

**结论**：Chaos 的移动式 demotion + 手动 reloc 是设计脆弱点（正踩中 A2b memory 的 "gc_demotion TypeInfo-less fallback
破坏性" 预警）。CoreCLR 用 **原地 demotion + 保持 card** 从根本上消除这类 bug。

**修复方向（提案，待用户选）**：
- **方案 A（CoreCLR 对齐，推荐）**：demotion 改「原地 + 保卡」，不 `memcpy` gen2→gen1；对象留 old-gen 原位，
  `SetRegionGen(→gen1)` 重标记 + 清 old-gen mark bit（不回收页）→ 后续 gen1 collection 把 old-gen 页里的 gen1 对象当
  survivor 扫；消除「移动后修引用」。改动大（demotion 语义 + gen1 collection 对跨页 gen1 对象的处理），需重点回归。
- **方案 B（保守）**：只修 `DemotionRelocate` / `CrossPageCompact` 的漏引用点（具体是 cycle1 CrossPageCompact 读到
  的 `-1` 来源）。改动小但治标 — 仍保留移动式 demotion 的脆弱性，且需精确定位 `-1` 出处。

**方案 A 落地验证结果（修正）**：把 moving-demotion 直接 disabled（no-op）**回归**了 `chaos_gc_region_barrier_stress_test`
（SEGFAULT，exit 139）—— demotion 在该测试是 **load-bearing**（full-collect/CrossPageCompact 依赖它把对象搬离碎片页）。
所以"直接禁用 demotion"不可行。mode3 仍绿（magic-miss=0），但 full-collect 路径崩。

**修正结论**：方案 A 的"就地标 gen1"在 Chaos 的 `IsInGen1`（gen1=独立 bump region，仅区间检查）模型下不可行；"禁用"又回归
stress test。真正可行的只有：
- **方案 B**（保留 moving-demotion，修 DemotionRelocate/CrossPageCompact 的漏引用）——demotion 是 load-bearing，需
  精确定位 probe mode2 cycle1 CrossPageCompact 读到的 `-1` 来源（疑似 page_list_/free-list 在 demotion+sweep 后
  写入 -1，非简单漏引用）。改动小、风险可控，贴合"demotion 保留但不再崩"。
- **完整方案 A**：重写 gen1 collection 以扫描 old-gen 页里标 gen1 的对象（跨页 gen1 模型，对标 CoreCLR regions）。
  架构改动大、风险高。

===#10 现状===：根因=移动式 demotion 的 relocation 缺口；方案 A"禁用"回归 stress test 已撤销（demotion 已恢复，stress
green）。待用户在新认知下重选：B（修 relocation 缺口，推荐）vs 完整 A（重写 gen1 模型）。

### 方案A Phase-1 实现状态（2026-08-26）——in-place demotion 核心已落地，但暴露 cascades

已实现（Phase 1.1-1.6 + compaction-skip）：
- `OldGenPage` 加固定内联 demoted set（`DemotedObj[128]` + count），page 复用/新页清零。
- `CollectDemotionCandidates` 改 in-place：不 memcpy、不清 mark bit，记录进 page demoted set（log 实测 "in-place
  gen1-owned in old-gen pages"，384+264 对象）。
- `DemotionRelocate` gate 掉（entries old==new 时不跑，log 消失）→ **原 moving-demotion stale-ref bug 消除**。
- full-GC `ScanInPlaceDemotedRoots`（demoted 对象当 root，避免被 sweep 回收）。
- `GcGetRegionGenPhysical` 查 demoted set → gen1。
- BGC STW gen1 re-mark 加 demoted scan。
- `PlanPageEvacuation` 跳过 demoted 对象（不 evacuate gen1-owned）。

**但 mode2 实测暴露 2 个 cascade 问题（未解）**：
1. **cycle0 full collect 60 秒**（`pause_ns=60016704300`）—— in-place demoted 对象与 full-GC compaction/relocation
   交互产生病态性能（疑似 demoted 地址在 relocation/global_relocate 被反复读/写，或 compaction 计划含 demoted 引发
   O(n^2)/死循环）。compaction-skip 已把 `pages=4 objects=80`（从 391 降），但仍秒级慢。
2. **cycle1-entry 崩**（pre-cycle1 后、collect 前）——与 demotion-disabled 时同一第二崩溃，独立于 demotion bug，
   Task#10 之外。

**诚实结论**：full 方案 A 是极深、cascading 的多 session 重构（正踩中 plan 的 "highest-risk interactions"：compaction、
perf、连环崩溃）。Phase 1 in-place core 已验证生效（demotion 不再移、不再 stale-ref），但 full-GC compaction 与
in-place demoted 的交互（60s perf + 后续崩）需要额外专项。当前工作树有未提交的 Phase 1 改动。

**Phase 1 收敛（死磕后）**：
- in-place demotion 已实现 + 确定性绿（mode3 magic-miss=0、region/young/card/old_gen 全绿、barrier stress 1/1）。
- demoted set 改 atomic demoted_count（barrier 热路径并发读安全：entry 先写后 release count；remove swap + store）。
- **原始 demotion stale-ref crash（task#10 主因）已消除**（DemotionRelocate gated，mode3/suite/stress 确定性绿）。
- **mode2 仍崩（确定性 4/4），但定位到 post-Collect 尾部**（marker：`COLLECT_END` 后、`RunFinalizers` 附近，读 -1）——
  **独立于 demotion**（demotion-disabled 时同样崩），是 Collect 尾部/finalizer 的 pre-existing bug，非本 task 根因。
- cdb 无法稳定捕获（debugger 改时序），崩溃对 instrumentation 敏感（flaky/timing）。
- 结论：task#10 主因（demotion stale-ref）已由 in-place Phase 1 修复；mode2 尾部崩是另一个独立 bug（finalizer/
  后处理），建议独立 session 专攻。Phase 1 改动未提交（工作树）。

## 十、residual flakiness 专项（task#16，未根治 — 诚实状态）

试图用 ASan/gflags 抓取 GC residual flakiness（stress 间歇 crash/hang；young_collector 'plan-gen rebind' ~1/3）：
- **尝试的 fix（GcGetRegionGenPhysical 加 old-gen→OLD 物理权威）**：修好了 young_collector（0/12），但**让 stress 恶化到 9/10 crash**（barrier/scavenge 分类反转）→ 已回滚。**两个 flakiness 根因对立/交互，naive 分类 fix 方向错误**。
- **ASan 树**：obj 编译产出，但 contract test exe 未 link/emitted（ASan 运行库加载/链接问题），无法本环境抓取。
- marker/cdb 都会避开 flakiness（timing 敏感），无法可靠复现。
- **结论**：这是 pre-existing 类 timing-dependent race（in-place demotion 交叠暴露），需要**专门的 sanitizer/时序 session**（先修好 ASan 树），非盲修量级。当前 8 个 fix commit 是确定性测试上的正确改进，但不能宣称 stress 稳定；未 push（用户选先根治再推）。

### residual flakiness 专项 — 追加（task#16 未根治，诚实收尾）
- 关键混淆澄清：`build/native` 与 `build/asan-native` **共享 `artifacts/native-runtime-core-test/Debug/` 输出目录 → 互相覆盖 exe**。早期跑的 stress 有时是 ASan-instrumented 版（行为/崩点/时序大不同），造成"flakiness 程度"误判。
- 用**正常 Debug 版**重启基线：stress 7/10 pass、3/10 HANG(124)（+偶发早崩 139）——真实 flakiness 存在，但没我之前看到的 9/10 那么惨（那是 ASan exe）。
- cdb 每 attempt 70s 都避开（更改时序，不崩）；marker 也避开；ASan exe link 但 ASan runtime 环境未就绪。
- **结论**：stress crash/hang 是真实 pre-existing/hard timing race，工具墙（cdb/marker/ASan-未就绪/共享输出目录混淆）。**盲试已到递减点**。根治需独立 session：先隔离 asan 树输出目录 + 修好 ASan runtime，再跑 stress 抓 exact 内存错误。
- 交接：8 个 fix commit 是确定性正确改进，未 push；stress flakiness 单独 track。
