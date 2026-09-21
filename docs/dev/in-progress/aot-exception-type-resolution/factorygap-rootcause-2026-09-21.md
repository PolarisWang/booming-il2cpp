# 62 项 factoryGap 根因 — 多 catch 降低把 try 体发射两次，第二个 catch 吞掉正确异常

> 日期：2026-09-21
> 方法：读生成体 + `entry.exe --fact-json` 抓插桩（`[GUO]` / `[EHMTCH]`）
> 结论：**已定位，一手证据充分**

## 1. 结论（一句话）

`BuildMultipleCatchExceptionIRBody` 把**共享 try 体的树重复挂到每个 catch 区域**，
生成**两个顺序的 `CHAOS_EH_TRY` 块**，每个各带一份 try 体。
第二个块的 catch 类型被解析成 **`System.Object`**（万能匹配）→
**捕获一切并抛 "wrong exception type"** → subject 必然失败。

**所以 factoryGap 的成因既不是 factory（native 返回了非空对象），
也不是异常类型解析（匹配其实成功了），而是多 catch 的 EH 降低。**

## 2. 被测 C#（ATG 生成，语义正确）

`CombinedSubjects.cs:2597`：

```csharp
public long Reset_0_Stream_0()
{
    try { SubjectInstanceFactory.Create<Utf8JsonWriter>().Reset(default(Stream)!);
          throw new System.Exception("AOT stub did not throw"); }
    catch (System.ObjectDisposedException) { }                    // ← 空处理器，应吸收
    catch { throw new System.Exception("wrong exception type"); } // ← 兜底
    return 42L;
}
```

## 3. 生成体实际结构（一手，行号绝对）

```
93316:  CHAOS_EH_TRY
          ... Create<Utf8JsonWriter>() ...            ← 第 1 次
          ... ChaosUtf8JsonWriterResetStream(...) ... ← 第 1 次
93341:    CHAOS_EH_THROW(_s0)   // "AOT stub did not throw"
93342:  CHAOS_EH_CATCH_BEGIN
93343:    if (!chaos_eh_match_type(EXC, chaos_mt_..._ObjectDisposedException...))
93344:    { CHAOS_EH_RETHROW; }
          _s0 = CHAOS_EH_EXCEPTION_OBJ;              // 空处理器 ✅ 语义正确
93346:  CHAOS_EH_END

93347:  CHAOS_EH_TRY                                  ← ❌ 第二个 try，重复整个 try 体
          ... Create<Utf8JsonWriter>() ...            ← 第 2 次
          ... ChaosUtf8JsonWriterResetStream(...) ... ← 第 2 次
93373:    CHAOS_EH_THROW(_s0)   // "AOT stub did not throw"
93374:  CHAOS_EH_CATCH_BEGIN
93375:    if (!chaos_eh_match_type(EXC, chaos_mt_System_Private_CoreLib_System_Object...))
93378:    { CHAOS_EH_RETHROW; }
          _s0 = "wrong exception type";
          ... new Exception("wrong exception type") ...
93388:    CHAOS_EH_THROW(_s0)                          // ❌ 恒抛
93389:  CHAOS_EH_END
```

**机械证据**：该函数体内
`SubjectInstanceFactory__Create_..._Utf8JsonWriter()` 出现 **2 次**、
`ChaosUtf8JsonWriterResetStream` 出现 **2 次** —— try 体被**完整发射两遍**。

## 4. 为什么表现为 factoryGap

| 步骤 | 现象 |
|:-----|:-----|
| 第 1 个 try 块 | `Create` 成功、`Reset` 正确抛 `ObjectDisposedException` → 被 typed catch 吸收 ✅ |
| **第 2 个 try 块重新执行** | 再次 `Create` + `Reset` → 再次抛 `ObjectDisposedException` |
| 第 2 个 catch 类型 | **`System.Object`** → 万能匹配成功，不 rethrow |
| 结果 | 落入 catch 主体 → `throw new Exception("wrong exception type")` → 逃逸到顶层 |
| runner | `passed=false`, `caught=true`, `assertFailed=false` |
| 分类器 | `is_factory_subject && !assertFailed` → **factoryGap** |

## 5. 两个已证伪的中间假设（保留以免重走）

### ❌ 假设 A：`GetUninitializedObject` 返回 0

`[GUO]` 插桩：**84 次调用**，`taskLike=0` 全走完整分配路径，
`stable_id=953245605641312966`（= `Chaos_TestFramework_Sdk_Utf8JsonWriter`）出现 **77 次**。
→ **native 返回非空对象。已排除。**

### ❌ 假设 B：`chaos_eh_match_type` 的类型匹配失败

`[EHMTCH]` 插桩实测：

```
exc_obj=0000019480000440 expected=00007FF68FDFD0C0
stable_actual=9595069693833675140  stable_exp=9595069693833675140   ← 相等 ✅
```

