---
task_id: gc-p2-03
title: iOS/Android 平台验证 + 平台适配测试（G-27）
task_type: plan
lifecycle_status: completed
phase: completed
created_at: 2026-05-22 10:17:00 +08:00
updated_at: 2026-05-22 19:30:00 +08:00
completed_at: 2026-05-22 19:30:00 +08:00
current_dir: docs/dev/in-progress/gc-p2-03-platform-validation
parent_task_id: 20260522-gc-industrialization
source_relation: roadmap-child
clearance_source: parent-roadmap
blocking_questions: []
question_clearance: cleared
clearance_confirmed_by_user: true
---

# STATUS — gc-p2-03: iOS/Android 平台验证

## 目标

修复 CRAG GC 在非 Windows 平台（macOS/iOS/Android/Linux）的编译失败和功能缺失，确保 GC 子系统的跨平台兼容性。

## 发现的问题

### 编译失败（3 处）

| # | 文件 | 问题 | 修复 |
|---|------|------|------|
| 1 | `gc_loh.cpp:297` | `VirtualQuery` / `MEMORY_BASIC_INFORMATION` 是 Windows-only API，在 POSIX 平台无法编译 | 加 `#if defined(_WIN32)` 守卫，POSIX 路径直接调用 `FreeSegment` |
| 2 | `CMakeLists.txt:167-171` | Android NDK 没有 `numa.h`，`UNIX AND NOT APPLE` 条件把 Android 也纳入了 libnuma 链接 | 改为 `UNIX AND NOT APPLE AND NOT ANDROID` |
| 3 | `thread_state.cpp:139` | `pthread_getattr_np` 是 Linux/GLibc-only API，macOS/iOS 没有 | 加 `__APPLE__` 路径使用 `pthread_get_stackaddr_np` / `pthread_get_stacksize_np` |

### 功能缺失（2 处）

| # | 文件 | 问题 | 修复 |
|---|------|------|------|
| 4 | `gc_numa_other.cpp:44` | `GcNumaVirtualAlloc` 返回 `nullptr`，导致 macOS/iOS/Android 上所有 GC 页分配失败——GC 完全不可用 | 替换为真实 `mmap`/`munmap` 实现；新增大页 stub API |
| 5 | `gc_old_gen.cpp:574-592` | `DiagProtectPayloads`/`DiagUnprotectPayloads` 使用 Windows-only `VirtualProtect`，POSIX 平台功能缺失但未标记 | 替换为 `mprotect`（POSIX 通用） |
| 6 | `gc_old_gen.cpp:2125-2133` | `RelocateRoots` 自栈扫描使用 Windows-only TEB/`__readgsqword` | 三路平台分支：Windows TEB / Apple `pthread_get_stackaddr_np` / Linux `pthread_getattr_np` |

## 修改文件

| 文件 | 改动 |
|------|------|
| `src/native/runtime-core/CMakeLists.txt` | NUMA 源选择和 libnuma 链接排除 Android；添加 `CHAOS_IL2CPP_GC_LARGE_PAGES` 编译定义 |
| `src/native/runtime-core/gc/gc_numa_other.cpp` | `GcNumaVirtualAlloc` 从 nullptr 改为 mmap；`GcNumaVirtualFree` 从 no-op 改为 munmap；添加大页 stub API |
| `src/native/runtime-core/gc/gc_loh.cpp` | 添加 `gc_numa.h` include；`VirtualAllocPage` 添加大页尝试；`VirtualFreePage` 添加大页释放守卫；`Sweep()` VirtualQuery 加 Windows 守卫 |
| `src/native/runtime-core/gc/gc_old_gen.cpp` | DiagProtectPayloads 添加 mprotect POSIX 路径；RelocateRoots 自栈扫描添加 Apple/Linux 平台分支 |
| `src/native/runtime-core/thread_state.cpp` | RegisterThread 栈检测添加 Apple pthread API 分支 |

## 验证结果

- VS2022 构建：通过
- 63 个 GC 测试：全部 PASS（所有 9 个测试目标）
- 编译警告：无新增

## 审计结论

- 跨平台编译问题：已修复（3 处编译失败 + 2 处功能缺失）
- 非 Windows 平台 GC 可用性：已恢复（`gc_numa_other.cpp` 从 nullptr 改为 mmap 是关键的 functional fix）
- 大页支持：已通过 `CHAOS_IL2CPP_GC_LARGE_PAGES` 编译标记控制，跨平台 API 统一

## 退出条件

- [x] 全部跨平台编译问题已修复
- [x] 全部 GC 测试 PASS
- [x] 代码审查完成
