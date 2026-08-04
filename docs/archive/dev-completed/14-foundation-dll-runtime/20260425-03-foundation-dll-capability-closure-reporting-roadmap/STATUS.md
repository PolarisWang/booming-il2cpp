---
task_id: 20260425-03-foundation-dll-capability-closure-reporting-roadmap
title: Foundation DLL Capability Closure Reporting Roadmap
task_type: roadmap
lifecycle_status: completed
phase: completed
created_at: 2026-04-25 22:30:00 +08:00
updated_at: 2026-04-27 02:20:00 +08:00
current_dir: docs/dev/completed/20260425-03-foundation-dll-capability-closure-reporting-roadmap
parent_task_id: 20260419-01-foundation-dll-translation-audit-roadmap
source_task_id: 20260419-01-foundation-dll-translation-audit-roadmap
source_relation: parallel-reporting-governance-roadmap-under-foundation-audit
roadmap_or_plan: docs/dev/completed/20260425-03-foundation-dll-capability-closure-reporting-roadmap/roadmap-v1-01.md
child_execution_mode: manual
auto_continue: false
auto_stop_policy: manual-review
recommended_next_child:
active: false
blocking_questions: []
question_clearance: cleared
clearance_confirmed_by_user: true
---

# 20260425-03 Foundation DLL Capability Closure Reporting Roadmap

## 当前状态

- 状态: `in-progress`
- 当前阶段: `phase-4`
- 当前目标: 完成 Phase 4 — 同步 authority 文档和 formal refresh 计划
- Phase 0-3 已完成，设计文档均已通过用户 review 确认

## 当前结论

- 真实 DLL 进度不能再由 `audit-input-and-ledger / managed-proof / native-proof / hotupdate-proof / benchmark / codegen-review / completion-certification` 这 7 个 project 的通过数直接代表
- 真实进度条改为 `Capability Closure`：分母是每个 DLL 的 `required capability families`
- `Workflow Progress` 保留，但它是辅助流程条，表示 capability family 对应的 required gates 推进到哪一步，而不是能力覆盖本身
- `completion-certification` 改为 DLL 级 gate，不再混入 capability family 的 workflow 分母
- `docs/**` 下的 HTML 报告继续作为审核入口；`summary.md` 退出 docs 发布面，只保留 formal archive 需要的 Markdown 摘要

## 已拍板设计

- denominator_authority:
  - 原始 authority: `audit input manifest + surface ledger + semantic ledger + nativeization plan`
  - 报告 authority: `capability-family ledger`
- capability_granularity:
  - 主粒度: 语义 `capability family`
  - 实现映射: `implementationFamilies[]`
  - 源码分组: `sourceGroups[]`
- workflow_denominator:
  - family 级 gate: `audit-input-and-ledger / managed-proof / native-proof / hotupdate-proof / benchmark / codegen-review`
  - DLL 级 gate: `completion-certification`
- completion_rule:
  - `required capability families` 全绿
  - 对应 `required workflow gates` 全绿
  - 所有未闭包 family 必须处于 `waived / excluded / platform-blocked` 且带正式 authority
- source_links:
  - `Subject Source`
  - `Verification Source`
  - `Generated Code`
  - `Evidence`
  - `Authority Docs` 可选

## AOT / IL2CPP / Reporting Governance Intake

- capabilityFamily: `foundation-dll-reporting`
- capabilityItem: `capability-closure-and-workflow-progress-governance`
- ownerSubjectId: `SolutionCorePack`
- proofRequired: `false`
- benchmarkRequired: `false`
- hotupdateImpact: `None`
- formalVerificationObjects:
  - `docs/verification/foundation-dll-audit/*`
  - `verification/projections/foundation-dll-audit/*`
  - `verification/catalog/programs/foundation-dll-translation-audit.program.json`
  - `wiki/03-功能模块/06-il2cpp核心架构/docs/archive/architecture/verification-v1/spec.md`
  - `wiki/06-测试验证/INDEX.md`
- requiredGates:
  - `projection-contract`
  - `reporting-contract`
  - `formal-refresh`

## 下一步

- next_action: 起草 Phase 4 设计文档 `reporting-authority-sync-v1-01.md`，包含 spec.md 更新方案、INDEX.md 同步方案、formal refresh 计划和三层产物一致性校验规则
- owner: codex
- trigger: user-approved-phase-4
- phase-4-deliverables:
  - `reporting-authority-sync-v1-01.md` 已起草（draft 状态）
  - 待用户 review 确认后，实际落地编辑 spec.md 和 INDEX.md

## 风险 / 阻塞

### risks

- 如果直接继续沿用当前 project-template 计数作为主进度条，会持续输出误导性的“已通过 / 总数”报告
- 如果 `capability-family ledger` 的 authority 没有先冻结，后续 dashboard / DLL detail / artifact index 的字段会再次漂移
- 如果 waiver authority 继续散落在 task `STATUS.md`，DLL completed 判定就无法成为长期 canonical 规则

### blockers

- none
