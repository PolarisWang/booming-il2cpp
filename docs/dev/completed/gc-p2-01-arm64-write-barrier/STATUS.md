---
task_id: gc-p2-01
title: ARM64 硬件写屏障 — DirtyCard + SATB barrier 内存序修复
task_type: plan
lifecycle_status: completed
phase: completed
created_at: 2026-05-22 18:30:00 +08:00
updated_at: 2026-05-22 18:45:00 +08:00
parent_task_id: 20260522-gc-industrialization
source_task_id: gc-p2-01
source_relation: roadmap-child
clearance_source: parent-roadmap
entry_skill: dev-writing-plans
---

# ARM64 硬件写屏障 — DirtyCard + SATB barrier 内存序修复

## 审计结论：✅ 已实现

## 修改文件
- `src/native/runtime-core/gc/gc_card_table.h` — `DirtyCard()` 中 card store 前增加 release fence

## 修改内容
在 `DirtyCard()` 的 card store（`seg->cards[card_idx] = 0xFF`）前加入 `std::atomic_thread_fence(std::memory_order_release)`，仅对非 x86 平台生效（ARM64 上 emit dmb ish）。

## 验证
- Managed build: 0 error
- Native build (chaos_gc_region_test): PASS
- chaos_gc_scheduler_test: 8 tests 0 failures
- chaos_gc_card_table_ext_test: 5 tests 0 failures
- chaos_gc_mark_bitmap_test: 6 tests 0 failures

## SATB barrier 审计结论
`BgcSatbPreWriteBarrier()` 无需修复：codegen 在 store 前调用，函数调用边界阻止编译器重排序，ARM64 不会对同一地址的 load-after-store 重排序。
