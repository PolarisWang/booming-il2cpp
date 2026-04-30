# Planner 设计模式

## 自定义属性物化模型

### 架构

Planner 编译期扫描所有可达方法指令中是否存在自定义属性查询调用和 `MemberInfo.IsDefined` 调用。如果不存在，返回空数据（零开销）。如果存在，遍历闭包程序集，通过 PEReader 打开 PE 元数据，读取 CustomAttribute handle collection，解析属性类型标识，解码固定参数和命名参数为 `CustomAttributeFieldAssignment` 记录。

### 关键类型

| 类型 | 说明 |
|------|------|
| `CustomAttributeSupportModel` | 按显示名称索引的属性类型查询、Materializations 列表 |
| `CustomAttributeMaterializationPlan` | 目标(类型/方法)→属性类型→按名称赋文字值的字段 |
| `CustomAttributeLiteralKind` | Null/Boolean/Byte/Int16/Int32/Int64/.../String |
| `DllImportAttribute` 特殊路径 | 从 PE 模块/入口点/flag 映射 |

### 关键函数

| 函数 | 说明 |
|------|------|
| `BuildCustomAttributeSupportModel()` | 编译期物化入口 |
| `CollectCustomAttributeMaterializations()` | 遍历 PE 自定义属性 handle collection |
| `CreateCustomAttributeMaterializationPlan()` | 解码固定参数和命名参数 |
| `CreateDllImportAttributeMaterializationPlan()` | DllImport 专用映射 |

### 与运行时关系

Planner 的物化结果送入 `NativeAotLoweringPlanner.ReflectionObjectEmission.cs` 发射 C++ 对象初始化代码，使得运行时可以在无 CLR 元数据引擎的情况下进行属性查询。

---

## 反射元数据物化模型

### AssemblyReflectionSupportModel

Planner 扫描可达方法指令中 5 个特定 callee SubjectId（`Assembly.GetType`、`Type.GetType`、`Assembly.GetName` 等）。如果命中，遍历闭包程序集，收集 `AssemblyReflectionTypeEntry` 记录（非泛型或开放定义的 SubjectId 匹配）。

### ReflectionMemberSupportModel

扫描 10 个 callee Id（`Type.GetMethod`、`Type.GetField`、`Type.GetConstructors`、`MethodBase.GetParameters` 等）。当激活时，遍历闭包程序集，填充三个有序列表：
- `ReflectionMemberTypeEntry` — 含泛型定义、泛型参数 SubjectId、MetadataToken
- `ReflectionMemberFieldEntry` — 声明类型、字段名、token
- `ReflectionMemberMethodEntry` — MethodSubjectId、声明类型、方法名、参数名、构造器标志、token

### 与运行时的关系

Planner 输出 → `NativeAotTemplateModel` → 发射为 C++ 只读数据区（`ReflectionQueryImageDescriptor`）。运行时通过 `reflection_query_model.h` 的标记指针直接读取。

---

## RVA 静态字段数据模型

### 架构

当 IL 中存在 `ldtoken` 指令且目标引用类型为 `Field` 时，Planner 收集字段 SubjectId，检查 `MetadataRegistrationArtifact` 中 `RegistrationKind == "field"` 且 `MemberType` 非空的条目。调用 `TryParseStaticFieldDataSize` 扫描 member type 字符串中的 `__StaticArrayInitTypeSize=` 标记，提取整数值。然后调用 `TryLoadStaticFieldDataBytes` 通过 PE section data 读取原始字节。

### 关键类型

| 类型 | 说明 |
|------|------|
| `StaticFieldDataSupportModel` | StaticFieldDataEntry 列表 |
| `StaticFieldDataEntry` | SubjectId + MemberType + 原始字节 |

### 发射管道

`EmitSpanRuntimePrelude` 迭代 `EntriesBySubjectId`，将字节格式化为 16 进制行，通过 Scriban 模板 `SpanRuntimePrelude` 渲染为 C++ 静态字节数组。这是 `RuntimeHelpers.InitializeArray` 和 `CreateSpan<T>` 的初始化数据嵌入方式。

---

## 泛型静态方法候选者识别

Planner 在 `Create()` 阶段过滤 `_methodsBySubjectId.Values`，使用复合谓词：

```
IsStatic && ParameterCount == 1 
&& ReturnAbi == NativeInt && ParameterAbis[0] == NativeInt 
&& SubjectId.Contains("!!0")
```

