# Tier 升级路径统一 实现计划

> **面向执行 Agent：** 使用 dev-executing-plans 执行本计划。步骤使用复选框（`- [ ]`）语法跟踪。完成后归档当前任务目录。

**目标：** 消除 TryTierUpgrade (entry_direct) 和 TryFastOsrPromotion (fast_dispatch) 之间 T1→T2 和 T2→T3 的 promotion 逻辑重复，引入共享 EvaluateTierPromotion 决策函数。

**架构：** TierManager 新增 EvaluateTierPromotion 作为所有 tier 升级 CAS+阈值的唯一判断点。TryTierUpgrade 和 TryFastOsrPromotion 各自按返回的 PromotionAction 执行对应工作。T1→T2 AllocateRegisters 在 entry 侧，T3→T4 Compile+OSR 在 FastExecute 侧，触发时序不变。

**技术栈：** C++17, atomic CAS, 函数内 enum 返回

**架构审核模式：** normal

**结构告警重点：**
- tier_manager.h 职责从"模块生命周期管理"扩展到"promotion 决策中心"
- 确保 EvaluateTierPromotion 不持有 mutex（CAS 足够，不应引入锁）
- entry_direct.cpp 和 fast_dispatch.cpp 中删除的重复 CAS 代码需要完整移除而非注释

**权责图审核主题：** n/a

**设计文档：** `docs/dev/in-progress/tier-promotion-unification/design-v1-01.md`

**问题清零来源：** brainstorm-approved STATUS.md + design-v1-01.md

**计划来源：** direct-plan

**预期知识沉淀：** n/a

**收尾约束：** 结构告警与架构审视 → 测试通过 → 归档 completed → 合并&提交

---

## 步骤

### Step 1: tier_manager.h — 新增 PromotionAction 枚举 + EvaluateTierPromotion 声明

- 在 TierManager class 的 public 区域新增 `PromotionAction` 枚举
- 声明 `static PromotionAction EvaluateTierPromotion(PatchMethod* pm, uint32_t call_count) noexcept;`

改动：`src/native/runtime-core/tier_manager.h`，~+15 行

### Step 2: tier_manager.cpp — 实现 EvaluateTierPromotion

实现完整的 6 步决策逻辑：
1. 读取 tier_state
2. T1→T2 CAS (kStackInterpreted → kRegisterLowering) → 返回 kToT2
3. T2→T3 CAS (kRegisterMapped → kOptimizeLowering) → 返回 kToT3
4. T3→T4 CAS (kOptimizedRegister → kJitted) → 返回 kToT4
5. kJitted 后读取 cached_native_method → 返回 kOsrReady / kNaredNative

关键实现要点：
- 所有 CAS 使用 acq_rel ordering
- T3→T4 时包含 codegen_fail_count backoff 计算（原 TryFastOsrPromotion 已有逻辑）
- 第 5 步只检测 cached_native_method，不调用 Compile
- 不包含 `if constexpr (kRuntimeConfig.jit)` 守卫（TierManager 编译时应该始终存在）

改动：`src/native/runtime-core/tier_manager.cpp`，~+60 行

### Step 3: entry_direct.cpp — 改造 TryTierUpgrade

替换 TryTierUpgrade 为 EvaluateTierPromotion 调用 + action dispatch：
- 调用 `auto action = TierManager::EvaluateTierPromotion(pm, tier_call_count);`
- kToT2: OptimizeToTier2 + store kRegisterMapped
- kToT3: EnqueueOptimization (TierManager::Get())
- kToT4: 编译 + 设 cached_native_method（CompileAndCache 封装）
- 其他 case: break

删除原 TryTierUpgrade 中 T1→T2 和 T2→T3 的 CAS 代码块（~40 行）。
删除原 TryTierUpgrade 中 T3→T4 的空 CAS 代码块（只有 CAS 到 kJitted 但不编译的那段）。

改动：`src/native/interpreter/interp_entry/entry_direct.cpp`，~-40 +20 行

### Step 4: fast_dispatch.cpp — 改造 TryFastOsrPromotion

替换 TryFastOsrPromotion promotion 部分为 EvaluateTierPromotion 调用：
- 调用 `auto action = TierManager::EvaluateTierPromotion(pm, call_count);`
- kToT4: CompileAndCache(pm) 兜底编译
- kOsrReady: ExecuteOSR(frame, pm->cached_native_method)
- kNaredNative: 继续 FastExecute 循环
- 删除原 T1→T2 和 T2→T3 的 CAS 代码块（~50 行）
- 保留 OSR 帧迁移逻辑

注意 T3→T4 的 Compile+OSR 逻辑原本在 TryFastOsrPromotion 中，现在 EvaluateTierPromotion 中 kToT4 返回后仍需 CompileAndCache。但由于 entry 侧（Step 3）也能触发 kToT4 编译，TryFastOsrPromotion 的 kToT4 分支是兜底路径——大多数情况下 entry 侧已经编译完成。

改动：`src/native/interpreter/fast_dispatch.cpp`，~-50 +30 行

### Step 5: 构建 & 测试

```bash
cmake --preset debug && cmake --build --target chaos_tests_jit && ctest -R jit --output-on-failure
cmake --build --target chaos_foundation_dll_test && ctest -R foundation --output-on-failure
```

关键验证点：
- 所有 JIT 测试通过（含 bench_jit_vs_aot、jit_mode_test、jit_abi_test）
- foundation-dll 枚举解析 benchmark 通过
- 行为等价：升级时序不变

### Step 6: 结构告警与架构审视

检查：
- [ ] EvaluateTierPromotion 是否持有 mutex？（不应该 — CAS 已足够）
- [ ] entry_direct.cpp 和 fast_dispatch.cpp 中删除的 CAS 代码是否完整移除（而非注释）？
- [ ] 是否引入了 tier_manager 对 interpreter 的回指依赖？（不应该 — EvaluateTierPromotion 只操作 PatchMethod* 和枚举）
- [ ] CompileAndCache 是否与 JitStubDispatchImpl 的编译路径重复？

### Step 7: 归档 & 提交

```bash
# STATUS.md 更新阶段为 completed
# git add + commit
```
