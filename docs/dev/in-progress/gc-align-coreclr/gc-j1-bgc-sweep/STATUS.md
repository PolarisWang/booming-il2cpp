# STATUS — GC-J1 BgcConcurrentSweep

## 元信息

```yaml
task_id: gc-align-coreclr/gc-j1-bgc-sweep
task_type: plan
phase: completed
parent_task_id: gc-align-coreclr
source_relation: roadmap-child
clearance_source: parent-roadmap
blocking_questions: []
question_clearance: cleared
clearance_confirmed_by_user: true
entry_skill: dev-executing-plans
created: 2026-08-10
created_by: main-agent
```

## Scope

BGC 并发 sweep 对齐 CoreCLR `background.cpp`（background_sweep 分相）。CRAG `BgcSweep` 现串行（曾并行后因 BGC hang 回退），基础设施已备（per-page `sweep_lock` + 线程安全 SweepPage/CoalescePage + GcWorkerPool）。重新安全并行化。

## 实现（gc_old_gen.cpp BgcSweep）

- 串行循环改为：`active` 页数(>8) 时 `GcWorkerPool::RunWorkers` 并行，用 `std::atomic<int>` 共享游标按**disjoint 页**分发；小 sweep 保持串行（免 worker-pool 启动开销）。
- **安全性**：每 worker 取互斥页（fetch_add 游标），`SweepPage` 内 `sweep_lock` 兜底并发；`clear_bitmap=false` 保留 mark bitmap 供后续 BgcCompact 的 DecideCompactMode。
- 每个 worker 独立 sweep，mutators 仍运行（并发语义）。

## 对比 CoreCLR

| CoreCLR | CRAG |
|---------|------|
| background_sweep 分 SOH/UOH 两段 + concurrent | ✅ 并行 sweep（GcWorkerPool） |
| per-page/segment 并发安全 | ✅ per-page sweep_lock |

## 验证

| 项 | 结果 |
|----|------|
| `chaos_runtime_core` Debug 编译 | ✅ |
| 确定性 GC 单测（old_gen 6/6、loh 8/8、atomic 5/5、region 4/4、gen1 11/11、scheduler 8/8） | ✅ 全部 0 |
| `chaos_gc_bgc_root_scan_test`（长跑 BGC 并发） | ✅ 完成 exit=0（concurrent_sweep_started/begin/complete，>90s 非 hang） |
| `chaos_gc_bgc_race_test`（长跑 BGC 并发） | ✅ 完成 exit=0 |

## 已提交

- commit（填）

## Exit Criteria

- [x] BgcSweep 并行化（>8 页走 GcWorkerPool），安全 disjoint 分发
- [x] 无 hang（BGC 测试完成）+ 确定性单测 0 回归

## 附注（非本任务，记录）

- `chaos_gc_bgc_race_test` young trace 的 `reclaimed`（如 2^64-1）疑似 pre-existing 统计/日志问题，源在 gc_young_collector（非本改动），另行跟踪。
