# GC-N6 调查记录 — 内容存活校验暴露的两个真实运行时缺陷

> 日期：2026-08-25 | 状态：发现已固化，修复部分落地（屏障），gen1 重叠待专项
> 关联：GC-N6（世代写屏障压力确定性化）、A2b 跨代引用链

---

## 一、背景

GC-N6 目标：把 `chaos_gc_region_barrier_stress_test` 的断言从"位置合法"（region-gen 分类）升级为"存活"（内容校验）——位置合法 ≠ 存活，一个被回收再循环的 nursery 地址仍分类为 managed 区域。

原型实现（本轮编写，已回滚测试本体）：每个 nursery 对象 offset 0 写入唯一 magic（`kMagicBase + thread*stride + index`），GC 周期后校验每个 old-gen 槽位目标对象的 magic 完整。

## 二、发现 1（✅ 已修复）：世代屏障 4MB region-gen chunk 碰撞 → 漏卡 → UAF

### 证据链（诊断输出，10/10 复现）

```
dangle[t=1 i=0] ref=0x1892F0B04C0 gen=0 actual=0xe7943218 card_dirty=0
dangle[t=2 i=0] ref=0x1892F150048 gen=0 actual=0x33160049 card_dirty=0
slot_region=0x18933160048
nursery=[0x1892F150048, 0x18933150048)  heap_base=0x189257A0000
```

- `card_dirty=0`：old-gen 槽位卡**从未被设置**（young GC 只清 nursery+gen1 卡，old-gen 卡持久）。
- `slot_region=0x18933160048` 与 nursery 尾部 `0x18933150048` 落在**同一 4MB chunk** `[0x18933000000, 0x18934000000)`。

### 根因

nursery 初始化 `SetRegionGen(nursery_begin, young)` 按 4MB 粒度标记 chunk；nursery 尾部所在 chunk 被标记 young。当 old-gen 页分配落到该 chunk（nursery 尾部 +64KB 处），其上的 old-gen 对象被 `GetRegionGen` 误判为 `kRegionGenYoung(0)` → `chaos_gc_dirty_card_dst_ref` 的 `dst_gen == kRegionGenYoung` 短路**跳过设卡** → young-GC Phase-1 扫不到该槽 → 槽引用的 nursery 对象被回收 → **跨代 UAF**。

这正是 A2b 调查（`gc-concurrency-fix-handoff.md`）记录的"4MB region-gen 粒度碰撞 + 旧代页不归 RegionManager 管理"问题——此前仅停留在诊断层面，本轮由内容校验首次**可复现触发**。

### 修复（✅ 已落地，`chaos_gc_dirty_card_dst_ref`）

- 目标侧：chunk 标签判定 → 精确 `RegionManager::IsNurseryPointer(dst)`（仅真 nursery 目标跳过设卡；碰撞的 old-gen 对象现在正确设卡）。
- 引用侧：`ref_gen >= dst_gen` → `ref_gen >= kRegionGenOld`（保守：gen1→gen1 现多设卡，无害；mature ref 仍跳过）。
- 行为差异：仅修复碰撞漏卡 + gen1→gen1 保守多卡，其余路径等价。
- **验证**：barrier stress（位置校验）5/5、test_gc_card_table_ext 5/5、test_gc_region 18/18 全绿。

### 遗留

- **codegen 侧同型问题**：`NativeAotLoweringPlanner` 发射的 `_dst_ref` 世代短路若同样用 chunk 标签判 dst，需同样改为精确 nursery 判定（未验证，待查）。
- `DirtyCard`（单参）的 nursery-range 快路径与 `chaos_is_gc_pointer` 的 `g_nursery_range_begin/end` 窗口同理存在边界近似，但属"多设卡无害"方向。

## 三、发现 2（🔴 OPEN）：gen1 region ↔ old-gen 页虚拟地址重叠 → gen1 集合破坏 old-gen 对象

### 证据链（屏障修复后仍 10/10 复现，模式改变）

```
dangle[t=0 i=0] ref=0x25CA8440048 gen=1 actual=0xa8452848   # ref = gen1->begin 精确
dangle[t=0 i=1] ref=0x0000000031415926 gen=2 safe=0          # 槽值 = magic 本身！
slot_region=0x25CA8450F00                                    # 槽(OldMessage)落在 gen1 区间内
nursery=[0x25CA4430048, 0x25CA8430048)  gen1 range 起点=0x25CA8440048
```

- 槽引用的对象**确实晋升到了 gen1**（ref = gen1 区起始），但 gen1 地址处内容非 magic。
- `slot_region`（OldMessage，old-gen 分配）落在 **gen1 collection 的 range `[0x25CA8440048, gen1_bump)` 之内**。
- 槽值出现"magic 当指针"（`0x31415926`）——OldMessage 内存被 gen1 集合的清扫/compact 覆写。

### 机制推断

old-gen 页分配与 gen1 region 从同一虚拟保留区内取址；当 old-gen 对象（如测试的 OldMessage）落到 gen1 活动区间 `[gen1->begin, gen1_bump)` 内，**gen1 collection 把它当作 gen1 对象清扫/compact → 破坏其内容与内部槽**。叠加 estimate-size（raw 对象无布局 → 最大 2048B 复制）使 gen1 填充模式与对象边界不可靠。

### 待确认

1. 生产路径下 old-gen 页分配是否会落入 gen1 活动区间（取决于两者虚拟保留策略——`RegionManager::SelectRegionSize` 与 old-gen `VirtualAllocExNuma` 保留的地址空间关系）。
2. gen1 collection 是否应对"落在 gen1 区间内的非 gen1 对象"有防护（如页归属校验）。
3. estimate-size 上限（`kMaxEstObjectSize=2048`）对 raw 对象晋升的边界正确性。

### 影响面

若生产可触发：**任何 old-gen 对象落入 gen1 区间 → 内存损坏**（P0 级）。需专项会话（建议走 `dev-systematic-debugging` + 地址布局验证）。

## 四、本轮处置

| 项 | 处置 |
|----|------|
| `gc_region_barrier_stress_test.cpp` | **回滚至 HEAD**（位置校验版）——快 gate 保持绿；内容校验原型见本记录（git history 可恢复） |
| `chaos_gc_dirty_card_dst_ref` | **保留修复**（发现 1）——严格更安全，三组测试全绿 |
| GC-N6 状态 | 发现已固化；测试强化本体待发现 2 修复后重新启用 |
| 发现 2 | 升级为独立 P0 专项（gen1↔old-gen 重叠） |

## 五、验证基线（本轮）

- `chaos_gc_region_barrier_stress_test`（位置校验）：5/5 exit 0
- `test_gc_card_table_ext`：5/5 PASSED
- `test_gc_region`：18/18 PASSED
