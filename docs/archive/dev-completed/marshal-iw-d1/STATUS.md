---
task_id: d1-internal-header
parent_task_id: marshal-industrialization
source_relation: roadmap-child
phase: D
lifecycle_status: in-progress
entry_skill: dev-executing-plans
clearance_source: parent-roadmap
blocking_questions: []
question_clearance: cleared
clearance_confirmed_by_user: true
---

# Phase D1 — marshal_internal.h 提取关键符号

## Scope

创建 `marshal_internal.h`，提取当前通过 Unity build 匿名 namespace 共享的内部符号。

## Expected Outputs

- `src/native/runtime-core/marshal_internal.h`

## Exit Criteria

- 头文件编译通过（作为现有编译目标的一部分验证）
- 所有现有 marshal 测试仍然通过
