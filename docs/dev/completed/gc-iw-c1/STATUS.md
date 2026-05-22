---
task_id: gc-iw-c1
title: LatencyMode 差异化行为实现（C11）
task_type: plan
lifecycle_status: completed
phase: completed
parent_task_id: 20260523-crag-gc-industrialization-sequel
source_relation: roadmap-child
clearance_source: parent-roadmap
created_at: 2026-05-23
updated_at: 2026-05-23
current_dir: docs/dev/completed/gc-iw-c1
---

# STATUS — gc-iw-c1：LatencyMode 差异化行为实现（C11）

## 问题

`GcLatencyMode` 枚举已存在但 `SetLatencyMode()`/`GetLatencyMode()` 仅做原子存储，`DecideCollection()` 只对 LOW_LATENCY/SUSTAINED_LOW_LATENCY 偏好 BGC 而非 STW FULL。BATCH 模式与 INTERACTIVE 无区别。

## 方案

为四种 LatencyMode 实现差异化调度行为：

| 模式 | Full GC 触发器 | Young GC 触发器 | Max Nursery | 偏好 BGC | 推迟 STW |
|------|---------------|----------------|-------------|---------|---------|
| BATCH | 2× (默认) | 3× (吞吐最优) | 4 MB | 否 | 否 |
| INTERACTIVE | 2× (默认) | 2× (默认) | 4 MB | 是 | 否 |
| LOW_LATENCY | 3× (推迟 STW) | 1× (更频繁) | 1 MB | 是 | 是 |
| SUSTAINED_LOW_LATENCY | 4× (极端推迟) | 1× (更频繁) | 1 MB | 是 | 是 |

### 新增 API

- `ShouldPreferBgc()` — BATCH 返回 false，其余 true
- `ShouldDeferStwFullGc()` — LOW_LATENCY/SUSTAINED_LOW_LATENCY 返回 true
- `GetEffectiveFullTriggerMultiplier()` — 模式感知的全 GC 触发器倍数
- `GetEffectiveYoungTriggerMultiplier()` — 模式感知的 Young GC 触发器倍数
- `GetMaxNurserySize()` — 模式感知的最大 Nursery 大小

### 改动文件

| 文件 | 改动 |
|------|------|
| `gc_scheduler.h` | 新增 5 个 LatencyMode 感知函数声明 + 5 个模式特定常量 |
| `gc_scheduler.cpp` | 实现 5 个函数 + DecideCollection/RecommendedNurserySize 使用模式感知倍数 |
| `gc_api.cpp` | chaos_gc_set_latency_mode 切换到 BATCH 时停 BGC |
| `gc_scheduler_test.cpp` | 新增 5 个 LatencyMode 行为测试 |

### 验证

| 场景 | 结果 |
|------|------|
| test_gc_scheduler（13 测试） | 13/13 PASS（含 5 个新增） |
| test_gc_bgc_unit（8 测试） | 8/8 PASS |
| test_gc_old_gen（6 测试） | 6/6 PASS |

## 完成证据

- arch_review: ok（现有调度器框架内新增模式感知辅助函数，无结构影响）
- authority_review: n/a
- verification:
  - unitTest: 27/27 (scheduler + bgc_unit + old_gen)
- test_result: passed
- wiki: n/a
- next: gc-iw-c2（GCMemoryInfo 托管侧结构体）
