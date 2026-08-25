# GC-N7 Release 基准调查 — YoungGcPauseUnderLoad SEH 崩溃 + gen1 基准 teardown AV

> 日期：2026-08-25 | 状态：2 个独立缺陷确认（1 已修 / 1 深度根因待 systematic-debugging 专项）
> 关联：GC-N7（Release 构建 GC 基准）、GC-N6（内容存活校验）、A2b

---

## 一、背景

GC-N7 目标：用 **RelWithDebInfo** 构建跑 GC 基准（分配吞吐 + young/gen1 GC 暂停），替换 Debug 数值。
本 session 构建完成后，`test_gc_throughput_benchmark` 的 `YoungGcPauseUnderLoad` **稳定失败**（SEH 0xc0000005），
gen1 benchmark 也在 teardown AV。这阻断了 GC-N7 产出 Release 基准。

## 〇、修复进度（2026-08-25 本轮）

| 缺陷 | 状态 | 效果 |
|------|------|------|
| 1. 析构 FreePage UAF | ✅ 已改 `gc_old_gen.cpp`（`~MarkSweepOldGen` unlink-before-free） | throughput teardown AV 消失 |
| 4. `ResizeGen1Region` 清空新 gen1 | ✅ 已改 `gc_region.cpp`（先 FreeRegion(old) 再发布 new） | gen1 不再被静默禁用；crash 率 ~80% → ~5-10% |
| 2. Phase 2 scan_ptr 越界 | 🔴 **残余**（同上 crash `gc_young_collector.cpp:537`，~5% 复现） | 12 跑后仍偶发；根因跨域待专项 |

**信号**：残余偶发 crash 仍是 `GcYoungCollection+0xaa3`（:537 `first_word = *obj`，obj=scan_ptr 未映射）。
年轻 GC Phase 2 的 `scan_ptr` 偶发到达未提交地址 -> `nursery_used`(bump) 与已提交区域的同步问题，
跨 gen1-active young GC + page_list_ 生命周期。已按三次修复规则停止本轮手动迭代，升级专项。

---

## 二、缺陷 1（✅ 已修）：`~MarkSweepOldGen` 析构 FreePage 读已释放页

### 证据（cdb 栈，gen1 benchmark + throughput benchmark 均复现）
```
~MarkSweepOldGen (gc_old_gen.cpp @ 90)  →  FreePage (gc_old_gen.cpp @ 415)
  415: if (p == page || !p->in_use.load(...)) continue;   // [page+0x25] 读已 VirtualFree 页 → AV
page = 0x...5d0000 / 0x...5f0000   (已被 VirtualFree(MEM_RELEASE) 的指针)
```

### 根因
`~MarkSweepOldGen` 遍历 `page_list_`，对每个节点调 `FreePage(page)`，但**从不把当前节点从链上摘除**。
`FreePage` 内层的 segment-sharing 检查（第 414-415 行）遍历整条 `page_list_` 读每个节点的 `in_use`。
于是释放第 0 页后，下次 `FreePage` 的解引用循环会读到**已 `VirtualFree(MEM_RELEASE)` 的上游兄弟节点** → Release 下未映射读 → AV。
Debug 下 `memcpy` 页仍保留（VirtualFree 行为/布局差异）→ 仅 Release 复现。

### 影响
- throughput benchmark：in-test AV 被 gtest SEH 吞掉后，逃逸于进程 teardown。
- gen1 benchmark：多轮 `GcGen1Collection》` 生成多页后 teardown AV（已可见）。

### 修复（✅ 已改 `gc_old_gen.cpp`，**未提交**）
析构时先 `page_list_ = next` 摘除头节点再 `FreePage(page)`，使后续 FreePage 内层扫描不再读到已释放节点。
```
验证：throughput benchmark teardown AV 消失（in-test SEH 现在干净逃逸/不再叠加 teardown AV）。
```

---

## 三、缺陷 2（🔴 OPEN，深度根因）：young GC Phase 2/3 扫描 `scan_ptr` 越过已提交 frontier → 未映射读

### 证据（cdb 栈，`--gtest_catch_exceptions=0`，~80% 复现于 GC3）
```
GcYoungCollection+0xaa3  ← gc_young_collector.cpp @ 537:
    const void* first_word = *static_cast<const void* const*>(obj);   // obj=scan_ptr=0x...821000 未映射 → AV
Caller: YoungGcPauseUnderLoad::TestBody @ 205 (GcYoungCollection())
locals: nursery_begin=0x...1e0a8, scan_ptr=0x...21000, check_limit=0x10000000010(垃圾), obj_size=0
```

