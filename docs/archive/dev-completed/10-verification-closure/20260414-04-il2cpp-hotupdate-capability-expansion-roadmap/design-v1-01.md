# IL2CPP 与热更能力覆盖扩展设计 v1.01

Date: 2026-04-14 09:47:42 +08:00

## 1. 设计目标

在不增加顶层 subject 数量的前提下，建立一套长期可维护的能力覆盖模型，使仓库能稳定回答以下问题：

- 某个 C# 语义能力是否被 native 验证过。
- 某个能力是否也被 hot-update 或 mixed execution 验证过。
- 某个完整 solution archetype 是否被完整 ingest、analysis、codegen、runtime 验证过。
- 新增测试目标时，应该往哪个 retained subject、哪个能力族、哪个 archetype 里放。

## 2. 核心原则

### 2.1 Top-level subject 只按 executable plan 划分

顶层继续只保留：

- `SolutionCorePack`
- `HotUpdateHostPack`
- `MixedExecutionFeaturePack`

只有出现新的 executable plan，才有资格讨论新增顶层 subject。

### 2.2 历史语法点机械拆成单文件，但不机械拆成单项目

规范是：

- 每个 proof / benchmark 条目尽量单文件。
- 同能力族条目尽量合并到同一个共享项目。
- 不用“一个语法点一个工程”的方式制造项目爆炸。

### 2.3 Subject 内部调度面不继续扩 string 协议

新增调度元数据优先使用：

- 枚举
- 位标志
- 小整数类型

避免继续依赖模糊字符串常量来表达 capability、archetype、support state。

### 2.4 测试声明只走 `Chaos.TestFramework`

subject 作者侧的公开 API 继续收口到：

- `[ChaosUnitTest(...)]`
- `[ChaosBenchmark(...)]`
- `Assert`

不回到大量外置 `validation/`、`scenario/`、proof family 文件定义模式。

## 3. 能力模型

## 3.1 一级能力族

建议一级能力族维持稳定的大类：

- `PrimitivesAndOps`
- `TypeSystemAndGenerics`
- `AsyncAndIterators`
- `DelegatesAndClosures`
- `ExceptionsAndControlFlow`
- `ReflectionAndMetadata`
- `InteropAndMarshaling`
- `ThreadingAndSynchronization`
- `RuntimeServices`
- `UnsafeAndLowLevel`
- `DiagnosticsAndTracing`
- `LinkerAndAotClosure`

## 3.2 二级能力项

真正进入 capability taxonomy freeze 的是二级能力项，而不是只停留在大类名字。

例如：

- `TypeSystemAndGenerics`
  - `GenericInstantiation`
  - `GenericConstraint`
  - `GenericVirtualDispatch`
  - `GenericInterfaceDispatch`
- `ReflectionAndMetadata`
  - `CustomAttributeLookup`
  - `MemberMetadataLookup`
  - `MetadataSupplement`
  - `LinkerPreserveContract`

二级能力项是后续 proof/benchmark/archetype 归属、dashboard 标签和文档矩阵的主键。

## 4. Subject 归属模型

## 4.1 `SolutionCorePack`

职责：

- 承担大部分 IL2CPP native correctness proof。
- 承担 solution archetype 验证。
- 承担 native/managed baseline benchmark。

目标结构：

```text
subjects/SolutionCorePack/source/
  SolutionCorePack.sln
  Launcher/
  Archetypes/
    SimpleLibrarySolution/
    MultiProjectSolution/
    PackageReferenceSolution/
    ReferenceAssemblySolution/
    CoreLibReferenceSolution/
    MixedReferenceClosureSolution/
  FeatureSlices/
    PrimitivesAndOps/
    TypeSystemAndGenerics/
    AsyncAndIterators/
    ExceptionsAndControlFlow/
    ReflectionAndMetadata/
    InteropAndMarshaling/
    ThreadingAndSynchronization/
    RuntimeServices/
    UnsafeAndLowLevel/
    DiagnosticsAndTracing/
    LinkerAndAotClosure/
  Benchmarks/
```

## 4.2 `HotUpdateHostPack`

职责：

- 承担完整 host-plus-patch workflow。
- 承担 patch compatibility / rollback / metadata supplement / package contract。
- 承担 hot-update benchmark。

目标结构：

```text
subjects/HotUpdateHostPack/source/
  HotUpdateHostPack.sln
  Launcher/
  Host/
  SharedContracts/
  PatchModules/
  Archetypes/
    SkeletonPatchSolution/
    FullProjectHotUpdateSolution/
    VersionCompatibilitySolution/
  Benchmarks/
```

## 4.3 `MixedExecutionFeaturePack`

职责：

- 承担 mixed / interpreter 专有链路。
- 承担 managed/native/interpreter 对照 benchmark。

目标结构：

```text
subjects/MixedExecutionFeaturePack/source/
  MixedExecutionFeaturePack.sln
  Launcher/
  ManagedBridge/
  Interpreter/
  Lowering/
  Archetypes/
    MixedBridgeSolution/
  Benchmarks/
```

## 5. `Chaos.TestFramework` 扩展方向

新增公开元数据应该继续放在：

- `src/reference/Chaos.TestFramework`

建议扩展点：

- `ChaosCapabilityFamily : byte`
- `ChaosCapabilityItem : ushort`
- `ChaosSolutionArchetype : byte`
- `ChaosHotUpdateCapability : ushort`
- `ChaosSupportState : byte`

要求：

- 所有公开枚举都带 XML 注释。
- attribute 只承载内在元数据，不承载大而复杂的外部矩阵配置。
- support state 可以在外部 matrix/manifest 层表达，不强行塞进每个测试方法。

## 6. Benchmark 设计约束

benchmark 不做“每个语法点一个 benchmark”。

更合理的原则是：

- correctness proof 尽量全面。
- benchmark 只覆盖高价值 runtime 路径。
- benchmark case 必须映射到能力族、archetype 或执行模式，而不是旧 subject 别名。
- dashboard 默认以 managed baseline 为对比基线。

## 7. 需要保留的 open questions

这轮 roadmap 中仍需逐条和用户对齐的问题，应该显式保留在 child task 中：

- OQ2: support state 仅在外部 matrix 表达，还是需要少量进入 attribute。
- OQ3: reference bundle 的 foundation 范围精确到哪些 core DLL。
- OQ4: 新拆出的历史语法点里，哪些只需要 proof，哪些必须进入 benchmark。
- OQ5: dashboard 默认主视图以 capability family 组织，还是以 archetype/case 组织。

## 8. 设计结论

新的 follow-up 应该是一条父 roadmap，而不是直接开写实现代码。

优先顺序应为：

1. 先冻结 capability taxonomy、archetype inventory 和 subject ownership。
2. 再扩展 `Chaos.TestFramework` 与 metadata discovery。
3. 再分别推进三个 retained subjects 的内部重构与能力补齐。
4. 最后统一 pipeline、dashboard、docs 和全量验证。
