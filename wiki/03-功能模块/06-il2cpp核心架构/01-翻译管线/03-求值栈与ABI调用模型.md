# 求值栈与ABI调用模型

## 求值栈值表示

Eval stack 上的每个值有 4 种表示形式：

| 表示 | 说明 | 标记方式 |
|------|------|---------|
| Integer | 原始整数值（通用寄存器传递） | 无标记 |
| FloatBitcast | 浮点数的 bitcast 表示 | 由 CarrierKind 隐含 |
| StringId | bit 63 标记的 StringId | `CHAOS_STRING_ID_TAG` (bit 63) |
| ManagedPointer | 托管指针（带 bit 63 标记） | bit 63 标记 |

## StringId 标记机制

参见 `06-字符串ID系统.md`。

## ABI Carrier 模型

12 种 `CarrierKind` 映射到 C/C++ 类型：

| CarrierKind | C++ 类型 | 说明 |
|-------------|---------|------|
| Void | `void` | 无返回值 |
| Int32 | `int32_t` | 32 位整数 |
| Int64 | `int64_t` | 64 位整数 |
| UInt32 | `uint32_t` | 32 位无符号整数 |
| UInt64 | `uint64_t` | 64 位无符号整数 |
| Float | `float` | 单精度浮点 |
| Double | `double` | 双精度浮点 |
| Bool | `bool` | 布尔值 |
| Pointer | `void*` | 通用指针 |
| ManagedPointer | `T*` | 托管指针（带 GC 跟踪） |
| StringId | `uint64_t` | bit63 标记的字符串 ID |
| Struct | `T` (by value) | 值类型结构体 |

## 调用约定映射

- 普通方法调用：`carrier_kind` 决定返回值类型
- 虚方法调用：通过 `resolve_virtual_method` 动态分发
- 委托调用：通过 `delegate_invoke` 桥接
