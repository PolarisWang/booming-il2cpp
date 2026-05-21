# System.Private.CoreLib Semantic Family Nativeization 实施计划 v1.01

> 面向执行 Agent: 必须使用 `dev:executing-plans` 执行本计划。步骤使用复选框语法跟踪。  
> 目标: 基于 `System.Private.CoreLib Certification Substrate` 的输出，冻结 stable semantic families，建立 family-level native proof / conformance / benchmark contract，并把首批语义族接到真实执行入口。  
> 架构: 先冻结 family partition，再把首批语义族绑定到仓库现有 registry / subject / engineering workload authority，最后进入正式实跑。  
> 技术栈: Markdown、JSON、PowerShell、`Chaos.FoundationPack.Analysis`、registry-driven proof orchestration

**AOT/IL2CPP/Test Governance Intake**
- capabilityFamily: `FrameworkPackNativeization`
- capabilityItem: `SystemPrivateCoreLibSemanticFamilyNativeization`
- ownerSubjectId: `DotNetFoundationPack (planned)`
- proofRequired: `true`
- benchmarkRequired: `true`
- hotupdateImpact: `None`
- formalVerificationObjects:
  - `docs/dev/in-progress/20260418-05-phase-3-system-private-corelib-semantic-family-nativeization/semantic-family-plan-v1-01.json`
  - `docs/dev/in-progress/20260418-05-phase-3-system-private-corelib-semantic-family-nativeization/family-proof-matrix-v1-01.json`
  - `docs/dev/in-progress/20260418-05-phase-3-system-private-corelib-semantic-family-nativeization/family-priority-benchmark-v1-01.json`
  - `docs/dev/in-progress/20260418-05-phase-3-system-private-corelib-semantic-family-nativeization/semantic-family-execution-entry-v1-01.json`
- requiredGates: `n/a (先建立 execution entry，再进入首批语义族实跑)`

**设计文档**: `docs/dev/in-progress/20260418-05-phase-3-system-private-corelib-semantic-family-nativeization/design-v1-01.md`

**预期知识沉淀**: 视 execution entry 与首批实跑结果决定  
**收尾约束**: 执行完成后必须进入“审视架构合理性 -> 测试通过 -> 归档 completed -> 合并&提交”固定链路

---

Status: completed

## 计划清单

- [x] 步骤 1: 冻结 corelib semantic family partition 规则与优先级
- [x] 步骤 2: 先补失败测试，锁定 family plan / proof matrix / benchmark contract
- [x] 步骤 3: 生成 family planning artifacts，并选择首批 family batch
- [x] 步骤 4: 为首批语义族建立 native proof / conformance / benchmark 执行入口
- [x] 步骤 5: 同步 child / parent `STATUS.md`、`docs/dev/ACTIVE.md`、progress note 与索引
- [x] 步骤 6: 执行“审视架构合理性 -> 测试通过 -> 归档 completed -> 合并&提交”；首批 batch 已完成实跑并满足当前 child task 的 completed 条件

## 需要创建或修改的文件

- `docs/dev/in-progress/20260418-05-phase-3-system-private-corelib-semantic-family-nativeization/STATUS.md`
- `docs/dev/in-progress/20260418-05-phase-3-system-private-corelib-semantic-family-nativeization/design-v1-01.md`
- `docs/dev/in-progress/20260418-05-phase-3-system-private-corelib-semantic-family-nativeization/plan-v1-01.md`
- `docs/dev/in-progress/20260418-05-phase-3-system-private-corelib-semantic-family-nativeization/semantic-family-plan-v1-01.json`
- `docs/dev/in-progress/20260418-05-phase-3-system-private-corelib-semantic-family-nativeization/family-proof-matrix-v1-01.json`
- `docs/dev/in-progress/20260418-05-phase-3-system-private-corelib-semantic-family-nativeization/family-priority-benchmark-v1-01.json`
- `docs/dev/in-progress/20260418-05-phase-3-system-private-corelib-semantic-family-nativeization/semantic-family-execution-entry-v1-01.json`
- `docs/dev/in-progress/20260418-05-phase-3-system-private-corelib-semantic-family-nativeization/notes/progress-v1-03.md`
- `docs/dev/in-progress/20260418-01-dotnet-foundation-pack-nativeization-roadmap/STATUS.md`
- `docs/dev/ACTIVE.md`
- `docs/dev/INDEX.md`
- `docs/dev/in-progress/INDEX.md`
- `tests/tooling/foundation/test_corelib_family_execution_entry.py`
- `src/tools/Chaos.FoundationPack.Analysis/SemanticFamilyExecutionEntryBuilder.cs`
- `src/tools/Chaos.FoundationPack.Analysis/Program.cs`

## 预期验证命令

- `python -m pytest tests/tooling/foundation/test_corelib_family_execution_entry.py -q`
- `python -m pytest tests/tooling/foundation/test_phase1_contract_lane.py tests/tooling/foundation/test_phase2_corelib_substrate.py tests/tooling/foundation/test_phase3_corelib_family_plan.py tests/tooling/foundation/test_corelib_family_execution_entry.py -q`
- `dotnet run --project src/tools/Chaos.FoundationPack.Analysis/Chaos.FoundationPack.Analysis.csproj -- phase3 --phase2-dir docs/dev/completed/20260418-04-phase-2-system-private-corelib-certification-substrate --output-dir docs/dev/in-progress/20260418-05-phase-3-system-private-corelib-semantic-family-nativeization --task-id 20260418-05-phase-3-system-private-corelib-semantic-family-nativeization`
- `Get-Content docs/dev/in-progress/20260418-05-phase-3-system-private-corelib-semantic-family-nativeization/semantic-family-execution-entry-v1-01.json -Raw | ConvertFrom-Json | Out-Null`

## 状态维护要求

- `docs/dev/ACTIVE.md` 指向当前 child task
- 父 roadmap 保持 `in_progress` 且 `active = false`
- `System.Private.CoreLib Certification Substrate` 已归档 `completed`
- 如果验证阶段出现 `dotnet build` / `dotnet test` / `msbuild` 崩溃，必须立即进入根因排查，未修复前不得归档
