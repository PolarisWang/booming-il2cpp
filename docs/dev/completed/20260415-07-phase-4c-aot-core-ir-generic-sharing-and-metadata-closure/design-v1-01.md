# Phase 4C Design v1.01

## 设计目标

- 为 `AotCoreIr` 建立最小但正式的 generic contract。
- 让 metadata closure 进入 typed carrier，而不是散落在 loader/runtime 私有结构里。
- 保持 Native AOT 生成链的职责边界：
  - `AotCoreIrLowering` 负责语义 lowering
  - `NativeAotLoweringPlanner` 负责模板模型规划
  - `NativeAotEmitter` 负责 Scriban 渲染

## 设计边界

- `Chaos.TestFramework` 继续位于 `src/reference/Chaos.TestFramework`，只作为被翻译的 managed dependency。
- `AotCoreIr` 不直接依赖测试框架，也不带 test-only 字段。
- metadata closure contract 只表达“需要什么”和“为什么需要”，不直接把 loader 的完整内部模型抬进 codegen。

## 最小 contract 方向

- generic owner identity
- generic type instantiation
- generic method instantiation
- shared body / exact body kind
- metadata closure role

这些 carrier 都应优先使用枚举或紧凑整数，而不是新字符串字段。

## proof 方向

- 优先复用 `subjects/SolutionCorePack/source/FeatureSlices/CoreRuntimeFeatures/GenericsAndCollections/`
- proof 继续使用 `ChaosUnitTest` / `Assert`
- 至少闭环一条 managed/native generic proof，再决定下一批 widening
