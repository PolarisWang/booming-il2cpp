# STATUS — GC-D1 OOMLadder

## 元信息

```yaml
task_id: gc-align-coreclr/gc-d1-oom
task_type: plan
phase: in-progress   # 增量1+增量2 done；记忆压力 decommit 待续
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

OOM 逐级降级链对齐 CoreCLR `handle_oom`（allocation.cpp:2055）。**增量1**（半量预算 + 归因）done；**增量2**（provisional force-blocking）done；**记忆压力 decommit** 待续。

## 实现

### 增量1（gc_api.cpp HandleOomCondition）
- `s_recent_gc_mem_failure`（归因）+ `kOomReportHalfBudget=32KB`（半量预算 clamp）。

### 增量2（provisional force-blocking）
- `gc_scheduler.h/.cpp`：新增 `provisional_mode_` + public `InProvisionalMode()/SetProvisionalMode()`。
- `DecideCollection`：provisional 激活时 `prefer_bgc=false`，FULL/BGC/NONE 决策都强制走 blocking `FULL`（不 defer，快速回收，对齐 CoreCLR gcpriv.h:4324 provisional 降级）。
- `HandleOomCondition`：Step4（真 OOM）进入 provisional；Step2/3 成功退出。

## 对比 CoreCLR

| 机制 | CoreCLR | CRAG |
|------|---------|------|
| 半量预算 clamp | `allocation.cpp:2058-2061` | ✅ 增量1 |
| OOM 归因升级 | `:2063-2070` | ✅ 增量1 |
| provisional force-blocking | `gcpriv.h:4324` / `collect.cpp:1638+` | ✅ 增量2（force blocking + 禁 expand intent，简化为 prefer_bgc 关闭） |
| 记忆压力 decommit | `regions_segments.cpp:1386-1450` | ⬜ 待续（从"仅 OOM 兜底"提升为常态压力回收） |

## 验证

| 项 | 结果 |
|----|------|
| `chaos_runtime_core` Debug 编译 | ✅ |
| GC 单测批（scheduler 8/8、atomic 5/5、region 4/4、young_collector 5/5、gen1 11/11、loh 8/8、old_gen 6/6、poh 10/10） | ✅ 全部 0 |
| `gc_stress_test` Scenario B（OOM 阶梯） | ✅ emergency reserve + recovery |

## 已提交

- 增量1：commit（填）
- 增量2：commit（填）

## Exit Criteria（增量1+2 已满足）

- [x] OOM 四级降级 + 半量 size + 真/瞬态归因
- [x] provisional 激活 → GC 强制 blocking（不 defer BGC）
- [x] GC 单测 0 回归

## 后续增量（本次未做，记录）

- **记忆压力 decommit**：从"仅 OOM 兜底"提升为"高记忆常态回收"（regions_segments.cpp 页对齐、保首页）。
- **GCProvModeStress 压测旋钮**。
