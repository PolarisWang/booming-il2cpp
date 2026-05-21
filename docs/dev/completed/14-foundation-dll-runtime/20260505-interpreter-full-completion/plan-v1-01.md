# Interpreter → AOT Bridge (Dispatch Callback) 实现计划

> **面向执行 Agent：** 必须使用 `dev:executing-plans` 或 `dev:subagent-driven-development` 来执行本计划。步骤使用复选框（`- [ ]`）语法跟踪。

**目标：** 实现 `InterpreterVM::Execute` 到 AOT 编译方法的完整调用链路，消除解释器中 `call`/`callvirt` 指令遇到的 "requires external bridge" 静默丢弃问题。

**架构：** 通过 `DispatchCallback` 函数指针将 call dispatch 从解释器循环外部化到 runtime_instantiation 层。Execute 遇到 `call` 时不再 `return`，而是同步调用 callback 完成 dispatch、push 返回值、继续循环。异常通过 `DispatchResult` 结构体（而非 C++ 异常）穿越 callback 边界，由 Execute 内部的 SEH 机制处理。

**技术栈：** C++17，依赖现有的 `MethodInvoke`（runtime_core.cpp）做最终 AOT/interpreted 路由，`FindInvokerPointer` 解析 method token，`LayoutEngine` 处理 struct 返回值。

**架构审核模式：** critical (IL2CPP 主线)

**结构告警重点：** SEH 处理逻辑提取（避免重复代码）、DispatchCallback 接口设计（避免过于泛化）、IRInstruction 字段增减（避免膨胀）

**权责图审核主题：** `InterpreterVM::Execute` 的职责边界 —— 它应该知道"如何 dispatch"还是仅仅"触发 dispatch"？本计划选择 "触发 dispatch"，授权给 runtime_instantiation 层。

**AOT/IL2CPP/Test Governance Intake：**
- capabilityFamily: interpreter
- capabilityItem: cross-boundary-call-dispatch
- ownerSubjectId: 20260505-interpreter-full-completion
- proofRequired: true
- benchmarkRequired: false
- hotupdateImpact: Proof（需要测试 hotupdate DLL 中的方法 dispatch）
- formalVerificationObjects: [interpreter_bridge_test.cpp 中的现有测试 + 新增 recursive/nested-call 测试]
- requiredGates: collector -> registry -> workspace

**设计文档：** 本次分析内容（未创建独立 design-v1-01.md，分析摘要在 STATUS.md 中记录）

**问题清零来源：** brainstorm-approved STATUS.md (2026-05-05 的遍历分析 + 用户确认)

**计划来源：** direct-plan（来源于 brainstorm 阶段的分析输出，问题已清零）

**预期知识沉淀：** n/a（实现完成后更新 wiki 翻译路径参考）

**收尾约束：** 执行完成后必须进入 "结构告警与架构审视 -> 测试通过 -> 归档 completed -> 合并&提交" 固定链路。

---

## 关键设计决策

| 决策 | 选择 | 理由 |
|------|------|------|
| dispatch 位置 | Execute 循环内同步 callback | 避免 resumable frame 的复杂度和性能损失 |
| 异常边界 | DispatchResult 返回码（非 C++ 异常） | 避免 C++ 异常穿透 callback 函数指针边界 |
| SEH 共享 | Lambda 捕获（3A），后续不重构为 SEHState | 最小变更原则，降低回归风险 |
| Instance 识别 | IRInstruction.is_instance_call（1A） | 一次 lowering 设置，dispatch 零开销 |
| Struct 返回值 | Dispatch 时 LayoutEngine 查询（2A） | Generic 场景准确，Struct 返回占比低 |
| 栈安全 | Tail Call 检测(4A) + Depth Guard(4B) | 性价比最高，不引入大规模架构变更 |

---

## 文件清单与职责

### 1. `src/native/interpreter/interpreter_vm.h` — 类型定义层

```
职责: DispatchCallback 类型、DispatchResult 结构体、ExecutionFrame 新增字段
变更:
  + DispatchResult 结构体 (has_value/value/threw_exception/exception_value)
  + DispatchCallback 函数指针类型
  + ExecutionFrame::dispatch_fn 字段 (DispatchCallback*)
  + ExecutionFrame::dispatch_context 字段 (void*)
  + IRInstruction::is_instance_call 字段 (bool)
```

### 2. `src/native/interpreter/interpreter_vm.cpp` — 核心执行层

