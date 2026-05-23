# A-P3-2: G7 Goto Elimination + G8 D3-C Phase 1 实现方案

> **面向执行 Agent：** 必须使用 dev:subagent-driven-development（如果可用子 Agent）或 dev:executing-plans 来执行本计划。步骤使用复选框（`- [ ]`）语法跟踪。执行完成时必须先归档当前子任务目录，再同步父任务。

**目标：** 消除 AOT codegen 中最后的 goto fallback（IRFlatRegion/FlatFallbackCount = 0），并完成 codegen pluginization 第一阶段（所有 NativeReference family handler 接入属性注册 + LoweringPlan/TemplateBundle 插件注册）。

**架构：**
- G7: 用 interval-based CFG restructuring 替换当前 `ApplyNodeSplitting`，修复 `RecoverStructure` 的 residual branch 和 stack underflow 问题，确保全部 6 种 EH shape 不走 fallback
- G8: 将 `NativeReferenceProofEmitter.cs` 中 hand-written 的 `RuntimeSkeleton{Category}FamilyHandlers` 子数组分派迁移到 `[RuntimeSkeletonFamilyPlugin]` 属性注册 + 源码生成器自动构造；同时为 NativeAot product line 添加 `[LoweringPlanPlugin]` 和 `[CodegenTemplateBundlePlugin]` 属性注册

**技术栈：** C# (.NET 8), Scriban 模板引擎, Roslyn source generator

**架构审核模式：** critical（涉及 AOT codegen 主线 + pluginization 架构变更）

**结构告警重点：**
- G7: IRFlatRegion 整个删除（record + s_flatRegionCount + FlatFallbackCount + EmitFlatGotoBody + 所有引用路径）
- G7: ApplyNodeSplitting 替换为 interval-based CFG restructuring（新文件 vs 原地替换）
- G8: NativeReferenceProofEmitter.cs 中 hand-written 的子数组分派器全部替换为 generated array
- G8: 避免重复记录（hand-written 和 attribute 两套共存）

**权责图审核主题：**
- CodeGenPluginRegistryGenerator 产出的是 `partial class` 追加定义，删除 hand-written array 时确保不删除同文件其他代码
- G8 完成后 RuntimeSkeletonFamily 分派权从 NativeReferenceProofEmitter.cs 转移到 CodeGenPluginRegistryGenerator + 各 `TryBuild*Handler` 方法上的 `[RuntimeSkeletonFamilyPlugin]` 属性

**AOT/IL2CPP/Test Governance Intake：**
- capabilityFamily: Codegen
- capabilityItem: GotoElimination + Pluginization
- ownerSubjectId: A-P3-2
- proofRequired: true
- benchmarkRequired: true
- hotupdateImpact: Proof（goto fallback 消除确保热更新路径也不走 goto）
- formalVerificationObjects: [foundation-dll generics families, snapshot test fixture baselines, NativeReference proof tests]
- requiredGates: collector -> registry -> workspace -> verification-pipeline

**设计文档：** n/a（继承自 codegen 工业化 brainstorm-v1-01.md 的设计结论）

**问题清零来源：** parent-roadmap（继承父 roadmap 的 clearance + 用户明确选择 "G7 Goto 消除：B, G8 方案B"）

**计划来源：** roadmap-child-auto

**预期知识沉淀：** n/a（goto 消除是删除代码，pluginization 继承已有模式）

**收尾约束：** 执行完成后必须进入"结构告警与架构审视 -> 测试通过 -> 归档 completed -> 合并&提交"固定链路。

---

## 现状分析

### G7: Goto Elimination

#### 当前 IRFlatRegion 触发路径

在 `TryBuildStructuredMethodBody`（StructuredIR.cs:1545）中，IRFlatRegion 在以下 4 条路径生成：

| # | 触发条件 | 位置 (StructuredIR.cs) | 原因 |
|---|---------|----------------------|------|
| 1 | Exception shape 检测失败 | 1564-1577 | 6 种 EH shape（catch-only/filter-only/finally-only/catch+finally/filter+finally/multi-catch）均不匹配，或 shape 内部 `BuildExceptionPartitionTree` 遇到不可约 CFG 时返回 IRFlatRegion |
| 2 | 非异常 CFG 不可约 + ApplyNodeSplitting 失败 | 1587-1606 | `IsCfgReducible` 返回 false，`ApplyNodeSplitting` 克隆 header 后仍然不可约 |
| 3 | Residual branch terminators | 1609-1618 | `RecoverStructure` 生成的 IR 树中仍有 br/leave 叶子节点 |
| 4 | Eval stack underflow on ret | 1625-1635 | `ComputeMaxEvalStackDepth` 返回 -1（structured emission 使用 monotonic slot counting 导致负值） |

