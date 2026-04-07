# Phase 3 Run-Scoped Artifact Spine Design v1.01

Date: 2026-04-07
Status: implemented

## 目录骨架

- canonical root: `artifacts/subjects/<subject-id>/runs/<run-id>`
- 共享分析桶: `runs/<run-id>/analysis/...`
- matrix 报告: `runs/<run-id>/matrices/<matrix-id>/pipeline-report/report.json`
- perf 侧产物: `runs/<run-id>/matrices/<matrix-id>/validations/perf/...`
- subject 汇总: `runs/<run-id>/subject-report/summary.json`
- run 级会话与日志: `runs/<run-id>/run-report/...`

## 兼容边界

- `last.json` / `current.json` 继续存在，但只作为指针，不复制 canonical 内容。
- 全局 `artifacts/logs/tests` 保留兼容 `last.json` 指针，真实 run 内容必须从 subject 根回查。

## 复用路径重写

- reused stage 读取上一次 run 的 manifest 后，会把 bucket-relative path 重写到当前 run。
- `stageResults` 中的证据、runtime diagnostics 与 perf 报告路径一并保留，保证复用结果可追踪。

## 验证命令

- `python -m unittest tests.tooling.run.test_subject_command tests.tooling.run.test_test_watch_summary tests.unit.run.test_subject_executor tests.unit.run.test_subject_reporting`
- `python -m unittest tests.integration.run.test_reporting_layout`
