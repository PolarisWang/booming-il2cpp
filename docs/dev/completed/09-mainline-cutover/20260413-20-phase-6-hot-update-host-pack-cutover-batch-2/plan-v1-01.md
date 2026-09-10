# Phase 6 Hot-Update Host Pack Cutover Batch 2 实现计划

> 面向执行 Agent：必须使用 `dev:executing-plans`、`dev:test-driven-development` 与 `dev:verification-before-completion` 执行本计划。
> 目标：建立 `HotUpdateHostPack` 作为 hot-update host proof/benchmark 的 canonical source bundle，并完成第一批 hot-update legacy roots 的 shell cutover。
> 设计文档：`docs/dev/in-progress/20260412-06-test-system-suite-redesign/design-v1-03.md`

---

## 计划任务

- [ ] Task 1: 冻结 Batch 2 inventory 与边界
  - 范围：
    - 盘点 `BenchHotUpdateDispatch`
    - 盘点 `BenchHotUpdateLoad`
    - 盘点 `BenchHotUpdateRoundtrip`
    - 盘点 `HotUpdateSkeletonProof`
    - 盘点 `MethodReplacementProof`
    - 盘点 `VersionRollbackProof`
    - 明确哪些属于：
      - declared benchmark entry
      - declared unit test entry
      - host/patch package helper code
      - legacy shell only
    - 明确本批不处理：
      - `BenchMixed`
      - `MixedExecutionProof`
      - `InterpreterArithmeticProof`
      - `InterpreterLoweringProof`
  - 验收：
    - `HotUpdateHostPack` 的归宿边界明确
    - 本批不再把 mixed execution / interpreter lowering 范围误并进来

- [ ] Task 2: 先写 RED tests，冻结 `HotUpdateHostPack` cutover 合同
  - 范围：
    - 扩展 declared metadata discovery，断言 canonical pack 同时声明 hot-update unit 与 benchmark entries
    - 扩展 planner/schema/compatibility 测试，断言 legacy hot-update benchmark/proof shell 的 source/path/entry/redirect/testDeclarationMode
    - 冻结 `MethodReplacementProof` 与 `VersionRollbackProof` 的 canonical declared coverage
  - 验收：
    - 新断言在实现前失败
    - 失败原因直指 canonical pack / shell projection 缺失

- [ ] Task 3: 实现 `HotUpdateHostPack` canonical source bundle 与 shell cutover
  - 范围：
    - 新建 `subjects/HotUpdateHostPack/`
    - 引入 `Chaos.TestFramework`
    - 用 `ChaosUnitTest(...)` 声明：
      - HotUpdateSkeleton
      - MethodReplacement
      - VersionRollback
    - 用 `ChaosBenchmark(...)` 声明：
      - HotUpdateDispatch
      - HotUpdateLoad
      - HotUpdateRoundtrip
    - 将 `BenchHotUpdateDispatch` / `BenchHotUpdateLoad` / `BenchHotUpdateRoundtrip` manifests 退化为 compatibility shell
    - 将 `HotUpdateSkeletonProof` manifest 退化为 compatibility shell
    - 保持现有 benchmark baselines / records / proof output 语义不回退
    - 尽量把旧 package 资产改为 C# 运行时生成，避免 canonical pack 带入额外静态目录
  - 验收：
    - canonical pack 稳定发现 declared hot-update unit/benchmark metadata
    - legacy shells 仍能通过原 subject id 工作
    - 旧 phase5/phase9 hot-update 回归没有被破坏

- [ ] Task 4: 完成验证并更新 roadmap 状态
  - 范围：
    - 运行定向 tests
    - 运行 `python -m pytest -q`
    - 更新 STATUS / ACTIVE / 索引 / 父 roadmap
  - 验收：
    - 本批全部验证通过
    - 父 roadmap 明确下一批将切到 mixed execution / interpreter pack

## 关键改动文件

- `subjects/HotUpdateHostPack/subject.manifest.json`
- `subjects/HotUpdateHostPack/source/*.cs`
- `subjects/HotUpdateHostPack/source/HotUpdateHostPack.csproj`
- `subjects/BenchHotUpdateDispatch/subject.manifest.json`
- `subjects/BenchHotUpdateLoad/subject.manifest.json`
- `subjects/BenchHotUpdateRoundtrip/subject.manifest.json`
- `subjects/HotUpdateSkeletonProof/subject.manifest.json`
- `tests/integration/registry/test_declared_metadata_discovery.py`
- `tests/unit/compatibility/test_subject_manifest_schema.py`
- `tests/unit/planning/test_subject_planner.py`
- `tests/unit/performance/test_benchmark_subject_sources.py`
- `build/toolchains/run/testing/subject_planner.py`

## 验证

- `python -m pytest tests/integration/registry/test_declared_metadata_discovery.py -q`
- `python -m pytest tests/unit/compatibility/test_subject_manifest_schema.py -q`
- `python -m pytest tests/unit/planning/test_subject_planner.py -q`
- `python -m pytest tests/unit/performance/test_benchmark_subject_sources.py -q`
- `python -m pytest tests/unit/run/test_phase5_hot_update_skeleton.py -q`
- `python -m pytest tests/unit/run/test_phase9_hot_update_e2e.py -q`
- `python -m pytest -q`
