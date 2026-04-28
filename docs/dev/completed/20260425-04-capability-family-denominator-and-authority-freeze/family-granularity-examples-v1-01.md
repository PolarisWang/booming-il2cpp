# Family Granularity Examples v1.01

Date: 2026-04-25 23:00:00 +08:00
Status: draft
Phase: phase-0 (20260425-04)
Parent: foundation-dll-capability-closure-reporting-roadmap

## 1. 概述

本文档提供 5 个实际的 capability family 粒度示例，覆盖简单到复杂的案例，并说明粒度选择理由。每个示例包括：
- family ID 和显示名称
- scope 范围（包含的方法、排除规则）
- 对应的 implementation family
- 对应的 verification gates
- 粒度选择理由

最后提供负面示例，说明过粗和过细粒度的风险。

## 2. 示例总览

| # | Family | DLL | 方法数 | 实现族数 | 复杂度 |
|---|--------|-----|--------|----------|--------|
| 1 | `System.Convert::ToChar` | System.Private.CoreLib | ~18 | 6 | 中等 |
| 2 | `System.Convert::ToString` | System.Private.CoreLib | ~19 | 2 | 中等 |
| 3 | `System.Buffer` | System.Private.CoreLib | ~15 | 2 | 简单 |
| 4 | `System.Enum` | System.Private.CoreLib | ~40 | 4 | 复杂 |
| 5 | `System.Text.Json::Serialize` | System.Text.Json | ~60 | 多 | 复杂 |

## 3. 示例 1: Convert.ToChar（中等复杂度）

### Family 条目

```json
{
  "familyId": "family/System.Private.CoreLib/convert/to-char",
  "displayName": "Convert.ToChar",
  "familyType": "semantic",
  "description": "System.Convert.ToChar 方法族的所有重载。将各种输入类型转换为 char 类型。包含 checked primitive convert、primitive return forwarder、boxed IConvertible invalid-cast、boxed value-type invalid-cast、string char provider、object char provider 和 IConvertible.ToChar 接口实现。",
  "closureStatus": "closed"
}
```

### Scope

```
包含:
  System.Convert::ToChar(System.SByte)
  System.Convert::ToChar(System.Int16)
  System.Convert::ToChar(System.UInt16)
  System.Convert::ToChar(System.UInt32)
  System.Convert::ToChar(System.UInt64)
  System.Convert::ToChar(System.Int32)
  System.Convert::ToChar(System.Int64)
  System.Convert::ToChar(System.Boolean)
  System.Convert::ToChar(System.Single)
  System.Convert::ToChar(System.Double)
  System.Convert::ToChar(System.Decimal)
  System.Convert::ToChar(System.DateTime)
  System.Convert::ToChar(System.String)
  System.Convert::ToChar(System.String, System.IFormatProvider)
  System.Convert::ToChar(System.Object)
  System.Convert::ToChar(System.Object, System.IFormatProvider)

不包含:
  System.Convert::ToChar(没有任何非 Convert 类型的输入重载)
  System.IConvertible::ToChar(...) 纳入同一 family 作为接口实现
```

### Implementation Family 映射

| Implementation Family | Emitter Handler | 模板 | 方法数 |
|----------------------|----------------|------|--------|
| `checked-primitive-convert` | `TryBuildRuntimeSkeletonConvertCheckedPrimitiveConvertStub` | `StaticCheckedByteConvertStub.cpp.scriban` | 5 |
| `primitive-return-forwarder` | `TryBuildRuntimeSkeletonConvertStaticByteReturnForwarderStub` | `StaticByteForwarderStub.cpp.scriban` | 2 |
| `boxed-iconvertible-invalid-cast` | `TryBuildRuntimeSkeletonConvertBoxedIConvertibleInvalidCastStub` | `BoxedValueTypeInstanceCallStub.cpp.scriban` | 3 |
| `boxed-value-type-invalid-cast` | `TryBuildRuntimeSkeletonConvertBoxedValueTypeCharInvalidCastStub` | `BoxedValueTypeInstanceCallStub.cpp.scriban` | 2 |
| `string-char-provider` | `TryBuildRuntimeSkeletonConvertStringCharProviderStub` | `StringCharProviderStub.cpp.scriban` | 2 |
| `object-char-provider` | `TryBuildRuntimeSkeletonConvertObjectCharProviderStub` | `ObjectCharProviderStub.cpp.scriban` | 2 |

