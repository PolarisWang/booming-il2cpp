# STATUS — Benchmark 差异化分析 + .NET 8 基准对比

## Phase
`brainstorming` → 设计已确认，进入 `writing-plans`

## 设计摘要

### 目标
每 method 以 .NET 8 为基准，对比 net10-jit / chaos-aot / chaos-jit 快了多少%，支持跨日 diff。

### 新增组件
1. `verification/stages/benchmark_report.py` — 新建 pipeline stage，读取 benchmark-history.jsonl，逐 methodSubjectId 对齐 technology，计算 % vs net8
2. `verification/benchmark_diff.py` — 新建 CLI 工具，读取 history/comparison-*.json 做跨日 diff

### 修改组件
3. `verification/stages/aggregate.py` — 读取 comparison-summary.json 扩展 dashboard
4. `verification/chunk_pipeline.py` — 注册 benchmark_report stage

### 边界拍板
- % 公式: `(net8_ms - tech_ms) / net8_ms * 100`, 正数 = 快
- diff threshold: 10pp（chaosAotVsNet8Pct 变化 ≥10pp 标记 regressed/improved）
- diff 进 pipeline（benchmark_report 产出结果融入 dashboard），benchmark_diff 为独立 CLI
- chaos-jit 为必选项，缺失时对应字段为 null
- highVariance (CV > 20%) / net8Error 的 method 从 aggregate 中排除

## Authority 决策
- `benchmark_report.py` — 全权：JSONL 解析、technology 对齐、% 计算、aggregate 统计
- `aggregate.py` — 扩展：只读 comparison-summary，写入 dashboard
- `chunk_pipeline.py` — 注册：benchmark_report 在 managed_benchmark + benchmark 之后，aggregate 之前

## 风险评估摘要
- 低风险：纯报告层，不改变 codegen/dispatch/native 构建
- 中风险：chaos-jit 缺失时需优雅降级（null 字段），不阻塞 pipeline

## 三优先级权衡结论
- P1 性能：无影响
- P2 架构：提升（补齐报告层断裂）
- P3 热更：无影响

## blocking_questions
`[]`

## question_clearance
`cleared`

## clearance_confirmed_by_user
用户已确认：2026-06-04

## 修改清单

| 文件 | 改动 | 规模 |
|------|------|------|
| `verification/stages/benchmark_report.py` | **新建** — 跨 tech comparison + % vs NET8 | ~280 行 |
| `verification/stages/aggregate.py` | 扩展 — 读取 comparison-summary → dashboard | ~40 行 |
| `verification/chunk_pipeline.py` | 注册 benchmark_report stage | ~10 行 |
| `verification/benchmark_diff.py` | **新建** — CLI 跨日 diff 工具 | ~180 行 |

## 下一步入口
调用 `dev-writing-plans` 生成执行计划。
