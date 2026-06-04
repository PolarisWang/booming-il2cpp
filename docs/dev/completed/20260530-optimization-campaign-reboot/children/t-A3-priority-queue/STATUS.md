---
parent_task_id: 20260530-optimization-campaign-reboot
source_task_id: T-A3
lifecycle_status: active
phase: executing
blocking_questions: []
question_clearance: cleared
clearance_confirmed_by_user: true
entry_skill: dev-writing-plans
---

# T-A3: Priority Queue 生成

## Scope
运行 benchmark_scanner.py 对所有 family（含新增的 28 个）输出完整的优化优先级队列。

## 注意
新增的 28 个 reflection family 只有 unified-verification-report.json 没有 multi-run-report.json。需要先更新 scanner 支持读取 unified-verification-report.json 作为 fallback。

## 步骤

1. **更新 scanner**：修改 `verification/analysis/benchmark_scanner.py`，增加对 `unified-verification-report.json` 的解析支持
   - 如果 `multi-run/multi-run-report.json` 不存在，尝试读取 `unified-verification-report.json`
   - 从 unified-verification-report.json 提取 benchmark/microbench 数据
   - 数据格式适配：从 `stages.benchmark.details.summaries` 获取 per-runtime summary
   - 标记 data_quality 为 "from_unified"

2. **生成 priority queue**：
```bash
cd D:/agent/booming-il2cpp/testing/foundation-dll
python -m verification.analysis.benchmark_scanner \
    --assembly System.Private.CoreLib \
    --sort-by vs-net8 \
    --include-missing \
    --output json \
    --top 30 \
    > optimization-priority.json
```

3. **验证**：
   - reflection-assembly 现在应该在 priority queue 中
   - enum-parsing 仍然是 rank 1

## 预期输出
- `verification/analysis/benchmark_scanner.py`（更新）
- `optimization-priority.json`

## STATUS.md 路径
`D:/agent/booming-il2cpp/docs/dev/in-progress/20260530-optimization-campaign-reboot/children/t-A3-priority-queue/STATUS.md`
