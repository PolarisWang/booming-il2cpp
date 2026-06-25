# StringBuilder 模式审计报告

## 全量分析

| 文件 | `AppendLine("硬编码")` | `$"插值"` | `AppendInterpolatedStringHandler` |
|------|----------------------|-----------|----------------------------------|
| ReflectionObjectEmission.cs | **737** | 0 | 112 |
| ObjectModelEmission.cs | **94** | 11 | 14 |
| Methods.Remaining.cs | **64** | 2 | 0 |
| MethodEmission.PInvoke.cs | **61** | 39 | 0 |
| ModuleRegistration.Dispatch.cs | **57** | 30 | 0 |
| ModuleRegistration.cs | **28** | 3 | 0 |
| LinearEmission.cs | 0 | **150** | 0 |
| LinearEmission.Advanced.cs | 0 | **144** | 0 |
| StructuredIR.Emit.cs | 0 | **84** | 0 |
| ExceptionEmission.Utilities.cs | 0 | 112 | 4 |
| ExceptionEmission.EmitInstruction.cs | 3 | **300** | 5 |

## 热点文件

### P0: ReflectionObjectEmission.cs (737次硬编码, 112次Handler)

```csharp
builder.AppendLine("extern \"C\" void f()"); // 硬编码
```
→ 应改为 `AppendInterpolatedStringHandler`。

### P1: ExceptionEmission.EmitInstruction.cs / LinearEmission.cs / StructuredIR.Emit.cs

这些文件大量使用 `$"..."` 插值字符串 (~700处)。
每个 `$"..."` 在 .NET 6+ 中编译为 `DefaultInterpolatedStringHandler` 加上 `AppendFormatted` 调用 —— 性能已较好。
主要收益在于减少临时字符串分配。

## 推荐优先级

| 优先级 | 文件 | 当前模式 | 预估工作量 |
|--------|------|---------|-----------|
| P0 | ReflectionObjectEmission.cs | 737硬编码 -> handler | 4h |
| P1 | ObjectModelEmission.cs | 94硬编码 -> handler | 1h |
| P1 | Methods.Remaining.cs | 64硬编码 -> handler | 0.5h |
| P2 | 所有 $"" 文件 | ~900处 -> handler | 6h |

## 建议

**最大的单文件收益是 ReflectionObjectEmission.cs** (737处硬编码)。
但这些硬编码的字符串输出非常规律 (extern $type信息等)，可以系统性地转换。
