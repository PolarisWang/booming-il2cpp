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

15 种 `AotCoreIrAbiCarrierKind` 映射到 C/C++ 类型：

| CarrierKind | 值 | C++ 类型 | 说明 |
|-------------|:--:|---------|------|
| Void | 0 | `void` | 无返回值 |
| Int32 | 1 | `int32_t` | 32 位整数 |
| NativeInt | 2 | `intptr_t` | 指针大小整数 |
| ValueTypeByValue | 3 | `T` (by value) | 值类型结构体 |
| Int8 | 4 | `int8_t` | 8 位有符号整数 |
| UInt8 | 5 | `uint8_t` | 8 位无符号整数 |
| Int16 | 6 | `int16_t` | 16 位有符号整数 |
| UInt16 | 7 | `uint16_t` | 16 位无符号整数 |
| Float32 | 8 | `float` | 单精度浮点 |
| Float64 | 9 | `double` | 双精度浮点 |
| Int64 | 10 | `int64_t` | 64 位整数 |
| UInt64 | 11 | `uint64_t` | 64 位无符号整数 |
| ByRef | 12 | `intptr_t` | 托管引用 (ref/out)，ABI 层为指针 |
| MultiReturn | 13 | `intptr_t` | 多返回值标记，hidden pointer 或 struct return |
| ByRefToValueType | 14 | `intptr_t` | 值类型的托管引用 (ref struct/in 参数) |

### 求值栈值与 ABI Carrier 的关系

求值栈上每个值有 4 种表示形式（Integer/FloatBitcast/StringId/ManagedPointer），在 ABI 边界通过 CarrierKind 映射到具体 C++ 类型：

- 调用时：参数按 CarrierKind 映射后传递
- 返回时：返回值按 CarrierKind 映射后写入 `chaos_abi_return`
- struct 多字段：通过 `ValueTypeByValue` 隐藏指针或寄存器拆分传递

## ABI Manifest

每个 NativeAOT DLL 在编译时生成一份自描述的 ABI manifest，记录模块中所有跨 DLL 可见方法的 carrier 签名：

```c
struct ChaosAbiManifestV0 {
    uint32_t abi_version;            // CHAOS_ABI_MANIFEST_VERSION
    uint32_t method_count;           // 本模块方法数
    uint32_t parameters_byte_count;  // 参数 carrier 数组总长度
    uint32_t checksum;               // FNV-1a 校验和（0 = 跳过）
};
// 紧随其后：
//   ChaosAbiMethodEntryV0 entries[method_count]
//   ChaosAbiCarrierCode   parameters[parameters_byte_count]
```

每个方法条目标记 return carrier 和参数数量，参数 carrier 值按方法顺序平铺在参数数组中。Checksum 使用 FNV-1a 算法覆盖 entries + params 载荷，由 C# codegen 在编译期计算、native 端 `ChaosAbiManifestValidate()` 在模块注册时验证。

## 调用约定映射

- 普通方法调用：`carrier_kind` 决定返回值类型
- 虚方法调用：通过 `resolve_virtual_method` 动态分发
- 委托调用：通过 `delegate_invoke` 桥接
