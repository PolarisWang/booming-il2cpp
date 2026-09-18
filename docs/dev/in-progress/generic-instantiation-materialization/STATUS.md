# 泛型实例化物化缺口 — 调查与立项

> task_id: `generic-instantiation-materialization`
> 类型：roadmap（跨域：Loader / Linker / CodeGen-Lowering）
> 创建：2026-09-18
> 前置：`generic-instantiation-demands-not-lowered-to-coreir`（记忆，2026-09-17 首次定位）

## 一、问题陈述

**泛型实例化需求（Demand）被正确采集，但从未被物化为闭合方法，
因而在 AOT 翻译中无声消失。**

后果不是「方法缺失」（那会显式失败），而是**静默降级**：
调用点落 `ChaosExternalRuntimeFallback` → 返回 fallback 值 / 抛
NotImplementedException → 在 fact 记录里表现为
`factoryGap` 或「断言未执行」，**覆盖率虚高**。

### 实测（threading-tasks chunk，2026-09-18）

```
demands=140   coreir methods=1062
subjectId 命中 CoreIR:            0
definitionSubjectId 命中 CoreIR:  0
MISSING:                        140
```

需求类型分布：

| 数量 | 类型 |
|---|---|
| 64 | `Int32>`（各种泛型闭合，截断显示） |
| 22 | `Task` |
| 14 | `DefaultInterpolatedStringHandler` |
| **11** | **`SubjectInstanceFactory`** ← 直接产出本次 65 个 factoryGap |
| 6 | `TaskFactory` |
| 5 | `Object>` |
| 4 | `Int32>>` / `Task>` |
| 2 | `EqualityComparer<!!0>` / `Linq.Enumerable` / `Array` / `Int32[]>` |

> 早先 `system` chunk 的测量是 **122/122 未进**（0%），影响 172 处
> `Assert.AreEqual<T>` 假绿。本次 140/140 是同一缺陷在 threading-tasks 上的复现。

## 二、根因（逐层证据，已定位到确切边界）

### 2.1 Demand 采集是**正确的**

`generic-instantiation-demand-graph.json` 条目完整且字段正确：
`requestingAssemblyName` / `owningAssemblyName` / `definitionSubjectId`
（如 `SubjectInstanceFactory::Create:!!0()`）/ `instantiationKey`
（`methodArguments: ["System.Threading.Tasks.Task<System.Int32>"]`）/
`isCrossAssembly: true` / `supportKindCode` / `specializationKindCode`。

### 2.2 但没有任何阶段把 Demand 变成 Method

| 阶段 | 行为 | 证据 |
|---|---|---|
| Loader | 只 **merge / 建查找表**，从不新增 method | `LoaderStage.GenericDemandGraph.cs:7,22,44,54` — 全是 `Build*`/`Merge*`/`BuildLookup`，无 `Methods.Add` / `new ManagedMethodModel` |
| **Linker** | **原样传递**，完全未处理 | `LinkerStage.cs:47` — `GenericInstantiationDemandGraph = semanticWorld.GenericInstantiationDemandGraph,` 一行透传 |
| Lowering | 只当**查找表** enrich 已存在的方法 | `AotCoreIrLowering.cs:58` `BuildGenericDemandLookup(...)`，随后 `linkedWorld.Methods.Select(TryCreateMethod)` —— **遍历的是 Methods，不是 Demands** |

**结论**：闭合实例若不在 Linker 阶段就已物化进 closure，
lowering 阶段**不会补**。需求图只用于 enrich，不用来新增。

### 2.3 这是「静默通过」族的第三次

| # | 路径 | 表现 |
|---|---|---|
| 1 | probe donor 类型漏发 | 类型级假绿 |
| 2 | JsonSerializer 无实现 | 整族静默 |
| **3** | **泛型实例化物化** | **断言级假绿（影响验证根基）** |

**关键分界**：`AreEqual(int,int)` 有真实体 → 真失败；
`AreEqual<T>` 无 → 假绿。这解释了为何 `system` chunk「只有 156 个 failed」。

## 三、动手前必须确认的三件事

按记忆 `generic-instantiation-demands-not-lowered-to-coreir` 的指引：

- **(a)** Linker 是否有意不物化 cross-assembly 方法级泛型？
  （`isCrossAssembly: true` 占绝大多数 —— 需确认是设计取舍还是遗漏）
- **(b)** 物化后的 ABI 如何处理 `!!0` 型参？
  （`AreEqual<T>` 的 T 在 ABI 上是什么 carrier？现在落 catch-all 返 0）
- **(c)** `GenericCapabilityMatrixBuilder.cs` 与该图的关系？
  （同一 repo 里已有一个消费该图的 builder，需确认职责边界）

## 四、❌ 明确禁止的修法

**勿按「给每种 T 写 native stub」的思路修。**
那是组合爆炸（T 的取值空间是全部 BCL 类型），且方向错误 ——
问题在物化阶段，不在 stub 层。

## 五、建议路径

1. **先做 (a)(b)(c) 三项确认**（只读调查，产出结论）
2. 若确认为遗漏 → `dev-brainstorm` 定物化方案（在 Loader 还是 Linker 阶段补）
3. 若确认为设计取舍 → 转为「如何让缺失显式化」：让落 catch-all 的泛型断言
   **报错而非静默通过**（把假绿变成真红，本身就有价值）

> 第 3 条是**无论 2 走哪条都该做**的 —— 它把「静默」消灭，
> 这与本 repo 反复出现的假绿戒律一致。

## 六、影响面

- `system` chunk：172 处 `Assert.AreEqual<T>` 断言不可信（real 1687 中含泛型部分）
- `threading-tasks`：65 个 factoryGap（`SubjectInstanceFactory.Create<T>`）
- **验证根基**：所有含泛型断言的 chunk 的 `real` 计数都偏高
