---
task_id: 20260405-01-il2cpp-reboot-after-abandoned-roadmap
title: IL2CPP-Like C# to C++ 重新 Brainstorm（废弃路线复盘）
task_type: roadmap
lifecycle_status: in_progress
phase: roadmap
created_at: 2026-04-05 20:51:20 +08:00
updated_at: 2026-04-06 01:21:23 +08:00
current_dir: docs/dev/in-progress/20260405-01-il2cpp-reboot-after-abandoned-roadmap
parent_task_id:
source_task_id: 20260329-01-il2cpp-like-cs-to-cpp
source_relation: reboot-after-abandonment
active: true
---

## 关键文档
- brainstorm: `docs/dev/in-progress/20260405-01-il2cpp-reboot-after-abandoned-roadmap/brainstorm-v1-01.md`
- design: `docs/dev/in-progress/20260405-01-il2cpp-reboot-after-abandoned-roadmap/design-v1-01.md`
- roadmap_or_plan: `docs/dev/in-progress/20260405-01-il2cpp-reboot-after-abandoned-roadmap/roadmap-v1-01.md`
- latest_progress: `docs/dev/in-progress/20260405-01-il2cpp-reboot-after-abandoned-roadmap/notes/progress-v1-05.md`
- finding: `docs/dev/in-progress/20260405-01-il2cpp-reboot-after-abandoned-roadmap/finding-2026-04-05-01-il2cpp-reboot-after-abandoned-roadmap.md`

## 当前判断
- current_focus: 作为 reboot 父任务，维护轻量 roadmap、阶段边界和子任务映射；Stage 3 已完成归档，当前 active 上下文回到父 roadmap，并进入 Stage 4 派生决策点。
- why_now: 用户明确要求废弃旧任务，并重新 brainstorm 一份计划，而且要求完整梳理废弃方案全过程。
- done_definition: roadmap 的阶段、依赖、子任务映射和推进顺序稳定，后续执行主要在 roadmap-child 子任务中推进。

