# 泛型实例化物化缺口 — 调查结论

> task_id: `generic-instantiation-materialization`
> 类型：调查已完成（结论：**不是单一缺陷，是两层独立问题**）
> 创建：2026-09-18 ｜ 收尾：2026-09-18
> 前置：`generic-instantiation-demands-not-lowered-to-coreir`（记忆）

## 〇、结论速览

**「122 需求 0 进 CoreIR」不是单一根因，至少两层，且第一层已被并发修复。**

| 层 | 问题 | 状态 |
|---|---|---|
| **L1** | 定义键泛型元数不对称 → 投影 `TryGetValue` 失配 → 静默 return | ✅ **已修** `c419e8385` |
| **L2** | 跨程序集 demand 的目标程序集**未加载**；⚠️ 真凶是 pipeline 配置解析 bug（已修 `842375d7f`），但补喂 BCL 后 codegen 爆炸不收敛 → 更像设计边界 | ⚠️ 见第八节 |
| **L3** | ABI 层会静默把未识别类型（含 `!!0` / `System.Char`）降级为 `NativeInt` | ⏳ 待办（L2 修复后才会暴露） |

## 一、L1 — 定义键泛型元数不对称 ✅ 已修

**提交 `c419e8385`（并发会话，2026-09-18 10:43）**

`LoaderStage.MetadataResolution.cs` 中同一条 `MethodReferenceSummary` 的
两个字段用**不对称**的函数算泛型元数：

| 字段 | 函数 | 跨程序集行为 |
|---|---|---|
| `SubjectId` | `GetMemberReferenceGenericParameterCount` | ✅ 从成员引用签名解码 → 正确带 `` `N `` |
| `DefinitionSubjectId` | `GetDefinitionGenericParameterCount` | ❌ 读 parent handle，跨程序集时 parent 是 **TypeRef 而非 MethodDefinition** → `_ => 0` |

→ 定义键缺 `` `N ``（实为 `...Assert::AreEqual\`1:...`，记为 `...Assert::AreEqual:...`）
→ `ProjectInstantiationMethod` 的 `TryGetValue` 失配（`GenericInstantiationProjection.cs:394`）
→ **静默 return** → 闭合实例从不进 `Methods`。

**修法**：`GetDefinitionGenericParameterCount` 改收 `MemberReference`，
parent 是 `MethodDefinition` 时沿用原路径，否则回退到
`GetMemberReferenceGenericParameterCount` —— 与 `SubjectId` 同源，两键一致。

> 另附修一处 codegen 遗漏：`EmitStructuredMethodReturn` 的 carrier switch
> 缺 `UInt32` case（落 `default` 抛 `NotSupportedException`）。

## 二、L2 — 跨程序集目标程序集未加载 ⏳ 待办

**独立复核（2026-09-18）在真实产物上的测量：**

以 `System.Threading.Tasks.Parallel/chunks/global-ns` 为例：

```
aot-core-ir.json 含 213 个方法，仅来自 2 个程序集：
  CombinedSubjects / Chaos.TestFramework.Sdk
而 27 条 demand 指向的目标程序集：
  System.Collections / System.Linq / System.Private.CoreLib /
  System.Threading.Tasks.Parallel   ← 全都不在 IR 里
27 条 demand 全部 isCrossAssembly: true
```

**机制**：`TryProjectCrossAssemblyMethodInstantiation` 里
**先 track 后 resolve**：

```csharp
// LoaderStage.CrossAssemblyInstantiation.cs
TrackMethodInstantiationDemand(...);   // :150  ← 计数在这里 +1
ProjectInstantiationMethod(...);       // :156  ← 解析失败则静默丢弃
```

所以**计数停在 140 而实际投影为 0** —— 这解释了「有 demand 无产物」的观测。

**关键**：Linker 不参与该路径，且它**没有任何泛型实例化分支**。
`LinkerStage.Reachability.cs` 的 `ExpandReachableMethods`（:404-474）只沿
`instruction.Callee` / `instruction.Reference` 走，且需
`TryResolveMethod` 命中 + `IsInternalAssembly` 通过（:423-429、:447-452）。
**闭包可达性从不查 demand 图** —— 因此投影出的泛型方法只能靠
「某条指令的 Callee 字符串恰好等于其 SubjectId」偶然进入闭包。

> ⚠️ 更正本文档早先版本：原写「Loader 只 merge、Linker 只透传、Lowering 只 enrich，
> 无任何阶段从 Demand 创建 Method」。**该表述不准确** ——
> Loader **确实**会创建（`AssemblyLoading.cs:63-66` 把
> `genericInstantiationProjection.Methods` concat 进 `allMethods`；
> `CrossAssemblyInstantiation.cs:156` 调 `ProjectInstantiationMethod`）。
> 问题是**创建的前提不满足**，不是创建代码不存在。

