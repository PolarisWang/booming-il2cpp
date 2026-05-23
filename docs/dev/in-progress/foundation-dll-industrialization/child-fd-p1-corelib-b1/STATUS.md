---
task_id: fd-p1-corelib-b1
task_name: Phase 1 CoreLib batch 1
task_type: plan
phase: execution
lifecycle_status: in-progress
parent_task_id: foundation-dll-industrialization
source_task_id: foundation-dll-industrialization
source_relation: roadmap-child
clearance_source: parent-roadmap
blocking_questions: []
question_clearance: cleared
clearance_confirmed_by_user: true
entry_skill: dev-executing-plans
---

# Phase 1: CoreLib Batch 1

## Scope

对 System.Private.CoreLib 的前 7 个 family 跑通完整 13-stage：

1. array-indexing-copy
2. boxing-unboxing-casts
3. buffer-memory
4. ccw-basic
5. codegen-edge-cases
6. collections-generic-core
7. convert-char

## 执行策略

使用 `batch_run_all.py --assembly System.Private.CoreLib --family <slug>` 逐 family 串行运行。如果某 family 失败，修复后继续下一 family。

## Exit Criteria

7/7 family 全部 13-stage passed。
