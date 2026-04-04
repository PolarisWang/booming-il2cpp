---
task_id: 20260404-05-test-verification-architecture
title: 测试与验证体系升级方案
task_type: plan
lifecycle_status: in_progress
phase: planning
created_at: 2026-04-04 20:00:07 +08:00
updated_at: 2026-04-04 20:00:07 +08:00
current_dir: docs/dev/in-progress/20260404-05-test-verification-architecture
parent_task_id:
source_task_id:
source_relation:
active: true
---

## 关键文档
- brainstorm: `docs/dev/in-progress/20260404-05-test-verification-architecture/brainstorm-v1-01.md`
- design: `docs/dev/in-progress/20260404-05-test-verification-architecture/design-v1-01.md`
- roadmap_or_plan: `docs/dev/in-progress/20260404-05-test-verification-architecture/plan-v1-01.md`
- latest_progress: `docs/dev/in-progress/20260404-05-test-verification-architecture/notes/progress-v1-01.md`

## 当前判断
- current_focus: 把现有 `run test` 统一测试框架扩展为“代码测试 / 模块验证 / 整体验证 / 测试管线”一体化体系，并给出文档、registry、入口、skill、错误定位的完整方案。
- why_now: 当前 `.codex` 通用 skill 与项目级测试说明、registry、入口语义、模块验证和系统场景之间还没有统一对象模型，继续叠加实现会放大漂移成本。
- done_definition: 形成经用户确认的 `brainstorm/design/plan` 三份文档，明确唯一测试知识入口、对象模型、统一 registry、`run test` 语法、TUI 视图、skill 集成、错误定位契约和一次性切换方案。

## 最近摘要
- 2026-04-04 20:00:07 +08:00: 已与用户逐条对齐测试体系的高影响决策，确认唯一入口为 `wiki/06-测试验证/`，并决定一次性废除 `run verify`，统一切换到 `run test`。
- 2026-04-04 20:00:07 +08:00: 已确认新增正式对象层 `module/...`、`system/...`、`pipeline/...`，以及统一 registry、静态索引快照、skill 引用链和结构化错误定位要求。

## 下一步
- next_action: 整理已确认决策为正式 `design-v1-01.md` 和 `plan-v1-01.md`，并同步 `docs/dev` 索引与 ACTIVE 状态。
- owner: codex
- trigger: 用户要求提供测试与验证体系升级方案，并已逐条完成关键决策对齐。

## 风险/阻塞
### risks
- 新体系涉及文档、registry、入口语法、TUI、skill 和错误模型的同步切换，如果对象边界不清晰，后续实现会出现多套注册与多套入口并存。
- 用户要求一次性废除 `run verify`，意味着迁移方案必须在入口层给出清晰的断点和兼容边界，不能依赖长期双轨。

### blockers
- 无。

## 关联任务
### parent
- 无。

### source
- 无。

### children
- 无。
