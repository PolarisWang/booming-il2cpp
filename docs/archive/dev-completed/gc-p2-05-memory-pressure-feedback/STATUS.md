---
task_id: gc-p2-05
title: 内存压力主动调度反馈闭环（G-28）
task_type: plan
lifecycle_status: completed
phase: completed
created_at: 2026-05-22 10:30:00 +08:00
updated_at: 2026-05-22 10:45:00 +08:00
current_dir: docs/dev/completed/gc-p2-05-memory-pressure-feedback
parent_task_id: 20260522-gc-industrialization
source_relation: roadmap-child
clearance_source: parent-roadmap
blocking_questions: []
question_clearance: cleared
clearance_confirmed_by_user: true
---

# STATUS — gc-p2-05: 内存压力主动调度反馈闭环（G-28）

## 目标

完善外部内存压力反馈机制（AddMemoryPressure/RemoveMemoryPressure），使 GC 调度器能根据外部（非托管）内存压力主动触发 GC，形成闭环反馈。

## 设计方案

### 问题
原有实现：
1. 独立计数器 + 硬编码 256MB 阈值（与托管堆大小无关）
2. 一次性边沿触发（超过阈值后请求一次 Full GC，后不再评估）
3. 外部压力对调度器 `DecideCollection()` 完全不可见
4. `RemoveMemoryPressure` 不重新评估 GC 需求

### 方案
将外部内存压力跟踪迁移到 `GcScheduler`，使其参与完整的 GC 调度决策：

1. **自适应阈值**：`max(256MB, 0.5 × estimated_heap_size)`
2. **持续反馈**：在 `DecideCollection()` 中将超出阈值的压力计入 full-GC 分配计数器（封顶 2× heap）
3. **分级响应**：边沿触发 → RequestFullGc + 持续超额贡献 → FULL_BGC/FULL 分级
4. **安全降压**：`RemoveMemoryPressure` 饱和到 0

### 改动

| # | 文件 | 改动 |
|---|------|------|
| 1 | `gc_scheduler.h` | 添加 `external_memory_pressure_` 原子计数器 + 常量 |
| 2 | `gc_scheduler.cpp` | 实现 `AddExternalMemoryPressure`、`RemoveExternalMemoryPressure`；修改 `DecideCollection()` 折叠外部压力 |
| 3 | `gc_api.cpp` | 简化 `chaos_gc_add_memory_pressure` / `chaos_gc_remove_memory_pressure` 委派到 `g_gc_scheduler` |

## 测试验证

```
chaos_gc_finalizer_test: 16 tests, 0 failures (4 new memory pressure tests)
- TestMemoryPressureTriggersGc: pressure counter + threshold crossing
- TestMemoryPressureAddRemove: add then remove decreases pressure
- TestMemoryPressureNegative: negative values rejected (WARN log)
- TestMemoryPressureLargeValues: 1GB+2GB without overflow

All GC tests (63+ across all targets): PASS, 0 failures
```

## 结果

- **External memory pressure tracking**: unified in GcScheduler with atomic counter
- **Adaptive threshold**: `max(256MB, 0.5 * heap_est)` — scales with managed heap
- **Continuous feedback**: excess above threshold feeds into `alloc_full` in `DecideCollection()`
- **Graduated response**: existing BGC vs STW full logic applies naturally
- **Saturation safety**: RemoveMemoryPressure saturates at 0, negative input rejected
