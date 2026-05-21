# B2 Module Registry + Two-Tier Metadata 反射架构决策

## 状态

✅ 已确认（2026-05-04）

## 背景

原有的反射元数据架构（方案 A）使用 `thread_local g_current_aot_image` 指针 + 每个 DLL 生成独立 constexpr `ReflectionQueryTypeDescriptor[]` 全量表，`TypeInfoHandle` 为 `constexpr T*` 指针。在多 DLL + HotUpdate 场景下暴露三个硬伤：

1. **跨 DLL 类型身份失效**：同一类型（如 `System.Object`）在不同 DLL 中生成不同的 `constexpr` 指针，`ptr_equal(a, b)` 为 false，导致 `if (type == typeof(object))` 等运行时类型判断错误
2. **Binary 膨胀**：每个 DLL 都重复生成全量 CoreLib 类型描述符（~30 个 DLL × 200 类型 × ~80 bytes ≈ 480KB+ 重复数据）
3. **HotUpdate 不可行**：constexpr 表不能在运行时创建，HotUpdate DLL 需要重新编译整套元数据

## 方案对比

| 维度 | A: TLS constexpr 指针 | B: CodeGen 按需生成 | **B2: Module Registry + Two-Tier（选定）** | C: Token 高位编码 flags | D: Unity IL2CPP 式全局 blob |
|------|----------------------|--------------------|------------------------------------------|------------------------|-----------------------------|
| 核心思想 | `thread_local` image 指针 + 每 DLL constexpr 全表 | CodeGen 按 closure 生成缩减表 | 全局 registry + `(module_id, token)` handle + 分层 metadata | token 高位直接编码类型属性 | 全局二进制 IL metadata 运行时解析 |
| TypeInfoHandle | `constexpr T*`（指针） | `constexpr T*`（指针） | `(module_id:32, token:32)` uint64_t | `[flags:8][module:8][index:16]` | `Il2CppType*`（全局索引） |
| get_IsValueType 延迟 | ~1 cycle（constexpr 位测试） | ~1 cycle | ~3 cycles（registry 寻址 + 查 flags） | ~1 cycle（token 掩码） | ~3-5 cycles（typeDef→flags 位测试） |
| 跨 DLL 身份 | ❌ 指针不等 | ❌ 指针不等需桥接 | ✅ (module_id, token) 天然自描述 | ✅ token 含 module_id | ✅ 全局表 |
| 30 DLL volume | 480KB+ 重复全表 | 240KB+ 按需 | ~72KB（仅 flags） | ~72KB | 10-50MB 全局 blob |
| HotUpdate | ❌ 不可用 | ⚠️ 需 token remapping | ✅ RegisterModule() 一行 | ⚠️ token 空间需预留 | ⚠️ 需侵入全局 blob |
| **最终判定** | ❌ 不可行 | ⚠️ 遗留跨 DLL bug | ✅ **选定** | ❌ flag 数超出 token 容量 | ⚠️ 实现量过大+性能差 |

## B2 架构要点

### TypeInfoHandle 格式

```cpp
typedef uint64_t TypeInfoHandle;

// 编码: [module_id:32 bits] [token:32 bits]
//   module_id = ModuleRegistry 中的索引（0 = CoreLib fallback）
//   token = ECMA 标准 metadata token（如 0x02000001）

// helper 函数在 module_registry.h 中:
constexpr TypeInfoHandle MakeTypeHandle(uint32_t module_id, uint32_t token) {
    return (static_cast<uint64_t>(module_id) << 32) | token;
}
```

### Module Registry

全局 `ModuleDescriptor[]` 数组，每个 AOT DLL 启动时注册自己：

```cpp
struct ModuleDescriptor {
    const char* name_utf8;           // 模块名称（如 "System.Linq"）
    const uint32_t* type_flags;      // Tier 1: per-type 位标志
    const char* const* type_names;   // Tier 1: per-type 名称
    const char* const* type_namespaces; // Tier 1: per-type 命名空间
    const uint32_t* type_parent_tokens; // Tier 1: per-type 父类型 token
    uint32_t type_count;             // 本模块管理的类型数量
};

uint32_t RegisterModule(const char* name, const ModuleDescriptor* descriptor);
const ModuleDescriptor* LookupModule(uint32_t module_id);
```

### Two-Tier 元数据分层

| 层级 | 内容 | 生成条件 | 每类型开销 |
|------|------|---------|-----------|
| Tier 1 | `type_flags[]`, `type_names[]`, `type_namespaces[]`, `type_parent_tokens[]` | **总是生成** | ~16 bytes |
| Tier 2 | `ReflectionQueryMethodDescriptor[]`, `ReflectionQueryFieldDescriptor[]`, 完整 `ReflectionQueryTypeDescriptor` | **仅当 family 使用 GetMethods/GetFields/Invoke 等冷路径** | ~100-500 bytes |

