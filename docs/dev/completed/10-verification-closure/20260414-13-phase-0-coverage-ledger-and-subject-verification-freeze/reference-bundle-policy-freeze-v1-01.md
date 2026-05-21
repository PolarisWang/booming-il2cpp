# Reference Bundle Policy Freeze v1.01

Date: 2026-04-14 12:52:36 +08:00
Status: frozen

## 1. 目的

冻结本轮 roadmap 对 reference/corelib/patch bundle 的工程边界，避免后续 child task 再次回到“本机 SDK 路径”或“临时复制 DLL”模式。

## 2. 基本结论

继续采用：

- repository-owned reference bundle

明确拒绝：

- 开发机绝对路径
- 运行时动态改写 `.csproj`
- 把整包 SDK 镜像复制进仓库

## 3. 当前冻结路径

当前 corelib foundation bundle 路径：

- `assets/reference-bundles/dotnet-foundation/net8.0`

当前受控 bundle 成员：

- `mscorlib.dll`
- `netstandard.dll`
- `System.Collections.dll`
- `System.Console.dll`
- `System.Linq.dll`
- `System.Runtime.dll`
- `System.Runtime.Extensions.dll`

## 4. Archetype 绑定关系

| Archetype | Bundle Policy | 说明 |
| --- | --- | --- |
| `ReferenceAssemblySolution` | repository-owned reference DLL | 验证 `<Reference Include=...><HintPath=...>` 的显式 DLL 引用闭包 |
| `CoreLibReferenceSolution` | repository-owned medium `foundation bundle` | 验证显式 corelib/BCL DLL 闭包 |
| `MixedReferenceClosureSolution` | `ProjectReference + Reference + PackageReference` 混合消费 | 验证复杂 solution 闭包 |
| `ReferenceAssemblyPatchSolution` | repository-owned patch reference bundle | 验证 patch 图引用 reference DLL 的 contract |

## 5. 刷新规则

只有同时满足以下条件时，才允许扩充 bundle：

1. 新 archetype 或 capability 的长期验证确实需要该 DLL。
2. 不能通过现有 bundle 或现有 archetype 表达。
3. 增加后仍然保持“foundation bundle”，而不是退化为 SDK 镜像。

刷新 bundle 时必须同步完成：

1. 更新 `assets/reference-bundles/...`
2. 更新受影响的 archetype `.csproj`
3. 更新 architecture / freeze 文档
4. 补对应 proof 或 solution 级验证

## 6. Phase 0 冻结判断

本轮默认不新增新的 bundle 目录，也不扩大 bundle 范围。Phase 0 只冻结口径：

- `CoreLibReferenceSolution` 继续消费现有 medium `foundation bundle`
- `ReferenceAssemblySolution` 和 `ReferenceAssemblyPatchSolution` 继续消费仓库受控 DLL
- `MixedReferenceClosureSolution` 继续作为复杂依赖闭包的工程级 archetype

## 7. 与后续阶段的关系

- Phase 1 以后若需要扩充 bundle，必须把变更理由写回 coverage ledger 和 docs。
- Phase 4 / Phase 5 负责验证 bundle 的工程级消费是否仍然稳定。
- 若只为个别样例临时需要 DLL，而不具备长期代表性，不允许扩充 bundle。
