# Codegen 故障诊断指南

> 适用版本: 2026-05  
> 本指南覆盖 AOT 代码生成（NativeAotLoweringPlanner + NativeAotEmitter）和 JIT 原生代码生成（code_generator）的常见故障排查方法。

---

## 1. 故障排查工作流

```
[发现故障]
    ↓
[1. 确定故障层]
   ├── AOT 编译期: chaos-il2cpp convert-to-cpp 失败
   ├── AOT 生成期: 生成的 C++ 编译失败
   ├── JIT 生成期: GenerateNativeCode 返回 nullptr
   ├── 运行时: 生成代码执行 crash/异常/结果错误
   └── 验证期: foundation-dll verification 失败
    ↓
[2. 选择对应的排查工具]
   ├── convert-to-cpp 失败 → §2 AOT 编译期故障
   ├── C++ 编译失败 → §3 生成代码编译故障
   ├── JIT 生成失败 → §4 JIT 原生代码生成故障
   ├── 运行时 crash → §5 运行时故障
   └── verification 失败 → §6 验证管线故障
    ↓
[3. 修复后验证]
   ├── 快照测试: SNAPSHOT_UPDATE=1 更新基线后 git diff 审查
   ├── 原生测试: test_codegen_native / test_codegen_abi / test_codegen_il_smoke
   └── 验证管线: foundation-dll verify_family
```

---

## 2. AOT 编译期故障（convert-to-cpp 阶段）

### 2.1 PipelinePlan 加载失败

**现象**: `chaos-il2cpp convert-to-cpp` 在 pipeline 加载阶段退出。

**排查步骤**:
1. 检查输入 assembly 路径是否正确，assembly 是否为有效的 .NET PE 文件
2. 确认 `--entry-point` 指定的 entry point subjectId 在 assembly 中存在
3. 检查 loader 阶段输出日志，查找 `Loader.EntryAndHandleKinds` 或 `Loader.OpcodeDecodeCoverage` 错误

### 2.2 AotCoreIr 降低失败

**现象**: `AotCoreIr` 生成阶段失败，日志中出现 `unsupported` 或 `not supported`。

**常见原因**:
| 原因 | 症状 | 修复方向 |
|------|------|---------|
| 不支持的 IL opcode | `AotCoreIrLowering.cs` 中缺少对应 opcode 映射 | 在 `AotCoreIrLowering.cs` 的 `LowerInstruction` 方法中添加映射 |
| 不支持的 exception region 类型 | `"unsupported managed exception region kind"` | 检查 `AotCoreIrLowering.cs` 中 `handlingKind` 的 switch 分支 |
| 不支持的 dispatch kind | `DispatchKindCode` 无法识别 | 检查 `AotCoreIr` 指令的 `DispatchKindCode` 字段 |

### 2.3 MetadataRegistration 失败

**现象**: MetadataRegistration 阶段失败，提示 `subjectId` 无法解析。

**排查**:
1. 确认 `methodsBySubjectId` 中包含目标方法
2. 检查 closure manifest 中是否包含了所有引用的类型
3. 使用 `--verbose` 参数运行查看详细日志

### 2.4 Flat fallback（非故障，但需要关注）

**现象**: 方法被记录为 `flatFallbackCount`，生成 C++ 中使用 `goto chaos_ip_*` 标签模式。

**排查步骤**:
1. 运行后检查 `native-aot.codegen-metrics.json` 中的 `flatFallbackCount` 字段
2. 查看构建日志中 `IRFlatRegion` 的输出，确认回退原因：
   - `"exception-shape-unhandled"` — EH 区域结构不被 5 种已识别 shape 覆盖
   - `"exception-shape-failed"` — EH shape 匹配但 partition stack balance 校验失败
   - `"residual-br-leave"` — 结构化恢复后仍存在残留分支指令
   - `"stack-underflow-on-ret"` — 返回值栈深度计算异常
   - 非 EH 方法的 irreducible CFG + node-splitting 失败
3. 修复方向：`TryBuildStructuredExceptionMethodBody` 中添加新的 shape detector，或修正 `ValidatePartitionStackBalance`

---

## 3. 生成代码编译故障

### 3.1 CHAOS_EH 宏未定义

