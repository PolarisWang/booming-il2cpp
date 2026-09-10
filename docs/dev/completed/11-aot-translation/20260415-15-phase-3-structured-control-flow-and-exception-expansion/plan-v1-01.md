# Phase 3 Structured Control Flow And Exception Expansion 实现计划

> 面向执行 Agent：必须使用 `RED -> GREEN -> REFACTOR` 推进，并同步维护 `STATUS.md`、`docs/dev/ACTIVE.md` 与 `notes/progress-*.md`。

## 目标

- 把 native AOT 的 structured EH / control-flow 从 catch-only 最小形状推进到 retained owner proof 真正需要的 finally/filter 形状
- 保持 Phase 3 与 Phase 4 的边界清晰，不在本阶段混入 dispatch widening

## 设计文档

- `docs/dev/in-progress/20260415-15-phase-3-structured-control-flow-and-exception-expansion/design-v1-01.md`

## 涉及文件与职责

- `subjects/SolutionCorePack/source/FeatureSlices/CoreRuntimeFeatures/ExceptionsAndControlFlow/FinallyAndFilterProof.cs`
- `src/managed/Chaos.IL2CPP.CodeGen/NativeAotLoweringPlanner.cs`
- `tests/unit/compatibility/` 下新增的 Phase 3 real-driver tests

## 执行步骤

- [ ] Task 1: 用 `FinallyAndFilterProofEntry::Run()` 锁定 RED
  - 新增 native AOT driver test
  - 记录当前 dispatch 噪音导致的失败形态
- [ ] Task 2: 收敛 owner proof，去掉与 Phase 3 无关的 dispatch 噪音
  - filter 条件改为 dispatch-free
  - 保持 finally/filter 的真实语义验证不丢失
- [ ] Task 3: 扩大 planner 的 structured EH / control-flow lowering
  - 补齐 finally/filter/leave/endfinally 所需最小 contract
  - 保持 `AotCoreIr -> NativeAotLoweringPlanner -> NativeAotEmitter + Scriban` 单向收口
- [ ] Task 4: 跑完验证并决定是否继续推进 `switch` 批次
  - targeted suites
  - 完整 `tests/unit/compatibility -q`
  - 更新父 roadmap / index / notes

## 本轮验证

- 尚未开始；先做 RED。
