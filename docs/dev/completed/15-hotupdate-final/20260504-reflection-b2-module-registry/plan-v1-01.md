# B2 Module Registry + Two-Tier Metadata 实现计划

> **面向执行 Agent：** 必须使用 dev:subagent-driven-development（如果可用子 Agent）或 dev:executing-plans 来执行本计划。步骤使用复选框（`- [ ]`）语法跟踪。完成时归档当前任务目录。

**目标：** 实现 B2 架构——全局 Module Registry + Two-Tier 分层元数据 + `(module_id, token)` handle 编码，替代现有 TLS `g_current_aot_image` 方案

**架构：** 全局 `ModuleDescriptor[]` registry 替代 `thread_local` 指针；`TypeInfoHandle` 从 `void*` 改为 `(module_id:32 << 32 | token:32)` 的 `uint64_t`；per-DLL 生成 Tier 1（flags）和 Tier 2（methods/fields 条件生成）元数据头文件

**技术栈：** C++17 constexpr flags 数组 + 全局 registry + emitter 增量生成 Scriban/fmt 输出

**架构审核模式：** critical — 涉及 IL2CPP codegen + runtime-core + contracts 主线

**结构告警重点：** 文件职责与拆分（registry 单一职责、emitter 端模块化输出、native 端解耦）

**权责图审核主题：** codegen (emitter 端) → contracts (handle 定义) → runtime-core (registry + 查询函数)，确认无权责交叉

**AOT/IL2CPP/Test Governance Intake：**
- capabilityFamily: System.Private.CoreLib/reflection/type (all families)
- capabilityItem: reflection metadata infrastructure
- ownerSubjectId: ReflectionTypeNativeEntry/ReflectionTypeNativeEntry::Run(System.Int32)
- proofRequired: true
- benchmarkRequired: false
- hotupdateImpact: Proof — B2 核心设计目标是 HotUpdate 可适配
- formalVerificationObjects: [ModuleRegistry 注册/查询, TypeInfoHandle encode/decode, cross-DLL type identity test]
- requiredGates: collector -> registry -> workspace

**设计文档：** n/a（对话中设计方案对比与确认）

**问题清零来源：** direct user confirmation（用户确认 B2 方案）

**计划来源：** direct-plan

**预期知识沉淀：** wiki/03-功能模块/il2cpp-核心架构/ — 需更新 reflection metadata 架构决策

**收尾约束：** 执行完成后必须进入"结构告警与架构审视 -> 测试通过 -> 归档 completed -> 合并&提交"固定链路。

---

## B2 架构要点

### TypeInfoHandle 格式

```cpp
// 现有: typedef void* TypeInfoHandle;
// B2:
typedef uint64_t TypeInfoHandle;  // 8 bytes

// 编码:
//   [module_id:32 bits] [token:32 bits]
//   module_id = registry index（0 = CoreLib fallback）
//   token = ECMA metadata token（例如 0x02000001）

// 在 generated.cpp 中构造:
constexpr TypeInfoHandle MakeTypeHandle(uint32_t module_id, uint32_t token) {
    return (static_cast<uint64_t>(module_id) << 32) | token;
}
```

### Module Registry

```cpp
// runtime-core 中全局 registry
constexpr uint32_t kMaxModules = 64;

struct ModuleDescriptor {
    const char* name_utf8;
    const ReflectionQueryImageDescriptor* image;
};

// 全局 registry（非 constexpr，在启动时由各 DLL 填充）
extern ModuleDescriptor g_modules[kMaxModules];
extern uint32_t g_module_count;
extern chaos::il2cpp::runtime_core::spin_lock g_module_lock;

// 注册 API — 在每个 AOT DLL 的 RunNativeAot() 入口调用
uint32_t RegisterModule(const char* name, const ReflectionQueryImageDescriptor* image);
```

### Two-Tier Per-DLL 生成

每个 AOT DLL 生成 `native-aot.module.h`：

```
// Tier 1（固定生成，~16 bytes/type）:
//   - type_flags[] (uint32_t per type)
//   - type_names[] (const char* per type)
//   - type_namespaces[] (const char* per type)
//   - type_parent_tokens[] (uint32_t per type)
//   - type_name_to_token_index 映射（用于 get_Type by name）

// Tier 2（条件生成，仅当 family 使用 GetMethods/GetFields/Invoke 时）:
//   - ReflectionQueryMethodDescriptor[] — per type
//   - ReflectionQueryFieldDescriptor[] — per type
//   - fully constructed ReflectionQueryTypeDescriptor
```

---

## 文件清单

### 新增文件（3 个）

