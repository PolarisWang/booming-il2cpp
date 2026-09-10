---
task_id: 20260406-02-stage-4-native-bootstrap-and-reference-proof
title: Stage 4 Native Bootstrap And Reference Proof
task_type: plan
lifecycle_status: completed
phase: archived
created_at: 2026-04-06 09:43:58 +08:00
updated_at: 2026-04-06 12:29:22 +08:00
current_dir: docs/dev/completed/20260406-02-stage-4-native-bootstrap-and-reference-proof
parent_task_id: 20260405-01-il2cpp-reboot-after-abandoned-roadmap
source_task_id: 20260405-01-il2cpp-reboot-after-abandoned-roadmap
source_relation: roadmap-child
active: false
---

## 关键文档

- brainstorm: `docs/dev/completed/20260406-02-stage-4-native-bootstrap-and-reference-proof/brainstorm-v1-01.md`
- design: `docs/dev/completed/20260406-02-stage-4-native-bootstrap-and-reference-proof/design-v1-01.md`
- roadmap_or_plan: `docs/dev/completed/20260406-02-stage-4-native-bootstrap-and-reference-proof/plan-v1-01.md`
- latest_progress: `docs/dev/completed/20260406-02-stage-4-native-bootstrap-and-reference-proof/notes/progress-v1-14.md`

## 当前判断

- current_focus: 本任务已完成归档；Stage 4 已在 `Windows x64` reference host 上真实跑通 `runtime-core -> bootstrap -> support -> generated C++ -> proof host` 的闭环。
- why_now: Stage 4 是 reboot 主线里第一个要求“真实 native configure/build/run”闭环的阶段；只有把真实编译、真实链接和真实 proof run gate 都打通，才能证明 Stage 3 managed closure bundle 与 Stage 4 native bootstrap 的边界成立。
- done_definition: 已满足。`artifacts/proof/native-reference/HelloWorldObject/run/stdout.log` 包含 `Hello, World!`，`run/exit-code.txt` 为 `0`，generated path 未回退到 `method_invoke` 或宿主救火路径，且 `verify-roadmap-0` Windows 路径已接入真实 `chaos_stage4_hello_world_object_proof_run` gate。

## 最近摘要

- 2026-04-06 09:43:58 +08:00: 从父 roadmap 正式派生 Stage 4 子任务，并把本轮 first proof host 固定为 `Windows x64`。
- 2026-04-06 10:16:32 +08:00: `emit-native-reference` 改用 Scriban 模板渲染，Stage 4 generated C++ emission skeleton 落地。
- 2026-04-06 11:52:45 +08:00: Stage 4 proof CMake 被拆为 `host_input_build` / `driver_build` / `managed_closure` / `codegen` 四个显式阶段，并引入 `proof_run` 产物收集。
- 2026-04-06 12:07:05 +08:00: `verify-roadmap-0.py` / `.ps1` 已具备 Stage 4 proof run artifact 合同校验 helper，但当时仍未打开真实 gate。
- 2026-04-06 12:29:22 +08:00: 已修复真实构建中的 bridge 状态常量缺口与静态链接导出宏问题，完成 `windows-x64-reference-real` 的真实 build/run，并把 `verify-roadmap-0` Windows 路径推进到真实 `proof_run` gate。

## 下一步

- next_action: 无。若继续 reboot 主线，由父 roadmap 决定是否派生 `Stage 5: Post-Proof Split`。
- owner: codex
- trigger: Stage 4 的真实 proof build/run 与 `verify-roadmap-0` Windows gate 均已通过。

## 风险 / 阻塞

### risks

- 如果后续 Stage 5 绕开当前固定的 Stage 3 bundle / Stage 4 proof gate，仍可能重新引入 generated path 与 native glue 的边界漂移。
- 如果未来把 `windows-x64-reference` 的 compile-only anchor 与真实 proof gate 混写成新的隐式链路，可能再次降低排障可见性。

### blockers

- 无。

## 关联任务

### parent

- `20260405-01-il2cpp-reboot-after-abandoned-roadmap`

### source

- `20260405-01-il2cpp-reboot-after-abandoned-roadmap`：roadmap-child

### children

- 无。
