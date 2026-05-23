# 30 — PatchLoader 运行时参考

> 热更新补丁数据（.patchdata）的运行时加载器：格式验证、元数据缓存、补丁上下文生命周期。

## 概述

PatchLoader 是 `.patchdata` 二进制格式的运行时加载端，负责：
- 验证补丁文件头部的完整性和版本兼容性
- 创建 `PatchContext` 和 `PatchMetadataCache`（域分配）
- 遍历 MethodDef 条目，通过 HotpatchNameRegistry 应用补丁
- 管理补丁卸载时的资源释放

PatchLoader 是 PatchData 工具链的运行时对端。工具链生成 `.patchdata`（格式参考 `wiki/.../05-patchdata工具链.md`），运行时通过 PatchLoader 加载。

### 核心文件

| 文件 | 职责 |
|------|------|
| `src/native/runtime-core/patch_loader.h` | PatchContext、PatchMethod、PatchMetadataCache 声明 |
| `src/native/runtime-core/patch_loader.cpp` | 全部实现（~900 行，最大 Loader 组件） |
| `contracts/native/v0/patch_data.h` | PatchDataHeader、PatchMethodDefEntry 等合约结构 |
| `src/native/hot-update/hotpatch_table.h` | HotpatchNameRegistry 声明 |

### 关键类型

**PatchContext** — 补丁的顶层上下文：
- `header`: PatchDataHeader 指针（原始数据块）
- `total_size`: 补丁数据总大小
- `methods`: PatchMethod 数组（与 MethodDef 一一对应）
- `method_count`: 方法条目数
- `metadata_cache`: PatchMetadataCache 指针
- 所有权: 域分配，`DestroyPatchContext` 释放

**PatchMethod** — 单个补丁方法的完整状态：
- `cached_ir`: heap-allocated `IRMethod*`（通过 new）
- `call_cache`: domain-allocated `CachedCallInfo[]`（调用目标预缓存）
- `cached_reg_method`: heap-allocated `RegisterMethod*`
- `cached_arg_types`: arg type 缓存（小缓冲区优化）
- `module_id` / `method_token`: AOT 模块标识
- `subject_id_utf8`: codegen subject ID

**PatchMetadataCache** — 元数据访问接口：
- `header_`: PatchDataHeader 引用
- `GetString(offset)`: 字符串堆访问
- `GetBlob(offset)`: blob 堆访问
- `GetBody(offset)`: IL body 数据访问
- `GetAotCoreIr(index)`: AOT IR JSON 访问（索引表 O(1)）
- `GetRegisterIr(index)`: 寄存器分配 IR 访问（v2+）
- `GetMethodDef(index)` / `FieldCount()` / `GetFieldDef()`: 表条目访问
- `ResolveTypeDef(token)` / `ResolveTypeRef(token)`: TypeDef/TypeRef 解析
- `FindInliningTarget(module_id, token)`: 内联映射查询

## API 参考

### ApplyPatchFromMemory(data, size, host_type_name, host_method_names) → PatchContext*

主入口。从内存块加载补丁：

1. 验证 magic (`PATCH_DATA_MAGIC`) 和 version (v1 或 v2)
2. 验证 header_size ≥ 最小值（v1: 112, v2: sizeof(PatchDataHeader)）
3. 验证 structural integrity（`body_data_offset + body_data_size` ≤ 总大小，包含 AotCoreIr section）
4. 创建 PatchContext（域分配）
5. 遍历 MethodDef 条目，通过 HotpatchNameRegistry 匹配方法
6. 对每个匹配方法执行 `PatchMethodLowerIR`（降低 IR → 分配 call_cache → 注册化）
7. 执行 `ReapplyInlining`（第二轮内联，捕获因调用者 IR 不可用而错过的机会）
8. 返回 PatchContext 或 nullptr

### Unpatch(ctx)

清除所有补丁方法的 dispatch 条目（清除 `kHotpatchActive` 标志）：
- 线程安全（无需额外锁 — 每个 dispatch 条目是独立的 `atomic<uint32_t>` flag）

### DestroyPatchContext(ctx)

释放补丁上下文所有资源：
1. 销毁 PatchMetadataCache（显式析构）
2. 对每个方法：`delete cached_ir`、`DOMAIN_CURRENT_FREE(call_cache)`、`delete[] cached_arg_types`（大缓冲区路径）
3. 销毁 PatchMethod 数组
4. `DomainFreeTagged(ctx)` — 释放整个上下文块

## 内存泄漏防护 (Phase 2 修复)

`ReapplyInlining` 和 `InlineLeafCallees` 在替换 `call_cache` 时：
1. 保存旧 `call_cache` 指针
2. 分配新 `call_cache`（域分配）
3. 调用 `CHAOS_IL2CPP_DOMAIN_CURRENT_FREE(old_cc)` 释放旧指针
4. 设置 `patch_method.call_cache = new_cc`

`DestroyPatchContext` 释放当前 `call_cache` 指针。所有中间分配都在域销毁时批量释放。

## 溢出保护 (Phase 2 加固)

`CreatePatchContext` 中：
- **乘法溢出**: `sizeof(PatchMethod) * method_count` 通过除法回验（`result / sizeof(PatchMethod) == method_count`）
- **加法溢出**: `ctx_size + cache_size + methods_size` 通过 `total_alloc < ctx_size || total_alloc < cache_size` 检查

这些保护防止格式错误的补丁数据导致内存分配不足。

## 与其他组件集成

| 组件 | 集成点 | 方向 |
|------|--------|------|
| **AssemblyManager** | LoadAssembly 调用 ApplyPatchFromMemory；UnloadAssembly 调用 Unpatch + DestroyPatchContext | AssemblyManager → PatchLoader |
| **MemoryDomain** | PatchContext 在域堆上分配 | PatchLoader → MemoryDomain |
| **HotpatchNameRegistry** | `ApplyPatchFromMemory` 通过 registry 定位补丁目标方法 | PatchLoader → hotpatch_table |
| **ModuleRegistry** | module_id 来自 AOT 模块，用于方法查找 | PatchLoader → ModuleRegistry |
| **Interpreter VM** | `PatchMethodLowerIR` 降低 IR；`ReapplyInlining` 内联 | PatchLoader → interpreter |
| **InstantiationEngine** | `PrecacheCallTarget` 预缓存调用目标 | PatchLoader → instantiation_engine |

## 测试覆盖

`patch_loader_test.cpp` (Loader 工业化 Phase 1)：
- 验证守卫条件 6 种（null/zero patch_data、magic、version、header_size、size）
- PatchMetadataCache 边界 8 种（GetString/GetBlob/GetBody null → ""/nullptr）
- PatchMethod 结构验证
- 死代码补完确认（GetFieldDef、GetTypeDefByIndex 启用）