另有 `BuildExceptionPartitionTree`（1957-1989）中的不可约 CFG 回退（`EmitExceptionPartitionFallback`），但它返回的是 IRBlock 而非 IRFlatRegion，不算 goto fallback，但影响代码质量。

#### 相关数据流

```
FlatFallbackCount (NativeAotLoweringPlanner.cs:302)
  ↓ NativeCodegenMetricsBuilder.cs:60 写入 metric
  ↓ NativeAotEmitter.cs:68 打印到日志
  ↓ 当前值 > 0 表示存在 goto fallback

IRFlatRegion record (StructuredIR.cs:61-64)
  └── EmitViaStructuredIR (StructuredIR.cs:1661-1668)
       └── FlatFallbackCount++
       └── _structLocalSlots = IdentifyStructLocalSlots(instructions)
       └── EmitFlatGotoBody(builder, method, ...)  // ExceptionEmission.cs:3319-3502
            └── ~180 行 goto C++ 代码生成
```

#### 当前 ApplyNodeSplitting 的局限

`ApplyNodeSplitting`（StructuredControlFlow.cs:1246-1346）只做了一件事：找到被不自然 backward edge 指向的 block，创建副本并将 forward edge 重定向到副本。这种方法：

- 只能处理单一 entry 被 multiple 不自然 backward edge 指向的情况
- 对 cross-nesting irreducible 模式（A→B→A, B→A→B 的 loop 交织）完全无效
- 无法处理 switch 导致的 irreducible 结构
- 在复杂 CFG 中 node count 暴增后性能差

#### 当前 RecoverStructure 的局限

`RecoverStructure`（StructuredControlFlow.cs:654-891）的主要局限：

- **Merge point 检测**：`FindMergePoint` 只做 forward walk + set intersection，在条件分支嵌套时可能找不到正确的 merge point，导致 residual br/leave
- **Loop exit 检测**：当 loop body 内有提前 exit 时（非 latch 路径离开 loop），`RecoverStructure` 可能把 exit br 残留为 IRBlock 内的 branch
- **Switch 处理**：`BuildSwitch` 在 switch target 本身有内部分支结构时可能解析不完整
- **自循环条件分支**：已针对 "conditional branch targets its own block" 做特殊处理，但未覆盖复合自循环

### G8: D3-C Phase 1 Pluginization

#### 当前 Family Dispatch 架构

```
TryBuildAssemblyFullClosureRuntimeSkeletonMethodStubVia4CDispatcher (NativeReferenceProofEmitter.cs:1766)
  └── foreach RuntimeSkeletonFamilyHandlers (generated array, 来自 CodeGenPluginRegistryGenerator)
       └── 每个 handler 调用 MatchKind 检查

各子分派器（手写的 sub-array 分派）：
  TryBuildRuntimeSkeletonConvertFamilyHandler →  RuntimeSkeletonConvertLikeFamilyHandlers[16]
  TryBuildRuntimeSkeletonBindingFamilyHandler →  RuntimeSkeletonBindingFamilyHandlers[5]
  TryBuildRuntimeSkeletonUtilityFamilyHandler →  RuntimeSkeletonUtilityFamilyHandlers[3]
  TryBuildRuntimeSkeletonPlatformFamilyHandler → RuntimeSkeletonPlatformFamilyHandlers[2]
  TryBuildRuntimeSkeletonInteropFamilyHandler →  RuntimeSkeletonInteropFamilyHandlers[3]
  TryBuildRuntimeSkeletonAsyncFamilyHandler →    RuntimeSkeletonAsyncFamilyHandlers[3]
  TryBuildRuntimeSkeletonArrayFamilyHandler →    RuntimeSkeletonArrayFamilyHandlers[4]
  TryBuildRuntimeSkeletonExceptionFamilyHandler → RuntimeSkeletonExceptionFamilyHandlers[5]
  TryBuildRuntimeSkeletonStringFamilyHandler →    RuntimeSkeletonStringFamilyHandlers[19]
  + ConvertLike 子分派
```

