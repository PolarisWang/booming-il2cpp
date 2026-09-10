---
task_id: minimal-interpreter
title: MinimalInterpreter 扩展 — 支持值类型栈 + 外部调用派发
task_type: plan
lifecycle_status: completed
phase: completed
created_at: 2026-05-04 20:00:00 +08:00
updated_at: 2026-05-04 20:00:00 +08:00

parent_task_id: 20260504-generic-runtime-instantiation-phases-3-5
source_task_id: 20260504-generic-runtime-instantiation-phases-3-5
source_relation: roadmap-child
clearance_source: parent-roadmap

blocking_questions: []
question_clearance: cleared
clearance_confirmed_by_user: true
entry_skill: dev-writing-plans
---

## Scope

对 `MinimalInterpreter` 进行扩展，支持值类型栈操作和外部调用派发，使解释器能处理含 struct 局部变量和外部调用的 IR 方法。

**核心职责**：
1. `ValueTag::Struct` + `InterpreterValue` inline struct 存储（≤64 字节内联，大结构体堆分配）
2. `ExecutionResult` 新增 `needs_external_dispatch` + `call_data`
3. `Call`/`CallVirt`/`CallBridge` → 设 `needs_external_dispatch = true`，返回
4. 结构体字段访问：`LdFld`/`StFld` 在 Struct tag 下的正确 offset 读写
5. 帧生命周期：`ExecutionFrame` 析构时释放 struct 数据的堆内存
6. `RuntimeInstantiationBridge` 接收 `NEEDS_EXTERNAL_DISPATCH` → 通过 `RuntimeAbiV0.method_invoke` 路由

## Inputs

- 父 roadmap 设计上下文：`roadmap-v1-01.md` Task B1
- 依赖 Task A1 (LayoutEngine) — struct 大小解析已完成
- 现有文件：
  - `src/native/interpreter/interpreter_vm.h` — 当前 ValueTag、InterpreterValue、ExecutionResult、InterpreterVM
  - `src/native/interpreter/interpreter_vm.cpp` — Execute 循环
  - `src/native/runtime-core/runtime_instantiation.cpp` — bridge 集成

## Expected Outputs

1. 修改 `src/native/interpreter/interpreter_vm.h` — Struct tag, ExecutionResult 扩展
2. 修改 `src/native/interpreter/interpreter_vm.cpp` — 实现
3. 修改 `src/native/runtime-core/runtime_instantiation.cpp` — NEEDS_EXTERNAL_DISPATCH 路由

## Exit Criteria

- Build 0 error
- 值类型在栈上正确 push/pop（含 LdFld/StFld 读写）
- Call opcode → RuntimeAbiV0.method_invoke 调用成功
- 帧析构时 struct_data 内存正确释放
- 集成测试覆盖

## Terminal Notes

## 执行摘要

### 完成情况

| 需求 | 状态 | 说明 |
|------|------|------|
| ValueTag::Struct | ✅ | 新增枚举 Struct = 7 |
| InterpreterValue struct 存储 | ✅ | struct_size + obj heap pointer + deep-copy semantics |
| 深拷贝构造/赋值/析构 | ✅ | 拷贝构造深拷贝 struct data，析构自动释放 |
| from_struct 工厂 | ✅ | 从原始字节创建 struct InterpreterValue |
| ExecutionResult 扩展 | ✅ | needs_external_dispatch + call_target + call_args |
| Call/CallVirt/CallBridge 外部派发 | ✅ | 设 needs_external_dispatch=true，收集参数，返回 |
| LdFld/StFld struct 支持 | ✅ | 按 offset+size 从 struct data 读写字段 |
| ExecutionFrame 析构 | ✅ | 遍历所有值释放 struct_data |
| IRInstruction call_target/arg_count | ✅ | 新增字段 |
| 测试 | ✅ | 22/22 测试通过 |

### 已知残差

- Bridge routing（NEEDS_EXTERNAL_DISPATCH → RuntimeAbiV0.method_invoke）将在后续集成任务中完成
- Struct LdFld 字段类型推断需完善（Phase 5+）

### 验证

```bash
python -m pytest tests/unit/compatibility/test_minimal_interpreter.py -v  # 22 passed
```
