---
task_id: gc-p3-02
title: GC 基准回归测试套件（G-15）
task_type: plan
lifecycle_status: completed
phase: completed
created_at: 2026-05-22 19:40:00 +08:00
updated_at: 2026-05-22 20:00:00 +08:00
current_dir: docs/dev/completed/gc-p3-02-benchmark-regression
parent_task_id: 20260522-gc-industrialization
source_relation: roadmap-child
clearance_source: parent-roadmap
entry_skill: dev-writing-plans
---

# STATUS — gc-p3-02: GC 基准回归测试套件（G-15）

## 目标

为 CRAG GC 子系统建立基准回归测试套件，测量 GC 性能指标（吞吐量、暂停时间、分配延迟）并输出结构化 BENCH| 行，用于跨版本的回归检测。

## 实现

### 新增基准文件

- `tests/contracts/native/runtime-core/gc_benchmark.cpp` — 5 个基准测试

### 5 个基准测试

| # | 名称 | 测量内容 | 方法 |
|---|------|---------|------|
| 1 | AllocationThroughput | nursery 分配吞吐量 (KB/s) | 100K 次 64 字节分配，高精度时钟计时 |
| 2 | YoungGcPause | Young GC 暂停时间分布 | 填满 nursery 10 次，GcGetSnapshot 差值测量 |
| 3 | FullGcPause | Full GC 暂停时间分布 | 分配 old-gen 对象后触发 5 次 full GC，计算 avg/median |
| 4 | MixedAllocPattern | 混合大小分配吞吐量 | 50K 次 32/512 字节交错分配，模拟 survivor 模式 |
| 5 | Gen1Pause | Gen1 收集暂停分布 | 多轮 young GC 促进对象到 Gen1，GcGetSnapshot 测量 |

### 输出格式

```
BENCH|<name>|<metric>|<value>|<unit>
```

示例输出：
```
BENCH|AllocationThroughput|throughput|2874677|KB/s
BENCH|AllocationThroughput|avg_alloc_ns|28|ns
BENCH|YoungGcPause|count|10|samples
BENCH|YoungGcPause|avg_ns|15234|ns
BENCH|FullGcPause|avg_ns|1797920|ns
BENCH|MixedAllocPattern|throughput_kb_s|121691|KB/s
BENCH|Gen1Pause|count|5|samples
```

### 关键设计决策

1. **使用 GcGetSnapshot() 测量暂停时间**：初始实现使用 `chaos_gc_get_total_pause_duration()` 前后差值，但该值无法准确捕获单次 young GC 暂停。改为 `GcGetSnapshot()` 读取每代累积计数器（`young_pause_ns_total`、`full_pause_ns_total`、`gen1_pause_ns_total`）。
2. **批量测量代替单次循环**：通过批量分配触发多次 GC，对比快照差值计算平均值，避免单次 GC 暂停被其他线程活动淹没。
3. **移除 ForceYoungGc() helper**：初始版本创建了 ForceYoungGc() 辅助函数，切换到 GcGetSnapshot 方法后不再需要，已彻底移除。

### 文件改动

| 文件 | 改动 |
|------|------|
| tests/contracts/native/runtime-core/gc_benchmark.cpp | 新建 — 5 个基准测试 + BENCH| 输出 |
| tests/contracts/native/runtime-core/CMakeLists.txt | 新增 chaos_gc_benchmark 构建目标 |

## 验证

- 构建成功（cmake --build）
- 运行成功，5 个基准测试全部通过，所有 BENCH| 行正确输出
- GC_CHECK 断言全部通过（0 failures）
