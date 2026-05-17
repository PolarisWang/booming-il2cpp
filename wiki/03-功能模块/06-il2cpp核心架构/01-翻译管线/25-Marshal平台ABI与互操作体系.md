# Marshal 平台 ABI 与互操作体系

## 概述

Marshal 平台 ABI 定义 `System.Runtime.InteropServices.Marshal` 类在 Native AOT 下的完整互操作路径。涵盖内存块分配/释放、字符串 marshal、结构体布局、委托桥接、COM Interop 和变体类型。

## ABI 载体

```cpp
// src/native/runtime-core/marshal_abi.h
namespace chaos::il2cpp::marshal_abi {

struct MarshalPlatformAbiRootV1 {
    uint32_t abi_version;
    size_t   struct_size;
    uint64_t capability_bits;          // 按位标记已实现能力
    const MarshalMemoryBlockAbiV1*   memory_block;
    const MarshalStringMarshalingAbiV1* string_marshaling;
    const MarshalStructureLayoutAbiV1* structure_layout;
    const MarshalDelegateBridgeAbiV1*  delegate_bridge;
    const ComInteropAbiV1*            com_interop;
    const VariantInteropAbiV1*        variant_interop;
};
```

`MarshalPlatformCapabilityBits` 枚举按位标记已支持能力：

| 位 | 能力 | 对应 Sprint |
|----|------|-------------|
| 0 | `MemoryBlock` — Alloc/Free/Realloc | Sprint 1 |
| 1 | `StringMarshaling` — HGlobal/CoTaskMem UTF8/UTF16 | Sprint 1 |
| 2 | `StructureLayout` — Struct/ByValArray/LPArray/Descriptor | Sprint 2+3 |
| 3 | `DelegateBridge` — GetFunctionPointer/GetDelegate | Sprint 3 |
| 4 | `ComCore` — RCW/CCW/IUnknown | Sprint 5 |
| 5 | `ComActivation` — COM activation | 不支持 |
| 6-8 | Variant 族 | 不支持 |

## 已支持能力表

### 内存管理（Sprint 1）

| 方法 | Codegen 路径 | Native Helper |
|------|-------------|---------------|
| `Marshal.AllocHGlobal(int)` → IntPtr | `RuntimeHelperShapeRegistry` generic shape | `MarshalAllocHGlobal` |
| `Marshal.FreeHGlobal(IntPtr)` | 直接 inline | `MarshalFreeHGlobal` |
| `Marshal.AllocCoTaskMem(int)` → IntPtr | generic shape | `MarshalAllocCoTaskMem` |
| `Marshal.FreeCoTaskMem(IntPtr)` | generic shape | `MarshalFreeCoTaskMem` |
| `Marshal.ReAllocHGlobal(IntPtr, IntPtr)` → IntPtr | generic shape | `MarshalReAllocHGlobal` |
| `Marshal.ReAllocCoTaskMem(IntPtr, int)` → IntPtr | generic shape | `MarshalReAllocCoTaskMem` |
| `Marshal.UnsafeAddrOfPinnedArrayElement(Array, int)` → IntPtr | generic shape | `MarshalUnsafeAddrOfPinnedArrayElement` |

### 字符串 Marshalling（Sprint 1）

| 方法 | 编码 | Native Helper |
|------|------|---------------|
| `Marshal.StringToHGlobalAnsi(string)` → IntPtr | UTF-8 | `MarshalStringToHGlobalAnsi` |
| `Marshal.StringToHGlobalUni(string)` → IntPtr | UTF-16 | `MarshalStringToHGlobalUni` |
| `Marshal.StringToCoTaskMemAnsi(string)` → IntPtr | UTF-8 | `MarshalStringToCoTaskMemAnsi` |
| `Marshal.StringToCoTaskMemUni(string)` → IntPtr | UTF-16 | `MarshalStringToCoTaskMemUni` |
| `Marshal.PtrToStringAnsi(IntPtr, int)` → string | UTF-8 | `MarshalPtrToStringAnsi` |
| `Marshal.PtrToStringUni(IntPtr, int)` → string | UTF-16 | `MarshalPtrToStringUni` |

### 结构体布局（Sprint 2-3）

| 方法 | 实现 |
|------|------|
| `Marshal.SizeOf<T>()` → int | 通过 descriptor 计算托管结构大小 |
| `Marshal.OffsetOf<T>(string)` → IntPtr | descriptor 字段偏移查询 |
| `Marshal.StructureToPtr<T>(T, IntPtr, bool)` | descriptor-driven 序列化 |
| `Marshal.PtrToStructure<T>(IntPtr)` → T | descriptor-driven 反序列化 |
| `Marshal.DestroyStructure<T>(IntPtr)` | 清理 string 等非 blittable 字段 |
| `Marshal.Copy` (byte/sbyte/short/int/long/float/double → native) | 按元素类型 memcpy |
| `Marshal.Copy` (native → byte/sbyte/short/int/long/float/double) | 按元素类型 memcpy |
| LPArray (`[MarshalAs(UnmanagedType.LPArray)]`) | 指针 + 可选长度 |
| ByValArray (`[MarshalAs(UnmanagedType.ByValArray, SizeConst=N)]`) | inline 固定大小数组 |

