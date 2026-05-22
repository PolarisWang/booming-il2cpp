---
task_id: gc-iw-d1
title: ARM64 编译验证 + 写屏障 UT（C4）
task_type: plan
lifecycle_status: completed
phase: completed
parent_task_id: 20260523-crag-gc-industrialization-sequel
source_relation: roadmap-child
clearance_source: parent-roadmap
preflight_review: warn
auto_execution_decision: continue
created_at: 2026-05-23
updated_at: 2026-05-23
current_dir: docs/dev/completed/gc-iw-d1
---

# STATUS — gc-iw-d1：ARM64 编译验证 + 写屏障 UT（C4）

## 方案

### 写屏障 UT（已完成）
CRAG 的 card table 是纯 C++ 实现（`gc_card_table.h`），无 x64 汇编依赖，在所有架构上语义一致。新增 4 个测试覆盖写屏障核心场景：

| 测试 | 验证内容 |
|------|---------|
| NurseryFastSkip | Nursery 范围内 DirtyCard 跳过 card 写入 |
| ClearCardRange | 范围清理 vs 全局清理 |
| ScanDirtyCardsBatched | 连续 dirty card 批处理合并 |
| ConcurrentDirtyAndScan | 并发写入+扫描不崩溃/不损坏 |

### ARM64 编译（回退方案触发）
按 roadmap 风险缓解策略，ARM64 完全编译受阻于 x64 特有 intrinsic，回退到部分验证：

**通过：**
- CMake 配置：`Visual Studio 17 2022 -A ARM64` 配置成功
- `chaos_common.lib`：ARM64 编译通过
- `chaos_fmt.lib`：ARM64 编译通过
- card table header：纯 C++，无架构依赖

**受阻（需后续修复）：**
- `thread_state.cpp`：`__readgsqword` → ARM64 需 `__readx18(2)`（TEB 自指针）
- `thread_state.cpp`：`_mm_pause` → ARM64 需 `__yield()`（CPU yield hint）
- `synchronization.cpp`：`_mm_pause` ×4 → ARM64 需 `__yield()`
- `gc_old_gen.cpp`：`_mm_pause` / `_mm_prefetch` → ARM64 需 `__yield()` / `__prefetch()`
- `gc_old_gen.cpp`：`__readgsqword` → ARM64 需 `__readx18(2)`

## 改动文件

| 文件 | 改动 |
|------|------|
| `CMakePresets.json` | 新增 `windows-arm64-reference` preset（Visual Studio 17 2022 + ARM64） |
| `CMakeLists.txt` | 新增 `windows-arm64-reference` 预设路由（编译 common，跳过 runtime-core） |
| `build/toolchains/windows-arm64-reference.cmake` | 新增 ARM64 toolchain 文件 |
| `testing/.../gc_card_table_ext_test.cpp` | 新增 4 个写屏障测试（共 9 个测试） |

## 验证

| 测试套件 | 结果 |
|---------|------|
| test_gc_card_table_ext（9 测试） | 9/9 PASS ✅ |
| test_gc_scheduler（13 测试） | 13/13 PASS ✅ |
| test_gc_managed_api（11 测试） | 11/11 PASS ✅ |
| chaos_common ARM64 编译 | PASS ✅ |
| chaos_runtime_core ARM64 编译 | BLOCKED（x64 intrisics） |

## 已知 ARM64 编译阻塞

```
需要平台抽象（建议在 gc-iw-e2 中完成）：
  CHAOS_IL2CPP_READ_TEB()      → __readgsqword(0x30) / __readx18(2)
  CHAOS_IL2CPP_CPU_PAUSE()     → _mm_pause() / __yield()
  CHAOS_IL2CPP_PREFETCH_READ() → _mm_prefetch() / __builtin_prefetch() / __prefetch()
```

## 完成证据

- arch_review: ok（纯 C++ card table + CMake preset 新增，无结构影响）
- authority_review: n/a
- verification:
  - unitTest: 9+13+11=33 测试通过
  - arm64_build: chaos_common 通过，runtime-core 阻塞（已知 intrinsic 不兼容）
- test_result: passed
- wiki: n/a
- next: gc-iw-d2（Server GC 生产硬化 + E2E 压力测试）
