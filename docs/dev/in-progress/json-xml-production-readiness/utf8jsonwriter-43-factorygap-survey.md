# Utf8JsonWriter 43 项 factoryGap — 根因普查（含两项被推翻的假设）

> 日期：2026-09-21
> worktree：`json-writer-overload-shapes`，基线 `055769a95`
> 全部结论均有一手证据；**过程中推翻了两条假设**（含我自己先写进任务书的那条）

## 0. 结论速览

43 项 factoryGap **不是一个缺陷，是两类**：

| 类 | 数量 | 机制 | 性质 |
|:---|:----:|:-----|:-----|
| **B 类** | **31** | shape 未注册该重载 → 落 0 参 catch-all（`return 0` 不抛）→ `throw "AOT stub did not throw"` | **缺实现**（需新 native 符号 + 注册） |
| **C 类** | **12** | shape 已注册、native 已被调用，但**抛出的异常类型与 .NET 不符** | **真 native 缺陷** |

**A 类（仅缺注册、native 已有符号）= 0。**

## 1. ❌ 被推翻的假设 1：「43 项全部落 0 参 catch-all」

**我在任务书里就是这么写的 —— 错的。** 只有 31 项如此。

**验证方式**（`verify_split.py`，按生成体逐函数分类）：

```
total factoryGap subjects        : 43
  -> call REAL native symbol     : 12
  -> call 0-arg catch-all        : 31
  -> call both                   : 0
```

**教训**：`resultKind == "factoryGap"` **不能**反推调用形态。
必须读生成体的实际调用点。

## 2. ❌ 被推翻的假设 2：「ATG 探针记错了异常类型」

发现 12 项的 C# 期望是 `NullReferenceException`，而 native 抛 `ObjectDisposedException`，
**一度怀疑是 ATG probe 记录有误**。用 .NET 实测证伪。

**验证方式**（`netcheck/Program.cs`，net10.0 真跑，裸对象 `GetUninitializedObject`）：

```
bare.WriteNullValue()                 -> System.NullReferenceException
bare.WriteBooleanValue(true)          -> System.NullReferenceException
bare.WriteNumberValue(1)              -> System.NullReferenceException
bare.WriteNumberValue(1m)             -> System.NullReferenceException
bare.WriteStringValue("s")            -> System.NullReferenceException
bare.WriteStringValue(Guid.Empty)     -> System.NullReferenceException
bare.WriteStartArray(JsonEncodedText) -> System.InvalidOperationException
bare.WriteStartArray("x")             -> System.InvalidOperationException
bare.WriteStartObject()               -> System.InvalidOperationException
bare.Flush()                          -> System.ObjectDisposedException
bare.Reset(default(Stream))           -> System.ObjectDisposedException
bare.WriteString("", "")              -> System.InvalidOperationException
bare.WriteString(null, null)          -> System.ArgumentNullException
```

**结论：ATG 期望 12/12 全对；native 12/12 全错。**

对照表（`compare_12.py`）：

```
subject                            ATG expects                    .NET truth                     native raises                  verdict
WriteBooleanValue_52_bool_0        System.NullReferenceException  System.NullReferenceException  System.ObjectDisposedException NATIVE-WRONG
WriteNullValue_51__0               System.NullReferenceException  System.NullReferenceException  System.ObjectDisposedException NATIVE-WRONG
WriteNumberValue_47_decimal_0      ...                            ...                            System.ObjectDisposedException NATIVE-WRONG
WriteNumberValue_48_double_0       ...                            ...                            System.ObjectDisposedException NATIVE-WRONG
WriteNumberValue_49_float_0        ...                            ...                            System.ObjectDisposedException NATIVE-WRONG
WriteNumberValue_56_long_0         ...                            ...                            System.ObjectDisposedException NATIVE-WRONG
WriteNumberValue_60_ulong_0        ...                            ...                            System.ObjectDisposedException NATIVE-WRONG
WriteStringValue_45_DateTime_0     ...                            ...                            System.ObjectDisposedException NATIVE-WRONG
WriteStringValue_46_DateTimeOffset ...                            ...                            System.ObjectDisposedException NATIVE-WRONG
WriteStringValue_50_Guid_0         ...                            ...                            System.ObjectDisposedException NATIVE-WRONG
WriteStringValue_58_string_0       ...                            ...                            System.ObjectDisposedException NATIVE-WRONG
WriteString_38_string_string_3     System.InvalidOperationException System.InvalidOperationException System.ArgumentException     NATIVE-WRONG
```

### C 类的两个真缺陷模式

