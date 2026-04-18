---
task_id: 20260418-06-dependency-driven-core-bcl-layer-nativeization
title: Dependency-Driven Core BCL Layer Nativeization
task_type: plan
lifecycle_status: completed
phase: completed
created_at: 2026-04-18 11:33:19 +08:00
updated_at: 2026-04-18 13:06:12 +08:00
latest_stop_point: completed after rerunning dependency-layer tooling verification, SolutionCorePack compatibility verification, syncing docs/dev indexes, and confirming archived formal artifacts remain parseable
current_dir: docs/dev/completed/20260418-06-dependency-driven-core-bcl-layer-nativeization
parent_task_id: 20260418-01-dotnet-foundation-pack-nativeization-roadmap
source_task_id: 20260418-01-dotnet-foundation-pack-nativeization-roadmap
source_relation: roadmap-child-dependency-layer-nativeization
design_doc: docs/dev/completed/20260418-06-dependency-driven-core-bcl-layer-nativeization/design-v1-01.md
plan_doc: docs/dev/completed/20260418-06-dependency-driven-core-bcl-layer-nativeization/plan-v1-01.md
total_tasks: 6
current_task: completed
active: false
---

## 完成结论

- 基于 phase0 / phase1 / phase2 / phase3 authority 产出 `layer-plan-v1-01.json`、`proof-benchmark-lane-v1-01.json` 与 `assembly-nativeization-plan/`。
- 候选集已收敛为“具有真实 runtime method bodies 的 shared `System.*` DLL”，避免把 `System.Runtime` 这类 facade 误选为 nativeization 起点。
- 当前 ready layer 已冻结为 `core-bcl-layer-01`：
  - `net8.0`: `80` 个候选 DLL，`8` 层，首层 `29` 个 ready DLL
  - `net10.0`: `83` 个候选 DLL，`7` 层，首层 `29` 个 ready DLL
- 首层 ready DLL 已生成逐程序集 plan，并为每层绑定 `SolutionCorePack` proof / benchmark / conformance / engineering workload authority。

## 本轮收口

- 在 `Chaos.FoundationPack.Analysis` 中新增 `dependency-layer` 命令入口。
- 新增 `DependencyLayerArtifactsBuilder.cs`，复用现有 metadata 装载与分类逻辑，生成：
  - `layer-plan-v1-01.json`
  - `proof-benchmark-lane-v1-01.json`
  - `assembly-nativeization-plan/*.json`
- 为首层 ready DLL 建立 machine-readable 计划，当前覆盖 `System.Collections`、`System.Memory`、`System.Threading`、`System.Console` 等 `29` 个程序集。
- 把 `CoreLibReferenceSolution` 的官方 ref DLL 迁到 `src/dll/dotnet-foundation/`，并新增自动化测试 `tests/tooling/foundation/test_dependency_layer_plan.py` 与兼容性断言。

## 架构审视

- dependency-layer 继续复用 phase0 catalog 与 analyzer 既有 assembly metadata 扫描逻辑，没有再引入第二套 pack / metadata authority。
- nativeization 起点从“共享 public surface”收紧为“shared + 具有真实 runtime method bodies”，符合“先从有实现的底层 DLL 往上推进”的路线。
- proof / benchmark lane 仍统一挂在 `SolutionCorePack` registry authority 上；layer plan 只负责选择和组织，不把验证 contract 混入 pack catalog。
- 当前残留的设计风险是 registry snapshot 解析在 semantic-family 与 dependency-layer builder 中仍有重复实现；它不影响本轮 artifact 正确性，但如果继续扩展更多 layer builders，建议上提为共享 helper。
- 结论：本任务交付的职责边界清晰，可归档为 `completed`。

## 已验证

- `python -m pytest tests/tooling/foundation/test_dependency_layer_plan.py tests/tooling/foundation/test_phase1_contract_lane.py tests/tooling/foundation/test_phase2_corelib_substrate.py tests/tooling/foundation/test_phase3_corelib_family_plan.py tests/tooling/foundation/test_corelib_family_execution_entry.py -q`
  - 结果: `11 passed`
- `python -m pytest tests/unit/compatibility/test_solution_core_pack_subject.py -q`
  - 结果: `6 passed`
- `dotnet run --project src/tools/Chaos.FoundationPack.Analysis/Chaos.FoundationPack.Analysis.csproj -- dependency-layer --catalog docs/dev/completed/20260418-02-phase-0-framework-pack-catalog-and-asset-policy/framework-catalog-v1-01.json --phase1-dir docs/dev/completed/20260418-03-phase-1-contract-facade-and-shim-certification-lane --phase2-dir docs/dev/completed/20260418-04-phase-2-system-private-corelib-certification-substrate --phase3-dir docs/dev/completed/20260418-05-phase-3-system-private-corelib-semantic-family-nativeization --output-dir docs/dev/completed/20260418-06-dependency-driven-core-bcl-layer-nativeization --task-id 20260418-06-dependency-driven-core-bcl-layer-nativeization`
  - 结果: 成功生成正式 artifact，退出码 `0`
- `Get-Content docs/dev/completed/20260418-06-dependency-driven-core-bcl-layer-nativeization/layer-plan-v1-01.json -Raw | ConvertFrom-Json | Out-Null`
  - 结果: 通过
- `Get-Content docs/dev/completed/20260418-06-dependency-driven-core-bcl-layer-nativeization/proof-benchmark-lane-v1-01.json -Raw | ConvertFrom-Json | Out-Null`
  - 结果: 通过
- 说明:
  - 本轮验证期间未出现 `dotnet build` / `dotnet test` / `msbuild` 崩溃
  - formal verification object 已全部落盘并可解析

## 风险 / 余留

- `core-bcl-layer-01` 已 ready，但尚未进入真实 native proof / benchmark 执行；这会成为 roadmap 的下一条 child 主线。
- 更高层 layer 仍处于 blocked 状态，阻塞原因来自对更低 layer 的依赖，而不是 authority 缺失。
- `net8.0` 与 `net10.0` 的 ready layer 规模一致，但 candidate 总数与层数已有差异；后续需要继续跟踪跨版本 delta。

## 下一步

- 回到父 roadmap，围绕 `core-bcl-layer-01` 的 `29` 个 ready DLL 创建下一条执行 child，优先把 `System.Collections` / `System.Memory` / `System.Threading` 这一批真正推进到 native proof / benchmark 闭环。

## wiki

- 本轮不写入 wiki
- 原因: 当前沉淀的是 roadmap 内部阶段性 artifact 与 layer 切片结果，尚未上升为长期 authority
