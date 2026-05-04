# Runtime-Centric 跨 DLL 分派 + STRING_ID 完整架构决策

**来源**: `docs/discuss/il2cpp-mainline/20260504-runtime-centric-method-table`

## 背景

在完成 foundation-dll System.Private.CoreLib convert-char family 的 native AOT benchmark 后，发现三个需要正式架构方案解决的跨 DLL 问题：

1. **TypeInfo 指针跨 DLL 不等**：所有 30+ type comparison 生成 `header->type_info == &chaos_type_info_XXX`，`inline constexpr` 保证跨 TU 唯一地址，但不保证跨 DLL 唯一。跨 DLL 场景下同一类型的 TypeInfo 实例在不同 DLL 有不同地址，指针比较必失败
2. **`ldstr` per-call 堆分配**：string benchmark 中 100k 次 `new String{}` 导致 2.46ms 耗时，比 managed 慢 6x。STRING_ID 基础设施已存在但未被 `EmitLoadStringLiteral` 使用
3. **跨 DLL 调用需统一分派层**：多个 native AOT DLL 部署时，直接 IAT 调用或 inline 都无法支持热更新方法替换

## 已存在的基础设施

审查代码库后发现，解决上述问题的关键基础设施已存在但未连接：

| 组件 | 位置 | 说明 |
|------|------|------|
| `StringIdEmission.cs` | codegen emitter | 编译期 FNV-1a 计算 constexpr 常量、AOT string table 生成 |
| `codegen_bridge.h:221-240` | 合约头文件 | `CHAOS_STRING_ID_TAG`、`chaos_is_string_id()`、`chaos_make_string_id_value()` |
| `string_table.h/.cpp` | runtime-core | `Intern()`/`Resolve()`/`InitializeFromAot()` 完整实现 |
| `TypeInfo::stable_id` | type_info.h | 已存在 FNV-1a 64-bit hash，跨 DLL 稳定 |
| `module_registry` | runtime-core | 64 模块注册表，已有 `RegisterModule()`/`LookupModule()` |
| String struct `string_id` 字段 | ObjectModelEmission.cs:764 | 已预留 `CHAOS_IL2CPP_UINT64 string_id` |
| StringId-aware equality | ObjectEqualityEmission.cs | 已生成 `chaos_is_string_id()` 快速路径 |
| StringId-aware 反射 | ReflectionObjectEmission.cs | 已用 `Resolve()` 解析 StringId |

## 决策

### 1. STRING_ID-first `ldstr`

`EmitLoadStringLiteral` 生成的代码从 per-call 堆分配改为 push tagged StringId：

```
改动前: new String{} → 设 header/length/utf8_data → push ptr
改动后: push chaos_make_string_id_value(chaos_string_id_XXXXX)
```

所有接收 `string` 参数的外部 runtime helper 必须用 `chaos_is_string_id()` + `Resolve()` 处理 StringId。

### 2. TypeInfo 双路径比较

Codegen 生成的 type comparison 从单指针比较改为双条件：

```cpp
// 改动前（跨 DLL 失败）：
chaos_header->type_info == &chaos_type_info_System_Int32

// 改动后（跨 DLL 正确）：
chaos_header->type_info == &chaos_type_info_System_Int32 ||
chaos_header->type_info->stable_id == CHAOS_STABLE_ID_System_Int32
```

- 单 DLL 场景：指针比较 1 条指令（短路求值，stable_id 不执行）
- 跨 DLL 场景：指针不等，stable_id 正确匹配

### 3. method_table 跨 DLL 分派

在 runtime_core 中新增 method_table，替代 IAT 和 inline 跨 DLL 调用：

```cpp
struct MethodTableEntry {
    void* fn_ptr;            // 当前函数指针
    uint32_t module_gen;     // 模块 generation 版本戳
    uint32_t reserved;
};

// 在 runtime_core .data 段预分配
__declspec(dllexport) MethodTableEntry g_method_table[kMethodTableSize];
```

Codegen 生成 `g_method_table[index].fn_ptr(args)` 跨 DLL 调用。

热更 DLL 启动时：`g_method_table[idx].fn_ptr = &hotupdate_fn; g_method_table[idx].module_gen = current_gen;`

| 场景 | 分派方式 | 可热更 |
|------|---------|--------|
| 同 DLL 调用 | direct call | 否 |
| 跨 DLL AOT → AOT | method_table | 是 |
| 热更 DLL → AOT | method_table | 是 |
| 热更替换 AOT | method_table write | 是 |

### 4. Decimal 结构修复

Decimal 作为 value type 错误走了 reference type 发射路径（`ObjectModelEmission.cs:722`），获得不必要的 `chaos_object_header`。修复为走 value type 路径或特化 DecimalCarrier 路径。

## 方案优势