### Verification Gates

| Gate | Policy | 当前状态 |
|------|--------|----------|
| audit-input-and-ledger | required | passed |
| managed-proof | required | passed |
| native-proof | required | passed |
| hotupdate-proof | required (CoreLib) | passed |
| benchmark | conditional | not-required |
| codegen-review | required | passed |
| completion-certification | required (DLL 级) | in-progress |

### 粒度选择理由

**为什么 ToChar 是一个独立的 semantic family 而不是 Convert 大类的一部分？**

- **审核判断明确**：审核者可以明确回答"Convert.ToChar 的所有重载是否都已覆盖"。如果放到整个 Convert 大类（100+ 方法），审核者无法一次性判断闭包。
- **实现形态多样但内聚**：6 种不同的 implementation family 都服务于"转换为 char"这一语义目标。审核者只需确认所有输入类型都有对应的处理路径。
- **方法数适中**：18 个方法（含 IConvertible 接口实现）不多不少，适合单次审核。
- **边界清晰**：ToXxx 之间的边界是天然的输出类型边界。ToChar 的输出全是 char，不会混淆到 ToInt32 或 ToDecimal。

**为什么不合并到更大的 Convert 大类？**

如果 Convert.ToChar + Convert.ToInt32 + Convert.ToInt16 + ... 合为一个 family，将包含 100+ 方法。审核者无法一次性确认所有重载的正确性，且单个 family 的闭包失去意义——"Convert 完成了 80%"对审核者没有帮助。

## 4. 示例 2: Convert.ToString（中等复杂度）

### Family 条目

```json
{
  "familyId": "family/System.Private.CoreLib/convert/to-string",
  "displayName": "Convert.ToString",
  "familyType": "semantic",
  "description": "System.Convert.ToString 方法族的所有重载。将各种输入类型转换为 string 类型。包含 primitive passthrough（intrinsic）和 static string call forwarder 路径。",
  "closureStatus": "closed"
}
```

### Scope

```
包含:
  System.Convert::ToString(System.String)
  System.Convert::ToString(System.String, System.IFormatProvider)
  System.Convert::ToString(System.Boolean)
  System.Convert::ToString(System.Boolean, System.IFormatProvider)
  System.Convert::ToString(System.Char)
  System.Convert::ToString(System.Char, System.IFormatProvider)
  System.Convert::ToString(System.Byte)
  System.Convert::ToString(System.Byte, System.IFormatProvider)
  System.Convert::ToString(System.Int16)
  System.Convert::ToString(System.Int16, System.IFormatProvider)
  System.Convert::ToString(System.Int32)
  System.Convert::ToString(System.Int32, System.IFormatProvider)
  System.Convert::ToString(System.Int64)
  System.Convert::ToString(System.Int64, System.IFormatProvider)
  System.Convert::ToString(System.Single)
  System.Convert::ToString(System.Single, System.IFormatProvider)
  System.Convert::ToString(System.Double)
  System.Convert::ToString(System.Double, System.IFormatProvider)
  System.Convert::ToString(System.Decimal)
  System.Convert::ToString(System.Decimal, System.IFormatProvider)
  System.Convert::ToString(System.DateTime)
  System.Convert::ToString(System.DateTime, System.IFormatProvider)
  System.Convert::ToString(System.Object)
  System.Convert::ToString(System.Object, System.IFormatProvider)
  System.Convert::ToString(System.UInt16)
  System.Convert::ToString(System.UInt16, System.IFormatProvider)
  System.Convert::ToString(System.UInt32)
  System.Convert::ToString(System.UInt32, System.IFormatProvider)
  System.Convert::ToString(System.UInt64)
  System.Convert::ToString(System.UInt64, System.IFormatProvider)
```

