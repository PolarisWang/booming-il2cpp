# Memory Domain 工业化补齐

> **task_id**: memory-domain-industrialization
> **创建日期**: 2026-05-23
> **基于报告**: Memory Domain 工业化品质综合评估报告（本会话内产出）

## 背景

Memory Domain 子系统核心实现质量高，但在工业化完备度上落后于 GC/反射/热更新：无 EventPipe 桥接、无压力/soak 测试、无基准测试、无 CapabilityTest proof，且存在 7 项已识别风险（R1-R7）。目标是对标其他子系统，达到完美级工业化标准。

## 设计摘要

### 边界拍板

| 维度 | 结论 |
|------|------|
| 目标水平 | 完美级工业化：EventPipe + 测试全覆盖 + benchmark + 风险清零 |
| 范围 | 6 个子任务：EventPipe 桥接、压力测试、基准测试、proof 测试、interpreter 泄漏修复、domain 修复 |
| 不覆盖 | 不重新设计架构、不改 ABI、不改 codegen 输出、不实现 ArenaHeap/PassthroughHeap |
| dispatch_model | parallel-batch |

### 三优先级权衡结论

无 P1/P2/P3 冲突。域操作在模块加载路径，非极热路径；修复类任务不影响 P1 性能。

### 子任务列表

| ID | 类型 | 负责人 | 冲突域 | 预估 |
|----|------|--------|--------|------|
| M1 | EventPipe 域诊断桥接 | terminal-1 | `src/native/diagnostics/` | 1 天 |
| M2 | 压力/Soak 测试 | terminal-2 | `testing/` | 1 天 |
| M3 | 域操作基准测试 | terminal-2 | `testing/` | 1 天 |
| M4 | Proof 测试套件 | terminal-2 | `testing/` | 1 天 |
| M5 | Interpreter 域分配泄漏修复 (R1+R3) | terminal-3 | `src/native/interpreter/` + `runtime-core/` | 2 天 |
| M6 | Domain 修复 (R2+R5+R6+R7) | terminal-3 | `src/native/runtime-core/` | 1 天 |

## blocking_questions

- [x] Q1: Proof 套件方案 — 方案 A（native GoogleTest，不建 managed xUnit）
- [x] Q2: ArenaHeap/PassthroughHeap — 方案 A（不实现，YAGNI）
- [x] Q3: R3 是否纳入 — 方案 A（纳入）
- [x] Q4: task_id — memory-domain-industrialization
- [x] Q5: R7 是否纳入 — 纳入
- [x] Q6: dispatch_model — parallel-batch

**blocking_questions: []**
**question_clearance: cleared**
**clearance_confirmed_by_user: true**

## 调度状态

```yaml
dispatch_doc: DISPATCH.md
dispatch_model: parallel-batch
active_batches: [batch-2]
completed_batches: [batch-1]
terminals_active:
  - terminal-2: M2+M3+M4 (in-progress)
pending_batches: []
completed_subtasks: [M1, M5, M6]
```
