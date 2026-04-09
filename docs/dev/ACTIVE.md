---
task_id: 20260409-07-phase-5-capability-batch-a-dispatch-generic-layout-and-array-boxing
title: Phase 5 Capability Batch A Dispatch Generic Layout And Array Boxing
task_dir: docs/dev/in-progress/20260409-07-phase-5-capability-batch-a-dispatch-generic-layout-and-array-boxing
status_file: docs/dev/in-progress/20260409-07-phase-5-capability-batch-a-dispatch-generic-layout-and-array-boxing/STATUS.md
plan_file: docs/dev/in-progress/20260409-07-phase-5-capability-batch-a-dispatch-generic-layout-and-array-boxing/plan-v1-01.md
lifecycle_status: in_progress
phase: executing
total_tasks: 5
current_task: `Chunk 5`：统一 perf 入口已执行完成；当前评估 perf baseline regression 是否可作为阶段完成证据。
recent_summary: `perf.profile` 已在 `20260409-212758-windows-1a96` 跑通，run status 为 `ok`，但 baseline compare 给出 `regressed`：`meanDurationMs=52.327` 对比基线 `50.0`，`maxDurationMs=180.221` 对比基线 `80.0`；同一次样本去掉首个冷启动值后，均值约 `20.354ms`、最大值约 `29.14ms`。
next_step: 与用户确认是否接受“本阶段功能与 correctness 完成，但 perf baseline 在并发构建负载下呈回退”这一状态；若不接受，则需在空闲环境重跑 perf 或重新定义 perf 采样/基线规则。
---

当前活动任务：`20260409-07-phase-5-capability-batch-a-dispatch-generic-layout-and-array-boxing`
