# P-1 修复方案深度分析（以「C# 语义一致」为准绳）

> 日期：2026-09-14 ｜ 状态：待用户选择
> 需求：语义上完全正确，尽量与 C# 保持一致的语法表现

---

## 1. 先确立 C# 的基准语义

### 1.1 断言失败在 C# 中如何表现

`Assert.Fail`（`src/reference/Chaos.TestFramework.Sdk/Assert.cs:250`）：

```csharp
[Conditional("VERIFY")]
public static void Fail(string message)
{
    s_exitCode = 1;
    System.Console.Error.WriteLine($"[ASSERT FAIL] {message}");
    throw new AssertionException(message);      // ← 抛出
}
```

且 `AssertionException : System.Exception`（同文件 :5）。

**C# 语义**：断言失败 = 抛出一个普通托管异常。
在**没有 try/catch 的方法**中，它应当**向外传播**（这正是 xUnit 等框架判定失败的机制）。

### 1.2 当前生成代码背离了这一语义

`MethodEmission.cs:460`：

```csharp
bool _wrapInTryCatch = _isSubjectMethod && method.ExceptionRegionCount == 0 && ...;
```

→ **无 EH 区域的方法**被整个包进 try/catch，而 catch 的语义是：

```cpp
} catch (const chaos_managed_exception&) {
    return {};                    // ← 把异常转成"返回默认值"
}
```

**这与 C# 不一致**：C# 中该异常会传播，这里被吞掉并返回 0。

### 1.3 现状本身也是自相矛盾的

| 方法类型 | 未捕获异常的去向 | 与 C# 是否一致 |
|---|---|---|
| **有 EH 区域**（`ExceptionRegionCount > 0`） | 传播到 runner 的 SEH | ✅ 一致 |
| **无 EH 区域**（被 `_wrapInTryCatch` 命中） | 被方法自己的 catch 吞掉 → return 0 | ❌ 不一致 |

**同一语义在两类方法上行为不同** —— 这本身就是缺陷，与断言无关。

### 1.4 `_wrapInTryCatch` 的真实目的（源码注释 :450-455）

> Phase 4: The try/catch below also guards against calls to unregistered external
> runtime symbols (`chaos_external_runtime_*`). If the symbol is not in
> `_externalRuntimeSubjects` or the `kChaosExternalRuntimeFnTable`, the fallback
> throws a C++ exception — caught here, returning default.
> **Wrap subject methods w/o EH regions in try/catch to prevent C++ exceptions from
> propagating to the fact-json `__except` handler.**

**关键**：它要防的是 **C++ 异常**（未注册符号的 fallback），
却用了 `catch (const chaos_managed_exception&)` —— **托管异常的 catch 子句**。

于是产生语义混淆：
- 想防的：C++ 侧 fallback 异常
- 实际还躲不掉地防了：托管断言异常（同样是 `chaos_managed_exception`）

---

## 2. 方案（按「C# 一致性」排序）

### 方案 D：按异常种类分离 catch —— **最贴合 C# 语义**（推荐）

**做法**：把"托管异常传播"与"C++ fallback 兜底"分成两个不同种类的异常。

```cpp
try {
    ... 方法体 ...
}
catch (const chaos_managed_exception&) {
    throw;                      // ← 托管异常按 C# 语义向外传播，不吞
}
catch (const chaos_external_fallback_exception&) {
    return {};                  // ← 仅兜底未注册符号的 C++ 异常
}
```

并把 `chaos_external_runtime_*` 未注册符号的 fallback 改为抛
`chaos_external_fallback_exception`（**另一个类型**），而非现在的 `chaos_managed_exception`。

**为什么这是最 C# 一致的**：
- 托管异常（含断言失败）的行为与 C# 完全一致：**未被捕获就向外传播**
- 有 EH 的方法（不包 catch）与无 EH 的方法（catch 后重抛）**行为一致**
- "吞掉未注册符号"变成对**另一类**异常的显式兜底，不再与托管语义混同

**成本**：中 —— 需新增一个 C++ 异常类型 + 找到 fallback 的抛出点并改类型。