问题：
1. `RuntimeSkeletonFamilyHandlers` 由 Source Generator 在编译时生成，但当前没有方法标注 `[RuntimeSkeletonFamilyPlugin]` 属性，因此 generated array 为空
2. 实际分派通过 9 个子分派器 + hand-written sub-arrays 完成，绕过 generated registry
3. sub-array 分派器与 generated array 分派器共存但互不通信，形成两层分派
4. NativeAot product line 完全没有 plugin 注册（`LoweringPlanPlugin` / `CodegenTemplateBundlePlugin` / `RuntimeProviderPlugin`）

---

## 实现步骤

### Phase 1: G7 Goto Elimination — Interval-based CFG Restructuring

#### Step 1.1: 实现 Interval Analysis（新增文件或原地替换）

**文件：** `src/managed/Chaos.IL2CPP.Generator/Emission/NativeAotLoweringPlanner.IntervalAnalysis.cs`（新建）

**内容：**
- `Interval` record：`{int HeaderIndex, HashSet<int> BodyIndices, int? ImmediateOuterInterval}`
- `BuildIntervalGraph(ControlFlowGraph cfg)`：自底向上构建 T1-T2 间隔图
  - T1 变换：如果节点 i 到 i 的所有边都是自循环，则 i 自身构成间隔
  - T2 变换：如果节点 j 有唯一前驱且该前驱在间隔内，则 j 加入间隔
  - 反复应用 T1/T2 直到没有节点可以加入
  - 输出：`List<Interval>`（间隔列表）+ 间隔图（interval → outer_interval 映射）
- `IsIntervalGraphReducible(List<Interval> intervals)`：检查间隔图是否可归约
- `SplitIrreducibleInterval(ControlFlowGraph cfg, int headerIndex, HashSet<int> bodyIndices)`：对不可约间隔进行节点分裂
  - 基于 interval header 的非独占前驱进行克隆
  - 新节点继承原节点的所有出边
  - 前驱中在 interval body 内的保留指向原节点，在 interval body 外的指向克隆

**关键设计决策：**
- Interval analysis 替代 `ApplyNodeSplitting`（而不是在其基础上增强），因为 interval-based 方法天然处理 cross-nesting
- 保留 `IsCfgReducible` 作为快速检查（O(n)），只有不可约时才进入 interval analysis + splitting
- `BuildIntervalGraph` 输入是已经构建好的 `ControlFlowGraph`，不需要重新解析 IL

**方法签名：**
```csharp
// Interval analysis entry point. Returns a modified CFG that is reducible.
private static ControlFlowGraph MakeCfgReducibleViaIntervalAnalysis(ControlFlowGraph cfg)
```

**调用替换：**
```csharp
// 替换 StructuredIR.cs:1587-1606
// if (!cfg.IsReducible) {
//     var splitCfg = ApplyNodeSplitting(cfg);  // ← 删除此行
//     var splitCfg = MakeCfgReducibleViaIntervalAnalysis(cfg);  // ← 替换为
//     ...
// }
```

#### Step 1.2: Fix Exception Shape Detection Gap

**文件：** `src/managed/Chaos.IL2CPP.Generator/Emission/NativeAotLoweringPlanner.StructuredIR.cs`

**分析：** `TryBuildStructuredExceptionMethodBody`（1691-1761）按顺序检查 6 种 EH shape。如果全不匹配，返回 `false`，导致外层 `TryBuildStructuredMethodBody` 生成 IRFlatRegion。

**修改：**
- 在 6 种 shape 全部不匹配后，新增最后一个 fallback：`TryBuildGenericExceptionMethodShape`
  - 解析 `method.ExceptionRegions` 元数据中的 try-catch-finally 组织
  - 按原始 EH 元数据结构生成 IRExceptionRegion 的嵌套结构
  - 各 partition 内部调用 `BuildExceptionPartitionTree` 处理结构化控制流
  - 不需要精确的 shape 识别，只需要正确的嵌套关系
- 这是安全网：理论上 6 种 shape 应覆盖全部 EH 模式，但实践中可能出现编译器生成的非常规 EH 结构

**EH shape 覆盖验证：**
```csharp
// 在 TryBuildStructuredExceptionMethodBody 末尾新增
if (TryBuildGenericExceptionMethodShape(method, out var genericShape) && genericShape is not null)
{
    body = BuildGenericShapeExceptionIRBody(genericShape, offsets);
    extraHandlerPushes = genericShape.HandlerPushes;
}
```

