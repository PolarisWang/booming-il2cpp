# DotNet Foundation DLL Staging

该目录保存仓库受控的官方 `.NET` DLL 子集，专门服务于两类消费方：

- `subjects/` 下需要显式 `<Reference HintPath=...>` 的工程。
- foundation pack nativeization 主线当前要处理的 runtime DLL。

当前受控布局如下：

- `net8.0/ref/`
  - `CoreLibReferenceSolution` 当前显式引用的官方 reference assemblies。
- `net8.0/runtime/System.Private.CoreLib.dll`
  - `.NET 8` 当前 nativeization 输入。
- `net10.0/runtime/System.Private.CoreLib.dll`
  - `.NET 10` 当前 nativeization 输入。

维护规则：

- 这里只放“提升进仓库受控范围”的子集，不镜像整套 SDK pack。
- 新增官方 DLL 时，必须同时更新消费它的 `.csproj`、架构文档和相关兼容性测试。
- phase0 catalog 仍然负责描述完整 pack authority；`src/dll/` 只负责项目消费与 proof/nativeization 的稳定落点。
