# P/Invoke & Marshal 子系统架构

> 适用范围：chaos::il2cpp marshal、COM RCW/CCW/ConnectionPoint、ICustomMarshaler
> 最后更新：2026-05-23

## 架构概述

P/Invoke & Marshal 子系统负责 managed-to-native 和 native-to-managed 的数据编组（marshalling），包括结构体布局转换、字符串编码转换、COM 互操作（RCW/CCW）、ICustomMarshaler 扩展点。子系统以 `engine_binding.h` 中声明的约 60 个函数为对外 API 边界，供 codegen 输出、engine_bridge、bootstrap 和 interpreter 调用。

### 三层架构

```
┌─────────────────────────────────────────────┐
│               API Layer                      │
│   engine_binding.h ~60 函数                  │
│   (P/Invoke, String, Struct, COM, Custom)    │
├─────────────────────────────────────────────┤
│           Orchestration Layer                │
│   marshal_api.cpp (1263 行) — 编组编排       │
│   struct_marshal.cpp (523 行) — 描述符遍历   │
│   com_rcw/ccw/cp (659 行) — COM 生命周期     │
├─────────────────────────────────────────────┤
│            Primitive Layer                   │
│   marshal_alloc.cpp — GC/Domain/Raw 分配     │
│   marshal_string.cpp — UTF-8/16/Ansi 转换    │
│   struct_marshal_descriptors.cpp — 注册表     │
└─────────────────────────────────────────────┘
```

### 文件清单（16 文件，~3587 行）

| 文件 | 行数 | 职责 |
|------|------|------|
| `marshal_abi.h` | 141 | ABI 契约：StructMarshallingDescriptorV1, StructFieldKind |
| `marshal_internal.h` | 153 | 内部辅助：MarshalAllocationHeader, StringNewUtf8 声明 |
| `marshal_copy_helpers.h` | 65 | 内存拷贝辅助 |
| `struct_marshal.h` | 57 | struct_marshal 命名空间 3 公开 API 声明 |
| `com_abi.h` | 21 | COM ABI 常量 |
| `com_rcw.h` | 72 | RCW 公开 API + ComRcwNative 结构体 |
| `com_ccw.h` | 134 | CCW 公开 API + ComCcw 结构体（含 IUnknown/IDispatch） |
| `com_connection_point.h` | 87 | 连接点 API + ConnectionPointList/Container 结构体 |
| `core/marshal_api.cpp` | 1263 | 编排层：CustomMarshaler、BSTR、Variant、COM 创建、P/Invoke 错误 |
| `core/marshal_alloc.cpp` | 157 | 分配：GC Heap, Domain Heap, Raw (malloc/free) |
| `core/marshal_string.cpp` | 183 | 字符串：UTF-8 ↔ UTF-16 ↔ Ansi 转换 |
| `core/struct_marshal.cpp` | 523 | 描述符驱动编组：ManagedToNative/NativeToManaged/Destroy |
| `core/struct_marshal_descriptors.cpp` | 72 | 描述符注册表：静态 + 运行时缓存 |
| `com_rcw.cpp` | 144 | RCW：全局表、FindOrCreate、Release、QI |
| `com_ccw.cpp` | 301 | CCW：CreateCcw/CreateAggregated、IUnknown/IDispatch |
| `com_connection_point.cpp` | 214 | 连接点：Advise/Unadvise、EventSink CCW |

### 依赖方向

```
engine_binding.h (API 声明)
    ↑
marshal_api.cpp ← marshal_string.cpp、marshal_alloc.cpp
    ↑
struct_marshal.cpp ← struct_marshal_descriptors.cpp ← marshal_abi.h (ABI 契约)
    ↑
com_rcw.cpp / com_ccw.cpp / com_connection_point.cpp
    ↑
runtime-core 基础设施 (GC, MemoryDomain, ThreadState)
```

**强制约束：** marshal 层只能依赖 runtime-core 基础设施（GC、MemoryDomain、String、ThreadState），不得反向依赖 bootstrap、interpreter 或 codegen 层。

## 编组描述符 ABI 合约

codegen 与 runtime 之间的核心合约是通过 `StructMarshallingDescriptorV1` 实现的。

### 描述符结构

```cpp
// marshal_abi.h
struct StructMarshallingDescriptorV1 {
    uint16_t total_size;          // 结构体总大小（字节）
    uint16_t field_count;         // 字段数量
    StructFieldDescriptorV1 fields[]; // 变长数组
};

struct StructFieldDescriptorV1 {
    StructFieldKind kind;          // 字段分类
    uint16_t offset;               // 结构体内偏移
    uint16_t size;                 // 字段大小（字符串为 0）
    uint16_t array_count;          // ByValArray 元素数
    NativeElementType element_type; // 数组元素类型
    uint8_t reserved;
    const StructMarshallingDescriptorV1* nested; // 嵌套结构体指针
    const char* custom_marshaler_cookie;          // ICustomMarshaler cookie
};
```

### 字段分类（StructFieldKind）

