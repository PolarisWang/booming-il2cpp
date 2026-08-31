# 实现计划：stub 残留根治 + ATG 测试覆盖扩展

> 版本：v1-01
> 设计来源：`docs/dev/in-progress/fact-266-stub-codegen/design-v1-01.md`
> 执行顺序：**从风险最高开始**

---

## 0. 风险排序（决定执行顺序）

| 风险等级 | 方法 | 风险描述 |
|---------|------|---------|
| 🔴 高 | **ATG 测试缺口** | 所有 stub 方法能蒙过，是因为 probe 只测 default 输入。这是根本风险，必须先修 |
| 🔴 高 | **Convert.ChangeType** | 返回 null 而不是真装箱转换。任何非-default 输入都会产生错误结果 |
| 🔴 高 | **Enum.TryParse\<T>** | 返回 false 而不是真解析。任何非-default 输入都会产生错误结果 |
| 🟡 中 | **ReadOnlySpan\<T>.ToArray** | 返回 null 而不是新数组 |
| 🟢 低 | **Nullable\<T>.GetValueRefOrDefaultRef** | 返回 struct 起始地址（读 hasValue 字段），应返回 value 字段（offset 4） |

**优先级**：P0（ATG 扩展）→ P1（Convert.ChangeType）→ P2（Enum.TryParse）→ P3（ReadOnlySpan）→ P4（Nullable）

---

## 1. 涉及文件清单

### 1.1 ATG 层（测试覆盖扩展）
| 文件 | 职责 |
|------|------|
| `src/tools/Chaos.IL2CPP.Tools.AutoTestGenerator/ValueGenerator.cs` | 增加 `AddSpecificMethodValueSets()` 方法，对已知 stub 方法生成多值探针集 |
| `src/tools/Chaos.IL2CPP.Tools.AutoTestGenerator/CSharpExpressionBuilder.cs` | 扩展 `GetExpression` 支持 `typeof(DayOfWeek)` 表达式生成 |

### 1.2 Codegen 层（注册新 native 实现）
| 文件 | 职责 |
|------|------|
| `src/managed/Chaos.IL2CPP.Generator/RuntimeSupport/RuntimeHelperShapeRegistry.CoreStubs.Part3.S23.cs` | 注册 Convert.ChangeType / Enum.TryParse 的 SimpleForward 或 InlineShapeDescriptor |
| `src/managed/Chaos.IL2CPP.Generator/RuntimeSupport/...` | 其他需要的注册文件 |

### 1.3 Runtime 层（native 实现）
| 文件 | 职责 |
|------|------|
| `src/native/runtime-core/parse_convert.cpp` | `ChaosConvertChangeType` — TypeCode 分发表 |
| `src/native/runtime-core/runtime_core.h` | extern 声明 |
| `src/native/runtime-core/runtime_stubs/array_stubs.cpp` | `ChaosSpanToArray` — 真数组分配 |
| `src/native/runtime-core/runtime_stubs/enum_stubs.cpp` | `ChaosEnumTryParse` — 反向元数据解析 |

---

## 2. P0：ATG 多值探针扩展（风险最高，先修）

### 改动：`ValueGenerator.cs`

**在 `Generate()` 方法中增加 `AddSpecificMethodValueSets()` 调用**（在 smart set 之后、collection variant 之前）：

```csharp
// 在 Generate() 的 smart set 之后加入：
AddSpecificMethodValueSets(method, paramTypes, sets, usedSignatures, methodIndex);
```

**新增方法**：

