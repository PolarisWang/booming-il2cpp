# R1.2 — Member flags 嵌入 codegen

> **task_id**: r1.2-member-flags
> **parent_task_id**: reflection-industrialization
> **source_task_id**: reflection-industrialization
> **source_relation**: roadmap-child
> **clearance_source**: parent-roadmap
> **phase**: completed
> **创建日期**: 2026-05-23
> **entry_skill**: dev-writing-plans

blocking_questions: []
question_clearance: cleared
clearance_confirmed_by_user: true

## Scope

在 `remaining_stubs.cpp` 中实现 5 个 member flags 函数，使其返回真实 descriptor flags 而非 0：

- `ChaosReflectionFieldGetIsInitOnly` — 解码 FieldInfoHandle → 检查 kFieldFlagIsInitOnly
- `ChaosReflectionFieldGetIsLiteral` — 解码 FieldInfoHandle → 检查 kFieldFlagIsLiteral
- `ChaosReflectionPropertyGetCanRead` — 解码 PropertyInfoHandle → 检查 kPropertyFlagCanRead
- `ChaosReflectionPropertyGetCanWrite` — 解码 PropertyInfoHandle → 检查 kPropertyFlagCanWrite
- `ChaosReflectionGetIsVirtual` — 解码 MethodInfoHandle → 检查 kMethodFlagIsVirtual

flag 常量和 descriptor flags 字段已在 R1.1 中就绪。

## Inputs

- R1.1 已完成：descriptor flags 字段 + flag 常量 + Scriban `/*flags*/ 0u` 初始值
- 文件：`reflection_query_model.h` — flag 常量
- 文件：`remaining_stubs.cpp` — 5 个 stub 函数（当前返回 0）
- 头文件：`runtime_stubs/reflection_stubs.h` — 函数声明

## Expected Outputs

- `remaining_stubs.cpp` — 5 个函数实现真实 flags 检查
- 构建验证通过

## Exit Criteria

- 5 个函数均返回真实 flag 值而非 0
- chaos_runtime_core 构建通过

## Terminal Notes

- 实现 5 个函数于 `remaining_stubs.cpp`：
  - `ChaosReflectionGetIsVirtual` (L44)：解码 MethodInfoHandle → `kMethodFlagIsVirtual` 检查
  - `ChaosReflectionFieldGetIsInitOnly` (L137)：解码 FieldInfoHandle → `kFieldFlagIsInitOnly` 检查
  - `ChaosReflectionFieldGetIsLiteral` (L149)：解码 FieldInfoHandle → `kFieldFlagIsLiteral` 检查
  - `ChaosReflectionPropertyGetCanRead` (L161)：解码 PropertyInfoHandle → `kPropertyFlagCanRead` 检查
  - `ChaosReflectionPropertyGetCanWrite` (L168)：解码 PropertyInfoHandle → `kPropertyFlagCanWrite` 检查
- 统一使用 `TryDecodeReflectionQueryHandle<T>(static_cast<HandleType>(param))` 解码模式
- 所有函数先 null 检查，再返回 flag 检查结果
- 更新了 FieldInfo/PropertyInfo 区域注释（Phase 2+ → Phase 1.2+）
- `chaos_runtime_core` 构建通过（RelWithDebInfo、MSVC）
- 用时：约 10 分钟
