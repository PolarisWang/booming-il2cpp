---
task_id: fd-p15-risk-ranking
task_name: Phase 1.5 — 风险排序
task_type: plan
phase: executing
lifecycle_status: completed
parent_task_id: foundation-dll-industrialization
source_task_id: foundation-dll-industrialization
source_relation: roadmap-child
clearance_source: parent-roadmap
blocking_questions: []
question_clearance: cleared
clearance_confirmed_by_user: true
entry_skill: dev-executing-plans
preflight_review: pass
preflight_summary: Phase 1 complete, baseline data available for risk scoring
auto_execution_decision: continue
---

# Phase 1.5: 风险排序

## 最近摘要

Phase 1.5 风险排序完成。12 个未通过 family 按 Risk_Score 排序为 P0(6) + P1(2) + P3(4)。识别 4 个 systemic issues（SYS-001~004）。

## 当前步骤

步骤 3/3：排序并填充 tracker

## 风险排序结果

| Tier | Score | Families | Key Issues |
|:----:|:-----:|:--------:|-----------|
| P0 | ≥ 7.5 | 6 | document-element (9.0), tag-model (7.5), buffer-memory (7.5), convert-char (7.5), custom-generics (7.5), garbage-collection (7.5) |
| P1 | ≥ 5.0 | 2 | decoder (5.0), triple-composite (5.0) |
| P2 | ≥ 3.0 | 0 | — |
| P3 | < 3.0 | 4 | 4 preflight failures (missing contracts) |

## 系统性阻塞

**SYS-001 (codegen chaos_* macros)** 阻塞全量 Phase 2 —— 必须先修复 codegen ↔ runtime headers 接口对齐。

## 跟踪

- [x] 步骤 1.5.1：计算风险评分
- [x] 步骤 1.5.2：识别系统性 bug
- [x] 步骤 1.5.3：排序并填充 tracker

## 输出

- FAMILY_ORDER.md — 按风险排序的执行顺序
- SYSTEMIC_ISSUES.md — 4 个系统性 bug 详细分析
- verification-tracker.json — 完整跟踪数据

## 下一步

解决 SYS-001（codegen 构建）后进入 Phase 2 逐 family 攻坚

## 完成证据

- arch_review: ok
- authority_review: n/a
- verification:
  - canonicalCommand: n/a (analysis task)
  - formalObject: n/a
  - summaryPath: FAMILY_ORDER.md, SYSTEMIC_ISSUES.md
  - eventsPath: n/a
  - consolePath: n/a
- verification_data:
  - refreshCommand: n/a
  - verificationOutputRoot: n/a
  - latestResultPath: n/a
  - masterResultPath: n/a
  - reportSummaryPath: n/a
  - codegenStubPath: n/a
- test_result: n/a (analysis)
- wiki: n/a
- next: resolve SYS-001 before Phase 2
