---
task_id: 20260409-11-contract-root-without-subject-content
title: Contract 去 Subject 内容化与 Repo 强规范化重组
task_type: roadmap
lifecycle_status: in_progress
phase: roadmap
created_at: 2026-04-09 23:58:00 +08:00
updated_at: 2026-04-10 00:38:00 +08:00
current_dir: docs/dev/in-progress/20260409-11-contract-root-without-subject-content
parent_task_id:
source_task_id:
source_relation:
active: false
---

## 关键文档

- brainstorm: `docs/dev/in-progress/20260409-11-contract-root-without-subject-content/brainstorm-v1-02.md`
- design: `docs/dev/in-progress/20260409-11-contract-root-without-subject-content/design-v1-02.md`
- roadmap_or_plan: `docs/dev/in-progress/20260409-11-contract-root-without-subject-content/roadmap-v1-01.md`

## 当前判断

- current_focus: 用户已确认选择方案 C；当前任务从单一目录调整升级为跨 `contracts / build / src / tests` 的强规范化重组 roadmap。
- current_focus_2: 推荐先启动 `layout-boundary-freeze` 与 `contract-fixture-ownership-cutover` 两个 child task，再推进 run tooling、managed source、tests 的逐层重组。
- why_now: 用户明确要求 `contract` 目录中不允许再出现 subject 下的内容，而当前 `contracts/artifacts/v0/samples`、`contracts/artifacts/v0/snapshots`、`contracts/native/v0/samples` 等路径仍承载 `HelloWorldObject` 为主的 concrete fixture。
- why_now_2: 用户又追加要求，希望一并优化 `src/`、`tests/`、`build/` 的脚本目录结构，并在必要时拆分过长文件。
- done_definition: 强规范化重组的阶段、child task、依赖与推荐推进顺序已冻结，后续可以按 roadmap 派生子任务，而不是直接进入单份实现计划。

## 最近摘要

- 2026-04-09 23:42:00 +08:00: 复查 `contracts/`、`build/toolchains/run/testing/subject_workers.py`、`build/toolchains/run/run_manifest.json`，确认 sample 和 snapshot 已被运行时、测试命令和文档同时消费。
- 2026-04-09 23:51:00 +08:00: 复查 `tests/unit/run/test_repo_layout.py`、`tests/unit/run/test_path_resolver.py`、`tests/unit/run/test_subject_contracts_source_cutover.py`，确认当前仓库已有多处测试把 `contracts/.../samples|snapshots` 当成 canonical 路径。
- 2026-04-10 00:05:00 +08:00: 冻结推荐方向为“formal contract 定义留在 `contracts/`，shared validation fixture 迁到 `tests/contracts/analysis/v0/**`，并顺手停止使用 `tests/contract/**` 遗留别名”。
- 2026-04-10 00:20:00 +08:00: 追加扫描 `build/toolchains/run/**`、`src/managed/**`、`tests/tooling/run/**`、`tests/unit/run/**`，确认 `tui.py`、`commands/test.py`、`subject_workers.py`、`NativeReferenceLoweringPlanner.cs`、`LoaderStage.cs` 已明显超长，适合并入本轮中度重组。
- 2026-04-10 00:38:00 +08:00: 用户明确选择方案 C；已新增 `roadmap-v1-01.md`，把当前任务升级为强规范化重组 roadmap，并冻结 6 个阶段与 8 个 child task。

## 下一步

- next_action: 优先创建并启动 `layout-boundary-freeze` child task；紧接着进入 `contract-fixture-ownership-cutover`。
- owner: codex
- trigger: 用户要求我继续从 roadmap 派生第一个 child task，或直接指定先做 `layout-boundary-freeze` / `contract-fixture-ownership-cutover`。

## 风险 / 阻塞

### risks

- 如果只搬目录、不改 `path_resolver` 和 consumer，`validate_analysis_contracts()`、`run test contract managed-closure-bundle`、subject report 会继续写旧路径。
- 如果把 shared contract fixture 直接挂到 `subjects/<subject-id>/...`，generic contract check 会被单个 subject 树反向绑死。
- 如果不顺手清理 `contracts/examples`、`contracts/native/examples` 与 `tests/contract/**`，仓库会继续保留多套“看似 canonical”的并行路径。
- 如果 repo 结构优化范围失控，当前主线会从“边界收敛”滑成“大规模机械重构”。
- 如果只拆文件不补目录语义，几轮之后仍会回到单目录堆叠状态。
- 如果 `tests` 早于 `build/src` 重排，会出现重复迁移和 test discovery 回归。

### blockers

- 当前没有实现阻塞；现在只缺按 roadmap 选择先启动哪个 child task。
