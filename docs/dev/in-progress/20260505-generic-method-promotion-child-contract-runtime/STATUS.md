---
task_id: 20260505-generic-method-promotion-child-contract-runtime
title: Generic Method AOT — Contract + Runtime
task_type: plan
lifecycle_status: in_progress
phase: executing
created_at: 2026-05-05
updated_at: 2026-05-05

parent_task_id: 20260505-generic-method-promotion
source_task_id: contract-runtime
source_relation: roadmap-child
clearance_source: parent-roadmap

blocking_questions: []
question_clearance: cleared
clearance_confirmed_by_user: true
---

# Generic Method AOT — Contract + Runtime

## Scope

C++ 合约层和运行时层：定义 `GenericMethodAotEntryV0` 结构体，扩展 `ModuleGenericRegistrationV0` 的 `method_aot_entries` / `method_aot_entry_count` 字段，实现 `QueryAotMethod` per-module 二分查找函数。

## Inputs

- `contracts/native/v0/codegen_bridge.h` — 现有 `GenericTypeRegistrationEntryV0`、`ModuleGenericRegistrationV0` 定义
- `src/native/runtime-core/runtime_instantiation.cpp` — 现有 `ResolveOrInstantiateMethod` 实现
- `src/native/runtime-core/runtime_instantiation.h` — 现有接口声明

## Expected Outputs

- `codegen_bridge.h` — 新增 `GenericMethodAotEntryV0` struct；`ModuleGenericRegistrationV0` 新增字段
- `runtime_instantiation.cpp` — 新增 `QueryAotMethod` 实现
- `runtime_instantiation.h` — 新增 `QueryAotMethod` 声明（如需要）

## Exit Criteria

- 编译通过
- `QueryAotMethod` 单元测试通过（正常命中、miss、空表、排序错误 assert）
- 已有测试不回归

## 计划

### 步骤

1. **codegen_bridge.h**: 新增 `GenericMethodAotEntryV0` 结构体，扩展 `ModuleGenericRegistrationV0`
2. **runtime_instantiation.h**: 声明 `QueryAotMethod`
3. **runtime_instantiation.cpp**: 实现二分查找 + type_args 线性扫描
4. **编写单元测试**: 覆盖 hit / miss / 空表 / 排序异常
5. **编译验证**: 确认编译通过，已有测试不回归
