# P2-X 移交：`chaos_external_runtime_*` 调用丢失实参

> **状态**: 未修复（三轮尝试后按「三次修复规则」停止）
> **归类**: **codegen 域**（非 EH）
> **日期**: 2026-09-28
> **关联**: `P2-L3-CONCLUSION.md` §四、§六、§七

---

## 一、问题一句话

`chaos_external_runtime_*` 的**调用点**与**stub 声明**使用了**两份互不同步的 arity 来源**，
导致二者对不上：调用传 N 个参数，声明是 `(void)` → **C2660 × 727**（修复第一层后）。

修复前则相反：**调用点本身零实参** → 参数被静默丢弃 → 被调方法不抛异常
→ subject 的 `"AOT stub did not throw"` 分支执行 → `caught=true / assertFailed=false`
（即 P2 诊断的 1614 条 EUR 的主因）。

---

## 二、已确证的事实链（全部有产物/反射证据）

### 2.1 IR 层

`aot-core-ir.json` 中该调用：

```json
{
  "op": "call",
  "callee": "System.Xml.ReaderWriter/System.Xml.XmlConvert::ToDateTime:System.DateTime(System.String,System.String)",
  "targetParameterCount": null,     ← 未填充
  "targetSymbol": null,             ← 未填充
  "targetReturnType": null,         ← 未填充
  "dispatchKindCode": 4             ← HybridDispatchKind.ExternalRuntime
}
```

**只有 `callee` 字符串携带参数信息。**

### 2.2 推断函数本身正确（反射实测）

```csharp
GetRequiredTargetParameterCount(instruction)  = 2   ← 正确
InferParameterCountFromSubjectId(callee)      = 2   ← 正确
```

方法：从 `Chaos.IL2CPP.Generator.dll` 反射调用（`BindingFlags.NonPublic | Static`）。

**所以「推断不正确」这个假设已被排除。**

### 2.3 真实执行路径（调用栈插桩）

在 `NativeAotLoweringPlanner.InvocationPlanning.Dispatch.Interface.cs` 的**全部 9 处**
`return new InvocationTarget(` 前插桩（打印 `StackTrace.GetFrame(0).GetMethod().Name`）：

```
13519  site#7  ComputeMaxEvalStackDepth
 3751  site#7  ResolveDirectInvocationTarget      ← 真实路径
  523  site#6  ResolveDirectInvocationTarget
  114  site#3  ResolveDirectInvocationTarget
   97  site#6  EmitLinearNewObject
   38  site#3  ComputeMaxEvalStackDepth
```

**site#7 = 第 340 行**，位于 `TryResolveDirectInvocationTarget(string? callee)` 内
（注意：该方法**没有 `instruction` 参数**，只有 `callee` 字符串 —— 这是后续修复的约束）。

---

## 三、缺陷 1（已定位，未保留修复）

### site#7 直接采用 helper 的 ABI

`Planning/NativeAotLoweringPlanner.InvocationPlanning.Dispatch.Interface.cs`
（`TryResolveDirectInvocationTarget`，约 340 行）：

```csharp
return new InvocationTarget(
    helperDefinition!.TargetSymbol,
    helperDefinition.ParameterAbis,      // ← 直接用，无 Count>0 检查、无回落推断
    helperDefinition.ReturnAbi,
    helperDefinition.RawArgumentIndices,
    DirectNativeSymbol: helperDefinition.DirectNativeSymbol,
    CtorReturnsNativeHandle: helperDefinition.CtorReturnsNativeHandle);
```

**它与上方 site#6 是同一缺陷**，而 site#6 **已修**，其注释原样记录了这个坑：

> ```
> // Reusing helperDefinition's ABI here describes the shim, not the target,
> // so the emitted call talked to the real symbol with the shim's signature:
> // zero arguments, and the result assigned to `const auto chaos_result` even
> // when the target is void.  That produced, for every redirected call:
> //
> //   C2660: '...': function does not take 0 arguments
> //   C3313: 'chaos_result': variable cannot have the type 'const void'
> //
> // The arguments were consumed from the eval stack by the shim's
> // (empty) parameter list, so they were silently dropped on the floor
> ```

site#6 的修法是**从 `lowerableAotMethod` 取 ABI**；但 site#7 处在
`TryGetLowerableMethod` 失败**或** `DirectNativeSymbol != null` 的 fallthrough，
**没有方法可取**，所以需要另一条路（见 §五）。

