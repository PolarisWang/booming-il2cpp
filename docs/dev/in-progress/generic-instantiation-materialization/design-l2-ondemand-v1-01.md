# L2 方案 B 设计 — 按需加载跨程序集泛型定义

> task_id: `l2-ondemand-generic-definition-loading`
> 父任务：`generic-instantiation-materialization`
> 类型：design（brainstorm 产出）
> 日期：2026-09-18
> 状态：**设计已定稿**（spike 已验证 §4.2(b)，用户已拍板：先 spike→已做 / 合并为一个任务）

## 一、问题重述（一句话）

跨程序集泛型方法（`Assert.AreEqual<T>` / `SubjectInstanceFactory.Create<T>`）
的**实例从未被物化**，因为物化需要"定义侧"程序集的 `ManagedMethodModel`，
而当前实现要求**目标程序集已被整份加载并完成全量建模** —— 对
`System.Private.CoreLib`（subjects DLL 的 131 倍）不可行。

## 二、关键实测数据（本设计的立论基础）

| 指标 | 数值 | 来源 |
|---|---|---|
| threading-tasks demand 总数 | 140 | demand-graph |
| **其中 distinct `definitionSubjectId`** | **89** | 本次实测 |
| system chunk demand | 311 | demand-graph |
| 目标程序集分布 | CoreLib 122 / TestFramework.Sdk 11 / Collections 4 / Linq 2 / Threading 1 | demand-graph |

**核心洞察**：**需要的是 89 个具体方法，不是 2 个程序集。**
当前架构把"我要 89 个方法"实现成了"我加载 2 个程序集并全量建模"。

## 三、根因（精确定位到"可替换的单元"）

### 3.1 消费侧只需要**单个方法模型**

`ProjectInstantiationMethod`（`GenericInstantiationProjection.cs:384-450`）
的全部需求就是一个 **by-key 的单方法查询**：

```csharp
!definitionMethodsBySubject.TryGetValue(methodReference.DefinitionSubjectId, out var definitionMethod)
```

拿到后只读该方法的：`Parameters` / `ReturnType` / `Body` / 若干 flag。
**不依赖方法所在程序集的其它任何东西。**

### 3.2 供给侧却是整份程序集建模

`definitionMethodsByAssembly` 由（`CrossAssemblyInstantiation.cs:36-38`）：

```csharp
assembly => assembly.Methods.ToDictionary(m => m.SubjectId, ...)
```

而 `assembly.Methods` 来自 `LoadAssembly` 的全量建模
（`AssemblyLoading.cs:29-45`：`LoadTypes` / `LoadFields` / `LoadProperties` /
`LoadMethods` / `BuildGenericInstantiationProjection`）。

### 3.3 且加载是**eager**的

`LoaderStage.LoadMultiple`（`LoaderStage.cs:44-56`）先把 **所有**
`assemblyPaths` 全部 `LoadAssembly`，然后才做投影（`:60`）。

**结论：`LoadMethods` 内部本来就是按方法为单位构造的**
（`AssemblyLoading.cs:404-425`：`DescribeMethodDefinition(...) +
DecodeMethodBody(...)` per `methodHandle`）——
**eager 的外层循环是唯一的阻碍**。

## 四、方案设计

### 4.1 核心思路

把"按程序集全量建模"换成"**按需解析被点名的定义方法**"：

```
阶段 1  只加载 input assembly（subjects DLL），扫出 demand 集合
          → 得到 {定义程序集 → [DefinitionSubjectId...]}          （89 个键）
阶段 2  对每个被点名的定义程序集：打开 PE，**只解析这 N 个方法定义**
          → 构造一个"稀疏"的 definitionMethodsBySubject
阶段 3  用稀疏字典跑既有 ProjectInstantiationMethod（逻辑不变）
```

关键：**第 2 阶段不调用 `LoadAssembly`**，只复用它的**方法级单元**
（`DescribeMethodDefinition` + `DecodeMethodBody`）。

### 4.2 三个必须解决的技术点

#### (a) 如何把 `DefinitionSubjectId` 解析到方法句柄？

