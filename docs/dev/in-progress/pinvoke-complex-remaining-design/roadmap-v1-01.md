# P/Invoke Complex Remaining — 执行路线图

> **面向执行 Agent：** 按阶段顺序推进，每阶段完成定义明确后可继续下一阶段。

**目标：** 完成 Phase 6 (Complex P/Invoke 结构体编组 + COM RCW + 委托编组) + Phase 7 (Reverse P/Invoke) 的全部实现

**架构方案：** C — 混合分层 (Hybrid Layered)
- Layer 1 (Blittable struct): AOT 内联 memcpy
- Layer 2 (Simple non-blittable struct): AOT 逐字段展开 + string helper
- Layer 3 (Complex struct): 运行时 descriptor 引擎
- COM RCW: 编译期 vtable 索引 + 运行时解析
- Delegate: 编译期 native thunk + 运行时缓存
- Reverse P/Invoke: 编译期 stub + bootstrap 注册

**设计文档：** STATUS.md (本目录)
**问题清零来源：** brainstorming 已确认方案 C，用户确认执行

---

## 阶段总览

| 阶段 | 内容 | 依赖 | 工作量 |
|------|------|------|--------|
| **P1** | Struct Layer 1: Blittable 结构体编组 (codegen-only) | 无 | ~80 行 |
| **P2** | Struct Layer 2: 含 string 字段的非 blittable 结构体 | P1 | ~60 行 |
| **P3** | Delegate Marshalling: GetFunctionPointerForDelegate + GetDelegateForFunctionPointer | 无 | ~120 行 |
| **P4** | Reverse P/Invoke: UnmanagedCallersOnly stub + bootstrap | 无 | ~100 行 |
| **P5** | COM RCW: COM vtable 调用生成 + IUnknown 支持 | 无 | ~100 行 |
| **P6** | Struct Layer 3: 复杂结构体运行时 descriptor 引擎 | P1+P2 | ~150 行 |
| **P7** | 综合验证: 全路径端到端测试 | P1-P6 | ~80 行 |

---

## P1: Struct Layer 1 — Blittable 结构体编组

### 目标
P/Invoke 方法中 blittable 结构体参数/返回值的 AOT 内联 memcpy marshalling。

### 当前问题
`EmitPInvokeMethod` 只处理了基元类型和 string，遇到结构体参数时：

```csharp
// 当前: 非基元非 string 类型 → 直接传 NativeInt
// 正确: blittable struct → Marshal.SizeOf + memcpy
```

### 实现

**1a. 检测 blittable 结构体参数**

在 `AotCoreIrLowering.cs` 的 `TryCreateMethod` 中，新增 `BlittableStructParameterIndices`：

```csharp
public IReadOnlyList<int>? BlittableStructParameterIndices { get; init; }
```

判定标准：
- 类型是结构体（IsValueType）
- 所有字段都是 blittable（基元类型 + 仅含基元的结构体）
- 没有 marshalling 属性（[MarshalAs]）

**1b. 扩展 AotCoreIrMethodArtifact**

```csharp
// TypedIlAndAotCoreIrContracts.cs
public IReadOnlyList<int>? BlittableStructParameterIndices { get; init; }
public int? BlittableStructReturnIndex { get; init; }  // -1 表示无结构体返回值
```

**1c. EmitPInvokeMethod 添加 Layer 1 路径**

```cpp
// 伪代码：
if (param is blittable struct) {
    // 前置: 从 managed 对象 memcpy 到 native 栈上
    BlittableStruct managed_struct = *(BlittableStruct*)arg;
    native_param = &managed_struct;  // 传指针
    
    // 后置: 如果 struct 包含需要回写的字段，memcpy 回 managed 对象
}
```

**1d. 返回值处理**

```cpp
if (return is blittable struct) {
    // 在 native 侧分配栈上空间
    // 传指针给 native 函数
    // 返回后 memcpy 到 managed 返回值
}
```

