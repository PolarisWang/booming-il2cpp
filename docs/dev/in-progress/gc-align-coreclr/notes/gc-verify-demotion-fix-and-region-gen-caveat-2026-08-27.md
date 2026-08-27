# Demotion GcVerify 误判修复 + 深层 region-gen 表 bug 交接

> 日期：2026-08-27 | 承接：`gc-pre-existing-issue-fixes-batch` task#10（demotion GcVerify crash）
> 状态：demotion 已修（`0adc9c12b`）+ CoreCLR 对标调研完成，region-gen 跨池 clobber 已分类为良性（`049d6e65c`）；**残留 A2b 未类型对象信号待专项**

## 一、demotion GcVerify 误判 — 已修复（commit `0adc9c12b`，仅 `gc_diagnostics.cpp`）

**根因**：`GcVerifyHeap` kFull（`gc_diagnostics.cpp` marked-object walk）把 old-gen 页 bitmap 里每个 marked bit 当"region-gen OLD + 合法 TypeInfo"的对象校验。
但 **in-place demotion 模型**（`gc_old_gen.h:79-85`）：demoted 对象是 live gen1-owned 对象，**仍驻留在 old-gen 页**且 mark-bit 保持置位，
而 `GetRegionGen` 正确分类它为 **Gen1（非 OLD）**。verify 工具早于 in-place 重构，对每个 marked demoted slot 刷 ERROR。

**实证**：全 suite 探针 `IsInDemotedSet(obj)` 显示——所有 `region-gen not OLD` 与 `first-word not valid TypeInfo` 诊断 **100% demoted=true**；
加 guard 后 demoted=true 归 0。同时消除后续 full-GC cycle 把已 sweep/poison 的 demoted 页当对象读的潜在 SEGFAULT。

**修法**：kFull marked-object 循环里，`IsInDemotedSet(obj)` 为真则 `continue`（legit gen1-resident in old-gen page）。冷路径诊断，无生产行为变化。

**验证**：`test_gc_demotion` 6/6 PASS（`CHAOS_GC_HeapVerify=2` 下 demoted=true 类误判归 0）。

## 二、深层 region-gen 表 bug — 交接专项（非 demotion 类，独立 bug）

demotion 误判消除后，kFull verify 暴露**另一批真实错误**（此前被 demotion 洪泛淹没）：

1. **~4k `region-gen not OLD` (demoted=false, gen=0) + ~96 (gen=1)**：old-gen 页里 marked 的**非 demoted** 对象被 `GetRegionGen` 分类为 young(0)/gen1(1) 而非 OLD(2)。
   怀疑根因：coarse 4MB region-gen 表 byte 被 co-located nursery/gen1 `SetRegionGen(young)` 冲掉（K1/K2b 类 last-writer-wins clobber），
   而 `GcGetRegionGenPhysical` 只对"物理在 nursery/Gen1 range"的地址权威，old-gen 页对象物理不在那是 → 回退到被冲坏的 4MB 表 → 误报 gen0/1。
   ⚠️ 若为真，写屏障读 gen0 → skip carding → **跨代边丢失/内存安全风险**。
2. **21 `bitmap poison clobbered`**：16-byte 0xCD poison guard（bitmap 尾部）被写为 0x0 → **真实 OOB bitmap 写**（需确认是否所有页都追加 poison，排除 verify 读边界假阳性）。
3. **15 `region->gen mismatch: old_gen_page expect_gen=2 got_gen=0`**：`GcVerifyRegionToGenerationMap` 报告 old-gen 页的 region-gen 映射为 0，与页实际 gen 不符——与 #1 同源（页 base 的 4MB 表 byte 被冲）。

**与并行轨道的边界**：并行轨道当前在改 `gc_old_gen.cpp`（mark-watchdog S2/DIVERGE 诊断，与 bitmap/region-gen 无关）、`gc_parallel_mark.cpp`、`gc_worker_pool.cpp`。这些 #1-#3 的根因调查会触碰 `gc_region.cpp GetRegionGen/GcGetRegionGenPhysical` + `gc_old_gen.cpp` 页注册路径，**可能与并行轨道撞车**，需定界后专项。

