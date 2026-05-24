---
task_type: roadmap
phase: executing
roadmap_or_plan: roadmap-v1-01.md
dispatch_model: hybrid
child_execution_mode: auto
auto_continue: true
auto_stop_policy: blocking-only
recommended_next_child: wf1-arch
source: brainstorm
created: 2026-05-23
---

# JIT 工业化收官 — STATUS

> task_id: 20260523-jit-industrialization-finale

## 关键文档

- [Roadmap](roadmap-v1-01.md) — 阶段定义与子任务映射
- [DISPATCH](DISPATCH.md) — 调度批次划分

## 设计摘要（来自 brainstorm）

- **范围**：全范围，覆盖 JIT 审计识别的全部 18 个问题
- **组织策略**：5 个工作流并行，其中流1（核心管线）内部串行，流1→流3串行（共享 jit_engine.cpp）
- **架构决策**：
  - IEncoder 接口抽象（编译期选择 x64/ARM64）
  - ISehHandler 接口抽象（VEH/Signal 两平台）
  - TLAB 内联分配（纯 codegen 优化，不改分配策略）
  - 调试信息：CodeView + DWARF
- **三优先级权衡**：全部方案通过 P1>P2>P3 审查

## 当前进度

| batch | status | tasks | 备注 |
|-------|--------|-------|------|
| batch-1 | in-progress | wf1-arch → wf1-gc → wf1-liveness → wf1-osr → wf1-unwind → wf1-tests → wf1-ci | 核心管线补全（串行） |
| batch-2 | partial | wf2-linux ✅（代码完成）, wf4-debug 🔲（未启动） | wf2-linux 代码已验证完成（LinuxSehHandler.cpp 637行 + DWARF .eh_frame），仅缺 Linux CI 集成 |
| batch-3 | pending | wf3-tlab | 等待 batch-1 |
| batch-4 | pending | wf5-arm64 | 等待 batch-2 + batch-3 |

## 调度状态

```yaml
dispatch_doc: DISPATCH.md
dispatch_model: hybrid
active_batches: [batch-1]
completed_batches: []
pending_batches: [batch-3, batch-4]
# batch-2: wf2-linux 代码已完成（LinuxSehHandler.cpp + DWARF），仅缺 CI 集成
```

## 子任务映射

| task_id | phase | status | owner | purpose | depends_on | batch_id | deliverables | exit_criteria | conflict_scope | estimated_effort |
|---------|-------|--------|-------|---------|------------|----------|--------------|---------------|----------------|----------------|
| wf1-arch | 1a | ready | main | IEncoder/ISehHandler 接口抽象 + jit_engine.cpp 重构 | — | batch-1 | IEncoder.h, ISehHandler.h, jit_engine.cpp 重构 | 架构可编译，全回归通过 | src/native/jit/ | 中 |
| wf1-gc | 1b | planned | main | GC 精确槽映射 + GPR 条目 | wf1-arch | batch-1 | jit_engine.cpp, jit_method.h | GcSlotMapV0 含 GPR 条目；nm->gc_slot_map 非空 | src/native/jit/ | 大 |
| wf1-liveness | 1c | planned | main | 开启 Liveness + 验证 | wf1-gc | batch-1 | jit_engine.cpp | liveness 默认开启，GC 精确扫描通过 | src/native/jit/ | 中 |
| wf1-osr | 1d | planned | main | OSR segfault 修复 | wf1-liveness | batch-1 | jit_helpers.cpp, jit_engine.cpp | Test_OsrEntry 不 segfault | src/native/jit/ | 小 |
| wf1-unwind | 1e | planned | main | Unwind 编译守卫 | wf1-osr | batch-1 | jit_unwind.cpp | static_assert 就位 | src/native/jit/ | 极小 |
| wf1-tests | 1f | planned | main | 模块级测试套件补齐 | wf1-unwind | batch-1 | testing/jit 扩展 | 全回归通过，新测试通过 | testing/src/native/jit/ | 大 |
| wf1-ci | 1g | planned | main | CI 性能基线建立 | wf1-tests | batch-1 | CI pipeline | 基线数据入库，构建时自动对比 | .github/workflows/ | 中 |
| wf2-linux | 2 | completed | main | Linux SEH + DWARF + CI | — | batch-2 | jit_seh.cpp Linux handler, jit_unwind.cpp | Linux x64 T4 JIT 可用（代码完成，CI pending） | src/native/jit/ | 大 |
| wf3-tlab | 3 | planned | main | TLAB 内联分配 | wf1-arch, wf1-tests | batch-3 | jit_engine.cpp, jit_helpers.cpp | NewObj/Box TLAB bump path | src/native/jit/ | 大 |
| wf4-debug | 4 | planned | main | 调试信息 + SOS | — | batch-2 | jit_engine.cpp, jit_unwind.cpp | T4 代码可调试 | src/native/jit/ | 大 |
| wf5-arm64 | 5 | planned | main | ARM64 完整支持 | wf1-arch | batch-4 | arm64_encoder.h | ARM64 回归通过 | src/native/jit/ | 极大 |

## 最新摘要

Roadmap 已创建，5 个工作流已定义。当前准备启动 batch-1（核心管线补全）的第一个子任务 wf1-arch（IEncoder/ISehHandler 接口抽象 + jit_engine.cpp 重构）。

## 下一步

自动启动 wf1-arch 子任务。

## 风险评估

- Liveness 开启风险：分两步推进（全量发射→验证→开启裁剪）
- ARM64 规模风险：接受估算偏差，扩展时间线
- 合并冲突：流1→流3 共享 jit_engine.cpp 已确认串行处理
