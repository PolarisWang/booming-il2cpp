# Phase 3 Run-Scoped Artifact Spine Brainstorm v1.01

Date: 2026-04-07
Status: closed

## 已确认决策

- `artifacts/subjects/<id>/runs/<run-id>` 是唯一 canonical run root。
- `artifacts/logs/tests` 只保留兼容指针，不再承担 canonical run ledger 职责。
- matrix report、validation report、subject summary、run report 都必须回收到 subject-local run 目录。
- 复用已有 stage 结果时，必须保留证据路径、运行时诊断和 perf 细节，不能因为 reuse/copy 丢失 traceability。

## 实现结论

- run-scoped artifact spine 已落地，subject 运行日志与中间产物均回到 `artifacts/subjects/...`。
- `run test summary --run <run-id>` 已能解析 subject-local run report。
- reused stage 元数据会回写到当前 run 路径，不再丢失 `primaryEvidencePaths`、诊断信息或 perf 明细。

## 后续交接

- 下一阶段由 `Phase 4` 继续把 `variant` 纳入 planner、codegen、native 和 report 元数据。
