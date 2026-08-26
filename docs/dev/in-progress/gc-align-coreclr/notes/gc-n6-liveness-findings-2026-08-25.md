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
