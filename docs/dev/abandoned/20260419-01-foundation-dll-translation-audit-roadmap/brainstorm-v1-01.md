# Foundation DLL Translation Audit Brainstorm v1.01

Date: 2026-04-19 12:00:00 +08:00
Status: approved-for-roadmap

## 1. 背景

上一条 foundation pack nativeization roadmap 已经完成以下事情：

- dependency-layer 已推进到终态，`net8.0` 与 `net10.0` 都没有剩余 ready layer。
- `HotUpdateHostPack` consumer lane 已完成 proof、benchmark 与 engineering/workload 验证。
- `.NET 10` managed ref delta 已完成 closure。

但新的要求不是继续宣称“roadmap 已完成”，而是补齐一条更强的、可审计的验证路径：

`实现 -> DLL 完整翻译证明 -> managed proof -> native proof -> hotupdate proof -> codegen 结果审核 -> 用户可编译/可验收产物`

这条路径必须回答两个问题：

- 某个 DLL 是否真的已经完整翻译，而不是只被 shared proof 或 layer summary 投影为 `finalStatus = ok`。
- 用户是否能从 subject 生成的 `.sln` 与 artifact 中看到完整链路，并自己编译和验证。

## 2. 需要调整的核心点

### 2.1 `System.Private.CoreLib` 必须先完整验证

当前已完成的 CoreLib 工作只覆盖了第一批 semantic-family execution entry 与相关 substrate，不应再被表述为“完整强验证已完成”。

新的任务必须采用已确认的方案 C：

- 把 `System.Private.CoreLib` 作为独立 full-verification target。
- 不以“若干代表性 proof 通过”替代完整结论。
- completion gate 必须覆盖 surface、semantic ledger、runtime helper、native codegen、managed/native/hotupdate consumer 和 codegen review。

### 2.2 Complex BCL 13 DLL 必须按顺序逐个验证

上一轮 `20260418-07` 中的 13 个 DLL 已有 layer-level certification 结果，但这次需要更强的顺序验收：

1. `System.Collections.Immutable`
2. `System.Formats.Asn1`
3. `System.IO.Compression.Brotli`
4. `System.IO.Compression.ZipFile`
5. `System.IO.Pipelines`
6. `System.Linq`
7. `System.Net.ServerSentEvents`
8. `System.ObjectModel`
9. `System.Runtime.InteropServices`
10. `System.Runtime.Serialization.Formatters`
11. `System.Security.Principal.Windows`
12. `System.Text.Json`
13. `System.Threading.Tasks.Parallel`

每个 DLL 都需要自己的 audit packet，而不是只依赖 layer summary。

### 2.3 hotupdate 路径要保留，但不能污染 foundation completion

hotupdate 仍然是 consumer lane，不是 foundation DLL nativeization 的能力发现入口。

但对用户验收来说，subject 必须能看到完整链路：

- managed proof 证明语义基线。
- native proof 证明 AOT/codegen 产物可执行。
- hotupdate proof 证明 certified foundation 能被 HybridCLR 风格 patch/host 路径消费。

### 2.4 codegen review 必须成为正式产物

之前的验证更偏向 proof/benchmark/report，这次必须把 codegen 审核显式纳入 gate：

- JSON 供机器消费。
- Markdown 供人工审阅。
- 审核范围包括 generated C++、manifest、dispatch binding、runtime helper binding、hotupdate binding。

## 3. 工作 DLL 范围

### Phase 1

- `System.Private.CoreLib`

### Phase 2

- `System.Runtime`
- `System.Runtime.Extensions`
- `System.ObjectModel`
- `System.Collections`
- `System.Collections.NonGeneric`
- `System.Collections.Concurrent`
- `System.Memory`
- `System.Buffers`
- `System.ValueTuple`
- `System.Runtime.CompilerServices.Unsafe`
- `System.Linq`
- `System.Text.Encoding`
- `System.Text.Encoding.Extensions`
- `System.Text.RegularExpressions`
- `System.IO`
- `System.IO.FileSystem`
- `System.Threading`
- `System.Threading.Tasks`
- `System.Threading.Tasks.Extensions`
- `System.Text.Json`

### Phase 3

- `System.Net.Http`
- `System.Net.Primitives`
- `System.Net.Requests`
- `System.Net.Security`
- `System.IO.Compression`
- `System.IO.Compression.ZipFile`
- `System.Threading.Channels`
- `System.Reflection`
- `System.Reflection.Primitives`
- `System.Reflection.TypeExtensions`
- `System.Runtime.InteropServices`
- `System.Diagnostics.Debug`
- `System.Diagnostics.DiagnosticSource`
- `System.Numerics`
- `System.Runtime.Numerics`
- `System.Security.Cryptography.Primitives`
- `System.Security.Cryptography.X509Certificates`

### Phase 4

- `System.Xml*`
- `System.Runtime.Serialization*`
- `System.Reflection.Emit*`
- `System.ComponentModel*`
- `System.Data*`
- `System.Runtime.Loader`
- `System.Console`
- `Microsoft.Win32*`
- `System.Drawing`
- `WindowsBase`
- `System.Windows`
- `System.Configuration`
- `System.ServiceProcess`
- `System.ServiceModel.Web`
- `Microsoft.VisualBasic*`
- `Microsoft.CSharp`

## 4. 分流判断

这不是单次实现任务，原因是：

- 需要多阶段推进。
- 需要为 CoreLib、Complex BCL、subject solution、hotupdate 和 codegen review 分别创建子任务。
- 每个 DLL 都需要独立 audit packet。
- 现阶段还需要先冻结 schema、顺序和验收口径。

因此进入 roadmap，而不是直接进入单个 implementation plan。

