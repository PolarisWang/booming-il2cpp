# Subject Target Structure Freeze v1.01

Date: 2026-04-14 01:42:47 +08:00

## 1. 冻结目标

本文件冻结三个 retained subject 的目标目录结构和当前目录到目标结构的迁移映射。

## 2. 通用规则

- 顶层 `subjects/` 继续只保留：
  - `SolutionCorePack`
  - `HotUpdateHostPack`
  - `MixedExecutionFeaturePack`
- 历史语法点改为单文件条目
- 同 capability family 尽量同项目合并
- 新项目只在 solution graph / dependency model / executable plan 确实不同的情况下引入
- subject 内部调度继续使用紧凑枚举，不继续扩大 string 协议面

## 3. `SolutionCorePack` 目标结构

```text
subjects/SolutionCorePack/
  subject.manifest.json
  source/
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
      ObjectModelAndDispatch/
      GenericsAndCollections/
      ExceptionsAndControlFlow/
      AsyncAndThreading/
      ReflectionAndMetadata/
      InteropAndMarshaling/
      RuntimeLowLevel/
    Benchmarks/
      CoreRuntimeBenchmarks/
```

当前到目标的映射：

- `Archetypes/SolutionSimpleLib`
  - -> `Archetypes/SimpleLibrarySolution`
- `Archetypes/SolutionMultiProject`
  - -> `Archetypes/MultiProjectSolution`
- `Archetypes/SolutionPackageReference`
  - -> `Archetypes/PackageReferenceSolution`
- `Slices/MainlineFeaturePack`
  - -> `FeatureSlices/*`
- `Slices/PerformanceFeaturePack`
  - -> `Benchmarks/CoreRuntimeBenchmarks`

结构冻结结论：

- `MainlineFeaturePack` 与 `PerformanceFeaturePack` 不再作为长期目录名保留
- 历史 proof 文件按 capability family 重新归档
- 新 archetype 直接进入 `Archetypes/`，不再走临时 `Solution*` 命名

## 4. `HotUpdateHostPack` 目标结构

```text
subjects/HotUpdateHostPack/
  subject.manifest.json
  source/
    HotUpdateHostPack.sln
    Launcher/
    Host/
    SharedContracts/
    PatchModules/
    Archetypes/
      SkeletonPatchSolution/
      FullProjectHotUpdateSolution/
      VersionCompatibilitySolution/
      ReferenceAssemblyPatchSolution/
    Benchmarks/
      HotUpdateBenchmarks/
```

当前到目标的映射：

- 当前 `source/` 平铺的 proof/benchmark/support 文件
  - -> 分拆到 `Host/`、`SharedContracts/`、`PatchModules/`、`Benchmarks/`
- 当前 flat entry files
  - -> 继续保留单文件 proof/benchmark 条目，但按 capability 归入对应目录

结构冻结结论：

- `FullProjectHotUpdateSolution` 是本轮新增核心 archetype
- patch package contract 会在后续进入单独 architecture 文档

## 5. `MixedExecutionFeaturePack` 目标结构

```text
subjects/MixedExecutionFeaturePack/
  subject.manifest.json
  source/
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

当前到目标的映射：

- 当前 flat mixed source
  - -> 拆分到 `ManagedBridge/`、`Interpreter/`、`Lowering/`
- `InterpreterArithmeticProof/`
  - -> 吸收进 `Archetypes/MixedBridgeSolution` 或 `Interpreter/` 下的子结构

结构冻结结论：

- mixed execution 暂时只保留一个 canonical archetype
- 如果后续出现明显不同的 solution graph，再新增 archetype，而不是现在提前拆分

## 6. 调度面冻结

### 保留

- 外层 CLI 可继续接受字符串形式的 subject id / matrix id
- attribute 的 `Alias` 可作为用户可见短名存在

### 不再扩大

- subject 内部 launcher 不继续引入新的自由字符串分发表
- capability、archetype、entry kind、entry slice 均优先走枚举
- 不再用新的 `scenario-name`、`proof-family`、`benchmark-family` 字符串协议扩展 subject 内部执行面

## 7. 结论

Phase 0 之后，retained subject 的未来结构边界已经固定：

- `SolutionCorePack` 以 `Archetypes + FeatureSlices + Benchmarks` 为核心
- `HotUpdateHostPack` 以 `Host + SharedContracts + PatchModules + Archetypes + Benchmarks` 为核心
- `MixedExecutionFeaturePack` 以 `ManagedBridge + Interpreter + Lowering + Archetypes + Benchmarks` 为核心

后续阶段只是在这个框架里填内容，不再重新发明目录语义。
