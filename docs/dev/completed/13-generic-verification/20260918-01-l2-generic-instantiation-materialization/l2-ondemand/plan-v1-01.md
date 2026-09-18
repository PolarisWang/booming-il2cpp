# L2 按需加载跨程序集泛型定义 — 实现计划

> **面向执行 Agent：** 必须使用 dev:executing-plans 执行本计划。步骤使用复选框
> （`- [ ]`）语法跟踪。本任务**非** roadmap-child，但完成时仍须执行固定收尾链路。

**目标：** 让跨程序集泛型方法（`Assert.AreEqual<T>` / `SubjectInstanceFactory.Create<T>`）
的闭合实例**真正被物化并进入闭包**，从而能被 codegen lower —— 而非落
catch-all 静默通过。

**架构：** 把 Loader 的「按程序集全量建模」改为「按 demand 点名的定义方法做稀疏解析」，
并给 Linker 的可达性分析增加一个以 demand 图为种子的分支。两侧必须同批完成，
端到端才见效。

**技术栈：** C# / .NET 8（Chaos.IL2CPP.Loader、Chaos.IL2CPP.Linker）、
`System.Reflection.Metadata`（PE/Metadata 读取）。

**架构审核模式：** `critical`（触及 loader + codegen 主线）

**结构告警重点：**
- 是否有重复逻辑：新解析器与 `LoadAssembly` 的方法级单元（`DescribeMethodDefinition`
  + `DecodeMethodBody`）必须**复用**，不得复制
- 文件职责：新文件只承载「按 SubjectId 定位并解析定义方法」，不得混入投影策略
- 依赖方向：Loader 内部新增，不得反向依赖 Linker

**权责图审核主题：**
- authority 主体：`ProjectInstantiationMethod`（物化的唯一权威）必须保持**逻辑原样** ——
  本计划只改它的**输入构造方式**（eager 全量 → lazy 按需）
- authority 主体：`LinkerStage.Reachability` 仍是可达性的唯一权威，本计划只**增加一个种子来源**
- 预期 proof：同一组 demand 输入下，投影结果与旧实现**逐项一致**（对可加载的程序集）

**AOT/IL2CPP/Test Governance Intake：**
- capabilityFamily: n/a（不新增 capability family）
- capabilityItem: n/a
- ownerSubjectId: n/a
- proofRequired: true（端到端：CoreIR 出现闭合实例 **且** 生成代码含实例）
- benchmarkRequired: false
- hotupdateImpact: None（不触碰 hotupdate 路径）
- formalVerificationObjects:
  - `generic-instantiation-demand-graph.json` 中 threading-tasks 的 140 条 demand
  - `aot-core-ir.json` 中出现的闭合泛型实例（`Assert::AreEqual<...>` / `Create<...>`）
  - 生成代码 `native-aot.page-*.cpp` 中的对应函数体
- requiredGates: collector -> registry -> workspace（本任务最低要求：pipeline build+fact 通过）

**设计文档：** `docs/dev/in-progress/generic-instantiation-materialization/design-l2-ondemand-v1-01.md`

**问题清零来源：** design-v1-01.md（brainstorm 产出，用户已批准）

**计划来源：** direct-plan

**预期知识沉淀：** `wiki/03-功能模块/il2cpp-核心架构/` 下补「跨程序集泛型物化路径」一节

**收尾约束：** 执行完成后必须进入「结构告警与架构审视 -> 测试通过 -> 归档 completed -> 合并&提交」固定链路。

---

## 前置事实（执行者必读，均已实测）

| 事实 | 值 | 出处 |
|---|---|---|
| threading-tasks demand 数 | 140 | demand-graph |
| 其中 distinct `definitionSubjectId` | **89** | 实测 |
| 目标程序集 | CoreLib 122 / TestFramework.Sdk 11 / Collections 4 / Linq 2 / Threading 1 | 实测 |
| `DecodeMethodBody` 对 owner 字典的依赖 | **仅「缓存+回退」**，函数体零索引 | spike 验证 |
| 阻碍点 | `LoadMultiple` 的 eager 全量 `LoadAssembly` | `LoaderStage.cs:44-60` |
| 闭包阻碍点 | `ExpandReachableMethods` 从不查 demand 图 | `LinkerStage.Reachability.cs:404` |

**关键文件与职责：**

| 文件 | 现有职责 | 本计划改动 |
|---|---|---|
| `LoaderStage.cs` | 加载编排 | 插入「收集 demand → 按需解析定义」两阶段 |
| `LoaderStage.CrossAssemblyInstantiation.cs` | 扫 demand + 投影 | 拆「收集」与「投影」 |
| `LoaderStage.AssemblyLoading.cs` | 全量建模（含 `LoadMethods` 方法级单元） | **不改**，仅复用其单元 |
| `LoaderStage.MetadataResolution.cs` | 定义描述 | **不改**，仅复用 |
| `LinkerStage.Reachability.cs` | 可达性闭包 | 新增 `IncludeGenericInstantiationDemandClosure` |
| **新增** `LoaderStage.OnDemandDefinitionResolver.cs` | — | 按 SubjectId 定位并解析定义方法 |

