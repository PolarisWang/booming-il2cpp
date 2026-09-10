# Codegen 调用模型统一：同模块直接符号调用

> 计划编号: P1-CALL-UNIFY
> 创建日期: 2026-05-17
> 状态: draft

## 问题描述

当前 AOT codegen 生成的 C++ 函数内部，跨 method 调用的解析路径不一致：

- **入口函数 → Subject_X**: `Run` 函数的 switch-case 直接调 `Subject_0()`（直接 C++ 调用，编译器可内联）
- **Subject_X 内部 → 其他 method**: 通过 `kChaosExternalRuntimeFnTable[i]()` 间接调用（即便被调方法就在同一 codegen 产出内）

这导致：
1. 同一模块内的链式调用要多过一层函数指针表
2. 编译器错失内联机会
3. 外部函数表的索引是手工维护的，随 subject 数量线性膨胀

## 目标

让 codegen 对同一产出的 method 之间的调用降级为直接 C++ 符号调用（跳过 extern table），对跨产出的调用保留 extern table。

## 执行步骤

### Step 1: 构建 codegen 内部方法符号表

**入口**: `NativeAotLoweringPlanner.cs`（或新建 `ModuleSymbolTable.cs`）

在 `EmitManagedMethod` 执行前，先遍历当前 assembly 的所有 AOT method，构建 `subjectId → nativeSymbol` 映射表。

```csharp
class ModuleSymbolTable {
    Dictionary<string, string> _subjectToSymbol;
    
    // 注册当前 module 的所有方法
    void RegisterMethods(IEnumerable<AotCoreIrMethodArtifact> methods);
    
    // 查询是否同模块方法
    bool TryResolveLocalSymbol(string subjectId, out string nativeSymbol);
}
```

### Step 2: EmitLinearCall 添加本地符号短路

**入口**: `NativeAotLoweringPlanner.ExceptionEmission.cs`

在 `EmitLinearCall` / `EmitLinearCallVirt` 中，解析 callee 时先查 `ModuleSymbolTable`：

```
calleeSubjectId → ModuleSymbolTable.TryResolveLocalSymbol?
  ├── 命中 → emit: NativeSymbol(args)  (直接调用)
  └── 未命中 → 走现有 extern table / InterpreterDispatchRaw
```

### Step 3: 移除重复的外部表条目

**入口**: `NativeAotLoweringPlanner.cs` 的 `kChaosExternalRuntimeFnTable` 生成逻辑

对外部表的条目进行去重——既然同模块调用不再走 extern table，table 只需包含真正的跨 assembly 外部符号。

### Step 4: 验证

- 验证 subject 的 codegen 产出中，所有 call 指令被正确分类（本地/外部）
- 对 foundation-dll 验证 subject 跑通
- 对比优化前后的 call site C++ 代码

## Test Governance Intake

| 字段 | 值 |
|------|-----|
| capabilityFamily | codegen-method-call |
| capabilityItem | local-symbol-direct-call |
| ownerSubjectId | CodeGen/NativeAotLoweringPlanner |
| proofRequired | foundation-dll call-site 验证 |
| benchmarkRequired | 否（这是正确性，不是性能优化） |
| hotupdateImpact | 无 — hotupdate 走 InterpreterEntryDirect 路径不变 |
| formalVerificationObjects | foundation-dll 中含跨方法调用的 subject（如 convert-char） |
| requiredGates | collector → registry → workspace |

## 终态条件

1. 同 module 的 method 之间的 call 在 C++ 层面不再走 extern table
2. foundation-dll 合约全部通过
3. `kChaosExternalRuntimeFnTable` 只包含跨 module 的外部符号
