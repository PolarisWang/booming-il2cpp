# Phase 5 Mainline Advanced Proof Shell Cutover Batch 4 实现计划

> 面向执行 Agent：必须使用 `dev:executing-plans` 与 `dev:test-driven-development` 执行本计划。步骤使用复选框语法跟踪。

**目标：** 将剩余一批 `mainline` proof roots 并回 `MainlineFeaturePack`，覆盖 dispatch / vtable / linker / interop / cross-boundary exception 等更复杂切片，同时保留 legacy root 的 compatibility shell 入口。

**架构：** 本批延续 Batch 2/3 的 canonical subject + compatibility shell 模型，但要额外处理 `InterfaceDispatchProof` 的 perf surface，以及更明确的 typed metadata（`category` / `requires`）。

**设计文档：** `docs/dev/in-progress/20260412-06-test-system-suite-redesign/design-v1-03.md`

---

## 计划任务

- [ ] Task 1: 先补 RED tests，冻结 advanced mainline proof shell contract
  - 范围：
    - 更新 `test_phase2_aot_runtime_completeness.py`
    - 更新 `test_phase2_linker_stripping_proof.py`
    - 更新 `test_phase2_marshaling_proof.py`
    - 为 `VTableDispatchProof` / `CrossBoundaryExceptionProof` 增加 schema/planner/discovery 断言
    - 扩展 `MainlineFeaturePack` manifest/discovery 的 declared-unit 期望
  - 验收：
    - 新断言在实现前失败
    - 失败原因直接指向缺失的 canonical slice / shell projection / declared metadata

- [ ] Task 2: 在 `MainlineFeaturePack` 中加入 advanced proof slices
  - 范围：
    - `InterfaceDispatchProofEntry`
    - `VTableDispatchProofEntry`
    - `LinkerStrippingProofEntry`
    - `MarshalingProofEntry`
    - `CrossBoundaryExceptionProofEntry`
    - 对齐 `ChaosUnitTest(...)` 的 alias / category / requires / evidence
    - 扩展 `MainlineFeaturePack` manifest 的 validation profiles / matrix overlays
  - 验收：
    - canonical subject 可稳定发现并选择上述 5 个 declared unit entries
    - interface/linker/marshaling 相关旧核心测试不回退

- [ ] Task 3: 将 advanced mainline legacy roots 退化为 compatibility shell
  - 范围：
    - `InterfaceDispatchProof`
    - `VTableDispatchProof`
    - `LinkerStrippingProof`
    - `MarshalingProof`
    - `CrossBoundaryExceptionProof`
    - 保留 `InterfaceDispatchProof` 上的 perf validation / baseline surface
    - 统一引入 canonical `source.path`、`source.entry`、`compatibility.redirectToSubject`、`testDeclarationMode=none`
  - 验收：
    - legacy root 不再拥有 canonical source
    - perf / planner / schema / phase tests 仍能从 legacy id 入口工作

- [ ] Task 4: 完成验证并更新 roadmap 状态
  - 范围：
    - 运行定向 tests
    - 运行 `python -m pytest -q`
    - 更新 STATUS / ACTIVE / 索引 / 父 roadmap
  - 验收：
    - 本批全部验证通过
    - 父 roadmap 明确切入 Phase 6 benchmark / hot-update migration

## 关键改动文件

- `subjects/MainlineFeaturePack/subject.manifest.json`
- `subjects/MainlineFeaturePack/source/*.cs`
- `subjects/InterfaceDispatchProof/subject.manifest.json`
- `subjects/VTableDispatchProof/subject.manifest.json`
- `subjects/LinkerStrippingProof/subject.manifest.json`
- `subjects/MarshalingProof/subject.manifest.json`
- `subjects/CrossBoundaryExceptionProof/subject.manifest.json`
- `tests/unit/run/test_phase2_aot_runtime_completeness.py`
- `tests/unit/run/test_phase2_linker_stripping_proof.py`
- `tests/unit/run/test_phase2_marshaling_proof.py`
- `tests/integration/registry/test_declared_metadata_discovery.py`
- `tests/unit/compatibility/test_subject_manifest_schema.py`
- `tests/unit/planning/test_subject_planner.py`

## 验证

- `python -m pytest tests/unit/run/test_phase2_aot_runtime_completeness.py -q`
- `python -m pytest tests/unit/run/test_phase2_linker_stripping_proof.py -q`
- `python -m pytest tests/unit/run/test_phase2_marshaling_proof.py -q`
- `python -m pytest tests/integration/registry/test_declared_metadata_discovery.py -q`
- `python -m pytest tests/unit/compatibility/test_subject_manifest_schema.py -q`
- `python -m pytest tests/unit/planning/test_subject_planner.py -q`
- `python -m pytest -q`
