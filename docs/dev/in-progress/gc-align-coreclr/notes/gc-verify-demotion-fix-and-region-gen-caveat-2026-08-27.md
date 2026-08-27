# Demotion GcVerify 误判修复 + 深层 region-gen 表 bug 交接

> 日期：2026-08-27 | 承接：`gc-pre-existing-issue-fixes-batch` task#10（demotion GcVerify crash）
> 状态：demotion 部分已修并提交 `0adc9c12b`；深层 region-gen 表 bug 待专项

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

Related: [[gc-pre-existing-issue-fixes-batch]]
