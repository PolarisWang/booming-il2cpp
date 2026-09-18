# 显式泛型实例化 callee 的 method 名带 `<T>` → shape 永不命中

> **日期**: 2026-09-18
> **发现路径**: threading chunk `CompareExchange_7_int_int_int` 调查
> **性质**: **系统性**（单 chunk 38 处），非单方法缺陷

## 一、机制

`TryMatchShape`（`RuntimeHelperShapeRegistry.cs:464`）用
`GetMethodNameFromSubjectId` 解析方法名，该函数**原样截取 `::` 与 `:` 之间的部分**：

```
callee: System.Threading/Interlocked::CompareExchange<System.Int32>:System.Int32(...)
                                          ^^^^^^^^^^^^^^^^^^^^^^^^^^^
解析结果: "CompareExchange<System.Int32>"
注册键:   "CompareExchange"
```

→ `BuildCanonicalKey` 得到 `Interlocked::CompareExchange<System.Int32>(...)`
→ 与注册的 `Interlocked::CompareExchange(...)` **hash 不等** → **永不命中** → 落 catch-all。

## 二、实测证据（非推断）

同一 chunk 内，**只有 Int32 失败，其余全通**：

| 方法 | 结果 |
|---|---|
| `CompareExchange(sbyte|short|byte|ushort|uint|ulong|float|double)` ×2 | ✅ `real` |
| `CompareExchange(long,long,long)` ×2 | ✅ `real` |
| `CompareExchange(object,object,object)` | ✅ `real` |
| **`CompareExchange(int,int,int)` ×2** | ❌ **`failed`** |

生成物对照（两条 wrapper 并存）：

```cpp
// 非泛型形态 —— 带参、真转发
extern "C" CHAOS_IL2CPP_INT32
chaos_external_runtime_..._CompareExchange_System_Int32_System_Int32__System_Int32_System_Int32_(
    INTPTR a0, INT32 a1, INT32 a2) noexcept
{ return ChaosInterlockedCompareExchangeInt32(a0, a1, a2); }

// 泛型形态 —— 零参、catch-all、返回 0
extern "C" CHAOS_IL2CPP_INTPTR
chaos_external_runtime_..._CompareExchange_System_Int32__System_Int32_..._() noexcept
{ ... "no native body" ... }
```

## 三、影响面（单 chunk 38 处）

| 类型 | 方法 |
|---|---|
| `SubjectInstanceFactory` | `Create<T>`（16 种 T） |
| `DefaultInterpolatedStringHandler` | `AppendFormatted<T>`（13 种 T） |
| `ThreadPool` | `QueueUserWorkItem<T>`、`UnsafeQueueUserWorkItem<T>` |
| `Interlocked` | `CompareExchange<T>`、`Exchange<T>` |
| `LazyInitializer` | `EnsureInitialized<T>` |
| `Volatile` | `Read<T>`、`Write<T>` |

**注意**：`SubjectInstanceFactory::Create<T>` 正是并发会话当前在修的
L2 线（`e9e1612ce` 等）—— 说明这条线同时从两侧被触碰。

## 四、修法选项

| 方案 | 做法 | 评估 |
|---|---|---|
| **A. 解析时剥离 `<...>`** | `GetMethodNameFromSubjectId` 返回前截断 `<` | **改动最小、影响最广**；但会波及所有依赖该函数的调用方，需评估 |
| **B. 泛型 descriptor** | 为每个类型新增 `RegisterGeneric` | 精确但重复；6 个类型各写一份 |
| **C. 不动匹配层** | 在各处补具体注册 | ✗ 治标不治本，新增泛型仍会漏 |

**倾向 A**，但必须先在 registry 层验证：剥离后
`CompareExchange<System.Int32>` 与 `CompareExchange<System.Int64>` 会
**坍缩到同一个 key** → 若两者 carrier 不同（Int32 vs Int64）则**错配**。

⚠️ **这正是必须验证的点**：`CompareExchange<System.Int32>` 与
`<System.Int64>` 的注册若都用 `System.Int32&/Int32/Int32` 是不合理的 ——
泛型实例的类型实参必须参与 key，否则会重演
「`Create<T>` 全部坍缩到 canonical 体」那一类错误
（见 memory: `create-t-sharing-failure-mechanism`）。

**因此正确修法是 A 的变体**：剥离 `<...>` 得到方法名，
**但同时把类型实参并入参数列表或 key**，使不同实例化可区分。

## 五、本轮未做

**未实施修复**，原因：
1. 影响面横跨 6 个类型族、与并发会话在修的 `Create<T>` 线重叠
2. 修法 A 需先验证「类型实参如何参与 key」这一设计问题
3. 属 registry 匹配层的**架构性改动**，不适合在「清残余 failed」的
   收尾阶段顺手改

**建议**：单独立项。判据与先例同族 ——
`discriminator-lookup-key-never-matched`、`Utf8JsonWriter 裸类型名`、
跨程序集泛型 `DefinitionSubjectId`。**这是第四次「查表键格式不符 → 静默落空」。**
