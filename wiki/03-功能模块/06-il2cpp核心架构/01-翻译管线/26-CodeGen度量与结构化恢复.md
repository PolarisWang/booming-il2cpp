# CodeGen 度量与结构化恢复

## 概述

Native AOT 主路径使用结构化控制流恢复（StructuredIR），而非 flat-goto 续流模型。CodeGen 度量系统在每个 subject 生成 `codegen/native-aot.codegen-metrics.json`，记录结构化恢复状态。

## Metrics 字段

| 字段 | 含义 |
|------|------|
| `structuredMethodCount` | 非 EH 方法中成功结构化恢复的数量 |
| `structuredExceptionBodyCount` | EH 方法中成功结构化恢复的数量 |
| `flatFallbackCount` | 退回到 flat-goto 的方法数（当前为 0） |
| `totalMethodCount` | 总方法数 |
| `structuredRecoveryRate` | `(structuredMethodCount + structuredExceptionBodyCount) / totalMethodCount` |

## 当前状态

所有 33 个有方法的 subject family 的 `structuredRecoveryRate` 为 **1.0 (100%)**。0 个 flat-goto fallback。

flat-goto 回退路径已被完全移除 — 任何方法必须通过结构化恢复才能发射。

## Flat-Fallback 消除模式

四种已知会导致结构化恢复失败的 P2-A2 模式已修复：

### Pattern 1: 多锁存循环（Multi-Latch Loop）

**问题：** `BuildLoop` 使用 `LatchIndices.Max()` 确定循环体范围，但多个 `continue` 语句会产生多个 latch。BodyRange 可能无法覆盖所有 latch 后的块。

**修复：** `BuildLoop` 改用 `BodyIndices.Max()` 计算 `bodyEnd`：

```csharp
int bodyEnd = Math.Max(maxLatchIdx, loopInfo.BodyIndices.Max());
```

使 `ConsumeBlocks` 能覆盖所有 latch 块。

**验证 Subject:** `Subject_0` — `for (int i=0; i<10; i++) { if (i%2==0) continue; if (i>5) continue; sum += i; }`

### Pattern 2: Switch-Case Fallthrough

**问题：** `BuildSwitch` 对空 case body（如 `case 0:` 后无代码直接落到 `case 1:`）产生无效输出，`EmitIRSwitch` 在空 case 后插入 `break;`。

**修复：** `BuildSwitch` 检测空 case body 并记录为 fallthrough（`FallthroughCaseValues` set）。`EmitIRSwitch` 检查 `FallthroughCaseValues.Contains(caseValue)`，跳过 break 插入。

```csharp
if (!IsControlFlowTerminator(body))
    fallthroughCaseValues.Add(caseKey);
```

**验证 Subject:** `Subject_1` — `switch(x) { case 0: case 1: result=10; break; case 2: result=20; break; }`

### Pattern 3: 深度 EH 嵌套（Deep EH Nesting）

**问题：** `TryBuildStructuredMethodBody` 在 5 种预定义 EH 形状都不匹配时返回 `false`，导致生成空函数体（broken output）。

**修复：** EH 形状匹配失败时回退到 `IRFlatRegion` 包装完整的 IL 指令范围，确保不会产生空函数体：

```csharp
// When all 5 EH shapes fail:
body = new IRSequence([
    new IRFlatRegion(instructions, exceptionPartitions),
    rest
]);
return true;
```

**验证 Subject:** `Subject_2` — `try { try { } finally { try { } catch { } } } catch { try { } finally { } }`

### Pattern 4: Finally 控制流（Finally Control Flow）

**问题：** `TryCreateFinallyHandlerEmissionPlan` 仅接受 `endfinally` 作为 guard 目标，但锁展开模式中 finally block 顶端可能有 forward-branch guard。

**修复：** 当 guard target 不是 `endfinally` 时，检查 target 是否在 finally block 范围内：

```csharp
int guardTarget = GetRequiredIntOperand(guardInst);
int endFinallyOffset = GetRequiredIlOffset(instructions[^1]);
if (guardTarget == endFinallyOffset || /* forward target in range */)
    return plan with { GuardTargetsEndFinally = ... };
```

**验证 Subject:** `Subject_3` — `try { if (x>0) return 1; else return 0; } finally { var t = x; }`

## 验证方式

- Foundation-dll 全量管线：`35/35 PASS`（含 codegen-edge-cases family）
- 每个 subject 的 `codegen-metrics.json` 中 `structuredRecoveryRate == 1.0`
- 快照测试：`53/53` 通过

## 关键文件

| 文件 | 职责 |
|------|------|
| `NativeAotLoweringPlanner.StructuredControlFlow.cs` | BuildLoop、BuildSwitch、BuildIfThenElse |
| `NativeAotLoweringPlanner.StructuredIR.cs` | TryBuildStructuredMethodBody、EmitIRSwitch |
| `NativeAotLoweringPlanner.ExceptionShapePlanning.cs` | 5 种 EH shape 匹配、FinallyHandlerEmissionPlan |
| `NativeAotLoweringPlanner.MethodEmission.cs` | EmitViaStructuredIR 入口 |
| `NativeAotLoweringPlanner.ObjectModelEmission.cs` | TypeInfoV0 发射、codegen-metrics 生成 |
| `CodeGenStage.cs` | CodegenMetrics 收集、LoweringPlan 创建 |