## 三、L3 — ABI 层静默降级 ⏳ 待办（L2 修好后才会暴露）

`InvocationAbi.Helpers.cs` 的 `CreateLegacyAbiSlot`（:170-250）**按字面名字符串**匹配，
`default` 分支（:243-247）返回 `CreateNativeIntAbiSlot()` ——
**任何未识别类型一律静默变成 `CHAOS_IL2CPP_INTPTR`**。

`AotCoreIrAbiCarrierKind` 枚举里**没有类型参数成员**
（`Void/Int32/Int8/UInt8/Int16/UInt16/UInt32/Float32/Float64/Int64/UInt64/NativeInt/ByRef/MultiReturn`）。

两个具体问题：
1. **`System.Char` 无任何 case** —— `AreEqual<System.Char>` 的 char 参数会变 `INTPTR`
   （对比：`Byte`/`SByte`/`Int16`/`UInt16` 都有 case，唯独漏 `Char`）
2. **`!!0` 可能存活到 ABI 层** —— `CreateSubstitutionMap`
   （`GenericInstantiationProjectionSubstitution.cs:137-154`）只在 demand 带
   `MethodInstantiation` 上下文时替换；实测 `EqualityComparer<!!0>::Equals`
   的 `contextKind: 1`（仅 TypeInstantiation）、`MethodArguments` 为空 →
   `!!0` **原样进入投影后的 SubjectId 与参数类型** → 落 `default` → `INTPTR`

## 四、GenericCapabilityMatrixBuilder 的定位

**它消费 demand 图，但只产出报表，不产出代码。**

- 输入：`GenericInstantiationDemandGraph`（`GenericCapabilityMatrixBuilder.cs:49-54`，枚举于 :63-68）
- 输出：`GenericCapabilityMatrixArtifact`（`Entries` / `FamilyBudgets` / `BoundaryCases` / `Gates`）
- **核心是校验器**：`BuildAuthorityLookup`（:349-409）建期望权威，
  `ValidateObservedAuthority`（:464-501）在观测与 demand 不符时**发射 leaks**
  （`unsupportedAuthorityLeak` / `authorityMismatch`）；
  `EnsureEquivalentObservation`（:503-521）冲突时**直接抛异常**
- `Status = "fail"` 的条件（:339）含 `missingAuthorityCount > 0`

**即：它本来就是为「检测这个缺口」而写的，不是为「修复它」。**
全仓无任何消费者用它的输出去创建方法。

> **这是重要的佐证**：repo 里已有一个专门检测该缺口的组件，
> 它的存在本身就说明「demand 与产物对不上」是被预期的失败模式，而非无人知晓。

## 五、建议路径（更新）

1. **L2 优先** —— 先确认 `--assembly-dir` / 闭包程序集集合为何缺目标程序集：
   是配置问题（少传 `--assembly-dir`）还是设计边界（跨程序集泛型不投影）？
   **若只是配置问题，修复成本极低、收益极大。**
2. **L3 随后** —— 补 `System.Char` 的 ABI case；决定 `!!0` 存活的处理
   （补 case 或让投影保证替换）。
3. **独立该做**（无论 L2/L3 结论）：让落 catch-all 的泛型断言**报错而非静默通过** ——
   `GenericCapabilityMatrixBuilder` 已经在检测，让它的 fail 真正阻断而非仅记录，
   假绿即变真红。

## 六、❌ 明确禁止

**勿按「给每种 T 写 native stub」的思路修** —— 组合爆炸且方向错。
问题在投影/物化的前提条件，不在 stub 层。

## 七、影响面

- `system` chunk：172 处 `Assert.AreEqual<T>` 曾落 catch-all（L1 修后应收窄）
- `threading-tasks`：65 个 factoryGap（`SubjectInstanceFactory.Create<T>`）
- **验证根基**：含泛型断言的 chunk 的 `real` 计数

---

## 八、L2 深挖（2026-09-18）：真凶是 pipeline 配置解析 bug，但实验未能跑完

### 8.1 ✅ 已提交的真缺陷（commit `842375d7f`）

**`_load_pipeline_config` 从不读 `chunks:`** —— 详见该 commit。
后果：`ctx.assembly_dirs` 恒为 `[]`，`--assembly-dir` 从未传给 codegen，
**配置里声明的 assemblyDirs 对所有 chunk 从未生效**。

这直接解释了 L2 的形态：`--assembly-dir` 机制本身是坏的，
所以跨程序集 demand 的目标程序集**从来不会被加载**。

已修（抽出 `_pipeline/pipeline_config.py`，缩进改从原始行算），
新增 8 个单测含对真实 shipped config 的断言，全量 229 passed。