| 维度 | 评分 | 说明 |
|------|------|------|
| 性能 | ⭐⭐⭐⭐ | ldstr 0 alloc；type comparison 指针快速路径 + stable_id fallback；string equality O(1)；method_table 1 次间接跳转 |
| 架构 | ⭐⭐⭐⭐⭐ | 复用已存在的 StringId、stable_id、module_registry、string_table 基础设施 |
| 热更 | ⭐⭐⭐⭐ | method_table 方法级替换 + generation 版本保护 + string_table 域名级清理 |
| 改动量 | 小 | 核心为 `EmitLoadStringLiteral` 单点改动 + string stub 渐进式更新 |

## 涉及文件

### C# CodeGen Emitter（修改）

| 文件 | 改动 |
|------|------|
| `MethodEmission.cs` | `EmitLoadStringLiteral` 改为 push StringId；type comparison 加 stable_id 双条件 |
| `ObjectEqualityEmission.cs` | 字符串比较加 StringId 处理（已部分实现） |
| `ObjectModelEmission.cs` | Decimal 分发修复 |
| `ReflectionObjectEmission.cs` | 反射路径加 StringId 处理（已部分实现） |

### C++ 运行时（新增/修改）

| 文件 | 改动 |
|------|------|
| `runtime-core/method_table.h` | **新增**：MethodTableEntry 结构、全局表声明 |
| `runtime-core/method_table.cpp` | **新增**：表初始化、WriteMethodTable API |
| `runtime-core/runtime_stubs.cpp` | Convert.ToChar(string) 等 stub 加 StringId 处理 |

### Scriban 模板

无改动（StringId 和 method_table 由 codegen emitter 直接生成 C++ 代码）。

## 实施结果

### 实际改动文件清单

#### C# CodeGen Emitter（修改）

| 文件 | 改动 |
|------|------|
| `MethodEmission.cs` | `EmitLoadStringLiteral` 改为 push StringId；virtual dispatch 和 isinst/castclass type comparison 加 stable_id 双条件 |
| `ObjectEqualityEmission.cs` | 字符串 equality 加 type_info->stable_id 双指针比较；字符串类型检测加 stable_id fallback |
| `ObjectModelEmission.cs` | `chaos_is_type_compatible` 内部指针比较改为 `|| stable_id` 双条件；Decimal 跳过 reference type 发射路径 |
| `ReflectionObjectEmission.cs` | 16 处反射类型检测（Type/MethodInfo/ConstructorInfo/FieldInfo）全部加 stable_id fallback |
| `RuntimeHelperShapeRegistry.cs` | Convert.ToChar 等 string 参数外部 helper 加 StringId 识别代码生成 |

#### C++ 运行时（新增/修改）

| 文件 | 改动 |
|------|------|
| `runtime-core/method_table.h` | **新增**：MethodTableEntry 结构、kMethodTableSize=65536 常量、g_method_table 全局声明 |
| `runtime-core/method_table.cpp` | **新增**：InitializeMethodTable/WriteMethodTable/ResolveMethodTable/ClearMethodTableByGeneration API |
| `runtime-core/runtime_core.h` | 新增 `#include "method_table.h"` |
| `runtime-core/CMakeLists.txt` | 新增 method_table.h/.cpp 编译 |

### 关键实现细节

#### 1. STRING_ID-first `ldstr`（已实现）

`EmitLoadStringLiteral` 在 `MethodEmission.cs:1471` 实现：

```csharp
// 已知 AOT 字符串 → push tagged StringId
chaos_eval_stack[chaos_stack_top++] = chaos_make_string_id_value(chaos_string_id_XXXXX);

// 未知字符串 → fallback per-call 堆分配
auto* chaos_string = new String{};
chaos_string->header.type_info = &chaos_type_info_System_String;
```

#### 2. TypeInfo 双路径比较（已实现）

采用 `target_type_info->stable_id` 运行时字段访问，而非生成 `CHAOS_STABLE_ID_XXX` 独立常量：

**`chaos_is_type_compatible` 内部**（ObjectModelEmission.cs）：
```cpp
// 遍历父类型链，每个节点做双条件匹配
if (chaos_current == chaos_target_type_info ||
    chaos_current->stable_id == chaos_target_type_info->stable_id)
```

**inline 指针比较**（MethodEmission.cs / ObjectEqualityEmission.cs）：
```cpp
chaos_header->type_info == &chaos_type_info_SomeType ||
chaos_header->type_info->stable_id == (&chaos_type_info_SomeType)->stable_id
```

**运行时双指针比较**（ObjectEqualityEmission.cs）：
```cpp
if (chaos_left_header->type_info != chaos_right_header->type_info
    && chaos_left_header->type_info->stable_id != chaos_right_header->type_info->stable_id)
```

覆盖范围：2 处 helper 函数 + 4 处 inline 比较 + 16 处反射类型检测 + 2 处 equality 比较。

#### 3. String boundary 物化（已实现）

