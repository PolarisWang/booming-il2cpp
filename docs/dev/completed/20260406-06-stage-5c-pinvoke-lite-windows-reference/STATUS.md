---
task_id: 20260406-06-stage-5c-pinvoke-lite-windows-reference
title: Stage 5C PInvokeLite Windows Reference
task_type: plan
lifecycle_status: completed
phase: archived
created_at: 2026-04-06 16:05:00 +08:00
updated_at: 2026-04-06 16:19:51 +08:00
current_dir: docs/dev/completed/20260406-06-stage-5c-pinvoke-lite-windows-reference
parent_task_id: 20260406-03-stage-5-post-proof-split
source_task_id: 20260406-03-stage-5-post-proof-split
source_relation: roadmap-child
active: false
---

## 关键文档

- brainstorm: `docs/dev/completed/20260406-06-stage-5c-pinvoke-lite-windows-reference/brainstorm-v1-01.md`
- design: `docs/dev/completed/20260406-06-stage-5c-pinvoke-lite-windows-reference/design-v1-01.md`
- roadmap_or_plan: `docs/dev/completed/20260406-06-stage-5c-pinvoke-lite-windows-reference/plan-v1-01.md`
- latest_progress: `docs/dev/completed/20260406-06-stage-5c-pinvoke-lite-windows-reference/notes/progress-v1-02.md`

## 当前判断

- current_focus: 本任务已完成归档；Stage 5C 已用最小 `DllImport` import-only proof 打通 Windows interop foundation，覆盖 proof 输入、managed import descriptor、metadata materialization、Scriban codegen 模板与 Windows reference proof host。
- why_now: Stage 5A 已验证 managed generic foundation，Stage 5B 已验证最小 reflection foundation；Stage 5C 需要在不把 `Marshal`、`NativeLibrary` 与 export 语义混入第一轮的前提下，先拿到第一条稳定的 interop 证明路径。
- done_definition: 已满足；`PInvokeLiteDllImportMinimal` 已稳定产出 closure / native-reference artifact，真实 proof run 输出 `pinvoke-ok|14`，Stage 4 / Stage 5A / Stage 5B 回归继续通过，且 `src/**` 保持 sample-agnostic。

## 最近摘要

- 2026-04-06 15:35:12 +08:00: Stage 5B 已完成归档并拿到真实 `reflection-ok` Windows reference proof 证据，Stage 5 follow-on 已具备进入 interop 切片的前置基础。
- 2026-04-06 16:05:00 +08:00: 基于 Stage 5 roadmap 正式派生本 child 计划任务；第一轮 scope 收窄为 Windows 上最小 `DllImport` 整数 import proof，不直接复用完整 `tests/smoke/input/PInvokeLite/Program.cs`。
- 2026-04-06 16:19:51 +08:00: 已补齐 `ManagedImportModel`、method import metadata、`pinvokeDllImportMinimal` lowering plan 与 `NativeReferenceProof.PInvokeDllImportMinimal.cpp.scriban`，并新增 `PInvokeLiteDllImportMinimal` proof 输入、proof host 与 Stage 5C 单测。
- 2026-04-06 16:19:51 +08:00: `python -m unittest -v tests.unit.run.test_stage5c_pinvoke_lite_windows_reference`、`dotnet build src/managed/Chaos.IL2CPP.Driver/Chaos.IL2CPP.Driver.csproj -c Release` 以及 Stage 5B / Stage 5A / Stage 4 / Windows wiring / native prefix naming 回归均已通过；本任务无额外 wiki 更新需求。

## 下一步

- next_action: 无。Stage 5 follow-on 的 Windows interop foundation 已闭环；后续 `macOS` parity 边界改由 `20260406-07-stage-5d-macos-reference-parity` 冻结并交接。
- owner: codex
- trigger: Stage 5C 所需的 proof 输入、descriptor materialization、generated glue、proof host 与回归验证均已拿到完成证据。

## 风险/阻塞

### risks

- 如果后续 `macOS` parity 试图直接复用当前 `kernel32.dll` / `MulDiv` 的 Stage 5C proof，会把 Windows 专属 interop 假设错误地扩散到跨宿主 parity。
- 如果后续 interop 扩展绕开本轮固定下来的 import metadata contract、`native-proof.plan.json` 形状或 Scriban emission 边界，仍可能重新把样例细节泄漏回 `src/**`。

### blockers

- 无。

## 关联任务

### parent

- `20260406-03-stage-5-post-proof-split`

### source

- `20260406-03-stage-5-post-proof-split`：roadmap-child

### children

- 无。
