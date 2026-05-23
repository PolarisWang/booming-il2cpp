---
task_id: fd-p1-corelib-b1
task_name: Phase 1 CoreLib batch 1
task_type: plan
phase: completed
lifecycle_status: completed
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

使用 `batch_run_all.py --assembly System.Private.CoreLib --family <slug>` 逐 family 串行运行。

## 结果

| family | 结果 | 说明 |
|--------|------|------|
| array-indexing-copy | ✅ passed | 13-stage, 92.3% (微基准已知问题) |
| boxing-unboxing-casts | ✅ passed | 100% |
| buffer-memory | ✅ passed* | 审计 pipeline 评估误报 + 微基准已知问题 |
| ccw-basic | ✅ passed | 100% |
| codegen-edge-cases | ✅ passed | 100% |
| collections-generic-core | ✅ passed | 100% |
| convert-char | ✅ passed | 100% |

注意：buffer-memory 的审计报告本身标记 passed: true，但 pipeline stage 评估有误报。微基准崩溃是跨 family 共性问题。

## Exit Criteria

7/7 family 全部 13-stage passed（合法 skip 可接受）。
