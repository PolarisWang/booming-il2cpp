# Solution Archetype Freeze v1.01

Date: 2026-04-14 10:12:00 +08:00

## 1. 冻结目标

本文档冻结 retained subjects 允许承载的 canonical solution archetype。

原则：

- archetype 名称以真实 solution graph 为准
- archetype 是完整 solution 级验证对象，不等于单个 proof 文件
- 新增 archetype 优先并入现有 retained subject
- 不因 archetype 增长而新增顶层 subject

## 2. Reference Bundle Freeze

`CoreLibReferenceSolution` 与相关 reference-based archetype 统一采用：

- repository-owned `foundation bundle`

冻结结论：

- OQ3 选择 medium `foundation bundle`
- 不使用 machine-local SDK resolver 作为 canonical 长期测试输入
- 不使用整包 .NET reference pack 镜像

建议固定路径：

- `assets/reference-bundles/dotnet-foundation/`

foundation bundle 的设计目标：

- 覆盖代表性的 corelib / BCL reference 依赖闭包
- 支持 `CoreLibReferenceSolution`
- 支持必要的 mixed reference archetype
- 不把仓库变成完整 reference mirror

## 3. Canonical Archetype Inventory

| Archetype | Owner Subject | Minimal Graph | Dependency Model | Current Seed | 说明 |
| --- | --- | --- | --- | --- | --- |
| `SimpleLibrarySolution` | `SolutionCorePack` | `App -> Library` | `ProjectReference` | current | 最小多项目 solution |
| `MultiProjectSolution` | `SolutionCorePack` | `App -> Middle -> Core` | `ProjectReference` | current | 验证 transitive project graph |
| `PackageReferenceSolution` | `SolutionCorePack` | `App + PackageReference` | `PackageReference` | current | 验证 package closure |
| `ReferenceAssemblySolution` | `SolutionCorePack` | `App + repo-owned DLL references` | `Reference` | current | 验证显式 `Reference` / `HintPath` |
| `CoreLibReferenceSolution` | `SolutionCorePack` | `App + foundation bundle` | `Reference (corelib bundle)` | current | 验证 corelib/BCL 显式引用 |
| `MixedReferenceClosureSolution` | `SolutionCorePack` | `App + ProjectReference + PackageReference + Reference` | mixed | current | 验证混合闭包 |
| `SkeletonPatchSolution` | `HotUpdateHostPack` | `Host + Patch` | host-plus-patch | current | 热更最小骨架 |
| `FullProjectHotUpdateSolution` | `HotUpdateHostPack` | `Host + SharedContracts + PatchModules + PackageMetadata` | host-plus-patch | target | 完整热更 canonical archetype |
| `VersionCompatibilitySolution` | `HotUpdateHostPack` | `Host + Versioned Patch Packages` | host-plus-patch | target | compatibility / rollback |
| `MixedBridgeSolution` | `MixedExecutionFeaturePack` | `Managed bridge + Interpreter bridge + AOT bridge` | mixed runtime | current | mixed/interpreter canonical archetype |

## 4. 明确不纳入本轮 first-class inventory 的 archetype

以下对象本轮不单独升级为一级 canonical archetype：

- `ReferenceAssemblyPatchSolution`
  - 若确有价值，优先作为 `FullProjectHotUpdateSolution` 的一个 capability-bearing slice，而不是单独扩大 archetype 面
- 额外的 patch-only / package-only 派生变体
  - 先在现有 archetype 内用 capability 和 matrix 表达

## 5. Archetype 命名规则

- 名称统一采用 `*Solution`
- 命名反映 graph，而不是反映 proof / benchmark 名字
- 不保留 compatibility alias 作为长期 canonical 名称

## 6. Archetype 与 Capability 的关系

- archetype 回答“工程长什么样”
- capability 回答“语义能力是什么”
- 一个 archetype 可承载多个 capability
- 一个 capability 也可跨多个 archetype 被验证

## 7. 结论

Phase 0 之后，完整 solution 的讨论边界固定为上述 10 个 archetype；后续 Phase 2/3/4 只能在这组 archetype 内补实现和补验证，而不是继续扩散顶层模型。
