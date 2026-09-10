# Progress v1.03

Date: 2026-04-13 19:47:15 +08:00

## 本轮进展

- 继续完成 `tests/` generic 化收尾，补齐了 `tests/support.py` 的通用 helper：
  - `write_json(...)`
  - `make_temp_repo_root(...)`
  - `rewrite_fixture_manifest_paths(...)`
  - `clone_fixture_subject_repo(...)`
  - `materialize_subject_manifest(...)`
  - `select_subject_record(..., repo_root=...)`
- 将 `tests/tooling/run/test_tui.py` 的菜单 / argv case 改为 fake project subject record 和 fake registry index，不再依赖 live retained subject。
- 将 `tests/unit/planning/test_subject_planner.py` 改为 synthetic manifest + temp repo 驱动，保留 planner contract，移除对 repo 内真实 subject 的直接依赖。
- 将 `tests/tooling/run/test_subject_command.py` 改为 command-level synthetic 测试：
  - fake registry index
  - synthetic planner output
  - temp repo artifacts
  - empty validation outcome stub
  - 不再依赖 live retained subject 与真实 manifest/planner 组合
- 对 `tests/unit/reporting/test_events_schema.py`、`tests/unit/performance/test_subject_perf_policy.py`、`tests/unit/compatibility/test_native_reference_bootstrap_support.py`、`tests/tooling/run/test_unified_test_command.py`、`tests/tooling/run/test_command_manifest.py`、`tests/integration/run/test_command_manifest.py` 完成去耦或对齐。

## 验证

- `python -m pytest tests/tooling/run/test_tui.py tests/tooling/run/test_subject_command.py tests/unit/planning/test_subject_planner.py tests/unit/reporting/test_events_schema.py tests/unit/performance/test_subject_perf_policy.py tests/unit/compatibility/test_native_reference_bootstrap_support.py tests/tooling/run/test_unified_test_command.py tests/tooling/run/test_command_manifest.py tests/integration/run/test_command_manifest.py -q`
  - 结果：`92 passed, 25 skipped`
- `python -m pytest tests/tooling/run tests/unit/planning/test_subject_planner.py tests/unit/reporting/test_events_schema.py tests/unit/performance/test_subject_perf_policy.py tests/unit/compatibility/test_native_reference_bootstrap_support.py tests/integration/run/test_command_manifest.py tests/integration/registry/test_registry_scan.py -q`
  - 结果：`220 passed, 34 skipped`
- `Get-ChildItem tests -Recurse -Filter *.py | Select-String -Pattern 'select_subject_record\\(' | Group-Object Path | Sort-Object Count -Descending | ForEach-Object { '{0}`t{1}' -f $_.Count, $_.Name }`
  - 结果：仅剩 `tests/integration/registry/test_registry_scan.py` 与 `tests/support.py`

## 结论

- unit / tooling 层中与 retained subject 直接耦合的 `select_subject_record(...)` 已清零。
- retained live subject 现在只保留给 integration / registry contract 边界。
- 这轮 “tests generic 化收口” 可以视为完成；若后续还要继续压缩 retained smoke 边界，应另起 follow-up。