```csharp
private void AddSpecificMethodValueSets(
    MethodSignature method,
    string[] paramTypes,
    List<ValueSet> sets,
    HashSet<string> usedSignatures,
    int methodIndex)
{
    // ── Convert.ChangeType(object, TypeCode) ──
    if (method.Name == "ChangeType" && paramTypes.Length == 2)
    {
        // object=42, TypeCode=Int32
        AddUnique(sets, usedSignatures, methodIndex, new[] { "42", "System.TypeCode.Int32" });
        // object=true, TypeCode=Boolean
        AddUnique(sets, usedSignatures, methodIndex, new[] { "true", "System.TypeCode.Boolean" });
        AddUnique(sets, usedSignatures, methodIndex, new[] { "\"hello\"", "System.TypeCode.String" });
    }
    // ── Convert.ChangeType(object, TypeCode, IFormatProvider) ──
    if (method.Name == "ChangeType" && paramTypes.Length == 3)
    {
        AddUnique(sets, usedSignatures, methodIndex, new[] { "42", "System.TypeCode.Int32", "System.Globalization.CultureInfo.InvariantCulture" });
        AddUnique(sets, usedSignatures, methodIndex, new[] { "true", "System.TypeCode.Boolean", "System.Globalization.CultureInfo.InvariantCulture" });
    }
    // ── Enum.TryParse(Type, string, out object) ──
    // SubjectId: System.Enum::TryParse:System.Boolean(System.Type,System.String,System.Object&)
    // paramTypes: [System.Type, System.String, System.Object&]
    if (method.Name == "TryParse" && paramTypes.Length >= 2 && paramTypes[0] == "System.Type")
    {
        // typeof(DayOfWeek), "Monday", out _
        AddUnique(sets, usedSignatures, methodIndex, new[] { "typeof(System.DayOfWeek)", "\"Monday\"", "out default(System.Object)" });
        AddUnique(sets, usedSignatures, methodIndex, new[] { "typeof(System.DayOfWeek)", "\"XYZInvalid\"", "out default(System.Object)" });
    }
    // ── Enum.TryParse(string, bool, out int) — 泛型变体 ──
    // SubjectId: Enum.TryParse<T>(string, bool, out T)
    // paramTypes: [System.String, System.Boolean, System.Int32&]
    if (method.Name == "TryParse" && paramTypes.Length >= 2 && paramTypes[0] == "System.String")
    {
        AddUnique(sets, usedSignatures, methodIndex, new[] { "\"Monday\"", "true", "out default(System.Int32)" });
        AddUnique(sets, usedSignatures, methodIndex, new[] { "\"XYZInvalid\"", "true", "out default(System.Int32)" });
    }
}
```

### 改动：`CSharpExpressionBuilder.cs`

**在 `GetInstanceExpression` 中增加 `typeof(T)` 支持**：当前第 240 行对可解析类型返回 `SubjectInstanceFactory.Create<T>()`，对 `Type` 类型应该返回 `typeof(DayOfWeek)` 这样的表达式。

**关键判断**：`method.Name == "TryParse" && paramTypes[0] == "System.Type"` 时，对 `Type` 类型参数不生成 `default(Type)!`，而是生成 `typeof(DayOfWeek)`。

### 验收
- [ ] Convert.ChangeType 在 probe 阶段生成至少 3 个值集（含非-default）
- [ ] Enum.TryParse 在 probe 阶段生成含 `typeof(DayOfWeek)` 的值集
- [ ] Probe.cs 编译通过（无 CS0103/CS0117）
- [ ] pipeline 跑通，发现 Convert.ChangeType 和 Enum.TryParse 在新值集上 fail

---

## 3. P1：Convert.ChangeType（风险最高的方法）

### 3.1 Runtime：ChaosConvertChangeType TypeCode 分发表

**文件**：`parse_convert.cpp`

```c++
extern "C" CHAOS_IL2CPP_INTPTR ChaosConvertChangeType(CHAOS_IL2CPP_INTPTR obj, CHAOS_IL2CPP_INT32 typeCode) noexcept
{
    if (obj == 0) return 0;
    // IConvertible dispatch: 调用 obj 的 IConvertible.ToXxx 方法
    // 简化实现：对已知类型走手动转换
    switch (typeCode)
    {
        case 3: // Boolean
            return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(ChaosBoxedBool(ChaosUnboxBool(obj)));
        case 9: // Int32
            return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(ChaosBoxedInt32(ChaosUnboxInt32(obj)));
        // 更多 TypeCode...
    }
    return 0;
}
```

**注意**：TypeCode 枚举值（`System.TypeCode`）：Boolean=3, Char=4, SByte=5, Byte=6, Int16=7, UInt16=8, Int32=9, UInt32=10, Int64=11, UInt64=12, Single=13, Double=14, Decimal=15, DateTime=16, String=18。