### 8.2 ⚠️ 加载 BCL 的实验：两次均未能跑完

修好解析器后，尝试让 threading-tasks 指向 BCL 程序集目录：

| 尝试 | assemblyDirs | 结果 |
|---|---|---|
| A | `C:/Program Files/dotnet/shared/Microsoft.NETCore.App/10.0.6`（185 个 DLL） | TPG 内存涨到 **5.0 GB**，>40 分钟无 codegen 输出，主动终止 |
| B | 仅 5 个 DLL（CoreLib/Collections/Linq/Threading/Runtime，17 MB） | TPG 稳定在 **1.8 GB**，CPU 燃烧 776s 仍未产出，主动终止 |

**结论：把 BCL 程序集喂给 codegen（作为 `--assembly-dir`）在时间与内存上都不可行。**
TPG 对该规模输入的加载/闭包构造呈爆炸式增长，两次都未收敛。

### 8.3 对 L2 判定的影响

- **不再是「配置少传一个参数」这么简单** —— 修好配置解析后，
  发现**即使正确传入，codegen 也处理不了** BCL 程序集。
- 因此「跨程序集泛型不物化」很可能是**设计边界**（或至少需要
  codegen 侧的架构改动），而非一行配置修复。
- config 注释里写的「Cross-assembly references are resolved by the BRIDGE-AOT
  compiler via runtime directory discovery」**在全仓无对应实现**
  （grep `BRIDGE-AOT` 零命中）—— 该注释描述的是一个**从未落地的设想**。

### 8.4 建议

1. **8.1 的解析器修复独立有价值**（它让配置机制第一次真正生效），已提交。
2. **L2 的下一步不应是「喂 BCL」**，而应先回答：
   codegen 为何对 185 个程序集的输入呈爆炸增长？
   （是否是 `ProjectCrossAssemblyMethodInstantiations` 的组合展开？）
   这是一个**性能/架构问题**，需要单独立项，不适合继续在本任务内试错。
3. 若目标是让泛型断言「不静默通过」，**L3 + 显式化**（让落 catch-all 的
   泛型断言报错而非返 0）成本远低于 L2，且与假绿戒律一致。

---

## 执行结论（2026-09-18 收尾）

### 本任务裁决：方案 B（L2）中断，转向方案 A 的衍生修复

| 项 | 结论 |
|---|---|
| L1 定义键 arity 不对称 | ✅ 并发会话已修（`c419e8385`） |
| L2 跨程序集目标未加载 | ⚠️ 部分修复（T1/T2/T3），**核心前提不成立** |
| 方案 A（假绿→显式失败） | ✅ 前提被推翻，但**发现并修了真缺陷**（见下） |

### B1 执行结果（T1/T2/T3，commits `ef6dadc2b`/`86223ff22`/`9d1623ca3`）

- **机制成立**：按需解析定义方法的三个 owner 字典是「缓存+回退」语义，
  `DecodeMethodBody` 本体零索引 —— spike 证实可行
- **但端到端不可行**：把 CoreLib 文件放到 chunk 可达处后，TPG 对其不收敛
  （同 L2-ANALYSIS 记录的形态）。结论：**「对 CoreLib 的可负担解析代价」
  这个前提不成立**
- **本 chunk 实测**：T3 未新增任何解析（Sdk 的 11 条改动前已在），
  因其目标核心——CoreLib 的 122 条 demand——需要把整个 BCL 纳入输入，
  而该输入爆炸

### 方案 A 的转折（commits `b03893810` / `f679bcdd1`）

原「让 172 处假绿变真红」的前提**机制上错了**：AreEqual<T> 通过 catch-all
**确实抛异常**，被分类为 stubGap（可见）。真正的缺陷是：

**门禁分子分母口径不一致** —— 分子用全量 passed，分母扣除了
factoryGap/stubGap/nullArg/envSensitive。reflection 实测
gateTotal=50 gatePassed=440（880%）：**390 个 stubGap（方法从未执行）
被计入"已验证"**。

修复：`compute_gate_counts` 让分子分母共用同一排除集合，reflection 修正为
50/50。8 个测试锁定（含不变式 `gate_passed <= gate_total`）。

随后修复自身引入的 NameError 回归（`f679bcdd1`，realTotal 读取未定义变量；
这次测试隔离让 bug 逃过，已记录教训）与 importlib 加载（`eb617816d`）。

### 遗留（非本任务范围）

- **闭包方法有槽无体**（`<>c::<...>b__N`）—— 发射/链接缺口，已在
  `il-data-table-not-registered` §8 立案
- created-T 共享体失效 —— 并发会话 T7 立案
- generic-callee-shape-key —— 并发会话已立案（`de08e14c7`）
