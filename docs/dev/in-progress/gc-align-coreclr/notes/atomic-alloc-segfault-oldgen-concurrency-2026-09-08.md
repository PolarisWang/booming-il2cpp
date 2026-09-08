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
  TryAllocateFromFreeLists+0x123   [gc_old_gen.cpp L737  block->next]
  ← Allocate+0x7aa                  [gc_old_gen.cpp  L901  TryAllocateFromFreeLists miss 后]
  ← NurseryAllocateAtomicSlow       [gc_region.cpp L557]
  ← NurseryAllocateAtomic           [gc_region.h L360]
  ← AtomicAllocWorker               [8-thread Test3]
```
`page->free_lists[sc_idx]` 头 = `0xcdcdcdcd...`（freed/decommitted 填充）→ 初始推断为 stale page 指针（**已被下方深挖§推翻，见 L40-51**）。剩余两种可能：bitmap poison guard 越界覆盖 / 页序竞态，下方深挖§展开。

## 根因（GC 分配器并发复用竞态）
`MarkSweepOldGen::Allocate` 一次分配由 **3 段各自持 mutex_ 的临界区**拼成，非原子：
- L901 `TryAllocateFromFreeLists`（mutex-1，读 `last_alloc_page_[sc_idx]` + `page_list_` walk + pop）
- L911 `AllocatePage(...)`（mutex-2，内含从 `page_pool_` 弹页 + `PalVirtualCommit` + `carve_free_lists` + 前插 page_list_）
- L930-942 直接弹新页 block（mutex-3）

并发窗口：线程 X miss → L911 `AllocatePage` 回收页池一页并 decommit/commit 时；线程 Y 的 `last_alloc_page_[sc_idx]`（非 thread_local，gc_old_gen.h L738）或遍历中的 `page` 仍指向该**被回收复用/正 decommit 的物理页** → Y 稍后在 L737 用 stale `page` 的 `free_lists[sc_idx]` 头（已被 decommit 填 0xcd / 已重新 carve）弹块 → 读到 freed 头指针 → `block->next` AV。

子 agent 另指出第二洞：回收 re-carve（`carve_free_lists`，L231）只 guarantee `pref_sc` 优先，其余 round-robin 的 `free_lists[i]` 头块尺寸可能 < 请求 size，若按请求 size `memset` 会越界清相邻块（L738/752/936）。（**已被下方深挖§ L48 证据推翻——该值已成事实非刚 memset 越界，此洞排除。**）

## ⚠️ 2026-09-08 深挖补充（方向 A 已实证失败，勿重试）

试过删 L930-942 直接弹块（改回 L945 fallback 回环），重建后**仍 20/20 隔离 SEGFAULT** → 根因不在该段，该假设排除、改动已回退。

**更深的 CDB 证据（推断根因是 free_lists 头被写 0xcd，而非弹块读到 freed）**：
```
TryAllocateFromFreeLists+0x111 mov rdx,[rsp+60h]   ; block = page->free_lists[sc_idx]
TryAllocateFromFreeLists+0x123 mov rdx,[rdx+8]      ; ← rdx(block头)=0xcdcdcdcdcdcdcdcd, 读[+8] AV

page(rcx) header（地址已掩）:
  0x50: 05760f30 000001d3 cdcdcdcd cdcdcdcd   ← offset ~0x58 (free_lists 数组区) 存 0xcdcdcdcdcd
