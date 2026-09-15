# 20 个 realDefect 深度调试任务

> **task_id**: debug-20-real-defects
> **创建日期**: 2026-09-15
> **entry_skill**: dev-il2cpp → dev-il2cpp-debug-expert
> **上游**: `docs/dev/completed/20260914-system-chunk-fact-rebuild/`（能力清单 + 结构化匹配 + Phase B）

## 症状

system chunk 能力清单判定 20 个方法为 `realDefect`（有 codegen shape 但 AOT 返回错误值）。表现在 `fact-results.json` 中：

```json
{
  "resultKind": "realDefect",
  "caught": true,           // AOT 抛了托管异常
  "assertFailed": false,    // 断言未执行
  "value": 0
}
```

**AOT 和 JIT 行为完全一致**，说明不是平台差异。

---

## 20 个缺陷的完整清单

| # | si | 类型::方法 | 生成代码（genId） |
|---|----|-----------|-------------------|
| 1 | 706 | `BitConverter::ToDouble(Byte[], Int32)` | `ToDouble_24_System_Byte_int_3` |
| 2 | 850 | `Char::IsDigit(Char)` | `IsDigit_13_char_1` |
| 3 | 1135 | `Convert::ToBoolean(String)` | `ToBoolean_18_string_2` |
| 4 | 1357 | `Convert::ToInt32(String)` | `ToInt32_129_string_2` |
| 5 | 1569 | `Convert::ToDecimal(String)` | `ToDecimal_235_string_2` |
| 6 | 1775 | `DateTime::AddHours(Double)` | `AddHours_2_double_0` |
| 7 | 1776 | `DateTime::AddHours(Double)` | `AddHours_2_double_1` |
| 8 | 1781 | `DateTime::AddMinutes(Double)` | `AddMinutes_5_double_0` |
| 9 | 1782 | `DateTime::AddMinutes(Double)` | `AddMinutes_5_double_1` |
| 10 | 1812 | `DateTime::Parse(String)` | `Parse_22_string_1` |
| 11 | 1813 | `DateTime::Parse(String)` | `Parse_22_string_2` |
| 12 | 1970 | `Decimal::Ceiling(Decimal)` | `Ceiling_3_decimal_0` |
| 13 | 1971 | `Decimal::Ceiling(Decimal)` | `Ceiling_3_decimal_1` |
| 14 | 2348 | `Enum::Parse(Type, String)` | `Parse_12_System_Type_string_2` |
| 15 | 2496 | `GC::EndNoGCRegion()` | `EndNoGCRegion_29__0` |
| 16 | 3195 | `Math::Floor(Double)` | `Floor_11_double_0` |
| 17 | 3196 | `Math::Floor(Double)` | `Floor_11_double_1` |
| 18 | 3201 | `Math::Pow(Double, Double)` | `Pow_13_double_double_0` |
| 19 | 3205 | `Math::Sin(Double)` | `Sin_14_double_1` |
| 20 | 3211 | `Math::Sqrt(Double)` | `Sqrt_17_double_1` |

---

## 已知的根源线索

### 1. 所有 20 个有 codegen shape

能力清单确认它们都是 `SimpleForward` 注册（native 符号存在）。例如 `ChaosMathFloor`、`ChaosConvertToInt32`、`ChaosDateTimeAddHours` 等都在 `runtime_stubs/` 中有实现。

### 2. 异常不来自 ChaosExternalRuntimeFallback

`entry.exe --fact-json` 的 stderr 日志显示 **0 次 Phase 3 catch-all**。1407 次 ExternalRuntimeFallback 调用全部被 Phase 1（IL data）或 Phase 2（dispatch table）成功解析。

### 3. 异常被 runner 的 __except handler 吞掉

`caught=true` 意味着在 `NativeAotLoweringPlanner.MethodEmission.cs` 的 try/catch 中被捕获。异常类型和位置**不输出到 stderr**。

### 4. 已定位的生成代码

```cpp
// Math::Floor 的 AOT body（page-0038.cpp）
const auto chaos_result = ChaosMathFloor(ChaosLoadFloat64(chaos_arg_0));

// 生成的 C# 断言（CombinedSubjects.cs）
var result_11_0 = global::System.Math.Floor(default(double));
Assert.AreEqual(0d, result_11_0);
return BitConverter.DoubleToInt64Bits(result_11_0);
```

### 5. 模式归纳

- **大多数是 *_1, *_2, *_3 变体**（非 `_0`），即 ATG 多值集输入的**第 2/3/4 个值集**
- `default(double)` 输入在某些情况下导致浮点运算 NaN → 抛托管异常
- `BitConverter.DoubleToInt64Bits(result)` 对 NaN 可能产生未定义行为

---

## 推荐的调试方法

### 方法 A：注入 cdb 调试脚本

```bash
cdb -c "g; q" -o artifacts/.../entry.exe --fact-json
```

捕获 crash 位置的堆栈。需要 Windows SDK 的 Debugging Tools。

### 方法 B：修改 MethodEmission.cs 的 try/catch

在 `NativeAotLoweringPlanner.MethodEmission.cs` 的 catch 块中输出异常类型：

```csharp
catch (const chaos_managed_exception& ex) {
    // 临时: 输出异常信息到 stderr
    fprintf(stderr, "[ASSERT FAIL] caught chaos_managed_exception\n");
    throw;
}
```

然后重跑 `--fact-json`，stderr 会显示哪些方法抛了什么异常。

### 方法 C：逐个类型隔离调试

挑一个子系统（如 Math::Floor）创建最小复现用例。

---

## 关键文件

| 文件 | 作用 |
|------|------|
| `src/native/runtime-core/runtime_stubs/math_stubs.cpp` | `ChaosMathFloor` / `ChaosMathPow` / `ChaosMathSin` / `ChaosMathSqrt` |
| `src/native/runtime-core/runtime_stubs/convert_stubs.cpp` | `ChaosConvertToInt32` / `ChaosConvertToBoolean` / `ChaosConvertToDecimal` |
| `src/managed/Chaos.IL2CPP.Generator/Emission/NativeAotLoweringPlanner.MethodEmission.cs` | runner try/catch — 输出异常类型的位置 |
| `artifacts/.../system/native/subjects/native-aot.page-0038.cpp` | Math::Floor / Sqrt / Sin / Pow 的 AOT 生成代码 |
| `tests/e2e/translation/.../combined/CombinedSubjects.cs` | 生成的 C# 测试断言 |
| `artifacts/.../system/results/fact-results.json` | 运行时结果（`caught=true` 行） |

---

## 准备工作环境

```bash
# entry.exe 已含 Phase B（NotImplementedException）
# 只需修改 MethodEmission.cs 的 catch 块 + 重编：
cd /d/agent/chaos-il2cpp/tests/e2e
CHAOS_FOUNDATION_DLL=D:/agent/chaos-il2cpp/tests/e2e/translation \
  python -m verification.chunk_pipeline --chunk system --stages build,fact --smoke
```

---

## 分类映射

```python
# 在 fact_chunk.py classify_fact_record 中：
if _has_codegen_shape(manifest_keys, subject_id) and not rec.get("passed"):
    return "realDefect"  # ← 这 20 个就是这个分类
```

结构化匹配逻辑在 `_has_codegen_shape()`（`fact_chunk.py:425-449`），manifest 精度已经过验证：`metadata rows with shape: 316`，无假阳性。