匹配模式：将运行时字段句柄（`RuntimeFieldHandle`，表示为 `NativeInt`）和长度转换为 `Span<T>` 的泛型静态方法（如 `RuntimeHelpers::CreateSpan<T>`）。结果按 SubjectId 排序，在 `ReflectionObjectEmission.cs` 中发射为运行时特化存根，基于运行时类型句柄进行分发。

---

## 委托组合/移除运行时支持

当 `Delegate.Combine` 或 `Delegate.Remove` 可达时，`EmitDelegateRuntimeSupportDefinitions` 发射完整的 C++ 委托运行时支持：

| 函数 | 用途 |
|------|------|
| `chaos_require_delegate(ptr)` | 空检查 + reinterpret_cast |
| `chaos_try_get_delegate_invocation_list(delegate)` | 读 invocation list 指针 |
| `chaos_delegate_single_entry_equals(a, b)` | 比较 type_id + target + method_ptr |
| `chaos_delegate_append_flattened_entries(dst, src)` | 递归展开多播列表 |
| `chaos_delegate_validate_entry_types(entries, type_id)` | 确保所有条目 type_id 一致 |
| `chaos_delegate_allocate_with_type_id(type_id)` | 按具体 delegate type 分配（switch over 所有可达 type） |
| `chaos_delegate_create_multicast_like(type_id, entries)` | 用展开的条目创建多播委托 |
| `chaos_delegate_combine(a, b)` | 展开双端 → 验证类型 → 创建多播 |
| `chaos_delegate_remove(source, value)` | 展开 → 后缀匹配搜索 → 删除匹配条目 |

可达 delegate type 通过 `CollectReachableDelegateTypeSubjectIds` 收集，扫描所有可达方法指令中的 delegate 类型引用、`::Invoke(` 调用、`Marshal.GetFunctionPointerForDelegate<T>` 等。

---

## 签名解码协议

### MetadataMethodSignatureTypeNameProvider

实现 `ISignatureTypeProvider<string, object?>`（`System.Reflection.Metadata`），将 ECMA-335 签名 blob 解码为 display name 字符串，用于构造 SubjectId。

### 解码规则

| 签名元素 | 格式 |
|---------|------|
| 原始类型 | `"System.Int32"`、`"System.String"` 等 |
| 数组 | `"ElementType[]"`（SZ）/ `"ElementType[,,,]"`（多维） |
| 引用参数 | `"Type&"` |
| 函数指针 | `"fnptr<ReturnType(Param1,Param2)>"` |
| 泛型实例化 | 委托 `ManagedNaming.CreateInstantiatedTypeDisplayName()` |
| 泛型方法参数 | `"!!index"` |
| 泛型类型参数 | `"!index"` |

实例化位置：`TryCreateMetadataMethodSubjectId` 中，读取 method definition 的签名后解码，与方法名组合通过 `ManagedNaming.CreateMethodSubjectId` 产生 SubjectId。

---

## 符号命名约定

所有生成的 C++ 符号名称使用统一的 `SanitizeSubjectId` 管道：非字母数字字符替换为 `_`，前导数字前缀 `_`，空字符串回退为 `"subject"`。

### 前缀总表

| 前缀 | 用途 |
|------|------|
| `chaos_type_` | 引用类型 C++ struct 名称 |
| `chaos_valuetype_` | 值类型 C++ struct 名称 |
| `chaos_type_id_` | 类型 ID 常量 |
| `chaos_static_` | 静态字段存储 |
| `chaos_boxed_type_` / `chaos_boxed_type_id_` | 装箱值类型 |
| `chaos_ensure_type_initialized_` / `chaos_type_init_once_` | cctor once_flag |
| `chaos_external_runtime_` | 运行时 helper 函数 |
| `field_` | 成员字段名称 |
| `chaos_string_id_` + hex hash | 字符串字面量 |

### 伪 MetadataHandle

`GetPseudoMetadataHandleLiteral` 为字段/类型/方法合成 32-bit 伪句柄：

| Token 类型 | 24-bit 来源 | OR 前缀 |
|-----------|------------|---------|
| 字段 | SubjectId 的 FNV-1a 哈希(掩码 24-bit) | `0x04000000` |
| 类型 | SubjectId 的 FNV-1a 哈希(掩码 24-bit) | `0x02000000` |
| 方法 | SubjectId 的 FNV-1a 哈希(掩码 24-bit) | `0x06000000` |

输出为 `static_cast<CHAOS_IL2CPP_INTPTR>(...)` 表达式，用于 `ldtoken` 和反射操作中期望 `Runtime*Handle` 的位置。
