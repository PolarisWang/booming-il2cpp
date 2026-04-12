---
task_id: 20260412-03-phase-c-hot-update-e2e
title: Phase C — 热更端到端
task_type: plan
lifecycle_status: completed
phase: archived
created_at: 2026-04-12 14:00:00 +08:00
updated_at: 2026-04-12 15:40:00 +08:00
current_dir: docs/dev/completed/20260412-03-phase-c-hot-update-e2e
parent_task_id: "20260411-10-post-phase7-development-roadmap"
source_task_id: "20260411-10-post-phase7-development-roadmap"
source_relation: child_of_roadmap_phase_C
active: false
---

## 关键文档

- parent_roadmap: docs/dev/in-progress/20260411-10-post-phase7-development-roadmap/roadmap-v1-01.md
- plan: docs/dev/completed/20260412-03-phase-c-hot-update-e2e/plan-v1-01.md
- latest_progress: docs/dev/completed/20260412-03-phase-c-hot-update-e2e/notes/progress-v1-01.md
- wiki:
  - wiki/04-工具与集成/hot-update-interpreter-vm-v1.md
  - wiki/04-工具与集成/hot-update-e2e-runtime-baseline-v1.md

## 当前判断

- current_focus: Phase C 的 5 个 Batch 已全部完成并归档，热更 package / method replacement / auto bridge / rollback / benchmark 闭环已落地
- why_now: Phase 7/9 回归、项目级测试、GC smoke、benchmark CLI 与根 CMake 全量构建都已提供最新证据，可以结束本子任务
- done_definition: HotUpdateSkeletonProof 端到端：加载包 → interpreter 执行 → 正确结果；方法替换后调用走 interpreter

## 最近摘要

- 2026-04-12 15:40:00 +08:00: Batch 1-5 全部完成；`tests/unit/run` 全量 258 项、Phase 7/9 回归、5 个 GC smoke、`chaos_interpreter_smoke`、`chaos_method_replacement_smoke`、3 个 hot-update benchmark CLI 记录以及根 CMake 全量构建均通过；已补充 Phase C wiki。
- 2026-04-12 14:00:00 +08:00: Phase C 计划创建

## 下一步

- next_action: 返回父 roadmap，准备 Phase D 多平台真机验证子计划
- trigger: 用户继续且 Android/iOS 真机环境可用时

## 风险

### risks
- AutoBridgeGenerator 当前只覆盖 5 类签名；复杂签名仍需后续扩展或回落手动 spec
- Phase D 依赖 Android/iOS 真机与对应宿主工具链，这部分环境风险仍存在
- 当前原生构建存在若干 C4819 编码告警，虽然不阻塞 Phase C，但后续应统一相关文件编码

### blockers
- 无

## 验证

- `python -m pytest tests/unit/run/test_phase7_interpreter_mixed_execution.py tests/unit/run/test_phase9_hot_update_e2e.py` → `16 passed`
- `python -m pytest tests/unit/run` → `258 passed`
- `.\run.ps1 benchmark --subject BenchHotUpdateLoad --mode managed --record` → `meanDurationMs: 7.1794 ms`
- `.\run.ps1 benchmark --subject BenchHotUpdateDispatch --mode managed --record` → `meanDurationMs: 0.2228 ms`, `opsPerSecond: 4,488,330`
- `.\run.ps1 benchmark --subject BenchHotUpdateRoundtrip --mode managed --record` → `meanDurationMs: 0.3456 ms`
- `cmake -S . -B artifacts/.tmp-phase-c-fullbuild -G "Visual Studio 17 2022" -DROADMAP0_PRESET_TARGET=windows-x64-reference` → configure success
- `cmake --build artifacts/.tmp-phase-c-fullbuild --config Release` → success（仅存在现有 C4819 编码告警，无 error）
- `artifacts\gc-contracts\Release\gc_{init,handle,thread,finalizer,safepoint}_smoke.exe` → 全部 `OK`
- `artifacts\.tmp-phase-c-fullbuild\tests\contracts\native\interpreter\Release\chaos_interpreter_smoke.exe` → `native-interpreter-smoke=42`
- `artifacts\.tmp-phase-c-fullbuild\tests\contracts\native\hot-update\Release\chaos_method_replacement_smoke.exe` → `method-replacement-after-revert=0`