| 文件 | 职责 |
|------|------|
| `src/native/runtime-core/module_registry.h` | `ModuleDescriptor` 定义、`RegisterModule()`、`LookupModule()`、`TypeInfoHandle` encode/decode helper |
| `src/native/runtime-core/module_registry.cpp` | 全局 `g_modules[]` 数组、`RegisterModule()` 实现、tls/spin_lock 同步 |
| `src/native/runtime-core/reflection_metadata_fallback.h` | 现有 `reflection_metadata_impl.h` 的升级版——CoreLib fallback 表适配新的 handle 模式 |

### 修改文件（7 个）

| 文件 | 职责 |
|------|------|
| `contracts/native/v0/runtime_abi.h` | `TypeInfoHandle` typedef: `void*` → `uint64_t` |
| `src/native/runtime-core/reflection_api.cpp` | 全部查询函数改为通过 Module Registry 查表，不依赖 TLS image 指针 |
| `src/native/runtime-core/reflection_metadata_impl.h` | 适配新 handle 模式（可选——可保留旧表做 fallback） |
| `src/native/runtime-core/runtime_core.h` | 删除 `SetCurrentAotImage/GetCurrentAotImage` 声明，保留 `RegisterModule` 声明 |
| `src/native/runtime-core/runtime_core.cpp` | 删除 `g_tls_aot_image` 及相关 TLS 代码 |
| `src/managed/Chaos.IL2CPP.CodeGen/NativeAotLoweringPlanner.cs` | 新增 `BuildModuleHeader()` 方法；可扫描 closure 判断是否需要 Tier 2 |
| `src/managed/Chaos.IL2CPP.CodeGen/NativeAotEmitter.cs` | `generatedSources` 中增加 `NativeAotArtifactNames.ModuleHeader` |
| `src/managed/Chaos.IL2CPP.Contracts/ManagedClosureArtifactModels.cs` | 新增 `NativeAotArtifactNames.ModuleHeader` 常量 |

### 可能受影响的文件（机械性 typedef 替换）

| 文件 | 改动量 | 说明 |
|------|--------|------|
| `contracts/native/v0/runtime_abi.h` | ~5 处 | `TypeInfoHandle` 定义 + 相关 handle typedef |
| `src/native/runtime-core/*.h` | ~10 处 | `TypeInfoHandle != nullptr` → `!= 0` |
| `src/native/runtime-core/*.cpp` | ~10 处 | 同上 |
| 不涉及 generated.cpp | 0 | generated.cpp 里目前不直接使用 `TypeInfoHandle` |

---

## 实现步骤

### Step 0: Handle 类型迁移（void* → uint64_t）

**目标：** 把 `contracts/native/v0/runtime_abi.h` 中的 `TypeInfoHandle` 从 `void*` 改为 `uint64_t`，修复所有使用处的编译错误。

```cpp
// runtime_abi.h
typedef uint64_t TypeInfoHandle;     // 原来是 void*
typedef uint64_t MethodInfoHandle;   // 同步迁移
typedef uint64_t FieldInfoHandle;    // 同步迁移
typedef uint64_t PropertyInfoHandle;
typedef uint64_t ParameterInfoHandle;
typedef uint64_t ImageHandle;
```

**迁移规则：**
- `handle == nullptr` → `handle == 0`
- `handle != nullptr` → `handle != 0`
- `reinterpret_cast<const T*>(handle)` → 需要查表，改为 `LookupModule(GetModuleId(handle))` + token decode
- 所有 handle 相关的 encode/decode helper 需更新

**风险缓解：** typedef 在头文件中，所有 native 编译单元同步修复。机械性修改，用 `rg -l` + sed 批量处理 + 编译验证。

### Step 1: Module Registry（runtime-core）

**1a. `module_registry.h`**

```cpp
namespace chaos::il2cpp::runtime_core {

constexpr uint32_t kModuleIdBits = 32;
constexpr uint32_t kMaxModules = 64;
constexpr uint32_t kInvalidModuleId = 0xFFFFFFFFu;

struct ModuleDescriptor {
    const char* name_utf8;
    const ReflectionQueryImageDescriptor* image;
    const uint32_t* type_flags;      // Tier 1: [type_count]
    const char* const* type_names;   // Tier 1: [type_count]
    const char* const* type_namespaces; // Tier 1: [type_count]
    const uint32_t* type_parent_tokens; // Tier 1: [type_count]
    uint32_t type_count;
};

// ── TypeInfoHandle encode/decode ──
inline uint32_t GetModuleId(TypeInfoHandle handle) {
    return static_cast<uint32_t>(handle >> 32);
}
inline uint32_t GetTypeToken(TypeInfoHandle handle) {
    return static_cast<uint32_t>(handle & 0xFFFFFFFFu);
}
inline TypeInfoHandle MakeTypeHandle(uint32_t module_id, uint32_t token) {
    return (static_cast<uint64_t>(module_id) << 32) | token;
}

// ── Registry API ──
uint32_t RegisterModule(const char* name, const ModuleDescriptor* descriptor);
const ModuleDescriptor* LookupModule(uint32_t module_id);
const ModuleDescriptor* LookupModuleByName(const char* name);

}  // namespace
```

