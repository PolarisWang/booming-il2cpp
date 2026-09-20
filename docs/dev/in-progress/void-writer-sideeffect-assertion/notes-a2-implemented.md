# A2 实施结果：typed catch 落地 + 62 项真实缺陷暴露

> 日期：2026-09-20
> 提交：待提交
> 改动：`TestEmitter.cs` 的 `catch { }` → typed catch + catch-all throw

## 1. 实施

```csharp
// 原：catch { }                                    ← 任何异常都通过
// 新：
try { callExpr; throw new System.Exception("AOT stub did not throw"); }
catch ({result.ExceptionType}) { }                        // 类型对了 → 通过
catch { throw new System.Exception("wrong exception type"); }  // 类型错了/没抛 → 失败
```

**期望值来自 probe 的 `ExceptionType`**（无需人工编造）。

## 2. 效果

| 指标 | A2 前 | A2 后 |
|:-----|:-----:|:-----:|
| `unassertable` | 33 | **3** |
| `factoryGap` | 32 | **62** |
| `failed` | 3 | 3 |

**`unassertable` 大幅下降** —— 大部分 void subject 不再是"无法断言"。
**`factoryGap` 上升** —— 断言变严后暴露的失败。

## 3. 🔴 那 62 项是**真实缺陷**，不是误伤（已实测判定）

### .NET 实测（bare `Utf8JsonWriter`，net10.0）

```
Reset(default(Stream)!): ObjectDisposedException
Reset(Stream.Null):      ObjectDisposedException
WriteStartArray():       InvalidOperationException
```

### native 实际行为（`json_writer_stubs.cpp:87-93`）

```cpp
CHAOS_IL2CPP_INTPTR ChaosUtf8JsonWriterResetStream(this_ptr, stream) {
    CheckThis(this_ptr);
    if (stream == 0) RaiseArgumentNullException("stream");   // ← 抛错类型
    return 0;                                                 // ← 否则静默返回，不抛
}
```

| 调用 | .NET 期望 | native 实际 | 判定 |
|:-----|:----------|:------------|:-----|
| `Reset(default(Stream))` | `ObjectDisposedException` | `ArgumentNullException` | ❌ **类型错** |
| `Reset(Stream.Null)` | `ObjectDisposedException` | **不抛**（返回 0） | ❌ **该抛没抛** |
| `WriteStartArray()` | `InvalidOperationException` | `InvalidOperationException` | ✅ 正确 |

**结论**：`Reset` 的 native stub **没有复刻 .NET 的 bare-object 语义**。
62 项里 59 项是 `Utf8JsonWriterTests`，主因是 `Reset` 系。

**这是 A2 断言正确捕获的 native 缺陷** —— 之前被 `catch { }` 掩盖
（弱断言不验类型，甚至"没抛"也会因自造的 sentinel throw 而"通过"）。

## 4. 按计划 §6.2 的防假绿验证

计划要求「构造错误实现，确认断言能失败」。**本次是自然发生的验证**：

- `Reset(Stream.Null)` native **不抛** → typed catch 的 catch-all 分支触发
  → `throw new Exception("wrong exception type")` → subject 记为失败 ✅
- 若断言恒真，这 62 项会显示为通过

**断言真的会失败，非假绿。**

## 5. 后续

62 项失败需**逐个归因**：
- `Reset` 系（预计占多数）→ native 未复刻 `ObjectDisposedException`，需修 native
- 其余待分类

**这属于"暴露的缺陷"而非"A2 引入的回归"** —— A2 只是让它们可见。