**下一步入口**：先确认 #1 是否为 4MB 表 clobber（用 probe 打印这些 gen0 地址的 `GetRegionGen` 是否物理 old-gen、其 4MB chunk 是否与 nursery/gen1 共享）；修法可能是把 old-gen 页 base（及 LOH/payload）的 gen 显式 `SetRegionGen(...,OLD)` 权威化（对齐 `GcMarkRangeOld` 的既有手段）。详见 [[gc-n6-mode3-content-wiring-iter16-bug]] [[gc-m11-config-knobs-done]]。

## 三、CoreCLR 对标调研结论（2026-08-27，定界 + 提交 `049d6e65c`）

**CoreCLR 权威机制**（本地树 `D:/OpenSource/dotnet/runtime/src/coreclr/gc/`）：
- `set_region_gen_num`（gcinternal.h:4452）分配 segment 时把 gen 写到覆盖所有 basic region 的 `map_region_to_generation[idx]`；`region_segments_alloc`（regions_segments.cpp:1270-1291）**所有代都走唯一分配器**、每次显式自标 gen。
- 归属不变式：每个 4MB basic region 完全属于**一个** gen segment，`get_region_gen_num` 读表并 `assert(map == region_of(obj).gen_num)`（regions_segments.cpp:1096）——**绝无跨分配器 last-writer-wins clobber**。

**Chaos 差异**：`MarkSweepOldGen::AllocatePage` 是独立 VirtualAlloc 池（非 region 分配器），可与 nursery/Gen1 **共享 4MB chunk**，nursery `SetRegionGen(young)` 覆盖 old-gen 页 chunk byte → gen0。CoreCLR 靠"不共享"杜绝，Chaos 无法廉价复刻同一 ownership。

**定案：gen0 是良性，非 live bug**。写屏障 dst 侧已 CoreCLR 对齐地精确化（`chaos_gc_dirty_card_dst_ref` 用 `IsNurseryPointer(dst)` 而非 4MB 表，gc_card_table.cpp:117-129），故 old-gen 对象读 gen0 **不丢边**；young 侧由 `GcGetRegionGenPhysical` 物理权威保护。既有的 `get_region_gen_num == OLD` verify 断言是"表必须 OLD"的不变式，在共享 chunk 架构下**无法恒成立**（CoreCLR 靠单一分配器保证，Chaos 不行）。

**落地（commit `049d6e65c`，仅 `gc_diagnostics.cpp`，冷路径）**：
1. `CheckRegionGenOldClobberAware`：old-gen/LOH 页读非 OLD 仅当**物理 nursery/gen1**（真冲突）才 ERROR；物理 old-gen 但 chunk byte 被跨池 young clobber → **WARN 良性**。应用于 `GcVerifyRegionToGenerationMap` old-gen/LOH + `GcVerifyHeap` kFull region-gen 检查。
2. `bitmap poison` 检查 **`#if CHAOS_IL2CPP_DEBUG`** 门控（poison 只在 debug 写，非 debug 恒读 0 = 假阳性）。
3. 不采纳"OLD chunk 不被 young 覆盖"（会破坏共享 chunk 里合法 young 对象 → 反成 UAF 源）；不采纳把 `IsInOldGen` 塞进 hot `GetRegionGen`（O(log n) barrier 变慢）。

**残留 A2b 未类型对象信号（待专项）**：kFull 仍报 ~4k `first-word not valid TypeInfo`——**非 interior-slot 噪声**（标记槽全无合法 TypeInfo head，是 genuinely untyped/raw old-gen 对象），即 A2b raw-object 类信号（`[[a2b-true-rootcause-fullgc-sweep-raw-objects]]`）。本轮已验证不是 interior（连续 8 槽全无 head TypeInfo）。需判断这些 raw 对象是否是测试态伪影或生产合法路径，勿与 region-gen clobber 混淆。

Related: [[gc-pre-existing-issue-fixes-batch]] [[a2b-true-rootcause-fullgc-sweep-raw-objects]] [[coreclr-region-barrier-solution-reference]]
