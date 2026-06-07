# 专项跟踪：32-byte Value Type ABI Fix（方案 C）

## 背景

numerics chunk 的 9 个 Vector SIMD fact 测试失败，根因是 IL2CPP 代码生成对 x64 ABI 的处理不完整——`Vector<T>`（32 bytes）被标记为 `ValueTypeByValue`（值传递），但 x64 调用约定要求 >16 bytes 的值类型必须通过 **hidden reference**（指针传递）。

D5/D1 阶段已将 closure 从 49,862 → 352 methods，构建时间从 6min → 37s，但 Vector SIMD 的修复需要方案 C。

## 执行状态（2026-06-07）

| 层 | 状态 | 结果 | 改动文件 |
|----|------|------|---------|
| 1 ABI 分类 | ✅ **完成** | Vector<T> >16byte → ByRefToValueType | `AotCoreIrLowering.cs` (cache + size check) |
| 2 Opcode 归一化 | ✅ **完成** | ldarg.N/ldc.i4.N/br.s 等短格式已处理 | `IlBytecodeDecoder.cs`, `ExceptionEmission.cs` (×3 处 switch), `AotCoreIrLowering.cs` (cache) |
| 3 Extern 声明 | 🔴 **阻塞** | BRIDGE-AOT 注入符号命名不一致（UInt32 vs System.UInt32），修复后导致 emission crash | 需统一 `DummySigProvider` 的类型格式 |
| 4 Runtime stubs | ⏳ 待定 | 暂不影响 build，后续补充 |

### 基线
- AOT IR: 356 methods
- Build: ~51s
- Fact: 112/121 passed (9 Vector SIMD 失败已转为 ABI 问题)
- Pipeline: build+fact 通过

### 当前已提交的修改
- `CodegenOrchestrator.cs` — D5: 去 assembly-dir
- `BridgeAotCompiler.cs` — D5: CompileAndIntegrate API + BuildGenericDemandLookup 去重
- `Program.cs` — D1: --assembly-dir CLI 参数
- `RuntimeEntry.cpp.scriban` — D5: Assert stub
- `build.py / context.py / chunk_pipeline.py` — D1: assemblyDirs 配置
- `fact_chunk.py` — D5: 变量 shadow bugfix
- `CodeGenStage.cs` — 去掉 BridgeCompiledModels
- `AotCoreIrLowering.cs` — Layer 1: s_simpleInstructionCache 含 ldarg.N → ldarg + Operand
- `IlBytecodeDecoder.cs` — Layer 2: ldc.i4.s → ldc.i4, br.s 归一化
- `ExceptionEmission.cs` — Layer 2: 3处 br.s/brfalse.s/brtrue.s 变体; GetRequiredIntOperand 支持 sbyte/byte/short/ushort

### Layer 1：ABI 分类（AotCoreIrLowering.cs）⏳ 已完成，待重新提交

**改动**：`ResolveAbiSlot` 中，当 `RequiresValueTypeByValueCarrier` 返回 true 且值类型大小 > 16 bytes 时，用 `ByRefToValueType` 替代 `ValueTypeByValue`。

**关键代码**：
```csharp
var abiSize = GetValueTypeNativeSizeForAbi(managedType);
if (abiSize > 16) {
    // Use ByRefToValueType (CHAOS_IL2CPP_INTPTR) instead of ValueTypeByValue
    CarrierKindCode = AotCoreIrAbiCarrierKind.ByRefToValueType, ...
}
```

**需要新增** `GetValueTypeNativeSizeForAbi` 方法，识别 `Vector`1<T>`、`Matrix4x4`、`Matrix3x2` 等大值类型。

**验证方式**：AOT IR 中 Vector<T> 参数的 `CarrierKindCode` 变为 `14` (ByRefToValueType)。

---

### Layer 2：ExceptionEmission 短格式 opcode 处理（ExceptionEmission.cs）⚠️ 需 VS Code 编辑

**根因**：IL 解码器输出 `ldarg.0`、`ldc.i4.6`、`br.s` 等短格式 opcode 名称，但 ExceptionEmission.cs 的 emitter switch 只处理 `ldarg`、`ldc.i4`、`br` 等基础格式。所有短格式落到 `default` 分支抛出 `NotSupportedException`。

**问题范围**（从 AOT IR 统计）：
- `ldarg.0` ~ `ldarg.3`：98 次
- `ldarg.1`：50 次（subjects 测试 wrapper 中）
- `ldc.i4.0` ~ `ldc.i4.8`：数量较少
- `br.s` / `brfalse.s` / `brtrue.s`：少量

**修复方式**（两个选项）：

**A. 在 ExceptionEmission 中添加 switch case**（推荐，最安全）：
- 在 `GetRequiredIntOperand` 方法中，对 `ldarg.0` / `ldarg.1` / `ldarg.2` / `ldarg.3` 短路返回 0/1/2/3
- 在 case `"ldc.i4"` 中添加 `ldc.i4.0` ~ `ldc.i4.8` 别名，从 opcode name 提取值
- 在 case `"br"` / `"brfalse"` / `"brtrue"` 等处添加 `.s` 后缀别名

```csharp
// 在 GetRequiredIntOperand 中插入（约第 4730 行）：
// 对 ldarg.N / ldloc.N / stloc.N 短路
if (instruction.Op is "ldarg.0" or "ldarg.1" or "ldarg.2" or "ldarg.3")
    return instruction.Op[^1] - '0';