**简化实现**：对 ATG 的 3 个新值集（42→Int32, true→Boolean, "hello"→String），需要 handled。对其他 TypeCode 返回 default。

### 3.2 Codegen：注册

**文件**：`Part3.S23.cs`

```csharp
registry.RegisterInline(new InlineShapeDescriptor(
    TypeDisplayNamePrefix: "System.Convert",
    MethodName: "ChangeType",
    Resolver: (callee, paramTypes) =>
    {
        if (paramTypes.Count != 2) return null;
        return "ChaosConvertChangeType({0}, {1})";
    }));
```

### 验收
- [ ] ATG 多值探针值集全部 passed（42→Int32, true→Boolean, "hello"→String）
- [ ] 现有 default 值集无回归
- [ ] codegen unit 2143/2143 PASS

---

## 4. P2：Enum.TryParse\<T>（风险次高）

### 4.1 Runtime：ChaosEnumTryParse 反向元数据查找

**文件**：`enum_stubs.cpp` 或新增 `enum_parse_stubs.cpp`

**实现方案**：利用已生成的 `enum_metadata.generated.h` 中的 `EnumMetadataTable` 做反向查找（字符串→枚举值）。

**关键挑战**：当前 `EnumMetadataTable` 是 `fnv24_hash → name` 的正向映射。反向查找需要扫描整个表，匹配 name 字符串，然后通过 fnv24 找到对应的 enum 值。

**简化实现**：
```c++
extern "C" CHAOS_IL2CPP_INT32 ChaosEnumTryParse(CHAOS_IL2CPP_INTPTR enumType, CHAOS_IL2CPP_INTPTR str) noexcept
{
    if (enumType == 0 || str == 0) return 0;
    // 查找 enum 类型的元数据表
    // 扫描 EnumMetadataTable 中的 name 字符串
    // 匹配到后返回对应的枚举值
    // 未匹配返回 0（false）
    return 0;
}
```

**返回值设计**：`ChaosEnumTryParse` 返回 `int32`（0 = false, 1 = true），out 参数通过额外参数传递。

### 验收
- [ ] `Enum.TryParse<DayOfWeek>("Monday", true, out _)` 返回 true, out=DayOfWeek.Monday
- [ ] `Enum.TryParse<DayOfWeek>("XYZInvalid", true, out _)` 返回 false
- [ ] 现有 default 值集无回归

---

## 5. P3：ReadOnlySpan\<T>.ToArray

### 5.1 Runtime：ChaosSpanToArray

**文件**：`array_stubs.cpp`

```c++
extern "C" CHAOS_IL2CPP_INTPTR ChaosSpanInt32ToArray(CHAOS_IL2CPP_INTPTR span_ptr) noexcept
{
    if (span_ptr == 0) return 0;
    // ReadOnlySpan<Int32> 结构: { CHAOS_IL2CPP_INTPTR _pointer; CHAOS_IL2CPP_INT32 _length; }
    auto* ptr = reinterpret_cast<CHAOS_IL2CPP_INTPTR*>(span_ptr);
    auto* data = reinterpret_cast<void*>(ptr[0]);
    auto length = static_cast<CHAOS_IL2CPP_INT32>(ptr[1]);
    // 分配 byte[] 并拷贝数据
    // 对于 default(ReadOnlySpan<int>) = { null, 0 } → 返回空数组
    auto* arr = reinterpret_cast<ManagedArrayAccessor*>(GcAllocateAtomic(sizeof(ManagedArrayAccessor) + length * sizeof(int32_t)));
    if (arr == nullptr) return 0;
    arr->header_data[0] = 0;
    arr->element_type_shape = 0; // byte
    arr->element_type_info = nullptr;
    arr->length = length;
    if (length > 0 && data != nullptr)
        std::memcpy(accessor_get_elements(arr), data, length * sizeof(int32_t));
    return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(arr);
}
```

### 5.2 Codegen：注册

**文件**：`Part3.S23.cs`（或 `Part2.S17.cs`）

