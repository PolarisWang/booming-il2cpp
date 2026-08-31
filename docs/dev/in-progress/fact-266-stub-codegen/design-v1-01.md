# 新项目设计：stub 残留根治 + ATG 测试覆盖扩展

> 设计版本：v1-01
> 状态：待审批
> 关联：`docs/dev/in-progress/fact-266-stub-codegen/FACT-REVIEW-2026-08-31.md`

---

## 0. 分类

**Architectural（架构级）** — 涉及 ATG 探针系统、codegen 发射器、运行时 native 三层，且需要跨子任务协调。

---

## 1. 问题定义

### 1.1 已知 stub 方法（4 个真实问题，8 个方法变体）

| 方法 | 当前 AOT 行为 | 正确 .NET 行为 | 根因 |
|------|--------------|----------------|------|
| `Convert.ChangeType(object, TypeCode)` | 返回 null | 应装箱转换 | 无 TypeCode 分发表 |
| `Convert.ChangeType(object, TypeCode, IFormatProvider)` | 返回 null | 同上 | 同上 |
| `Enum.TryParse<T>(string, out T)` | 返回 false, out=0 | 应解析枚举字符串 | 无 EnumMetadata 解析 |
| `Enum.TryParse<T>(string, bool, out T)` | 返回 false, out=0 | 同上（加忽略大小写） | 同上 |
| `ReadOnlySpan<T>.ToArray()` | 返回 null | 应分配 `T[]` | 无 Span 运行时 |
| `Nullable<T>.GetValueRefOrDefaultRef(ref Nullable<T>)` | 返回 &struct (读 hasValue) | 应返回 value 字段指针 | 偏移量错误 |

### 1.2 ATG 测试缺口

**根因**：`ValueGenerator.cs` 的 `Generate()` 方法对所有参数类型生成 `default(T)` 作为 Set0，对引用类型（`object`、`Type`、`string`）的 `DefaultValue` 返回 `null`。因此：

```
Convert.ChangeType(default(object)!, default(TypeCode)!)  →  probe 捕获 null  →  AOT 返回 null  →  测试通过
```

但 `Convert.ChangeType(42, TypeCode.Int32)` 从未被 probe → 永远不会验证 AOT 的完整语义。

**同样的机制**：`Enum.TryParse(default(Type)!, default(string)!, out object)` 中 `default(Type)=null`、`default(string)=null` → probe 返回 false → AOT 返回 false → 测试通过。但 `Enum.TryParse("Monday", true, out DayOfWeek)` 从未被测试。

### 1.3 测试覆盖截止条件

这些 stub 方法通过了 fact，是因为 **ATG 的 value set 生成器没有为这些方法生成非-default 输入**。当前生成策略：
- Set0: 所有参数 default
- Sets 1-3: 逐个参数取边界值（仅对 `BoundaryValues` 表中有定义的原始类型）
- Smart set: 对 delegate/interface/array 用非 null 表达式
- Collection variant: 对数组参数用非空数组

**覆盖盲区**：对于 `object`、`Type`、`TypeCode`（枚举）等参数，`BoundaryValues` 表中没有定义非-default 值。Set0 全是 default → 不暴露 stub 行为。

---

## 2. 方案设计

### 2.1 方案 A：只修实现 + 不扩展 ATG（不推荐）

**做法**：逐个实现 stub 方法的真实 .NET 语义，但不扩展 ATG 测试集。

**问题**：无法验证实现是否正确。当前 fact 测试只覆盖 default 输入，实现真语义后 default 输入仍返回 default，fact 值不变。无法区分"真实现了"和"还是 stub"。

**结论**：**否决**。必须同时扩展 ATG 测试覆盖。

### 2.2 方案 B：扩展 ATG + 逐个实现（推荐）

**两阶段**：
1. 先扩展 ATG value set 生成器，使这些方法被 probe 更多输入值
2. 再逐个实现 native 运行时行为

#### 2.2.1 阶段 1：ATG 扩展

**改动文件**：`ValueGenerator.cs`

**策略**：对 `ValueGenerator.Generate()` 中的"smart set"（第 194-224 行）增加针对特定方法签名的多值生成逻辑。

**具体设计**：

```csharp
// 在 ValueGenerator.cs 的 Generate() 方法中，smart set 之后加入：

// 方案 B 的核心：对已知 stub 方法加入多值集合
// 这些方法不能用 default 值测试，因为 AOT 的 stub 返回 default 恰好匹配 probe
if (method.Name == "ChangeType" && paramTypes.Length >= 2)
{
    // Convert.ChangeType(object, TypeCode) 的额外值集
    // object=42, TypeCode=Int32 → 期望返回装箱 42
    AddUnique(sets, usedSignatures, methodIndex, new[] { "42", "System.TypeCode.Int32" });
    // object=true, TypeCode=Boolean → 期望返回装箱 true
    AddUnique(sets, usedSignatures, methodIndex, new[] { "true", "System.TypeCode.Boolean" });
    // object="hello", TypeCode=String → 期望返回 "hello"
    AddUnique(sets, usedSignatures, methodIndex, new[] { "\"hello\"", "System.TypeCode.String" });
}
```

