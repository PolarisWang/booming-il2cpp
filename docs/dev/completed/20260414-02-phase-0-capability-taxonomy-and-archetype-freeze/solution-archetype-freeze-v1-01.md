# Solution Archetype Freeze v1.01

Date: 2026-04-14 01:42:47 +08:00

## 1. 冻结目标

本文件冻结 retained subjects 未来允许承载的 canonical solution archetype。

原则：

- archetype 名称以真实 solution graph 为准
- archetype 是完整 solution 级验证对象，不等于单个 proof 文件
- 新增 archetype 优先并入现有 retained subject
- 不因 archetype 增长而新增顶层 subject

## 2. 依赖来源冻结

`CoreLibReferenceSolution` 与相关 reference-based archetype 统一采用：

- repository-owned `reference bundle`

冻结结论：

- 不使用绝对 SDK 路径作为长期测试输入
- 不采用 SDK resolver + machine-local cache 作为 canonical 输入
- 不采用整包 .NET reference pack 镜像

基础 bundle 方向：

- `foundation bundle`
- 代表性中等闭包

建议路径冻结：

- `assets/reference-bundles/dotnet-foundation/`

这是后续 Phase 2/3 的共享 repository-owned 依赖源。

## 3. Canonical Archetype 清单

| Archetype | Owner Subject | Minimal Graph | Dependency Model | Current Seed | 说明 |
| --- | --- | --- | --- | --- | --- |
| `SimpleLibrarySolution` | `SolutionCorePack` | `App -> Library` | `ProjectReference` | `Archetypes/SolutionSimpleLib` | 当前最小多项目 solution；不再误称为单 project |
| `MultiProjectSolution` | `SolutionCorePack` | `App -> Middle -> Core` | `ProjectReference` | `Archetypes/SolutionMultiProject` | 验证 transitive project graph |
| `PackageReferenceSolution` | `SolutionCorePack` | `App + PackageReference` | `PackageReference` | `Archetypes/SolutionPackageReference` | 验证 package closure |
| `ReferenceAssemblySolution` | `SolutionCorePack` | `App + repo-owned DLL references` | `Reference` | new | 验证显式 `Reference` / `HintPath` |
| `CoreLibReferenceSolution` | `SolutionCorePack` | `App + foundation bundle` | `Reference (corelib bundle)` | new | 验证 dotnet 核心 DLL 显式引用 |
| `MixedReferenceClosureSolution` | `SolutionCorePack` | `App + ProjectReference + PackageReference + Reference` | mixed | new | 验证混合闭包 |
| `SkeletonPatchSolution` | `HotUpdateHostPack` | `Host + Patch` | host-plus-patch | current flat source | 热更最小骨架 |
| `FullProjectHotUpdateSolution` | `HotUpdateHostPack` | `Host + SharedContracts + PatchModules + PackageMetadata` | host-plus-patch | new | 全工程热更 canonical archetype |
| `VersionCompatibilitySolution` | `HotUpdateHostPack` | `Host + Versioned Patch Packages` | host-plus-patch | new | compatibility / rollback / version band |
| `ReferenceAssemblyPatchSolution` | `HotUpdateHostPack` | `PatchModules + repo-owned DLL references` | host-plus-patch + reference | new | patch 对 reference bundle 的消费 |
| `MixedBridgeSolution` | `MixedExecutionFeaturePack` | `Managed bridge + Interpreter bridge + AOT bridge` | mixed runtime | current `InterpreterArithmeticProof` + mixed source | mixed/interpreter 主 archetype |

## 4. Archetype 命名规则

- 名称统一用 `*Solution`
- 不保留 `SolutionSimpleLib`、`SolutionPackageReference` 这类历史命名作为长期 canonical 名字
- 不保留 compatibility alias
- 名称必须反映 graph，而不是反映某个 proof 或 benchmark 名字

## 5. Archetype 与 Capability 的关系

- archetype 回答“工程长什么样”
- capability 回答“语义能力是什么”
- 一个 archetype 可以承载多个 capability
- 一个 capability 也可以跨多个 archetype 被验证

例子：

- `GenericsAndCollections/GenericMethodFlow`
  - 可以在 `SimpleLibrarySolution`
  - 也可以在 `CoreLibReferenceSolution`
- `HotUpdateWorkflow/MethodReplacement`
  - 可以在 `SkeletonPatchSolution`
  - 也可以在 `FullProjectHotUpdateSolution`

## 6. 第一批实施优先级

### `SolutionCorePack`

优先级顺序：

1. `SimpleLibrarySolution`
2. `MultiProjectSolution`
3. `PackageReferenceSolution`
4. `ReferenceAssemblySolution`
5. `CoreLibReferenceSolution`
6. `MixedReferenceClosureSolution`

### `HotUpdateHostPack`

优先级顺序：

1. `SkeletonPatchSolution`
2. `FullProjectHotUpdateSolution`
3. `VersionCompatibilitySolution`
4. `ReferenceAssemblyPatchSolution`

### `MixedExecutionFeaturePack`

优先级顺序：

1. `MixedBridgeSolution`

## 7. 结论

本轮 roadmap 中，solution archetype 是完整 solution 验证 contract 的核心单位。

后续实现阶段必须遵守：

- archetype 名称一次性 canonical 化
- `CoreLibReferenceSolution` 固定使用 repository-owned `foundation bundle`
- 不把新的工程形态重新包装成更多顶层 subject
