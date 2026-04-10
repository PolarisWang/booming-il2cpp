---
task_id: 20260410-03-run-tooling-domain-reorg
title: Run Tooling Domain Reorg - 重组 build/toolchains/run/** 的目录与模块边界
task_type: plan
lifecycle_status: completed
phase: completed
created_at: 2026-04-10 11:10:00 +08:00
updated_at: 2026-04-10 12:00:00 +08:00
current_dir: docs/dev/in-progress/20260410-03-run-tooling-domain-reorg
parent_task_id: 20260409-11-contract-root-without-subject-content
source_task_id: 20260409-11-contract-root-without-subject-content
source_relation: child_of_roadmap_phase_2
active: false
---

## 关键文档

- layout-target: `docs/dev/completed/20260410-01-layout-boundary-freeze/layout-target-v1.md`
- plan: `docs/dev/in-progress/20260410-03-run-tooling-domain-reorg/plan-v1-01.md`

## 当前判断

- current_focus: 已完成 core/ 和 subject/ 的目录重组。
- done_definition: core/ (5 files) 和 subject/ (2 files) 迁移完成；所有 import 已 rewire；276/276 测试通过（8 个 pre-existing failures）。testing/ 子目录重组和大文件拆分留给后续 Phase 3-5。

## 最近摘要

- 2026-04-10 11:10:00 +08:00: 创建 child task，开始执行。
- 2026-04-10 11:30:00 +08:00: Batch 1 完成：创建 core/，移动 common.py, manifest.py, result.py, operation_reporting.py, tooling.py，rewire 所有 import（~25 文件），更新所有测试文件路径。
- 2026-04-10 11:45:00 +08:00: Batch 2 完成：创建 subject/，移动 project_workspace.py, deploy_layout.py，rewire import，修复 sed 造成的语法错误。
- 2026-04-10 12:00:00 +08:00: 验证通过：276 passed, 8 pre-existing failures, 32 skipped, 0 regressions。

## 下一步

- next_action: Phase 2 完成。后续 testing/ 子目录和大文件拆分可在 Phase 3-5 中推进。
- owner: codex

## 风险 / 阻塞

### risks

- 无。

### blockers

- 无。
