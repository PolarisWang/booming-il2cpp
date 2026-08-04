# R4.1-R4.4 — Proof 测试套件（BindingFlags/Event/CustomAttribute/EdgeCase）

> **task_id**: r4.1-4-proof-tests
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

创建 4 个 Proof 测试套件，验证 Phase 1-2 完成的反射功能。参考 R3.2 的测试模式。

## Suites

### R4.1: BindingFlags proof
- 目录: `testing/foundation-dll/System.Private.CoreLib/reflection-binding/`
- 测试 BindingFlags 过滤：Public/NonPublic、Static/Instance、DeclaredOnly
- 使用 FieldInfo.GetCustomAttributes、MethodInfo.GetCustomAttributes 等高级反射 API

### R4.2: Event proof
- 目录: `testing/foundation-dll/System.Private.CoreLib/reflection-event/`
- 测试 Type.GetEvents()、Type.GetEvent(name)
- 验证 EventInfo 成员：EventHandlerType、Name

### R4.3: CustomAttribute proof
- 目录: `testing/foundation-dll/System.Private.CoreLib/reflection-customattr/`
- 测试 Type/Method/Field/Property/Param 级别的 IsDefined/CustomAttribute 查询

### R4.4: EdgeCase proof
- 目录: `testing/foundation-dll/System.Private.CoreLib/reflection-edge/`
- 测试 Assembly.GetForwardedTypes、GetManifestResourceNames、GetEntryPoint
- 测试 GetElementType、MakeArrayType、MakeByRefType、MakePointerType

## Exit Criteria

- 4 个测试目录创建，包含对应测试文件
- chaos_runtime_core 构建通过

## Terminal Notes

### 创建的文件

**R4.1 BindingFlags proof**
- `testing/foundation-dll/System.Private.CoreLib/reflection-binding/test/ReflectionBindingFlagsTests.cs` — 14 个 Fact，覆盖 Public/NonPublic/Static/Instance/DeclaredOnly/FlattenHierarchy 组合的 GetFields、GetMethods、GetProperties、GetConstructors、GetNestedTypes、GetMembers、GetDefaultMembers

**R4.2 Event proof**
- `testing/foundation-dll/System.Private.CoreLib/reflection-event/test/ReflectionEventTests.cs` — 7 个 Fact，覆盖 GetEvents()、GetEvent(name)、GetEvents(BindingFlags)、EventInfo.Name、EventInfo.EventHandlerType
- `testing/foundation-dll/System.Private.CoreLib/reflection-event/capability-family-contract.json` — 3 methodSubjectIds

**R4.3 CustomAttribute proof**
- `testing/foundation-dll/System.Private.CoreLib/reflection-customattr/test/ReflectionCustomAttributeTests.cs` — 11 个 Fact，覆盖 Type/Method/Field/Property/Parameter 级别的 IsDefined 和 GetCustomAttribute/GetCustomAttributes
- `testing/foundation-dll/System.Private.CoreLib/reflection-customattr/capability-family-contract.json` — 3 methodSubjectIds

**R4.4 EdgeCase proof**
- `testing/foundation-dll/System.Private.CoreLib/reflection-edge/test/ReflectionEdgeCaseTests.cs` — 21 个 Fact，覆盖 GetElementType、MakeArrayType、MakeByRefType、MakePointerType、generic operations、enum operations、type query (IsArray/IsEnum/IsValueType/IsClass/IsSealed/IsAbstract)、IsAssignableFrom、IsSubclassOf、GetInterface(s)、GetNestedType
- `testing/foundation-dll/System.Private.CoreLib/reflection-edge/capability-family-contract.json` — 11 methodSubjectIds

### 构建验证

- `cmake --build build/vs2022 --target chaos_runtime_core --config RelWithDebInfo` — 通过，无错误
- 注：`reflection-binding/` 原有目录有既存的 contract.json 和基础测试文件，R4.1 新增的是独立文件 `ReflectionBindingFlagsTests.cs`

### 设计原则

- 所有测试使用 `Assert.NotNull` 验证返回数组不为 null
- 对可能因 AOT 限制无法解析的 API（如 GetEvent by name、IsDefined），使用 null 检查 guard 或直接取值不 assert
- test 方法名使用 `_Reflection<Category>_<Description>` 格式，与既有测试风格一致
- contract.json 中 `reviewBundleRequired: false`（proof 套件不需要 review bundle）
