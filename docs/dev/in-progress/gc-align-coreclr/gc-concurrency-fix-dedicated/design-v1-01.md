# design-v1-01 — 修复 young-GC 跨代崩溃：两层根因设计方案

> 域：GC(6) + 运行时(2)。日期：2026-08-11。本设计承接
> `gc-concurrency-fix-handoff.md` 的"修正"章节（已证伪 store-then-barrier 假设），
> 针对专门会话逐层实验定位到的**两层真实根因**。用户已要求设计两层修复方案。

## 背景与问题

`tests/contracts/native/runtime-core/gc_region_barrier_stress_test.cpp` 是 committed
known-failing 复现器。逐层实验后定位到两层独立根因：

- **层1（随机失败，HEAD 5/10）**：旧代/LOH 页由独立 VirtualAlloc 池分配，只注册 card 表、
  从不写 region-gen 表。`GetRegionGen` 用 4MB 粒度，nursery/Gen1 Region 的
  `SetRegionGen(r->begin, young=0)` 会把落在同一 4MB chunk 的旧代字节**覆盖成 0**。
  2 参屏障 `if dst_gen==0 return` → 跳过 carding → 边丢失 → 悬挂。
- **层2（确定 212/thread7，层1 修复后暴露）**：屏障正确 card 后，Phase-1 确实扫到全部
  旧 message 页卡，但仍有 212 个 ref 未晋升/回写。`kMaxPromoteObjects`=256K 已排除。
  指向 young-GC Phase-1 的**扫描覆盖 / 晋升回写 / IsInOldGen 判定**路径。

`kMaxPromoteObjects` 与 SPB 并发均已实验排除 → 层2 是确定性 GC 晋升路径缺陷。

## 三层优先级

- P1 性能最优 > P2 架构完美 > P3 热更适配。
- 层1 的 `IsNurseryPointer` 在 barrier 热路径增加一次 lock-free range 查询，代价可接受
  （2 参 barrier 本就不是绝对 hot path；单参 `DirtyCard` 不变）。
- 层2 修复不改变扫描复杂度，只是让已卡住的边被正确晋升/回写。

## 方案拍板（推荐）

### 层1：`chaos_gc_dirty_card_dst_ref` 改为 `RegionManager::IsNurseryPointer` + 旧代/LOH 页显式标 OLD

**为什么选它**：根因是"4MB region-gen 字节碰撞 + 旧代页不归 RegionManager"，而
`RegionManager::IsNurseryPointer` 是 **lock-free、精确** 的 range 判定（不依赖碰撞的 4MB 字节）。
同时显式把旧代/LOH 页 region-gen 标 OLD，双保险，且不再被 nursery 覆盖影响（barrier 不再读
region-gen 判 young）。

改动点：
1. `src/native/runtime-core/gc/gc_region.h/.cpp`: 新增公共 `GcMarkRangeOld(start, end)` —
   EnsureRegionGenCoverage + 逐 4MB chunk SetRegionGen(OLD)。实现放 gc_region.cpp。
2. `src/native/runtime-core/gc/gc_old_gen.cpp` `MarkSweepOldGen::AllocatePage`: 在
   `GcRegisterHeapRange` 后调 `GcMarkRangeOld(page, page+size)`。
3. `src/native/runtime-core/gc/gc_loh.cpp` LohSegment Allocate: 同 2。
4. `src/native/runtime-core/gc/gc_card_table.cpp` `chaos_gc_dirty_card_dst_ref`:
   将 `dst_gen==0` / `ref_gen` 的 region-gen 判定替换为
   `IsNurseryPointer(dst)` / `IsNurseryPointer(ref)`：
   ```
   if (IsNurseryPointer(dst)) return;      // 旧：dst_gen==0
   if (ref==nullptr || !IsNurseryPointer(ref)) return;  // 旧：ref_gen>=dst_gen
   DirtyCard(dst);
   ```

> 单参 `chaos_gc_dirty_card(dst)`（解释器/部分 codegen 用）不受影响：`DirtyCard` 只用
> `g_nursery_range_begin/end` 跳过 nursery，从不读 region-gen，已是正确的。

### 层2：先加确定性诊断，再按命中分支修晋升路径

**为什么诊断优先**：层2 有 3 个候选分支（扫描覆盖 / 晋升回写 / IsInOldGen 判定），当前证据
（8 卡全扫到却仍毒 212）不足以唯一锁定。设计先加一项确定性诊断把 b/c/d 分支量化，据实修，
避免盲目改 Phase-1 引入新错。

