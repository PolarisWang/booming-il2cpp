# Hotupdate 性能优化

## 当前阶段
- **phase**: brainstorming → ready for approval
- **blocking_questions**: 
  1. 执行范围：P0-P3 全部实施 ✅
  2. 执行顺序：按优先级从高到低 ✅
  3. Worktree 策略：直接在 main 上操作 ✅
  4. 验证标准：最终一次性验证 ✅
- **question_clearance**: cleared
- **clearance_confirmed_by_user**: pending

## 设计摘要

### 边界拍板
- P0-P3 全部实施，按优先级从高到低
- 直接在 main 上开发，最终一次性验证
- 所有优化走完整 dispatch 路径（不跳过 hotpatch 检查）

### 三阶段 Roadmap

**Phase 1 — 低成本高收益**
1. Tier 升级加速：降阈值 `100→10` / `500→50`（低工作量）
2. Patch 预热：ApplyPatchFromMemory 时触发 IR lowering（低工作量）
3. InterpreterEntryDirect setup 精简：合并 tier check 跳转表（中工作量）

**Phase 2 — 核心架构改造**
4. 寄存器 IR 替代栈 IR：FastExecute 支持寄存器 IR（中-高工作量）
5. 预生成桥接函数消除 method_invoke：codegen + runtime 联动（高工作量）

**Phase 3 — 功能增强**
6. DHE 差分混合执行：emit-patch-data diff + keep-native 标记（中工作量）

### 风险评估摘要
- 桥接函数保留 method_invoke fallback
- 寄存器 IR 走独立路径，不影响栈 IR
- 预热放在后台 defer 执行
- DHE 默认关闭

### 三优先级权衡结论
- 性能最优 (P1)：桥接函数 + 寄存器 IR 为最大收益点
- 架构完美 (P2)：寄存器 IR 复用现有 reg_ir_data 路径
- HotUpdate (P3)：DHE 默认关闭，不影响既有 hotupdate 流程

## 关键文档
- 设计方案: `design-v1-01.md`
- Roadmap: `roadmap-v1-01.md`

## 下一步入口
用户确认后 → 逐 Phase 进入 `writing-plans` 实现
