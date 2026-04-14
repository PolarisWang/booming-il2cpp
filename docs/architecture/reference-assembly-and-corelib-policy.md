# Reference Assembly And CoreLib Policy

## 目的

这份文档定义仓库内所有显式 DLL 引用的来源策略，尤其是：

- `ReferenceAssemblySolution`
- `CoreLibReferenceSolution`
- `MixedReferenceClosureSolution`
- 热更 patch 场景下的 reference assembly 依赖

目标是让 solution 级测试工程保持可复现、可移植、可进入 IL2CPP 主线，而不是依赖开发机上的偶然路径。

## 总原则

### 1. 只允许仓库受控的 reference 来源

禁止：

- 直接把本机 SDK 绝对路径写进 `.csproj`
- 让 archetype 依赖开发机上不可复现的临时缓存

允许：

- `ProjectReference`
- `PackageReference`
- 仓库受控的 `<Reference Include=...><HintPath=...>`
- 仓库受控的 corelib foundation bundle

### 2. CoreLib 闭包使用 repository-owned bundle

当前 frozen 方案是：

- `Q0 = repository-owned reference bundle`

因此 `CoreLibReferenceSolution` 不走机器本地 SDK 解析器，而是显式引用仓库中的 foundation bundle。

当前路径：

- `assets/reference-bundles/dotnet-foundation/net8.0`

当前 bundle 成员：

- `mscorlib.dll`
- `netstandard.dll`
- `System.Collections.dll`
- `System.Console.dll`
- `System.Linq.dll`
- `System.Runtime.dll`
- `System.Runtime.Extensions.dll`

## 四类依赖图的使用边界

### `ProjectReference`

适用：

- 仓库拥有源码
- 目标是验证 project graph 本身

不适用：

- 明确要测试 DLL reference 行为

### `ReferenceAssemblySolution`

适用：

- 仓库拥有 DLL
- 需要验证显式 `<Reference>` / `HintPath` 行为

当前 canonical 样例：

- `subjects/SolutionCorePack/source/Archetypes/ReferenceAssemblySolution/App/GoldenReferenceAssembly.App.csproj`

其中显式引用：

- `assets/reference-bundles/dotnet-foundation/lib/ReferenceGreeter.dll`

### `CoreLibReferenceSolution`

适用：

- 需要验证显式 dotnet 核心 DLL 引用闭包
- 需要证明 IL2CPP 能 ingest 这类完整 solution

当前 canonical 样例：

- `subjects/SolutionCorePack/source/Archetypes/CoreLibReferenceSolution/App/GoldenCoreLibReference.App.csproj`

关键规则：

- `DisableImplicitFrameworkReferences=true`
- 所有核心 DLL 明确写出 `<Reference>` + `HintPath`

### `MixedReferenceClosureSolution`

适用：

- 需要在一个 solution 中同时覆盖：
  - `ProjectReference`
  - `Reference`
  - `PackageReference`

它是工程级验证最重要的组合 archetype 之一，因为它最接近真实业务 solution。

## Foundation Bundle 策略

### 1. 使用“中等规模、代表性闭包”

当前 frozen 方案：

- `Q3 = medium foundation bundle`

这意味着 bundle 不追求：

- 极小到失去代表性
- 或完整镜像整个 SDK pack

而是只保留能稳定代表核心闭包行为的一组 DLL。

### 2. Bundle 更新条件

只有在以下情况下才应扩 bundle：

1. 新 archetype 需要新的核心 DLL，且不能通过现有 bundle 表达。
2. 该 DLL 是长期稳定依赖，而不是单个临时样例需求。
3. 增加后仍然保持“foundation bundle”而非“SDK 镜像”。

### 3. 更新方式

更新 bundle 时必须同时完成：

- 更新 `assets/reference-bundles/...`
- 更新受影响的 canonical `.csproj`
- 更新 architecture 文档
- 补 proof / solution 级验证

不能只替换 DLL 文件而不更新验证。

## Patch 场景策略

热更 patch 若需要 reference assembly：

- 仍然使用仓库受控 bundle
- 仍然通过 archetype / manifest / proof 显式声明
- 不允许 patch project 私自引用机器本地 SDK 路径

这对应：

- `ReferenceAssemblyPatchSolution`

## 不允许的做法

- 在 subject source 中引入本机绝对路径。
- 为了图省事把整个 SDK pack 复制进仓库。
- 让 corelib archetype 与 reference assembly archetype 混在同一组规则里。
- 用脚本在运行时偷偷改写 `.csproj` 指向开发机路径。

## 选择指南

| 需求 | 选型 |
| --- | --- |
| 只验证多 project 工程图 | `SimpleLibrarySolution` / `MultiProjectSolution` |
| 验证外部 nuget 包 | `PackageReferenceSolution` |
| 验证显式 DLL 引用 | `ReferenceAssemblySolution` |
| 验证显式 dotnet 核心 DLL 闭包 | `CoreLibReferenceSolution` |
| 同时验证 project + reference + package | `MixedReferenceClosureSolution` |

## 维护规则

1. 新增 reference/corelib 依赖时，先判断它属于哪种 archetype，而不是先加路径。
2. 引用来源必须可复现、可审计。
3. 只要能用现有 archetype 表达，就不新增新的 solution archetype。
