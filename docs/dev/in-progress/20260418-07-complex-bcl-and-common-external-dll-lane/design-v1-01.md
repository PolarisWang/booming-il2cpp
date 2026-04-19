# Complex BCL And Common External DLL Lane 设计 v1.01

## 目标

在 `core-bcl-layer-01` ready set 已达成 `29/29 finalStatus = ok` 后，把 dependency-layer generator 推进到下一层 ready set，并对 `core-bcl-layer-02` 执行逐 DLL nativeization certification。

## 范围

当前 child 覆盖 `core-bcl-layer-02` 的跨版本并集，共 `13` 个程序集：

- `System.Collections.Immutable`
- `System.Formats.Asn1`
- `System.IO.Compression.Brotli`
- `System.IO.Compression.ZipFile`
- `System.IO.Pipelines`
- `System.Linq`
- `System.Net.ServerSentEvents`
- `System.ObjectModel`
- `System.Runtime.InteropServices`
- `System.Runtime.Serialization.Formatters`
- `System.Security.Principal.Windows`
- `System.Text.Json`
- `System.Threading.Tasks.Parallel`

## 输入 Authority

- 已认证输入: `docs/dev/in-progress/20260418-11-core-bcl-layer-01-remaining-ready-dll-execution/layer-certification-summary-v1-01.json`
- layer generator: `Chaos.IL2CPP.Tools.FoundationPack.Analysis dependency-layer`
- per-DLL certification: `Chaos.IL2CPP.Tools.FoundationPack.Analysis dependency-layer-certify`
- registry authority: `artifacts/tests/registry/current/index.json`

## 关键设计

- `dependency-layer` generator 现在接受 `--certified-assemblies-path`，从 `certifiedAssemblies` 或 `certificationReports` 中读取 `finalStatus = ok` 的 assembly。
- 已认证的 `core-bcl-layer-01` 被标记为 `certified`，下游依赖全部满足后，`core-bcl-layer-02` 变为新的 `ready` layer。
- 当前 child 只消费 `core-bcl-layer-02`，不把后续 layer 混入本轮 certification。

## 非目标

- 不在本 child 内验证 hotupdate consumer。
- 不把 `net10.0` delta lane 合并到当前执行任务。
- 不用共享 proof / benchmark 结果替代逐 DLL report；每个 DLL 必须有自己的 `assembly-certification-report/<assembly>.json`。
