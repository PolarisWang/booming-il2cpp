# 泛型共享策略：compile-time sharing determination

> 计划编号: P1-GENERIC-SHARING
> 创建日期: 2026-05-17
> 状态: draft

## 问题描述

当前 AOT codegen 对泛型方法的处理通过 `InstantiationStubId` 生成转发桩：

```
// EmitGenericInstantiationStub 产生的代码:
extern "C" ReturnType GenericNullableValueSubjects_Subject_4_Stub(args) {
    return GenericNullableValueSubjects_Subject_4(args);
}
```

但两个关键问题未解决：

1. **引用类型共享缺失**: `List<string>` 和 `List<object>` 被生成两份相同的 C++ 代码（因为 codegen 为每个封闭泛型类型独立特化）
2. **泛型代码爆炸无管控**: 值类型 `Span<byte>`、`Span<int>`、`Span<double>` 各自独立特化，没有阈值控制

## 目标

实现 compile-time 的泛型共享决策：引用类型参数 → 共享一份代码；值类型参数 → 有限制地特化。

## 执行步骤

### Step 1: 泛型参数分类器

**入口**: 新建 `GenericSharingClassifier.cs`

对每个封闭泛型方法的类型参数做分类：

| 参数类型 | 共享策略 | 示例 |
|---------|---------|------|
| 引用类型 (class/interface/array/string) | SHARED | `List<string>` 和 `List<object>` 走同一份代码 |
| 小型值类型 (≤8 bytes) | SPECIALIZED | `Span<int>` 独立特化 |
| 大型值类型 (>8 bytes) | SPECIALIZED-ON-DEMAND | `Span<MyBigStruct>` 只有实际用到才特化 |
| 泛型参数本身 | SHARED | `T` 在运行时决议 |

```csharp
enum GenericSharingKind { Shared, Specialized, SpecializedOnDemand }

class GenericSharingClassifier {
    GenericSharingKind Classify(AotCoreIrMethodArtifact method);
    bool ShouldShareWith(AotCoreIrMethodArtifact a, AotCoreIrMethodArtifact b);
}
```

### Step 2: InstantiationStubId 增强

**入口**: `NativeAotLoweringPlanner.MethodEmission.cs`

当前 `InstantiationStubId` 是简单转发。改为：

- SHARED: 只生成一个 canonical body，所有共享特化的 stub 转发到该 canonical symbol
- SPECIALIZED: 各自生成独立 body（当前行为，保持不变）
- SPECIALIZED-ON-DEMAND: 添加惰性标记，只在被引用时才生成 body

### Step 3: 跨 module 泛型共享注册

**入口**: `HotpatchNameRegistry` / `module_registry.cpp`

native 侧的 `RegisterHotpatchModule` 需要支持泛型方法的 canonical symbol 注册，让跨 assembly 的共享泛型调用也能解析到正确的 canonical body。

### Step 4: 验证

- foundation-dll 中泛型 subject（如 generic-nullable-value）跑通
- 对比启用共享前后的二进制体积
- 验证跨 assembly 泛型调用正确性

## Test Governance Intake

| 字段 | 值 |
|------|-----|
| capabilityFamily | codegen-generics |
| capabilityItem | compile-time-generic-sharing |
| ownerSubjectId | CodeGen/GenericSharingClassifier |
| proofRequired | foundation-dll generic-nullable-value 合约 |
| benchmarkRequired | 二进制体积对比 |
| hotupdateImpact | 无 — hotupdate 不走泛型共享路径 |
| formalVerificationObjects | foundation-dll 中含泛型调用的 subject |
| requiredGates | collector → registry → workspace |

## 终态条件

1. 引用类型泛型不再生成重复代码（数量级减少）
2. 值类型特化有明确的阈值控制
3. foundation-dll 全部合约通过
4. 输出两个数据：codegen 前泛型方法数 vs codegen 后实际特化数
