# Core BCL Layer 01 Remaining Ready DLL Execution 设计 v1.01

## 1. 背景

`20260418-10` 已完成 `core-bcl-layer-01` representative priority batch 的真实 proof / benchmark / engineering gate 闭环，证明 `SolutionCorePack` 当前 registry canonical commands 可以支撑 layer-01 的执行模板。但 `core-bcl-layer-01` 共有 `29` 个 ready DLL，其中仍有 `21` 个程序集尚未进入真实执行闭环。

下一步不应重新做“代表切片”，而应直接针对剩余 `21` 个 ready DLL 建立 execution child，消费 `20260418-06` 的 per-assembly authority，并把剩余 batch 的 formal proof / benchmark / engineering gate 真正跑完。同时，需要把共享 run evidence 回填为逐 DLL 的 machine-readable certification 结论，避免继续停留在“batch 模板已验证”但单 DLL 仍不可审计的状态。

## 2. 目标

- 冻结 `core-bcl-layer-01` 剩余 `21` 个 ready DLL 的 machine-readable scope。
- 汇总并执行这些程序集对应的全部 unique native proofs、benchmarks 与 engineering validations。
- 为每个 DLL 生成独立的 `assembly-certification-report`，把共享 proof / benchmark / engineering gate 证据回填成逐 DLL `full certified` 判断。
- 为后续声明 `core-bcl-layer-01` full certification 提供剩余覆盖面的正式执行证据与逐 DLL closeout 结果。

## 3. 非目标

- 本任务不进入 `core-bcl-layer-02` 或更高 layer。
- 本任务不新增新的 `SolutionCorePack` proof / benchmark 资产；只消费 registry 与 `assembly-nativeization-plan/*.json` 中已有 authority。
- 本任务不为共享 proof / benchmark 做逐 DLL 重跑；执行仍按 unique canonical commands 去重。
- 本任务不把 hotupdate consumer validation 合并进来；`hotupdateImpact` 仍为 `None`。
- 本任务不修改 dependency-layer builder、registry schema 或 `subject.features.json` 的长期 authority。

## 4. 设计原则

- 剩余覆盖优先：直接覆盖 representative batch 之外的全部 `21` 个 ready DLL，而不是再次抽样。
- per-assembly authority 聚合：formal verification objects 由 `20260418-06` 的逐程序集 plan 聚合得到，避免口头归纳。
- 执行去重、认证逐 DLL：proof / benchmark / engineering gate 继续按 unique canonical commands 执行；completion 判断回到逐程序集 report。
- owner subject 复用：继续使用 `SolutionCorePack` 作为 proof / benchmark / engineering validation authority。
- 双 TFM 同构：`net8.0` 与 `net10.0` 保持同一批程序集、同一套 required gates 与 formal objects。

## 5. Remaining Batch 范围

当前 remaining ready batch 冻结为 `21` 个程序集：

- runtime-services:
  - `System.ComponentModel`
  - `System.Diagnostics.DiagnosticSource`
  - `System.Diagnostics.FileVersionInfo`
  - `System.Diagnostics.StackTrace`
  - `System.Formats.Tar`
  - `System.Resources.Writer`
  - `System.Runtime.CompilerServices.VisualC`
  - `System.Runtime.Numerics`
  - `System.Security.Claims`
- io-and-networking:
  - `System.IO.Compression`
  - `System.IO.FileSystem.DriveInfo`
  - `System.IO.IsolatedStorage`
  - `System.IO.Pipes`
- reflection-and-metadata:
  - `System.Reflection.DispatchProxy`
  - `System.Reflection.TypeExtensions`
  - `System.Runtime.Serialization.Primitives`
  - `System.Xml.XPath.XDocument`
- text-and-encoding:
  - `System.Text.Encoding.CodePages`
  - `System.Text.Encodings.Web`
  - `System.Text.RegularExpressions`
- unsafe-and-low-level:
  - `System.Runtime.InteropServices.JavaScript`

### `gameplay-core-ready` 阶段性验收目标

当前 child 仍然负责剩余 `21` 个 ready DLL 的 shared execution 与 per-DLL closeout，但在整层 `core-bcl-layer-01 completed` 之前，需要先给出一个更贴近基础游戏业务逻辑的阶段性验收目标：`gameplay-core-ready`。

该 milestone 的范围固定为 `15` 个 DLL：

- 来自 `20260418-10-core-bcl-layer-01-priority-batch-execution` 的 `8` 个基线 DLL：
  - `System.Collections`
  - `System.Collections.Concurrent`
  - `System.Collections.NonGeneric`
  - `System.Memory`
  - `System.Threading`
  - `System.Threading.Channels`
  - `System.Threading.Tasks.Dataflow`
  - `System.Console`
- 来自当前 child 的 `7` 个核心 DLL：
  - `System.ComponentModel`
  - `System.Runtime.Numerics`
  - `System.Text.Encodings.Web`
  - `System.Text.Encoding.CodePages`
  - `System.IO.Compression`
  - `System.Reflection.TypeExtensions`
  - `System.Runtime.Serialization.Primitives`

