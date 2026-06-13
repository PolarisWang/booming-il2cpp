# Execution Architecture Reform — STATUS

## Lifecycle

- **phase**: brainstorming —— cleared
- **lifecycle**: active
- **task_id**: 20260604-exec-arch-reform
- **question_clearance**: cleared
- **clearance_confirmed_by_user**: 2026-06-04

## 边界拍板

| 决策 | 结论 |
|------|------|
| 运行时 JIT 开关 | 放弃。改用 `#if CHAOS_IL2CPP_ENABLE_JIT` / `#if CHAOS_IL2CPP_ENABLE_AOT` 宏方案，编译时决策零运行时开销 |
| Test-only hotpatch | 标记为设计边界，不做支持 |
| 分层编译 | 保留 per-method tier_state 升降级，去掉全局运行时开关 |
| `ExecutionConfig` struct | 移除，替换为纯宏守卫 |
| 问题 4/6/9/10 (DP1-a 脆弱性/SEH QuickJIT/ResolveDirectFn/CallVirt mono) | wontfix / watch_item |

## Authority 决策

- Dispatch 路径优化：`fast_dispatch.cpp` + `entry_direct.cpp`
- 宏方案迁移：`execution_config.h` + 所有引用点
- Tier 调优：`tier_manager.cpp`
- Houpatch 优化：`patch_loader.cpp` + `PatchMethod` struct
- 配置治理：`CMakeLists.txt`
- **本轮新增**: JIT 代码生命周期管理（降级、GC_TRANSITION、slot map）、内存序修复、deopt 稳定性

## Phase 1 执行记录

Phase 1-4 已全部执行完毕（tasks 253-261 completed）。

## 设计方案摘要（第二轮 brainstrom — 2026-06-05）

详见 `/root/.claude/projects/-home-debian-agent-booming-il2cpp/memory/execution_flow_analysis.md`

### 工作线 A: JIT 代码生命周期修复（P0）

| 问题 | 方案 | 复杂度 |
|------|------|--------|
| A-1: Deopt 降级后 direct_ptr 残留指向已降级 JIT 代码 | PatchMethod 保存 original_aot_ptr，降级时恢复 | 低 |
| A-2: Eager JIT 代码可能通过 Step A0 绕过 GC_TRANSITION | 在 Step A0 检测 cached_native_method 并重路由到 Step A | 低 |
| A-3: GC slot map 双注册 | CompileAndCacheEntry 注册新 slot map 前注销旧的 | 低 |
| A-4: Eager JIT 无 deopt | 保持 enable_deopt=false（eager 无需 deopt），问题 A-2 的 GC_TRANSITION 保证已覆盖 | wontfix |

### 工作线 B: Deopt 稳定性（P1）

| 问题 | 方案 | 复杂度 |
|------|------|--------|
| B-1: OSR deopt 反弹循环 | OsrTrigger 中检查 deopt_count 递增，跳过 OSR 重试 | 低 |

### 工作线 C: 内存序修复（P1）

| 问题 | 方案 | 复杂度 |
|------|------|--------|
| C-1: direct_ptr 非原子缺写 fence | direct_ptr 写后加 release fence，tier_state acquire load 保证可见性 | 低 |
| C-2: call_cache T3 重建无同步 | pm->call_cache = new_cache 改为 release store | 低 |
| C-3: patch_method_token 在 eager compile 中未设置 | 修复 eager compile 路径传递 token | 低 |

### 工作线 D: 架构简化（P2）

| 问题 | 方案 | 复杂度 |
|------|------|--------|
| D-1: 三级真值源不一致导致级联分发 | 单次 tier_state load 决定所有路径，消除 Step A/A-QuickJit/A0 三路径级联 | 高 |

### wontfix / watch_item

| 问题 | 理由 |
|------|------|
| DP1-a 所有权转移脆弱性 | 当前方案已工作，增加断言够用 |
| SEH Quick JIT 全量管线 | is_tier0_ 不发射 SEH 子句表，无法绕过 |
| ResolveDirectFn JIT 蹦床展开 | PrecodeArena 生命周期长于所有方法 |
| CallVirt monomorphic shortcut | 已注释说明是有意取舍 |

## 风险评估摘要

