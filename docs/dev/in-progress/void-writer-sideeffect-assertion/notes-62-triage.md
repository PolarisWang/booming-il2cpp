# 62 项 factoryGap 归因

> 日期：2026-09-20
> 来源：A2 实施后 text-json 新暴露的失败

## 归因结论：**重载未注册 shape → 落 catch-all**

### 判据（读产物）

失败 subject 的生成体调用的是**零参数 catch-all**：

```cpp
chaos_external_runtime_System_Text_Json_Utf8JsonWriter__WriteNumber_System_Void_JsonEncodedText_System_Decimal_();
```

对应 wrapper（`native-aot.generated.cpp:5630`）：
```cpp
extern "C" CHAOS_IL2CPP_INTPTR chaos_external_runtime_..._WriteNumber_System_Void_JsonEncodedText_System_Decimal_() noexcept
{
    ... "catch-all helper invoked: ... WriteNumber:System.Void(JsonEncodedText,System.Decimal)
        — no native body; delegates to Phase 1/2, else returns 0"
    return ChaosExternalRuntimeFallback(...);   // ← 返回 0，不抛
}
```

→ **返回 0、不抛异常** → typed catch 期望的 `InvalidOperationException` 未发生
→ catch-all 分支 `throw new Exception("wrong exception type")` → subject 失败

### 未注册的重载清单（从产物提取）

```
Utf8JsonWriter::WriteString:System.Void(System.String, System.DateTime)
Utf8JsonWriter::WriteString:System.Void(System.String, System.DateTimeOffset)
Utf8JsonWriter::WriteString:System.Void(System.String, System.Guid)
Utf8JsonWriter::WriteString:System.Void(JsonEncodedText, System.String)
Utf8JsonWriter::WriteString:System.Void(JsonEncodedText, System.DateTime)
Utf8JsonWriter::WriteString:System.Void(JsonEncodedText, System.Guid)
Utf8JsonWriter::WriteStringValue:System.Void(JsonEncodedText)
Utf8JsonWriter::WriteNumber:System.Void(JsonEncodedText, System.Decimal)
...
```

**共同特征**：涉及 **`JsonEncodedText`** 或 **`DateTime`/`DateTimeOffset`/`Guid`/`Decimal`**
这些值类型的重载组合。

### 按方法分布

| 方法 | 数量 |
|:-----|:----:|
| `WriteNumber` 系 | 21 |
| `WriteString` 系 | 16 |
| `WriteTo` | 3 |
| `WriteStart{Array,Object}` | 6 |
| 其余（Reset/WriteNull/Boolean/PropertyName/RawValue/Flush/End*） | 16 |

## ⚠️ 重要更正：`Reset` **不是**主因

我在 A2 实施记录（`notes-a2-implemented.md`）里推测「59 项里主因是 Reset 系」——
**该推测错误**。实测归因显示主因是 **未注册的重载**（`JsonEncodedText`/`DateTime` 组合），
`Reset` 只占 2 项。

**教训**：数量分布要用数据说话，不能凭"最显眼的那几个"推测。

## 两类不同性质的问题（需分开处理）

| 类型 | 表现 | 修法 |
|:-----|:-----|:-----|
| **A. 重载未注册 shape**（本批 60+ 项） | 落 catch-all，返回 0 不抛 | **补注册**（native 函数可能已存在） |
| **B. native 语义未复刻**（`Reset` 2 项） | shape 匹配上，但抛错类型/不抛 | **修 native**（见下） |

### B 类的实测证据（.NET vs native）

| 调用 | .NET 实际 | native 实际 |
|:-----|:----------|:------------|
| `Reset(default(Stream))` | `ObjectDisposedException` | `ArgumentNullException` |
| `Reset(Stream.Null)` | `ObjectDisposedException` | **不抛**（返回 0） |

## 下一步

1. **A 类**：逐个补 shape 注册。需先确认对应的 native 函数是否已实现
   （若未实现，则属「实现缺口」而非「注册缺口」）
2. **B 类**：修 `Reset` 系 native，复刻 bare-object 的 `ObjectDisposedException` 语义
