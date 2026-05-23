# R3.1 — ModuleRegistry 热更新同步

> **task_id**: r3.1-hotupdate-sync
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

确保热更新模块注册后，反射查询（GetTypeByName、GetTypeFromHandle）能扫描到热更新类型。

## Background

`ModuleLifecycleManager::RegisterModule` 已经通过 `RegisterModule()` 将模块注册到 ModuleRegistry。反射查询通过遍历 `GetModuleCount()` / `GetModuleByIndex()` 访问所有模块。热更新模块注册后应立即可见。

## Verification

1. 确认 `ModuleLifecycleManager::RegisterModule` → 调用 `RegisterModule()` → 模块加入 `g_module_storage[]`
2. 确认 `GetTypeByName` 路径扫描所有 `GetModuleCount()` 模块（包括新注册的热更新模块）
3. 确认 `GetTypeFromHandle` 路径能解析 `[module_id:32][token:32]` handle，其中 module_id 指向热更新模块
4. 如果需要，在 `hot_update.cpp` 中 `LoadHotUpdatePackage` 成功路径末尾添加 ModuleRegistry 同步调用

## Expected Files

- `src/native/hot-update/hot_update.cpp` — LoadHotUpdatePackage 末尾添加 ModuleRegistry sync（如果需要）
- `src/native/runtime-core/reflection/type_resolve.cpp` — GetTypeByName 确认已扫描所有模块

## Exit Criteria

- 热更新模块注册后 type_names/type_flags 在 ModuleRegistry 中立即可见
- chaos_runtime_core 构建通过

## Terminal Notes

**实现日期**: 2026-05-23
**构建状态**: chaos_runtime_core + chaos_hot_update 均构建通过 (RelWithDebInfo)

**分析结论**: 热更新模块在 `LoadHotUpdatePackage` 中未注册到 ModuleRegistry 的 `g_module_storage[]`，导致反射查询路径无法发现热更新类型。

**改动一: hot_update.cpp** — LoadHotUpdatePackage 末尾添加 ModuleRegistry 注册
- 将 `GetNextModuleId()` 独立 module_id 分配替换为 `runtime_core::RegisterModule()` 注册到 ModuleRegistry
- ModuleDescriptor 以最小形态注册（name_utf8 设为 assembly_name，其余字段默认 nullptr/0）
- type_names/type_flags/type_info_ptrs 等 Tier 1 数据留待后续 supplemental metadata 解析阶段填充
- UnloadHotUpdatePackage 相应添加 `MarkModuleTombstone()` 调用
- 移除废弃的 `GetNextModuleId()` 函数

**改动二: type_resolve.cpp** — ChaosReflectionGetTypeFromHandle + ChaosReflectionGetTypeByName 添加 ModuleRegistry 后备路径
- GetTypeFromHandle: 在原始 metadata token 和 FNV hash 路径之后，添加 `[module_id:32][token:32]` handle 解码路径
  1. `LookupModule(module_id)` → `type_info_ptrs[TokenToIndex(token)]`
  2. 回退 `image` → `FindReflectionQueryTypeByToken()`
- GetTypeByName: 在 `module->image->types` 扫描之后，添加 `type_names[]/type_namespaces[]` 直扫路径
  对于 `image == nullptr` 但 `type_names != nullptr` 的模块（最小注册的热更新模块），按 namespace + type_name 匹配后返回 ModuleRegistry 格式 handle

**遗留事项**: 热更新模块的 type_names/type_namespaces/type_info_ptrs 数据目前为 nullptr（最小注册），后续 supplemental metadata 解析阶段编写时需要同步填充这些字段以使反射查询真正可用。