当前 child 内部对上述 `7` 个核心 DLL 的优先顺序固定为：

1. `System.ComponentModel`
2. `System.Runtime.Numerics`
3. `System.Text.Encodings.Web`
4. `System.Text.Encoding.CodePages`
5. `System.IO.Compression`
6. `System.Reflection.TypeExtensions`
7. `System.Runtime.Serialization.Primitives`

该 milestone 的验收口径是：

- 上述 `15` 个 DLL 都存在自己的 `assembly-certification-report/<assembly>.json`
- 上述 `15` 个 DLL 都满足 `finalStatus = ok`
- 每个 DLL 都满足“完整 native 化四层判定”：`surface complete -> semantic complete -> native executable evidence complete -> per-DLL certified complete`

`gameplay-core-ready` 只代表 `dependency-layer` 内的阶段性收口，不等于 `core-bcl-layer-01 completed`。像 `System.Diagnostics.DiagnosticSource`、`System.Reflection.DispatchProxy`、`System.Runtime.InteropServices.JavaScript`、`System.Text.RegularExpressions` 这类尾部 DLL，仍然留在后续整层 closeout 中继续处理。

### 单 DLL 完整 native 化判定

当前 child 对单 DLL 的“完全翻译成 native 代码”不采用“能 build/link/smoke 就算通过”的口径，而是统一复用 `dll-full-certified-acceptance-path-v1-01.md` 中的四层判定：

1. `surface complete`
2. `semantic complete`
3. `native executable evidence complete`
4. `per-DLL certified complete`

这意味着当前 child 的 shared execution 只是证据来源；真正的完成判定必须回落到逐 DLL 的 machine-readable report。

## 6. Formal Objects

从 `20260418-06` 的逐程序集 plan 聚合得到，本轮需要执行：

- `11` 个 unique native proofs：
  - `abi-widening-proof`
  - `aot-closure-validation-proof`
  - `conversion-ops-proof`
  - `direct-callvirt-proof`
  - `function-pointer-proof`
  - `member-metadata-lookup-proof`
  - `metadata-supplement-proof`
  - `reflection-interop-closure-proof`
  - `required-instantiation-closure-proof`
  - `span-memory-proof`
  - `task-valuetask-flow-proof`
- `7` 个 unique benchmarks：
  - `allocation-bench`
  - `function-pointer-bench`
  - `member-metadata-lookup-bench`
  - `required-instantiation-closure-bench`
  - `span-memory-bench`
  - `string-utf8-marshaling-bench`
  - `task-valuetask-flow-bench`
- `4` 个 engineering validations：
  - `managed-build`
  - `managed-runtime-output`
  - `native-executable-smoke`
  - `project-graph`

## 7. 产物

- `remaining-ready-scope-v1-01.json`
- `native-proof-summary-v1-01.json`
- `nativeization-throughput-benchmark-v1-01.json`
- `dll-full-certified-acceptance-path-v1-01.md`
- `assembly-certification-report/`
- `layer-certification-summary-v1-01.json`
- `STATUS.md`
- `plan-v1-01.md`
- `notes/progress-v1-01.md`

## 8. 执行思路

1. 读取 `20260418-06` 的 `layer-plan-v1-01.json` 与 `assembly-nativeization-plan/*.json`，冻结 remaining batch scope。
2. 先刷新 registry snapshot，确保 canonical command authority 与当前 repo 状态一致。
3. 依次执行 `11` 个 unique native proofs、`4` 个 engineering validations、`7` 个 unique benchmarks。
4. 生成 batch 级 machine-readable summary，记录所有 run id、summary/events/console 路径与 benchmark throughput 指标。
5. 将共享 run evidence 回填到 `21` 个 `assembly-nativeization-plan/<assembly>.json`，为每个 DLL 生成 `assembly-certification-report/<assembly>.json`。
6. 只有当全部逐 DLL report 均满足 `requiredGates`、formal proofs、benchmarks 与 blocker 清零时，才判断 `core-bcl-layer-01` 是否具备进入“整层 completed”前置条件。

逐 DLL closeout 的具体状态机、report 最小字段与 benchmark 特殊规则，以 `dll-full-certified-acceptance-path-v1-01.md` 为准。

## 9. 完成判断

- `remaining-ready-scope-v1-01.json` 能完整描述剩余 `21` 个程序集与聚合后的 formal objects。
- `11` 个 unique proofs、`4` 个 engineering validations、`7` 个 benchmarks 全部进入真实执行闭环。
- `native-proof-summary-v1-01.json` 与 `nativeization-throughput-benchmark-v1-01.json` 可回溯到具体 run evidence。
- `assembly-certification-report/<assembly>.json` 对剩余 `21` 个 DLL 全量生成，且每个 report 都能追溯到对应的 shared run evidence。
- `layer-certification-summary-v1-01.json` 只能在全部逐 DLL report 完整后生成，并以逐 DLL `finalStatus` 作为整层 completed 的唯一判断口径。
- child task、父 roadmap、`docs/dev/ACTIVE.md` 与索引文件保持同步。