if (instruction.Op is "ldloc.0" or "ldloc.1" or "ldloc.2" or "ldloc.3")
    return instruction.Op[^1] - '0';
```

**⚠️ 注意**：ExceptionEmission.cs 约 5000 行，使用纯 TAB 缩进，Claude Code Edit 工具无法可靠处理。必须用 VS Code 或 VS 等传统编辑器打开并编辑。

**B. 在 IlBytecodeDecoder 中归一化**（已尝试，风险较高）：
- 将 `ldarg.0` → `ldarg` 并设置 `operand = 0`
- 将 `ldc.i4.6` → `ldc.i4` 并设置 `operand = 6`
- 将 `br.s` → `br`（operand 不变）
- **风险**：会影响其他 consumer（`MapOpStringToEnum`、类型栈追踪等），上次尝试导致 `structured slot stack underflow`。

---

### Layer 3：Extern 声明缺失（codegen 发射）🔴 未开始

**根因**：BRIDGE-AOT 编译 Vector 方法时，对于 `DirectNativeSymbol` 的调用，生成的 C++ page file 直接引用 `chaos_external_runtime_*` 函数名，但没有任何头文件或 extern 声明。

**修复方式**：
- 在 `ModuleRegistration.cs` 的 `BuildExternalRuntimeDispatchTable` 中，除了生成函数定义外，也生成 `extern "C"` 声明，并确保 page files include 的头文件中包含这些声明
- 或者：在 `EmitExternalRuntimeTableDispatch` 中，当 `DirectNativeSymbol` 不为 null 时，也输出 extern 声明

**验证方式**：不再出现 `C3861: identifier not found` for `chaos_external_runtime_*` 符号。

---

### Layer 4：外部运行时 stub（ThrowHelper 等）🔴 未开始

**根因**：Vector 方法调用了 `ThrowHelper.ThrowForUnsupportedNumericsVectorBaseType()` 等方法，这些方法的 `chaos_external_runtime_*` 符号在 runtime 中没有注册。

**修复方式**：
- 在 `RuntimeEntry.cpp.scriban` 中添加 CHAOS_IL2CPP_FAIL stub
- 或者在 `RuntimeHelperShapeRegistry.cs` 中注册为 SimpleForward

**需要收集的符号列表**：
- `chaos_external_runtime_System_Numerics_Vectors_System_ThrowHelper__ThrowForUnsupportedNumericsVectorBaseType_System_Void__`
- `chaos_external_runtime_System_Numerics_Vectors_System_ThrowHelper__ThrowArgumentOutOfRangeException_System_Void_System_ExceptionArgument_`
- `chaos_external_runtime_System_Numerics_Vectors_System_ThrowHelper__ThrowArgumentException_DestinationTooShort_System_Void__`

---

### 回归确认

每次 Layer 修改后，必须运行：
```bash
cd D:/agent/booming-il2cpp/testing/foundation-dll
python -m verification.chunk_pipeline --assembly System.Private.CoreLib --chunk numerics --stages build,fact
```

预期逐步推进：
| 阶段 | 预期 passed |
|------|------------|
| D5 基线 | 112/121 |
| Layer 1 | 112/121（不影响结果） |
| Layer 1+2 | 112/121（opcode 不再报错） |
| Layer 1+2+3 | 112/121（build 成功） |
| Layer 1+2+3+4 | 121/121 ✅ |

## 当前代码状态

| 文件 | 当前状态 | 最后修改 |
|------|---------|---------|
| `CodegenOrchestrator.cs` | D5 版本 | ✅ 已确认 |
| `BridgeAotCompiler.cs` | D5 版本（含新 API） | ✅ 已确认 |
| `IlBytecodeDecoder.cs` | 原始版本 | 🔄 被 linter 恢复 |
| `AotCoreIrLowering.cs` | 原始版本 | 🔄 被 linter 恢复 |
| `ExceptionEmission.cs` | 原始版本 | ⚠️ 需 VS Code 编辑 |
| `RuntimeEntry.cpp.scriban` | D5 版本 | ✅ 已确认 |

## 预计工期

| 层 | 预估 | 依赖 |
|----|------|------|
| Layer 1 | 0.5 天 | 无 |
| Layer 2 | 1 天 | VS Code |
| Layer 3 | 1 天 | Layer 2 |
| Layer 4 | 0.5 天 | Layer 3 |
| 验证 | 1 天 | 全部 |
| **合计** | **~2 周** | |