**现象**: 生成的 C++ 文件中使用了 `CHAOS_EH_TRY`、`CHAOS_EH_CATCH_BEGIN` 等宏，但编译时报 undefined。

**排查**:
1. 确认 `<chaos/eh.h>` 在 include 路径中
2. 确认 `<chaos/common.h>` 包含了 `<chaos/eh.h>`
3. 确认 `config.h` 中没有被 `#define CHAOS_IL2CPP_EH_SETJMP` 或 `CHAOS_IL2CPP_EH_CPP_THROW` 以外的宏干扰

### 3.2 类型/符号未定义

**现象**: 生成的 C++ 引用了类型 `ThinLockableHeader`、`CHAOS_IL2CPP_INTPTR` 等，编译报 undefined。

**排查**:
1. 确认生成文件 `#include` 了 `<chaos/common.h>`
2. 运行 `NativeCodegenValidator` 检查生成代码是否符合规范：
   ```csharp
   NativeCodegenValidator.ValidateGeneratedFile("path/to/generated.cpp")
   ```
3. 检查 codegen emitter 中是否缺少对应符号的 output

### 3.3 Codegen Validator 报错

**现象**: Codegen validator 报告规则违反（error/warning）。

**9 条校验规则**:
| # | 规则 | 严重性 |
|---|------|--------|
| 1 | 禁止 `using namespace std;` 或 `using namespace chaos;` | Error |
| 2 | 禁止 `using std::...` 或 `using chaos::...` | Error |
| 3 | 禁止 `NULL` → 用 `nullptr` | Error |
| 4 | 优先使用 `<cstdint>` 类型 | Warning |
| 5 | 头文件必须有 `CHAOS_IL2CPP_*_H_` include guard | Warning |
| 6 | 禁止 `chaos_` 前缀的 ChaosIl2cpp::Common 符号 | Error |
| 7 | .cpp 文件必须包含 `<chaos/common.h>` | Error |
| 8 | 禁止原始 `std::` 类型 → 用 `CHAOS_IL2CPP_*` 宏 | Error |
| 9 | 禁止原始 `<cstdint>` 类型 → 用宏 | Error |

---

## 4. JIT 原生代码生成故障

### 4.1 GenerateNativeCode 返回 nullptr

**现象**: JIT 调用 `GenerateNativeCode` 返回空指针，方法降级回解释器执行。

**排查步骤**:
1. 调用 `CanGenerateNativeCode(rm)` 预检方法是否可生成原生代码。如果返回 false，检查原因：
   - 包含不支持的 opcode（103 opcodes 以外）
   - SEH clause 索引越界（`CanGenerateNativeCode` 中 clause 范围验证失败）
   - CodeBuffer OOM（`CheckFailed()` 返回 true）
2. 检查 `code_generator.cpp` 中 `Generate()` 方法的各阶段 CheckFailed 输出
3. 检查是否因方法体过大导致寄存器分配失败（超过 `kMaxLivenessInstrs` = 2048）

### 4.2 JIT 生成代码运行时 crash

**现象**: JIT 生成的 x64 代码执行时 segfault 或 illegal instruction。

**排查步骤**:
1. 检查 `NativeMethod` 的 `slot_map_data` 是否已注册（GC 扫描时可能需要）
2. 检查 `t4_seh_handler.cpp` 中 VEH handler 是否已注册（`RegisterT4SehHandler` 调用）
3. 检查生成的 code buffer 边界：是否有 `Reserve()` / `Emit*` 写出界（OOM 安全已在 I-R11 中加固）
4. 检查 deopt runtime 的 `DeoptTrap` 二分搜索是否因 `deopt_entries` 未排序而失败

### 4.3 JIT GC Slot Map 问题

**现象**: JIT 代码中的 GC 对象未被正确扫描，导致 dangling pointer 或 premature collection。

**当前限制**（已知）:
| 限制 | 影响 | 状态 |
|------|------|------|
| 无 per-safepoint 过滤 | 所有 GC 点扫描全部 ObjectRef vreg | 待修复（B-P1-2） |
| LdFld/LdSFld 默认 ObjectRef | 非对象字段被误标记为对象引用 | 待修复（B-P1-2） |
| Call 无 call_cache 时默认 ObjectRef | 返回值类型推断保守 | 待修复（B-P1-2） |

**临时缓解**：确保 `PatchMethod::cached_sig_valid` 和 `arg_type_tags` 在 SetupFastFrame 时已填充

