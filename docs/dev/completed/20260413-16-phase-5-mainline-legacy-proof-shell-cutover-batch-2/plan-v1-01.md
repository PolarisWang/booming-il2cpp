# Phase 5 Mainline Legacy Proof Shell Cutover Batch 2 实现计划

> 面向执行 Agent：必须使用 `dev:executing-plans` 与 `dev:test-driven-development` 执行本计划。步骤使用复选框语法跟踪。

**目标：** 把第一批低耦合 legacy opcode/object proof roots 并回 `MainlineFeaturePack`，并把旧 top-level subjects 退化为 compatibility shell。

**架构：** 本批次不追求一次性合并全部 `*Proof` roots，而是先用一组低耦合、源码短小、几乎无旧测试锚定的 proofs 打通完整收口路径：`legacy standalone subject -> canonical MainlineFeaturePack slice -> compatibility shell -> planner/schema/catalog`。兼容壳继续保留旧 subject id，但源码真源收敛到 `MainlineFeaturePack`。

**技术栈：** `MainlineFeaturePack`、`Chaos.TestFramework`、`compiled_catalog.py`、subject manifest / planner Python harness

**设计文档：** `docs/dev/in-progress/20260412-06-test-system-suite-redesign/design-v1-03.md`

**预期知识沉淀：** 按任务决定；如 compatibility shell 语义在本批稳定，再考虑沉淀到 wiki。

---

## 计划任务

- [ ] Task 1: 用 RED 测试冻结 Batch 2 的 compatibility shell 与 canonical slice 契约
  - 范围：
    - 为 `MainlineFeaturePack` 增加 batch2 proof slice 的 manifest/discovery 断言
    - 为 legacy proof shell 增加 planner/schema 断言，要求其 source 指向 canonical subject
    - 为 `testDeclarationMode=none` 增加 catalog 断言，要求 compatibility shell 可显式关闭声明发现
  - 验收：
    - 新增测试在实现前失败
    - 失败原因直接指向缺失的 canonical slice / shell 语义 / catalog 行为

- [ ] Task 2: 在 `MainlineFeaturePack` 落地第一批 legacy opcode/object proof slices
  - 范围：
    - 新增 `ArrayOpsProof` / `BitwiseOpsProof` / `BranchOpsProof` / `ConversionOpsProof` / `ObjectOpsProof` / `OverflowOpsProof` 对应源码切片
    - 为这些 entry methods 增加 `ChaosUnitTest(...)`
    - 更新 `MainlineFeaturePack` manifest 的 validation profiles / matrices / source entry overlays
  - 验收：
    - `MainlineFeaturePack` 的真实编译产物可发现新增 declared unit tests
    - canonical subject 自身可以选择这些新 proof slices

- [ ] Task 3: 将 6 个 legacy proof subjects 退化为 compatibility shell
  - 范围：
    - 更新 6 个 legacy manifests，使其 source 指向 `subjects/MainlineFeaturePack/source/MainlineFeaturePack.csproj`
    - 增加 `compatibility.redirectToSubject=MainlineFeaturePack`
    - 将 compatibility shell 明确标记为 `testDeclarationMode=none`
    - 对齐 schema / planner / capability 投影，使 shell 可被合法发现与规划
  - 验收：
    - 旧 subject id 仍可构建计划并运行旧 proof entry
    - shell 不会把 canonical pack 的全部 declared tests 错误暴露为自身 catalog

- [ ] Task 4: 完成验证并更新父 roadmap 状态
  - 范围：
    - 运行定向 tests
    - 运行 `python -m pytest -q`
    - 更新 STATUS / ACTIVE / 进度记录 / 索引 / 父 roadmap
  - 验收：
    - 本批次全部验证通过
    - 父 roadmap 明确切入下一批 Phase 5 或 Phase 6 工作

## 关键改动文件

- `subjects/MainlineFeaturePack/subject.manifest.json`
- `subjects/MainlineFeaturePack/source/*.cs`
- `subjects/ArrayOpsProof/subject.manifest.json`
- `subjects/BitwiseOpsProof/subject.manifest.json`
- `subjects/BranchOpsProof/subject.manifest.json`
- `subjects/ConversionOpsProof/subject.manifest.json`
- `subjects/ObjectOpsProof/subject.manifest.json`
- `subjects/OverflowOpsProof/subject.manifest.json`
- `build/toolchains/run/testing/compiled_catalog.py`
- `tests/integration/registry/test_declared_metadata_discovery.py`
- `tests/unit/compatibility/test_subject_manifest_schema.py`
- `tests/unit/planning/test_subject_planner.py`

## 验证

- `python -m pytest tests/integration/registry/test_declared_metadata_discovery.py -q`
- `python -m pytest tests/unit/compatibility/test_subject_manifest_schema.py -q`
- `python -m pytest tests/unit/planning/test_subject_planner.py -q`
- `python -m pytest -q`

## 执行备注

- 本批次优先选取低耦合 proof roots，避免一上来碰 `NestedExceptionProof`、`ThreadingProof`、`InterfaceDispatchProof` 这类仍被旧测试直接锚定的 subject。
- compatibility shell 在本批次仍允许保留旧 `expected/` 资产；重点先收敛 canonical source，而不是一次性清理全部历史证据目录。
- 若发现 shell 语义需要额外公共契约字段，优先用最小 manifest/loader 变更实现，不在本批次扩散到新的 mega migration 设计。
