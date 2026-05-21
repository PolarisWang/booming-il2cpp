# Progress v1.02

日期：2026-04-13 01:59:08 +08:00

## 本轮完成

- 在 `subjects/MainlineFeaturePack/source/` 中新增 5 个 runtime proof slice，并全部使用真实 `ChaosUnitTest(...)` 标注：
  - `AsyncAwaitProofEntry`
  - `ThreadingProofEntry`
  - `NestedExceptionProofEntry`
  - `GenericCollectionProofEntry`
  - `DelegateChainProofEntry`
- 扩展 `subjects/MainlineFeaturePack/subject.manifest.json`：
  - 新增 5 个 `validationProfiles`
  - 新增 5 个 matrix source entry overlay
- 将以下 legacy roots 退化为 compatibility shell：
  - `AsyncAwaitProof`
  - `ThreadingProof`
  - `NestedExceptionProof`
  - `GenericCollectionProof`
  - `DelegateChainProof`
- 对齐 shell manifest 语义：
  - canonical `source.path`
  - canonical `source.entry`
  - `compatibility.redirectToSubject=MainlineFeaturePack`
  - `testDeclarationMode=none`
- 完成旧 phase2 tests、schema、planner、declared metadata discovery 的批次回归。

## 验证结果

- `python -m pytest tests/unit/run/test_phase2_async_await_proof.py -q` -> `3 passed`
- `python -m pytest tests/unit/run/test_phase2_threading_gc_proof.py -q` -> `3 passed`
- `python -m pytest tests/unit/run/test_phase2_nested_exception_proof.py -q` -> `3 passed`
- `python -m pytest tests/integration/registry/test_declared_metadata_discovery.py -q` -> `5 passed`
- `python -m pytest tests/unit/compatibility/test_subject_manifest_schema.py -q` -> `17 passed`
- `python -m pytest tests/unit/planning/test_subject_planner.py -q` -> `27 passed`
- `python -m pytest -q` -> `726 passed, 40 skipped`

## 下一步提示

- 归档当前 Phase 5 Batch 3。
- 创建 Phase 5 Batch 4，继续压缩 remaining mainline proof roots。