#### Step 1.3: Fix Residual Branch Terminators

**文件：** `src/managed/Chaos.IL2CPP.Generator/Emission/NativeAotLoweringPlanner.StructuredControlFlow.cs`

**根本原因分析：** `ContainsResidualBranchTerminators` 检查 IR 树中是否有 `IRBlock { Terminator: { Op: "br" or "leave" } }`。residual branch 的出现意味着 `RecoverStructure` 无法将该 branch 映射到结构化控制流（如 `IRBreak`、`IRContinue`、`IRIfThenElse` 的 fallthrough）。

**修复方案：**

1. **改进 `FindMergePoint`**（StructuredControlFlow.cs:499-531）
   - 当前实现：forward walk + set intersection
   - 改进：添加 blocked-based merge 检测，考虑嵌套结构中的 merge 点
   - 对 if-then-else 后的代码，即 else block 后的第一个 shared block

2. **增强 `RemoveTrailingBranch`**（StructuredControlFlow.cs:573-605+）
   - 当前覆盖：IRBlock, IRSequence, IRIfThenElse
   - 增加覆盖：IRWhileLoop, IRDoWhileLoop, IRSwitch, IRExceptionRegion
   - 在 `RecoverStructure` 递归构建中，对每个子结构调用 `RemoveTrailingBranch`

3. **改进 loop exit 识别**（StructuredControlFlow.cs 中的 BuildLoop 以及 RecoverStructure 中的 br/leave 处理）
   - 当前：loopExitOffset 只能匹配单一出口 block 的 offset
   - 改进：支持 `HashSet<int> loopExitOffsets`，允许 loop 有多个非 latch 出口
   - 在 `RecoverStructure` 中将 `int? loopExitOffset` 改为 `IReadOnlySet<int>? loopExitOffsets`

4. **新增：Post-structural pass** — 在 `RecoverStructure` 完成后，运行 `ConvertResidualBranches` pass
   - 扫描 IR 树中的 residual br/leave IRBlock
   - 检查 target offset 是否指向当前 IR 树中某个可识别的 exit 点
   - 如果是，替换为 IRBreak/IRContinue
   - 如果不属于任何 loop，替换为 IRReturn（结构性 return）

**风险缓解：** `ContainsResidualBranchTerminators` 仍然保留作为 assert-only check（仅在 DEBUG 构建中启用），不再作为 IRFlatRegion 的 gate。

#### Step 1.4: Fix Stack Underflow on Ret

**文件：** `src/managed/Chaos.IL2CPP.Generator/Emission/NativeAotLoweringPlanner.StructuredIR.cs`

**根本原因：** `ComputeMaxEvalStackDepth` 的 monotonic 模式下，eval stack slot 在 branch 边界不会重用。当 `ComputeMaxEvalStackDepth(instructions, method.ReturnAbi)` 返回 -1，表示 return ABI 检查发现 stack 深度不匹配。

**修复方案：**
- 在 `TryBuildStructuredMethodBody` 中，将 `maxDepth < 0` 的处理从"fallback to IRFlatRegion"改为"fallback to non-monotonic computation"

```csharp
// 替换 StructuredIR.cs:1625-1641
maxDepth = ComputeMaxEvalStackDepth(instructions, method.ReturnAbi);
if (maxDepth < 0)
{
    // Non-monotonic computation (slot reuse allowed) — may produce slightly
    // larger eval stack than strictly needed for structured emission, but
    // guarantees correctness without goto fallback.
    maxDepth = ComputeMaxEvalStackDepth(instructions, monotonic: false);
    // Log but don't fallback to IRFlatRegion
    LogStructuredMethod(method, "stack-depth-fixup", instructions.Count,
        cfg.Blocks.Count, cfg.LoopHeaders.Count, method.ExceptionRegionCount);
}
else
{
    maxDepth = ComputeMaxEvalStackDepth(instructions, monotonic: true);
}
```

**风险分析：** Non-monotonic stack depth 可能导致 GC slot map 略微偏大（upper bound 而非 exact），但不影响正确性。GC 扫描基于 slot map bitmap，多出的 slot 只会导致扫描范围略大，不会产生 false positive。

#### Step 1.5: 删除 IRFlatRegion 及相关代码

**文件：** `src/managed/Chaos.IL2CPP.Generator/Emission/NativeAotLoweringPlanner.StructuredIR.cs`