**针对每个 stub 方法的扩展值集**：

| 方法 | 新值集 | 预期正确行为 |
|------|--------|-------------|
| `Convert.ChangeType(object, TypeCode)` | `(42, Int32)`, `(true, Boolean)`, `(\"hello\", String)` | 返回装箱值 |
| `Convert.ChangeType(object, TypeCode, IFormatProvider)` | `(42, Int32, InvariantCulture)` | 返回装箱值 |
| `Enum.TryParse(Type, string, out object)` | `(typeof(DayOfWeek), "Monday", out _)` | 返回 true, out=DayOfWeek.Monday |
| `Enum.TryParse(Type, string, bool, out object)` | `(typeof(DayOfWeek), "monday", true, out _)` | 返回 true, out=DayOfWeek.Monday |
| `Enum.TryParse<T>(string, out T)` | `("Monday", out DayOfWeek)` | 返回 true, out=DayOfWeek.Monday |
| `Enum.TryParse<T>(string, bool, out T)` | `("monday", true, out DayOfWeek)` | 返回 true, out=DayOfWeek.Monday |
| `ReadOnlySpan<T>.ToArray()` | 需 Span 实例化，暂不扩展 | 返回 `T[]` |
| `Nullable<T>.GetValueRefOrDefaultRef` | 需 non-default Nullable，暂不扩展 | 返回值字段指针 |

**实现细节**：在 `ValueGenerator.cs` 中增加 `AddSpecificMethodValueSets` 方法，由 `Generate()` 调用。这个方法在 `AddUnique` 之后加入额外值集。

**风险**：ATG 的 `CSharpExpressionBuilder` 需要能生成 `typeof(DayOfWeek)` 表达式。当前 `GetExpression` 方法已经能处理 `Type` 类型（返回 `default(Type)!`），需要扩展为 `typeof(DayOfWeek)`。

#### 2.2.2 阶段 2：运行时实现

按优先级排列：

**P0：Nullable\<T>.GetValueRefOrDefaultRef**（低难度，纯 codegen 修）

**问题**：当前 inline 返回 `{0}`（struct 起始地址），wrapper 读 `hasValue` 字段（offset 0）→ 0。但 .NET 语义应返回 `value` 字段地址（offset 4）。

**修复**：在 inline 表达式中加上偏移量 4：
```csharp
// 对于 Nullable<int>，值字段在偏移 4 处
// struct Nullable<T> { bool hasValue; T value; }
// 由于 hasValue=bool(1)+padding=4，value 在 offset 4
return "reinterpret_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_UINT8*>((void*)({0})) + 4)";
```

**验收**：fact 值不变（default(Nullable<int>) 的 hasValue=false 且 value=0 → 读 0）。但 ATG 扩展后，`new Nullable<int>(42).GetValueRefOrDefaultRef()` 应返回 42。

**P1：ReadOnlySpan\<T>.ToArray**（中低难度）

**实现**：在 `array_stubs.cpp` 加 native `ChaosSpanToArray`：
```c++
extern "C" CHAOS_IL2CPP_INTPTR ChaosSpanToArray(CHAOS_IL2CPP_INTPTR span_ptr) noexcept {
    // span_ptr 指向 &chaos_locals[0]（span 结构体）
    // Span<Int32> 结构: CHAOS_IL2CPP_INTPTR _pointer; CHAOS_IL2CPP_INT32 _length;
    auto* ptr = reinterpret_cast<CHAOS_IL2CPP_INTPTR*>(span_ptr);
    auto* elems = reinterpret_cast<void*>(ptr[0]);  // pointer field
    auto length = static_cast<int32_t>(ptr[1]);      // length field
    // 分配并拷贝
    ...
}
```

**验收**：`default(ReadOnlySpan<int>).ToArray()` 返回空数组 `new int[0]`。

**P2：Convert.ChangeType**（高难度）

**实现**：TypeCode→IConvertible 分发表。在 `parse_convert.cpp` 中加 `ChaosConvertChangeType`：
```c++
extern "C" CHAOS_IL2CPP_INTPTR ChaosConvertChangeType(CHAOS_IL2CPP_INTPTR obj, CHAOS_IL2CPP_INT32 typeCode) noexcept {
    if (obj == 0) return 0;
    switch (typeCode) {
        case 3: /* Int32 */ return static_cast<CHAOS_IL2CPP_INTPTR>(42); // 简化
        // 更完整的实现需要 IConvertible 接口调度
    }
    return 0;
}
```

