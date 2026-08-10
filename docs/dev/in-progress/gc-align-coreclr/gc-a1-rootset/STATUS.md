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
- 2026-08-10：步骤 1-2 已实现并编译通过。**已提交 `e4dae1f97`**（GC-A1 全根集扫描）。
- 2026-08-10：回归验证。`chaos_runtime_core` Debug 编译通过。11 项 root-scan/full-GC 测试 0 失败。**本改动零新回归。**

## 已提交

| commit | 内容 |
|--------|------|
| `6659812d4` | 移除 GC 核心 CHAOS_GC_STRESS（对齐 CoreCLR 零 stress） |
| `e4dae1f97` | GC-A1 young GC 全线程精确根扫描（步骤 1-2） |
| `f5ceb0072` | 修复 young_collector SUB-2 测试断言（pre-existing 失败） |

## 已知 pre-existing 失败（非本任务，需独立 debug）

`chaos_gc_gen1_test` 5 failures（gc_gen1_test.cpp:145-148/371/560）——**已确认 baseline 同失败，非 GC-A1 引入**。根因初步定位：Test 2 (TestSingleLiveObject) 的 `GcGen1Collection()` 在对 `NurseryAllocate(gen0_ref)` 之后调用时返回**空集合**（gen1_bump 已被 reset，objs=0），疑似 nursery 分配的 young GC 触发重置 Gen1 状态。`ClearNursery()` 试验未解决，需专门系统性调试（不草率打补丁以免掩盖运行时问题）。其余失败（fragmentation 372、low-frag 561）疑同源。

**注**：`chaos_gc_young_collector_test` SUB-2 已修复（commit f5ceb0072）。gen1 5 failures 是**独立、更深的 pre-existing 运行时 bug**，已记录为独立跟进项，不在 GC-A1 范围内草率修复。
