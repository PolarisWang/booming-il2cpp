# Phase 7 Engine Binding Tail Roll In 实现计划

> 目标：把 `engine binding` 作为 roadmap 最后阶段正式接入主线，采用三个最小 engine subject 和最小 engine-aware planning/reporting 增量，完成 `Phase A` 的尾阶段收口。
>
> 面向执行：使用 `executing-plans` 按 TDD 推进；所有状态同步写入 `STATUS.md`、`docs/dev/ACTIVE.md`、`notes/progress-*.md`。

**设计文档：** `docs/dev/in-progress/20260410-12-phase-7-engine-binding-tail-roll-in/design-v1-01.md`

**预期知识沉淀：** `wiki/runtime/phase-7-engine-binding-tail-roll-in.md`

## 任务清单

- [ ] 任务 1：冻结 engine subject / planner / report surface
  - 产物：
    - 新增或扩展 unit tests，覆盖：
      - `engineProofProfile`
      - `generated-engine-proof` / `runtime-engine-observe` / `runtime-engine-trace-compare`
      - `engineContractSummary` / `engineEmissionSummary` / `engineObservationSummary` / `engineProofSummary`
      - 三个 engine subject 的 manifest / planner surface
  - 验证：
    - `python -m unittest ...` 对应 engine surface 测试先 RED 再 GREEN

- [ ] 任务 2：接入 `EngineBindings` / `HostBindings` 及 summary contract
  - 产物：
    - `ManagedClosureContracts.cs`
    - `LinkerStage.cs`
    - `NativeReferenceLoweringPlanner.cs`
    - `NativeReferenceProofEmitter.cs`
    - subject reporting / executor / worker 相关实现
  - 目标：
    - `LoweringPlan` 有正式 `EngineBindings` / `HostBindings`
    - analysis/generated/runtime/report 四层 summary 只做摘要，不重复真源

- [ ] 任务 3：实现三个最小 engine subject 与最小 runtime/codegen 支撑
  - 产物：
    - `subjects/EngineLogWriteLite/*`
    - `subjects/EngineObjectHandleLite/*`
    - `subjects/EngineLifecycleCallbackLite/*`
    - 必要的 codegen template / native runtime glue / bootstrap helper
  - 目标：
    - `service-call`、`object-handle`、`lifecycle-callback` 三类 engine 语义各自具备最小 proof 闭环

- [ ] 任务 4：跑 focused correctness / trace，并回写父 roadmap
  - 验证：
    - 三个 subject 的 `correctness.dev`
    - 三个 subject 的 `correctness.platform`
    - 必要的 engine unit / reporting / command / planner suites
  - 收口：
    - 回写 Phase 7 child 与父 roadmap 状态
    - 明确 `Phase A` 已完成，但这仍不等价于 total solution 完成
