# R4.6 — 反射压力/soak 测试

> **task_id**: r4.6-reflection-stress
> **parent_task_id**: reflection-industrialization
> **source_task_id**: reflection-industrialization
> **source_relation**: roadmap-child
> **clearance_source**: parent-roadmap
> **phase**: completed
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

- [x] 测试文件创建
- [x] chaos_runtime_core 构建通过
- [x] reflection_stress_test 构建通过
- [x] 6 个测试全部通过

## Notes

### Bug found: GetMethods handle fallback crash

`ChaosReflectionGetMethods` 内部调用 `GetTypeInfoFromReflectionOrGcHandle`，当 `type_info_ptrs` 为 null 时，fallback 路径将模块注册表 handle（如 `MakeTypeHandle(module_id, token)`）当作 GC Type 对象指针，从 `handle + 16` 读取数据导致访问违例。

解决方法：测试中提供 `type_info_ptrs` 数组（指向最小 TypeInfoHot，parent=nullptr 终止父链遍历）。

### Test design

- **成员查询**（GetFields/GetMethods/GetProperties）：使用 Module Registry handle (`MakeTypeHandle`)
- **类型操作**（GetElementType/MakeArrayType）：使用 tag-bit encoded handle (`EncodeReflectionQueryTypeHandle`)
- **类型标记查询**（IsArray/IsEnum/IsPublic 等）：使用 Module Registry handle（flag stubs 使用 `ResolveTypeRef`，只接受 Module Registry handle）
- 6 个测试包括：模块注册验证 + 3 类独立压力测试 + 混合压力测试 + 附加 API soak
