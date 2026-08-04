# Roadmap — Foundation-DLL Profile Comparison (.NET 8 Baseline)

## 目标

在 foundation-dll 验证管线中增加 AOT vs .NET 8/10 的 GC/内存行为对比能力，使 profile 数据不再孤立存在，能够通过跨技术栈对比定位优化目标。

## 范围

- 扩展 Chaos.TestFramework.Runtime runner 输出 gcInfo（分配量、GC 次数、堆变化）
- managed_benchmark.py 透传 gcInfo 到 benchmark-history.jsonl
- benchmark_report.py 扩展 comparison.json 增加 gcComparison 节
- 新增 analysis/profile_compare.py 做逐方法 profile 对比

## 非目标

- 不修改 AOT 侧 profile stage (stages/profile.py)
- 不新建 jsonl 文件
- 不修改 hotupdate stage

## 调度策略

```yaml
dispatch_model: sequential
child_execution_mode: auto
auto_continue: true
auto_stop_policy: blocking-only
```

## 阶段列表

### Phase 1 — .NET Runner 改造

| 字段 | 内容 |
|------|------|
| goal | Chaos.TestFramework.Runtime 的 benchmark 模式输出 gcInfo |
| exit_criteria | `dotnet exec runner.dll --assembly X --kind benchmark` 输出包含 gcInfo 字段 |
| deliverables | Runner.cs / BenchmarkRunner.cs 修改 |
| dependencies | 无 |
| resolved_decisions | 用 GC.GetAllocatedBytesForCurrentThread / GC.GetTotalMemory / GC.CollectionCount |
| watch_items | .NET 8 vs .NET 10 的 CollectionCount 行为差异 |

### Phase 2 — managed_benchmark.py 透传

| 字段 | 内容 |
|------|------|
| goal | gcInfo 从 runner 输出写入 benchmark-history.jsonl |
| exit_criteria | benchmark-history.jsonl 包含 gcInfo 字段的 net8-jit 记录 |
| deliverables | managed_benchmark.py 修改 |
| dependencies | Phase 1 |
| resolved_decisions | gcInfo 透传而不做聚合，保留原始格式 |
| watch_items | 向后兼容 — 旧 jsonl 没有 gcInfo 字段 |

### Phase 3 — benchmark_report 扩展

| 字段 | 内容 |
|------|------|
| goal | comparison.json 包含 GC 对比节 |
| exit_criteria | comparison-summary.json 的 aggregate 包含 gcAllocComparison |
| deliverables | benchmark_report.py 修改 |
| dependencies | Phase 2 |
| resolved_decisions | GC 对比节是 comparison.json 的新增可选节，不破坏现有 schema |
| watch_items | missing gcInfo 时回退为无对比 |

### Phase 4 — profile_compare.py

| 字段 | 内容 |
|------|------|
| goal | 按 methodSubjectId 对齐 AOT profile + managed gcInfo |
| exit_criteria | profile_compare.py 输出 per-method 对比报告 |
| deliverables | analysis/profile_compare.py |
| dependencies | Phase 2 + 已有的 AOT profile stage |
| resolved_decisions | AOT 侧用 profile.json 的逐方法数据，Managed 侧用 benchmark-history.jsonl |
| watch_items | 确保 methodSubjectId 在不同阶段一致 |

## 子任务映射

| task_id | phase | status | purpose | depends_on | requirements | deliverables | exit_criteria | conflict_scope | effort |
|---------|-------|--------|---------|------------|--------------|--------------|---------------|----------------|--------|
| p01-runner-gcinfo | 1 | planned | Runner 输出 gcInfo | 无 | 扩展 benchmark 输出格式 | Runner.cs 修改 | runner 输出包含 gcInfo | `src/reference/` | 1d |
| p02-managed-benchmark | 2 | planned | 透传 gcInfo 到 jsonl | p01 | managed_benchmark.py 改造 | managed_benchmark.py | jsonl 包含 gcInfo | `verification/stages/` | 0.5d |
| p03-benchmark-report | 3 | planned | comparison 增加 GC 节 | p02 | benchmark_report.py 扩展 | benchmark_report.py | comparison.json 含 gcComparison | `verification/stages/` | 1d |
| p04-profile-compare | 4 | planned | 逐方法 profile 对比 | p02 | 新增分析模块 | analysis/profile_compare.py | per-method 对比报告 | `verification/analysis/` | 1d |

## 依赖图

```
p01 → p02 → p03
         └→ p04
```

## 风险

| 风险 | 影响 | 缓解 |
|------|------|------|
| GC.GetAllocatedBytesForCurrentThread 在部分运行时不可用 | P1 | 运行时检测，不可用时输出 -1 |
| GC.CollectionCount 跨 .NET 版本语义差异 | P1 | 只比较同版本的 baseline |
| 旧 benchmark-history.jsonl 无 gcInfo | P3 | comparison.json 的 gcComparison 节可选 |

## 当前建议推进顺序

```
Phase 1: p01-runner-gcinfo → Phase 2: p02-managed-benchmark
Phase 3: p03-benchmark-report
Phase 4: p04-profile-compare
```

## 子任务执行策略

```yaml
child_execution_mode: auto
auto_continue: true
auto_stop_policy: blocking-only
recommended_next_child: p01-runner-gcinfo
```
