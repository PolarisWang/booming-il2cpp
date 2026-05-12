# Chaos IL2CPP 开发规则

## 全局优先级约束（强制）

以下三条优先级在所有开发阶段必须遵守，从 brainstorm 到验证验收覆盖全流程：

1. **性能最优（第一优先级）** — 一切以 il2cpp 生成的 C++ 代码运行性能为第一考量。方案选择、翻译路径决策、代码生成策略，当其他因素与性能冲突时，性能优先。
2. **方案完美性（第二优先级）** — il2cpp 最终方案的架构完整性、正确性、可维护性为第二考量。在性能不受损的前提下，追求架构的干净、一致、可维护。
3. **HotUpdate 支持（第三优先级）** — 最大化支持热更新为第三考量。在性能和方案完整性都满足的前提下，优先选择对热更新友好的方案。

**裁决规则**：低优先级与高优先级冲突时，低优先级让位于高优先级。但高优先级方案不应故意破坏低优先级——在保证高优先级的前提下，尽量兼顾低优先级。

## Trace 优先调试

遇到任何失败、测试红、编译错或意外行为时，**必须先查 trace 系统定位问题**，再分析代码或猜测原因：

1. `run trace` — 查看最新会话的 span 树定位失败阶段
2. `trace-analyze` — 分析阶段耗时分布
3. 只有 trace 信息不足时，才考虑加 print/log 或断点调试

详见 `wiki/04-工具与集成/统一追踪体系.md#错误排查工作流`。

## 架构优先开发

在开发任何 il2cpp 新功能或修改现有翻译行为前，必须先执行 `dev-architecture-first-development` 技能完成架构查询，确认翻译路径与既有架构一致。禁止在未查表的情况下直接进入实现。

## 完成前自测

任何开发任务完成后，必须按以下顺序完成自测：

1. **查 wiki 要求** — 查阅 `wiki/` 中对应功能模块的自测要求，按说明执行
2. **无要求则走 TDD** — 如果 wiki 中没有自测要求，则按 TDD 规范补充测试：先写失败测试，再改实现，直到测试通过
3. **验证管线** — 如果项目有对应验证管线（如 foundation-dll verification pipeline），必须跑通后再声称完成

## 知识记录

- 新翻译路径必须记录到 `wiki/03-功能模块/il2cpp-核心架构/il2cpp-核心翻译路径参考.md`
- 设计讨论必须在 `docs/discuss/` 完成
- 每个功能的 STATUS.md 必须包含 `## 架构映射` 小节（记录格式见 `dev-architecture-first-development` 技能）

## 项目文档原则

- `wiki/` 记录长期知识，`docs/` 记录过程
- 仅当任务确实产生长期有效知识时才更新 wiki
- 历史决策只保留最终方案，不保留讨论过程

## 技能系统

技能通过 `.claude/skills/` 注册，支持 `/dev-<skill-name>` 或 Skill 工具调用。完整技能目录见 `skills/discovery/skill-index.md`（自动加载）。自进化系统详情见 `skills/` 目录。

## Native 调试/性能分析规范

### 三档构建配置

| 配置 | LOG_LEVEL | TRACE | ASSERT | PROFILE_SCOPE | 用途 |
|------|-----------|-------|--------|---------------|------|
| CHECK | 3 (DEBUG) | 启用 | 启用 | 启用 | 日常开发 |
| PROFILE | 2 (INFO) | 启用 | 关 | 启用 | 性能分析 |
| SHIP | 0 (ERROR) | 关 | 关 | 关 | 发布 |

通过 CMakePresets.json 一键切换：`cmake --preset debug` / `cmake --preset profile` / `cmake --preset ship`。

### LOG_DEBUG/INFO 使用规则

- **追逻辑问题**：在怀疑路径直接插入 `CHAOS_IL2CPP_LOG_DEBUG(...)`，定位后**必须删除**（不允许残留到提交）
- **INFO**：可用于关键生命周期事件（初始化完成、模块加载等），可保留
- **WARN**：用于可恢复的异常情况，可保留
- **ERROR**：始终启用，用于不可恢复的错误

### 热点路径约束

`src/native/runtime-core/fast_dispatch.cpp` 是**最热执行路径**，默认 `#define CHAOS_IL2CPP_LOG_LEVEL 0` 仅保留 ERROR：
```cpp
// fast_dispatch.cpp 顶部
#define CHAOS_IL2CPP_LOG_LEVEL 0
```
其他文件如需强制覆盖日志级别，在 `#include "chaos/log.h"` 前定义：
```cpp
#define CHAOS_IL2CPP_LOG_LEVEL 3  // 临时开启 DEBUG
#include <chaos/log.h>
```