## 最近摘要
- 2026-04-05 20:51:20 +08:00: 新任务已创建，作为旧父级 roadmap 废弃后的唯一新规划入口。
- 2026-04-05 20:51:20 +08:00: 本轮工作必须覆盖旧方案全过程复盘，而不是只给一个新的标题式路线图。
- 2026-04-05 21:00:00 +08:00: 用户已明确选择“先重整执行入口与工程骨架，再进入 HelloWorld proof-first”，当前需要收窄这次重整的边界。
- 2026-04-05 21:05:00 +08:00: 用户进一步选择重量版 Stage 1，意味着 `typed-il-ir`、`runtime ABI`、`codegen bridge` 以及 proof 所需的 manifest/registration 边界都要纳入重整讨论。
- 2026-04-05 21:08:00 +08:00: 用户同意将 `AOT manifest`、`code registration`、`metadata registration` 一并纳入重量版 Stage 1 的 contract reset。
- 2026-04-05 21:12:00 +08:00: 已细化 `Pack A: Proof Envelope` 与 `Pack B: Frontend Artifact Contract`，当前准备进入 `Pack C: Runtime Boundary Contract`。
- 2026-04-05 21:20:00 +08:00: 已补齐 `Pack C / D / E` 的冻结建议，并新增建议目录结构与对应技术路线，当前 Stage 1 已形成完整的 contract pack 草案。
- 2026-04-05 21:28:00 +08:00: 已补充 contract 分层、`Chaos.IL2CPP.*` managed 命名与 tests 结构的多方案对比，并给出当前推荐组合。
- 2026-04-05 21:34:00 +08:00: 用户已确认结构组合：contracts 选单根目录、managed 选细粒度 `Chaos.IL2CPP.*`、tests 选按角色重组。
- 2026-04-05 21:42:00 +08:00: 已创建 `plan-v1-01.md`，作为 design 通过后的执行草案。
- 2026-04-05 21:48:00 +08:00: 根据当前流程语义修正任务状态；当前应视为 design 阶段，而不是已正式进入 planning 执行。
- 2026-04-05 22:10:00 +08:00: 已完成 design 审查收口：first proof 显式采用 generated direct-call landing，`typed-il-ir` 显式纳入 canonical `subjectId`，Stage 1 设计边界保持重量版 A 但允许执行分期，`tests/contracts/trace` 固定为 canonical trace contract 路径。
- 2026-04-05 22:20:00 +08:00: 已完成更新后一致性复核，未发现新的 design 阻塞；当前 design checkpoint 可视为关闭，任务进入 planning。
- 2026-04-05 22:40:00 +08:00: 已将父任务从单一大 plan 升级为轻量 roadmap，并拆出 `20260405-02-stage-1a-contract-surface-reset` 与 `20260405-03-stage-1b-structure-and-test-migration` 两个子任务。
- 2026-04-05 23:00:12 +08:00: Stage 1A 已完成 contract surface 主体落地：`contracts/` canonical artifact/doc 入口、`typed-il-ir` 的 canonical `subjectId`、first-proof `HelloWorldObject` artifact 样例，以及 runtime/bridge proof-boundary 文档均已写入；当前只剩 native compile-only smoke 因 shell 缺少 `cmake` 尚未补跑。
- 2026-04-05 23:09:33 +08:00: Stage 1A 已补完 native compile-only smoke，并完成归档；reboot 主线当前等待切换到 `20260405-03-stage-1b-structure-and-test-migration`。
- 2026-04-05 23:32:04 +08:00: Stage 1B 已完成归档；`src/managed/Chaos.IL2CPP.*` skeleton、`tests/proof` 骨架和 `tests/contracts` canonical path 已全部落地，reboot 主线进入 Stage 2 派生点。
- 2026-04-05 23:50:13 +08:00: 已派生 `20260405-04-stage-2-helloworld-proof-spec` 作为新的 active 子任务，承接 Stage 2 `HelloWorld` proof spec freeze。
- 2026-04-05 23:50:13 +08:00: 已确认 `tests/proof/input/HelloWorldObject/Program.cs` 与 canonical artifact / native landing 存在偏差；Stage 2 默认按 `Hello, World!` / `return 0` / generated direct-call landing 方向收口。
- 2026-04-06 00:04:02 +08:00: Stage 2 已完成并准备归档；`HelloWorldObject` 的长期 spec 文档、`Program.cs` 真源、typed-il example / snapshot 与 overview 入口均已对齐。
- 2026-04-06 00:04:02 +08:00: reboot 主线当前回到父 roadmap 决策点；下一步不再是收口 Stage 2，而是决定 Stage 3 `Managed Minimal Closure` 与 Stage 4 `Native Bootstrap And Reference Proof` 的派生顺序。
- 2026-04-06 00:13:36 +08:00: 已确定默认先派生 `20260406-01-stage-3-managed-minimal-closure`，把下一步收敛到 managed closure，而不是继续把 Stage 3 / Stage 4 混做同一轮大任务。
- 2026-04-06 00:13:36 +08:00: Stage 3 已切为新的 active 子任务；当前推荐边界是“以 `HelloWorldObject.dll` 为输入，输出 managed closure bundle”，native bootstrap 仍留给 Stage 4。
- 2026-04-06 00:55:37 +08:00: Stage 3 已完成第一轮实现与计划内验证；通用 managed closure pipeline 已能从真实 `HelloWorldObject.dll` 产出与 Stage 2 canonical examples 对齐的五件套 bundle，且 Stage 2 回归未受影响。
- 2026-04-06 01:18:35 +08:00: Stage 3 已补齐正式 wiki / registry / unified-entry 接入；`pipeline/completion-managed-closure` 与 `test registry check-consistency --json` 均已通过。
- 2026-04-06 01:21:23 +08:00: `20260406-01-stage-3-managed-minimal-closure` 已完成归档；父 roadmap 重新接回 active 上下文，等待 Stage 4 `Native Bootstrap And Reference Proof` 的派生决策。

## 下一步
- next_action: 维护 roadmap 状态与 parent-child 依赖；当前进入 Stage 4 派生决策点，准备创建 `Native Bootstrap And Reference Proof` 子任务。
- owner: codex
- trigger: `20260406-01-stage-3-managed-minimal-closure` 已完成归档，reboot 主线重新回到父 roadmap 决策层。

## 风险/阻塞
### risks
- 旧任务的设计/roadmap/finding 体量较大，容易把历史结论和当前执行承诺混在一起。
- 如果不先完成“保留/重开/废弃”三分法，新计划很容易再次滑回旧路线的体量和节奏。
- Stage 1 改为重量版后，存在再次扩张成“新一轮大 roadmap” 的风险，必须用 proof 相关性来限制边界。

### blockers
- 无。

## 关联任务
### parent
- 无。

### source
- `20260329-01-il2cpp-like-cs-to-cpp`：reboot-after-abandonment

### children
- `20260405-02-stage-1a-contract-surface-reset`
- `20260405-03-stage-1b-structure-and-test-migration`
- `20260405-04-stage-2-helloworld-proof-spec`
- `20260406-01-stage-3-managed-minimal-closure`
