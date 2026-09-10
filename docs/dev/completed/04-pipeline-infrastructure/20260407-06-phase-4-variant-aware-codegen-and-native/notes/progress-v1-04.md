# Progress v1.04

Date: 2026-04-07 22:45:00 +08:00

## Summary

- 已把 analysis contract snapshot baseline 从 `tests/contracts/schema` 收拢到 `contracts/artifacts/v0/snapshots/`，并同步更新 worker、contract validator、Python 回归测试与活文档引用。
- 已删除 `tests/proof/hello-world-object`、`tests/proof/input/*`、`tests/proof/native-reference/*` 中的 subject 专属资产；`tests/proof/` 现在只保留通用 `README.md` 与通用 props。
- 已把 subject-owned xUnit 断言切到新的边界：检查 `tests/proof/` 只剩迁移说明，并明确禁止旧 subject proof 目录回流。
- 已补 wiki 与架构文档，明确 `tests/` 只保留通用测试基础设施，subject 资产归 `subjects/`，contract snapshot 归 `contracts/`。

## Verification

- `python -B -m unittest tests.integration.run.test_reporting_layout tests.tooling.run.test_test_watch_summary tests.tooling.run.test_tui tests.unit.run.test_repo_layout tests.tooling.run.test_subject_command tests.unit.run.test_subject_workers tests.unit.run.test_subject_workers_perf`
  - `Ran 58 tests in 0.633s`
  - `OK (skipped=25)`
- `python -B build/toolchains/run/run.py test subject --id subject/HelloWorldObject --json`
  - `source-resolve -> host-input-build -> analysis-frontend -> generated-native-proof` 通过
  - contract report 已切到 `contracts/artifacts/v0/snapshots`
  - xUnit validation 通过，summary 位于 `artifacts/subjects/HelloWorldObject/runs/20260407-223712-windows-0522/matrices/windows-dev-output/validations/unit/summary.json`
  - `build-target` 仍因本机缺少 C++ 编译器失败：`No CMAKE_CXX_COMPILER could be found`

## Evidence

- contract validate report: `artifacts/subjects/HelloWorldObject/runs/20260407-223712-windows-0522/analysis/analysis/contract-validate.report.json`
- unit validation summary: `artifacts/subjects/HelloWorldObject/runs/20260407-223712-windows-0522/matrices/windows-dev-output/validations/unit/summary.json`
- run summary: `artifacts/subjects/HelloWorldObject/runs/20260407-223712-windows-0522/run-report/summary.json`

## Blocker

- 当前仓库结构与 subject 主链收敛已完成，但 Windows native `build-target` 仍受环境阻塞；若要把本任务状态推进到真正 completed，需要本机可用的 C++ toolchain / `CMAKE_CXX_COMPILER`。