**1b. `module_registry.cpp`**

```cpp
#include "module_registry.h"
#include <cstring>

namespace chaos::il2cpp::runtime_core {

// 使用足够对齐的存储，避免静态初始化顺序问题
static ModuleDescriptor g_module_storage[kMaxModules] = {};
static uint32_t g_module_count = 1;  // [0] = CoreLib fallback, 永远存在

uint32_t RegisterModule(const char* name, const ModuleDescriptor* descriptor) {
    // 简单数组，无锁（AOT 启动是串行的）
    uint32_t id = g_module_count;
    g_module_storage[id] = *descriptor;
    g_module_count++;
    return id;
}

const ModuleDescriptor* LookupModule(uint32_t module_id) {
    if (module_id >= g_module_count) return nullptr;
    return &g_module_storage[module_id];
}

}  // namespace
```

### Step 2: CoreLib Fallback 元数据

将现有 `reflection_metadata_impl.h` 中的 7 个手写类型改为 `ModuleDescriptor` 格式（flags + name ptr），注册到 `g_modules[0]`。

```cpp
// reflection_metadata_fallback.h
namespace chaos::il2cpp::runtime_core::fallback {

constexpr uint32_t kFallbackTypeFlags[] = {
    [0] = 0,                                           // Object
    [1] = kFlagIsValueType,                             // ValueType
    [2] = kFlagIsValueType | kFlagIsEnum,               // Enum
    [3] = kFlagIsValueType,                             // Byte
    [4] = 0,                                            // String
    [5] = kFlagIsAbstract,                              // Type
    [6] = 0,                                            // Exception
};

constexpr uint32_t kFallbackTypeParentTokens[] = {
    [0] = 0,                                            // Object → null
    [1] = 0x02000001,                                   // ValueType → Object
    [2] = 0x02000002,                                   // Enum → ValueType
    [3] = 0x02000002,                                   // Byte → ValueType
    [4] = 0x02000001,                                   // String → Object
    [5] = 0x02000001,                                   // Type → Object
    [6] = 0x02000001,                                   // Exception → Object
};

// names, namespaces 同理
constexpr ModuleDescriptor kFallbackModule = {
    .name_utf8 = "System.Private.CoreLib",
    .image = nullptr,
    .type_flags = kFallbackTypeFlags,
    .type_names = kFallbackTypeNames,
    .type_namespaces = kFallbackTypeNamespaces,
    .type_parent_tokens = kFallbackTypeParentTokens,
    .type_count = 7,
};

}  // namespace
```

在 `runtime_core.cpp` 启动时注册：
```cpp
RegisterModule("System.Private.CoreLib", &fallback::kFallbackModule);
```

### Step 3: Emitter — 生成 per-DLL native-aot.module.h

**3a. `ManagedClosureArtifactModels.cs`** — 新增常量

```csharp
public const string ModuleHeader = "generated/native-aot.module.h";
```

**3b. `NativeAotLoweringPlanner.cs`** — 新增 `BuildModuleHeader()` 方法

输入：`ManagedClosureManifestArtifact` 中的 `.Types` + 引用类型 metadata token 列表 + closure 扫描结果（是否使用了 GetMethods/GetFields）

输出格式：

```cpp
// Auto-generated AOT module header
// Family: <family-name>
#pragma once
#include "module_registry.h"

namespace <sanitized_family_name>::aot_module {

// ── Tier 1: Type flags ──
constexpr uint32_t kTypeFlags[] = {
    0,                          // 0x02000001: System.Object
    kFlagIsValueType,           // 0x02000002: System.ValueType
    // ...
};

constexpr const char* kTypeNames[] = {
    "Object", "ValueType", "Byte", /* ... */
};
constexpr const char* kTypeNamespaces[] = {
    "System", "System", "System", /* ... */
};
constexpr uint32_t kTypeParentTokens[] = {
    0, 0x02000001, 0x02000002, /* ... */
};

// ── Tier 2: Conditionally generated ──
// (only if family uses GetMethods/GetFields)

constexpr ModuleDescriptor kModule = {
    .name_utf8 = "<module-name>",
    .image = nullptr,  // not used in B2
    .type_flags = kTypeFlags,
    .type_names = kTypeNames,
    .type_namespaces = kTypeNamespaces,
    .type_parent_tokens = kTypeParentTokens,
    .type_count = <N>,
};

}  // namespace
```

