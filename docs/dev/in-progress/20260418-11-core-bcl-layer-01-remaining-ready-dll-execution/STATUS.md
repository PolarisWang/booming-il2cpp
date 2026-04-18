---
task_id: 20260418-11-core-bcl-layer-01-remaining-ready-dll-execution
title: Core BCL Layer 01 Remaining Ready DLL Execution
task_type: plan
lifecycle_status: in_progress
phase: execution
created_at: 2026-04-18 14:56:35 +08:00
updated_at: 2026-04-18 22:47:46 +08:00
latest_stop_point: cleared the CodeGen build lock, revalidated async native-aot unit coverage, and proved that `TaskAndValueTaskFlowBenchmark` currently routes only to `windows-managed-perf` by mode-aware declared benchmark selection
current_dir: docs/dev/in-progress/20260418-11-core-bcl-layer-01-remaining-ready-dll-execution
parent_task_id: 20260418-01-dotnet-foundation-pack-nativeization-roadmap
source_task_id: 20260418-01-dotnet-foundation-pack-nativeization-roadmap
source_relation: roadmap-child-core-bcl-layer-01-remaining-ready-dll-execution
design_doc: docs/dev/in-progress/20260418-11-core-bcl-layer-01-remaining-ready-dll-execution/design-v1-01.md
plan_doc: docs/dev/in-progress/20260418-11-core-bcl-layer-01-remaining-ready-dll-execution/plan-v1-01.md
total_tasks: 7
current_task: 冻结剩余 21 个 ready DLL 的 scope、程序集清单、`gameplay-core-ready` milestone 与 formal verification objects，并纳入单 DLL 完整 native 化 checklist
active: true
---

## 2026-04-18 22:47 Stop Point

- 已清理 `Chaos.IL2CPP.CodeGen.dll` 构建锁：执行 `dotnet build-server shutdown` 并清理残留 `dotnet` / `VBCSCompiler` 后，`dotnet build src/managed/Chaos.IL2CPP.CodeGen/Chaos.IL2CPP.CodeGen.csproj -c Release` 通过。
- 已复验 async native-aot 相关最小单测集：
  - `python -m pytest tests/unit/compatibility/test_phase3_async_task_builder_native_aot.py -q`
  - `python -m pytest tests/unit/compatibility/test_phase5_virtual_dispatch_native_aot.py -k task_and_valuetask_benchmark -q`
- 已重跑 `python build/toolchains/run/run.py test declared-benchmark --id "declared-benchmark/SolutionCorePack::CoreRuntimeBenchmarks::CoreRuntimeBenchmarks.TaskAndValueTaskFlowBenchmarkEntry::RunWorkload()" --json`，run id 为 `20260418-224041-windows-ad67`，结果仅产出 `windows-managed-perf`，状态 `ok`。
- 已确认这是当前 authority 生效而非 native benchmark 静默失败：benchmark 源定义为 `Modes = ChaosExecutionMode.Managed`，`build/toolchains/run/testing/registry.py` 的 `_select_declared_matrix(...)` 也会按 mode 选择 `runtime-perf-collect`。
- 已补充自动化证据：
  - `python -m pytest tests/unit/registry/test_declared_registry_matrix_selection.py -q`
  - `python -m pytest tests/unit/performance/test_benchmark_subject_sources.py -k solution_core_pack_declares_internal_performance_slice_benchmarks -q`
- 结论：本 child 当前不能声称完成 `managed -> native -> hotupdate` 链路验证；本轮只确认 async 相关 native-aot 单测恢复稳定，且 `TaskAndValueTaskFlowBenchmark` 在当前仓库规则下属于 managed-only benchmark。

## 2026-04-18 19:41 Stop Point

- 已把 `gameplay-core-ready` 固化为当前 child 内的阶段性验收目标，不新增 sibling child，也不新建顶层 phase。
- 已把“DLL 完整 native 化”明确为 `surface complete -> semantic complete -> native executable evidence complete -> per-DLL certified complete` 四层判定。
- 已把当前 child 的下一步改为：先补齐 `20260418-10` 基线 DLL 的 report 输入，再优先处理当前批次 `7` 个 gameplay core DLL 的 evidence projection。

## 2026-04-18 16:41 Stop Point

- 已确认“逐 DLL full certified”不另起顶层 phase，而是直接并入当前 child `20260418-11`。
- 当前 child 已从“batch execution”扩展为“batch execution + per-assembly certification closeout”。
- 后续除 batch summary 外，还需输出 `assembly-certification-report/<assembly>.json` 与 `layer-certification-summary-v1-01.json`。
- 逐 DLL `full certified` 的状态机、最小 report 字段与 benchmark 特殊规则，已收口到 `dll-full-certified-acceptance-path-v1-01.md`。

## 2026-04-18 14:56 Stop Point

- 新建子任务 [20260418-11-core-bcl-layer-01-remaining-ready-dll-execution](/D:/agent/booming-il2cpp/docs/dev/in-progress/20260418-11-core-bcl-layer-01-remaining-ready-dll-execution/STATUS.md) 并激活。
- 已冻结 `core-bcl-layer-01` 剩余 `21` 个 ready DLL：
  - `System.ComponentModel`
  - `System.Diagnostics.DiagnosticSource`
  - `System.Diagnostics.FileVersionInfo`
  - `System.Diagnostics.StackTrace`
  - `System.Formats.Tar`
  - `System.IO.Compression`
  - `System.IO.FileSystem.DriveInfo`
  - `System.IO.IsolatedStorage`
  - `System.IO.Pipes`
  - `System.Reflection.DispatchProxy`
  - `System.Reflection.TypeExtensions`
  - `System.Resources.Writer`
  - `System.Runtime.CompilerServices.VisualC`
  - `System.Runtime.InteropServices.JavaScript`
  - `System.Runtime.Numerics`
  - `System.Runtime.Serialization.Primitives`
  - `System.Security.Claims`
  - `System.Text.Encoding.CodePages`
  - `System.Text.Encodings.Web`
  - `System.Text.RegularExpressions`
  - `System.Xml.XPath.XDocument`