### 已尝试的修复及其结果

```csharp
var site7ParamAbis = helperDefinition!.ParameterAbis.Count > 0
    ? helperDefinition.ParameterAbis
    : CreateLegacyAbiParameterSlots(InferParameterCountFromSubjectId(callee));
```

**效果**：调用点开始正确传参（`chaos_arg_0, chaos_arg_1, chaos_arg_2`）✅
**代价**：暴露缺陷 2 → **C2660 × 727**。

---

## 四、缺陷 2（已定位，未攻克）

### 调用点与声明用了两份独立的 arity 来源

**填充侧**（调用点发射时写入）：

| 文件 | 行 |
|---|---|
| `ExceptionEmission.Utilities.cs` | ~301 |
| `ExceptionEmission.Utilities.cs` | ~689 |
| `ExceptionEmission.Linear.cs` | ~768 |

均为：

```csharp
_emittedExternalRuntimeSymbolParams[<symbol>] = invocationTarget.ParameterAbis.Count;
```

**读取侧**（stub 声明生成）：

| 文件 | 行 | 缺省值 |
|---|---|---|
| `Methods.Remaining.cs` | ~693 | **1** |
| `Methods.Remaining.cs` | ~836 | **0** |

```csharp
int paramCount = _emittedExternalRuntimeSymbolParams.TryGetValue(sym, out var pc) ? pc : 0;
```

**两个声明点的缺省值不一致（1 vs 0）**，且都依赖同一个可能查不到的表。

### 已尝试的修复及其结果

把三处填充改为 **max-merge**（`ConcurrentDictionary.AddOrUpdate` + `Math.Max`），
理由是「arity 只能被低估，取最大值是安全上界」。

**结果：C2660 仍为 727，完全无变化。**

**推论**：**声明生成时该符号根本不在表里**（而非被覆盖）。
即问题不是「合并策略」，而是**表的可见性/时序** ——
`Methods.Remaining.cs` 的声明生成可能在填充之前执行，或读的是另一个 planner 实例。

---

## 五、建议的下一步（未实施）

按可行性排序：

### 方案 1（推荐）：让声明侧不依赖表，直接推断

声明生成处能拿到**符号名**，但**符号名无法还原 arity**
（`GetExternalRuntimeHelperSymbol` = `"chaos_external_runtime_" + SanitizeSubjectId(subjectId)`，
标点全被压成 `_`，与记忆 `[symbol-encoder-must-be-injector]` 同族的问题）。

因此需要**另建一张 `symbol → subjectId` 的映射**，让声明侧能调
`InferParameterCountFromSubjectId(subjectId)`。

### 方案 2：修填充/读取的时序

先确认 `Methods.Remaining.cs` 的两处声明生成相对于
`Parallel.For` 方法体发射的执行顺序。若是并行导致的读不到，
应在**全部方法体发射完成后**再做一次「补全」pass。

### 方案 3：核对两个声明点的缺省值

`1` 与 `0` 不一致本身就是隐患，且注释互相矛盾：
- 693 行注释说「Default to 1 … ALL external runtime stubs are called with at least chaos_arg_0」
- 836 行注释说「Without this, the stub declares () noexcept but the caller passes arguments」

至少应统一，并加**产物级守卫**：符号名暗示 arity > 0 而声明为 `(void)` → 生成期报错。

---

## 六、方法论教训（本轮新增）

### 6.1 静态阅读会锁定到「看起来对」的错误路径

本缺陷中，我先后在 **4 个不同的 `return new InvocationTarget`** 上做了静态分析，
每次都得出「链路正确」的结论 —— 因为**实际执行的不是那一个**。

**有效手段**：在**全部**候选点插桩并打印**调用栈**：

```csharp
System.Console.Error.WriteLine($"[SITE] {idx} " +
    new System.Diagnostics.StackTrace(1, false).GetFrame(0).GetMethod().Name);
```

一次运行即可定位真实路径（本轮 3751 次 site#7 vs 我之前反复读的 site#2）。

### 6.2 修复 arity 类缺陷时必须同时验证「两侧」