- Phase A：低风险，修复清晰可测试，不改变正常路径行为
- Phase B：低风险，仅影响 deopt → OSR → 再次 deopt 的恶性循环
- Phase C：低风险，fence 和 release store 不影响功能只保证可见性
- Phase D：高风险，重构 dispatch 优先逻辑可能引入回归

## 三优先级权衡结论

- 性能最优：Phase C 的 fence 不增加运行时开销（编译时额外 barrier），Phase A 恢复 AOT 代码执行而非落入解释器 ✓
- 方案完美性：Phase D 消除级联分发，架构简化 ✓
- HotUpdate: Phase A 的降级恢复确保热更后 AOT 回退路径正确 ✓

---

## Roadmap 元数据

- **task_type**: roadmap
- **phase**: roadmap
- **roadmap_or_plan**: roadmap-v2-01.md
- **child_execution_mode**: auto
- **auto_continue**: true
- **auto_stop_policy**: blocking-only
- **dispatch_model**: sequential
- **recommended_next_child**: A1

## 调度状态

- active_phase: D (架构简化)
- status: COMPLETED
- completed_children: [A1, A2, A3, A4, B1, B2, C1, C2, C3, C4, D1, D2]

## 最近摘要

所有 Phase A/B/C/D 已完成。

Phase A (JIT 代码生命周期修复):
- A1: PatchMethod.original_aot_ptr 字段 + SetPatchedBySlot 保存
- A2: Deopt 降级路径恢复 entry->direct_ptr = original_aot_ptr
- A3: Step A0 GC_TRANSITION 保护 — cached_native_method != nullptr 时跳过
- A4: CompileAndCacheEntry 注册新 slot map 前注销旧的

Phase B (Deopt 稳定性):
- B1: RegisterExecute OSR deopt backoff — deopt_count 指数退避
- B2: FastExecute OSR deopt backoff — deopt_count 指数退避

Phase C (内存序修复):
- C1: QuickJitAndCacheEntry + CompileAndCacheEntry direct_ptr 写后加 release fence
- C2: TransferPrecodeOwnership 调整 direct_ptr 写时序到 tier_state release 之前
- C3: RebuildCallCacheForT3 call_cache 赋值后加 release fence
- C4: eager compile token 已正确传递 (无需修改)

Phase D (架构简化):
- D1: InterpreterEntryDirect 重构 — 单次 tier_state load (t4_tier) 驱动所有 JIT/AOT 路径
- D2: 回归测试 — full build passed

## 变更总结

| 文件 | 变更 |
|------|------|
| src/native/runtime-core/patch_loader.h | 新增 original_aot_ptr 字段 |
| src/native/runtime-core/hotpatch_table.cpp | SetPatchedBySlot 保存 original_aot_ptr |
| src/native/interpreter/interp_entry/entry_direct.cpp | A2/A3/A4 + C1/C3 + D1 重构 + deopt_count 自复位 |
| src/native/jit/jit_precode.cpp | C2: direct_ptr 写时序调整 |
| src/native/interpreter/ir_reg_alloc.cpp | B1: OSR deopt backoff |
| src/native/interpreter/fast_dispatch.cpp | B2: OSR deopt backoff |

## 第二轮变更 (2026-06-05)

### Workline 1b: deopt_count 自复位

| 问题 | 方案 | 结果 |
|------|------|------|
| deopt 后 OSR backoff 永久使用抬高的 threshold | JIT 正常返回时递减 deopt_count | 编译验证通过 |

实现: `entry_direct.cpp:749` — 在 `!deopt_happened` 路径上 `if (patch_method->deopt_count > 0) --patch_method->deopt_count;`

### Workline 2: foundation-dll benchmark 回归验证

| 项目 | 结果 |
|------|------|
| enum-parsing build | passed (56 methods lowered, 11 subjects) |
| enum-parsing fact | 11/11 passed |
| enum-parsing benchmark | passed (1 methods, 10 samples, 10000 iterations) |
| test_jit_osr | 5/5 passed |
| test_jit_gc_slot_map | 12/12 passed |
| test_jit_seh | 6/7 passed (1 pre-existing: UnregisterThenLookupReturnsNull) |
| test_jit_abi | 31/31 passed |
| smoke_test | 2/2 passed |
