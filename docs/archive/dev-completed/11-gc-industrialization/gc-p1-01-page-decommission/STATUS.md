---
task_id: gc-p1-01
title: Page decommission — 100%-free pages 归还 OS
task_type: plan
lifecycle_status: completed
phase: completed
created_at: 2026-05-22 18:00:00 +08:00
updated_at: 2026-05-22 18:10:00 +08:00
parent_task_id: 20260522-gc-industrialization
source_task_id: gc-p1-01
source_relation: roadmap-child
clearance_source: parent-roadmap
entry_skill: dev-writing-plans
---

# Page decommission — 100%-free pages 归还 OS

## 审计结论：✅ 已实现

实际代码审计确认：Page decommission 已在 `gc_old_gen.cpp` Phase 4b 完整实现。

## 实际实现细节

**代码位置**：`gc_old_gen.cpp` 第 3030-3094 行，`gc_old_gen.h` 第 514-530 行

**机制**：
1. BgcSweep() 中 Phase 4b 扫描 page_list_，检测 100%-free normal pages
2. 对符合条件的 page：从 page_list_ 移除 → VirtualFree(MEM_DECOMMIT) 释放物理页 → 加入 page_pool_
3. 池大小上限 `kMaxPoolSize = 16`（~1MB）
4. 超出上限的 pool entry：VirtualFree(MEM_RELEASE) 彻底归还 OS
5. 从 page_list_ 移除时清空对应的 `last_alloc_page_` 缓存，并 `page_count_--`
6. 之后 `RebuildPageArray()` 重建 page_array_ 确保并发读安全

**解决 free list dangling pointer 的方式**：不等到 SweepPage 把 free blocks 加入 free list 后再检查——而是在 Phase 4b 中从 page_list_ 整体移除 page，free list 自然不再包含该 page 的 block。

## 测试建议（可选增强）
- 可以补充一个单元测试：创建大量对象 → 释放全部 → 触发 full GC → 验证 page_count 回落到基线
- 但这不是阻塞项
