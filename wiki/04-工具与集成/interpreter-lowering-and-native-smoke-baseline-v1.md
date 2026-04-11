# Interpreter Lowering、Mixed Execution 与 Native Smoke Baseline v1

## 适用范围

- 适用于 Phase 7 interpreter core / mixed execution 的当前 proof-first baseline。
- 目标是把 managed `IL -> IR lowering`、managed mixed execution proof 与 native `InterpreterVM` 的最小可执行承载面固定下来。
- 本页只记录长期有效的项目边界、proof 选型与验证入口，不记录单次执行流水。

## Managed Baseline

- `Chaos.IL2CPP.Interpreter` 继续只依赖 `Chaos.IL2CPP.Contracts` 与 `Chaos.IL2CPP.HotUpdate`，不能反向依赖 `Chaos.IL2CPP.CodeGen`。
- `ManagedInstructionModel` 必须保留 `IlOffset`。`LoaderStage` 负责对每条 IL 指令打点 `IlOffset`，并把 `br` / `brtrue` / `brfalse` / `leave` 的操作数统一编码为绝对 IL target offset。
- 为兼容现有 `CodeGen` 对 `Body.Blocks.Count == 1` 的假设，loader 继续对外暴露单 block body；IR block 切分由 `ILToIRLowering` 基于 `IlOffset`、branch target 与异常区域边界完成。
- `ILToIRLowering` 当前至少覆盖以下 lowering 面：
  - `ldarg`
  - `ldloc`
  - `stloc`
  - `ldc.i4`
  - `ldstr`
  - `ldnull`
  - `add` / `sub` / `mul` / `div` / `rem`
  - `call`
  - `callvirt`
  - `ceq` / `clt` / `cgt`
  - `br` / `brtrue` / `brfalse`
  - `leave`
  - `endfinally`
  - `throw`
  - `rethrow`
  - `pop`
  - `ret`
- 当前 call lowering 规则是：
  - 跨程序集方法调用优先降到 `IROpCode.CallBridge`
  - 同程序集静态调用保留为 `IROpCode.Call`
  - 同程序集实例调用保留为 `IROpCode.CallVirt`
  - `System.String::get_Length()` 这类外部实例 `callvirt` 目前仍作为 `CallBridge` lowering proof，而不是实例 receiver 执行 proof
- `ILToIRLowering` 当前会把真实 `ManagedExceptionRegionModel` 降到块级 `IRExceptionRegion` / `IRExceptionRegionKind`，最小闭环覆盖 `catch` 与 `finally`。
- `LoaderStage` 必须同步暴露对应 opcode decode；如果 lowering 想支持新 opcode，先补 loader 输入面。
- `ManagedInterpreterExecutor` 是当前 mixed execution 的 managed proof 承载层，不直接代表 native runtime 已接管 mixed mode。当前最小执行面覆盖：
  - `LdcI4`
  - `LdStr`
  - `LdNull`
  - `LdArg`
  - `LdLoc`
  - `StLoc`
  - `Add` / `Sub` / `Mul` / `Div` / `Rem`
  - `Call`
  - `CallVirt`
  - `CallBridge`
  - `Ceq` / `Clt` / `Cgt`
  - `Br`
  - `BrTrue`
  - `BrFalse`
  - `Leave`
  - `EndFinally`
  - `Throw`
  - `Rethrow`
  - `Pop`
  - `Ret`
- `ManagedInterpreterExecutor` 当前使用最小 `object?` 值槽：
  - `int` 继续服务于算术、比较、分支与 `ExecuteInt32`
  - receiver、`string` 与 `null` 只为最小 mixed execution / `callvirt` proof 开放，不代表完整对象模型已经落地
- `Pop` 当前是 no-op executor 指令，用于承接真实 catch handler 常见的 `pop` 前导，而不是完整 EH 栈语义的定型。
- 同程序集调用仍依赖 `methodResolver` 按 `subject id` 解析 callee；这是 proof 机制，不等于完整调用栈模型。

## Lowering Proof Baseline

