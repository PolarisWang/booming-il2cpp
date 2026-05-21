# Phase 1 structured benchmark routing 收口通过

Date: 2026-04-15 01:52:44 +08:00

## 本批改动

- `build/toolchains/run/testing/subject_workers.py`
  - runtime / interpreter benchmark 执行路径增加 declared benchmark metadata 解析。
  - `entrySelection` 保留 `entryIndex`，并通过 `entryIndex -> stableId -> alias` 解析 declared benchmark。
  - perf harness 优先发出结构化参数，不再要求 `workloadEntry` 参与真实定位。
- `build/toolchains/run/testing/registry.py`
  - declared benchmark default matrix 选择改为优先按 declared `modes` 和 stage kind 对齐，不再依赖 `workloadEntry/sourceEntry` 匹配。
- `src/validation/perf/Benchmark.WorkloadEntry.PerfHarness/Program.cs`
  - 新增结构化 benchmark metadata 解析路径。
  - 保留 `workloadEntry` 仅用于展示与追踪证据。

## 目的

- 把 Phase 1 最后仍会回流到 benchmark runtime 执行链上的字符串路由面压缩到 display / evidence 用途。
- 为 Phase 2 引入统一 typed-il / body availability / hybrid dispatch 契约腾出干净边界。

## 验证

1. `python -m pytest tests/unit/registry/test_declared_registry_matrix_selection.py tests/unit/performance/test_subject_workers_perf.py -k "declared_registry_matrix_selection or declared_benchmark_metadata or preserves_entry_index"`
   - 结果：`4 passed`
2. `dotnet build src/validation/perf/Benchmark.WorkloadEntry.PerfHarness/Benchmark.WorkloadEntry.PerfHarness.csproj -c Release -m:1`
   - 结果：`build succeeded`
3. `python -m pytest tests/unit/execution/test_subject_workers.py tests/unit/performance/test_subject_workers_perf.py tests/unit/registry/test_declared_registry_matrix_selection.py tests/integration/registry/test_registry_scan.py -k "declared_benchmark_by_entry_index_for_structured_harness_execution or runtime_perf or interpreter_runtime_perf or declared_registry_matrix_selection or projects_engineering_and_declared_catalog_object_families"`
   - 结果：`13 passed`
4. `python -m pytest tests/tooling/run/test_subject_command.py tests/unit/performance/test_benchmark_command.py tests/unit/planning/test_subject_planner.py tests/unit/performance/test_benchmark_dashboard_generator.py tests/integration/registry/test_declared_metadata_discovery.py tests/integration/registry/test_registry_scan.py tests/unit/execution/test_subject_workers.py tests/unit/performance/test_subject_workers_perf.py tests/unit/registry/test_declared_registry_matrix_selection.py`
   - 结果：`110 passed`

## 下一步

- 归档 Phase 1 child task。
- 返回父 roadmap，切入 `Phase 2: Typed-IL Shared Contract And Hybrid Dispatch Foundation`。