**删除内容：**
- `IRFlatRegion` record（line 61-64）
- `s_flatRegionCount` static field（line 2022）
- `s_flatRegionCount` 的所有 `Interlocked.Increment` 调用（5 处）
- `FlatFallbackCount` 的引用（但保留字段本身为 0，兼容 metrics 输出）

**文件：** `src/managed/Chaos.IL2CPP.Generator/Emission/NativeAotLoweringPlanner.StructuredIR.cs` 中 `EmitViaStructuredIR`

**修改：** 删除 IRFlatRegion 分支（line 1661-1668）：
```csharp
// 删除整个 if (body is IRFlatRegion flatRegion) 块
// 直接进入 _structLocalSlots = IdentifyStructLocalSlots(instructions);
// 但注意要保留 _structLocalSlots 初始化（structured 分支也需要）
```

实际上，`_structLocalSlots` 初始化需要保留在 structured 分支之前。修改后的 `EmitViaStructuredIR`：
```csharp
TotalMethodCount++;

_structLocalSlots = IdentifyStructLocalSlots(instructions);
try
{
    // ...结构化发射...
}
finally
{
    _structLocalSlots = null;
}
```

**文件：** `src/managed/Chaos.IL2CPP.Generator/Emission/NativeAotLoweringPlanner.ExceptionEmission.cs`

**删除内容：**
- `EmitFlatGotoBody` 整个方法（line 3319-3502 及所有 helper）
- `FilterRedundantStoreReloadPairs` helper
- `ConsumeEvalStackValueExpression` 的 goto-flavor（如果有独立版本）

**文件：** `src/managed/Chaos.IL2CPP.Generator/NativeCodegenMetricsBuilder.cs`

**修改：** 保留 `FlatFallbackCount` 字段（用于向前兼容 JSON schema），始终设为 0。

#### Step 1.6: 改进 Exception Partition Fallback

**文件：** `src/managed/Chaos.IL2CPP.Generator/Emission/NativeAotLoweringPlanner.StructuredIR.cs`

**修改：** `EmitExceptionPartitionFallback`（line 1997+）当前返回单个 IRBlock 包含所有指令。原因是在异常分片中，branch target 可能指向分片外的指令，导致 CFG 不可约。改进为：
- 尝试使用 `MakeCfgReducibleViaIntervalAnalysis` 使 partition 的 CFG 可约
- 如果仍然不可约，保留现有 fallback（IRBlock），但不触发 `s_flatRegionCount`

**注意：** Exception partition fallback 不产生 `goto` 输出，只产生无标签的线性指令序列。不删除此 fallback，只在 interval analysis 应用后减少其触发频率。

### Phase 2: G8 Pluginization

#### Step 2.1: 添加 [RuntimeSkeletonFamilyPlugin] 属性

**文件：** `src/managed/Chaos.IL2CPP.Generator/NativeReferenceProofEmitter.cs`

对以下 60+ handler 方法添加 `[RuntimeSkeletonFamilyPlugin]` 属性：

| Handler Method | ID | Kind |
|---------------|-----|------|
| TryBuildRuntimeSkeletonConvertLikeIntForwarderHandler | "convert-like-int-forwarder" | ManagedInvoke |
| TryBuildRuntimeSkeletonConvertLikeBoolIdentityForwarderHandler | "convert-like-bool-identity" | ManagedInvoke |
| ...ConvertLike 全部 16 个 | ... | ManagedInvoke |
| TryBuildRuntimeSkeletonConstructorFieldSetterHandler | "binding-ctor-field-setter" | ManagedInvoke |
| ...Binding 全部 5 个 | ... | ManagedInvoke |
| TryBuildRuntimeSkeletonStaticVoidObjectSinkNoOpHandler | "utility-void-sink-noop" | ManagedInvoke |
| ...Utility 全部 3 个 | ... | ManagedInvoke |
| TryBuildRuntimeSkeletonInterfaceDispatchMessageHandler | "platform-interface-dispatch" | Kernel |
| TryBuildRuntimeSkeletonThreadingThreadStaticMonitorHandler | "platform-thread-static-monitor" | Kernel |
| TryBuildRuntimeSkeletonMarshalingUtf8ExportHandler | "interop-marshal-utf8-export" | ImportedBridge |
| ...Interop 全部 3 个 | ... | ImportedBridge |
| TryBuildRuntimeSkeletonAsyncTaskFactoryHandler | "async-task-factory" | ManagedInvoke |
| ...Async 全部 3 个 | ... | ManagedInvoke |
| TryBuildRuntimeSkeletonArrayBoxingReferenceArrayHandler | "array-boxing-ref" | ManagedInvoke |
| ...Array 全部 4 个 | ... | ManagedInvoke |
| TryBuildRuntimeSkeletonStaticExceptionThrowLiteralHandler | "exception-throw-literal" | ManagedInvoke |
| ...Exception 全部 5 个 | ... | ManagedInvoke |
| TryBuildRuntimeSkeletonStaticStringReturnForwarderHandler | "string-return-forwarder" | ManagedInvoke |
| ...String 全部 19 个 | ... | ManagedInvoke |

