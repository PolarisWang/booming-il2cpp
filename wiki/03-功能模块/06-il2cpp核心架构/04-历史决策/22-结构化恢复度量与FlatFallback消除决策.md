# 22-结构化恢复度量与 Flat-Fallback 消除决策

## 背景

P2-A1 实现了结构化恢复率度量（`codegen-metrics.json`）。P2-A2 识别并修复了 4 种 flat-fallback 模式。本文档记录相关决策。

## 决策

### D1: 100% StructuredRecoveryRate 为硬契约

所有方法必须通过结构化控制流恢复才能发射。`structuredRecoveryRate != 1.0` 为 blocking issue。

**Why:** flat-goto 回退路径（`label/goto` 续流模型）已被完全移除。任何 flat-fallback 都会导致方法无法正常发射，生成空函数体或 ICE。

**How to apply:** CI 管线应检查 `codegen-metrics.json` 的 `structuredRecoveryRate == 1.0`。新增 subject 时确保不引入新的 flat-fallback 模式。

### D2: 度量驱动消除

不是预判所有可能的 flat-fallback 模式，而是通过 CI 度量收集 → 修复 → 验证的循环逐步消除。

**Why:** 结构化恢复的失败模式受 IL 编译器输出模式影响，无法通过静态分析全覆盖。度量驱动方式更务实。

**How to apply:** 每次 CI 运行收集 `codegen-metrics.json`，当 `flatFallbackCount > 0` 时触发分析 → 修复流程。

### D3: EH Flat-Fallback 使用 IRFlatRegion 而非扩展 Shape 匹配

当 EH 形状不匹配 5 种预定义形状时，回退到 `IRFlatRegion` 而非新增 EH shape。

**Why:** EH shape 的匹配模式随 IL 编译器版本和优化开关可能变化。`IRFlatRegion` 提供语义正确的降级路径，保持生成代码可执行。新增 EH shape 应基于真实覆盖率数据。

**How to apply:** `TryBuildStructuredMethodBody` 中所有 EH 形状匹配失败时，构建 `IRFlatRegion` 包装完整 IL 指令范围。

### D4: 不引入新的转换阶段

所有模式修复直接在 `StructuredControlFlow.cs` / `StructuredIR.cs` / `ExceptionShapePlanning.cs` 中完成，不新增结构化恢复处理的中间阶段。

**Why:** 结构化恢复的设计是单通道 CFG → StructuredIR 构建。引入额外转换阶段会破坏 BuiltBlocks → BuildControlFlowGraph → RecoverStructure 的单向管道性质。

**How to apply:** 新 flat-fallback 模式在现有 RecoverStructure/ExceptionShape 阶段内修复。

### D5: 结构化 Slot 栈深度不足为 Blocking Issue

结构化 IR 成功恢复但 C++ 发射阶段因 slot 深度不足失败的，与 flat-fallback 同等对待（blocking）。

**Why:** 4 个 methods（ArithmeticCompareHelper.RunCompare、BranchCompareBHelper.RunBranchCompareB、BranchDupHelper.RunBranchDup、LdftnHelper.GetFnPtr）通过结构化恢复但 `EmitIRIfThenElse` 中 `PopValue()` 抛出 `InvalidOperationException("structured slot stack underflow.")`，退回到 stub 发射。这些方法不会被 `structuredRecoveryRate` 捕获。

**How to apply:** 在 `BuildMethodSourceSafe` 中捕获 `InvalidOperationException` 且消息含 `"slot stack"` 时，记录到新的 `slotStackFailureCount` 度量。

### D6: PreConditionDepth + PostConditionDepth 修复 Slot 深度

EmitIRIfThenElse 的条件指令段（condBlock）需要正确的前驱深度和条件后残余深度来初始化 slot 上下文。

**Why:** 
- 条件指令以 `stloc` 开头时需要 pop 前驱 CFG 块 push 的值，但深度为 0（前驱的 push 发生在不同 IR 节点）
- 条件指令可能 push 多于 terminator pop 的值（如 `ldsfld + dup + brtrue`），else/postMerge body 的 `RestoreDepth` 必须使用残余深度而非前驱深度

**How to apply:** `ComputePreConditionDepth` 分析 condBlock 前驱的 net pushes，`postConditionDepth = preConditionDepth + condPushes - condPops - termPops`，else/postMerge body 使用 `postConditionDepth`。

## A1+A2 架构集成

A1（typed dispatch table header）和 A2（proxy wrapper source）是 codegen 输出的补充文件，不参与结构化恢复度量。

| 文件 | 内容 |
|------|------|
| `chaos_generated_module.h` | A1: 按类型分组的 typed function pointer 表 + A2: proxy wrapper 静态内联类 |
| `chaos_generated_module.cpp` | extern "C" 符号接线 + kFunctions 表 + ChaosGeneratedModuleActivate |

**生成方式:** `BuildGeneratedModuleHeader()` / `BuildGeneratedModuleSource()` 通过 Scriban 模板 `NativeAot.GeneratedModule.h.scriban` / `NativeAot.GeneratedModule.cpp.scriban` 渲染。

**触发时机:** 在 `NativeAotLoweringPlanner.Create()` 中调用，内容存入 `NativeAotTemplateModel.GeneratedModuleHeaderContent` / `GeneratedModuleSourceContent`，由 `NativeAotEmitter.BuildGeneratedSources()` 写入文件。

**CMake 接入:** CMakeLists.txt 需通过 `file(GLOB ... chaos_generated_module.cpp)` 将新源文件加入编译。

**运行时:** `ChaosRuntimeHost`（RAII 类，`chaos_runtime_host.h`）提供 `Initialize()` + `RegisterModule()`，`ChaosGeneratedModuleActivate` 将 codegen 符号注册到运行时。
