---
task_id: gc-p3-01
title: ETW 原生诊断事件系统（G-7）
task_type: plan
lifecycle_status: completed
phase: completed
created_at: 2026-05-22 19:30:00 +08:00
updated_at: 2026-05-22 19:35:00 +08:00
current_dir: docs/dev/completed/gc-p3-01-etw-diagnostic-events
parent_task_id: 20260522-gc-industrialization
source_relation: roadmap-child
clearance_source: parent-roadmap
entry_skill: dev-executing-plans
---

# STATUS — gc-p3-01: ETW 原生诊断事件系统（G-7）

## 目标

为 CRAG GC 子系统添加 Windows ETW（Event Tracing for Windows）原生诊断事件，使 GC 事件可通过 Xperf/WPA/PerfView 等标准 profiler 工具捕获和分析。

## 实现

### 新增 ETW Provider
- **Provider GUID**: `{3A5C4B6D-8E9F-4A2B-9C1D-7E8F0A3B6C5D}`
- **Keywords**: 0x1 = GC lifecycle events
- **Level**: 4 (Informational)
- **Lazy init**: provider 在首次 GcFireEtwEvent 调用时自动注册，无需显式初始化

### ETW Event Descriptors

| Event ID | Name | Payload |
|----------|------|---------|
| 0x01 | GCStart | (none) |
| 0x02 | GCEnd | pause_ns, reclaimed_bytes |
| 0x03 | GCYoungStart | (none) |
| 0x04 | GCYoungEnd | pause_ns, reclaimed_bytes, promoted_bytes, promoted_objects |
| 0x05 | GCFullStart | (none) |
| 0x06 | GCFullEnd | pause_ns, reclaimed_bytes, objects_marked, pages_collected |
| 0x07 | GCOOM | (none) |
| 0x08 | GCGen1Collect | pause_ns, reclaimed_bytes |

### 集成方式
- `GcFireEvent()` 现在同时调用 `GcFireEtwEvent()`，所有 GC 事件自动同步到 ETW
- 平台无关：`#if defined(_WIN32)` 保护 ETW 代码，非 Windows 平台编译为 no-op
- 零开销：provider 未注册时跳过所有 EventWrite 调用

### 文件改动

| 文件 | 改动 |
|------|------|
| src/native/runtime-core/gc/gc_events.h | 添加 GcFireEtwEvent, GcInitializeEtw, GcShutdownEtw 声明；GcFireEvent 加入 ETW 调用 |
| src/native/runtime-core/gc/gc_events.cpp | 实现 ETW provider (EventRegister/EventWrite/EventUnregister)，定义事件 payload 结构和写入函数 |

### 复用模式
遵循 `threadpool_events.cpp` 已有的经典 ETW（EventRegister/EventWrite/EventUnregister）模式，与 ThreadPool ETW 体系一致。

## 验证

- gc_events_test: 5 tests, 0 failures
- gc_finalizer_test: 16 tests, 0 failures
- gc_api_test: 11 tests, 0 failures
- gc_scheduler_test: 15 tests, 0 failures
- 所有依赖 runtime_core 的测试编译通过