**属性格式示例：**
```csharp
[RuntimeSkeletonFamilyPlugin(
    "convert-like-int-forwarder",
    CodeGenPluginProductLine.NativeReference,
    100,
    RuntimeSkeletonFamilyPluginKind.ManagedInvoke)]
private static RuntimeSkeletonFamilyHandlerResult TryBuildRuntimeSkeletonConvertLikeIntForwarderHandler(
    RuntimeSkeletonStubBuildContext buildContext) =>
    TryBuildRuntimeSkeletonIntForwarderForType(buildContext, "System.Int32", "System.Int32");
```

**Ordering 策略：**
- ConvertLike: 100-115 (order by sub-category)
- Binding: 200-204
- Utility: 300-302
- Platform: 400-401
- Interop: 500-502
- Async: 600-602
- Array: 700-703
- Exception: 800-804
- String: 900-918

#### Step 2.2: 删除 Hand-written Sub-Array 分派

**文件：** `src/managed/Chaos.IL2CPP.Generator/NativeReferenceProofEmitter.cs`

**删除以下数组定义和子分派器：**
- `RuntimeSkeletonConvertLikeFamilyHandlers` 数组（line 271-290）
- `RuntimeSkeletonBindingFamilyHandlers` 数组（line 292-299）
- `RuntimeSkeletonUtilityFamilyHandlers` 数组（line 301-306）
- `RuntimeSkeletonPlatformFamilyHandlers` 数组（line 308-312）
- `RuntimeSkeletonInteropFamilyHandlers` 数组（line 314-319）
- `RuntimeSkeletonAsyncFamilyHandlers` 数组（line 321-326）
- `RuntimeSkeletonArrayFamilyHandlers` 数组（line 328-334）
- `RuntimeSkeletonExceptionFamilyHandlers` 数组（line 336-343）
- `RuntimeSkeletonStringFamilyHandlers` 数组（line 345-366）

**删除以下子分派方法：**
- `TryBuildRuntimeSkeletonConvertFamilyHandler`（line 1781-1783）
- `TryBuildRuntimeSkeletonBindingFamilyHandler`（line 1785-1798）
- `TryBuildRuntimeSkeletonUtilityFamilyHandler`（line 1800-1812）
- `TryBuildRuntimeSkeletonPlatformFamilyHandler`（line 1815-1828）
- `TryBuildRuntimeSkeletonInteropKernel32PlatformCapabilityFamilyHandler`（line 1830-1832）
- `TryBuildRuntimeSkeletonInteropFamilyHandler`（line 1834-1847）
- `TryBuildRuntimeSkeletonAsyncFamilyHandler`（line 1849-1862）
- `TryBuildRuntimeSkeletonArrayFamilyHandler`（line 1864-1877）
- `TryBuildRuntimeSkeletonExceptionFamilyHandler`（line 1879-1897）
- `TryBuildRuntimeSkeletonConvertLikeFamilyHandler`（line 1899+）
- 其他类似子分派器

**保留：** `TryBuildAssemblyFullClosureRuntimeSkeletonMethodStubVia4CDispatcher`（line 1766-1779）不变，因为 `RuntimeSkeletonFamilyHandlers` 现在由 Source Generator 正确填充。

#### Step 2.3: 添加 LoweringPlanPlugin 属性

**文件：** `src/managed/Chaos.IL2CPP.Generator/Emission/NativeAotLoweringPlanner.GenericSharing.cs` 及相关文件

在 NativeAot 的 lowering plan 扩展方法上添加 `[LoweringPlanPlugin]` 属性：