**风险**：低-中 —— 但"未注册符号"若原本就抛 `chaos_managed_exception`，
改类型后需确认所有依赖该兜底行为的路径仍然兜得住。

### 方案 E：无 EH 方法也重抛（最小改动版）

**做法**：catch 内直接 `throw;`（不再 `return {}`）。

```cpp
catch (const chaos_managed_exception&) { throw; }
```

**语义**：与 C# 一致（异常传播）。但**丢掉了** `_wrapInTryCatch` 原本要的兜底 ——
未注册符号的异常会穿透到 runner 的 SEH，被记为 `caught=true` → `passed=false`。
即"未注册符号"从"静默返回默认值"变成"该 subject 失败"。

**这可能恰恰是 C# 正确的**：C# 里调用一个不存在的实现，本就该失败。

**成本**：**最低**（一行）。**风险**：中 —— 会让一批"靠兜底静默通过"的 subject 变红。

### 方案 F：断言不抛异常（改 `Fail` 语义）

**做法**：`Fail` 只置退出码，不抛。

**语义**：**与 C# 不一致** —— C# 的 `Fail` 确实抛异常，且调用方可能依赖
"断言失败后不再继续执行"。改成不抛会让测试方法**在断言失败后继续跑**，
可能掩盖后续错误或产生额外副作用。

**评估**：最省事，但**明确违背你要求的"C# 一致"**。仅作兜底记录。

### 方案 G：保留现状 + runner 侧只看退出码

**做法**：不动 codegen，只改 runner：`assertFailed = Complete() != 0`（去掉 `!caught`）。

**语义**：断言失败**能被检出**（退出码是 1），但方法体的行为仍与 C# 不一致
（异常被吞、返回 0）。属于"结果对了、过程不对"。

**成本**：最低（runner 一行）。**风险**：低 —— 但没解决语义问题。

---

## 3. 对比矩阵

| 方案 | C# 语义一致 | 保留 fallback 兜底 | 成本 | 让断言失败变红 |
|---|---|---|---|---|
| **D** 分离 catch 种类 | ✅ **完全** | ✅ 是（显式） | 中 | ✅ |
| E 无 EH 也重抛 | ✅ 异常传播一致 | ❌ 丢弃 | 低 | ✅ |
| F `Fail` 不抛 | ❌ 不一致 | ✅ | 低 | ✅（靠退出码） |
| G 只改 runner | ❌ 过程不一致 | ✅ | 最低 | ✅ |

---

## 4. 建议

**方案 D**。理由：

1. **唯一同时满足"C# 语义一致"与"保留兜底意图"的方案**
2. 它把被混同的两件事**显式分开**：
   - "托管异常" → 遵循 C# 语义传播
   - "未注册符号" → 显式的、另一类型的兜底
3. 顺带修掉 1.3 节的**自相矛盾**（有/无 EH 两类方法行为不一）
4. 与已完成的判别性期望表（P0）配合，才能让断言失败真正产生红灯

**若你更看重"最小风险"**，方案 E 是次优：一行改动、语义基本一致，
代价是失去对未注册符号的静默兜底（它们会变成 subject 失败——**很可能本就是对的**）。

---

## 5. 影响面（所有方案共有）

无论选哪个，**打开后都会让一批既有"通过"变红**。这需要：

1. **先在反射 chunk 量化** —— 跑一次，统计新增红灯
2. **逐个甄别**：真实缺陷 / 断言本身写错 / 兜底行为变化
3. 再决定是否全量推广到其它 chunk

**在量化完成前不应合入 main。**

---

## 6. 附带必须同修的（否则仍有假绿）

| # | 问题 | 位置 | 说明 |
|---|---|---|---|
| 层 2 | `assertFailed` 与 `!caught` 耦合 | `RuntimeEntry.cpp.scriban:427` | 异常路径会掩盖退出码 |
| 层 3 | JIT 分支 `assertFailed = false` | 同文件 `#else` | JIT 下断言失败永不上报 |
| 层 1 | `s_exitCode` 是 `[ThreadStatic]` | `Assert.cs:12` | 当前 main-thread dispatch 下不是活跃成因，但跨线程场景会失效 |
