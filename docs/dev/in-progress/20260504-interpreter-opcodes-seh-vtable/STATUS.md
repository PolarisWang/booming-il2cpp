---
task_id: 20260504-interpreter-opcodes-seh-vtable
title: 解释器完整 Opcode 覆盖 + SEH 异常处理 + 泛型虚方法派发
task_type: roadmap
lifecycle_status: roadmap
phase: roadmap
created_at: 2026-05-04 22:00:00 +08:00
updated_at: 2026-05-04 22:00:00 +08:00

child_execution_mode: auto
auto_continue: true
auto_stop_policy: blocking-only
dispatch_model: hybrid

blocking_questions: []
question_clearance: cleared
clearance_confirmed_by_user: true
clearance_source: brainstorming-approval

roadmap_or_plan: roadmap-v1-01.md

key_documents:
  - roadmap-v1-01.md (阶段拆分与子任务映射)
  - DISPATCH.md (并行批次调度)
---

## 设计摘要

基于 brainstorm 的三项决策，覆盖解释器 + SEH + 虚方法派发三个独立工作项。

### 边界拍板

1. **范围**：A（解释器完整 opcode 覆盖）+ B（SEH 异常处理纯状态机）+ C（泛型虚方法派发类继承 vtable）
2. **A 方案**：方案2（完整覆盖 ~20-30 opcodes，不含 overflow-check 延迟变体）
3. **B 方案**：方案A（纯状态机，两阶段模型：搜索→展开→处理）
4. **C 方案**：方案A（类继承 vtable，不含 interface dispatch）
5. **执行顺序**：Batch 1 并行 A+C → Batch 2 B（SEH）

### Authority 决策

| 决策项 | 决策 | 依据 |
|--------|------|------|
| A opcode 集范围 | 完整覆盖所有常用 opcodes，跳过 overflow 变体 (add.ovf 等) | YAGNI + 后续可增量添加 |
| B SEH 架构 | 纯状态机扩展 InterpreterVM::Execute() | P1 性能最优 + P3 热更友好 + 业内先例 (SSCLI/Mono) |
| B 跨帧传播 | ExecutionResult.has_exception + caller 侧递归搜索 | 复用 needs_external_dispatch 模式，一致性强 |
| C vtable 策略 | 复用现有 vtable_registry (token-based)，不新建接口 dispatch | C 范围限定为类继承链，interface 后续独立实现 |
| 并行策略 | Batch 1: A + C 并行（无文件冲突）；Batch 2: B 串行 | A 与 B 共享 interpreter_vm.cpp，需串行 |

### 当前结论

三项边界与方案均已拍板。执行前问题已清零。进入 roadmap 分流。

### 风险评估摘要

| 风险 | 评估 | 缓解策略 |
|------|------|---------|
| R1 opcode 新增破坏既有测试 | 🟢 低 | 已有 16 个集成测试作为回归门禁 |
| R2 SEH 状态机复杂度导致边缘 case 遗漏 | 🟡 中 | 两阶段模型 + 增量 TDD：先 catch/finally → 后 fault/filter |
| R3 跨帧 SEH 传播与外部派发交互 | 🟡 中 | ExecutionResult.has_exception 字段 + caller 侧递归搜索 |
| R4 vtable_registry 与泛型实例化的集成 | 🟢 低 | 已通过 Phase 5 验证，ResolveVirtualMethodPointer 已可用 |
| R5 并行批次文件冲突 | 🟢 低 | A 与 C 无文件重叠，conflict_scope 校验通过 |
| R6 完成后 wiki 更新遗漏 | 🟢 低 | 强制收尾链路包含 wiki 更新步骤 |

### 三优先级权衡结论

| 优先级 | 评价 |
|--------|------|
| P1 性能最优 | 🟢 SEH 纯状态机零 C++ EH 开销；opcodes 使用已有执行模式；vtable O(depth) 搜索在解释器场景可接受 |
| P2 方案完美性 | 🟢 SEH 两阶段模型覆盖完整 ECMA 语义；opcode 增量 TDD 保证正确性 |
| P3 HotUpdate 支持 | 🟢 SEH clauses 是 method 数据，热更 DLL 直接使用；vtable_registry 支持运行时注册 |

### 关键文档

- `roadmap-v1-01.md`：阶段拆分、子任务映射、依赖关系
- `DISPATCH.md`：并行批次调度策略

## 子任务映射

| task_id | phase | status | purpose | depends_on | conflict_scope |
|---------|-------|--------|---------|------------|----------------|
| interpreter-opcodes | batch-1 | ready | 实现解释器完整 opcode 覆盖 | 无 | `src/native/interpreter/`, `src/native/runtime-core/il_to_ir_lowerer.*`, `tests/contracts/native/interpreter/` |
| generic-vtable-dispatch | batch-1 | ready | 实现泛型虚方法派发 (类继承 vtable) | 无 | `src/native/runtime-core/vtable_registry.*`, `src/native/runtime-core/runtime_instantiation.*` |
| seh-exception-handling | batch-2 | planned | 实现 SEH 纯状态机异常处理 | interpreter-opcodes | `src/native/interpreter/`, `tests/contracts/native/interpreter/` |

## 下一步

创建 roadmap-v1-01.md 和 DISPATCH.md。Batch 1 两个子任务并行启动。

## 最近摘要

2026-05-04：Brainstorm 完成。A（完整 opcode 覆盖）、B（SEH 纯状态机）、C（类继承 vtable）三项拍板确认。执行顺序：并行 A+C → B。

## 调度状态

dispatch_doc: DISPATCH.md
dispatch_model: hybrid
active_batches: []
completed_batches: []
terminals_active: {}
pending_batches: [batch-1, batch-2]

## latest_stop_point

Brainstorm 完成，等待 roadmap 创建。

## Wiki 更新计划

全部子任务完成后，更新以下 wiki 页面：
- `wiki/03-功能模块/06-il2cpp核心架构/01-翻译管线/19-解释器VM架构.md` — 完整 opcode 表、SEH 状态机架构、执行模型
- `wiki/03-功能模块/06-il2cpp核心架构/01-翻译管线/14-VTable注册表.md` — 添加运行时 vtable_registry 说明

