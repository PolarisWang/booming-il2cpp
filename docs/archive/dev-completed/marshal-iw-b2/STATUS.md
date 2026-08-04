---
task_id: b2-string-test
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

# Phase B2 — marshal_string 函数测试

## Scope

为 marshal_string 模块编写测试：StringToWide、StringToCoTaskMemUtf8/Wide、StringToHGlobalAnsi/Uni、MarshalFreeBSTR。
不依赖 GC 堆（mock StringObjectHeader 模拟托管字符串）。

## Expected Outputs

- `testing/src/native/runtime-core/marshal_string_test.cpp`

## Exit Criteria

10 个测试用例全部通过 `marshal_string` 测试目标。
