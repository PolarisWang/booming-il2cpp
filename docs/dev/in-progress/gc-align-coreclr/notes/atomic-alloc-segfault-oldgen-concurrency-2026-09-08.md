# atomic_alloc 确定性 SEGFAULT — old-gen 并发分配器 stale-page/recycle 竞态

## 状态：真 GC 缺陷已确认，修复待 GC 域专项（本次只取证据未 patch）

## 复现
`artifacts/presets/windows-x64-reference/tests/unit/runtime-native/runtime-core/gc/Debug/test_gc_atomic_alloc.exe`
隔离单跑 3/3 **SEGFAULT (rc=139)**，崩于 Test3（8 线程并发 `NurseryAllocateAtomic` → old-gen `Allocate`）。
不在 known-failures 清单内（过去被 "GC suite 全绿" 覆盖漏报）。CDB stack + register 铁证如下。

注意：`artifacts/native-runtime-core-test/Debug/chaos_gc_atomic_alloc_test.exe`（contracts 预设，同源码）隔离却 PASS——故 root 只在特定 preset/时序暴露，实为并发竞态。

## CDB 寄存器证据（AV @ TryAllocateFromFreeLists）
```
Access violation 0xc0000005, rip = TryAllocateFromFreeLists+0x123
  mov rdx, qword ptr [rdx+8]    ; rdx(=block 头) = 0xcdcdcdcdcdcdcdcd → 读 [cdcd..+8] fault
rax=0xe  rbx=0x0  rcx=<page>  rdx=cdcdcdcdcdcdcdcd  r8=0x2
Call site:
  TryAllocateFromFreeLists+0x123   [gc_old_gen.cpp @737  block->next]
  ← Allocate+0x7aa                  [gc_old_gen.cpp  L901  TryAllocateFromFreeLists miss 后]
  ← NurseryAllocateAtomicSlow       [gc_region.cpp @557]
  ← NurseryAllocateAtomic           [gc_region.h @360]
  ← AtomicAllocWorker               [8-thread Test3]
```
`page->free_lists[sc_idx]` 头 = `0xcdcdcdcd...`（freed/decommitted 填充）→ 说明遍历用的 `page` 或 `block` 引用了一个**已进 page_pool_ 回收 / 已 PalVirtualDecommit** 的页（stale page 指针）。

## 根因（GC 分配器并发复用竞态）
`MarkSweepOldGen::Allocate` 一次分配由 **3 段各自持 mutex_ 的临界区**拼成，非原子：
- L901 `TryAllocateFromFreeLists`（mutex-1，读 `last_alloc_page_[sc_idx]` + `page_list_` walk + pop）
- L911 `AllocatePage(...)`（mutex-2，内含从 `page_pool_` 弹页 + `PalVirtualCommit` + `carve_free_lists` + 前插 page_list_）
- L930-942 直接弹新页 block（mutex-3）

并发窗口：线程 X miss → L911 `AllocatePage` 回收页池一页并 decommit/commit 时；线程 Y 的 `last_alloc_page_[sc_idx]`（非 thread_local，gc_old_gen.h L738）或遍历中的 `page` 仍指向该**被回收复用/正 decommit 的物理页** → Y 稍后在 L737 用 stale `page` 的 `free_lists[sc_idx]` 头（已被 decommit 填 0xcd / 已重新 carve）弹块 → 读到 freed 头指针 → `block->next` AV。

子 agent 另指出第二洞：回收 re-carve（`carve_free_lists`，L231）只 guarantee `pref_sc` 优先，其余 round-robin 的 `free_lists[i]` 头块尺寸可能 < 请求 size，若按请求 size `memset` 会越界清相邻块（L738/752/936）。

## 建议修复方向（GC 域专项，勿在当前并行工作树盲 patch）
A（轻，推荐先验）：把 L930-942 这段"假设新页必含请求精确块"的直接弹块删除，改回 L945 的 `TryAllocateFromFreeLists` 回环（该路径做 in_use + sweep_lock + 精确 sc 校验）。避免在两个独立 mutex 区之间缓存 stale page。
B（根治）：将 Allocate 全程收敛成单一 mutex_ 临界区 + 重验 page，杜绝 decommit/reclaim 与分配交错；但 AllocatePage 内部已取锁，需拆"纯 carve/create" 与 "取锁 link" 两步。
验证 = 真机构造重跑 test_gc_atomic_alloc 连续 N 轮 0 SEGFAULT + contracts 版同验。

## 附：如何拿到这条证据（接续者）
```
B=artifacts/presets/windows-x64-reference/tests/unit/runtime-native/runtime-core/gc/Debug/test_gc_atomic_alloc.exe
timeout 60 $B → rc=139
"/c/Program Files (x86)/Windows Kits/10/Debuggers/x64/cdb.exe" -c "sxe av; g; r; k L10; q" -G -o "$B"
```
