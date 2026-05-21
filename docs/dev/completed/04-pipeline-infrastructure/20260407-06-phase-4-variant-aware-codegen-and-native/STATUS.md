---
task_id: 20260407-06-phase-4-variant-aware-codegen-and-native
title: Phase 4 Variant-Aware Codegen And Native
task_type: plan
lifecycle_status: completed
phase: archived
created_at: 2026-04-07 16:47:13 +08:00
updated_at: 2026-04-08 00:03:55 +08:00
current_dir: docs/dev/completed/20260407-06-phase-4-variant-aware-codegen-and-native
parent_task_id: 20260407-02-subject-layout-and-variant-traceability-roadmap
source_task_id: 20260407-02-subject-layout-and-variant-traceability-roadmap
source_relation: roadmap-child
active: false
---

## 关键文档

- brainstorm: `docs/dev/completed/20260407-06-phase-4-variant-aware-codegen-and-native/brainstorm-v1-01.md`
- design: `docs/dev/completed/20260407-06-phase-4-variant-aware-codegen-and-native/design-v1-01.md`
- roadmap_or_plan: `docs/dev/completed/20260407-06-phase-4-variant-aware-codegen-and-native/plan-v1-01.md`
- latest_progress: `docs/dev/completed/20260407-06-phase-4-variant-aware-codegen-and-native/notes/progress-v1-05.md`

## 当前判断

- current_focus: 本阶段已归档；`CHECK|PROFILE|SHIP` 已贯通到 planner/worker/codegen/native/report，Windows native reference host 也已切到 subject 自有目录并通过端到端验证。
- why_now: 用户要求的结构约束、变体语义与 traceability 主链都已经落地，最后一项 `build-target` 环境阻塞也已被修复并完成验证。
- done_definition: 已满足；focused Python 回归与 `run test subject --id subject/HelloWorldObject --json` 已全部通过。

## 最近摘要

- 2026-04-07 22:45:00 +08:00: 已将 analysis contract snapshot baseline 从 `tests/contracts/schema` 切到 `contracts/artifacts/v0/snapshots/`，并清空 `tests/proof/**` 的 subject 专属资产。
- 2026-04-08 00:03:55 +08:00: 已修复 Windows subject native build 链路：worker 改为绝对 `cmake.exe` + VS developer environment + VS bundled `ninja.exe` + `Ninja Multi-Config`。
- 2026-04-08 00:03:55 +08:00: 已修复 `windows_developer_environment()` 的 `VsDevCmd.bat` 调用与 PATH/Path 去重问题，避免覆盖真实 MSVC 工具链路径。
- 2026-04-08 00:03:55 +08:00: 已修复 `subjects/HelloWorldObject/validation/proof/native-reference/CMakeLists.txt`，把 generated cpp 视为已有输入并在缺失时立即 fail fast。
- 2026-04-08 00:03:55 +08:00: 端到端 `run test subject --id subject/HelloWorldObject --json` 已完整通过，`build-target` 与 `runtime-observe` 闭环完成。

## 下一步

- next_action: 无。父 roadmap 已同步归档；后续若扩展更多 subject 或更多 native host，再从已冻结的 variant/wiki 规则派生 follow-up。
- owner: codex
- trigger: 仅当出现新的 subject/native 变体或新的 public surface 需求时，才需要新开 follow-up。

## 风险 / 阻塞

### risks

- 若后续新测试或新文档再次把 subject 资产放回 `tests/`，结构约束会回退。
- 若后续有人把 `variant` 重新折回 `matrix` 或 `goal`，`CHECK|PROFILE|SHIP` 的宏与编译选项追踪会再次混乱。
- 受限沙箱中的 Windows 原生验证可能在 compiler ABI probe 阶段长时间停滞；正式验证应优先在沙箱外完成，但 canonical 证据仍必须写回 `artifacts/subjects/...`。

### blockers

- 无。

## 关联任务

### parent

- `20260407-02-subject-layout-and-variant-traceability-roadmap`

### source

- `20260407-02-subject-layout-and-variant-traceability-roadmap`（roadmap-child）

### children

- 无。
