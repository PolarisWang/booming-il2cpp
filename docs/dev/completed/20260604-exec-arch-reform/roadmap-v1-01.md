# Execution Architecture Reform — Roadmap

## 目标

将 AOT/JIT/Interpreter 执行流程从当前架构（`if constexpr` + 双重入口指针 + 无 callsite 缓存）改造为宏驱动 + 单一入口 + callsite 缓存的架构，修复已识别的 11 项问题。

## 范围边界

- 全部修改在 native C++ 侧（jit / runtime-core / interpreter）
- 不涉及 codegen C# 生成器
- 不涉及 CMakePresets.json 新增预设
- 不涉及 test-only hotpatch 支持（标记为设计边界）

## 非目标

- 运行时 JIT 开关（已放弃）
- test-only hotpatch 支持（已关闭）
- PIC（多态内联缓存，YAGNI）
- Step C 编译时验证表（P3，不优先实现）

## 阶段列表

| 阶段 | 名称 | 依赖 | 优先级 |
|------|------|------|--------|
| 1 | `#if` 宏迁移 + 统一 direct_ptr | 无 | P0 |
| 2 | CallSite Cache（JIT→JIT 直调） | 阶段 1 | P0 |
| 3 | Precode 硬化 + Hotpatch O(1) | 阶段 1 | P2 |
| 4 | Tier 阈值调优 + 配置治理 | 阶段 1 | P1 |

## 每阶段完成定义

### Phase 1: Macro Migration + Unified Entry

| 项 | 内容 |
|----|------|
| goal | 将 ExecutionConfig / if constexpr 替换为 `#if CHAOS_IL2CPP_ENABLE_JIT/AOT` 宏，统一 direct_ptr 为单一入口 |
| exit_criteria | build & test pass (全部 3 个 preset)，Step A/A0/A5/B 使用宏守卫，aot_entry 消除 |
| deliverables | execution_config.h 重写, entry_direct.cpp 宏迁移, patch_loader.h aot_entry 消除, CMakeLists.txt JIT/AOT 宏定义 |
| dependencies | 无 |
| resolved_decisions | 见设计摘要 |
| watch_items | 6-8 处 #if 散布，注意 #endif 注释标记 |

Sub-tasks:
- 1a: `execution_config.h` 重写 — 移除 ExecutionConfig struct，替换为纯宏守卫
- 1b: `entry_direct.cpp` 宏迁移 — Step A/A0/A5/B 的 if constexpr → #if
- 1c: `fast_dispatch.cpp` 宏迁移 — 剩余 if constexpr 引用
- 1d: `patch_loader.h/.cpp` — 消除 aot_entry 双重指针，统一到 direct_ptr
- 1e: `CMakeLists.txt` — 新增 CHAOS_IL2CPP_ENABLE_JIT/AOT 宏定义，移除 ExecutionProfile enum

### Phase 2: CallSite Cache

| 项 | 内容 |
|----|------|
| goal | Handle_Call 中实现 JIT→JIT 直调 callsite cache，跳过 InterpreterEntryDirect cascade |
| exit_criteria | JIT compiled 方法互相调用时不再经过 InterpreterEntryDirect，benchmark 验证周期减少 |
| deliverables | fast_dispatch.cpp CallSiteCache struct + Handle_Call 改造, hotpatch invalidate 机制 |
| dependencies | Phase 1 |
| watch_items | hotpatch 时的 cache invalidation, JIT→JIT 直调的 GC safety |

Sub-tasks:
- 2a: `fast_dispatch.cpp` — CallSiteCache 实现（含 hotpatch invalidate）
- 2b: Benchmark verify — 调用链性能对比

### Phase 3: Precode Hardening + Hotpatch O(1)

| 项 | 内容 |
|----|------|
| goal | Precode timeout 补充 fail count → kJitSkip；PatchMethod 缓存 HotpatchEntryV0* 消除哈希查找 |
| exit_criteria | precode 超时后不再反复触发 JIT；hotpatch dispatch 不经过字符串哈希 |
| deliverables | jit_precode.cpp 硬化, PatchMethod + patch_loader.cpp hotpatch_entry* 缓存 |
| dependencies | Phase 1 |

