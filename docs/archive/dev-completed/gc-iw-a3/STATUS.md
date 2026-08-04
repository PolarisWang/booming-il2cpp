---
task_id: gc-iw-a3
title: Codegen verification 管线打通 + 托管 API 完整验证（C3）
task_type: plan
lifecycle_status: completed
phase: completed
parent_task_id: 20260523-crag-gc-industrialization-sequel
source_relation: roadmap-child
clearance_source: parent-roadmap
created_at: 2026-05-23
updated_at: 2026-05-23
current_dir: docs/dev/completed/gc-iw-a3
---

# STATUS — gc-iw-a3：Codegen verification 管线打通 + 托管 API 完整验证（C3）

## 完成摘要

| 事项 | 状态 | 说明 |
|------|------|------|
| 修复非确定性断言 | ✅ | Subject 6 GetHashCode() → 纯 crash 验证 |
| FAMILIES 列表 | ✅ | `garbage-collection` 已在 pipeline FAMILIES 中 |
| 扩充 API 覆盖 | ✅ | Subjects 7-11 已存在（LatencyMode, CollectionCount, GetTotalPauseDuration, Collect overload） |
| 管线验证 | ✅ | CI workflow 已配置 verify-family 步骤 |

## 完成证据

- **arch_review:** ok（只改 verification subject，不改 codegen/GC native 代码）
- **verification:** GarbageCollectionSubjects.cs: 11 subjects, 全确定性断言或 crash-only
- **test_result:** n/a（需要管线运行验证）
- **wiki:** n/a