类型属性位标志（`type_flags`）包含所有 .NET 类型 boolean 属性的位编码：

```cpp
constexpr uint32_t kFlagIsValueType          = 1u << 0;
constexpr uint32_t kFlagIsEnum               = 1u << 1;
constexpr uint32_t kFlagIsAbstract           = 1u << 2;
constexpr uint32_t kFlagIsSealed             = 1u << 3;
constexpr uint32_t kFlagIsInterface          = 1u << 4;
constexpr uint32_t kFlagIsArray              = 1u << 5;
constexpr uint32_t kFlagIsGenericType        = 1u << 6;
constexpr uint32_t kFlagIsGenericTypeDef     = 1u << 7;
constexpr uint32_t kFlagIsConstructedGeneric = 1u << 8;
constexpr uint32_t kFlagIsNested             = 1u << 9;
constexpr uint32_t kFlagIsPublic             = 1u << 10;
// ... 扩展空间充足（uint32_t 支持 32 个 flag）
```

### Token → Index 映射

ECMA metadata token 格式：`0x02TTTTTT` 中，低 24 位为 table index（TypeDef 表从 1 开始）：

```cpp
inline uint32_t TokenToIndex(uint32_t token) {
    uint32_t raw_index = token & 0x00FFFFFFu;
    // TypeDef 表从 1 开始（0 保留），所以 type index = raw_index - 1
    return raw_index > 0 ? raw_index - 1 : 0;
}
```

### 跨 DLL 类型身份

因为 TypeInfoHandle 包含 `module_id`，无论哪个 DLL 构造 handle：

```cpp
// DLL-A: handle = MakeTypeHandle(corelib_id, 0x02000001)  // System.Object
// DLL-B: 同样 decode 为 (corelib_id, 0x02000001)
// → 查到同一个 g_modules[corelib_id] 的同一个 type_flags[] 条目
```

对比方案 A/B 的 constexpr 指针：`&kTypeObject` 在 DLL-A 和 DLL-B 中指向不同地址，但 B2 的 `(module_id, token)` 对始终一致。

### HotUpdate 路径

```cpp
// 1. 热更 DLL 加载，运行时构造 ModuleDescriptor（非 constexpr）
auto* hotfixModule = new ModuleDescriptor{ ... };

// 2. 注册为新模块
uint32_t hotfix_id = RegisterModule("GameLogic_v2", hotfixModule);

// 3. 构造热更类型的 handle
TypeInfoHandle handle = MakeTypeHandle(hotfix_id, 0x02000005);

// 4. 这个 handle 可以在任意 AOT DLL 中传给 chaos_reflection_* 函数
//   →  registry 自动路由到正确的模块
```

## 与旧方案（方案 A）的关键差异

| 维度 | 旧方案（TLS + constexpr 全表） | B2 方案 |
|------|-------------------------------|---------|
| 全局状态 | `thread_local g_current_aot_image` | 全局 `g_modules[64]` 数组 |
| Handle 类型 | `void*`（constexpr 指针） | `uint64_t`（module_id, token） |
| 跨 DLL 身份 | 指针不等，需要额外 token 归一化 | Handle 自描述 |
| Per-DLL 生成 | 全量 `ReflectionQueryTypeDescriptor[]` | 仅 `uint32_t flags[]` + 字符串指针 |
| Cold path 生成 | 总是全量 | 条件生成（YAGNI） |
| HotUpdate | constexpr 运行时不可替代 | 运行时构造 ModuleDescriptor |

## 受影响组件

| 组件 | 改动类型 |
|------|---------|
| `contracts/native/v0/runtime_abi.h` | `TypeInfoHandle` typedef 修改 |
| `src/native/runtime-core/module_registry.h` | **新增** — registry + handle helper |
| `src/native/runtime-core/module_registry.cpp` | **新增** — registry 实现 |
| `src/native/runtime-core/reflection_metadata_fallback.h` | **新增** — CoreLib 7 类型 fallback |
| `src/native/runtime-core/reflection_api.cpp` | 重写为 registry 模式 |
| `src/native/runtime-core/runtime_core.h` | 删除 `SetCurrentAotImage`，保留 `RegisterModule` |
| `src/managed/Chaos.IL2CPP.Generator/NativeAotLoweringPlanner.cs` | 新增 `BuildModuleHeader()` emitter |
| `src/managed/Chaos.IL2CPP.Generator/NativeAotEmitter.cs` | 新增 `ModuleHeader` 输出 |
| `src/managed/Chaos.IL2CPP.Contracts/ManagedClosureArtifactModels.cs` | 新增 `ModuleHeader` artifact name |

## 相关文档

- [反射查询模型](../01-翻译管线/17-反射查询模型.md)
- [全局优先级约束](../03-架构约束/05-全局优先级约束.md)
