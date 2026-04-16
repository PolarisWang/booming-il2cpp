# Phase 4B Native AOT Planner + Scriban Emitter 设计

## 目标

- 将 Native AOT 生成链从单体 `NativeAotEmitter` 重构为：
  - `NativeAotLoweringPlanner`
  - `NativeAotEmitter`
  - `Templates/NativeAot*.scriban`
- 保持语义来源仍然是 `AotCoreIr`。
- 在新架构上继续承接 Phase 4B 剩余 object model/runtime surface widening。

## 设计原则

- 语义规划与文本渲染彻底分离。
- Planner 只做语义归一和模板模型构造，不做文本排版。
- Emitter 只做模板加载、渲染、产物写出，不重新理解 IL/AOT 语义。
- 模板按稳定结构切分，不按临时 proof 切分。

## 目标结构

### C# 侧

- `src/managed/Chaos.IL2CPP.CodeGen/NativeAotLoweringPlanner.cs`
  - 读取 `NativeAotLoweringPlanArtifact`
  - 读取 `AotCoreIrArtifact`
  - 收集 reachable methods
  - 构造 `NativeAotTemplateModel`
  - 归一 object model declarations、compatibility helpers、method bodies、entry wrapper

- `src/managed/Chaos.IL2CPP.CodeGen/NativeAotEmitter.cs`
  - 校验输入
  - 加载 planner 产出的 template model
  - 加载 Scriban 模板
  - 渲染 translation unit

- 视需要新增：
  - `NativeAotTemplateModels.cs`
  - `NativeAotTemplateRenderer.cs`

### 模板侧

- `src/managed/Chaos.IL2CPP.CodeGen/Templates/NativeAot.TranslationUnit.cpp.scriban`
- `src/managed/Chaos.IL2CPP.CodeGen/Templates/NativeAot.ObjectModel.cpp.scriban`
- `src/managed/Chaos.IL2CPP.CodeGen/Templates/NativeAot.Method.cpp.scriban`
- 如需要：
  - `NativeAot.EntryWrapper.cpp.scriban`
  - `NativeAot.HelperBlock.cpp.scriban`

## Template Model

### 顶层

- `includes`
- `helper_functions`
- `object_model`
- `method_declarations`
- `methods`
- `entry_wrapper`

### Object Model

- `reference_type_ids`
- `interface_type_ids`
- `boxed_type_ids`
- `reference_type_layouts`
- `value_type_layouts`
- `boxed_type_layouts`
- `static_fields`
- `compatibility_helpers`
- `managed_array_contract`

### Method

- `native_symbol`
- `return_type`
- `parameter_signature`
- `parameter_count`
- `local_count`
- `labels`
- `instructions`

### Instruction

- `offset`
- `opcode`
- `rendered_lines`
- `next_offset`
- `requires_label`

说明：
- 本轮允许 instruction 级 lowering 先在 planner 中归一成 `rendered_lines`。
- 这样可以先完成“架构双阶段 + Scriban 化”目标，不把每个 opcode 的局部模板拆分强行塞进同一轮。

## 执行顺序

1. 先把当前 `NativeAotEmitter` 中的可复用逻辑提炼为 planner 辅助函数。
2. 建立 `NativeAotTemplateModel`。
3. 引入 `NativeAotLoweringPlanner`。
4. 把 translation unit 顶层渲染切到 Scriban。
5. 把 object model declarations 与 method 框架切到 Scriban。
6. 在新生成链上继续补 `array covariance` 等剩余 4B 语义。

## 非目标

- 本轮不新增更多 Native AOT plan kinds。
- 本轮不重做 GC、layout、完整 ABI。
- 本轮不要求每个 opcode 都有独立模板文件。

## 验收标准

- Native AOT 主生成链变成 planner + emitter 双阶段。
- `NativeAotEmitter` 不再负责构造整份 translation unit 的字符串。
- 模板缺失/解析失败有明确错误。
- 迁移后当前 Phase 4B 已绿切片继续通过。
- `array covariance` 在新架构上推进，而不是绕回旧 builder 路径。