### Implementation Family 映射

| Implementation Family | Emitter Handler | 模板 | 方法数 |
|----------------------|----------------|------|--------|
| `static-string-call-forwarder` | `TryBuildRuntimeSkeletonConvertStaticStringCallForwarderHandler` | `StaticStringCallForwarderStub.cpp.scriban` | 19 (ToChar residual 归零后的全部 ToString 重载) |
| `object-provider` | TBD (下一轮) | TBD | ~2 (Object 重载含 IFormatProvider 分支) |

### 粒度选择理由

**为什么 ToString 与 ToChar 是独立的 family 而不是合并的 Convert 大类？**

与 ToChar 相同的理由——ToChar 和 ToString 的闭包可以独立判定。审核者回答"ToString 的所有重载都覆盖了吗"时，不需要同时检查 ToChar 的异常语义是否也正确。

**但为什么 ToString 实现形态比 ToChar 简单（只有 2 个 implementation family vs 6 个），却仍然有效？**

这正是语义粒度的优势：即使实现形态少，语义边界仍然清晰。随着 ToString 的 widening 深入（处理 object/radix 分支），implementation family 会自然增加，但 semantic family 的 ID 和 scope 不变。

## 5. 示例 3: System.Buffer（简单）

### Family 条目

```json
{
  "familyId": "family/System.Private.CoreLib/buffer/memory-operations",
  "displayName": "Buffer Memory Operations",
  "familyType": "semantic",
  "description": "System.Buffer 类型的内存操作静态方法族。包含内存复制、内存比较、块复制等底层内存操作。",
  "closureStatus": "in-progress"
}
```

### Scope

```
包含:
  System.Buffer::MemoryCopy(...)
  System.Buffer::BlockCopy(...)
  System.Buffer::ByteLength(...)
  System.Buffer::GetByte(...)
  System.Buffer::SetByte(...)
  System.Buffer::_Memmove(...)
  System.Buffer::_Memcmp(...)
  System.Buffer::_ZeroMemory(...)
  System.Buffer::IsTypePrimitive(...)
  System.Buffer::IsPrimitiveTypeArray(...)

不包含:
  System.Buffer 的内部辅助方法（不进入 public surface）
```

### Implementation Family 映射

| Implementation Family | 说明 | 方法数 |
|----------------------|------|--------|
| `native-pinvoke-forwarder` | 直接转发到 native API（`Memmove`、`Memcmp`、`ZeroMemory`） | 3 |
| `managed-implementation` | 有托管 body 的方法（`BlockCopy`、`ByteLength`、`GetByte`、`SetByte`） | 4 |

### Verification Gates

| Gate | Policy | 当前状态 |
|------|--------|----------|
| audit-input-and-ledger | required | in-progress |
| managed-proof | required | in-progress |
| native-proof | required | in-progress |
| hotupdate-proof | required (CoreLib) | in-progress |
| codegen-review | required | pending |

### 粒度选择理由

**为什么 Buffer 整体是一个 family 而不是按方法拆成多个？**

- **类型单一**：全部方法属于 `System.Buffer` 一个类型，语义高度内聚。
- **方法数少**：~10 个方法，审核者可以一次性审查完毕。
- **实现形态有限**：只有 P/Invoke forwarder 和 managed implementation 两种形态。
- **审核判断简单**：审核者查看是否有遗漏的方法即可做出闭包判定。

**为什么不把 Buffer 和更广泛的 Memory 操作合并？**

`System.Buffer` 与 `System.Memory`、`System.Span<T>` 属于不同语义域。Buffer 是底层字节操作，Memory/Span 是类型化内存视图。合并后审核者需要同时理解两种不同的语义约定。

## 6. 示例 4: System.Enum（复杂）

### Family 条目

```json
{
  "familyId": "family/System.Private.CoreLib/enum/has-flag",
  "displayName": "Enum.HasFlag",
  "familyType": "semantic",
  "description": "System.Enum.HasFlag 相关的方法族。包含 HasFlag 的泛型和非泛型版本，以及底层 IsPrimitiveTypeArray 等辅助方法。",
  "closureStatus": "in-progress"
}
```

