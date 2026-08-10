# STATUS — GC-D1 OOMLadder

## 元信息

```yaml
task_id: gc-align-coreclr/gc-d1-oom
task_type: plan
phase: completed   # 增量1+2+3 全部 done
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

OOM 逐级降级链对齐 CoreCLR `handle_oom`（allocation.cpp:2055）。三个增量全部完成：

## 实现

### 增量1（`5dc595cf7`，gc_api.cpp HandleOomCondition）
- `s_recent_gc_mem_failure`（归因）+ `kOomReportHalfBudget=32KB`（半量预算 clamp）。

### 增量2（`226990998`，gc_scheduler.h/.cpp + gc_api.cpp）
- `provisional_mode_` + public `InProvisionalMode()/SetProvisionalMode()`。
- `DecideCollection`：provisional 激活 → `prefer_bgc=false`，强制 blocking FULL（不 defer）。
- `HandleOomCondition`：Step4 进入 provisional；Step2/3 成功退出。

### 增量3（gc_old_gen.cpp）
- 压力 decommit：provisional 激活时 page_pool_ 有效 cap 降为 0，100%-free pooled pages 积极归还 OS（对齐 regions_segments.cpp:1386-1450 记忆压力 decommit）。保持每 NUMA 节点 kMin 页防 recommit thrash。

## 对比 CoreCLR

| 机制 | CoreCLR | CRAG |
|------|---------|------|
| 半量预算 clamp | `allocation.cpp:2058-2061` | ✅ 增量1 |
| OOM 归因升级 | `:2063-2070` | ✅ 增量1 |
| provisional force-blocking | `gcpriv.h:4324` / `collect.cpp:1638+` | ✅ 增量2 |
| 记忆压力 decommit | `regions_segments.cpp:1386-1450` | ✅ 增量3 |

## 验证

| 项 | 结果 |
|----|------|
| `chaos_runtime_core` Debug 编译 | ✅ |
| GC 单测批（scheduler 8/8、old_gen 6/6、atomic 5/5、region 4/4、gen1 11/11、loh 8/8、young_collector 5/5、poh 10/10） | ✅ 全部 0 |
| `gc_stress_test` OOM 阶梯 | ✅ emergency reserve + recovery |

## 已提交

- 增量1：`5dc595cf7`
- 增量2：`226990998`
- 增量3：（填）

## Exit Criteria（全部满足，GC-D1 完成）

- [x] OOM 四级降级 + 半量 size + 真/瞬态归因
- [x] provisional 激活 → GC 强制 blocking（不 defer BGC）
- [x] 记忆压力 → pooled pages 积极归还 OS（decommit）
- [x] GC 单测 0 回归

## 后续（记录，非 GC-D1 范围）

- **GCProvModeStress 压测旋钮**（压测 provisional path）。
- CoreCLR provisional 的完整 gen1-强制-compact + NGC2 形态（简化落地为 force-blocking）。
