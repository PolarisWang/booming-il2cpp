# TryParse 残差交接 — F2 家族（2 参核心）待运行时定位

> **创建日期**: 2026-09-17
> **上游**: `docs/dev/in-progress/debug-20-real-defects/triage-305-failures.md`（D 组）
> **前置 commit**: `3361f6434`（TryParse 接线）+ `b9a82df25`（arity 参数顺序修复）

## 一、当前状态

数值 `TryParse` 已接线并修复参数顺序错位，**相对 clean baseline +108 fixed / −1 regressed**：

```
TryParse: pass=207  fail=43
```

剩余 43 个已精确分成两个家族：

| 家族 | 数量 | 期望分布 | 性质 |
|---|---|---|---|
| **F1** 3/4 参含 `NumberStyles` | 34 | 28 true / 6 false | 与并发 agent 的 `StringIsDigitsOnly` gate 交互；**多数语义已正确** |
| **F2** 2 参核心 | 9 | **全 true** | **我的代码的真缺陷 —— 本交接的目标** |

F2 成员（全部"应成功却抛异常"）：

```
DoubleTests::TryParse_14_string_double_3      "3.14159"
SingleTests::TryParse_14_string_float_3       "3.14"
HalfTests / DecimalTests 的同类 2 参重载
```

## 二、已排除的假设（逐项读生成代码验证，勿重复）

| 环节 | 证据 | 结论 |
|---|---|---|
| 调用形态 | `ChaosTryParseDouble(STRING_ID("3.14159"), &chaos_locals[0])` | ✅ 正确 |
| 2 参是否被 gate | gate 只加在 `*Styles`/`*StylesProvider` 上 | ✅ 未被 gate |
| out 槽写入语义 | `chaos_locals[0]` 初值 `ChaosStoreFloat64(0)`（位编码 double 槽），我的 native 写原始 double 位型 | ✅ 语义一致 |
| 输入合法性 | `"3.14159"` 经 `std::strtod` 可解析 | ✅ |
| native 是否链接 | `chaos_runtime_core.lib` 中 `ChaosTryParseInt32` 计数 20 | ✅ 已链接 |
| 参数顺序 | 三层（生成代码/shape 槽/native 签名）已核对为 `(str, styles, prov, out)` | ✅ 已修 |

## 三、关键规律（定位线索）

> **所有「期望 true」的路径全部抛异常（`caught=True`）；
> 所有「期望 false」的路径全部正常。**

原因：失败路径直接 `return 0`，**从不触碰 out 指针**；成功路径多两步 ——
`*out = v` 与 `return 1`。

**推论：异常发生在 out 指针写入附近。** 但三层读代码均显示写入语义正确，
说明问题在更底层（tag 约定 / 栈槽生命周期 / ABI 边界），需运行时证据。

## 四、建议的下一步（唯一未尝试的手段）

**运行时埋点**——在 `ChaosTryParseDouble` 成功分支加临时诊断：

```cpp
// parse_convert.cpp, ChaosTryParseDouble 成功分支
CHAOS_IL2CPP_LOG_WARN("TryParseDbg",
    "str=%s out=%p v=%f", NullTerminate(data,len), (void*)out, v);
*reinterpret_cast<CHAOS_IL2CPP_FLOAT64*>(static_cast<CHAOS_IL2CPP_INTPTR>(out)) = v;
CHAOS_IL2CPP_LOG_WARN("TryParseDbg", "write done");
```

观察：`out` 值是否合理（应为合法栈地址）、`write done` 是否打印。
- 若 `write done` 未打印 → 写入本身触发异常（地址非法/越界）
- 若打印了 → 异常在其后（返回路径或调用方的 out 读取）

`CHAOS_IL2CPP_LOG_WARN` 定义在 `src/native/common/chaos/log.h`；
`parse_convert.cpp` 通过 `runtime_core.h` 间接可见。

## 五、⚠️ 环境约束（重要）

1. **并发 agent 正在改同一文件**：`src/native/runtime-core/parse_convert.cpp`
   有未提交的 45 行改动（`StringIsDigitsOnly` gate）。**它语义正确，勿回退**。
   动此文件前先 `git status` 确认对方是否已提交，避免覆盖。

2. **`grep` 判据陷阱（本会话踩了两次）**：
   - `grep -c ChaosTryParseInt32` 返回 0 **不代表函数不存在** —— 宏展开对文本搜索不可见
   - `entry.exe` 里搜不到符号名也**不代表没链接** —— 最终 exe 符号被剥离，只有字符串字面量保留
   - **正确判据**：查 `.lib`（保留符号）或查独有的**字符串字面量**

3. **构建失败会静默复用陈旧产物**：`TPG generate-dll FAILED` 只打一行日志，
   fact 阶段照常读上一次的 `entry.exe`。改 codegen 后务必确认
   `entry.exe` mtime > 源码 mtime。

## 六、F1 家族的补充结论（供参考）

F1 里 28 个"期望 true"的用例，其输入是 `-100` / `3.14159` / `2024-01-15` / GUID /
`"A"` 等非纯数字串，且传 `default(NumberStyles)` = **None**。

**实测 .NET 确认 gate 语义正确**：
```
sbyte.TryParse("-100",   NumberStyles.None, ...) = False
double.TryParse("3.14159", NumberStyles.None, ...) = False
double.TryParse("3.14159")                        = True   ← 2 参默认非 None
```

即 F1 里相当一部分是 **ATG 对 `NumberStyles.None` 的期望本身可疑**，
值得先按 `nullArg`/`envSensitive` 的思路做一次分类复核，再决定是否修复。
剩余未接线类型：`DateTime` / `Guid` / `Decimal` / `TimeSpan` / `Int128` / `UInt128` /
`Char` / `Version`（这些需要各自的 native，不是 shape 能解决的）。
