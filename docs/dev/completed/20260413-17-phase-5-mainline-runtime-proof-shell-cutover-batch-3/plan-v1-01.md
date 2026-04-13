# Phase 5 Mainline Runtime Proof Shell Cutover Batch 3 实现计划

> 面向执行 Agent：必须使用 `dev:executing-plans` 与 `dev:test-driven-development` 执行本计划。步骤使用复选框语法跟踪。

**目标：** 将第二批 mainline/runtime proof roots 并回 `MainlineFeaturePack`，并证明仍被旧 phase tests 直接锚定的 subject 也可以退化为 compatibility shell。

**架构：** 这一批沿用 Batch 2 的模型，但要补上“旧阶段测试仍盯着 legacy subject”的迁移能力。策略是：保留 legacy subject id 与 proof 资产目录，manifest/source 改为指向 canonical `MainlineFeaturePack`，同时把旧 phase tests 的 source path / entry 断言改成面向 compatibility shell 的 canonical projection。

**技术栈：** `MainlineFeaturePack`、legacy subject manifests、phase2 proof tests、Python planner/schema harness

**设计文档：** `docs/dev/in-progress/20260412-06-test-system-suite-redesign/design-v1-03.md`

**预期知识沉淀：** 按任务决定；如旧 phase tests 的 shell 迁移模式稳定，再考虑补 wiki。

---

## 计划任务

- [ ] Task 1: 用 RED 测试冻结第二批 runtime proof shell 契约
  - 范围：
    - 扩展 `MainlineFeaturePack` manifest/discovery assertions
    - 为 `AsyncAwaitProof` / `ThreadingProof` / `NestedExceptionProof` / `GenericCollectionProof` / `DelegateChainProof` 增加 shell schema/planner assertions
    - 更新旧 phase2 tests 的 source path / entry 断言为 canonical projection 目标
  - 验收：
    - 新增或更新测试在实现前失败
    - 失败原因直接指向缺失的 canonical slice / shell manifest / phase-test contract

- [ ] Task 2: 在 `MainlineFeaturePack` 增加第二批 runtime proof slices
  - 范围：
    - 新增 `AsyncAwaitProofEntry`
    - 新增 `ThreadingProofEntry`
    - 新增 `NestedExceptionProofEntry`
    - 新增 `GenericCollectionProofEntry`
    - 新增 `DelegateChainProofEntry`
    - 为这些 entry methods 添加 `ChaosUnitTest(...)`
    - 更新 `MainlineFeaturePack` manifest 的 validation profiles / matrices
  - 验收：
    - canonical subject 可发现并选择这 5 个新 slices
    - 原有 runtime/codegen 锁定测试不回退

- [ ] Task 3: 将 5 个 runtime proof subjects 退化为 compatibility shell，并对齐旧 phase tests
  - 范围：
    - 更新 5 个 legacy manifests 的 canonical source projection
    - 保留旧 subject id 与旧 phase test 入口
    - 调整旧 phase tests，使其校验 compatibility shell 行为而不是要求 legacy subject 继续拥有 canonical source
  - 验收：
    - 旧 phase tests 仍能验证这些 proof 的真实行为
    - legacy subject 不再作为源码真源存在

- [ ] Task 4: 完成验证并更新父 roadmap 状态
  - 范围：
    - 运行定向 tests
    - 运行 `python -m pytest -q`
    - 更新 STATUS / ACTIVE / 索引 / 父 roadmap
  - 验收：
    - 本批次全部验证通过
    - 父 roadmap 明确下一批 Phase 5 或 Phase 6 入口

## 关键改动文件

- `subjects/MainlineFeaturePack/subject.manifest.json`
- `subjects/MainlineFeaturePack/source/*.cs`
- `subjects/AsyncAwaitProof/subject.manifest.json`
- `subjects/ThreadingProof/subject.manifest.json`
- `subjects/NestedExceptionProof/subject.manifest.json`
- `subjects/GenericCollectionProof/subject.manifest.json`
- `subjects/DelegateChainProof/subject.manifest.json`
- `tests/unit/run/test_phase2_async_await_proof.py`
- `tests/unit/run/test_phase2_threading_gc_proof.py`
- `tests/unit/run/test_phase2_nested_exception_proof.py`
- `tests/unit/compatibility/test_subject_manifest_schema.py`
- `tests/unit/planning/test_subject_planner.py`
- `tests/integration/registry/test_declared_metadata_discovery.py`

## 验证

- `python -m pytest tests/unit/run/test_phase2_async_await_proof.py -q`
- `python -m pytest tests/unit/run/test_phase2_threading_gc_proof.py -q`
- `python -m pytest tests/unit/run/test_phase2_nested_exception_proof.py -q`
- `python -m pytest tests/integration/registry/test_declared_metadata_discovery.py -q`
- `python -m pytest tests/unit/compatibility/test_subject_manifest_schema.py -q`
- `python -m pytest tests/unit/planning/test_subject_planner.py -q`
- `python -m pytest -q`

## 执行备注

- `GenericCollectionProof` 与 `DelegateChainProof` 虽然没有被旧 phase tests 直接锚定，但与本批 runtime 行为 proof 归属一致，放在同一批一起迁移更划算。
- 本批不碰 `MarshalingProof`、`BridgeRoundtripProof`、`MethodReplacementProof` 这类更偏 interop/hot-update 的 roots，避免 pack 归属混乱。
- 对旧 phase tests 的修改要尽量只改 canonical source projection 断言，不要动它们对 lowering/linker/codegen/runtime surface 的核心锁定。
