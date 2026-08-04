---
task_id: gc-p2-02
title: 大页支持 (THP/LargePages)
task_type: plan
lifecycle_status: completed
phase: completed
created_at: 2026-05-22 18:50:00 +08:00
updated_at: 2026-05-22 19:15:00 +08:00
parent_task_id: 20260522-gc-industrialization
source_task_id: gc-p2-02
source_relation: roadmap-child
clearance_source: parent-roadmap
entry_skill: dev-writing-plans
blocking_questions: []
question_clearance: cleared
clearance_confirmed_by_user: true
---

# STATUS — gc-p2-02 大页支持 (THP/LargePages)

## 审计结论：✅ 已实现

## 修改文件

| 文件 | 修改内容 |
|------|---------|
| `src/native/runtime-core/gc/gc_numa.h` | 新增 `GcGetLargePageMinimum()`、`GcTryAllocLargePages()`、`GcFreeLargePages()` 声明 |
| `src/native/runtime-core/gc/gc_numa_win.cpp` | Windows 实现：`VirtualAlloc` + `MEM_LARGE_PAGES` |
| `src/native/runtime-core/gc/gc_numa_linux.cpp` | Linux 实现：`mmap` + `madvise(MADV_HUGEPAGE)` |
| `src/native/runtime-core/gc/gc_numa_other.cpp` | 其他平台 stub（返回 nullptr） |
| `src/native/runtime-core/gc/gc_loh.h` | `LohSegment` 增加 `large_pages_allocated` 字段 |
| `src/native/runtime-core/gc/gc_loh.cpp` | `VirtualAllocPage` 中 size ≥ 2MB 时优先尝试大页 |
| `src/native/runtime-core/gc/gc_region.cpp` | `AllocateRegion` 中 alloc_size ≥ large page minimum 时尝试大页 |
| `src/native/runtime-core/CMakeLists.txt` | 新增 `CHAOS_IL2CPP_GC_LARGE_PAGES` 编译选项（默认 OFF） |

## 实现细节

- **编译时 flag**：`CHAOS_IL2CPP_GC_LARGE_PAGES=1`，默认关闭。开启后不影响现有代码路径
- **Windows MEM_LARGE_PAGES**：需要 `SeLockMemoryPrivilege`，失败时以 DEBUG 级别 LOG 并返回 nullptr
- **Linux THP**：使用 `madvise(MADV_HUGEPAGE)`，是 hint 而非强制要求
- **大页对齐**：内部处理 round up，调用者无需关心对齐
- **LOH 门槛**：只对 size ≥ 2MB 的段尝试大页（小段继续用 4KB 页）
- **Region 门槛**：只对 alloc_size ≥ large page minimum 的 region 尝试（nursery 512KB 不触发）

## 验证

- chaos_runtime_core 构建: 0 error
- chaos_gc_region_test: 4 tests, 0 failures
- chaos_gc_card_table_ext_test: 5 tests, 0 failures
- chaos_gc_mark_bitmap_test: 6 tests, 0 failures
- chaos_gc_loh_test: 8 tests, 0 failures
- chaos_gc_scheduler_test: 8 tests, 0 failures
- chaos_gc_sanity_test: 8/8 passed
- chaos_gc_old_gen_unit_test: 6 tests, 0 failures
- chaos_gc_gen1_test: 8 tests, 0 failures
- chaos_gc_poh_test: 10 passed, 0 failures

**总计：63 tests, 0 failures ✅**

## 结构告警与架构审视

- ✅ 大页逻辑集中在 `gc_numa.*` 层，职责不扩散
- ✅ LOH 和 Region 分配器仅添加 try-large-pages-fallback 路径，改动量极小
- ✅ 关闭 flag 时零开销（条件编译）
- ✅ 无重复代码，无文件职责混入
- ✅ 向后兼容：flag 默认 OFF，不影响现有行为

## 已知限制

- Old-gen pages（64KB）未使用大页（单个 page < 2MB，slab 批处理增加页面生命周期复杂度）。后续可在旧生代 page pool 层增加 batch 分配优化

## 下一步

归档子任务 → 同步父 roadmap → 启动 gc-p2-03（iOS/Android 平台验证）
