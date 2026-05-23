# R1.1 — BindingFlags 过滤 Native 实现

> **task_id**: r1.1-bindingflags
> **parent_task_id**: reflection-industrialization
> **source_task_id**: reflection-industrialization
> **source_relation**: roadmap-child
> **clearance_source**: parent-roadmap
> **phase**: completed
> **创建日期**: 2026-05-23
> **完成日期**: 2026-05-23
> **entry_skill**: dev-writing-plans

blocking_questions: []
question_clearance: cleared
clearance_confirmed_by_user: true

## Scope

在 Native 侧实现 BindingFlags 过滤。修改 codegen descriptor flags（FieldDescriptor/MethodDescriptor/PropertyDescriptor 新增 flags 字段），在 C++ 反射 API 中 `GetFieldsBindingFlags`, `GetMethodsBindingFlags`, `GetPropertiesBindingFlags`, `GetConstructorsDefault`, `GetMembers`, `GetNestedTypes` 实现 BindingFlags 参数过滤逻辑。

## Terminal Notes

已实现 7 个文件变更：
1. `reflection_query_model.h` — 新增 flags 字段（FieldDescriptor/MethodDescriptor/PropertyDescriptor 末尾）+ flag 常量
2. `reflection_binding_flags.h` — 新建文件，BindingFlags 常量 + MatchFieldFlags/MatchMethodFlags/MatchPropertyFlags
3. `reflection/type_properties.cpp` — ChaosReflectionGetConstructors 使用 NormalizeBindingFlags + MatchMethodFlags
4. `reflection/type_stubs.cpp` — ChaosTypeGetFieldBindingFlags 使用 NormalizeBindingFlags + MatchFieldFlags
5. `reflection/misc.cpp` — GetFieldsBindingflags / GetPropertiesBindingflags / GetMethodsBindingflags 真实实现（static buffer + flags 过滤 + EEClass fallback）
6. `reflection_api.cpp` — 新增 `#include "reflection_binding_flags.h"`
7. Scriban 模板 — field/method descriptor 数组新增 `/*flags*/ 0u` 初始值

验证：chaos_runtime_core 构建通过，28/28 metadata_registry_test 通过。
规格合规性审查 + 代码质量审查均已通过。
