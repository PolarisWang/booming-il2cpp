---
task_id: c1-struct-marshal-test
parent_task_id: marshal-industrialization
source_relation: roadmap-child
phase: C
lifecycle_status: in-progress
entry_skill: dev-executing-plans
clearance_source: parent-roadmap
blocking_questions: []
question_clearance: cleared
clearance_confirmed_by_user: true
---

# Phase C1 — 结构体编组测试

## Scope

测试 struct marshalling descriptor 的注册、字段名解析、MarshalSizeOf/MarshalOffsetOf 空安全。
ManagedToNative/NativeToManaged roundtrip 需要 GC 堆，在当前阶段不可测试。

## Expected Outputs

- `testing/src/native/runtime-core/struct_marshal_test.cpp`

## Exit Criteria

12 个测试用例全部通过 `struct_marshal` 测试目标。
