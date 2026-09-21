# S0.baseline — 基线固化与判据建立

> task_id: `S0-baseline`
> parent_task_id: `subjectid-decoupling`
> source_relation: roadmap-child
> clearance_source: parent-roadmap
> entry_skill: dev-executing-plans
> lifecycle_status: in-progress
> phase: implementing

blocking_questions: []
question_clearance: cleared
clearance_confirmed_by_user: true
preflight_review: pass
preflight_summary: 边界在父 roadmap 内；无 authority 变化；无新执行前问题；depends_on 为空；无新增 obligation
auto_execution_decision: continue

## Scope

**只读**：固化 clean main 的对比判据，锁定"当前的坏"与"不该回退的好"。
不改任何代码。

## Inputs

- `docs/dev/in-progress/subjectid-decoupling/roadmap-v1-01.md` §6 Phase 0
- 隔离 worktree `lock-true-ctor`（当前 @ ce61b1183，clean）

## Expected Outputs

- `docs/dev/in-progress/subjectid-decoupling/notes/phase0-baseline.md`

## Exit Criteria

1. threading chunk fact 基线（passed / Interlocked 族状态）
2. 归属指标（空壳类型数）
3. **判据含非零期望值 subject**（Increment 期望 1 / Decrement 期望 -1）

## Terminal Notes

（执行中填写）
