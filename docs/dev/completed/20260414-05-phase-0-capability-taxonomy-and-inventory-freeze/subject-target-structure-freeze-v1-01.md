# Subject Target Structure Freeze v1.01

Date: 2026-04-14 10:12:00 +08:00

## 1. 冻结目标

本文档冻结三个 retained subject 的目标目录结构与 ownership 边界。

通用规则：

- 顶层 `subjects/` 继续只保留：
  - `SolutionCorePack`
  - `HotUpdateHostPack`
  - `MixedExecutionFeaturePack`
- 历史语法点改成单文件条目
- capability family 尽量同项目合并
- 新项目只在 solution graph / dependency model / executable plan 真正不同的情况下引入
- `bin/`、`obj/` 属于生成产物，不是 canonical 结构

## 2. `SolutionCorePack` 目标结构

当前真实结构：

- `Archetypes/`
- `Benchmarks/`
- `FeatureSlices/`
- `Launcher/`
- `SolutionCorePack.sln`

冻结后的 canonical 结构：

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
    DelegatesAndClosures/
    ExceptionsAndControlFlow/
    ReflectionAndMetadata/
    InteropAndMarshaling/
    ThreadingAndSynchronization/
    RuntimeServices/
    UnsafeAndLowLevel/
    DiagnosticsAndTracing/
    LinkerAndAotClosure/
  Benchmarks/
    CoreRuntimeBenchmarks/
```

冻结结论：

- `FeatureSlices/` 是历史语法点的主要归宿
- `Archetypes/` 承担完整 solution graph 验证
- `Benchmarks/` 承担共享 benchmark，而不是在每个 capability family 下重复堆 benchmark 工程

## 3. `HotUpdateHostPack` 目标结构

当前真实结构：

- `Archetypes/`
- `Host/`
- `PatchModules/`
- `SharedContracts/`
- `HotUpdateHostPack.sln`
- 现有 `HotUpdateHostPack.csproj`

冻结后的 canonical 结构：

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
    HotUpdateBenchmarks/
```

冻结结论：

- `FullProjectHotUpdateSolution` 是本轮新增核心 archetype
- `Host/SharedContracts/PatchModules/` 是长期保留的工程边界
- 如果 patch reference 只是局部差异，优先作为 capability/matrix，不新增顶层 archetype

## 4. `MixedExecutionFeaturePack` 目标结构

当前真实结构：

- `Archetypes/`
- `Interpreter/`
- `Lowering/`
- `ManagedBridge/`
- `MixedExecutionFeaturePack.sln`
- 当前无独立 `Benchmarks/` 目录

冻结后的 canonical 结构：

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
    MixedExecutionBenchmarks/
```

冻结结论：

- mixed execution 暂时只保留一个 canonical archetype
- benchmark 目录需要补齐，但不新增额外顶层 subject

## 5. 调度面冻结

允许保留：

- 外层 CLI 继续接受字符串形式的 `subject id` / `matrix id`
- 用户可见短名或 alias 继续存在于展示层

不再扩大：

- subject 内部 launcher 不再引入新的自由字符串分发表
- capability、archetype、entry kind、entry slice 优先全部走枚举或紧凑数值
- 不再新增 `scenario-name` / `proof-family` / `benchmark-family` 一类内部字符串协议

## 6. 结论

Phase 0 之后，三个 retained subject 的结构边界已经固定。后续阶段只是在这些结构里填内容，不再重新发明目录语义。
