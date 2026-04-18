---
task_id: 20260418-01-dotnet-foundation-pack-nativeization-roadmap
title: DotNet Foundation Pack Nativeization Roadmap
task_type: roadmap
lifecycle_status: in_progress
phase: roadmap
created_at: 2026-04-18 01:19:47 +08:00
updated_at: 2026-04-18 11:33:19 +08:00
latest_stop_point: semantic-family child task 20260418-05 has been completed and archived; dependency-layer child task 20260418-06 is now created and active
current_dir: docs/dev/in-progress/20260418-01-dotnet-foundation-pack-nativeization-roadmap
parent_task_id:
source_task_id: 20260416-01-aot-core-ir-gap-audit-and-next-expansion-roadmap
source_relation: architectural-redesign-from-entry-driven-aot-to-framework-pack-nativeization
active: false
---

## 2026-04-18 11:21 Stop Point

- 子任务 [20260418-05-phase-3-system-private-corelib-semantic-family-nativeization](/D:/agent/booming-il2cpp/docs/dev/completed/20260418-05-phase-3-system-private-corelib-semantic-family-nativeization/STATUS.md) 已完成并归档。
- 子任务 [20260418-06-dependency-driven-core-bcl-layer-nativeization](/D:/agent/booming-il2cpp/docs/dev/in-progress/20260418-06-dependency-driven-core-bcl-layer-nativeization/STATUS.md) 已创建并激活。
- `System.Private.CoreLib` 首批 semantic-family batch 已完成 `managed-cil`、`internal-call`、`field-metadata` 的真实 proof / conformance / engineering workload / benchmark 实跑。
- 首批 batch 共执行 `27` 条 canonical commands，全部 `finalStatus = ok`，且未出现 `dotnet build` / `dotnet test` / `msbuild` 崩溃。

## 当前结论

- roadmap 已完成：
  - framework pack catalog / asset policy
  - facade/shim contract lane
  - `System.Private.CoreLib` certification substrate
  - `System.Private.CoreLib` 首批 semantic-family execution entry 与闭环实跑
- roadmap 仍未完成；下一阶段工作已从“首批 family batch 是否能跑通”切换为“如何把已稳定的 corelib 能力向 dependency-driven 核心 `System.*` DLL 分层扩展”。

## 最近摘要

- 2026-04-18 11:21:56 +08:00: `20260418-05` 已修复 subject-entry host 选择回归，并完成首批 27 条 canonical commands 的全量实跑。
- 2026-04-18 08:28:49 +08:00: `20260418-05` 生成 `semantic-family-execution-entry-v1-01.json`，把首批 family 绑定到 `SolutionCorePack` authority。
- 2026-04-18 03:14:08 +08:00: `20260418-04` 已归档完成，提供 corelib semantic substrate 与 helper contract 基底。
- 2026-04-18 02:52:08 +08:00: `20260418-03` 已归档完成，稳定输出 facade/shim classification、surface proof、native binding proof 与 benchmark artifacts。

## 下一步

- 继续执行 [20260418-06-dependency-driven-core-bcl-layer-nativeization](/D:/agent/booming-il2cpp/docs/dev/in-progress/20260418-06-dependency-driven-core-bcl-layer-nativeization/STATUS.md)，先冻结 layer plan 输入 authority 与首层 ready / blocked 分类。
- 后续新增子任务继续使用语义化命名，不再扩散 `Phase{x}` 风格。

## 风险 / 阻塞

### risks

- `System.Private.CoreLib` 首批 semantic-family batch 的闭环结果已经证明 execution entry 和 authority 接线可用，但还不能替代更高层 `System.*` 依赖族的完整认证。
- 一旦 dependency-layer 阶段暴露新的 metadata / helper / interop 缺口，仍可能回补到 substrate 或 semantic-family coverage mapping。
- 复杂 BCL、外部 DLL、hotupdate consumer 与 `.NET 10` delta closure 还未开始，它们的 scope 明显大于当前已归档 child。

### blockers

- 当前 blocker: `none`
- 说明: 当前没有工具链阻塞；剩余工作是继续推进 roadmap 的下一条 child task。

## 验证

- [20260418-05 STATUS](/D:/agent/booming-il2cpp/docs/dev/completed/20260418-05-phase-3-system-private-corelib-semantic-family-nativeization/STATUS.md)
  - 结果: 已记录首批 batch 的单测、真实命令、架构审视与 formal verification 证据
- [summary.json](/D:/agent/booming-il2cpp/artifacts/subjects/SolutionCorePack/runs/20260418-105927-windows-0ea8/run-report/summary.json)
  - 结果: `managed-runtime-output` 回归修复后的真实验证 run
- `20260418-110105-windows-c5c3` 到 `20260418-111909-windows-aea9`
  - 结果: 首批 `27` 条 canonical commands 全部通过

## wiki

- 本轮仍不写入 wiki
- 原因: 需要等 dependency-layer 阶段明确长期 authority 边界后，再决定哪些 execution-entry / layered-nativeization 经验值得固化
