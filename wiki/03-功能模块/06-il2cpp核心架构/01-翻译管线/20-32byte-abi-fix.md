# 32-byte Value Type ABI 修复

## 根因

x64 调用约定要求 >16 bytes 的值类型通过 hidden reference（指针）传递，而非值传递。IL2CPP 的 `ResolveAbiSlot` 在 `AotCoreIrLowering.cs` 中通过 `RequiresValueTypeByValueCarrier` 判断值类型的传递方式。该函数对所有不在 `System.Private.CoreLib/System.*` 前缀下的值类型一律返回 `ValueTypeByValue`，但 `Vector<T>`（32 bytes）和 `Matrix4x4`（64 bytes）等类型需要 `ByRefToValueType`。

## 修复（Layer 1）

在 `ResolveAbiSlot` 中增加 size check：

```csharp
var abiSize = GetValueTypeNativeSizeForAbi(managedType);
if (abiSize > 16)
{
    return new AotCoreIrAbiSlotArtifact
    {
        CarrierKindCode = AotCoreIrAbiCarrierKind.ByRefToValueType,
        ...
    };
}
```

`GetValueTypeNativeSizeForAbi` 根据 SubjectId 识别大值类型：
- `Vector`1<T>`：T=int/float → 32 bytes, T=long/double → 64 bytes, T=byte/short → 16 bytes
- `Matrix4x4` → 64 bytes
- `Matrix3x2` → 24 bytes

## Fix（Layer 2）

### Opcode 短格式归一化

IL 解码器输出 `ldarg.0`、`ldc.i4.6`、`br.s` 等短格式 opcode 名称，但结构化 EH 线性降低器的 switch 只处理基础格式（`ldarg`、`ldc.i4`、`br`）。三处修复：

1. **IlBytecodeDecoder** — 源头归一化：`ldc.i4.s` → `ldc.i4`（全局），`br.s` → `br`（全局）
2. **AotCoreIrLowering.s_simpleInstructionCache** — 中间层归一化：ldarg.0 → ldarg + Operand=0（cache 命中时跳过短格式）
3. **ExceptionEmission.cs** — 最终端：3 处 switch 添加 br.s/brfalse.s/brtrue.s 变体

### Operand 类型扩展

`GetRequiredIntOperand` 原只检查 `operand is int`，但 BRIDGE-AOT 解码的 `ldc.i4.s` 操作数是 `sbyte`。扩展支持 sbyte/byte/short/ushort/long/ulong/uint。

### Extern 声明（Layer 3，部分完成）

BRIDGE-AOT 编译的方法引用 `chaos_external_runtime_*` 符号，但这些符号在生成的头文件中缺少 `extern "C"` 声明。修复思路：
- 常规符号：已通过 `_externalRuntimeSubjects` fallback 循环生成 `extern "C" void symbol()` 声明
- BRIDGE-AOT 注入符号：存在 `DummySigProvider` 类型命名不一致问题（`UInt32` vs `System.UInt32`），修复后影响 SubjectId 格式，导致 emission 阶段 crash

## 影响范围

| 指标 | 修复前 | 修复后 |
|------|--------|--------|
| Vector<T> ABI | ValueTypeByValue（值传递） | ByRefToValueType（引用传递） |
| Fact 通过率 | 112/121 | 112/121（不直接改变结果） |
| 构建时间 | ~37s | ~51s（更多 Vector 方法编译成功） |
| codegen failed | 有 opcode 报错 | 仅剩 Layer 3 链接错误 |

## 后续

- Layer 3（Extern 声明）：修复 `DummySigProvider` 命名一致性后补全 BRIDGE-AOT 注入符号的 extern 声明
- Layer 4（Runtime stubs）：为 `ThrowHelper.ThrowForUnsupportedNumericsVectorBaseType` 等提供 CHAOS_IL2CPP_FAIL 实现