1. **值语义方法全用了 `RaiseDisposedOrInvalid()`（→ ObjectDisposedException），
   而 .NET 对裸对象的值写入方法抛 `NullReferenceException`**（11 项）。
   根因在 `json_writer_stubs.cpp`：这些函数一律调 `CheckThis()` + `RaiseDisposedOrInvalid()`。
2. **参数校验先于 disposed 校验**（1 项）。
   `ChaosUtf8JsonWriterWriteStringStr` 先查 `n_len == 0` → 抛 `ArgumentException`，
   但 fixture 传的是 `WriteString("", "")`（空串非 null），
   .NET 实测抛 `InvalidOperationException`。**这是注释里已描述过的顺序坑的又一实例**
   （`Reset` 系曾修过同类问题）。

## 3. B 类（31 项）的真正缺口

shape 键 = `TypeDisplayName::Method(Param1,Param2,...)` **精确字符串匹配**
（`RuntimeHelperShapeRegistry.cs:88-110`，FNV-1a 哈希）。
**无放宽、无强制转换、无最佳匹配** —— 参数列表只要不是逐字符相同就落 catch-all。

需要的重载大致三类：
- `arg0 = JsonEncodedText` 族（~13 键）：native 只有 `string` arg0 版本
- 标量宽度缺失：`WriteNumber(…,Int32)` / `WriteNumberValue(Int32)` / `WriteNumberValue(UInt32)` / `WriteNumber(…,UInt32)` **任何元数都没有**
- `decimal` 的 property-name 形式、`WriteRawValue(…,bool)` 的第二个 bool 参数

**注意 C2660 坑**：多参重载必须用**独立 native 符号名**
（如 `WriteNumberStr` 已是 `(INTPTR,INTPTR,INT64)`，`WriteNumber(string,int)` 不能复用）。

### 关于 `JsonEncodedText` 是 struct（我原先担心的那条）

**不是阻断点**。证据：兄弟重载对

```
WriteNumberValue_56_long_0  -> ChaosUtf8JsonWriterWriteNumberValueInt(...)  ✅ 工作
WriteNumberValue_55_int_0   -> 0 参 catch-all                                ❌ 失败
```

两者**都不含 JsonEncodedText**，唯一差别是**标量宽度**。
`JsonEncodedText` 在所有注册里统一映射到 `objAbi`（`NativeInt` 载体），
包括那些工作正常的注册，故 struct 传递本身没问题。

## 4. 修复量估计

| 类 | 工作 |
|:---|:-----|
| **C 类（12）** | **小** —— 改 `json_writer_stubs.cpp` 里值语义方法的抛出类型 + 调 `WriteStringStr` 的校验顺序。**无需新符号、无需新注册**，改完即应转绿 |
| **B 类（31）** | **大** —— ~20-24 个新 native 导出（`.cpp` + `.h`）+ ~28 个 S21 注册（每个需独立 ABI 槽列表）+ Generator→TPG 重建（TPG 捆绑陈旧 Generator.dll） |

**建议顺序：先做 C 类**（改动小、可立即用同一套指标验证），再做 B 类。

## 5. 诚实标注

- 本文所有数字均来自生成体/源码/.NET 实测的**一手读取**，非推断
- `compare_12.py` 的第一次运行因正则漏配产出全 `?`，我**没有**把解析失败当结论，
  修正提取后重跑才得到上表（过程已记录，避免重蹈）
- B 类的「31 项 / 28 个不同 shape 键」按普查 agent 统计，
  我核对了「31」这个总数（`verify_split.py`），**未逐一核对 28 这个细分**
- §1/§2 初次普查读的是**主检出**的产物（worktree 的 `artifacts/` 是 gitignored）。
  其 `provenance.json` 的 `gitCommit` 在本仓库历史中查不到 ——
  **那次构建来源不可审计**；但 shape 注册集已与 `git show HEAD:…S21.cs` 核对一致。
  §6 的修复验证是**在 worktree 内重跑**的（自带 SDK + 自带 artifacts），
  产物时间戳与运行同刻，**不存在并发污染**

---

## 6. C 类修复落地与验证（2026-09-21）

### 改动（`json_writer_stubs.cpp`，单文件）

引入 `enum class BareWriterKind { Disposed, NullReference, InvalidOperation }`，
`CheckThis(this_ptr, kind)` 按族选类型；新增
`RaiseBareWriterNullReference()` / `RaiseBareWriterInvalidOperation()`。

**关键发现**：`RaiseDisposedOrInvalid()`（ObjectDisposedException）被
**三个族共用**，但 .NET 实测三者各不相同：

