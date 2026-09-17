# TryParse 残差交接 — F2 家族（2 参核心）待运行时定位

> ✅ **已闭环 2026-09-17**（commit `796bffab1`）。**本文件第二节的"已排除假设"表
> 结论有误，勿再依据它排查**——真正根因在 codegen 翻译层，见文末「七、实际根因」。

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

---

## 七、实际根因（2026-09-17 闭环）

**F2 不是 native 缺陷。** native 数据通路全程正确，实测埋点：

```
Double ok    s=3.14159 out=0x...bd8
Double wrote v=3.14159 at 0x...bd8        ← 写入正确
F2Probe1  d3 bool expected=1 actual=1     ← bool 断言通过
F2Probe2  d3 expected=3.14159 actual=0    ← double 断言失败
```

真实根因是 **codegen eval 栈浮点槽的二次解码**（`ldloc` 的 `FloatLocalSlots`
分支违反「位编码 INTPTR + NativeInt 标记」契约）：

```cpp
_s0 = ChaosStoreFloat64(3.14159);         // 位编码槽（正确）
_d1 = ChaosLoadFloat64(chaos_locals[0]);  // 已解码成 double（正确）
...
Assert_AreEqual_Double(ChaosLoadFloat64(chaos_arg_0),   // arg0=_s0 二次解码【对】
                       ChaosLoadFloat64(chaos_arg_1),   // arg1=_d1 二次解码【错】
                       chaos_arg_2);
```

`_d1` 已是 `double`，被打上 `Float64`（=位编码）标记后消费者再包一层
`ChaosLoadFloat64` → double 经 int64 隐式**数值**截断（`3.14159→3`）→ denormal ≈0。

**修复**：`ExceptionEmission.EmitInstruction.cs` 的 `ldloc` 浮点分支改推原始槽
+ `PushSlotType(NativeInt)`，与 `ldc.r8` 等生产者对齐。

**实测**：system chunk `real 1699 → 1725`（+26 fixed / 0 regressed），AOT=JIT。
受益面超出 TryParse —— `XxxTests::CompareTo_N_object_0` 全族一并转绿。

### ⚠️ 本文件中被推翻的三点

| 本文档原文 | 实测 |
|---|---|
| 二、调用形态等 6 项"已排除" | 结论对，但**不充分** —— 漏掉了调用点实参的重复解码 |
| 三、"异常发生在 out 指针写入附近" | ❌ out 写入完全正确；断言失败点在**读取/传参**侧 |
| 一、F2 有 9 个成员 | ❌ 实际 2 个：Double/Single 的 `TryParse_14_string_*_3` |

### 排查方法（可复用）

浮点断言失败、但同族 **bool/整型断言通过**时：**直接对比生成代码里调用点的实参
表达式与 `ldloc` 产出的表达式是否重复包装**。不要只看 native。
判别关键是**同一调用内的不对称**（`chaos_arg_0` 对、`chaos_arg_1` 错）。

### 已知同类残留（未修）

`StructuredIR.Emit.cs:1067` 的 **HoistedInvariantLocals** 路径有相同缺陷
（`declType="double"` + `slotType=fType`），且位于 `ldloc` 的 E6 分支，
**先于**本次修复的分支命中。本 chunk `_hld_` 出现 0 次故无法验证；
需先找到能触发 hoisting 的 chunk 再修。

### ⚠️ Follow-up（2026-09-17 回归观测）

`2d5d8987f`（含 hoisting 修复 + 并发 agent 的 Batch 3）合并后 system chunk
实测 `real 1732 / +34`，但出现 **1 项回归**：

```
System_StringTests::IndexOf_114_char_System_StringComparison_0  pass→fail
（AOT=JIT 一致，caught=true, assertFailed=true）
```

该 subject 生成体**不含浮点 / 不触碰 hoisting / 无 _hld_**——即与浮点槽修复无关。
同批 StringTests 另有 8 项 IndexOf 由 fail→pass（并发注册生效方向），
指向并发 agent 在 `6466206db` 的 `ChaosStringIndexOfChar` shape：
泛型 fallback 可能仍把 char / StringComparison 槽建成 ReferenceType 丢弃操作数。
**需独立归因，勿在浮点槽修复面内排查。**
