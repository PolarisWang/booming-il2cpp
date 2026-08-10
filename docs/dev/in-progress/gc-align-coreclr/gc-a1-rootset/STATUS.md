# STATUS — GC-A1 RootsSet

## 元信息

```yaml
task_id: gc-align-coreclr/gc-a1-rootset
task_type: plan
phase: implementation
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
- `src/native/runtime-core/gc/gc_young_collector.cpp`（Phase 0：418-447）
- `src/native/runtime-core/thread_state.cpp`（`GcScanAllThreadRoots`：590-650）
- `src/native/runtime-core/gc/gc_root_scanner.h/.cpp`（`GcScanFrameHybrid`/`GcScanPreciseFrame`）
- `src/native/runtime-core/gc/gc_card_table.h`（`g_heap_base`）

## Expected Outputs

- 修改：`gc_young_collector.cpp`、`thread_state.cpp`、（可能）`gc_root_scanner.cpp`
- 新增：针对"线程 B 栈持有线程 A nursery 对象 → 存活"的回归测试（若 test 目录允许）
- 更新：`design-gc-a1-rootset.md` 进展、父 roadmap STATUS

## 实现顺序（经架构优先核查确认）

1. **步骤 1：修正地址判据** — `thread_state.cpp:633` 的 `>= g_heap_base` → `IsInGCHeap(ptr)`（组合 nursery/old/LOH 范围测试）
2. **步骤 3：扫描器逐帧化** — `GcScanAllThreadRoots` 从整栈保守扫重构为逐帧 `GcScanFrameHybrid`（精确优先，offstack/locals 用 GcSlotMap，未知帧回退保守）
3. **步骤 2：young GC Phase 0 改全线程** — `gc_young_collector.cpp` 遍历全部线程精确根
4. **步骤 4：回归** — LOH/old-gen full GC + 压缩 relocation 无回归
5. **步骤 5：已核实非缺口**（AOT 不做寄存器缓存，根在 C++ 栈局部），无需改动，仅记录差异

## Exit Criteria

- [ ] young GC 下，线程 B 栈持有线程 A nursery 对象 O → O 存活，无 UAF（针对性测试通过）
- [ ] AOT / JIT / 解释器三种帧下的跨线程引用均正确存活
- [ ] `chaos_gc_region_test` / `chaos_gc_atomic_alloc_test` 分配路径测试 0 回归
- [ ] LOH/old-gen full GC + 压缩 relocation 无回归
- [ ] `chaos_runtime_core` Debug 编译通过

## Terminal Notes

- 2026-08-10：架构优先核查完成。步骤 5（AOT 寄存器根）经核实为**非缺口**——AOT 不做寄存器缓存（colored vreg/gpr_color 仅 JIT 用），ObjectRef 恒在 C++ 栈局部（GcSlotMap 覆盖），风险从 🔴 降 🟢。设计文档已修正。
- 2026-08-10：步骤 1-2 已实现并编译通过：
  - **步骤 1（thread_state.cpp）**：`GcScanAllThreadRoots` 的候选根判据 `>= g_heap_base` → `>= g_heap_base || IsInNursery(read)`。因 `g_heap_base`=老年代base不是全堆下界，nursery 可低于它；现在 nursery 指针不再被过滤丢弃。short-circuit 保证常见路径（指针已 ≥base）零额外开销。
  - **步骤 2（gc_young_collector.cpp Phase 0）**：从「仅扫描 current_thread 栈」改为「`threading::GcScanAllThreadRoots` 扫描全部挂起线程」，回调中跳过无 GcSlotMap 帧保守+带 GcSlotMap 帧精确的 hybrid，对每个 nursery 根 promote 并写回 tenured 指针。修正了跨线程栈持有 nursery 对象被误回收的 UAF。
  - **关键发现**：`GcYoungCollection` 所有活跃调用点（gc_region.cpp:201/217/362 + gc_api.cpp + gc_coordinator.cpp:52）都先 `RequestGlobalSafepoint()`，故全线程根扫描在 STW 下安全。`PromoteNursery`（gc_region.cpp:968）无活跃调用者，为死路径。旧 Phase 0 注释"not at a STW safepoint"已过时。
- 2026-08-10：回归验证。`chaos_runtime_core` Debug 编译通过。GC 测试批 6 项中 5 项 0 失败（atomic_alloc 5/5、region 4/4、bump_cache 6/6、layout 4/4、mark_bitmap 6/6）。
- 2026-08-10：`chaos_gc_young_collector_test` 1 失败（SUB 2 "card should be dirty before collect"）——**已确认为 pre-existing，非本改动引入**（git checkout 到 HEAD baseline 重建后同位置同样失败）。根因是 `DirtyCard` 的 nursery fast-skip 与该测试的 nursery-range 设置交互，与全根集扫描无关。记录为已知失败，非本任务回归。

## 待完成

- 步骤 4：LOH/old-gen full GC + 压缩 relocation 回归 **已验证通过**（见下）
- 记录 pre-existing young_collector SUB-2 失败为已知，并修复（用户要求全部修复）

## 步骤 4 回归验证（2026-08-10，全部 0 失败）

我的改动直接影响的 root-scan / full-GC 路径测试全部通过：

| 测试 | 结果 |
|------|------|
| chaos_gc_root_scanner_test | ✅ 5/5（直接验证 GcScanAllThreadRoots/GcSlotMap） |
| chaos_gc_loh_test | ✅ 8/8 |
| chaos_gc_old_gen_unit_test | ✅ 6/6 |
| chaos_gc_poh_test | ✅ 10/10 |
| chaos_gc_safepoint_test | ✅ 4/4 |
| chaos_gc_finalizer_test | ✅ 16/16 |
| chaos_gc_handle_test | ✅ 11/11 |
| chaos_gc_events_test / stats / parallel_mark / tlab / card_table_ext / scheduler / sanity | ✅ 全部 0 |

**pre-existing 失败（非本改动，已确认 baseline 同失败）**：
- `chaos_gc_young_collector_test` SUB-2（card fast-skip 与 nursery range 交互）
- `chaos_gc_gen1_test` 5 failures（gc_gen1_test.cpp:145-148/371/560）

**本改动零新回归。** 剩余工作：修复上述 2 个 pre-existing 测试失败（用户要求全部修复）。