StringId 在遇到 GC 跟踪内存（数组存储、引用类型字段写入）时必须物化为真实 `System.String*` 指针。在 `MethodEmission.cs` 中新增 `EmitStringMaterialization` 私有方法，生成以下 C++ 模式：

```cpp
// StringId 物化模板
if (chaos_is_string_id(chaos_value)) {
    auto* chaos_resolved = ::chaos::il2cpp::string_table::Resolve(
        chaos_make_string_id_value(chaos_value));
    if (chaos_resolved != nullptr) [[unlikely]] {
        // 分配托管 String，拷贝 utf8_data，释放临时解析结果
    }
}
```

**EmitArrayStore**（`stelem.ref`）：将 `const auto chaos_value` 改为 `auto chaos_value`，声明后插入物化调用。

**EmitFieldStore**（字段写入）：对所有字段类型（static/instance/value-type）在字段赋值逻辑前插入物化保护。位测试（`chaos_is_string_id()`）为快速路径开销约 1 条指令；`Resolve()` 空指针检查防止非 StringId 值误物化。

安全保证：即使非 StringId 值恰巧第 63 位为 1，`Resolve()` 返回 null 使保护不触发，行为不受影响。

#### 4. method_table codegen 集成（已实现）

`method_table.h` 定义 `MethodTableEntry`（16 字节）和 `kMethodTableSize=65536`（总计 1MB）。提供 4 个 API：
- `InitializeMethodTable()` — 全部置零
- `WriteMethodTable(index, fn_ptr, module_gen)` — 单条目写入（热更使用）
- `ResolveMethodTable(index)` — 读取函数指针
- `ClearMethodTableByGeneration(module_gen)` — 按 generation 清理（域卸载）

在 `NativeAotLoweringPlanner.cs` 中新增 codegen 集成：

**索引分配**：`TryGetMethodTableIndex(string? callee, string nativeSymbol, out uint index)` — 从 callee SubjectId 提取 assembly 前缀，与当前模块 `_assemblyName` 比较。跨模块时分配/复用索引。

**调用发射修改**：
- `EmitDirectCall` — 跨模块调用替换 targetSymbol 为 `(*reinterpret_cast<{ret}(*)({params})>(g_method_table[{idx}].fn_ptr))`
- `EmitCallVirt` — HybridDispatchKind.None/Direct/ExternalRuntime 分支应用相同模式

**初始化代码**：`BuildMethodTableInitialization()` 生成 C++ static lambda，在模块注册时调用 `WriteMethodTable()` 填充所有跨模块条目。

当前为 static library 内全局数组，所有测试 family 在 System.Private.CoreLib（同模块）中运行，跨模块检测返回 false。多 DLL 部署时自动激活。

#### 5. Decimal 修复

在 `ObjectModelEmission.cs` reference type 发射循环中新增：
```csharp
if (string.Equals(ns, "System.Private.CoreLib/System.Decimal", StringComparison.Ordinal))
{
    continue;
}
```

### 验证状态

| 验证项 | 状态 |
|--------|------|
| `dotnet build` (Chaos.IL2CPP.CodeGen) | ✅ 0 Error, 89 Warnings（均为 pre-existing nullable 警告） |
| `batch_native_aot_runner.py --families convert-char` | ✅ PASS (codegen pipeline) |
| 跨 DLL type comparison | ✅ 代码已生成，严格等价验证待跨 DLL 测试环境 |
| method_table 基础设施 | ✅ .h/.cpp 创建，CMake 集成，4 个 API |
| StringId 物化 — EmitStringMaterialization | ✅ EmitArrayStore + EmitFieldStore 调用点就绪 |
| method_table codegen 集成 | ✅ 索引分配 + 跨模块检测 + 调用发射修改 + 初始化代码生成 |
| `batch_native_aot_runner.py`（28/32 families） | ✅ PASS（4 个 pre-existing convert 失败） |

## 验证标准

1. `dotnet build` — C# codegen 0 错误 ✅
2. `batch_native_aot_runner.py` — 28/32 families PASS ✅（4 个 pre-existing convert 失败：delegate-core-invocation、generic-nullable-value、reflection-member-basics、threading-tasks-primitives）
3. string benchmark：`ldstr` 100k iteration 从 ~2.46ms 降至 <0.1ms（待 benchmark 验证）
4. 跨 DLL type comparison：模拟两个 DLL 同一类型，stable_id 正确匹配（待集成测试）
5. method_table：热更 DLL 成功替换方法条目，调用走新函数（待集成测试）
6. StringId 物化：数组/字段写入路径含 `chaos_is_string_id()` 保护 + `Resolve()` 空指针安全检查 ✅
7. method_table codegen：`TryGetMethodTableIndex` + `BuildMethodTableInitialization` + `EmitDirectCall`/`EmitCallVirt` 修改 ✅
