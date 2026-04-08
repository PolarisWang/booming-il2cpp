# 进展记录 1

- 时间：2026-04-08 14:32:41 +08:00
- 结论：
  - live canonical 命名已从 `roadmap-0` 迁移到 `runtime-baseline` / `platform-gates`
  - 旧 `roadmap-0` CLI、system-id 与 `verify-roadmap-0.*` 保持兼容包装
  - 统一入口、registry、low-level verify、live docs/wiki 与 artifact root 已全部对齐
- 验证：
  - `python -m unittest tests.tooling.run.test_command_manifest tests.tooling.run.test_prepare_scopes tests.tooling.run.test_unified_test_command tests.tooling.run.test_registry_command tests.tooling.run.test_tui tests.tooling.run.test_verify_roadmap0 tests.unit.run.test_repo_layout`
  - `python build/toolchains/run/run.py test registry list --json`
  - `python build/toolchains/run/run.py test registry check-consistency --json`
- 下一步：
  - 无。若后续继续清理历史命名残留，再开 follow-up。