`9595069693833675140` = `chaos_type_id_..._ObjectDisposedException`。
→ **类型匹配成功。已排除**（也就解释了为何上一轮的异常表修复对此无影响）。

> **教训**：`exc_obj < 0` 这一行曾让我误以为 tag 句柄被拒；实测该分支与本次失败无关。
> **结论性判断前必须插桩取一手数据，不要从代码形态推断。**

## 6. 真正的缺陷点

`NativeAotLoweringPlanner.StructuredIR.Emit.cs:2133`
`BuildMultipleCatchExceptionIRBody`：

```csharp
var tryTree = BuildExceptionPartitionTree(multiCatch.TryInstructions, offsets);
for (int i = 0; i < multiCatch.CatchRegions.Count; i++)
{
    var handlerTree = ...;
    nodes.Add(new IRExceptionRegion(
        IRExceptionKind.TryCatch,
        tryTree,                                    // ← 同一个 tryTree 被挂 N 次
        handlerTree,
        CatchTypeSubjectId: multiCatch.CatchRegions[i].CatchTypeSubjectId));
}
```

注释自称 *"The catch regions are siblings (sequential), not nested"* ——
但是**顺序 ≠ 等价于 C# 的多 catch 语义**。
C# 的多 catch 是**对同一次异常依次尝试匹配**；这里却是**每个 catch 各跑一遍 try**。

### 为什么第二个 catch 变成 `System.Object`

第二个区域本应对应 C# 的 `catch { }`（无类型 = `catch (object)`）。
降低器**正确地**把它解析成 `System.Object` ——
**是它被放在"再跑一遍 try"的位置才变得致命**。
若结构正确（同一 try 上顺序匹配多个 catch 类型），`catch (object)` 本就是最后兜底，语义无误。

## 7. 修复方向（待定，需先确认既有架构约定）

**应在同一个 `CHAOS_EH_TRY` 内发射多个 catch 匹配分支**：

```cpp
CHAOS_EH_TRY
    <try body 一次>
CHAOS_EH_CATCH_BEGIN
    if (match(EXC, ObjectDisposedException)) { <handler 1> ; goto done; }
    if (match(EXC, Object))                 { <handler 2> ; goto done; }
    CHAOS_EH_RETHROW;
done: ;
CHAOS_EH_END
```

**待确认**：
1. `ER.CatchTypeSubjectId` 单值的 `IRExceptionRegion` 记录是否需扩为**多 catch 列表**
   （当前 record 只能表达一个 catch）
2. `CHAOS_EH_CATCH_BEGIN` 宏在三种 EH 模式（SEH/SETJMP/CPP_THROW）下是否都支持
   多分支 + `goto` 跳出
3. **影响面**：这是**通用 EH 降低缺陷**，凡「同一 try 上有 ≥2 个 catch」的方法全部受影响。
   实测 `System.Text.Json` text-json chunk 中
   `chaos_eh_match_type(..., System_Object)` 出现 **114 次** —— **远超 62 项 factoryGap**。

## 8. 对既有结论的修正

| 既有说法 | 修正 |
|:---------|:-----|
| 「62 项 factoryGap 是 `SubjectInstanceFactory.Create<T>()` 返回 null」 | ❌ 错误。native 返回非空；失败在**多 catch 降低** |
| 分类器把失败归入 `factoryGap` | ⚠️ **归因误导** —— 该项恒为 factory 型 subject，但真因与 factory 无关 |
| 上一轮异常表修复（`ac83d7621`）对此无变化 | ✅ 与此吻合 —— 类型匹配本来就成功 |

---

## 9. 影响面量化（2026-09-21 实测）

跨 **74 个生成体** 统计：

| 模式 | 计数 |
|:-----|:----:|
| `CHAOS_EH_TRY` | 1742 |
| `CHAOS_EH_CATCH_BEGIN` | 1608 |
| `chaos_eh_match_type(..., System_Object)` | **881** |

在 text-json 单个 chunk 内做逐方法分析（按 `extern "C" ` 切分方法体）：

| 指标 | 计数 |
|:-----|:----:|
| 「同一 try 体重复发射 且 ≥2 个 catch 类型」的方法 | **114** |
| `System_Object` catch 位点 | **114** |

**完全 1:1 对应** —— 每个 `System.Object` catch 位点都恰来自一个 try 体重复的方法。
**机制确证。**

### 影响不止 62 项 factoryGap

114 个受影响方法中只有 62 个落入 `factoryGap` 桶（其余因 subject 形态不同落在
`stubGap` / `failed` / `smoke`）。

**且该缺陷是通用的**：任何「同一 try 上有 ≥2 个 catch」的方法，
**无论是否涉及 factory**，都会：
1. try 体被执行两次（**副作用翻倍** —— 对 writer/reader 类 API 是正确性问题）
2. 第一个 catch 的正确匹配被第二个 catch 吞掉

**这是本 chunk 之外也应受影响的代码生成缺陷**，不限于 JSON/XML 线。
