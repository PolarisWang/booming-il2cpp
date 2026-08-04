---
parent_task_id: 20260530-optimization-campaign-reboot
source_task_id: T-A1
source_relation: roadmap-child
clearance_source: parent-roadmap
lifecycle_status: completed
phase: executing
blocking_questions: []
question_clearance: cleared
clearance_confirmed_by_user: true
entry_skill: dev-writing-plans
---

# T-A1: benchmark_scanner.py 实现

## Scope

实现 `verification/analysis/benchmark_scanner.py`，全量扫描所有 family 的 benchmark 数据，输出优化优先级队列。

## 功能要求

1. 扫描 `testing/foundation-dll/System.Private.CoreLib/*/multi-run/multi-run-report.json`，提取每个 family 的：
   - 方法数量
   - chaos-aot geometric_mean_ns
   - net8-jit geometric_mean_ns
   - chaos-aot vs net8-jit 加速比
   - 数据新鲜度（timestamp）
   - 各 runtime 的 method_count / ok_count / stub_count / throws_count
2. 支持 `--json` 和 `--table` 两种输出格式
3. 支持 `--sort-by` 参数（chaos-ns / vs-net8 / methods / freshness）
4. 标记数据缺失的 family（无 multi-run-report.json）
5. 输出优化优先级排序：按 `(chaos-aot ns/op) × (slowdown vs net8) × (method_count)` 加权

## 输入

- `testing/foundation-dll/System.Private.CoreLib/*/multi-run/multi-run-report.json`
- `testing/foundation-dll/System.Private.CoreLib/*/capability-family-contract.json`（可选，补充方法数）

## 预期输出

- `verification/analysis/benchmark_scanner.py` — CLI 工具
- CLI 支持: `python -m verification.analysis.benchmark_scanner [--json] [--sort-by chaos-ns] [--min-methods N]`

## 退出标准

- CLI 可运行并输出正确的排行榜
- 手动验证 3 个已知 family（enum-parsing, dispatch-basic, convert-char）的数据正确
- 输出格式包含 JSON 和 human-readable table

## 终端备注

benchmark_scanner.py 已创建并验证通过。文件路径: `testing/foundation-dll/verification/analysis/benchmark_scanner.py`。

CLI 支持:
- `--output json|table` 输出格式切换
- `--sort-by chaos-ns|vs-net8|methods|freshness` 排序字段
- `--min-methods N` 最小方法数过滤
- `--include-missing` 包含无数据 family
- `--top N` 仅显示前 N 个
- `--assembly NAME` 目标 assembly
- `--json` 等价于 `--output json`

验证结果:
- System.Private.CoreLib: 67 families 扫描, 27 with data, 40 missing, 295 total methods
- enum-parsing rank 1 (602.51ns chaos, 56.44ns net8, 10.67x slowdown, priority_score=83574)
- dispatch-basic 不进入 priority_queue (4138x speedup, 无优化必要)
- partial/missing families 正确标记
- 所有断言通过（enum-parsing rank 1, dispatch-basic 靠后, missing 标记）
