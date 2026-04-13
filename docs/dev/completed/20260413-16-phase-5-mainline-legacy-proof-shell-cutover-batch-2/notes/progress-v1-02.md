# Progress v1.02

日期：2026-04-13 01:42:10 +08:00

## 本轮完成

- 在 `MainlineFeaturePack` 中新增 6 个 proof slices，并使用真实 `ChaosUnitTest(...)` 标注：
  - `ArrayOpsProofEntry`
  - `BitwiseOpsProofEntry`
  - `BranchOpsProofEntry`
  - `ConversionOpsProofEntry`
  - `ObjectOpsProofEntry`
  - `OverflowOpsProofEntry`
- 扩展 `subjects/MainlineFeaturePack/subject.manifest.json`：
  - 新增 6 个 validation profiles
  - 新增 6 个 matrix source entry overlays
- 将以下 top-level legacy subjects 退化为 compatibility shell：
  - `ArrayOpsProof`
  - `BitwiseOpsProof`
  - `BranchOpsProof`
  - `ConversionOpsProof`
  - `ObjectOpsProof`
  - `OverflowOpsProof`
- 为 shell manifests 引入：
  - canonical `source.path`
  - canonical `source.entry`
  - `compatibility.redirectToSubject=MainlineFeaturePack`
  - `testDeclarationMode=none`
- 在 `build/toolchains/run/testing/compiled_catalog.py` 中实现 `testDeclarationMode=none` 的声明发现短路，确保 compatibility shell 不会错误投影 canonical pack 的全部 declared tests。
- 对齐 schema/planner/discovery tests，使 canonical subject 与 legacy shell 入口同时可验证。

## 验证结果

- `python -m pytest tests/integration/registry/test_declared_metadata_discovery.py -q` -> `5 passed`
- `python -m pytest tests/unit/compatibility/test_subject_manifest_schema.py -q` -> `15 passed`
- `python -m pytest tests/unit/planning/test_subject_planner.py -q` -> `26 passed`
- `python -m pytest -q` -> `720 passed, 40 skipped`

## 下一步提示

- 归档当前 Phase 5 Batch 2。
- 创建 Phase 5 Batch 3，开始处理仍由旧 phase tests 直接锚定的第二批 mainline/runtime proof roots。
