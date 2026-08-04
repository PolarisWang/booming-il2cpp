---
task_id: d2-struct-header
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

# Phase D2 — struct_marshal.h 公开头文件

## Scope

创建 `struct_marshal.h` 公开头文件，声明 struct_marshal namespace 的三个公共 API 函数：
MarshalStructManagedToNative, MarshalStructNativeToManaged, DestroyMarshalledStruct。

## 产物

- `src/native/runtime-core/struct_marshal.h` — 新公开头文件
- `src/native/runtime-core/core/struct_marshal.cpp` — 添加 `#include "struct_marshal.h"` 自验证

## Exit Criteria

- 头文件编译通过（通过 struct_marshal 测试目标链接验证）
- 现有 marshal 测试全部通过
