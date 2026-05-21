# Phase 5 Mainline Declared Unit Cutover Batch 1 实现计划

> 面向执行 Agent：必须使用 `dev:executing-plans` 与 `dev:test-driven-development` 执行本计划。步骤使用复选框语法跟踪。

**目标：** 让 `MainlineFeaturePack` 成为第一个真实落地 `ChaosUnitTest` 声明式 proof 的 subject，同时保持现有工程级 matrix / perf / trace 主链不回退。

**架构：** 本批次不做大规模 proof root 物理合并，只在既有 `MainlineFeaturePack` 上增加 `Chaos.TestFramework` 引用、声明式 proof 标注和 manifest `testDeclarationMode` 收口，用它打通 “真实 subject -> 编译产物 -> declaration discovery -> compiled catalog” 的第一条 functional 主线。

**技术栈：** `Chaos.TestFramework`、`Chaos.IL2CPP.DeclarationDiscovery`、Python test harness、subject manifest / planner tests

**设计文档：** `docs/dev/in-progress/20260412-06-test-system-suite-redesign/design-v1-03.md`

**预期知识沉淀：** 按任务决定；若形成稳定 authoring 规范，再补 wiki。

---

## 计划任务

- [ ] Task 1: 用 RED 测试冻结 `MainlineFeaturePack` 的声明式 proof 目标面
  - 范围：
    - 新增或更新 tests，要求 `MainlineFeaturePack` manifest 明确切到 `testDeclarationMode=require`
    - 要求 source project 引用 `src/reference/Chaos.TestFramework`
    - 要求 declaration discovery 能从真实 `MainlineFeaturePack` 编译产物中发现一组 `declaredUnitTests[]`
  - 验收：
    - 新增测试在实现前失败
    - 失败原因直接指向 manifest/source 尚未完成 declared-unit cutover

- [ ] Task 2: 在 `MainlineFeaturePack` 落地最小声明式 proof 改造
  - 范围：
    - 更新 `subjects/MainlineFeaturePack/source/MainlineFeaturePack.csproj`
    - 为现有 proof entry methods 增加 `ChaosUnitTest` 标注
    - 保持当前 `ProofEntry` / matrix `source.entry` 主链仍可工作
    - 更新 manifest 的 `testDeclarationMode`
  - 验收：
    - discovery 返回真实 `declaredUnitTests[]`
    - 现有 planner / onboarding / perf / trace 相关测试仍然成立

- [ ] Task 3: 对齐 schema / compatibility / discovery tests
  - 范围：
    - 更新默认 `testDeclarationMode` 断言，使其允许已切换 subject 使用 `require`
    - 补充 `MainlineFeaturePack` 的 declared metadata 断言
    - 保证不会误伤其他仍处于 `auto` 的 subject
  - 验收：
    - 定向 tests 全绿
    - 断言覆盖 manifest contract 与 discovery payload

- [ ] Task 4: 完成验证并归档本批次
  - 范围：
    - 运行定向 tests
    - 运行 `python -m pytest -q`
    - 更新 STATUS / ACTIVE / 进度记录 / 索引 / 父 roadmap
  - 验收：
    - 本批次验证全部通过
    - 父 roadmap 明确转入下一批 functional proof consolidation

## 关键改动文件

- `subjects/MainlineFeaturePack/subject.manifest.json`
- `subjects/MainlineFeaturePack/source/MainlineFeaturePack.csproj`
- `subjects/MainlineFeaturePack/source/*.cs`
- `tests/integration/registry/test_declared_metadata_discovery.py`
- `tests/unit/run/test_phase4_mainline_feature_pack_onboarding.py`
- `tests/unit/compatibility/test_subject_manifest_schema.py`

## 验证

- `python -m pytest tests/integration/registry/test_declared_metadata_discovery.py -q`
- `python -m pytest tests/unit/run/test_phase4_mainline_feature_pack_onboarding.py -q`
- `python -m pytest tests/unit/compatibility/test_subject_manifest_schema.py -q`
- `python -m pytest tests/unit/planning/test_subject_planner.py -q`
- `python -m pytest -q`

## 执行备注

- 本批次先证明声明式 proof 能在真实 subject 上落地，不在这一批就强推所有 `*Proof` roots 物理合并。
- 现有 `MainlineFeaturePack` matrix/source.entry 切片是 Phase 4~6 既有测试资产，本批次默认保留；后续 functional consolidation 再逐步把切片语义收敛到 declared catalog。
- 如果 `MainlineFeaturePack` 的 declared proof authoring 形成稳定模式，再以同一模式迁移其他 proof-like roots。
