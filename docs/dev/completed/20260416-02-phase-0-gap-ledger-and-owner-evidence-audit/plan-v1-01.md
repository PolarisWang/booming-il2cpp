# Phase 0 Gap Ledger And Owner Evidence Audit 实现计划

> **面向执行 Agent：** 必须使用 `dev:executing-plans` 执行本计划。步骤使用复选框语法跟踪。

**目标：** 冻结 capability-first gap ledger 与 owner evidence matrix，完成父 roadmap 的 `Phase 0`。

**架构：** 先收集 `LoaderStage`、`AotCoreIrLowering`、`NativeAotLoweringPlanner`、`NativeAotEmitter + Scriban` 的显式边界，再把这些边界归并成 capability family，并绑定到 `SolutionCorePack` 的 proof / benchmark / gate 证据。最后用代表性 compatibility 用例确认当前冻结事实成立。

**技术栈：** Markdown 文档、PowerShell 源码审计、`pytest`

**设计文档：** `docs/dev/in-progress/20260416-02-phase-0-gap-ledger-and-owner-evidence-audit/design-v1-01.md`

**预期知识沉淀：** 按任务决定

---

Status: completed

## 计划清单

- [x] 建立 child task 文档骨架，并把 `ACTIVE.md` 切到 `20260416-02`
- [x] 输出 `gap-ledger-v1-01.md`
- [x] 输出 `owner-evidence-matrix-v1-01.md`
- [x] 运行代表性 compatibility 验证，确认 ledger 基线成立
- [x] 更新 child / parent `STATUS.md`、索引、progress note，并将 child task 归档为 completed

## 需要创建或修改的文件

- `docs/dev/in-progress/20260416-02-phase-0-gap-ledger-and-owner-evidence-audit/STATUS.md`
- `docs/dev/in-progress/20260416-02-phase-0-gap-ledger-and-owner-evidence-audit/design-v1-01.md`
- `docs/dev/in-progress/20260416-02-phase-0-gap-ledger-and-owner-evidence-audit/plan-v1-01.md`
- `docs/dev/in-progress/20260416-02-phase-0-gap-ledger-and-owner-evidence-audit/gap-ledger-v1-01.md`
- `docs/dev/in-progress/20260416-02-phase-0-gap-ledger-and-owner-evidence-audit/owner-evidence-matrix-v1-01.md`
- `docs/dev/in-progress/20260416-02-phase-0-gap-ledger-and-owner-evidence-audit/notes/progress-*.md`
- `docs/dev/ACTIVE.md`
- `docs/dev/INDEX.md`
- `docs/dev/in-progress/INDEX.md`
- `docs/dev/in-progress/20260416-01-aot-core-ir-gap-audit-and-next-expansion-roadmap/STATUS.md`
- `docs/dev/in-progress/20260416-01-aot-core-ir-gap-audit-and-next-expansion-roadmap/roadmap-v1-01.md`

## 验证命令

- `python -m pytest tests/unit/compatibility/test_phase7_loader_function_pointer_decode.py tests/unit/compatibility/test_phase7_loader_generic_method_reference_decode.py tests/unit/compatibility/test_phase4d_abi_widening_native_aot.py tests/unit/compatibility/test_phase4d_exception_native_aot.py tests/unit/compatibility/test_phase5_owner_subject_coverage.py tests/unit/compatibility/test_solution_core_pack_subject.py tests/unit/compatibility/test_scriban_vendor_build.py -q`

## 状态维护要求

- 执行中由 `docs/dev/ACTIVE.md` 指向本 child task
- 完成后将本 child task 移到 `docs/dev/completed/`
- 父 roadmap 保持 `in_progress`，并把 `20260416-02` 状态更新为 `completed`