---

## Task 1: 类型→方法句柄轻索引

**目标：** 给定「程序集路径 + `DefinitionSubjectId`」，能定位到
`MethodDefinitionHandle`，**不解码任何方法体**。

**Files:**
- Create: `src/managed/Chaos.IL2CPP.Loader/LoaderStage.OnDemandDefinitionResolver.cs`

- [ ] **1.1** 实现 `BuildTypeHandleIndex(MetadataReader)`：
  遍历 `metadataReader.TypeDefinitions`，产出
  `Dictionary<string /*typeFullName*/, TypeDefinitionHandle>`。
  **只读类型名与命名空间，不解码签名词表之外的任何东西。**
- [ ] **1.2** 实现 `ResolveDefinitionHandle(MetadataReader, index, string definitionSubjectId)`：
  - 解析 SubjectId 形态：`AssemblyName/Namespace.Type::Method\`N:Signature`
  - 取 `Type::` 前的类型全名 → 查 index
  - 在该类型 `GetMethods()` 中按「名 + 泛型元数」匹配
  - **元数解析必须复用 L1 修复的统一逻辑**（`c419e8385`），
    不得自行实现第二套 arity 计算（结构告警点）
- [ ] **1.3** 单元测试：对 `threading-tasks` 的 89 个 distinct
  `definitionSubjectId` 逐个断言「能解析到句柄」，并断言**不做全量方法建模**
  （用计时或计数器验证：解析 89 个 应显著快于全量 `LoadMethods`）。

**验证命令：**
```bash
cd tests/e2e && python -m pytest verification/tests/ -q
dotnet test tests/unit/managed/codegen/Chaos.IL2CPP.CodeGen.Tests.csproj --filter "FullyQualifiedName~OnDemand"
```

---

## Task 2: 稀疏定义解析（复用既有方法级单元）

**目标：** 产出 `Dictionary<string /*DefinitionSubjectId*/, ManagedMethodModel>`，
只含被点名的方法。

**Files:**
- Modify: `src/managed/Chaos.IL2CPP.Loader/LoaderStage.OnDemandDefinitionResolver.cs`

- [ ] **2.1** 实现 `ResolveDefinitions(assemblyPath, IEnumerable<string> definitionSubjectIds)`：
  - 打开 PE，建轻索引（Task 1）
  - **只对命中的 handle** 调用既有的 `DescribeMethodDefinition` + `DecodeMethodBody`
  - owner 字典给**空字典**（spike 证明会走 `CreateTypeModel` 回退）
- [ ] **2.2** 断言复用而非复制：新文件**不得**出现 `DescribeMethodDefinition` /
  `DecodeMethodBody` 的副本实现（grep 校验，仅允许调用）。
- [ ] **2.3** 若回退路径实测过慢（见风险），加一个**局部** type 缓存；
  仍不得改 `LoadAssembly`。

**⚠️ 已知风险点：** `TryResolveOwningTypeHandle`（`AssemblyLoading.cs:300-316`）
是 O(类型数×字段数) 全表扫描。若 Task 2 实测慢，**先测量再优化**，
不要预设它慢。

**验证命令：** 同 Task 1，另加一次计时对比。

---

## Task 3: 两阶段重排（收集 → 投影）

**目标：** 把「边扫边投影」改成「先收齐 demand，再批量投影」。

**Files:**
- Modify: `src/managed/Chaos.IL2CPP.Loader/LoaderStage.CrossAssemblyInstantiation.cs`
- Modify: `src/managed/Chaos.IL2CPP.Loader/LoaderStage.cs`

- [ ] **3.1** 把 `TryProjectCrossAssemblyMethodInstantiation` 拆为
  `CollectDemand(...)`（只记录）与既有 `ProjectInstantiationMethod`（投影，**不动**）。
- [ ] **3.2** 在 `LoaderStage.LoadMultiple` 中插入按需解析阶段：
  收集到的 `{程序集 → [DefinitionSubjectId]}` → Task 2 的稀疏解析
  → 合并进 `definitionMethodsByAssembly`。
- [ ] **3.3** **回归保护**：对**已加载**的程序集（subjects DLL / TestFramework.Sdk），
  投影结果必须与改动前**逐项一致**。用一个对比测试锁定
  （同一输入 → 同一组 `SubjectId` 集合）。
- [ ] **3.4** 保留原有「目标程序集未加载则静默 return」的行为作为回退 ——
  新的按需路径失败时不得比旧行为更差。

**验证命令：**
```bash
# 端到端第一步：CoreIR 是否出现闭合实例
cd /d/agent/chaos-il2cpp
CHAOS_FOUNDATION_DLL=D:/agent/chaos-il2cpp/testing/foundation-dll \
PYTHONPATH=D:/agent/chaos-il2cpp/tests/e2e \
python tests/e2e/verification/chunk_pipeline.py \
  --assembly System.Private.CoreLib --chunk threading-tasks --stages build
python -c "
import json,collections
d=json.load(open('artifacts/foundation-dll/System.Private.CoreLib/chunks/threading-tasks/native/codegen/generated/aot-core-ir.json',encoding='utf-8'))
c=collections.Counter(m.get('declaringAssemblyName') for m in d['methods'])
print(c.most_common(10))
"
```
**通过判据：** 出现除 `CombinedSubjects` / `Chaos.TestFramework.Sdk` 之外的程序集，
或 subjectId 中出现带 `<` 的闭合实例。

