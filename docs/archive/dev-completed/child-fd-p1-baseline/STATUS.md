---
task_id: fd-p1-baseline
task_name: Phase 1 — 基线扫描
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
preflight_summary: scope clear, exit criteria defined, dependencies satisfied (Phase 0 complete)
auto_execution_decision: continue
---

# Phase 1: 基线扫描

## 最近摘要

基线扫描完成。138/164 已通过（84.1%），10 失败，16 无报告（15 个是 reports/ 目录）。

## 当前步骤

步骤 2/2：汇总基线结果

## 基线扫描结果

| 分类 | 数量 | 说明 |
|------|:----:|------|
| 已通过（基线） | 138 | 基于现有 unified-verification-report.json |
| 预检失败（缺 capability contract） | 4 | --family/pinvoke-dllimport, threading-monitor-interlocked, surrogates-selection, windows-identity |
| Codegen 失败（KI-001） | 3 | tag-model, decoder, triple-composite family |
| Subjects 编译错误 | 1 | garbage-collection — GC.LatencyMode 不存在 |
| Patch entry 编译错误 | 1 | buffer-memory — _exitCode 重复定义 |
| Audit principle VIOLATION | 1 | document-element — p1_lowering: 0/138 有 lowering |
| 无报告 | 16 | 15 个 reports/ 目录（非真实 family），1 个 custom-generics（无 entry.exe） |

## 跟踪

- [x] 步骤 1：运行基线扫描
- [x] 步骤 2：汇总基线结果

## 下一步

Phase 1 完成，进入 Phase 1.5 风险排序

## 完成证据

- arch_review: ok
- authority_review: n/a
- verification:
  - canonicalCommand: serial_full_verify.py (baseline analysis)
  - formalObject: n/a
  - summaryPath: child-fd-p1-baseline/verification-tracker.json
  - eventsPath: n/a
  - consolePath: n/a
- verification_data:
  - refreshCommand: n/a
  - verificationOutputRoot: n/a
  - latestResultPath: n/a
  - masterResultPath: n/a
  - reportSummaryPath: n/a
  - codegenStubPath: n/a
- test_result: baseline established: 138/164 passed (84.1%), 10 failed, 16 no-report
- wiki: n/a
- next: phase 1.5 risk ranking
