# gc-p2-02 大页支持 (THP/LargePages) 实现计划

> **面向执行 Agent：** 必须使用 dev:subagent-driven-development 或 dev:executing-plans 来执行本计划。步骤使用复选框（`- [ ]`）语法跟踪。执行完成时必须先归档当前子任务目录，再同步父任务。

**目标：** 在 GC 虚拟内存分配路径中增加 2MB 大页支持（Windows MEM_LARGE_PAGES + Linux THP MADV_HUGEPAGE），减少 TLB miss。

**架构：** 在 NUMA 内存分配层新增 `GcTryAllocLargePages()` API；旧生代页、LOH 段、Region 分配路径优先尝试大页，失败回退到 4KB 页。

**技术栈：** VirtualAlloc (MEM_LARGE_PAGES) / mmap + madvise (MADV_HUGEPAGE)

**架构审核模式：** normal

**结构告警重点：** 文件职责与拆分（大页逻辑应放在 gc_numa 层，不散落到各分配器）

**设计文档：** n/a（继承父 roadmap 边界，方案空间收敛，轻量级设计）

**问题清零来源：** parent-roadmap（roadmap-v1-01.md 已获批，blocking_questions=[]）

**计划来源：** roadmap-child-auto

**预期知识沉淀：** n/a

**收尾约束：** 执行完成后必须进入"结构告警与架构审视 -> 测试通过 -> 归档 completed -> 合并&提交"固定链路。

---

## 修改文件及职责

| 文件 | 职责 |
|------|------|
| `src/native/runtime-core/gc/gc_numa.h` | 声明 `GcTryAllocLargePages()` |
| `src/native/runtime-core/gc/gc_numa_win.cpp` | Windows 实现：`VirtualAlloc` + `MEM_LARGE_PAGES` |
| `src/native/runtime-core/gc/gc_numa_linux.cpp` | Linux 实现：`mmap` + `madvise(MADV_HUGEPAGE)` |
| `src/native/runtime-core/gc/gc_numa_other.cpp` | stub：always returns nullptr |
| `src/native/runtime-core/gc/gc_old_gen.cpp` | Old-gen 页分配：slab 式 batch 分配大页块 |
| `src/native/runtime-core/gc/gc_loh.cpp` | LOH 段分配：size ≥ 2MB 时尝试大页 |
| `src/native/runtime-core/gc/gc_region.cpp` | Region 分配：tenured region 尝试大页 |
| `CMakeLists.txt` | 新增 `CHAOS_IL2CPP_GC_LARGE_PAGES` 编译选项 |

## 实现步骤

### Step 1: gc_numa.h — 声明 GcTryAllocLargePages

```cpp
/// Try to allocate @a size bytes using OS large pages (2MB on most systems).
/// Returns nullptr if large pages are not available or allocation fails
/// (caller should fall back to normal page allocation).
/// @param size  Requested size (will be rounded up to large page alignment internally).
void* GcTryAllocLargePages(CHAOS_IL2CPP_SIZE size) noexcept;

/// Free memory allocated by GcTryAllocLargePages.
/// @param ptr   Pointer to free (must be from GcTryAllocLargePages).
/// @param size  Size passed to GcTryAllocLargePages.
void GcFreeLargePages(void* ptr, CHAOS_IL2CPP_SIZE size) noexcept;

/// Get the minimum large page size on this system.
/// Returns 0 if large pages are not supported.
CHAOS_IL2CPP_SIZE GcGetLargePageMinimum() noexcept;
```

### Step 2: gc_numa_win.cpp — Windows MEM_LARGE_PAGES

- `GetLargePageMinimum()` 获取大页大小（通常 2MB）
- `VirtualAlloc(nullptr, aligned_size, MEM_RESERVE | MEM_COMMIT | MEM_LARGE_PAGES, PAGE_READWRITE)`
- 前置条件：调用进程必须有 `SeLockMemoryPrivilege`
- 失败时返回 nullptr，不 LOG ERROR（正常回退路径）

### Step 3: gc_numa_linux.cpp — Linux THP

