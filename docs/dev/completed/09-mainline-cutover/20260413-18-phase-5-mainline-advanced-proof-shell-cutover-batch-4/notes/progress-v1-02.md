# Progress v1.02

日期：2026-04-13 02:52:33 +08:00

## 本轮完成

- 在 `subjects/MainlineFeaturePack/source/` 中新增 5 个 advanced proof slice，并全部改为真实 `ChaosUnitTest(...)` 声明：
  - `InterfaceDispatchProofEntry`
  - `VTableDispatchProofEntry`
  - `LinkerStrippingProofEntry`
  - `MarshalingProofEntry`
  - `CrossBoundaryExceptionProofEntry`
- 扩展 `subjects/MainlineFeaturePack/subject.manifest.json`：
  - 新增 5 个 `validationProfiles`
  - 新增 5 个 matrix `source.entry` overlay
  - 保留 `perf-profile`，确保 `InterfaceDispatchProof` 的既有 perf 入口不丢失
- 将以下 legacy roots 退化为 compatibility shell：
  - `InterfaceDispatchProof`
  - `VTableDispatchProof`
  - `LinkerStrippingProof`
  - `MarshalingProof`
  - `CrossBoundaryExceptionProof`
- 对齐 shell manifest 语义：
  - canonical `source.path`
  - canonical `source.entry`
  - `compatibility.redirectToSubject=MainlineFeaturePack`
  - `testDeclarationMode=none`
- 补齐 loader / semantic world 的 decode 与 capability 回归，让 `MainlineFeaturePack` umbrella subject 能完整通过 convert/analysis/linker 主链
- 明确 `LinkerStrippingProof` 通过 legacy shell 执行时，analysis artifact 的 subject identity 采用 canonical `MainlineFeaturePack/*`，不再伪装回 legacy root

## 验证结果

- `python -m pytest tests/unit/run/test_phase5_capability_batch_a.py -q` -> `3 passed`
- `python -m pytest tests/unit/run/test_phase2_linker_stripping_proof.py -q` -> `4 passed`
- `python -m pytest tests/unit/run/test_phase2_aot_runtime_completeness.py tests/unit/run/test_phase2_marshaling_proof.py tests/integration/registry/test_declared_metadata_discovery.py tests/unit/compatibility/test_subject_manifest_schema.py tests/unit/planning/test_subject_planner.py -q` -> `62 passed`
- `python -m pytest -q` -> `740 passed, 40 skipped`

## 下一步提示

- 归档当前 Phase 5 Batch 4。
- 更新父 roadmap，标记 Phase 5 主线收口完成。
- 创建并激活 Phase 6 Batch 1，先盘点 benchmark / hot-update / mixed execution roots 的 canonical 归宿。
