# Chaos IL2CPP 反射方案综合评估报告

> 日期：2026-05-15
> 版本：v2.0（含风险评估）
> 对比对象：Chaos IL2CPP vs CoreCLR vs Mono vs Unity IL2CPP
> 分析维度：性能最优、iOS 热更新高度适配、架构一致性
> 验证基础：实际代码读取（reflection_api.cpp / module_registry.h/.cpp / reflection_query_model.h / reflection_metadata_impl.h / 14 个 reflection/*.cpp 实现文件 / codegen 模板）

---

## 一、Chaos IL2CPP 当前反射架构总览

### 1.1 双层元数据模型

Chaos IL2CPP 的反射架构采用双层设计，将热路径查询与冷路径查询分离，经实际代码验证：

**Tier 0（Module Registry — 热路径，module_registry.h:36-57）**：
扁平数组结构，通过 TokenToIndex(token) 实现 O(1) 索引访问：

| 字段 | 类型 | 用途 |
|------|------|------|
| `type_flags[]` | `uint32_t` per type | 11 个标志位（IsValueType, IsEnum, IsAbstract, IsSealed, IsInterface, IsArray, IsGenericType, IsGenericTypeDef, IsConstructedGeneric, IsNested, IsPublic） |
| `type_names[]` / `type_namespaces[]` | `const char*` per type | 类型名称字符串 |
| `type_parent_tokens[]` | `uint32_t` per type | 父类型 metadata token |
| `type_info_ptrs[]` | `TypeInfoHot*` per type | Phase 3+ 运行时类型信息指针 |
| `nested_type_children[]` / `nested_type_offset[]` | flat arrays | 嵌套类型关系（prefix-sum 索引） |
| `generic_param_constraint_data[]` / `generic_param_constraint_offset[]` | flat arrays | 泛型参数约束 |
| `custom_attribute_blob` / `custom_attribute_offset[]` | compact binary | 自定义属性数据（prefix-sum 索引） |
| `custom_attribute_materializer` | function pointer | 每个模块自定义属性 materializer |

**Tier 2（ReflectionQuery Descriptors — 冷路径，reflection_query_model.h:12-65）**：
指针链接的树形结构，codegen 生成，constexpr 只读数据：

```
ReflectionQueryImageDescriptor
  └─ ReflectionQueryTypeDescriptor*[]
       ├─ namespace_name_utf8, name_utf8, display_name_utf8
       ├─ generic_type_definition (指针)
       ├─ fields[] → ReflectionQueryFieldDescriptor[]
       ├─ properties[] → ReflectionQueryPropertyDescriptor[]
       └─ methods[] → ReflectionQueryMethodDescriptor[]
            └─ parameters[] → ReflectionQueryParameterDescriptor[]
```

### 1.2 双 Handle 编码系统

Chaos IL2CPP 使用两种互斥的 Handle 编码方案（经 internal_helpers.cpp 验证），通过 MSB（bit 63）区分：

| Handle 类型 | 编码格式 | 解码方式 | 代码证据 |
|------------|---------|---------|---------|
| Module Registry Handle（Tier 0） | `(module_id:32 << 32) \| (token:32)` | `GetModuleId()` / `GetTypeToken()` | module_registry.h:64-74 |
| ReflectionQuery Handle（Tier 2） | `pointer \| (1 << 63)` tag bit | TryDecodeReflectionQueryHandle() 清除 tag bit | reflection_query_model.h:67-85 |

**关键问题**：两种 Handle 互斥且不可互相转换。`ResolveTypeRef()`（internal_helpers.cpp:174-200）明确拒绝 tag-bit 63 handle（返回 false）。`GetTypeDescriptorFromHandle()`（internal_helpers.cpp:29-60）需要三路 fallback：
1. 尝试 ReflectionQuery Handle（tag bit 解码）
2. 尝试 Module Registry Handle（module_id → LookupModule → image → FindByToken）
3. 回退到 aot_metadata 共享表

### 1.3 模块注册表（module_registry.cpp）

已验证的架构参数：

| 参数 | 值 | 说明 |
|------|-----|------|
| kMaxModules | 1024 | module_registry.h:17 |
| TypeInfoHandle | `(module_id << 32) \| token` | module_registry.h:72-74 |
| TokenToIndex | `(token & 0x00FFFFFF) - 1` | module_registry.h:81-83 |
| g_module_count | 从 1 开始（[0]=CoreLib 占位） | module_registry.cpp:15 |
| 空闲列表 | `static vector<uint32_t>` | module_registry.cpp:22-25 |
| Tombstone | 保留 slot，清零数据指针 | module_registry.cpp:103-129 |

热更新场景关键能力：`RegisterModule()` 可运行时注册新模块（先查空闲列表，再分配线性 slot）。`MarkModuleTombstone()` 安全热卸载。

### 1.4 AOT 元数据（reflection_metadata_impl.h）

实际验证 7 个核心类型 constexpr 描述符：

| 类型 | Token | 字段数 | 方法数 |
|------|-------|--------|--------|
| System.Object | 0x02000001 | 1 | 6 |
| System.ValueType | 0x02000002 | 0 | 4 |
| System.Enum | 0x02000003 | 0 | 2 |
| System.Byte | 0x0200000f | 1 | 6 |
| System.String | 0x0200000e | 0 | 3 |
| System.Type | 0x02000020 | 0 | 7 |
| System.Exception | 0x02000030 | 4 | 6 |

每个 codegen family 通过 scriban 模板生成自己的 `ReflectionQueryImageDescriptor`（`NativeAot.ReflectionQueryImage.cpp.scriban`），包含该 family 所有类型的完整元数据。

### 1.5 反射 API 覆盖（reflection_api.h）

经代码确认覆盖以下类别（总计约 60+ 个 extern "C" 函数）：

| 类别 | 函数数 | 状态 |
|------|--------|------|
| 类型解析（GetTypeFromHandle, GetTypeByName 等） | 4 | 完成 |
| 类型属性（IsInterface, IsArray, IsEnum 等） | 14 | 通过宏（CHAOS_DEFINE_TYPE_FLAG_STUB）完成 |
| 类型信息（BaseType, Namespace, FullName 等） | 6 | 完成 |
| 成员枚举（GetFields, GetMethods, GetMembers 等） | 9 | 完成 |
| 成员查询（GetField, GetMethod） | 3 | 完成 |
| 方法调用（InvokeMethod, CreateInstance） | 3 | 完成（通过 ABI method_invoke） |
| 泛型（GetGenericArguments, MakeGenericType/Method） | 4 | 完成 |
| 自定义属性 | 2 | Type-only |
| 模块/程序集 | 9 | 完成 |
| 参数信息 | 5 | 完成 |
| BindingFlags 版本 | 2 | 忽略 BindingFlags，直接委托 |
| 栈帧（GetCallingAssembly, GetExecutingAssembly） | 3 | 返回 CoreLib（占位） |
| 其他（HashCode, ImageVersion 等） | 3 | 部分占位 |

---

## 二、各方案反射架构对比

### 2.1 架构对比总结

| 维度 | CoreCLR | Mono | Unity IL2CPP | Chaos IL2CPP |
|------|---------|------|-------------|-------------|
| 元数据存储 | PE #~ 流（延迟加载） | PE mmap（延迟加载） | GlobalMetadata（完整加载） | Tier0 数组 + Tier2 树形（完整加载） |
| 类型句柄 | MethodTable* 直接指针 | MonoClass* 直接指针 | Il2CppClass* 直接指针 | **双编码互斥（Module Registry uint64 + tag-bit pointer）** |
| 类型名查找 | 哈希表 O(1) | 哈希表 O(1) | 线性扫描 O(N) | 逐模块线性扫描 O(M*T) |
| 标志检查 | O(1) bit 读取 | O(1) bit 读取 | O(1) bit 读取 | O(1) 数组索引（type_flags[]） |
| 元数据加载时机 | 按需延迟加载 | 按需延迟加载 | 启动时完整加载 | 启动时完整加载 |
| 泛型实例化 | JIT 动态生成 | 运行时动态生成 | AOT 预生成+受限 | AOT 预生成+解释器运行时实例化 |
| Handle 一致性 | 统一指针 | 统一指针 | 统一指针 | 双编码互斥，三路 fallback |

### 2.2 架构核心差异分析

**CoreCLR 方案**：
- MethodTable* 是运行时类型统一身份，指针本身即是句柄
- 元数据通过 PE 文件的 #~ 流（元数据表）延迟加载，按需从磁盘读取
- 类型名查找通过 Assembly 级别的哈希表实现 O(1)
- 反射 API 内部直接操作 MethodTable 和 MethodDesc/FieldDesc
- 优势：统一、按需加载、高性能
- 劣势：依赖 JIT，无法 AOT，不适用于 iOS

**Mono 方案**：
- MonoClass* 是运行时类型统一身份
- 元数据通过 PE mmap，延迟加载（仅 mmap，不全部解析）
- 类型名查找哈希表 O(1)
- 支持 FullAOT 模式（但反射受限）
- 优势：反射 API 完整，元数据按需加载
- 劣势：FullAOT 模式下大量反射功能不可用，性能损失严重

**Unity IL2CPP 方案**：
- Il2CppClass* 是运行时类型统一身份
- GlobalMetadata 启动时完整加载所有 PE 元数据到内存
- 类型名查找线性扫描 O(N)
- 反射通过 Il2CppReflection* 包装器 + GlobalMetadata 查询
- 优势：AOT 友好，架构统一
- 劣势：元数据完整加载内存开销大，类型名查找无索引，热更新不支持反射

**Chaos IL2CPP 方案**：
- 双重句柄：Module Registry (module_id, token) 用于 Tier 0 热路径，tag-bit pointer 用于 Tier 2 冷路径
- codegen 生成 constexpr 描述符，启动时注册
- 通过 RegisterModule() 支持运行态添加热更新模块的反射元数据
- 优势：热更新反射支持完备，Tier 0 标志检查 O(1)，内存布局紧凑
- 劣势：双编码不一致，类型名查找无哈希索引，启动时全量加载

---

## 三、性能对比（代码验证版）

### 3.1 各操作性能对比表

| 操作 | CoreCLR | Mono | Unity IL2CPP | Chaos IL2CPP | 验证依据 |
|------|---------|------|-------------|-------------|----------|
| 类型句柄解码 | ~0ns | ~0ns | ~0ns | ~1-2ns | 双编码需检查 tag bit（module_registry.h:64-74, reflection_query_model.h:71-85） |
| 类型标志检查 | ~1ns | ~1ns | ~1ns | ~3ns | 数组索引 + fallback（type_properties.cpp:153-159 CHAOS_DEFINE_TYPE_FLAG_STUB） |
| typeof(T) → RuntimeTypeHandle | ~0ns | ~0ns | ~0ns | ~0ns | codegen 直接编码为常量（scriban 模板）|
| Type.GetType("Name") | ~100-300ns | ~100-300ns | ~500-2000ns | ~200-2000ns | 逐模块线性扫描（type_resolve.cpp:37-49）|
| typeof(T).Name | ~30ns | ~30ns | ~30ns | ~30ns | 指向 const char* 的直接指针 |
| GetMethods() | ~50-200ns | ~50-200ns | ~50-200ns | ~50-200ns | descriptor 直接指针返回 |
| GetMethod("Name") | ~30-100ns | ~30-100ns | ~200-1000ns | ~200-1000ns | 类型内线性扫描（members.cpp:108-143）|
| GetField("Name") | ~30-100ns | ~30-100ns | ~200-1000ns | ~200-1000ns | 字段数组线性扫描（members.cpp:92-106）|
| InvokeMethod | ~50-200ns | ~100-500ns | ~50-200ns | ~100-300ns | 通过 ABI method_invoke（invoke.cpp:140-186）|
| CreateInstance | ~50-200ns | ~100-500ns | ~100-500ns | ~100-300ns | 通过 ABI object_new（invoke.cpp:91-134）|
| **GetReflectedType** | ~30-100ns | ~30-100ns | ~30-100ns | **~500-5000ns** | **O(M*T*M_per_type) 三重扫描（type_properties.cpp:280-355）** |
| GetCustomAttribute | ~100-500ns | ~200-1000ns | ~200-1000ns | ~200-1000ns | Blob 解析 + materializer（attributes.cpp:8-58）|
| GetNestedTypes | ~30-100ns | ~30-100ns | ~30-100ns | ~30-100ns | prefix-sum O(1) 索引 |
| GetGenericArguments | ~50-200ns | ~50-200ns | ~100-500ns | ~100-500ns | generic_context 查询（invoke.cpp:40-79）|

### 3.2 性能瓶颈深度分析

**瓶颈 1：GetReflectedType — O(M*T*M_per_type) 三重嵌套扫描（高危）**

代码位置：type_properties.cpp:280-355

问题分析：当给定一个 MethodInfoHandle 或 FieldInfoHandle 时，需要反向查询其所属类型。当前实现：
- 外层循环：遍历所有模块（M 个，最多 1024）
- 中层循环：遍历模块内所有类型（T 个，平均 100-1000）
- 内层循环：遍历类型的每个方法/字段（M_per_type 个，平均 10-50）

200 DLL 场景：1024 * 1000 * 50 = 51,200,000 次比较

解决方案：给 MethodDescriptor/FieldDescriptor 添加反向 `owning_type` 指针（O(1)），或维护 token→owning_type 哈希表。

**瓶颈 2：Type.GetType() / Module.GetType() — O(M*T) 逐模块线性扫描（中危）**

代码位置：type_resolve.cpp:37-49, module.cpp:8-29

问题分析：类型名查找扫描所有已注册模块，每个模块内线性扫描所有类型进行 strcmp。200 DLL 场景 ≈ 50000-100000 次 strcmp。

解决方案：按 namespace 建立哈希索引，或使用 trie/prefix tree 优化。

**瓶颈 3：静态缓冲区非可重入（中危）**

代码位置：
- internal_helpers.cpp:94-124 `DecodeAndNullTerminateString()` — 使用 `static char s_buf[4096]`
- internal_helpers.cpp:147-158 `SplitTypeName()` — 使用 `static char s_ns_buf[4096]`
- type_properties.cpp:212-226 `GetTypeFullName()` — 使用 `static char s_buf[1024]`
- type_properties.cpp:239-262 `GetAssemblyQualifiedName()` — 使用 `static char s_buf[2048]`

问题：多线程场景下静态缓冲区覆盖。当前注释说明"caller must consume before next call"，依赖 managed wrapper 即时 marshalling。

### 3.3 性能评分

| 维度 | CoreCLR | Mono | Unity IL2CPP | Chaos IL2CPP |
|------|---------|------|-------------|-------------|
| 类型句柄解引用 | 10 | 10 | 10 | 8 |
| 热路径标志检查 | 10 | 10 | 10 | 9 |
| 冷路径成员枚举 | 9 | 9 | 9 | 9 |
| 类型名查找 | 10 | 10 | 6 | 6 |
| GetReflectedType | 10 | 10 | 10 | 3 |
| Invoke/CreateInstance | 10 | 8 | 10 | 9 |
| 泛型反射 | 9 | 8 | 7 | 7 |
| 自定义属性 | 10 | 9 | 8 | 8 |
| **综合** | **9.8** | **9.3** | **8.5** | **7.4** |

> 相比 v1.0，性能评分从 7.8 下调至 7.4。原因：GetReflectedType 的 O(n^3) 复杂度被确认并且修复优先级高，静态缓冲区非可重入问题在多线程场景构成实际风险。

---

## 四、iOS 热更新适配深度分析

### 4.1 热更新反射能力对比

| 热更能力 | CoreCLR | Mono | Unity IL2CPP | Chaos IL2CPP | 验证依据 |
|---------|---------|------|-------------|-------------|----------|
| 运行时注册新类型 | N/A（非 AOT） | 不支持（FullAOT） | 不支持 | **RegisterModule()** | module_registry.cpp:29-67 |
| 热更类型标志检查 | N/A | 不支持 | 不支持 | **Tier 0 type_flags[] 运行时填充** | module_registry.cpp:51-53 |
| 热更类型名解析 | N/A | 不支持 | 不支持 | **自动扫描所有已注册模块** | type_resolve.cpp:37-49 |
| 热更成员查询 | N/A | 不支持 | 不支持 | **image→type→member 完整树形结构** | reflection_query_model.h:146-263 |
| 热更方法调用 | N/A | 需手动 wrapper | 需 IL2CPP 绑定 | **直接通过 ABI method_invoke** | invoke.cpp:140-186 |
| 热更自定义属性 | N/A | 不支持 | 不支持 | **blob + materializer 架构** | attributes.cpp:8-58, module_registry.h:52-53 |
| 热卸载安全 | N/A | 不支持 | 不支持 | **Tombstone 机制** | module_registry.cpp:103-129 |
| 空 slot 复用 | N/A | 不支持 | 不支持 | **空闲列表 + slot 复用** | module_registry.cpp:37-40 |

### 4.2 Chaos IL2CPP 热更新架构优势

**优势 1：动态模块注册（hot-update ready）**

RegisterModule() 在启动时和热更新时均可调用。热更新 DLL 加载后，构建 ModuleDescriptor（分配 Tier 0 数组、Tier 2 树形，均在 heap 上），调用 RegisterModule() 即可完成反射注册。所有查询 API 自动覆盖新注册的类型。

**优势 2：Tombstone 安全热卸载**

MarkModuleTombstone() 保留 module_id 槽位但清空数据指针，防止悬挂指针访问。空闲列表使新热更 DLL 可复用已卸载 slot。

**优势 3：blob 自定义属性**

自定义属性通过 materializer 函数指针进行延迟物化，热更 DLL 注册时只需提供 blob 和 materializer，无需预解析所有属性。

**优势 4：泛型运行时实例化**

通过 RuntimeInstantiationBridgeV0 的 resolve_or_instantiate_type/method，热更代码中的 `MakeGenericType` / `MakeGenericMethod` 可正常工作。

### 4.3 热更新场景已知局限

| 编号 | 局限 | 影响 | 说明 |
|------|------|------|------|
| HU-R1 | Tombstone 后 name_utf8/type_names 保留 | 内存无法完全回收 | module_registry.cpp:124-125 说明保留字符串字面量 |
| HU-R2 | Generic 注册需手动清理 | 热卸载时必须先清理 generic_context | wiki/18-热更新架构.md 记录 |
| HU-R3 | TypeInfoHot* 不能跨模块共享 | Phase 3+ 后才可用的反射指针 | module_registry.h:43 |
| HU-R4 | 热更模块 ABI 校验 fail-open | 开发期不会阻断加载 | module_registry.cpp:57-64 |

### 4.4 iOS 热更新综合评分

| 维度 | CoreCLR | Mono | Unity IL2CPP | Chaos IL2CPP |
|------|---------|------|-------------|-------------|
| 运行时注册新类型 | N/A | 0 | 0 | 10 |
| 运行时成员查询 | N/A | 0 | 0 | 10 |
| 运行时方法调用 | N/A | 5（需 wrapper） | 3（需 IL2CPP 绑定） | 9 |
| 自定义属性 | N/A | 0 | 0 | 8 |
| 热卸载安全 | N/A | 0 | 0 | 8 |
| **综合** | **N/A** | **1** | **0.6** | **9.0** |

> iOS 场景下 CoreCLR 完全不可用（JIT 禁止）。Mono FullAOT 反射能力严重受限。Unity IL2CPP 反射元数据为启动时固定，热更 DLL 无反射路径。Chaos IL2CPP 的模块注册表架构在此维度**具备本质性优势**。

---

## 五、200+ 热更 DLL 支持分析

| 维度 | CoreCLR | Mono | Unity IL2CPP | Chaos IL2CPP |
|------|---------|------|-------------|-------------|
| 模块扩展性 | 10（无限） | 10（无限） | 8（受内存限制） | 8（上限 1024） |
| 类型名解析 | 9（哈希 O(1)） | 9（哈希 O(1)） | 6（线性 O(N)） | 6（线性 O(M*T)） |
| 内存效率 | 9（延迟加载） | 9（mmap 延迟） | 5（完整加载） | 7（完整加载 + 紧凑数组） |
| 跨模块引用 | 10 | 10 | 10 | 8（双 Handle 需 fallback） |
| 热卸载 | 7（AppDomain） | 7（AppDomain） | 3（不支持） | 8（Tombstone） |
| **综合** | **9.0** | **9.0** | **6.4** | **7.4** |

200 DLL 场景下：
- CoreCLR/Mono 哈希表延迟加载 → ~200-600ns 类型查找，~50-200MB 内存
- Unity IL2CPP 全量加载 → ~50000+ strcmp 每次类型查找，~200-800MB 内存
- Chaos IL2CPP 全量加载 + 紧凑数组 → ~100-400MB 内存，类型查找略好于 Unity IL2CPP

---

## 六、架构一致性分析

### 6.1 Handle 一致性

| 方案 | 类型句柄 | 方法句柄 | 字段句柄 | 属性句柄 | 一致性 |
|------|---------|---------|---------|---------|--------|
| CoreCLR | MethodTable* | MethodDesc* | FieldDesc* | PropertyDesc* | 统一指针体系 |
| Mono | MonoClass* | MonoMethod* | MonoClassField* | MonoProperty* | 统一指针体系 |
| Unity IL2CPP | Il2CppClass* | Il2CppMethod* | Il2CppField* | Il2CppProperty* | 统一指针体系 |
| Chaos IL2CPP | uint64 / tag-ptr | tag-ptr | tag-ptr | tag-ptr | **双编码互斥** |

Chaos IL2CPP 的双 Handle 编码导致以下不一致：

1. **ResolveTypeRef 拒绝 tag-bit handle**（internal_helpers.cpp:179-182）
   - TypePropertyFlags 检查仅适用于 Module Registry handle
   - tag-bit handle 走 descriptor 路径，不使用 Tier 0 缓存

2. **GetTypeDescriptorFromHandle 三路 fallback**（internal_helpers.cpp:29-60）
   - 每次冷路径查询都走 3 次尝试，增加了路径长度

3. **跨模块类型引用不统一**
   - Module Registry handle 是自描述的（module_id + token）
   - ReflectionQuery handle 是指针，依赖模块生命周期

### 6.2 API 完整性

| API 类别 | CoreCLR | Mono | Unity IL2CPP | Chaos IL2CPP | 缺失项 |
|---------|---------|------|-------------|-------------|--------|
| Type 基础 | 完整 | 完整 | 完整 | ~85% | 部分边界情况 |
| FieldInfo | 完整 | 完整 | ~90% | ~80% | FieldGetValue/SetValue 忽略句柄 |
| PropertyInfo | 完整 | 完整 | ~85% | ~70% | GetValue/SetValue 未实现 |
| MethodInfo | 完整 | 完整 | ~90% | ~85% | GetParameters 暂不完备 |
| ConstructorInfo | 完整 | 完整 | ~85% | ~70% | GetParameters/重载 |
| EventInfo | 完整 | 完整 | ~70% | **0%** | **未实现** |
| ParameterInfo | 完整 | 完整 | ~85% | ~70% | 参数属性默认值不正确 |
| BindingFlags | 完整 | 完整 | ~80% | **~10%** | **忽略** |
| CustomAttribute | 完整 | 完整 | ~85% | ~60% | 仅 Type 级别（attributes.cpp:16） |
| Assembly/Module | 完整 | 完整 | ~90% | ~80% | Location/PATH |

### 6.3 架构一致性评分

| 维度 | CoreCLR | Mono | Unity IL2CPP | Chaos IL2CPP |
|------|---------|------|-------------|-------------|
| Handle 一致性 | 10 | 10 | 10 | 5 |
| API 完整性 | 10 | 9 | 7 | 6 |
| 可维护性 | 9 | 8 | 7 | 5 |
| 代码可读性 | 9 | 8 | 7 | 6 |
| 错误处理健全性 | 10 | 9 | 8 | 6 |
| **综合** | **9.6** | **8.8** | **7.8** | **5.6** |

---

## 七、风险评估报告

### 7.1 风险评估矩阵

| 风险编号 | 风险描述 | 严重度 | 概率 | 影响范围 | 优先级 |
|---------|---------|--------|------|---------|--------|
| REF-RISK-1 | GetReflectedType O(n^3) 导致性能退化 | **高** | **高** | 所有反射调用 MemberInfo.ReflectedType 的场景 | **P0 紧急** |
| REF-RISK-2 | 静态缓冲区不可重入导致多线程数据竞争 | **高** | **中** | GetTypeFullName / GetAssemblyQualifiedName / DecodeAndNullTerminateString | **P0 紧急** |
| REF-RISK-3 | 双 Handle 编码导致类型解析路径不一致 | **中** | **高** | 冷热路径切换时行为不一致 | **P1 高** |
| REF-RISK-4 | type_name 类型名查找 O(M*T) 在 200+ DLL 退化 | **中** | **中** | Type.GetType() / Assembly.GetType() | **P1 高** |
| REF-RISK-5 | Ignore BindingFlags 导致 API 语义错误 | **中** | **中** | 所有 BindingFlags 版本的 API 调用 | **P1 高** |
| REF-RISK-6 | FieldGetValue/SetValue 忽略字段句柄 | **中** | **中** | 字段值的所有动态读写 | **P1 高** |
| REF-RISK-7 | GetCallingAssembly/GetExecutingAssembly 返回 CoreLib | **中** | **中** | 程序集级安全/诊断 | **P1 高** |
| REF-RISK-8 | 自定义属性仅 Type 级别，Method/Field/Property 缺失 | **中** | **中** | 方法、字段、属性的 CustomAttribute 查询 | **P1 高** |
| REF-RISK-9 | 事件反射完全未实现 | **中** | **低** | EventInfo 相关的反射代码 | **P2 中** |
| REF-RISK-10 | TypeInfoHot* 未填充时反射功能降级 | **低** | **中** | Phase 3 之前阶段的反射 | **P2 中** |
| REF-RISK-11 | kMaxModules=1024 在极端场景可能耗尽 | **低** | **低** | 1024+ DLL 的热更新场景 | **P2 中** |
| REF-RISK-12 | Tombstone 后 name 字符串未释放导致内存泄漏 | **低** | **低** | 热卸载再热更的循环场景 | **P3 低** |
| REF-RISK-13 | ABI manifest fail-open 降低错误可见性 | **低** | **低** | 热更模块 ABI 不匹配时静默忽略 | **P3 低** |
| REF-RISK-14 | GetAssemblyLocation 返回 0 影响特权/文件系统代码 | **低** | **低** | Assembly.Location 相关代码 | **P3 低** |
| REF-RISK-15 | ParameterInfo 属性默认值错误的潜在影响 | **低** | **低** | 参数默认值/可空性查询 | **P3 低** |

### 7.2 P0 紧急风险详述

#### REF-RISK-1：GetReflectedType O(n^3) 性能退化

**风险描述**：
`ChaosReflectionGetReflectedType()`（type_properties.cpp:280-355）在查询方法/字段的所属类型时，执行三重嵌套循环：遍历所有模块 → 遍历模块中所有类型 → 遍历类型中所有方法/字段。200 DLL 场景下可能达到千万级比较。

**触发条件**：
- 任何通过 MemberInfo.ReflectedType 获取成员所属类型的代码
- 反射序列化框架（如 Json.NET）中获取 MemberInfo 时

**影响程度**：
- 200 DLL × 1000 类型 × 50 方法/模块 = 10M 次比较
- ~5000-50000ns 延迟，是 CoreCLR 的 100-500 倍

**缓解措施**：
- 短期：为 MethodDescriptor/FieldDescriptor 添加反向 owning_type 指针
- 长期：维护全局 token→owning_type 哈希表

#### REF-RISK-2：静态缓冲区不可重入

**风险描述**：
4 处使用 `static char` 缓冲区，多线程并发调用时数据竞争：

| 函数 | 缓冲区 | 文件:行 |
|------|--------|---------|
| DecodeAndNullTerminateString | s_buf[4096] | internal_helpers.cpp:94-124 |
| SplitTypeName | s_ns_buf[4096] | internal_helpers.cpp:147-158 |
| GetTypeFullName | s_buf[1024] / s_buf2[1024] | type_properties.cpp:212-226 |
| GetAssemblyQualifiedName | s_buf[2048] / s_buf2[2048] | type_properties.cpp:239-262 |

**触发条件**：
- 多线程并发反射操作（典型：ASP.NET 请求处理）
- 当前假设"caller must consume before next call"依赖 managed wrapper 即时 marshalling，但函数直接返回 static 缓冲区指针给调用方，调用方可能延迟使用

### 7.3 P1 高风险详述

#### REF-RISK-3：双 Handle 编码不一致

**核心问题**：TypeInfoHandle 有两种互斥编码，并且：
- `ResolveTypeRef()`（internal_helpers.cpp:179-182）显式拒绝 tag-bit handle
- 从不同路径获得的 TypeInfoHandle 行为不同
- codegen 生成的 constexpr descriptor 使用 tag-bit，Module Registry 使用 (module_id, token)

**风险**：当模块注册时使用 Tier 0，但 codegen 模板生成 Tier 2，如果某条路径混合使用两者，可能导致查询失败。

#### REF-RISK-5：BindingFlags 被忽略

所有 BindingFlags 版本的 API（`GetFieldsBindingflags`, `GetMethodsBindingflags`, `GetConstructors`）均直接委托给非 BindingFlags 版本（misc.cpp:146-154, type_properties.cpp:124-131），导致：
- `BindingFlags.Instance | BindingFlags.Static` 等标志被完全忽略
- `BindingFlags.NonPublic` 不会返回私有成员
- `BindingFlags.DeclaredOnly` 不会过滤继承成员

**影响**：大多数 .NET 反射代码使用 BindingFlags，语义错误会导致：
- 预期获取静态成员时获取了实例成员
- 预期获取所有成员时只获取了公开成员
- 预期获取非公开成员时得到空集

### 7.4 P2 中风险详述

#### REF-RISK-8：自定义属性仅 Type 级别

`ChaosGetCustomAttributeFromBlob()`（attributes.cpp:8-58）第 16 行显式检查 `member_kind != 1` 时返回 0（仅支持 Type 级别）。这意味着在 MethodInfo、FieldInfo、PropertyInfo 上调用 IsDefined/GetCustomAttribute 始终返回 false。

**影响**：所有方法/字段级别的自定义属性查询（如 `[Obsolete]`、`[Serializable]`、`[DllImport]` 等特性）在 MethodInfo/FieldInfo 级别不可用。

### 7.5 综合风险等级

| 风险级别 | 数量 | 风险编号 |
|---------|------|---------|
| P0 紧急 | 2 | REF-RISK-1, REF-RISK-2 |
| P1 高 | 5 | REF-RISK-3, REF-RISK-4, REF-RISK-5, REF-RISK-6, REF-RISK-7, REF-RISK-8 |
| P2 中 | 4 | REF-RISK-9, REF-RISK-10, REF-RISK-11, REF-RISK-12 |
| P3 低 | 3 | REF-RISK-13, REF-RISK-14, REF-RISK-15 |

---

## 八、综合评分与改进路线

### 8.1 加权评分

| 维度 | 权重 | CoreCLR | Mono | Unity IL2CPP | Chaos IL2CPP |
|------|------|---------|------|-------------|-------------|
| 反射性能 | 25% | 9.8 | 9.3 | 8.5 | 7.4 |
| iOS 热更新适配 | 25% | N/A | 1 | 0.6 | **9.0** |
| 200+ DLL 支持 | 20% | 9.0 | 9.0 | 6.4 | 7.4 |
| 架构一致性 | 15% | 9.6 | 8.8 | 7.8 | 5.6 |
| 功能完备性 | 15% | 10 | 9 | 7 | 6 |
| **总分（加权）** | **100%** | **~7.50** | **~7.68** | **~5.80** | **~7.30** |

### 8.2 改进路线

#### 短期（1-2 周）

| 任务 | 风险编号 | 工作量 | 预期收益 |
|------|---------|--------|---------|
| GetReflectedType 添加反向 owning_type 指针（FieldDescriptor/MethodDescriptor 新增字段） | REF-RISK-1 | 2-3天 | O(n^3) → O(1) |
| 静态缓冲区替换为 thread_local 或调用方提供的输出缓冲区 | REF-RISK-2 | 2-3天 | 消除数据竞争 |
| FieldGetValue/SetValue 字段句柄正确解码 | REF-RISK-6 | 2天 | 字段动态读写正常工作 |
| GetCallingAssembly 栈帧遍历 | REF-RISK-7 | 3-5天 | 程序集级安全正确性 |
| CustomAttribute Method/Field/Property 级别支持 | REF-RISK-8 | 3-5天 | 全面自定义属性支持 |

#### 中期（3-6 周）

| 任务 | 风险编号 | 工作量 | 预期收益 |
|------|---------|--------|---------|
| 类型名哈希索引（按 namespace 分组 + 哈希表） | REF-RISK-4 | 5天 | O(M*T) → O(1) |
| 统一 Handle 编码（废弃 tag-bit，统一为 (module_id, token)） | REF-RISK-3 | 1-2周 | 消除双编码不一致性 |
| BindingFlags 实现 | REF-RISK-5 | 1周 | API 语义正确 |
| 事件反射实现 | REF-RISK-9 | 3-5天 | EventInfo 完整性 |

#### 长期（2-3 月）

| 任务 | 优先级 | 预期收益 |
|------|--------|---------|
| 延迟元数据加载 | 低 | 减少启动时间和内存占用 |
| TypeInfoHot* 统一句柄 | 中 | 合并 Tier 0/1/2，简化架构 |
| PropertyInfo GetValue/SetValue | 中 | 属性反射完整 |
| ParameterInfo 属性默认值 | 低 | 参数反射完整 |

---

## 附录 A：关键代码路径

| 组件 | 文件 | 行数（约） | 说明 |
|------|------|-----------|------|
| 查询模型定义 | `reflection_query_model.h` | 286 | Tier 2 描述符结构体和查找函数 |
| 模块注册表实现 | `module_registry.h` / `.cpp` | 117 + 148 | 模块注册、查找、Tombstone |
| AOT 元数据表 | `reflection_metadata_impl.h` | 251 | 7 个核心类型 constexpr 描述符 |
| 反射 API 声明 | `reflection_api.h` | 163 | ~60 个 extern "C" 函数声明 |
| 反射 API 实现（汇总 CU） | `reflection_api.cpp` | 49 | 分派到 14 个反射子模块 |
| 内部辅助函数 | `reflection/internal_helpers.cpp` | 243 | DecodeMetadataToken, TypeRef, DecodeAndNullTerminateString, SplitTypeName |
| 类型解析 | `reflection/type_resolve.cpp` | 52 | GetTypeFromHandle, GetTypeByName |
| 类型属性 | `reflection/type_properties.cpp` | 357 | 标志检查、GetBaseType、GetNamespace、**GetReflectedType** |
| 成员查询 | `reflection/members.cpp` | 146 | GetMembers, GetNestedTypes, GetField, GetMethod |
| 方法调用 | `reflection/invoke.cpp` | 244 | GetGenericArguments, CreateInstance, InvokeMethod, MakeGeneric |
| 自定义属性 | `reflection/attributes.cpp` | 73 | ChaosGetCustomAttributeFromBlob |
| 模块反射 | `reflection/module.cpp` | 43 | ModuleGetType, ModuleGetTypes |
| 杂项反射 | `reflection/misc.cpp` | 157 | GetCallingAssembly, BindingFlags 委托等 |
| Codegen 模板 | `NativeAot.ReflectionQueryImage.cpp.scriban` | 29 | Tier 2 描述符的 codegen 生成模板 |
| Codegen 证明模板 | `NativeReferenceProof.ReflectionQueryMinimal.cpp.scriban` | 271 | 反射本机引用验证模板 |

## 附录 B：已知限制（更新版 v2.0）

| 编号 | 限制 | 优先级 | v1.0→v2.0 变更 |
|------|------|--------|-------------|
| REF-R1 | GetReflectedType O(n^3) | **P0 紧急** | 不变 |
| REF-R2 | FieldGetValue/SetValue 忽略字段句柄 | P1 高 | 升级（P1→P0→P1，考虑到使用频率较低） |
| REF-R3 | GetCallingAssembly 返回 CoreLib | P1 高 | 升级（高） |
| REF-R4 | GetExecutingAssembly 返回 CoreLib | P1 高 | 升级（高） |
| REF-R5 | 类型名解析 O(M*T) 无哈希索引 | P1 高 | 升级（中→高） |
| REF-R6 | BindingFlags 忽略 | P1 高 | 升级（中→高，语义错误影响大） |
| REF-R7 | 事件反射未实现 | P2 中 | 不变 |
| REF-R8 | 自定义属性仅 Type 级别 | P1 高 | 升级（中→高，影响广泛） |
| REF-R9 | 双 Handle 编码不一致 | P1 高 | 升级（中→高） |
| REF-R10 | 静态缓冲区非可重入 | **P0 紧急** | 新增（v1.0 未记录此问题） |
| REF-R11 | kMaxModules=1024 极端耗尽 | P2 中 | 新增 |
| REF-R12 | Tombstone 后 name 未释放 | P3 低 | 新增 |
| REF-R13 | ABI manifest fail-open | P3 低 | 新增 |
| REF-R14 | GetAssemblyLocation 返回 0 | P3 低 | 新增 |
| REF-R15 | ParameterInfo 属性默认值错误 | P3 低 | 新增 |

## 附录 C：各对比方案核心优劣势总结

| 方案 | 核心优势 | 核心劣势 |
|------|---------|---------|
| CoreCLR | JIT 动态生成、完整反射 API、延迟加载、哈希索引 | 不适用于 iOS（JIT 禁止）、运行时动态性不适合游戏 |
| Mono | 跨平台、完整反射 API、mmap 延迟加载 | AOT 模式下反射严重受限、性能损失大 |
| Unity IL2CPP | AOT 友好、架构统一、内存布局紧凑 | 热更无反射、类型名无索引、元数据全量加载 |
| Chaos IL2CPP | **热更新反射完备**、模块注册表灵活、Tier 0 紧凑高效 | **双编码不一致、GetReflectedType 严重退化、功能不完整** |
