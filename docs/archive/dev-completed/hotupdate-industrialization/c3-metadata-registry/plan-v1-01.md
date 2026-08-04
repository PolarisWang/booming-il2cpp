# C3: MetadataRegistry — 统一元数据注册表 实现计划

> **面向执行 Agent：** 必须使用 dev:executing-plans 来执行本计划。步骤使用复选框（`- [ ]`）语法跟踪。执行完成时必须先归档当前子任务目录，再同步父任务 hotupdate-industrialization。

**目标：** 在已有 `ModuleLifecycleManager` + `ModuleAwareResolveXxx` 基础上，补齐缺口并正式化为 MetadataRegistry，消除 AOT/热更新双分元数据体系。

**架构：** 现有 `ModuleLifecycleManager`（在 `metadata_interface.h/.cpp` 中）已实现三合一注册（ModuleRegistry + HotpatchNameRegistry + GenericContext）。`ModuleAwareResolveTypeInfo/Method/Field` 已提供统一查询。C3 需要：(1) 补齐缺少的查询能力和注册路径；(2) 新增 hash-based 快速查找缓存；(3) 连接 AOT bootstrap 和 hot-update 加载路径到统一注册；(4) 新增 contract 头文件供 Phase 2/3（调试器/寄存器 VM）消费；(5) 测试。

**技术栈：** C++20, CHAOS_IL2CPP_UNORDERED_DENSE_MAP_IDENTITY, metadata_interface.h, module_registry.h

**架构审核模式：** critical（涉及 AOT bootstrap + contract + runtime-core）

**结构告警重点：** 现有 ModuleLifecycleManager 职责边界模糊（既管注册又管生命周期查询）。C3 需要明确分层：LifecycleManager 只管注册/反注册，MetadataRegistry 提供统一的查询接口。

**权责图审核主题：** 
- `ModuleLifecycleManager` → 模块注册/反注册（写路径）
- `MetadataRegistry`（新建） → 统一查询（读路径）
- `ModuleAwareResolveXxx` → 逐步退役，由 MetadataRegistry 替代

**AOT/IL2CPP/Test Governance Intake：**
- capabilityFamily: n/a（基础设施改造）
- capabilityItem: n/a
- ownerSubjectId: n/a
- proofRequired: false
- benchmarkRequired: false
- hotupdateImpact: Smoke
- formalVerificationObjects: []
- requiredGates: [collector -> registry -> workspace]

**设计文档：** n/a（设计在父 brainstorm 中已拍板）

**问题清零来源：** parent-roadmap (hotupdate-industrialization, clearance confirmed)

**计划来源：** roadmap-child-auto

**预期知识沉淀：** wiki/03-功能模块/06-il2cpp核心架构/01-翻译管线/22-统一元数据注册表.md

**收尾约束：** 执行完成后必须进入"结构告警与架构审视 -> 测试通过 -> 归档 completed -> 合并&提交"固定链路。

---

## 现有基础设施评估

### 已存在（不重复造轮子）

| 组件 | 文件 | 功能 | C3 处理 |
|------|------|------|---------|
| `ModuleLifecycleManager` | `metadata_interface.h/.cpp` | 三合一注册（ModuleReg + HotpatchNameReg + GenericContext） | 保留，补充 RegisterHotpatchContext 实现 |
| `ModuleAwareResolveTypeInfo` | `metadata_interface.h/.cpp` | 统一类型解析（AOT/热更/动态路径） | 保留，优化 hash 查找缓存 |
| `ModuleAwareResolveMethod` | `metadata_interface.h/.cpp` | 统一方法解析 | 保留，补充 hotpatch 路径 |
| `ModuleAwareResolveField` | `metadata_interface.h/.cpp` | 统一字段解析 | 保留 |
| `ModuleAwareResolveToken` | `metadata_interface.h/.cpp` | 通用 token 解析 | 补充实现（当前多数表格返回 0） |
| `LookupCallTarget` | `metadata_interface.h/.cpp` | subject_id → MethodInfoHandle | 保留 |
| `ModuleRegistry` | `module_registry.h/.cpp` | 模块存储、module_id 分配、tombstone | 保留 |
| `HotpatchNameRegistry` | `hotpatch_table.h/.cpp` | 两级名称查找、Token→Slot 映射 | 保留，MetadataRegistry 内嵌引用 |

