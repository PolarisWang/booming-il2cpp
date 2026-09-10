# Runtime External 依赖实现计划 — Reflection/Type + Exception

> **面向执行 Agent：** 必须使用 dev:subagent-driven-development（如果可用子 Agent）或 dev:executing-plans 来执行本计划。步骤使用复选框（`- [ ]`）语法跟踪。当前任务带有 `task_id: runtime-external-reflection-type`，执行完成时必须先归档当前子任务目录，再同步父任务。

**目标：** 实现 reflection-type family 的 ~35 个 `chaos_reflection_*` 原生 C++ 函数，使 reflection-type family 从 stub 变为真实 il2cpp 翻译路径

**架构：** 方案 C 三层分层——Layer 1（codegen SimpleForward 注册）已就绪，Layer 2（AOT 元数据 constexpr 表）需 emitter 增量生成，Layer 3（runtime-core 原生实现）需新建 reflection_api.cpp + exception_api.cpp

**技术栈：** C++17 constexpr 元数据表 + `reflection_query_model.h` 现有查询基础设施 + TLS RuntimeState + `extern "C"` 符号导出

**架构审核模式：** critical — 涉及 IL2CPP codegen + runtime-core 主线

**结构告警重点：** 文件职责与拆分（emitter 端新增 metadata.h 输出职责；native 端新建独立文件不混入 runtime_core.cpp）

**权责图审核主题：** extension_cpp (emitter 端) → native_reflection (runtime-core 端) — 验证 emitter 生成的 metadata.h 与 native 端 `reflection_api.cpp` 的接口契约一致性

**AOT/IL2CPP/Test Governance Intake：**
- capabilityFamily: System.Private.CoreLib/reflection/type + System.Private.CoreLib/exception
- capabilityItem: reflection-type family + exception family
- ownerSubjectId: ReflectionTypeNativeEntry/ReflectionTypeNativeEntry::Run:System.Int32(System.Int32)
- proofRequired: true
- benchmarkRequired: true
- hotupdateImpact: Smoke — reflection/exception functions are not hotupdate-path but may be called from hotupdate code
- formalVerificationObjects: [reflection-type generated C++, exception metadata, chaos_reflection_get_type_from_handle, chaos_reflection_set_exception_metadata]
- requiredGates: collector -> registry -> workspace

**设计文档：** n/a（brainstorming 结论已写入 STATUS.md）

**问题清零来源：** brainstorming-approval（blocking_questions: [], question_clearance: cleared, clearance_confirmed_by_user: true）

**计划来源：** direct-plan

**预期知识沉淀：** n/a

**收尾约束：** 执行完成后必须进入"结构告警与架构审视 -> 测试通过 -> 归档 completed -> 合并&提交"固定链路。

**全局优先级约束：**
- 优先级 1（性能最优）：元数据表为 constexpr，避免运行时构造开销；chaos_reflection_* 函数直接查表，不走 vtable 间接
- 优先级 2（方案完美性）：三层分层独立（codegen 不触 native、native 不触 codegen）；新增文件职责单一
- 优先级 3（HotUpdate 支持）：constexpr 元数据表在 hotupdate 中通过重新 #include 替换

---

## 文件清单

### 新增文件（3 个）
| 文件 | 职责 |
|------|------|
| `src/native/runtime-core/reflection_api.cpp` | 实现全部 `chaos_reflection_*` 原生函数 |
| `src/native/runtime-core/exception_api.cpp` | 实现 `chaos_reflection_set_exception_metadata`、`chaos_reflection_get_exception_message`、`chaos_reflection_get_exception_param_name` |
| `src/native/runtime-core/reflection_metadata_impl.h` | `reflection_api.cpp` 内部使用的元数据查找 inline helper（可选，如函数简短可 inline 到 cpp） |

### 修改文件（4 个）
| 文件 | 职责 |
|------|------|
| `src/managed/Chaos.IL2CPP.CodeGen/NativeAotLoweringPlanner.cs` | ① 在 `Create()` 中收集 type metadata 并构造 metadata.h content 输出到 template model；② Includes 增加 `"native-aot.metadata.h"` |
| `src/managed/Chaos.IL2CPP.CodeGen/NativeAotEmitter.cs` | 在 `Generate()` 中增加 metadata header 作为第三个 `NativeAotGeneratedSource` 输出 |
| `src/managed/Chaos.IL2CPP.Contracts/ManagedClosureArtifactModels.cs` | 新增 `NativeAotArtifactNames.MetadataHeader` 常量 |
| `src/native/runtime-core/runtime_core.h` | 新增 `chaos_reflection_*` 和 `chaos_reflection_set_exception_metadata` 等函数的声明 |
| `src/native/runtime-core/CMakeLists.txt`（或等效构建文件） | 加入 `reflection_api.cpp` 和 `exception_api.cpp` |

