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

**Roadmap 已全部完成。** A+B+C 三项全部实现并通过集成测试。

### 风险评估结果

| 风险 | 最终评估 | 结果 |
|------|---------|------|
| R1 opcode 新增破坏既有测试 | 🟢 未发生 | 16 个原有回归测试 + 10 个新增 opcode 测试全部通过 |
| R2 SEH 状态机边缘 case | 🟢 已覆盖 | 5 个 SEH 测试覆盖 throw/catch/finally/unwind/rethrow/leave |
| R3 跨帧 SEH 传播 | 🟢 已实现 | ExecutionResult.threw_exception + caller 侧传播 |
| R4 vtable_registry 集成 | 🟢 已验证 | vtable_registry_stub 自包含实现，CallVirt 2 个测试通过 |
| R5 文件冲突 | 🟢 未发生 | A+C 并行后 B 串行合并无冲突 |
| R6 wiki 更新 | 🟢 已完成 | 3 个 wiki 页面已更新 |

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
| interpreter-opcodes | batch-1 | **completed** | 实现解释器完整 opcode 覆盖 (~24 个新 opcode) | 无 | `src/native/interpreter/`, `src/native/runtime-core/il_to_ir_lowerer.*`, `tests/contracts/native/interpreter/` |
| generic-vtable-dispatch | batch-1 | **completed** | 实现泛型虚方法派发 (类继承 vtable) + CallVirt | 无 | `src/native/runtime-core/vtable_registry.*`, `src/native/runtime-core/runtime_instantiation.*` |
| seh-exception-handling | batch-2 | **completed** | 实现 SEH 纯状态机异常处理 (两阶段模型) | interpreter-opcodes | `src/native/interpreter/`, `tests/contracts/native/interpreter/` |

## 下一步

**Roadmap 已全部完成。** 进入新工作项阶段。

## 最近摘要

2026-05-04：Brainstorm 完成。A（完整 opcode 覆盖）、B（SEH 纯状态机）、C（类继承 vtable）三项拍板确认。执行顺序：并行 A+C → B。

**2026-05-05：Roadmap 全部完成。** 所有 3 个子任务已实现并通过集成测试。

### Batch 1 完成摘要

**interpreter-opcodes（24 个新 opcode）：**
- 算术：Add/Sub/Mul/Div/DivUn/Rem/RemUn/Neg
- 位运算：And/Or/Xor/Not/Shl/Shr/ShrUn
- 转换：ConvI4/ConvI8/ConvR4/ConvR8/ConvRUn/ConvI/ConvU
- 杂项：Dup/InitObj/SizeOf/LocAlloc/LdToken
- 10 个新增集成测试，16 个回归测试全部通过

**generic-vtable-dispatch（CallVirt 支持）：**
- vtable_registry_stub：自包含运行时 vtable 注册表实现
- ResolveVirtualMethodPointer：沿继承链查找虚方法
- 2 个 CallVirt 集成测试通过

### Batch 2 完成摘要

**seh-exception-handling（纯状态机 SEH）：**
- 两阶段模型：Phase 1 搜索 catch → Phase 2 展开 finally/fault → 进入 handler
- 状态变量：exception_in_flight / exception_obj / unwind 队列
- EndFinally 正确处理 exception_in_flight / pending_leave / normal 三种路径
- 5 个 SEH 集成测试：ThrowUnhandled / ThrowCatch / ThrowFinallyUnwind / LeaveFinally / Rethrow

## 调度状态

dispatch_doc: DISPATCH.md
dispatch_model: hybrid
active_batches: []
completed_batches: [batch-1, batch-2]
terminals_active: {}
pending_batches: []

## latest_stop_point

**Roadmap 已全部完成。** A+B+C 全部实现。

| task_id | phase | status | 结果 |
|---------|-------|--------|------|
| interpreter-opcodes | batch-1 | completed | 24 新 opcode + 10 测试 |
| generic-vtable-dispatch | batch-1 | completed | CallVirt + 2 测试 |
| seh-exception-handling | batch-2 | completed | SEH 两阶段 + 5 测试 |

**全部 31 个解释器集成测试通过。** wiki 已更新。

## Wiki 更新完成

以下 wiki 页面已更新：
- ✅ `wiki/03-功能模块/06-il2cpp核心架构/01-翻译管线/19-解释器VM架构.md` — 78 opcode 表、SEH 状态机、执行模型
- ✅ `wiki/03-功能模块/06-il2cpp核心架构/01-翻译管线/14-VTable注册表.md` — 解释器 CallVirt 分派、vtable_registry
- ✅ `wiki/03-功能模块/06-il2cpp核心架构/01-翻译管线/05-异常处理架构.md` — 解释器 SEH 两阶段模型