```
职责: SEH lambda 提取、Call/CallVirt/CallBridge handler 改造
变更:
  + findCatchHandler lambda (从 Throw case 提取)
  + setupFinallyUnwind lambda (从 Throw case 提取)  
  * Call/CallBridge handler: 新增 callback 分支
  * CallVirt handler: 新增 callback 分支
  * CallVirtConstrained handler: 新增 callback 分支
  ~ Throw handler: 改为调用提取后的 lambda
```

### 3. `src/native/runtime-core/token_resolver.cpp` — Token 解析层

```
职责: arg_count 修复、is_instance_call 设置
变更:
  * DefaultTokenResolver::IROpCode::Call case: 
    + 解析 method descriptor 获取 parameter_count
    + 设置 instruction.arg_count
    + 设置 instruction.is_instance_call
  * DefaultTokenResolver::IROpCode::CallVirt/CallVirtConstrained case:
    + 设置 arg_count = parameter_count + 1 (含 this)
```

### 4. `src/native/runtime-core/runtime_instantiation.cpp` — Dispatch 实现层

```
职责: InterpreterDispatch callback 实现、InterpretMethodCall 改造
变更:
  + InterpreterDispatchContext 结构体 (runtime_state/thread_state/arg_buffer)
  + InterpreterDispatch 函数 (完整 dispatch 逻辑)
  * InterpretMethodCall: 构建 InterpreterDispatchContext
  * InterpretMethodCall: 设置 frame.dispatch_fn + frame.dispatch_context
  * InterpretMethodCall: 移除 vm.Execute() 后的 needs_external_dispatch 检查（不再需要）
```

### 5. `tests/contracts/native/interpreter/interpreter_bridge_test.cpp` — 测试层

```
职责: 新增非 leaf 方法测试
变更:
  + TestBridgeCallOtherMethod: 解释器方法 A 调用解释器方法 B
  + TestBridgeCallAotMethod: 解释器方法调 AOT 方法（需 mock）
  + TestBridgeExceptionInCallee: 被调方法抛异常，调用者 catch
  + TestBridgeDeepCallChain: 深度调用链 (depth 10-20)
  + TestBridgeTailCall: 尾调用优化验证
```

---

## 执行任务

### Phase 1：IRInstruction 字段扩展 + arg_count 修复

**文件：** `token_resolver.cpp` + `interpreter_vm.h`

```
期望：Call/CallVirt 指令的 arg_count 和 is_instance_call 被正确设置
验证：在 lowering 后打印 IR instruction 确认字段值
```

- [ ] `interpreter_vm.h`: IRInstruction 新增 `bool is_instance_call = false`
- [ ] `token_resolver.cpp` DefaultTokenResolver::`IROpCode::Call` case:
  - 解析 method handle → TryDecodeReflectionQueryMethodHandle
  - 读取 parameter_count
  - 判断 instance 方法（从 calling convention / signature 读取）
  - 设置 `instruction.arg_count = param_count + (instance ? 1 : 0)`
  - 设置 `instruction.is_instance_call`
- [ ] `token_resolver.cpp` DefaultTokenResolver::`IROpCode::CallVirt` / `CallVirtConstrained` case:
  - 设置 `instruction.arg_count = param_count + 1`
  - `is_instance_call = true`（CallVirt 永远是实例方法）
- [ ] 编译验证：`msbuild chaos_interpreter` / `cmake --build` 通过

### Phase 2：SEH 处理逻辑提取

**文件：** `interpreter_vm.cpp`

```
期望：Throw handler 的 catch-finding + finally-unwind 逻辑可被 Call handler 复用
验证：现有 Throw 测试全部通过（chaos_interpreter_bridge_test）
```

- [ ] 在 `Execute` 函数顶部，定义 lambda `findCatchHandler`：
  - 输入：`instruction_index`, `exception_obj`
  - 输出：catch clause index（-1 = 未找到）
  - 逻辑：从当前的 `Throw handler Phase 1` 提取
- [ ] 在 `Execute` 函数顶部，定义 lambda `setupFinallyUnwind`：
  - 输入：`catch_idx`, `instruction_index`
  - 逻辑：填充 `unwind_finally_list`, `unwind_finally_count`, `exception_in_flight`
  - 从 `Throw handler Phase 2` 提取
- [ ] 重构 `case IROpCode::Throw:`:
  - 调用 `findCatchHandler` 替代内联逻辑
  - 调用 `setupFinallyUnwind` 替代内联逻辑
  - 其余逻辑（跳转到 handler / return threw_exception）保持不变
- [ ] 编译 + 运行现有测试确认无 regression

### Phase 3：DispatchCallback 类型定义 + ExecutionFrame 扩展

**文件：** `interpreter_vm.h`