Sub-tasks:
- 3a: `jit_precode.cpp` — fail count + kJitSkip 机制
- 3b: `PatchMethod + patch_loader.cpp` — HotpatchEntryV0* 指针缓存

### Phase 4: Tier Tuning + Config Governance

| 项 | 内容 |
|----|------|
| goal | 可调 tier promotion 阈值 + CMake/static_assert 配置组合验证 |
| exit_criteria | tier_threshold 可通过环境变量/配置结构调整；非法配置组合在编译时报错 |
| deliverables | tier_manager.cpp 阈值配置化, CMakeLists.txt + execution_config.h static_assert |
| dependencies | Phase 1 |

Sub-tasks:
- 4a: `tier_manager.cpp` — 阈值从硬编码改为可配置结构体
- 4b: `CMakeLists.txt` + `execution_config.h` — 配置组合编译时断言

## 子任务映射

| task_id | phase | status | owner | purpose | depends_on | batch_id | conflict_scope | estimated_effort |
|---------|-------|--------|-------|---------|-----------|----------|----------------|-----------------|
| 1a | 1 | planned | main | execution_config.h 重写为纯宏 | 无 | 1 | src/native/common/chaos/runtime/ | small |
| 1b | 1 | planned | main | entry_direct.cpp 宏迁移 | 1a | 1 | src/native/interpreter/ | medium |
| 1c | 1 | planned | main | fast_dispatch.cpp 宏迁移 | 1a | 1 | src/native/interpreter/ | small |
| 1d | 1 | planned | main | 消除 aot_entry 统一到 direct_ptr | 1a | 1 | src/native/runtime-core/ | medium |
| 1e | 1 | planned | main | CMakeLists.txt 宏定义 | 1b,1c,1d | 1 | CMakeLists.txt | small |
| 2a | 2 | planned | main | CallSite Cache 实现 | Phase 1 | 2 | src/native/interpreter/ | medium |
| 2b | 2 | planned | main | Benchmark verify | 2a | 2 | (benchmark) | small |
| 3a | 3 | planned | main | Precode timeout + kJitSkip | Phase 1 | 3 | src/native/jit/ | small |
| 3b | 3 | planned | main | PatchMethod hotpatch_entry* 缓存 | Phase 1 | 3 | src/native/runtime-core/ | small |
| 4a | 4 | planned | main | Tier 阈值可配置化 | Phase 1 | 4 | src/native/jit/ | small |
| 4b | 4 | planned | main | 配置组合编译时断言 | Phase 1 | 4 | CMakeLists.txt, execution_config.h | small |

## 依赖

- Phase 1 → Phase 2/3/4（所有后续阶段依赖宏迁移完成）
- Phase 2a → Phase 2b
- 1a → 1b/1c/1d → 1e（CMake 最终修改需要所有源文件变更完成）

## 风险

| 风险 | 可能性 | 影响 | 缓解 |
|------|--------|------|------|
| #if 散布遗漏导致混淆 | 中 | 运行时行为异常 | 每处变更后 full build + test |
| aot_entry 消除后 AOT 路径不工作 | 低 | AOT 不能使用 | Phase 1 后运行 AOT gate CI |
| CallSite Cache hotpatch invalidate 遗漏 | 中 | hotpatch 后调用旧代码 | Cache 使用 slot_id 做 invalidation key |
| Phase 1 构建错误多 | 中 | 开发周期延长 | 先对每个文件单独改+验证再统一 commit |

## 当前建议推进顺序

Phase 1 (macro migration) → Phase 2 (callsite cache) → Phase 3 (precode + hotpatch) → Phase 4 (tier + config)

## 子任务执行策略

- `child_execution_mode`: auto
- `auto_continue`: true
- `auto_stop_policy`: blocking-only
- `dispatch_model`: sequential（子任务共享文件，不能并行）
- `recommended_next_child`: 1a
