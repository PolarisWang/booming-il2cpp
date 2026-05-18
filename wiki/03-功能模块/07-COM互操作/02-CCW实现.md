# CCW 实现 (COM Callable Wrapper)

## 数据结构

### ComCcw (com_ccw.h)

```cpp
struct ComCcw {
    ComCcwVtbl* vtable;                         // → s_ccw_vtbl (IUnknown)
    CHAOS_IL2CPP_UINT32 refcount;               // 外部 COM 引用计数
    CHAOS_IL2CPP_UINT64 gc_handle;              // GCHandle (root 托管对象)
    void* runtime_state;                        // RuntimeState* (GC handle 生命周期)
    CHAOS_IL2CPP_SIZE interface_count;           // 已注册接口数
    ComCcwInterfaceEntry interfaces[kMaxCcwInterfaces]; // 接口表 (entry 0 = IUnknown)
};
```

布局满足 COM 对象要求：第一个字段是 vtable 指针。

### ComCcwInterfaceEntry

```cpp
struct ComCcwInterfaceEntry {
    const CHAOS_IL2CPP_UINT8* guid;   // 16 字节 GUID
    void* vtable;                       // 接口 vtable
    void* ccw_ptr;                      // → 所属 ComCcw (thunk 反向恢复用)
};
```

`ccw_ptr` 是关键设计：QI 返回 `&entry.vtable` 作为接口指针，thunk 通过 `CcwFromInterface` 从这个地址反向计算 entry 位置，再通过 `ccw_ptr` 恢复 CCW。

## 核心函数

### CreateCcw

分配 `ComCcw`（malloc），创建 GCHandle，初始化 IUnknown 接口槽。

```cpp
CHAOS_IL2CPP_INTPTR CreateCcw(void* managed_object, void* runtime_state);
```

### CcwQueryInterface

IUnknown::QueryInterface 实现：

```cpp
for each registered interface:
    if memcmp(iid, guid) == 0:
        if i == 0: *ppv = self          // IUnknown identity
        else:      *ppv = &interfaces[i].vtable  // per-interface identity
        refcount++
        return S_OK
return E_NOINTERFACE
```

### CcwFromInterface

从接口指针反向恢复 ComCcw*：

```cpp
inline ComCcw* CcwFromInterface(void* iface_ptr) {
    auto* entry = reinterpret_cast<ComCcwInterfaceEntry*>(
        static_cast<char*>(iface_ptr) - offsetof(ComCcwInterfaceEntry, vtable));
    return static_cast<ComCcw*>(entry->ccw_ptr);
}
```

### CcwDispatchMethod

通用方法分发器，通过托管对象的 `TypeInfoWarm.iface_map` 查找目标接口方法：

```cpp
void CcwDispatchMethod(void* ccw_ptr, uint64_t iface_stable_id, uint32_t method_index) {
    // 1. 从 GCHandle 获取托管对象
    // 2. 获取 TypeInfoWarm
    // 3. 遍历 iface_map 匹配 iface_stable_id
    // 4. 通过 vtable_offset + method_index 定位方法指针
    // 5. 调用方法
}
```

## IDispatch 支持 (V4)

IDispatch 是 COM 的晚期绑定接口，允许通过名称或 DISPID 调用方法。

### IDispatchVtbl 布局 (com_abi.h)

```cpp
struct IDispatchVtbl : IUnknownVtbl {
    CHAOS_IL2CPP_INT32 (*GetTypeInfoCount)(void* self, CHAOS_IL2CPP_UINT32* pctinfo);
    CHAOS_IL2CPP_INT32 (*GetTypeInfo)(void* self, CHAOS_IL2CPP_UINT32 iTInfo, CHAOS_IL2CPP_INT32 lcid, void** ppTInfo);
    CHAOS_IL2CPP_INT32 (*GetIDsOfNames)(void* self, const CHAOS_IL2CPP_UINT8* riid,
        CHAOS_IL2CPP_INTPTR* rgszNames, CHAOS_IL2CPP_UINT32 cNames, CHAOS_IL2CPP_INT32 lcid,
        CHAOS_IL2CPP_INT32* rgDispId);
    CHAOS_IL2CPP_INT32 (*Invoke)(void* self, CHAOS_IL2CPP_INT32 dispIdMember, const CHAOS_IL2CPP_UINT8* riid,
        CHAOS_IL2CPP_INT32 lcid, CHAOS_IL2CPP_INT16 wFlags, CHAOS_IL2CPP_INTPTR* pDispParams,
        CHAOS_IL2CPP_INTPTR* pVarResult, CHAOS_IL2CPP_INTPTR* pExcepInfo, CHAOS_IL2CPP_INT32* puArgErr);
};
```

### Thunk 实现 (com_ccw.cpp)

| 方法 | 实现策略 |
|------|---------|
| GetTypeInfoCount | 返回 `0`（不支持类型库） |
| GetTypeInfo | 返回 `E_NOTIMPL` |
| GetIDsOfNames | 遍历 iface_map 查找方法名→DISPID 映射，返回顺序 DISPID |
| Invoke | 根据 DISPID 匹配方法，通过 CcwDispatchMethod 分发 |

### IDispatch 的 codegen 支持

当接口标记了 `[ComInterfaceType(ComInterfaceType.InterfaceIsIDispatch)]` 时，codegen 在 vtable 中填充 7 个槽（3 IUnknown + 4 IDispatch）而非 3 个。Scriban 模板条件判断：

```
{{ if is_idispatch }}
    {{ include "IDispatch thunks" }}
{{ end }}
```

全局线程安全工厂表，使 codegen 生成的 `CreateCcwFor_X` 函数可从托管代码访问。

```cpp
using CcwFactoryFn = CHAOS_IL2CPP_INTPTR (*)(void* managed_object, void* runtime_state);

void RegisterCcwFactory(const uint8_t* guid, CcwFactoryFn factory);
CHAOS_IL2CPP_INTPTR CreateCcwForGuid(const uint8_t* guid, void* obj, void* state);
```

每个模块通过静态初始化自动注册其工厂：

```cpp
unsigned int s_ccw_factory_registered_X = []() {
    RegisterCcwFactory(s_ccw_guid_X, CreateCcwFor_X);
    return 1u;
}();
```

## 版本演进

| 版本 | 能力 |
|------|------|
| V1 | IUnknown 基础 (QI/AddRef/Release)，单接口 |
| V2 | 多接口支持，零初始化 vtable |
| V3 | 方法 thunk + CcwDispatchMethod，codegen vtable 填充，factory 注册 |
| V4 | IDispatch 支持 (GetTypeInfoCount/GetTypeInfo/GetIDsOfNames/Invoke) |
