# Execution Architecture Reform — Roadmap v2

## 目标

修复 AOT/JIT/Interpreter 执行流程分析报告中识别的 13 个问题，按优先级分 4 个工作线推进。

## 范围边界

- 全部修改在 native C++ 侧（jit / runtime-core / interpreter）
- 不涉及 codegen C# 生成器
- 不涉及 CMakePresets.json 新增预设
- 问题 4/6/9/10 (DP1-a 脆弱性/SEH QuickJIT/ResolveDirectFn/CallVirt mono) 已标记 wontfix / watch_item

## 非目标

- 运行时 JIT 开关（已关闭）
- Test-only hotpatch 支持（已关闭）
- DP1-a 架构重构（watch_item）
- SEH Quick JIT 全量管线（is_tier0_ 限制，wontfix）

## 阶段列表

| 阶段 | 名称 | 依赖 | 优先级 | 风险 |
|------|------|------|--------|------|
| A | JIT 代码生命周期修复 | 无 | P0 | 低 |
| B | Deopt 稳定性 | A (共享验证环境) | P1 | 低 |
| C | 内存序修复 | A (共享验证环境) | P1 | 低 |
| D | 架构简化 — dispatch 真值源统一 | A, B, C | P2 | 高 |

## 每阶段完成定义

### Phase A: JIT Code Lifecycle Fix

| 项 | 内容 |
|----|------|
| goal | 修复 deopt 降级后 direct_ptr 残留、Eager JIT GC_TRANSITION 缺失、slot map 双注册 |
| exit_criteria | 降级后 Step A0 正确回退到 AOT 代码；Eager JIT 代码总是走 GC_TRANSITION；slot map 无重复 |
| deliverables | entry_direct.cpp 降级路径 + Step A0 / CompileAndCacheEntry + patch_loader.h |
| dependencies | 无 |
| resolved_decisions | PatchMethod 新增 original_aot_ptr 字段保存原始 AOT 函数指针 |
| watch_items | 确保 eager JIT 的 code 与 jit->code 一致 |

Sub-tasks:
- A1: `patch_loader.h` + `hotpatch_table.cpp` — PatchMethod 新增 `original_aot_ptr`，SetPatchedBySlot 保存
- A2: `entry_direct.cpp` 降级路径 — 恢复 `entry->direct_ptr = pm->original_aot_ptr`
- A3: `entry_direct.cpp` Step A0 — 检测 `cached_native_method != nullptr` 重路由到 Step A
- A4: `entry_direct.cpp` `CompileAndCacheEntry` — 注册新 slot map 前注销旧的

### Phase B: Deopt Stability

| 项 | 内容 |
|----|------|
| goal | 修复 OSR deopt 反弹循环，deopt 后跳过立即 OSR 重试 |
| exit_criteria | deopt 后 OSR 检测使用 deopt_count 判断跳过多试 |
| deliverables | ir_reg_alloc.cpp OsrTrigger / fast_dispatch.cpp TryFastOsrPromotion |
| dependencies | Phase A (共享验证环境) |

Sub-tasks:
- B1: `ir_reg_alloc.cpp` OSR 检测点 — deopt 后跳过立即 OSR，增加 backoff 阈值
- B2: `fast_dispatch.cpp` `TryFastOsrPromotion` — 同步检查 deopt_count

### Phase C: Memory Ordering Fix

| 项 | 内容 |
|----|------|
| goal | direct_ptr 写 fence、call_cache release store、eager compile token 传递 |
| exit_criteria | 所有 direct_ptr 写入后保证通过 tier_state acquire load 可见；call_cache 使用 release store |
| deliverables | entry_direct.cpp / jit_precode.cpp / eager_compile.cpp |
| dependencies | Phase A (共享验证环境) |

Sub-tasks:
- C1: `entry_direct.cpp` — `QuickJitAndCacheEntry` / `CompileAndCacheEntry` direct_ptr 写后加 release fence
- C2: `jit_precode.cpp` — `JitStubDispatchImpl` direct_ptr 写后加 release fence
- C3: `entry_direct.cpp` `RebuildCallCacheForT3` — `pm->call_cache` 赋值改为 release store
- C4: `eager_compile.cpp` — 确认 `cfg.method_token` / `cfg.method_module_id` 正确传递