### PROFILE_SCOPE 预埋点

以下热点函数已预埋 `CHAOS_IL2CPP_PROFILE_SCOPE`。因 `config.h` 在 CHECK/PROFILE 构建下默认启用 `CHAOS_IL2CPP_PROFILE_ENABLED=1`，这些 scope 在 `cmake --preset debug` / `cmake --preset profile` 中自动生效。如需强制关闭，在包含 config.h 前定义 `#define CHAOS_IL2CPP_PROFILE_ENABLED 0`。

**runtime_core.cpp**: GcAllocate, GcAllocateAtomic, ObjectNew, ArrayNew, StringNewUtf8, BoxValueObject, MethodInvoke
**fast_dispatch.cpp**: FastExecute(主循环), Handle_Call, Handle_Box, Handle_NewObj, Handle_NewArr, Handle_LdArg, Handle_LdLoc, Handle_StLoc, Handle_LdFld, Handle_StFld, Handle_LdStr, Handle_Ret, Handle_Pop, Handle_Dup, Handle_Br, Handle_BrTrue, Handle_BrFalse, Handle_Throw, Handle_Leave, Handle_Unbox, Handle_LdLen, Handle_Conv_I4, Handle_Conv_I8, Handle_Conv_R4, Handle_Conv_R8, Handle_Add, Handle_Sub, plus 所有比较/分支/位运算 Handler
**runtime_instantiation.cpp**: InterpreterDispatch, InterpreterDispatchRaw
**vtable_registry.cpp**: ResolveVirtualMethodPointer
**method_table.cpp**: ResolveMethodTable
**thread_state.cpp**: SafepointPoll
**gc_bump_cache.h**: GcAllocateImpl
**interpreter_entry.cpp**: InterpreterEntryDirect, Step1_LowerIR, Step1c_2InstrFastPath, FastExecute, SetupFrame, FastExecuteCall

### profile.h 实现特性

`src/native/common/chaos/profile.h` 使用 RDTSC 做零 I/O 热点统计，具备以下优化：

- **Hash 加速槽查找**：FNV-1a 开放寻址哈希表取代 O(n) 线性扫描，scope 进入时 ~O(1) 定位
- **嵌套层级跟踪**：thread_local depth 计数器，`kProfileHashSize = 128` 低冲突率
- **RDTSC→ns 校准**：首次构造时通过 `QueryPerformanceFrequency` 校准，dump 输出 avg_ns/total_ns
- **编译时零开销**：`CHAOS_IL2CPP_PROFILE_ENABLED=0` 时展开为 `NullProfileScope`，编译器完全消除

### 调试模板（AI Agent 使用）

```cpp
// 1. 追踪值的变化 — 临时插入，用完即删
CHAOS_IL2CPP_LOG_DEBUG("变量 X 的值: %d (0x%x)", x, x);

// 2. 确认分支到达
CHAOS_IL2CPP_LOG_DEBUG("进入 OptimizedPath, is_special=%d", is_special);

// 3. 性能热点分析 — 已预埋 PROFILE_SCOPE，只需 cmake 开关
//    cmake -DCHAOS_IL2CPP_PROFILE_ENABLED=ON ...
//    运行后查看 stdout 的 RDTSC 耗时表
```

## 统一内存分配约束（强制）

IL2CPP 生成 C++ 代码会被引入游戏引擎源码，因此分配行为必须遵循以下约束：

1. **禁止全局 operator new/delete 重载** — 任何情况下不得添加全局 `operator new` / `operator delete` 重载。生成代码引入游戏引擎后，全局重载会污染引擎的分配行为。
2. **codegen 输出必须使用 CHAOS_IL2CPP_ 宏** — codegen 生成 `new T{}` / `new T[N]` / `malloc` / `free` 都必须替换为 `CHAOS_IL2CPP_NEW_GC` / `CHAOS_IL2CPP_NEW_GC_ARRAY` / `CHAOS_IL2CPP_MALLOC` 等宏。
3. **禁止跨域分配/free 不匹配** — GC 域内存必须通过 GC 回收，Domain 域通过 heap->Destroy() 批量释放，Raw 域通过 std::free 释放。不得混用。
4. **新分配策略必须通过 GcAllocate/GcAllocateAtomic 内部切换** — A→B→C 三阶段（Bump Arena → TLS GC Cache → Precise Generational GC）对 codegen 完全透明，codegen 输出无需修改。

详见 `wiki/03-功能模块/06-il2cpp核心架构/01-翻译管线/17-统一内存分配体系.md`。