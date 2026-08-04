# T2-6: MetadataRegistry::GetMethodName 实现

> **parent_task_id**: 20260523-typesystem-vtable-industrialization
> **source_task_id**: roadmap-v1-01.md
> **source_relation**: roadmap-child
> **lifecycle_status**: completed
> **phase**: completed

## Terminal Notes

已实现 `MetadataRegistry::GetMethodName()`。实现方案：

1. 在 `MetadataRegistry` 类新增 `method_name_cache_`（`unordered_dense_map<uint64_t, const char*>`）私有成员
2. 查询路径：
   - 先查缓存（O(1)）
   - Path 1: 按 module_id 查找 `ModuleDescriptor.image`，遍历 types→methods 匹配 `metadata_token`
   - Path 2: 全模块回退扫描（处理 module_id 不明确的情况）
   - 缓存结果（含 nullptr，避免重复回退扫描）
3. 数据源：`ReflectionQueryMethodDescriptor.name_utf8`（constexpr .rodata，进程生命周期稳定）

修改文件：
- `src/native/runtime-core/metadata_interface.h` — 新增 `method_name_cache_` 成员
- `src/native/runtime-core/metadata_interface.cpp` — 完整 GetMethodName 实现

**注意**: 当前不包含 HotpatchNameRegistry 路径（HotpatchEntryV0 不直接暴露 method name）。该路径将在 T2-11（LookupCallTarget hotpatch）中统一解决。
