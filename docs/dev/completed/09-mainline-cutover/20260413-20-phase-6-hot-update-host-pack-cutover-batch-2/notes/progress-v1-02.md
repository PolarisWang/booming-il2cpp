# Progress v1.02

日期：2026-04-13 03:47:47 +08:00

## 本轮完成

- 新建 `subjects/HotUpdateHostPack/`，把 hot-update host proof/benchmark roots 合并为 canonical source bundle。
- 新增 declared unit entries：
  - `HotUpdateSkeletonProofEntry`
  - `MethodReplacementProofEntry`
  - `VersionRollbackProofEntry`
- 新增 declared benchmark entries：
  - `HotUpdateDispatchBenchmarkEntry`
  - `HotUpdateLoadBenchmarkEntry`
  - `HotUpdateRoundtripBenchmarkEntry`
- 把以下 roots 退化为 compatibility shell：
  - `BenchHotUpdateDispatch`
  - `BenchHotUpdateLoad`
  - `BenchHotUpdateRoundtrip`
  - `HotUpdateSkeletonProof`
- 对齐 planner/schema/discovery/performance/runtime 相关测试，修正 managed-output query 对 redirect shell 的 source-owner 断言。

## 验证

- `python -m pytest tests/unit/performance/test_benchmark_subject_sources.py tests/unit/planning/test_subject_planner.py tests/unit/compatibility/test_subject_manifest_schema.py tests/integration/registry/test_declared_metadata_discovery.py tests/unit/run/test_phase5_hot_update_skeleton.py -q`
  - `72 passed`
- `python -m pytest tests/unit/run/test_phase9_hot_update_e2e.py -q`
  - `6 passed`
- `python -m pytest -q`
  - `756 passed, 40 skipped`

## 结论

- Phase 6 Batch 2 可以归档。
- Phase 6 下一步应切到 mixed execution / interpreter family，把 `BenchMixed` 与 `Interpreter*` / `MixedExecutionProof` 收束到同一个 canonical feature pack。