```
期望：类型定义完整，ExecutionFrame 支持 callback 通道
验证：编译通过
```

- [ ] 定义 `DispatchResult` 结构体：
```cpp
struct DispatchResult {
    bool has_value = false;
    InterpreterValue value = {};
    bool threw_exception = false;
    InterpreterValue exception_value = {};
};
```
- [ ] 定义 `DispatchCallback` 函数指针类型：
```cpp
using DispatchCallback = auto (*)(
    void* call_target,
    const InterpreterValue* call_args,
    CHAOS_IL2CPP_UINT32 arg_count,
    bool is_instance_call,
    void* dispatch_context) -> DispatchResult;
```
- [ ] `ExecutionFrame` 新增字段：
```cpp
DispatchCallback dispatch_fn = nullptr;
void* dispatch_context = nullptr;
```
- [ ] 编译验证通过

### Phase 4：Call/CallVirt/CallBridge handler 改造

**文件：** `interpreter_vm.cpp`

```
期望：所有 call 类指令支持 dispatch callback 路径
验证：新编写的小型 dispatcher 能成功接收参数并返回
```

- [ ] `IROpCode::Call` / `IROpCode::CallBridge` handler 新增 callback 分支：
  - 检查 `frame->dispatch_fn != nullptr`
  - 收集 call_args（使用 `kMaxCallArgs` 栈缓冲 + heap fallback）
  - 调用 `frame->dispatch_fn(call_target, arg_buf, arg_count, is_instance_call, ctx)`
  - 检查 `dret.threw_exception`:
    - true → 调用 `findCatchHandler` + `setupFinallyUnwind` 做 SEH 处理
    - 没找到 handler → `result.threw_exception = true; return result;`
  - `dret.has_value` → push 到 stack
  - `break` 继续循环
  - 保留 `dispatch_fn == nullptr` 时原有 `needs_external_dispatch` fallback
- [ ] `IROpCode::CallVirt` handler 同样改造（注意 vtable 解析在前，callback 在后）
- [ ] `IROpCode::CallVirtConstrained` 同样改造
- [ ] 编译验证通过

### Phase 5：InterpreterDispatch callback 实现

**文件：** `runtime_instantiation.cpp`

```
期望：dispatch callback 能正确路由到 AOT 或 interpreted 方法并返回结果
验证：interpreter_bridge_test 中新增的 call-other-method 测试通过
```

- [ ] 定义 `InterpreterDispatchContext` 结构体：
```cpp
struct InterpreterDispatchContext {
    RuntimeState* runtime_state;
    ThreadState* thread_state;
    // ABI 参数缓冲区（最多 8 个参数 + this = 9 指针）
    void* abi_buffer[9];
};
```

- [ ] 实现 `InterpreterDispatch` 函数：
  - 参数：`call_target`, `call_args`, `arg_count`, `is_instance_call`, `dispatch_context`
  - 步骤：
    1. `call_target` 转为 `MethodInfoHandle`
    2. 分离 this（`is_instance_call` 时 `call_args[0]` 是 this）
    3. 构建 `void* argv[]`：每个 `InterpreterValue` 根据 tag 写入对应数据类型
    4. 调 `MethodInvoke(runtime_state, thread_state, handle, this_obj, argv, param_count, &ret_raw, sizeof(ret_raw), &ex)`
    5. 如果 `status == CHAOS_RUNTIME_STATUS_MANAGED_EXCEPTION`：
       - 设置 `result.threw_exception = true`
       - `exception_value` 包装 `ex`
    6. 如果正常返回：
       - 解析 `result.member_type_utf8` → `MapMemberTypeToValueTag`
       - 根据 tag 转换返回值
       - Struct 类型：调 LayoutEngine 获取 size，做 `from_struct`
    7. 返回 `DispatchResult`

- [ ] 改造 `InterpretMethodCall`：
  - 创建 `InterpreterDispatchContext`（使用函数参数的 `runtime_state`/`thread_state`）
  - 设置 `frame.dispatch_fn = InterpreterDispatch`
  - 设置 `frame.dispatch_context = &ctx`
  - 调用 `vm.Execute()`（不再需要 `needs_external_dispatch` 循环）
  - 保持现有的异常处理和返回值提取逻辑不变

- [ ] 编译验证通过

### Phase 6：Tail Call 检测

**文件：** `interpreter_vm.cpp`

```
期望：call 后紧跟着 ret 时，跳过 push+pop 直接透传返回值
验证：新增的 tail call 测试验证返回值正确
```

