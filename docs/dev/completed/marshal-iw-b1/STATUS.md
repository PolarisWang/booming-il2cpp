---
task_id: b1-alloc-test
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

# Phase B1 — marshal_alloc 函数测试

## Scope

为 marshal_alloc 模块编写正确性 + 性能双验证测试：HGlobal/CoTaskMem alloc/free/realloc、null safety、读写所有整数类型。

## Expected Outputs

- `testing/src/native/runtime-core/marshal_alloc_test.cpp`

## Exit Criteria

8 个测试用例全部通过 `marshal_alloc` 测试目标。