- managed lowering proof 使用 dedicated minimal input assembly，即 `subjects/InterpreterArithmeticProof`。
- `subjects/InterpreterArithmeticProof/source/AotBridge/InterpreterArithmeticProof.AotBridge.csproj` 专门用于制造真实外部 bridge call metadata；主项目必须显式排除 `AotBridge/**/*.cs`。
- `subjects/InterpreterArithmeticProof` 当前至少包含以下 proof 方法：
  - 纯算术：`Add`
  - 外部 bridge call：`CallAotBridgeAdd`
  - 同程序集静态 call：`CallLocalAdd`
  - 外部 `callvirt` lowering：`CallStringLength`
  - 同程序集实例 `callvirt`：`CallInstanceAddOne`
  - 真实 `try/catch`：`DivideOrCatch`
  - 真实 `rethrow`：`DivideOrRethrow`
  - 真实 `try/finally`：`AddWithFinally`
- `subjects/InterpreterLoweringProof` 当前至少打印以下调用证据：
  - `call-ops=ldarg,ldarg,callbridge,ret`
  - `local-call-ops=ldarg,ldarg,call,ret`
  - `callvirt-ops=ldstr,callbridge,ret`
  - `callvirt-target=System.Private.CoreLib/System.String::get_Length()`
  - `instance-call-method-subject=InterpreterArithmeticProof/NativeExports::CallInstanceAddOne(InstanceArithmetic,System.Int32)`
  - `instance-call-ops=ldarg,ldarg,callvirt,ret`
  - `instance-call-target=InterpreterArithmeticProof/InstanceArithmetic::AddOne(System.Int32)`
- `CallStringLength` 证明的是外部 `callvirt -> CallBridge` lowering 结果。
- 实例 receiver 的真实执行 proof 以 `CallInstanceAddOne` 为准，不再依赖字符串 getter 路径外推。
- 观察与打印 lowering 结果统一使用 `subjects/InterpreterLoweringProof`。
- 避免直接拿复杂 subject 充当 lowering baseline proof input。loader 会扫描整个 assembly，复杂入口更容易先撞上尚未覆盖的 opcode。

## Mixed Execution Baseline

- `subjects/MixedExecutionProof` 是当前 Phase 7 mixed execution 的统一 proof 入口。
- 当前 proof 输出至少固定包含：
  - `mixed-aot-to-interpreter-before-load=22`
  - `mixed-aot-to-interpreter=42`
  - `mixed-interpreter-to-aot=30`
  - `mixed-interpreter-to-aot-ops=ldarg,ldarg,callbridge,ret`
  - `mixed-interpreter-to-aot-target=InterpreterArithmeticProof.AotBridge/AotBridgeExports::Add(System.Int32,System.Int32)`
  - `mixed-interpreter-local-call=42`
  - `mixed-interpreter-local-call-ops=ldarg,ldarg,call,ret`
  - `mixed-interpreter-instance-call=42`
  - `mixed-interpreter-instance-call-ops=ldarg,ldarg,callvirt,ret`
  - `mixed-interpreter-instance-call-target=InterpreterArithmeticProof/InstanceArithmetic::AddOne(System.Int32)`
  - `mixed-interpreter-string-bridge=5`
  - `mixed-interpreter-string-bridge-ops=ldstr,callbridge,ret`
  - `mixed-interpreter-to-engine=7`
  - `mixed-interpreter-throw-caught=ok`
  - `mixed-interpreter-leave-finally=ok`
  - `mixed-interpreter-rethrow-caught=ok`
  - `mixed-interpreter-real-catch=42`
  - `mixed-interpreter-real-catch-region-kind=catch`
  - `mixed-interpreter-real-rethrow-caught=ok`
  - `mixed-interpreter-real-rethrow-region-kind=catch`
  - `mixed-interpreter-real-leave-finally=42`
  - `mixed-interpreter-real-leave-finally-region-kind=finally`
  - `mixed-interpreter-real-leave-finally-opcodes=leave,endfinally`
  - `mixed-aot-to-interpreter-after-unload=22`
