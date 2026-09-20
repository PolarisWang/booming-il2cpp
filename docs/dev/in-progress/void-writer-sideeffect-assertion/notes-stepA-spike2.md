# Spike 2 — A2 方案端到端验证

> 日期：2026-09-20
> 方法：改 ATG 一行（`catch { }` → `catch (具体类型) { } catch { throw ... }`），
> 跑 `text-json` chunk 全链路。**spike 代码已恢复，未留在树上。**

---

## ✅ 结论 1：A2 端到端可行

### 证据 A：ATG 生成正确的 C#

```csharp
try { SubjectInstanceFactory.Create<JsonElement>().WriteTo(default(Utf8JsonWriter)!);
      throw new System.Exception("AOT stub did not throw"); }
catch (System.ArgumentNullException) { }
catch { throw new System.Exception("wrong exception type"); }
```

### 证据 B：codegen 正确翻译为具体类型匹配

```cpp
CHAOS_EH_CATCH_BEGIN
    if (!chaos_eh_match_type(CHAOS_EH_EXCEPTION_OBJ,
                             chaos_mt_System_Private_CoreLib_System_ArgumentNullException.AsTypeInfoHot()))
    { CHAOS_EH_RETHROW; }        // ← 类型不匹配则重新抛出
CHAOS_EH_END
```

生成体里的 catch 类型分布（spike 后）：

| 类型 | 数量 |
|:-----|:----:|
| `System_Object`（原有弱断言） | 114 |
| **`InvalidOperationException`** | **48** |
| **`ArgumentNullException`** | **32** |
| **`NullReferenceException`** | **25** |
| **`ObjectDisposedException`** | **5** |
| **`ArgumentException`** | **3** |

**这证明 `chaos_eh_match_type` 能承载具体类型，且 codegen 已支持**
（`StructuredIR.Emit.cs:1343` 的 `if (er.CatchTypeSubjectId != null)` 分支）。

### 证据 C：「错误类型会失败」成立

```
spike 前: factoryGap=35  unassertable=30
spike 后: factoryGap=62  unassertable=3
```

**27 个 subject 从「弱断言通过」变成「失败」** —— 断言**真的会失败**，不是恒真假绿。

---

## 🔴 结论 2：那 27 个失败是**真实暴露**，不是误伤

### 判定过程

以 `JsonElement::WriteTo` 为例，生成体的执行路径：

```cpp
chaos_external_runtime_..._JsonElement__WriteTo_...();   // ← catch-all，零参数
// ChaosExternalRuntimeFallback(callee) → 返回 0，**不抛异常**

_s2 = "AOT stub did not throw";
CHAOS_EH_THROW(_s2);                                     // 抛 System.Exception
CHAOS_EH_CATCH_BEGIN
    if (!chaos_eh_match_type(..., ArgumentNullException))
    { CHAOS_EH_RETHROW; }                                // ← 类型不匹配 → 逃逸 → 失败
```

**根因**：`WriteTo` 的 **shape 未被匹配**，走了 catch-all
（wrapper 里明写 `no native body`），而 catch-all **只返回 0、不复刻托管异常语义**。

### 为什么 shape 没匹配上

`S21.cs:235-244` 的注册**是正确的**（`{ recvAbi, wAbi }` 两参数）：

```csharp
registry.Register(typeName, "WriteTo",
    new[] { "Utf8JsonWriter" }, ShapeKind.SimpleForward,
    "ChaosUtf8JsonWriterWriteTo",
    new[]{ recvAbi, wAbi },           // ← 两个 slot，正确
    CreateVoidAbiSlot(), writeToR);
```

但生成的是**零参数 catch-all** → **匹配失败**。原因未定位
（可能 key 不匹配，如 `typeName` 格式 vs subject 的声明类型）。

### 这是**既存缺陷**，非 spike 引入

- 我的改动**只动 catch 类型**，不碰 shape 匹配
- catch-all 的 `no native body` 警告在 spike 前就存在

**spike 的价值在于**：它**暴露**了这个既存缺口 —— 之前被 `catch { }` 弱断言**掩盖**了
（弱断言不验类型，catch-all 返回 0 也算"通过"）。

---

## 对 Step A 实施的影响

| 项 | 结论 |
|:---|:-----|
| **A2 方案** | ✅ 可行，无需新 codegen shape |
| **工作量** | ✅ 只改 ATG 一处（如 spike 所示） |
| **附加发现** | ⚠️ 会**暴露一批既存构造缺陷**（shape 未匹配 → catch-all），需逐个判定 |
| **风险** | 启用类型校验后，「失败的 subject 数」会**先上升** —— 这是诚实化的正常代价 |

## 建议的后续

1. **实施 A2**（ATG 一行改动）
2. **对新增失败逐个归因**：是「native 未实现」还是「shape 未匹配」
3. 前者归入 stubGap 类工作；后者是**独立的 codegen 缺陷**，应单独立项
