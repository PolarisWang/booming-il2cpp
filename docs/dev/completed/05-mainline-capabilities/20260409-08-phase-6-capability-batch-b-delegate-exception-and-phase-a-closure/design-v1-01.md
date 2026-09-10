# Phase 6 Capability Batch B Design v1.01

Date: 2026-04-09

## 1. 设计目标

`Phase 6` 的目标不是继续扩大“能跑的例子集合”，而是把 `Phase A` 主线里最后一批强制能力接回完整 `MainlineFeaturePack`：

- `delegates/callbacks`
- `exceptions`
- 必要的 `reflection/interop` 补位

所有设计继续服从两个前提：

1. 当前主线仍然不是完整 `C# -> C++ total solution`
2. 当前主线的一切工程取舍仍以 generated native 工程的 `C++` 性能优先

## 2. 非目标

本阶段不做：

- `engine binding`
- multicast delegates 全量支持
- 完整 CLR exception system
- 完整 reflection surface
- 完整 interop surface
- 为 `Phase 6` 再拆平行 subject

## 3. 总体方案

### 3.1 继续保持一个完整 subject

`MainlineFeaturePack` 继续作为唯一 full subject。

`Phase 6` 不再重新启用 `ReflectionLite` / `PInvokeLite` 作为主执行面，而是把必要能力接回 `MainlineFeaturePack` 的 proof slice 与 perf 主线。

### 3.2 继续通过 proof slice 承载 correctness

建议新增以下 proof entry：

- `DelegateProofEntry::Run()`
- `ExceptionProofEntry::Run()`
- `ReflectionInteropClosureEntry::Run()`

这些 entry 只承担 focused correctness，不替代统一 perf 入口。

### 3.3 继续复用 matrix-scoped `source.entry` override

`Phase 5` 已经引入并验证 matrix 级 `source.entry` override。

`Phase 6` 不再重新设计入口选择机制，而是直接复用：

- correctness slice 分矩阵覆盖
- perf 继续走统一 `perf.profile`

## 4. Capability 设计

### 4.1 Delegates / Callbacks

目标：

- 让 delegate object、target capture 与 invoke 路径在 generated native 中形成真实语义
- 在 exact-target 场景优先退化成 direct call，而不是强制所有 invoke 都走 runtime dispatch

建议首批覆盖：

- closed static delegate
- closed instance delegate
- delegate parameter pass-through
- delegate field capture + invoke

设计策略：

- `SemanticWorld` / `Linker` 输出 delegate invocation 所需的 target / invoke facts
- `CodeGen` 侧分两类：
  - exact-target 可证明时，lower 成 direct call 或薄包装 direct call
  - 不能证明时，lower 到 runtime delegate invoke helper

这样可以把 hot path 尽量压到静态直调。

### 4.2 Exceptions

目标：

- 让 `throw / catch / finally` 在 generated native 工程中形成最小真实语义
- 避免为了异常语义把每个普通调用都强行变成显式状态传播链

推荐策略：

- 采用 cold-path exception carrier + generated native catch/finally glue
- 正常路径保持 direct call / direct return
- 只有 throw edge 才进入 exception runtime helper

不推荐策略：

- 把所有调用都包装成 `status + payload` 传播

原因：

- 这会直接污染 hot path
- 与“generated native `C++` 性能优先”冲突

因此，本阶段的异常设计应优先保证：

- 不抬高非异常路径成本
- 可以在 proof slice 里明确观察 `throw / catch / finally`

### 4.3 Reflection / Interop Closure 补位

`Phase 6` 不把 reflection / interop 作为独立主线，而是作为 `Phase A closure` 补位项处理。

建议范围：

- reflection：闭型类型查询、基础成员查询
- interop：最小 `DllImport` 直调能力

设计原则：

- 只补足 `MainlineFeaturePack` 里的 closure 证据
- 复用现有 minimal lowering family 与 runtime helper
- 不扩写成新的独立能力批次

## 5. Subject 布局建议

建议在 `MainlineFeaturePack/source/` 下新增：

- `DelegateProof.cs`
- `ExceptionProof.cs`
- `ReflectionInteropClosureProof.cs`

validation / matrix 侧建议：

- `windows-delegate-check`
- `windows-exception-check`
- `windows-reflection-interop-closure-check`

perf 继续保留：

- `windows-native-profile`

也就是说：

- correctness 分开
- perf 合并

## 6. 需要改动的核心位置

### 6.1 Subject / Planner

- `build/toolchains/run/testing/subject_planner.py`
- `tests/unit/run/test_subject_manifest_schema.py`
- `tests/unit/run/test_subject_planner.py`

目标：为 `Phase 6` 新增 focused matrices 与 proof slice entry。

### 6.2 Loader / Semantic / Linker

- `src/managed/Chaos.IL2CPP.Loader/LoaderStage.cs`
- `src/managed/Chaos.IL2CPP.SemanticWorld/SemanticWorldStage.cs`
- `src/managed/Chaos.IL2CPP.Linker/LinkerStage.cs`

目标：补足 delegates / exceptions 所需的 opcode、semantic facts 与 optimization facts。

### 6.3 CodeGen / Runtime

- `src/managed/Chaos.IL2CPP.CodeGen/NativeReferenceLoweringPlanner.cs`
- `src/managed/Chaos.IL2CPP.CodeGen/NativeReferenceProofEmitter.cs`
- `src/native/runtime-core/runtime_core.cpp`
- `src/native/bootstrap/bootstrap.cpp`

目标：

- delegate invoke exact-target / fallback 双路径
- exception cold-path carrier 与 catch/finally glue
- reflection / interop closure 的最小复用

### 6.4 Subject Source / Validation

- `subjects/MainlineFeaturePack/source/**`
- `subjects/MainlineFeaturePack/subject.manifest.json`
- `subjects/MainlineFeaturePack/baselines/perf/**`

目标：为 `Phase 6` 提供 proof slice 与统一 perf evidence。

## 7. 验证策略

`Phase 6` 实施时至少需要四层验证：

1. planner / manifest schema unit tests
2. `Phase 6` focused unit tests（loader / linker / codegen / runtime）
3. `MainlineFeaturePack` focused subject correctness
4. 统一 `perf.profile`

建议的 focused run 形态：

- `windows-delegate-check`
- `windows-exception-check`
- `windows-reflection-interop-closure-check`
- `perf.profile`

## 8. 设计结论

推荐冻结以下设计：

- 继续只用一个 `MainlineFeaturePack`
- `delegates` 采用“能直调则直调，不能直调再走 runtime helper”
- `exceptions` 采用“cold-path exception carrier，不污染正常路径”的策略
- `reflection/interop` 只做 `Phase A closure` 所需的最小补位
- `engine binding` 继续留在 `Phase 7`

## 9. 进入下一步前的确认

如果用户认可本设计，下一步应补 `plan-v1-01.md`，再按以下顺序进入实现：

1. planner / manifest RED
2. delegate / exception / reflection-interop closure proof slice RED
3. loader / linker / codegen / runtime incremental implementation
4. focused verification
5. `Phase A closure` 回写与 follow-up 缺口整理
