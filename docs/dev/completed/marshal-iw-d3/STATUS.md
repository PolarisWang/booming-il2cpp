---
task_id: d3-concurrent-fix
parent_task_id: marshal-industrialization
source_relation: roadmap-child
phase: D
lifecycle_status: completed
entry_skill: dev-executing-plans
clearance_source: parent-roadmap
blocking_questions: []
question_clearance: cleared
clearance_confirmed_by_user: true
---

# Phase D3 — ICustomMarshaler 自旋锁并发修复

## Scope

修复 `marshal_api.cpp` 中 ICustomMarshaler 缓存的并发安全问题。添加 `std::atomic_flag` 自旋锁保护所有缓存读写路径。

## 产物

- `src/native/runtime-core/core/marshal_api.cpp` — 添加自旋锁 + CHAOS_SPIN_HINT 后备定义

## Exit Criteria

- 自旋锁保护以下路径：
  - `ResolveOrCreateMarshaler` 的缓存扫描和写入
  - `CustomMarshalerNativeToManaged` / `ManagedToNative` 的 slot 查询
  - `CustomMarshalerCleanupNativeData` / `CleanupManagedData` 的 slot 查询
  - `ClearMarshalerCache` 的缓存清空
- 锁不保护类型解析和方法调用（避免死锁）
- `CHAOS_SPIN_HINT` 后备定义确保独立 TU 编译安全