### 结构体 Marshalling 分类

```cpp
enum class MarshalStructureMarshallingClass : uint32_t {
    FullyBlittableFastPath = 0,        // 全 blittable — 直接 memcpy
    LayoutFixedMarshallerPath = 1,     // 布局固定 marshaller
    DescriptorDrivenMarshallerPath = 2 // 复杂结构 — descriptor 驱动
};
```

Descriptor 驱动路径使用 `StructMarshallingDescriptorV1`：

```cpp
struct StructMarshallingDescriptorV1 {
    uint16_t total_size;              // 结构总大小
    uint16_t field_count;             // 字段数
    StructFieldDescriptorV1 fields[]; // 变长字段数组
};
```

支持字段类型：Blittable, BoolField, StringField, NestedStruct, ByValArray, LPArray, DecimalField, DateTimeField, ObjectField, GuidField。

### ObjectField GC 安全（Sprint 4.2）

Marshalling 描述符中的 `ObjectField` 类型使用 GCHandle 保护 managed 对象引用：

- `StructureToPtr`: 为 object 字段创建 GCHandle，存入 native 结构
- `PtrToStructure`: 从 GCHandle 解码回 object 引用
- `DestroyStructure`: 释放 GCHandle

### HR Exception Helper（Sprint 4.1）

Marshalling 相关的 HRESULT 异常处理：

| Helper | 行为 |
|--------|------|
| `ThrowExceptionForHR(int)` | HR→异常映射 |
| `GetHRForException(Exception)` → int | 异常→HR 映射 |

### 委托桥接（Sprint 3）

| 方法 | 实现 |
|------|------|
| `Marshal.GetFunctionPointerForDelegate<T>(T)` → IntPtr | 通过 thunk registry 分配原生调用 thunk |
| `Marshal.GetDelegateForFunctionPointer<T>(IntPtr)` → T | 创建指向 native 函数的 dispatch thunk |

### COM Interop V1（Sprint 5）

参见 `12-PInvoke翻译路径.md#com-interop-架构v1`（包含在 P/Invoke 路径文档中）。

## Codegen 实现

Marshal 方法的 codegen 通过 `RuntimeHelperShapeRegistry.cs` 中的 `GenericShapeDescriptor` 注册，由运行时 Helper 形状驱动：

```csharp
registry.RegisterGeneric(new GenericShapeDescriptor(
    TypeDisplayNamePrefix: "System.Runtime.InteropServices.Marshal",
    MethodName: "StringToHGlobalAnsi",
    Resolver: static (planner, callee, typeArgs) =>
    {
        var symbol = GetExternalRuntimeHelperSymbol(callee);
        var src = RenderSimpleExternalRuntimeHelper("CHAOS_IL2CPP_INTPTR", symbol,
            "CHAOS_IL2CPP_INTPTR chaos_arg_0",
        [
            "    if (chaos_arg_0 == 0) return 0;",
            "    auto* chaos_runtime = GetCurrentRuntimeState();",
            "    return chaos::il2cpp::runtime_core::MarshalStringToHGlobalAnsi(",
            "        chaos_runtime, nullptr, reinterpret_cast<void*>(chaos_arg_0));",
        ]);
        return new GenericShapeResolution(src, symbol, ...);
    }));
```

Native 实现位于 `src/native/runtime-core/core/marshal_api.cpp`。

## 文件位置

| 文件 | 职责 |
|------|------|
| `src/native/runtime-core/marshal_abi.h` | ABI 类型定义（能力位、descriptor 结构体） |
| `src/native/runtime-core/core/marshal_api.cpp` | 内存/字符串/结构体 marshalling native 实现 |
| `src/native/runtime-core/core/struct_marshal.cpp` | Descriptor-driven 结构体序列化/反序列化 |
| `src/native/runtime-core/engine_binding.h` | Engine → codegen ABI 表注册 |
| `src/managed/.../RuntimeHelperShapeRegistry.cs` | GenericShapeDescriptor 注册 |
| `src/managed/.../MethodEmission.cs` | P/Invoke 4 路径 codegen |
| `verification/foundation-dll/System.Private.CoreLib/pinvoke-dllimport/` | P/Invoke + COM 验证 |

## 已排除能力

- COM Activation (CoCreateInstance)
- COM+/事件/连接点
- WinRT interop
- Variant 类型 (scalar/bulk/extended)
- varargs P/Invoke
