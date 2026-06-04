---
parent_task_id: 20260530-optimization-campaign-reboot
source_task_id: T-D1
lifecycle_status: active
phase: executing
blocking_questions: []
question_clearance: cleared
clearance_confirmed_by_user: true
entry_skill: dev-writing-plans
---

# T-D1: 增量 Benchmark

## Scope
修改 CLI 入口支持增量 benchmark：只重跑有改动的 family，不做全量。

## 需要修改的文件
- `verification/entry_points/cli.py`

## 设计

在 CLI 中支持：
1. `--incremental` 模式：运行前检查目标 family 的 `multi-run/multi-run-report.json` 是否存在以及是否过期
2. 过期判断：比较 `codegen/` 目录下源文件的 mtime 与 multi-run-report.json 的 mtime
3. 如果 up-to-date → 跳过，输出 "SKIPPED (up-to-date)"
4. 如果 stale 或 missing → 正常运行

## 实现

1. 在 cli.py 的 main() 中添加 `--incremental` 参数
2. 实现 `_check_benchmark_freshness(family_dir) -> bool` 函数：
   - 检查 `multi-run/multi-run-report.json` mtime
   - 如果不存在 → return False (needs run)
   - 如果存在，对比 codegen 目录中最新文件的 mtime
   - 如果 codegen 更新 → return False (stale, needs run)
   - 否则 → return True (fresh)

3. 在 benchmark stage 入口处调用此函数：
   - 如果 `--incremental` 且 fresh → 跳过

## 验证
```bash
# 对一个已有数据的 family 运行（应该 skip）
python -m verification.entry_points.cli enum-parsing --assembly System.Private.CoreLib --mode strict --stages benchmark --incremental

# 查看输出是否包含 "SKIPPED"
```

## 预期输出
- `verification/entry_points/cli.py`（修改）

## STATUS.md 路径
`D:/agent/booming-il2cpp/docs/dev/in-progress/20260530-optimization-campaign-reboot/children/t-D1-incr-benchmark/STATUS.md`
