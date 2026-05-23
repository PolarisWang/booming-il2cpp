# 29 — ModuleRegistry 运行时参考

> AOT 模块描述符注册表，提供模块 ID 分配、类型信息查找、墓碑化回收和并发安全查询。

## 概述

ModuleRegistry 管理全局 `ModuleDescriptor` 数组，为 AOT 编译的每个模块分配固定 ID，支持模块卸载时的墓碑化（tombstone）和槽位回收。它是运行时许多子系统的中枢依赖：
- 类型身份通过 `(module_id, token)` 编码
- 虚表分派通过 module_id 查找
- 泛型实例化按 module_id 组织
- 热更新卸载通过 module_id 标记墓碑

### 核心文件

| 文件 | 职责 |
|------|------|
| `contracts/native/v0/module_registry.h` | ModuleDescriptor 结构、TypeInfoHandle 编码/解码宏 |
| `src/native/runtime-core/module_registry.cpp` | 全部实现 |
| `src/native/runtime-core/module_registry.h` | 公开 API 声明 |

### 关键数据结构

**ModuleDescriptor** — AOT 编译单元的完整描述：
- `name_utf8`: 模块名（codegen 产生的字符串字面量）
- `type_count` / `type_names` / `type_namespaces`: 类型索引
- `image`: PE 文件内存映射指针
- `type_flags` / `type_info_ptrs`: 类型标志位和方法表指针
- `custom_attribute_*`: 自定义属性数据
- `abi_manifest`: ABI 兼容性清单
- `tombstone`: 是否已卸载（hotupdate 路径）

**TypeInfoHandle** — 类型身份的 64-bit 编码：
```c
// module_id[31:16] | token[15:0]
uint64_t TypeInfoHandle = ((uint64_t)module_id << 32) | (uint64_t)token;
```

## API 参考

### RegisterModule(name, descriptor) → module_id

注册一个模块并分配 ID：
1. 优先复用空闲列表中的墓碑槽
2. 无空闲槽时使用 `g_module_count` （线性增长至 `kMaxModules = 4096`）
3. 验证 ABI manifest（失败仅 WARN，不阻止加载）
4. 返回 `module_id` 或 `kInvalidModuleId`

锁: `unique_lock<shared_mutex>` 保护 `g_module_count` 和空闲列表

### LookupModule(module_id) → const ModuleDescriptor*

按 ID 查找模块：
- 验证 `module_id < kMaxModules`
- 验证 `name_utf8 != nullptr`（槽已占用）
- 包含墓碑槽查找（tombstone 槽返回有效指针，但大部分字段已置空）
- 不含 tombstone 标志检查 — 调用者需使用 `IsModuleTombstone`
- 不含 `g_module_count` 边界检查（兼容空闲列表回收的槽位）

锁: `shared_lock<shared_mutex>`

### LookupModuleByName(name) → const ModuleDescriptor*

按名称扫描（O(N) 扫描至多 `kMaxModules` 槽）：
- 跳过 `g_module_count` 之外的墓碑槽（非墓碑空槽处 break）
- 性能注意：存在墓碑 + 回收场景时可能扫描到 `kMaxModules`

锁: `shared_lock<shared_mutex>`

### LookupTypeInfoPtr(module_id, idx) → void*

返回指定模块第 idx 个类型的 `type_info_ptrs[idx]`：
- 验证 module_id 边界、墓碑、type_flags、type_info_ptrs 非空
- 验证 `idx < type_count`
- 返回 `type_info_ptrs[idx]` 或 nullptr

### MarkModuleTombstone(module_id)

将模块标记为墓碑（hotupdate 卸载路径）：
1. 验证 `module_id < kMaxModules`, `module_id != 0`（CoreLib）
2. 检查是否已是墓碑（幂等）
3. 清空 type_count、image、type_flags、type_info_ptrs、custom_attribute 等字段
4. 保留 `name_utf8`（字符串字面量，无需释放）
5. 将 module_id 加入空闲列表

锁: `unique_lock<shared_mutex>`

### IsModuleTombstone(module_id) → bool

检查 module_id 是否为墓碑：
- 验证 `module_id < kMaxModules`
- 返回 `g_module_storage[module_id].tombstone`

锁: `shared_lock<shared_mutex>`

## 生命周期

```
RegisterModule
     │
     ▼
  [已分配]         ← ModuleDescriptor 已填充，tombstone = false
     │
     │ 正常使用
     ▼
MarkModuleTombstone
     │
     ▼
  [墓碑]           ← 大部分字段置空，name_utf8 保留
     │
     │ 通过空闲列表回收
     ▼
RegisterModule     ← 复用墓碑槽，tombstone 置 false
```

## 两级架构（B2 决策）

参考 `wiki/.../04-历史决策/14-B2-ModuleRegistry-TwoTier-决策.md`。

Tier 1 模块（前 1024 个）使用快速线性查找；Tier 2 模块通过 overflow map 访问。当前实现使用统一的 `ModuleDescriptor[]` 数组 + 空闲列表，不区分 tier。

## 并发安全

使用 `g_module_mutex()` 函数局部静态 `shared_mutex`：
- 读取者: `LookupModule`, `LookupModuleByName`, `LookupTypeInfoPtr`, `IsModuleTombstone` — shared_lock
- 写入者: `RegisterModule`, `MarkModuleTombstone` — unique_lock

## 测试覆盖

- `module_registry_test.cpp`: 注册/查找/墓碑/重用、TypeInfoHandle 编解码、并发 tombstone+register（16 测试，Loader 工业化 Phase 1）
- 跨单元调用通过集成测试验证

## 与 AssemblyManager 的集成

`ModuleRegistry` 和 `AssemblyManager` 通过 `module_id` 关联：
- `AssemblyManager::LoadAssembly` 验证 module_id 在 ModuleRegistry 中已注册（Phase 3 加固）
- `AssemblyManager::UnloadAssembly` 调用 `MarkModuleTombstone(module_id)`
- 一个 AOT module 最多对应一个 hotpatch ALC
- module_id 在 AOT 编译时分配，热更新包引用其要替换的 module_id