`DefinitionSubjectId` 形如：
```
Chaos.TestFramework.Sdk/Chaos.TestFramework.SubjectInstanceFactory::Create`1:!!0()
```

需要「类型全名 + 方法名 + 泛型元数 → MethodDefinitionHandle」的反查。
候选实现：
- **A1**：遍历该程序集 `methodOwners`，按 `DescribeMethodDefinition` 算 SubjectId
  比对 —— **仍是全表扫描**（回到原问题）
- **A2**：**建一次性索引缓存**（type fullname → TypeDefinitionHandle，
  再取其 MethodDefinitions 按名匹配）—— 只遍历命名空间/类型表，
  **不 decode 方法体**。成本远低于全量建模。
- **A3**：PE 层用 `MetadataReader.GetTypeDefinition` 直接按类型定位，
  成本与 A2 同阶。

→ **推荐 A2**：建「类型 → 方法句柄」的轻索引（只读 metadata 表，
不解码 IL），然后**只对命中的方法**跑 `DecodeMethodBody`。

#### (b) `DecodeMethodBody` 的依赖能否满足？—— ✅ **spike 已验证：可以**

**已实测**（2026-09-18，静态分析 `LoaderStage.InstructionDecoding.cs` +
`MetadataResolution.cs` + `AssemblyLoading.cs`）：

三个 owner 字典（`typeModels` / `fieldOwners` / `methodOwners`）
**全程只以"缓存 + 回退"方式使用，从不是硬依赖**：

| 位置 | 用法 | 证据 |
|---|---|---|
| `DescribeMethodDefinition` | `methodOwners.TryGetValue(handle, out ...)` **失败则** `ResolveOwningTypeModel(...)` | `MetadataResolution.cs:66-68` |
| `ResolveOwningTypeModel` | `typeModels.TryGetValue(...)` **失败则** `CreateTypeModel(metadataReader, typeResolver, typeHandle)` | `AssemblyLoading.cs:254-258` |
| `DecodeMethodBody` 本体 | **完全不索引**这三个字典 —— 只从 `peReader.GetMethodBody(rva)` 读 IL | `InstructionDecoding.cs:12-60+`，grep 三个字典在函数体内零命中 |
| 叶子解码器 | 字典只**向下透传**给 `ResolveMethodReference` / `ResolveFieldReference` | `InstructionDecoding.cs:118-146` |

**结论**：**一切都能从 PE metadata 按需重建**。字典的作用是**避免重复解析**，
不是提供唯一数据源。因此「稀疏/空字典 + 按需 `CreateTypeModel`」
在机制上成立 —— **B1 可行**。

> ⚠️ 仍有一个**未实测**点：性能。回退路径 `ResolveOwningTypeModel` →
> `TryResolveOwningTypeHandle` 内部是 **O(全部类型 × 该类型字段)** 的全表扫描
> （`AssemblyLoading.cs:300-316`）。稀疏模式下若频繁走回退，可能慢。
> **但这属于"实现后调优"，不改变可行性判定** —— 且可先建轻索引缓解。

#### (c) 顺序问题：demand 在扫描中产生，但需要先有定义侧

当前 `CollectProjectedCrossAssemblyMethodInstantiations` 是
**边扫边投影**。改为两阶段后，必须：
1. 先扫出**全部** demand（只记录，不投影）
2. 再解析定义、批量投影

这要求把 `TryProjectCrossAssemblyMethodInstantiation` 拆成
「收集」与「投影」两步。**逻辑本身不变，只是重排。**

### 4.3 与闭包的关系（第二重问题，**本任务一并解决**）

⚠️ **即使物化成功，方法仍要进闭包才能被 lower**，而
`LinkerStage.Reachability.cs::ExpandReachableMethods`（:404-474）
**从不查 demand 图** —— 它只沿 `instruction.Callee` 字符串匹配。

**用户已拍板：合并为一个任务**（§11），即本设计**包含**闭包种子改动：

在 `ExpandReachableMethods` 增加一条分支：以 `GenericInstantiationDemandGraph`
的 `definitionSubjectId` 为种子，把对应的投影实例纳入可达集。
这样 B1 产出的闭合实例才不会被 Linker 丢弃。

## 五、方案变体与取舍

| 变体 | 做法 | 优点 | 缺点 |
|---|---|---|---|
| **B1 稀疏按需**（推荐） | 只解析被点名的定义方法 | 成本 ∝ demand 数（89），非程序集大小 | 需 (a)(b)(c) 三点落地 |
| B2 全量但缓存 | 仍全量 `LoadAssembly`，跨 chunk 复用结果 | 改动最小 | 首次仍爆炸；chunk 间缓存复杂；**实测已证明不可行** |
| B3 预生成定义清单 | 离线产出"BCL 泛型定义库"，运行时查表 | 运行时最快 | 需维护离线产物，且仍要解析方法体 |

**推荐 B1**。B2 已被实测否决（5GB/1.8GB 不收敛）；B3 引入新产物且未解决核心问题。

## 六、三优先级权衡

| 优先级 | B1 评估 |
|---|---|
| **P1 性能最优** | ✅ 有益：减少 Loader 工作量（不建模未用类型）。但**生成代码运行时性能不变** —— 只是让原本无法生成的方法能生成 |
| **P2 架构完美** | ✅ 有益：把"加载模型"从 eager 全量改为 lazy 按需，是方向正确的架构演进；且**不破坏**既有模式（复用 `LoadMethods` 的单元） |
| **P3 热更适配** | ➖ 中性：不触碰 hotupdate 路径 |

**无优先级冲突** —— 三个维度方向一致，无需裁决。

## 七、风险评估

| 风险 | 等级 | 触发条件 | 缓解 |
|---|---|---|---|
| **(b) 未知**：稀疏解码依赖全量 type/field 模型 | 🔴 高 | `DecodeMethodBody` 内部访问非局部的 type/field 字典 | **落地前先做 spike 验证**（见 §8 第一步）；若不可行则退化为"只对目标类型建模" |
| SubjectId 反查精度不足（泛型元数/嵌套类型） | 🟡 中 | `Create`1` 这类带反引号的键匹配失败 | 复用 L1 修复（`c419e8385`）已统一的 arity 逻辑 |
| 两阶段重排引入回归 | 🟡 中 | 拆"收集/投影"时改变既有语义 | 保留 `ProjectInstantiationMethod` 原样，只重排调用点；以现有 demand 数为基线对比 |
| 物化后仍进不了闭包（白做） | 🔴 高 | §4.3 —— 已纳入本任务 | 闭包种子与 B1 **同批做**；验证必须端到端（CoreIR 有实例 **且** 生成代码含实例） |
| 闭包种子引入过度可达（包体膨胀） | 🟡 中 | 以 demand 为种子可能拉入超出预期的方法 | 只种子 `definitionSubjectId`（89 个），不做传递闭包扩张；对比生成物体积 |
| 与并发会话冲突 | 🟡 中 | Loader 目录被他人改动 | 开工前核对（本轮已多次踩） |

