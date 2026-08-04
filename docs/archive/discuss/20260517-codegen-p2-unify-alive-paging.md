# 双发射路径统一 + 活跃方法集 + 分页输出

> 计划编号: P2-UNIFY-ALIVE-PAGING
> 创建日期: 2026-05-17
> 状态: draft

## 问题描述

### P2-A: 双发射路径统一

当前 codegen 有两条独立的 C++ 发射路径：

| 路径 | 入口 | 评价栈 | 控制流 | 编译器优化潜力 |
|------|------|--------|--------|-------------|
| 结构化 IR | `EmitStructuredIRNode` | slot-based `_s0`, `_s1` | if/while/switch/try | 高 |
| Flat Goto | `EmitFlatGotoBody` | 数组 `chaos_eval_stack[top]` | `goto chaos_ip_*` | 低 |

当 `TryBuildStructuredMethodBody` 失败时退化到 Flat Goto。没有量化指标知道结构化恢复的成功率。

### P2-B: 活跃方法集

当前 `CodeRegistration.Registrations` 包含 linkedWorld 中 **全部** 方法 → codegen 为每方法都生成 C++ 函数签名和存储条目。

但实际上，只有从 entry point 出发在 AOT 调用图中可达的方法才需要完整的 C++ 实现。仅从 interpreter 调用的方法只需要 `InterpreterEntryDirect` 入口。

### P2-C: 分页输出

单文件 `native-aot.generated.cpp` 在 subject 方法数 > 几百时会带来编译 OOM 风险。codegen 已有 `AuditTranslationUnitPageSize = 1024` 的分页逻辑（仅 audit 模式），需扩展到真正的 codegen 产出。

## 目标

- P2-A: 将结构化恢复成功率提升到 >95%，消除 flat goto 代码中不可优化的数组栈模式
- P2-B: 只对 AOT-reachable 方法生成完整 C++ body（其余只留 entry）
- P2-C: 多文件编译单元输出

## 执行步骤

### Step P2-A1: 结构化恢复率度量

**入口**: `NativeAotLoweringPlanner.cs` / 新建 `StructuredRecoveryMetric.cs`

在 `EmitViaStructuredIR` 中添加计数器：

```
total_methods = linkedWorld.Methods.Count
structured_ok  = TryBuildStructuredMethodBody 成功数
flat_fallback  = TryBuildStructuredMethodBody 失败数
```

产出报告到 codegen-metrics.json。当前基线："未知"——需先跑一遍得到数据。

### Step P2-A2: 常见 flat-fallback 模式补全

`TryBuildStructuredMethodBody` 的核心 CFG 恢复逻辑在 `NativeAotLoweringPlanner.StructuredIR.cs` 的 `TryBuildStructuredExceptionMethodBody` 链中。

分析 flat-fallback 的前 5 个原因，为每种原因添加 CFG 恢复规则：

| fallback 原因 | 修复策略 |
|-------------|---------|
| 多出口循环 (multi-latch loop) | 扩展 loop header 识别，支持多 latch block |
| Switch-case 落入 (fall-through cases) | 为 C++ switch 不支持的落入模式生成等效 if-else |
| 深层嵌套 EH (depth > 2) | 递归限制在 3+ 层时展开为 IRFlatRegion，而不是放弃整个方法 |
| Finally 内部的控制流 | 分析 finally body 的子 CFG，支持有限模式 |

### Step P2-A3: 移除非必须的 FlatGoto 路径

当 CFG 是可化简的（`ControlFlowGraph.IsReducible == true`）但结构化 EH 恢复失败时，当前退化为 flat goto。改为：

- 如果方法无 EH → 始终走结构化 IR（无异常时 `CanFastExecute` 的判断就是明确的，不需要 flat goto）
- 如果有 EH 但结构化恢复失败 → flat goto（保留当前行为，但记录覆盖率）

### Step P2-B1: AOT 可达性分析

**入口**: 新建 `AotReachabilityAnalysis.cs` / 集成到 `CodeGenStage.cs`

从 entry point subjectId 出发，做 call graph 遍历：

```csharp
HashSet<string> ComputeAotReachableMethods(
    string entrySubjectId, 
    IReadOnlyList<AotCoreIrMethodArtifact> methods)
{
    // BFS from entry point
    // Follow call, callvirt, newobj, ldftn, ldvirtftn targets
    // Record all reachable subjectIds
}
```

只对 reachable 方法生成完整 body。对 unreachable 方法：
- 保留 dispatch table entry（为了让 InterpreterEntryDirect 能找到）
- 跳过 `EmitManagedMethod`

### Step P2-B2: codegen-metrics 扩展

在 `codegen-metrics.json` 中添加输出：

```json
{
    "totalMethods": 100,
    "aotReachable": 45,
    "aotUnreachable": 55,
    "structuredOk": 42,
    "flatFallback": 3
}
```

### Step P2-C1: 翻译单元分页

**入口**: `CodeGenStage.cs` 的 `AuditTranslationUnitPageSize` 扩展到实时 codegen

- 当 subject 方法数 > `TranslationUnitPageSize`(默认 512) 时，拆分到 `native-aot.generated.page-0001.cpp`、`.page-0002.cpp` 等
- 跨页符号声明：同 assembly 的 global 声明集中到 `native-aot.generated.header.h`
- 翻译单元通过 CMake `add_library` 中的多源文件列表组合

### Step P2-C2: 跨页 extern 声明优化

分页后，A 页方法调用 B 页方法需要通过 extern 声明。在每页头部生成 `extern` 声明块：

```
// native-aot.generated.page-0001.cpp
extern ReturnType Subject_X(args);
extern ReturnType Subject_Y(args);
```

## Test Governance Intake

| 字段 | 值 |
|------|-----|
| capabilityFamily | codegen-structural-unification |
| capabilityItem | structured-ir-dominance + live-analysis + multi-file-output |
| ownerSubjectId | CodeGen/NativeAotLoweringPlanner |
| proofRequired | foundation-dll 全部 subjects + metrics json 输出 |
| benchmarkRequired | flat-fallback 率 < 5% |
| hotupdateImpact | 无 |
| formalVerificationObjects | foundation-dll 完整 suite |
| requiredGates | collector → registry → workspace |

## 终态条件

1. 结构化恢复成功率量化已知且 >95%
2. AOT codegen 只产出 reachable 方法的 body
3. codegen-metrics.json 包含完整统计
4. 大 subject 自动分页，编译 OOM 风险消除
