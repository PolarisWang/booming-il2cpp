---
task_id: 20260419-21-complex-bcl-ordered-dll-01-system-collections-immutable
title: System.Collections.Immutable Ordered DLL Verification
task_type: plan
lifecycle_status: in-progress
phase: blocked-before-start
created_at: 2026-04-25 21:58:00 +08:00
updated_at: 2026-04-25 21:58:00 +08:00
current_dir: docs/dev/in-progress/20260419-21-complex-bcl-ordered-dll-01-system-collections-immutable
parent_task_id: 20260419-01-foundation-dll-translation-audit-roadmap
source_task_id: 20260419-03-system-private-corelib-full-verification
source_relation: roadmap-child-phase-2
clearance_source: parent-roadmap
blocking_questions: []
question_clearance: cleared
clearance_confirmed_by_user: true
preflight_review: warn
preflight_summary: 当前 phase-2 第一个 DLL 仍处于父 roadmap 已批准边界内，但前置依赖 `20260419-03-system-private-corelib-full-verification` 仍为 `completed-blocked`，因此本任务只能建立 DLL-first 审计 authority 和阻塞状态，不能提前进入 managed/native/hotupdate 真实执行。
auto_execution_decision: continue
design_doc: docs/dev/in-progress/20260419-01-foundation-dll-translation-audit-roadmap/design-v1-01.md
plan_doc: docs/dev/in-progress/20260419-01-foundation-dll-translation-audit-roadmap/roadmap-v1-01.md
active: false
---

# 20260419-21 System.Collections.Immutable Ordered DLL Verification

## 当前状态

- 当前任务是 phase-2 的第一个 DLL 审计入口，对应 `System.Collections.Immutable`。
- 当前状态为 `blocked-before-start`。
- 当前阻塞原因不是本 DLL 局部实现问题，而是上游 `System.Private.CoreLib` 仍未达到 `fullCoreLibTranslated = true`。

## DLL-First Audit Authority

- 上游 authority：
  - `docs/dev/in-progress/20260419-01-foundation-dll-translation-audit-roadmap/roadmap-v1-01.md`
  - `docs/dev/in-progress/20260419-01-foundation-dll-translation-audit-roadmap/design-v1-01.md`
  - `docs/dev/completed/20260419-03-system-private-corelib-full-verification/STATUS.md`
  - `docs/dev/in-progress/20260419-37-corelib-supplemental-runtime-skeleton-coverage-widening/STATUS.md`
- 预期 subject/workspace：
  - `subjects/SolutionCorePack/source/EngineeringScenarios/FoundationDllTranslationSolution/FoundationDllTranslationSolution.sln`
- 当前没有本 DLL 的真实 audit packet、proof run 或 benchmark artifact，因此任何 `passed` 状态都不成立。

## 预期 Verification Projects

- `audit-input-and-ledger`
  - expected_artifact: `audit/System.Collections.Immutable/input-manifest.json`
  - current_state: not-started
- `managed-proof`
  - expected_artifact: `audit/System.Collections.Immutable/managed-proof-report.json`
  - current_state: blocked-before-start
- `native-proof`
  - expected_artifact: `audit/System.Collections.Immutable/native-proof-report.json`
  - current_state: blocked-before-start
- `hotupdate-proof`
  - policy: `conditional`
  - current_state: undecided-until-corelib-unblocked
- `benchmark`
  - policy: `conditional`
  - current_state: no-perf-obligation-yet
- `codegen-review`
  - expected_artifact: `audit/System.Collections.Immutable/codegen-review.json`
  - current_state: not-started
- `completion-certification`
  - expected_artifact: `audit/System.Collections.Immutable/completion-report.json`
  - current_state: blocked

## 阻塞链

- immediate_blocker:
  - `docs/dev/completed/20260419-03-system-private-corelib-full-verification/STATUS.md`
- active_upstream_work:
  - `docs/dev/in-progress/20260419-37-corelib-supplemental-runtime-skeleton-coverage-widening/STATUS.md`
- current_program_views:
  - `verification/projections/foundation-dll-audit/dashboard.html`
  - `verification/archive/reports/completed/foundation-dll-audit/summary.md`

## 最新摘要

- 2026-04-25 21:58:00 +08:00：补齐 `System.Collections.Immutable` 的 DLL-first 审计状态页。当前只允许把它标记为 `blocked-before-start`，并把阻塞链显式绑定到 `20260419-03` 与活动 child `20260419-37`；在 CoreLib full verification 真正关闭前，不得伪造本 DLL 的 managed/native/hotupdate/completion 绿灯。

## 下一步

- next_action: 保持本 DLL 为 blocked authority，并继续推进 `20260419-37-corelib-supplemental-runtime-skeleton-coverage-widening`。只有当 `20260419-03` 的 `completion-certification` 真正关闭后，才允许为 `System.Collections.Immutable` 创建真实 audit packet 与 proof run。
- owner: codex
- trigger: after-corelib-unblocked

## 验证

- `python build/toolchains/run/run.py verify verification-v1 --json`
  - `finalStatus = ok`
  - `dashboardPath = verification/projections/foundation-dll-audit/dashboard.html`
  - `reportSummaryPath = verification/archive/reports/completed/foundation-dll-audit/summary.md`

## 风险 / 阻塞

### risks

- 如果在 CoreLib full verification 未关闭前就为 `System.Collections.Immutable` 写入伪 managed/native 结果，会污染 phase-2 顺序审计 authority。
- 如果继续只在 roadmap 表里保留一行 blocked，而没有单独 `STATUS.md`，最终审计者无法判断这个 DLL 的拆分项目、证据缺口和启动条件。

### blockers

- `20260419-03-system-private-corelib-full-verification` 仍为 `completed-blocked`
- `20260419-37-corelib-supplemental-runtime-skeleton-coverage-widening` 仍为 active unblocker