| 枚举值 | 字段类型 | 编组行为 |
|--------|---------|---------|
| Blittable | 值类型、指针 | 直接 memcpy |
| BoolField | System.Boolean | 4 字节 ↔ 1 字节转换 |
| StringField | System.String | Ansi/Unicode 编码转换，GC 分配 |
| NestedStruct | 嵌套结构体 | 递归描述符遍历 |
| ByValArray | 固定大小数组 | char[N] ↔ String 或 byte[N] ↔ byte[] |
| LPArray | 长度前缀数组 | marshal_api 处理 |
| DecimalField | System.Decimal | 特定布局转换 |
| DateTimeField | System.DateTime | COM DATE 格式转换 |
| ObjectField | System.Object | COM Variant 编组 |
| GuidField | System.Guid | 二进制布局 |
| CustomMarshalerField | ICustomMarshaler | cookie 查找 + 委托调用 |

### 描述符来源

1. **静态描述符（codegen 发出）：** AOT codegen 将 `StructMarshallingDescriptorV1` 常量结构体放入 `.rodata`，通过 `RegisterStaticMarshallingDescriptor(stable_id, desc)` 注册。这是主要路径。
2. **运行时描述符（反射回退）：** 当静态描述符缺失时，`ResolveStructMarshallingDescriptor` 通过 type info 运行时构建描述符，并在 `g_runtime_descriptor_cache` 中缓存。
3. **解析优先级：** 静态注册表 → 运行时反射缓存 → nullptr（无描述符时走 legacy 路径）。

### 编组类别

| 类别 | 值 | 策略 |
|------|-----|------|
| FullyBlittableFastPath | 0 | 直接 memcpy，零开销 |
| LayoutFixedMarshallerPath | 1 | IL2CPP 风格布局固定 |
| DescriptorDrivenMarshallerPath | 2 | 描述符逐个字段遍历 |

## COM RCW（Runtime Callable Wrapper）

RCW 包装 COM 接口指针，使其可被 managed 代码消费。

### 数据结构

```cpp
struct ComRcwNative {
    uint32_t magic;                          // kComRcwMagic = 0x52435721u
    void* identity_unknown;                  // COM 对象 IUnknown 指针
    std::atomic<uint32_t> wrapper_refcount;  // RCW 引用计数
    uint32_t cache_count;                    // 缓存接口数
    InterfaceCacheEntry cache[8];            // QI 缓存
};
```

### 生命周期

1. **创建：** `FindOrCreateRcw(com_ptr)` 检查全局表 `s_rcw_table`，未命中时分配新 ComRcwNative → 调用 AddRef → 写入表。
2. **查找：** 相同 IUnknown 指针返回同一 RCW（全局表保证了唯一性），引用计数递增。
3. **查询接口：** `QueryInterfaceCached(rcw, iid)` 先在缓存中查找，未命中时调用 COM QueryInterface，结果写入缓存（最多 8 个）。
4. **释放：** `ReleaseRcw(rcw)` 递减 wrapper_refcount，到 0 时从全局表移除并调用 Release → free。

### 全局表

```cpp
CHAOS_IL2CPP_MUTEX s_rcw_table_mutex;
CHAOS_IL2CPP_UNORDERED_DENSE_MAP_IDENTITY(void*, ComRcwNative*) s_rcw_table;
```

`std::mutex` 保护，key 为 COM 对象的 `identity_unknown` 指针。

### 非 Win32 行为

在非 Win32 平台上，COM AddRef/Release/QueryInterface 被编译为 no-op。RCW 仍然通过 `std::malloc` 分配和管理，但底层 COM 操作被跳过。

## COM CCW（COM Callable Wrapper）

CCW 包装 managed 对象，使其可被 COM 消费者调用。

### 数据结构

```cpp
struct ComCcw {
    ComCcwVtbl* vtable;              // IUnknown vtable
    std::atomic<uint32_t> refcount;  // COM 引用计数
    uint64_t gc_handle;              // GCHandle 到 managed 对象
    void* runtime_state;
    size_t interface_count;          // 已注册接口数
    void* outer_unknown;             // 聚合用 outer IUnknown
    bool is_aggregated;
    ComCcwInterfaceEntry interfaces[4]; // 接口槽
    ConnectionPointContainer* cp_container; // 连接点容器
};
```

### 身份标识解析

`ResolveCcw(self)` 通过 COM 对象的标准内存布局（第一个字段是 vtable 指针）进行指针运算，从接口标识指针恢复 ComCcw：

```cpp
static ComCcw* ResolveCcw(void* self) {
    ComCcwInterfaceEntry* entry = reinterpret_cast<ComCcwInterfaceEntry*>(
        static_cast<char*>(self) - offsetof(ComCcwInterfaceEntry, vtable));
    return entry->ccw_ptr;
}
```

### IUnknown 实现

