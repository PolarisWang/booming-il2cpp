# Solution Archetype Matrix

## 目的

这份文档定义仓库中所有 canonical solution archetype 的长期含义、归属和验证目标。它回答的问题不是“某个 proof 测了什么”，而是“某类完整工程应该长什么样，以及如何进入 IL2CPP / 热更流水线”。

## 总览

| Archetype | Owner Subject | 典型路径 | 依赖图形态 | 核心问题 |
| --- | --- | --- | --- | --- |
| `SimpleLibrarySolution` | `SolutionCorePack` | `subjects/SolutionCorePack/source/Archetypes/SimpleLibrarySolution` | `App -> Library` 的最小 project graph | 最小 solution/project 引用能否完整 ingest 与运行 |
| `MultiProjectSolution` | `SolutionCorePack` | `subjects/SolutionCorePack/source/Archetypes/MultiProjectSolution` | `App -> Middle -> Core` 的传递 project graph | 多 project 传递闭包是否正确 |
| `PackageReferenceSolution` | `SolutionCorePack` | `subjects/SolutionCorePack/source/Archetypes/PackageReferenceSolution` | `PackageReference` 闭包 | package 依赖是否正确进入工程闭包 |
| `ReferenceAssemblySolution` | `SolutionCorePack` | `subjects/SolutionCorePack/source/Archetypes/ReferenceAssemblySolution` | 显式 `<Reference>` + repo bundle DLL | 非 project DLL 引用是否稳定被 ingest |
| `CoreLibReferenceSolution` | `SolutionCorePack` | `subjects/SolutionCorePack/source/Archetypes/CoreLibReferenceSolution` | 关闭 implicit framework reference，显式引用 repo-owned foundation bundle | dotnet 核心 DLL 引用闭包能否被 canonical 化 |
| `MixedReferenceClosureSolution` | `SolutionCorePack` | `subjects/SolutionCorePack/source/Archetypes/MixedReferenceClosureSolution` | `ProjectReference + Reference + PackageReference` 混合闭包 | 最复杂的常见 solution 依赖图是否可被完整处理 |
| `SkeletonPatchSolution` | `HotUpdateHostPack` | host/patch 最小骨架条目 | 最小 host + patch | 最小热更加载链是否成立 |
| `FullProjectHotUpdateSolution` | `HotUpdateHostPack` | `subjects/HotUpdateHostPack/source/Archetypes/FullProjectHotUpdateSolution` | `HostApp + SharedContracts + PatchModules` | 完整热更工程如何组织与验证 |
| `VersionCompatibilitySolution` | `HotUpdateHostPack` | retained hot-update compatibility 验证条目 | host/patch 版本带宽 | AOT 主包与 patch 版本兼容如何验证 |
| `ReferenceAssemblyPatchSolution` | `HotUpdateHostPack` | retained patch assembly 条目 | patch 显式消费 repo-owned reference DLL | patch 图引用外部 reference 时的 contract |
| `MixedBridgeSolution` | `MixedExecutionFeaturePack` | `subjects/MixedExecutionFeaturePack/source/Archetypes/MixedBridgeSolution` | managed + native bridge + interpreter | mixed execution 边界是否保持同一语义 |

## 设计规则

### 1. Archetype 是完整工程 contract，不是单个 proof 分类

Archetype 用来描述 solution graph，而不是语法点。一个 archetype 内部可以承载多个 proof / benchmark 条目，但它必须回答同一个工程级问题。

### 2. 只有 canonical graph 才能成为 archetype

如果一个结构只是某个 proof 的实现细节，而不是长期需要复用的 solution 形态，就不应升级为 archetype。

### 3. 同一 archetype 只归属一个 retained subject

不能出现同一个 archetype 在多个顶层 subject 中重复定义。需要跨 subject 复用时，应复用能力语义，不复用 owner。

## `SolutionCorePack` Archetype 说明

### `SimpleLibrarySolution`

- 目标：提供最小 `App + Library` baseline。
- 适用：任何需要验证 project 引用能否成功被 ingest 的最小闭包。
- 非目标：不承载 package、reference DLL 或 corelib 级别问题。

### `MultiProjectSolution`

- 目标：验证传递 project graph 与中间层组合。
- 适用：工程级验证的最小代表样例之一。
- 典型用途：solution-* 级验证的基础样式。

### `PackageReferenceSolution`

- 目标：验证 `PackageReference` 的 restore、build 与后续 ingest。
- 适用：外部包闭包是核心变量时。
- 非目标：不混入 reference DLL 或 corelib bundle。

### `ReferenceAssemblySolution`

- 目标：验证显式 `<Reference Include=...><HintPath=...>`。
- 适用：仓库拥有 DLL、且不希望改写成 project 的场景。
- 依赖源：必须来自仓库受控 bundle，不允许机器绝对路径。

### `CoreLibReferenceSolution`

- 目标：验证显式 dotnet foundation bundle。
- 关键特征：
  - 关闭 `ImplicitFrameworkReferences`
  - 显式引用 `mscorlib.dll`、`netstandard.dll`、`System.Runtime.dll` 等 bundle 成员
- 这是 solution 级验证的核心 archetype 之一，因为它直接覆盖“完整 solution + 核心 DLL 依赖闭包”。

### `MixedReferenceClosureSolution`

- 目标：把 project、reference、package 三种依赖方式同时纳入一个 solution。
- 适用：需要工程级验证多种依赖组合的真实闭包时。
- 优先级：高。它是最接近真实业务 solution 的 canonical 样式之一。

## `HotUpdateHostPack` Archetype 说明

### `SkeletonPatchSolution`

- 目标：保留最小 host-plus-patch 骨架，快速验证热更链路没断。
- 适用：冒烟和最小负反馈定位。

### `FullProjectHotUpdateSolution`

- 目标：提供完整热更工程标准样式。
- 组成：
  - `HostApp`
  - `SharedContracts`
  - `PatchModules`
- 它不是案例之一，而是完整热更工程的 canonical archetype。

### `VersionCompatibilitySolution`

- 目标：验证 AOT 主包版本与 patch 版本带宽。
- 适用：rollback、兼容窗口、发布门禁。

### `ReferenceAssemblyPatchSolution`

- 目标：定义 patch 图如何消费仓库受控 reference bundle。
- 适用：patch 不能依赖本机 SDK 路径，又必须显式引用 DLL 的情况。

## `MixedExecutionFeaturePack` Archetype 说明

### `MixedBridgeSolution`

- 目标：以单一 archetype 承载 managed/native/interpreter bridge。
- 组成：
  - bridge 导出
  - interpreter 侧 proof
  - AOT bridge project
- 原则：mixed execution 先保持一个 archetype + 多个 capability slice，直到出现明显不同的 solution graph 才再拆分。

## Archetype 与 Capability 的关系

- Archetype 回答“工程如何组织”。
- Capability 回答“语义能力是什么”。
- 一个 archetype 可以覆盖多个 capability。
- 一个 capability 也可以在多个 archetype 中被间接验证，但 owner 仍然只有一个。

## 新增 Archetype 的准入条件

必须同时满足：

1. 它代表新的 solution graph，而不是现有 graph 中的一个 proof 变体。
2. 它回答的是长期稳定的工程级问题。
3. 现有 retained subjects 中没有任何一个 archetype 可以合理承接。
4. 它能通过 `ChaosSolutionArchetype` 形成稳定公开 contract。

只要其中任一条件不满足，就应优先落到现有 archetype 或 feature slice。
