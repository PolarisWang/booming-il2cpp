# Reference Assembly And CoreLib Policy

## 目的

这份文档定义仓库内显式 DLL 引用的受控来源，覆盖以下 archetype 和消费场景：

- `ReferenceAssemblySolution`
- `CoreLibReferenceSolution`
- `MixedReferenceClosureSolution`
- hotupdate / proof 场景里的官方 foundation DLL 消费

目标是让 solution 级验证保持可复现、可审计、可进入 nativeization 主线，而不是依赖开发机本地 SDK 的偶然路径。

## 总原则

### 1. 官方 DLL 的受控落点固定在 `src/dll/`

凡是要被仓库内工程直接 `<Reference HintPath=...>` 消费的官方 `.NET` `ref/runtime` DLL，统一放在：

- `src/dll/dotnet-foundation/`

当前已经提升为仓库受控子集的内容只有：

- `src/dll/dotnet-foundation/net8.0/ref/*.dll`
- `src/dll/dotnet-foundation/net8.0/runtime/System.Private.CoreLib.dll`
- `src/dll/dotnet-foundation/net10.0/runtime/System.Private.CoreLib.dll`

这里的 `src/dll/` 不是完整 SDK pack 镜像，而是“被项目消费”或“被当前 nativeization 主线消费”的 promoted subset。

### 2. phase0 catalog 仍然是完整 pack authority

完整官方 pack 的来源、版本、diff 和资产策略，仍然以 phase0 产物为准：

- `docs/archive/dev-completed/20260418-02-phase-0-framework-pack-catalog-and-asset-policy/framework-catalog-v1-01.json`

换句话说：

- `framework-catalog-v1-01.json` 负责描述“完整官方 pack 在哪里”。
- `src/dll/dotnet-foundation/` 负责描述“当前仓库哪些官方 DLL 被正式提升为稳定消费输入”。

### 3. 禁止事项

禁止：

- 把开发机本地 SDK 的绝对路径直接写进 `.csproj`
- 为图省事把整套 `Microsoft.NETCore.App.Ref` / `Microsoft.NETCore.App` 镜像进仓库
- 运行时临时改写 `.csproj` 指向机器路径

允许：

- `ProjectReference`
- `PackageReference`
- 指向 `src/dll/` 的仓库受控 `<Reference HintPath=...>`
- 指向 `assets/reference-bundles/` 的仓库自有非官方样例 DLL

## Archetype 边界

### `ReferenceAssemblySolution`

适用：

- 仓库自有样例 DLL 的显式 `<Reference>` 行为验证

当前 canonical 输入仍然是：

- `assets/reference-bundles/dotnet-foundation/lib/ReferenceGreeter.dll`

`ReferenceGreeter.dll` 不是官方 foundation DLL，因此继续保留在 `assets/reference-bundles/`，不迁入 `src/dll/`。

### `CoreLibReferenceSolution`

适用：

- 显式引用官方 core foundation reference assemblies
- 验证 subject solution 能稳定消费受控官方 DLL 子集

当前 canonical 输入固定为：

- `src/dll/dotnet-foundation/net8.0/ref/`

当前成员：

- `mscorlib.dll`
- `netstandard.dll`
- `System.Collections.dll`
- `System.Console.dll`
- `System.Linq.dll`
- `System.Runtime.dll`
- `System.Runtime.Extensions.dll`

关键约束：

- `DisableImplicitFrameworkReferences=true`
- 需要显式写出 `<Reference>` + `HintPath`
- 官方 ref DLL 不再从 `assets/reference-bundles/dotnet-foundation/net8.0` 消费

### `MixedReferenceClosureSolution`

适用：

- 在一个 solution 中同时覆盖 `ProjectReference`、`Reference`、`PackageReference`

规则：

- 仓库自有样例 DLL 继续从 `assets/reference-bundles/` 消费
- 官方 foundation DLL 一旦需要显式 `<Reference>`，必须从 `src/dll/dotnet-foundation/<tfm>/ref/` 消费

## Runtime Nativeization 输入策略

当前“要被 native 化”的 runtime DLL，统一放在对应 TFM 的 runtime 子目录：

- `src/dll/dotnet-foundation/net8.0/runtime/System.Private.CoreLib.dll`
- `src/dll/dotnet-foundation/net10.0/runtime/System.Private.CoreLib.dll`

这类 runtime DLL 的职责不是给 subject `.csproj` 直接引用，而是作为：

- phase2 semantic ledger 输入
- phase2 helper contract 输入
- phase3 semantic-family execution planning 输入
- dependency-layer 继续扩展 `System.*` DLL 时的基础 authority 输入

## 更新规则

只有在以下条件同时满足时，才应把新的官方 DLL 提升到 `src/dll/`：

1. 已经有 subject / proof / nativeization 主线明确消费它。
2. 现有受控子集无法表达该场景。
3. 提升后仍然保持“受控子集”，而不是演变成 pack 镜像。

每次新增或调整官方 DLL 时，必须一起完成：

- 更新 `src/dll/` 下的受控 DLL 文件
- 更新受影响的 `.csproj`
- 更新本架构文档
- 更新相关兼容性测试

不能只替换 DLL 文件而不更新验证。
