# RCW 实现 (Runtime Callable Wrapper)

## 数据结构

### ComRcwNative (com_rcw.h)

```cpp
struct ComRcwNative {
    CHAOS_IL2CPP_UINT32 magic;           // Magic 值 (类型标签)
    void* identity_unknown;              // 缓存的 IUnknown* (COM identity)
    CHAOS_IL2CPP_INT32 refcount;         // 托管引用计数
    // IID→接口指针缓存 (大小可配置)
};
```

**关键设计：** RCW 在创建时通过 QI 获取 `identity_unknown` 并缓存。COM identity 规则保证：同一 COM 对象的所有接口查询 IUnknown 都返回相同指针。

## 核心函数

### FindOrCreateRcw

全局 RCW 缓存表，以 `identity_unknown` 为键。线程安全（mutex 保护）。

```cpp
ComRcwNative* FindOrCreateRcw(void* unknown_ptr) {
    // 1. QI 获取 identity_unknown
    // 2. 查缓存表
    // 3. 存在 → refcount++，返回
    // 4. 不存在 → 创建新 RCW，插入缓存表，返回
}
```

### QueryInterfaceCached

通过 RCW 查询 COM 接口，可选择缓存结果。

```cpp
void* QueryInterfaceCached(ComRcwNative* rcw, const uint8_t* iid_bytes);
```

### ReleaseRcw

减少引用计数，为 0 时释放 RCW 并从缓存表移除。

## Marshal 桥接函数

通过 P/Invoke 从托管代码访问的本地导出函数：

| 函数 | 用途 |
|------|------|
| `MarshalCreateRcw(unknownPtr)` | 从 IUnknown* 创建 RCW |
| `MarshalReleaseRcw(rcwPtr)` | 释放 RCW |
| `MarshalGetRcwUnknown(rcwPtr)` | 获取 RCW 的 identity_unknown |
| `MarshalRcwQueryInterface(rcwPtr, iidBytes)` | 通过 RCW 查询接口 |
| `MarshalIsRcwHandle(ptr)` | 检查 IntPtr 是否为 RCW |
| `MarshalCallComMethod(ptr, slot, a, b)` | RCW 感知的 ComVtable 分发（自动解 RCW handle） |
| `MarshalCallDirectComMethod(ptr, slot, a, b)` | 直通 ComVtable 分发（无 RCW 检查） |

## 线程安全

- RCW 缓存表使用 `std::mutex` 保护
- 引用计数使用原子操作
- 创建和查询操作均持有锁期间完成

## 版本演进

| 版本 | 能力 |
|------|------|
| V1 | 基础 RCW 创建、缓存、QI |
| V2 | 方法分发（通过 vtable 调用 COM 方法） |
