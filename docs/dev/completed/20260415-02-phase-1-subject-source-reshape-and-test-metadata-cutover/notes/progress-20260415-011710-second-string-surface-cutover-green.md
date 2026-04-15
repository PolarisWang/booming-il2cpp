# Phase 1 第二批 string surface cutover 已通过

Date: 2026-04-15 01:17:10 +08:00

## 本批改动

- `build/toolchains/run/commands/test.py`
  - `declared-unit-test` 在 workspace execution 可用时不再向 planner 传 `source_entry`
  - `declared-benchmark` 在 workspace execution 可用时不再重复传 `source_entry`，只保留 `workload_entry`
  - declared object 的 `entrySelection` 开始携带 registry / workspace 提供的 `entryIndex`
- `build/toolchains/run/commands/benchmark.py`
  - declared benchmark case 收口为 `workload_entry + entrySelection`
- `build/toolchains/run/testing/subject_planner.py`
  - `entrySelection` 纳入 stage fingerprint
  - 支持“只给 `workload_entry` 也能成为 benchmark 有效 `source.entry`”的 override 语义
- `build/toolchains/run/testing/registry.py`
  - declared registry object 开始显式透传 `entryIndex`
- `build/toolchains/run/testing/benchmark_dashboard_generator.py`
  - declared benchmark case rekey 开始优先使用 `entryIndex`
  - 记录和汇总的 case payload 同步保留 `entryIndex`

## RED -> GREEN

- 新增 / 更新 RED 测试：
  - `tests/tooling/run/test_subject_command.py`
  - `tests/unit/performance/test_benchmark_command.py`
  - `tests/unit/planning/test_subject_planner.py`
  - `tests/unit/performance/test_benchmark_dashboard_generator.py`
  - `tests/integration/registry/test_registry_scan.py`
- RED 失败点：
  - command 层仍在双传 `source_entry`
  - planner 未将 `entrySelection` 纳入 fingerprint
  - benchmark dashboard 未能基于 `entryIndex` rekey 到当前 declared case
  - registry / command 未显式透传 `entryIndex`
- GREEN 后结果：全部定向和更宽回归均通过

## 验证

1. `python -m pytest tests/tooling/run/test_subject_command.py tests/unit/performance/test_benchmark_command.py tests/unit/planning/test_subject_planner.py tests/unit/performance/test_benchmark_dashboard_generator.py`
   - 结果：`56 passed`
2. `python -m pytest tests/integration/registry/test_declared_metadata_discovery.py tests/integration/registry/test_registry_scan.py`
   - 结果：`18 passed`

## 剩余问题

- `registry.py` 中仍有 `defaultSourceEntry` / `defaultWorkloadEntry` 这类可能兼具“展示 + fallback”双重语义的字段，需要继续收口到 display-only contract 或移出路由面。
- summary / perf dashboard 仍存在部分基于 `workloadEntry` 的汇总指针，后续需要明确它是否仅用于展示。
