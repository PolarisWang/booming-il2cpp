---
task_id: 20260406-05-stage-5b-reflection-lite-windows-reference
title: Stage 5B ReflectionLite Windows Reference
task_type: plan
lifecycle_status: completed
phase: archived
created_at: 2026-04-06 14:38:08 +08:00
updated_at: 2026-04-06 15:35:12 +08:00
current_dir: docs/dev/completed/20260406-05-stage-5b-reflection-lite-windows-reference
parent_task_id: 20260406-03-stage-5-post-proof-split
source_task_id: 20260406-03-stage-5-post-proof-split
source_relation: roadmap-child
active: false
---

## 关键文档

- brainstorm: `docs/dev/completed/20260406-05-stage-5b-reflection-lite-windows-reference/brainstorm-v1-01.md`
- design: `docs/dev/completed/20260406-05-stage-5b-reflection-lite-windows-reference/design-v1-01.md`
- roadmap_or_plan: `docs/dev/completed/20260406-05-stage-5b-reflection-lite-windows-reference/plan-v1-01.md`
- latest_progress: `docs/dev/completed/20260406-05-stage-5b-reflection-lite-windows-reference/notes/progress-v1-03.md`

## 当前判断

- current_focus: 本任务已完成归档；Stage 5B 已把最小 query-only `ReflectionLite` proof 路径打通，覆盖 managed closure、native lowering plan、Windows reference proof host 与真实 proof run。
- why_now: Stage 5A 已验证 closed generic foundation，Stage 5B 需要在不引入 `Invoke` / `EventInfo` / `MakeGenericMethod` 的前提下，先冻结第一条 metadata / reflection query 路径。
- done_definition: 已满足；`ReflectionLiteQueryMinimal` 能沿现有 Windows reference spine 输出稳定 metadata / reflection query artifacts，真实 proof run 输出 `reflection-ok|ReflectionFixture<System.Int32>|BackingField|Value|Echo|nextValue|ReflectionFixture\`1`，Stage 4 / Stage 5A 基线持续通过，且 `src/**` 保持 sample-agnostic。

## 最近摘要

- 2026-04-06 12:48:45 +08:00: Stage 5 子 roadmap 已把 follow-on 路线固定为 `GenericEcho -> ReflectionLite -> PInvokeLite -> macOS parity`。
- 2026-04-06 14:26:02 +08:00: Stage 5A 已完成归档，并拿到 `generic-ok` / `exit code 0` 的真实 proof 证据。
- 2026-04-06 14:38:08 +08:00: 已正式派生 Stage 5B，并把当前 scope 收窄为最小 query-only `ReflectionLite` proof。
- 2026-04-06 15:35:12 +08:00: 已补齐 `NativeReferenceProofEmitter`、Scriban 模板、`runtime_abi.h`、`runtime_core.cpp`、`bootstrap.cpp` 与 compile-only ABI smoke 的最小 ReflectionLite query glue，`python -m unittest -v tests.unit.run.test_stage5b_reflection_lite_windows_reference` 5 个用例全部通过。
- 2026-04-06 15:35:12 +08:00: 已完成回归验证：`dotnet build src/managed/Chaos.IL2CPP.Driver/Chaos.IL2CPP.Driver.csproj -c Release`、Stage 5A、Stage 4 native/reference wiring、native prefix naming、bootstrap/runtime-core skeleton 均通过；本任务无需额外 wiki 更新，长期约束已沉淀到 proof gate 与 ABI smoke。

## 下一步

- next_action: 无。Stage 5 follow-on 已继续推进到 `20260406-06-stage-5c-pinvoke-lite-windows-reference`；后续由父 roadmap 继续维护 child mapping 与阶段边界。
- owner: codex
- trigger: Stage 5B 所需的 managed metadata materialization、native query glue、Windows reference proof run 与回归验证均已拿到完成证据。

## 风险 / 阻塞

### risks

- 后续 Stage 5C / Stage 5D 如果绕开本轮固定下来的 `native-proof.plan.json`、Scriban 模板边界或 tagged descriptor handle 方案，仍可能把样例细节重新泄漏回 `src/**`。
- 本轮只冻结 query-only ReflectionLite；若后续直接叠加 `MethodInfo.Invoke`、`EventInfo`、`MakeGenericMethod` 等高阶语义，仍需要新的 proof 与 gate，而不能默认为这轮自然覆盖。

### blockers

- 无。

## 关联任务

### parent

- `20260406-03-stage-5-post-proof-split`

### source

- `20260406-03-stage-5-post-proof-split`：roadmap-child

### children

- 无。