### 需要新增/修改

| 项 | 说明 | 文件 |
|----|------|------|
| `MetadataRegistry` 类 | 统一读路径的入口类，封装 ModuleAwareResolveXxx | `metadata_interface.h/.cpp`（扩展现有文件）|
| `contracts/native/v0/unified_metadata.h` | 供调试器/寄存器 VM 消费的统一查询接口 | 新建 |
| hash-based 查找缓存 | 替换部分 bsearch 路径 | 新增 internal 结构，不影响现有 .rodata 布局 |
| `EnumerateMethods` | 调试器断点列表支持 | 新增到 MetadataRegistry |
| AOT bootstrap 连接 | 使 AOT 启动走 ModuleLifecycleManager | `bootstrap.cpp` (待定位) |
| Hot-update 加载连接 | 使 LoadHotUpdatePackage 走 ModuleLifecycleManager | `hot_update.cpp` |
| RegisterHotpatchContext 实现 | 当前是 stub | `metadata_interface.cpp` |
| ModuleAwareResolveToken 补齐 | 当前多种表格返回 0 | `metadata_interface.cpp` |

## 执行步骤

### Step 1: 代码上下文确认（~30 min）

- [ ] 定位 AOT bootstrap 入口：搜索 `CodegenBridgeV0` 的调用方，确认 AOT 模块注册路径
- [ ] 确认 HotpatchNameRegistry 当前在 AOT bootstrap 中的注册位置
- [ ] 确认 LoadHotUpdatePackage 当前不经过 ModuleLifecycleManager 的确凿证据
- [ ] 确认 RegisterHotpatchContext 的 stub 状态

### Step 2: 新建 contract 头文件 `contracts/native/v0/unified_metadata.h`（~2h）

```cpp
// unified_metadata.h — 统一元数据查询接口（面向 VM / 调试器 / OSR）
//
// 为 Phase 2（寄存器式 VM）和 Phase 3（调试器）提供
// 不依赖 metadata_interface.h 内部实现的纯接口声明。

#ifndef CHAOS_UNIFIED_METADATA_H_
#define CHAOS_UNIFIED_METADATA_H_

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

// Opaque handle types
typedef uint64_t TypeHandle;     // composite: (module_id<<32) | type_token
typedef uint64_t MethodHandle;   // composite: (module_id<<32) | method_token
typedef uint64_t FieldHandle;    // composite: (module_id<<32) | field_token

// Module classification
#define CHAOS_METADATA_SOURCE_AOT      0x01
#define CHAOS_METADATA_SOURCE_HOTPATCH 0x02
#define CHAOS_METADATA_SOURCE_DYNAMIC  0x04
#define CHAOS_METADATA_SOURCE_ANY      0xFF

// ── MetadataRegistry v0 （过程内查询接口）──
// 挂载在 CodegenBridgeV0 或通过全局单例访问

typedef struct UnifiedMetadataRegistryV0 {
    uint32_t abi_version;
    uint32_t struct_size;

    // 类型解析
    TypeHandle (*resolve_type)(uint32_t module_id, uint32_t type_token);
    const char* (*get_type_name)(TypeHandle type, const char** out_namespace);

    // 方法解析
    MethodHandle (*resolve_method)(uint32_t module_id, uint32_t method_token);
    const char* (*get_method_name)(MethodHandle method);

    // 字段解析
    FieldHandle (*resolve_field)(uint32_t module_id, uint32_t field_token);

    // 跨模块查找
    uint64_t (*find_token)(uint32_t method_token);
    // 返回值: (module_index<<32) | token, 0 = not found

    // 调试器支持
    uint32_t (*enumerate_methods)(
        uint32_t module_id,
        void** out_methods,    // 输出缓冲区
        uint32_t max_count);   // 缓冲区大小

} UnifiedMetadataRegistryV0;

#ifdef __cplusplus
}
#endif

#endif
```

