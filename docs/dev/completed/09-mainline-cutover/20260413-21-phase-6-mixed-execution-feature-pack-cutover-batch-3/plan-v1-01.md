# Phase 6 Mixed-Execution Feature Pack Cutover Batch 3 实现计划

> 面向执行 Agent：必须使用 `dev:executing-plans`、`dev:test-driven-development` 与 `dev:verification-before-completion` 执行本计划。  
> 目标：建立 `MixedExecutionFeaturePack` 作为 mixed execution / interpreter family 的 canonical source bundle，并完成首批 mixed/interpreter legacy roots 的 shell cutover。  
> 设计文档：`docs/dev/in-progress/20260412-06-test-system-suite-redesign/design-v1-03.md`

---

## 计划任务

- [ ] Task 1: 冻结 Batch 3 inventory 与 canonical 边界
  - 范围：
    - 盘点 `BenchMixed`
    - 盘点 `MixedExecutionProof`
    - 盘点 `InterpreterArithmeticProof`
    - 盘点 `InterpreterLoweringProof`
    - 明确哪些属于：
      - declared benchmark entry
      - declared unit test entry
      - canonical bundle 内 support project
      - legacy shell only
  - 验收：
    - `MixedExecutionFeaturePack` 的归宿边界明确
    - `InterpreterArithmeticProof` 的 support-project 角色被显式建模

- [ ] Task 2: 先写 RED tests，冻结 mixed/interpreter cutover 合同
  - 范围：
    - 扩展 performance/schema/planner/discovery 测试，断言 canonical pack 声明 mixed/interpreter unit 与 benchmark entries
    - 断言 legacy mixed/interpreter shells 的 `source.path` / `source.entry` / `workloadEntry` / `redirect` / `testDeclarationMode`
    - 冻结 canonical bundle 内 support project 的投影路径
  - 验收：
    - 新断言在实现前失败
    - 失败原因直接指向 canonical pack / shell projection 缺失

- [ ] Task 3: 实现 `MixedExecutionFeaturePack` canonical source bundle 与 shell cutover
  - 范围：
    - 新建 `subjects/MixedExecutionFeaturePack/`
    - 引入 `Chaos.TestFramework`
    - 用 `ChaosUnitTest(...)` 声明：
      - `InterpreterLoweringProofEntry`
      - `MixedExecutionProofEntry`
    - 用 `ChaosBenchmark(...)` 声明：
      - `MixedExecutionBenchmarkEntry`
    - 在 canonical bundle 内保留 `InterpreterArithmeticProof` support project，维持程序集 identity
    - 把 `BenchMixed` / `MixedExecutionProof` / `InterpreterArithmeticProof` / `InterpreterLoweringProof` manifests 退化为 compatibility shell
  - 验收：
    - canonical pack 稳定发现 declared mixed/interpreter unit 与 benchmark metadata
    - legacy shells 仍能通过旧 subject id 工作
    - 现有 mixed/interpreter 相关运行验证不回退

- [ ] Task 4: 完成验证并更新 roadmap 状态
  - 范围：
    - 运行定向 tests
    - 运行 `python -m pytest tests/unit/run/test_phase7_interpreter_mixed_execution.py -q`
    - 运行 `python -m pytest -q`
    - 更新 STATUS / ACTIVE / 索引 / 父 roadmap
  - 验收：
    - 本批全部验证通过
    - 父 roadmap 明确 Phase 6 完成，进入 Phase 7 / Phase 8 收口

## 关键改动文件

- `subjects/MixedExecutionFeaturePack/subject.manifest.json`
- `subjects/MixedExecutionFeaturePack/source/*.cs`
- `subjects/MixedExecutionFeaturePack/source/MixedExecutionFeaturePack.csproj`
- `subjects/MixedExecutionFeaturePack/source/InterpreterArithmeticProof/**`
- `subjects/BenchMixed/subject.manifest.json`
- `subjects/MixedExecutionProof/subject.manifest.json`
- `subjects/InterpreterArithmeticProof/subject.manifest.json`
- `subjects/InterpreterLoweringProof/subject.manifest.json`
- `tests/unit/performance/test_benchmark_subject_sources.py`
- `tests/unit/compatibility/test_subject_manifest_schema.py`
- `tests/unit/planning/test_subject_planner.py`
- `tests/integration/registry/test_declared_metadata_discovery.py`

## 验证

- `python -m pytest tests/unit/performance/test_benchmark_subject_sources.py -q`
- `python -m pytest tests/unit/compatibility/test_subject_manifest_schema.py -q`
- `python -m pytest tests/unit/planning/test_subject_planner.py -q`
- `python -m pytest tests/integration/registry/test_declared_metadata_discovery.py -q`
- `python -m pytest tests/unit/run/test_phase7_interpreter_mixed_execution.py -q`
- `python -m pytest -q`