```csharp
[LoweringPlanPlugin("generic-sharing", CodeGenPluginProductLine.NativeAot, 100)]
public void PlanGenericSharing(...) { ... }
```

目前 NativeAotLoweringPlanner 的 lowering plan 通过 `Create` 方法中的顺序调用组织。PlugInization 第一阶段不需要改变调用顺序，只需要注册元数据。

**要注册的 LoweringPlan：**
| 方法 | ID | Order | 所属文件 |
|------|----|-------|---------|
| EmitRuntimePrelude | "runtime-prelude" | 0 | NativeAotLoweringPlanner.cs |
| EmitGenericRegistration | "generic-registration" | 100 | GenericRegistration.cs |
| PlanGenericSharing | "generic-sharing" | 200 | GenericSharing.cs |
| EmitManagedMethod | "managed-method" | 300 | MethodEmission.cs |
| EmitMetadataRegistrationGenericFields | "metadata-generic-fields" | 400 | GenericRegistration.cs |

#### Step 2.4: 添加 CodegenTemplateBundlePlugin 属性

**文件：** `src/managed/Chaos.IL2CPP.Generator/Templates/` 下的 Scriban 模板（如果已有对应的类定义）

目前 Scriban 模板通过 `NativeAotTemplateCatalog` 静态方法提供。如果有对应的模板 bundle 类，添加 `[CodegenTemplateBundlePlugin]` 属性。如果没有独立类，此步骤可以跳过（pluginization Phase 1 不要求重构模板加载方式）。

### Phase 3: 验证

#### Step 3.1: Codegen Compilation Test

```bash
cd src/managed
dotnet build Chaos.IL2CPP.Generator
```

验证编译通过，无 warning。

#### Step 3.2: Foundation-DLL Generics Family Verification

```bash
cd testing/foundation-dll
python _core/python/cli.py collections-generic-core
python _core/python/cli.py generic-nullable-value
python _core/python/cli.py reflection-generics
```

验证全部 passing。

#### Step 3.3: Foundation-DLL Snapshot Verification

```bash
cd testing/foundation-dll
python _core/python/cli.py snapshot-test
```

验证 snapshot baselines 不因 goto elimination 而异常变化。如果 baseline 变化（可预期的，因为结构化发射与 goto 发射的 C++ 代码不同），更新 baseline。

#### Step 3.4: FlatFallbackCount = 0 验证

构建输出日志中 `FlatFallbackCount` 应为 0。在 `NativeAotEmitter.cs:68` 添加 assert：
```csharp
System.Diagnostics.Debug.Assert(planner.FlatFallbackCount == 0, "Goto fallback must be eliminated");
```

#### Step 3.5: NativeReference Proof Tests

```bash
cd testing/foundation-dll
python _core/python/cli.py <any-native-reference-family>
```

验证 NativeReference proof 路径不受 pluginization 影响。

---

## 文件修改清单

| 文件 | 修改类型 | G7/G8 | 修改内容 |
|------|---------|-------|---------|
| `src/managed/Chaos.IL2CPP.Generator/Emission/NativeAotLoweringPlanner.IntervalAnalysis.cs` | **新增** | G7 | Interval analysis (T1-T2) + interval-based CFG restructuring |
| `src/managed/Chaos.IL2CPP.Generator/Emission/NativeAotLoweringPlanner.StructuredIR.cs` | 修改 | G7 | 删除 IRFlatRegion, s_flatRegionCount；修复 TryBuildStructuredMethodBody 中 4 条 IRFlatRegion 路径；修复 TryBuildStructuredExceptionMethodBody 添加 generic shape fallback；改进 EmitExceptionPartitionFallback；修复 EmitViaStructuredIR |
| `src/managed/Chaos.IL2CPP.Generator/Emission/NativeAotLoweringPlanner.StructuredControlFlow.cs` | 修改 | G7 | 替换 ApplyNodeSplitting → MakeCfgReducibleViaIntervalAnalysis；改进 FindMergePoint；改进 RemoveTrailingBranch 循环覆盖；loop exit 支持多出口；新增 ConvertResidualBranches pass |
| `src/managed/Chaos.IL2CPP.Generator/Emission/NativeAotLoweringPlanner.ExceptionEmission.cs` | 修改 | G7 | 删除 EmitFlatGotoBody, FilterRedundantStoreReloadPairs |
| `src/managed/Chaos.IL2CPP.Generator/NativeAotLoweringPlanner.cs` | 修改 | G7 | FlatFallbackCount 保留字段但不再被更新 |
| `src/managed/Chaos.IL2CPP.Generator/NativeCodegenMetricsBuilder.cs` | 修改 | G7 | FlatFallbackCount 始终设为 0 |
| `src/managed/Chaos.IL2CPP.Generator/NativeAotEmitter.cs` | 修改 | G7 | 添加 Debug.Assert(FlatFallbackCount == 0) |
| `src/managed/Chaos.IL2CPP.Generator/NativeReferenceProofEmitter.cs` | 修改 | G8 | 60+ handler 添加 [RuntimeSkeletonFamilyPlugin]；删除 9 个 hand-written sub-arrays；删除 10+ 子分派器方法 |
| `src/managed/Chaos.IL2CPP.Generator/Emission/NativeAotLoweringPlanner.GenericSharing.cs` | 修改 | G8 | 添加 [LoweringPlanPlugin] 属性 |
| `src/managed/Chaos.IL2CPP.Generator/Emission/NativeAotLoweringPlanner.MethodEmission.cs` | 修改 | G8 | 添加 [LoweringPlanPlugin] 属性 |
| `src/managed/Chaos.IL2CPP.Generator/Emission/NativeAotLoweringPlanner.GenericRegistration.cs` | 修改 | G8 | 添加 [LoweringPlanPlugin] 属性 |

