# Codegen 并行管线重构设计

## 当前阶段依赖图

```
Phase 0 [assemble] — 顺序构建支持数据模型 (~5ms)
  BuildCustomAttributeSupportModel -> BuildAssemblyReflectionSupportModel -> etc.

Phase 1 [post_parse_enum_helpers] — 顺序 (~170ms avg, 2.1s worst)
  CollectEnumTypeSubjectIds -> BuildEnumValueToNameMap -> etc.

Phase 2 [object_model_emit] — 顺序 (~130ms avg, 1.1s worst)
  EmitObjectModelDeclarations <- EmitGenericRegistration (最大瓶颈)

Phase 3-4 [method_bodies] — 已并行化 (~325ms avg, 3.5s worst)

Phase 4-5 [registration_dispatch] — 顺序 (~685ms avg, 12.5s worst)
  BuildModuleRegistration -> EmitCodeRegistrationStructs -> etc.
```

## 可并行化的阶段

### 候选1: Phase 5 内部分阶段并行 (最高收益, 12.5s -> ~4s)

BuildHotpatchTable + BuildExternalRuntimeDispatchTable + BuildGcSlotMapSection
三者之间没有数据依赖, 可以完全并行。

### 候选2: Phase 1 + Phase 2 部分并行

CollectEnumTypeSubjectIds 的输出是 BuildEnumValueToNameMap 的输入, 但 CollectExternalRuntimeHelpers 与其他子阶段无依赖。

### 候选3: Phase 5 -> 引入 DataFlow pipeline

最大块 chunk (System.Linq/System.Private.CoreLib) 的 registration_dispatch 从 12.5s -> 预估 4s。

## 预估收益

| 阶段 | 当前最差 | 预估优化后 | 收益 |
|------|---------|-----------|------|
| Phase 5 (registration_dispatch) | 12,525ms | ~4,000ms | -8.5s |
| Phase 2 (object_model_emit) | 1,132ms | ~800ms | -0.3s |
| Phase 1 (post_parse_enum_helpers) | 2,181ms | ~1,500ms | -0.7s |
| **总计** | **~15.8s** | **~6.3s** | **-9.5s** |

## 实施方案

### Step1: Phase 5 内部分阶段并行

```csharp
// 独立阶段 (无数据依赖)
Task t1 = Task.Run(() => BuildHotpatchTable(methodsForLowering, metadataRegistration));
Task t2 = Task.Run(() => BuildExternalRuntimeDispatchTable(...));
Task t3 = Task.Run(() => BuildGcSlotMapSection(methodsForLowering));

// 依赖主心轴 — 需要等待部分任务
string abiManifestCode = BuildAbiManifest(methodsForLowering);
string cryptoCode = BuildCryptoAotIrCode();

// 等独立任务完成
Task.WaitAll(t1, t2, t3);

// 组装 moduleRegSb (依赖所有先行任务)
// EmitCodeRegistrationStructs (可并行)
Task t4 = Task.Run(() => EmitCodeRegistrationStructs(...));
Task t5 = Task.Run(() => EmitReflectionQueryImage(...));
Task.WaitAll(t4, t5);
```

### Step2: `Parallel.ForEach` 应用到 Phase 1

### Step3: 在 Phase 0-2 间引入 Pipe/Channel