---

## 实现步骤

### Step 1: 定义合约（新增 artifact name + 原生函数声明）

#### 1a. ManagedClosureArtifactModels.cs — 新增常量
```csharp
public static class NativeAotArtifactNames
{
    // 已有
    public const string GeneratedTranslationUnit = "native-aot.generated.cpp";
    public const string ShapeDispatchHeader = "runtime_helper_shapes.h";
    // 新增
    public const string MetadataHeader = "native-aot.metadata.h";
}
```

#### 1b. runtime_core.h — 新增函数声明

在 `namespace chaos::il2cpp::runtime_core` 末尾，`#endif` 之前新增：

```cpp
// ── Reflection API (implemented in reflection_api.cpp) ──
CHAOS_IL2CPP_INTPTR chaos_reflection_get_type_from_handle(CHAOS_IL2CPP_INTPTR runtime_type_handle);
CHAOS_IL2CPP_INTPTR chaos_reflection_get_type_by_name(CHAOS_IL2CPP_INTPTR name_string_id, CHAOS_IL2CPP_INT32 throw_on_error, CHAOS_IL2CPP_INT32 ignore_case);
CHAOS_IL2CPP_INTPTR chaos_reflection_get_assembly(CHAOS_IL2CPP_INTPTR type_handle);
CHAOS_IL2CPP_INTPTR chaos_reflection_get_assembly_name(CHAOS_IL2CPP_INTPTR assembly_handle);
CHAOS_IL2CPP_INTPTR chaos_reflection_get_assembly_name_value(CHAOS_IL2CPP_INTPTR assembly_name_handle);
CHAOS_IL2CPP_INTPTR chaos_reflection_get_declaring_type(CHAOS_IL2CPP_INTPTR type_handle);
CHAOS_IL2CPP_INTPTR chaos_reflection_get_member_name(CHAOS_IL2CPP_INTPTR member_handle);
CHAOS_IL2CPP_INTPTR chaos_reflection_get_parameters(CHAOS_IL2CPP_INTPTR method_handle);
CHAOS_IL2CPP_INTPTR chaos_reflection_get_parameter_name(CHAOS_IL2CPP_INTPTR parameter_handle, CHAOS_IL2CPP_INT32 index);
CHAOS_IL2CPP_INTPTR chaos_reflection_get_method_handle(CHAOS_IL2CPP_INTPTR method_handle);
CHAOS_IL2CPP_INTPTR chaos_reflection_invoke_method(CHAOS_IL2CPP_INTPTR method_handle, CHAOS_IL2CPP_INTPTR obj, CHAOS_IL2CPP_INTPTR args);
CHAOS_IL2CPP_INTPTR chaos_reflection_make_generic_method(CHAOS_IL2CPP_INTPTR method_handle, CHAOS_IL2CPP_INTPTR type_args);
CHAOS_IL2CPP_INTPTR chaos_reflection_get_constructors(CHAOS_IL2CPP_INTPTR type_handle);
CHAOS_IL2CPP_INTPTR chaos_reflection_get_field(CHAOS_IL2CPP_INTPTR type_handle, CHAOS_IL2CPP_INTPTR name_string_id);
CHAOS_IL2CPP_INTPTR chaos_reflection_get_method(CHAOS_IL2CPP_INTPTR type_handle, CHAOS_IL2CPP_INTPTR name_string_id, CHAOS_IL2CPP_INTPTR param_types);
CHAOS_IL2CPP_INTPTR chaos_reflection_get_generic_arguments(CHAOS_IL2CPP_INTPTR type_handle);
CHAOS_IL2CPP_INTPTR chaos_reflection_get_generic_type_definition(CHAOS_IL2CPP_INTPTR type_handle);
CHAOS_IL2CPP_INTPTR chaos_reflection_get_type_handle(CHAOS_IL2CPP_INTPTR type_handle);
CHAOS_IL2CPP_INTPTR chaos_reflection_get_metadata_token(CHAOS_IL2CPP_INTPTR member_handle);
CHAOS_IL2CPP_INTPTR chaos_reflection_create_instance(CHAOS_IL2CPP_INTPTR type_handle, CHAOS_IL2CPP_INTPTR args);
CHAOS_IL2CPP_INTPTR chaos_reflection_get_custom_attribute(CHAOS_IL2CPP_INTPTR member_handle, CHAOS_IL2CPP_INTPTR attribute_type_handle);

// ── Exception API (implemented in exception_api.cpp) ──
void chaos_reflection_set_exception_metadata(CHAOS_IL2CPP_INTPTR exception_obj, CHAOS_IL2CPP_INTPTR message_string_id);
void chaos_reflection_set_exception_metadata_2(CHAOS_IL2CPP_INTPTR exception_obj, CHAOS_IL2CPP_INTPTR message_string_id, CHAOS_IL2CPP_INTPTR param_name_string_id);
CHAOS_IL2CPP_INTPTR chaos_reflection_get_exception_message(CHAOS_IL2CPP_INTPTR exception_obj);
CHAOS_IL2CPP_INTPTR chaos_reflection_get_exception_param_name(CHAOS_IL2CPP_INTPTR exception_obj);

// ── String/Concat helpers ──
CHAOS_IL2CPP_INTPTR chaos_reflection_concat_string_pair_values(CHAOS_IL2CPP_INTPTR left, CHAOS_IL2CPP_INTPTR right);
```