### Scope

```
包含:
  System.Enum::HasFlag(T)          // 泛型版本
  System.Enum::HasFlag(Enum)       // 非泛型实例方法
  System.Enum::IsPrimitiveTypeArray(...)  // 辅助

不包含:
  System.Enum::ToString(...)       // 属于 enum/to-string family
  System.Enum::Parse(...)          // 属于 enum/parse family
  System.Enum::GetValues(...)      // 属于 enum/get-values family
  System.Enum::GetNames(...)       // 属于 enum/get-names family
  System.Enum::GetName(...)        // 属于 enum/get-name family
  System.Enum::IsDefined(...)      // 属于 enum/is-defined family
  System.Enum::TryParse(...)       // 属于 enum/try-parse family
```

### Implementation Family 映射

| Implementation Family | 说明 | 方法数 |
|----------------------|------|--------|
| `generic-enum-constraint` | 泛型约束下的 Enum 方法实现 | 1 |
| `runtime-type-dispatch` | 运行时类型分发的非泛型实现 | 2 |

### Verification Gates

| Gate | Policy | 当前状态 |
|------|--------|----------|
| audit-input-and-ledger | required | in-progress |
| managed-proof | required | in-progress |
| native-proof | required | in-progress |

### 粒度选择理由

**为什么 Enum 不是一个 family，而是需要拆成 6 个 sub-families？**

`System.Enum` 有 40+ 方法，跨越完全不同的语义域：
- `HasFlag` 检查标志位（位运算语义）
- `Parse` / `TryParse` 解析字符串到枚举（解析语义）
- `ToString` 格式化枚举值为字符串（格式化语义）
- `GetValues` / `GetNames` / `GetName` 反射枚举元数据（反射语义）
- `IsDefined` 检查枚举值是否合法（校验语义）

审核者不可能一次性判断 40 个方法的覆盖率。按语义拆分为 6 个 family 后，每个 family 的闭包判定变得可行：

- 审核者可以独立检查"HasFlag 及其泛型变体是否都覆盖了"
- "Parse 和 TryParse 的所有重载"比"Enum 的所有方法"更容易审查

**Enum 拆成 6 个 family 是否过多？**

40 个方法 / 6 个 family ≈ 平均每个 family 6-7 个方法。这正好落在"适度宏观"区间（5-200）。而且每个 family 对应的 verification gate 状态可以独立追踪，不会因为某个 family 的阻塞而拖慢其他 family。

## 7. 示例 5: System.Text.Json Serialize（复杂/大型 DLL）

### Family 条目

```json
{
  "familyId": "family/System.Text.Json/serialize/root",
  "displayName": "JsonSerializer.Serialize",
  "familyType": "semantic",
  "description": "System.Text.Json.JsonSerializer.Serialize 方法族的所有 public 重载。包含泛型和非泛型序列化入口。",
  "closureStatus": "not-started"
}
```

### Scope

```
包含:
  JsonSerializer.Serialize<TValue>(TValue, JsonSerializerOptions?)
  JsonSerializer.Serialize<TValue>(TValue, JsonTypeInfo<TValue>)
  JsonSerializer.Serialize(object?, Type, JsonSerializerOptions?)
  JsonSerializer.Serialize(Stream, ...)  // Stream overloads
  ...

不包含:
  JsonSerializer.Deserialize(...)  // 属于 serialize/deserialize family
  Utf8JsonWriter 的方法           // 属于 utf8-writer family
  JsonDocument 的方法              // 属于 json-document family
```

### Implementation Family 映射

| Implementation Family | 说明 | 方法数 |
|----------------------|------|--------|
| `reflection-based` | 基于反射的序列化路径 | ~20 |
| `source-generated` | 基于 source generator 的序列化路径 | ~20 |
| `stream-wrapper` | Stream 包装器 | ~10 |

### 粒度选择理由

**为什么 System.Text.Json 需要更多比 CoreLib 更细的 family？**

