---
task_id: gc-p2-06
title: GC 编译时选择（G-29）
task_type: plan
lifecycle_status: completed
phase: completed
created_at: 2026-05-22 10:45:00 +08:00
updated_at: 2026-05-22 10:50:00 +08:00
current_dir: docs/dev/completed/gc-p2-06-compile-time-selection
parent_task_id: 20260522-gc-industrialization
source_relation: roadmap-child
clearance_source: parent-roadmap
blocking_questions: []
question_clearance: cleared
clearance_confirmed_by_user: true
---

# STATUS — gc-p2-06: GC 编译时选择（G-29）

## 目标

添加 GC 编译时 feature toggle 体系（CMake option → preprocessor define），使构建时可以选择启用/禁用特定 GC 功能，类似 CoreCLR 的 FEATURE_* 宏体系。

## 设计方案

### 方案
在 `src/native/runtime-core/CMakeLists.txt` 中添加 `option()` 声明，通过 `target_compile_definitions` 传播为 `#define`。

### 新增选项

| 选项 | 默认 | 说明 |
|------|------|------|
| `CHAOS_IL2CPP_GC_BGC` | ON | 后台 GC 并发 mark-sweep |
| `CHAOS_IL2CPP_GC_PARALLEL_MARK` | ON | 并行标记工作线程 |
| `CHAOS_IL2CPP_GC_EVENTS` | ON | GC 事件追踪和回调 |
| `CHAOS_IL2CPP_GC_STRESS` | OFF | GC 压力测试模式 |
| `CHAOS_IL2CPP_GC_LARGE_PAGES` | OFF | 大页支持（已有，新增 CMake option 声明） |

### 改动

| # | 文件 | 改动 |
|---|------|------|
| 1 | `CMakeLists.txt` | 添加 5 个 `option()` + 条件 `target_compile_definitions` |
| 2 | `gc/gc_features.h` | 新建集中式 feature flag 参考文档 |
| 3 | `CMakePresets.json` | debug preset 显式声明 GC feature 值 |

## 测试验证

```
chaos_gc_finalizer_test: 16 tests, 0 failures
All existing GC tests continue to pass (all features default ON → no behavioral change)
```

## 结果

- **CMake infrastructure**: option() declarations with sensible defaults, propagated as CHAOS_IL2CPP_GC_*=1
- **Reference header**: gc_features.h documents all flags, their effects, and CoreCLR equivalents
- **Default-safe**: all features default to their current state (ON for production, OFF for stress/large-pages)
- **Preset integration**: debug preset explicitly declares all values for CI reproducibility