arity 有**两个消费端**（调用点 + 声明），修一侧必然以 C2660 的形式暴露另一侧。
本次第一层修复的效果（调用点传参正确）与代价（C2660）都是**预期内**的，
检查时不应把 C2660 当作「修复失败」。

### 6.3 三次修复规则的实际触发点

- 第 1 次：site#7 改推断 → 调用点对了，暴露 C2660
- 第 2 次：填充改 max-merge → 无变化
- 第 3 次：（同上，覆盖三处）→ 无变化

第 3 次后停止，转文档移交。**「无变化」是最强的停止信号** ——
它说明假设错了（不是合并策略问题），继续叠加修复只会掩盖。

---

## 八、第二次推进（2026-09-28）—— 五轮尝试全部回退

按 §五方案 1 继续推进，**共 5 轮尝试，全部回退**。但产出了一条**关键的新证据**。

### 8.1 尝试记录

| # | 改动 | C2660 |
|---|---|---|
| 1 | **只改声明侧**（`Methods.Remaining.cs:692` 用 `InferParameterCountFromSubjectId(kvp.Key)` 替代查表） | **0** ✅ |
| 2 | 只改调用侧（site#7） | 727 |
| 3 | 两侧都改（声明 692 + site#7） | 727 |
| 4 | 加 `symbol → subjectId` 表供声明侧使用 | 727 |
| 5 | 完整同步（4 处 + 表） | **727** |

### 8.2 关键发现：唯一成功的组合是「只改声明侧」

第 1 轮把 C2660 从 727 打到 **0** —— 证明**声明侧修复确实生效**
（它把 `(void)` 改成了正确 arity）。

但那一轮产物里**调用点也是零参**，即：**声明与调用点是「协调地坏」** ——
两者都是 0 参，所以编译通过、C2660 消失，**但实参仍然被丢弃**（问题未解决，
只是换了个形态隐藏）。

一旦让调用侧也正确传参（第 2/3/5 轮），C2660 立刻回来 —— 说明
**声明侧仍未真正跟上**。

### 8.3 由此推出的、必须下一轮验证的结论

**`Methods.Remaining.cs:692` 不是生成那条 `(void)` 声明的唯一位置，甚至可能不是主要位置。**

本轮已发现**至少 3 处**声明生成点：

| 位置 | 用的 key | 读的表 |
|---|---|---|
| `Methods.Remaining.cs:692` | **subjectId**（`kvp.Key`）| `_emittedExternalRuntimeSymbolParams` |
| `Methods.Remaining.cs:922` | **symbol** | `_emittedExternalRuntimeSymbolParams` |
| `AddExternalRuntimeStubs`（`NativeAotEmitter.Shared.cs:1091`）| **正则扫产物文本** | 不读表，从 call site 数参数 |

**第 3 处最可疑**：它是**正则驱动的后处理**，规则是

```csharp
var defMatch = Regex.Match(text, Escape(sym) + "\(\) noexcept\s*\{");
if (defMatch.Success) continue;   // 已有 () noexcept { 的定义 → 跳过、不补声明
```

即「若产物里已有 `() noexcept {` 的定义，就不再补写正确 arity 的声明」——
**它会把一个错误的零参定义固化为最终形态。**

### 8.4 建议的下一轮做法（与之前不同）

**不要**再从「读代码猜哪一处生效」入手（本轮 5 次都栽在这里）。
改用**运行时插桩直接标注每一处**：

1. 在 §8.3 的**三处**各插一条带唯一标记的诊断（`[DECL-A]` / `[DECL-B]` / `[DECL-C]`）；
2. 只对 `ToDateTime` 这一个符号运行；
3. 看**哪几条标记被打出来** —— 这才是「哪一处真正生成了那条声明」的答案。

带标记的插桩在本缺陷上已被证明有效：§2.3 用同样手法（`StackTrace`）
一次定位到 site#7，而此前 4 轮纯静态阅读全部锁定了错误的返回点。

### 8.5 本轮的方法论教训（新增）

- **「C2660 归零」不等于修复成功**。本轮第 1 轮就出现了归零，
  但那只是「两侧一起坏」的假象。判据必须是**调用点是否真的传了实参**
  （读产物），而不是「编译是否通过」。
- 这与项目记忆里 `[assert-complete-weak-stub-always-won]` 同属一类：
  **一个看似正面的信号，掩盖了缺陷只是换了形态。**
