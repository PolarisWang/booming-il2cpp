# Phase 5 Capability Batch A Design v1.01

Date: 2026-04-09

## 1. 设计目标

`Phase 5` 要解决的不是“再新增几个能跑的例子”，而是把首批最影响 generated native 工程质量上限的能力接到已经落地的完整 subject 主线上：

- `dispatch`
- `closed generics/layout`
- `arrays/boxing`

所有设计继续服从两个前提：

1. 当前主线仍然不是完整 `C# -> C++ total solution`。
2. 当前主线的一切工程取舍仍以 generated native 工程的 `C++` 性能优先。

## 2. 非目标

本阶段不做：

- `delegates/callbacks`
- `exceptions`
- 把 `engine binding` 提前插入
- 把 arrays 语义一次性扩到完整通用集合
- 重新拆出多个 probe subject 替代 `MainlineFeaturePack`

## 3. 总体方案

### 3.1 保持一个完整 subject

`MainlineFeaturePack` 继续作为唯一 full subject，不再为 Batch A 新建平行 subject。

### 3.2 通过 proof slice 承载 capability correctness

在 `MainlineFeaturePack/source/` 下增加多个 capability proof entry，例如：

- `DispatchProofEntry::Run()`
- `GenericLayoutProofEntry::Run()`
- `ArrayBoxingProofEntry::Run()`

这些 entry 只负责 proof correctness，不承担 trace CLI 入口职责，也不承载 perf harness 全部逻辑。

### 3.3 通过 matrix 级 entry override 选择 proof slice

manifest 默认 `source.entry` 继续指向当前基线 proof entry。

`environmentMatrices[]` 新增可选的 matrix 级 source entry override，用来覆盖 planner 最终送给 frontend/driver 的 entry subject id。设计目标不是引入新套件，而是在现有 `selection["source"]` 上做最小 overlay。

建议形状：

```json
{
  "matrixId": "windows-dispatch-check",
  "pipelineId": "proof-runtime-output",
  "source": {
    "entry": "MainlineFeaturePack/DispatchProofEntry::Run()"
  }
}
```

planner 行为：

- 先复制 manifest 根部 `source`
- 再用 matrix 内的 `source` 字段做浅覆盖
- worker 与 driver 继续只消费 `selection["source"]`

这样不会引入新的执行分叉，也最容易复用现有测试与 artifact root。

## 4. capability 设计

### 4.1 Dispatch

目标：

- 让 `callvirt` 不再只停留在 loader 可识别，而是能在 native lowering / runtime bridge 中形成真实语义路径。

实现策略：

- `Linker/OptimizationFacts` 继续输出 `DispatchFact`
- `CodeGen` 侧采用双路径：
  - 若 closed world facts 能证明 exact target，可直接 lower 为去虚 direct call
  - 若不能证明 exact target，则 lower 到 runtime virtual dispatch helper
- runtime 侧复用 `ResolveVirtualMethod` / `InvokeVirtual`

这样既满足 correctness，也把 `C++` 性能优先贯彻到可去虚场景。

### 4.2 Closed Generics / Layout

目标：

- 在完整 subject 内验证 closed generic instantiation 与 layout 访问不再依赖旧 probe subject 的局部形状。

实现策略：

- 尽量复用已有 `managed-generic.static-forwarder-captured-getter.minimal` 路线
- 通过新的 proof slice 把 closed generic type、field layout、value/reference layout 约束显式挂到 `MainlineFeaturePack`
- 让 `OptimizationFacts.LayoutFacts` 成为 codegen 选择与验证的显式输入，而不是从 subject shape 侧推断

### 4.3 Arrays / Boxing

目标：

- 补齐 Batch A 所需的最小数组与装箱语义，不扩写成完整数组系统。

首批范围建议：

- loader: `newarr`、`box`
- 如 proof slice 需要，再补 `stelem.ref` 与对应 unbox 路径
- runtime: 复用 `ArrayNew`、`BoxValue`、`UnboxValue`
- codegen: 新增一个专门的 arrays/boxing lowering family，不把它强行塞进现有 object/generic family

原因：

- arrays/boxing 的 helper、值语义与对象消息 shape 差异太大
- 强行复用现有 family 只会把后续 `Phase 6` 继续拖进 emitter/template 污染

## 5. subject 布局建议

建议在 `MainlineFeaturePack` 内新增：

- `source/DispatchProof.cs`
- `source/GenericLayoutProof.cs`
- `source/ArrayBoxingProof.cs`

validation / matrix 侧建议：

- proof correctness 为每个 capability 新增独立 check matrix
- 现有 `trace-platform` 继续保留，不为 Batch A 复制
- 现有 `perf.profile` 继续保留为统一 full-subject perf 入口，在其中增加按 capability 分场景的 perf harness 与 baseline evidence

也就是说：

- correctness slice 分开
- perf evidence 合并

这是当前最符合“性能优先 + 完整 subject 不分裂”的结构。

## 6. 需要改动的核心位置

### 6.1 Subject / Planner

- `build/toolchains/run/testing/subject_planner.py`
- `build/toolchains/run/testing/subjects.py`
- `tests/unit/run/test_subject_manifest_schema.py`
- `tests/unit/run/test_subject_planner.py`

目标：支持 matrix 级 `source.entry` override，并为 `MainlineFeaturePack` 新增 capability slice matrix。

### 6.2 Loader / Semantic / Linker

- `src/managed/Chaos.IL2CPP.Loader/LoaderStage.cs`
- `src/managed/Chaos.IL2CPP.SemanticWorld/SemanticWorldStage.cs`
- `src/managed/Chaos.IL2CPP.Linker/LinkerStage.cs`

目标：补齐 Batch A 所需 opcode / semantic facts / optimization facts。

### 6.3 CodeGen / Native Runtime

- `src/managed/Chaos.IL2CPP.CodeGen/NativeReferenceLoweringPlanner.cs`
- `src/managed/Chaos.IL2CPP.CodeGen/NativeReferenceProofEmitter.cs`
- `src/native/runtime-core/runtime_core.cpp`
- `src/native/bootstrap/bootstrap.cpp`

目标：为 dispatch 与 arrays/boxing 落地新的 lowering path 与 runtime helper 调用。

### 6.4 Subject Source / Validation

- `subjects/MainlineFeaturePack/source/**`
- `subjects/MainlineFeaturePack/subject.manifest.json`
- `subjects/MainlineFeaturePack/baselines/perf/**`

目标：提供 capability proof slice 与统一 perf evidence。

## 7. 验证策略

`Phase 5` 实施时至少需要三层验证：

1. planner/schema unit tests
2. Batch A focused unit tests（loader/linker/codegen）
3. `MainlineFeaturePack` subject runs

建议的最终执行形态：

- capability check matrices：
  - `dispatch`
  - `generic/layout`
  - `arrays/boxing`
- 统一 perf matrix：
  - `perf.profile`

## 8. 决策结论

推荐冻结以下设计：

- 一个 `MainlineFeaturePack`，不回退到多个 probe subject
- 多个 capability proof slice
- matrix-scoped `source.entry` override
- dispatch 采用“优先去虚，保留 runtime virtual fallback”
- arrays/boxing 独立 lowering family

## 9. 进入下一步前的确认

如果用户认可本设计，下一步应补 `plan-v1-01.md`，再按以下顺序进入实现：

1. planner/schema RED
2. MainlineFeaturePack capability slice RED
3. loader/linker/codegen/runtime incremental implementation
4. focused verification
