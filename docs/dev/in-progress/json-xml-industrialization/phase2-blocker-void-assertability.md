# 🔴 Phase 2 阻塞：void 方法无法被断言 —— `real` 口径系统性失效

> 日期：2026-09-20
> 状态：**需要用户拍板**（影响 Phase 2 的目标设定）

## 1. 问题

Phase 2 的目标是「核心类型族 `realVerified > 60%`」。但**这个目标在当前口径下不可达**。

### 证据

矩阵里「缺口最大」的两个类型：

| 类型 | 总 | real | 缺口主因 |
|:-----|:--:|:----:|:---------|
| `System.Text.Json.Utf8JsonWriter` | 115 | **0** | `unassertable` **104** |
| `System.Xml.XmlTextWriter` | 56 | **0** | `unassertable` **50** |

**`unassertable` 不是"未实现"，是"void 返回，ATG 无法断言"。**

### 全量量化

`unassertable` 共 **213** 条，返回类型分布：

```
System.Void                    166
System.Threading.Tasks.Task     45
System.Threading.Tasks.ValueTask 2
```

**100% 是结构性无法断言的**（void 无返回值；async 返回 Task，value 无法承载断言）。

## 2. 根因

`real` 的判据是 **`value != 42`**（`fact_chunk.py:355`）：

```python
# passed=True 分支
"real"      : rec.get("value") != 42     # value != 42: genuine assertion value
"unassertable": return_type in {Void, Task, ValueTask}   # 42 是结构性的
```

**void 方法没有返回值** → `value` 恒为 42 → **永远无法成为 `real`**。

**对 JSON/XML 的致命性**：这两个库的**写面 API 绝大多数是 void**
（`WriteBoolean`/`WriteString`/`Close`/`Flush`/`WriteCData`…）。

**因此 `real 14.8%` 严重低估了实际实现程度** —— 它把"实现了但无法断言"和
"未实现"混为一谈。

## 3. 影响

| 受影响项 | 后果 |
|:---------|:-----|
| **Phase 2 出口判据** | 「核心族 realVerified > 60%」**不可达**（分母含大量 void） |
| **矩阵 14.8%** | 是**下限**，但下限低得多，失去参考价值 |
| **无法区分** | "void 方法已实现" vs "void 方法未实现" |

## 4. 候选方案

### 方案 A：为 void 方法建立独立判据（推荐）

void 方法无法用返回值断言，但**可以用副作用断言**：
- `Utf8JsonWriter`：写入后检查底层 buffer 内容
- `XmlTextWriter`：写入后检查输出字符串

**做法**：ATG 为 void 方法生成「调用 → 检查副作用」的断言。
**成本**：需改 ATG 生成逻辑，跨层。
**收益**：让 166+47 个方法变得可验证。

### 方案 B：接受现状，改用「实现覆盖」口径

新增一个「已实现」判据（区别于「已验证」）：
- `implemented` = 调用了 native 且未走 catch-all（不看返回值）

**做法**：从 `bodyAvailability` 或 native 侧调用记录判定。
**成本**：低。
**收益**：能区分"实现 vs 未实现"，但仍不验证正确性。

### 方案 C：缩小 Phase 2 范围

只对**有返回值**的核心 API 设 >60% 目标（如 `JsonDocument`/`JsonElement`/
`Utf8JsonReader`），void 方法留给后续。

**成本**：最低。
**收益**：目标可达，但回避了问题。

## 5. 我的建议

**先做方案 B（快，立即恢复可度量性），再评估方案 A（根本解决）。**

理由：
- 方案 B 成本低，能**立即**给出"实现了多少"的可信数字，让 Phase 2 有标尺
- 方案 A 是正确解法但跨层（ATG + codegen），应**独立立项**而非塞进 Phase 2
- 方案 C 是回避，不推荐（会让 void 方法永远无法验证）

## 6. 诚实标注

- 本问题是**执行 Phase 2 时发现的**，不在原 roadmap 的预判中
- `unassertable` 的 213 条**已在 fact 口径中被正确标记** ——
  问题不在 fact 层，而在**目标设定与实现验证手段**
- 若采用方案 A，需评估 ATG 生成「副作用断言」的可行性（未评估）
