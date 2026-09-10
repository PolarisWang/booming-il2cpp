---
task_id: gc-p3-02
title: GC 基准回归测试套件（G-15）
task_type: plan
lifecycle_status: completed
phase: completed
created_at: 2026-05-22 19:45:00 +08:00
updated_at: 2026-05-22 21:30:00 +08:00
current_dir: docs/dev/in-progress/gc-p3-02-benchmark
parent_task_id: 20260522-gc-industrialization
source_task_id: gc-p3-02
source_relation: roadmap-child
clearance_source: parent-roadmap
blocking_questions: []
question_clearance: cleared
clearance_confirmed_by_user: true
roadmap_or_plan: plan-v1-01.md
entry_skill: dev-writing-plans
---

# gc-p3-02 — GC 基准回归测试套件

## 设计摘要

继承父 roadmap `docs/dev/in-progress/20260522-gc-industrialization/roadmap-v1-01.md`：

- **purpose**: GC 基准回归测试套件（G-15）
- **conflict_scope**: 新基准测试文件
- **estimated_effort**: 2 周
- **depends_on**: phase-1（已完成）
- **Phase 2 exit_criteria**: 基准套件可运行并产生可对比报告

### 边界拍板（继承父级）

1. 性能最优（第一优先级）— 基准测试本身必须低开销，不干扰被测指标
2. 范围：`src/native/runtime-core/gc/` + 测试目录
3. 非目标：不改 codegen/interpreter，不改 MemoryDomain
4. 测试产物可对比：生成 JSON/CSV 格式报告，支持两次运行 diff

## 关键文档

- 父 roadmap: `docs/dev/in-progress/20260522-gc-industrialization/roadmap-v1-01.md`
- 父 STATUS: `docs/dev/in-progress/20260522-gc-industrialization/STATUS.md`
- 实现计划: `plan-v1-01.md`

## 完成摘要

### 产物文件

- `testing/src/native/runtime-core/gc/gc_regression_benchmark.cpp` — 6 个基准测试
- `testing/src/native/runtime-core/gc/CMakeLists.txt` — Batch F 注册（benchmark;gc 标签）

### 基准测试结果

| 测试 | 指标 | 值 |
|------|------|----|
| AllocThroughput | NurseryAllocate 吞吐量 | 1704 MB/s, 35 ns/alloc |
| AllocAtomicThroughput | NurseryAllocateAtomic 吞吐量 | 1990 MB/s, 30 ns/alloc |
| MixedAllocThroughput | 混合大小吞吐量 | 2416 MB/s, 89 ns/alloc |
| LohAllocLatency | 大对象分配延迟 | avg 56.8µs, p99 178µs |
| YoungGcPause | Young GC STW 暂停 | avg 242µs, p99 970µs |
| MultiThreadAlloc | 4 线程并发吞吐量 | 48.6M allocs/sec |

### 修复的问题

1. 时间基准循环 `latencies_ns` 向量增长 OOM — 改为固定次数分配
2. BGC 后台线程导致 TearDown leak 检测失败 — 在 fixture SetUp 中 `snapshot_.Capture()` 前重置 TLAB
3. 测试间 TLAB 泄露 — 所有 benchmark 结束时 `tls_tlab = TLAB{}`
4. 多线程 Safepoint 超时 — 改为固定分配次数而非时间基准

### 执行阶段问题

- 基准标签 `benchmark;gc` 排除于默认 ctest 运行（仅手动 `--gtest_filter` 执行）
- 预构建 chaos_*.lib 在 `build/native/` 下编译，`testing/` 构建依赖  `/FORCE:MULTIPLE` 链接

## 下一步

物理移入 `docs/dev/completed/`，然后同步父 roadmap 的 child 映射。
