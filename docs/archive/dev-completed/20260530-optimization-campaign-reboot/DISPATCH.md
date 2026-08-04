# DISPATCH.md — 调度总纲

dispatch_model: hybrid
parallel_merge_policy: independent-completed

## 批次说明

| batch_id | 模式 | 描述 | merge_after |
|----------|------|------|-------------|
| batch-1 | parallel | 数据基建 + 技能设计并行启动 | all-completed |
| batch-2 | parallel | 执行 Pipeline + Skill 细节修复 | all-completed |
| batch-3 | hybrid | 优先级生成 + 瓶颈分析 + 增量 Benchmark | continue-on-failure |
| batch-4 | parallel | 优化实施 + Regression + 依赖图 | all-completed |
| batch-5 | parallel | 回归验证 + 历史存储 + Auto-Bisect | all-completed |
| batch-6 | sequential | 全量自测验收 | all-completed |

## Batch-1: 并行启动

batch_id: batch-1
mode: parallel
description: "benchmark_scanner 实现 + 三阶段 Skill 架构设计"
wait_for: []

tasks:
  - id: T-A1
    conflict_check: no-shared-output
    wait_for: []

  - id: T-B1
    conflict_check: no-shared-output
    wait_for: []

## Batch-2: Skill 细节 + 数据采集

batch_id: batch-2
mode: parallel
description: "Batch Pipeline 执行 + Freeze 修复 + ROI 门禁 + 三明治模型"
wait_for: [batch-1]

tasks:
  - id: T-A2
    conflict_check: no-shared-output
    wait_for: []

  - id: T-B2
    conflict_check: no-shared-output
    wait_for: []

  - id: T-B3
    conflict_check: no-shared-output
    wait_for: []

  - id: T-B4
    conflict_check: no-shared-output
    wait_for: []

## Batch-3: 分析阶段

batch_id: batch-3
mode: hybrid
description: "Priority Queue + 瓶颈分析 + 增量 Benchmark"
wait_for: [batch-2]

tasks:
  - id: T-A3
    conflict_check: no-shared-output
    wait_for: []

  - id: T-C1
    conflict_check: read-only
    wait_for: []

  - id: T-D1
    conflict_check: no-shared-output
    wait_for: []

## Batch-4: 实施阶段

batch_id: batch-4
mode: parallel
description: "enum-parsing 优化 + Regression + 依赖图"
wait_for: [batch-3]

tasks:
  - id: T-C2
    conflict_check: shared-output (src/ codegen + native)
    wait_for: []

  - id: T-D2
    conflict_check: no-shared-output
    wait_for: []

  - id: T-D3
    conflict_check: no-shared-output
    wait_for: []

## Batch-5: 收尾阶段

batch_id: batch-5
mode: parallel
description: "回归验证 + 历史存储 + Auto-Bisect"
wait_for: [batch-4]

tasks:
  - id: T-C3
    conflict_check: no-shared-output
    wait_for: []

  - id: T-E1
    conflict_check: no-shared-output
    wait_for: []

  - id: T-E2
    conflict_check: no-shared-output
    wait_for: []

## Batch-6: 验收

batch_id: batch-6
mode: sequential
description: "全量自测验收"
wait_for: [batch-5]

tasks:
  - id: T-FINAL
    conflict_check: read-only
    wait_for: []