### Step 3: 扩展 `metadata_interface.h` — 新增 MetadataRegistry 类（~4h）

- [ ] 在 `namespace chaos::il2cpp::runtime_core` 中新增 `MetadataRegistry` 类
- [ ] `MetadataRegistry` 封装 `ModuleAwareResolveTypeInfo/Method/Field/Token`
- [ ] 新增 hash-based 查找缓存：`CHAOS_IL2CPP_UNORDERED_DENSE_MAP_IDENTITY<uint64_t, void*>` 用于 token→handle 快速查找
- [ ] 新增 `EnumerateMethods(module_id, callback, ctx)` 接口
- [ ] MetadataRegistry 引用 HotpatchNameRegistry 实例（不重复注册）
- [ ] MetadataRegistry 导出 `GetUnifiedRegistryV0()` 以获得 `UnifiedMetadataRegistryV0` 接口指针

**主要接口：**

```cpp
class MetadataRegistry {
public:
    static MetadataRegistry& Get() noexcept;

    // 读路径——统一查询
    TypeInfoHot* ResolveType(uint32_t module_id, uint32_t type_token) noexcept;
    const char* GetTypeName(TypeHandle type, const char** out_namespace) noexcept;
    MethodInfoHandle ResolveMethod(uint32_t module_id, uint32_t method_token) noexcept;
    const char* GetMethodName(MethodHandle method) noexcept;
    FieldInfoHandle ResolveField(uint32_t module_id, uint32_t field_token) noexcept;
    uint64_t FindToken(uint32_t method_token) noexcept;
    uint32_t EnumerateMethods(uint32_t module_id, void** out_buf, uint32_t max) noexcept;

    // 导出统一接口指针
    const UnifiedMetadataRegistryV0* GetUnifiedRegistry() noexcept;

private:
    // 快速查找缓存（按需填充）
    CHAOS_IL2CPP_UNORDERED_DENSE_MAP_IDENTITY(uint64_t, void*) resolve_cache_;
    std::atomic<bool> cache_built_{false};
    
    void BuildCache() noexcept;
};
```

### Step 4: 实现 `metadata_interface.cpp` — MetadataRegistry 实现（~6h）

- [ ] `ResolveType` — 调用现有 `ModuleAwareResolveTypeInfo`，结果缓存到 `resolve_cache_`
- [ ] `ResolveMethod` — 调用现有 `ModuleAwareResolveMethod`，补充 hotpatch 路径
- [ ] `ResolveField` — 调用现有 `ModuleAwareResolveField`
- [ ] `FindToken` — 遍历 ModuleRegistry + HotpatchNameRegistry，返回 composite key
- [ ] `EnumerateMethods` — 遍历模块的 type table + method table，填充输出缓冲区
- [ ] `BuildCache` — 启动时构建所有已知模块的 token→handle 映射
- [ ] `GetUnifiedRegistry` — 返回静态 `UnifiedMetadataRegistryV0` 实例

### Step 5: 补齐 `ModuleAwareResolveToken`（~2h）

- [ ] 补充 MethodDef token（0x06）的解析路径
- [ ] 补充 FieldDef token（0x04）的解析路径
- [ ] 补充 MemberRef token（0x0A）的解析路径

### Step 6: 连接 AOT bootstrap 到 ModuleLifecycleManager（~4h）

- [ ] 定位 AOT bootstrap 入口（`bootstrap.cpp` 或等效）
- [ ] 确认当前 AOT 模块注册路径（CodeRegistrationV0 → module_registry）
- [ ] 修改为通过 `ModuleLifecycleManager::RegisterModule` 一次性注册
- [ ] 确保模块号（module_id=0 for AOT root）一致

### Step 7: 连接 HotUpdate 加载到 ModuleLifecycleManager（~2h）

- [ ] 在 `hot_update.cpp::LoadHotUpdatePackage` 中，新增调用 `ModuleLifecycleManager::RegisterModule`
- [ ] 在 `hot_update.cpp::UnloadHotUpdatePackage` 中，新增调用 `ModuleLifecycleManager::UnregisterModule`
- [ ] 传递 hotpatch 模块数据（从 .patchdata 解析后）

