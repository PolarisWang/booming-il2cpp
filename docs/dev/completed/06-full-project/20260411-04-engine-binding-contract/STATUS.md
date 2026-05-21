---
task_id: 20260411-04-engine-binding-contract
title: Engine Binding Contract
task_type: plan
lifecycle_status: completed
phase: archived
created_at: 2026-04-11 01:50:00 +08:00
updated_at: 2026-04-11 13:59:04 +08:00
current_dir: docs/dev/completed/20260411-04-engine-binding-contract
parent_task_id: 20260409-10-total-solution-and-ios-hot-update-analysis
source_task_id: 20260409-10-total-solution-and-ios-hot-update-analysis
source_relation: child_of_roadmap_phase_3
active: false
---

## 关键文档

- parent_roadmap: `docs/dev/in-progress/20260409-10-total-solution-and-ios-hot-update-analysis/roadmap-v1-01.md`
- phase_architecture: `docs/dev/in-progress/20260409-10-total-solution-and-ios-hot-update-analysis/phase-architecture-v1-01.md`
- plan: `docs/dev/completed/20260411-04-engine-binding-contract/plan-v1-01.md`
- brainstorm: `docs/dev/completed/20260411-04-engine-binding-contract/brainstorm-v1-03.md`
- design: `docs/dev/completed/20260411-04-engine-binding-contract/design-v1-03.md`
- latest_progress: `docs/dev/completed/20260411-04-engine-binding-contract/notes/progress-v1-03.md`

## 当前判断

- current_focus: Batch 1-4.5 已全部闭环；`contracts/engine/v0/`、`src/native/engine-bridge/`、`Chaos.IL2CPP.EngineBinding`、`subjects/EngineHostProof` 与 `HostEmbeddingLite` ownership proof 已形成 Phase 3 的最小 engine-facing contract 链路。
- why_now: Phase 3 的 lifecycle / ownership / callback contract 已取得 managed/native 双侧证据，父 roadmap 可以切换到 `20260411-05-mobile-runtime-host`，将 Android/iOS 从 toolchain gate 推进到 host/runtime evidence。
- done_definition: `plan-v1-01.md` 的 Batch 1-4.5 已完成；专项 RED/GREEN、广义回归、managed build、`EngineHostProof` 真实 subject matrix 与 `HostEmbeddingLite` output/trace matrix 均已通过。

## 最近摘要

- 2026-04-11 12:51:22 +08:00: `20260411-03-aot-runtime-completeness` 已归档，Phase 2 perf governance 已形成真实验证证据，主线切换到 `20260411-04-engine-binding-contract`。
- 2026-04-11 12:58:38 +08:00: 完成 Batch 1 contract freeze：新增 `contracts/engine/v0/lifecycle.h`、`ownership.h`、`callback.h`、`thread-model.md`，并通过 `test_phase3_engine_contract_freeze.py` / `test_phase7_engine_binding.py` / `test_repo_layout.py`。
- 2026-04-11 13:21:37 +08:00: 完成 Batch 2 `engine-bridge` 与 Batch 3 `Chaos.IL2CPP.EngineBinding` stub baseline：新增 managed 项目、`[EngineExport]` / `[EngineCallback]`、`EngineBindingStubEmitter`、managed smoke 合同与 native compile-only smoke，并通过 30 项聚合 pytest、core solution build、baseline 对比与 `chaos_engine_binding_compile_only_smoke` 构建验证。
- 2026-04-11 13:59:04 +08:00: 完成 Batch 4 `EngineHostProof` / `HostEmbeddingLite` ownership proof 闭环：`EngineHostProof` 已形成 windows dev output + reference trace 双矩阵证据，并补齐 `HostEmbeddingLite` 的 `GCHandle` strong/weak ownership 模拟、managed output / trace 双矩阵验证与 74 项回归；任务归档并切换到 `20260411-05-mobile-runtime-host`。

## 下一步

- next_action: 激活 `20260411-05-mobile-runtime-host`，先审计移动端 host/framework 现有复用面，再为 Batch 1 shared host framework 写 RED。
- owner: codex
- trigger: Phase 3 engine-facing contract 已闭环，父 roadmap 进入下一依赖阶段。

## 验证

- passed: `python -m pytest tests/unit/run/test_phase3_engine_binding_codegen_baseline.py -v`
- passed: `python -m pytest tests/unit/run/test_phase3_engine_bridge_minimal.py -v`
- passed: `python -m pytest tests/unit/run/test_phase3_engine_contract_freeze.py -v`
- passed: `python -m pytest tests/unit/run/test_phase7_engine_binding.py -v`
- passed: `python -m pytest tests/unit/run/test_repo_layout.py -v`
- passed: `python -m pytest tests/unit/run/test_phase3_engine_host_proof.py -v`
- passed: `python -m pytest tests/unit/run/test_phase3_host_embedding_ownership_protocol.py -v`
- passed: `python -m pytest tests/unit/run/test_phase3_engine_host_proof.py tests/unit/run/test_phase3_host_embedding_ownership_protocol.py -v`
- passed: `python -m pytest tests/unit/run/test_phase3_engine_binding_codegen_baseline.py tests/unit/run/test_phase3_engine_bridge_minimal.py tests/unit/run/test_phase3_engine_contract_freeze.py tests/unit/run/test_phase3_engine_host_proof.py tests/unit/run/test_phase3_host_embedding_ownership_protocol.py tests/unit/run/test_phase7_engine_binding.py tests/unit/run/test_repo_layout.py tests/unit/run/test_subject_manifest_schema.py tests/unit/run/test_subject_planner.py tests/unit/run/test_subject_workers.py tests/unit/run/test_subject_reporting.py -v`
- passed: `dotnet build solutions/core/windows/chaos-il2cpp-core.sln -c Debug`
- passed: `dotnet run --project tests/contracts/managed/engine-binding-smoke/EngineBindingSmoke.csproj -- --check-baseline tests/contracts/native/engine-binding/compile_only_engine_binding_smoke.cpp`
- passed: `cmake --preset windows-x64-reference`
- passed: `cmake --build artifacts/presets/windows-x64-reference --config Debug --target chaos_engine_binding_compile_only_smoke`
- passed: `dotnet run --project subjects/HostEmbeddingLite/source/HostEmbeddingLite.csproj --`
- passed: `EngineHostProof windows-dev-output subject matrix` -> `status=ok`, `terminalStageId=runtime-engine-observe`
- passed: `EngineHostProof windows-reference-trace subject matrix` -> `status=ok`, `terminalStageId=runtime-engine-trace-compare`
- passed: `HostEmbeddingLite windows-managed-output subject matrix` -> `status=ok`, `terminalStageId=runtime-managed-output`
- passed: `HostEmbeddingLite windows-managed-trace subject matrix` -> `status=ok`, `terminalStageId=runtime-trace-compare`

## 风险 / 阻塞

### risks

- `EngineHostProof` 仍是 mock host 的最小 proof，不等于 production engine integration，也不覆盖 richer callback signature / 多线程调度。
- `HostEmbeddingLite` 的 ownership roundtrip 仍是 managed-side `GCHandle` strong/weak 模拟，不等于新的共享 runtime ABI。
- `EngineBindingStubEmitter` 仍只支持 `void` + 无参数 interface method；更丰富的 signature 需要后续独立 proof。

### blockers

- 无。