**注意**：`TypeCode` 枚举值：Empty=0, Object=1, DBNull=2, Boolean=3, Char=4, SByte=5, Byte=6, Int16=7, UInt16=8, Int32=9, UInt32=10, Int64=11, UInt64=12, Single=13, Double=14, Decimal=15, DateTime=16, String=18。

**P3：Enum.TryParse\<T>**（高难度）

**实现**：利用 `EnumMetadataExtractor` 生成的 `enum_metadata.generated.h` 中的元数据表做反向查找（字符串→枚举值）。需要：
1. 在 `EnumToString` 的反向操作中，扫描 `EnumMetadataTable` 找到匹配的字符串
2. 返回对应的枚举值

**困难**：当前 `EnumMetadataTable` 是 `Fnv24→name` 的单向映射，没有 `name→Fnv24→value` 的反向索引。需要新增反向查找表或线性扫描回调。

---

## 3. 验收标准

### 3.1 ATG 扩展验收
- pipeline 跑的 probe 阶段必须为 `Convert.ChangeType` 生成至少 3 个值集（含非-default）
- `Enum.TryParse` 必须生成含 `typeof(DayOfWeek) + "Monday"` 的值集
- 这些值集必须编译通过（Probe.cs 无 CS0103/CS0117）

### 3.2 实现验收
- 每个方法的新值集在新的 entry.exe 中必须 passed
- 现有 default 值集仍 passed（无回归）
- codegen unit 2143/2143 PASS

---

## 4. 风险

| 风险 | 缓解 |
|------|------|
| `typeof(DayOfWeek)` 在 Probe.cs 中需要 `System` 命名空间 | 已包含（`using System;`） |
| `Enum.TryParse<T>` 泛型在 Probe.cs 中实例化 | ATG 的 `genericSuffix` 已处理 |
| Convert.ChangeType 返回值类型 `object` → 序列化需特殊处理 | `returnValueJson` 处理装箱值 |
| ReadOnlySpan 实例化需要通过 ref struct 限制 | 方案：暂不扩展 Span 的输入，只修实现 |

---

## 5. 子任务分解

```
1. ATG: ValueGenerator.cs 增加 AddSpecificMethodValueSets
   1a. Convert.ChangeType 多值
   1b. Enum.TryParse 多值（含 typeof 表达式生成）
   1c. ReadOnlySpan/Nullable 暂不扩展（泛型/ref struct 限制）

2. Runtime: 实现 nullable GetValueRefOrDefaultRef 偏移量修复
   2a. 改 inline 表达式，加 offset 4
   2b. 验证 fact 无回归

3. Runtime: 实现 ReadOnlySpan.ToArray 真数组分配
   3a. 写 ChaosSpanToArray native
   3b. 注册 InlineShapeDescriptor
   3c. 验证 fact 无回归

4. Runtime: 实现 Convert.ChangeType TypeCode 分发表
   4a. 写 ChaosConvertChangeType 分发表
   4b. 注册 InlineShapeDescriptor（或 SimpleForward）
   4c. 验证 ATG 多值 probe 全部 passed

5. Runtime: 实现 Enum.TryParse<T> 反向元数据查找
   5a. 新增反向查找表或回调
   5b. 写 ChaosEnumTryParse 真实现
   5c. 验证 ATG 多值 probe 全部 passed
```

---

## 6. 优先级建议

| 优先级 | 任务 | 预估工作量 | 收益 |
|--------|------|-----------|------|
| P0 | ATG 扩展（多值探针） | 1-2 天 | 让 stub 从"不可见"变"可见" |
| P1 | Nullable 偏移量修复 | 0.5 天 | 1 个方法变真实现 |
| P2 | ReadOnlySpan.ToArray | 1 天 | 1 个方法变真实现 |
| P3 | Convert.ChangeType | 2-3 天 | 2 个方法变真实现 |
| P4 | Enum.TryParse<T> | 3-5 天 | 4 个方法变真实现 |

---

## 7. 待定问题（需用户确认）

1. **ATG 扩展粒度**：是为所有方法统一加多值探针，还是只针对已知 stub 方法加白名单？
2. **typeof 表达式**：ATG 的 `CSharpExpressionBuilder` 是否应支持 `typeof(T)` 表达式生成？
3. **Enum.TryParse 的泛型约束**：`Enum.TryParse<T>(string, out T)` 是泛型方法，ATG 的泛型实例化是否已支持？当前 `DllScanner` 能否识别出 `T : Enum` 约束？
4. **ReadOnlySpan 的 ref struct 限制**：ATG 的 Probe.cs 中能否实例化 `ReadOnlySpan<int>`？（ref struct 不能作为泛型参数，但作为局部变量是允许的）