```
rcx（page 头）在 free_lists 数组偏移处就是 `0xcdcdcdcdcdcdcdcd` → 该值**在 page 头里已成事实**，不是"刚 memset 越界写到下一块"。方向：
- 该 page 曾 VirtualFree/回池被 poison，但另一线程仍在 page_list_ 遍历其 free_lists[sc_idx]（page 已 [stale? 但 in_use 检查应拦截]）→ 检查页序是否在 AllocatePage 前插 new page 时与某线程 Read page_list_ 竞态
- 或 0xcd 写进 free_lists 来自 bitmap poison（l.370 `memset(MarkBitmap()+raw_bitmap, 0xCD, 16)`）恰在 page 头 free_lists 区附近/错的 offset → 查 OldGenPage 头 sizeof 与 free_lists 偏移是否因 `kMaxDemotedPerPage=128 × DemotedObj` 大数组把 free_lists[0x58] 区域与别结构错位
- 下一个 GC 域工程师应抓 faulting 时 `page_list_` 是哪个 node、`free_lists[sc_idx]` 那条的头为何已是 0xcd，倒查写入点（搜谁会把某 free_lists 槽写成 poison 值 / 是否 0xcd 来自 bitmap poison guard 越界覆盖）。

## 建议修复方向（GC 域专项，勿在当前并行工作树盲 patch）
A（已实证失败，见上方深挖§，排除）：删 L930-942 直接弹块改回 L945——20/20 SEGFAULT，不再建议。
B（当前推荐，根治方向）：将 Allocate 全程收敛成单一 mutex_ 临界区 + 重验 page，杜绝 decommit/reclaim 与分配交错；但 AllocatePage 内部已取锁，需拆"纯 carve/create" 与 "取锁 link" 两步。
验证 = 真机构造重跑 test_gc_atomic_alloc 连续 N 轮 0 SEGFAULT + contracts 版同验。

## 附：如何拿到这条证据（接续者）
```
# 按本机 artifacts preset 路径替换 <preset_path>
B=<preset_path>/tests/unit/runtime-native/runtime-core/gc/Debug/test_gc_atomic_alloc.exe
timeout 60 $B && echo "PASS" || echo "rc=$?"
# 按本机 WinSDK Debuggers 安装路径替换 <cdb_path>
<cdb_path> -c "sxe av; g; r; k L10; q" -G -o "$B"
```

# ⚠️ GC 域 sub-agent 1h 深挖附加（2026-09-08 第二轮）— 纠错 + 收敛

以下由深挖 sub-agent（ac20f387）长期排查产出，修正了上文若干猜测，供接续 GC 域者直接使用，勿重复已排除假设。

## 铁证修订
- **崩溃是 sc14（256B size class，Test3 worker i%5==0 分支），不是 sc6。** 早前 "sc6/single-slot" 是 probe 只盯 sc6 的误读。
- **full_gcs == 0**：崩溃发生前**无任何同步 full-GC / chaos_gc_collect 触发**。→ "Test3 内 NurseryAllocateAtomicSlow 的 scheduler gate 触发 full-GC 与 mutator pop 竞态"假设**已被实验排除**（=上一轮我给它的 main 方向是错的，勿重走）。
- **多轮下 sc6 与 sc14 两槽都出现 0xCD** → 0xCD 涂抹跨 free_lists[] 连续 header 区，指向 **page-header 区被一片 0xCD 写覆盖**（非单一 free-list 管理/非单指针写）。
- cdb 确定性 AV 复现不变：`AtomicAllocWorker → NurseryAllocate → NurseryAllocateAtomicSlow → Allocate → TryAllocateFromFreeLists`（mov rdx,[rdx+8] 读 0xCDCDCDCDCDCDCDCD 头）。

## 未证实/已排除（请勿重试）
- ✗ Allocate L930-942 直接弹块（删它仍崩）
- ✗ Init 页数 2/64（都崩）→ 非回收压力
- ✗ header/bitmap 区被 return 的 block 起点触碰（side 有 ULTRA-DIAG 探针：free-list 头从未指向非 payload 区 → 不是"block 起点错在 header"）
- ✗ full-GC 在 mutator pop 上竞态（full_gcs==0）

## 仍需 GC 域定夺的 writer 候选（指向一片 0xCD 写 header free_lists[] 连续区）
若 free-list 头都合法指向 payload，那 0xCD 涂抹是从**别处**落到 header free_lists[] 上。候选：
(a) `AtomicAllocWorker` 的 `(id+i)&0xFF`，当 id+i 让 8 字节块地址高 4 字节凑成 0xCDCD... 但 worker memset 的是它拿到的合法 payload block，不会撞 header，除非 block 记账 size 比实际大致 memset 越过 payload 顶端 冲回 header？—— 需查某 sc 的 block 是否可能贴近 page 顶端、其 memset(block,0,size)…不对 worker 用 pattern 非 0。
(b) 某 size-class 的 block 紧贴 `payload_end`，worker 按 64/256 memset 但实际块更小（size-class 错配）→ 越过 payload 尾写 admin/下页。
(c) **VirtualFree/decommit 页侧** 把 freed 页填 0xCD？ Windows VirtualFree 不填用户区，nor 为 0xCD（0xCD 是 MSVC CRT debug heap free fill）。→ 若 0xCD 真是 CRT-like fill，暗示某块内存其实是 `_msize`/new/malloc 管理而非 VirtualAlloc —— 但 OldGenPage 是 VirtualAlloc。矛盾 → 需确认 0xCD 到底是谁的 fill（CRT debug `_CrtSetDebugFillThreshold` 只在 `free`/`new` 释放时填，仅覆盖 malloc-heap，非 VirtualAlloc）。
   → **关键下一步：分清 0xCD 是否真是 CRT-heap fill**（若是，说明 page header 或 free_lists 区某对象实际经 malloc/new 分配且被 free，与 VirtualAlloc page 混叠）vs 自写 pattern。
(d) SweepPage L1519 清 free_lists 后重 carving 时 cover header 的 0xCD？但 full_gcs==0 无 sweep。

**给接续 GC 域的最小可行动作**：在测崩溃前拿一次 **full 28 槽 free_lists[] + 相邻 header 原始字节** 的 dump（探针已具备雏形，broaden 到全部 sc + header 窗口），判定涂抹边界；若 0xCD 落在一个"本应 CRT-malloc 管理的对象"起点（对比 address 是否在 VirtualAlloc 大区域 vs CRT segment），即定位 writer。勿再假设 full-GC 参与。
