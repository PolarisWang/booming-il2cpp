# R4.6 — 反射压力/soak 测试

> **task_id**: r4.6-reflection-stress
> **parent_task_id**: reflection-industrialization
> **source_task_id**: reflection-industrialization
> **source_relation**: roadmap-child
> **clearance_source**: parent-roadmap
> **phase**: planned
> **创建日期**: 2026-05-23
> **entry_skill**: dev-executing-plans

blocking_questions: []
question_clearance: cleared
clearance_confirmed_by_user: true

## Scope

创建反射压力测试，在循环中反复调用反射 API 验证稳定性。

## Implementation

创建 `testing/src/native/runtime-core/reflection_stress_test.cpp`

使用 GoogleTest，在高频循环中调用反射 API：
- GetFields / GetMethods / GetProperties 循环 1000 次
- GetElementType / MakeArrayType 循环 1000 次
- Type query flags（IsArray, IsEnum, IsPublic 等）循环 1000 次

## Exit Criteria

- 测试文件创建
- chaos_runtime_core 构建通过
