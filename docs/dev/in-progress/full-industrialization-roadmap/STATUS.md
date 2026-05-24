---
task_id: full-industrialization-roadmap
task_name: 三模式全面工业化收官
task_type: roadmap
phase: roadmap
lifecycle_status: in-progress
roadmap_or_plan: roadmap-v1-01.md
child_execution_mode: auto
auto_continue: true
auto_stop_policy: blocking-only
dispatch_model: sequential
recommended_next_child: foundation-dll-industrialization (Phase 2)
blocking_questions: []
question_clearance: cleared
clearance_confirmed_by_user: true
source: brainstorm
created: 2026-05-24
---

# 三模式全面工业化收官

## 关键文档

- [Roadmap](roadmap-v1-01.md) — 阶段定义与子任务映射

## 设计摘要（来自 brainstorm）

### 目标

将 AOT/JIT/Hybrid 三模式的工业化就绪度从当前水平全面提升至 5/5 满分。

### 边界拍板

- **Hybrid 定位**：正式生产模式，需独立工业化冲刺
- **ARM64 JIT**：推迟到下一周期，本周期不做
- **CI/CD 投入**：完整 CI/CD 体系（PR 触发 + 回归自动检测 + 基线对比 + 失败通知）
- **wf1-ci 归属**：移入 WS5（CI/CD 统一建设），JIT 只做功能完工
- **多平台支持**：结构预留，本周期不执行

### 架构决策

- **Hybrid 端到端测试**：完整三阶段（AOT→升级→JIT），验证升级前后一致性
- **Foundation-DLL 集成**：新增 stage 14（Hybrid 验证），不作参数扩展
- **CoreLib 16 failures**：硬修复，blocking=0 才能通过，无 accepted 豁免
- **AOT PHASE 5**：按完整计划推进（内联+去虚拟化+LTO+大程序集分页）

### 风险评估

- GC Liveness 首次启用暴露旧有问题风险 → 分步策略先验证再裁剪
- Linux signal handler 语义差异 → siglongjmp + 帧重建
- TLAB 内联 GC slot 注册遗漏 → safepoint 强制检查
- JIT/Hybrid 共享 jit_engine.cpp 冲突 → 串行化（WS1→WS2）
- 16 failures 逐个定位周期不确定 → 已确认硬修复策略
- PHASE 5（内联/去虚拟化）投入回报不确定 → 按完整计划推进

## 当前进度

| batch | status | tasks | 备注 |
|-------|--------|-------|------|
| phase-1 | in-progress | ws1, ws3, ws4, ws5, ws7 | 核心工业化（并行组，独立无冲突） |
| phase-2 | ready | ws2 | 依赖 phase-1 的 ws1 完成后启动 — WS1 ✅ 已完成 |

## 调度状态

```yaml
dispatch_doc: roadmap-v1-01.md
dispatch_model: sequential
active_phase: phase-2
completed_phases: [phase-1-ws1]
pending_phases: [phase-1-remaining, phase-2]
```

## 子任务映射

| task_id | phase | status | owner | purpose | depends_on | estimated_effort |
|---------|-------|--------|-------|---------|------------|-----------------|
| ws1-jit-industrialization | phase-1 | completed | main | JIT 工业化收官 ✅ 199/199 测试通过 | — | 大 |
| ws3-foundation-dll | phase-1 | ready | main | Foundation-DLL 全量验证攻坚（16 failures 硬修复） | — | 大 |
| ws4-aot-phase-4-5 | phase-1 | ready | main | AOT PHASE 4 HotUpdate 完整化 + PHASE 5 性能优化 | — | 极大 |
| ws5-cicd | phase-1 | ready | main | CI/CD 完整体系（WF 框架/fd CI/JIT 基线/AOT 门禁/Dashboard） | — | 中 |
| ws7-engineering-excellence | phase-1 | ready | main | 工程卓越度（文档/pipeline 自测/压力测试/基准回归） | — | 大 |
| ws2-hybrid-industrialization | phase-2 | completed | main | Hybrid 工业化冲刺 ✅ 41/41 测试通过 | ws1-jit-industrialization ✅ | 中 |

## 最新摘要

2026-05-24: WS2 (Hybrid 工业化冲刺) 已完成。6 个子任务全部完成：h1-core(19 tests) + h2-e2e(8 tests) + h3-race(7 tests) + h4-fd(Foundation-DLL stage 14) + h5-bench(7/7 benchmarks) + h6-doc(wiki 文档)。ws2 目录已移至 docs/dev/completed/。

已完成工作流：
- ✅ WS1 (JIT工业化收官) — 199/199 测试通过
- ✅ WS2 (Hybrid工业化冲刺) — 41/41 测试通过，wiki 文档完成

剩余工作流：
- 🔄 WS3 (Foundation-DLL 全量验证) — 已作为独立 foundation-dll-industrialization 任务在推进，Phase 2 已解除 SYS-001 阻塞，准备逐 family 攻坚
- ⏳ WS4 (AOT PHASE 4+5) — 未启动（HotUpdate 完整化 + 性能优化）
- ⏳ WS5 (CI/CD 完整体系) — 未启动
- ⏳ WS7 (工程卓越度) — 未启动

## 下一步

WS3 已存在独立任务 `foundation-dll-industrialization`，Phase 2 阻塞已解除。其余 WS 等待启动。
