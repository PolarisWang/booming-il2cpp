# Phase 3 Run-Scoped Artifact Spine Plan v1.01

Date: 2026-04-07
Status: completed

## Tasks

- [x] 把 subject canonical run root 收口到 `artifacts/subjects/<id>/runs/<run-id>`
- [x] 把 matrix report、perf validation、subject summary、run report 移到 run-scoped 目录
- [x] 保留 `last.json` / `current.json` 兼容指针，但取消全局 canonical ledger
- [x] 修复 reused stage 元数据回放，确保 evidence / diagnostics / perf 细节不丢失
- [x] 更新 tooling 与 focused tests，确保 report/summary 查询命令可读新目录

## Verification

- [x] `python -m unittest tests.tooling.run.test_subject_command tests.tooling.run.test_test_watch_summary tests.unit.run.test_subject_executor tests.unit.run.test_subject_reporting`
- [x] `python -m unittest tests.integration.run.test_reporting_layout`
