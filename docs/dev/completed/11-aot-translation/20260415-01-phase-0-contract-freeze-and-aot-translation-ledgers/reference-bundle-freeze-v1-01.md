# Reference Bundle Freeze v1.01

Date: 2026-04-15 00:11:06 +08:00
Status: draft-freeze

## 1. 目标

冻结母 roadmap 的 `Q0`:

- `reference bundle = repository-owned canonical base bundle + controlled supplement`

这里的“冻结”不是只写原则，而是明确：

- canonical base bundle 在仓库里的唯一位置；
- controlled supplement 的允许来源；
- 哪些 solution / patch archetype 应消费哪一层；
- 新增 DLL 的准入和刷新规则。

## 2. 冻结结论

### 2.1 Canonical Base Bundle

仓库当前唯一的 canonical base bundle 为：

- `assets/reference-bundles/dotnet-foundation/net8.0`

当前成员：

- `mscorlib.dll`
- `netstandard.dll`
- `System.Collections.dll`
- `System.Console.dll`
- `System.Linq.dll`
- `System.Runtime.dll`
- `System.Runtime.Extensions.dll`

这层只承载“仓库拥有、长期稳定、可复现的 dotnet foundation/corelib 基线”。

### 2.2 Controlled Supplement

受控 supplement 当前基线为：

- `assets/reference-bundles/dotnet-foundation/lib/ReferenceGreeter.dll`

对应仓库内源头：

- `assets/reference-bundles/dotnet-foundation/src/ReferenceGreeter`

这层用于表达：

- 非 corelib，但仍由仓库拥有和复现的显式 DLL 引用；
- `ReferenceAssemblySolution`、`ReferenceAssemblyPatchSolution` 这类 archetype 需要的 canonical reference 样本。

它不是第二套 foundation bundle，也不能演化成 SDK 镜像。

## 3. 消费矩阵

| archetype | 允许消费的引用来源 | 当前冻结口径 |
| --- | --- | --- |
| `SimpleLibrarySolution` | `ProjectReference` | 不需要 bundle；如能用 project graph 表达，就不引入 DLL |
| `MultiProjectSolution` | `ProjectReference` | 不需要 bundle |
| `PackageReferenceSolution` | `PackageReference` | 不需要 bundle |
| `ReferenceAssemblySolution` | repo-owned controlled supplement | 消费 `assets/reference-bundles/dotnet-foundation/lib/*` |
| `CoreLibReferenceSolution` | repo-owned canonical base bundle | 消费 `assets/reference-bundles/dotnet-foundation/net8.0/*` |
| `MixedReferenceClosureSolution` | `ProjectReference + PackageReference + repo-owned bundle` | 同时允许 base bundle 和 controlled supplement |
| `ReferenceAssemblyPatchSolution` | repo-owned controlled supplement | patch 场景不能绕过仓库 bundle 直接去读本机 SDK |
| `FullProjectHotUpdateSolution` | `ProjectReference` 为主，必要时可消费 controlled supplement | 仍然禁止本机绝对路径 |

## 4. 明确禁止项

以下做法在本 roadmap 内直接视为违规：

- 在 `.csproj` 里写本机 SDK 或 runtime pack 绝对路径。
- 为了省事复制整套 SDK pack 到仓库。
- 在 patch / hot-update project 里私自引入“只有当前机器上有”的临时 DLL。
- 用脚本在运行时偷偷改写 `.csproj` 指向开发机路径。

## 5. Controlled Supplement 的准入条件

新增 supplement DLL 必须同时满足：

1. 不能用已有 `ProjectReference` 或 `PackageReference` 更自然表达。
2. 不是某个一次性实验 subject 的偶然依赖，而是长期 archetype 需要的 canonical witness。
3. 能在仓库中给出可追溯的源头，或给出清晰的引入说明与更新策略。
4. 新增后仍属于“controlled supplement”，而不是新的平行 bundle。

## 6. 刷新流程

刷新 base bundle 或 supplement 时，必须同步完成：

1. 更新 `assets/reference-bundles/...` 对应文件。
2. 更新受影响的 canonical `.csproj` / `.sln` archetype。
3. 更新 architecture / freeze 文档。
4. 跑通对应 solution archetype 的 proof 或 full-solution ingest 验证。

只替换 DLL、不更新验证或文档，视为不完整修改。

## 7. 对后续阶段的约束

- Phase 1 不再讨论“bundle 放哪里”；只按本文件消费。
- Phase 3 hot-update patch 仍然复用同一套 repo-owned policy，不允许单独演化出 patch 私有 reference 规则。
- Phase 6 的 engineering gate 必须能区分：
  - base bundle 闭包问题；
  - controlled supplement 闭包问题；
  - 项目图自身的问题。

## 8. 验收标准

满足以下条件即视为本阶段冻结完成：

- 当前 retained solution archetype 都能落到 `ProjectReference / PackageReference / canonical base bundle / controlled supplement` 四种来源之一。
- `CoreLibReferenceSolution` 与 `ReferenceAssemblySolution` 的语义边界清晰，不再混用。
- patch/reference 场景不再需要本机绝对路径作为隐含前提。
