# T2-8b: EEClass 动态填充（PatchMetadataCache 扩展）

> **parent_task_id**: 20260523-typesystem-vtable-industrialization
> **source_task_id**: roadmap-v1-01.md
> **source_relation**: roadmap-child
> **lifecycle_status**: completed
> **phase**: completed

## Scope

实现动态类型（.patchdata hotupdate 类型）的 EEClass Ensure*Filled 数据填充：
1. 扩展 PatchMetadataCache 暴露 `GetFieldDef(uint32_t index)` accessor
2. 添加 `GetTypeDefByIndex(uint32_t index)` 直接索引访问器
3. 创建 `EEClassFromPatchData()` 从 PatchMetadataCache 分配 domain 内存并填充 LazyArray
4. 更新 EnsureMethodsFilled/EnsureFieldsFilled 在动态路径下调用 EEClassFromPatchData()

**已知限制**: patch_data.h 无 PatchPropertyDefEntry → properties 留空

## Inputs

- `contracts/native/v0/patch_data.h` — PatchFieldDefEntry/PatchTypeDefEntry 定义
- `src/native/runtime-core/patch_loader.h` — PatchMetadataCache 类声明
- `src/native/runtime-core/patch_loader.cpp` — 实现
- `src/native/runtime-core/eeclass.h` — EEClass/LazyArray 定义
- `src/native/runtime-core/eeclass.cpp` — Ensure*Filled stubs

## Expected Outputs

- `patch_loader.h` — 新增 GetFieldDef、GetTypeDefByIndex 声明
- `patch_loader.cpp` — accessor 实现
- `eeclass.h` — 新增 EEClass::patch_cache/patch_type_index 字段 + EEClassFillFromPatchData 声明
- `eeclass.cpp` — 动态类型填充实现

## Deliverables

### patch_loader.h 扩展
- `GetFieldDef(uint32_t index)` — 返回 FieldDef entry
- `GetTypeDefByIndex(uint32_t index)` — 返回 TypeDef entry

### eeclass.h 扩展
- `EEClass::patch_cache` / `patch_type_index` — 动态类型上下文
- `EEClassFillFromPatchData()` 声明

### eeclass.cpp 实现

**EEClassFillFromPatchData()**:
- 通过 TypeDef token 扫描 method_def 表计数
- 通过 TypeDef token 扫描 field_def 表计数
- domain-allocate ReflectionMethodEntry[] 和 ReflectionFieldEntry[]
- 填充 metadata_token + name_utf8（strings heap）
- 设 ee->patch_cache / ee->patch_type_index 供后续 Ensure*Filled 重入

**Ensure*Filled helpers updated**:
- EnsureMethodsFilled: patch_cache != null → EEClassFillFromPatchData
- EnsureFieldsFilled: patch_cache != null → EEClassFillFromPatchData
- EnsurePropertiesFilled: 直接标记 filled（无 PatchPropertyDefEntry）
- EnsureEventsFilled: 直接标记 filled（无 PatchEventDefEntry）

## Exit Criteria

- PatchMetadataCache GetFieldDef 返回正确的 FieldDefEntry ✅
- PatchMetadataCache GetTypeDefByIndex 返回正确的 TypeDefEntry ✅
- 动态类型 EnsureMethodsFilled 填充 EEClass 的 methods 数据 ✅
- 动态类型 EnsureFieldsFilled 填充 field 数据 ✅
- 已知限制：properties 留空（无 PatchPropertyDefEntry）✅
- 现有 eeclass_test 全部通过（9/9）✅

## Completion Notes

- All 4 Ensure*Filled helpers updated to check `ee->patch_cache` and delegate to `EEClassFillFromPatchData`
- Properties and events intentionally left empty (patch_data.h has no corresponding entries)
- First call to any Ensure*Filled triggers full fill via EEClassFillFromPatchData; subsequent calls are no-ops via filled=true guard
- AOT path unaffected — patch_cache is null for AOT types, Ensure*Filled remains no-op

## Terminal Notes

T2-8b completed in same session as T2-8a. Implementation spans:
- `patch_loader.h`: 2 new method declarations
- `patch_loader.cpp`: 2 new accessor implementations (GetFieldDef, GetTypeDefByIndex)
- `eeclass.h`: 2 new EEClass fields + 1 function declaration
- `eeclass.cpp`: ~90 lines for EEClassFillFromPatchData, ~30 lines for updated Ensure*Filled helpers
