# CoreLib Supplemental Runtime Skeleton Coverage Widening 实现计划

> **面向执行 Agent：** 必须使用 `dev:executing-plans` 来执行本计划。步骤使用复选框（`- [ ]`）语法跟踪。完成时必须先归档当前 child 任务目录，再回写父 roadmap。

**目标：** 在真实 `System.Private.CoreLib.dll` supplemental full-closure evidence 已接入的前提下，继续扩大 runtime-skeleton translated coverage，优先消化可通过最小新 stub 形态支持的 CoreLib uncovered families。

**架构：** 保持现有 `assembly-full-closure-runtime-skeleton` 主链不变，在 `NativeReferenceProofEmitter` 的 runtime-skeleton stub builder 中新增最小可复用的支持分支，并通过现有 coverage report / canonical subject proof 验证真实 coverage 提升。

**技术栈：** `Chaos.IL2CPP.CodeGen`、runtime skeleton Scriban templates、Python subject worker/reporting、pytest、canonical `run.py test subject`

**架构审核运行模式：** critical

**结构告警重点：** 避免把 runtime-skeleton widening 写成大量特例复制；优先复用现有 string-forwarder / literal-return / page support primitive，只新增能在 CoreLib 中批量命中的最小新 helper。

**权责图审核运行主题：** authority 仍归 `NativeReferenceProofEmitter` 与其 templates；不得把 supplemental CoreLib widening 扩张成新的 subject authority 或新的 proof kind。

**AOT/IL2CPP/Test Governance Intake：**
- capabilityFamily: foundation-dll-translation-audit
- capabilityItem: corelib-supplemental-runtime-skeleton-widening
- ownerSubjectId: subject/SolutionCorePack
- proofRequired: true
- benchmarkRequired: false
- hotupdateImpact: Proof
- formalVerificationObjects: [`subject/SolutionCorePack::windows-corelib-reference-native-hotupdate-proof`]
- requiredGates: collector -> registry -> workspace

**设计文档：** `docs/dev/in-progress/20260419-01-foundation-dll-translation-audit-roadmap/design-v1-01.md`

**问题清零来源：** parent-roadmap

**计划来源：** roadmap-child-auto

**预期知识沉淀：** n/a

**收尾约束：** 执行完成后必须进入“结构告警与架构审视 -> 测试通过 -> 归档 completed -> 合并&提交”固定链路。

---

- [ ] 固化当前 child 文档载体
  - 文件：`docs/dev/in-progress/20260419-37-corelib-supplemental-runtime-skeleton-coverage-widening/STATUS.md`
  - 文件：`docs/dev/in-progress/20260419-37-corelib-supplemental-runtime-skeleton-coverage-widening/plan-v1-01.md`
  - 文件：父 roadmap / `ACTIVE.md` / 索引
  - 目标：确保新的 phase-1 blocker child 合法接管自动续跑

- [ ] 先写 RED 测试锁定首个 widening 形态
  - 文件：`tests/unit/compatibility/test_full_assembly_closure_codegen_contracts_string_templates_basic.py`
  - 文件：必要时 `tests/unit/compatibility/test_full_assembly_closure_codegen_contracts_runtime_templates.py`
  - 文件：必要时 `tests/unit/compatibility/test_full_assembly_closure_codegen_audit_plan.py`
  - 目标：先证明当前 runtime-skeleton emitter 尚不能覆盖新的 CoreLib supplemental method shape

- [ ] 实现 runtime-skeleton 最小扩面
  - 文件：`src/managed/Chaos.IL2CPP.CodeGen/NativeReferenceProofEmitter.cs`
  - 文件：必要时 `src/managed/Chaos.IL2CPP.CodeGen/ReferenceProof/NativeReferenceProofCatalog.cs`
  - 文件：必要时 `src/managed/Chaos.IL2CPP.CodeGen/Templates/*.scriban`
  - 目标：让选定的 CoreLib supplemental method family 进入 emitted 集合并复用现有 runtime-skeleton primitive

- [ ] 用受影响测试与 canonical proof 验证真实 coverage 改善
  - 测试：
    - `python -m pytest tests/unit/compatibility/test_full_assembly_closure_codegen_contracts_string_templates_basic.py -q`
    - `python -m pytest tests/unit/compatibility/test_full_assembly_closure_codegen_contracts_runtime_templates.py -q`
    - `python -m pytest tests/unit/compatibility/test_full_assembly_closure_codegen_audit_plan.py -q`
  - formal：
    - `python build/toolchains/run/run.py test subject --id subject/SolutionCorePack --matrix windows-corelib-reference-native-hotupdate-proof --json`
  - 目标：留下新的 canonical evidence，并确认 supplemental runtime-skeleton `uncoveredMethodCount` 下降
