# STATUS — Foundation-DLL Profile Comparison (.NET 8 Baseline)

## 元信息

```yaml
phase: roadmap
task_id: foundation-dll-profile-comparison
task_type: roadmap
roadmap_or_plan: roadmap-v1-01.md
created: 2026-06-07
source: foundation-dll profile stage review → dev-brainstorm
blocking_questions: []
question_clearance: cleared
clearance_confirmed_by_user: true

# 子任务执行策略
child_execution_mode: auto
auto_continue: true
auto_stop_policy: blocking-only
dispatch_model: sequential

# 调度状态
active_batches: [batch-1]
completed_batches: []
active_children:
  - p01-runner-gcinfo: planned
pending_batches: [batch-2, batch-3, batch-4]

# 下一步
latest_stop_point: 2026-06-07 — roadmap 创建
recommended_next_child: p01-runner-gcinfo
下一步: 启动 p01-runner-gcinfo (.NET Runner gcInfo 采集)
```

## 架构映射

| 评估维度 | 当前状态 | 设计目标 |
|---------|---------|---------|
| AOT vs JIT 内存对比 | 无 | per-method GC pause + alloc + collection count vs .NET 8 |
| Managed 侧 profile 数据 | 无 | GC.GetAllocatedBytesForCurrentThread + GC.CollectionCount |
| 对比存储 | 无 | benchmark-history.jsonl 扩展 gcInfo 字段 |
| 对比分析 | 无 | benchmark_report + profile_compare.py |

## 边界拍板

- **方案**: C — 扩展 managed_benchmark runner 输出 gcInfo
- **对比目标**: net8-jit / net10-jit 的 GC 分配 + 频率 + 堆变化
- **存储**: 写入现有 benchmark-history.jsonl，不新建文件
- **分析**: benchmark_report 扩展 + 新增 profile_compare.py
- **非目标**: 不修改 AOT 侧 profile stage

## 阶段切分

```
Phase 1 — .NET Runner 改造: Chaos.TestFramework.Runtime 增加 gcInfo 采集
Phase 2 — managed_benchmark.py 透传: gcInfo → benchmark-history.jsonl
Phase 3 — benchmark_report 扩展: comparison.json 增加 gcComparison 节
Phase 4 — profile_compare.py: AOT profile + managed gcInfo 对齐对比
```

## 风险评估摘要

| 风险 | 缓解 |
|------|------|
| .NET 8/10 GC API 差异 | 降级仅采集 Gen0/Gen1 collection count |
| GC pause 无法直接获取 | 从 benchmark 耗时与净时间差值估算 (estimatedGcPauseMs) |
| profile 影响 benchmark | gcInfo 在 benchmark 循环外采集 |
| methodSubjectId 不对齐 | 共用 metadata 保证一致性 |

## 关键文档

- [权威输出] 本 STATUS.md 中的设计摘要
