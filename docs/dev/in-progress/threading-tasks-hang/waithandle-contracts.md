# WaitHandle 静态重载契约（.NET 8 / net10 双版本实测）

> 来源：反射枚举 + 实测异常，非推断。
> 用于 `waithandle-native-surface` 任务的 shape 注册与 native 实现。

## 一、权威签名（反射枚举 `WaitHandle` 公开静态方法）

```
WaitAll(WaitHandle[] waitHandles)
WaitAll(WaitHandle[] waitHandles, Int32 millisecondsTimeout)
WaitAll(WaitHandle[] waitHandles, TimeSpan timeout)
WaitAll(WaitHandle[] waitHandles, Int32 millisecondsTimeout, Boolean exitContext)
WaitAll(WaitHandle[] waitHandles, TimeSpan timeout, Boolean exitContext)

WaitAny(WaitHandle[] waitHandles)
WaitAny(WaitHandle[] waitHandles, Int32 millisecondsTimeout)
WaitAny(WaitHandle[] waitHandles, TimeSpan timeout)
WaitAny(WaitHandle[] waitHandles, Int32 millisecondsTimeout, Boolean exitContext)
WaitAny(WaitHandle[] waitHandles, TimeSpan timeout, Boolean exitContext)

SignalAndWait(WaitHandle toSignal, WaitHandle toWaitOn)
SignalAndWait(WaitHandle toSignal, WaitHandle toWaitOn, Int32 millisecondsTimeout, Boolean exitContext)
SignalAndWait(WaitHandle toSignal, WaitHandle toWaitOn, TimeSpan timeout, Boolean exitContext)
```

**注意**：单参 `WaitAll(WaitHandle[])` **确实存在**（不是探针臆造）。

## 二、参数校验契约（实测结果）

| 输入 | 结果 |
|---|---|
| `WaitAll(null!, 0)` | `ArgumentNullException` |
| `WaitAll(null!, TimeSpan.Zero)` | `ArgumentNullException` |
| `WaitAll(new WaitHandle[0], 0)` | `ArgumentException` |
| `WaitAny(null!, 0)` | `ArgumentNullException` |
| `WaitAny(new WaitHandle[0], 0)` | `ArgumentException` |
| `WaitAll(new WaitHandle[]{ev, null!}, 0)` | `ArgumentNullException`（**含 null 元素**） |

**.NET 8 与 net10 结果一致** —— 非版本相关。

## 三、关键设计洞察

**校验发生在等待之前，且所有重载校验逻辑一致。**

即：`WaitAll(arr, 100)` 与 `WaitAll(arr, 100, false)` 在 `arr == null`
时抛的是**同一个** `ArgumentNullException`，与该走哪个 timeout 无关。

⇒ **可以用一个统一的校验 native**，让所有重载转发：

```cpp
CHAOS_IL2CPP_INT32 chaos_wait_handle_wait_all_checked(INTPTR array)
{
    if (array == 0) RaiseArgumentNullException("waitHandles");
    if (ArrayLength(array) == 0) RaiseArgumentException("...");
    if (ContainsNullElement(array)) RaiseArgumentNullException("waitHandles");
    return 0;   // 校验通过：真实等待未实现，返回 0（非异常路径）
}
```

## 四、arity 约束（来自既有踩坑记录）

`RuntimeHelperShapeRegistry.CoreStubs.Part1.S16.cs:1805`：

> `DirectNativeSymbol` makes codegen call the native symbol with
> **the call site's OWN arguments**, so the **declared slot count must equal
> the CALLEE's arity** — not the count of parameters we happen to forward.
>
> Registering a 1-slot shim for `WaitAll(Task[], TimeSpan)` left the second
> operand on the evaluation stack; the generated body did `_s1 = &chaos_locals[1]`
> and passed THAT as the array handle → `STATUS_ACCESS_VIOLATION`.

**⇒ 2 参重载必须注册 2 槽 shim，3 参必须 3 槽。** 不能用一个统一 shim 覆盖所有 arity。

**但可以让所有 shim 都转发到同一个 1 参 native**（shim 负责丢弃 timeout 实参）：

```csharp
// 2 参 shim
"CHAOS_IL2CPP_INT32 symbol(INTPTR a0, INTPTR a1) { return chaos_wait_handle_wait_all_checked(a0); }"
// 3 参 shim
"CHAOS_IL2CPP_INT32 symbol(INTPTR a0, INTPTR a1, INTPTR a2) { return chaos_wait_handle_wait_all_checked(a0); }"
```

**这是唯一不违反 arity 规则又能复用校验逻辑的形态。**

## 五、范围与预期收益

| 方法 | 案例数 |
|---|---:|
| `WaitAll` | 21 |
| `WaitAny` | 21 |
| `SignalAndWait` | 5 |
| **合计** | **47** |

**不含**实例类 13 个（probe 对抽象类 `WaitHandle` 调 `Create<T>()`，
异常来自工厂而非被测方法 —— 见 `STATUS-waithandle-native.md` §三）。
