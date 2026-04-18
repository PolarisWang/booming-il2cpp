---
task_id: 20260418-04-phase-2-system-private-corelib-certification-substrate
title: Phase 2 System.Private.CoreLib Certification Substrate
task_type: plan
lifecycle_status: completed
phase: completed
created_at: 2026-04-18 02:52:08 +08:00
updated_at: 2026-04-18 03:14:08 +08:00
latest_stop_point: Phase 2 completed after generating System.Private.CoreLib semantic substrate artifacts, passing targeted plus combined regression tests, and finishing architecture review plus formal verification
current_dir: docs/dev/completed/20260418-04-phase-2-system-private-corelib-certification-substrate
parent_task_id: 20260418-01-dotnet-foundation-pack-nativeization-roadmap
source_task_id: 20260418-01-dotnet-foundation-pack-nativeization-roadmap
source_relation: roadmap-child-phase-2
design_doc: docs/dev/completed/20260418-04-phase-2-system-private-corelib-certification-substrate/design-v1-01.md
plan_doc: docs/dev/completed/20260418-04-phase-2-system-private-corelib-certification-substrate/plan-v1-01.md
total_tasks: 6
current_task: completed
active: false
---

## 完成结论

- 已生成 `Phase 2` 的 5 个正式 substrate artifacts：
  - `assembly-semantic-ir-system-private-corelib-v1-01.json`
  - `runtime-helper-contracts-v1-01.json`
  - `corelib-surface-proof-report-v1-01.json`
  - `corelib-semantic-proof-report-v1-01.json`
  - `corelib-substrate-benchmark-v1-01.json`
- `assembly-semantic-ir-system-private-corelib-v1-01.json` 证明：
  - `net8.0`: `phase1CandidateAssemblyCount = 19`, `publicTypeCount = 2045`, `publicMemberCount = 26154`
  - `net10.0`: `phase1CandidateAssemblyCount = 21`, `publicTypeCount = 2276`, `publicMemberCount = 32489`
- `runtime-helper-contracts-v1-01.json` 证明：
  - `net8.0`: `classifiedPublicMemberCount = 26154`, `managed-cil = 21152`, `internal-call = 117`, `pinvoke = 4`, `runtime-special = 274`
  - `net10.0`: `classifiedPublicMemberCount = 32489`, `managed-cil = 27137`, `internal-call = 71`, `pinvoke = 4`, `runtime-special = 276`
- `corelib-surface-proof-report-v1-01.json` 证明：
  - `net8.0`: `unresolvedPublicTypeCount = 0`, `issueCount = 0`
  - `net10.0`: `unresolvedPublicTypeCount = 0`, `issueCount = 0`
- `corelib-semantic-proof-report-v1-01.json` 证明：
  - `net8.0`: `unclassifiedPublicMemberCount = 0`, `issueCount = 0`
  - `net10.0`: `unclassifiedPublicMemberCount = 0`, `issueCount = 0`

## 本轮收口

- 扩展 `Chaos.FoundationPack.Analysis`，新增 `phase2` 命令
- 用自动化测试锁定 `phase2` 的输出契约：
  - 需要生成 corelib semantic IR、helper contracts、surface proof、semantic proof、benchmark 五件套
  - public surface 与 semantic classification 都不能留下 unresolved / unclassified 项
- 将 `System.Private.CoreLib` public surface 冻结到 member-level semantic ledger
- 将公开成员按 `managed-cil` / `internal-call` / `pinvoke` / `runtime-special` / `abstract` / `field-metadata` / `composite` 分类
- 将 `Phase 1` candidate assembly inventory 作为 `Phase 2` 的上游 authority 输入写入 substrate artifacts

## 架构审视

- `Phase 2` 只负责 substrate，不负责 per-family native emit，仍然保持了 substrate 与 implementation 的职责边界
- semantic ledger 以公开成员为最小单位，已经摆脱 entry-driven proof 补洞模式；后续 `Phase 3` 可以直接在 machine-readable substrate 上切 family
- helper contract inventory 被固化为正式 JSON 对象，而不是临时控制台统计，后续 runtime service 设计有了稳定输入
- `Phase 1 -> Phase 2` 的衔接点被明确为 `phase1CandidateAssemblyCount` 与 contract lane authority，而不是错误地假设 facade/shim owner 会直接落到 corelib
- 结论：当前实现符合 `Phase 2` 设计目标，架构合理，可以归档 `completed`

## 已验证

- `python -m pytest tests/tooling/foundation/test_phase2_corelib_substrate.py -q`
  - 结果：`2 passed in 4.89s`
- `python -m pytest tests/tooling/foundation/test_phase1_contract_lane.py tests/tooling/foundation/test_phase2_corelib_substrate.py -q`
  - 结果：`5 passed in 12.79s`
- `dotnet run --project src/tools/Chaos.FoundationPack.Analysis/Chaos.FoundationPack.Analysis.csproj -- phase2 --catalog docs/dev/completed/20260418-02-phase-0-framework-pack-catalog-and-asset-policy/framework-catalog-v1-01.json --phase1-dir docs/dev/completed/20260418-03-phase-1-contract-facade-and-shim-certification-lane --output-dir docs/dev/in-progress/20260418-04-phase-2-system-private-corelib-certification-substrate --task-id 20260418-04-phase-2-system-private-corelib-certification-substrate`
  - 结果：成功生成 5 个正式 JSON 产物
- `Get-Content docs/dev/in-progress/20260418-04-phase-2-system-private-corelib-certification-substrate/assembly-semantic-ir-system-private-corelib-v1-01.json -Raw | ConvertFrom-Json | Out-Null`
  - 结果：通过
- `Get-Content docs/dev/in-progress/20260418-04-phase-2-system-private-corelib-certification-substrate/runtime-helper-contracts-v1-01.json -Raw | ConvertFrom-Json | Out-Null`
  - 结果：通过
- `Get-Content docs/dev/in-progress/20260418-04-phase-2-system-private-corelib-certification-substrate/corelib-surface-proof-report-v1-01.json -Raw | ConvertFrom-Json | Out-Null`
  - 结果：通过
- `Get-Content docs/dev/in-progress/20260418-04-phase-2-system-private-corelib-certification-substrate/corelib-semantic-proof-report-v1-01.json -Raw | ConvertFrom-Json | Out-Null`
  - 结果：通过
- `Get-Content docs/dev/in-progress/20260418-04-phase-2-system-private-corelib-certification-substrate/corelib-substrate-benchmark-v1-01.json -Raw | ConvertFrom-Json | Out-Null`
  - 结果：通过
- 说明：
  - 本阶段执行过程中没有出现 `dotnet build` / `dotnet test` / `msbuild` 编译崩溃
  - `dotnet run` 的编译与执行均成功返回 `0`

## 风险 / 余留

- `Phase 3` 仍需把当前 substrate 切成可执行的 semantic families，并对每个 family 建立 native proof / conformance / benchmark
- 当前 helper contract inventory 只冻结输入层，并未实现 runtime service 本体；这属于 `Phase 3+` 职责

## 下一步

- 激活 `20260418-05-phase-3-system-private-corelib-semantic-family-nativeization`
- 聚焦：
  - corelib semantic family 切分策略
  - family-level proof / conformance / benchmark contract
  - 首批 family batch child task 规划

## wiki

- 本阶段暂不直接写入 wiki
- 原因：corelib semantic family authority 还要在 `Phase 3` 继续冻结，当前先保留在任务归档中
