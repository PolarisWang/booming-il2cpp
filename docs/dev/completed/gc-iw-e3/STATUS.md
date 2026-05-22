---
task_id: gc-iw-e3
title: AsyncPinned handle + Gen1 部分保留（C19/C20）
task_type: plan
lifecycle_status: completed
phase: completed
parent_task_id: 20260523-crag-gc-industrialization-sequel
source_relation: roadmap-child
clearance_source: parent-roadmap
preflight_review: pass
auto_execution_decision: continue
created_at: 2026-05-23
updated_at: 2026-05-23
completed_at: 2026-05-23
current_dir: docs/dev/completed/gc-iw-e3
---

# STATUS — gc-iw-e3：AsyncPinned handle + Gen1 部分保留（C19/C20）

## 设计摘要

Phase E 第三个子任务，继承父 roadmap 边界拍板。覆盖两个差距项：

### C19 — AsyncPinned handle

CoreCLR GCHandleType::AsyncPinned 等价类型：强引用 + pin 的组合。用于 async/await 场景下确保回调对象不被 GC 移动或回收。

**实现**：
1. GcHandleEntry 新增 `bool async_pinned` 标志位
2. 新增 GcCreateAsyncPinnedHandle() — 创建强引用 + pin set 注册
3. GcFreeHandle 处理 async_pinned 的 pin set 移除
4. 托管侧 API 暴露 chaos_gc_create_async_pinned_handle

### C20 — Gen1 部分保留（3 代分代对齐）

**当前问题**：GcGen1Collection 在 Phase 4 将全部 live 对象 promote 到 Gen2，Gen1 实质是"drain"而非"partial retention"。

**改为**：使用 `gen1_prev_compact_end` 边界标记区分"老"对象和"新"对象：
- 老对象（地址 < gen1_prev_compact_end）：promote 到 Gen2
- 新对象（地址 >= gen1_prev_compact_end）：compact 在 Gen1 内保留
- 死对象：sweep（不保留）

## 计划文档

- `plan-v1-01.md`

## 完成证据

- arch_review: ok
- authority_review: n/a
- verification:
  - canonicalCommand: test_gc_gen1, test_gc_handle, test_gc_scheduler
  - formalObject: test_gc_gen1 (14/14), test_gc_handle (10/10), test_gc_scheduler (13/13)
  - summaryPath: n/a
  - eventsPath: n/a
  - consolePath: n/a
- verification_data:
  - refreshCommand: n/a
  - verificationOutputRoot: n/a
  - latestResultPath: n/a
- test_result: passed
- wiki: n/a
- next: archive-parent-roadmap