### 触发链（每 run 一致）
```
GC1: usage=65536, promoted=4, OK
GC2: gen1_collection span=4352 (early_exit), gen1_resize 67108864->4194304
     reclaimed=18446744073709547456  ← = -4160（nursery_used < nursery_begin，unsigned 下溢）
GC3: Phase 2 扫描 scan_ptr 读到未映射 → AV
```
`reclaimed=-4160` 每次固定出现，不随 resize 幅度（32MB/4MB）改变 → **locals 已在 GC2 早期阶段被破坏**（`nursery_used` local 在 line 407 与 line 699 之间被覆写），resize 是伴随而非直接元凶。

### 已排除/未收敛点
- 破坏发生在 GC2 的 **Phase 1/2/3**（`reclaimed` 在 line 699 于 resize 之前计算，已是 -4160）。
- `ResizeGen1Region` 有**独立真 bug**（见四），但它清的是 gen1 指针，不直接解释 nursery local 破坏。
- gen1 benchmark 的 teardown AV 在**修复缺陷 1 后仍复现** → `page_list_` 本身被破坏（含指向未映射页的节点），
  说明存在**更底层的 old-gen page 生命周期 / page_list_ 完整性破坏**，横跨多轮 gen1 collection。

### 结论
根因**横跨 ≥2 域**（young GC Phase 扫描 + old-gen page_list_/Sweep + gen1 resize + 卡表 segment 残留），
是 Release-only、跨 GC cycle 累积的**页表/堆元数据破坏**。按 CLAUDE.md 深度调试触发（三次修复规则 + 跨域根因），
**升级 dev-systematic-debugging 专项**。缺陷 1 作为确定症状优先保留提交。

---

## 四、附带发现的独立真 bug（✅ 已修）：`ResizeGen1Region` → `FreeRegion(old)` 清空新发布的 gen1 指针

`gc_region.cpp` `ResizeGen1Region`（666-692）原实现：
1. line 679-681 发布新 `gen1_region=bgen1` + `gen1_end/bump`；
2. line 691 `FreeRegion(old_gen1->id)`；
3. `FreeRegion` 内（946-949）**无条件**把 `g_young_gen.gen1_region/gen1_end/gen1_bump` 清 nullptr
   （因 `r->kind == REGION_GEN1`，`r` 是旧 region）。

**结果**：每次 `gen1_resize` 后 `gen1_region` 变 nullptr，gen1 被静默禁用，后续全部幸存者直接进 old-gen。
**修复（✅ 已改，已构建验证）**：`ResizeGen1Region` 先 `FreeRegion(old_gen1->id)` 再 publish 新 gen1；删除重复的 FreeRegion 调用。
（修正后 gen1 保持 active，benchmark 每 cycle 执行 gen1 collection——crash 率从 ~80% 降到 ~5-10%。）

---

## 五、处置建议

1. **缺陷 1**：保留修复（析构 unlink-before-free），作为独立 commit（`root_cause`/`fix_strategy`/`regression_check`）。
2. **缺陷 4**：`ResizeGen1Region` 先 `FreeRegion(old)` 再发布 new，或在 `FreeRegion` 不清已替换的 gen1 指针。
3. **缺陷 2**：专项 systematic-debugging，重点：GC2 Phase 1 `ScanDirtyCardsInRegisteredSegments`
   是否经 resize 残留的旧 gen1 segment 扫到垃圾区并写坏 nursery local；是否 `page_list_` 在 Sweep/Coalesce 中被破坏。
4. GC-N7 Release 基准在缺陷 2 修复前暂用 gen1_benchmark + throughput 前三项（NurseryAllocate/OldGen/BgcLatency）数据。

## 六、本地重建能力（本 session 固化）

非 VS 环境需：`vcvars64.bat` 直连 + `msbuild` 全路径。见 `/tmp` 下的 build batch（后续 session 可复用）：
```
call "...\vcvars64.bat"
msbuild build\native\tests\...\test_gc_throughput_benchmark.vcxproj /p:Configuration=RelWithDebInfo /m
```
cmake --build 在此环境静默失败，必须走 msbuild。