### 涉及文件
- `src/managed/Chaos.IL2CPP.Contracts/TypedIlAndAotCoreIrContracts.cs` — 新增字段
- `src/managed/Chaos.IL2CPP.CodeGen/AotCoreIrLowering.cs` — blittable struct 检测
- `src/managed/Chaos.IL2CPP.CodeGen/Emission/NativeAotLoweringPlanner.MethodEmission.cs` — 内联 memcpy

### 完成定义
- [ ] blittable 结构体参数正确 memcpy 到 native 侧
- [ ] blittable 结构体返回值正确 memcpy 回 managed
- [ `dotnet build Chaos.IL2CPP.CodeGen.csproj` 通过
- [ ] cmake --build build 通过
- [ ] 33/33 测试通过

---

## P2: Struct Layer 2 — 含 string 字段的非 blittable 结构体

### 目标
结构体中 string 字段的正确编组：输入转 UTF-8 CoTaskMem，输出转回 managed String。

### 实现

**2a. StructFieldMarshallingDescriptor (编译期)**

```csharp
// 新 record: 描述每个字段的 marshalling 需求
public sealed record StructFieldMarshalInfo {
    public required int FieldOffset { get; init; }
    public required string FieldType { get; init; }
    public bool IsStringField { get; init; }
}
```

**2b. 扩展 Struct 参数处理**

在 P1 的 memcpy 之后，对 string 字段走 Phase 4 的 helper：
```cpp
// 伪代码：
memcpy(&native_struct, managed_struct_ptr, sizeof(native_struct));
// 遍历 string 字段进行转换
for each string_field:
    auto* managed_str = *(void**)((char*)&native_struct + field.offset);
    auto* native_utf8 = MarshalStringToCoTaskMemUtf8(rs, ts, managed_str);
    *(CHAOS_IL2CPP_INTPTR*)((char*)&native_struct + field.offset) = (CHAOS_IL2CPP_INTPTR)native_utf8;

// 调用 native
native_func(&native_struct);

// 回写 string 字段
for each string_field:
    auto* native_utf8 = *(CHAOS_IL2CPP_INTPTR*)((char*)&native_struct + field.offset);
    auto* managed_str = MarshalPtrToStringUtf8(rs, ts, native_utf8, -1, false);
    *(void**)((char*)managed_struct_ptr + field.offset) = managed_str;
    MarshalFreeCoTaskMem(rs, native_utf8);  // 清理 native buffer
```

### 涉及文件
- `src/managed/Chaos.IL2CPP.Contracts/TypedIlAndAotCoreIrContracts.cs` — StructFieldMarshalInfo
- `src/managed/Chaos.IL2CPP.CodeGen/AotCoreIrLowering.cs` — 字段分析
- `src/managed/Chaos.IL2CPP.CodeGen/Emission/NativeAotLoweringPlanner.MethodEmission.cs` — 字段级 AOT 展开

### 完成定义
- [ ] string 字段在结构体参数中正确编组
- [ ] 回写方向正确 (native → managed)
- [ ] build 通过
- [ ] 测试通过

---

## P3: Delegate Marshalling

### 目标
`GetFunctionPointerForDelegate<T>(T)` 和 `GetDelegateForFunctionPointer<T>(IntPtr)` 的完整 codegen + runtime 路径。

### 实现

**3a. 编译期 native thunk 生成**

为每个需要 delegate marshalling 的 delegate 类型，在 codegen 中生成 native-callable thunk：

```csharp
// MethodEmission.cs
private void EmitDelegateThunk(string delegateTypeId) {
    // 生成：
    // extern "C" ReturnType DelegateTypeId_Thunk(ParamTypes... args) {
    //     auto* rs = GetCurrentRuntimeState();
    //     auto* ts = GetCurrentThreadState();
    //     auto handle = GetDelegateThunkHandle("DelegateTypeId");
    //     auto* del = (DelegateType*)GCHandleToPointer(handle);
    //     return del->Invoke(args...);
    // }
}
```

**3b. 运行时 thunk 注册表**

```cpp
// runtime_core.cpp 新增
struct DelegateThunkEntry {
    const char* type_id;
    void* thunk_fn;
    CHAOS_IL2CPP_UINTPTR gc_handle;
};
static DelegateThunkEntry g_delegate_thunks[64];
static int g_delegate_thunk_count = 0;

void RegisterDelegateThunk(const char* type_id, void* thunk_fn);
void* GetDelegateThunkHandle(const char* type_id);
```

**3c. GetFunctionPointerForDelegate 桥接**

`MarshalGetFunctionPointerForDelegateMethodPrefix` 对应的 stub：

```cpp
// 调用 RegisterDelegateThunk 注册
// 返回 thunk 函数指针
```

**3d. GetDelegateForFunctionPointer 桥接**

```cpp
// 查找已注册的 thunk 匹配
// 创建 delegate 包装器
```

### 涉及文件
- `src/managed/Chaos.IL2CPP.CodeGen/Emission/NativeAotLoweringPlanner.MethodEmission.cs` — thunk 生成
- `src/managed/Chaos.IL2CPP.CodeGen/NativeAotLoweringPlanner.cs` — delegate 类型收集扩展
- `src/native/runtime-core/runtime_core.cpp` — thunk 注册表
- `src/native/runtime-core/runtime_core.h` — API 声明
- `src/native/runtime-core/engine_binding.h` — 外部桥接声明

### 完成定义
- [ ] delegate → native fnptr 正确转换
- [ ] native fnptr → delegate 正确转换
- [ ] thunk 生命周期管理正确（无 dangling pointer）
- [ ] build 通过
- [ ] 测试通过

---

## P4: Reverse P/Invoke — UnmanagedCallersOnly

### 目标
`[UnmanagedCallersOnly]` 方法的完整路径：编译期生成 native stub → bootstrap 注册 → native 代码可直接调用。

### 实现

**4a. NativeAotLoweringPlanner 收集 UnmanagedCallersOnly 方法**

在 planner 中检测标记了 `UnmanagedCallersOnlyAttribute` 的方法，生成 stub 注册信息。

**4b. Stub 生成**

```csharp
// MethodEmission.cs
private void EmitReversePInvokeStub(string methodId) {
    // 生成：
    // extern "C" ReturnType MethodName_ReversePInvokeStub(Params...) {
    //     auto* rs = GetCurrentRuntimeState();
    //     auto* ts = GetCurrentThreadState();
    //     // 调用原始 managed 方法
    //     return MethodName(rs, ts, args...);
    // }
}
```

**4c. CodeRegistrationV0 扩展**

```cpp
// bootstrap.cpp
for (int i = 0; i < codegen.reverse_pinvoke_wrapper_count; i++) {
    auto* stub = codegen.reverse_pinvoke_wrappers[i];
    // stub 已编译到二进制中，直接注册到反向 P/Invoke 表
    RegisterReversePInvokeStub(stub);
}
```

### 涉及文件
- `src/managed/Chaos.IL2CPP.CodeGen/NativeAotLoweringPlanner.cs` — UnmanagedCallersOnly 检测
- `src/managed/Chaos.IL2CPP.CodeGen/Emission/NativeAotLoweringPlanner.MethodEmission.cs` — stub 生成
- `src/native/runtime-core/runtime_core.cpp` — 反向 P/Invoke 注册表
- `src/native/runtime-core/runtime_core.h` — API 声明
- `src/native/runtime-core/bootstrap.cpp` — 消费 wrapper 列表

### 完成定义
- [ ] UnmanagedCallersOnly stub 正确生成
- [ ] bootstrap 正确注册所有 stub
- [ ] native 代码可通过 stub 调用托管方法
- [ ] build 通过
- [ ] 测试通过

---

## P5: COM RCW

### 目标
最小可用 COM RCW 支持：给定 IUnknown 指针，通过 vtable 调用方法。

### 实现

**5a. IUnknown Vtbl 定义**

```cpp
// com_abi.h (新建)
struct IUnknownVtbl {
    CHAOS_IL2CPP_INT32 (*QueryInterface)(void*, const void* riid, void** ppv);
    CHAOS_IL2CPP_INT32 (*AddRef)(void*);
    CHAOS_IL2CPP_INT32 (*Release)(void*);
};
```

**5b. COM RCW codegen**

对于 `[ComImport]` 接口：
```csharp
// 生成 vtable 索引常量
int vtableIndex = GetVtableIndex(method);
// 生成调用：
// auto* vtbl = *reinterpret_cast<IUnknownVtbl**>(comPtr);
// return vtbl->methods[vtableIndex](comPtr, args...);
```

**5c. RCW 生命周期**

```cpp
// AddRef 在包装创建时
// Release 在 GC 时或包装器 Dispose 时
```

### 涉及文件
- `src/native/runtime-core/com_abi.h` — COM Vtbl 定义
- `src/managed/Chaos.IL2CPP.CodeGen/Emission/NativeAotLoweringPlanner.MethodEmission.cs` — vtable 调用生成
- `src/managed/Chaos.IL2CPP.CodeGen/NativeAotLoweringPlanner.cs` — ComImport 检测

### 完成定义
- [ ] IUnknown vtable 调用正确
- [ ] RCW AddRef/Release 生命周期正确
- [ ] build 通过
- [ ] 测试通过

---

## P6: Struct Layer 3 — 复杂结构体运行时 Descriptor 引擎

### 目标
含嵌套结构体、复杂字段的结构体通过运行时 descriptor 引擎完成 marshalling。

### 实现

**6a. StructMarshallingDescriptor (运行时)**

```cpp
enum class StructFieldKind : uint8_t {
    Blittable,    // 定长 blittable 字段，memcpy
    String,       // string ↔ UTF-8 CoTaskMem
    NestedStruct, // 嵌套结构体，递归
};

struct StructFieldDescriptor {
    StructFieldKind kind;
    uint16_t offset;          // 在结构体中的 offset
    uint16_t size;            // 字段大小
    uint16_t element_count;   // >1 表示数组
    const StructMarshallingDescriptor* nested; // NestedStruct 时指向子描述符
};

struct StructMarshallingDescriptor {
    uint16_t total_size;
    uint16_t field_count;
    StructFieldDescriptor fields[];
};
```

**6b. RuntimeStructMarshaller**

```cpp
CHAGetCurrentRuntimeStateS_IL2CPP_INTPTR RuntimeStructMarshaller(
    const StructMarshallingDescriptor* desc,
    void* native_ptr,           // 目标 native 缓冲区
    void* managed_ptr,          // 源 managed 对象
    MarshalDirection direction // NativeToManaged / ManagedToNative
);
```

**6c. Codegen 生成 descriptor**

```csharp
// 代码生成 StructMarshallingDescriptor 静态实例
// 调用 RuntimeStructMarshaller
```

### 涉及文件
- `src/native/runtime-core/marshal_abi.h` — descriptor 类型定义
- `src/native/runtime-core/runtime_core.cpp` — RuntimeStructMarshaller
- `src/managed/Chaos.IL2CPP.CodeGen/Emission/NativeAotLoweringPlanner.MethodEmission.cs` — descriptor 生成

### 完成定义
- [ ] 嵌套结构体递归 marshalling 正确
- [ ] descriptor 路径不泄漏内存
- [ ] build 通过
- [ ] 测试通过

---

## P7: 综合验证

### 目标
所有路径的端到端验证，确保无回归。

### 实现
- blittable struct P/Invoke 端到端
- string-in-struct P/Invoke 端到端
- delegate marshalling 端到端
- Reverse P/Invoke 端到端
- COM RCW 端到端
- 现有 33 测试无回归

### 涉及文件
- 测试文件（按需创建）

### 完成定义
- [ ] 所有新路径测试通过
- [ ] 33/33 回归测试通过
- [ ] build 通过