- 这些输出分别证明：
  - package load 前 AOT fallback 生效
  - package load 后 AOT -> interpreter 路径可切换
  - interpreter -> AOT bridge 来自真实 lowered IL
  - 同程序集静态 `Call` 已有真实执行证据
  - 同程序集实例 `CallVirt` 已有真实 receiver 执行证据
  - `ldstr + callbridge` 的字符串桥执行路径仍可运行
  - interpreter -> engine bridge 最小可执行
  - interpreter throw 可以被 AOT catch
  - 手写 IR 的 `leave/finally` 与 `catch + rethrow` smoke proof 仍保留
  - 真实 `ManagedExceptionRegionModel -> IRExceptionRegion` lowering 已支撑最小 `try/catch`、`rethrow` 与 `try/finally` managed proof
  - package unload 后可回退到 AOT fallback
- `mixed-interpreter-string-bridge` 当前仍是手写 IR proof，用于保持对象值槽与桥路径烟测；真实 receiver proof 以 same-assembly `CallVirt` 为准。

## Native Baseline

- native interpreter 模块固定在 `src/native/interpreter/`，target 名称为 `chaos_interpreter`。
- `InterpreterVM` 当前最小执行面包括：
  - `LdcI4`
  - `LdArg`
  - `LdLoc`
  - `StLoc`
  - `Add`
  - `Sub`
  - `Mul`
  - `Div`
  - `Rem`
  - `Ceq`
  - `Clt`
  - `Cgt`
  - `Br`
  - `BrTrue`
  - `BrFalse`
  - `Ret`
- `tests/contracts/native/interpreter/chaos_interpreter_smoke` 当前至少覆盖：
  - `ldarg,stloc,ldloc,ldarg,add,ret -> 42`
  - `ceq + brtrue -> 42`

## 验证入口

- Phase 7 定向入口：`python -m pytest tests/unit/run/test_phase7_interpreter_mixed_execution.py -v`
- Phase 6 回归入口：`python -m pytest tests/unit/run/test_phase6_capability_batch_b.py -v`
- managed interpreter 项目构建：`dotnet build src/managed/Chaos.IL2CPP.Interpreter/Chaos.IL2CPP.Interpreter.csproj -c Release`
- managed 侧综合构建：`dotnet build solutions/core/windows/chaos-il2cpp-core.sln -c Release`
- native 侧构建：
  - `cmake -S . -B artifacts/.tmp-interpreter-native -G "Visual Studio 17 2022" -DROADMAP0_PRESET_TARGET=windows-x64-reference`
  - `cmake --build artifacts/.tmp-interpreter-native --config Release --target chaos_interpreter`
- native smoke proof 由 `test_phase7_interpreter_mixed_execution.py` 统一驱动。

## Windows 文件锁约束

- Windows 上 `dotnet` / `.NET Host` 残留进程可能锁住 subject proof 输出 DLL。
- Phase 7 的 managed proof 测试必须使用唯一 `BaseOutputPath`，不能依赖共享 `bin/Release`。
- 后续新增 subject-scoped proof 时默认沿用这一策略。

## 边界说明

- 当前 baseline 证明的是“真实 lowering 可复现 + managed mixed execution 已有最小闭环 + native VM 最小 execute path 可运行”，不等于 Phase 7 已演化为完整 production interpreter。
- 当前已经闭环的关键点包括：
  - 外部静态 / 实例调用的 `CallBridge` lowering proof
  - 同程序集 `Call` 的真实执行
  - 同程序集 `CallVirt` 的真实 receiver 执行
  - 真实 `ManagedExceptionRegionModel -> IRExceptionRegion` EH lowering
  - 真实 `try/catch` / `rethrow` / `try/finally` managed proof
- 当前仍未闭环的关键点包括：
  - 完整对象模型
  - 完整虚派发语义与更宽的 `callvirt` 覆盖面
  - `catch` type filtering
  - 更宽的 opcode 面
  - native runtime 对 managed mixed mode 的直接承载
- `ManagedInterpreterExecutor` 的 EH dispatcher 仍是 proof-first 的块级最小语义，目标是证明 lowering/dispatch 主线闭环，而不是宣称完整 CLR 级异常语义已经实现。
