# STATUS — Foundation-DLL Profile Pipeline

## 元信息

```yaml
phase: roadmap
task_id: foundation-dll-profile-pipeline
task_type: roadmap
roadmap_or_plan: roadmap-v1-01.md
created: 2026-06-07
source: foundation-dll 验证管线综合评估报告 → dev-brainstorm
blocking_questions: []
question_clearance: cleared
clearance_confirmed_by_user: true

# 子任务执行策略
child_execution_mode: auto
auto_continue: true
auto_stop_policy: blocking-only
dispatch_model: sequential

# 调度状态
active_batches: [batch-1, batch-2, batch-3, batch-4]
completed_batches: [batch-1, batch-2, batch-3, batch-4]
active_children: []
all_children_completed: true
pending_batches: []

# 下一步
latest_stop_point: 2026-06-07 — 全部子任务代码实现完成 + 编译验证通过
recommended_next_child: null (全部完成)
下一步: 完整管线验证 + Wiki 更新
```

## 架构映射

| 评估维度 | 当前状态 | 设计目标 |
|---------|---------|---------|
| 内存 Profile | 只有 allocatedBytes | GC pause + allocation + heap delta + fast path rate |
| 分析模块 | analysis/ 仅 __init__.py | perf_baseline / perf_compare / gc_check / regression_report |
| 优化闭环 | 有性能数据，无根因数据 | Profile + Benchmark 交叉分析 → 优化报告 |
| 代码体积 | 无追踪 | Per-function code size via map file |
| 多家族覆盖 | 仅 S.P.C 完整 | batch_manifest.py 批量生成 |

## 风险评估摘要

| 风险 | 缓解 |
|------|------|
| --profile 路由复杂度 | 复用 RunMode 枚举 |
| Per-function code size 依赖 .map | 抽象 PE parser 层，fallback section 级 |
| Profile stage 管线时间 +30-60s | 默认不启用，--stages profile 时触发 |
| 多家族 DLL 路径不确定 | DOTNET_ROOT → local --assembly-dir fallback |

## 三优先级权衡结论

无优先级冲突。Profile stage 默认不启用，不影响现有管线性能；独立 stage 架构整洁；HotUpdate 路径可通过同一框架复用。

## 边界拍板

- **整体策略**: 新增独立 Profile Stage (方案 B)
- **入口模式**: entry.exe --profile flag (复用 --benchmark-all 框架)
- **采集粒度**: GC pause + allocation volume + heap delta + fast path rate + per-function code size
- **分析模块**: B1-B4 全部实现
- **多家族**: E2 批量扫描脚本
- **存储**: JSONL 延续 benchmark-history.jsonl 模式
- **默认不启用**: 只在 --stages profile 或 --profile flag 触发

## 阶段切分

```
Phase 1 — C++ 侧采集 + Profile Stage 编排 + gc_check.py
Phase 2 — perf_baseline.py + perf_compare.py
Phase 3 — Per-function code size + regression_report.py
Phase 4 — batch_manifest.py + 多家族验证
```

## 完成条件 (Roadmap Exit Criteria)

全部 Phase 1-4 完成后，必须执行以下操作才能关闭 roadmap：

1. ✅ **完整 Stage 验证**: 对 System.Private.CoreLib 全量 chunk 运行：
   ```
   python -m verification --assembly System.Private.CoreLib --all-chunks --stages build,fact,profile,benchmark,benchmark_report,aggregate
   ```
   - build: 全部通过或已知失败记录
   - fact: passed/total ≥ 95%
   - profile: 输出完整的 profile.json (GC pause + alloc + code size)
   - benchmark: 与完成前基线对比，标记回归
2. ✅ **Wiki 更新**: 将 profile stage 的使用方法和设计文档写入 `wiki/06-测试验证/`
3. ✅ **报告归档**: 最终 profile 数据 + benchmark comparison 快照写入 `_dll/reports/history/`
4. ⛔ 以上全部完成后才能将 roadmap 标记为 completed

## 关键文档

- [Roadmap] `roadmap-v1-01.md` — 阶段切分与子任务表
- [设计输入] 本 STATUS.md 中的设计摘要
