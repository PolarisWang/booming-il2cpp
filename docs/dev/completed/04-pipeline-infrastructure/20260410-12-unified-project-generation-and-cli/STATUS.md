---
task_id: 20260410-12-unified-project-generation-and-cli
title: 统一工程生成与 CLI 重构
task_type: roadmap
lifecycle_status: completed
phase: completed
created_at: 2026-04-10 14:00:00 +08:00
updated_at: 2026-04-10 15:30:00 +08:00
current_dir: docs/dev/in-progress/20260410-12-unified-project-generation-and-cli
parent_task_id:
source_task_id:
source_relation:
active: false
---

## 关键文档

- brainstorm: `docs/dev/in-progress/20260410-12-unified-project-generation-and-cli/brainstorm-v1-01.md`
- design: `docs/dev/in-progress/20260410-12-unified-project-generation-and-cli/design-v1-01.md`
- roadmap_or_plan: `docs/dev/in-progress/20260410-12-unified-project-generation-and-cli/roadmap-v1-01.md`

## 当前判断

- current_focus: 完成 brainstorm 和 design 文档，等待用户审查后进入 roadmap。
- why_now: 用户要求调整生成工程功能，新增 CLI 子命令风格入口、统一 subject solution 结构、优化 TUI 流程。
- done_definition: chaos-il2cpp CLI 可用（convert/build/publish），每个 subject 生成完整 4 工程 solution，统一入口进度反馈和错误提示优化完成。

## 最近摘要

- 2026-04-10 14:00:00 +08:00: brainstorm 完成，用户确认 4 个核心变更方向。

## 下一步

- next_action: 编写 design 文档，用户审查后进入 roadmap。
- owner: codex

## 风险 / 阻塞

### risks

- Driver CLI 重构需要保持现有 Pipeline 接口不变。
- CMake build 调用需要处理多平台工具链差异。

### blockers

- 无。
