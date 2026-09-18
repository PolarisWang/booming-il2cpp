# L2 深度分析 — 跨程序集泛型实例化的架构问题

> task_id: `generic-instantiation-materialization` 子问题
> 日期：2026-09-18
> 前置：`STATUS.md` §8（配置 bug 已修 + 两次实验未收敛）

## 一、L2 到底是什么问题（一句话）

**要把 `Assert.AreEqual<T>` / `SubjectInstanceFactory.Create<T>` 这类
跨程序集泛型方法物化成可执行代码，就必须加载"定义侧"程序集
（`System.Private.CoreLib`）；而当前 codegen 的加载模型是
「以 subjects DLL 为根、按需加载」，无法承受 CoreLib 的规模。**

现象（两次实测）：

| 输入 | TPG 内存 | 结果 |
|---|---|---|
| 5 个 BCL DLL（17 MB） | 1.8 GB 稳态 | CPU 776s 无产出 |
| 185 个 DLL | 5.0 GB 爬升 | >40 min 无产出 |

输入体积对比：`CoreLib` 是 subjects DLL 的 **131 倍**。

## 二、根因链（逐层，带 file:line）

### 2.1 为什么 demand 会产生

subjects DLL 里有一句 `Assert.AreEqual<T>(...)`，其成员引用指向
`System.Private.CoreLib/Chaos.TestFramework...Assert::AreEqual`1`。
Loader 扫 MemberReferences / MethodSpec 表时记录一条 **demand**
（`CrossAssemblyInstantiation.cs:103,116`）。

### 2.2 为什么 demand 无法满足

`TryProjectCrossAssemblyMethodInstantiation`（`CrossAssemblyInstantiation.cs:132`）
第一道门槛是**目标程序集必须已在 `definitionMethodsByAssembly` 里**：

```csharp
if (!definitionMethodsByAssembly.TryGetValue(methodReference.AssemblyName, out var targetDefinitionMethods)
    || ...)
{
    return;   // ← 静默丢弃
}
```

`definitionMethodsByAssembly` 由 `BuildAssemblyDict(loadedAssemblies, ...)`
构建 —— **只含 `assemblyPaths` 里加载过的程序集**。

### 2.3 为什么 CoreLib 不在里面

`assemblyPaths` = `[InputAssemblyPath] + AdditionalAssemblyPaths`
（`LoaderStage.cs:27-40`）。
`AdditionalAssemblyPaths` 来自 `--assembly-dir` —— 而 threading-tasks
**在 pipeline-config 里没声明**，所以只有 subjects DLL。

（配置声明机制此前还是坏的 —— `_load_pipeline_config` 不读 `chunks:`，
已修 `842375d7f`。）

### 2.4 关键：**即使补上，也不可行**

这是本轮最重要的发现。修好配置后补喂 BCL，codegen 不收敛。

原因在 `ProjectCrossAssemblyMethodInstantiations` 的结构
（`CrossAssemblyInstantiation.cs:58-67`）：

```csharp
foreach (var assemblyPath in assemblyPaths)          // ← 每个加载的程序集
{
    CollectProjectedCrossAssemblyMethodInstantiations(...);
}
// 其中：
//   foreach (var memberReferenceHandle in metadataReader.MemberReferences)   // ← 扫全部成员引用
//       TryProjectCrossAssemblyMethodInstantiation(...);
//   for (rowNumber = 1 .. MethodSpec 表行数)                                  // ← 再扫全部泛型实例化
//       TryProjectCrossAssemblyMethodInstantiation(...);
```

**即：对每个加载的程序集，遍历它全部的 MemberReference 与 MethodSpec。**

代入 CoreLib（~10k 类型 / ~70k 方法量级）：仅「读表 + 构造 SubjectId 字符串 +
查字典」就是**百万级的字符串构造与字典查找**。而 `LoadAssembly`
（`AssemblyLoading.cs:12-60`）还要对每个程序集做
`LoadTypes` / `LoadFields` / `LoadProperties` / `LoadMethods` /
`BuildGenericInstantiationProjection` 全量建模。

**这是 O(程序集数 × 该程序集全部成员引用) 的组合展开**，
不是「多传一个参数」的量级。

> ⚠️ **诚实标注**：以上是**代码结构推论**，尚未 profile 验证。
> 成本中心可能是三处之一（或叠加）：
> (a) `LoadAssembly` 的 eager 建模（LoadTypes/LoadMethods/...）
> (b) MemberReferences + MethodSpec 的全表扫描
> (c) `DescribeMemberReferenceMethod` 的每项 SubjectId 字符串构造
> **在动方案 B 之前必须先 profile 确认**（见第六节问题 1），
> 否则可能改错地方 —— 这正是本轮「IL 数据表接线」失败的教训。

### 2.5 还有第二重放大

投影出的方法要进闭包才能被 lower。而
`LinkerStage.Reachability.cs` 的 `ExpandReachableMethods`（:404-474）
**从不查 demand 图** —— 它只沿 `instruction.Callee` 字符串匹配走。
所以即使物化了，还得「某条指令的 Callee 恰好等于其 SubjectId」才能进闭包。

## 三、这意味着什么（判定）

**L2 是设计边界的暴露，不是配置疏漏。**

项目的加载模型隐含假设：**输入是"小的 subjects 程序集"，
依赖的 BCL 由 runtime 侧的 stub/fallback 承接**。
这个假设对「写 native stub」类是成立的，但对
**「需要把 BCL 泛型方法真正编译成代码」** 这类需求直接崩掉。

config 注释写的「Cross-assembly references are resolved by the BRIDGE-AOT
compiler via runtime directory discovery」在全仓**零命中** ——
它是一个**设想**，不是现状。L2 就是这个设想未落地所留下的洞。

## 四、方案（4 个，按代价/收益排序）

### 方案 A：**不物化，改为显式失败**（灭假绿，不修覆盖率）

**做什么**：让落 catch-all 的泛型方法**报错**而非返 0。
`GenericCapabilityMatrixBuilder` 已经在检测（`Status="fail"` +
`unsupportedAuthorityLeak`），只需让它的 fail **真正阻断**。

**代价**：小（改 fact/gating 口径 + 让 builder 的 leaks 影响 gate）。
**收益**：把 **172 处 `Assert.AreEqual<T>` 假绿变成真红** —— 覆盖面数字会
变小（诚实），但**不再是错的**。
**能否解根因**：❌ 不解。但**消灭了"看起来通过"这个更危险的问题**。

> 这是唯一**确认成本可控**的方案，且与 repo 反复出现的假绿戒律一致。

### 方案 B：**按需加载目标程序集**（治本，但工程量大）

**做什么**：不预加载全部 BCL。改为**两阶段**：
1. 先只加载 subjects DLL，收集 demand（已知需求清单）
2. 对 demand 里出现的 `(目标程序集, 具体方法 SubjectId)`，
   **只加载能提供该方法的程序集，且只物化被点名的实例**

**关键改动**：`definitionMethodsByAssembly` 不能是「全量 ToDictionary」
（`CrossAssemblyInstantiation.cs:36-38`），要改成**惰性/按需**查找。

**代价**：中-大。要动 Loader 的加载模型（当前是 eager 全量建模）。
**收益**：真正让泛型断言可执行。
**能否解根因**：✅ 能，但要先把 2.4 的 O(N×M) 展开换成按需。

**真正的难点**：即使按需加载，也要处理 2.5 —— 投影出的方法如何进闭包。
这需要 Linker 增加「以 demand 图为种子」的可达性分支。

### 方案 C：**在 subjects 侧直接编写闭合泛型调用**（绕过跨程序集）

**做什么**：ATG 生成测试时，把 `Assert.AreEqual<T>(a,b,s)`
**改为调用具体重载** `Assert.AreEqual(int,int,string)` 等。
即**在源头避免泛型**。

**代价**：中（改 ATG，属测试生成策略）。
**收益**：只对**测试自己的断言**有效（这正是 172 处假绿的主体）。
**能否解根因**：⚠️ 只解决"测试断言"这一子集；`SubjectInstanceFactory.Create<T>`
这类框架设施仍无解。且**掩盖了真实缺口**。

### 方案 D：**接受边界，缩小目标**（务实）

**做什么**：明确「跨程序集泛型不物化」为已知边界，写进文档与 gate 白名单，
**同时用方案 A 保证不假装通过**。

**代价**：小。
**收益**：数字诚实 + 不再重复投入。
**能否解根因**：❌ 不解，但**止损**。

## 五、推荐组合

**A + D 立即做**（成本小、消灭假绿、止损），**B 单独立项**（若确实需要覆盖率）。

理由：
- A 是**独立于 B 的价值** —— 无论 B 做不做，让假绿变真红都该做
- B 的风险被低估了：2.4 的 O(N×M) 与 2.5 的闭包种子是**两个**独立架构改动
- C 我**不推荐** —— 它把问题从"运行时缺口"挪到"测试生成"，掩盖而非解决

## 六、如果做 B，必须先回答的三个问题

1. **按需加载的最小单元是什么？** 加载整个 `System.Private.CoreLib`
   仍是 131 倍输入。能否**只读 PE metadata 表、不建模全部类型**？
   （`LoadTypes`/`LoadMethods` 的 eager 建模是主要成本，还是"扫 MemberReferences"？）
   → **需要先做一次 profile**，否则改错地方。

2. **物化一个 `AreEqual<System.Char>` 需要哪些前置？**
   其 IL body 引用的所有类型/方法是否也要同批物化（递归展开）？
   若是，递归深度是否可控？

3. **闭包如何以此为种子？** `LinkerStage.Reachability.cs` 需要新增分支，
   否则物化后仍被丢弃（2.5）。

> 未回答 1-3 之前不应动 B —— 否则就是本轮"IL 数据表接线"的重演
> （先实现、后验收益、发现为负）。

## 七、与「IL 数据表接线」失败的对照

本轮另一个失败（`il-data-table-not-registered`）的教训**同样适用**：

> 我在识别出「Phase 1 正确性存疑」后，**没有先花十分钟验证，而是先投入实现**，
> 结果收益为负、全部回退。

**B 必须避免同样错误**：先 profile 定位成本中心（问题 1），
再决定是否投入。**不要先写代码。**