- 已基于 `20260418-06` 的逐程序集 plan 聚合 formal verification objects，当前冻结为 `11` 个 unique native proofs、`7` 个 unique benchmarks、`4` 个 engineering validations，并落盘 `remaining-ready-scope-v1-01.json`。

## 当前结论

- 当前 child 不再只是“剩余 `21` 个 ready DLL 的整批执行任务”，而是同时承载一个更窄的阶段性验收目标：`gameplay-core-ready`。
- `gameplay-core-ready` 的范围固定为 `15` 个 DLL：`20260418-10` 的 `8` 个基线 DLL，加上当前 child 中 `7` 个更贴近基础游戏业务逻辑的核心 DLL。
- 当前 child 的整批 shared execution 仍然保留，但阶段性结论必须先回落到这 `15` 个 DLL 的逐 DLL machine-readable report，而不是只看 batch summary。
- `20260418-10` 已证明 representative execution template 可用；本轮不再做代表抽样，而是直接消化余下的 `21` 个 ready DLL。
- 剩余 batch 已覆盖 runtime-services、io-and-networking、reflection-and-metadata、text-and-encoding、unsafe-and-low-level 五类能力，能显著提升 `core-bcl-layer-01` 的实际覆盖率。
- 下一步应先刷新 registry snapshot，然后按聚合后的 canonical commands 直接进入 proof / engineering / benchmark 执行，并把 shared run evidence 回填成逐 DLL certification 结论。

## 最近摘要

- 2026-04-18 19:41:19 +08:00: 将 `gameplay-core-ready` 固化为当前 child 的阶段性验收目标，并把单 DLL 完整 native 化四层 checklist 接入设计、计划与 scope JSON。
- 2026-04-18 16:41:13 +08:00: 将当前 child 扩展为“batch execution + per-assembly certification closeout”，正式纳入逐 DLL `full certified` 路径。
- 2026-04-18 14:56:35 +08:00: 创建并激活 `20260418-11`，冻结 remaining ready batch scope、formal verification objects 与 `docs/dev/ACTIVE.md` 指针。
- 2026-04-18 14:27:06 +08:00: `20260418-10` 已归档 `completed`，完成 representative batch 的 proof / engineering / benchmark 闭环，并生成 machine-readable summary。

## 下一步

- 先把 `20260418-10` 代表性批次 `8` 个 DLL 的 shared evidence 对接到逐 DLL report 输入，补齐 `gameplay-core-ready` 的 baseline 一侧。
- 运行 `python build/toolchains/run/run.py test registry refresh --json`，确认 registry authority 与当前 repo 状态一致。
- 依次执行 `11` 个 unique native proofs、`4` 个 engineering validations 与 `7` 个 unique benchmarks，并优先回填当前 `7` 个 gameplay core DLL 的 `assembly-certification-report`。
- 当 `15` 个 gameplay core DLL 都具备逐 DLL report 后，先判断 `gameplay-core-ready`；其后再继续完整投影剩余 tail DLL，并最终决定 `core-bcl-layer-01` 是否可整层 completed。

## 风险 / 阻塞

### risks

- 剩余 `21` 个 DLL 跨越多个 capability family，proof / benchmark 集合显著多于 `20260418-10`，更容易暴露新的 metadata / runtime helper / interop closure 缺口。
- `System.Runtime.InteropServices.JavaScript`、`System.Reflection.DispatchProxy`、`System.Text.RegularExpressions` 等方向的 formal objects 更偏 runtime-services / reflection / marshaling，若失败，可能回指到更底层 substrate。
- benchmark 可能继续返回 `no-baseline`，因此本轮仍需要区分“吞吐证据”与“基线回归认证”。
- 如果共享 run evidence 不能稳定回填成逐 DLL machine-readable report，本 child 仍只能证明 execution template，而不能完成 full-certified closeout。

- 如果 `20260418-10` 的 `8` 个基线 DLL 不能补齐逐 DLL report 输入，`gameplay-core-ready` 会停在阶段性目标而无法 machine-readable 收口。

### blockers

- 当前 blocker: `none`
- 说明: 当前阻塞不是工具链，而是尚未开始执行剩余 batch 的 canonical commands。

## 验证

- `Get-Content docs/dev/in-progress/20260418-11-core-bcl-layer-01-remaining-ready-dll-execution/design-v1-01.md -Raw | Out-Null`
  - 结果: 通过
- `Get-Content docs/dev/in-progress/20260418-11-core-bcl-layer-01-remaining-ready-dll-execution/plan-v1-01.md -Raw | Out-Null`
  - 结果: 通过
- `Get-Content docs/dev/in-progress/20260418-11-core-bcl-layer-01-remaining-ready-dll-execution/dll-full-certified-acceptance-path-v1-01.md -Raw | Out-Null`
  - 结果: 通过
- `Get-Content docs/dev/in-progress/20260418-11-core-bcl-layer-01-remaining-ready-dll-execution/remaining-ready-scope-v1-01.json -Raw | ConvertFrom-Json | Out-Null`
  - 结果: 通过
- `Get-Content docs/dev/in-progress/20260418-01-dotnet-foundation-pack-nativeization-roadmap/roadmap-v1-01.md -Raw | Out-Null`
  - 结果: 通过

## wiki

- 当前不写入 wiki
- 原因: 当前新增的是 roadmap / child 内的阶段性验收口径与执行顺序，尚未改变 repo 级 test pipeline / AOT onboarding authority
