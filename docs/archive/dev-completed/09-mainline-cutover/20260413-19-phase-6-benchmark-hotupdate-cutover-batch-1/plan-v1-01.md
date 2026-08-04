# Phase 6 Benchmark And Hot-Update Cutover Batch 1 实现计划

> 面向执行 Agent：必须使用 `dev:executing-plans`、`dev:test-driven-development` 与 `dev:verification-before-completion` 执行本计划。
> 目标：先冻结 benchmark/hot-update roots 的归宿边界，再处理第一批 pure runtime benchmark legacy roots 的 canonical cutover，为后续 hot-update / mixed execution host pack 收口清出干净主线。
> 设计文档：`docs/dev/in-progress/20260412-06-test-system-suite-redesign/design-v1-03.md`

---

## 计划任务

- [ ] Task 1: 冻结 Phase 6 inventory 与分类合同
  - 范围：
    - 盘点 `Bench*` roots
    - 盘点 `BenchHotUpdate*` / `BenchMixed`
    - 盘点 `HotUpdateSkeletonProof` / `MethodReplacementProof` / `VersionRollbackProof` / `MixedExecutionProof` / `Interpreter*`
    - 给每个 root 标注：
      - declared benchmark
      - engineering workload
      - host solution / patch solution
      - compatibility shell only
    - 明确纯 runtime benchmark 第一批范围：
      - `BenchArithmetic`
      - `BenchAllocation`
      - `BenchDispatch`
      - `BenchGeneric`
  - 验收：
    - Phase 6 Batch 1 范围内不再混淆 benchmark 与 engineering workload
    - 挂起任务 `20260412-05-phase-b-generic-native-aot-benchmark-backend` 的结论被映射进新分类

- [ ] Task 2: 先写 RED tests，冻结第一批 pure runtime benchmark cutover 合同
  - 范围：
    - 扩展 declared metadata discovery 对真实 benchmark subject 的断言
    - 扩展 planner/schema/registry 对 declared benchmark 与 compatibility shell 的断言
    - 冻结第一批 benchmark source 统一归宿、legacy shell 语义、`testDeclarationMode` 与 typed metadata
  - 验收：
    - 新断言在实现前失败
    - 失败原因直接指向缺失的 canonical benchmark declaration / shell projection / catalog surface

- [ ] Task 3: 实现第一批 pure runtime benchmark cutover
  - 范围：
    - 将 `BenchArithmetic` / `BenchAllocation` / `BenchDispatch` / `BenchGeneric` 改造成 canonical benchmark solution source 下的 declared benchmarks
    - 为 benchmark source 引入 `Chaos.TestFramework`
    - 用 `ChaosBenchmark(...)` 替代外部 benchmark 切片定义
    - 将 legacy `Bench*` manifests 退化为 compatibility shell
    - 保持 generic native AOT workload、managed/interpreter perf 路线与 record/baseline 语义不回退
  - 验收：
    - canonical benchmark subject 可以稳定发现第一批 `declaredBenchmarks`
    - legacy benchmark root 仍可通过原有入口工作
    - native AOT benchmark backend 相关现有测试无回归

- [ ] Task 4: 完成验证并更新 roadmap 状态
  - 范围：
    - 运行定向 tests
    - 运行 `python -m pytest -q`
    - 更新 STATUS / ACTIVE / 索引 / 父 roadmap
  - 验收：
    - 本批全部验证通过
    - 父 roadmap 明确下一批将切向 hot-update / mixed execution host 收口，或继续 benchmark rollout

## 关键改动文件

- `subjects/BenchArithmetic/subject.manifest.json`
- `subjects/BenchAllocation/subject.manifest.json`
- `subjects/BenchDispatch/subject.manifest.json`
- `subjects/BenchGeneric/subject.manifest.json`
- `subjects/*/source/*.cs`
- `tests/integration/registry/test_declared_metadata_discovery.py`
- `tests/unit/compatibility/test_subject_manifest_schema.py`
- `tests/unit/planning/test_subject_planner.py`
- `tests/unit/performance/test_benchmark_subject_sources.py`
- `tests/unit/performance/test_benchmark_command.py`
- `build/toolchains/run/testing/compiled_catalog.py`
- `build/toolchains/run/testing/subject_planner.py`

## 验证

- `python -m pytest tests/integration/registry/test_declared_metadata_discovery.py -q`
- `python -m pytest tests/unit/compatibility/test_subject_manifest_schema.py -q`
- `python -m pytest tests/unit/planning/test_subject_planner.py -q`
- `python -m pytest tests/unit/performance/test_benchmark_subject_sources.py -q`
- `python -m pytest tests/unit/performance/test_benchmark_command.py -q`
- `python -m pytest -q`
