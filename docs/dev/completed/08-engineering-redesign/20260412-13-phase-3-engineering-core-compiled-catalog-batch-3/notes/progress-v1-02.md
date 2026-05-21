# Progress v1.02

日期：2026-04-13 00:34:29 +08:00

## 本轮完成

- 在 `build/toolchains/run/testing/compiled_catalog.py` 新增 engineering-core compiled catalog foundation，支持 `engineeringProfile` 向 `engineeringValidations[]` 与 `engineeringWorkloads[]` 的稳定投影。
- 新增 `EngineeringValidationKind`、`EngineeringWorkloadKind`，并落地 `managed-output`、`native-executable`、`device-package`、`hot-update-host` 四类工程档位。
- `build_compiled_subject_catalog(...)` 现在会把工程级条目与已声明的 `declaredUnitTests[]`、`declaredBenchmarks[]` 合并为统一 catalog 视图。
- 新增并通过 compiled catalog 定向测试，冻结第一版 engineering-core schema。

## 验证结果

- `python -m pytest tests/unit/compatibility/test_compiled_subject_catalog.py -q` -> `3 passed`
- `python -m pytest tests/integration/registry/test_declared_metadata_discovery.py -q` -> `3 passed`
- `python -m pytest -q` -> `708 passed, 40 skipped`

## 下一步提示

- 归档当前 Batch 3。
- 父 roadmap 进入 Phase 4 Batch 1，开始 `golden` 到 `Solution*` archetype subject cutover。
