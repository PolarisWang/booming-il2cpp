# STATUS — GC-D1 OOMLadder

## 元信息

```yaml
task_id: gc-align-coreclr/gc-d1-oom
task_type: plan
phase: in-progress   # increment-1 done; provisional/decommit pending
roadmap_or_plan: docs/dev/in-progress/gc-align-coreclr/gc-d1-oom/design-d1-v1-01.md
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

OOM 逐级降级链对齐 CoreCLR `handle_oom`（allocation.cpp:2055）。本次实现 **增量 1**（对齐终态 OOM 路径）：**半量预算 clamp + OOM 归因升级**。provisional mode 与记忆压力 decommit 为后续增量。

## 实现（gc_api.cpp HandleOomCondition）

- 新增 `s_recent_gc_mem_failure`（进程级 atomic）：区分真内存耗尽 vs 瞬态/预算误报。
  - 置位：emergency reserve 也失败（Step 4）→ 真 OOM。
  - 复位：分配成功（Step 2 重试 / Step 3 reserve）→ 已恢复。
- 新增 `kOomReportHalfBudget = 32KB`（nursery min 64KB / 2，对齐 CoreCLR oom_budget = dd_min_size(gen0)/2）。
- Step 4：上报 OOM 事件前，size 钳为 half-budget（规整失败量级），日志标注 `<true-mem-exhaustion>`。
- 恢复：无显式 flag，每次 HandleOomCondition 结束复位 + 下次分配重走阶梯（对齐 CoreCLR :2083）。

## 对比 CoreCLR

| 机制 | CoreCLR | CRAG 增量1 |
|------|---------|-----------|
| 半量预算 clamp | `allocation.cpp:2058-2061` | ✅ `kOomReportHalfBudget` |
| OOM 归因升级 | `:2063-2070` fgm | ✅ `s_recent_gc_mem_failure` |
| 复位 | `:2083` | ✅ 成功恢复即复位 |
| provisional mode | `collect.cpp:1638+` | ⬜ 后续增量 |
| 记忆压力 decommit | `regions_segments.cpp` | ⬜ 后续增量 |

## 验证

| 项 | 结果 |
|----|------|
| `chaos_runtime_core` Debug 编译 | ✅ |
| GC 单测批（atomic 5/5、region 4/4、young_collector 5/5、gen1 11/11、loh 8/8、old_gen 6/6、scheduler 8/8、poh 10/10） | ✅ 全部 0 |
| `gc_stress_test` Scenario B（OOM 阶梯） | ✅ emergency reserve 分配 + "OOM recovery: succeeded after full GC"（归因成功复位） |

## 已提交

- commit（填）

## Exit Criteria（增量 1 已满足）

- [x] `chaos_runtime_core` Debug 编译通过
- [x] 内存耗尽走四级降级，OOM 上报半量 size + 真/瞬态 reason 可区分
- [x] emergency reserve 非耗尽场景无回归（GC 单测 0 失败）

## 后续增量（本次未做，记录）

- **provisional mode**：高记忆+高碎片 → 禁 expand + gen1 强制 compact + NGC2 而非全GC（CoreCLR collect.cpp:1638+，涉及调度器全局）。
- **记忆压力 decommit**：从"仅 OOM 兜底"提升为"高记忆常态回收"（regions_segments.cpp，页对齐、保首页）。
- **GCProvModeStress 压测旋钮**。