### Step 8: 实现 `RegisterHotpatchContext`（~3h）

- [ ] 当前是 stub，返回 true
- [ ] 需要实现：对 PatchContext 中的每个方法，激活 hotpatch dispatch entry（`SetPatchedBySlot`）
- [ ] 记录活跃的 patch context 以便 UnregisterHotpatchContext 撤销

### Step 9: 测试（~8h）

- [ ] 创建 `testing/src/native/runtime-core/metadata/metadata_registry_test.cpp`
- [ ] 测试 1: `AOT registration` — 模拟 AOT 模块注册，验证查询
- [ ] 测试 2: `HotUpdate registration` — 模拟热更新模块注册，验证查询
- [ ] 测试 3: `Cross-module query` — 复合键查询，验证 module_id 隔离
- [ ] 测试 4: `Token collision` — 验证两个模块有相同 token 时正确按 module_id 区分
- [ ] 测试 5: `EnumerateMethods` — 验证调试器枚举接口
- [ ] 测试 6: `UnregisterModule` — 验证卸载后查询返回 nullptr
- [ ] 测试 7: `FindToken across modules` — 跨模块 FindToken 正确性
- [ ] 集成验证：编译全部 hotupdate 测试目标，确认全部通过

### Step 10: 结构告警与架构审视（~1h）

- [ ] 检查 `ModuleLifecycleManager` 和 `MetadataRegistry` 的职责边界是否清晰
- [ ] 检查是否存在重复的解析路径
- [ ] 检查 `unified_metadata.h` contract 是否与 `metadata_interface.h` 定义一致
- [ ] 检查新增代码是否符合项目的 include 规范、命名空间规范

### Step 11: 归档与同步父任务（~30 min）

- [ ] 全部测试通过
- [ ] 更新 `c3-metadata-registry/STATUS.md` 为 `phase: completed`
- [ ] 物理移动目录到 `docs/dev/completed/hotupdate-industrialization/c3-metadata-registry/`
- [ ] 更新父 `hotupdate-industrialization/STATUS.md` 将 C3 status 改为 completed
- [ ] 更新 `recommended_next_child` 为 C6
- [ ] 提交并 push

## 文件影响清单

| 文件 | 操作 | 说明 |
|------|------|------|
| `contracts/native/v0/unified_metadata.h` | **新建** | 统一元数据查询接口（面向 VM/调试器） |
| `src/native/runtime-core/metadata_interface.h` | 修改 | 新增 MetadataRegistry 类声明 |
| `src/native/runtime-core/metadata_interface.cpp` | 修改 | MetadataRegistry 实现 + ResolveToken 补齐 + RegisterHotpatchContext 实现 |
| `src/native/hot-update/hot_update.cpp` | 修改 | LoadPackage/UnloadPackage 连接 ModuleLifecycleManager |
| `src/native/runtime-core/CMakeLists.txt` | 修改 | 检查是否需要新增编译单元 |
| `testing/src/native/runtime-core/metadata/metadata_registry_test.cpp` | **新建** | C3 测试 |
| `testing/src/native/runtime-core/metadata/CMakeLists.txt` | **新建** | 测试构建配置 |
| AOT bootstrap 文件（待定位） | 修改 | 连接 ModuleLifecycleManager |

## 关键测试与验证命令

```bash
# 编译测试
cmake --build build/testing --config CHECK
# 运行 C3 专项测试
./build/testing/runtime-core/metadata/CHECK/test_metadata_registry.exe
# 回归已有 hotupdate 测试
ctest --test-dir build/testing -R hotupdate -C CHECK
# 全部测试
ctest --test-dir build/testing -C CHECK
```

## 交接说明

执行完成后，父 roadmap 的 `recommended_next_child` 应更新为 **C6: 跨平台原子操作抽象**（Phase 1 第二个子任务）。C6 工作量较小（~1 周），可与 C3 的后续优化交替进行。
