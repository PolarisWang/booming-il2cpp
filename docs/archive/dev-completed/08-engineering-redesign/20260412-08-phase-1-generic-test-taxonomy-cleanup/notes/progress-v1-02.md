# Phase 1 进展记录 v1.02

日期: 2026-04-12 22:04:48 +08:00

## 本批收口

- 完成 `tests/fixtures/registry/{modules,systems,pipelines}` canonical root，`build/toolchains/run/testing/registry.py` 同时保留 legacy `tests/registry/*` 路径兼容。
- 补充 `tests/unit/registry/test_registry_manifest_paths.py`，并为 `tests/fixtures/registry/` 与 `tests/registry/` 增加迁移说明文档。
- 完成 `tests/unit/run` 第二批归类，把 execution / reporting / performance / planning / selection / compatibility 测试迁入独立 bucket，并保留 legacy shim。
- 扩展 `tests/unit/run/test_repo_layout.py`，锁定 registry fixture root、generic unit buckets、run-entry shim，以及新的根级 `pytest` 收集边界。
- 新增仓库根 `pytest.ini`，把默认收集收敛到 `tests/`，同时排除 `artifacts/`，解决 third-party tests 干扰与 `tests.*` 包解析冲突。
- 对齐一批陈旧断言，使其与当前 canonical 设计一致：`runtime-baseline-*` workflow 命名、`Project And IDE` 菜单分组、`tests/integration/platform-hosts/*` host 根，以及 mobile benchmark 使用 `generated-native-aot`。

## 验证

- `python -m pytest tests/integration/run/test_command_manifest.py tests/integration/run/test_prepare_scopes.py tests/integration/run/test_stage_execution.py tests/integration/run/test_tui.py tests/integration/run/test_unified_test_command.py tests/unit/run/test_phase4_mainline_feature_pack_onboarding.py tests/unit/run/test_phase4_mobile_runtime_host_batch1.py tests/unit/run/test_phase8_mobile_foundation.py -q`
  - 结果：`33 passed, 6 skipped`
- `python -m pytest -q --collect-only`
  - 结果：`723 tests collected`
- `python -m pytest -q`
  - 结果：`683 passed, 40 skipped in 76.34s`

## 结论

- Phase 1 exit criteria 已满足，当前子任务可以归档。
- 父 roadmap 下一步应基于当前 taxonomy 基线规划 Phase 2，继续处理 `tests/tooling/run`、`tests/integration/run` 与 `Chaos.TestFramework` API 化切换。