---

### Step 2: Emitter — 生成 native-aot.metadata.h

#### 2a. 理解需要的数据

`reflection-type` family 大量使用 `typeof(byte)` — 即 `System.Byte` 类型。Emiter 需要为 closure manifest 中每个被引用的 Type 生成 `constexpr ReflectionQueryTypeDescriptor`。

数据来源：`ManagedClosureManifestArtifact` 中的 `.Types` 列表（每个 `ManagedTypeModel` 包含 SubjectId、NamespaceName、Name、IsValueType、BaseTypeSubjectId 等）。

#### 2b. NativeAotLoweringPlanner.cs — 修改 `Create()` 

在 `Create()` 方法中新增步骤：构建 metadata.h 内容。

```csharp
// Collect type metadata for AOT metadata table
var metadataContent = BuildMetadataHeader(closureManifest, reachableMethods, externalRuntimeHelpers);

// Add to template model
return new NativeAotTemplateModel
{
    Includes =
    [
        "<chaos/common.h>",
        "\"runtime_core.h\"",
        "\"native-aot.metadata.h\"",  // 新增
    ],
    // ... existing fields ...
    MetadataHeaderContent = metadataContent,  // 新增字段
};
```

同时需要在 `NativeAotTemplateModel` record 中新增 `MetadataHeaderContent` 字段。

#### 2c. 新增 `BuildMetadataHeader()` 方法

输出格式示例：
```cpp
// Auto-generated AOT metadata table for: System.Private.CoreLib
// Generated by Chaos.IL2CPP.CodeGen

#pragma once
#include "reflection_query_model.h"

namespace chaos::il2cpp::runtime_core::aot_metadata {

// Types
constexpr ReflectionQueryFieldDescriptor kFields_System_Private_CoreLib_System_Byte[] = {
    { 0x04000001, "System.Private.CoreLib/System.Byte::m_value", "m_value", "System.Byte" },
};

constexpr ReflectionQueryMethodDescriptor kMethods_System_Private_CoreLib_System_Byte[] = {
    { 0x06000001, "System.Private.CoreLib/System.Byte::CompareTo(System.Object)", "CompareTo", "System.Int32", ... },
};

constexpr ReflectionQueryTypeDescriptor kType_System_Private_CoreLib_System_Byte = {
    .metadata_token = 0x0200000f,
    .subject_id_utf8 = "System.Private.CoreLib/System.Byte",
    .definition_subject_id_utf8 = "System.Private.CoreLib/System.Byte",
    .namespace_name_utf8 = "System",
    .name_utf8 = "Byte",
    .display_name_utf8 = "Byte",
    .generic_type_definition = nullptr,
    .fields = kFields_System_Private_CoreLib_System_Byte,
    .field_count = 1,
    .properties = nullptr,
    .property_count = 0,
    .methods = kMethods_System_Private_CoreLib_System_Byte,
    .method_count = 1,
};

// Image descriptor
constexpr ReflectionQueryTypeDescriptor const* kTypes_System_Private_CoreLib[] = {
    &kType_System_Private_CoreLib_System_Byte,
    // ... more types ...
};

constexpr ReflectionQueryImageDescriptor kImage_System_Private_CoreLib = {
    .image_name_utf8 = "System.Private.CoreLib",
    .types = kTypes_System_Private_CoreLib,
    .type_count = 1,
};

}  // namespace chaos::il2cpp::runtime_core::aot_metadata
```

