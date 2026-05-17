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
