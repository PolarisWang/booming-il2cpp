---
parent_task_id: 20260530-optimization-campaign-reboot
source_task_id: T-A2
lifecycle_status: active
phase: executing
blocking_questions: []
question_clearance: cleared
clearance_confirmed_by_user: true
entry_skill: dev-writing-plans
---

# T-A2: Batch Pipeline 执行

## Scope
对 40 个无 benchmark 数据的 family 跑 `--stages benchmark --native-config profile`，填补数据空白。

## 输入
- benchmark_scanner.py 输出的 missing families 列表
- `testing/foundation-dll/System.Private.CoreLib/*/capability-family-contract.json`

## 预期输出
- 尽可能多的 family 的 multi-run-report.json
- batch-pipeline-run.log（记录每个 family 的执行状态）

## 退出标准
- 至少 50% 的无数据 family 成功生成 benchmark 数据
- 失败的 family 有失败原因记录

## 终端备注