#### 2d. NativeAotEmitter.cs — 修改 `Generate()`

在 `generatedSources` 列表中增加 metadata header：

```csharp
generatedSources =
[
    new NativeAotGeneratedSource
    {
        RelativePath = NativeAotArtifactNames.GeneratedTranslationUnit,
        Contents = BuildGeneratedTranslationUnit(templateModel),
    },
    new NativeAotGeneratedSource
    {
        RelativePath = NativeAotArtifactNames.ShapeDispatchHeader,
        Contents = templateModel.ShapeDispatchHeaderContent,
    },
    new NativeAotGeneratedSource  // 新增
    {
        RelativePath = NativeAotArtifactNames.MetadataHeader,
        Contents = templateModel.MetadataHeaderContent,
    },
];
```

---

### Step 3: Native — 实现 reflection_api.cpp

文件位置：`src/native/runtime-core/reflection_api.cpp`

核心模式：
```cpp
#include "runtime_core.h"
#include "reflection_query_model.h"
#include "reflection_metadata_impl.h"  // if needed

namespace chaos::il2cpp::runtime_core::aot_metadata {
    // Generated metadata tables are included from native-aot.metadata.h
    // But that header is per-family, not in runtime-core's include path.
    // Instead, runtime-core provides the implementation, and the generated
    // code includes both runtime_core.h and native-aot.metadata.h.
}

// ── Helper: resolve the current image ──
// The current image/family is tracked in RuntimeState.
// For AOT mode, we iterate aot_metadata tables registered at compile time.

CHAOS_IL2CPP_INTPTR chaos_reflection_get_type_from_handle(CHAOS_IL2CPP_INTPTR runtime_type_handle)
{
    // Decode the handle: it could be a metadata token or a null/zero
    if (runtime_type_handle == 0) return 0;
    
    // The runtime_type_handle from ldtoken is a metadata token (ECMA format)
    uint32_t metadata_token = static_cast<uint32_t>(runtime_type_handle);
    
    // Look up in the AOT metadata table.
    // In AOT mode, the per-family metadata header provides the image descriptor.
    // But this function is in runtime-core which doesn't know the per-family tables.
    // 
    // Solution: The metadata tables are included INTO the generated C++ translation unit,
    // and this function is called from that same translation unit.
    // So the constexpr tables defined in the translation unit's namespace are accessible.
    //
    // But wait — chaos_reflection_* are extern "C" in runtime-core, they can't access
    // per-family constexpr tables.
    //
    // Solution: Use a callback-based approach. Runtime-state stores the current
    // image descriptor pointer. When codegen registers a family, it sets the
    // image descriptor. Then reflection functions use it.
    
    auto* runtime_state = GetCurrentRuntimeState();
    if (runtime_state == nullptr) return 0;
    
    // ... use runtime_state to find the image descriptor,
    //     then FindReflectionQueryTypeByToken
}

// ... (more implementations following same pattern)
```

**重要设计决策**：`chaos_reflection_*` 函数需要访问每个 family 独有的元数据表。有三种方式：

| 方式 | 描述 | P1 | P2 | P3 |
|------|------|----|----|----|
| **A: TLS 指针** | `RuntimeState` 中存当前 `ImageDescriptor*` 指针，codegen 注册时设置 | ⭐⭐⭐ | ⭐⭐⭐ | ⭐⭐ |
| **B: 全局注册表** | `runtime-core` 维护一个 `unordered_map<family_id, ImageDescriptor*>` | ⭐⭐ | ⭐⭐⭐ | ⭐⭐⭐ |
| **C: 内联查找** | `reflection_api.cpp` 内直接包含所有已知 family 的 metadata（硬编码） | ⭐⭐ | ⭐ | ⭐ |

**推荐：方式 A（TLS 指针）**。在 `RunNativeAot()` 入口处用 `SetCurrentRuntimeState()` 时，同时设置 `RuntimeState` 中的 image 指针。`chaos_reflection_*` 函数通过 `GetCurrentRuntimeState()` 获取后直接取 image 指针。