## 八、落地路径（建议）

**第一步（spike，必须先做）**：验证 §4.2(b) —— 写一个最小实验，
对 `System.Private.CoreLib` **只解析 1 个** `Create`1` 定义方法，
确认 `DescribeMethodDefinition` + `DecodeMethodBody` 能独立跑通。
**若此步失败，整个 B1 不成立，应回到方案 A。**

第二步：实现 (a) 的类型→方法句柄轻索引。
第三步：把 `CollectProjectedCrossAssemblyMethodInstantiations` 拆成
「收集全部 demand」+「批量投影」两阶段。
第四步：**闭包种子** —— `ExpandReachableMethods` 增加 demand 分支（§4.3）。
第五步：端到端验证 —— 以 threading-tasks 的 140 demand 为准，
必须同时观察：(a) CoreIR 出现闭合实例，(b) 这些实例出现在生成代码中。
    ⚠️ 只验 (a) 不够 —— 那正是本任务要求"合并"的原因。

## 九、明确不做（非目标）

- ❌ 不改 ABI 层（L3 的 `System.Char` / `!!0` 降级）—— 独立问题，**注意**：B1+闭包
  成功后，L3 的静默降级才会**真正暴露**（原本进不了 ABI 的实例现在会进），
  因此 L3 需作为**紧随其后的验证项**，而非可有可无
- ❌ 不做跨 chunk 缓存（B2 已被实测否决）
- ❌ 不为每种 T 写 stub（方向错误，组合爆炸）

## 十、问题清零（blocking_questions: []）

| # | 问题 | 结论 | 依据 |
|---|---|---|---|
| 1 | §4.2(b) 稀疏解码是否可行 | ✅ **可行** | spike 已做：三个 owner 字典全程「缓存+回退」语义，`DecodeMethodBody` 本体零索引 |
| 2 | 是否接受"先物化、闭包另立项" | ❌ **不接受** | 用户拍板：**合并为一个任务** —— B1 + 闭包种子一起做 |
| 3 | 改动落点是否可接受 | ✅ 接受 | Loader 新增解析器 + `LoaderStage.cs` + `CrossAssemblyInstantiation.cs` + Linker `Reachability.cs` |

**question_clearance: cleared**

## 十一、合并后的任务范围（用户拍板）

**本任务 = B1（按需物化）+ 闭包种子（Reachability 消费 demand 图）**

即：**必须端到端见效**（泛型实例既进 CoreIR，也能被 lower），
不接受"只做一半"。

两部分的依赖方向：

```
B1（Loader）: 按需解析定义 → 投影出闭合实例 → 进 Methods
                                    ↓
闭包（Linker）: ExpandReachableMethods 增加 demand 种子
              → 让上一步产出的实例进入 closure
                                    ↓
                          codegen 能 lower → 断言可执行
```

**建议实施顺序**：先 B1（无它则闭包无内容可收），再闭包。
但**验证必须端到端**（以 threading-tasks 的 140 demand 为准，
观察 CoreIR 是否出现闭合实例 **且** 这些实例是否出现在生成代码中）。

## 十二、三优先级权衡（结论）

| 优先级 | 评估 | 结论 |
|---|---|---|
| **P1 性能最优** | Loader 工作量下降（不建模未用类型）；生成代码运行时性能不变 | ✅ 无冲突 |
| **P2 架构完美** | eager→lazy 是正确演进方向，复用既有方法级单元 | ✅ 无冲突 |
| **P3 热更适配** | 不触碰 hotupdate 路径 | ➖ 中性 |

**无优先级冲突，无需裁决。**
