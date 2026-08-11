# 交接 — 专门会话：修复 young-GC 跨代 rescan 的 store-then-barrier 并发竞态

> 日期：2026-08-10。本会话已精确定位根因，交接给专门会话修复。
> 分支：`main`（与 origin/main 同步）。复现器已提交推送。

## 一句话根因（已诊断证实）

**多线程"写索引 + 卡屏障"非原子 vs 并发 GC**：
`Worker: old_slot[i]=nursery_obj; chaos_gc_dirty_card_dst_ref(old_slot,...)` 是两步。
若协调 GC 的 safepoint 落在两步之间，young-GC Phase-1 扫该 old 页时卡仍 clean，
看不到刚写的 nursery ref → 该对象被回收（first_word 清零）→ 之后设卡已晚 → dangling。

## 复现（4/5，高度可复现）

```
test: tests/contracts/native/runtime-core/gc_region_barrier_stress_test.cpp
run:  artifacts/native-runtime-core-test/Debug/chaos_gc_region_barrier_stress_test.exe
      （或 ctest -R region_barrier -L stress，或 --stress-only）
结果: 约 4/5 失败（0 dangling 断言触发）
诊断: old_slot region_gen=2（屏障设卡决策正确）、ref region_gen=0（短路正确）、
      first_word=0x0（对象被回收）→ 确认不是屏障卡决策问题
```

## 为什么不是单个函数 bug

- `chaos_gc_dirty_card_dst_ref`（K2b）3 短路逻辑正确（单测 6/6 过）。
- 问题在 **store 与 barrier 之间的 safepoint 窗口**。
- 真实 AOT/JIT 生成代码（store 后紧跟 barrier）也可能踩中——窗口窄但存在。
- 是**内存序 / 屏障放置**的架构级并发问题，不是逻辑错。

## 修复方向（专门会话选用，需设计，三选一或组合）

1. **先标卡再写索引（swap 顺序）**：但必须验证与 young-GC Phase-1 扫描语义兼容
   （CoreCLR 是先写 ref 再标卡；swap 需证明不漏）。
2. **store+barrier 放进 no-safepoint 窗口**：JIT/AOT 发射时让这两条指令之间不允许
   GC（禁 poll / 嵌套 no-GC region）；生成代码侧改动 + 验证。
3. **young-GC Phase-1 补扫边界**：对"本周期可能有未设卡写入"的 old 页加温和重扫
   / 或让 coordinator GC 在 safepoint 生效前等待在途 barrier 完成。

**推荐先评估方案 2**（最贴近 CoreCLR 语义：JIT 在 GC 点前让引用状态稳定）。

## 关键文件

- 复现器：`tests/contracts/native/runtime-core/gc_region_barrier_stress_test.cpp`
- 屏障：`src/native/runtime-core/gc/gc_card_table.cpp`（`chaos_gc_dirty_card_dst_ref`）
- young-GC Phase-1 重扫：`src/native/runtime-core/gc/gc_young_collector.cpp:449-490`
- 卡表：`src/native/runtime-core/gc/gc_card_table.h`（`CardBundle*`/`DirtyCard`）

## 验证标准

- 复现器连续跑 10 次 0 失败（当前 ~4/5 失败）。
- 全量 GC 单测无回归。
- 多平台指针语义（`std::atomic`/barrier 内存序）无平台差异；JIT/AOT 两态验证。
- 热更（解释器路径）不受影响；域卸载仍 0-reg 语义。

---

## ⚠️ 修正：专门会话 2026-08-10 实测发现"store-then-barrier"诊断不完整

本专门会话复现并逐层分离，发现**真正根因与上方"store-then-barrier 竞态"不同**（该假设已被
实验否定），已回滚所有代码改动（复现器保持 HEAD 已知失败状态）。证据链：

1. **HEAD 基线**被证明确实失败：5/10，悬挂数随机（53 / 212），thread 不一定（多为 7）。
2. **DIAG-ALLOC（分配后立即）**：8 个 old-gen message 对象 `region_gen=2`（正确）；
   **workers 跑 30ms 后**：4 个（old[0-3]）`region_gen=0(young!)`、`IsDirty=0`。
   → 屏障 `if dst_gen==0 return` **跳过 carding** → 这些 old→nursery 边永不进卡 → young-GC
   Phase-1 漏扫 → 被引用对象被收集 → dangling。
3. **region-gen 字节踩踏**：`SetRegionGen` 用 4MB 粒度(`kRegionGenShift=22`)；旧代页(VirtualAlloc
   独立池)与 nursery/Gen1 Region 落在同一 4MB chunk → nursery `SetRegionGen(r->begin, young=0)`
   覆盖旧代字节 → 我的 `GcMarkRangeOld` 被后续覆盖回 0。**不是屏障逻辑错，是"4MB region-gen 粒度
   碰撞 + 旧代页不归 RegionManager 管理"**。
4. **修复尝试**：`GcMarkRangeOld`(alloc 标记 OLd) + 屏障改 `RegionManager::IsNurseryPointer`
   （精确实时 nursery range，非 4MB 字节）→ 悬挂从随机(53/212) 变成 **确定性 212/thread=7 恒定**。
   → **证明还藏一层：即便屏障正确 card 所有 old-gen，young-GC 扫描/晋升仍系统性漏掉 thread-7 的
   引用（或晋升后 `IsInOldGen(tenured)` 判定为 false 计为 dangling）。** 需继续查
   `gc_young_collector.cpp` Phase-1/2 对 thread-7（最后写的）old 页的扫描覆盖与晋升路径。
5. **SPB 假设被否定**：worker 在 store+barrier 前加 blocking `SafepointPoll()` 仍确定性 212/thread=7
   → 不是 SPB ack-and-conting 真并发，而是**确定性 GC 扫描/晋升缺陷**。

## 下一步（专门会话继续时）

- 追 `gc_young_collector.cpp` Phase-1：thread-7 的 old-gen message 页是否真被
  `ScanDirtyCardsInPagesBatched` 扫到（`page_list_`/`in_use`/扫到的 card cell 数）。
- 追晋升（`GcScavengeObjectKnownNursery`）后 slot 是否更新为 tenured 地址、`IsInOldGen(tenured)`
  判定。`cards=8` 扫描 vs 8 message 对象 × ~3 card cell 的预期 ~24，倾向"扫描覆盖不全"。
- 修复方向建议：a) 旧代页归属原生 region 或改用精确 nursery-range 判定（已实验）；b) Phase-1 扫描
  覆盖 thread-7 页；c) 晋升 slot 回写 + IsInOldGen 一致。**三选一或组合，需设计拍板。**

