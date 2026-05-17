# 21-MarshalABI 平台能力决策

## 背景

`System.Runtime.InteropServices.Marshal` 类在 CoreCLR 中部分通过 runtime-internal 调用、部分通过 P/Invoke 到 `xlat` 库实现。Native AOT 需要为所有 Marshal 方法提供 codegen 直出路径。

## 决策

### D1: GenericShapeDescriptor 注册优先

所有 Marshal 方法通过 `RuntimeHelperShapeRegistry` 的 `GenericShapeDescriptor` 注册为运行时 Helper 形状，而非扩展 IL 指令翻译路径。

**Why:** Marshal 方法语义与特定 host 平台关联（Win32/CoTaskMem 分配器），不属于 IL 语义。注册为 Helper 形状后，codegen 直接发射 native C++ 函数调用 + 空值检查，无需经过 IR lowering 或异常分区。

**How to apply:** 新增 Marshal 方法一律注册 `GenericShapeDescriptor`，不允许在 IL 指令层添加 marshal 特殊处理。

### D2: 结构体 Marshalling 采用 Descriptor-Driven

复杂结构体（含 string/object/嵌套结构体/数组）使用 `StructMarshallingDescriptorV1` 描述符驱动序列化/反序列化，而非生成逐字段 C++ 赋值代码。

**Why:** 描述符方案在 .rodata 中占用 ~24 字节/字段，函数体生成方案会膨胀生成代码（影响 I-cache）且增加 codegen 复杂度。描述符驱动 marshaller 通过运行时 `struct_marshal.cpp` 一次实现，codegen 只需发射描述符常量。

**How to apply:** codegen 通过 `GetNativeSymbol("chaos_marshal_desc_", subjectId)` 发射描述符指针。运行时 marshaller 根据描述符遍历字段。

### D3: Alloc/Free 使用平台分配器

`AllocHGlobal` → `::HeapAlloc(GetProcessHeap(), ...)` on Windows, `::malloc` on other platforms。`AllocCoTaskMem` → `::CoTaskMemAlloc`。

**Why:** 语义上 HGlobal 对应 Win32 `GlobalAlloc`，CoTaskMem 对应 COM 分配器。在非 Windows 平台映射为 `malloc`/`free` 以保证 ABI 兼容。

**How to apply:** 运行时 marshaller 通过预处理器宏选择平台实现，codegen 透明。

### D4: Delegate Marshalling 通过 Thunk Registry

`GetFunctionPointerForDelegate` 通过 thunk registry 分配 native 可调用的 codegen thunk。`GetDelegateForFunctionPointer` 创建指向 native 函数指针的 dispatch thunk。

**Why:** Managed delegate 调用 convention 与 native 函数调用 convention 不同，需要 thunk 做 ABI 转换。Thunk registry 提供生命周期管理。

**How to apply:** codegen 发射对 `RuntimeHelperShapeRegistry` 中注册的 `MarshalGetFunctionPointerForDelegate`/`MarshalGetDelegateForFunctionPointer` 形状的调用。

### D5: COM Interop 使用 Runtime-Centric VTable Dispatch

COM 方法调用通过 `HybridDispatchKind.ComVtable` 发射 vtable slot 调用（`vtable[3 + method_index]`），CCW 通过 `ComCcw` 结构体管理 GCHandle + 接口表。

**Why:** COM vtable 布局固定（前 3 槽 = IUnknown 方法），slot 计算简单可靠。CCW 通过 GCHandle 保护 managed 对象不被 GC 回收。

**How to apply:** codegen `ExceptionEmission.cs` 发射 ComVtable 调用模版。COM `[PreserveSig]` 通过 metadata 解析。

### D6: ObjectField GC Safety

结构体 Marshalling 中的 object 字段使用 GCHandle 保护，而非暴露裸 managed 引用给 native 代码。

**Why:** Native 代码无法参与 GC 跟踪。如果不创建 GCHandle，GC 可能在 native 读取前回收该对象。

**How to apply:** `FieldKind == ObjectField` 时，`StructureToPtr` 创建 GCHandle，`PtrToStructure` 解码，`DestroyStructure` 释放。
