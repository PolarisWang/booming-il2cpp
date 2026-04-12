# Hot Update Interpreter VM v1

## 目标

Phase C 把原生 `InterpreterVM` 从 Phase 7 的最小 `int32` 算术执行器扩展为可承载热更闭环的 value model。这个页面只记录长期稳定的边界，不记录具体任务流水。

## 当前结构

### 值模型

- `src/native/interpreter/interpreter_vm.h` 中的 `InterpreterValue` 使用 tagged union。
- 当前 `ValueTag` 固定为 7 类：`Void`、`Int32`、`Int64`、`Float32`、`Float64`、`ObjectRef`、`Null`。
- `ExecutionFrame.arguments / locals / stack` 全部统一为 `std::vector<InterpreterValue>`。
- `ExecutionResult` 保留兼容字段 `int32_value`，同时新增通用 `return_value`。

### opcode 范围

- 原生 `IROpCode` 现为 54 条，覆盖常量、字段、调用、分支、算术、对象、数组、转换与异常控制流。
- Phase C 新增并稳定下来的关键 opcode：
- `LdcI8 / LdcR4 / LdcR8 / LdStr / LdNull`
- `StArg / LdFld / StFld / LdSFld / StSFld`
- `Beq / Blt / Bgt / Ble / Bge`
- `NewObj / Box / Unbox / CastClass / IsInst`
- `NewArr / LdElem / StElem / LdLen`
- `Conv_I4 / Conv_I8 / Conv_R4 / Conv_R8`
- `Throw / Rethrow / Leave / EndFinally / EndFilter`

## 执行语义

### 已支持

- 数值常量可直接进入 `InterpreterValue`。
- `LdStr` 以对象引用语义承载字符串指针。
- `NewObj` / `NewArr` / `Box` 在原生侧使用轻量内存对象承载字段、数组元素和装箱值。
- 比较、分支、转换会根据 `ValueTag` 做最小可用的数值转换。
- `LdFld / StFld / LdElem / StElem / LdLen` 已具备 native smoke 级别的读写能力。

### 明确边界

- `Call / CallVirt / CallBridge` 在原生 `InterpreterVM` 中仍要求外部桥接；单独运行 native VM 时这些 opcode 会抛出错误，而不是隐式做不完整分派。
- 实际的 managed 递归调用、`CallBridge` 路由与异常区域解析，当前仍以 `ManagedInterpreterExecutor` 为主。
- `EndFilter` 已进入 opcode catalog，但当前 native VM 只把它视为控制流占位，不承担完整 filter 求值。

## 架构取舍

- `Chaos.IL2CPP.HotUpdate` 已经依赖 `Chaos.IL2CPP.Contracts`，而 `Chaos.IL2CPP.Interpreter` 反向依赖 `Chaos.IL2CPP.HotUpdate`。
- 因此 Phase C 没有把 `RuntimeManager` 直接做成“读 package 后立刻把 IL lowering 成 `IRMethod` 并在本项目内解释执行”的形态。
- 当前长期稳定方案是：
- native `InterpreterVM` 负责 value model 与原生 smoke 面；
- managed `ManagedInterpreterExecutor` 负责 `IRMethod` 级递归调用与 bridge 分派；
- hot update 运行时通过 subject-id 级泛型注册避免项目循环依赖。

## 验证基线

### 项目级回归

```powershell
python -m pytest tests/unit/run/test_phase7_interpreter_mixed_execution.py tests/unit/run/test_phase9_hot_update_e2e.py
python -m pytest tests/unit/run
```

### 原生构建与 smoke

```powershell
cmake -S . -B artifacts/.tmp-phase-c-fullbuild -G "Visual Studio 17 2022" -DROADMAP0_PRESET_TARGET=windows-x64-reference
cmake --build artifacts/.tmp-phase-c-fullbuild --config Release
artifacts\.tmp-phase-c-fullbuild\tests\contracts\native\interpreter\Release\chaos_interpreter_smoke.exe
artifacts\gc-contracts\Release\gc_init_smoke.exe
artifacts\gc-contracts\Release\gc_handle_smoke.exe
artifacts\gc-contracts\Release\gc_thread_smoke.exe
artifacts\gc-contracts\Release\gc_finalizer_smoke.exe
artifacts\gc-contracts\Release\gc_safepoint_smoke.exe
```

## 与 Phase 5/6/7 的关系

- Phase 5 提供 hot update skeleton 和最小常量替换。
- Phase 6 提供 supplemental metadata 与 bridge template 基础。
- Phase 7 提供 `ManagedInterpreterExecutor`、real lowering 与 mixed execution proof。
- Phase C 把这几块拼成真正可验证的 hot update 端到端路径，并把 native value model 扩到热更需要的最小闭环。
