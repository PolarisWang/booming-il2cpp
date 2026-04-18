---
task_id: 20260418-01-dotnet-foundation-pack-nativeization-roadmap
title: DotNet Foundation Pack Nativeization Roadmap
task_type: roadmap
lifecycle_status: in_progress
phase: roadmap
created_at: 2026-04-18 01:19:47 +08:00
updated_at: 2026-04-18 19:41:19 +08:00
latest_stop_point: inserted `gameplay-core-ready` as a dependency-layer staged acceptance milestone and froze the single-DLL complete-nativeization checklist into the roadmap and active child
current_dir: docs/dev/in-progress/20260418-01-dotnet-foundation-pack-nativeization-roadmap
parent_task_id:
source_task_id: 20260416-01-aot-core-ir-gap-audit-and-next-expansion-roadmap
source_relation: architectural-redesign-from-entry-driven-aot-to-framework-pack-nativeization
active: false
---

## 2026-04-18 19:41 Stop Point

- 已把 `gameplay-core-ready` 明确为当前 `dependency-layer` 内的阶段性验收目标，不新增顶层 phase。
- 已把“DLL 完整 native 化”收口为统一四层判定：`surface complete -> semantic complete -> native executable evidence complete -> per-DLL certified complete`。
- 已把当前 child `20260418-11` 从“剩余 `21` 个 DLL 平推执行”调整为“保留整批 scope，但先围绕 `gameplay-core-ready` 组织阶段性验收结论”。

## 2026-04-18 16:41 Stop Point

- 已确认“逐 DLL full certified”不新增顶层 phase，而是直接插入当前 `dependency-layer` 主线。
- 父 roadmap 已补充 `assembly-certification-report/<assembly>.json` 与 `layer-certification-summary.json` 作为 `dependency-layer` deliverables。
- 激活 child `20260418-11` 已扩展为“batch execution + per-assembly certification closeout”路径。

## 2026-04-18 14:56 Stop Point

- 子任务 [20260418-11-core-bcl-layer-01-remaining-ready-dll-execution](/D:/agent/booming-il2cpp/docs/dev/in-progress/20260418-11-core-bcl-layer-01-remaining-ready-dll-execution/STATUS.md) 已创建并激活。
- 当前 child 已冻结 `core-bcl-layer-01` 剩余 `21` 个 ready DLL，并聚合出 `11` 个 unique native proofs、`7` 个 unique benchmarks、`4` 个 engineering validations。
- `docs/dev/ACTIVE.md` 已重新创建并指向当前 child。

## 当前结论

- roadmap 已完成：
  - framework pack catalog / asset policy
  - facade/shim contract lane
  - `System.Private.CoreLib` certification substrate
  - `System.Private.CoreLib` 首批 semantic-family execution entry 与闭环实跑
  - dependency-driven 核心 `System.*` DLL layer / nativeization authority 冻结
  - `core-bcl-layer-01` representative priority batch proof / benchmark / engineering gate 真跑闭环
- roadmap 当前还额外明确了一个中间里程碑：
  - `gameplay-core-ready`
  - 其范围是 `20260418-10` 的 `8` 个基线 DLL，加上当前 child 中 `7` 个更贴近基础游戏业务逻辑的核心 DLL
  - 它代表阶段性验收通过，不等于 `core-bcl-layer-01` 已 completed
- roadmap 仍未完成；当前结论是 dependency-layer 主线已经从“纯 authority 冻结”推进到“真实 execution template 已被验证”，并已正式纳入逐 DLL `full certified` closeout，但整层 `29` 个 ready DLL 尚未 full certified。

## 最近摘要

- 2026-04-18 19:41:19 +08:00: 将 `gameplay-core-ready` 并入当前 `dependency-layer` 主线作为阶段性验收目标，并冻结单 DLL 完整 native 化四层 checklist。
- 2026-04-18 16:41:13 +08:00: 确认逐 DLL `full certified` 目标直接插入当前 `dependency-layer` 主线，更新父 roadmap 与激活 child `20260418-11` 的 deliverables / plan。
- 2026-04-18 14:56:35 +08:00: 创建并激活 `20260418-11`，冻结剩余 `21` 个 ready DLL、聚合 formal verification objects，并恢复 `docs/dev/ACTIVE.md` 指针。
- 2026-04-18 14:27:06 +08:00: `20260418-10` 已归档 `completed`，完成 representative batch 的 proof / engineering / benchmark 闭环，并生成 machine-readable summary。
- 2026-04-18 13:17:03 +08:00: 创建并激活 `20260418-10`，冻结 representative batch scope、formal verification objects 与 `docs/dev/ACTIVE.md` 指针。
- 2026-04-18 13:06:12 +08:00: `20260418-06` 已归档 `completed`，完成 dependency-layer artifacts、首层 assembly plans、官方 DLL 受控落点与 `docs/dev` 索引同步。
- 2026-04-18 11:21:56 +08:00: `20260418-05` 已修复 subject-entry host 选择回归，并完成首批 27 条 canonical commands 的全量实跑。
- 2026-04-18 08:28:49 +08:00: `20260418-05` 生成 `semantic-family-execution-entry-v1-01.json`，把首批 family 绑定到 `SolutionCorePack` authority。
- 2026-04-18 03:14:08 +08:00: `20260418-04` 已归档完成，提供 corelib semantic substrate 与 helper contract 基底。
- 2026-04-18 02:52:08 +08:00: `20260418-03` 已归档完成，稳定输出 facade / shim classification、surface proof、native binding proof 与 benchmark artifacts。

