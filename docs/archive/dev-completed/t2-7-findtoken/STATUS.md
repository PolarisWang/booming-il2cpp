# T2-7: MetadataRegistry::FindToken AOT 索引

> **parent_task_id**: 20260523-typesystem-vtable-industrialization
> **source_task_id**: roadmap-v1-01.md
> **source_relation**: roadmap-child
> **lifecycle_status**: completed
> **phase**: completed

## Terminal Notes

已实现 `MetadataRegistry::FindToken()` AOT 索引。关键变更：

1. `metadata_interface.h`: 新增 `token_index_`（`unordered_dense_map<uint32_t, uint64_t>`）和 `token_index_built_`（`atomic<bool>`）私有成员，`BuildTokenIndex()` 声明
2. `metadata_interface.cpp`: 
   - `BuildTokenIndex()`: 扫描所有模块的 `ReflectionQueryImageDescriptor` types→methods，构建 `method_token → (module_id<<32 | token)` 映射
   - `FindToken()`: 先查 HotpatchNameRegistry，再查 AOT token 索引（懒加载构建），替换原来的 `(void)mod` no-op 扫描

修改文件：
- `src/native/runtime-core/metadata_interface.h`
- `src/native/runtime-core/metadata_interface.cpp`
