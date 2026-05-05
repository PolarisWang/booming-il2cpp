---
task_id: 20260505-interpreter-full-completion
title: Interpreter → AOT Bridge (Dispatch Callback)
task_type: plan
lifecycle_status: completed
phase: completed
created_at: 2026-05-05
updated_at: 2026-05-05
completed_at: 2026-05-05
---

# Interpreter → AOT Bridge (Dispatch Callback)

## 完成摘要

9 个 Phase 全部完成，所有测试通过。

### Phase 1-8 成果

| Phase | 内容 | 状态 |
|-------|------|------|
| 1 | IRInstruction 字段扩展 + arg_count 修复 (token_resolver.cpp) | 完成 |
| 2 | SEH 处理逻辑提取 (findCatchHandler/setupFinallyUnwind lambda) | 完成 |
| 3 | DispatchCallback 类型定义 + ExecutionFrame 扩展 (interpreter_vm.h) | 完成 |
| 4 | Call/CallVirt/CallBridge handler 改造 (interpreter_vm.cpp) | 完成 |
| 5 | InterpreterDispatch callback 实现 (runtime_instantiation.cpp) | 完成 |
| 6 | Tail Call 检测 | 完成 |
| 7 | Depth Guard (kMaxDispatchDepth = 200) | 完成 |
| 8 | 测试覆盖 (bridge test + integration test) | 完成 |
| 9 | 结构审计 + handleDispatchResult lambda 重构 | 完成 |

### 测试结果

```
interpreter-bridge=failures=0
interpreter-integration=failures=0
```

### 关键决策

- DispatchCallback 函数指针作为 interpreter ↔ runtime 的唯一契约
- DispatchResult 取代 C++ 异常穿越 callback 边界
- SEH 保留在 Execute 层（handleDispatchResult lambda）
- Tail Call + Depth Guard 组合保障栈安全

### 新增 Wiki 文档

- `wiki/03-功能模块/06-il2cpp核心架构/02-翻译路径参考/10-解释器调用分发翻译表.md`

## 架构映射

- `InterpreterVM::Execute` (interpreter 层): call 指令触发 dispatch → `DispatchCallback`
- `InterpreterDispatch` (runtime-core 层): 实现 dispatch → `MethodInvoke` (AOT/interpreted 路由)
- `LayoutEngine`: struct 返回值 size 查询
- `token_resolver`: arg_count + is_instance_call 设置

## 涉及文件

| 文件 | 变更类型 |
|------|---------|
| `src/native/interpreter/interpreter_vm.h` | 新增 DispatchResult/DispatchCallback/ExecutionFrame 字段 |
| `src/native/interpreter/interpreter_vm.cpp` | SEH lambda 提取 + call handler 改造 + handleDispatchResult 重构 |
| `src/native/runtime-core/runtime_instantiation.cpp` | InterpreterDispatch + InterpretMethodCall 改造 |
| `src/native/runtime-core/token_resolver.cpp` | arg_count / is_instance_call 设置 |
| `tests/contracts/native/interpreter/interpreter_integration.cpp` | 新增 dispatch 测试 |
| `wiki/03-功能模块/06-il2cpp核心架构/02-翻译路径参考/10-解释器调用分发翻译表.md` | 新增 wiki 文档 |