但 `RuntimeState` 结构体需要扩展一个字段来存储 image 指针。这个字段是运行时状态，不影响序列化。

**实际情况**：查看 `RuntimeState` 定义：

实际上，`RuntimeState` 在 `runtime_core.cpp` 中定义。需要确认它是否有用户数据 slot 或扩展点。

对于 AOT 模式，更简单的方式：在 `reflection_api.cpp` 中使用一个独立的 `thread_local` 变量存储当前 image 指针。这和 `g_tls_runtime_state` 是同样的模式。

但更好的方式是用现有的 RuntimeState 结构——检查它是否有 custom data 字段。如果没有，用独立的 thread_local。

**简化方案：** 使用独立的 TLS `thread_local const ReflectionQueryImageDescriptor* g_current_aot_image = nullptr;` + setter `SetCurrentAotImage()`。在 `RunNativeAot()` 入口处由生成的代码调用 setter。

**更简化方案：** 不引入全局 TLS，而是让每个 `chaos_reflection_*` 函数接收 image 上下文作为参数——但生成的代码不传递它。所以必须用 TLS。

**最简化方案：** 在 `RuntimeState` 中新增 `aot_image` 字段。在 `bootstrap_runtime()` 中按 family 设置它。这是架构上最干净的方案。

---

### Step 4: Native — 实现 exception_api.cpp

```cpp
#include "runtime_core.h"

// Exception objects in AOT mode use the chaos_object_header format.
// Their structure is:
// struct chaos_exception_object {
//     chaos_object_header header;
//     CHAOS_IL2CPP_INTPTR message;   // StringId
//     CHAOS_IL2CPP_INTPTR inner_exception;
//     CHAOS_IL2CPP_INTPTR stack_trace;
//     CHAOS_IL2CPP_INTPTR hresult;
// };

void chaos_reflection_set_exception_metadata(CHAOS_IL2CPP_INTPTR exception_obj, CHAOS_IL2CPP_INTPTR message_value)
{
    if (exception_obj == 0) return;
    
    // The message can be a StringId (bit 63 set) or a native string pointer
    // Store the message in the exception object's message field
    auto* header = reinterpret_cast<chaos_object_header*>(exception_obj);
    // header + offsetof(message field) = message_value
    // The exact field layout depends on the generated type definition
    // For simplicity, store message in the first data field after header
    auto* message_slot = reinterpret_cast<CHAOS_IL2CPP_INTPTR*>(
        reinterpret_cast<CHAOS_IL2CPP_UINT8*>(header) + sizeof(chaos_object_header));
    *message_slot = message_value;
}

// etc.
```

---

### Step 5: 构建系统

在 `runtime-core` 的 CMakeLists.txt 中新增 `reflection_api.cpp` 和 `exception_api.cpp`。

---

### Step 6: 验证与收尾

1. `dotnet build Chaos.IL2CPP.CodeGen.csproj` — C# codegen 编译通过
2. `cmake --build .` — native 编译通过
3. 重新生成 reflection-type family 的 native-aot.generated.cpp，验证：
   - `native-aot.metadata.h` 存在且包含 `System.Byte` 的 type descriptor
   - generated.cpp 中 `#include "native-aot.metadata.h"`
4. 运行 reflection-type managed proof
5. 运行 codegen-review

---

## 风险

| 风险 | 概率 | 影响 | 缓解 |
|------|------|------|------|
| RuntimeState 中无 image 字段 | 高 | 中 | 新增字段或用独立 thread_local |
| constexpr 表过大影响编译速度 | 低 | 低 | reflection-type 只涉及 ~30 个 type，很小 |
| 异常对象布局不确定 | 中 | 高 | 需要检查生成的 chaos_type_Exception struct 定义 |
| HotUpdate 路径需要额外适配 | 低 | 中 | 第一批不做 hotupdate，仅确保 SimpleForward 不阻塞 |

---

## 执行顺序（按优先级）

```
Step 1a (artifact name) ─→ Step 1b (header declarations)
                              │
                              ↓
                         Step 2 (emitter metadata)
                              │
                              ↓
                    ┌─────────┴─────────┐
                    ↓                   ↓
            Step 3 (reflection)    Step 4 (exception)
                    │                   │
                    └─────────┬─────────┘
                              ↓
                         Step 5 (build)
                              ↓
                         Step 6 (verify)
```