- **QueryInterface：** 扫描 `interfaces[]` 数组的 GUID 匹配，IUnknown 请求（零 GUID）返回 CCW 自身指针。
- **AddRef/Release：** 原子操作 refcount，Release 到 0 时触发销毁。
- **接口注册：** `RegisterCcwInterface` 在 `interfaces[]` 中追加（最多 4 个），支持运行时扩展。

### IDispatch 实现

`CcwGetTypeInfoCount` / `CcwGetTypeInfo` 提供 IDispatch 支持，当前返回 E_NOTIMPL（typeinfo 存根）。

### 创建

- **`CreateCcw(managed_obj, runtime_state)`：** 通过 `abi->gc_handle_new` 创建 GCHandle，初始化 ComCcw，返回 opaque handle。
- **`CreateCcwAggregated(managed_obj, runtime_state, outer_unknown)`：** 聚合场景，outer 非空时 `is_aggregated = true`。

## COM 连接点

连接点（Connection Point）实现 COM 事件模型，允许 COM 客户端订阅 managed 对象的事件。

### 数据结构

```cpp
struct ConnectionPoint {
    void* sink_unknown;    // 事件接收器 IUnknown
    uint32_t cookie;       // 唯一标识
    ConnectionPoint* next; // 链表指针
};

struct ConnectionPointList {
    const uint8_t* iid;    // 接口 IID
    ConnectionPoint* head; // 链表头
    uint32_t next_cookie;  // 下一个 cookie 值
};

struct ConnectionPointContainer {
    ConnectionPointList lists[8]; // 最多 8 个接口
    uint32_t list_count;
};
```

### 操作

- **`ComFindConnectionPoints(ccw_handle, iid, conn_ptrs)`：** 查找 CCW 的连接点容器，匹配 IID。
- **`ComAdvise(ccw_handle, iid, sink_unknown, cookie)`：** 创建新的 ConnectionPoint（懒分配容器），追加到链表。
- **`ComUnadvise(ccw_handle, cookie)`：** 按 cookie 移除连接。
- **`ComCreateEventSinkCcw(managed_obj, runtime_state)`：** 创建事件接收器 CCW。

## ICustomMarshaler

ICustomMarshaler 提供托管类型的自定义编组支持，通过 cookie 字符串查找对应 marshaler 实例。

### 缓存机制

```cpp
struct MarshalerSlot {
    char cookie[64];
    void* marshaler_instance;
    MethodInfoHandle method_n2m, method_m2n, method_cnn, method_cmm;
};

const uint32_t kMarshalerCacheSize = 8;
static MarshalerSlot s_marshaler_cache[kMarshalerCacheSize];
static std::atomic_flag s_cache_lock = ATOMIC_FLAG_INIT;
```

### 操作流程

1. `FindMarshalerSlotLocked(cookie_utf8)` 在 8 槽缓存中线性扫描匹配 cookie。
2. 未命中时，通过 `ResolveOrCreateMarshaler` 反射解析 marshaler 类型，创建实例，缓存方法指针。
3. `CustomMarshalerNativeToManaged/ManagedToNative` 调用缓存的 `marshaler_instance->NativeToManaged/ManagedToNative`。
4. `ClearMarshalerCache()` 清空所有槽（hotupdate 场景调用）。

### 并发安全

`std::atomic_flag` 自旋锁保护写路径（缓存更新）。读路径在自旋锁外完成槽位检查，只有未命中时才获取锁。

## P/Invoke 错误管理

每个 ThreadState 持有 `last_pinvoke_error`（int32）。`SetLastPInvokeError` / `GetLastPInvokeError` 提供线程本地 P/Invoke 错误跟踪，对应 `Marshal.GetLastWin32Error`。

## 测试覆盖

### Native 测试（107 passed, 5 skipped）

| 测试套件 | 测试数 | 覆盖范围 |
|---------|--------|---------|
| marshal_smoke | 7 | fixture 基础验证 |
| marshal_alloc | 8 | GC/Domain/Raw 三层分配 |
| marshal_string | 11 | UTF-8/16/Ansi/BSTR 编码转换 |
| marshal_api_basic | 12 | 基础 API 功能 |
| struct_marshal | 14 | 11 种 FieldKind 全覆盖 |
| custom_marshaler | 18 | 缓存、并发、空安全 |
| com_rcw | 12 (7+5skipped) | RCW 创建/缓存/释放/QI |
| com_ccw | 20 | CCW QI/AddRef/Release/注册 |
| com_connection_point | 10 | 连接点空安全 |

### Managed 测试（181 fact tests passed）

11 个 interop 测试族通过 foundation-dll pipeline 验证，覆盖 P/Invoke、COM RCW/CCW/ErrorInfo/Dispatch、S.R.IS API。

## Hotupdate 交互点

| 交互点 | 影响 | 机制 |
|--------|------|------|
| ICustomMarshaler 缓存 | 缓存失效 | `ClearMarshalerCache()` |
| struct_marshal 描述符 | 只读 .rodata | codegen 重新生成 |
| COM RCW 全局表 | 不需要更新 | COM 对象生命周期独立 |