### Phase D: Architecture Simplification

| 项 | 内容 |
|----|------|
| goal | 统一 dispatch 真值源，单次 tier_state load 决定所有路径 |
| exit_criteria | Step A / Step A-QuickJit / Step A0 合并为单一 if-else 链，消除二次加载和级联 |
| deliverables | entry_direct.cpp InterpreterEntryDirect 重构 |
| dependencies | Phase A, B, C (所有正确性修复先行) |

Sub-tasks:
- D1: `entry_direct.cpp` — 重构 `InterpreterEntryDirect` dispatch 瀑布，单次 tier_state load 决定路径
- D2: 回归测试 — foundation-dll benchmark 全量 family 通过

## 子任务映射

| task_id | phase | status | owner | purpose | depends_on | conflict_scope | estimated_effort |
|---------|-------|--------|-------|---------|-----------|----------------|-----------------|
| A1 | A | planned | main | patch_loader.h + hotpatch_table.cpp: original_aot_ptr | 无 | src/native/runtime-core/ | small |
| A2 | A | planned | main | entry_direct.cpp 降级路径恢复 direct_ptr | A1 | src/native/interpreter/ | small |
| A3 | A | planned | main | Step A0 GC_TRANSITION 保护 | 无 | src/native/interpreter/ | small |
| A4 | A | planned | main | CompileAndCacheEntry slot map 注销 | 无 | src/native/interpreter/ | small |
| B1 | B | planned | main | ir_reg_alloc.cpp OSR deopt backoff | 无 | src/native/interpreter/ | small |
| B2 | B | planned | main | fast_dispatch.cpp TryFastOsrPromotion | 无 | src/native/interpreter/ | small |
| C1 | C | planned | main | entry_direct.cpp direct_ptr fence | 无 | src/native/interpreter/ | small |
| C2 | C | planned | main | jit_precode.cpp direct_ptr fence | 无 | src/native/jit/ | small |
| C3 | C | planned | main | call_cache release store | 无 | src/native/interpreter/ | small |
| C4 | C | planned | main | eager_compile token 传递 | 无 | src/native/interpreter/ | small |
| D1 | D | planned | main | InterpreterEntryDirect 重构 | A, B, C | src/native/interpreter/ | large |
| D2 | D | planned | main | 回归测试 | D1 | (benchmark) | small |

## 依赖

- Phase A → 无（独立可并行执行 A1/A3/A4，A2 依赖于 A1）
- Phase B → Phase A 完成后（共享验证环境，但代码独立）
- Phase C → Phase A 完成后（共享验证环境，但代码独立）
- Phase D → Phase A + B + C（所有正确性修复先行）

## 内部依赖关系（Phase 内）

- A1 → A2（A2 使用 original_aot_ptr）
- A3, A4, B1, B2, C1-C4 均无内部依赖，可并行

## 推荐执行顺序

**串行 Phase 顺序**: A → B → C → D

**Phase A 内部**: 建议先 A1+A3+A4（可并行），再 A2（依赖 A1）

推荐启动 **A1 + A3 + A4** 并行。

## 风险

| 风险 | 可能性 | 影响 | 缓解 |
|------|--------|------|------|
| Phase D 重构引入 dispatch 回归 | 中 | 运行时行为异常 | Phase D 在 A+B+C 之后执行，基础正确性已有保障 |
| original_aot_ptr 在模块卸载后无效 | 低 | 释放后使用 | 模块卸载前所有方法已被降级或清理 |
| release fence 对 x86-64 是多余的 barrier | 高（多余但无害） | 编译时额外 barrier 指令 | 在 x86-64 上 `dmb` 指令约 10-20 周期，仅发生在 JIT 编译路径（非热点） |

## 当前建议推进顺序

Phase A (A1+A3+A4 并行) → A2 → Phase B → Phase C → Phase D

## 子任务执行策略

- `child_execution_mode`: auto
- `auto_continue`: true
- `auto_stop_policy`: blocking-only
- `dispatch_model`: sequential-by-phase (Phase 内无冲突的可并行)
- `recommended_next_child`: A1, A3, A4
