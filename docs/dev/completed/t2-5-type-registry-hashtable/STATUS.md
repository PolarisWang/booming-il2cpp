# T2-5: 动态类型注册表 256→哈希表

> **parent_task_id**: 20260523-typesystem-vtable-industrialization
> **source_task_id**: t2-5-type-registry-hashtable
> **source_relation**: roadmap-child
> **clearance_source**: parent-roadmap
> **创建日期**: 2026-05-23
> **更新日期**: 2026-05-23
> **task_type**: plan
> **phase**: execution
> **lifecycle_status**: in-progress
> **child_execution_mode**: auto
> **entry_skill**: dev-executing-plans

## Scope

将动态类型注册表从 256 条目平数组升级为 `CHAOS_IL2CPP_UNORDERED_DENSE_MAP_IDENTITY` 哈希表，解除 `kChaosMaxDynamicTypes` 硬上限。

**改动文件**：`type_registry.cpp`

**方案**：
- 移除平数组 `types[kChaosMaxDynamicTypes]` + `count` 计数器
- 替换为 `CHAOS_IL2CPP_UNORDERED_DENSE_MAP_IDENTITY(uint64_t, MethodTable*)`，key = stable_id
- `chaos_register_type` 不再需要 capacity 检查（哈希表动态扩容）
- `chaos_find_type_by_stable_id` 变为 O(1) map 查找
- 保留 `kChaosMaxDynamicTypes` 常量定义（可能有外部引用，不删除）

## Exit Criteria

- 12 个 type_registry_test 全部通过
- 47 个 vtable_registry_test 全部通过
- 6 个并发压力测试通过
- 注册超过 256 个动态类型不再失败
