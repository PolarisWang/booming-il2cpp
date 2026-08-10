# STATUS — GC-A1 RootsSet

## 元信息

```yaml
task_id: gc-align-coreclr/gc-a1-rootset
task_type: plan
phase: completed
roadmap_or_plan: docs/dev/in-progress/gc-align-coreclr/design-gc-a1-rootset.md
parent_task_id: gc-align-coreclr
source_relation: roadmap-child
clearance_source: parent-roadmap
blocking_questions: []
question_clearance: cleared
clearance_confirmed_by_user: true
entry_skill: dev-executing-plans
created: 2026-08-10
created_by: main-agent
```

## Scope

修复 young GC 全根集覆盖不完整（UAF）。将 `GcScanAllThreadRoots` 提升为精确优先 hybrid 根扫描，young GC Phase 0 扫全线程精确根，修正 `g_heap_base` 地址判据为 `IsInGCHeap`。

## Inputs

- `docs/dev/in-progress/gc-align-coreclr/design-gc-a1-rootset.md`（本子任务设计）
- `src/native/runtime-core/gc/gc_young_collector.cpp`（Phase 0）
- `src/native/runtime-core/thread_state.cpp`（`GcScanAllThreadRoots`）
- `src/native/runtime-core/gc/gc_root_scanner.h/.cpp`
- `src/native/runtime-core/gc/gc_card_table.h`（`g_heap_base`）

## Expected Outputs

- 修改：`gc_young_collector.cpp`、`thread_state.cpp`
- 修改：`tests/contracts/native/runtime-core/gc_young_collector_test.cpp`、`gc_gen1_test.cpp`
- 更新：`design-gc-a1-rootset.md`、本 STATUS

## 实现内容（步骤 1-5）

1. **步骤 1**（thread_state.cpp）：`GcScanAllThreadRoots` 候选根判据 `>= g_heap_base` → `>= g_heap_base || IsInNursery(read)`。因 `g_heap_base`=老年代base不是全堆下界，nursery 可低于它，旧过滤器漏掉 nursery 指针。short-circuit 保 fast path。
2. **步骤 2**（gc_young_collector.cpp Phase 0）：从「仅扫 current_thread 栈」→「`threading::GcScanAllThreadRoots` 扫全部 STW 挂起线程」，hybrid（保守全栈 + T4 帧 GcSlotMap 精确），回调对每个 nursery 根 promote 并写回 tenured。修正跨线程 UAF。
3. **步骤 3**：扫描器逐帧化已由 `GcScanAllThreadRoots` 既有的 Phase1保守 + Phase2 T4精确 hybrid 覆盖。
4. **步骤 4**：LOH/old-gen full GC + 压缩 relocation 回归验证全部通过。
5. **步骤 5**：已核实 AOT 寄存器根为**非缺口**（AOT 不做寄存器缓存，根恒在 C++ 栈局部），无需改动。

## 已提交

| commit | 内容 |
|--------|------|
| `6659812d4` | 移除 GC 核心 CHAOS_GC_STRESS（对齐 CoreCLR 零 stress） |
| `e4dae1f97` | GC-A1 young GC 全线程精确根扫描（步骤 1-2） |
| `f5ceb0072` | 修复 young_collector SUB-2 测试断言（pre-existing） |
| `b9311d2e9` | 修复 gen1 5 个 pre-existing 测试失败 |
| `3996e1471` | 记录 GC-A1 完成 + gen1 失败修复 |

## pre-existing 测试失败 — 已全部修复（2026-08-10）

两个 pre-existing GC 测试失败均已定位并修复（**均不改运行时**，只对齐测试与运行时实际）：

1. **`chaos_gc_young_collector_test` SUB-2**（`f5ceb0072`）：DirtyCard 对 nursery 指针有 fast-skip（young GC Phase 2 精确扫 nursery），测试对 nursery 卡断言 dirty 恒失败。改为直接对 nursery 区间卡表字节置位 + 验证 ClearCardRange 清除。

2. **`chaos_gc_gen1_test` 5 failures**（`b9311d2e9`）：
   - **Test2**：`NurseryAllocate(gen0_ref)` 首笔慢分配触发 young GC 刷新 Gen1 region 并重置 gen1_bump，孤立了先分配的 gen1_obj。修复：把 gen0_ref 提前到 gen1_obj 分配之前 settle。
   - **frag 测试**：硬编码 "1MB in 16MB" 期望，实际 `kDefaultYoungRegionSize=64MB`，1MB/64MB=0.984 恒 >0.95。修复：size-aware（分配 survivor/8，断言 0.80<frag<0.92）。
   - **high-survival**：50×64B 在 64MB 上 frag≈0.99996 恒 >0.2。修复：保持 kHighSurvObjs=50（static_assert span<4096 走 Tier-1 promote-all 保 "all promoted"）+ frag 相对 empty 基线断言。

## 最终验证（2026-08-10）

| 测试批 | 结果 |
|--------|------|
| 22 项确定性 GC 单测 | **全部 0 failures**（atomic 5/5、region 4/4、young_collector 5/5、gen1 11/11、loh 8/8、old_gen 6/6、poh 10/10、root_scanner 5/5、safepoint 4/4、handle 11/11、events/stats/parallel_mark/tlab/card_table_ext/scheduler/sanity/bit_utils 全部 0、finalizer 16/16） |
| `chaos_runtime_core` Debug | ✅ 编译通过 |
| 长跑 BGC/stress/smoke | 未纳入确定性批（worker_pool_smoke 等为长跑 smoke，独立跑） |

**GC-A1 全根集扫描 + 两个 pre-existing 测试问题已全部完成并提交。**

## 架构优先结论（步骤 5 核查）

AOT 寄存器根非缺口：AOT 不做寄存器缓存（colored vreg/GcRefRegs 仅 JIT 用），ObjectRef 恒在 C++ 栈局部（GcSlotMap 覆盖 localsBase/evalBase 槽），机器级跨 `chaos_safepoint_poll()` 由 ABI 保护 callee-saved。这是 AOT(JIT 无寄存器缓存) vs JIT(有) 的结构性差异，无需补 AOT spill。详见 `design-gc-a1-rootset.md` 步骤 5。
