# Progress v1.01

日期：2026-04-13 03:47:47 +08:00

## 本轮启动

- 承接 Phase 6 Batch 2 的归档结果，开始 mixed execution / interpreter family 的最终收口。
- 本批先冻结 roots 边界：
  - `BenchMixed`
  - `MixedExecutionProof`
  - `InterpreterArithmeticProof`
  - `InterpreterLoweringProof`
- 设计选择：
  - 新建 canonical subject：`MixedExecutionFeaturePack`
  - canonical pack 承载 declared mixed/interpreter proof + benchmark entries
  - `InterpreterArithmeticProof` 作为 canonical bundle 内 support project 保持程序集 identity
  - legacy roots 退化为 compatibility shell，保留旧 subject id 与 pipeline/baseline 语义
