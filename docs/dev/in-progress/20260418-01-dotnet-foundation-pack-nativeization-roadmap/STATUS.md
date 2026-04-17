---
task_id: 20260418-01-dotnet-foundation-pack-nativeization-roadmap
title: DotNet Foundation Pack Nativeization Roadmap
task_type: roadmap
lifecycle_status: in_progress
phase: roadmap
created_at: 2026-04-18 01:19:47 +08:00
updated_at: 2026-04-18 01:56:05 +08:00
latest_stop_point: phase-0 child task produced the first net8 catalog baseline and froze net10 as a repo-frozen snapshot policy, but the actual net10 snapshot is still missing
current_dir: docs/dev/in-progress/20260418-01-dotnet-foundation-pack-nativeization-roadmap
parent_task_id:
source_task_id: 20260416-01-aot-core-ir-gap-audit-and-next-expansion-roadmap
source_relation: architectural-redesign-from-entry-driven-aot-to-framework-pack-nativeization
active: false
---

## 2026-04-18 01:56 Stop Point

- 已确认新的正式目标：
  - foundation 起点不是仓库业务 DLL，而是官方 `Microsoft.NETCore.App`
  - 第一阶段只覆盖 `System.Private.CoreLib / mscorlib / netstandard / System.*`
  - `Microsoft.AspNetCore.App` / `Microsoft.WindowsDesktop.App` 不进入第一阶段
  - 完成标准是“单个 DLL 的全部公开 API / IL 语义都必须可 native 化”
- 已放弃继续沿用 `entry-driven` 主线作为 foundation 能力发现入口
- 已完成新的 brainstorm / design / roadmap 草案
- 当前默认假设：
  - `.NET 10` foundation packs 将以仓库内冻结快照形式管理
  - 如果用户后续改为外部预装提供，只影响 Phase 0 资产策略
- 已正式启动第一个 child task：
  - `20260418-02-phase-0-framework-pack-catalog-and-asset-policy`
  - 已产出：
    - `asset-source-policy-v1-01.md`
    - `framework-catalog-v1-01.json`
    - `pack-diff-summary-v1-01.json`
    - `phase-0-native-proof-and-benchmark-matrix-v1-01.md`
  - 当前阻塞：`.NET 10` repo-frozen snapshot 资产尚未入仓

## 关键文档

- brainstorm: `docs/dev/in-progress/20260418-01-dotnet-foundation-pack-nativeization-roadmap/brainstorm-v1-01.md`
- design: `docs/dev/in-progress/20260418-01-dotnet-foundation-pack-nativeization-roadmap/design-v1-01.md`
- roadmap_or_plan: `docs/dev/in-progress/20260418-01-dotnet-foundation-pack-nativeization-roadmap/roadmap-v1-01.md`
- source_roadmap: `docs/dev/abandoned/20260416-01-aot-core-ir-gap-audit-and-next-expansion-roadmap/roadmap-v1-01.md`

## 当前结论

- 当前仓库现有 native AOT 主线适合项目/入口级 proof，不适合作为 foundation DLL 认证主线
- 新主线必须改成：
  - `framework-pack-driven`
  - `single-dll certification-driven`
  - `consumer validation downstream`
- 需要新增 canonical owner `DotNetFoundationPack`
- 第一批 child task 应聚焦：
  - `.NET 8/.NET 10` asset policy
  - framework catalog
  - assembly classification
  - contract/facade/shim certification lane
- `20260418-02` 已从 `ready` 切到 `in_progress`
- 父 roadmap 当前不直接承担执行，活动入口已切到 child task
- `Phase 0` 当前已经具备 `.NET 8` catalog baseline，但还不能声称形成 `.NET 8/.NET 10` 双版本可重现输入层

## 最近摘要

- 2026-04-18 01:56:05 +08:00: `20260418-02` 已生成首批 Phase 0 产物，固定 `.NET 8` 为本机 `8.0.11` 基线，并把 `.NET 10` 固定为 repo-frozen snapshot 策略。
- 2026-04-18 01:50:00 +08:00: 已派生并激活 `20260418-02-phase-0-framework-pack-catalog-and-asset-policy`，开始冻结 Phase 0 source policy 与 catalog 基线。
- 2026-04-18 01:19:47 +08:00: 用户明确要求以 `.NET 8/.NET 10` `Microsoft.NETCore.App` foundation assemblies 为起点，且单 DLL 必须满足全部公开 API / IL 语义 native 化
- 2026-04-18 01:19:47 +08:00: 本机核验到仅存在 `.NET 8` / `.NET 9` packs，本地缺失 `.NET 10`
- 2026-04-18 01:19:47 +08:00: 已完成 foundation-pack-driven roadmap 文档，默认推荐 `.NET 10` 采用仓库内冻结快照

## 下一步

- next_action: 继续执行 `20260418-02-phase-0-framework-pack-catalog-and-asset-policy`，补齐 `.NET 10` repo-frozen snapshot 资产，或由用户改写 source model
- owner: codex
- trigger: 用户要求继续推进 foundation-pack nativeization 主线

## 风险 / 阻塞

### risks

- 若不先冻结 `.NET 10` 资产策略，phase 0 的 catalog 与后续 CI 可重复性会漂移
- 若不先建立 facade/shim/implementation 分类，后续单 DLL completion gate 会失真
- 若 corelib 继续按 proof 驱动补洞，phase 3 很容易退化为长尾 unsupported backlog

### blockers

- 当前实质 blocker：`.NET 10` 官方 packs 尚未进入执行环境或仓库冻结资产
- 说明：roadmap 已可成立，但 phase 0 的落地需要该资产来源最终固定

## 验证

- `dotnet --list-sdks`
  - 结果：本机只有 `8.0.402` 与 `9.0.101`
- `dotnet --list-runtimes`
  - 结果：本机有 `Microsoft.NETCore.App 8.0.x` 与 `9.0.0`，无 `.NET 10`
- `Get-ChildItem "$env:ProgramFiles\\dotnet\\packs\\Microsoft.NETCore.App.Ref"`
  - 结果：可见 `8.0.8`、`8.0.11`、`9.0.0`
- `Get-ChildItem "$env:ProgramFiles\\dotnet\\shared\\Microsoft.NETCore.App"`
  - 结果：可见 `8.0.8`、`8.0.11`、`9.0.0`

## wiki

- 本轮暂不写入 wiki
- 原因：当前仍处于 roadmap intake，等 phase 0 固定 `framework-pack` 资产与 owner 规则后再决定是否提升为长期 authority
