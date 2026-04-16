# 当前活动任务

- task_id: `20260416-05-phase-3-native-aot-planner-and-entry-abi-generalization`
- title: `Phase 3 NativeAot Planner And Entry ABI Generalization`
- phase: `executing`
- updated_at: `2026-04-16 13:07:19 +08:00`
- current_dir: `docs/dev/in-progress/20260416-05-phase-3-native-aot-planner-and-entry-abi-generalization`
- key_doc: `docs/dev/in-progress/20260416-05-phase-3-native-aot-planner-and-entry-abi-generalization/plan-v1-01.md`
- next_step: 继续审计 `CoreRuntimeFeatures/ProofEntry::Run()` 的下一层真实 blocker；当前 aggregate native-aot 已从 generic-constraint dispatch 链推进到 `System.Runtime.CompilerServices.AsyncTaskMethodBuilder<System.Int32>::Create()` 的 external-runtime direct-call target。

## 关注点
- 父任务仍是 `20260416-01-aot-core-ir-gap-audit-and-next-expansion-roadmap`
- `20260416-05` 本轮已完成：
  - `callvirt` direct-vs-virtual 误判修复
  - generic constraint interface slot 闭包
  - planner `ldarga` 最小 widening
  - linker interface dispatch implementation retention backfill
  - `GenericConstraintProof` owner 纯化，移除字符串插值噪音
- 新增 focused 证据：
  - `tests/unit/compatibility/test_phase4_generic_constraint_dispatch_native_aot.py`
- 当前新的真实 blocker 已切换到 async / state-machine / external-runtime helper family，而不是 generic dispatch family