## 下一步

- 先补 `20260418-10` 代表性批次 `8` 个基线 DLL 的 per-DLL report 输入映射，避免 `gameplay-core-ready` 只停留在 batch summary。
- 再继续执行 [20260418-11-core-bcl-layer-01-remaining-ready-dll-execution](/D:/agent/booming-il2cpp/docs/dev/in-progress/20260418-11-core-bcl-layer-01-remaining-ready-dll-execution/STATUS.md)，刷新 registry 后执行 `11` 个 proof、`4` 个 engineering validation 与 `7` 个 benchmark，并优先投影当前 `7` 个核心 DLL。
- 在 `15` 个 gameplay core DLL 都具备逐 DLL report 后，先给出 `gameplay-core-ready` 结论；其后再继续消化 tail DLL 并判断 `core-bcl-layer-01` 是否达到整层 completed 前置条件。
- 后续新增子任务继续使用语义化命名，不再扩散 `Phase{x}` 风格。

## 风险 / 阻塞

### risks

- 当前 roadmap 只完成了 representative batch，并未覆盖 `core-bcl-layer-01` 全部 `29` 个 ready DLL。
- 本轮 benchmark 全部为 `no-baseline`，只能证明当前吞吐可执行，尚不能替代历史回归基线。
- 如果共享 proof / benchmark 结果没有回填成逐 DLL machine-readable report，roadmap 会继续停留在“execution template 已验证”而不是“full certified”。
- 如果 `20260418-10` 的 `8` 个基线 DLL 不能补齐逐 DLL report，`gameplay-core-ready` 会被卡在阶段性验收而无法转成 machine-readable 收口。
- 更高层 layer 与 `.NET 10` delta closure 仍未开始，它们的 scope 明显大于当前已归档 child。

### blockers

- 当前 blocker: `none`
- 说明: 当前已进入剩余 batch 的正式执行 child，下一步主要是跑 canonical commands，而不是重新规划。

## 验证

- [20260418-10 STATUS](/D:/agent/booming-il2cpp/docs/dev/completed/20260418-10-core-bcl-layer-01-priority-batch-execution/STATUS.md)
  - 结果: 已记录 representative batch 的 completed 结论、run evidence 与下一步扩展建议
- [20260418-11 STATUS](/D:/agent/booming-il2cpp/docs/dev/in-progress/20260418-11-core-bcl-layer-01-remaining-ready-dll-execution/STATUS.md)
  - 结果: 已记录剩余 `21` 个 ready DLL 的 scope、formal verification objects、逐 DLL closeout 路径与下一步执行计划
- `Get-Content docs/dev/in-progress/20260418-11-core-bcl-layer-01-remaining-ready-dll-execution/dll-full-certified-acceptance-path-v1-01.md -Raw | Out-Null`
  - 结果: 通过
- [20260418-10 native-proof-summary-v1-01.json](/D:/agent/booming-il2cpp/docs/dev/completed/20260418-10-core-bcl-layer-01-priority-batch-execution/native-proof-summary-v1-01.json)
  - 结果: 已落盘 proof / engineering gate 证据
- [20260418-10 nativeization-throughput-benchmark-v1-01.json](/D:/agent/booming-il2cpp/docs/dev/completed/20260418-10-core-bcl-layer-01-priority-batch-execution/nativeization-throughput-benchmark-v1-01.json)
  - 结果: 已落盘 benchmark 吞吐指标与 `no-baseline` 状态
- `Get-Content docs/dev/in-progress/20260418-01-dotnet-foundation-pack-nativeization-roadmap/roadmap-v1-01.md -Raw | Out-Null`
  - 结果: 通过
- `Get-Content docs/dev/in-progress/20260418-11-core-bcl-layer-01-remaining-ready-dll-execution/remaining-ready-scope-v1-01.json -Raw | ConvertFrom-Json | Out-Null`
  - 结果: 通过
- `Get-Content docs/dev/in-progress/20260418-11-core-bcl-layer-01-remaining-ready-dll-execution/design-v1-01.md -Raw | Out-Null`
  - 结果: 通过
- `python build/toolchains/run/run.py test registry refresh --json`
  - 结果: `status = ok`

## wiki

- 本轮仍不写入 wiki
- 原因: 当前新增的是 roadmap 内部的阶段性验收口径与 child 执行顺序，尚未改变 repo 级 test pipeline / AOT onboarding authority
