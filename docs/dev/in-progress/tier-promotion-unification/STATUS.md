# Tier 升级路径统一

## 背景

TryTierUpgrade (entry_direct.cpp) 和 TryFastOsrPromotion (fast_dispatch.cpp) 各自维护了 T1→T2 和 T2→T3 的 promotion 决策逻辑，存在 CAS+阈值判断重复。T3→T4（编译+OSR）唯一在 TryFastOsrPromotion 中，不重复。

## 拍板

### 方案：共享决策层（Option C）

新增 `EvaluateTierPromotion()` 函数，含所有 tier 的 CAS 阈值判断。入口和 FastExecute 各自根据返回的 `PromotionAction` 执行对应工作。

### 边界

- 不改变触发时序（entry 和 hot backedge 都能触发升级）
- 不改变执行主体（T1→T2 AllocateRegisters 在 entry，T3→T4 Compile+OSR 在 FastExecute）
- 不引入新依赖方向

### 权威设计

详见 `design-v1-01.md`

## 变更文件

- `tier_manager.h` — 新增 PromotionAction 枚举 + EvaluateTierPromotion 声明
- `tier_manager.cpp` — EvaluateTierPromotion 实现
- `entry_direct.cpp` — TryTierUpgrade 改用 EvaluateTierPromotion，删除重复 CAS
- `fast_dispatch.cpp` — TryFastOsrPromotion 改用 EvaluateTierPromotion，删除重复 CAS

## 风险评估摘要

- T3→T4 编译在 entry 侧完成、OSR 在 FastExecute 侧：cached_native_method null 检查保证 domain unload 安全
- 先编译后 CAS：CAS 失败释放产物，不存在双重编译
- 长时间 loop 在 TryFastOsrPromotion kTransferToOsr 分支处理，无退化

## 三优先级权衡

性能最优 (P1)：共享 CAS 消除双重编译，无运行时开销
架构完美 (P2)：决策逻辑单点维护，职责清晰
HotUpdate (P3)：无影响

## blocking_questions

- [x] 长时间 loop 场景 entry 侧不触发的处理？→ TryFastOsrPromotion kCompileToNative 兜底编译 + kTransferToOsr OSR
- [x] generation 校验？→ cached_native_method null 检查已覆盖

## question_clearance

cleared

## 下一步入口

completed

## 实现摘要

- `tier_manager.h` — 新增 PromotionAction 枚举(kNoAction/kPromoteToTier2/kPromoteToTier3/kCompileToNative/kTransferToOsr/kContinueInterpreting) + EvaluateTierPromotion 声明
- `tier_manager.cpp` — 实现 EvaluateTierPromotion：6 步决策逻辑(CAS+阈值)，无锁，acq_rel ordering
- `entry_direct.cpp` — TryTierUpgrade 改用 EvaluateTierPromotion + switch dispatch，提取 CompileAndCacheEntry 本地辅助函数（保留 PIC/dispatch_ctx/aot_entry）
- `fast_dispatch.cpp` — TryFastOsrPromotion 改用 EvaluateTierPromotion + switch dispatch，提取 CompileAndCacheOsr 本地辅助函数，kTransferToOsr 分支处理 OSR 帧迁移

删除重复 CAS 代码约 90 行（entry_direct 40 行 + fast_dispatch 50 行），净增约 30 行。
