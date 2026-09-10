# Post Phase 7 Deferred AOT Shape And Entry ABI Expansion Roadmap v1.01

Date: 2026-04-15 23:52:48 +08:00
Status: completed
Completed At: 2026-04-16 00:48:20 +08:00

## 1. 目标

在 `20260415-11` 已完成的 typed-IL translation expansion 基础上，继续收口上一轮明确 deferred 的 AOT surface，重点覆盖：

- `byref` 参数、返回值、局部和 carrier contract
- widened method shape、wrapper / thunk shape
- entry ABI normalization
- `generic + byref + value-type` mixed dispatch / invocation
- `NativeAotEmitter + Scriban` 对 widened shape 的稳定输出

本 roadmap 的完成标准不是抽象声明“能力扩大”，而是把这批 deferred surface 拆成有 owner、有证据、有项目级验证的六个 child task，并在归档时给出完整闭环证据。

## 2. 最终结果

- `20260415-19` 至 `20260415-24` 六个 child task 全部完成并归档。
- loader / IR / planner / emitter / evidence 已形成真实闭环，不再停留在父 roadmap 的待办状态。
- widened codegen 仍然固定为 `AotCoreIr -> NativeAotLoweringPlanner -> NativeAotEmitter + Scriban`。
- `Chaos.TestFramework` 没有混入 `AotCoreIr`、planner、emitter 的底层语义实现。
- 没有继续扩大 subject 内部调度的 string 协议面。

## 3. 执行映射

### Phase 0: Deferred Surface Freeze And Contract Ledger

- child task: `20260415-19-phase-0-deferred-surface-freeze-and-contract-ledger`
- outcome:
  - 冻结 deferred shape ledger
  - 固定 capability family、owner、evidence、unsupported reason 与 child-task mapping

### Phase 1: Byref And Method Shape Contract Expansion

- child task: `20260415-20-phase-1-byref-and-method-shape-contract-expansion`
- outcome:
  - 扩大 loader / metadata / `AotCoreIr` 对 `byref` 与 widened method shape 的表达能力

### Phase 2: Entry ABI Normalization And Planner Widening

- child task: `20260415-21-phase-2-entry-abi-normalization-and-planner-widening`
- outcome:
  - 扩大 `NativeAotLoweringPlanner` 对 entry ABI、wrapper、thunk、hidden receiver、return carrier 的统一规划能力

### Phase 3: Generic Byref Mixed Dispatch Closure

- child task: `20260415-22-phase-3-generic-byref-mixed-dispatch-closure`
- outcome:
  - 收口 `generic + byref + value-type` 在 dispatch / invocation / wrapper 路径上的真实 lowering

### Phase 4: Native AOT Emitter And Scriban Template Widening

- child task: `20260415-23-phase-4-native-aot-emitter-and-scriban-template-widening`
- outcome:
  - widened plan model 已真实落到 `NativeAotEmitter + Scriban`

### Phase 5: Owner Evidence, Benchmark, And Gate Closure

- child task: `20260415-24-phase-5-owner-evidence-benchmark-and-gate-closure`
- outcome:
  - owner proof / benchmark / gate / reporting 已能正确承载 widened surface 的工程级证据

## 4. 子任务状态表

| task_id | phase | status | purpose | depends_on |
| --- | --- | --- | --- | --- |
| `20260415-19-phase-0-deferred-surface-freeze-and-contract-ledger` | `Phase 0` | `completed` | 冻结 deferred shape ledger，并固定 capability family、owner、证据与 child mapping | `20260415-18` |
| `20260415-20-phase-1-byref-and-method-shape-contract-expansion` | `Phase 1` | `completed` | 扩大 loader / metadata / `AotCoreIr` 对 `byref` 与 widened method shape 的支持 | `20260415-19` |
| `20260415-21-phase-2-entry-abi-normalization-and-planner-widening` | `Phase 2` | `completed` | 扩大 planner 对 entry ABI、wrapper、thunk、hidden receiver、return carrier 的统一规划 | `20260415-20` |
| `20260415-22-phase-3-generic-byref-mixed-dispatch-closure` | `Phase 3` | `completed` | 收口 `generic + byref + value-type` mixed dispatch / invoke lowering | `20260415-21` |
| `20260415-23-phase-4-native-aot-emitter-and-scriban-template-widening` | `Phase 4` | `completed` | 让 widened plan model 落到 `NativeAotEmitter + Scriban` | `20260415-22` |
| `20260415-24-phase-5-owner-evidence-benchmark-and-gate-closure` | `Phase 5` | `completed` | 用 owner subject / proof / benchmark / gate 收口 widened surface 的证据 | `20260415-23` |

## 5. 验证结果

- roadmap 相关验证：
  - `python -m pytest tests/unit/compatibility tests/integration/registry/test_declared_metadata_discovery.py tests/unit/performance/test_native_aot_workload_entry_bundle.py tests/unit/reporting tests/unit/registry`
  - 结果：`274 passed`
- 项目级验证：
  - `python -m pytest -q`
  - 结果：`713 passed, 40 skipped`

## 6. 归档结论

- 本 roadmap 已满足 completed 条件：
  - child task 全部完成
  - roadmap 相关回归通过
  - 项目级测试套件通过
  - `docs/dev` 索引与归档状态已对齐
- 如后续继续扩展 deferred typed-IL / ABI / hybrid execution surface，应另起新的 follow-up roadmap，而不是复用本任务。