**3c. `NativeAotEmitter.cs`** — 新增 `ModuleHeader` 到 generatedSources

**3d. 修改 `Create()`** — 注入 `#include "generated/native-aot.module.h"` 和 `RegisterModule()` 调用：

```cpp
// 在 RunNativeAot() 开头（generated.cpp 中）
#include "generated/native-aot.module.h"

extern "C" void RunNativeAot() {
    uint32_t module_id = RegisterModule(
        "System.Private.CoreLib/reflection-type",
        &reflection_type::aot_module::kModule);
    // ... rest of generated code uses module_id to build TypeInfoHandle
}
```

### Step 4: 重写 reflection_api.cpp

用 Module Registry 替换 TLS `g_current_aot_image` 模式。

```cpp
// ── Hot paths ──

extern "C" CHAOS_IL2CPP_INTPTR chaos_reflection_get_is_value_type(
    CHAOS_IL2CPP_INTPTR type_handle)
{
    auto handle = static_cast<TypeInfoHandle>(type_handle);
    uint32_t module_id = GetModuleId(handle);
    uint32_t token = GetTypeToken(handle);
    auto* module = LookupModule(module_id);
    if (!module || !module->type_flags) return 0;
    uint32_t type_idx = TokenToIndex(token);
    if (type_idx >= module->type_count) return 0;
    return (module->type_flags[type_idx] & kFlagIsValueType) ? 1 : 0;
}

extern "C" CHAOS_IL2CPP_INTPTR chaos_reflection_get_member_name(
    CHAOS_IL2CPP_INTPTR member_handle)
{
    auto handle = static_cast<TypeInfoHandle>(member_handle);
    uint32_t module_id = GetModuleId(handle);
    uint32_t token = GetTypeToken(handle);
    auto* module = LookupModule(module_id);
    if (!module || !module->type_names) return 0;
    uint32_t type_idx = TokenToIndex(token);
    if (type_idx >= module->type_count) return 0;
    return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(module->type_names[type_idx]);
}

// ── Cold paths（Tier 2）──

extern "C" CHAOS_IL2CPP_INTPTR chaos_reflection_get_methods(
    CHAOS_IL2CPP_INTPTR type_handle)
{
    // 需要 Tier 2 metadata → 通过 image descriptor 拿到 methods 数组
    auto handle = static_cast<TypeInfoHandle>(type_handle);
    uint32_t module_id = GetModuleId(handle);
    auto* module = LookupModule(module_id);
    if (!module || !module->image) return 0;
    auto* typeDesc = FindTypeInImage(module->image, GetTypeToken(handle));
    if (!typeDesc || !typeDesc->methods) return 0;
    return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(typeDesc->methods);
}
```

### Step 5: 更新旧 reflection_api.cpp 函数

当前 `reflection_api.cpp` 中所有 `GetCurrentImage()` / `GetTypeDescriptorFromHandle()` 调用路径都需要替换为 Module Registry 模式。

关键替换：
- `GetCurrentImage()` → `LookupModule(module_id)->image`
- `DecodeMetadataToken(handle)` → `GetTypeToken(handle)`（token 已在 handle 中）
- `GetTypeDescriptorFromHandle(handle)` → 使用新的 `FindTypeInImageByToken()`

### Step 6: Generated.cpp 输出更新

当前 emitter 在 generated.cpp 中输出：

```cpp
// SetCurrentAotImage(&chaos::il2cpp::runtime_core::aot_metadata::kImageCoreLib);  // COMMMENTED
```

需要改为：

```cpp
#include "generated/native-aot.module.h"

extern "C" void RunNativeAot() {
    uint32_t g_module_id = RegisterModule(
        "System.Private.CoreLib/reflection-type",
        &reflection_type::aot_module::kModule);
    // ... rest
}
```

同时所有 `TypeInfoHandle` 的构造点，从 `reinterpret_cast<TypeInfoHandle>(constexpr ptr)` 改为 `MakeTypeHandle(g_module_id, metadata_token)`。

**但注意：** generated.cpp 中的 TypeInfoHandle 用法需要先确认——当前 generated.cpp 中是否直接操作 TypeInfoHandle。之前的调研显示 generated.cpp 主要调 `chaos_reflection_*` 函数，参数是 `CHAOS_IL2CPP_INTPTR`，不直接使用 `TypeInfoHandle` 类型。验证后如果确实不直接使用，则 generated.cpp 受影响极小（只需在 emitted source 中增加 RegisterModule 调用）。

