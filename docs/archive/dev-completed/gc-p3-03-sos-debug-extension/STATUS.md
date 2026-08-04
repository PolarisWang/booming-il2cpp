---
task_id: gc-p3-03
title: SOS 调试扩展（G-16）
task_type: plan
lifecycle_status: completed
phase: completed
created_at: 2026-05-22 20:20:00 +08:00
updated_at: 2026-05-22 20:30:00 +08:00
current_dir: docs/dev/completed/gc-p3-03-sos-debug-extension
parent_task_id: 20260522-gc-industrialization
source_relation: roadmap-child
clearance_source: parent-roadmap
entry_skill: dev-writing-plans
---

# STATUS — gc-p3-03: SOS 调试扩展（G-16）

## 目标

为 CRAG GC 子系统创建 WinDbg SOS 样式的调试器扩展 DLL，使开发者可以在 WinDbg 中检查 GC 状态（统计信息、事件环形缓冲区、暂停直方图），无需侵入式日志。

## 实现

### 新增模块

- `src/native/diagnostics/sos/gc_sos.cpp` — WinDbg 扩展 DLL，6 个调试命令
- `src/native/diagnostics/sos/CMakeLists.txt` — 构建配置，链接 DbgEng.lib
- `src/native/runtime-core/gc/gc_debug_contract.h` — extern "C" 调试符号契约
- `src/native/runtime-core/gc/gc_debug_contract.cpp` — 初始化契约指针
- `tests/contracts/native/runtime-core/gc_debug_contract_test.cpp` — 契约验证测试

### 修改的文件

- `CMakeLists.txt`（根）— 添加 `src/native/diagnostics/sos` 子目录
- `src/native/runtime-core/CMakeLists.txt` — 添加 `gc_debug_contract.cpp`
- `tests/contracts/native/runtime-core/CMakeLists.txt` — 添加 `chaos_gc_debug_contract_test`

### WinDbg 命令

| 命令 | 功能 | 数据源 |
|------|------|--------|
| `!gc.help` | 显示所有可用命令和用法 | — |
| `!gc.info` | GC 子系统状态（代次、暂停时间概览） | GcStats POD |
| `!gc.stats` | 完整的 GC 计数器（young/full/gen1 收集、分配） | GcStats POD |
| `!gc.events` | GC 事件环形缓冲区（最后 64 个事件） | g_gc_event_ring |
| `!gc.histogram` | 暂停时间分布直方图（6 个桶） | g_gc_pause_histogram |
| `!gc.dumpall` | 一次转储所有 GC 状态 | 以上全部 |

### 架构

- **extern "C" 调试契约**：`gc_debug_contract.h` 定义了 7 个 C 链接全局变量，包含指向 GC 运行时数据结构的指针（`g_gc_stats`、事件环、直方图）。WinDbg 扩展使用 `IDebugSymbols::GetOffsetByName` 定位这些符号。
- **进程间读取**：扩展通过 `IDebugDataSpaces::ReadVirtual` 读取目标进程内存。定义了 `GcStatsPod` 等 POD 镜像结构，与 `std::atomic` 布局兼容。
- **独立 DLL**：`chaos_gc_sos.dll` 是一个独立的 WinDbg 扩展。在使用 `!gc.<command>` 之前用 `.load chaos_gc_sos` 加载。

### 构建依赖

- 需要 Windows SDK 10.0.22621.0 或更高版本
- 需要 Debugging Tools for Windows (`dbgeng.h` / `dbgeng.lib`)
- 在 `windows-x64-reference` 预设下构建

## 验证

- `chaos_gc_debug_contract_test`: 4 tests, 0 failures
- `chaos_gc_events_test`: 5 tests, 0 failures（回归检查）
- `chaos_gc_sos.dll`: 构建成功（64KB）
