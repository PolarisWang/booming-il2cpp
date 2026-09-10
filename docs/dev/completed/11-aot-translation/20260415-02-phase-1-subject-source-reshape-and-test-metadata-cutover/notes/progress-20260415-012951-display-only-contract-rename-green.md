# Phase 1 display-only contract rename 已通过

Date: 2026-04-15 01:29:51 +08:00

## 本批改动

- `build/toolchains/run/testing/registry.py`
  - subject 级默认摘要字段从：
    - `defaultSourceEntry`
    - `defaultSubjectEntrySelection`
    - `defaultWorkloadEntry`
  - 改为：
    - `displaySourceEntry`
    - `displaySubjectEntrySelection`
    - `displayWorkloadEntry`

## 目的

- 明确这些字段只用于 registry 展示摘要，不再暗示自己是默认路由主键。
- 把“展示 contract”和“真正参与调度 / 路由的 contract”在命名层面切开，避免 string surface 回流。

## 验证

1. `python -m pytest tests/integration/registry/test_registry_scan.py -k "surfaces_retained_subject_default_entry_metadata or projects_engineering_and_declared_catalog_object_families"`
   - 结果：`2 passed`
2. `python -m pytest tests/tooling/run/test_subject_command.py tests/unit/performance/test_benchmark_command.py tests/unit/planning/test_subject_planner.py tests/unit/performance/test_benchmark_dashboard_generator.py`
   - 结果：`56 passed`
3. `python -m pytest tests/integration/registry/test_declared_metadata_discovery.py tests/integration/registry/test_registry_scan.py`
   - 结果：`18 passed`

## 下一步

- 继续盘点真正仍参与路由的字符串面，重点检查：
  - declared matrix 选择
  - benchmark / perf dashboard summary 匹配
  - 是否还存在可以被 `entryIndex` / `entrySelection` 替代的 string fallback
