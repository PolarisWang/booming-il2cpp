# STATUS — GC-G1 DependentConvergence

## 元信息

```yaml
task_id: gc-align-coreclr/gc-g1-dependent
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

Dependent/Ephemeron 处理去掉固定 3 轮定点（`kMaxFixedPointRounds`），改 do-while 运行期收敛，修深层 Ephemeron 链(>3 层) secondary 丢失。对齐 CoreCLR `objecthandle.cpp:1203-1265`。

## 实现（engine_lifecycle.cpp GcProcessDependentHandlesAfterFullGC）

- 删除 `constexpr int kMaxFixedPointRounds = 3`。
- `for (round...)` → `for (;;)` 无上限，`kept_this_round == 0` break 收敛。
- **收敛保证**：`G_OldGen().MarkObject`（`MarkRange` 返回 `newly_set`，:1042）与 `G_Loh().MarkObject`（`compare_exchange(expected=false,true)`）均幂等 —— 已标记返 false，故 `kept_this_round` 必到 0 → 必然终止，无死循环。

## 对比 CoreCLR

| CoreCLR | CRAG |
|---------|------|
| do-while 重扫至稳定（objecthandle.cpp:1222-1265 `m_fUnpromotedPrimaries && m_fPromoted`） | ✅ `for(;;)` + `kept==0` break，运行期收敛 |
| 无固定轮数 | ✅ 去 3 轮 cap |

## 验证

| 项 | 结果 |
|----|------|
| `chaos_runtime_core` Debug 编译 | ✅ |
| GC 单测：handle 11/11、finalizer 16/16、events 5/5、atomic 5/5、region 4/4、gen1 11/11 | ✅ 全部 0 |

## 已提交

- commit（填）

## Exit Criteria

- [x] 去固定 3 轮 → 运行期收敛（深链不丢 secondary）
- [x] `chaos_runtime_core` 编译 + GC 单测 0 回归