- `mmap(nullptr, aligned_size, PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_ANONYMOUS, -1, 0)`
- `madvise(ptr, aligned_size, MADV_HUGEPAGE)` 提示内核使用透明大页
- 即使 `madvise` 失败也返回指针（THP 是 hint，非强制），仅 LOG_DEBUG

### Step 4: gc_numa_other.cpp — Stub

- `GcTryAllocLargePages` → return nullptr
- `GcGetLargePageMinimum` → return 0

### Step 5: CMakeLists.txt — 编译选项

```cmake
option(CHAOS_IL2CPP_GC_LARGE_PAGES "Enable large page support for GC heap allocations" OFF)
target_compile_definitions(chaos_runtime_core PRIVATE
    $<$<BOOL:${CHAOS_IL2CPP_GC_LARGE_PAGES}>:CHAOS_IL2CPP_GC_LARGE_PAGES=1>)
```

### Step 6: gc_loh.cpp — LOH 段使用大页

- 在 `VirtualAllocPage()` 中，如果 `CHAOS_IL2CPP_GC_LARGE_PAGES` 定义且 `size >= 2MB`：
  - 先调用 `GcTryAllocLargePages(size)`
  - 成功则返回大页内存
  - 失败则回退到普通 `VirtualAlloc`
- 对应的 `VirtualFreePage()` 根据是否大页分配选择 `GcFreeLargePages` 或普通 `VirtualFree`

**区分大页 vs 普通页的方法：** 在 `LargeObjectHeap` 中为每个 segment 增加 `bool large_pages_allocated` 字段，分配时记录。

### Step 7: gc_region.cpp — Tenured Region 使用大页

- 在 `AllocateRegion()` 的 `REGION_TENURED` case：
  - 如果 `CHAOS_IL2CPP_GC_LARGE_PAGES` 定义且 region_size >= 2MB：
  - `region_size` round up 到 `GcGetLargePageMinimum()` 对齐
  - 调用 `GcTryAllocLargePages()`，成功则用大页，失败回退

### Step 8: gc_old_gen.cpp — Old-gen 页 slab 分配

Old-gen 页（64KB）远小于 2MB，需要 batch 分配：

- 在 `MarkSweepOldGen` 中增加：
  ```cpp
  // Large page slab state
  char* large_page_slab_ = nullptr;   // current 2MB slab base
  CHAOS_IL2CPP_SIZE large_page_slab_remaining_ = 0;  // bytes left in slab
  ```
- 新增 `AllocateFromLargePageSlab(CHAOS_IL2CPP_SIZE size)`：
  - 如果 slab 剩余足够，从中 carve
  - 如果不足，分配新的 2MB 大页块作为 slab
  - 返回 slab 中的地址
- 在 `AllocatePage()` 中，启用大页时优先从 slab 分配
- 在 `VirtualFreePage()` / `FreePage()` 中不做特殊处理（slab 在 `~MarkSweepOldGen` 时整体释放）

## 验证

1. **构建验证** — 两种配置均需无编译错误：
   - `cmake --preset debug`（大页关闭，确保不影响现有代码）
   - `cmake -DCHAOS_IL2CPP_GC_LARGE_PAGES=ON --preset debug`（大页开启）
2. **GC 回归测试** — 全部现有 GC 测试 PASS：
   - `chaos_gc_region_test`
   - `chaos_gc_old_gen_unit_test`
   - `chaos_gc_loh_test`
   - `chaos_gc_scheduler_test`
   - `chaos_gc_card_table_ext_test`
   - `chaos_gc_mark_bitmap_test`
   - `chaos_gc_sanity_test`
3. **基本功能验证**：大页开启时，分配若干对象并触发 GC，确认无崩溃

## 收尾链路

1. 结构告警与架构审视
2. 测试通过
3. 归档 completed — `mv docs/dev/in-progress/gc-p2-02-large-pages docs/dev/completed/12-gc-industrialization/`
4. 回写父任务 — 更新 roadmp 的 child mapping、摘要、recommended_next_child
5. 合并&提交
