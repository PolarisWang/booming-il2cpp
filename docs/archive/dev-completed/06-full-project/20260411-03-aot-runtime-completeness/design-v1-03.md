# Design v1-03

## Batch 4 最小实现设计

### 目标

为 `Phase 2 / Batch 4` 建立一个可验证的 nested EH proof slice，使主线第一次具备以下最小能力：

- `NestedExceptionProof` subject 明确表达嵌套 `try/catch/finally`
- `SemanticWorldStage` 能识别 nested EH surface
- `LinkerStage` 能为 nested EH family 冻结 shape 入口
- `NativeReferenceLoweringPlanner` / `NativeReferenceProofEmitter` 能为 `NestedExceptionProof` 选择独立 lowering family
- native proof 模板能观察到“内层 catch 生效 + 内层 finally 生效 + 外层 finally 生效”的结果

### 设计范围

#### Subject

- 新增 `subjects/NestedExceptionProof/`
- `Program.cs` 包含：
  - `NestedExceptionHarness`
  - `ThrowInner()`
  - `Capture()` 中的嵌套 `try/catch/finally`
  - `NestedExceptionProofEntry.Run()`
- proof scaffold 复用现有 native-reference 结构

#### SemanticWorld

- 在现有 `requires-exception-handler` / `requires-finally` 之外，新加：
  - `requires-nested-exception-handler`
- 识别规则：
  - 方法存在 2 个及以上 exception region
  - 且至少存在一对 region 的 `try` 区间呈嵌套关系
- 这样不需要新增 contracts 字段，只利用现有 `ManagedExceptionRegionModel`

#### Linker

- 在 `BuildExceptionFacts(...)` 旁边新增 nested EH shape 判定
- 为具有 nested EH 形态的方法追加 `nested-throw-catch-finally` 事实入口
- 保持 `ExceptionFact` 合同不变，不扩展 artifact schema

#### CodeGen

- 新增 lowering family：
  - `managed-exceptions.nested-throw-catch-finally.minimal`
- 选择条件：
  - assembly 名称为 `NestedExceptionProof`
  - world capability 命中 `requires-exception-throw`
  - world capability 命中 `requires-exception-handler`
  - world capability 命中 `requires-finally`
  - world capability 命中 `requires-nested-exception-handler`
- 新增模板：
  - `NativeReferenceProof.NestedExceptionThrowCatchFinally.cpp.scriban`
- lowering plan 继续沿用现有 exception 计划字段：
  - `ConstructorSymbol`
  - `InstanceMethodSymbol`
  - `ThrowMethodSymbol`
  - `FinallyLiteral`
  - `ExpectedOutput`
- 不新增 `NativeReferenceLoweringPlanArtifact` 字段

#### Runtime

- 本轮不改 `runtime_core.h/.cpp` 公共表面
- 模板继续用 `ManagedExceptionCarrier`
- “finally” 语义仍由 proof 模板中的 scope 对象显式表达

### 验证策略

先写 RED 测试冻结以下边界：

- `NestedExceptionProof` subject 树存在
- source 明确包含嵌套 `try/catch/finally`
- semantic / linker / codegen 新 surface 存在
- nested EH 模板存在

然后补实现并运行：

- `python -m pytest tests/unit/run/test_phase2_nested_exception_proof.py -v`
- `python -m pytest tests/unit/run -v`
- `dotnet build solutions/core/windows/chaos-il2cpp-core.sln -c Debug`
- `dotnet build subjects/NestedExceptionProof/source/NestedExceptionProof.csproj -c Debug`

必要时再补：

- `cmake -S . -B artifacts/cmake-batch4-check -DROADMAP0_PRESET_TARGET=windows-x64-reference`
- `cmake --build artifacts/cmake-batch4-check --target chaos_subject_reference_proof --config Debug`

### 风险

- C# 编译器生成的 EH region 边界可能与手写直觉不同，因此 nested 判定必须使用“区间嵌套”而不是写死 region 数量
- 本轮只证明 nested `try/catch/finally`，并不代表 `filter` / `fault` 已经被 lowering family 覆盖
- 本轮不触及 reverse P/Invoke，Batch 4 还需要第二个切片继续补齐 marshaling
