# STATUS — GC-H1 TriggerReason

## 元信息

```yaml
task_id: gc-align-coreclr/gc-h1-events
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

GC 事件 + 触发原因追踪对齐 CoreCLR gcrecord.h（per-GC 触发原因）。本增量加 **GcTriggerReason 枚举 + 每全 GC 原因记录**，DecideCollection 设原因、stats 快照暴露。

## 实现

- **`gc_scheduler.h/.cpp`**：
  - 新增 `enum class GcTriggerReason`（NONE/ALLOC_PRESSURE/PAGE_GROWTH/EXPLICIT_REQUEST/HARD_LIMIT/EXTERNAL_PRESSURE/PROVISIONAL）。
  - private `mutable std::atomic<uint8_t> last_trigger_reason_`（enum class 不能直接 atomic，存 uint8_t）。
  - public `LastTriggerReason()` / `SetLastTriggerReason()`（const）。
  - `DecideCollection` FULL 返回点设置原因：显式请求→EXPLICIT，页增长→PAGE_GROWTH，分配→ALLOC/EXTERNAL，provisional 覆盖→PROVISIONAL。
- **`gc_stats.h/.cpp`**：`GcSnapshot` 加 `int32_t last_trigger_reason`，`GcGetSnapshot` 从 `G_Scheduler().LastTriggerReason()` 填充（include gc_heap.h）。

## 对比 CoreCLR

| CoreCLR | CRAG |
|---------|------|
| gcrecord.h 每 GC 原因位图 (condemn/compact/expand) | ✅ GcTriggerReason 枚举 + 每全 GC 原因（简化：原因枚举替代位图） |
| GCGlobalHeapHistory 事件 | ⬜ 后续可接入事件 |

## 验证

| 项 | 结果 |
|----|------|
| `chaos_runtime_core` Debug 编译 | ✅ |
| GC 单测：scheduler 8/8、stats 4/4、events 5/5、atomic 5/5、region 4/4、young_collector 5/5、gen1 11/11、handle 11/11、finalizer 16/16、loh 8/8、old_gen 6/6 | ✅ 全部 0 |

## 已提交

- commit（填）

## Exit Criteria

- [x] per-GC 触发原因记录 + stats 暴露
- [x] 无回归（GC 单测 0）
