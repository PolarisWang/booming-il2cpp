---
task_id: b3-api-basic-test
parent_task_id: marshal-industrialization
source_relation: roadmap-child
phase: B
lifecycle_status: in-progress
entry_skill: dev-executing-plans
clearance_source: parent-roadmap
blocking_questions: []
question_clearance: cleared
clearance_confirmed_by_user: true
---

# Phase B3 — marshal API 基础测试

## Scope

为 marshal_api 基础函数编写测试：P/Invoke error、OS interop、TaskKernel、NativeLibrary、MarshalDestroyStructure、MarshalSizeOf/OffsetOf 的空安全测试。

## Expected Outputs

- `testing/src/native/runtime-core/marshal_api_basic_test.cpp`

## Exit Criteria

12 个测试用例全部通过 `marshal_api_basic` 测试目标。
