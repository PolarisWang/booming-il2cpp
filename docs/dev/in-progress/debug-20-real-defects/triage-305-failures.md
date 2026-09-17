# system chunk 失败归因表 — 305 个 `failed` 的分级

> **生成日期**: 2026-09-17
> **数据源**: `artifacts/foundation-dll/System.Private.CoreLib/chunks/system/results/fact-results.json`
> **基线**: `real=1872 / failed=305 / stubGap=1036 / realDefect=0 / nullArg=2 / envSensitive=4`
> **上游**: 本轮已闭环 `realDefect`（Math 族 + UInt32 carrier），本文回答"接下来修什么"

## 一、为什么需要这张表

305 个 `failed` 此前是一锅粥。本轮最重要的教训（见 `BitDecrement` 假绿）是：
**不先区分真假信号就动手修，会把假绿当真绿、把真缺陷当回归**。
本表把 305 条按**根因家族**分层，使修复顺序可判定。

## 二、总量拆分

| 失败形态 | 记录数 | 方法数 | 含义 |
|---|---|---|---|
| 断言执行并失败 (`assertFailed`) | 259 | 179 | 断言跑了、值不符 —— **真语义失败** |
| 断言前抛异常 (`caught` only) | 46 | 31 | 未到断言就抛 —— 多为无效输入 |
| **合计** | **305** | **210** | |

## 三、按根因家族分级

| 组 | 家族 | 方法数 | 根因 | 优先级 |
|---|---|---|---|---|
| **A** | INumber 标量谓词 | 33 | **无 shape 注册** → 0 参 catch-all | 🔴 高 |
| **B** | BitOperations 标量 | 16 | **无 shape 注册** → 0 参 catch-all | 🔴 高 |
| **C** | 字节序列化 (`GetBytes` 等) | 15 | 待确认 | 🟡 中 |
| **D** | Parse/TryParse 族 | 51 | 混合（需拆） | 🟡 中 |
| **E** | 其它 | 64 | 逐个判定 | ⚪ 低 |

### A. INumber 标量谓词（33 个方法，最高优先）

`System.Double.IsFinite` / `System.UInt32.IsEvenInteger` / `IsPositive` 等。

**已验证根因**：`RuntimeHelperShapeRegistry.CoreStubs.Part2.S3.cs:986-988` 的
`floatPredicates` 注册**只覆盖 `Vector*` 类型**（`Vector128.IsFinite` 等），
标量 `System.Double::` / `System.UInt32::` 的同名成员**没有任何 shape**。

```csharp
// Part2.S3.cs:986 —— 只注册到 Vector* 前缀
var floatPredicates = new[] { "IsNaN", "IsInfinity", "IsFinite", ... };
foreach (var methodName in floatPredicates)
    RegisterVectorUnaryOp(methodName, $"VectorFixed{methodName}");
```

**证据**：
- `IsFinite_0_double_0()` 断言 `Double.IsFinite(0.0) == true`，实测 `assertFailed`
- 与已修复的 Math 族（`ChaosMathExp` 等）**完全同根因** —— 有语义、无 shape → catch-all 返回 0

**修复路径**：为标量 `System.Double/Single/Half/UInt32/...` 补谓词注册 + native 实现
（多数是单行 `std::isfinite` / `(v % 2) == 0`）。**预计收益 ~33 方法**。

### B. BitOperations 标量（16 个方法）

`System.UInt32.LeadingZeroCount` / `TrailingZeroCount` 等 INumber 静态成员。

**已验证**：`Part2.S3.cs:38+` 注册的是 **`System.Numerics.BitOperations::LeadingZeroCount`**，
而失败的是 **`System.UInt32::LeadingZeroCount`**（INumber 静态抽象成员）—— **类型不同，不命中**。

注意 native 实现**已存在**（`ChaosBitOpsLeadingZeroCount32`），只需补标量类型的 shape 注册。

**预计收益 ~16 方法**，改动量小。

### C. 字节序列化（15 个方法）

`GetBytes` / `ToByteArray` / `ToHexString` 族。**未验证根因**，需先查是否同类 shape 缺失。

### D. Parse/TryParse 族（51 个方法）

混合：部分可能是无效输入（→ nullArg 同族），部分是真实缺陷。
**需先按参数形态拆分**（空串 / `default` / 有效字面量），不能整族处理。

### E. 其它（64 个方法）

主要为 `CompareTo`(25) / `Combine`(14) / `Compare`(10) / `Deconstruct`(4) 等。

**已抽查**：`CompareTo(default(object)!)` 断言 `1` —— 实测 .NET `char/int.CompareTo(null)`
**确实返回 1**，故这些是**真缺陷**而非坏期望，值得单独排查（`CompareTo(null)` 路径）。

### 断言前抛异常（31 个方法）

| 类 | 记录数 | 备注 |
|---|---|---|
| `System_ActivatorTests` | 21 | 多为 `CreateInstance(null,...)` → ArgumentNullException（正确行为），
应归 `nullArg`（现有检测器可能因参数形态未命中） |
| `System_ConvertTests` | 20 | `ToDecimal(object)` 等，需查是否同类 |
| `System_MathTests` | 4 | 待查 |
| `System_StringTests` | 1 | 待查 |

**这部分很可能是分类遗漏而非真缺陷** —— 先复核归位（零风险），再谈修复。

## 四、建议执行顺序

1. **先归位，后修复**：复核 31 个"断言前抛异常"和 D 组中无效输入的子集，
   把属于 `nullArg` 的归位（像本轮 `Enum` 空串那样）。**零风险，立即提升信号纯度。**
2. **A + B 组一起修**（~49 方法）：同根因（无 shape）、native 多已存在、改动集中。
   这是**单点收益最大**的一块。
3. **C 组**：先验证根因再决定。
4. **E 组 `CompareTo`**：已确认是真缺陷，但需单独调查 `CompareTo(null)` 路径。
5. **D 组**：最后做，需手工拆分。

## 五、配套的基础设施加固（建议同步）

本轮暴露、**尚未修复**的工具链缺陷：

> **build 失败时 pipeline 仍继续跑 fact，产出陈旧数字**
>
> `TPG generate-dll FAILED` 只打一行日志，fact 阶段照常执行并读取**上一次**的
> `entry.exe`。本轮多次出现"改了没生效/莫名回归"的误判（如重复 shape 注册
> `LogP1` 中止整个 codegen）。虽有 `_invalidate_stale_results` 作废旧结果文件，
> 但 stage 状态仍是 `failed` 而非硬失败，数字依然被消费。

建议：build 失败时应让 fact 阶段**不可执行**（而非仅作废文件），
使"构建坏了"与"事实是坏的"在输出上不可混淆。这与本轮已修复的
SDK 哈希盲区（[[float64-eval-stack-double-encode]] 同族）属同一类
"静默陈旧产物"问题。
