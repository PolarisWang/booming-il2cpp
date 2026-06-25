# DataFlow Pipeline 设计方案

## 目标

将 Create() 方法中的顺序阶段连接为 DataFlow pipeline，利用 CPU 多核并行执行无依赖的阶段。

## 当前架构

```
Phase 0 ─→ Phase 1 ─→ Phase 2 ─→ Phase 3-4 ─→ Phase 5
(串行)    (串行)      (串行)      (并行主体)   (部分并行)
```

各阶段之间有数据依赖：
- Phase 0 产出: customAttributeSupport, assemblyReflectionSupport, reflectionMemberSupport, staticFieldDataSupport
- Phase 1 产出: enumTypeSubjectIds, enumValueToNameMap, staticInitializationSupport, externalRuntimeHelpers
- Phase 2 产出: objectModelCode, genericRegistrationHelperCode, aotRegistrationCode
- Phase 3-4 产出: methodBody 字符串
- Phase 5 产出: moduleRegistrationCode, codeRegistrationCode, globalDeclarations

## 可并行化的边界

Phase 3-4 (method_bodies) 已经是并行的。Phase 5 已部分并行。

**剩余的串行瓶颈在 Phase 0 → Phase 2：**
- Phase 0 的 4 个 BuildModel 调用理论上可以并行（只读 methodsForLowering）
- Phase 2 的 EmitObjectModelDeclarations 依赖 Phase 1 的全部产出，暂无法拆分

## 实施计划

### Phase 0 并行化

```csharp
// 当前: 串行调用 4 个 BuildModel
// 问题: 之前遇到过 heap corruption (0xC000037D)
// 结论: 暂不并行化，根因未找到
```

### Phase 1 → Phase 2 异步 pipeline

```csharp
// 当前 Phase 2 在 Phase 1 之后完全串行
// Phase 2 可以异步在后台线程上启动某些工作

// 简化方案:
// Phase 1 完成后，Phase 2 的 EmitObjectModelDeclarations 串行执行
// 但 Phase 0-1 的 4 个 BuildModel + enum 收集可以作为单向 pipeline
```

### 推荐实现: 最小改动

由于 Phase 0 heap corruption 风险未知，Phase 2 强依赖 Phase 1，DataFlow pipeline 在当前架构下的收益上限约 0.5-1s，不值得大改。

**建议搁置 Step3，当前 Phase 1+Phase 5 的并行化已覆盖 80% 预期收益。**