诊断（临时，定位后移除）：
- 在确定性 212 下，遍历 thread7 的 128 个 slot，对每个 ref 统计：
  - `IsInNursery(ref)` 为 false（已被晋升 / 合法 old-gen / 真毒）
  - `IsForwarded(ref)`（晋升过但 slot 未更新 → Phase-1 或 Phase-2 未回写）
  - 未转发未晋升（Phase-1 没扫到这个 slot 的卡）
  - 晋升后 `IsInOldGen(tenured)` 判定结果
- 据 b/c/d 命中分布，选对应修复：
  - **b 命中**（已晋升但 slot 未更新）：Phase-1/Phase-2 回调对 `IsInNursery(val)` 为 true
    的 slot，统一用 `GcScavengeObjectKnownNursery` 返回值回写；对已 `IsForwarded(val)`
    的直接回写 forward 地址。确保所有被旧→nursery 边引用的 nursery 对象所在 slot 都被更新。
  - **c 命中**（卡扫到了但对象在 Phase-1 后又写入/未转发）：处理并发/边界的补扫窗口。
  - **d 命中**（晋升后 `IsInOldGen(tenured)!=true`）：修 `IsInOldGen` 的 page_array 覆盖
    或晋升地址不属于已发布 page → 回到层1 GcMarkRangeOld / page 注册一致性。

参考实现（Phase-1 回调，`gc_young_collector.cpp:461-470`）当前：
```cpp
if (val != nullptr && IsInNursery(val)) {
    void* tenured = GcScavengeObjectKnownNursery(val, &result);
    if (tenured != nullptr) { *ptr_slot = tenured; }
}
```
若诊断显示 `IsForwarded(val)` 但 slot 未更新，应改为对 forwarded 也回写 effective 地址；
确保 `GcScavengeObjectKnownNursery` 返回的 tenured（含转发路径）都被回写。

## 关键文件

- `src/native/runtime-core/gc/gc_card_table.cpp`（barrier）+ `.h`
- `src/native/runtime-core/gc/gc_region.h/.cpp`（GcMarkRangeOld + IsNurseryPointer）
- `src/native/runtime-core/gc/gc_old_gen.cpp`（AllocatePage 标 OLD）
- `src/native/runtime-core/gc/gc_loh.cpp`（LohSegment 标 OLD）
- `src/native/runtime-core/gc/gc_young_collector.cpp`（Phase-1/2 晋升回写）
- `tests/contracts/native/runtime-core/gc_region_barrier_stress_test.cpp`（复现/验证）

## 验证标准

1. 复现器连续跑 **15x `0 failures`**（当前 HEAD 5/10、层1修复后确定性 10/10 失败）。
2. 层1 单独落地后：随机悬挂应消失，确定性 212 暴露 → 证明层1 组件的正确性。
3. 层2 落地后：确定性 212 → 0。
4. `python tests/runner/test_driver.py --layer unit`：GC 全量单测无回归（无非 known FAIL）。
5. 多平台：`IsNurseryPointer` 是纯 C++ lock-free range（std::atomic），x86/ARM 均正确。
6. 热更/域卸载：解释器走单参 `DirtyCard` 路径不受影响；域卸载 0-reg 语义保持。

## 风险评估

| 风险 | 触发 | 后果 | 缓解 |
|------|------|------|------|
| 层1 barrier 热路径代价 | `IsNurseryPointer` 每次 2 参 barrier 查询 | 性能略降 | 2 参 barrier 非 hot path；bounds-check 先过滤 O(1) |
| 层2 诊断后命中分支猜测错误 | 未 lock 根因 b/c/d | 修错位置 | 诊断把 b/c/d 量化，据实修；不强改 Phase-1 |
| `GcMarkRangeOld` 覆盖 nursery Region | 旧代 chunk 与 nursery Region 同 4MB | nursery 被误标 OLD | barrier 改用 IsNurseryPointer 后不再读 region-gen 判 young；IsInNursery 仍按 range |
| 晋升 slot 回写引入二次更新 | 已 forwarded 强制回写 | 性能/正确性 | 仅对 `IsInNursery`/`IsForwarded` 命中回写，幂等 |

## watch_items

- thread7 确定性：验证层1 落地后 thread7 是否仍是唯一悬挂线程（属于层2 现象）。
- `IsInOldGen(tenured)` 对 LOH 晋升对象的覆盖（层2 d 分支）。
