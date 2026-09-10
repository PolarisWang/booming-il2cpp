# RuntimeInstantiationBridgeV0 — 泛型运行时方法执行链路

## 概述

实现 RuntimeInstantiationBridgeV0 ABI 扩展（Option C），连接 MethodInvoke → LowerMethodBody → InterpreterVM 的运行时泛型方法执行链路，使运行时实例化的泛型方法能够正确执行。

## 关联

- **设计文档**: `wiki/03-功能模块/06-il2cpp核心架构/04-历史决策/17-GenericContextRegistry-B+SegmentedShard-决策.md`（含 GenericContextRegistry B+ 决策）
- **父任务**: `20260504-interpreter-opcodes-seh-vtable`（解释器基础能力 — opcodes、SEH、vtable）

## 文件变更

### contracts/native/v0/runtime_instantiation.h
- ABI 结构体 ChaosRuntimeInstantiationBridgeV0 新增 `interpret_method_call` 函数指针字段
- 参数: RuntimeState*, ThreadState*, MethodInfoHandle, void* object_instance, void* const* argv, uint32_t argc, void* out_return_value, size_t out_return_value_size, ExceptionHandle* out_exception
- 返回 RuntimeStatus (OK/NOT_FOUND/MANAGED_EXCEPTION)

### src/native/interpreter/interpreter_vm.h
- ExecutionResult 新增 `InterpreterValue exception_value = {}` 字段，支持异常传播

### src/native/interpreter/interpreter_vm.cpp
- `throw` 指令处理路径：设置 `result.exception_value = exception_obj` 后返回

### src/native/runtime-core/token_resolver.h
- TokenResolverContext 新增 `method_type_args`/`method_arg_count` 字段

### src/native/runtime-core/token_resolver.cpp
- DefaultTokenResolver 新增 ELEMENT_TYPE_VAR (0x11xxxxxx) 检测：通过 `type_args[index]` 解析
- DefaultTokenResolver 新增 ELEMENT_TYPE_MVAR (0x12xxxxxx) 检测：通过 `method_type_args[index]` 解析

### src/native/runtime-core/generic_method_instantiation.h
- RuntimeInstantiatedMethod 新增 `bool is_unloaded = false` 卸载保护

### src/native/runtime-core/runtime_instantiation.cpp
- 新增 `InterpretMethodCall()` 桥接函数（~95行）
  - container_of 恢复 RuntimeInstantiatedMethod*
  - is_unloaded 检查 → NOT_FOUND
  - 惰性 LowerMethodBody（IL 可用时）
  - ExecutionFrame 构建 + InterpreterVM::Execute
  - 异常传播：throw ManagedExceptionCarrier
  - return value tag dispatch（Int32/Int64/Float32/Float64/ObjectRef/Null/Struct）
  - interpreted_method_call_count 统计
- 新增 `s_runtime_methods`/`s_runtime_methods_mutex` RuntimeInstantiatedMethod 跟踪
- UnregisterModuleGenerics: 标记 is_unloaded = true 并从跟踪中移除
- g_bridge 新增 `.interpret_method_call = InterpretMethodCall`

### src/native/runtime-core/runtime_core.cpp
- MethodInvoke 回退：FindInvokerPointer 返回 nullptr 且 method_token >= 0x80000000 时调用 bridge->interpret_method_call

### src/native/runtime-core/vtable_registry_stub.cpp
- LayoutEngine stub: `GetOrComputeLayout` 返回 nullptr（测试用）

### tests/contracts/native/interpreter/interpreter_integration.cpp
- 4 个新测试函数：
  1. TestRuntimeMethodExecute — ldc.i4.s + ldarg.0 + ldarg.1 + add + ret → 结果=30 PASSED
  2. TestRuntimeMethodReturnValueDispatch — ldc.i4.s 42 + ret → tag dispatch 提取=42 PASSED
  3. TestRuntimeMethodTypeParamResolution — ldarg.0 + box 0x11000000 + ret → call_target=0xDEADBEEF PASSED
  4. TestRuntimeMethodExceptionPropagation — push 99 + throw → threw_exception + exception_value=99 PASSED

### _build_and_run.bat
- 新增 runtime_instantiation.cpp 和 token_resolver.cpp 到编译源列表

### wiki/03-功能模块/06-il2cpp核心架构/04-历史决策/
- 17-GenericContextRegistry-B+SegmentedShard-决策.md

## 架构约束

- chaos_runtime_core 禁止依赖 {common, support, interpreter, hot-update, engine-bridge} 模块
- RuntimeCore → Bridge ABI 函数指针（无直接 include）满足模块分层约束
- Token range isolation: AOT < 0x80000000, runtime >= 0x80000000
- V0 限制：所有 argv 按 int32 处理（V1+ 需要 signature-aware marshalling）
- V0 限制：ManagedExceptionCarrier 抛出 null handle（V1+ 需要 interpreter-to-runtime exception bridge）

## 验证

- 全部 35 个测试通过（31 个原有 + 4 个新增）
- 构建命令: `_build_and_run.bat`

## 生命周期

- **lifecycle_status**: completed
- **phase**: completed
- **updated_at**: 2026-05-05
- **completed_at**: 2026-05-05
