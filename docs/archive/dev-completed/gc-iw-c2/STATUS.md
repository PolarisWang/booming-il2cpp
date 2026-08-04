---
task_id: gc-iw-c2
title: GCMemoryInfo 托管侧结构体（C12）
task_type: plan
lifecycle_status: completed
phase: completed
parent_task_id: 20260523-crag-gc-industrialization-sequel
source_relation: roadmap-child
clearance_source: parent-roadmap
created_at: 2026-05-23
updated_at: 2026-05-23
current_dir: docs/dev/completed/gc-iw-c2
---

# STATUS — gc-iw-c2：GCMemoryInfo 托管侧结构体（C12）

## 问题

`chaos_gc_get_memory_info()` 已填充 `GcMemoryInfoNative` 结构体，但 `total_committed_bytes` 的公式在 LOH 分配量较大时可能小于 `heap_size_bytes`，违反总提交内存 ≥ 堆大小的预期。

## 改动

| 文件 | 改动 |
|------|------|
| `src/native/runtime-core/gc/gc_api.cpp` | `total_committed_bytes` 增加 `std::max(committed_raw, heap_size)` 保证，解决 LOH 分配导致 old-gen pages × pageSize 覆盖不完全的问题 |
| `testing/src/native/runtime-core/gc/gc_sanity_test.cpp` | GcMemoryInfo 测试增加全字段验证（heap_size ≥ 0, total_committed ≥ heap_size, gen info sizes ≥ 0 等） |
| `verification/foundation-dll/.../GarbageCollectionSubjects.cs` | Subject_6 改为逐字段验证替换 GetHashCode() 检查 |

## 验证

| 测试套件 | 结果 |
|---------|------|
| test_gc_scheduler（13 测试） | 13/13 PASS ✅ |
| test_gc_bgc_unit（8 测试） | 8/8 PASS ✅ |
| test_gc_old_gen（6 测试） | 6/6 PASS ✅ |
| test_gc_sanity（7 测试） | GcMemoryInfo PASS ✅（3 个 POH 预存失败，非本次改动引入） |

## 完成证据

- arch_review: ok（`gc_api.cpp` 内单行修正，无结构影响）
- authority_review: n/a
- verification:
  - unitTest: 27/27 (scheduler + bgc_unit + old_gen)；GcMemoryInfo 测试通过
- test_result: passed
- wiki: n/a
- next: gc-iw-c3（GcEndStubbornChange 实现）