---

## 风险评估与缓解

| 风险 | 概率 | 影响 | 缓解措施 |
|------|------|------|---------|
| Interval analysis 新增 bug 导致 CFG 还原出错的 C++ 编译失败 | 中 | 高 | Step 1.1 在 interval analysis 通过后保留 IsCfgReducible 双重验证；添加详细 log 输出 |
| Residual branch 修复后仍存在漏网分支 | 低 | 中 | ConvertResidualBranches post-pass 作为最后的安全网 |
| Generic exception shape fallback 生成的 IR 树结构不正确导致运行时崩溃 | 中 | 高 | 仅在 6 种标准 shape 都不匹配时触发；在 foundation-dll 中覆盖所有 EH family |
| Stack underflow 修复导致 eval stack 偏大增加 GC 扫描范围 | 低 | 低 | GC slot map 采用 bitmap 扫描，额外 slot 不会导致 false positive |
| Pluginization 属性标注遗漏某个 handler 导致 proof 测试失败 | 中 | 高 | Step 2.2 删除 hand-written array 前置条件：所有 handler 均已标注属性并编译通过；在测试中验证对应 family 的 proof |
| 生成的 RuntimeSkeletonFamilyHandlers 顺序与原始 hand-written 数组顺序不同 | 低 | 中 | order 字段控制排序；源码生成器默认按 order 排序 |
| Baseline 因结构化输出变化而大规模变更 | 中 | 中 | 预期行为（goto vs 结构化 C++ 输出不同），需要更新 baseline |
| Assert(FlatFallbackCount == 0) 在 release build 中不生效 | 低 | 低 | 改用 CHAOS_IL2CPP_LOG_WARN 输出警告 |

---

## 退出条件

1. **所有 IRFlatRegion 相关代码已删除**（record, s_flatRegionCount, EmitFlatGotoBody, FilterRedundantStoreReloadPairs）
2. **FlatFallbackCount = 0**（通过 codegen metrics 日志验证）
3. **所有 foundation-dll generics family 全部通过**（collections-generic-core, generic-nullable-value, reflection-generics）
4. **snapshot test baselines 更新并全部通过**
5. **NativeReference proof 测试全部通过**
6. **NativeReferenceProofEmitter.cs 中 hand-written sub-arrays 和子分派器全部删除**
7. **所有 handler 方法已标注 [RuntimeSkeletonFamilyPlugin] 属性**
8. **LoweringPlanPlugin 已注册**（GenericSharing, MethodEmission, GenericRegistration）
9. **`dotnet build Chaos.IL2CPP.Generator` 零 warning 通过**

---

## 执行顺序建议

```
Phase 1 (G7):  1.1 → 1.2 → 1.3 → 1.4 → 1.5 → 1.6
Phase 2 (G8):  2.1 → 2.2 → 2.3 → 2.4
Phase 3 (验证): 3.1 → 3.2 → 3.3 → 3.4 → 3.5
```

G7 和 G8 可以并行执行（修改的文件不同），但建议串行以减少上下文切换。