`System.Text.Json` 是一个大型 DLL，包含数十种 JSON 处理能力。如果它作为一个整体 family：
- 审核者需要在一次审核中理解序列化、反序列化、JSON 写入、JSON 文档、JSON 节点等不同语义
- 一个 family 包含太多方法（200+），闭包判定不可能在单次审核中完成

所以 `System.Text.Json` 需要拆分为多个 family：
- `serialize/root`: Serialize 入口方法
- `serialize/async`: 异步序列化
- `deserialize/root`: Deserialize 入口
- `deserialize/async`: 异步反序列化
- `utf8-writer`: Utf8JsonWriter 方法
- `json-document`: JsonDocument 方法
- `json-node`: JsonNode 方法

**每个 family 控制在 20-60 个方法**，保持审核可行。

## 8. 负面示例

### 8.1 过粗粒度：把整个 CoreLib 当作一个 family

```
❌ family/System.Private.CoreLib/all
   - 包含 ~30000 方法
```

**风险**：
- 审核者无法判定闭包：没有人能一次性审查 30000 个方法
- closureStatus 永远只能是 "in-progress" 或 "waived"，失去审核意义
- 退化为当前的 project gate 计数：capability closure 再次被空洞化

### 8.2 过粗粒度：把整个 Convert 当作一个 family

```
❌ family/System.Private.CoreLib/convert/all
   - 包含 ~100 方法
   - 跨 ToChar/ToString/ToInt32/ToDecimal/ToDateTime 等不同输出类型
```

**风险**：
- 审核者需要同时理解 char/string/int/decimal/datetime 的转换规则
- 无法回答"转换能力是否完整"——ToChar 已闭包但 ToDateTime 还没做，应该显示什么？
- closure 失去分层粒度：审核者不知道哪个子域已就绪

### 8.3 过细粒度：每个重载是一个 family

```
❌ family/System.Private.CoreLib/convert/to-char/sbyte-only
   - 仅包含 System.Convert::ToChar(System.SByte)
```

**风险**：
- DLL 会有几千个 family，审核者被大量冗余信息淹没
- 审核者需要在数千个"closed"条目中找到真正未闭包的那几个
- 退化为 method count——当审核者看到的不是能力闭包而是方法列表时，capability family 失去意义
- redundancy 增加：ToChar 的 5 个 checked primitive convert 需要 5 个独立的 family 条目，但它们的语义完全相同（"checked byte/char convert"）

### 8.4 过细粒度：按 implementation family 拆分 semantic family

```
❌ family/System.Private.CoreLib/convert/to-char/checked-primitive  (semantic)
❌ family/System.Private.CoreLib/convert/to-char/primitive-return   (semantic)
❌ family/System.Private.CoreLib/convert/to-char/boxed-iconvertible (semantic)
```

**风险**：
- 审核者无法回答"ToChar 的所有变体是否都覆盖了"，因为审核者需要逐个检查 6 个 family
- implementation family 是 codegen 的实现细节，会随 emitter 重构而变化——semantic family 不应随实现变化
- 今天 emiter 拆分 6 个 handler，明天合并成 2 个，semantic family 不应跟着来回变

## 9. 粒度检查清单

判断一个候选 semantic family 的粒度是否合适的检查清单：

| 检查项 | 过粗 | 过细 | 合适 |
|--------|------|------|------|
| 审核者能否在一次审核中判断闭包？ | 否 | 是 | 是 |
| 方法数在 5-200 之间？ | >200 | <5 | 是 |
| 语义是否内聚？ | 跨语义域 | 单方法 | 是 |
| 是否会随 emitter 重构而变化？ | 否 | 是 | 否 |
| 接口/抽象是否共享相同的异常约定？ | 不共享 | N/A | 共享 |
| 审核者是否能看懂 family 的名称和描述？ | 太笼统 | 太细节 | 是 |

## 10. 变更日志

| 日期 | 版本 | 变更 | 原因 |
|------|------|------|------|
| 2026-04-25 | 1.0.0 | 初始版本 | Phase 0 初始创建 |
