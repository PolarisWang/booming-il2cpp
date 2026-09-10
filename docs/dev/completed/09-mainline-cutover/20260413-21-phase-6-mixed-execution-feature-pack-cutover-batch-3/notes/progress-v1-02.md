# Progress v1.02

日期：2026-04-13 04:18:00 +08:00

## 本轮完成

- 新建 `subjects/MixedExecutionFeaturePack/`，把 mixed execution / interpreter family 收敛为 canonical source bundle。
- 新增 declared unit entries：
  - `MixedExecutionProofEntry`
  - `InterpreterLoweringProofEntry`
- 新增 declared benchmark entry：
  - `MixedExecutionBenchmarkEntry`
- 在 canonical bundle 内保留 `InterpreterArithmeticProof` support project，并改为由 `MixedExecutionFeaturePack` 运行时解析加载。
- 把以下 roots 退化为 compatibility shell：
  - `BenchMixed`
  - `MixedExecutionProof`
  - `InterpreterArithmeticProof`
  - `InterpreterLoweringProof`
- 对齐 performance / schema / planner / discovery / runtime 相关测试，并修复 canonical pack 编译时的 support-project 重复编译冲突。

## 验证

- `dotnet build subjects/MixedExecutionFeaturePack/source/MixedExecutionFeaturePack.csproj -c Release -m:1`
  - 通过
- `python -m pytest tests/unit/performance/test_benchmark_subject_sources.py tests/unit/compatibility/test_subject_manifest_schema.py tests/unit/planning/test_subject_planner.py tests/integration/registry/test_declared_metadata_discovery.py -q`
  - `74 passed`
- `python -m pytest tests/unit/run/test_phase7_interpreter_mixed_execution.py -q`
  - `11 passed`
- `python -m pytest -q`
  - `771 passed, 40 skipped`

## 结论

- Phase 6 Batch 3 可以归档。
- 父 roadmap 的下一步应切入 Phase 7，把 compiled catalog、selector、CLI、artifact、report 接成统一主入口，然后再做 Phase 8 兼容层收尾。