```csharp
registry.RegisterInline(new InlineShapeDescriptor(
    TypeDisplayNamePrefix: "System.ReadOnlySpan",
    MethodName: "ToArray",
    Resolver: (callee, paramTypes) =>
    {
        if (paramTypes.Count != 0) return null;
        // 目前只为 Int32 实现（泛型特化）
        return "ChaosSpanInt32ToArray({0})";
    }));
```

### 验收
- [ ] `default(ReadOnlySpan<int>).ToArray()` 返回空 `int[]`（非 null）
- [ ] 非 default Span 的 ToArray 返回正确数组
- [ ] 无回归

---

## 6. P4：Nullable\<T>.GetValueRefOrDefaultRef 偏移量修复

### 6.1 Codegen：改 inline 表达式

**文件**：`Part3.S23.cs`

```csharp
// 当前：返回 {0}（struct 起始地址）
// 改为：返回 struct 起始地址 + 4（value 字段偏移）
// Nullable<T> 内存布局: [bool hasValue(1B) + padding(3B)][T value(N bytes)]
// value 字段偏移量 = 4（对齐后）
registry.RegisterInline(new InlineShapeDescriptor(
    TypeDisplayNamePrefix: "System.Nullable`1",
    MethodName: "GetValueRefOrDefaultRef",
    Resolver: (callee, paramTypes) =>
        "reinterpret_cast<CHAOS_IL2CPP_INTPTR>(" +
        "static_cast<CHAOS_IL2CPP_UINT8*>((void*)({0})) + 4)"));
```

### 验收
- [ ] `default(Nullable<int>).GetValueRefOrDefaultRef()` 返回 0（hasValue=false, value=0）
- [ ] 非 default Nullable 的正确值
- [ ] 无回归

---

## 7. 验证流程

每个任务完成后：
1. 改 ATG（ValueGenerator.cs）→ `dotnet build -c Release` ATG → pipeline 重跑 probe 阶段
2. 改 codegen（Part3.S23.cs）→ 单测 2143/2143 PASS
3. 改 runtime（parse_convert.cpp 等）→ SDK preset 重编 → 同步 chunk → 重建 entry → fact 验证
4. pipeline 全量验证

### 全量验证命令
```bash
cd tests/e2e
export CHAOS_FOUNDATION_DLL="D:/agent/chaos-il2cpp/tests/e2e/translation"
export CHAOS_TESTING_DIR="D:/agent/chaos-il2cpp/tests/e2e"
export PYTHONPATH="$CHAOS_TESTING_DIR"
python -m verification.chunk_pipeline --chunk system-2 --stages build,fact
```

---

## 8. 风险与注意事项

| 风险 | 说明 | 缓解 |
|------|------|------|
| Enum.TryParse 泛型 T:Enum 约束 | ATG 可能无法实例化 `T=DayOfWeek`（需 `where T : Enum`） | 先验证 DllScanner 已支持（已确认 `DllScanner.cs:932` 检测 `Enum` 约束） |
| typeof 表达式在 Probe.cs 中 | `typeof(System.DayOfWeek)` 需要 `using System;` | 已包含在 Probe.cs 的 using 列表 |
| TypeCode 枚举值硬编码 | 不同 .NET 版本 TypeCode 值可能不同 | 只用 ATG probed 的变体，不做全量实现 |
| ReadOnlySpan 是 ref struct | Probe.cs 中不能作为泛型参数但可做局部变量 | 当前 ATG 的 `DllScanner` 已跳过 ref struct 参数的 subject 生成（`IsRefStruct` 检查） |
| Nullable 偏移量 4 可能因平台变化 | 32 位 vs 64 位对齐 | 当前只支持 x64 Windows |

---

## 9. 子任务列表

| 序号 | 任务 | 预估 | 依赖 |
|------|------|------|------|
| M1 | ATG ValueGenerator 多值探针扩展 | 1 天 | 无 |
| M2 | Convert.ChangeType 实现 + 注册 | 1 天 | M1 |
| M3 | Enum.TryParse 反向元数据实现 + 注册 | 2 天 | M1 |
| M4 | ReadOnlySpan.ToArray 实现 + 注册 | 1 天 | 无 |
| M5 | Nullable 偏移量修复 | 0.5 天 | 无 |
| M6 | 全量回归验证 | 0.5 天 | M1-M5 |