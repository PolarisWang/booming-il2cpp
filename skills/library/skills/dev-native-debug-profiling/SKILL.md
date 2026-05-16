# Native 调试与性能分析规范

## 构建配置

| 配置 | LOG_LEVEL | TRACE | ASSERT | PROFILE_SCOPE | 用途 |
|------|-----------|-------|--------|---------------|------|
| CHECK | 3 (DEBUG) | 启用 | 启用 | 启用 | 日常开发 |
| PROFILE | 2 (INFO) | 启用 | 关 | 启用 | 性能分析 |
| SHIP | 0 (ERROR) | 关 | 关 | 关 | 发布 |

通过 CMakePresets.json 切换：`cmake --preset debug` / `cmake --preset profile` / `cmake --preset ship`。

## LOG_DEBUG/INFO 使用规则

- **追逻辑问题**：在怀疑路径插入 `CHAOS_IL2CPP_LOG_DEBUG(...)`，定位后**必须删除**
- **INFO**：可用于关键生命周期事件，可保留
- **WARN**：用于可恢复异常情况，可保留
- **ERROR**：始终启用，不可恢复错误

## 热点路径约束

`src/native/runtime-core/fast_dispatch.cpp` 默认 `#define CHAOS_IL2CPP_LOG_LEVEL 0`（仅保留 ERROR）：
```cpp
#define CHAOS_IL2CPP_LOG_LEVEL 0
```
其他文件如需强制覆盖日志级别，在 `#include "chaos/log.h"` 前定义：
```cpp
#define CHAOS_IL2CPP_LOG_LEVEL 3
#include <chaos/log.h>
```

## 调试模板

```cpp
// 追踪值的变化 — 用完即删
CHAOS_IL2CPP_LOG_DEBUG("X=%d (0x%x)", x, x);

// 确认分支到达
CHAOS_IL2CPP_LOG_DEBUG("进入 OptimizedPath, is_special=%d", is_special);

// 性能分析 — 已预埋 PROFILE_SCOPE，cmake 开关切换
//    cmake -DCHAOS_IL2CPP_PROFILE_ENABLED=ON ...
```

## PROFILE_SCOPE 预埋点

因 `config.h` 在 CHECK/PROFILE 下默认 `CHAOS_IL2CPP_PROFILE_ENABLED=1`，以下 scope 自动生效。需强制关闭则在 `#include "chaos/config.h"` 前定义 `#define CHAOS_IL2CPP_PROFILE_ENABLED 0`。

**runtime_core.cpp**: GcAllocate, GcAllocateAtomic, ObjectNew, ArrayNew, StringNewUtf8, BoxValueObject, MethodInvoke
**fast_dispatch.cpp**: FastExecute, Handle_Call, Handle_Box, Handle_NewObj, Handle_NewArr, Handle_LdArg, Handle_LdLoc, Handle_StLoc, Handle_LdFld, Handle_StFld, Handle_LdStr, Handle_Ret, Handle_Pop, Handle_Dup, Handle_Br, Handle_BrTrue, Handle_BrFalse, Handle_Throw, Handle_Leave, Handle_Unbox, Handle_LdLen, Handle_Conv_I4/I8/R4/R8, Handle_Add, Handle_Sub + 所有比较/分支/位运算
**runtime_instantiation.cpp**: InterpreterDispatch, InterpreterDispatchRaw
**vtable_registry.cpp**: ResolveVirtualMethodPointer
**method_table.cpp**: ResolveMethodTable
**thread_state.cpp**: SafepointPoll
**gc_bump_cache.h**: GcAllocateImpl
**interpreter_entry.cpp**: InterpreterEntryDirect, Step1_LowerIR, Step1c_2InstrFastPath, FastExecute, SetupFrame, FastExecuteCall

## profile.h 实现特性

`src/native/common/chaos/profile.h` 使用 RDTSC 做零 I/O 热点统计：

- **Hash 加速槽查找**：FNV-1a 开放寻址哈希表取代 O(n) 线性扫描
- **嵌套层级跟踪**：thread_local depth 计数器，`kProfileHashSize = 128` 低冲突率
- **RDTSC→ns 校准**：首次通过 QueryPerformanceFrequency 校准，dump 输出 avg_ns/total_ns
- **编译时零开销**：`CHAOS_IL2CPP_PROFILE_ENABLED=0` 时展开为 NullProfileScope
