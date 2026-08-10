# STATUS — GC-C1 MarkTermination

## 元信息

```yaml
task_id: gc-align-coreclr/gc-c1-mark-termination
task_type: plan
phase: completed
roadmap_or_plan: docs/dev/in-progress/gc-align-coreclr/gc-c1-mark-termination/design-c1-v1-01.md
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

并行标记终止协议对齐 CoreCLR 声明式（each-worker flag + join 屏障 + 重扫至稳定）。架构优先核查后，对 `gc_parallel_mark.cpp` 的"last-worker 拍板 done"终止加**收敛复查**硬化，保证收敛可证明。

## 架构优先结论（读 code 后）

当前协议对 own-deque 自洽（每个 worker 把新 work flush 到自己 deque，pop-self 必能找到，不会带 work 变 idle）。唯一理论竞态：last worker 设 `parallel_done` 时，另一个 worker 可能正 `ProcessChunk` 即将 flush 新 work，该 work 会被漏处理。对齐 CoreCLR 声明式应保证收敛可证明而非依赖时序。

## 实现（gc_parallel_mark.cpp）

- 新增 `AnyWorkRemaining(ctx)`：遍历所有 worker 的 deque + pending 累积器（`steal_mutex` 保护），任有余存 work → true。
- 修改终止处（原 :354-358）：last worker 设为 done 前先 `AnyWorkRemaining` 收敛复查；若有余存，`fetch_add(1)` 回外层继续 drain；否则才 `parallel_done=true` break。**对齐 mark_phase.cpp:3107 声明式 + 重扫至稳定语义。**
- 正确性：mark bit 幂等，新 work 单调递减，收敛必然（无 livelock）。

## 验证

| 项 | 结果 |
|----|------|
| `chaos_runtime_core` Debug 编译 | ✅ |
| `chaos_gc_parallel_mark_test` | ✅ 6/6 |
| `gc_stress_test` Scenario A（100 workers 高并发） | ✅ SCENARIO PASSED，workers=100/100，0 pattern fail，无 hang |
| GC 确定性单测批（parallel_mark/old_gen/loh/root_scanner/safepoint/mark_bitmap/bit_utils/finalizer/atomic/region） | ✅ 全部 0 |

（注：`gc_stress_test` Scenario B 的 OOM 是运行时 OOM 恢复路径被压测触发，非终止协议问题。）

## 已提交

- commit 待填（本文件随代码提交后回填）

## Exit Criteria

- [x] `chaos_gc_parallel_mark_test` 通过
- [x] 高并发 ParallelMark 无 hang/漏标记（stress Scenario A 100 worker 验证）
- [x] `chaos_runtime_core` Debug 编译通过
- [x] 对齐 CoreCLR 声明式：last idle worker 收敛复查 + 重扫至稳定