---

## Task 4: 闭包种子（Linker）

**目标：** 让 Task 3 物化出的实例进入可达闭包 —— 否则前功尽弃。

**Files:**
- Modify: `src/managed/Chaos.IL2CPP.Linker/LinkerStage.Reachability.cs`

- [ ] **4.1** 新增 `IncludeGenericInstantiationDemandClosure(ReachabilityContext ctx)`，
  **仿照** `IncludePreservedClosure`（`:120`）的结构：
  从 `ctx.SemanticWorld` 的 demand 图取 `definitionSubjectId`，
  在 `ctx.MethodMap` 中查得方法 → `ReachableMethodIds.Add` + enqueue。
- [ ] **4.2** 在 `ComputeReachableClosure` 的 `Include*` 序列中调用它
  （`LinkerStage.Reachability.cs:62-74` 区块内）。
- [ ] **4.3** **不做传递扩张** —— 只种子 `definitionSubjectId` 本身；
  其 body 引用的其它方法由既有 `ExpandReachableMethods` 自然展开。
- [ ] **4.4** 监控包体：记录改动前后 `chaos_codegen.lib` / page 文件总大小，
  若显著膨胀需在 STATUS 中说明。

**验证命令：** 同 Task 3，但判据升级为见 Task 5。

---

## Task 5: 端到端验证（本计划的验收门）

**目标：** 证明泛型实例**既进 CoreIR，也进生成代码**。

- [ ] **5.1** 跑 `threading-tasks` 全量 `build,fact`。
- [ ] **5.2** 断言 (a)：`aot-core-ir.json` 中出现闭合泛型实例（如
  `Assert::AreEqual<...>` 或 `SubjectInstanceFactory::Create<...>`）。
- [ ] **5.3** 断言 (b)：对应函数体出现在
  `native/codegen/generated/native-aot.page-*.cpp` 中。
  **只验 (a) 不算通过** —— 这正是本任务要求「合并」的原因。
- [ ] **5.4** 对比 fact 结果：`factoryGap` 数应下降（threading-tasks 基线 65）。
  记录实际数字，**不预设降幅**。
- [ ] **5.5** 跑一次 `system` chunk 作为跨 chunk 回归（其 demand 为 311 条，规模更大）。
  ⚠️ 若遇并发会话占用，记录并延后，不得伪造结果。

**⚠️ L3 曝光检查：** 5.4 之后，原本进不了 ABI 的实例现在会进，
可能触发 `System.Char` / `!!0` 静默降级为 `INTPTR`（见设计 §12 L3）。
**观察并记录**，但不在本计划内修 —— 它是独立任务。

---

## Task 6: 收尾

- [ ] **6.1** 结构告警与架构审视：检查是否产生重复 helper、文件职责是否混杂。
- [ ] **6.2** 更新 wiki：`wiki/03-功能模块/il2cpp-核心架构/` 补跨程序集泛型物化路径。
- [ ] **6.3** `STATUS.md` 写入实测数据（demand 物化数 / factoryGap 变化 / 包体变化）。
- [ ] **6.4** 归档：本任务目录移到 `docs/dev/completed/`，
  回写父任务 `generic-instantiation-materialization/STATUS.md`。
- [ ] **6.5** 合并 & 提交。

---

## 回退策略

任一 Task 失败时：
1. **Task 1/2 失败**（定位或解析不通）→ 停止，回报 —— 方案 B1 不成立，退回方案 A（显式失败）。
2. **Task 3 失败**（投影结果与旧不一致）→ 回退到改动前，重新设计两阶段切分。
3. **Task 4 失败**（闭包不收）→ 单独排查 Linker，**不得**为了通过而放宽可达性判据。
4. **Task 5 只通过 (a) 未通过 (b)** → 这是「物化了但没生效」，按
   memory `system-chunk-verification-loop-discipline` 的「多发射点」判别法排查。

## 执行纪律（本轮血泪，务必遵守）

1. **跑 pipeline 前先查并发**：`tasklist | grep -iE "MSBuild|cl\.exe"` 应为 0；
   `git log -1` 确认无他人新提交。本轮已有三次被并发污染的前例。
2. **改 Generator 后必须重建 TPG**，否则新发射静默不生效
   （memory: `tpg-bundles-stale-generator-dll`，本轮踩了两次）。
3. **管线必须带两个环境变量**（`CHAOS_FOUNDATION_DLL` + `PYTHONPATH`）。
4. **验证要看 mtime**：`entry.exe` 必须新于 codegen 产物。
5. **不预设收益** —— 每个 Task 都实测，发现为负立即停止并回报
   （本轮「IL 数据表接线」的教训：先实现后验证，结果全回退）。
