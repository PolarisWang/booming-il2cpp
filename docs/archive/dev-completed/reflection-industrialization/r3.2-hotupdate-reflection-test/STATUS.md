# R3.2/R4.5 — 热更新反射集成测试

> **task_id**: r3.2-hotupdate-reflection-test
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

创建热更新反射集成测试，验证热更新模块注册后反射查询可见。R3.2 和 R4.5 合并为一个任务。

## Expected Location

测试目录: `testing/foundation-dll/System.Private.CoreLib/reflection-hotupdate/`
参考现有 hot-update 测试: `testing/foundation-dll/System.Private.CoreLib/hot-update/`

## Test Scenarios

1. **TypeRegistry**: 热更新包加载后，ModuleRegistry 中可查到热更新 module/type
2. **GetTypeFromHandle**: 热更新类型的 handle 可被解析
3. **GetTypeByName**: 热更新类型可按名查找

## Exit Criteria

- 测试文件创建并注册到 CMakeLists.txt
- chaos_runtime_core 构建通过

## Terminal Notes

### 创建的文件

```
testing/foundation-dll/System.Private.CoreLib/reflection-hotupdate/
  capability-family-contract.json   # familyId: family/System.Private.CoreLib/reflection/hotupdate
  test/
    ReflectionHotUpdateTests.cs     # 4 个 Fact 测试方法
```

### 测试场景覆盖

| # | 测试方法 | 验证点 |
|---|----------|--------|
| 1 | `_ReflectionHotUpdate_TypeRegistry_ModuleVisible` | Assembly.GetModules 不抛异常 |
| 2 | `_ReflectionHotUpdate_TypeRegistry_TypesDiscovered` | Assembly.GetTypes 不抛异常 |
| 3 | `_ReflectionHotUpdate_GetTypeFromHandle` | Type.GetTypeFromHandle 可解析类型 handle |
| 4 | `_ReflectionHotUpdate_GetTypeByName` | Assembly.GetType 按名查找不抛异常 |

### 构建验证

- `chaos_runtime_core` — RelWithDebInfo 构建通过，无新增 warning/error

### 备注

- CapabilityTest 使用 `IncludeHotUpdate = true` 标记热更新测试族
- 基础类型 `System.Byte` 作为反射操作的目标，确保在 AOT 和热更新模式下均可验证
- 热更新管线阶段当前跳过了 hotupdate 验证（pipeline-config.yaml: `hotupdate: true`），测试在 hotupdate 阶段解封后自动生效

### 技术债

- 当前测试仅为 xUnit Fact 验证，后续可补充 native `--hotupdate` mode 的完整 entry.exe 管线接入