### 4.4 Deoptimization 失败

**现象**: JIT 代码触发 deopt 时 crash 或状态异常。

**排查**:
1. 检查 `deopt_entries[]` 是否按 `native_offset` 排序（`DeoptRuntime::FindEntry` 依赖二分搜索）
2. 检查 `NativeContext` 中的寄存器值是否在被 deopt 前已被破坏
3. 检查溢出 deopt（`AddOvf`/`SubOvf`/`MulOvf`）是否是 `kDeoptMagic` + 重新执行模式（非真正 OSR）

---

## 5. 运行时故障

### 5.1 Exception handling 异常传播失败

**现象**: managed exception throw/catch 行为不正确。

**排查**:
1. 确认使用的是哪种 EH 模式：`CHAOS_IL2CPP_EH_CPP_THROW`（桌面端）vs `CHAOS_IL2CPP_EH_SETJMP`（移动端）
2. 检查 `FastFrame::seh_clauses` 和 `seh_clause_count` 在 SetupFastFrame 时是否已正确设置
3. 检查 `Handle_Throw` 中 `FindEnclosingCatch` 是否能正确匹配 catch clause
4. 检查 `Handle_EndFinally` 的三路状态机（异常展开 / pending_leave / 正常继续）是否正确工作
5. Sentinel 异常（ThreadAbort/COMException）的 typed catch 跳过逻辑当前未实现

### 5.2 FastFrame 溢出

**现象**: `Push*` 方法中 `sp >= kMaxStack` 断言触发。

**当前容量**: `kMaxStack=64`, `kMaxLocals=32`, `kMaxTracked=32`
**池容量**: `kPoolSize=32`（`FastFramePool`）

**排查**:
1. 检查方法 `local_count` 是否超过了 `kMaxLocals`（32）
2. 检查 eval stack peak depth 是否超过了 `kMaxStack`（64）
3. 池满时 `Acquire()` 返回 nullptr，调用者应 fallback 到栈分配 + memset（当前已实现）

### 5.3 Tier upgrade 不触发

**现象**: 方法一直停留在 T1/T2 层，不升级到 T4。

**排查**:
1. 确认 `I-R2a`（call-site tier upgrade）已合入，`call_count` fetch_add 逻辑正常
2. 检查 `kT1Cold → kT2Ready → kT3Lowering → kT4Ready` 状态机是否在正常流转
3. 检查 `TierManager::EnqueueOptimization` 异步队列是否正常工作

---

## 6. 验证管线故障

### 6.1 Foundation-dll verification 失败

**现象**: `verify_family.py` 在某个阶段失败。

**排查各阶段错误**:
| 阶段 | 常见错误 | 排查方向 |
|------|---------|---------|
| preflight | method count 不匹配 | 检查 family contract 中的 `methodSubjectIds` 与实际 assembly 方法是否一致 |
| codegen | 构建 entrypoint 失败 | 检查 `chaos-il2cpp convert-to-cpp` 日志（见 §2） |
| codegen | 生成 C++ 编译失败 | 检查生成的 .cpp 文件（见 §3） |
| fact | 功能验证 FAIL | 检查 fact 测试代码与生成 C++ 的语义匹配 |
| audit | false passing 或存根检测 | 检查是否有方法走存根路径而非原生路径 |
| asm_compare | IR 扩展比异常 | 检查 AOT IR 到 JIT x64 的 opcode 映射是否正确 |

### 6.2 Snapshot test 基线不匹配

**现象**: 快照测试运行失败，`SnapshotMismatchException` 提示 C++ 输出有差异。

**排查**:
1. 运行 `git diff` 查看 baseline 文件变化
2. 如果变化符合预期：设置 `SNAPSHOT_UPDATE=1` 环境变量，重新运行测试，审查基线更新后提交
3. 如果变化不符合预期：回退代码改动，定位引起 diff 的具体 emitter 变更
4. 行尾差异（CRLF vs LF）会被自动标准化，不属于真正的 mismatch

### 6.3 codegen-metrics 指标异常

**现象**: `native-aot.codegen-metrics.json` 中的指标不符合预期。

