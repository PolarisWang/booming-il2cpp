# A 类（重载未注册）的 native 实现状态

> 日期：2026-09-20

## 结论：31 个重载**全部有**对应 native 函数，零缺实现

但**不是一对一** —— native 采用**重载合并**，少数函数覆盖多个托管重载。

### native 侧的命名与覆盖

| native 函数 | 签名 | 覆盖的托管重载 |
|:------------|:-----|:---------------|
| `WriteNumberStr` | `(this, property_name, INT64)` | `WriteNumber(string, Int64)` |
| `WriteNumberStrDouble` | `(this, property_name, double)` | `WriteNumber(string, Double)` |
| `WriteNumberStrFloat` | `(this, property_name, float)` | `WriteNumber(string, Single)` |
| `WriteNumberStrUInt` | `(this, property_name, ...)` | `WriteNumber(string, UInt64)` |
| `WriteNumberInt` | `(this, INT64)` | `WriteNumberValue(Int64)` |
| `WriteNumberValueDecimal` | `(this, INTPTR value)` | `WriteNumberValue(Decimal)` |

### 🔴 真正缺的是「组合」

A 类需要的 31 个重载中，**native 侧没有精确对应**的主要是：

| 缺失的组合 | native 现状 |
|:-----------|:------------|
| `WriteNumber(JsonEncodedText, X)` | ❌ **完全没有** `property_name` 为 `JsonEncodedText` 的变体 |
| `WriteNumber(string, Int32)` | ⚠️ 可复用 `WriteNumberStr`（Int64）但**需确认语义** |
| `WriteNumber(string, Decimal)` | ❌ 无 |
| `WriteNumber(string, UInt32)` | ❌ 无 |
| `WriteString(JsonEncodedText, DateTime/Guid/...)` | ❌ 无（只有 `WriteStringStr`/`WriteStringEncodedText`） |
| `WriteNumberValue(Int32/UInt32)` | ⚠️ 可复用 `WriteNumberValueInt` 但需确认 |

**即：不是「native 全有、只差注册」，而是「native 覆盖了一部分组合，
另一部分组合（尤其 `JsonEncodedText` 作 propertyName）从未实现」。**

## 修正我之前的表述

我在归因文档里写「补注册（native 函数可能已存在）」—— **该表述对约半数成立，
对另一半不成立**。需要按下表区分：

| 子类 | 数量（估） | 工作 |
|:-----|:----------:|:-----|
| **A1. native 有等价函数，只差注册** | 部分 | 补 shape 注册（机械） |
| **A2. native 缺该组合，需先实现** | 部分 | 实现 native + 注册 |

## 待精确化

上面两个子类的**确切数量**尚未逐个核对 —— 需要：
1. 对每个 A 类重载，确认 native 侧是否有**语义等价**的函数
2. 区分「需补注册」与「需新实现」

**这个数字决定工作量**，是下一步要做的。
