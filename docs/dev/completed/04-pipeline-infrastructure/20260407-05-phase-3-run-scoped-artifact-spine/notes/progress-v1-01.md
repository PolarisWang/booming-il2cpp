# Progress v1.01

Date: 2026-04-07 16:47:13 +08:00

## Summary

- Canonical subject run root 已收口到 `artifacts/subjects/<id>/runs/<run-id>`。
- matrix report、perf validation、subject summary 与 run report 已迁移到 run-scoped 目录。
- reused stage 结果现在会把 evidence、runtime diagnostics 与 perf 元数据一起重写到当前 run。

## Verification

- `python -m unittest tests.tooling.run.test_subject_command tests.tooling.run.test_test_watch_summary tests.unit.run.test_subject_executor tests.unit.run.test_subject_reporting`
- `python -m unittest tests.integration.run.test_reporting_layout`

## Outcome

- `Phase 3` 完成归档。
- 下一步切入 `Phase 4` 的 `CHECK|PROFILE|SHIP` 变体链路。