**关注指标**:
| 字段 | 正常范围 | 异常信号 |
|------|---------|---------|
| `structuredRecoveryRate` | 1.0 | < 1.0 表示有 flat fallback |
| `flatFallbackCount` | 0 | > 0 需要排查（见 §2.4） |
| `aotUnreachableMethodCount` | 0 | > 0 表示 AOT closure 覆盖不全 |
| `generatedCppTotalBytes` | 视 family 而定 | 异常大幅增长可能表示 codegen 生成了冗余代码 |

---

## 7. 常见错误码速查

| 错误码/日志模式 | 含义 | 章节 |
|----------------|------|------|
| `NotSupportedException: native-aot lowering does not support opcode` | AOT emitter 遇到不认识的 opcode | §2.2 |
| `NotSupportedException: native-aot structured EH linear` | AOT EH lowering 遇到不支持的指令 | §2.4 |
| `IRFlatRegion` + `exception-shape-unhandled` | EH region 结构不被 5 种 shape 覆盖 | §2.4 |
| `CanGenerateNativeCode` returns false | JIT 预检失败 | §4.1 |
| `CheckFailed()` | CodeBuffer OOM 或断言失败 | §4.1 |
| `SnapshotMismatchException` | 快照测试基线不一致 | §6.2 |
| `s_flatRegionCount` incremented | 方法回退到 flat goto 发射 | §2.4 |
| `FAST_FRAME_POOL_EMPTY` | FastFramePool 耗尽 | §5.2 |

---

## 8. 调试快速命令

```bash
# 查看最新 trace 会话的 span 树
run trace

# 分析 trace 阶段耗时分布
trace-analyze

# 运行 codegen 原生测试
ctest -R test_codegen_native -V
ctest -R test_codegen_abi -V
ctest -R test_codegen_il_smoke -V

# 运行快照测试（Linux/macOS 需加前缀）
SNAPSHOT_UPDATE=1 dotnet test tests/snapshots/Chaos.IL2CPP.CodeGen.SnapshotTests/

# 更新所有基线
$env:SNAPSHOT_UPDATE=1
dotnet test tests/snapshots/Chaos.IL2CPP.CodeGen.SnapshotTests/

# 验证 foundation-dll family
cd testing/foundation-dll && python -m _core.cli verify-family System.Private.CoreLib/garbage-collection

# 查看 codegen metrics
cat testing/foundation-dll/System.Private.CoreLib/garbage-collection/codegen/native-aot.codegen-metrics.json
```

---

## 附录: 关键文件索引

| 文件 | 作用 |
|------|------|
| `src/managed/Chaos.IL2CPP.Generator/Emission/NativeAotLoweringPlanner.StructuredIR.cs` | 结构化 IR 恢复与 EH 发射 |
| `src/managed/Chaos.IL2CPP.Generator/Emission/NativeAotLoweringPlanner.ExceptionEmission.cs` | AOT 线性 EH lowering（备用路径） |
| `src/managed/Chaos.IL2CPP.Generator/NativeAotEmitter.cs` | AOT emitter 入口 |
| `src/managed/Chaos.IL2CPP.Generator/NativeCodegenMetricsBuilder.cs` | codegen-metrics 构建 |
| `src/managed/Chaos.IL2CPP.Generator/Validation/NativeCodegenValidator.cs` | 9 条 C++ 代码规范校验 |
| `src/native/codegen/code_generator.cpp` | JIT x64 代码生成器主文件 |
| `src/native/codegen/t4_seh_handler.cpp` | JIT VEH 异常处理器 |
| `src/native/codegen/deopt_runtime.cpp` | JIT deopt 运行时 |
| `src/native/codegen/native_method.h` | NativeMethod/GcPoint/GcSlot 数据结构 |
| `src/native/runtime-core/gc/gc_root_scanner.cpp` | GC slot map 注册与扫描 |
| `src/native/common/chaos/eh.h` | CHAOS_EH_TRY/CATCH/END 宏定义 |
| `contracts/native/v0/codegen_bridge.h` | GcSlotMapV0、CodegenBridgeV0 ABI 合约 |
| `testing/src/native/codegen/codegen_native_test.cpp` | JIT 原生代码生成测试 |
| `testing/src/native/codegen/codegen_abi_test.cpp` | Codegen ABI 合约测试 |
| `testing/src/native/codegen/codegen_il_smoke_test.cpp` | IL 冒烟测试 |
