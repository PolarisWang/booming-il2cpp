---
task_type: roadmap
phase: roadmap
roadmap_or_plan: roadmap-v1-01.md
dispatch_model: sequential
child_execution_mode: auto
auto_continue: true
auto_stop_policy: blocking-only
recommended_next_child: p6-simd
source: brainstorm
created: 2026-05-24
blocking_questions: []
question_clearance: cleared
clearance_confirmed_by_user: true
---

# JIT 性能优化路线 — STATUS

> task_id: 20260524-jit-perf-optimization

## 关键文档
- [Roadmap](roadmap-v1-01.md) — 阶段定义与子任务映射
- [设计文档](design-v1-01.md) — P0-P6 架构决策

## 设计摘要

从当前 JIT 引擎（真 x64 代码生成器，但无内联/无 CSE/无边界消除）出发，逐步达到 CoreCLR JIT 级别性能，同时保持 hot-update 安全。

**核心决策：**
- **D1 调用间接化**：`call [rip+off]` slot 间接 + ReverseSlotMap
- **D2 版本失效**：HotpatchEntryV0 加 version + caller tracking + 内联失效
- **D3 优化 IR**：单 BB 表达式树 + VN + CSE + 线性化器
- **D4 Pass 编排**：函数式，预留 PassManager 迁移

**7 个阶段：** P0 Slot 间接化(3-4w) → P1 树 IR(4-5w) → P2 内联器(3-4w) → P3 Intrinsic(2-3w) → P4 边界消除(2-3w) → P5 循环优化(3-4w) → P6 SIMD(4-6w)

## 子任务映射

| task_id | phase | status | owner | purpose | depends_on | estimated_effort |
|---------|-------|--------|-------|---------|------------|-------------------|
| p0-call-site-slot | P0 | completed | main | Slot 间接化 + Version + ReverseSlotMap | — | 3-4w |
| p1-tree-ir | P1 | completed | main | 轻量树 IR 框架 + VN + CSE | p0-call-site-slot | 4-5w |
| p2-inliner | P2 | completed | main | 热更感知内联器 | p1-tree-ir | 3-4w |
| p3-intrinsic | P3 | completed | main | Intrinsic 识别 | p2-inliner | 2-3w |
| p4-bounds-check | P4 | completed | main | 边界检查消除 | p1-tree-ir | 2-3w |
| p5-loop-opt | P5 | completed | main | 循环优化 | p4-bounds-check | 3-4w |
| p6-simd | P6 | planned | main | SIMD / HW intrinsics | p3-intrinsic + p4-bounds-check | 4-6w |

## 边界拍板
- 不做完整 GenTree 体系，只做单 BB 表达式树
- 不做全局值编号
- 不与已有 JIT 工业化 roadmap（20260523-jit-industrialization-finale）冲突

## 风险评估
- P0 slot 表内存布局（RX vs RW 独立页面）
- P1 树→线性化语义等价性（空管线回归基线）
- P2 SEH 子句合并复杂度
- P6 SIMD 对齐 crash

## 最新摘要

- P5 循环优化已完成：CFG + Lengauer-Tarjan 支配树 + 自然循环检测 + LICM(后处理) + IV(检测) + Unrolling(分析框架)
- P1/P2/P3/P4/P5 均已归档
- 当前最新完成阶段：P5（循环优化）
- 阶段完成时间：2026-05-24

## 下一步

由于当前测试 subjects (convert-char) 全部为单 BB，P5 的循环 pass 在现有测试中不会触发。剩余 P6 SIMD 需要 P2 内联器 + P3 Intrinsic + P4 边界消除作为前置依赖（均已就绪）。

下一推荐子任务：p6-simd（SIMD / HW intrinsics）。
建议启动 P6 SIMD 子任务（Vector128/256 生成 + SSE/AVX 指令）。
