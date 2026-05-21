# Progress v1.01

日期：2026-04-13 02:52:33 +08:00

## 本轮启动

- 承接 Phase 5 Batch 4 的归档结果，切入 Phase 6 主线。
- 本批先冻结 pure runtime benchmark 与 hot-update / mixed execution roots 的归宿边界。
- 当前建议优先处理的 pure runtime benchmark roots：
  - `BenchArithmetic`
  - `BenchAllocation`
  - `BenchDispatch`
  - `BenchGeneric`
- 暂缓直接处理：
  - `BenchHotUpdateDispatch`
  - `BenchHotUpdateLoad`
  - `BenchHotUpdateRoundtrip`
  - `BenchMixed`
  - `HotUpdateSkeletonProof`
  - `MethodReplacementProof`
  - `VersionRollbackProof`
  - `MixedExecutionProof`
  - `InterpreterArithmeticProof`
  - `InterpreterLoweringProof`
- 目标是先把最清晰的一批 pure runtime benchmark roots 纳入 declared benchmark 模型，再回头处理 host/patch/mixed execution 的复杂交叉语义。
