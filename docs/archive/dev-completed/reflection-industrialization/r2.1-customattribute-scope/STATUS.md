# R2.1 — CustomAttribute scope 扩展

> **task_id**: r2.1-customattribute-scope
> **parent_task_id**: reflection-industrialization
> **source_task_id**: reflection-industrialization
> **source_relation**: roadmap-child
> **clearance_source**: parent-roadmap
> **phase**: completed
> **创建日期**: 2026-05-23
> **完成日期**: 2026-05-23
> **entry_skill**: dev-executing-plans

blocking_questions: []
question_clearance: cleared
clearance_confirmed_by_user: true

## Scope

扩展 CustomAttribute 查询支持到 assembly/method/field/property/param 多 scope 级别。当前仅支持 Type 级别（member_kind == 1）。

## 设计方案

### 最小变更方案（推荐）

保持 blob 格式不变，为 Method/Field/Property/Param 新增独立的 offset 数组。不是修改现有 Type offset 数组结构。

### ModuleData 新增字段 (`module_registry.h`)

```cpp
const uint32_t* custom_attribute_method_offset;   // [method_count+1] prefix-sum into blob
const uint32_t* custom_attribute_field_offset;     // [field_count+1] prefix-sum into blob
const uint32_t* custom_attribute_property_offset;  // [property_count+1] prefix-sum into blob
const uint32_t* custom_attribute_param_offset;     // [param_count+1] prefix-sum into blob
```

### attributes.cpp 修改

`ChaosGetCustomAttributeFromBlob` 解除 `member_kind != 1` 的限制，按 kind 选择 offset 数组：

```cpp
const uint32_t* offset_array = nullptr;
uint32_t entity_idx = 0;
switch (member_kind) {
    case 1: // Type
        entity_idx = TokenToIndex(type_token);
        offset_array = mod->custom_attribute_offset;
        break;
    case 4: // Method
        entity_idx = TokenToIndex(method_token);
        offset_array = mod->custom_attribute_method_offset;
        break;
    // ... Field(8), Property(16), Param(32)
}
```

### codegen 修改 (`NativeAotLoweringPlanner.ModuleRegistration.cs`)

当前 blob builder (`EmitCustomAttributeBlob`) 只收集 Type 级别的 custom attributes。需要扩展为收集所有 entity 级别的 attributes：

1. 在 PE metadata 扫描中收集 Method/Field/Property/Param 的 CustomAttribute 数据
2. 为每种 entity kind 构建独立的 offset 数组
3. Emit 到 ModuleRegistration Scriban 模板

### 简化策略

为减少复杂性，Method/Field/Property/Param scope 使用与 Type 相同的 blob 格式（[attr_count:uint16] + per-attribute records），共享同一个 blob 二进制数据流。不同 entity kind 只使用独立的 offset 数组索引到 blob 的对应位置。

## Inputs

- `attributes.cpp` — 当前仅支持 member_kind == 1
- `module_registry.h` — ModuleData 结构
- `NativeAotLoweringPlanner.ModuleRegistration.cs` — blob 生成
- `remaining_stubs.cpp` — ChaosReflectionIsDefined 当前返回 0

## Expected Outputs

- `module_registry.h` — ModuleData 新增 4 个 offset 数组字段
- `module_registry.cpp` — 初始化新字段为 nullptr（tombstone 兼容）
- `attributes.cpp` — ChaosGetCustomAttributeFromBlob 支持 5 种 member_kind
- `NativeAotLoweringPlanner.ModuleRegistration.cs` — 多 scope CA 数据收集 + offset 数组生成
- `remaining_stubs.cpp` — ChaosReflectionIsDefined 实现
- 构建验证通过

## Exit Criteria

- Type/Method/Field/Property/Param 5 个 scope 的 custom attribute 查询都可用
- ChaosReflectionIsDefined 不返回 0（至少做 blob 查询）
- chaos_runtime_core 构建通过

## Terminal Notes

**执行日期**: 2026-05-23
**执行结论**: completed

### 变更清单

1. **`module_registry.h`** — ModuleDescriptor 新增 8 个字段：
   - `custom_attribute_method_offset/field_offset/property_offset/param_offset` (const uint32_t*)
   - `custom_attribute_method_count/field_count/property_count/param_count` (uint32_t)

2. **`module_registry.cpp`** — MarkModuleTombstone 新增 null-out 和 0 初始化 for 8 个新字段

3. **`attributes.cpp`** — ChaosGetCustomAttributeFromBlob 解除 member_kind != 1 限制，支持 5 种 kind：
   - member_kind 1=Type, 2=Method, 3=Field, 4=Property, 5=Param
   - 统一使用 [module_id:32][token:32] 64-bit handle 编码
   - 每种 kind 选择对应的 offset 数组和 entity count

4. **`NativeAotLoweringPlanner.Models.cs`** — CustomAttributeTargetKind 新增 Field=3, Property=4, Param=5
   - CustomAttributeMaterializationPlan 新增 TargetMetadataToken 字段

5. **`NativeAotLoweringPlanner.cs`** — 更新所有 plan 创建函数签名以接收 entity token：
   - CreateCustomAttributeMaterializationPlan 新增 entityMetadataToken 参数
   - CreateDllImportAttributeMaterializationPlan 新增 methodToken 参数
   - CollectCustomAttributeMaterializations 新增 entityMetadataToken 参数
   - CollectSyntheticMethodCustomAttributeMaterializations 新增 methodToken 参数
   - 更新所有调用点传递 token

6. **`NativeAotLoweringPlanner.MetadataSupport.cs`** — 更新 CA 收集调用点传递 type/method token

7. **`NativeAotLoweringPlanner.ModuleRegistration.cs`** — 重写 BuildCustomAttributeBlobAndMaterializer：
   - 按 TargetKind 分组，遍历所有 5 种 entity kind
   - 读取 PE metadata 获取 MethodDef/Field/Property/Param 表行数
   - 每种 kind 构建独立的 entity index -> plans 映射和 offset 数组
   - 所有 kind 共享同一个 blob stream
   - EmitCustomAttributeModuleDescriptorFields 增加新字段的发射

8. **`remaining_stubs.cpp`** — ChaosReflectionIsDefined 实现：调用 ChaosGetCustomAttributeFromBlob 做 blob 查询

### 关键设计决策

- **Handle 编码**: 非 Type 的 member_handle 使用 [module_id:32][token:32] 64-bit 编码（与 TypeInfoHandle 相同），这是 generated code 的调用规约
- **Entity index**: TokenToIndex(token) = (token & 0x00FFFFFF) - 1 适用于所有 ECMA 表
- **共享 blob**: 5 种 entity kind 共享同一个 custom_attribute_blob，只增加 offset 数组
- **向后兼容**: 现有 Type 级别的 blob 格式不变，_moduleTypeCount 不变

### 构建验证

- chaos_runtime_core 构建通过（RelWithDebInfo）
- 存在 pre-existing LNK4006 警告（delegate_thunks duplicate symbols，非本次引入）
