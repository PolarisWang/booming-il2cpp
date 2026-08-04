---
task_id: 20260411-08-interpreter-mixed-execution
title: Interpreter Mixed Execution
task_type: plan
lifecycle_status: completed
phase: archived
created_at: 2026-04-11 16:00:23 +08:00
updated_at: 2026-04-11 19:10:21 +08:00
current_dir: docs/dev/completed/20260411-08-interpreter-mixed-execution
parent_task_id: 20260409-10-total-solution-and-ios-hot-update-analysis
source_task_id: 20260409-10-total-solution-and-ios-hot-update-analysis
source_relation: child_of_roadmap_phase_7
active: false
---

## 关键文档

- parent_roadmap: `docs/dev/in-progress/20260409-10-total-solution-and-ios-hot-update-analysis/roadmap-v1-01.md`
- phase_architecture: `docs/dev/in-progress/20260409-10-total-solution-and-ios-hot-update-analysis/phase-architecture-v1-01.md`
- plan: `docs/dev/completed/20260411-08-interpreter-mixed-execution/plan-v1-01.md`
- latest_progress: `docs/dev/completed/20260411-08-interpreter-mixed-execution/notes/progress-20260411-191021.md`

## 当前判断

- current_focus: Phase 7 的 proof-first baseline 已从“真实 `call/callvirt` lowering + mixed execution 最小闭环”推进到“same-assembly `CallVirt` 真实执行 + 基于 `ManagedInstructionModel.IlOffset` 的真实 `ManagedExceptionRegionModel -> IRExceptionRegion` EH lowering + 真实 `try/catch` / `throw;` / `try/finally` proof”。
- why_now: 实例 receiver / instance call lowering 与真实 EH lowering / dispatcher 是 Phase 7 剩余收益最高的两个缺口；本轮将其压成 dedicated minimal proof 后，父 roadmap 在本地环境内已没有新的解释器主线缺口。
- done_definition: `plan-v1-01.md` 的当前执行范围已闭环：`Chaos.IL2CPP.Interpreter` 项目边界稳定；真实 `call` / `callvirt` lowering 存在；same-assembly `Call` / `CallVirt` 最小执行闭环；AOT/interpreter/engine mixed execution 与 unload fallback 闭环；真实 `ManagedExceptionRegionModel -> IRExceptionRegion` lowering 已形成；真实 `try/catch` / `rethrow` / `try/finally` managed proof 已跑通；native `InterpreterVM` smoke 维持通过；专项 pytest、解释器项目构建与 core solution 构建均通过。

## 最近摘要

- 2026-04-11 16:00:23 +08:00: `20260411-07-metadata-supplement-bridge` 归档后，Phase 7 主线激活，开始执行 interpreter core / mixed execution。
- 2026-04-11 16:51:58 +08:00: 完成 managed lowering proof 与 native `InterpreterVM` smoke baseline。
- 2026-04-11 17:37:14 +08:00: 补齐真实外部 `callvirt` lowering proof，固化 `System.String::get_Length()` 目标。
- 2026-04-11 18:09:50 +08:00: 补齐块级 `catch + rethrow` 手写 IR proof，形成 `Throw -> catch block -> Rethrow -> AOT catch` 最小链路。
- 2026-04-11 19:10:21 +08:00: 新增 same-assembly `CallVirt` 真实执行 proof、`ManagedInstructionModel.IlOffset` 与真实 EH lowering、以及真实 `DivideOrCatch` / `DivideOrRethrow` / `AddWithFinally` proof，Phase 7 本地实现面归档完成。

## 下一步

- next_action: 父 roadmap 保持 `in_progress`，等待 Phase 4 `20260411-05-mobile-runtime-host` 所需 Android NDK / emulator / device 与 macOS/Xcode 环境恢复后继续。
- owner: codex
- trigger: Phase 7 在当前 Windows 环境内的解释器主线缺口已收口，主任务剩余阻塞转回 mobile runtime host 环境证据。

## 验证

- passed: `python -m pytest tests/unit/run/test_phase7_interpreter_mixed_execution.py -v`
- passed: `python -m pytest tests/unit/run/test_phase6_capability_batch_b.py -v`
- passed: `dotnet build src/managed/Chaos.IL2CPP.Interpreter/Chaos.IL2CPP.Interpreter.csproj -c Release`
- passed: `dotnet build solutions/core/windows/chaos-il2cpp-core.sln -c Release`

## 风险 / 阻塞

### risks

- 当前交付的是 Phase 7 proof-first baseline，不等于完整 production interpreter；对象模型、完整虚派发、catch type filtering 与更宽的 opcode 面仍未展开。
- `ManagedInterpreterExecutor` 的 EH dispatcher 仍是最小 block-level 语义，目的是证明 lowering/dispatch 主线闭环，而不是宣称完整 CLR 级异常语义已实现。
- `dotnet build solutions/core/windows/chaos-il2cpp-core.sln -c Release` 仍包含既有 warning：`LoaderStage.cs(153)` 的 `CS1717`，以及 `CodeGen` / `Driver` 里的若干 nullable warning；本轮未处理这些非 Phase 7 回归。

### blockers

- 无。
