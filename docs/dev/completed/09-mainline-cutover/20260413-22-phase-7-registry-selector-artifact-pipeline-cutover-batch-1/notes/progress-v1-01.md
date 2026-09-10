# Progress v1.01

日期：2026-04-13 04:18:00 +08:00

## 本轮启动

- 承接 Phase 6 全量 cutover 归档结果，开始 unified catalog 主入口切换。
- 本批先冻结：
  - registry object family
  - selector / CLI 语义
  - family-specific report / artifact path
- 设计目标：
  - `subject`
  - `engineering-validation`
  - `engineering-workload`
  - `declared-unit-test`
  - `declared-benchmark`
  统一进入 registry / selector / reporting 主路径。

## 本轮完成

- registry scan 已正式投影 4 个新对象族：
  - `engineering-validation`
  - `engineering-workload`
  - `declared-unit-test`
  - `declared-benchmark`
- selector / CLI / dynamic test command 已支持按统一对象族直接选择并执行。
- planner 已支持 `source_entry`、`workload_entry`、`entry_selection`，执行结果会把 entry family 写入 selection / summary / result。
- report / artifact 路径已稳定区分：
  - `engineering/validations`
  - `engineering/workloads`
  - `declared/unit`
  - `declared/benchmark`
- 对 `subject` 对象保留原有 goal/matrix 自动选择行为，仅对统一对象族使用 registry 默认 goal/matrix，避免破坏已有 subject 执行语义。

## 验证结果

- `python -m pytest tests/unit/compatibility/test_compiled_subject_catalog.py tests/integration/registry/test_registry_scan.py tests/integration/run/test_command_manifest.py tests/unit/planning/test_subject_planner.py tests/unit/reporting/test_subject_reporting.py tests/unit/selection/test_selectors.py -q`
  - 结果：`65 passed`
- `python -m pytest tests/integration/run/test_unified_test_command.py tests/unit/run/test_subject_planner.py tests/unit/run/test_subject_reporting.py tests/unit/run/test_adapter_registry.py tests/unit/run/test_subject_manifest_schema.py tests/unit/run/test_suite_manifest.py -q`
  - 结果：`78 passed`
- `python -m pytest -q`
  - 结果：`782 passed, 40 skipped`

## 下一步

- 归档 `20260413-22`，切换到 `20260413-23-phase-8-compatibility-closure-and-legacy-cleanup-batch-1`。
- 先冻结 Phase 8 清理边界：
  - `tests/unit/run` 的 phase/stage 实测文件重命名与分桶
  - `tests/proof|perf|platform|gate|registry` 顶层 legacy 目录去留
  - legacy proof / benchmark / golden shell roots 的 expected / baseline / redirect 资产迁移顺序