| 族 | .NET 实抛 | 原 native 抛 |
|:---|:---------|:------------|
| 值语义写（`WriteNullValue`/`WriteBooleanValue`/`WriteNumberValue`/`WriteStringValue`/`WriteCommentValue`/`WriteRawValue`） | **NullReferenceException** | ❌ ObjectDisposedException |
| 属性名写（`WriteNull`/`WriteNumber`/`WriteBoolean`/`WriteString`/`WritePropertyName`） | **InvalidOperationException** | ❌ ObjectDisposedException |
| `Flush` / `Reset` / `JsonDocument.WriteTo` | **ObjectDisposedException** | ✅ 正确 |

另修 `WriteStringStr` 的**校验顺序**：空属性名检查原先先于接收器检查，
但 .NET 实测 `WriteString("", null)` → `InvalidOperationException`（非 ArgumentException）。

**未改** `WriteRawValue` 的空串 → `ArgumentException`（.NET 实测确实如此，原实现正确）。

### 验证（worktree 内 build+fact 实测，产物时间戳 15:48 与运行同刻）

> ## 🔴 下表已被重测推翻（2026-09-22 加注）
>
> **下表每一列都与 2026-09-22 的一手重测对不上，请勿再引用。**
>
> 重测方式：在 `worktree-json-writer-review-fixes` 内跑
> `python -u -m verification.chunk_pipeline --assembly System.Text.Json --chunk text-json --stages build,fact`
> （产物根 `artifacts/foundation-dll/System.Text.Json/chunks/text-json/results/fact-results.json`，
> 247 项，AOT 侧 resultKind 分布）：
>
> | metric | 2026-09-22 重测 | 下表「修复后」 | 下表「修复前」 |
> |:-------|:---------------:|:-------------:|:-------------:|
> | **factoryGap** | **0** | 31 | 62 |
> | stubGap | **177** | 176 | 176 |
> | unassertable | **65** | 34 | 3 |
> | real | **2** | 2 | 2 |
> | failed | **3** | 3 | 3 |
>
> **最致命的矛盾**：下表用**同一组 62/31 数字**同时声称
> `factoryGap −31` 与 `preAssertionRaise −31 / unassertable +31`。
> 但 `preAssertionRaise` 转入参 raise 的那批**测不到 catch 类型**，
> 不可能与 `factoryGap −31` 共用同一个计数。重测里根本不存在的「31」，
> 说明这两列至少有一列是**把中间态误记成了本 commit 的 delta**。
>
> **本表应视为不可复现，不得作为 C 类修复效果的证据。**
> §6 的结论（异常类型按族区分是真的、`WriteString` 校验顺序已修）
> 由 `941436a26` 的源码本身与 .NET 实测支撑，**不依赖这张表**，故仍成立。

| metric | 修复前 | 修复后 | delta |
|:-------|:------:|:------:|:-----:|
| **passed** | 181 | **212** | **+31** |
| **factoryGap** | 62 | **31** | **−31** |
| preAssertionRaise | 65 | 34 | −31 |
| unassertable | 3 | 34 | +31 |
| realVerified | 2 | 2 | 0 |
| stubGap | 176 | 176 | **0（无回归）** |
| failed | 3 | 3 | **0（无回归）** |

**12 项 C 类逐一核对**：`factoryGap` 残留 **0/12**；
全部转为 `passed=True, caught=False, kind=unassertable`。

> ⚠️ 同前：这些是 **void** 方法，`value=42` 是结构性 sentinel，
> 故落 `unassertable` 而非 `real`。**`caught=False` 是关键证据** ——
> 说明 typed catch 按序正确吸收了异常，异常语义现在是对的。

> 🔴 **上一句的证据效力已被修正（2026-09-22）**：`caught` 标志在 harness 里
> **没有写进 schema 的语义定义**，所以"`caught=False` = 被按序正确吸收"
> 是**一种读法**，不是从字段推出的事实 —— 它与"根本没被捕捉"不可区分。
> 这与本文 §1 自己总结的教训（`resultKind` 不能反推调用形态）**是同一个错误**：
> 字段名不可反推语义。
>
> `caught=False` 真正能支持的是**弱断言**：异常没有从测试体里逃逸到 harness。
> 要坐实"类型正确"，需要一个能读到**实际抛出异常类型**的判据
> （例如 Fixture 侧记录 catch 到的类型），当前计数里没有这一项。
> 结论仍可能是对的，但**本段的证据链不成立**。

**剩余 31 项** = B 类（shape 未注册该重载），与本次修复无关。
