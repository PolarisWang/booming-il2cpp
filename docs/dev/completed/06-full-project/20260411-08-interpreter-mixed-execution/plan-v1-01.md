# Interpreter Core And Mixed Execution 实现计划

> **面向执行 Agent：** 必须使用 dev:executing-plans 来执行本计划。

**目标：** 建立 IL 解释器核心（IL→IR lowering + InterpreterVM），验证 AOT↔HotUpdate 混合执行调用链。

**架构：** 新建 `Chaos.IL2CPP.Interpreter` managed 项目（依赖 Contracts + HotUpdate，禁止依赖 CodeGen）+ `src/native/interpreter/` native 模块。IR 指令集基于 Phase 0 决策（选项 B：内部 IR）。

**技术栈：** C# (.NET 8), C/C++

**设计文档：** `phase-architecture-v1-01.md` Phase 7 节

---

## 步骤

### Batch 1: InterpreterIR 定义

- [ ] **1.1** 创建 `Chaos.IL2CPP.Interpreter` 项目（依赖 Contracts, HotUpdate）
- [ ] **1.2** 定义 `InterpreterIR` 数据模型：IRMethod, IRInstruction, IROperand, IRBasicBlock
- [ ] **1.3** 定义 IR 指令集（~30 条）：
  - 常量：`ir_ldc_i4`, `ir_ldstr`, `ir_ldnull`
  - 局部变量：`ir_ldloc`, `ir_stloc`
  - 参数：`ir_ldarg`, `ir_starg`
  - 字段：`ir_ldfld`, `ir_stfld`, `ir_ldsfld`, `ir_stsfld`
  - 调用：`ir_call`, `ir_callvirt`, `ir_call_bridge`（跨模式调用）
  - 分支：`ir_br`, `ir_brtrue`, `ir_brfalse`, `ir_beq`, `ir_blt`, `ir_bgt`, `ir_ble`, `ir_bge`
  - 算术：`ir_add`, `ir_sub`, `ir_mul`, `ir_div`, `ir_rem`, `ir_neg`
  - 比较：`ir_ceq`, `ir_clt`, `ir_cgt`
  - 对象：`ir_newobj`, `ir_box`, `ir_unbox`, `ir_castclass`, `ir_isinst`
  - 数组：`ir_newarr`, `ir_ldelem`, `ir_stelem`, `ir_ldlen`
  - 异常：`ir_throw`, `ir_rethrow`, `ir_leave`, `ir_endfinally`
  - 返回：`ir_ret`
- [ ] **1.4** 定义 IR operand 编码（typed: 每个 operand 带类型标签，不用 stack-machine 模拟）

### Batch 2: IL → IR Lowering

- [ ] **2.1** 实现 `ILToIRLowering`：读取 hot-update assembly 的 IL → 生成 InterpreterIR
- [ ] **2.2** 处理 IL evaluation stack → IR 显式 register（stack → register 转换）
- [ ] **2.3** 处理 branch target 解析（IL offset → IR basic block index）
- [ ] **2.4** 处理 method call resolution（same-assembly vs cross-assembly vs AOT bridge）
- [ ] **2.5** 测试：简单方法（`int Add(int a, int b) => a + b`）的 IL→IR 正确性

### Batch 3: InterpreterVM（Native 实现）

- [ ] **3.1** 创建 `src/native/interpreter/` 项目
- [ ] **3.2** 实现 `InterpreterVM` 核心循环：fetch-decode-execute
- [ ] **3.3** 实现操作数栈 + 局部变量 + 参数表
- [ ] **3.4** 实现基础指令：常量、算术、比较、分支、返回
- [ ] **3.5** 实现对象/数组指令（通过 runtime ABI 调用 object_new/array_new）
- [ ] **3.6** 实现异常处理（try-catch-finally dispatcher）

### Batch 4: Mixed Execution

- [ ] **4.1** 实现 AOT → interpreter 调用：bridge stub → `InterpreterVM.Execute(method, args)`
- [ ] **4.2** 实现 interpreter → AOT 调用：`ir_call_bridge` → 查找 AOT 函数指针 → native call
- [ ] **4.3** 实现 interpreter → engine 调用：通过 engine contract bridge
- [ ] **4.4** 实现 exception 跨边界传播：interpreter throw → AOT catch, AOT throw → interpreter catch

### Batch 5: Subject + Performance

- [ ] **5.1** 创建 `subjects/InterpreterArithmeticProof/`：纯算术热更方法验证
- [ ] **5.2** 创建 `subjects/MixedExecutionProof/`：
  - AOT `Calculator.Add(1, 2)` → 3（AOT）
  - 热更 `HotFormatter.Format(3)` → "Result: 3"（interpreter）
  - 热更调 AOT `Calculator.Add(10, 20)` → 30（interpreter → AOT）
  - 异常传播：interpreter throw → AOT catch
- [ ] **5.3** Perf baseline：
  - 解释器纯算术循环 vs native ≤ 50x 慢
  - AOT → interpreter 跨边界 ≤ 1μs/call
  - interpreter → AOT 跨边界 ≤ 500ns/call
  - 热更方法首次执行 (IL→IR + execute) ≤ 10ms
