# 进度记录 v1-04

## 时间

- 2026-04-06 10:16:32 +08:00

## 已完成

- 为 `emit-native-reference` 补充回归测试，先锁定 `Chaos.IL2CPP.CodeGen` 必须接入 Scriban 且仓库内存在 `.scriban` 模板资产。
- 在 `src/managed/Chaos.IL2CPP.CodeGen/Chaos.IL2CPP.CodeGen.csproj` 中接入 vendored `third_party/scriban/src/Scriban/Scriban.csproj`。
- 新增 `src/managed/Chaos.IL2CPP.CodeGen/Templates/NativeReferenceProof.cpp.scriban`，把 Stage 4 generated C++ translation unit 改为由 Scriban 模板渲染。
- 重构 `src/managed/Chaos.IL2CPP.CodeGen/NativeReferenceProofEmitter.cs`，改为 `Template.Parse` + `TemplateContext` + `ScriptObject` 的模板渲染路径，并补充模板缺失或解析失败时的显式错误。
- 回归验证通过：
  - `python -m unittest tests.unit.run.test_stage4_native_reference_proof`
  - `python -m unittest tests.unit.run.test_stage3_managed_minimal_closure`

## 当前状态

- 任务状态：`in_progress`
- 当前阶段：`implementation`

## 下一步

- 等待 `cmake` 可用后继续 Chunk 3，进入 `src/native/runtime-core/` 的最小 ABI 骨架与 native compile 验证。
