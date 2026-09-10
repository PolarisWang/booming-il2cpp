# Phase 1 启动记录

- 时间：2026-04-14 13:02:35 +08:00
- 结论：Phase 0 已完成，当前切入 Phase 1 `subject verification closure`。
- 已知根因：`benchmark_dashboard_generator.py` 会把历史 `records.jsonl` 中已不属于当前 declared benchmark cases 的 case 继续写进 dashboard JSON，导致 `docs/benchmark/subjects/SolutionCorePack.json` 混入旧的 `PerformanceFeaturePack/*` 标识。
- 下一步：先补失败测试，再做最小修复和回归。
