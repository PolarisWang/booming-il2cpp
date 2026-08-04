# C3: MetadataRegistry — 统一元数据注册表

> **task_id**: c3-metadata-registry
> **parent_task_id**: hotupdate-industrialization
> **source_task_id**: hotupdate-industrialization
> **source_relation**: roadmap-child
> **phase**: completed
> **创建日期**: 2026-05-23
> **完成日期**: 2026-05-23

## 完成摘要

统一元数据注册表（MetadataRegistry）已实现，消除 AOT/热更新双分元数据体系。

### 核心交付

1. **Contract 头文件**: `contracts/native/v0/unified_metadata.h`
   - UnifiedMetadataRegistryV0 C-compatible 接口定义
   - TypeHandle/MethodHandle/FieldHandle (composite key: module_id<<32 | token)
   - resolve_type/get_type_name/resolve_method/resolve_field/find_token/enumerate_methods

2. **MetadataRegistry 类**: `metadata_interface.h/.cpp`（扩展）
   - 统一读路径入口：ResolveType/ResolveMethod/ResolveField/FindToken/EnumerateMethods
   - Hash-based 快速查找缓存（`resolve_cache_`，延迟构建）
   - `GetUnifiedRegistry()` 导出 C-compatible 接口指针
   - HotpatchNameRegistry 作为子组件集成（通过全局单例引用，不重复注册）

3. **ModuleAwareResolveToken 补齐**
   - TypeDef (0x02) / TypeRef (0x01) / Method (0x06) / Field (0x04) / MemberRef (0x0A)
   - 通过 CodegenBridgeV0 解析，失败时返回 raw token 作为 opaque handle

4. **AOT Bootstrap 连接**
   - `ModuleLifecycleManager::RegisterAotModuleData()` — 统一 AOT 热更新表+泛型注册
   - bootstrap.cpp 改为通过 ModuleLifecycleManager 注册 AOT root 数据

5. **HotUpdate 加载连接**
   - `ModuleLifecycleManager::RegisterHotUpdateGenerics()` / `UnregisterHotUpdateGenerics()`
   - hot_update.cpp 改为通过 ModuleLifecycleManager 注册/反注册泛型

6. **RegisterHotpatchContext 实现**
   - 遍历 PatchContext 的 method list，为每个方法调用 SetPatchedBySlot
   - 记录活跃 patch 上下文用于 UnregisterHotpatchContext 撤销

### 测试

- `testing/src/native/runtime-core/metadata/metadata_registry_test.cpp`
- 覆盖：singleton 访问、contract 接口非空、空注册表稳定性、未知模块查询保护
- 集成测试（需完整 AOT bootstrap）以 `FULL_INTEGRATION_TESTS` 宏守卫

### 文件影响

| 文件 | 操作 | 说明 |
|------|------|------|
| `contracts/native/v0/unified_metadata.h` | **新建** | 统一元数据查询接口 |
| `src/native/runtime-core/metadata_interface.h` | 修改 | 新增 MetadataRegistry 类、RegisterAotModuleData、RegisterHotUpdateGenerics |
| `src/native/runtime-core/metadata_interface.cpp` | 修改 | MetadataRegistry 实现、ResolveToken 补齐、RegisterHotpatchContext、AOT/HU 连接 |
| `src/native/bootstrap/bootstrap.cpp` | 修改 | AOT bootstrap 通过 ModuleLifecycleManager 注册 |
| `src/native/hot-update/hot_update.cpp` | 修改 | 热更新包加载通过 ModuleLifecycleManager 注册 |
| `testing/src/native/runtime-core/metadata/metadata_registry_test.cpp` | **新建** | 单元测试 |
| `testing/src/native/runtime-core/metadata/CMakeLists.txt` | **新建** | 测试构建配置 |
| `testing/src/native/runtime-core/CMakeLists.txt` | 修改 | 新增 metadata 子目录 |

### 架构审视

- ModuleLifecycleManager（写路径）与 MetadataRegistry（读路径）职责边界清晰
- 无重复解析路径 — MetadataRegistry 调用 ModuleAwareResolveXxx 作为后端
- unified_metadata.h contract 与 metadata_interface.h 定义一致
- 代码符合项目的 include/namespace 规范

### 已知限制

- `GetMethodName` 当前返回 nullptr（待 Phase 2 寄存器 VM 补充方法名表）
- `FindToken` 仅搜索 HotpatchNameRegistry（AOT-only 方法需要额外索引）
- 集成测试需要完整 AOT bootstrap 环境