- [ ] 在 Call handler 的 callback 分支中：
  - 检查 `instruction_index + 1 < method.instructions.size()`
  - 检查 `method.instructions[instruction_index + 1].op_code == IROpCode::Ret`
  - 如果是 tail call：
    - 调 callback 获取 `DispatchResult`
    - 直接设置为 `result.return_value`，`return result`（跳过 push/pop）
- [ ] 编译验证通过

### Phase 7：Depth Guard

**文件：** `runtime_instantiation.cpp` + `runtime_core.h`

```
期望：递归深度超过阈值时返回明确的错误状态
验证：深度过大的调用链返回 CHAOS_RUNTIME_STATUS_STACK_OVERFLOW
```

- [ ] `InterpreterDispatchContext` 增加 `int recursion_depth` 字段
- [ ] `InterpreterDispatch` 入口 `++ctx->recursion_depth`
- [ ] 检查 `recursion_depth > kMaxDepth (200)`：
  - 超限返回 `DispatchResult::StackOverflow()`（或等价信号）
- [ ] `InterpreterDispatch` 出口 `--ctx->recursion_depth`
- [ ] 编译验证通过

### Phase 8：测试 + 验证

**文件：** `tests/contracts/native/interpreter/interpreter_bridge_test.cpp`

```
期望：所有新增测试通过
验证：chaos_interpreter_bridge_test.exe 输出 failures=0
```

- [ ] `TestBridgeCallOtherMethod`：
  - 方法 A: `ldc.i4.1, ldc.i4.2, call B, ret` → 期望 3
  - 方法 B: `ldarg.0, ldarg.1, add, ret`
- [ ] `TestBridgeCallChain`：
  - 方法 A → B → C，深度 3 层
- [ ] `TestBridgeExceptionInCallee`：
  - 方法 A: `call B, pop, ldc.i4.1, ret`（B 抛异常，A 有 catch handler 捕获 → 返回 42）
  - 需要 SEH try/catch 在 IL 中表达（通过模拟 SEH 子句）
- [ ] `TestBridgeTailCall`：
  - 方法 A: `ldc.i4 42, call B, ret`（尾调用）
  - 方法 B: `ldarg.0, ldc.i4.1, add, ret` → 期望 43
- [ ] 运行所有现有测试确认无 regression

### Phase 9：结构告警与架构审视

```
期望：确认无重复代码、抽象过度、职责污染
验证：代码审查
```

- [ ] 审视 `interpreter_vm.cpp`：
  - Call/CallVirt/CallBridge 三个 handler 的 callback 分支是否有重复？
  - 如果有 → 提取公共助手函数
- [ ] 审视 `runtime_instantiation.cpp`：
  - DispatchCallback 中的 ABI marshalling 是否与 `InterpretMethodCall` 入口处的 marshalling 有重复？
  - 如果有 → 提取公共的 `InterpreterValueToAbi` / `AbiToInterpreterValue` 函数
- [ ] 审视 `DispatchResult` 是否与 `ExecutionResult` 有概念重叠
- [ ] 审视异常边界：`DispatchResult::threw_exception` 路径是否覆盖了全部 SEH 场景

---

## 风险与缓解

| 风险 | 概率 | 影响 | 缓解 |
|------|------|------|------|
| SEH lambda 提取导致 Throw 回归 | 低 | 中 | Phase 2 后运行全部测试确认无 regression |
| `arg_count` 修复后现有 lowering 行为变化 | 中 | 中 | lowering 不产生语义变化，但 `arg_count` 从 0 变为真实值；检查所有使用 `arg_count` 的代码点 |
| Struct 返回值 LayoutEngine 查询为空 | 低 | 低 | 回退到 V0 的 void* → int32 行为 |
| Dispatch callback 尾递归在 `call brtrue/brfalse` 后无法优化 | 中 | 低 | Tail Call 只覆盖最简单的 `call + ret` 模式，不覆盖条件分支后的尾调用 |
| Depth Guard 200 层不够 | 低 | 中 | 从 200 加到 500；将来可改为可配置 |

---

## 验证命令

```bash
# 编译所有 native 项目
cmake --build build/native --target chaos_interpreter
cmake --build build/native --target chaos_runtime_core
cmake --build build/native --target chaos_interpreter_bridge_test
cmake --build build/native --target chaos_interpreter_integration

# 运行 bridge 测试
build/native/tests/contracts/native/interpreter/chaos_interpreter_bridge_test.exe

# 运行 interpreter 集成测试
build/native/tests/contracts/native/interpreter/chaos_interpreter_integration.exe

# 完整测试
cmake --build build/native --target check-interpreter
```