### Step 7: Wiki 更新

**更新文件：** `wiki/03-功能模块/il2cpp-核心架构/`

需要更新/新增的内容：
1. **架构决策记录：** B2 Module Registry + Two-Tier Metadata 方案（替代原有 TLS image 方案）
2. **TypeInfoHandle 格式：** `(module_id, token)` uint64_t 编码
3. **Module Registry API：** `RegisterModule()`、`LookupModule()` 契约
4. **Two-Tier 生成规则：** 什么条件下生成 Tier 2（methods/fields）
5. **跨 DLL 类型身份保证方式：** 为什么 `(module_id, token)` 比指针更可靠

---

## 执行顺序

```
Step 0: Handle 类型迁移 (void* → uint64_t)
    │
    ▼
Step 1: Module Registry (module_registry.h/.cpp)
    │
    ▼
Step 2: CoreLib Fallback 元数据
    │
    ▼
Step 3: Emitter 生成 per-DLL module header
    │
    ▼
Step 4: 重写 reflection_api.cpp
    │
    ▼
Step 5: 更新旧 reflection_api.cpp 函数
    │
    ▼
Step 6: Generated.cpp 输出更新
    │
    ▼
Step 7: Wiki 更新
    │
    ▼
验证: build 0 error + reflection-type family PASS
```

**Step 0→1→2 可并行**（头文件定义无关，先确认 Step 0 通过编译即可）。Step 3 依赖 Step 1 的 `module_registry.h` 最终格式。Step 4 依赖 Step 1+2。Step 6 依赖 Step 3。

---

## 边界与非目标

- **不做**：不在本计划中实现 GetMethods 完整托管数组构造——Tier 2 只生成 constexpr 数组指针，数组包装（分配托管 `MethodInfo[]`）留到后续
- **不做**：不在本计划中实现 `Invoke` / `CreateInstance` — 这些需要 RuntimeState + GC 分配能力
- **包含**：全部 hot-path 类型属性（IsValueType, IsEnum, IsAbstract, IsSealed, IsInterface, IsArray, IsGenericType 等）
- **包含**：Name/Namespace/BaseType 查询
- **包含**：Handle 编解码 + Module Registry + 跨 DLL 身份
- **包含**：Tier 2 的条件生成框架（即使当前没有 family 用到，框架先搭好，后续增量填数据）

---

## 关键测试与验证命令

```bash
# Step 0 验证
dotnet build Chaos.IL2CPP.CodeGen.csproj    # 0 errors
cmake --build . --target chaos_runtime_core   # 0 errors

# Step 3 验证
dotnet run --project Chaos.IL2CPP.Driver -- emit-native-aot \
  verification/.../reflection-type/.../analysis \
  verification/.../reflection-type/.../genuine
# 检查输出中有 native-aot.module.h

# Step 6 验证
# 检查 generated.cpp 中包含 RegisterModule 调用

# Family 验证
python verification/.../batch_native_aot_runner.py --family reflection-type

# 全量验证
python verification/.../batch_native_aot_runner.py
```

---

## 风险

| 风险 | 概率 | 影响 | 缓解 |
|------|------|------|------|
| `void*` → `uint64_t` 迁移有遗漏引用 | 中 | 高 | 编译时 0 error 是最低门，codegen-review 额外扫描 |
| 现有 reflection-api.cpp 中多处使用 TLS image 指针，替换遗漏 | 中 | 中 | 逐函数审查，删除所有 `GetCurrentImage()` 调用 |
| emitter 获取 closure 中实际使用的类型列表复杂 | 低 | 中 | reflection-type family 只有 ~30 个类型，先从 manifest.Types 取全量 |
| 跨 DLL 测试环境复杂 | 高 | 低 | 先验证单 DLL 正确性，跨 DLL 作为后续验收项 |
| HotUpdate RegisterModule 并发安全性 | 低 | 低 | AOT 启动是串行的，HotUpdate 加载器再加锁 |

---

## STATUS.md 维护规则

执行时在 `docs/dev/in-progress/reflection-b2-module-registry/STATUS.md` 中记录：
- 当前 step 状态（`not-started` / `in-progress` / `completed`）
- 卡住的 step 与原因
- 验证结果（build 0 error, family pass/fail）
- 发现的新问题或决策变更

`notes/progress-*.md` 仅在阻塞或重要决策变更时创建，不是例行更新点。
